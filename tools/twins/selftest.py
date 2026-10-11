#!/usr/bin/env python3
"""Self-test for twins.

Asserts results that were verified by hand against the reference binaries, so
that a regression in the tool shows up as a failure rather than as a
plausible-looking report.

Run it after a build:

    python3 tools/twins/selftest.py

Each case below was checked independently of the tool. They are the ones that
caught real bugs during development, which is why they are here:

  - branch normalisation must rewrite intra-function branch targets. objdump
    masks the top bit of a 26-bit branch field, so RAM 0x800B09C4 prints as
    0xb09c4; a range check against the unmasked value silently marked every
    internal branch as external and the tool still produced output.
  - the RAM-address pattern in .s headers must accept any 0x8xxxxxxx address,
    not just ones starting 800, or every magic overlay is skipped.
  - offsets must be derived and cross-checked, not assumed.
  - D_* and jtbl_* are data, not decompilable functions.
"""

import importlib.util
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))

spec = importlib.util.spec_from_file_location("twins", os.path.join(HERE, "twins.py"))
twins = importlib.util.module_from_spec(spec)
spec.loader.exec_module(twins)

failures = []
checks = 0


def check(name, condition, detail=""):
    global checks
    checks += 1
    if condition:
        print("  ok    %s" % name)
    else:
        print("  FAIL  %s%s" % (name, (": " + detail) if detail else ""))
        failures.append(name)


def read(rel):
    with open(os.path.join(ROOT, rel), "rb") as fh:
        return fh.read()


def overlay_disk(name, config):
    return open(os.path.join(ROOT, config[name]["disk_path"]), "rb").read()


# ---------------------------------------------------------------------------
print("config and offsets")
# ---------------------------------------------------------------------------

config = twins.parse_config(os.path.join(ROOT, "config/us.yaml"))
check("config parses", len(config) > 100, "%d overlays" % len(config))

consts = twins.derive_constants(config, os.path.join(ROOT, "asm/us"))
verified = consts.get("_verified", set())
check(
    "offsets verified against vram_start for most overlays",
    len(verified) >= 100,
    "%d verified" % len(verified),
)
check(
    "magic overlays are verified, not merely assumed",
    "alex" in verified and "hades" in verified,
    "alex/hades in verified=%s" % ("alex" in verified),
)
check(
    "main resolves to vram_start - 0x800",
    consts.get("main") == config["main"]["vram_start"] - 0x800,
    "main=%s" % hex(consts.get("main", 0)),
)
check(
    "overlays resolve to vram_start",
    consts.get("chocobo") == config["chocobo"]["vram_start"],
    "chocobo=%s" % hex(consts.get("chocobo", 0)),
)

# ---------------------------------------------------------------------------
print("\nbranch normalisation")
# ---------------------------------------------------------------------------

hi = overlay_disk("highway", config)
fi = overlay_disk("field", config)
k = consts["highway"]

a = twins.disassemble(hi, 0x800B0754 - k, 0xB5C)
b = twins.disassemble(fi, 0x800ACC5C - consts["field"], 0xB5C)
check("highway func_800B0754 disassembles", len(a) == 727, "%d insns" % len(a))
check("field FieldModelCreatePktsForPart disassembles", len(b) == 727, "%d insns" % len(b))
ca = twins.canonical(a, 0x800B0754, 0x800B0754 - k)
cb = twins.canonical(b, 0x800ACC5C, 0x800ACC5C - consts["field"])
check(
    "the 727-instruction pair is identical after normalisation",
    ca == cb,
    "%d insns differ" % sum(1 for x, y in zip(ca, cb) if x != y),
)
check(
    "internal branches were rewritten, not left as addresses",
    any(op.startswith("R") for _, op in ca),
    "no relative branch found",
)
check(
    "the pair is only equal BECAUSE of normalisation (raw operands differ)",
    [x[2] for x in a] != [x[2] for x in b],
    "raw operands already matched, so normalisation is untested here",
)

# fire is [0, c, fire]: code starts at offset 0 and runs to 0x39C.
fd = overlay_disk("fire", config)
ins = twins.disassemble(fd, 0, 0x39C)
check("fire code region disassembles", len(ins) > 100, "%d insns" % len(ins))
check(
    "fire ends on jr ra at its declared boundary",
    bool(ins) and ins[-2][1] == "jr",
    repr(ins[-2]) if ins else "no insns",
)

# ---------------------------------------------------------------------------
print("\nshared work")
# ---------------------------------------------------------------------------

matched, pending, oor, unread, zeroed, unread, nonmatching, unmatched = twins.collect(
    config, consts, os.path.join(ROOT, "asm/us"), os.path.join(ROOT, "src")
)

check(
    "unmatched functions were found",
    len(pending) > 1000,
    "%d pending" % len(pending),
)


def group_of(overlay, symbol):
    for name, sym, data, off, size, ram, count in pending:
        if name == overlay and sym == symbol:
            key = twins.canonical(twins.disassemble(data, off, size), ram, off)
            return key
    return None


def members_of(key):
    return unmatched.get(key, [])


# the pair verified by hand
gk = group_of("sting", "func_801B0080")
ck = group_of("catastro", "func_801B0080")
check("sting/catastro pair both indexed", gk is not None and ck is not None)
check("sting and catastro share a body", gk is not None and gk == ck)
if gk is not None:
    grp = members_of(gk)
    check("the pair has 2 members", len(grp) == 2, "%d" % len(grp))
    check(
        "it is 650 instructions",
        all(m[2] == 650 for m in grp),
        "%s" % sorted({m[2] for m in grp}),
    )

# the largest group by payoff
best = max(
    (v for v in unmatched.values() if len({m[0] for m in v}) > 1),
    key=lambda v: v[0][2] * len(v),
    default=None,
)
check("a large multi-copy group exists", best is not None)
if best is not None:
    check(
        "its largest member is the 8-copy 624-instruction body",
        len(best) == 8 and best[0][2] == 624,
        "%d copies of %d insns" % (len(best), best[0][2]),
    )
    check(
        "all members agree on instruction count",
        len({m[2] for m in best}) == 1,
        "%s" % sorted({m[2] for m in best}),
    )

groups = [v for v in unmatched.values() if len({m[0] for m in v}) > 1]
check("several shared groups exist", len(groups) >= 20, "%d" % len(groups))
check(
    "magic overlays are represented in the shared groups",
    any("hades" in {m[0] for m in v} for v in groups),
)

# ---------------------------------------------------------------------------
print("\noverlay attribution")
# ---------------------------------------------------------------------------

# func_801B102C exists in both lamu and reazer. lamu's copy is solved and
# reazer's is a stub, so keying the stub index by bare symbol name marked the
# solved one as pending too and reported it as a quick win against itself.
check(
    "a name reused across overlays is attributed to each overlay separately",
    twins.is_pending("func_801B102C", "reazer", nonmatching)
    and not twins.is_pending("func_801B102C", "lamu", nonmatching),
)
check(
    "the solved lamu copy is indexed as matched, not pending",
    not any(
        name == "lamu" and sym == "func_801B102C"
        for name, sym, *_ in pending
    ),
)
check(
    "the stub reazer copy is still indexed as pending",
    any(
        name == "reazer" and sym == "func_801B102C"
        for name, sym, *_ in pending
    ),
)
# every stub should be attributable, or at least most of them; an unattributed
# file falls back to matching any overlay, which is safe but hides mistakes
attributed = sum(1 for owner, _ in nonmatching if owner is not None)
check(
    "stubs are attributed to an overlay",
    attributed > len(nonmatching) * 0.5,
    "%d of %d" % (attributed, len(nonmatching)),
)
by_dir = twins.overlay_owners(config)
check(
    "magic overlays map to the shared src/magic directory",
    len(by_dir.get("magic", [])) > 200,
    "%d overlays" % len(by_dir.get("magic", [])),
)
check(
    "a magic overlay is identified by its file name",
    twins.owning_overlay("magic", "lamu.c", by_dir) == "lamu"
    and twins.owning_overlay("mini/highway", "highway_model.c", by_dir) == "highway",
    "lamu.c->%s highway_model.c->%s"
    % (
        twins.owning_overlay("magic", "lamu.c", by_dir),
        twins.owning_overlay("mini/highway", "highway_model.c", by_dir),
    ),
)
# --find has to cope with the same ambiguity
ambiguous = "func_801B102C"
seen = set()
for name, sym, *_ in pending:
    if sym == ambiguous:
        seen.add(name)
for key, members in matched.items():
    for name, sym, _ in members:
        if sym == ambiguous:
            seen.add(name)
check(
    "the ambiguous name really does span two overlays",
    len(seen) == 2,
    "%s" % sorted(seen),
)

# ---------------------------------------------------------------------------
print("\nsymbol filtering")
# ---------------------------------------------------------------------------

syms = twins.elf_symbols(os.path.join(ROOT, "build/us/comet.elf"))
check(
    "D_ data blobs are excluded from function bodies",
    not any(s.startswith("D_") for s, _, _ in syms),
)
check(
    "jtbl_ switch tables are excluded",
    not any(s.startswith("jtbl_") for s, _, _ in syms),
)
check(
    "real functions are still included",
    any(not s.startswith("D_") and not s.startswith("jtbl_") for s, _, _ in syms),
)

# ---------------------------------------------------------------------------
print("\ncoverage")
# ---------------------------------------------------------------------------

check("no symbol fell outside the disk image", sum(oor.values()) == 0,
      "%d" % sum(oor.values()))
check("zeroed regions are counted as data, not as unread code",
      sum(len(v) for v in zeroed.values()) > 0)
check("every overlay was examined", not unread,
      "%s" % sorted(unread)[:5])

# ---------------------------------------------------------------------------
print()
if failures:
    print("FAILED %d of %d checks:" % (len(failures), checks))
    for f in failures:
        print("  - %s" % f)
    sys.exit(1)
print("all %d checks passed" % checks)
