#!/usr/bin/env python3
"""Run `./mako.sh rank <module>` for each module and combine results into a Markdown file."""

import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
MAKO = REPO_ROOT / "mako.sh"
OUTPUT = REPO_ROOT / "automation" / "rank_all.md"

def discover_modules() -> list[str]:
    asm_dir = REPO_ROOT / "asm" / "us"
    return sorted(p.name for p in asm_dir.iterdir() if p.is_dir())


def rank_module(module: str) -> list[str]:
    result = subprocess.run(
        [str(MAKO), "rank", module],
        capture_output=True,
        text=True,
        cwd=REPO_ROOT,
    )
    if result.returncode != 0:
        print(f"  WARNING: rank failed for {module}: {result.stderr.strip()}", file=sys.stderr)
        return []
    lines = [l for l in result.stdout.splitlines() if l.strip()]
    return lines


def main() -> None:
    modules = discover_modules()
    sections: list[str] = ["# Function Rank (easiest → hardest)\n"]

    for module in modules:
        print(f"Ranking {module}...")
        lines = rank_module(module)
        sections.append(f"## {module}\n")
        if lines:
            rows = ["| Score | Function |", "| --- | --- |"]
            for line in lines:
                parts = line.split(": ", 1)
                if len(parts) == 2:
                    score, func = parts
                    rows.append(f"| {score} | {func} |")
                else:
                    rows.append(f"| — | {line} |")
            sections.append("\n".join(rows))
        else:
            sections.append("_No non-decompiled functions._")
        sections.append("")

    output = "\n".join(sections)
    OUTPUT.write_text(output, encoding="utf-8")
    print(f"\nWrote {OUTPUT}")


if __name__ == "__main__":
    main()
