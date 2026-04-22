#!/usr/bin/env python3
"""
Function Discovery Scanner

Scans the FF7 decompilation codebase to find all INCLUDE_ASM macros
and populates the tracking database.

Usage:
    python discover.py --scan          # Scan codebase and update database
    python discover.py --stats         # Show statistics
    python discover.py --list battle   # List functions in battle module
    python discover.py --list-todo     # Show next functions to decompile
"""

import re
import os
import argparse
import subprocess
from pathlib import Path
from typing import List, Tuple, Optional, Dict
from database import DecompDatabase


# Project root is one level up from automation/
PROJECT_ROOT = Path(__file__).parent.parent
SRC_DIR = PROJECT_ROOT / "src"
ASM_DIR = PROJECT_ROOT / "asm" / "us"


def find_include_asm_functions(c_file_path: Path) -> List[Tuple[str, str]]:
    """
    Parse a C file and find all INCLUDE_ASM macros.
    
    Returns:
        List of (function_name, asm_path_hint) tuples
    """
    functions = []
    
    try:
        content = c_file_path.read_text(encoding='utf-8', errors='ignore')
    except Exception as e:
        print(f"Warning: Could not read {c_file_path}: {e}")
        return functions

    # Honour the "do not decompile" sentinel comment: stop scanning at that point.
    # This is checked by content rather than line number so it survives edits above it.
    SENTINEL = "// NOTE: please do not decompile any of these functions."
    sentinel_pos = content.find(SENTINEL)
    if sentinel_pos != -1:
        content = content[:sentinel_pos]
    
    # Match patterns like:
    # INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1158);
    # INCLUDE_ASM(const s32, "asm/us/main/nonmatchings/1050", D_8002AA14);
    pattern = r'INCLUDE_ASM\s*\([^,]*,\s*"([^"]+)"\s*,\s*([^);\s]+)'
    matches = re.finditer(pattern, content)
    
    for match in matches:
        asm_path_hint = match.group(1)
        func_name = match.group(2)
        functions.append((func_name, asm_path_hint))
    
    # Also match simpler pattern without type:
    # INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1158);
    pattern2 = r'INCLUDE_ASM\s*\(\s*"([^"]+)"\s*,\s*([^);\s]+)'
    matches2 = re.finditer(pattern2, content)
    
    for match in matches2:
        asm_path_hint = match.group(1)
        func_name = match.group(2)
        # Avoid duplicates
        if (func_name, asm_path_hint) not in functions:
            functions.append((func_name, asm_path_hint))
    
    return functions


def infer_module_from_path(c_file_path: Path) -> str:
    """Infer the module name from the source file path."""
    rel_path = c_file_path.relative_to(SRC_DIR)
    parts = rel_path.parts
    if len(parts) > 0:
        return parts[0]  # First directory is usually the module
    return "unknown"


def count_asm_lines(asm_path: Path) -> int:
    """Count the number of assembly instruction lines (rough difficulty metric)."""
    if not asm_path.exists():
        return 0
    
    try:
        content = asm_path.read_text(encoding='utf-8', errors='ignore')
        # Count lines that look like instructions (start with whitespace + instruction)
        lines = [l for l in content.split('\n') if l.strip() and not l.strip().startswith('#')]
        return len(lines)
    except Exception:
        return 0


def resolve_asm_path(asm_path_hint: str) -> Optional[Path]:
    """
    Try to find the actual .s file from the hint in INCLUDE_ASM.
    
    The hint is usually like "asm/us/battle/nonmatchings/battle"
    and we need to find the .s file in that directory.
    """
    # Remove "asm/us/" prefix if present
    hint = asm_path_hint.replace("asm/us/", "").replace("asm/", "")
    
    # This is typically a directory; function .s files are inside
    asm_dir_path = ASM_DIR / hint
    
    return asm_dir_path if asm_dir_path.exists() else None


def scan_codebase(db: DecompDatabase):
    """Scan the entire src/ directory for INCLUDE_ASM functions."""
    print(f"Scanning {SRC_DIR} for INCLUDE_ASM functions...")
    
    total_found = 0
    
    # Find all .c files
    c_files = list(SRC_DIR.rglob("*.c"))
    print(f"Found {len(c_files)} C files to scan")
    
    for c_file in c_files:
        functions = find_include_asm_functions(c_file)
        
        if not functions:
            continue
        
        module = infer_module_from_path(c_file)
        rel_c_path = str(c_file.relative_to(PROJECT_ROOT))
        
        for func_name, asm_hint in functions:
            # Try to resolve the actual asm path
            asm_dir = resolve_asm_path(asm_hint)
            asm_path_str = str(Path(asm_hint) / f"{func_name}.s") if asm_dir else asm_hint
            
            # Try to count lines for difficulty estimate
            if asm_dir:
                asm_file = asm_dir / f"{func_name}.s"
                line_count = count_asm_lines(asm_file)
            else:
                line_count = 0
            
            # Add to database
            db.add_function(
                name=func_name,
                c_file_path=rel_c_path,
                asm_file_path=asm_path_str,
                module=module,
                line_count=line_count
            )
            
            total_found += 1
            print(f"  Found: {func_name} in {module} ({line_count} asm lines)")

        # Remove any previously-scanned entries from this file that are now beyond
        # the sentinel comment (i.e. not in the set we just collected).
        valid_names = {fn for fn, _ in functions}
        removed = db.remove_functions_not_in_set(rel_c_path, valid_names)
        for name in removed:
            print(f"  Removed (below sentinel): {name}")
    
    print(f"\n✓ Scan complete! Found {total_found} functions")


def show_statistics(db: DecompDatabase):
    """Display statistics about the decompilation progress."""
    stats = db.get_statistics()
    
    print("\n" + "="*60)
    print("DECOMPILATION STATISTICS")
    print("="*60)
    
    print(f"\nTotal functions: {stats['total_functions']}")
    
    print("\nBy Status:")
    for status, count in sorted(stats['by_status'].items()):
        percentage = (count / stats['total_functions'] * 100) if stats['total_functions'] > 0 else 0
        print(f"  {status:15s}: {count:4d} ({percentage:5.1f}%)")
    
    print("\nBy Module:")
    for module, count in sorted(stats['by_module'].items()):
        print(f"  {module:15s}: {count:4d}")
    
    print("\n" + "="*60 + "\n")


def list_functions_by_module(db: DecompDatabase, module: str):
    """List all functions in a specific module."""
    functions = db.get_functions_by_module(module)
    
    if not functions:
        print(f"No functions found in module '{module}'")
        return
    
    print(f"\nFunctions in {module} module ({len(functions)} total):")
    print("-" * 80)
    
    for func in functions:
        status_emoji = {
            'todo': '⏳',
            'in_progress': '🔄',
            'decompiled': '✓',
            'verified': '✓✓',
            'failed': '✗'
        }.get(func['status'], '?')
        
        print(f"{status_emoji} {func['name']:30s} | {func['status']:12s} | "
              f"{func['line_count']:3d} lines | {func['c_file_path']}")


def populate_rank_scores(db: DecompDatabase) -> int:
    """Run `mako.sh rank` for every module and store scores in the database.

    The rank score (0.0 easiest → 1.0 hardest) comes from the asm-differ
    similarity metric embedded in mako.sh rank.  Lower scores mean the
    function is structurally closer to its original assembly, making it a
    better candidate for automated decompilation.

    Returns:
        Total number of database rows updated.
    """
    mako = PROJECT_ROOT / "mako.sh"
    asm_base = PROJECT_ROOT / "asm" / "us"

    modules = sorted(p.name for p in asm_base.iterdir() if p.is_dir())
    scores: Dict[str, float] = {}

    for module in modules:
        try:
            result = subprocess.run(
                [str(mako), "rank", module],
                capture_output=True,
                text=True,
                cwd=PROJECT_ROOT,
            )
            if result.returncode != 0:
                print(f"  rank: skipping {module} ({result.stderr.strip()[:80]})")
                continue
            for line in result.stdout.splitlines():
                line = line.strip()
                if not line:
                    continue
                parts = line.split(": ", 1)
                if len(parts) != 2:
                    continue
                try:
                    score = float(parts[0])
                except ValueError:
                    continue
                func_name = parts[1].removesuffix(".s")
                scores[func_name] = score
        except Exception as e:
            print(f"  rank: error for {module}: {e}")

    updated = db.update_rank_scores(scores)
    print(f"✓ Rank scores updated for {updated} functions ({len(scores)} from mako.sh rank)")
    return updated


def list_todo_functions(db: DecompDatabase, limit: int = 20):
    """List the next functions to work on."""
    functions = db.get_next_todo(limit)
    
    if not functions:
        print("No TODO functions found! 🎉")
        return
    
    print(f"\nNext {limit} functions to decompile (sorted by difficulty):")
    print("-" * 80)
    
    for i, func in enumerate(functions, 1):
        print(f"{i:2d}. {func['name']:30s} | {func['module']:10s} | "
              f"{func['line_count']:3d} asm lines | {func['c_file_path']}")
    
    print("\nTo decompile a function manually:")
    print(f"  ./mako.sh dec {functions[0]['name']}")


def list_by_status(db: DecompDatabase, status: str, limit: int = 50):
    """List functions with a specific status."""
    functions = db.get_functions_by_status(status, limit=limit)
    
    if not functions:
        print(f"No functions with status '{status}' found.")
        return
    
    print(f"\nFunctions with status '{status}' ({len(functions)} shown):")
    print("-" * 80)
    
    for i, func in enumerate(functions, 1):
        attempts = func.get('attempt_count', 0)
        notes = func.get('notes', '')
        note_preview = (notes[:50] + '...') if notes and len(notes) > 50 else (notes or '')
        
        print(f"{i:3d}. {func['name']:30s} | {func['module']:10s} | "
              f"{func['line_count']:3d} lines | attempts: {attempts}")
        if note_preview:
            print(f"      Note: {note_preview}")
    
    print(f"\nTotal {status}: {len(functions)}")
    if len(functions) >= limit:
        print(f"(Limited to {limit}, use --limit to see more)")


def main():
    parser = argparse.ArgumentParser(
        description="Discover and track INCLUDE_ASM functions in FF7 decompilation"
    )
    parser.add_argument('--scan', action='store_true',
                        help='Scan the codebase and update database')
    parser.add_argument('--stats', action='store_true',
                        help='Show decompilation statistics')
    parser.add_argument('--list', type=str, metavar='MODULE',
                        help='List functions in a specific module')
    parser.add_argument('--list-todo', action='store_true',
                        help='List next functions to decompile')
    parser.add_argument('--list-status', type=str, metavar='STATUS',
                        help='List functions with specific status (blocked, failed, decompiled_needs_refine, etc.)')
    parser.add_argument('--update-ranks', action='store_true',
                        help='Run mako.sh rank for all modules and update difficulty scores in DB')
    parser.add_argument('--limit', type=int, default=50,
                        help='Limit number of results (default: 50)')
    parser.add_argument('--db', type=str, default='functions.db',
                        help='Database file path (default: functions.db)')
    
    args = parser.parse_args()
    
    # Initialize database
    db = DecompDatabase(args.db)
    
    try:
        if args.scan:
            scan_codebase(db)
            populate_rank_scores(db)
            show_statistics(db)
        elif args.update_ranks:
            populate_rank_scores(db)
        elif args.stats:
            show_statistics(db)
        elif args.list:
            list_functions_by_module(db, args.list)
        elif args.list_todo:
            list_todo_functions(db, args.limit if args.limit != 50 else 20)
        elif args.list_status:
            list_by_status(db, args.list_status, args.limit)
        else:
            parser.print_help()
    finally:
        db.close()


if __name__ == "__main__":
    main()
