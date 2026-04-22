# FF7 Decompilation Automation — Context Handoff

## Project Overview

- **Repo:** `Xeeynamo/ff7-decomp`, branch `automation-attempt`
- **Local path:** `/home/calvin/GitHub/ff7-decomp-armstrca`
- **Goal:** Decompile as much of this FF7 PSX codebase as possible without human intervention, as deterministically as possible.
- **Toolchain:** PSYQ 3.3 (cc1-psx-26 / cc1-psx-35 compiler), m2c (assembly-to-C decompiler), mako.sh wrapper script
- **Build system:** Docker + make (`./tools/docker-build.sh "make build"`)

---

## Key Directories

| Path | Purpose |
|------|---------|
| `automation/` | All automation scripts (Python) |
| `src/` | Decompiled C source (battle/, main/, field/, world/, menu/, etc.) |
| `asm/us/main/` | Assembly source for undecompiled functions (INCLUDE_ASM stubs) |
| `include/` | Header files (common.h, game.h, etc.) |
| `build/us/` | Build artifacts + .yaml overlay config |
| `config/` | Linker scripts, symbol tables |
| `bin/` | Compiler binaries (cc1-psx-26, cc1-psx-272, str, clang-format) |
| `disks/us/` | Game disc contents |

---

## Automation Scripts

| Script | Purpose |
|--------|---------|
| `agent.py` | Main orchestration agent — runs the full loop |
| `decompile.py` | Wraps `mako.sh dec`, validates output, updates DB |
| `build.py` | PSY-Q compile-check + objdiff binary matching |
| `database.py` | SQLite DB wrapper (`decomp.db`) |
| `discover.py` | Scans ASM files to populate the DB queue |
| `dependency_analyzer.py` | Call-graph analysis for prioritization |
| `llm_helper.py` | Ollama LLM interface (requires `qwen2.5-coder:7b`) |
| `fix_existing_errors.py` | TypeErrorFixer — LLM-based PSY-Q error fixing |
| `review_llm_fixes.py` | CLI to approve/reject LLM-proposed fixes |
| `CLAUD.md` | Instructions for using Claude |

---

## Database Status States

```
todo → in_progress → decompiled → verified
                  ↘ failed
                  ↘ blocked
                  ↘ decompiled_needs_refine   ← compile errors after accept
```

---

## PSY-Q Compiler Pipeline (used in `build.py::check_file_compiles`)

```bash
mipsel-linux-gnu-cpp -Iinclude -Iinclude/psxsdk -DUSE_INCLUDE_ASM -DFF7_STR \
  -lang-c -undef -Wall -fno-builtin <rel_path> \
  | bin/str \
  | iconv --from-code=UTF-8 --to-code=Shift-JIS \
  | bin/cc1-psx-26 -quiet -mcpu=3000 -g -mgas -gcoff <cc_flags> > /dev/null
```

Compiler flags (`cc1=` and `cc_flags=`) are read from `build.ninja` per source file.

---

## mako.sh Commands

```bash
./mako.sh dec <func_name>               # Decompile one function
./mako.sh dec <func_name> --fix-structs # Also replace D_8009XXXX with Savemap fields
./mako.sh build                         # Full build
./mako.sh rank <asm_dir>               # Rank functions by difficulty
./mako.sh format                        # clang-format all C files
```

---

## Agent Run Commands

From `automation/` directory:

```bash
# Status only
python3 agent.py --status

# Full agent run (recommended starting point)
python3 agent.py --run --verbose

# Single function smoke test
python3 agent.py --run --target 1 --batch 1 --verbose

# LLM-assisted (requires Ollama + qwen2.5-coder:7b)
python3 agent.py --run --llm --llm-auto-fix --verbose

# Timed run (e.g. 8 hours)
python3 agent.py --run --duration 28800 --batch 20 --llm --llm-auto-fix

# Module-specific
python3 agent.py --run --module battle --target 50 --verbose

# Bootstrap DB from ASM files (run once first)
python3 discover.py
```

---

## Key Implementation Decisions (This Conversation)

### 1. Per-file PSY-Q Compile Gate (`build.py`)
Every decompiled function is now compiled through the real PSY-Q toolchain **before** being committed to `decompiled` status. If it fails, the file is reverted and the function is marked `failed` / `blocked`.

Added methods:
- `BuildVerifier._get_psyq_params(rel_path)` — parses `build.ninja` for cc1 + cc_flags
- `BuildVerifier.check_file_compiles(file_path)` — runs the full CPP→str→iconv→cc1 pipeline, returns `(ok, errors)`

Failure mode: if `build.ninja` doesn't exist or toolchain errors out, returns `(True, [])` to avoid false reverts.

### 2. `--fix-structs` Always On (`decompile.py`, `agent.py`)
`mako.sh dec <func> --fix-structs` replaces `D_8009XXXX` savemap globals with proper `Savemap.field` struct member references. This eliminates an entire class of incompatible-declaration errors.

- `DecompilationRunner.__init__` now takes `fix_structs: bool`
- `run_mako_dec()` appends `--fix-structs` when enabled
- `AgentConfig.fix_structs = True` (always on)

### 3. Startup Cleanup (`agent.py::_cleanup_failure_comments`)
On every agent startup, sweeps all `src/**/*.c` files and removes `/* Decompilation failure: ... */` comment blocks injected by m2c. These blocks corrupt subsequent decompile attempts if left in place.

### 4. Startup Audit (`agent.py::_initial_audit`)
On every agent startup, compiles all C files containing `decompiled` functions. Any function whose line range contains compile errors is downgraded to `decompiled_needs_refine`. This catches bad decompilations from before the compile gate was added.

### 5. `full_build_interval` Lowered
`AgentConfig.full_build_interval = 10` (was 60). Full builds are cheap to run now that per-file checks handle individual errors.

---

## Text-Based Validation Patterns in `decompile.py::validate_c_file`

These heuristics run **before** the PSY-Q compile check:

| Pattern | What it catches |
|---------|----------------|
| `M2C_ERROR` | m2c hit an internal error |
| `saved_reg_ra` | Callee-save register leak (m2c gave up) |
| `Decompilation failure:` | m2c added a failure comment block |
| `#error` | Preprocessor error in output |
| `INCLUDE_ASM` still present after decompile | mako.sh exited 0 but didn't write C |

---

## Previously Fixed Functions

- **`func_80034BB0`** (`src/main/psxsdk.c`): LZSS decompressor — cleaned up m2c output, fully decompiled
- **`func_80034DB0`** (`src/main/psxsdk.c`): Reverted to `INCLUDE_ASM` stub (too complex)

---

## PSX SDK Sentinel Exclusion

Functions in `src/main/psxsdk.c` (and other PSX SDK wrappers) that are too complex or have compiler intrinsics are excluded from the automation queue via a sentinel comment at the top of the file. `discover.py` respects this sentinel and won't add excluded functions to the DB.

---

## Known Issues / Follow-Up Work

1. **`build.ninja` must exist** before `check_file_compiles` works. Run `./tools/docker-build.sh "make build"` once to generate it if it doesn't exist.
2. **CLI `--build-interval` argparse default** is still `100` in `agent.py`'s argparse, while `AgentConfig` default is now `10`. Pass `--build-interval 10` explicitly or update argparse.
3. **`--fix-structs` is not a CLI flag** — it's hardcoded `True` in `AgentConfig`. Can add `--no-fix-structs` flag if needed.
4. **`decompiled_needs_refine` functions** surfaced by `_initial_audit` need either LLM post-fixer or manual revert + re-queue.
5. **Dependency analyzer call graph** takes <1s for 3500+ functions at startup.

---

## Important File Paths

| File | Lines | Notes |
|------|-------|-------|
| `automation/agent.py` | ~1100 | `AgentConfig`, `DecompilationAgent`, `main()` CLI |
| `automation/decompile.py` | ~640 | `DecompilationRunner`, `validate_c_file`, `decompile_function` |
| `automation/build.py` | ~400 | `BuildVerifier`, `check_file_compiles`, `verify_decompiled_functions` |
| `automation/database.py` | ~300 | `DecompDatabase`, SQLite wrapper |
| `automation/discover.py` | ~250 | `DiscoverFunctions`, sentinel exclusion |
| `include/common.h` | - | Core types: s8, s16, s32, u8, u16, u32, Savemap |
| `src/main/psxsdk.c` | - | PSX SDK wrappers, partially decompiled |

---

## Quick Verification

All Python imports should pass:
```bash
cd /home/calvin/GitHub/ff7-decomp-armstrca/automation
python3 -c "from build import BuildVerifier; print('OK')"
python3 -c "from decompile import DecompilationRunner; print('OK')"
python3 -c "from agent import AgentConfig, DecompilationAgent; print('OK')"
```
