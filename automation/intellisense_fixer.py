#!/usr/bin/env python3
"""
Deterministic IntelliSense / clangd error fixer for m2c-generated C code.

These passes run after m2c writes a decompiled function and BEFORE validation,
so they reduce spurious validation failures caused by known m2c artifacts rather
than genuine type-inference problems.

Passes (all text-based, no LLM required):
  1. fix_unk_negative_fields  — ->unk-C  →  ->unkC    (m2c hex-suffix bug)
  2. hoist_early_typedefs     — move typedefs that appear after their first use
  3. remove_duplicate_typedefs — deduplicate identical typedef blocks
  4. hoist_extern_declarations — collect all top-level extern / forward-decl lines
                                  and place them in a single block before the first
                                  INCLUDE_ASM / function definition
"""

import re
from typing import List, Tuple, Optional


# ---------------------------------------------------------------------------
# Pass 1: fix ->unk-N / .unk-N  →  ->unkN / .unkN
# ---------------------------------------------------------------------------

def fix_unk_negative_fields(content: str) -> Tuple[str, int]:
    """
    Fix m2c artifact where struct field names get a spurious '-' before their
    hex offset suffix.

      Before:  D_800AF3D4.unk-C = 0;
      After:   D_800AF3D4.unkC = 0;

      Before:  temp->unk-4
      After:   temp->unk4

    Returns (fixed_content, num_fixes).
    """
    # Match ->unk-HEX  or  .unk-HEX  (hex digits follow the minus)
    pattern = re.compile(r'(->|\.)unk-([0-9A-Fa-f]+)\b')
    count = [0]

    def _replace(m: re.Match) -> str:
        count[0] += 1
        sep = m.group(1)
        hex_part = m.group(2)
        return f'{sep}unk{hex_part}'

    fixed = pattern.sub(_replace, content)
    return fixed, count[0]


# ---------------------------------------------------------------------------
# Pass 2 & 3: typedef hoisting and deduplication
# ---------------------------------------------------------------------------

def _find_typedef_blocks(lines: List[str]) -> List[Tuple[int, int, str, str]]:
    """
    Scan lines for typedef blocks, including multi-line struct typedefs.

    Returns list of (start_idx, end_idx, typedef_name, full_text).
    Indices are 0-based into `lines`.
    """
    blocks: List[Tuple[int, int, str, str]] = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()
        if not re.match(r'typedef\b', stripped):
            i += 1
            continue

        start = i

        if '{' in line:
            # Multi-line struct/union typedef — scan until braces balance
            depth = line.count('{') - line.count('}')
            j = i + 1
            while j < len(lines) and depth > 0:
                depth += lines[j].count('{') - lines[j].count('}')
                j += 1
            end = j - 1
            # typedef name is on the closing line: } TypeName;
            m = re.search(r'\}\s*(\w+)\s*;', lines[end])
            if m:
                name = m.group(1)
                text = '\n'.join(lines[start:end + 1])
                blocks.append((start, end, name, text))
            i = end + 1
        else:
            # Single-line typedef.  Try several name-extraction patterns:
            #   1. Standard:          typedef <type> Name;
            #   2. Function pointer:  typedef RetType (*Name)(Params);
            #   3. Array:             typedef Type Name[N];
            name = None

            # Pattern 1: function pointer — typedef ... (*Name)(...)
            m = re.search(r'typedef\s+.+?\(\s*\*\s*(\w+)\s*\)\s*\(', line)
            if m:
                name = m.group(1)

            # Pattern 2: standard — last \w+ before the final ;
            if name is None:
                m = re.search(r'typedef\s+.+?\s+(\w+)\s*(?:\[\w*\])?\s*;', line)
                if m:
                    name = m.group(1)

            if name:
                blocks.append((i, i, name, line))
            i += 1

    return blocks


def _first_use_line(lines: List[str], typedef_name: str,
                    exclude_start: int, exclude_end: int) -> Optional[int]:
    """
    Return the index of the first line that uses `typedef_name` as a type name,
    excluding the definition lines [exclude_start, exclude_end].

    We look for the name at a word boundary that looks like a type position:
      - extern TypeName ...
      - TypeName* var
      - TypeName var
      - function parameter of that type
    We also explicitly ignore lines that are only inside the typedef definition
    itself and comment lines.
    """
    # Build a pattern that matches the name used as a type (not as part of a larger word)
    pat = re.compile(rf'\b{re.escape(typedef_name)}\b')
    for idx, line in enumerate(lines):
        if exclude_start <= idx <= exclude_end:
            continue
        stripped = line.strip()
        if stripped.startswith('//') or stripped.startswith('*'):
            continue
        if pat.search(line):
            return idx
    return None


def hoist_early_typedefs(content: str) -> Tuple[str, int]:
    """
    For any typedef whose first use appears *before* its definition, move the
    typedef to just before that first use.

    Also removes exact duplicate typedef blocks (same name, keeps first).

    Returns (fixed_content, num_typedefs_moved).
    """
    lines = content.split('\n')
    blocks = _find_typedef_blocks(lines)

    if not blocks:
        return content, 0

    # --- Deduplicate: keep first occurrence of each name ---
    seen_names: set = set()
    unique_blocks: List[Tuple[int, int, str, str]] = []
    dup_ranges: set = set()  # line ranges to delete (duplicates)

    for start, end, name, text in blocks:
        if name in seen_names:
            for ln in range(start, end + 1):
                dup_ranges.add(ln)
        else:
            seen_names.add(name)
            unique_blocks.append((start, end, name, text))

    # Remove duplicate lines first (in a copy we'll work with)
    if dup_ranges:
        lines = [line for i, line in enumerate(lines) if i not in dup_ranges]
        # Re-scan after removal
        blocks = _find_typedef_blocks(lines)
        unique_blocks_new = []
        seen2: set = set()
        for start, end, name, text in blocks:
            if name not in seen2:
                seen2.add(name)
                unique_blocks_new.append((start, end, name, text))
        unique_blocks = unique_blocks_new

    moved = 0
    # Process typedefs in reverse order so that removing/inserting lines doesn't
    # invalidate earlier indices.
    for start, end, name, text in reversed(unique_blocks):
        first_use = _first_use_line(lines, name, start, end)
        if first_use is None or first_use >= start:
            # No earlier use — leave in place
            continue

        # Extract the typedef block
        typedef_lines = lines[start:end + 1]

        # Remove it from current position (may leave a blank line)
        del lines[start:end + 1]

        # Recalculate insertion point (indices shifted after delete)
        insert_at = first_use  # first_use < start, so not affected by the delete

        # Insert before first use, preceded by a blank line if not already present
        insertion = typedef_lines + ['']
        lines[insert_at:insert_at] = insertion
        moved += 1

    return '\n'.join(lines), moved + len(dup_ranges)


# ---------------------------------------------------------------------------
# Pass 4: hoist extern declarations to the file header
# ---------------------------------------------------------------------------

# Patterns for extern declarations (both forms produced by m2c / mako.sh dec)
_EXTERN_KW_PAT = re.compile(r'^\s*extern\b')          # extern TYPE NAME;
_EXTERN_FWD_PAT = re.compile(r'//\s*extern\s*$')      # TYPE FUNC(...);  // extern


def _is_extern_decl(line: str) -> bool:
    """Return True if the line is a top-level extern declaration (either style)."""
    stripped = line.strip()
    # Skip declarations with m2c '?' unknown types — PSY-Q can't compile them
    if '?' in stripped:
        return False
    # Skip extern declarations with initializers (e.g. m2c %gp annotations like
    # `extern s32 D_80062F10 = 0; // %gp`). These define symbols, which will
    # conflict with ASM BSS/sdata objects that already own those symbols.
    if _EXTERN_KW_PAT.match(stripped) and '=' in stripped:
        return False
    if _EXTERN_KW_PAT.match(stripped):
        return True
    # Forward-decl comment style: must also look like a prototype (has parens)
    if _EXTERN_FWD_PAT.search(stripped) and '(' in stripped and ')' in stripped:
        return True
    return False


def _extern_type_name(line: str) -> Optional[str]:
    """
    Extract the primary type name used in an extern declaration, so we can
    check whether that type is defined later in the file.

    Returns the first identifier after 'extern' (or at the start of a
    forward-decl line), stripping qualifiers like 'const', '/*?*/', etc.
    Returns None if extraction fails.
    """
    stripped = line.strip()
    # Remove leading /*?*/ comment if present
    stripped = re.sub(r'^/\*\??\*/\s*', '', stripped)
    # Remove 'extern' keyword
    stripped = re.sub(r'^extern\s+', '', stripped)
    # Remove const / volatile / static
    stripped = re.sub(r'^(const|volatile|static)\s+', '', stripped)
    # First word is the type name (may have trailing * or [)
    m = re.match(r'^([A-Za-z_]\w*)', stripped)
    if m:
        return m.group(1)
    return None


def _typedef_name_to_last_line(lines: List[str]) -> dict:
    """
    Return a dict mapping typedef name -> last line index of its definition.
    Only covers typedefs that are locally defined in the file (not in headers).
    """
    result: dict = {}
    blocks = _find_typedef_blocks(lines)
    for start, end, name, _ in blocks:
        result[name] = end
    return result


def _is_function_boundary(line: str) -> bool:
    """Return True if the line starts a function definition or INCLUDE_ASM stub."""
    stripped = line.strip()
    if stripped.startswith('INCLUDE_ASM('):
        return True
    # Function definition: contains ( and ends with { (no = before the brace,
    # so we don't accidentally match const array initialisers)
    if '{' not in stripped or ';' in stripped:
        return False
    if stripped.startswith(('//', '/*', 'typedef', 'extern', '#', 'const ')):
        return False
    brace_pos = stripped.rfind('{')
    before_brace = stripped[:brace_pos]
    if '(' not in before_brace or '=' in before_brace:
        return False
    return True


def hoist_extern_declarations(content: str) -> Tuple[str, int]:
    """
    Collect every top-level extern declaration that appears *after* the first
    INCLUDE_ASM / function definition and move them to a single block just
    before that first function boundary.

    Handles both declaration styles:
    - ``extern TYPE NAME;``                        (standard extern variable/function)
    - ``/*?*/ TYPE FUNCNAME(PARAMS);  // extern``  (m2c forward declaration)
    - ``TYPE FUNCNAME(PARAMS);        // extern``  (forward declaration)

    Deduplicates against externs already present in the header section so
    the pass is idempotent.

    Returns (fixed_content, num_externs_moved).
    """
    lines = content.split('\n')

    # ----------------------------------------------------------------
    # 1. Find the first function boundary at brace-depth 0
    # ----------------------------------------------------------------
    depth = 0
    func_boundary: Optional[int] = None
    for i, line in enumerate(lines):
        stripped = line.strip()
        if depth == 0 and _is_function_boundary(line):
            func_boundary = i
            break
        depth += stripped.count('{') - stripped.count('}')
        if depth < 0:
            depth = 0

    if func_boundary is None:
        return content, 0  # No functions/INCLUDE_ASM found — nothing to do

    # ----------------------------------------------------------------
    # 2. Build set of externs already in the header section (before boundary)
    # ----------------------------------------------------------------
    existing: set = set()
    depth = 0
    for i in range(func_boundary):
        stripped = lines[i].strip()
        if depth == 0 and _is_extern_decl(lines[i]):
            existing.add(stripped)
        depth += stripped.count('{') - stripped.count('}')
        if depth < 0:
            depth = 0

    # ----------------------------------------------------------------
    # 3. Collect top-level externs that appear AT or AFTER the boundary
    # ----------------------------------------------------------------
    lines_to_remove: set = set()
    new_externs: List[str] = []
    seen: set = set(existing)

    # Build a map of typedef name -> last line of its definition so we can
    # skip hoisting any extern whose type is defined *after* the function
    # boundary (i.e., in the body section of the file, not the header).
    # Using `> func_boundary` instead of `> i` because hoisting moves the
    # extern to *before* func_boundary — so any typedef that lives anywhere
    # after func_boundary would end up after the extern post-hoist.
    typedef_last_line = _typedef_name_to_last_line(lines)

    depth = 0  # depth AT func_boundary (reset; recompute from scratch)
    for i in range(func_boundary):
        stripped = lines[i].strip()
        depth += stripped.count('{') - stripped.count('}')
    if depth < 0:
        depth = 0

    for i in range(func_boundary, len(lines)):
        stripped = lines[i].strip()
        if depth == 0 and _is_extern_decl(lines[i]):
            # Don't hoist if the extern's type is defined in the body section
            # (i.e., after the function boundary) — hoisting would place the
            # extern before its typedef.
            type_name = _extern_type_name(lines[i])
            if type_name and type_name in typedef_last_line and typedef_last_line[type_name] > func_boundary:
                pass  # skip — type defined after this extern, leave in place
            else:
                if stripped not in seen:
                    new_externs.append(lines[i].rstrip())
                    seen.add(stripped)
                lines_to_remove.add(i)
        depth += stripped.count('{') - stripped.count('}')
        if depth < 0:
            depth = 0

    if not new_externs:
        return content, 0

    # ----------------------------------------------------------------
    # 4. Rebuild lines without removed externs; collapse extra blank lines
    # ----------------------------------------------------------------
    result: List[str] = []
    prev_blank = False
    for i, line in enumerate(lines):
        if i in lines_to_remove:
            continue
        blank = line.strip() == ''
        if blank and prev_blank:
            continue  # collapse consecutive blanks
        prev_blank = blank
        result.append(line)

    # ----------------------------------------------------------------
    # 5. Find new function boundary in the rebuilt list and insert block
    # ----------------------------------------------------------------
    new_boundary: Optional[int] = None
    depth = 0
    for i, line in enumerate(result):
        if depth == 0 and _is_function_boundary(line):
            new_boundary = i
            break
        stripped = line.strip()
        depth += stripped.count('{') - stripped.count('}')
        if depth < 0:
            depth = 0

    if new_boundary is None:
        # Shouldn't happen; fall back to appending
        result += [''] + new_externs
        return '\n'.join(result), len(new_externs)

    # Build insertion block: blank separator + externs + blank separator
    insert: List[str] = []
    if new_boundary > 0 and result[new_boundary - 1].strip() != '':
        insert.append('')
    insert.extend(new_externs)
    insert.append('')

    result[new_boundary:new_boundary] = insert
    return '\n'.join(result), len(new_externs)


# ---------------------------------------------------------------------------
# Combined entry point
# ---------------------------------------------------------------------------

def apply_all_fixes(content: str, verbose: bool = False) -> Tuple[str, List[str]]:
    """
    Apply all deterministic IntelliSense fixes to `content`.

    Returns (fixed_content, list_of_change_descriptions).

    Order matters:
      1. fix_unk_negative_fields — clean up field-name artifacts first
      2. hoist_extern_declarations — move all externs to the header section so
         their type references become the "first use" of those types
      3. hoist_early_typedefs — now move typedefs to before those first uses
         (which are the extern declarations in the header)
    """
    changes: List[str] = []

    content, n = fix_unk_negative_fields(content)
    if n:
        changes.append(f"fixed {n} unk-N field name(s)")

    content, n = hoist_extern_declarations(content)
    if n:
        changes.append(f"hoisted {n} extern declaration(s) to file header")

    content, n = hoist_early_typedefs(content)
    if n:
        changes.append(f"hoisted/deduped {n} typedef(s)")

    return content, changes
