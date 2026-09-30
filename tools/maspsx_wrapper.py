"""Run tools/maspsx with a local fix for https://github.com/mkst/maspsx/issues/148

When a single `j` separates mflo/mfhi from a following mult/div, maspsx puts
the hazard nop after any label that follows the `j`, instead of in the jump's
delay slot before it as ASPSX does. The instructions come out the same, but a
switch jump table entry for a case starting with a divide then points one
instruction early (e.g. BattleOpcodeMakeMath), preventing a match in the rodata.

Once the submodule includes the upstream fix, delete this file and point
tools/ninja/gen.py back at tools/maspsx/maspsx.py.
"""

import runpy
import sys
from pathlib import Path

MASPSX_DIR = Path(__file__).parent / "maspsx"
sys.path.insert(0, str(MASPSX_DIR))

import maspsx  # noqa: E402

# maspsx already puts the hazard nop straight after a branch, filling its delay
# slot, so treat `j` as one. branch_mnemonics is otherwise only checked where
# `j` is already handled first or alongside jump_mnemonics, so nothing else changes.
maspsx.branch_mnemonics.add("j")

sys.argv[0] = str(MASPSX_DIR / "maspsx.py")
runpy.run_path(sys.argv[0], run_name="__main__")
