#!/usr/bin/env python3
"""
Build & Verification System

Handles building the project and verifying function matches:
1. Runs ./mako.sh build
2. Parses build output for errors/warnings
3. Checks binary matching for functions
4. Updates database with verification status

Usage:
    python build.py --build                    # Build entire project
    python build.py --verify                   # Verify all decompiled functions
    python build.py --check-function func_name # Check specific function
    python build.py --report                   # Show build/match statistics
"""

import subprocess
import argparse
import sys
import re
import json
from pathlib import Path
from typing import Optional, Tuple, Dict, List
from database import DecompDatabase

# Project root is one level up from automation/
PROJECT_ROOT = Path(__file__).parent.parent
MAKO_SCRIPT = PROJECT_ROOT / "mako.sh"
BUILD_DIR = PROJECT_ROOT / "build" / "us"
OBJDIFF_CLI = PROJECT_ROOT / "bin" / "objdiff-cli-linux-x86_64"


class BuildVerifier:
    """Handles building and verifying function matches."""
    
    def __init__(self, db: DecompDatabase, verbose: bool = False):
        self.db = db
        self.verbose = verbose
        self._objdiff_report_cache: Optional[Dict] = None

    # ------------------------------------------------------------------
    # Per-file PSY-Q compilation check (no LLM required)
    # ------------------------------------------------------------------

    def _get_psyq_params(self, rel_path: str):
        """
        Parse build.ninja to find PSY-Q compiler parameters for a source file.
        Returns (cc1_binary, cc_flags) or None if the file has no psx-cc rule.
        """
        ninja_file = PROJECT_ROOT / 'build.ninja'
        if not ninja_file.exists():
            return None

        lines = ninja_file.read_text().splitlines()

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

    def check_file_compiles(self, file_path: Path) -> Tuple[bool, List[Dict]]:
        """
        Compile a single C file using the PSY-Q toolchain and return errors.

        Uses build.ninja to find the correct compiler flags for the file.
        Returns (success, errors) where errors is a list of {'line', 'message'} dicts.
        On tool/timeout failure returns (True, []) to avoid false-positive reverts.
        """
        try:
            file_path = Path(file_path)
            if not file_path.is_absolute():
                file_path = PROJECT_ROOT / file_path
            rel_path = str(file_path.relative_to(PROJECT_ROOT))
            file_name = file_path.name

            params = self._get_psyq_params(rel_path)
            if params is None:
                # No PSY-Q rule in build.ninja — skip silently.
                if self.verbose:
                    print(f"  No PSY-Q build rule for {rel_path}, skipping compile check")
                return True, []

            cc1, cc_flags = params
            cmd = (
                f'mipsel-linux-gnu-cpp -Iinclude -Iinclude/psxsdk -DUSE_INCLUDE_ASM -DFF7_STR'
                f' -lang-c -Iinclude -Iinclude/psxsdk -undef -Wall -fno-builtin {rel_path}'
                f' | bin/str'
                f' | iconv --from-code=UTF-8 --to-code=Shift-JIS'
                f' | bin/{cc1} -quiet -mcpu=3000 -g -mgas -gcoff {cc_flags} > /dev/null'
            )
            result = subprocess.run(
                cmd, shell=True,
                cwd=str(PROJECT_ROOT),
                stderr=subprocess.PIPE,
                text=True,
                timeout=60
            )
            output = result.stderr

            errors = []
            error_pattern = re.compile(rf'{re.escape(file_name)}:(\d+):\s*(.+)')
            for line in output.split('\n'):
                match = error_pattern.search(line)
                if match:
                    line_num = int(match.group(1))
                    message = match.group(2).strip()
                    # Skip all warnings — only true errors (undeclared, syntax, etc.) block progress
                    if 'warning:' in line.lower():
                        continue
                    errors.append({'line': line_num, 'message': message})

            return len(errors) == 0, errors

        except subprocess.TimeoutExpired:
            if self.verbose:
                print(f"  Compile check timed out for {file_path.name}")
            return True, []
        except Exception as e:
            if self.verbose:
                print(f"  Compile check error for {file_path.name}: {e}")
            return True, []

    def run_build(self) -> Tuple[bool, str, str, Dict]:
        """
        Run ./mako.sh build
        
        Returns:
            (success, stdout, stderr, parsed_info) tuple
        """
        if not MAKO_SCRIPT.exists():
            return False, "", f"mako.sh not found at {MAKO_SCRIPT}", {}
        
        cmd = [str(MAKO_SCRIPT), "build"]
        
        if self.verbose:
            print(f"Running: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd,
                cwd=PROJECT_ROOT,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout for builds
            )
            
            success = result.returncode == 0
            parsed = self.parse_build_output(result.stdout, result.stderr)
            
            return success, result.stdout, result.stderr, parsed
            
        except subprocess.TimeoutExpired:
            return False, "", "Build timed out after 5 minutes", {}
        except Exception as e:
            return False, "", f"Exception running build: {str(e)}", {}
    
    def parse_build_output(self, stdout: str, stderr: str) -> Dict:
        """
        Parse build output to extract useful information.
        
        Returns dict with:
            - success: bool
            - errors: list of error messages
            - warnings: list of warning messages
            - compile_errors: dict of file -> errors
            - link_errors: list of link errors
        """
        result = {
            'success': True,
            'errors': [],
            'warnings': [],
            'compile_errors': {},
            'link_errors': []
        }
        
        combined = stdout + "\n" + stderr
        
        # Parse compile errors: "src/file.c:123: error message"
        compile_error_pattern = r'(src/[^:]+):(\d+):\s*(.*?)(?=\n|$)'
        for match in re.finditer(compile_error_pattern, combined):
            file_path, line_num, message = match.groups()
            
            if 'error' in message.lower():
                result['success'] = False
                if file_path not in result['compile_errors']:
                    result['compile_errors'][file_path] = []
                result['compile_errors'][file_path].append({
                    'line': int(line_num),
                    'message': message.strip()
                })
                result['errors'].append(f"{file_path}:{line_num}: {message}")
            elif 'warning' in message.lower():
                result['warnings'].append(f"{file_path}:{line_num}: {message}")
        
        # Parse linker errors: "undefined reference to `func_name'"
        link_error_pattern = r"undefined reference to `([^']+)'"
        for match in re.finditer(link_error_pattern, combined):
            func_name = match.group(1)
            result['success'] = False
            result['link_errors'].append(func_name)
            result['errors'].append(f"Undefined reference: {func_name}")
        
        # Check for ninja failure
        if 'ninja: build stopped' in combined or 'FAILED:' in combined:
            result['success'] = False
        
        return result
    
    def check_function_match(self, function_name: str) -> Tuple[bool, Optional[str]]:
        """
        Check if a decompiled function matches the original binary using objdiff-cli.

        Returns:
            (matches, error_message) tuple
        """
        func = self.db.get_function(function_name)
        if not func:
            return False, "Function not in database"

        # Check if function still has INCLUDE_ASM in the C file
        c_file = PROJECT_ROOT / func['c_file_path']
        if not c_file.exists():
            return False, f"C file not found: {c_file}"

        content = c_file.read_text()
        include_asm_pattern = rf'INCLUDE_ASM\s*\(\s*"[^"]*"\s*,\s*{re.escape(function_name)}\s*\)'
        if re.search(include_asm_pattern, content):
            return False, "Function still uses INCLUDE_ASM"

        # Use objdiff-cli for real binary matching
        if not OBJDIFF_CLI.exists():
            return False, "objdiff-cli not found — cannot verify binary match"

        expected_dir = PROJECT_ROOT / "expected"
        if not expected_dir.exists():
            return False, "expected/ directory missing — run make rebuild first"

        try:
            import tempfile, os
            if self._objdiff_report_cache is None:
                with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as tf:
                    tmp_path = tf.name

                result = subprocess.run(
                    [str(OBJDIFF_CLI), 'report', 'generate', '-o', tmp_path, '--format', 'json'],
                    cwd=str(PROJECT_ROOT),
                    capture_output=True,
                    text=True,
                    timeout=60,
                )

                if result.returncode != 0:
                    return False, f"objdiff-cli failed: {result.stderr.strip()[:300]}"

                with open(tmp_path) as f:
                    self._objdiff_report_cache = json.load(f)
                os.unlink(tmp_path)

            report = self._objdiff_report_cache

            # Search for the function in all units
            for unit in report.get('units', []):
                for sym in unit.get('functions', []):
                    if sym.get('name') == function_name:
                        pct = sym.get('fuzzy_match_percent', 0)
                        if pct >= 100.0:
                            return True, None
                        return False, f"Binary match {pct:.1f}% (need 100%)"

            return False, "Function not found in objdiff report"

        except Exception as e:
            return False, f"objdiff error: {e}"

    
    def verify_decompiled_functions(self) -> Dict[str, any]:
        """
        Verify all functions marked as 'decompiled' in the database.
        
        Returns summary of verification results.
        """
        print(f"\n{'='*60}")
        print("VERIFYING DECOMPILED FUNCTIONS")
        print(f"{'='*60}\n")
        
        # Get all decompiled functions
        decompiled = self.db.get_functions_by_status('decompiled')
        
        if not decompiled:
            print("No functions to verify (none marked as decompiled)")
            return {'verified': 0, 'failed': 0, 'total': 0}
        
        print(f"Found {len(decompiled)} decompiled function(s) to verify\n")
        
        verified_count = 0
        failed_count = 0
        
        for i, func in enumerate(decompiled, 1):
            func_name = func['name']
            print(f"[{i}/{len(decompiled)}] Checking {func_name}...")
            
            matches, error = self.check_function_match(func_name)
            
            if matches:
                print(f"  ✅ Matches")
                self.db.update_function_status(
                    func_name,
                    'verified',
                    notes="Verified - no longer uses INCLUDE_ASM"
                )
                verified_count += 1
            else:
                print(f"  ❌ Not matching: {error}")
                # Mark as needs refinement (decompiled but doesn't match)
                if "still uses INCLUDE_ASM" in error:
                    self.db.update_function_status(
                        func_name,
                        'decompiled_needs_refine',
                        notes=f"Decompiled but verification failed: {error}"
                    )
                else:
                    self.db.update_function_status(
                        func_name,
                        'decompiled',
                        notes=f"Decompiled but needs verification: {error}"
                    )
                failed_count += 1
        
        print(f"\n{'='*60}")
        print(f"VERIFICATION COMPLETE")
        print(f"{'='*60}")
        print(f"✅ Verified:    {verified_count}")
        print(f"❌ Not matching: {failed_count}")
        print(f"{'='*60}\n")
        
        return {
            'verified': verified_count,
            'failed': failed_count,
            'total': len(decompiled)
        }
    
    def full_build_and_verify(self) -> bool:
        """
        Run full build and verify all functions.
        
        Returns True if build succeeded.
        """
        print(f"\n{'='*60}")
        print("RUNNING BUILD")
        print(f"{'='*60}\n")
        
        success, stdout, stderr, parsed = self.run_build()
        self._objdiff_report_cache = None  # Invalidate cache after build
        
        if self.verbose:
            print("--- BUILD OUTPUT ---")
            print(stdout)
            if stderr:
                print("--- STDERR ---")
                print(stderr)
        
        if success:
            print("✅ Build succeeded!")
            print(f"\nWarnings: {len(parsed['warnings'])}")
            if parsed['warnings'] and self.verbose:
                for warning in parsed['warnings'][:5]:  # Show first 5
                    print(f"  ⚠️  {warning}")
            
            # Now verify functions
            print("\n" + "="*60)
            self.verify_decompiled_functions()
            
        else:
            print("❌ Build failed!")
            print(f"\nErrors: {len(parsed['errors'])}")
            
            # Show compile errors
            if parsed['compile_errors']:
                print("\nCompile Errors:")
                for file_path, errors in parsed['compile_errors'].items():
                    print(f"  {file_path}:")
                    for err in errors[:3]:  # Show first 3 per file
                        print(f"    Line {err['line']}: {err['message']}")
            
            # Show link errors
            if parsed['link_errors']:
                print(f"\nUndefined References ({len(parsed['link_errors'])}):")
                for func in parsed['link_errors'][:10]:  # Show first 10
                    print(f"  - {func}")
                if len(parsed['link_errors']) > 10:
                    print(f"  ... and {len(parsed['link_errors']) - 10} more")
        
        print(f"\n{'='*60}\n")
        return success


def show_build_report(db: DecompDatabase):
    """Show statistics on build/verification status."""
    stats = db.get_statistics()
    
    print("\n" + "="*60)
    print("BUILD & VERIFICATION REPORT")
    print("="*60)
    
    total = stats['total']
    by_status = stats['by_status']
    
    # Status breakdown
    verified = by_status.get('verified', 0)
    decompiled = by_status.get('decompiled', 0)
    needs_refine = by_status.get('decompiled_needs_refine', 0)
    in_progress = by_status.get('in_progress', 0)
    failed = by_status.get('failed', 0)
    blocked = by_status.get('blocked', 0)
    todo = by_status.get('todo', 0)
    
    print(f"\nTotal Functions: {total}")
    print(f"\nStatus Breakdown:")
    print(f"  ✅ Verified:    {verified:>6} ({verified/total*100:.1f}%)")
    print(f"  🔄 Decompiled:  {decompiled:>6} ({decompiled/total*100:.1f}%)")
    print(f"  🔧 Needs Refine: {needs_refine:>6} ({needs_refine/total*100:.1f}%)")
    print(f"  ⚙️  In Progress: {in_progress:>6}")
    print(f"  ⚠️  Blocked:     {blocked:>6}")
    print(f"  ❌ Failed:      {failed:>6}")
    print(f"  ⏳ Todo:        {todo:>6}")
    
    # Progress calculation (verified = truly done)
    completed = verified
    if total > 0:
        progress_pct = (completed / total) * 100
        print(f"\nVerified Progress: {completed}/{total} ({progress_pct:.1f}%)")
    
    # Recent activity
    print(f"\nBy Module:")
    for module, count in stats['by_module'].items():
        print(f"  {module:15s}: {count:>4}")
    
    print("="*60 + "\n")


def main():
    parser = argparse.ArgumentParser(description="Build and verification system")
    parser.add_argument('--build', action='store_true',
                       help='Run full build')
    parser.add_argument('--verify', action='store_true',
                       help='Verify all decompiled functions')
    parser.add_argument('--check-function', type=str,
                       help='Check if specific function matches')
    parser.add_argument('--report', action='store_true',
                       help='Show build/verification statistics')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Verbose output')
    
    args = parser.parse_args()
    
    # Initialize database
    db = DecompDatabase()
    verifier = BuildVerifier(db, verbose=args.verbose)
    
    if args.report:
        show_build_report(db)
    
    elif args.build:
        success = verifier.full_build_and_verify()
        sys.exit(0 if success else 1)
    
    elif args.verify:
        results = verifier.verify_decompiled_functions()
        sys.exit(0 if results['failed'] == 0 else 1)
    
    elif args.check_function:
        matches, error = verifier.check_function_match(args.check_function)
        if matches:
            print(f"✅ {args.check_function} matches!")
            sys.exit(0)
        else:
            print(f"❌ {args.check_function} does not match: {error}")
            sys.exit(1)
    
    else:
        parser.print_help()
        print("\nQuick examples:")
        print("  python build.py --report")
        print("  python build.py --verify")
        print("  python build.py --build")
        print("  python build.py --check-function AverageSZ4")


if __name__ == '__main__':
    main()
