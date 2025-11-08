import os
import sys
from dataclasses import dataclass
import os

import ninja_syntax
import yaml

CPP_FLAGS = "-Iinclude -Iinclude/psxsdk -DUSE_INCLUDE_ASM -DFF7_STR"
LD_FLAGS = ""

nw: ninja_syntax.Writer = None
objs: list[str] = []
work_dir = "build/us"
if len(sys.argv) > 1:
    work_dir = sys.argv[1]
generate_report = os.environ.get("GENERATE_REPORT") == "1"
if generate_report:
    # https://decomp.wiki/en/tools/decomp-dev
    CPP_FLAGS += " -DSKIP_ASM=1"
check_path = os.path.join(work_dir, "check.sha1")


def basename(cfg) -> str:
    return cfg["options"]["basename"]


def asm_path(cfg) -> str:
    return cfg["options"]["asm_path"]


def build_path(cfg) -> str:
    return cfg["options"]["build_path"]


def ld_path(cfg) -> str:
    return cfg["options"]["ld_script_path"]


def src_path(cfg) -> str:
    return cfg["options"]["src_path"]


def asset_path(cfg) -> str:
    return cfg["options"]["asset_path"]


def platform(cfg) -> str:
    return cfg["options"]["platform"]


@dataclass
class CompilerParams:
    cc1: str
    cc_opt: str
    cc_gp: str
    as_flags: str


def default_compiler_params() -> CompilerParams:
    return CompilerParams(
        "cc1-psx-272", "-O2", "-G0", "--expand-div --aspsx-version=2.34"
    )


def parse_compiler_params(line: str) -> CompilerParams:
    c = default_compiler_params()
    for param in line.strip().split(" "):
        pair = param.split("=")
        if not pair:
            continue
        if len(pair) == 2:
            key, value = pair[0].strip(), pair[1].strip()
        elif len(pair) == 1:
            key, value = pair[0].strip(), ""
        else:
            raise Exception(f"compiler flag {param} is invalid")
        if key == "PSYQ":
            if value == "3.3":
                c.cc1 = "cc1-psx-26"
                c.as_flags = "--expand-div --aspsx-version=2.21"
            elif value == "3.5":
                c.cc1 = "cc1-psx-26"
                c.as_flags = "--expand-div --aspsx-version=2.34"
            elif value == "3.6":
                c.cc1 = "cc1-psx-272"
                c.as_flags = "--expand-div --aspsx-version=2.34"
            elif value == "4.0":
                c.cc1 = "cc1-psx-272"
                c.as_flags = "--expand-div --aspsx-version=2.56"
            else:
                raise Exception(f"{key} value {value} is not recognized")
        elif key == "CC1":
            if value == "2.6.3":
                c.cc1 = "cc1-psx-26"
            elif value == "2.7.2":
                c.cc1 = "cc1-psx-272"
            else:
                raise Exception(f"{key} value {value} is not recognized")
        elif key == "G":
            try:
                n = int(value)
                c.cc_gp = f"-G{n}"
            except ValueError:
                raise Exception(f"{key} value {value} is not a valid integer")
        elif key == "O":
            try:
                n = int(value)
                c.cc_opt = f"-O{n}"
            except ValueError:
                raise Exception(f"{key} value {value} is not a valid integer")
        else:
            raise Exception(f"{key} is not recognized")
    return c


def get_compiler_params(source_file_name: str) -> CompilerParams:
    if not os.path.exists(source_file_name):
        return default_compiler_params()
    with open(source_file_name, "r") as file:
        for i in range(10):  # read the top 10 lines of code
            line = file.readline()
            if not line:
                break
            if line.startswith("//!"):
                return parse_compiler_params(line[3:])
    return default_compiler_params()


def add_s(cfg: any, file_name: str):
    in_path = f"{asm_path(cfg)}/{file_name}.s"
    out_path = f"{build_path(cfg)}/{in_path}.o"
    if out_path in objs:
        return
    objs.append(out_path)
    nw.build(
        rule=f"{platform(cfg)}-as",
        outputs=[out_path],
        inputs=[in_path],
    )
    nw.build(
        rule="phony",
        outputs=[in_path],
        implicit=[ld_path(cfg)],
    )


def add_c(cfg: any, file_name: str):
    in_path = f"{src_path(cfg)}/{file_name}.c"
    out_path = f"{build_path(cfg)}/{in_path}.o"
    if out_path in objs:
        return
    compiler_flags = get_compiler_params(in_path)
    objs.append(out_path)
    nw.build(
        rule=f"{platform(cfg)}-cc",
        outputs=[out_path],
        inputs=[in_path],
        implicit=["include/common.h", "include/game.h"],
        variables={
            "cc1": compiler_flags.cc1,
            "as_flags": compiler_flags.as_flags,
            "cc_flags": f"{compiler_flags.cc_opt} {compiler_flags.cc_gp}",
        },
    )
    nw.build(
        rule="phony",
        outputs=[in_path],
        implicit=[ld_path(cfg)],
    )


def add_copy(cfg: any, file_name: str):
    in_path = f"{asset_path(cfg)}/{file_name}"
    out_path = f"{build_path(cfg)}/{in_path}.o"
    if out_path in objs:
        return
    objs.append(out_path)
    nw.build(
        rule=f"copy",
        outputs=[out_path],
        inputs=[in_path],
    )
    nw.build(
        rule="phony",
        outputs=[in_path],
        implicit=[ld_path(cfg)],
    )


def add_splat_config(file_name: str):
    with open(file_name) as f:
        cfg = yaml.load(f, Loader=yaml.SafeLoader)
    nw.build(
        rule="splat",
        outputs=[ld_path(cfg)],
        inputs=file_name,
        implicit=cfg["options"]["symbol_addrs_path"],
    )
    objs.clear()
    is_main = basename(cfg) == "main"
    is_battle = basename(cfg) == "battle"
    is_batini = basename(cfg) == "batini"
    is_magic = "/magic" in src_path(cfg)
    if platform(cfg) == "psx" and is_main:
        add_s(cfg, "header")
    for segment in cfg["segments"]:
        if not "type" in segment:
            continue
        if segment["type"] != "code":
            continue
        for sub in segment["subsegments"]:
            offset = int(sub[0])
            if len(sub) < 2:
                kind = "data"
                name = segment["name"]
            else:
                kind = str(sub[1])
                if len(sub) > 2:
                    name = str(sub[2])
                else:
                    name = str.format("{0:X}", offset)
            if kind == "data":
                add_s(cfg, f"data/{name}.data")
            elif kind == "rodata":
                add_s(cfg, f"data/{name}.rodata")
            elif kind == "bss":
                add_s(cfg, f"data/{name}.bss")
            elif kind == "asm":
                add_s(cfg, name)
            elif kind == "c" or kind == ".data":
                add_c(cfg, name)
    if generate_report:
        return
    output_name = f"{build_path(cfg)}/{basename(cfg)}.elf"
    sym_export = "config/sym_export.us.txt"
    if is_main:
        nw.build(
            rule="sym-export",
            outputs=[sym_export],
            inputs=[output_name],
        )
    else:
        objs.append(sym_export)
    sym_paths = [
        f"-T {cfg["options"]["undefined_syms_auto_path"]}",
        f"-T config/sym_extern.us.txt",
    ]
    if is_main:
        sym_paths.append("-T config/sym_ovl_export.us.txt")
    if is_battle:
        nw.build(  # needs to export symbol list for batini
            rule="sym-export",
            outputs=f"config/sym_export_{basename(cfg)}.us.txt",
            inputs=[output_name],
        )
        sym_paths.append("-T config/sym_battle_import.us.txt")
    if is_batini or is_magic:
        # batini uses symbols from battle
        sym_paths.append("-T config/sym_export_battle.us.txt")
    nw.build(
        rule="psx-ld",
        outputs=[output_name],
        inputs=[ld_path(cfg)],
        implicit=objs,
        variables={
            "map_path": f"{build_path(cfg)}/{basename(cfg)}.map",
            "obj_paths": objs,
            "symbol_path": str.join(" ", sym_paths),
        },
    )
    if is_main:
        # main must be linked twice:
        # 1. generate sym_export.*.txt and allow other overlays to use SDK funcs
        # 2. to allow overlays re-generating sym_ovl_export.*.txt
        nw.build(
            rule="sym-export",
            outputs=["config/sym_ovl_export.us.txt"],
            inputs=get_ovl_elf_list(check_path),
        )
        nw.build(
            rule="psx-ld",
            outputs=[f"{build_path(cfg)}/{basename(cfg)}_final.elf"],
            inputs=[ld_path(cfg)],
            implicit=objs + ["config/sym_ovl_export.us.txt"],
            variables={
                "map_path": f"{build_path(cfg)}/{basename(cfg)}.map",
                "obj_paths": objs,
                "symbol_path": str.join(" ", sym_paths),
            },
        )
        nw.build(
            rule="psx-exe",
            outputs=[f"{build_path(cfg)}/{basename(cfg)}.exe"],
            inputs=[f"{build_path(cfg)}/{basename(cfg)}_final.elf"],
        )
    else:
        nw.build(
            rule="psx-exe",
            outputs=[f"{build_path(cfg)}/{basename(cfg)}.exe"],
            inputs=[f"{build_path(cfg)}/{basename(cfg)}.elf"],
        )


def get_check_list(file_path) -> list[str]:
    with open(file_path, "r") as f:
        lines = f.readlines()
    return [line.strip().split(" ")[2] for line in lines if line]


def get_ovl_elf_list(file_path) -> list[str]:
    checks = get_check_list(file_path)
    return [x.replace(".exe", ".elf") for x in checks if not x.endswith("main.exe")]


with open("build.ninja", "w") as f:
    nw = ninja_syntax.Writer(f)
    nw.rule(
        "splat",
        command=".venv/bin/splat split $in > /dev/null && touch $out",
        description="splat $in",
    )
    nw.rule(
        "psx-as",
        command="mipsel-linux-gnu-as -Iinclude -march=r3000 -mtune=r3000 -no-pad-sections -O1 -G0 -o $out $in",
        description="psx as $in",
    )
    nw.rule(
        "psx-cc",
        command=(
            f"mipsel-linux-gnu-cpp {CPP_FLAGS} -MMD -MF $out.d -lang-c -Iinclude -Iinclude/psxsdk -undef -Wall -fno-builtin $in"
            " | bin/str"  # convert C-style strings _S("FOO") into FF7-style strings "\x26\x2F\x2F\xFF"
            " | iconv --from-code=UTF-8 --to-code=Shift-JIS"
            " | bin/$cc1 -quiet -mcpu=3000 -g -mgas -gcoff $cc_flags"
            " | python3 tools/maspsx/maspsx.py $as_flags"
            " | mipsel-linux-gnu-as -Iinclude -march=r3000 -mtune=r3000 -no-pad-sections -O1 -G0 -o $out"
        ),
        depfile="$out.d",
        deps="gcc",
        description="psx cc $in",
    )
    nw.rule(
        "copy",
        command="mipsel-linux-gnu-ld -r -b binary -o $out $in",
        description="copy $in",
    )
    nw.rule(
        "psx-ld",
        command=f"mipsel-linux-gnu-ld -nostdlib --no-check-sections {LD_FLAGS} -Map $map_path -T $in $symbol_path -o $out $obj_paths",
        description="psx ld $in",
    )
    nw.rule(
        "psx-exe",
        command=f"mipsel-linux-gnu-objcopy -O binary $in $out",
        description="psx exe $in",
    )
    nw.rule(
        "sym-export",
        command=".venv/bin/python3 tools/symbols.py $in > $out",
        description="sym export $in",
    )
    nw.rule(
        "check",
        command=f"sha1sum -c {check_path}",
        description="check",
    )
    if not generate_report:
        nw.build(
            rule="check",
            outputs=["build/check.dummy"],
            inputs=get_check_list(check_path),
        )
    for ovl in [
        "main",
        # BATTLE
        "batini",
        "battle",
        "brom",
        # MISC
        "dschange",
        "ending",
        # FIELD
        "field",
        # MINI
        # MENU
        "bginmenu",
        "cnfgmenu",
        "savemenu",
        # MAGIC
        "barrier",
    ]:
        add_splat_config(os.path.join(work_dir, f"{ovl}.yaml"))
