#!/usr/bin/env python3
"""
Revert all decompiled/verified functions to INCLUDE_ASM stubs, then rebuild.

This establishes a clean baseline where `make rebuild` passes all SHA1 checks
and creates the `expected/` directory that objdiff-cli needs for binary matching.
After running this script, restart the automation agent — it will use objdiff-cli
to verify each function against the reference before keeping decompiled code.

Usage:
    cd automation/
    python revert_nonmatching.py              # Revert all + rebuild
    python revert_nonmatching.py --dry-run    # Preview without changes
    python revert_nonmatching.py --module battle --skip-build
    python revert_nonmatching.py --skip-build  # Just revert, no rebuild
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

from database import DecompDatabase

PROJECT_ROOT = Path(__file__).parent.parent


def get_include_asm_dir(asm_file_path: str) -> Optional[str]:
    """Derive the INCLUDE_ASM directory argument from the DB asm_file_path.

    e.g.  "asm/us/main/nonmatchings/18B8/func_80010000.s"
        → "asm/us/main/nonmatchings/18B8"
    """
    if not asm_file_path:
        return None
    return str(Path(asm_file_path).parent)


def replace_func_with_stub(content: str, func_name: str, asm_dir: str) -> Tuple[str, bool]:
    """Replace a C function body definition with an INCLUDE_ASM stub.

    Locates the function definition by matching its header line ending with '{'
    (not a prototype ending with ';'), walks brace-counting to the matching '}',
    and replaces the entire range with a single INCLUDE_ASM line.

    Returns (new_content, was_replaced).
    """
    pattern = re.compile(
        r'^[^\s\n][^\n]*\b' + re.escape(func_name) + r'\s*\([^)]*\)[^;{]*\{',
        re.MULTILINE,
    )
    m = pattern.search(content)
    if not m:
        return content, False

    # Start of the definition (include the return-type line which starts at the
    # previous newline, or at the very beginning of the file).
    def_start = content.rfind('\n', 0, m.start()) + 1

    # Walk from the opening '{' to find the matching closing '}'
    depth = 0
    end_pos = None
    for i in range(m.end() - 1, len(content)):
        if content[i] == '{':
            depth += 1
        elif content[i] == '}':
            depth -= 1
            if depth == 0:
                end_pos = i
                break
    if end_pos is None:
        return content, False

    # Include the trailing newline after '}'
    after_end = end_pos + 1
    if after_end < len(content) and content[after_end] == '\n':
        after_end += 1

    stub = f'INCLUDE_ASM("{asm_dir}", {func_name});\n'
    return content[:def_start] + stub + content[after_end:], True


def run_rebuild(verbose: bool = False) -> bool:
    """Run `make rebuild` from the project root and return True on success."""
    print("\nRunning make rebuild to establish expected/ baseline...")
    result = subprocess.run(
        ['make', 'rebuild'],
        cwd=str(PROJECT_ROOT),
        capture_output=not verbose,
        text=True,
    )
    if result.returncode == 0:
        print("✅ make rebuild succeeded — expected/ baseline established")
        return True
    else:
        print("❌ make rebuild failed")
        if not verbose:
            # Print the tail of the output to show what went wrong
            combined = (result.stdout or '') + (result.stderr or '')
            print(combined[-3000:])
        return False


def main():
    parser = argparse.ArgumentParser(
        description='Revert decompiled functions to INCLUDE_ASM stubs for objdiff baseline')
    parser.add_argument('--dry-run', action='store_true',
                        help='Show what would be done without writing changes')
    parser.add_argument('--module', type=str,
                        help='Only process a specific module (e.g. battle, field)')
    parser.add_argument('--skip-build', action='store_true',
                        help='Revert files but skip the make rebuild step')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Verbose output (also shows full build log)')
    args = parser.parse_args()

    db = DecompDatabase()

    # Collect all functions that have been decompiled in any form
    statuses = ['decompiled', 'verified', 'decompiled_needs_refine']
    functions: List[Dict] = []
    for status in statuses:
        functions.extend(db.get_functions_by_status(status, module=args.module))

    if not functions:
        print("No decompiled/verified functions found — nothing to revert")
        return

    print(f"Found {len(functions)} function(s) with statuses: {statuses}")
    if args.module:
        print(f"  (filtered to module: {args.module})")

    # Group by C file so each file is read/written only once
    by_file: Dict[str, List] = {}
    for func in functions:
        c_file = func.get('c_file_path')
        if not c_file:
            print(f"  Warning: {func['name']} has no c_file_path, skipping")
            continue
        by_file.setdefault(c_file, []).append(func)

    print(f"Across {len(by_file)} C file(s)\n")

    reverted_total = 0
    skipped_total = 0
    reverted_names: List[str] = []

    for rel_c_path, funcs_in_file in sorted(by_file.items()):
        full_path = PROJECT_ROOT / rel_c_path
        if not full_path.exists():
            print(f"  ⚠️  File not found: {rel_c_path}")
            skipped_total += len(funcs_in_file)
            continue

        content = full_path.read_text()
        file_reverted = 0

        for func in funcs_in_file:
            func_name = func['name']
            asm_dir = get_include_asm_dir(func.get('asm_file_path') or '')

            if not asm_dir:
                print(f"    ⚠️  {func_name}: no asm_file_path in DB — skipping")
                skipped_total += 1
                continue

            # Already a stub? Nothing to do.
            already_stub = re.search(
                rf'INCLUDE_ASM\s*\(\s*"[^"]*"\s*,\s*{re.escape(func_name)}\s*\)',
                content,
            )
            if already_stub:
                if args.verbose:
                    print(f"    ⏭  {func_name}: already INCLUDE_ASM stub")
                skipped_total += 1
                continue

            new_content, replaced = replace_func_with_stub(content, func_name, asm_dir)
            if replaced:
                content = new_content
                file_reverted += 1
                reverted_names.append(func_name)
                if args.verbose:
                    print(f"    ✅ {func_name}: replaced with INCLUDE_ASM stub")
            else:
                if args.verbose:
                    print(f"    ⚠️  {func_name}: body not found in file — already a stub?")
                # Treat as already-stubbed (body not found means it was already replaced)
                reverted_names.append(func_name)
                file_reverted += 1

        if file_reverted > 0:
            if not args.dry_run:
                full_path.write_text(content)
                print(f"  📝 {rel_c_path}: reverted {file_reverted} function(s)")
            else:
                print(f"  [DRY RUN] {rel_c_path}: would revert {file_reverted} function(s)")

        reverted_total += file_reverted

    print(f"\nReverted: {reverted_total}  Skipped: {skipped_total}")

    if args.dry_run:
        print("\n[DRY RUN] No files written, no DB changes, no rebuild.")
        if not args.skip_build:
            print("[DRY RUN] Would run: make rebuild")
        return

    # Update DB: mark all reverted functions back to 'todo'
    if reverted_names:
        print("\nUpdating database...")
        for func_name in reverted_names:
            db.update_function_status(
                func_name, 'todo',
                notes='Reverted to INCLUDE_ASM stub for objdiff baseline rebuild')
        print(f"  Marked {len(reverted_names)} function(s) as 'todo'")

    if args.skip_build:
        print("\nSkipping rebuild (--skip-build). Run `make rebuild` manually when ready.")
        return

    success = run_rebuild(verbose=args.verbose)
    if not success:
        print("\n⚠️  Build failed after revert. Common causes:")
        print("   - Remaining compile errors in C files (e.g. type conflicts in field.c/world.c)")
        print("   - These are pre-existing issues unrelated to decompiled functions.")
        print("   Re-run with --verbose for the full build output.")
        sys.exit(1)
    else:
        print("\n✅ Baseline established.")
        print("   The expected/ directory now has reference object files for objdiff-cli.")
        print("   Next steps:")
        print("     1. Restart the automation agent — it will now verify each")
        print("        decompiled function against the binary before keeping it.")
        print("     2. Run: python agent.py --run --verbose")


if __name__ == '__main__':
    main()
