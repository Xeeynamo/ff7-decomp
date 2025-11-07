#!.venv/bin/python3
import os
import sys
import subprocess


def parse_symbol_line(symbol_entry: str) -> tuple[str, str, str]:
    parts = symbol_entry.strip().split(" ")
    if len(parts) != 3:
        raise Exception(f"invalid symbol entry: {symbol_entry}")
    return parts[0], parts[1], parts[2]


def get_symbols_from_file(file_path: str) -> list[tuple[str, str, str]]:
    if not os.path.isfile(file_path):
        raise Exception(f"file {file_path} not found or is not a file")
    result = subprocess.run(
        ["nm", file_path], capture_output=True, text=True, check=True
    )
    return [
        parse_symbol_line(line) for line in result.stdout.strip().split("\n") if line
    ]


def is_section_header(name: str) -> bool:
    if len(name) < 2:
        return False
    if name.endswith("_c") or name.endswith("__s"):
        return True
    if name.endswith("_END") or name.endswith("_START") or name.endswith("VRAM"):
        return True
    return False


def is_label(name: str) -> bool:
    return name.startswith(".L")


def is_jump_table(name: str) -> bool:
    return name.startswith("jtbl_")


def is_splat_data(name: str) -> bool:
    return name.startswith("D_")


def is_function(name: str) -> bool:
    return (
        not is_section_header(name)
        and not is_label(name)
        and not is_jump_table(name)
        and not is_splat_data(name)
    )


def filter_functions(
    symbol_list: list[tuple[str, str, str]],
) -> list[tuple[str, str, str]]:
    return [
        symbol
        for symbol in symbol_list
        if symbol[1] == "T"
        and is_function(symbol[2])
        and not symbol[2].endswith(".NON_MATCHING")
    ]


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(
            f"Usage: {sys.executable} {sys.argv[0]} <elf_file_path> [elf_file_paths...]"
        )
        sys.exit(1)
    functions = []
    for elf_file_path in sys.argv[1:]:
        symbols = get_symbols_from_file(elf_file_path)
        functions += filter_functions(symbols)
    sys.stdout.writelines([f"{s[2]} = 0x{s[0]};\n" for s in functions])
