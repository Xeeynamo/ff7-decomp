#!/usr/bin/env python3
"""twins - find reusable code across overlays in a decompilation project.

Two kinds of sharing exist, and this reports both:

  1. Duplicated function bodies. The same function compiled into two different
     overlays. These are copy-paste-and-go: point at the solved one and the
     other is nearly mechanical.

  2. Call-level reuse. Overlays that do not duplicate any code but call shared
     helpers defined elsewhere. Magic's 200+ effect overlays are almost entirely
     this shape, so a byte-comparison alone reports them as empty.

This is a signposting tool ONLY, not a progress report. It answers "what can I reuse?"
and "where would work be useful?", it says plainly which overlays it could not
examine rather than passing over them.

Read-only: it never writes to src/, config/, or build/.

Offsets
-------
Overlays are not flat images, so file_offset = RAM - K where K differs per
overlay. K is derived from the overlay's own .s headers (each comment carries
"offset RAMADDR encoding") crosschecked against vram_start in the config.
main resolves to vram_start - 0x800 because it is a real PSX exe with a header;
overlays resolve to vram_start itself. Anything that fails the cross-check is
reported, not guessed at.

Normalisation
-------------
Two identical functions sitting at different addresses differ in branch
displacements, because those encode absolute addresses. Intra-function branch
targets are rewritten to an instruction offset from the function start; every
other operand, including external calls and global addresses, is left
byte-exact. Relaxing anything further was tested and found no additional
matches, only a risk of false twins.
"""

import argparse
import collections
import os
import re
import struct
import subprocess
import sys
import tempfile

# ---------------------------------------------------------------------------
# configuration
# ---------------------------------------------------------------------------


def parse_config(path):
    """Return {overlay: {name, disk_path, vram_start}} plus vram_start values."""
    overlays = collections.OrderedDict()
    text = open(path).read()
    for block in re.split(r"\n(?=\s*- name: )", text):
        m = re.match(r"\s*- name:\s*(\w+)", block)
        if not m:
            continue
        name = m.group(1)
        disk = re.search(r"disk_path:\s*(\S+)", block)
        vram = re.search(r"vram_start:\s*(0x[0-9A-Fa-f]+)", block)
        base = re.search(r"base_path:\s*(\S+)", block)
        if not disk:
            continue
        overlays[name] = {
            "disk_path": disk.group(1),
            "vram_start": int(vram.group(1), 16) if vram else None,
            # The source directory this overlay is built from. It equals the
            # overlay name for most overlays but is "magic" for all 275 magic
            # ones, which is what lets a stub be attributed to a single magic
            # overlay instead of to all of them.
            "base_path": base.group(1) if base else name,
        }
    return overlays


def derive_constants(overlays, asm_root):
    """K = RAM - file_offset, derived per overlay from its own .s headers.

    Falls back to vram_start when an overlay has no .s files (the magic
    overlays have data-only extractions). The fallback is still verified
    downstream by confirming symbols land on plausible instructions.
    """
    derived = collections.defaultdict(collections.Counter)
    if os.path.isdir(asm_root):
        for dirpath, _, files in os.walk(asm_root):
            for fn in files:
                if not fn.endswith(".s"):
                    continue
                path = os.path.join(dirpath, fn)
                rel = os.path.relpath(path, asm_root).replace(os.sep, "/")
                parts = rel.split("/")
                owner = None
                for p in parts:
                    if p in overlays:
                        owner = p
                        break
                if owner is None:
                    continue
                try:
                    head = open(path, errors="ignore").read(400)
                except OSError:
                    continue
                m = re.search(
                    r"/\*\s*([0-9A-Fa-f]+)\s+(8[0-9A-Fa-f]{7})\s+[0-9A-Fa-f]{8}\s", head
                )
                if m:
                    off, ram = int(m.group(1), 16), int(m.group(2), 16)
                    derived[owner][ram - off] += 1

    consts = {}
    for name, ov in overlays.items():
        vb = ov["vram_start"]
        counter = derived.get(name)
        if not counter or vb is None:
            consts[name] = vb
            consts.setdefault("_fallback", set()).add(name)
            continue
        k, _ = counter.most_common(1)[0]
        if k == vb or k == vb - 0x800:
            consts[name] = k
            consts.setdefault("_verified", set()).add(name)
        else:
            consts[name] = vb
            consts.setdefault("_fallback", set()).add(name)
    return consts


# ---------------------------------------------------------------------------
# disassembly
# ---------------------------------------------------------------------------

OBJDUMP = ["mips-linux-gnu-objdump", "-D", "-b", "binary", "-m", "mips:3000", "-EL"]
BRANCH = re.compile(
    r"^(j|beq|bne|beqz|bnez|bgez|blez|bltz|bgtz|bgezal|bltzal|beql|bnel)$"
)


def disassemble(data, offset, size):
    """Return [(byte_offset, mnemonic, operands)] or [] if unreadable."""
    if size <= 0 or size % 4 or offset < 0 or offset + size > len(data):
        return []
    fd, tmp = tempfile.mkstemp(suffix=".bin")
    try:
        os.write(fd, data[offset : offset + size])
        os.close(fd)
        out = subprocess.run(
            OBJDUMP + [tmp], capture_output=True, text=True
        ).stdout
    finally:
        try:
            os.unlink(tmp)
        except OSError:
            pass
    result = []
    for line in out.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\t[0-9a-f]{8}\s+(\S+)\s*(.*)$", line)
        if m:
            result.append((int(m.group(1), 16), m.group(2), m.group(3).strip()))
    return result


def canonical(insns, ram, file_offset):
    """Canonical form: intra-function branches become relative instruction offsets.

    objdump prints branch targets with the top bit of the 26-bit field masked
    off (a target of RAM 0x800B09C4 prints as "0xb09c4"), so an intra-function
    target is recognised by masking the printed value to 31 bits and testing it
    against the function's own RAM range. Anything outside stays literal, so
    external calls and global addresses remain byte-exact and a difference
    there is a real difference.
    """
    size = len(insns) * 4
    out = []
    for off, mnem, ops in insns:
        if BRANCH.match(mnem):
            m = re.search(r"0x([0-9a-fA-F]+)$", ops)
            if m:
                printed = int(m.group(1), 16)
                for candidate in (printed | 0x80000000, printed):
                    if ram <= candidate < ram + size:
                        ops = re.sub(
                            r"0x[0-9a-fA-F]+$",
                            "R%d" % ((candidate - ram) // 4),
                            ops,
                        )
                        break
        out.append((mnem, re.sub(r"\s+", " ", ops).strip()))
    return tuple(out)


# ---------------------------------------------------------------------------
# collection
# ---------------------------------------------------------------------------


def overlay_owners(overlays):
    """overlay -> source directory it is built from.

    The directory is base_path, which is not the overlay name for magic: 275
    overlays all build from src/magic. Where one directory holds many overlays
    the file name is what distinguishes them, src/magic/lamu.c being overlay
    lamu, so that mapping is recorded too and consulted per file.
    """
    by_dir = collections.defaultdict(list)
    for name, ov in overlays.items():
        by_dir[ov.get("base_path") or name].append(name)
    return by_dir


def owning_overlay(relpath, filename, by_dir):
    """Which overlay a source file belongs to, or None if it cannot be told.

    relpath is the source file's directory relative to src/. base_path is not
    always a single directory - it is "magic" for 286 magic overlays but
    "mini/highway" for highway - so the path is normalised and matched whole.
    """
    directory = relpath.replace(os.sep, "/")
    candidates = by_dir.get(directory)
    if not candidates:
        return None
    if len(candidates) == 1:
        return candidates[0]
    # Several overlays share one directory, so the file name picks the overlay.
    stem = filename[:-2]  # strip .c
    if stem in candidates:
        return stem
    return None


def include_asm_symbols(src_root, overlays):
    """Every function still waiting to be decompiled, repo-wide.

    Keyed by (overlay, symbol), not by symbol alone. Placeholder names are
    reused across overlays - func_801B102C exists in both lamu and reazer - so a
    bare name set makes a stub in one overlay mark the identically named,
    already-solved function in every other overlay as pending. That reported a
    just-matched function as a quick win against itself.

    A file whose overlay cannot be attributed is keyed under None and matches
    any overlay, which errs toward reporting rather than hiding.
    """
    by_dir = overlay_owners(overlays)
    result = set()
    for dirpath, _, files in os.walk(src_root):
        relpath = os.path.relpath(dirpath, src_root)
        for fn in files:
            if not fn.endswith(".c"):
                continue
            try:
                text = open(os.path.join(dirpath, fn), errors="ignore").read()
            except OSError:
                continue
            owner = owning_overlay(relpath, fn, by_dir)
            for sym in re.findall(r'INCLUDE_ASM\("[^"]*",\s*(\w+)\)', text):
                result.add((owner, sym))
    return result


def is_pending(symbol, overlay, nonmatching):
    """True when this specific overlay's copy of `symbol` is still a stub."""
    return (overlay, symbol) in nonmatching or (None, symbol) in nonmatching


def elf_symbols(elf):
    """[(name, ram, size)] for text symbols that are actually functions.

    Some overlays place large data blobs in a text segment, and nm reports them
    as 'T'. Those show up as D_* and are not decompilable functions, so they
    are excluded rather than reported as absurdly large matches.
    """
    if not os.path.exists(elf):
        return []
    out = subprocess.run(
        ["mips-linux-gnu-nm", "-S", elf], capture_output=True, text=True
    ).stdout
    result = []
    for line in out.splitlines():
        parts = line.split()
        if len(parts) != 4 or parts[2] not in "tT" or int(parts[1], 16) < 16:
            continue
        name, size = parts[3], int(parts[1], 16)
        # D_*/DATA are data blobs parked in a text segment; jtbl_* are switch
        # tables. Neither is a decompilable function.
        if (name.startswith("D_") or name.startswith("_D")
                or "DATA" in name or name.startswith("jtbl_")):
            continue
        result.append((name, int(parts[0], 16), size))
    return result


def elf_imports(elf):
    """Absolute symbols, things this overlay borrows from elsewhere."""
    if not os.path.exists(elf):
        return []
    out = subprocess.run(
        ["mips-linux-gnu-nm", elf], capture_output=True, text=True
    ).stdout
    return [
        parts[2]
        for parts in (l.split() for l in out.splitlines())
        if len(parts) == 3 and parts[1] == "A"
    ]


def collect(overlays, consts, asm_dir, src_root, verbose=False):
    """Walk every overlay; index matched functions and record nonmatchings.

    Two distinct things are not code and are counted separately, because
    lumping them together is misleading: a symbol outside the disk image, and a
    zeroed region. The latter is almost always bss - an uninitialised C variable
    that nm reports as 'T' with a size. lv5deth's 48 KB g_Lv5DeathPrimBuffer0 is
    the clearest example, in an overlay that is itself fully matched.
    """
    nonmatching = include_asm_symbols(src_root, overlays)
    matched = collections.defaultdict(list)  # canonical -> [(overlay, name, insns)]
    pending = []  # (overlay, name, data, offset, size, ram, insn_count)
    out_of_range = collections.Counter()
    unreadable_syms = collections.Counter()
    zeroed = collections.defaultdict(list)  # overlay -> [(symbol, size)]
    unreadable = set()

    for name, ov in overlays.items():
        disk = ov["disk_path"]
        elf = os.path.join("build", "us", "%s.elf" % name)
        if not os.path.exists(disk):
            unreadable.add(name)
            continue
        data = open(disk, "rb").read()
        k = consts.get(name)
        if k is None:
            unreadable.add(name)
            continue
        for sym, ram, size in elf_symbols(elf):
            offset = ram - k
            if offset < 0 or offset + size > len(data):
                out_of_range[name] += 1
                continue
            if not any(data[offset : offset + size]):
                zeroed[name].append((sym, size))
                continue
            insns = disassemble(data, offset, size)
            if not insns:
                unreadable_syms[name] += 1
                continue
            canon = canonical(insns, ram, offset)
            if is_pending(sym, name, nonmatching):
                pending.append((name, sym, data, offset, size, ram, len(insns)))
            else:
                matched[canon].append((name, sym, len(insns)))
        if verbose:
            sys.stderr.write("  indexed %s\n" % name)

    # Two unmatched functions can also be identical to each other. That group is
    # the largest source of reuse in the magic overlays, where nothing is solved
    # yet, so it has to be indexed separately or it stays invisible.
    unmatched = collections.defaultdict(list)
    for name, sym, data, offset, size, ram, count in pending:
        key = canonical(disassemble(data, offset, size), ram, offset)
        unmatched[key].append((name, sym, count))

    return (matched, pending, out_of_range, unreadable_syms, zeroed,
            unreadable, nonmatching, unmatched)


# ---------------------------------------------------------------------------
# reporting
# ---------------------------------------------------------------------------


def report_bodies(matched, pending, limit):
    """Duplicated bodies, largest first. This is the near copy-paste-and-go case. double check"""
    groups = []
    for canon, members in matched.items():
        if len({m[0] for m in members}) < 2:
            continue
        groups.append(members)

    # A nonmatching is a quick win when it is identical to something matched.
    quick = []
    for name, sym, data, offset, size, ram, count in pending:
        insns = disassemble(data, offset, size)
        key = canonical(insns, ram, offset)
        if key in matched:
            helpers = [m for m in matched[key] if m[0] != name]
            if helpers:
                quick.append((count, name, sym, helpers))
    quick.sort(reverse=True, key=lambda r: r[0])

    print("=" * 78)
    print("DUPLICATED FUNCTION BODIES")
    print("=" * 78)
    if quick:
        print()
        print("  Ready to assist: a nonmatching that is identical to a solved function.")
        print("  Copy the solved source, check, adjust the symbol names, build - compare.")
        print()
        print("  %-9s %-24s %6s  %s" % ("overlay", "nonmatching", "insns", "use this to assist"))
        for count, name, sym, helpers in quick[:limit]:
            also = " ".join(
                "%s:%s" % (h[0], h[1]) for h in helpers[:2]
            )
            print("  %-9s %-24s %6d  %s" % (name, sym, count, also))
        print()
        print("  %d nonmatching%s already have a solved twin."
              % (len(quick), "" if len(quick) == 1 else "s"))
    else:
        print("\n  None. No unmatched function is byte-identical to a solved one.")

    if groups:
        print()
        print("  Solved pairs sharing an identical body (shows the sharing map):")
        for members in sorted(groups, key=lambda g: -g[0][2])[:limit]:
            parts = "  ".join("%s:%s" % (m[0], m[1]) for m in members)
            print("    %-4d insns  %s" % (members[0][2], parts))
    print()


def report_dependencies(overlays, limit):
    """What each overlay borrows. The magic overlays are almost all this."""
    rows = []
    for name in overlays:
        elf = os.path.join("build", "us", "%s.elf" % name)
        imports = elf_imports(elf)
        if imports:
            rows.append((len(imports), name, collections.Counter(imports)))
    rows.sort(reverse=True)

    print("=" * 78)
    print("CALL-LEVEL REUSE")
    print("=" * 78)
    print()
    print("  Overlays that do not duplicate code but lean on shared helpers.")
    print("  Before writing one of these, check what already exists.")
    print()
    if rows:
        top = collections.Counter()
        for _, _, counts in rows:
            top.update(counts)
        print("  Most-borrowed symbols across all overlays:")
        for sym, n in top.most_common(15):
            if sym.startswith("__rom") or sym.endswith("_ROM_START"):
                continue
            print("    %-36s used by %d overlays" % (sym, n))
        print()
        print("  Per overlay:")
        for n, name, counts in rows[:limit]:
            head = ", ".join("%s x%d" % (s, c) for s, c in counts.most_common(4))
            print("    %-10s %3d imports   %s" % (name, n, head[:60]))
    print()


def report_shared_unmatched(unmatched, matched, limit):
    """Unmatched functions that are identical to other unmatched functions.

    Worth its own section: in the magic overlays nothing is solved yet, so every
    twin is unmatched-on-both-sides and the "use a solved twin" report is empty
    there. Matching one of these pays off immediately across the whole group.
    """
    groups = []
    for key, members in unmatched.items():
        if len({m[0] for m in members}) < 2:
            continue
        groups.append((members, key in matched))
    groups.sort(key=lambda g: (-g[0][0][2], -len(g[0])))

    print("=" * 78)
    print("SHARED WORK: UNMATCHED FUNCTIONS THAT ARE IDENTICAL TO EACH OTHER")
    print("=" * 78)
    print()
    print("  Neither side is solved yet, but they are the same code. Matching one")
    print("  covers the whole group: write it once, adjust the symbol names, repeat.")
    print()
    if not groups:
        print("  None.")
        print()
        return
    # Sort by payoff (size x copies), not size alone: a 95-instruction function
    # shared by 38 overlays saves more work than a 650-instruction one shared by 2.
    groups.sort(key=lambda g: -(g[0][0][2] * len(g[0])))
    print("  %-6s %-6s %-8s %s" % ("insns", "copies", "work saved", "group"))
    for members, has_solved in groups[:limit]:
        parts = "  ".join("%s:%s" % (m[0], m[1]) for m in members[:4])
        if len(members) > 4:
            parts += "  (+%d more)" % (len(members) - 4)
        print("  %-6d %-6d %-8d %s"
              % (members[0][2], len(members), members[0][2] * len(members), parts))
    covered = sum(len(m) for m, _ in groups)
    saved = sum(m[0][2] * len(m) for m, _ in groups)
    print()
    print("  %d distinct bodies across %d functions. Matching one body of a group"
          % (len(groups), covered))
    print("  covers every copy, so %d instructions of decompilation work cover all"
          % saved)
    print("  %d of those functions." % covered)
    print()


def report_coverage(overlays, consts, pending, out_of_range, unreadable_syms,
                    zeroed, unreadable, verbose):
    print("=" * 78)
    print("COVERAGE")
    print("=" * 78)
    print()
    total = len(overlays)
    built = sum(
        1 for n in overlays if os.path.exists(os.path.join("build", "us", "%s.elf" % n))
    )
    print("  overlays in config        : %d" % total)
    print("  overlays with a built elf : %d" % built)
    per = collections.Counter(p[0] for p in pending)
    print("  unmatched functions seen  : %d, in %d overlays"
          % (len(pending), len(per)))
    print()
    fallback = consts.get("_fallback", set())
    verified = consts.get("_verified", set())
    print("  offset constant verified against vram_start : %d overlays" % len(verified))
    print("  offset constant fell back to vram_start     : %d overlays"
          % len(fallback))
    if fallback:
        print("    (all fully matched already, so they have no nonmatchings to check)")
    print()
    n_oor = sum(out_of_range.values())
    print("  symbols outside the disk image: %d%s"
          % (n_oor, "" if n_oor else "  (every symbol was located)"))
    for name, n in out_of_range.most_common(5):
        print("    %-10s %d  <- NOT read" % (name, n))
    n_un = sum(unreadable_syms.values())
    print("  symbols objdump could not read: %d%s"
          % (n_un, "" if n_un else "  (nothing unreadable)"))
    for name, n in unreadable_syms.most_common(5):
        print("    %-10s %d  <- NOT read" % (name, n))
    if zeroed:
        n = sum(len(v) for v in zeroed.values())
        print()
        print("  zeroed regions skipped: %d, all bss or data rather than code." % n)
        print("  These come from overlays that are matched; nothing was missed.")
        biggest = sorted(
            ((sz, ov, sym) for ov, lst in zeroed.items() for sym, sz in lst),
            reverse=True,
        )[:3]
        for sz, ov, sym in biggest:
            print("    %-10s %-26s %d bytes" % (ov, sym, sz))
    if unreadable:
        print()
        print("  NOT EXAMINED (no disk file or no elf): %s"
              % ", ".join(sorted(unreadable)))
    print()


def find(query, matched, pending):
    """Look up one function and show its family, from either end.

    Asked about an unmatched function, this answers "what can help me?".
    Asked about a solved one, it answers "where would this be useful?".

    Placeholder names are reused across overlays, so a bare name can be
    ambiguous - func_801B102C exists in both lamu and reazer. Accept
    "overlay:symbol" to pick one; given a bare name that matches more than one,
    list every candidate rather than silently reporting the first.
    """
    overlay = None
    if ":" in query:
        overlay, query = query.split(":", 1)
        matches = [(name, sym) for name, sym, *_ in pending if sym == query and name == overlay]
        matches += [
            (name, sym)
            for key, members in matched.items()
            for name, sym, _ in members
            if sym == query and name == overlay
        ]
        if not matches:
            print("no function named %s:%s is known. Is it built? Try --verbose." % (overlay, query))
            return
    else:
        candidates = [(name, sym) for name, sym, *_ in pending if sym == query]
        candidates += [
            (name, sym)
            for key, members in matched.items()
            for name, sym, _ in members
            if sym == query
        ]
        distinct = sorted(set(candidates))
        if len(distinct) > 1:
            print()
            print("  %s is ambiguous, it exists in %d overlays:" % (query, len(distinct)))
            for name, _ in distinct:
                print("    %-9s %s" % (name, query))
            print()
            print("  Ask about one of them as overlay:symbol.")
            return
        if not distinct:
            print("no function named %s is known. Is it built? Try --verbose." % query)
            return
        overlay = distinct[0][0]

    for name, sym, data, offset, size, ram, count in pending:
        if sym == query and name == overlay:
            insns = disassemble(data, offset, size)
            key = canonical(insns, ram, offset)
            report_family(query, overlay, key, matched, pending, solved=False)
            return
    for key, members in matched.items():
        if any(m[0] == overlay and m[1] == query for m in members):
            report_family(query, overlay, key, matched, pending, solved=True)
            return


def report_family(query, overlay, key, matched, pending, solved):
    print()
    print("=" * 78)
    print("  %s  (%s)" % (query, overlay))
    print("=" * 78)
    siblings = matched.get(key, [])
    if solved:
        print()
        print("  This is solved. Anywhere else it appears, the same work applies:")
    else:
        print()
        print("  This is unmatched. Identical, already-solved copies:")
    others = [m for m in siblings if m[1] != query]
    if not others:
        print("    (no other overlay has an identical copy)")
    for o, name, count in others:
        tag = "solved" if True else ""
        print("    %-9s %-30s %5d insns" % (o, name, count))
    # and where this one is still needed
    if solved:
        wanted = [
            (o, n)
            for o, n, d, off, sz, ram, c in pending
            if canonical(disassemble(d, off, sz), ram, off) == key
        ]
        if wanted:
            print()
            print("  Still unmatched elsewhere:")
            for o, n in wanted:
                print("    %-9s %-30s" % (o, n))
    print()


# ---------------------------------------------------------------------------


def main():
    ap = argparse.ArgumentParser(
        description="Find reusable code across overlays: duplicated bodies and "
        "call-level reuse."
    )
    ap.add_argument("--config", default="config/us.yaml")
    ap.add_argument("--asm", default="asm/us")
    ap.add_argument("--src", default="src")
    ap.add_argument("--limit", type=int, default=25)
    ap.add_argument("--only", choices=["bodies", "shared", "deps", "coverage"],
                    help="report just one section")
    ap.add_argument("-v", "--verbose", action="store_true")
    ap.add_argument("--find", metavar="FUNC",
                    help="look up one function and show its twin family, "
                         "either direction: 'what can help me?' or "
                         "'where would my work apply?'. Names reused across "
                         "overlays take OVERLAY:FUNC.")
    args = ap.parse_args()

    if not os.path.exists(args.config):
        sys.exit("no config at %s (run from the repo root)" % args.config)

    overlays = parse_config(args.config)
    consts = derive_constants(overlays, args.asm)
    (matched, pending, out_of_range, unreadable_syms, zeroed,
     unreadable, _, unmatched) = collect(
        overlays, consts, args.asm, args.src, args.verbose
    )

    if args.find:
        find(args.find, matched, pending)
        return

    show = {args.only} if args.only else {"shared", "bodies", "deps", "coverage"}
    if "shared" in show:
        report_shared_unmatched(unmatched, matched, args.limit)
    if "bodies" in show:
        report_bodies(matched, pending, args.limit)
    if "deps" in show:
        report_dependencies(overlays, args.limit)
    if "coverage" in show:
        report_coverage(overlays, consts, pending, out_of_range,
                         unreadable_syms, zeroed, unreadable, args.verbose)


if __name__ == "__main__":
    main()
