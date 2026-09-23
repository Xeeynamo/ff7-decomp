import base64
import os
import pathlib
import sys
from dataclasses import dataclass

import ninja_syntax
import yaml

CPP_FLAGS = "-Iinclude -Iinclude/psxsdk -DUSE_INCLUDE_ASM -DFF7_STR"
LD_FLAGS = ""

nw: ninja_syntax.Writer = None
objs: list[str] = []
work_dir = "build/us"
if len(sys.argv) > 1:
    work_dir = sys.argv[1]
config_path = "config/us.yaml"
if len(sys.argv) > 2:
    config_path = sys.argv[2]
progress_report = os.environ.get("FF7_PROGRESS_REPORT") == "1"
dummy_object = bytes()
if progress_report:
    # https://decomp.wiki/en/tools/decomp-dev
    CPP_FLAGS += " -DSKIP_ASM=1"
    dummy_object = base64.b64decode(
        "f0VMRgEBAQAAAAAAAAAAAAEACAABAAAAAAAAAAAAAABYAAAAABAAADQAAAAAACgABgAFAAAuc2hz"
        "dHJ0YWIALnRleHQALmRhdGEALmJzcwAucGRyAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAALAAAAAQAAAAYAAAAAAAAANAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAEQAA"
        "AAEAAAADAAAAAAAAADQAAAAAAAAAAAAAAAAAAAAQAAAAAAAAABcAAAAIAAAAAwAAAAAAAAA0AAAA"
        "AAAAAAAAAAAAAAAAEAAAAAAAAAAcAAAAAQAAAAAAAAAAAAAANAAAAAAAAAAAAAAAAAAAAAQAAAAA"
        "AAAAAQAAAAMAAAAAAAAAAAAAADQAAAAhAAAAAAAAAAAAAAABAAAAAAAAAA=="
    )  # minimal stripped object file generated from an empty assembly file
check_path = os.path.join(work_dir, "check.sha1")

with open(config_path) as f:
    us_cfg = yaml.load(f, Loader=yaml.SafeLoader)
ovl_by_name = {o["name"]: o for o in us_cfg["overlays"]}


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


def imports_path(name: str) -> str:
    return f"{work_dir}/{name}.imports.txt"


def vram_range(splat_cfg) -> tuple[int, int]:
    code_seg = next(
        s
        for s in splat_cfg["segments"]
        if isinstance(s, dict) and s.get("type") == "code"
    )
    start = code_seg["vram"]
    file_size = next(
        s[0] for s in splat_cfg["segments"] if isinstance(s, list) and len(s) == 1
    )
    end = start + (file_size - code_seg["start"])
    return start, end


@dataclass
class CompilerParams:
    cc1: str
    cc_opt: str
    cc_gp: str
    as_flags: str
    g_opt: str
    gcoff_opt: str
    unroll_opt: str = ""


def default_compiler_params() -> CompilerParams:
    return CompilerParams(
        "cc1-psx-272", "-O2", "-G0", "--expand-div --aspsx-version=2.34", "-g", "-gcoff"
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
                c.as_flags += f" -G{n}"
            except ValueError:
                raise Exception(f"{key} value {value} is not a valid integer")
        elif key == "COMM":
            if value == "true":
                c.as_flags += " --use-comm-section"
            elif value != "false":
                raise Exception(f"{key} value {value} is not a valid boolean")
        elif key == "O":
            try:
                n = int(value)
                c.cc_opt = f"-O{n}"
            except ValueError:
                raise Exception(f"{key} value {value} is not a valid integer")
        elif key == "FORCE_MEM":
            if value == "true":
                c.cc_opt += " -fforce-mem"
            elif value != "false":
                raise Exception(f"{key} value {value} is not a valid boolean")
        elif key == "g":
            if value == "true":
                c.g_opt = "-g"
            elif value == "false":
                c.g_opt = ""
            else:
                raise Exception(f"{key} value {value} is not a valid boolean")
        elif key == "gcoff":
            if value == "true":
                c.gcoff_opt = "-gcoff"
            elif value == "false":
                c.gcoff_opt = ""
            else:
                raise Exception(f"{key} value {value} is not a valid boolean")
        elif key == "UNROLL":
            if value == "true":
                c.unroll_opt = "-funroll-loops"
            elif value == "false":
                c.unroll_opt = ""
            else:
                raise Exception(f"{key} value {value} is not a valid boolean")
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


def add_s(cfg: any, file_name: str, is_hasm=False, implicit: list[str] = []):
    if is_hasm:
        in_path = f"{src_path(cfg)}/{file_name}.s"
    else:
        in_path = f"{asm_path(cfg)}/{file_name}.s"
    out_path = f"{build_path(cfg)}/{in_path}.o"
    if out_path in objs:
        return
    objs.append(out_path)
    if progress_report and not is_hasm:
        out = pathlib.Path(out_path)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_bytes(dummy_object)
        return
    nw.build(
        rule=f"{platform(cfg)}-as",
        outputs=[out_path],
        inputs=[in_path],
        implicit=implicit,
    )
    if not is_hasm:
        nw.build(
            rule="phony",
            outputs=[in_path],
            implicit=[ld_path(cfg)],
        )


def add_s_as(cfg: any, in_path: str, out_path: str):
    if out_path in objs:
        return
    objs.append(out_path)
    nw.build(
        rule=f"{platform(cfg)}-as",
        outputs=[out_path],
        inputs=[in_path],
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
            "cc_flags": f"{compiler_flags.cc_opt} {compiler_flags.cc_gp} {compiler_flags.g_opt} {compiler_flags.gcoff_opt}"
            + (f" {compiler_flags.unroll_opt}" if compiler_flags.unroll_opt else ""),
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


def add_asset(cfg: any, name: str, steps: list[dict]):
    outputs = []
    for step in steps:
        nw.build(
            rule="asset",
            outputs=step["outputs"],
            inputs=step["inputs"],
            variables={"cmd": step["command"]},
        )
        outputs += step["outputs"]
    add_s(cfg, f"data/{name}", implicit=outputs)


def add_splat_config(ovl_name: str, file_name: str):
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
    if platform(cfg) == "psx" and is_main:
        add_s(cfg, "header")
        add_s_as(
            cfg,
            f"{src_path(cfg)}/common.s",
            f"{build_path(cfg)}/{asm_path(cfg)}/data/common.sbss.s.o",
        )
    for segment in cfg["segments"]:
        if not "type" in segment:
            continue
        if segment["type"] != "code":
            continue
        for sub in segment["subsegments"]:
            if isinstance(sub, dict):
                kind = str(sub.get("kind", sub["type"]))
                name = str(sub["name"])
            else:
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
            elif kind == "sbss":
                add_s(cfg, f"data/{name}.sbss")
            elif kind == "asm":
                add_s(cfg, name)
            elif kind == "hasm":
                add_s(cfg, name, True)
            elif kind == "c" or kind == ".data":
                add_c(cfg, name)
            elif isinstance(sub, dict) and "kind" in sub:
                add_asset(cfg, name, sub.get("build") or [])

    ovl = ovl_by_name[ovl_name]
    import_names = ovl.get("imports") or []
    own_files = cfg["options"]["symbol_addrs_path"]
    if import_names:
        own_files = own_files[:-1]
    import_files = [
        f for imp in import_names for f in ovl_by_name[imp]["symbol_addrs_path"]
    ]

    if import_names:
        assert imports_path(ovl_name) in cfg["options"]["symbol_addrs_path"]
        vram_start, vram_end = vram_range(cfg)
        nw.build(
            rule="sym-imports",
            outputs=[imports_path(ovl_name)],
            inputs=own_files + import_files,
            variables={
                "range": f"--vram-start 0x{vram_start:X} --vram-end 0x{vram_end:X}",
                "own": " ".join(f"--own {p}" for p in own_files),
                "imports": " ".join(f"--import {p}" for p in import_files),
            },
        )

    if progress_report:
        return

    output_name = f"{build_path(cfg)}/{basename(cfg)}.elf"
    syms_ld_path = f"{build_path(cfg)}/{basename(cfg)}.syms.ld"
    nw.build(
        rule="sym-ld",
        outputs=[syms_ld_path],
        inputs=list(objs),
        implicit=own_files + import_files + [ld_path(cfg)],
        variables={
            "own": " ".join(f"--own {p}" for p in own_files),
            "imports": " ".join(f"--import {p}" for p in import_files),
        },
    )
    nw.build(
        rule="psx-ld",
        outputs=[output_name],
        inputs=[ld_path(cfg)],
        implicit=objs + [syms_ld_path],
        variables={
            "map_path": f"{build_path(cfg)}/{basename(cfg)}.map",
            "obj_paths": objs,
            "symbol_path": f"-T {cfg["options"]["undefined_syms_auto_path"]} -T {syms_ld_path}",
        },
    )
    nw.build(
        rule="psx-exe",
        outputs=[f"{build_path(cfg)}/{basename(cfg)}.exe"],
        inputs=[output_name],
    )


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
            " | bin/$cc1 -quiet -mcpu=3000 -mgas $cc_flags"
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
        "asset",
        command="$cmd $in $out",
        description="asset $in",
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
        "sym-imports",
        command=".venv/bin/python3 tools/symbols.py splat-imports -o $out $range $own $imports",
        description="sym imports $out",
        restat=True,
    )
    nw.rule(
        "sym-ld",
        command=".venv/bin/python3 tools/symbols.py ld -o $out $own $imports $in",
        description="sym ld $out",
        restat=True,
    )
    nw.rule(
        "check",
        command=f"sha1sum -c {check_path}",
        description="check",
    )
    if not progress_report:
        nw.build(
            rule="check",
            outputs=["build/check.dummy"],
            inputs=[
                line.strip().split(" ")[2]
                for line in open(check_path, "r").readlines()
                if line
            ],
        )
    for ovl_cfg in us_cfg["overlays"]:
        name = ovl_cfg["name"]
        add_splat_config(name, os.path.join(work_dir, f"{name}.yaml"))
