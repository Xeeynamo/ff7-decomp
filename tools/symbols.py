#!.venv/bin/python3
import argparse
import os
import re
import subprocess
import sys

ASSIGN_RE = re.compile(r"^\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;")
AUTO_RE = re.compile(r"^(func|D|jtbl)_([0-9A-Fa-f]{8})(_\d+)?$")


def parse_symbol_file(path: str) -> list[tuple[str, int, str]]:
    result = []
    with open(path) as f:
        for line in f:
            code = line.split("//", 1)[0]
            m = ASSIGN_RE.match(code)
            if not m:
                continue
            attrs = line.split("//", 1)[1].strip() if "//" in line else ""
            result.append((m.group(1), int(m.group(2), 16), attrs))
    return result


def write_if_changed(path: str, content: str) -> None:
    if os.path.isfile(path):
        with open(path) as f:
            if f.read() == content:
                return
    with open(path, "w") as f:
        f.write(content)


def nm(objs: list[str]) -> tuple[dict[str, int], set[str]]:
    if not objs:
        return {}, set()
    result = subprocess.run(
        ["mipsel-linux-gnu-nm"] + objs, capture_output=True, text=True, check=True
    )
    defined: dict[str, int] = {}
    undefined = set()
    for line in result.stdout.splitlines():
        parts = line.strip().split()
        if len(parts) == 2:
            addr = None
            sym_type, name = parts
        elif len(parts) == 3:
            addr, sym_type, name = parts
        else:
            continue
        if sym_type == "U":
            if not name.startswith("__maspsx_include_asm_hack_"):
                undefined.add(name)
        elif addr is not None:
            defined[name] = int(addr, 16)
    return defined, undefined - defined.keys()


def parse_attrs(attrs: str) -> dict[str, str]:
    result = {}
    for tok in attrs.split():
        if ":" in tok:
            key, value = tok.split(":", 1)
            result[key] = value
    return result


def format_attrs(attrs: dict[str, str]) -> str:
    if not attrs:
        return ""
    return " ".join(f"{k}:{v}" for k, v in attrs.items())


def cmd_splat_imports(args: argparse.Namespace) -> int:
    vram_start = int(args.vram_start, 16)
    vram_end = int(args.vram_end, 16)

    own_names: set[str] = set()
    own_addrs: set[int] = set()
    for path in args.own:
        for name, addr, _ in parse_symbol_file(path):
            own_names.add(name)
            own_addrs.add(addr)

    # picked: name -> (addr, attrs, is_auto)
    picked: dict[str, tuple[int, str, bool]] = {}
    picked_addrs: dict[int, str] = {}

    for path in args.imports:
        for name, addr, attrs in parse_symbol_file(path):
            parsed_attrs = parse_attrs(attrs)
            if parsed_attrs.get("ignore") == "true":
                continue
            kept_attrs = {
                k: v for k, v in parsed_attrs.items() if k in ("size", "type")
            }
            if name in own_names:
                # verified against the address the owner file already has for this name.
                own_entry = next(
                    (n, a)
                    for p in args.own
                    for n, a, _ in parse_symbol_file(p)
                    if n == name
                )
                if own_entry[1] != addr:
                    print(
                        f"error: {name} = 0x{addr:X} conflicts with own address 0x{own_entry[1]:X}",
                        file=sys.stderr,
                    )
                    return 1
                continue
            if addr in own_addrs:
                continue
            if vram_start <= addr < vram_end:
                print(
                    f"warning: import {name} = 0x{addr:X} points into the importer's own range",
                    file=sys.stderr,
                )
                continue
            is_auto = bool(AUTO_RE.match(name))
            if addr in picked_addrs:
                existing_name = picked_addrs[addr]
                if existing_name != name:
                    existing_is_auto = picked[existing_name][2]
                    if existing_is_auto and not is_auto:
                        del picked[existing_name]
                        picked[name] = (addr, format_attrs(kept_attrs), is_auto)
                        picked_addrs[addr] = name
                    # otherwise keep the existing entry
                continue
            if name in picked:
                if picked[name][0] != addr:
                    print(
                        f"error: {name} picked at two addresses 0x{picked[name][0]:X} and 0x{addr:X}",
                        file=sys.stderr,
                    )
                    return 1
                continue
            picked[name] = (addr, format_attrs(kept_attrs), is_auto)
            picked_addrs[addr] = name

    entries = sorted(picked.items(), key=lambda kv: kv[1][0])
    lines = []
    for name, (addr, attrs, _) in entries:
        if attrs:
            lines.append(f"{name} = 0x{addr:08X}; // {attrs}\n")
        else:
            lines.append(f"{name} = 0x{addr:08X};\n")
    write_if_changed(args.out, "".join(lines))
    return 0


def cmd_ld(args: argparse.Namespace) -> int:
    defined, undefined = nm(args.objs)

    table: dict[str, int] = {}
    for path in args.own + args.imports:
        for name, addr, _ in parse_symbol_file(path):
            if name in table and table[name] != addr:
                print(
                    f"error: {name} has two different addresses 0x{table[name]:X} and 0x{addr:X}",
                    file=sys.stderr,
                )
                return 1
            table[name] = addr
    entries: list[tuple[str, int]] = []
    for name in undefined - defined.keys():
        if name in table:
            entries.append((name, table[name]))
            continue
        m = AUTO_RE.match(name)
        if m:
            entries.append((name, int(m.group(2), 16)))
            continue

    entries.sort(key=lambda e: e[1])
    lines = [f"{name} = 0x{addr:08X};\n" for name, addr in entries]
    write_if_changed(args.out, "".join(lines))
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="command", required=True)

    p_imports = sub.add_parser("splat-imports")
    p_imports.add_argument("-o", dest="out", required=True)
    p_imports.add_argument("--vram-start", required=True)
    p_imports.add_argument("--vram-end", required=True)
    p_imports.add_argument("--own", action="append", default=[])
    p_imports.add_argument("--import", dest="imports", action="append", default=[])
    p_imports.set_defaults(func=cmd_splat_imports)

    p_ld = sub.add_parser("ld")
    p_ld.add_argument("-o", dest="out", required=True)
    p_ld.add_argument("--own", action="append", default=[])
    p_ld.add_argument("--import", dest="imports", action="append", default=[])
    p_ld.add_argument("objs", nargs="*")
    p_ld.set_defaults(func=cmd_ld)

    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
