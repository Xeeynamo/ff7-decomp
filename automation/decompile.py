#!/usr/bin/env python3
"""
Automated Decompilation Runner

Orchestrates the decompilation process:
1. Picks functions from the database (easiest first)
2. Runs ./mako.sh dec <function>
3. Updates database with results
4. Optionally verifies the decompilation
5. Handles errors gracefully

Usage:
    python decompile.py --auto              # Process next function
    python decompile.py --auto --batch 10   # Process 10 functions
    python decompile.py --auto --verify      # Auto-verify after decompilation
    python decompile.py --function func_800A1158  # Specific function
    python decompile.py --status            # Show decompilation progress
"""

import subprocess
import argparse
import sys
import re
import json
from pathlib import Path
from datetime import datetime
from typing import Optional, Tuple, Dict
from database import DecompDatabase

# Project root is one level up from automation/
PROJECT_ROOT = Path(__file__).parent.parent
MAKO_SCRIPT = PROJECT_ROOT / "mako.sh"

# Validation errors that represent m2c artifacts that are syntactically valid C and
# can be fixed without reverting the decompiled code.  Functions that fail only with
# one of these errors are kept in the source file and marked 'decompiled_needs_refine'
# instead of being reverted to INCLUDE_ASM and marked 'blocked'.
#
# Hard errors (still revert → blocked):
#   M2C_ERROR(), Decompilation failure:, INCLUDE_ASM still present, void*/sp pointer,
#   saved_reg_ra, missing jr $ra, internal error traceback, func_XXXX as integer,
#   PSY-Q compilation failed, negative-offset unk-N field, C99 mid-block declaration.

# Base set: always treated as soft (code compiles, just needs renaming/casting).
BASE_SOFT_VALIDATION_ERRORS = (
    # saved_reg_s0..s7, saved_reg_v0, etc. — valid C identifiers, code compiles,
    # just needs the variables renamed to something meaningful.
    "saved_reg_",
    # (unaligned type) casts — compile fine, just need a cast substitution.
    "unaligned type",
)

# Extended set (used by default): additionally treats '?' type-unknown artifacts as
# soft errors so that functions are kept as decompiled_needs_refine rather than
# being immediately reverted to INCLUDE_ASM.  Pass --strict-validation to agent.py
# to use only BASE_SOFT_VALIDATION_ERRORS and revert these functions instead.
SOFT_VALIDATION_ERRORS = BASE_SOFT_VALIDATION_ERRORS + (
    # Unknown '?' type markers — m2c could not infer a type; the surrounding code
    # structure is intact and the issue is purely a missing type annotation.
    "pointer to unknown type",
    "unknown parameter type",
    "unknown variable type",
    "unknown type marker",
    "unknown return type",
    "'?' type in cast expression",
)


class DecompilationRunner:
    """Automates running mako.sh dec and tracking results."""
    
    def __init__(self, db: DecompDatabase, verbose: bool = False, enable_llm_fixes: bool = False,
                 build_verifier=None, fix_structs: bool = False, strict_validation: bool = False):
        self.db = db
        self.verbose = verbose
        self.enable_llm_fixes = enable_llm_fixes
        # Optional BuildVerifier for per-file PSY-Q compilation check
        self.build_verifier = build_verifier
        # Pass --fix-structs to mako.sh dec (replaces D_8009XXXX with Savemap fields)
        self.fix_structs = fix_structs
        # Strict mode: only BASE_SOFT_VALIDATION_ERRORS are treated as soft.
        # Default (False) uses the extended SOFT_VALIDATION_ERRORS set which also
        # keeps '?' type-unknown functions as decompiled_needs_refine.
        self.soft_errors = BASE_SOFT_VALIDATION_ERRORS if strict_validation else SOFT_VALIDATION_ERRORS

        # Optional LLM support for auto-fixing
        self.llm = None
        if enable_llm_fixes:
            try:
                from llm_helper import LLMHelper
                self.llm = LLMHelper(verbose=verbose)
                if not self.llm.available:
                    if verbose:
                        print("⚠️  LLM requested but not available, continuing without auto-fix")
                    self.llm = None
            except ImportError:
                if verbose:
                    print("⚠️  LLMHelper not available, continuing without auto-fix")
                self.llm = None
        
    def run_mako_dec(self, function_name: str) -> Tuple[bool, str, str]:
        """
        Run ./mako.sh dec <function_name>
        
        Returns:
            (success, stdout, stderr) tuple
        """
        if not MAKO_SCRIPT.exists():
            return False, "", f"mako.sh not found at {MAKO_SCRIPT}"
        
        cmd = [str(MAKO_SCRIPT), "dec", function_name]
        if self.fix_structs:
            cmd.append("--fix-structs")

        if self.verbose:
            print(f"Running: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd,
                cwd=PROJECT_ROOT,
                capture_output=True,
                text=True,
                timeout=60  # 60 second timeout
            )
            
            success = result.returncode == 0
            return success, result.stdout, result.stderr
            
        except subprocess.TimeoutExpired:
            return False, "", "Command timed out after 60 seconds"
        except Exception as e:
            return False, "", f"Exception running command: {str(e)}"
    
    def validate_c_file(self, c_file_path: Path, function_name: str, original_content: Optional[str] = None) -> Tuple[bool, Optional[str]]:
        """
        Validate the C file after decompilation to catch m2c failures.
        
        Checks both function-specific issues and file-wide pollution from m2c errors.
        
        Returns:
            (is_valid, error_message) tuple
        """
        if not c_file_path.exists():
            return False, "C file not found"
        
        try:
            content = c_file_path.read_text()
            
            # File-wide check: Did m2c add decompilation failure comments?
            # These appear when m2c can't parse external declarations
            if "Decompilation failure:" in content:
                if original_content:
                    old_failure_count = original_content.count("Decompilation failure:")
                    new_failure_count = content.count("Decompilation failure:")
                    if new_failure_count > old_failure_count:
                        return False, f"m2c added {new_failure_count - old_failure_count} decompilation failure comment(s) (dependency issues)"
                else:
                    return False, "Contains 'Decompilation failure' marker from m2c"
            
            # Primary check: Is INCLUDE_ASM still present for THIS specific function?
            # If mako.sh dec succeeded, it should have replaced INCLUDE_ASM with actual C code
            include_asm_pattern = rf'INCLUDE_ASM\s*\([^,]*,\s*"[^"]*",\s*{re.escape(function_name)}'
            if re.search(include_asm_pattern, content):
                return False, "INCLUDE_ASM still present (decompilation incomplete)"

            # Sanity check: the function name must appear as a definition in the file.
            # Catches LLM hallucinations where the function was renamed or completely
            # replaced with a fabricated stub under a different name.
            func_def_pattern = rf'\b{re.escape(function_name)}\s*\('
            if not re.search(func_def_pattern, content):
                return False, f"Function '{function_name}' not found as a definition after decompilation (LLM may have renamed it)"
            
            # ENHANCED: Hard structural checks run first (before any soft/? checks).
            # These detect type-safety violations that produce incorrect runtime behaviour
            # and must NOT be masked by a preceding soft-error short-circuit.

            # Helper: given a position in content, return the position of the closing '}'
            # that ends the innermost enclosing function body. Returns len(content) if not
            # found. Strips string literals and comments before counting braces so that
            # '{'/'}' characters inside them don't skew the depth counter.
            def _find_function_end(text, decl_pos):
                # Replace string/char literals and comments with same-length whitespace
                # so that character offsets stay valid after the substitution.
                _clean = re.sub(r'"(?:[^"\\]|\\.)*"',
                                lambda m: ' ' * len(m.group()), text)
                _clean = re.sub(r"'(?:[^'\\]|\\.)*'",
                                lambda m: ' ' * len(m.group()), _clean)
                _clean = re.sub(r'//[^\n]*',
                                lambda m: ' ' * len(m.group()), _clean)
                _clean = re.sub(r'/\*.*?\*/',
                                lambda m: ' ' * len(m.group()), _clean,
                                flags=re.DOTALL)
                depth = 0
                for i in range(decl_pos):
                    if _clean[i] == '{': depth += 1
                    elif _clean[i] == '}': depth -= 1
                target = max(1, depth)
                for i in range(decl_pos, len(_clean)):
                    if _clean[i] == '{':
                        depth += 1
                    elif _clean[i] == '}':
                        depth -= 1
                        if depth < target:
                            return i + 1
                return len(text)

            # H1. void* LOCAL variables / parameters accessing struct fields via ->
            # m2c sometimes fails to infer the actual struct type, leaving variables
            # typed as void*. This produces type-unsafe code that compiles under old
            # GCC but is semantically wrong — it must be a hard reject.
            _VOID_PTR_RE = re.compile(r'\bvoid\s*\*\s*(\w+)\s*[;,=]')
            for _vm in _VOID_PTR_RE.finditer(content):
                _void_var = _vm.group(1)
                _func_end = _find_function_end(content, _vm.start())
                _scope = content[_vm.start():_func_end]
                if re.search(rf'\b{re.escape(_void_var)}\s*->', _scope):
                    return False, f"Local variable '{_void_var}' is void* but used with -> (m2c struct type inference failure)"

            # H2. Primitive pointer types used with ->unk* field access.
            # Legitimate struct pointers use named fields; ->unkN means m2c couldn't
            # determine the struct type and left the variable as s32*/u8* etc.
            _PRIM_PTR_RE = re.compile(
                r'\b(?:s8|s16|s32|s64|u8|u16|u32|u64|f32|f64)\s*\*\s*(\w+)\s*[;,=]'
            )
            for _m in _PRIM_PTR_RE.finditer(content):
                _prim_var = _m.group(1)
                _func_end = _find_function_end(content, _m.start())
                _scope = content[_m.start():_func_end]
                if re.search(rf'\b{re.escape(_prim_var)}\s*->unk\w*', _scope):
                    return False, f"Local variable '{_prim_var}' is a primitive pointer but used with ->unk (m2c struct type inference failure)"

            # ENHANCED: Check for m2c incomplete type artifacts in the entire file
            # These indicate m2c lacked context to determine proper types

            # 1. Check for ? as return type: "? sprintf(s8*, s8*, s8*);"
            if re.search(r'^\s*\?\s+\w+\s*\(', content, re.MULTILINE):
                return False, "Contains function declaration with unknown return type '?'"
            
            # 2. Check for ?* pointer type: "?* var_a2_2;"
            if re.search(r'\?\s*\*\s+\w+', content):
                return False, "Contains pointer to unknown type '?*'"
            
            # 3. Check for /*?*/ unknown type comments
            # m2c uses /*?*/primitive to annotate guessed primitive types on extern
            # declarations (e.g. "extern /*?*/s32 D_800F9F3C;").  These are valid C
            # because the comment is stripped by the preprocessor, so we allow them.
            # Only flag /*?*/ that is NOT immediately followed by a known primitive.
            _PRIMITIVE_PAT = (r'(?:s8|s16|s32|s64|u8|u16|u32|u64|f32|f64'
                              r'|char|short|int|long|float|double'
                              r'|unsigned|signed|void)')
            _content_stripped = re.sub(r'/\*\?\*/\s*' + _PRIMITIVE_PAT, '', content)
            if '/*?*/' in _content_stripped:
                return False, "Contains unknown type marker /*?*/"
            
            # 4. Check for ? in parameter lists or declarations
            # Matches: (? argname), (, ? argname), (?), (?,) etc.
            # The (?:\w|\s*[,)]) ensures we catch both (? name) and (?)
            if re.search(r'[,(]\s*\?\s*(?:[,)]|\w)', content):
                return False, "Contains unknown parameter type '?'"

            # 5. Check for M2C_ERROR macro (unset registers, unknown instructions)
            # Only match uncommented instances (not //...M2C_ERROR(...) or /* ... M2C_ERROR ... */)
            if re.search(r'^(?!\s*//).*M2C_ERROR\(', content, re.MULTILINE):
                return False, "Contains M2C_ERROR() macro (m2c could not determine register/instruction)"

            # 5a. Check for negative-offset field names: ->unk-N / .unk-N (m2c arithmetic error)
            # These should have been fixed by intellisense_fixer.apply_all_fixes() before
            # validation runs.  If any remain it means the fixer was skipped or missed an
            # occurrence — still reject so the file is reverted cleanly.
            if re.search(r'(?:->|\.)unk-[0-9A-Fa-f]+\b', content):
                return False, "Contains negative-offset field access unk-N (m2c struct offset error)"

            # 5b. Check for '?' used inside cast expressions: (?**), (?*), (? *)
            # This occurs when m2c cannot determine the type in a cast context but the
            # outer expression type is valid, so checks 1–4 above miss it.
            if re.search(r'\(\s*\?[\s*]+\)', content):
                return False, "Contains '?' type in cast expression (m2c type inference failure)"

            # 6. Check for saved_reg_ra (m2c artifact for $ra — never valid in C)
            if 'saved_reg_ra' in content:
                return False, "Contains saved_reg_ra (m2c return-address artifact)"

            # 7. Check for "Warning: missing \"jr $ra\"" comments injected by m2c
            if re.search(r'//\s*Warning: missing "jr \$ra"', content):
                return False, "Contains m2c 'missing jr $ra' warning comment"

            # 8. Check for Internal error tracebacks injected by m2c
            if re.search(r'/\*\s*Internal error in function', content):
                return False, "Contains m2c 'Internal error in function' traceback"
            
            # Extract the function body (approximately) to check for local issues
            # Look for the function definition and the next few lines
            func_pattern = rf'{re.escape(function_name)}\s*\([^)]*\)\s*{{[^}}]*}}'
            func_match = re.search(func_pattern, content, re.DOTALL)
            
            if func_match:
                func_body = func_match.group(0)

                # Hard checks first (must not be reordered after any soft check)

                # Check for m2c-specific error patterns in the function body
                if "Decompilation failure:" in func_body:
                    return False, "Function contains 'Decompilation failure' marker from m2c"

                # Check for void* parameters being dereferenced — hard check, must precede
                # the soft '? variable type' check below so that a function with BOTH a
                # void*→struct access AND a '?' type annotation isn't mis-classified soft.
                sig_match = re.search(rf'{re.escape(function_name)}\s*\(([^)]*)\)', func_body)
                if sig_match:
                    params = sig_match.group(1)
                    void_ptr_params = re.findall(r'void\s*\*\s*(\w+)', params)
                    for param in void_ptr_params:
                        if re.search(rf'{re.escape(param)}\s*->', func_body):
                            return False, f"Parameter '{param}' is void* but accessed as struct (m2c couldn't determine type)"

                # Check for incomplete multiline comment that might indicate failure
                if func_body.count("/*") != func_body.count("*/"):
                    return False, "Unclosed comment block (possible m2c error)"

                # Soft checks below (may produce decompiled_needs_refine state)

                # Check for (unaligned type) casts
                if re.search(r'\(\s*unaligned\s+', func_body):
                    return False, "Contains '(unaligned type)' cast (m2c error)"

                # Check for unknown type markers in variable declarations
                if re.search(r'\?\s+\w+\s*[;,=]', func_body):
                    return False, "Contains unknown variable type '?' (m2c cannot determine type)"

                # Check for C99-style mid-block variable declarations (PSY-Q cc1 is C89 only).
                # In C89, all declarations must precede any statements in a block.
                # Pattern: a type declaration appearing after any statement in the function body.
                _c99_decl_re = re.compile(
                    r'^\s+(?:const\s+)?(?:unsigned\s+|signed\s+)?'
                    r'(?:u8|u16|u32|u64|s8|s16|s32|s64|f32|f64|char|short|int|long|float|double)\s*\*?\s*\w+\s*='
                )
                _found_stmt = False
                for _line in func_body.split('\n'):
                    _stripped = _line.strip()
                    if not _stripped or _stripped.startswith('//') or _stripped.startswith('/*'):
                        continue
                    if _c99_decl_re.match(_line):
                        if _found_stmt:
                            return False, "Contains C99-style mid-block variable declaration (PSY-Q cc1 requires C89)"
                    elif _stripped.endswith(';') and not _c99_decl_re.match(_line):
                        _found_stmt = True

            # If we can't find the function body, that's suspicious but not necessarily invalid
            # (could be a complex function or macro)

            # --- File-wide structural checks (not limited to function body) ---

            # 9 (now H1) and 10 (now H2) were moved above checks #1-8 — see the
            # '_find_function_end / H1 / H2' block near the top of this function.

            # 11. Check for saved_reg_ artifacts (undeclared m2c callee-save register variables)
            # saved_reg_ra is already caught above; this catches saved_reg_s0..s7, saved_reg_v0, etc.
            if re.search(r'\bsaved_reg_[a-z]\d*\b', content):
                return False, "Contains saved_reg_* artifact (m2c callee-save register not resolved)"

            # 12. Check for 'sp' used as dereference base (leaked stack pointer register).
            # Valid uses include: &sp (taking address of local struct named 'sp'),
            # sp.member (struct member access), sp10/sp14 etc. (suffixed local names).
            # Bad use: sp -> field access or *(sp + offset) — sp treated as a pointer.
            _content_no_comments = re.sub(r'//[^\n]*', '', content)
            _content_no_comments = re.sub(r'/\*.*?\*/', '', _content_no_comments, flags=re.DOTALL)
            if re.search(r'\bsp\s*->', _content_no_comments) or \
               re.search(r'\*\s*\(\s*(?:.*?[+\-]\s*)?sp\b(?!\d|\w)', _content_no_comments):
                return False, "Contains 'sp' used as a pointer/dereference base (leaked PSX stack pointer register)"

            # 13. Check for func_XXXXXXXX used as an integer/data value (not a call)
            # Valid: func_XXXXXXXX(args)  — a call
            # Bad:   func_XXXXXXXX * 4, (func_XXXXXXXX + ...), &D_...[func_XXXXXXXX]
            # Pattern: function name followed by arithmetic operator or used as array index,
            # but NOT followed by '(' (which would be a call).
            if re.search(r'\bfunc_[0-9A-Fa-f]+\s*(?:[+\-\*/%]|\[)', content):
                return False, "Function name used as integer value in arithmetic (m2c function pointer / data confusion)"

            return True, None
            
        except Exception as e:
            return False, f"Error reading C file: {e}"
    
    def extract_missing_types(self, c_file_path: Path, function_name: str, validation_error: str) -> Dict:
        """
        Extract detailed information about missing types from the decompiled code.
        This creates actionable reports for manual type definition.
        
        Note: Extracts ALL missing type issues from the file, not just the first validation error.
        
        Returns dict with:
            - function_name: str
            - file: str
            - primary_error: str (the validation error that triggered the revert)
            - missing_items: list of dicts describing what's missing
        """
        report = {
            'function_name': function_name,
            'file': str(c_file_path.relative_to(PROJECT_ROOT)),
            'primary_error': validation_error,
            'missing_items': []
        }
        
        try:
            content = c_file_path.read_text()
            
            # Check for ALL types of missing type issues in the file
            # (not just the one that triggered validation failure)
            
            # 1. External functions with unknown return types (? or /*?*/)
            extern_with_unknown = re.findall(r'(?:/\*\?\*/\s+)?(\w+)\s+(\w+)\s*\(([^)]*)\)\s*;?\s*//\s*extern', content)
            for return_type, func_name, params in extern_with_unknown:
                if '?' in return_type or '?' in params:
                    report['missing_items'].append({
                        'type': 'extern_function',
                        'name': func_name,
                        'return_type': return_type if '?' not in return_type else 'unknown',
                        'parameters': params,
                        'suggestion': f'Define proper signature for {func_name} in header'
                    })
            
            # Also check for inline ? function declarations
            inline_unknown = re.findall(r'^\s*\?\s+(\w+)\s*\(([^)]*)\)', content, re.MULTILINE)
            for func_name, params in inline_unknown:
                if func_name not in [item.get('name') for item in report['missing_items']]:
                    report['missing_items'].append({
                        'type': 'extern_function',
                        'name': func_name,
                        'return_type': 'unknown',
                        'parameters': params,
                        'suggestion': f'Define proper signature for {func_name} in header'
                    })
            
            # 2. void* parameters used as structs
            # Find function with void* parameters
            void_ptr_params = re.findall(rf'({re.escape(function_name)})\s*\([^)]*\bvoid\s*\*\s*(\w+)[^)]*\)', content)
            for func, param_name in void_ptr_params:
                # Find all field accesses for this parameter
                field_accesses = re.findall(rf'{re.escape(param_name)}\s*->\s*(\w+)', content)
                if field_accesses:  # Only report if there are actual field accesses
                    unique_fields = sorted(set(field_accesses))
                    report['missing_items'].append({
                        'type': 'void_pointer_struct',
                        'parameter_name': param_name,
                        'accessed_fields': unique_fields,
                        'suggestion': f'Define struct for {param_name} with fields: {", ".join(unique_fields[:10])}{"..." if len(unique_fields) > 10 else ""}'
                    })
            
            # 3. /*?*/ markers (unknown types in comments)
            comment_unknown = re.findall(r'/\*\?\*/\s*(\w+)\s+(\w+)', content)
            for type_name, var_name in comment_unknown:
                # Check if it's extern declaration
                if re.search(rf'extern\s+/\*\?\*/\s*{re.escape(type_name)}\s+{re.escape(var_name)}', content):
                    report['missing_items'].append({
                        'type': 'extern_variable',
                        'name': var_name,
                        'declared_type': type_name,
                        'suggestion': f'Unknown type for extern variable {var_name}'
                    })
            
            # 4. ?* pointers
            unknown_pointers = re.findall(r'\?\s*\*\s+(\w+)', content)
            for var_name in unknown_pointers:
                report['missing_items'].append({
                    'type': 'unknown_pointer',
                    'name': var_name,
                    'suggestion': f'm2c could not determine type of pointer {var_name}'
                })
            
            # 5. ? types in variable declarations
            unknown_vars = re.findall(r'^\s*\?\s+(\w+)\s*[;,=]', content, re.MULTILINE)
            for var_name in unknown_vars:
                if var_name not in [item.get('name') for item in report['missing_items']]:
                    report['missing_items'].append({
                        'type': 'unknown_variable',
                        'name': var_name,
                        'suggestion': f'm2c could not determine type of variable {var_name}'
                    })
            
            # 6. unaligned casts
            unaligned_casts = re.findall(r'\(\s*unaligned\s+(\w+)\s*\)', content)
            for type_name in set(unaligned_casts):
                report['missing_items'].append({
                    'type': 'unaligned_cast',
                    'cast_type': type_name,
                    'suggestion': f'Unaligned access to {type_name} - may need struct packing or alignment fix'
                })
            
        except Exception as e:
            if self.verbose:
                print(f"   Warning: Could not extract missing types: {e}")
        
        return report
    
    def parse_decompilation_output(self, stdout: str, stderr: str, returncode: int) -> Dict[str, any]:
        """
        Parse mako.sh output to extract useful information.
        
        Returns dict with keys:
            - success: bool
            - already_decompiled: bool (if function already in C)
            - error_message: str
            - error_type: str (infra, compile, link, unknown)
            - warnings: list of str
        """
        result = {
            'success': False,
            'already_decompiled': False,
            'error_message': None,
            'error_type': 'unknown',
            'warnings': []
        }
        
        combined_output = stdout + "\n" + stderr
        
        # First check return code - if non-zero, it's a failure
        if returncode != 0:
            result['success'] = False
            
            # tools/decompile.py prints "<name> found in <file>" and exits 1 when the
            # function already exists as C code (not as INCLUDE_ASM).  This is the
            # "already fully decompiled into C" case — treat it as success.
            if re.search(r'\bfound in\b', combined_output):
                result['already_decompiled'] = True
                result['success'] = True
                return result
            
            # Classify the error type
            if "not found" in combined_output.lower():
                result['error_message'] = "Function not found"
                result['error_type'] = 'infra'
            elif "timeout" in combined_output.lower():
                result['error_message'] = "Command timed out"
                result['error_type'] = 'infra'
            elif "undefined reference" in combined_output.lower():
                # Extract undefined symbols
                symbols = re.findall(r"undefined reference to `([^']+)'", combined_output)
                result['error_message'] = f"Undefined references: {', '.join(symbols[:3])}"
                result['error_type'] = 'link'
            elif "error:" in combined_output.lower():
                # Compile error
                error_lines = [line for line in combined_output.split('\n') 
                              if 'error:' in line.lower()]
                result['error_message'] = error_lines[0].strip() if error_lines else "Compile error"
                result['error_type'] = 'compile'
            else:
                result['error_message'] = "Command failed with non-zero exit code"
                result['error_type'] = 'unknown'
        else:
            # Return code 0 - check for success indicators
            if "could not find function" in combined_output.lower():
                result['success'] = False
                result['error_message'] = "Function not found in version"
                result['error_type'] = 'infra'
            elif re.search(r'already\s+(?:been\s+)?decompiled', combined_output, re.IGNORECASE):
                result['already_decompiled'] = True
                result['success'] = True
            elif "decompiled in" in combined_output.lower():
                # mako.sh typically says "func_name decompiled in path/to/file.c"
                result['success'] = True
            elif stdout.strip() and "error" not in combined_output.lower():
                # Has output and no error mentions
                result['success'] = True
            else:
                # Ambiguous case
                result['error_message'] = "Unclear if decompilation succeeded"
                result['error_type'] = 'unknown'
        
        return result
    
    def decompile_function(self, function_name: str) -> bool:
        """
        Decompile a single function and update database.
        
        Returns:
            True if decompilation succeeded, False otherwise
        """
        print(f"\n{'='*60}")
        print(f"Attempting to decompile: {function_name}")
        print(f"{'='*60}")
        
        # Get function info from database
        func = self.db.get_function(function_name)
        if not func:
            print(f"❌ Function {function_name} not found in database")
            return False
        
        print(f"Module: {func['module']}")
        # print(f"File: {func['c_file_path']}")
        # print(f"ASM lines: {func['line_count']}")
        print(f"Current status: {func['status']}")
        
        # Save original C file content before decompilation
        c_file = PROJECT_ROOT / func['c_file_path']
        original_content = c_file.read_text() if c_file.exists() else None
        
        # PRE-DECOMPILATION TYPE INFERENCE
        # If LLM is available, try to infer types for extern symbols before m2c runs
        if self.llm:
            try:
                from dependency_analyzer import DependencyAnalyzer
                analyzer = DependencyAnalyzer(PROJECT_ROOT, self.db.db_path)
                
                # Extract extern symbols from assembly
                extern_symbols = analyzer.get_function_extern_symbols(function_name)
                
                if extern_symbols:
                    if self.verbose:
                        print(f"🔍 Found {len(extern_symbols)} extern symbols, inferring types...")
                    
                    # Check if we already have types for these symbols
                    known_types = self.db.get_all_known_types(min_confidence=0.3)
                    unknown_symbols = [s for s in extern_symbols if s not in known_types]
                    
                    if unknown_symbols:
                        if self.verbose:
                            print(f"   🤖 Inferring types for {len(unknown_symbols)} unknown symbols: {', '.join(list(unknown_symbols)[:3])}...")
                        
                        # Infer types from assembly
                        inferred = self.llm.infer_types_from_assembly(function_name, list(unknown_symbols))
                        
                        if inferred:
                            # Generate header file with inferred types
                            temp_header = PROJECT_ROOT / "include" / "auto_types.h"
                            self.llm.generate_type_header(inferred, temp_header)
                            
                            # Store learned types in database
                            for symbol, type_info in inferred.items():
                                field_map = {
                                    offset: {'name': field[0], 'type': field[1]}
                                    for offset, field in type_info.get('fields', {}).items()
                                }
                                self.db.learn_struct_pattern(
                                    symbol,
                                    likely_type=type_info.get('typedef', '').split('}')[-1].strip().rstrip(';'),
                                    field_map=field_map,
                                    confidence=type_info.get('confidence', 0.5)
                                )
                                self.db.record_struct_usage(function_name, symbol)
                            
                            if self.verbose:
                                print(f"   ✅ Generated type definitions in {temp_header}")
                    else:
                        if self.verbose:
                            print(f"   ✅ All extern symbols have known types")
            except Exception as e:
                if self.verbose:
                    print(f"   ⚠️  Type inference failed: {e}")
                # Continue with decompilation anyway
        
        # Mark as in progress
        self.db.update_function_status(
            function_name, 
            'in_progress',
            notes="Attempting automatic decompilation"
        )
        
        # Run decompilation
        success, stdout, stderr = self.run_mako_dec(function_name)
        
        if self.verbose:
            print(f"\n--- STDOUT ---\n{stdout}")
            print(f"\n--- STDERR ---\n{stderr}")
        
        # Parse output (pass returncode for better analysis)
        returncode = 0 if success else 1
        parse_result = self.parse_decompilation_output(stdout, stderr, returncode)
        
        # Update database based on result
        if parse_result['already_decompiled']:
            print(f"✅ Function already decompiled!")
            self.db.record_decompilation_attempt(
                function_name,
                'decompiled',
                notes="Already decompiled (found in C code)"
            )
            return True
            
        elif parse_result['success']:
            # Apply deterministic IntelliSense fixes (unk-N field names, typedef hoisting)
            # before validation so these artifacts don't cause spurious failures.
            try:
                from intellisense_fixer import apply_all_fixes
            except ImportError:
                apply_all_fixes = None
            if apply_all_fixes is not None:
                raw = c_file.read_text()
                fixed, changes = apply_all_fixes(raw)
                if changes:
                    c_file.write_text(fixed)
                    if self.verbose:
                        print(f"  🔧 intellisense_fixer: {', '.join(changes)}")

            # Additional validation: check C file for m2c failures
            is_valid, validation_error = self.validate_c_file(c_file, function_name, original_content)

            # If text validation passes, run the actual PSY-Q compiler on the changed file.
            # This catches type errors, incompatible declarations, and other issues that
            # heuristic text checks miss.  Failures are treated the same as text-validation
            # failures: LLM auto-fix is attempted if available, then the file is reverted.
            if is_valid and self.build_verifier is not None:
                compile_ok, compile_errors = self.build_verifier.check_file_compiles(c_file)
                if not compile_ok:
                    errors_summary = "; ".join(e['message'] for e in compile_errors[:3])
                    is_valid = False
                    validation_error = f"PSY-Q compilation failed: {errors_summary}"
                    if self.verbose:
                        print(f"  Compile errors: {errors_summary}")

            if not is_valid:
                print(f"⚠️  Decompilation produced invalid output: {validation_error}")
                
                # Attempt LLM auto-fix if enabled
                llm_fixed = False
                # Trigger auto-fix for multiple error types
                should_try_llm_fix = self.llm and any([
                    "unaligned" in validation_error.lower(),
                    "decompilation failure" in validation_error.lower(),
                    "syntax error" in validation_error.lower(),
                    "m2c_error" in validation_error.lower(),
                    "psy-q compilation failed" in validation_error.lower(),
                    "void*" in validation_error.lower(),
                    "unknown parameter type" in validation_error.lower(),
                    "unknown variable type" in validation_error.lower(),
                    "primitive pointer" in validation_error.lower(),
                    "function pointer" in validation_error.lower(),
                    "saved_reg" in validation_error.lower(),
                    "unaligned type" in validation_error.lower(),
                    "unknown type" in validation_error.lower(),
                    "cast expression" in validation_error.lower(),
                ])
                
                if should_try_llm_fix:
                    print(f"🤖 Attempting automatic fix with LLM...")
                    
                    # Retrieve pre-analysis from database if available
                    pre_analysis = None
                    try:
                        cursor = self.db.conn.cursor()
                        cursor.execute("SELECT notes FROM functions WHERE name = ?", (function_name,))
                        row = cursor.fetchone()
                        if row and row['notes']:
                            # Try parsing as JSON first (new format)
                            try:
                                notes_data = json.loads(row['notes'])
                                if 'llm_analysis' in notes_data:
                                    pre_analysis = notes_data['llm_analysis']
                                    print(f"   📋 Using pre-analysis: {pre_analysis['complexity']} - {pre_analysis['summary'][:50]}...")
                            except (json.JSONDecodeError, KeyError):
                                # Fall back to old string format
                                if 'LLM Analysis:' in row['notes']:
                                    notes = row['notes']
                                    analysis_match = re.search(r'LLM Analysis: (\w+) - (.+)', notes)
                                    if analysis_match:
                                        pre_analysis = {
                                            'complexity': analysis_match.group(1),
                                            'summary': analysis_match.group(2),
                                            'c_suggestion': None
                                        }
                                        print(f"   📋 Using pre-analysis: {pre_analysis['complexity']} - {pre_analysis['summary'][:50]}...")
                    except Exception as e:
                        if self.verbose:
                            print(f"   Could not retrieve pre-analysis: {e}")
                    
                    # Extract function body
                    try:
                        content = c_file.read_text()
                        # Find function start — handle both "type func_name(" and "func_name("
                        # Use brace counting to handle nested braces correctly
                        func_header_pattern = rf'\b{re.escape(function_name)}\s*\('
                        header_match = re.search(func_header_pattern, content)
                        broken_code = None
                        if header_match:
                            # Walk backwards to include return type (one line)
                            start = content.rfind('\n', 0, header_match.start())
                            start = 0 if start == -1 else start + 1
                            # Find the opening brace
                            brace_start = content.find('{', header_match.end())
                            if brace_start != -1:
                                depth = 0
                                i = brace_start
                                while i < len(content):
                                    if content[i] == '{':
                                        depth += 1
                                    elif content[i] == '}':
                                        depth -= 1
                                        if depth == 0:
                                            broken_code = content[start:i+1]
                                            break
                                    i += 1
                        if broken_code is not None:
                            fix_result = self.llm.fix_m2c_error(
                                function_name, 
                                broken_code, 
                                validation_error,
                                pre_analysis=pre_analysis
                            )
                            
                            if fix_result and fix_result['fixed_code'] and fix_result['confidence'] in ('high', 'medium'):
                                print(f"   Confidence: {fix_result['confidence']}")
                                print(f"   {fix_result['explanation']}")
                                
                                # Replace the function in the file
                                new_content = content.replace(broken_code, fix_result['fixed_code'])
                                c_file.write_text(new_content)
                                
                                # Re-validate
                                is_valid_again, validation_error_again = self.validate_c_file(c_file, function_name, original_content)
                                
                                if is_valid_again:
                                    print(f"\u2705 LLM fix successful!")
                                    if fix_result['confidence'] == 'high':
                                        status = 'decompiled'
                                        print(f"   ✅ High confidence - marked as decompiled")
                                    else:
                                        status = 'decompiled_needs_refine'
                                        print(f"   ⚠️  Medium confidence - flagged for manual review")
                                    
                                    self.db.record_decompilation_attempt(
                                        function_name,
                                        status,
                                        notes=f"LLM auto-fix ({fix_result['confidence']} confidence): {fix_result['explanation']}"
                                    )
                                    llm_fixed = True
                                    
                                    # Log to audit file for manual review tracking
                                    try:
                                        audit_log = PROJECT_ROOT / "automation" / "llm_fixes_audit.log"
                                        with open(audit_log, 'a') as f:
                                            timestamp = datetime.now().isoformat()
                                            f.write(f"{timestamp}\t{function_name}\t{c_file}\t{fix_result['confidence']}\t{validation_error}\t{fix_result['explanation']}\n")
                                    except Exception as e:
                                        if self.verbose:
                                            print(f"   Warning: Could not write to audit log: {e}")
                                    
                                    return True
                                else:
                                    print(f"   \u274c LLM fix didn't resolve the issue: {validation_error_again}")
                            else:
                                confidence = fix_result['confidence'] if fix_result else 'unknown'
                                print(f"   \u274c LLM fix confidence too low ({confidence}), skipping")
                    except Exception as e:
                        print(f"   \u274c LLM fix failed: {e}")
                
                # Extract missing types information before reverting
                missing_types_report = None
                try:
                    missing_types_report = self.extract_missing_types(c_file, function_name, validation_error)
                    
                    # Store report to JSON file
                    reports_file = PROJECT_ROOT / "automation" / "missing_types_reports.json"
                    reports = []
                    
                    if reports_file.exists():
                        try:
                            with open(reports_file, 'r') as f:
                                reports = json.load(f)
                        except (json.JSONDecodeError, FileNotFoundError):
                            reports = []
                    
                    # Add timestamp and append
                    missing_types_report['timestamp'] = datetime.now().isoformat()
                    reports.append(missing_types_report)
                    
                    with open(reports_file, 'w') as f:
                        json.dump(reports, f, indent=2)
                    
                    if self.verbose and missing_types_report['missing_items']:
                        print(f"📊 Extracted {len(missing_types_report['missing_items'])} missing type requirements")
                except Exception as e:
                    if self.verbose:
                        print(f"   Warning: Could not extract missing types: {e}")
                
                # Determine if this is a "soft" error: the decompiled code is valid C
                # but has m2c artifacts that a human (or future pass) can clean up.
                # For soft errors we keep the code in the file instead of reverting.
                is_soft_error = not llm_fixed and any(
                    pattern in validation_error for pattern in self.soft_errors
                )

                if is_soft_error:
                    # Keep the decompiled code — it compiles and just needs tidying.
                    # But first strip any extern declarations that contain '?' types:
                    # those are syntactically invalid and cause IntelliSense errors in
                    # other editors even though PSY-Q cc1 can compile past them.
                    _content = c_file.read_text()
                    _cleaned = re.sub(
                        r'^[^\n]*//\s*extern[^\n]*\?[^\n]*\n',
                        '',
                        _content,
                        flags=re.MULTILINE,
                    )
                    # Also remove bare extern declarations with ? type markers
                    _cleaned = re.sub(
                        r'^\s*/\*\?\*/\s*\w[\w\s\*,]*\([^)]*\?[^)]*\)[^;]*;\s*//\s*extern\n',
                        '',
                        _cleaned,
                        flags=re.MULTILINE,
                    )
                    if _cleaned != _content:
                        c_file.write_text(_cleaned)
                        if self.verbose:
                            print(f"   🧹 Stripped invalid extern declarations with '?' types from soft-error file")
                    print(f"📋 Soft error — keeping decompiled code, marking as needs_refine")
                    notes_text = f"m2c soft error (kept for manual fix): {validation_error}"
                    if missing_types_report and missing_types_report['missing_items']:
                        notes_text += f"\nMissing types tracked: {len(missing_types_report['missing_items'])} items"
                    self.db.record_decompilation_attempt(
                        function_name,
                        'decompiled_needs_refine',
                        notes=notes_text
                    )
                else:
                    # Hard error — revert to INCLUDE_ASM and mark blocked.
                    if original_content and not llm_fixed:
                        print(f"🔄 Reverting {c_file.name} to INCLUDE_ASM (keeping file clean)")
                        c_file.write_text(original_content)

                    notes_text = f"m2c decompilation error (reverted): {validation_error}"
                    if missing_types_report and missing_types_report['missing_items']:
                        notes_text += f"\nMissing types tracked: {len(missing_types_report['missing_items'])} items"
                    self.db.record_decompilation_attempt(
                        function_name,
                        'blocked',
                        notes=notes_text
                    )
                return False
            
            print(f"\u2705 Decompilation successful!")
            self.db.record_decompilation_attempt(
                function_name,
                'decompiled',
                notes="Successfully decompiled by automation"
            )
            
            # Mark successful use of struct patterns in learning database
            try:
                from dependency_analyzer import DependencyAnalyzer
                analyzer = DependencyAnalyzer(PROJECT_ROOT, self.db.db_path)
                extern_symbols = analyzer.get_function_extern_symbols(function_name)
                
                for symbol in extern_symbols:
                    known_type = self.db.get_known_type(symbol)
                    if known_type:
                        # This symbol's type helped with successful decompilation
                        self.db.mark_struct_success(symbol)
                        if self.verbose:
                            print(f"   📈 Increased confidence for {symbol} pattern")
            except Exception as e:
                if self.verbose:
                    print(f"   ⚠️  Could not update struct learning: {e}")
            
            return True
            
        else:
            error_msg = parse_result['error_message'] or "Unknown error"
            error_type = parse_result.get('error_type', 'unknown')
            print(f"❌ Decompilation failed: {error_msg}")
            
            # Revert to original INCLUDE_ASM to keep file clean
            if original_content:
                print(f"🔄 Reverting {c_file.name} to INCLUDE_ASM (keeping file clean)")
                c_file.write_text(original_content)
            
            # Classify based on error type
            if error_type in ('compile', 'link'):
                # Blocking errors - likely needs manual intervention
                status = 'blocked'
                notes = f"Blocked by {error_type} error (reverted): {error_msg}"
            else:
                # Other failures - can retry
                status = 'failed'
                notes = f"Decompilation failed ({error_type}, reverted): {error_msg}"
            
            self.db.record_decompilation_attempt(
                function_name,
                status,
                notes=notes
            )
            return False
    
    def decompile_next_batch(self, batch_size: int = 1, module: Optional[str] = None):
        """
        Decompile the next N easiest functions.
        
        Args:
            batch_size: Number of functions to attempt
            module: Optional module filter (e.g., 'battle', 'field')
        """
        print(f"\n{'='*60}")
        print(f"BATCH DECOMPILATION: Processing {batch_size} function(s)")
        if module:
            print(f"Filtering by module: {module}")
        print(f"{'='*60}\n")
        
        # Get functions to process
        functions = self.db.get_functions_by_status(
            'todo',
            limit=batch_size,
            module=module,
            order_by='difficulty_score'  # Easiest first (from mako.sh rank scores)
        )
        
        if not functions:
            print("✅ No more functions to decompile!")
            return
        
        print(f"Found {len(functions)} function(s) to process\n")
        
        success_count = 0
        failed_count = 0
        
        for i, func in enumerate(functions, 1):
            print(f"\n[{i}/{len(functions)}] Processing: {func['name']}")
            
            if self.decompile_function(func['name']):
                success_count += 1
            else:
                failed_count += 1
        
        # Summary
        print(f"\n{'='*60}")
        print(f"BATCH COMPLETE")
        print(f"{'='*60}")
        print(f"✅ Successful: {success_count}")
        print(f"❌ Failed: {failed_count}")
        print(f"{'='*60}\n")


def show_status(db: DecompDatabase):
    """Show overall decompilation progress."""
    stats = db.get_statistics()
    
    print("\n" + "="*60)
    print("DECOMPILATION PROGRESS")
    print("="*60)
    
    total = stats['total']
    by_status = stats['by_status']
    
    # Calculate progress
    verified = by_status.get('verified', 0)
    decompiled = by_status.get('decompiled', 0)
    needs_refine = by_status.get('decompiled_needs_refine', 0)
    in_progress = by_status.get('in_progress', 0)
    failed = by_status.get('failed', 0)
    blocked = by_status.get('blocked', 0)
    todo = by_status.get('todo', 0)
    
    if total > 0:
        # Progress includes verified + decompiled + needs_refine
        completed = verified + decompiled + needs_refine
        progress_pct = (completed / total) * 100
        print(f"\nTotal Functions: {total}")
        print(f"Progress: {completed}/{total} ({progress_pct:.1f}%)")
        print(f"\nStatus Breakdown:")
        print(f"  ✅ Verified:    {verified:>6}")
        print(f"  🔄 Decompiled:  {decompiled:>6}")
        print(f"  🔧 Needs Refine: {needs_refine:>6}")
        print(f"  ⚙️  In Progress: {in_progress:>6}")
        print(f"  ⚠️  Blocked:     {blocked:>6}")
        print(f"  ❌ Failed:      {failed:>6}")
        print(f"  ⏳ Todo:        {todo:>6}")
        
        # Show by module
        print(f"\nBy Module:")
        for module, count in stats['by_module'].items():
            print(f"  {module:15s}: {count:>4}")
    
    print("="*60 + "\n")


def main():
    parser = argparse.ArgumentParser(description="Automated decompilation runner")
    parser.add_argument('--auto', action='store_true', 
                       help='Process next function(s) automatically')
    parser.add_argument('--batch', type=int, default=1, 
                       help='Number of functions to process (default: 1)')
    parser.add_argument('--function', type=str, 
                       help='Decompile a specific function by name')
    parser.add_argument('--module', type=str, 
                       help='Filter by module (e.g., battle, field)')
    parser.add_argument('--status', action='store_true', 
                       help='Show decompilation progress')
    parser.add_argument('--verify', action='store_true',
                       help='Verify decompiled functions after completion')
    parser.add_argument('--audit', action='store_true',
                       help='Audit existing decompiled/verified functions for m2c errors')
    parser.add_argument('--llm-auto-fix', action='store_true',
                       help='Enable automatic LLM fixing of m2c errors (requires Ollama)')
    parser.add_argument('--verbose', '-v', action='store_true', 
                       help='Verbose output')
    
    args = parser.parse_args()
    
    # Initialize database
    db_path = PROJECT_ROOT / "automation" / "functions.db"
    db = DecompDatabase(str(db_path))
    runner = DecompilationRunner(db, verbose=args.verbose, enable_llm_fixes=args.llm_auto_fix)
    
    if args.status:
        show_status(db)
    
    elif args.audit:
        # Audit existing decompilations for m2c errors
        print("\n" + "="*60)
        print("AUDITING DECOMPILED FUNCTIONS")
        print("="*60 + "\n")
        
        # Check both decompiled and verified functions
        to_check = []
        to_check.extend(db.get_functions_by_status('decompiled'))
        to_check.extend(db.get_functions_by_status('verified'))
        
        if not to_check:
            print("No decompiled/verified functions to audit.")
        else:
            print(f"Auditing {len(to_check)} function(s)...\n")
            
            invalid_count = 0
            for i, func in enumerate(to_check, 1):
                c_file = PROJECT_ROOT / func['c_file_path']
                is_valid, error = runner.validate_c_file(c_file, func['name'], None)
                if not is_valid:
                    invalid_count += 1
                    print(f"[{i}/{len(to_check)}] ❌ {func['name']}: {error}")
                    # Mark as blocked
                    db.update_function_status(
                        func['name'],
                        'blocked',
                        notes=f"Audit found m2c error: {error}"
                    )
                elif args.verbose:
                    print(f"[{i}/{len(to_check)}] ✅ {func['name']}")
            
            print(f"\n{'='*60}")
            print(f"Audit complete: {invalid_count} invalid decompilations found")
            if invalid_count > 0:
                print(f"Marked {invalid_count} function(s) as 'blocked' for manual review")
            print(f"{'='*60}\n")
    
    elif args.function:
        # Decompile specific function
        success = runner.decompile_function(args.function)
        
        # Optionally verify
        if success and args.verify:
            print("\n" + "="*60)
            print("Running verification...")
            print("="*60)
            try:
                from build import BuildVerifier
                verifier = BuildVerifier(db, verbose=args.verbose)
                verifier.verify_decompiled_functions()
            except ImportError:
                print("⚠️  build.py not found, skipping verification")
        
        sys.exit(0 if success else 1)
    
    elif args.auto:
        # Batch decompilation
        runner.decompile_next_batch(
            batch_size=args.batch,
            module=args.module
        )
        
        # Optionally verify
        if args.verify:
            print("\n" + "="*60)
            print("Running verification...")
            print("="*60)
            try:
                from build import BuildVerifier
                verifier = BuildVerifier(db, verbose=args.verbose)
                verifier.verify_decompiled_functions()
            except ImportError:
                print("⚠️  build.py not found, skipping verification")
    
    else:
        parser.print_help()
        print("\nQuick examples:")
        print("  python decompile.py --status")
        print("  python decompile.py --auto")
        print("  python decompile.py --auto --batch 10")
        print("  python decompile.py --auto --verify")
        print("  python decompile.py --function AverageSZ4")


if __name__ == '__main__':
    main()
