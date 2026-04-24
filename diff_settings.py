#!/usr/bin/env python3

import glob
import os
import re
import sys


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
        if next_val is None or not os.path.isfile(f"build/us/{next_val}.map"):
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


def estimate_overlay_from_func_name(func_name: str) -> str | None:
    # match symbol definitions but not imports (imports have "= 0x..." after the name)
    matches = []
    pattern = re.compile(
        r"^\s+0x[0-9a-f]+\s+" + re.escape(func_name) + r"\s*$", re.MULTILINE
    )
    for map_path in glob.glob("build/us/*.map"):
        with open(map_path) as f:
            if pattern.search(f.read()):
                ovl = map_path.rsplit("/", 1)[-1].removesuffix(".map")
                matches.append(ovl)
    if len(matches) == 1:
        return matches[0]
    return None


def apply(config, args):
    overlay = args.overlay
    if overlay is None:
        func_name = next((a for a in sys.argv[1:] if not a.startswith("-")), None)
        if func_name:
            overlay = estimate_overlay_from_func_name(func_name)
        if overlay is None:
            overlay = "main"
    config["baseimg"] = "disks/us/SCUS_941.63"
    config["myimg"] = f"build/us/{overlay}.bin"
    config["mapfile"] = f"build/us/{overlay}.map"
    config["source_directories"] = [f"src/{overlay}"]
    config["build_dir"] = "build/"
    config["expected_dir"] = "expected/"
    config["objdump_executable"] = "mipsel-linux-gnu-objdump"
    config["arch"] = "mipsel"
    config["makeflags"] = []
