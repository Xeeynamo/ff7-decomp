# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a decompilation project for Final Fantasy VII (PlayStation 1). The goal is to reverse-engineer the game's binary executable and overlays back into readable C source code that compiles to match the original game binaries byte-for-byte.

## Naming

- `version` refers to different game builds, or game ROMs. The only existing version as of now is `us`
- `overlay` a small programmable unit that can be loaded and unloaded in memory at a specific address
- `base` object files or linked binary from the decompiled source
- `target` object files or linked binary that are known to be matching

## Overlays

Game sections loaded on demand. Each overlay is compiled separately and has its own address space:

- `0x80010000-0x8009FFFF` `main`: game engine
- **battle overlays**: Battle system components
  - `0x800A0000-0x8017FFFF` `brom`: Battle ROM data
  - `0x800A0000-0x8017FFFF` `battle`: Main battle logic (split across different battle files)
  - `0x801B0000-0x801FCFFF` `batini`: Battle initialization
- **field overlays**: Field/map system
  - `0x800A0000-0x800FFFFF` `field`: Main field logic
  - `0x800A0000-0x800FFFFF` `dschange`: Disk change handling
  - `0x800A0000-0x800FFFFF` `ending`: Ending sequences
- **menu overlays**: Menu system
  - `0x801D0000-0x801FCFFF` `savemenu`, `bginmenu`, `cnfgmenu`
- **magic overlays**: Magic effect handlers `0x801B0000-0x801FCFFF`

Configuration for each overlay is in `config/us.yaml`, which defines segments (rodata, code, data, bss), Virtual RAM addresses, symbol files, and disk paths.

## Tooling and structure

- **make build**: builds project
- **make clean**: clean built files
- **make rebuild**: clean and rebuild
- **make format**: format codebase
- **make submit**: stage decompiled functions
- **make build/us/src/battle/battle.c.o**: build specific source file
- **./mako.sh rank <source_path>**: find easier functions to decompile
- **./mako.sh dec <function_name>**: decompile one specific function

### Symbol Management

Symbol addresses are tracked in `config/` directory:

- `symbols.*.txt`: Function/variable symbols per overlay
- `sym_export.us.txt`: Exported symbols from main executable
- `sym_export_battle.us.txt`: Exported symbols from battle system
- `sym_extern.us.txt`: External library symbols

Use `./mako.sh symbols add config/symbols.{overlay}.txt 0x<offset> [size]` to add symbol

### Common File Locations

- Source: `src/<overlay>/<file>.c`
- Headers: `include/` (common.h, game.h) and `include/psxsdk/`
- Private headers, local to overlay: `src/<overlay>/<overlay>_private.h`
- Public headers, shared with other overlays: `src/<overlay>/<overlay>.h`
- Assembly: `asm/us/<overlay>/` (data, nonmatchings, matchings)
- Base build output: `build/us/`
- Target binaries: `expected/build/us/`
- `build/{version}/<overlay>.elf` built overlay in ELF format
- `build/{version}/<overlay>.map` generated map file
- `build/{version}/<overlay>.bin` stripped elf file used to match target binary

### Functions too difficult

`tools/functions_too_difficult` is a plain-text file where each line contains function name of previous attempt. Claude will not attempt to decompile these functions again.

## Must not

- Change order of functions
- Stage changes with `git add`
- Start decompiling next function if `make rebuild` fails
- Must not leave types in C code as `?`
- Strictly follow checklists

## Development Workflow

These steps must be followed in order:

1. `make rebuild` must succeed with zero exit code.
2. `./mako.sh rank <source_file> | grep -vf tools/functions_too_difficult | head -10` finds next function to decompile
3. Decompile Function Checklist must be followed
4. Code Quality Checklist must be followed
5. Call `make submit`

## Decompile Function Checklist

When decompiling a function, this guide must be followed

1. `make rebuild` must succeed with zero exit code
2. Chosen function has `INCLUDE_ASM` and not `#ifndef NON_MATCHINGS`
3. `./mako.sh dec <function_name>` did not write any errorenous code to destination file
4. `.venv/bin/python3 tools/asm-differ/diff.py -mo --overlay <overlay> <function_name> | head <lines>` must be called on each change to track differences on each code iteration following Decompilation Checklist. Output contains assembly code and correspondent C line.
5. Code Quality Checklist must be followed
6. If `make submit` succeeds, then function is successfully decompiled

If `make submit` fails, then mark function as `NON_MATCHING`, `make rebuild` must succeed and function is added to `tools/functions_too_difficult`.

### Decompilation Checklist

Checklist must be followed to get better matches, while iterating changes with `diff.py`:

- [ ] No prototypes or parameters with `?` as type
- [ ] No `void*` parameters that should be typed structs
- [ ] No pointer arithmetic with manual offset calculations
- [ ] Use array indices to access arrays, do not use arithmetic calculations
- [ ] All struct field accesses use `->` or `.` operators
- [ ] Struct sizes match the assembly access patterns
- [ ] `goto loop_*` are converted as `while` loops
- [ ] `goto block_*` in `switch` are inlined, to reverse code optimization
- [ ] Decompilation patterns guide is followed

Alignment is critical. Code and data are aligned by 4-byte.

Continue iterating through changes for a maximum of 30 times. If exceeding, decompilation of function failed.

### Array access pattern

```c
var_a0 = 0;
var_v1 = &D_801518E4->D_80151909;
do {
   var_a0 += 1;
   *var_v1 = (u8)*var_v1 | 0x10;
   var_v1 += 0xB9C;
} while (var_a0 < 0xA);
```

can be re-written as:

```c
for (var_a0 = 0; var_a0 < 10; var_a0++) {
   D_801518E4[var_a0].D_80151909 |= 0x10;
}
```

### Inline variables

```c
var_a0 = 0;
if (condition) {
   var_a0 = 1;
}
func(var_a0);
```

can be re-written as:

```c
if (condition) {
   func(1);
} else {
   func(0);
}
```

### Pointer Arithmetic

When you see pointer arithmetic patterns like `*(type*)((u8*)ptr + offset)`:

1. **Identify the access pattern:**

   - What offset is being accessed? (e.g., `0xC` means field at offset 12)
   - Is it accessing an array element? (e.g., `arg1 * 36` means 36-byte elements)
   - What field within the element? (e.g., `+ 0xA` means field at offset 10)

2. **Create appropriate structs:**

   - Define the element struct with correct size and field offsets
   - Define the container struct with pointer at correct offset
   - Use meaningful names or `unk[Offset]` naming convention

3. **Verify struct sizes:**

   - Calculate total size to ensure it matches the multiplier in pointer arithmetic
   - Example: `arg1 * 36` means struct must be exactly 36 (0x24) bytes

4. **Verify array access with indices**

   When finding patterns such as `*(&D_801518E4->D_80151906 + (D_801590CC * 0xB9C))`,
   if the struct of `D_801518E4` is `0xB9C` bytes long then the snippet can be written
   as `D_801518E4[D_801590CC].D_80151906`

### Struct Modification and Extension

When adding or modifying struct definitions:

- Search headers for matching struct
- Check if other functions access fields at nearby offsets
- Verify ALL affected functions still match after struct changes
- Example: If you add a field at offset 0x14, search for all functions accessing that struct and verify they still compile to the correct offsets

### Explore Similar Functions

Similar functions might share the same pattern of the current decompiling function. Leverage existing decompiled functions for known patterns.

### Use NON_MATCHING for unsuccessful functions

Functions too difficult to compile are not discarded, but ignored at compilation time. Use this pattern to not discard:

```c
#ifndef NON_MATCHING
// TODO comment why it couldn't be matched
// TODO function prototype
INCLUDE_ASM(path, function_name); // restore previous macro
#else
...
function_name() {
   ...
}
#endif
```

## Code Quality Checklist

For each checklist, `make rebuild` must succeed or change must be rolled back.

- [ ] Move function prototype from C code to header
- [ ] Verify new `extern` do not overlap with existing ones defined in headers
- [ ] Move `extern` from C code to header
- [ ] Remove unnecessary type casts (e.g. `(s32)`, `(u16)`, etc.)
- [ ] `while` loops are converted into `for` loops
- [ ] `for` index is renamed to `i`
- [ ] Attempt marking function as `static`
- [ ] Call `make format`
