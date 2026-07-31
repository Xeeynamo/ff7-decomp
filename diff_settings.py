#!/usr/bin/env python3

import glob
import os
import re
import sys
import yaml


def add_custom_arguments(parser):
    # Peek at argv: if the value after --overlay is a function name (not a real
    # overlay), register it as a flag so argparse doesn't consume the function name.
    # this allows aliases with --overlay burned into it to always work:
    # differ='.venv/bin/python3 tools/asm-differ/diff.py -mows --overlay'
    # allowed: differ main func_8002B958
    # allowed: differ func_8002B958

    overlay_as_flag = False
    if "--overlay" in sys.argv:
        idx = sys.argv.index("--overlay")
        next_val = sys.argv[idx + 1] if idx + 1 < len(sys.argv) else None
        build_path = load_config("us")["build_path"]
        if next_val is None or not os.path.isfile(f"{build_path}/{next_val}.map"):
            overlay_as_flag = True
    if overlay_as_flag:
        parser.add_argument(
            "--overlay",
            action="store_const",
            const=None,
            default=None,
            dest="overlay",
        )
    else:
        parser.add_argument(
            "--overlay",
            default=None,
            dest="overlay",
        )


def load_config(version: str) -> dict:
    with open(f"config/{version}.yaml") as f:
        return yaml.safe_load(f)


def estimate_overlay_from_func_name(func_name: str, build_path: str) -> str | None:
    # match symbol definitions but not imports (imports have "= 0x..." after the name)
    matches = []
    pattern = re.compile(
        r"^\s+0x[0-9a-f]+\s+" + re.escape(func_name) + r"\s*$", re.MULTILINE
    )
    for map_path in glob.glob(f"{build_path}/*.map"):
        with open(map_path) as f:
            if pattern.search(f.read()):
                ovl = map_path.rsplit("/", 1)[-1].removesuffix(".map")
                matches.append(ovl)
    if len(matches) == 1:
        return matches[0]
    return None


def apply(config, args):
    cfg = load_config("us")
    build_path = cfg["build_path"]
    overlays = {ovl["name"]: ovl for ovl in cfg["overlays"]}

    overlay = args.overlay
    if overlay is None:
        func_name = next((a for a in sys.argv[1:] if not a.startswith("-")), None)
        if func_name:
            overlay = estimate_overlay_from_func_name(func_name, build_path)
        if overlay is None:
            overlay = "main"
    if overlay not in overlays:
        raise KeyError(
            f"unknown overlay '{overlay}'\nknown: {', '.join(sorted(overlays))}"
        )
    ovl_cfg = overlays[overlay]
    config["baseimg"] = ovl_cfg["disk_path"]
    config["myimg"] = f"{build_path}/{overlay}.elf"
    config["mapfile"] = f"{build_path}/{overlay}.map"
    config["source_directories"] = [f"{cfg['src_path']}/{ovl_cfg['base_path']}"]
    config["build_dir"] = build_path
    config["expected_dir"] = "expected"
    config["objdump_executable"] = "mipsel-linux-gnu-objdump"
    config["arch"] = "mipsel"
    config["makeflags"] = []
