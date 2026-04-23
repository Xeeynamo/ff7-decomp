#!/usr/bin/env python3
"""
Generate supplementary forward-declaration headers for m2c context.

When m2c decompiles a function it only sees the context of that function's
own C source file.  Functions defined in OTHER C files are invisible, so m2c
emits /*?*/ unknown-type markers for every cross-file call.

This script scans every C source file, extracts the return type and parameter
list of each function that already has a concrete C body (not INCLUDE_ASM),
and writes all those signatures to:

    include/decomp_decls.h

tools/decompile.py appends that file to the m2c temp-context file, so m2c
can resolve cross-file call signatures automatically.

Run this manually or let the automation agent call generate() during startup.
"""

import re
import subprocess
import sys
from pathlib import Path
from typing import Dict, Optional, Set

# Regex to match C function definitions (non-static, public ABI)
# Matches: return_type func_name(params) {
#   - Handles multi-word return types (s32, void, u8*, etc.)
#   - Anchored so it won't match calls or prototypes
_FUNC_DEF_RE = re.compile(
    r'^'
    r'((?:const\s+)?(?:unsigned\s+|signed\s+)?'
    r'(?:void|u8|s8|u16|s16|u32|s32|int|char|short|long|unk_data|unk_ptr|ff7s|Yamada|Unk\w+)'
    r'(?:\s*\*+)?)'  # optional pointer stars
    r'\s+'
    r'(func_[0-9A-Fa-f]+|[A-Za-z_][A-Za-z0-9_]*)'  # function name
    r'\s*\('
    r'([^)]*)'       # parameter list (no nesting needed)
    r'\)'
    r'\s*\{',
    re.MULTILINE,
)

# Skip patterns that indicate INCLUDE_ASM or a call (not a definition)
_INCLUDE_ASM_RE = re.compile(r'INCLUDE_ASM\s*\(')
# Skip static functions (file-internal; m2c context already has them)
_STATIC_RE = re.compile(r'^\s*static\s+', re.MULTILINE)

# Broader regex that captures ALL function signatures, including struct-typed ones.
# Used for per-module declaration files (where the module's own struct types are
# always available because the module's C file is already in the m2c context).
_FUNC_DEF_RE_ALL = re.compile(
    r'^'
    r'((?:const\s+)?(?:unsigned\s+|signed\s+)?'
    r'\w[\w\s]*?'     # return type — any identifier sequence (non-greedy)
    r'(?:\s*\*+)?)'   # optional pointer stars
    r'\s+'
    r'(func_[0-9A-Fa-f]+|[A-Za-z_][A-Za-z0-9_]*)'  # function name
    r'\s*\('
    r'([^)]*)'        # parameter list
    r'\)'
    r'\s*\{',
    re.MULTILINE,
)


def _extract_signatures(c_file: Path) -> Dict[str, str]:
    """
    Parse a C source file and return {func_name: declaration_line} for every
    non-static function that has a concrete C body (not INCLUDE_ASM).

    Returns a dict like:
        {"func_800BEAD4": "void func_800BEAD4(u8* arg0, s32 arg1);"}
    """
    try:
        text = c_file.read_text(errors='replace')
    except OSError:
        return {}

    result: Dict[str, str] = {}

    for m in _FUNC_DEF_RE.finditer(text):
        ret_type = m.group(1).strip()
        func_name = m.group(2).strip()
        params = m.group(3).strip()

        # Skip if the match is preceded by 'static' on the same "line"
        line_start = text.rfind('\n', 0, m.start()) + 1
        prefix = text[line_start:m.start()]
        if 'static' in prefix:
            continue

        # Skip declarations that reference struct/typedef names (identifiers starting
        # with an uppercase letter, e.g. RECT, GzHeader, AKAO_TRACK) or PSX SDK
        # platform types like u_long that are not defined in common.h.
        # Including these would cause parse errors in modules that don't have
        # their headers.
        if re.search(r'\b[A-Z][A-Za-z0-9_]*\b|\bu_long\b', ret_type + ' ' + params):
            continue

        # Normalise param list (collapse whitespace, remove comments)
        params_clean = re.sub(r'\s+', ' ', params).strip()
        if not params_clean:
            params_clean = 'void'

        decl = f"{ret_type} {func_name}({params_clean});"
        result[func_name] = decl

    return result


def _get_module_types(module_dir: Path, project_root: Path) -> Set[str]:
    """
    Run cpp on the module's C files to collect all typedef names that are
    defined after preprocessing.  These are the types it's safe to reference
    in a per-module declaration file.

    Returns an empty set on failure (caller should then skip struct-typed
    declarations for that module).
    """
    # Try the canonical private header first, fall back to any .h in the module dir
    candidates = [
        module_dir / f"{module_dir.name}_private.h",
        module_dir / f"{module_dir.name}.h",
    ]
    header = next((h for h in candidates if h.exists()), None)
    if header is None:
        return set()

    cpp_flags = [
        "-Iinclude", "-Isrc", "-Iinclude/psxsdk",
        "-D_LANGUAGE_C", "-DM2CTX", "-D_MIPS_SZLONG=32",
        "-DSCRIPT(...)={}", "-D__attribute__(...)=", "-D__asm__(...)=",
        "-ffreestanding",
    ]
    try:
        result = subprocess.check_output(
            ["gcc", "-E", "-P"] + cpp_flags + [str(header.relative_to(project_root))],
            cwd=str(project_root),
            encoding="utf-8",
            stderr=subprocess.DEVNULL,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return set()

    types: Set[str] = set()
    # typedef { ... } TypeName;  or  typedef ExistingType TypeName;
    for m in re.finditer(r'}\s*(\w+)\s*;', result):
        types.add(m.group(1))
    for m in re.finditer(r'^typedef\s+\S+\s+(\w+)\s*;', result, re.MULTILINE):
        types.add(m.group(1))
    return types


# Primitive types that are always safe regardless of module
_PRIMITIVE_TYPES = frozenset({
    'void', 'char', 'short', 'int', 'long', 'float', 'double',
    'u8', 's8', 'u16', 's16', 'u32', 's32', 'u64', 's64',
    'f32', 'f64', 'unk_data', 'unk_ptr', 'ff7s',
})


def _extract_signatures_all(c_file: Path, allowed_types: Set[str]) -> Dict[str, str]:
    """
    Like _extract_signatures but includes struct-typed signatures whose types are
    all in `allowed_types` (the set of types defined by the module's own headers).
    """
    try:
        text = c_file.read_text(errors='replace')
    except OSError:
        return {}

    result: Dict[str, str] = {}

    for m in _FUNC_DEF_RE_ALL.finditer(text):
        ret_type = m.group(1).strip()
        func_name = m.group(2).strip()
        params = m.group(3).strip()

        # Skip static functions — check both the prefix text before the regex
        # match start AND the ret_type itself (since \w[\w\s]*? can consume 'static')
        line_start = text.rfind('\n', 0, m.start()) + 1
        prefix = text[line_start:m.start()]
        if 'static' in prefix or 'static' in ret_type.split():
            continue

        # Strip any leading qualifiers that slipped into ret_type (e.g. 'static')
        ret_type = ret_type.lstrip()

        # Skip multi-word return types spanning lines (bogus match)
        if '\n' in ret_type:
            continue

        # Skip control-flow keywords
        if func_name in ('if', 'else', 'while', 'for', 'do', 'switch', 'return', 'goto'):
            continue

        # Collect all identifier tokens in return type and params
        combined = ret_type + ' ' + params
        identifiers = set(re.findall(r'\b([A-Z]\w*)\b', combined))  # uppercase-start = type names

        # All non-primitive uppercase type names must be in allowed_types.
        # Also reject u_long (lowercase but not a primitive — requires psxsdk/types.h).
        unknown = identifiers - allowed_types
        if unknown or re.search(r'\bu_long\b', combined):
            continue

        params_clean = re.sub(r'\s+', ' ', params).strip()
        if not params_clean:
            params_clean = 'void'

        decl = f"{ret_type} {func_name}({params_clean});"
        # Keep the FIRST definition found — #ifndef NON_MATCHINGS blocks come before
        # the #else alternatives, and the #ifndef version is what actually compiles.
        result.setdefault(func_name, decl)

    return result


def generate_module_decls(project_root: Optional[Path] = None, verbose: bool = False) -> int:
    """
    For each module under src/, scan all *.c files and write
    include/decomp_decls_{module}.h with ALL decompiled function signatures
    (including struct-typed ones).

    These per-module files are appended to the m2c context only when decompiling
    a function from that same module, ensuring the struct types referenced in the
    declarations are already defined (via the module's own preprocessed headers).

    Returns the total number of declarations written across all modules.
    """
    if project_root is None:
        here = Path(__file__).resolve().parent
        project_root = here.parent if here.name == 'automation' else here

    src_dir = project_root / 'src'
    include_dir = project_root / 'include'

    if not src_dir.is_dir():
        return 0

    total = 0

    # Each subdirectory of src/ is a module
    for module_dir in sorted(src_dir.iterdir()):
        if not module_dir.is_dir():
            continue
        module_name = module_dir.name

        # Collect the types defined by this module's headers so we only emit
        # declarations whose parameter types are actually available.
        allowed_types = _get_module_types(module_dir, project_root)

        all_decls: Dict[str, str] = {}
        for c_file in sorted(module_dir.rglob('*.c')):
            sigs = _extract_signatures_all(c_file, allowed_types)
            all_decls.update(sigs)

        if not all_decls:
            continue

        out_path = include_dir / f"decomp_decls_{module_name}.h"
        lines = [
            f"/* AUTO-GENERATED by automation/generate_decls.py — do not edit manually */",
            f"/* Per-module declarations for '{module_name}' — only valid in {module_name} m2c context */",
            f"/* struct types referenced here are defined by the module's own preprocessed headers */",
            "",
            f'#include "common.h"',
            ""
        ]
        # Always emit #include "<module>_private.h" if the file exists — it defines
        # the struct types referenced by the declarations below, and must not be
        # removed even though tools/decompile.py strips '#'-prefixed lines before
        # feeding this file to m2c (the include is kept for editor tooling / clangd).
        private_header = module_dir / f"{module_name}_private.h"
        if private_header.exists():
            lines.append(f'#include "../src/{module_name}/{module_name}_private.h"')
            lines.append("")
        for func_name in sorted(all_decls):
            lines.append(all_decls[func_name])
        lines.append("")

        out_path.write_text('\n'.join(lines))
        total += len(all_decls)

        if verbose:
            print(
                f"[generate_decls] {module_name}: {len(all_decls)} declarations → "
                f"{out_path.relative_to(project_root)}"
            )

    return total


def generate(project_root: Optional[Path] = None, verbose: bool = False) -> int:
    """
    Scan all src/**/*.c files and write include/decomp_decls.h.

    Returns the number of declarations written.
    """
    if project_root is None:
        # Running from automation/ or from project root
        here = Path(__file__).resolve().parent
        project_root = here.parent if here.name == 'automation' else here

    src_dir = project_root / 'src'
    out_path = project_root / 'include' / 'decomp_decls.h'

    if not src_dir.is_dir():
        if verbose:
            print(f"[generate_decls] src dir not found: {src_dir}", file=sys.stderr)
        return 0

    all_decls: Dict[str, str] = {}
    files_scanned = 0

    for c_file in sorted(src_dir.rglob('*.c')):
        sigs = _extract_signatures(c_file)
        all_decls.update(sigs)
        files_scanned += 1

    if not all_decls:
        # Nothing to write — don't create an empty file that would clobber a good one
        if verbose:
            print("[generate_decls] No signatures extracted", file=sys.stderr)
        return 0

    lines = [
        "/* AUTO-GENERATED by automation/generate_decls.py — do not edit manually */",
        "/* Forward declarations for all decompiled functions, used as m2c context. */",
        "#ifndef DECOMP_DECLS_H",
        "#define DECOMP_DECLS_H",
        "",
        '#include "common.h"',
        "",
    ]
    for func_name in sorted(all_decls):
        lines.append(all_decls[func_name])
    lines += ["", "#endif /* DECOMP_DECLS_H */", ""]

    out_path.write_text('\n'.join(lines))

    if verbose:
        print(
            f"[generate_decls] Wrote {len(all_decls)} declarations from "
            f"{files_scanned} files → {out_path.relative_to(project_root)}"
        )

    # Also regenerate per-module declaration files
    generate_module_decls(project_root=project_root, verbose=verbose)

    return len(all_decls)


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description="Generate m2c supplementary context declarations")
    parser.add_argument('--root', type=Path, default=None,
                        help='Project root (default: auto-detect)')
    parser.add_argument('--verbose', '-v', action='store_true')
    args = parser.parse_args()

    count = generate(project_root=args.root, verbose=True)
    print(f"Generated {count} declarations.")
