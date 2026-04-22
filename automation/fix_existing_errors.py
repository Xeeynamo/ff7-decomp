#!/usr/bin/env python3
"""
Standalone script to fix type errors in existing decompiled functions.

This scans files with compilation errors and uses LLM to fix void* type issues.
"""

import os
import sys
import re
import subprocess
from pathlib import Path
from datetime import datetime
from typing import List, Tuple, Dict, Optional
from database import DecompDatabase

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from llm_helper import LLMHelper
    HAS_LLM = True
except ImportError:
    HAS_LLM = False
    LLMHelper = None


class TypeErrorFixer:
    """Fix type errors in decompiled functions using LLM."""
    
    def __init__(self, model: str = "qwen2.5-coder:7b", verbose: bool = False):
        """Initialize the fixer with Ollama."""
        if not HAS_LLM:
            print("[ERROR] llm_helper not available")
            sys.exit(1)
        
        self.llm = LLMHelper(model=model, verbose=verbose)
        if not self.llm.available:
            print("[ERROR] Ollama not available. Please install and start Ollama:")
            print("        https://ollama.ai/")
            sys.exit(1)
        
        self.db = DecompDatabase()
        self.workspace_root = Path(__file__).parent.parent.absolute()
        self.file_context = {}  # Cache for file-level context
        # Track functions that already caused validation failures so we skip them
        # in subsequent passes. Key: func_name, Value: number of validation failures.
        self._validation_failures: Dict[str, int] = {}
    
    def extract_file_context(self, file_path: str) -> Dict:
        """
        Extract file-level context (typedefs, externs, includes).
        Returns dict with 'typedefs', 'externs', 'includes'.
        """
        if file_path in self.file_context:
            return self.file_context[file_path]
        
        with open(file_path, 'r') as f:
            content = f.read()
        
        context = {
            'typedefs': [],
            'externs': [],
            'includes': [],
            'header_typedef_names': []
        }
        
        # Extract typedef declarations
        typedef_pattern = re.compile(r'typedef\s+(?:struct|enum|union)?\s*\w*\s*\{[^}]+\}\s*\w+;', re.MULTILINE | re.DOTALL)
        for match in typedef_pattern.finditer(content):
            typedef_text = match.group(0)
            # Extract the name
            name_match = re.search(r'\}\s*(\w+);', typedef_text)
            if name_match:
                context['typedefs'].append({
                    'name': name_match.group(1),
                    'text': typedef_text
                })
        
        # Extract extern declarations
        extern_pattern = re.compile(r'extern\s+[^;]+;')
        for match in extern_pattern.finditer(content):
            context['externs'].append(match.group(0))
        
        # Extract includes and follow them (up to 3 levels deep) to collect all visible typedef names
        include_pattern = re.compile(r'#include\s+[<"]([^>"]+)[>"]')
        header_typedef_names = set()
        
        def _collect_from_header(hpath: Path, depth: int):
            if depth <= 0 or not hpath.exists():
                return
            names = self._extract_typedef_names_from_header(hpath)
            header_typedef_names.update(names)
            try:
                hcontent = hpath.read_text(errors='replace')
            except Exception:
                return
            for m in include_pattern.finditer(hcontent):
                sub_name = m.group(1)
                sub_path = self._resolve_include(sub_name, str(hpath))
                if sub_path:
                    _collect_from_header(sub_path, depth - 1)
        
        for match in include_pattern.finditer(content):
            include_name = match.group(1)
            context['includes'].append(f'#include "{include_name}"')
            header_path = self._resolve_include(include_name, file_path)
            if header_path:
                _collect_from_header(header_path, depth=5)
        context['header_typedef_names'] = sorted(header_typedef_names)
        
        self.file_context[file_path] = context
        return context
    
    def _resolve_include(self, include_name: str, source_file: str) -> Optional[Path]:
        """Resolve an #include filename to an actual path, searching standard dirs."""
        candidates = [
            Path(source_file).parent / include_name,
            self.workspace_root / 'include' / include_name,
            self.workspace_root / 'include' / 'psxsdk' / include_name,
            self.workspace_root / include_name,
        ]
        for candidate in candidates:
            if candidate.exists():
                return candidate
        return None
    
    def _extract_typedef_names_from_header(self, header_path: Path) -> List[str]:
        """
        Extract all typedef names from a header file.
        Returns a flat list of type name strings.
        """
        try:
            content = header_path.read_text(errors='replace')
        except Exception:
            return []
        names = []
        # Struct/union/enum typedef: '} TypeName;'
        names += re.findall(r'\}\s*(\w+)\s*;', content)
        # Simple typedef: 'typedef <something> TypeName;'
        for m in re.finditer(r'^typedef\s+\S.*?\s+(\w+)\s*;', content, re.MULTILINE):
            names.append(m.group(1))
        return [n for n in names if n and not n[0].isdigit()]

    def _extract_struct_field_maps(self, header_path: Path) -> Dict[str, Dict[int, tuple]]:
        """
        Parse struct definitions from a header file.
        Returns {struct_name: {byte_offset: (field_name, c_type)}} using the
        '/* 0xNN */' offset comments common in this codebase.  If a field has
        no offset comment the offset is derived from sequential layout.
        """
        TYPE_SIZES = {
            'u8': 1, 's8': 1, 'char': 1,
            'u16': 2, 's16': 2,
            'u32': 4, 's32': 4, 'int': 4, 'float': 4,
            'u64': 8, 's64': 8, 'double': 8,
        }
        try:
            content = header_path.read_text(errors='replace')
        except Exception:
            return {}

        result: Dict[str, Dict[int, tuple]] = {}
        # Match: typedef struct [OptName] { body } TypeName;
        struct_re = re.compile(
            r'typedef\s+struct\s*\w*\s*\{([^}]+)\}\s*(\w+)\s*;', re.DOTALL
        )
        field_re = re.compile(
            r'(?:/\*\s*(0x[0-9a-fA-F]+|\d+)\s*\*/\s*)?'   # optional /* 0xNN */
            r'([\w\s\*]+?)\s+(\w+)\s*(?:\[[\w\s\+\-\*]*\])?\s*;'  # type name[array];
        )
        for sm in struct_re.finditer(content):
            body = sm.group(1)
            struct_name = sm.group(2)
            fields: Dict[int, tuple] = {}
            current_offset = 0
            for fm in field_re.finditer(body):
                offset_str, ftype, fname = fm.group(1), fm.group(2).strip(), fm.group(3)
                if offset_str:
                    try:
                        current_offset = int(offset_str, 16) if '0x' in offset_str else int(offset_str)
                    except ValueError:
                        pass
                fields[current_offset] = (fname, ftype)
                # Advance for next field
                base = ftype.rstrip('* ').split()[-1] if ftype.split() else ftype
                size = 4 if '*' in ftype else TYPE_SIZES.get(base, 4)
                current_offset += size
            if fields:
                result[struct_name] = fields
        return result

    def _collect_struct_field_maps(self, file_path: str) -> Dict[str, Dict[int, tuple]]:
        """
        Walk the transitive include chain for file_path and return the merged
        struct field map (see _extract_struct_field_maps).
        """
        include_pattern = re.compile(r'#include\s+[<"]([^>"]+)[>"]')
        all_maps: Dict[str, Dict[int, tuple]] = {}
        visited: set = set()

        def _walk(path: Path, depth: int):
            key = str(path)
            if key in visited or depth <= 0 or not path.exists():
                return
            visited.add(key)
            maps = self._extract_struct_field_maps(path)
            for k, v in maps.items():
                if k not in all_maps:
                    all_maps[k] = v
            try:
                content = path.read_text(errors='replace')
            except Exception:
                return
            for m in include_pattern.finditer(content):
                sub = self._resolve_include(m.group(1), str(path))
                if sub:
                    _walk(sub, depth - 1)

        try:
            with open(file_path) as f:
                src = f.read()
        except Exception:
            return all_maps
        for m in include_pattern.finditer(src):
            hp = self._resolve_include(m.group(1), file_path)
            if hp:
                _walk(hp, depth=5)
        return all_maps

    def _get_psyq_params(self, rel_path: str):
        """
        Parse build.ninja to find PSY-Q compiler parameters for a source file.
        Returns (cc1_binary, cc_flags) or None if the file has no psx-cc rule.
        """
        ninja_file = self.workspace_root / 'build.ninja'
        if not ninja_file.exists():
            return None

        lines = ninja_file.read_text().splitlines()

        # Find: "build <obj>.o: psx-cc <rel_path>" (possibly with a trailing " | $")
        build_line_idx = None
        for i, line in enumerate(lines):
            if re.match(r'build\s+\S+\.o\s*:\s*psx-cc\s+' + re.escape(rel_path) + r'\b', line):
                build_line_idx = i
                break

        if build_line_idx is None:
            return None

        # Skip the build rule and any continuation lines (ending with "$")
        idx = build_line_idx + 1
        while idx < len(lines) and lines[idx - 1].endswith('$'):
            idx += 1

        # Read the indented variable block that follows
        cc1 = 'cc1-psx-26'
        cc_flags = '-O2 -G0'
        while idx < len(lines) and lines[idx].startswith('  '):
            m = re.match(r'\s+cc1\s*=\s*(\S+)', lines[idx])
            if m:
                cc1 = m.group(1)
            m = re.match(r'\s+cc_flags\s*=\s*(.+)', lines[idx])
            if m:
                cc_flags = m.group(1).strip()
            idx += 1

        return cc1, cc_flags

    def get_compilation_errors(self, file_path: str) -> List[Dict]:
        """
        Get compilation errors for a file by running the PSY-Q compiler directly.
        Parses build.ninja to find the correct cc1 binary and flags for this file,
        then invokes the cpp | str | iconv | cc1 pipeline directly — bypassing the
        full `make` chain that fails at the linker step before overlay files compile.
        Falls back to `make` if no PSY-Q rule is found in build.ninja.
        """
        try:
            rel_path = str(Path(file_path).relative_to(self.workspace_root))
            file_name = Path(file_path).name

            params = self._get_psyq_params(rel_path)

            if params:
                cc1, cc_flags = params
                # Run the PSY-Q pipeline up to cc1; redirect cc1 stdout (assembly)
                # to /dev/null — errors go to stderr which we capture.
                cmd = (
                    f'mipsel-linux-gnu-cpp -Iinclude -Iinclude/psxsdk -DUSE_INCLUDE_ASM -DFF7_STR'
                    f' -lang-c -Iinclude -Iinclude/psxsdk -undef -Wall -fno-builtin {rel_path}'
                    f' | bin/str'
                    f' | iconv --from-code=UTF-8 --to-code=Shift-JIS'
                    f' | bin/{cc1} -quiet -mcpu=3000 -g -mgas -gcoff {cc_flags} > /dev/null'
                )
                result = subprocess.run(
                    cmd, shell=True,
                    cwd=str(self.workspace_root),
                    stderr=subprocess.PIPE,
                    text=True,
                    timeout=60
                )
                output = result.stderr
            else:
                # Fallback for files that go through make directly (e.g. main overlay)
                Path(file_path).touch()
                result = subprocess.run(
                    ['make'],
                    cwd=str(self.workspace_root),
                    capture_output=True,
                    text=True,
                    timeout=60
                )
                output = result.stdout + '\n' + result.stderr

            errors = []
            error_pattern = re.compile(
                rf'{re.escape(file_name)}:(\d+):\s*(?:warning:\s*)?(.+)'
            )

            for line in output.split('\n'):
                match = error_pattern.search(line)
                if match:
                    line_num = int(match.group(1))
                    message = match.group(2).strip()

                    # Skip pure warnings unless they're relevant
                    if 'warning:' in line.lower() and not any(x in message.lower()
                            for x in ['incompatible', 'pointer', 'cast', 'type']):
                        continue

                    category = self._categorize_error(message)
                    errors.append({
                        'line': line_num,
                        'type': category,
                        'message': message,
                        'raw_type': 'error' if 'error' in line.lower() else 'warning'
                    })

            return errors

        except subprocess.TimeoutExpired:
            print(f"[WARN] Compilation timed out for {file_path}")
            return []
        except Exception as e:
            print(f"[WARN] Could not compile {file_path}: {e}")
            return []
    
    def _categorize_error(self, message: str) -> str:
        """Categorize error message into error type."""
        msg_lower = message.lower()
        
        # PSY-Q compiler specific error messages
        if 'undeclared' in msg_lower or 'not declared' in msg_lower:
            return 'undefined_identifier'
        elif 'parse error' in msg_lower:
            return 'syntax_error'
        elif 'incompatible types' in msg_lower or 'incompatible pointer' in msg_lower:
            return 'type_mismatch'
        elif 'invalid type argument' in msg_lower:
            return 'invalid_operation'
        elif 'redefinition' in msg_lower or 'previously declared' in msg_lower:
            return 'redefinition'
        elif 'duplicate label' in msg_lower:
            return 'duplicate_label'
        elif 'struct' in msg_lower and ('member' in msg_lower or 'field' in msg_lower):
            return 'missing_struct_field'
        elif 'void' in msg_lower and ('dereference' in msg_lower or 'incomplete type' in msg_lower):
            return 'void_pointer_dereference'
        elif 'pointer' in msg_lower and ('integer' in msg_lower or 'cast' in msg_lower):
            return 'pointer_cast'
        elif 'assignment' in msg_lower and 'incompatible' in msg_lower:
            return 'type_mismatch'
        else:
            return 'unknown'
    
    def detect_type_errors_in_function(self, code: str, start_line: int, 
                                       compiler_errors: List[Dict]) -> List[Dict]:
        """
        Detect type errors in function code using compiler errors.
        Returns list of error dictionaries filtered to this function's line range.
        """
        code_lines = code.count('\n') + 1
        end_line = start_line + code_lines - 1
        
        # Filter compiler errors to this function
        func_errors = []
        for err in compiler_errors:
            err_line = err['line']
            if start_line <= err_line <= end_line:
                # Add relative line number within function
                err_copy = err.copy()
                err_copy['func_line'] = err_line - start_line + 1
                func_errors.append(err_copy)
        
        # Also detect potential issues from code patterns
        # Pattern 1: void* pointer dereference with ->
        if 'void*' in code or 'void *' in code:
            code_lower = code.lower()
            if '->' in code and ('void*' in code_lower or 'void *' in code_lower):
                # Check if not already in compiler errors
                has_void_error = any(e['type'] == 'void_pointer_dereference' for e in func_errors)
                if not has_void_error:
                    func_errors.append({
                        'line': start_line,
                        'func_line': 1,
                        'type': 'void_pointer_dereference',
                        'message': 'void* pointer used with -> operator',
                        'raw_type': 'potential'
                    })
        
        # Pattern 2: Struct field access on void* variables
        void_ptr_pattern = re.compile(r'\b(\w+)\s*\*\s*(\w+)\s*=.*?void\s*\*', re.DOTALL)
        struct_access_pattern = re.compile(r'\b(\w+)->unk\w*')
        
        void_pointers = set()
        for match in void_ptr_pattern.finditer(code):
            var_name = match.group(2)
            void_pointers.add(var_name)
        
        for match in struct_access_pattern.finditer(code):
            var_name = match.group(1)
            if var_name in void_pointers:
                has_struct_error = any(
                    var_name in e.get('message', '') for e in func_errors
                )
                if not has_struct_error:
                    func_errors.append({
                        'line': start_line,
                        'func_line': 1,
                        'type': 'void_pointer_dereference',
                        'message': f'Struct field access on void* variable: {var_name}',
                        'raw_type': 'potential'
                    })
        
        return func_errors
    
    def infer_struct_type_from_fields(self, code: str) -> Optional[str]:
        """
        Infer struct type from field access patterns.
        Returns struct typedef or None.
        """
        # Find all field accesses (->unk0, ->unk4, etc.)
        field_accesses = re.findall(r'->unk([0-9A-Fa-fxX]+)', code)
        if not field_accesses:
            return None
        
        # Convert hex to decimal and get unique fields
        fields = set()
        for f in field_accesses:
            try:
                # Remove 0x prefix if present and convert
                f_clean = f.replace('0x', '').replace('0X', '')
                fields.add(int(f_clean, 16) if any(c in f_clean.upper() for c in 'ABCDEF') else int(f_clean))
            except ValueError:
                continue
        
        if not fields:
            return None
        
        fields = sorted(fields)
        max_field = max(fields)
        
        # Build struct based on fields (4-byte aligned for PS1)
        struct_def = "typedef struct SpuMallocBlock {\n"
        for offset in range(0, max_field + 4, 4):
            hex_offset = f"{offset:X}"
            if offset in fields:
                struct_def += f"    s32 unk{hex_offset};  /* offset 0x{hex_offset} */\n"
        struct_def += "} SpuMallocBlock;"
        
        return struct_def
    
    def discover_functions_in_file(self, file_path: str) -> List[Tuple[str, int, int]]:
        """
        Discover all actual decompiled functions in a file by parsing the code.
        Returns list of (func_name, start_line, end_line).
        """
        with open(file_path, 'r') as f:
            lines = f.readlines()
        
        functions = []
        i = 0
        while i < len(lines):
            line = lines[i]
            
            # Skip INCLUDE_ASM lines
            if 'INCLUDE_ASM' in line:
                i += 1
                continue
            
            # Strip inline // comments before checking whether the line is a
            # declaration (ends with ';') vs. a definition (has a body).
            # Lines like "void func_800D5350();  // extern" must be skipped;
            # rstrip() alone keeps the comment, so the ';' check fails.
            stripped_no_comment = re.sub(r'//.*$', '', line).rstrip()
            if '(' not in line or stripped_no_comment.endswith(';'):
                i += 1
                continue
            
            # Check if this looks like a function definition
            match = re.search(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\s*\(', line)
            if match and not line.strip().startswith('//') and not line.strip().startswith('/*'):
                func_name = match.group(1)
                
                # Skip common non-function keywords
                if func_name in ['if', 'while', 'for', 'switch', 'return', 'sizeof', 
                                  'typedef', 'struct', 'union', 'enum', 'INCLUDE_ASM']:
                    i += 1
                    continue
                
                # Find opening brace
                brace_line = None
                for j in range(i, min(i + 15, len(lines))):
                    if '{' in lines[j]:
                        brace_line = j
                        break
                
                if brace_line is not None:
                    # Find closing brace
                    brace_count = 0
                    end_line = None
                    for j in range(brace_line, len(lines)):
                        brace_count += lines[j].count('{')
                        brace_count -= lines[j].count('}')
                        if brace_count == 0:
                            end_line = j
                            break
                    
                    if end_line is not None and end_line > brace_line:
                        functions.append((func_name, i, end_line))
                        i = end_line + 1
                        continue
            
            i += 1
        
        return functions

    # ------------------------------------------------------------------
    # Deterministic pre-pass fixes (no LLM required)
    # ------------------------------------------------------------------

    def _offset_from_unk_name(self, field_name: str) -> Optional[int]:
        """
        Convert a decompiler unk-field name to a byte offset integer.
        e.g. 'unk4' -> 4, 'unk1C' -> 28, 'unk16' -> 22.
        Returns None if the name doesn't look like an unk-offset field.
        """
        m = re.match(r'^unk([0-9A-Fa-f]+)$', field_name)
        if not m:
            return None
        hex_part = m.group(1)
        # Treat as hex if it contains A-F or starts with 0, else try decimal first
        if any(c in hex_part.upper() for c in 'ABCDEF') or (len(hex_part) > 1 and hex_part[0] == '0'):
            return int(hex_part, 16)
        # Ambiguous (e.g. "unk8", "unk4") — in this codebase these are hex offsets
        return int(hex_part, 16)

    def apply_deterministic_fixes(self, file_path: str, compiler_errors: List[Dict],
                                   struct_maps: Dict[str, Dict[int, tuple]]) -> int:
        """
        Apply mechanical fixes for errors that don't need LLM reasoning:
          - missing_struct_field: look up the correct field name by byte offset
          - pointer_cast (integer↔pointer): insert explicit u32/intptr cast
        Modifies file_path in-place.  Returns number of lines changed.
        """
        # Process errors from last line to first so edits don't invalidate indices
        fixable = sorted(
            [e for e in compiler_errors if e['type'] in ('missing_struct_field', 'pointer_cast')],
            key=lambda e: e['line'], reverse=True
        )
        if not fixable:
            return 0

        with open(file_path, 'r') as fh:
            lines = fh.readlines()

        changed = 0
        for err in fixable:
            idx = err['line'] - 1
            if idx < 0 or idx >= len(lines):
                continue
            original = lines[idx]
            fixed = self._fix_one_line(original, err, struct_maps)
            if fixed is not None and fixed != original:
                lines[idx] = fixed
                changed += 1

        if changed:
            with open(file_path, 'w') as fh:
                fh.writelines(lines)
        return changed

    def _fix_one_line(self, line: str, err: Dict,
                      struct_maps: Dict[str, Dict[int, tuple]]) -> Optional[str]:
        """Attempt a deterministic single-line fix for one compiler error."""
        msg = err['message']
        err_type = err['type']

        if err_type == 'missing_struct_field':
            return self._fix_missing_field(line, msg, struct_maps)
        if err_type == 'pointer_cast':
            return self._fix_pointer_cast(line, msg)
        return None

    def _fix_missing_field(self, line: str, msg: str,
                            struct_maps: Dict[str, Dict[int, tuple]]) -> Optional[str]:
        """
        Fix 'structure has no member named unkXX' by replacing the access with a
        byte-offset cast.  This is always type-safe without needing to infer
        the pointer's declared struct type.

        Handles both LHS (assignment target) and RHS (value read) positions.
        The cast type defaults to s16 for small offsets (< 0x10) and s32 otherwise,
        which is a reasonable guess for PS1 struct fields.
        """
        bad_field_m = re.search(r"no member named [`'\"]?(\w+)[`'\"]?", msg)
        if not bad_field_m:
            return None
        bad_field = bad_field_m.group(1)       # e.g. 'unk4'
        offset = self._offset_from_unk_name(bad_field)
        if offset is None:
            return None  # Not an unkXX name — can't determine offset safely

        # Access pattern: ptr->unkX  or  ptr.unkX
        access_re = re.compile(r'(\w+)(->|\.)(' + re.escape(bad_field) + r')\b')
        if not access_re.search(line):
            return None  # Pattern not found on this line — nothing to do

        # Choose a sensible cast type based on field size hint from any known struct
        cast_type = 's16' if offset < 0x10 else 's32'
        for _sname, field_map in struct_maps.items():
            if offset in field_map:
                _, ftype = field_map[offset]
                # Clean up noise from multi-line comments in the header
                base = ftype.strip().split('\n')[0].strip()
                if base in ('u8', 's8', 'u16', 's16', 'u32', 's32', 'u64', 's64'):
                    cast_type = base
                    break

        def byte_cast_sub(m: re.Match) -> str:
            ptr = m.group(1)
            op  = m.group(2)
            ref = ptr if op == '->' else f'&{ptr}'
            return f'(*({cast_type}*)((u8*){ref} + {offset}))'

        result = access_re.sub(byte_cast_sub, line)
        if result == line:
            return None
        return result

    def _fix_pointer_cast(self, line: str, msg: str) -> Optional[str]:
        """
        Add explicit casts to silence integer↔pointer warnings on a single line.
        Only applies safe, well-understood transformations.
        """
        if 'makes integer from pointer without a cast' in msg:
            # lhs = ptr_expr;  →  lhs = (u32)ptr_expr;
            # Only handle simple `var = rhs;` assignments — avoid touching calls with
            # multiple = in the same statement.
            m = re.match(r'^(\s*[\w.\[\]\->]+\s*=\s*)([^=;][^;]*)(;.*)$', line)
            if m and not re.search(r'\(u(32|64)\s*\)', m.group(2)):
                return f'{m.group(1)}(u32){m.group(2)}{m.group(3)}\n'

        elif 'makes pointer from integer without a cast' in msg:
            # lhs = int_expr;  →  lhs = (void*)int_expr;
            # (The correct pointer type would need type inference; void* is safe.)
            m = re.match(r'^(\s*[\w.\[\]\->]+\s*=\s*)([^=;][^;]*)(;.*)$', line)
            if m and not re.search(r'^\s*\(', m.group(2)):
                return f'{m.group(1)}(void*){m.group(2)}{m.group(3)}\n'

        elif 'cast to pointer from integer of different size' in msg:
            # (SomeType*)intval → (SomeType*)(u32)intval
            result = re.sub(
                r'\((\w[\w\s]*\s*\*+)\)\s*(\w[\w.\[\]\->]*)',
                r'(\1)(u32)\2', line
            )
            if result != line:
                return result

        return None

    def fix_function_with_llm(self, func_name: str, file_path: str, code: str,
                              start_line: int, compiler_errors: List[Dict]) -> Optional[str]:
        """
        Use LLM to fix type errors in a function.
        Returns fixed code or None if fixing failed.
        """
        errors = self.detect_type_errors_in_function(code, start_line, compiler_errors)
        if not errors:
            return None  # No errors detected
        
        code_size = len(code)
        error_count = len(errors)
        
        # Adjust timeout based on size and error count
        if code_size > 5000 or error_count > 20:
            timeout = 360  # 6 minutes for very large/complex functions
        elif code_size > 3000 or error_count > 10:
            timeout = 240  # 4 minutes for large functions
        else:
            timeout = 120  # 2 minutes for normal functions
        
        print(f"[INFO]   🔍 {func_name}: Found {error_count} error(s) ({code_size} chars, {timeout}s timeout)")
        
        # Get file-level context
        file_ctx = self.extract_file_context(file_path)

        # Build struct field maps (byte_offset → field_name) for all known structs
        struct_maps = self._collect_struct_field_maps(file_path)
        
        # Build context string
        context_str = ""
        
        # Collect ALL known type names from both the .c file and its included headers
        all_known_types = set(t['name'] for t in file_ctx['typedefs'])
        all_known_types.update(file_ctx.get('header_typedef_names', []))
        if all_known_types:
            context_str += f"\n**ALL Defined Types (from file and headers) — DO NOT redefine any of these:**\n"
            context_str += ', '.join(sorted(all_known_types)) + '\n'
            context_str += ("CRITICAL: These types are already defined in included headers. "
                            "Adding a new typedef or struct definition for any of these names "
                            "will cause cascading redefinition errors across the entire file. "
                            "If a struct is missing a field, you CANNOT add that field here — "
                            "only fix the code within the function body using casts or type changes.\n")

        # Inject struct field maps so the LLM knows the EXACT correct field names
        if struct_maps and 'missing_struct_field' in {e['type'] for e in errors}:
            context_str += "\n**Struct Field Reference (byte offset → field name, type):**\n"
            context_str += "Use these to replace accesses to non-existent unkXX fields.\n"
            for sname, field_map in sorted(struct_maps.items()):
                if not field_map:
                    continue
                # Only include structs that appear in the function's code or its errors
                if sname not in code and not any(sname in e['message'] for e in errors):
                    continue
                fields_repr = ', '.join(
                    f'0x{off:02X}: {fname} ({ftype})'
                    for off, (fname, ftype) in sorted(field_map.items())
                )
                context_str += f"  {sname}: {{ {fields_repr} }}\n"
        
        if file_ctx['externs']:
            extern_samples = file_ctx['externs'][:10]
            context_str += f"\n**Existing Extern Declarations:** (showing first 10)\n"
            for ext in extern_samples:
                context_str += f"  {ext}\n"
        
        # Group errors by type
        error_types = {}
        for err in errors:
            err_type = err['type']
            if err_type not in error_types:
                error_types[err_type] = []
            error_types[err_type].append(err)
        
        # Build error summary for prompt
        error_summary = []
        for err_type, err_list in error_types.items():
            error_summary.append(f"**{err_type.replace('_', ' ').title()}** ({len(err_list)} occurrences):")
            for err in err_list[:5]:  # Limit to first 5 of each type
                line_info = f"Line {err.get('func_line', '?')}" if 'func_line' in err else f"Line {err['line']}"
                error_summary.append(f"  - {line_info}: {err['message']}")
            if len(err_list) > 5:
                error_summary.append(f"  ... and {len(err_list) - 5} more")
        
        error_list = '\n'.join(error_summary)
        
        # Generate specialized instructions based on dominant error type
        dominant_type = max(error_types.keys(), key=lambda k: len(error_types[k]))
        instructions = self._generate_fix_instructions(dominant_type, error_types, struct_maps)
        
        # For large functions, infer struct type first to help LLM - but only
        # suggest new types that are NOT already defined anywhere
        struct_hint = ""
        if code_size > 2000 or 'void_pointer_dereference' in error_types:
            inferred_struct = self.infer_struct_type_from_fields(code)
            if inferred_struct:
                # Check if this struct already exists in file or headers
                struct_name_match = re.search(r'typedef struct \w+ \{', inferred_struct)
                if struct_name_match:
                    struct_name = struct_name_match.group(0).split()[-2]
                    if struct_name not in all_known_types:
                        struct_hint = f"\n**Suggested New Struct Type (safe to add — not already defined):**\n```c\n{inferred_struct}\n```\n"
        
        # Build prompt for LLM
        prompt = f"""## Project: Final Fantasy VII (PS1 USA) Decompilation
Goal: byte-accurate recompilation — the output C must compile to IDENTICAL machine code as the original PSX binary.

### PSX SDK Types — use ONLY these, never int/short/char/long
s8/u8  (signed/unsigned char, lb/lbu)  |  s16/u16  (signed/unsigned short, lh/lhu)  |  s32/u32  (signed/unsigned int, lw/sw)

### Decompilation Rules (MUST follow)
- No `/*?*/` unknown types — infer from register/context
- No `void*` for struct pointer params — use concrete struct types
- Struct fields via `->` / `.` only — no manual pointer arithmetic `*(ptr + offset)`
- Arrays via `arr[i]` — not `*(arr + i * n)`
- `goto loop_XX` → rewrite as `while` / `do-while`
- `goto block_XX` in switch → inline the branch target body
- Type must match load instruction: s8→`lb`, u8→`lbu`, s16→`lh`, u16→`lhu`

### Common Matching Pitfalls
- Merged calls: duplicate common calls inside each if/else branch (don't hoist them out)
- Struct vs isolated locals: use `RECT`/struct for groups of related stack vars
- Unknown struct fields: use `unkXX` (offset hex) or cast `*(s16*)((u8*)ptr + 0x10)`

**Function:** {func_name}
**File:** {file_path}
{context_str}
**Compilation Errors:**
{error_list}
{struct_hint}
**Current Code:**
```c
{code}
```

**Fix Instructions:**
{instructions}

**Output Format:**
Return ONLY the fixed C function code (no markdown, no explanation, no comments about what you changed).

CRITICAL RULES:
1. NEVER add or redefine any typedef, struct, union, or enum that appears in the "ALL Defined Types" list above
2. NEVER add a new struct/typedef definition to fix "structure has no member" errors — those require header changes and cannot be fixed here
3. For "structure has no member named X" errors: fix by casting the pointer to a compatible type (e.g., change the variable's declared type or add an explicit cast like `(StructType*)ptr`)
4. Do NOT include code fences (```) or explanatory text
5. Only add NEW extern declarations if the variable is truly undeclared and not in the headers
6. Start directly with the function's return type (no preamble)
7. Add `/* LLM: Fixed [error_type] */` comment only on lines you modify within the function body

Start your response with the function's return type (e.g., 'void', 's32', etc.)."""
        
        try:
            # Call Ollama via LLMHelper with extended timeout
            response = self.llm._call_ollama(prompt, temperature=0.1, max_tokens=8192, timeout=timeout)
            
            if not response:
                print(f"[WARN]   ❌ {func_name}: No response from LLM (timed out after {timeout}s or error)")
                return None
            
            fixed_code = response.strip()
            
            # Remove markdown code fences more aggressively
            lines = fixed_code.split('\n')
            cleaned_lines = []
            in_code_block = False
            skip_explanation = False
            
            for line in lines:
                # Detect code fence markers
                if line.strip().startswith('```'):
                    in_code_block = not in_code_block
                    continue
                
                # Skip explanation text (starts with -, *, or numbered lists outside code)
                if not in_code_block and (line.strip().startswith('-') or 
                                         line.strip().startswith('*') or
                                         re.match(r'^\d+\.', line.strip())):
                    skip_explanation = True
                    continue
                
                # Skip common explanation phrases
                if any(phrase in line.lower() for phrase in [
                    'explanation:', 'note:', 'changes made:', 'output:',
                    'i have', 'i\'ve', 'the fix', 'this fix'
                ]):
                    skip_explanation = True
                    continue
                
                # Reset skip if we see code-like content
                if line.strip() and ('{' in line or '}' in line or ';' in line or '#include' in line):
                    skip_explanation = False
                
                # Only add actual code lines
                if not skip_explanation or in_code_block:
                    cleaned_lines.append(line)
            
            fixed_code = '\n'.join(cleaned_lines).strip()
            
            # Reject fixes that add new struct/typedef definitions for types that
            # already exist — that's the main cause of error count exploding.
            for known_type in all_known_types:
                if re.search(r'\btypedef\b.*\b' + re.escape(known_type) + r'\b', fixed_code):
                    print(f"[WARN]   ❌ {func_name}: LLM tried to redefine '{known_type}' which already exists, skipping")
                    return None
            
            # Final validation - ensure it starts with a function signature
            if not (fixed_code.startswith('/*') or
                   fixed_code.startswith('//') or
                   func_name in fixed_code[:200]):
                print(f"[WARN]   ❌ {func_name}: Fixed code doesn't look valid (missing function)")
                return None
            
            # Validate the fix
            if len(fixed_code) < len(code) * 0.5:
                print(f"[WARN]   ❌ {func_name}: Fixed code too short, skipping")
                return None
            
            # Brace balance check — unbalanced braces mean the LLM truncated or
            # duplicated part of the function, which causes cascading errors.
            open_braces = fixed_code.count('{')
            close_braces = fixed_code.count('}')
            if open_braces != close_braces:
                print(f"[WARN]   ❌ {func_name}: LLM output has unbalanced braces "
                      f"({open_braces} open vs {close_braces} close), skipping")
                return None

            # Also reject if line count changed dramatically (truncated/appended output)
            orig_lines = code.count('\n')
            fixed_lines = fixed_code.count('\n')
            if fixed_lines < orig_lines * 0.7 or fixed_lines > orig_lines * 1.5:
                print(f"[WARN]   ❌ {func_name}: LLM output line count changed drastically "
                      f"({orig_lines} → {fixed_lines}), skipping")
                return None

            # Check if meaningful changes were made
            if fixed_code.strip() == code.strip():
                print(f"[WARN]   ❌ {func_name}: No changes made, skipping")
                return None
            
            has_llm_comment = '/* LLM:' in fixed_code
            if not has_llm_comment:
                print(f"[WARN]   ⚠️  {func_name}: No LLM markers found, but applying anyway")
            
            print(f"[INFO]   ✅ {func_name}: Successfully fixed")
            return fixed_code
        
        except Exception as e:
            print(f"[ERROR]  ❌ {func_name}: LLM fix failed: {e}")
            return None
    
    def _generate_fix_instructions(self, dominant_type: str, all_error_types: Dict,
                                    struct_maps: Optional[Dict] = None) -> str:
        """Generate specialized fixing instructions based on error types."""
        
        instructions = []
        
        if dominant_type == 'void_pointer_dereference' or 'void_pointer_dereference' in all_error_types:
            instructions.append("""
1. **Void Pointer Fixes:**
   - Find all variables declared as `void*` that are used with struct field access (`->`)
   - Replace `void*` with the appropriate struct pointer type (use inferred type if provided)
   - For `extern void*` declarations, replace with `extern StructType*`
""")
        
        if 'missing_struct_field' in all_error_types:
            # Build a lookup hint from the struct maps if available
            field_hint = ""
            if struct_maps:
                lines = []
                for sname, field_map in sorted(struct_maps.items()):
                    if field_map:
                        entries = ', '.join(
                            f'0x{off:02X}→{fname}({ftype})'
                            for off, (fname, ftype) in sorted(field_map.items())
                        )
                        lines.append(f"    {sname}: {entries}")
                if lines:
                    field_hint = (
                        "\n   Use these offset maps to find the CORRECT field name:\n"
                        + '\n'.join(lines) + '\n'
                    )
            instructions.append(f"""
2. **Missing Struct Field Fixes:**
   The struct is defined in an included header — you CANNOT add new fields to it.
   Fix only the code INSIDE this function using one of these strategies:

   **Strategy A (preferred) — rename to the correct field:**{field_hint}
   - Look up which field is at that byte offset in the struct reference above.
   - Replace `ptr->unkX` with `ptr->correct_field_name` from the table.
   - Example: if unk4 is at offset 0x04 and the table shows `0x04→D_8016297C(s16)`,
     change `ptr->unk4` to `ptr->D_8016297C`.

   **Strategy B — byte-offset cast (when no field exists at that offset):**
   - For a READ: `var = *(s16*)((u8*)ptr + offset);`
   - For a WRITE: `*(s16*)((u8*)ptr + offset) = value;`
   - CRITICAL: replace the ENTIRE `ptr->unkX` — do NOT leave `ptr->unkX` on the LHS
     of an assignment.  Wrong: `ptr->unk4 = ...`.  Right: `*(s16*)((u8*)ptr+4) = ...`.
   - Pick the cast type (s16/u16/s32/u32/u8) by the field's size in the struct.
""")
        
        if 'undefined_identifier' in all_error_types:
            instructions.append("""
3. **Undefined Identifier Fixes:**
   - Find undefined variables (often global vars like `D_8004A698`, or locals like `sp1C`)
   - For D_XXXXXXXX globals: add extern declaration at top of file
   - For spXX stack variables: declare them properly at function start
   - Infer the type from usage context (how it's assigned/used)
""")
        
        if 'type_mismatch' in all_error_types:
            instructions.append("""
4. **Type Mismatch Fixes:**
   - Fix incompatible type assignments (e.g., assigning struct to u8)
   - Use proper pointer dereferencing: if assigning struct to scalar, use `&variable` or cast
   - Ensure pointer arithmetic uses correct types
   - Add necessary type casts where appropriate (e.g., `(u8*)` for byte access)
""")
        
        if 'pointer_cast' in all_error_types:
            instructions.append("""
5. **Pointer Cast Fixes:**
   - "assignment makes integer from pointer without cast":
     Change `var = ptr;` to `var = (u32)ptr;`
   - "assignment makes pointer from integer without cast":
     Change `ptr = int_val;` to `ptr = (TYPE*)(u32)int_val;` (use the correct pointer type)
   - "cast to pointer from integer of different size":
     Change `(TYPE*)val` to `(TYPE*)(u32)val` to avoid size mismatch
   - "passing arg N ... makes pointer from integer":
     Wrap the argument: `(TYPE*)(u32)arg`
""")
        
        if 'invalid_operation' in all_error_types:
            instructions.append("""
6. **Invalid Operation Fixes:**
   - Fix "invalid type argument of unary *" - trying to dereference non-pointer
   - Change variable to pointer type, or remove dereference operator
   - Check if variable should be a pointer based on context
""")
        
        if 'syntax_error' in all_error_types or 'parse_error' in all_error_types:
            instructions.append("""
7. **Parse/Syntax Error Fixes:**
   - Fix "parse error before *" - usually missing semicolon or typedef
   - Check for missing struct/typedef declarations
   - Ensure proper syntax for pointer declarations
""")
        
        if 'redefinition' in all_error_types:
            instructions.append("""
8. **Redefinition Fixes:**
   - Remove duplicate struct/typedef declarations
   - If same struct defined twice, keep only one definition
   - Ensure typedefs are not redeclared
""")
        
        if 'duplicate_label' in all_error_types:
            instructions.append("""
9. **Duplicate Label Fixes:**
   - Rename duplicate labels to unique names (e.g., block_25 -> block_25_alt)
   - Ensure goto targets use the correct renamed label
""")
        
        instructions.append("""
**General Rules (PSX / FF7 decompilation standards):**
- Preserve all existing logic, calculations, and control flow exactly
- Keep all variable names unchanged (except renaming duplicate labels)
- Only fix type declarations and type-related errors
- Do NOT refactor or optimize code
- Ensure PS1/PSX SDK compatibility (32-bit, 4-byte aligned structs)
- Use only PSX SDK types: s8/u8/s16/u16/s32/u32 — never int/short/char/long
- Type determines the load instruction: s8→lb, u8→lbu, s16→lh, u16→lhu — change the type if the wrong instruction is generated
- `goto loop_XX` patterns MUST be rewritten as `while` or `do-while` loops
- `goto block_XX` inside switch statements: inline the branch target, do not leave dangling gotos
- Array access must use `arr[i]`, never `*(arr + i * sizeof(...))`
- Struct field access must use `->` or `.`, never pointer arithmetic offsets
- Add `/* LLM: Fixed [issue] */` comment on lines you modify within the function body
""")
        
        return '\n'.join(instructions)
    
    def extract_function_code(self, file_path: str, func_name: str) -> Optional[Tuple[str, int, int]]:
        """
        Extract function code from file.
        Returns (code, start_line, end_line) or None if not found.
        """
        try:
            with open(file_path, 'r') as f:
                lines = f.readlines()
            
            # Find function start - look for function name followed by ( on same or next line
            start_line = None
            for i, line in enumerate(lines):
                # Match function definition: func_name followed by (
                # Handle cases where function spans multiple lines
                if func_name + '(' in line.replace(' ', ''):
                    start_line = i
                    break
                # Also try with space
                if func_name + ' (' in line or func_name + '  (' in line:
                    start_line = i
                    break
            
            if start_line is None:
                return None
            
            # Find the opening brace
            brace_line = None
            for i in range(start_line, min(start_line + 10, len(lines))):  # Look within 10 lines
                if '{' in lines[i]:
                    brace_line = i
                    break
            
            if brace_line is None:
                return None
            
            # Find function end (matching braces)
            brace_count = 0
            end_line = None
            for i in range(brace_line, len(lines)):
                line = lines[i]
                brace_count += line.count('{')
                brace_count -= line.count('}')
                if brace_count == 0:
                    end_line = i
                    break
            
            if end_line is None:
                return None
            
            code = ''.join(lines[start_line:end_line+1])
            return (code, start_line, end_line)
        
        except Exception as e:
            print(f"[ERROR] Could not extract {func_name} from {file_path}: {e}")
            return None
    
    def apply_fix_to_file(self, file_path: str, func_name: str, fixed_code: str, 
                          start_line: int, end_line: int) -> bool:
        """
        Apply fixed code to file, replacing the original function.
        Returns True if successful.
        """
        try:
            with open(file_path, 'r') as f:
                lines = f.readlines()
            
            # Don't add header comment - it was already added by LLM or not needed
            # Just ensure the code ends with newline
            if not fixed_code.endswith('\n'):
                fixed_code = fixed_code + '\n'
            
            # Replace function - build as a single string to avoid mixing
            # \n-terminated strings (from readlines) with non-terminated strings
            # (from split('\n')), which would cause writelines to merge lines.
            new_content = ''.join(lines[:start_line]) + fixed_code + ''.join(lines[end_line+1:])
            
            # Write to temp file first
            temp_path = file_path + '.tmp'
            with open(temp_path, 'w') as f:
                f.write(new_content)
            
            # Move temp to actual file
            import shutil
            shutil.move(temp_path, file_path)
            
            return True
        
        except Exception as e:
            print(f"[ERROR] Could not apply fix to {file_path}: {e}")
            return False
    
    def validate_fix(self, file_path: str, func_name: str,
                    original_error_count: int,
                    func_error_count_before: int = 0,
                    func_start_line: int = 0,
                    func_end_line: int = 0) -> bool:
        """
        Validate a fix by recompiling and checking if errors decreased.
        When function line bounds are provided, uses function-level error
        counting as the primary signal — this avoids false-positive "inconclusive"
        results where the LLM moves an error without actually fixing it.
        Returns True if fix is valid.
        """
        print(f"[INFO]   Validating fix for {func_name}...")
        
        # Force recompile
        Path(file_path).touch()
        
        # Get new error count
        new_errors = self.get_compilation_errors(file_path)
        new_error_count = len(new_errors)

        # --- Function-level check (preferred when bounds are known) ---
        if func_start_line > 0 and func_end_line > 0 and func_error_count_before > 0:
            # Re-discover the function's new position (line numbers may have shifted)
            discovered = self.discover_functions_in_file(file_path)
            new_start, new_end = func_start_line, func_end_line  # fallback
            for fname, s, e in discovered:
                if fname == func_name:
                    new_start, new_end = s, e
                    break

            func_errors_after = sum(
                1 for e in new_errors if new_start <= e['line'] <= new_end + 5
            )

            if func_errors_after > func_error_count_before:
                print(f"[WARN]   ❌ Validation failed: function errors increased "
                      f"{func_error_count_before} → {func_errors_after}")
                return False
            elif func_errors_after == func_error_count_before:
                # No change in the function itself — only allow if global count decreased
                if new_error_count < original_error_count:
                    print(f"[INFO]   ✅ Validation passed: global errors decreased "
                          f"{original_error_count} → {new_error_count}")
                    return True
                print(f"[WARN]   ⚠️  Validation inconclusive: function errors unchanged "
                      f"({func_errors_after}), global {original_error_count} → {new_error_count}")
                # Allow only if at least the file didn't get worse
                return new_error_count <= original_error_count
            else:
                print(f"[INFO]   ✅ Validation passed: function errors decreased "
                      f"{func_error_count_before} → {func_errors_after}")
                return True

        # --- Fallback: file-level check only ---
        if new_error_count > original_error_count:
            print(f"[WARN]   ❌ Validation failed: errors increased from {original_error_count} to {new_error_count}")
            return False
        elif new_error_count == original_error_count:
            print(f"[WARN]   ⚠️  Validation inconclusive: error count unchanged ({new_error_count})")
            return True
        else:
            print(f"[INFO]   ✅ Validation passed: errors decreased from {original_error_count} to {new_error_count}")
            return True
    
    def backup_file(self, file_path: str) -> str:
        """Create a backup of the file. Returns backup path."""
        import shutil
        backup_path = file_path + '.backup'
        shutil.copy2(file_path, backup_path)
        return backup_path
    
    def restore_backup(self, file_path: str, backup_path: str):
        """Restore file from backup."""
        import shutil
        shutil.move(backup_path, file_path)
        print(f"[INFO]   Restored from backup")
    
    def fix_file(self, file_path: str, max_functions: int = 20, max_passes: int = 3,
                 target_function: str = None) -> int:
        """
        Fix type errors in a file, looping until no more progress can be made
        or max_passes is reached.
        If target_function is given, only that function is fixed (all others are skipped).
        Returns total number of functions fixed across all passes.
        """
        print(f"[INFO] Processing {file_path}...")
        
        # Create a single backup before any passes
        backup_path = self.backup_file(file_path)
        print(f"[INFO]   Created backup at {backup_path}")

        # Reset per-file failure tracker
        self._validation_failures = {}
        
        total_fixed = 0
        total_failed = 0
        
        for pass_num in range(1, max_passes + 1):
            if pass_num > 1:
                print(f"[INFO] --- Pass {pass_num}/{max_passes} ---")
            
            # Get compilation errors for this pass
            print(f"[INFO]   Compiling to detect errors...")
            compiler_errors = self.get_compilation_errors(file_path)
            print(f"[INFO]   Found {len(compiler_errors)} compilation error(s)")
            
            if not compiler_errors:
                print(f"[INFO]   No compilation errors found, done!")
                break
            
            # Group errors by type
            error_types = {}
            for err in compiler_errors:
                err_type = err['type']
                error_types[err_type] = error_types.get(err_type, 0) + 1
            
            print(f"[INFO]   Error breakdown: {', '.join(f'{k}: {v}' for k, v in error_types.items())}")
            
            # Discover all actual decompiled functions in the file
            print(f"[INFO]   Discovering decompiled functions...")
            discovered_functions = self.discover_functions_in_file(file_path)
            print(f"[INFO]   Found {len(discovered_functions)} decompiled function(s) in file")
            
            if not discovered_functions:
                print(f"[INFO]   No decompiled functions found")
                break
            
            # Debug: show some discovered functions and error lines
            print(f"[DEBUG] Sample functions: {discovered_functions[:3]}")
            print(f"[DEBUG] Sample error lines: {[e['line'] for e in compiler_errors[:5]]}")

            # ── Deterministic pre-pass ──────────────────────────────────────
            # Handle pointer_cast and missing_struct_field errors mechanically
            # before spending LLM time on them.
            struct_maps = self._collect_struct_field_maps(file_path)
            det_changed = self.apply_deterministic_fixes(file_path, compiler_errors, struct_maps)
            if det_changed:
                print(f"[INFO]   🔧 Deterministic pre-pass: fixed {det_changed} line(s)")
                # Re-read errors after deterministic edits
                compiler_errors = self.get_compilation_errors(file_path)
                print(f"[INFO]   Errors after deterministic pass: {len(compiler_errors)}")
                if not compiler_errors:
                    print(f"[INFO]   All errors resolved by deterministic pass!")
                    total_fixed += det_changed
                    break
            # ────────────────────────────────────────────────────────────────
            
            # Map functions to their errors
            functions_with_errors = []
            for func_name, start_line, end_line in discovered_functions:
                func_errors = [e for e in compiler_errors 
                              if start_line <= e['line'] <= end_line]
                
                if func_errors:
                    # Skip functions that have already had N validation failures
                    prior_failures = self._validation_failures.get(func_name, 0)
                    if prior_failures >= 2:
                        print(f"[DEBUG] {func_name}: skipping (failed validation {prior_failures} times already)")
                        continue

                    result = self.extract_function_code(file_path, func_name)
                    if not result:
                        print(f"[WARN]   ⚠️  {func_name}: Could not extract function code")
                        continue
                    
                    code, extracted_start, extracted_end = result
                    
                    functions_with_errors.append({
                        'func_name': func_name,
                        'code': code,
                        'start_line': extracted_start,
                        'end_line': extracted_end,
                        'errors': func_errors,
                        'func_error_count': len(func_errors),
                    })
                    print(f"[DEBUG] {func_name}: {len(func_errors)} error(s), lines {extracted_start}-{extracted_end}")
                    
                    # Limit to max_functions *with errors*, not overall discovered
                    if len(functions_with_errors) >= max_functions:
                        break
            
            # If a specific function was requested, restrict to it
            if target_function:
                functions_with_errors = [f for f in functions_with_errors
                                         if f['func_name'] == target_function]

            if not functions_with_errors:
                print(f"[INFO]   No decompiled functions have errors")
                break
            
            print(f"[INFO]   {len(functions_with_errors)} function(s) with errors to fix")
            
            original_error_count = len(compiler_errors)
            pass_fixed = 0
            pass_failed = 0
            
            for func_info in functions_with_errors:
                func_name = func_info['func_name']
                code = func_info['code']
                start_line = func_info['start_line']
                end_line = func_info['end_line']
                func_error_count_before = func_info['func_error_count']
                
                # Create a function-specific backup
                func_backup = None
                try:
                    import shutil
                    func_backup = file_path + f'.func_{func_name}.bak'
                    shutil.copy2(file_path, func_backup)
                except:
                    pass
                
                # Fix with LLM (pass compiler errors)
                fixed_code = self.fix_function_with_llm(
                    func_name, file_path, code, start_line, compiler_errors
                )
                if not fixed_code:
                    print(f"[INFO]   ℹ️  {func_name}: No fixes applied or fixing skipped")
                    if func_backup:
                        Path(func_backup).unlink(missing_ok=True)
                    continue
                
                # Apply fix
                if not self.apply_fix_to_file(file_path, func_name, fixed_code, start_line, end_line):
                    print(f"[ERROR]  ❌ {func_name}: Failed to apply fix to file")
                    if func_backup:
                        Path(func_backup).unlink(missing_ok=True)
                    pass_failed += 1
                    continue
                
                # Validate using function-level error counts where possible
                if not self.validate_fix(file_path, func_name, original_error_count,
                                         func_error_count_before, start_line, end_line):
                    print(f"[ERROR]  ❌ {func_name}: Fix validation failed, rolling back")
                    self._validation_failures[func_name] = self._validation_failures.get(func_name, 0) + 1
                    if func_backup:
                        import shutil
                        shutil.move(func_backup, file_path)
                        print(f"[INFO]   Rolled back changes for {func_name}")
                    pass_failed += 1
                    continue
                
                # Success — reset failure counter for this function
                self._validation_failures.pop(func_name, None)
                # Success - clean up function backup
                if func_backup:
                    Path(func_backup).unlink(missing_ok=True)
                
                # Try to update database if function exists
                try:
                    self.db.update_status(func_name, 'decompiled_needs_refine', 
                                         notes='LLM fixed type errors, needs re-verification')
                except:
                    pass  # Function might not be in database
                
                pass_fixed += 1
                # Update original error count for next validation
                original_error_count = len(self.get_compilation_errors(file_path))
            
            total_fixed += pass_fixed
            total_failed += pass_failed
            
            if pass_fixed == 0:
                print(f"[INFO]   No progress in pass {pass_num}, stopping")
                break
            
            print(f"[INFO]   Pass {pass_num} fixed {pass_fixed} function(s), {pass_failed} failed")
        
        # Clean up master backup
        if total_failed == 0:
            Path(backup_path).unlink(missing_ok=True)
            print(f"[INFO]   All fixes successful, removed backup")
        else:
            print(f"[INFO]   Backup kept at {backup_path} due to {total_failed} failed fix(es)")
        
        return total_fixed


def main():
    """Main entry point."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Fix type errors in existing decompiled functions')
    parser.add_argument('--file', type=str, help='Specific file to fix (relative to workspace)')
    parser.add_argument('--all', action='store_true', help='Fix all files with errors')
    parser.add_argument('--max-functions', type=int, default=20, 
                       help='Maximum functions to fix per file (default: 20)')
    parser.add_argument('--max-passes', type=int, default=3,
                       help='Maximum LLM fix passes per file (default: 3)')
    parser.add_argument('--model', type=str, default='qwen2.5-coder:7b',
                       help='Ollama model to use (default: qwen2.5-coder:7b)')
    parser.add_argument('--verbose', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    fixer = TypeErrorFixer(model=args.model, verbose=args.verbose)
    
    if args.file:
        # Fix specific file
        file_path = fixer.workspace_root / args.file
        if not file_path.exists():
            print(f"[ERROR] File not found: {file_path}")
            sys.exit(1)
        
        fixed = fixer.fix_file(str(file_path), max_functions=args.max_functions, max_passes=args.max_passes)
        print(f"\n✅ Fixed {fixed} function(s) in {args.file}")
    
    elif args.all:
        print("[ERROR] --all mode not yet implemented")
        print("[INFO] Use --file to fix a specific file, e.g.:")
        print("       python3 fix_existing_errors.py --file src/main/psxsdk.c")
        sys.exit(1)
    
    else:
        # Default: show help
        parser.print_help()
        print("\nExample usage:")
        print("  python3 fix_existing_errors.py --file src/main/psxsdk.c")


if __name__ == '__main__':
    main()
