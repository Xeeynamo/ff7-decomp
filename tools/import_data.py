#!/usr/bin/env python3
import argparse
import os
import sys

version = "us"


def asm_path(c_path: str):
    # src/battle/battle.c 👉 asm/us/battle/
    path = os.path.dirname(c_path).replace("src", f"asm/{version}")
    if not os.path.isdir(path):
        raise Exception(f"asm_path did not find path {path}")
    return path


def read_asm_data(asm_path: str, sym_name: str) -> list[str] | None:
    search_sym = f"glabel {sym_name}\n"
    search_end = f".size {sym_name}, . - {sym_name}\n"
    for root, dirs, files in os.walk(asm_path):
        for file_name in files:
            file_path = os.path.join(root, file_name)
            try:
                with open(file_path, "r") as file:
                    for line in file:
                        if line == search_sym:
                            line_data = [line]
                            for line in file:
                                if line == search_end:
                                    break
                                line_data.append(line)
                            return line_data
            except Exception as e:
                sys.stderr.write(f"find_symbol failed to read file {file_path}: {e}\n")
                continue
    return None


def get_splat_asm_line(line: str) -> str:
    idx = line.index("*/ ")
    if idx < 0:
        return line
    return line[idx + 3 :].strip()


def to_c_array(asm_data: list[str], is_signed: bool, is_const: bool) -> str:
    if len(asm_data) < 2:
        sys.stderr.write(f"malformed asm data:\n{asm_data}\n")
        return ""
    sym_name = asm_data[0].split(" ")[1].strip()
    sym_type = get_splat_asm_line(asm_data[1]).split(" ")[0]
    data = [get_splat_asm_line(x).split(" ")[1] for x in asm_data[1:]]
    if is_signed:
        for i in range(len(data)):
            n = int(data[i][2:], 16)
            if sym_type == ".short":
                if n & 0x8000:
                    n -= 0x10000
            elif sym_type == ".byte":
                if n & 0x80:
                    n -= 0x100
            data[i] = str(n)
    if sym_type == ".byte":
        sym_type = "s8" if is_signed else "u8"
    if sym_type == ".short":
        sym_type = "s16" if is_signed else "u16"
    if sym_type == ".word":
        sym_type = "s32" if is_signed else "u32"
    array = "const " if is_const else ""
    array += f"{sym_type} {sym_name}[] = {{"
    array += ", ".join(data)
    array += "};\n"
    return array


def find_first_function_ref(asm_path: str, sym_name: str) -> str | None:
    for root, dirs, files in os.walk(asm_path):
        for file_name in files:
            if file_name.startswith("D_"):
                continue
            file_path = os.path.join(root, file_name)
            with open(file_path, "r") as file:
                if sym_name in file.read():
                    return os.path.splitext(file_name)[0]
    return None


def import_data(c_path: str, sym_name: str, is_signed: bool, is_const: bool):
    asm_search_path = asm_path(c_path)
    asm_data = read_asm_data(f"{asm_search_path}/data", sym_name)
    if not asm_data:
        return sys.stderr.write(f"symbol {sym_name} not found in {asm_search_path}\n")
    c_array = to_c_array(asm_data, is_signed, is_const)
    func_name = find_first_function_ref(f"{asm_search_path}/nonmatchings", sym_name)
    if not func_name:
        func_name = find_first_function_ref(f"{asm_search_path}/matchings", sym_name)
    if not func_name:
        print(c_array)
        return
    with open(c_path, "r") as file:
        lines = []
        for line in file:
            if "INCLUDE_ASM" in line and func_name in line:
                lines.append(c_array)
            if ") {" in line and func_name in line:
                lines.append(c_array)
            lines.append(line)
    with open(c_path, "w") as file:
        file.writelines(lines)


def main():
    parser = argparse.ArgumentParser(description="Import a data or rodata to a C file")
    parser.add_argument("c_path", type=str, help="Path of C file to import data into")
    parser.add_argument("sym_name", type=str, help="Name of the symbol to import")
    parser.add_argument(
        "-s", "--signed", action="store_true", help="Import data as signed"
    )
    parser.add_argument(
        "-c", "--const", action="store_true", help="Import data as const"
    )
    args = parser.parse_args()

    import_data(args.c_path, args.sym_name, args.signed, args.const)


if __name__ == "__main__":
    main()
