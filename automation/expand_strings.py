#!/usr/bin/env python3
"""expand_strings.py

Finds FF7-encoded byte arrays in .c source files and converts them back to
human-readable _S() / _SL() string macros.

A byte array is treated as an FF7 string when:
  - It contains a 0xFF terminator byte
  - Every byte before 0xFF is in the range 0x00-0x5E (maps to ASCII 0x20-0x7E)
  - Every byte after 0xFF is 0x00 (padding)

Encoding: byte + 0x20 = ASCII character (e.g. 0x28 -> 'H', 0x00 -> ' ')

Usage:
    python3 expand_strings.py                    # all src/**/*.c files
    python3 expand_strings.py src/menu           # specific directory or file
    python3 expand_strings.py --in-place         # overwrite files instead of printing
"""

import argparse
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

# Matches a braced list of hex bytes (0xNN) and/or bare zeros, possibly multiline.
# Captures the interior so we can extract individual values.
_ARRAY_RE = re.compile(
    r'\{((?:\s*(?:0x[0-9A-Fa-f]{1,2}|0)\s*,?\s*)+)\}',
    re.DOTALL,
)

# Pulls every numeric token out of the interior matched above.
_TOKEN_RE = re.compile(r'0x([0-9A-Fa-f]{1,2})|(?<!\w)(0)(?!\w)')


def _parse_bytes(interior: str) -> list[int] | None:
    """Return list of byte values from the array interior, or None on parse error."""
    values = []
    for m in _TOKEN_RE.finditer(interior):
        if m.group(1) is not None:
            values.append(int(m.group(1), 16))
        else:
            values.append(0)
    return values if values else None


def _decode_ff7(byte_list: list[int]) -> tuple[str, int] | None:
    """
    Try to decode a byte list as an FF7 ASCII string.

    Returns (decoded_text, num_padding_zeros) if valid, else None.
    Valid means:
      - contains 0xFF
      - all bytes before 0xFF are in 0x00-0x5E  (printable ASCII when + 0x20)
      - all bytes after 0xFF are 0x00 (padding)
    """
    try:
        ff_idx = byte_list.index(0xFF)
    except ValueError:
        return None

    content = byte_list[:ff_idx]
    padding = byte_list[ff_idx + 1:]

    if any(b > 0x5E for b in content):
        return None  # contains non-ASCII FF7 special bytes
    if any(b != 0x00 for b in padding):
        return None  # non-zero bytes after terminator

    text = ''.join(chr(b + 0x20) for b in content)
    return (text, len(padding))


def _encode_c_string(text: str) -> str:
    """Escape a string for inclusion inside a C double-quoted literal."""
    return text.replace('\\', '\\\\').replace('"', '\\"')


def collect_file(content: str) -> str:
    """
    Scan *content* for byte-array initializers that look like FF7 strings and
    replace them with _S() / _SL() macros.  Returns the rewritten content.
    """
    def replace(m: re.Match) -> str:
        interior = m.group(1)
        byte_list = _parse_bytes(interior)
        if byte_list is None:
            return m.group(0)

        result = _decode_ff7(byte_list)
        if result is None:
            return m.group(0)

        text, num_padding = result
        escaped = _encode_c_string(text)

        if num_padding == 0:
            return f'_S("{escaped}")'
        else:
            # _SL(N, "text") produces N-1 bytes total, so N = len(byte_list) + 1
            return f'_SL({len(byte_list) + 1}, "{escaped}")'

    return _ARRAY_RE.sub(replace, content)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert FF7-encoded hex byte arrays in C source files back to _S()/_SL() macros"
    )
    parser.add_argument(
        "target",
        nargs="?",
        default=str(REPO_ROOT / "src"),
        help="path to a .c file or directory to search recursively (default: src/)",
    )
    parser.add_argument(
        "--in-place",
        action="store_true",
        help="overwrite source files with converted output instead of printing to stdout",
    )
    args = parser.parse_args()

    target = Path(args.target)
    if target.is_file():
        files = [target]
    elif target.is_dir():
        files = sorted(target.rglob("*.c"))
    else:
        print(f"error: not a file or directory: {target}", file=sys.stderr)
        sys.exit(1)

    for path in files:
        original = path.read_text(errors="replace")
        converted = collect_file(original)
        if args.in_place:
            if converted != original:
                path.write_text(converted)
                print(f"converted: {path}")
        else:
            print(f"/* === {path} === */")
            print(converted)


if __name__ == "__main__":
    main()
