#!/usr/bin/env python3
"""Build and debug a single FF7 overlay as a standalone PsyZ executable.

The PsyZ build compiles the overlays but links only psyz/main/menu/misc into
`ff7`; everything else lands in libff7.a with nothing calling it, so an overlay
can compile and still have no way to run.

This closes that gap. It works out what one overlay is still missing, writes PC
stand ins for those symbols, emits a C harness that calls the overlay's entry
point directly, and generates a cmake target linking just those. The result
goes straight into one minigame instead of driving the whole game to reach it.

Commands (run `help <command>` for detail):
  help [cmd]        this, or detail for one command
  list              overlays, how complete, and whether they can run
  probe   <ovl>     symbols the overlay is still missing
  gen     <ovl>     write stubs, harness and cmake
  build   <ovl>     gen, then compile
  run     <ovl>     build, then launch
  debug   <ovl>     dump resolved paths, flags and entry point, build nothing
  clean   [ovl]     remove generated files and the build dir

Debug options (accepted by run, and compiled in by gen/build):
  --frame-step      pause after every frame, Enter to advance, q to quit
  --break-on-stub   abort the moment an unimplemented stub is reached
  --log-stubs       log each stub hit with a running count
  --trace           verbose harness logging to stderr
  --gdb             launch under gdb with a backtrace on fault
"""
import argparse, os, shutil, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GEN_CMAKE = os.path.join(ROOT, 'tools', 'overlay_runner.cmake')
INCLUDE_LINE = 'include("${CMAKE_CURRENT_SOURCE_DIR}/tools/overlay_runner.cmake" OPTIONAL)'
DEFAULT_BUILD = os.path.join(ROOT, 'build', 'overlay')

# Entry point the game calls each overlay through, found in src/main/110B8.c.
# load = (lba symbol, sector count) from that file's Yamada table; the overlay
# image is read to 0x80180000 and unpacked to 0x800A0000 exactly as the game does.
# Generated sources live here, never in src/pc itself: src/pc/stubs_condor.c
# is a hand written file and a generator must not be able to reach it.
GENDIR = 'src/pc/ovlrun'

OVERLAYS = {
  'jet':     dict(src='src/mini/jet',     entry='MINI_Jet',     ret='u16',
                  lba='LBA_MINI_JET',     sectors=14067),
  'chocobo': dict(src='src/mini/chocobo', entry='MINI_Chocobo', ret='void',
                  lba='LBA_MINI_CHOCOBO', sectors=36521),
  'condor':  dict(src='src/mini/condor',  entry='func_800B6B58', ret='void',
                  lba='LBA_MINI_CONDOR',  sectors=39600),
  'highway': dict(src='src/mini/highway', entry='func_800A00D0', ret='void',
                  lba='LBA_MINI_HIGHWAY', sectors=34138),
  # WORLD_Main takes the four values 110B8.c passes it at the dispatch.
  'world':   dict(src='src/world',        entry='WORLD_Main',   ret='s32',
                  lba='LBA_WORLD_WORLD',  sectors=66715,
                  decl='s32 WORLD_Main(s32*, s32*, s32*, s32)',
                  args='&w_exit, &w_field, &w_flags, 0',
                  locals='s32 w_exit = 0, w_field = 0, w_flags = 0;'),
  'magic':   dict(src='src/magic',        entry=None, ret=None, lba=None, sectors=0),
}

# Only used when nix is present. Everywhere else the tool just uses the cmake,
# ninja and C compiler already on PATH, so the hard requirements are python3
# (stdlib only), cmake, a generator and a C compiler.
NIX = ['gcc13','cmake','ninja','pkg-config','zlib','wayland','wayland-protocols',
       'wayland-scanner','libxkbcommon','libGL','mesa','libdecor','dbus',
       'alsa-lib','pipewire','vulkan-loader','gdb']

VERBOSE = False
IS_WIN = sys.platform.startswith('win')
IS_MAC = sys.platform == 'darwin'

def have(x): return shutil.which(x) is not None

def use_nix():
    """nix only when it is actually there and we are not on win/mac."""
    return (not IS_WIN and not IS_MAC and have('nix-shell')
            and os.environ.get('OVERLAY_RUNNER_NO_NIX') is None)

def log(*a):
    if VERBOSE: print("[overlay_runner]", *a, file=sys.stderr)

def shq(s): return "'" + s.replace("'", "'\\''") + "'"

def sh(cmd, capture=True):
    log("sh:", cmd)
    return subprocess.run(cmd, shell=True, cwd=ROOT, text=True,
                          capture_output=capture)

def in_env(cmd, capture=True):
    """Run a build command in whatever environment this machine provides."""
    if use_nix():
        return sh("nix-shell -p %s --run %s" % (' '.join(NIX), shq(cmd)), capture)
    return sh(cmd, capture)

def entry_is_asm(ov):
    """True when the overlay's entry point is still INCLUDE_ASM. Then there is
    nothing to run: the base stub in src/pc/stubs.c answers the call and
    returns, which looks like a clean exit and is not one."""
    e = OVERLAYS[ov].get('entry')
    if not e:
        return False
    import re
    pat = re.compile(r'INCLUDE_ASM\([^)]*\b%s\s*\)' % re.escape(e))
    return any(pat.search(open(os.path.join(ROOT, f)).read()) for f in sources(ov))


def need(ov):
    if ov not in OVERLAYS:
        sys.exit("unknown overlay %r. known: %s" % (ov, ', '.join(sorted(OVERLAYS))))
    if not os.path.isdir(os.path.join(ROOT, OVERLAYS[ov]['src'])):
        sys.exit("no such directory: %s" % OVERLAYS[ov]['src'])
    return OVERLAYS[ov]

def sources(ov):
    d = os.path.join(ROOT, OVERLAYS[ov]['src'])
    return sorted('%s/%s' % (OVERLAYS[ov]['src'], f)
                  for f in os.listdir(d) if f.endswith('.c'))

def asm_left(ov):
    return sum(sum(1 for l in open(os.path.join(ROOT, s)) if 'INCLUDE_ASM(' in l)
               for s in sources(ov))

# ------------------------------------------------------------------ codegen

HARNESS = r'''/* Generated by tools/overlay_runner.py for the %(ov)s overlay. Do not edit.
 *
 * Replaces src/pc/main.c for this target. It does the same psyz bring up, then
 * loads the overlay image off the disc the way src/main/110B8.c does and calls
 * the entry point straight away, instead of driving the game to that state.
 *
 * Runtime switches, all read from the environment so one binary covers them:
 *   FF7_FRAME_STEP=1   pause after each frame, Enter advances, q quits
 *   FF7_TRACE=1        log bring up and every frame to stderr
 *   FF7_STUB_BREAK=1   abort the moment an unimplemented stub is reached
 *   FF7_STUB_LOG=1     log each stub hit with a running count
 */
#include <game.h>
#include <libapi.h>
#include <libetc.h>
#include <libgpu.h>
#include <libgte.h>
#include <libspu.h>
%(incs)s#include <psyz/audio.h>
#include <psyz/dbgserver.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/mman.h>
#endif

#define DISK_CUE "disks/Final Fantasy VII (USA) (Disc 1).cue"

int g_OvlFrameStep, g_OvlTrace, g_OvlStubBreak, g_OvlStubLog, g_OvlFrame;

/* The overlays dereference psx addresses directly (0x800A0000, 0x80180000),
 * so those have to be real memory in this process. Same reservation
 * src/pc/main.c makes, spelled for each platform. */
static void ReserveAddrRam(uintptr_t addr, size_t size) {
#if defined(_WIN32)
    void* p = VirtualAlloc((LPVOID)addr, size, MEM_COMMIT | MEM_RESERVE,
                           PAGE_READWRITE);
    if (p == NULL) {
#else
    void* p = mmap((void*)addr, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (p == MAP_FAILED) {
#endif
        fprintf(stderr, "harness: could not reserve psx ram at %%p\n", (void*)addr);
        exit(1);
    }
}

/* Called by the generated stubs. */
void OvlStubHit(const char* name) {
    static int n;
    n++;
    if (g_OvlStubLog || g_OvlStubBreak)
        fprintf(stderr, "harness: STUB #%%d %%s\n", n, name);
    if (g_OvlStubBreak) {
        fprintf(stderr, "harness: FF7_STUB_BREAK set, aborting at first stub\n");
        abort();
    }
}

/* Called from the frame loop so --frame-step works without a debugger. */
void OvlFrameMark(void) {
    g_OvlFrame++;
    if (g_OvlTrace) fprintf(stderr, "harness: frame %%d\n", g_OvlFrame);
    if (g_OvlFrameStep) {
        int c;
        fprintf(stderr, "harness: frame %%d, Enter to step, q to quit: ", g_OvlFrame);
        c = getchar();
        if (c == 'q') exit(0);
    }
}

/* The overlay's own sources are compiled with VSync redirected here, so the
 * frame hook sits in the overlay under test and nowhere else. One VSync per
 * frame is what every overlay's loop does. */
int OvlVSync(int mode) {
    OvlFrameMark();
    return VSync(mode);
}

%(decl)s;

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    g_OvlFrameStep = getenv("FF7_FRAME_STEP") != NULL;
    g_OvlTrace     = getenv("FF7_TRACE") != NULL;
    g_OvlStubBreak = getenv("FF7_STUB_BREAK") != NULL;
    g_OvlStubLog   = getenv("FF7_STUB_LOG") != NULL;

    if (g_OvlTrace) fprintf(stderr, "harness: overlay=%(ov)s entry=%(entry)s\n");
    Psyz_DebugServer(8081);
    ReserveAddrRam(0x80010000, 0x1F0000);
    ReserveAddrRam(0x1F800000, 0x400);
    if (Psyz_CdSetDiskPath(DISK_CUE) < 0) {
        fprintf(stderr, "harness: cannot open %%s (run from the repo root)\n", DISK_CUE);
        return 1;
    }
    if (Psyz_AudioInit() < 0) fprintf(stderr, "harness: no audio device, continuing\n");

    // What SysInitBase does before any overlay runs. It is static in
    // src/main/110B8.c so it cannot be called, and without it the first
    // SetDispMask in an overlay goes through an uninitialised GPU and
    // segfaults. The game's vsync callback is left out on purpose: it only
    // drives the save timer and the fades.
    SetMem(8);
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    SpuInit();
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
    SysCdromInit();
    if (g_OvlTrace) fprintf(stderr, "harness: graphics and cdrom up\n");

%(load)s
    if (g_OvlTrace) fprintf(stderr, "harness: calling %(entry)s\n");
%(locals)s    %(entry)s(%(args)s);
    if (g_OvlTrace) fprintf(stderr, "harness: %(entry)s returned after %%d frames\n", g_OvlFrame);
    return 0;
}
'''

LOAD_TMPL = """    if (g_OvlTrace) fprintf(stderr, "harness: loading overlay image\\n");
    SystemLoadFileBySector(%(lba)s, %(sectors)d, (u_long*)0x80180000, NULL);
    while (SystemCdromReadChain()) {
    }
    SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
"""

def write_harness(ov):
    o = OVERLAYS[ov]
    load = (LOAD_TMPL % dict(lba=o['lba'], sectors=o['sectors'])) if o['lba'] else \
           "    /* no disc image for this overlay */\n"
    txt = HARNESS % dict(ov=ov, entry=o['entry'], ret=o['ret'] or 'void', load=load,
                     incs=ovl_includes(ov, main_private=True),
                     decl=o.get('decl') or 'extern %s %s(void)' % (
                         o['ret'] or 'void', o['entry']),
                     args=o.get('args', ''),
                     locals=('    %s\n' % o['locals']) if o.get('locals') else '')
    os.makedirs(os.path.join(ROOT, GENDIR), exist_ok=True)
    p = os.path.join(ROOT, GENDIR, 'harness_%s.c' % ov)
    open(p, 'w').write(txt)
    log("wrote", p)
    return '%s/harness_%s.c' % (GENDIR, ov)

def write_cmake(ov, probe_only, extra=(), stub_renames=()):
    """Self contained so it can be included last, which is why it repeats the
    strd custom command instead of joining FF7_ALL earlier in the file."""
    srcs = list(sources(ov)) + list(extra)
    tgt = 'ovl_%s' % ov
    L = ["# Generated by tools/overlay_runner.py. Do not edit.",
         'set(OVL_INC "${CMAKE_CURRENT_BINARY_DIR}/ovlinc/%s")' % ov,
         'file(MAKE_DIRECTORY "${OVL_INC}")']
    for name, target in sorted(shims(ov).items()):
        L.append('file(WRITE "${OVL_INC}/%s" "#include <%s>\\n")' % (name, target))
    L += [
         "set(OVL_SRC %s)" % ' '.join(srcs),
         "unset(OVL_STRD)",
         "foreach(src ${OVL_SRC})",
         '  set(strd "${CMAKE_CURRENT_BINARY_DIR}/strd_ovl/%s/${src}")' % ov,
         "  get_filename_component(sd ${strd} DIRECTORY)",
         "  get_filename_component(od ${src} DIRECTORY)",
         "  add_custom_command(OUTPUT ${strd}",
         '    COMMAND ${CMAKE_COMMAND} -E make_directory "${sd}"',
         "    COMMAND $<TARGET_FILE:str> < ${CMAKE_CURRENT_SOURCE_DIR}/${src} > ${strd}",
         "    DEPENDS ${src} str)",
         '  set_source_files_properties(${strd} PROPERTIES INCLUDE_DIRECTORIES'
         ' "${CMAKE_CURRENT_SOURCE_DIR}/${od};${OVL_INC}")',
         "  list(APPEND OVL_STRD ${strd})",
         "endforeach()",
         # Only the overlay's own translation units get the frame hook, so it
         # cannot perturb the base or the generated files.
         "foreach(src %s)" % ' '.join(sources(ov)),
         '  set_source_files_properties('
         '"${CMAKE_CURRENT_BINARY_DIR}/strd_ovl/%s/${src}" PROPERTIES'
         ' COMPILE_DEFINITIONS "VSync=OvlVSync")' % ov,
         "endforeach()"]
    if probe_only:
        L += ["add_library(%s OBJECT ${OVL_STRD})" % tgt, "ff7_configure(%s)" % tgt,
              'target_include_directories(%s PRIVATE "${OVL_INC}")' % tgt]
    else:
        # src/pc/main.c is not only main(): it carries the PC stand ins for
        # SysCdromGetPackPointer, func_80014804 and friends that the rest of
        # the base links against. So take our own copy of it with main renamed
        # the way src/main/110B8.c already renames the game's, and drop the
        # base copy, leaving main() to the harness.
        L += ['set_source_files_properties('
              '"${CMAKE_CURRENT_BINARY_DIR}/strd_ovl/%s/src/pc/main.c"'
              ' PROPERTIES COMPILE_DEFINITIONS "main=ff7_pc_main_unused")' % ov]
        if stub_renames:
            # src/pc/stubs.c holds placeholders for the overlays the PC build
            # does not have. The real overlay defines them for real, so define
            # the placeholder out of our copy rather than the whole file, which
            # still has the other overlays' placeholders in it.
            L.append('set_source_files_properties('
                     '"${CMAKE_CURRENT_BINARY_DIR}/strd_ovl/%s/src/pc/stubs.c"'
                     ' PROPERTIES COMPILE_DEFINITIONS "%s")'
                     % (ov, ';'.join('%s=%s_base_stub_unused' % (x, x)
                                     for x in sorted(stub_renames))))
        L += ["set(OVL_BASE ${FF7_STR_PSYZ})",
              'list(FILTER OVL_BASE EXCLUDE REGEX "src/pc/(main|stubs)\\\\.c$")',
              "add_executable(%s ${OVL_BASE} ${FF7_STR_MAIN} ${FF7_STR_MENU}"
              " ${FF7_STR_MISC} ${OVL_STRD})" % tgt,
              "ff7_configure(%s)" % tgt,
              'target_include_directories(%s PRIVATE "${OVL_INC}")' % tgt]
    open(GEN_CMAKE, 'w').write("\n".join(L) + "\n")
    log("wrote", GEN_CMAKE)
    return tgt

def ensure_include():
    p = os.path.join(ROOT, 'CMakeLists.txt'); s = open(p).read()
    if INCLUDE_LINE in s: return False
    open(p, 'w').write(s.rstrip() + "\n\n" + INCLUDE_LINE + "\n"); return True

def generator():
    return "Ninja" if have('ninja') else ("NMake Makefiles" if IS_WIN else "Unix Makefiles")

def cc_wrapper(bd):
    """upstream's 110B8.c trips its own -Werror=stringop-overflow on gcc 13+, and
    target_compile_options beat CMAKE_C_FLAGS, so the warning has to be relaxed
    at the driver. On Windows there is no shell wrapper, so skip it and let the
    build fail loudly if the compiler objects."""
    os.makedirs(bd, exist_ok=True)
    if IS_WIN:
        return None
    if use_nix():
        base = sh("nix eval --raw nixpkgs#gcc13.outPath").stdout.strip() + "/bin/gcc"
    else:
        base = shutil.which('cc') or shutil.which('gcc') or shutil.which('clang')
        if not base:
            sys.exit("no C compiler found on PATH (need cc, gcc or clang)")
    p = os.path.join(bd, 'cc-relaxed')
    open(p,'w').write("#!/bin/sh\nexec %s \"$@\" -Wno-error=stringop-overflow "
                      "-Wno-error=array-bounds -Wno-error=stringop-overread\n" % base)
    os.chmod(p, 0o755); return p

def cmake_build(tgt, bd):
    if not have('cmake') and not use_nix():
        sys.exit("cmake not found on PATH")
    cc = cc_wrapper(bd)
    ccflag = (" -DCMAKE_C_COMPILER=%s" % cc) if cc else ""
    r = in_env('cmake -B %s -G "%s"%s %s' % (bd, generator(), ccflag, ROOT))
    if r.returncode:
        print(r.stdout[-2500:] or r.stderr[-2500:]); return False
    r = in_env("cmake --build %s -j 12 --target %s" % (bd, tgt))
    if r.returncode:
        out = (r.stdout or '') + (r.stderr or '')
        errs = [l for l in out.splitlines()
                if ' error' in l or 'undefined reference' in l or 'FAILED' in l]
        print("\n".join(errs[:25]) or out[-2500:]); return False
    return True

# ---------------------------------------------------------------- symbols

def ident(name):
    """A real C identifier. nm also prints compiler locals (.LC0, __func__.0,
    JetPathSample.constprop.0) and blank lines between objects."""
    import re
    return bool(re.match(r'^[A-Za-z_][A-Za-z0-9_]*$', name))


def nm_undefined(bd, tgt, ov=None):
    """Objects are derived from the overlay's own sources, not globbed. A glob
    picks up whatever a previous full build left under the same target dir, and
    main.c's object drags the whole of libc in as undefined."""
    objs = []
    if ov:
        for src in sources(ov):
            o = os.path.join(bd, 'CMakeFiles', '%s.dir' % tgt, 'strd_ovl', ov,
                             src + '.o')
            if os.path.exists(o): objs.append(o)
    if not objs:
        objs = sh("find %s -path '*%s.dir*' -name '*.o'" % (bd, tgt)).stdout.split()
    if not objs: return None, [], set()
    u = sh("nm -u %s 2>/dev/null | awk '{print $2}' | sort -u" % ' '.join(objs))
    # nm -u is per object, so a symbol one overlay file defines and another
    # references still shows up. Subtract what the overlay itself defines.
    d = sh("nm --defined-only %s 2>/dev/null | awk '{print $3}' | sort -u"
           % ' '.join(objs))
    mine = set(x for x in d.stdout.split() if ident(x))
    return objs, [x for x in u.stdout.split() if ident(x) and x not in mine], mine

def base_stub_defined(bd):
    """What src/pc/stubs.c defines for the base build. Those are placeholders
    for overlays the PC build does not have, so each one the real overlay also
    defines is a duplicate at link time."""
    o = os.path.join(bd, 'CMakeFiles', 'ff7.dir', 'strd', 'src', 'pc', 'stubs.c.o')
    if not os.path.exists(o): return set()
    r = sh("nm --defined-only --extern-only %s 2>/dev/null | awk '{print $3}'" % o)
    return set(x for x in r.stdout.split() if ident(x))


def base_defined(bd):
    """Symbols the psyz/main/menu/misc half already provides."""
    objs = sh("find %s -name '*.o' -not -path '*ovl_*'" % bd).stdout.split()
    if not objs: return set()
    d = sh("nm --defined-only %s 2>/dev/null | awk '{print $3}' | sort -u" % ' '.join(objs))
    return set(x for x in d.stdout.split() if x)

# Supplied by libc or the toolchain at link time, never by us.
TOOLCHAIN = {'memset','memcpy','memmove','memcmp','strlen','strcpy','strncpy',
             'strcmp','strncmp','strcat','sprintf','snprintf','printf','fprintf',
             'puts','putchar','fwrite','fread','fopen','fclose','getc','getenv',
             'setvbuf','stdin','stdout','stderr','abort','exit','malloc','calloc',
             'realloc','free','sqrt','sqrtf','rand','srand','atan2','sin','cos',
             '_GLOBAL_OFFSET_TABLE_'}


# Provided by the generated harness, so never stubbed.
HARNESS_SYMS = {'OvlVSync', 'OvlStubHit', 'OvlFrameMark'}


def toolchain_sym(name):
    """Anything libc or the compiler provides. The __ prefixed ones are the
    _FORTIFY_SOURCE variants (__sprintf_chk and friends)."""
    return (name in TOOLCHAIN or name in HARNESS_SYMS
            or name.startswith('__'))

def shims(ov):
    """Bare quoted includes an overlay uses that only exist in include/psxsdk.
    That directory stays off the PC path on purpose: its types.h redefines the
    stdint types when __psyz is not defined, and __psyz is private to the psyz
    target. So forward the individual names instead of adding the directory.
    types.h goes to psyz's own, everything else to the psxsdk copy, which is
    already guarded on PLATFORM_PSYZ."""
    import re
    d = os.path.join(ROOT, OVERLAYS[ov]['src'])
    local = set(os.listdir(d))
    out = {}
    files = list(sources(ov)) + ['%s/%s' % (OVERLAYS[ov]['src'], f)
                                 for f in local if f.endswith('.h')]
    for src in files:
        txt = open(os.path.join(ROOT, src)).read()
        for inc in re.findall(r'^\s*#\s*include\s+[<"]([^>"/]+\.h)[>"]', txt, re.M):
            if inc in local or inc in out:
                continue
            # already reachable on the real path, so leave it alone
            if any(os.path.exists(os.path.join(ROOT, r, inc)) for r in
                   ('include', 'src/pc', 'tools/psyz/psyz/include')):
                continue
            if os.path.exists(os.path.join(ROOT, 'include', 'psxsdk', inc)):
                out[inc] = 'psyz/types.h' if inc == 'types.h' else 'psxsdk/%s' % inc
    return out


def defines_data(path):
    """True when a header defines an object instead of declaring one, i.e. it
    is a data table meant to be included exactly once."""
    import re
    return bool(re.search(r'^[A-Za-z_][A-Za-z0-9_ ]*[ \*]+[A-Za-z_]\w*\s*'
                          r'(\[[^\]]*\])*\s*=', open(path).read(), re.M))


def ovl_includes(ov, main_private=False):
    """Include lines for the overlay's own headers, relative to src/pc where the
    generated files land. Without them the stand ins cannot name the overlay's
    types and the harness cannot see its entry point."""
    d = OVERLAYS[ov]['src']
    rel = os.path.relpath(d, GENDIR).replace(os.sep, '/')
    # every header in the overlay's directory, not only *_private.h: world
    # keeps its types in world.h. _private last so it can build on the others.
    # Headers that define data rather than declare it are skipped: chocobo's
    # sincos.h and acos.h are lookup tables meant for one .c each, and
    # including them anywhere else duplicates the symbol at link time.
    allh = [f for f in sorted(os.listdir(os.path.join(ROOT, d)))
            if f.endswith('.h') and not defines_data(os.path.join(ROOT, d, f))]
    hs = ([f for f in allh if not f.endswith('_private.h')]
          + [f for f in allh if f.endswith('_private.h')])
    if main_private and os.path.exists(os.path.join(ROOT, 'src/main/main_private.h')):
        hs = [os.path.relpath('src/main/main_private.h', GENDIR).replace(os.sep,'/')] + ['%s/%s' % (rel, h) for h in hs]
    else:
        hs = ['%s/%s' % (rel, h) for h in hs]
    return ''.join('#include "%s"\n' % h for h in hs)


def declarations(ov):
    """Every declaration the overlay publishes, as name -> (kind, text) where
    text is the declaration with any `extern` removed. Stubs are generated from
    these so a stand in always matches the real signature; guessing
    `void f(void)` conflicts the moment a header declares otherwise."""
    import re
    out = {}
    d = os.path.join(ROOT, OVERLAYS[ov]['src'])
    files = list(sources(ov)) + ['%s/%s' % (OVERLAYS[ov]['src'], f)
                                 for f in os.listdir(d) if f.endswith('.h')]
    # The sdk headers come last so an overlay's own declaration wins, but they
    # have to be here: psyz declares names it never implements (SetFogNearFar),
    # and a stub that does not match the visible prototype will not compile.
    for root in ('include', 'include/psxsdk', 'tools/psyz/psyz/include',
                 'tools/psyz/psyz/include/psyz', 'src/main'):
        rd = os.path.join(ROOT, root)
        if os.path.isdir(rd):
            files += ['%s/%s' % (root, f) for f in sorted(os.listdir(rd))
                      if f.endswith('.h')]
    for src in files:
        txt = open(os.path.join(ROOT, src)).read()
        txt = re.sub(r'/\*.*?\*/', ' ', txt, flags=re.S)
        txt = re.sub(r'//[^\n]*', ' ', txt)
        txt = re.sub(r'^\s*#.*(?:\\\n.*)*$', ' ', txt, flags=re.M)
        for chunk in txt.split(';'):
            c = ' '.join(chunk.split())
            if not c or '{' in c or '}' in c or c.startswith('typedef'):
                continue
            c = re.sub(r'^extern\s+', '', c)
            m = re.match(r'^([A-Za-z_][\w\s\*]*?[\s\*])(\w+)\s*\((.*)\)$', c)
            if m and not m.group(1).strip().endswith(','):
                out[m.group(2)] = ('fn', m.group(1).strip(), m.group(3).strip())
                continue
            m = re.match(r'^([A-Za-z_][\w\s\*]*?[\s\*])(\w+)\s*((?:\[[^\]]*\])*)$', c)
            if m:
                out[m.group(2)] = ('data', m.group(1).strip(), m.group(3).strip())
    return out


def hand_defined(path):
    """Symbols a hand written stubs file defines, so gen can report what it
    still leaves out rather than failing at the link."""
    import re
    txt = open(path).read()
    txt = re.sub(r'/\*.*?\*/', ' ', txt, flags=re.S)
    txt = re.sub(r'//[^\n]*', ' ', txt)
    return set(re.findall(r'\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*[;=({]', txt))


def stub_for(name, decl):
    """One stand in. Data becomes a definition of the declared type; a function
    becomes a body that reports itself and returns a zero of the right type."""
    if not decl:
        return 'void %s(void) { OvlStubHit("%s"); }' % (name, name)
    kind, ty, rest = decl
    if kind == 'data':
        return '%s %s%s;' % (ty, name, rest or '')
    args = rest or 'void'
    body = 'OvlStubHit("%s");' % name
    if ty.replace(' ', '') != 'void':
        body += ' return (%s)0;' % ty if '*' not in ty else ' return (%s)0;' % ty
    return '%s %s(%s) { %s }' % (ty, name, args, body)


def cmd_probe(a):
    need(a.overlay); bd = a.build_dir
    tgt = write_cmake(a.overlay, probe_only=True); ensure_include()
    # ff7 first: its objects are what tells us a symbol is already provided
    if not cmake_build('ff7', bd): return 1
    if not cmake_build(tgt, bd): return 1
    objs, und, mine = nm_undefined(bd, tgt, a.overlay)
    have = base_defined(bd)
    missing = sorted(x for x in und if x not in have and not toolchain_sym(x))
    print("overlay %s: %d objects, %d undefined, %d provided by the base, "
          "%d from the toolchain, %d MISSING"
          % (a.overlay, len(objs or []), len(und),
             len([x for x in und if x in have]),
             len([x for x in und if toolchain_sym(x)]), len(missing)))
    decls = declarations(a.overlay)
    for m in missing:
        d = decls.get(m)
        if not d:
            what = "no declaration, stubbed as void f(void)"
        elif d[0] == 'data':
            what = "data  %s%s" % (d[1], d[2])
        else:
            what = "fn    %s (%s)" % (d[1], d[2] or 'void')
        print("   %-28s %s" % (m, what))
    return 0

def cmd_gen(a):
    o = need(a.overlay)
    if not o['entry']:
        sys.exit("no entry point known for %s. add one to OVERLAYS in this file." % a.overlay)
    if entry_is_asm(a.overlay):
        print("warning: %s is still INCLUDE_ASM, so there is nothing to run."
              % o['entry'])
        print("         the build will link the base stub and return at once.")
    bd = a.build_dir
    tgt = write_cmake(a.overlay, probe_only=True); ensure_include()
    if not cmake_build('ff7', bd): return 1
    if not cmake_build(tgt, bd): return 1
    _, und, mine = nm_undefined(bd, tgt, a.overlay)
    have = base_defined(bd)
    missing = sorted(x for x in und if x not in have and not toolchain_sym(x))
    decls = declarations(a.overlay)
    L = ["/* Generated by tools/overlay_runner.py for the %s overlay. Do not edit." % a.overlay,
         " *",
         " * PC stand ins for what this overlay still references: its own data, which",
         " * lives in a psx segment the PsyZ build does not link, and any function",
         " * still behind INCLUDE_ASM. Each stub reports itself through OvlStubHit so",
         " * FF7_STUB_LOG and FF7_STUB_BREAK can tell you which one you reached.",
         " */",
         "#include <game.h>", ovl_includes(a.overlay).rstrip(),
         "", "void OvlStubHit(const char*);", ""]
    nd = nf = 0
    for m in missing:
        d = decls.get(m)
        L.append(stub_for(m, d))
        if d and d[0] == 'data': nd += 1
        else: nf += 1
    # condor keeps CondorUnit inside condor.c, so no header can name it and a
    # generated stand in cannot be typed. Where the repo already carries a
    # hand written stubs file, that one is authoritative: use it and say so.
    hand = os.path.join(ROOT, 'src', 'pc', 'stubs_%s.c' % a.overlay)
    if os.path.exists(hand):
        h = write_harness(a.overlay)
        write_cmake(a.overlay, probe_only=False,
                    extra=['src/pc/stubs_%s.c' % a.overlay, h, 'src/pc/main.c',
                           'src/pc/stubs.c'],
                    stub_renames=sorted(mine & base_stub_defined(bd)))
        print("using hand written src/pc/stubs_%s.c, not generating stubs"
              % a.overlay)
        unmet = [m for m in missing if m not in hand_defined(hand)]
        if unmet:
            print("   it does not cover: %s" % ', '.join(unmet))
        print("generated: %s" % h)
        print("generated: tools/overlay_runner.cmake  (target ovl_%s)" % a.overlay)
        return 0
    os.makedirs(os.path.join(ROOT, GENDIR), exist_ok=True)
    p = os.path.join(ROOT, GENDIR, 'stubs_%s.c' % a.overlay)
    open(p, 'w').write("\n".join(L) + "\n")
    h = write_harness(a.overlay)
    write_cmake(a.overlay, probe_only=False,
                extra=['%s/stubs_%s.c' % (GENDIR, a.overlay), h, 'src/pc/main.c',
                       'src/pc/stubs.c'],
                stub_renames=sorted(mine & base_stub_defined(bd)))
    print("generated: %s/stubs_%s.c (%d data, %d function stubs)"
          % (GENDIR, a.overlay, nd, nf))
    print("generated: %s" % h)
    print("generated: tools/overlay_runner.cmake  (target ovl_%s)" % a.overlay)
    return 0

# ---------------------------------------------------------------- commands

def cmd_build(a):
    if cmd_gen(a): return 1
    if not cmake_build('ovl_%s' % a.overlay, a.build_dir): return 1
    exe = os.path.join(a.build_dir, 'ovl_%s' % a.overlay)
    print("built: %s (%d bytes)" % (exe, os.path.getsize(exe)))
    return 0

def cmd_run(a):
    exe = os.path.join(a.build_dir, 'ovl_%s' % a.overlay)
    if not os.path.exists(exe):
        if cmd_build(a): return 1
    env = []
    if a.frame_step:    env.append("FF7_FRAME_STEP=1")
    if a.trace:         env.append("FF7_TRACE=1")
    if a.break_on_stub: env.append("FF7_STUB_BREAK=1")
    if a.log_stubs:     env.append("FF7_STUB_LOG=1")
    # On nix only: SDL3 is linked static and dlopens wayland/vulkan/alsa at run
    # time, and nix-shell leaves LD_LIBRARY_PATH unset, so those never resolve
    # and SDL reports "No available video device". Point it at a buildEnv of
    # them, with the driver path first for the vulkan ICD. Everywhere else the
    # system loader already finds them and this is skipped.
    if use_nix():
        libs = sh("nix-build --no-out-link -E 'with import <nixpkgs> {}; buildEnv "
                  "{ name = \"ovl-libs\"; paths = [ wayland libxkbcommon libdecor "
                  "libGL vulkan-loader dbus alsa-lib pipewire ]; }'").stdout.strip()
        if libs:
            env.append("LD_LIBRARY_PATH=/run/opengl-driver/lib:%s/lib" % libs)
    cmd = "%s %s" % (' '.join(env), shq(exe))
    if a.gdb:
        cmd = "%s gdb -q -batch -ex run -ex bt -ex 'info registers rip' --args %s" % (
              ' '.join(env), exe)
        return in_env(cmd, capture=False).returncode
    print("running:", cmd)
    return subprocess.run(cmd, shell=True, cwd=ROOT).returncode

def cmd_debug(a):
    o = need(a.overlay)
    print("overlay        %s" % a.overlay)
    print("sources        %s" % ', '.join(sources(a.overlay)))
    print("asm remaining  %d" % asm_left(a.overlay))
    print("entry point    %s (returns %s)" % (o['entry'] or '(unknown)', o['ret'] or '-'))
    print("disc image     %s, %d sectors -> 0x80180000, unpacked to 0x800A0000"
          % (o['lba'] or '(none)', o['sectors']))
    print("build dir      %s" % a.build_dir)
    print("cmake include  %s" % ("present" if INCLUDE_LINE in
          open(os.path.join(ROOT,'CMakeLists.txt')).read() else "NOT yet added"))
    for f in ('%s/stubs_%s.c' % (GENDIR, a.overlay),
              '%s/harness_%s.c' % (GENDIR, a.overlay),
              'tools/overlay_runner.cmake'):
        p = os.path.join(ROOT, f)
        print("generated      %-34s %s" % (f, "yes, %d bytes" % os.path.getsize(p)
              if os.path.exists(p) else "no"))
    print("runtime switches  FF7_FRAME_STEP FF7_TRACE FF7_STUB_BREAK FF7_STUB_LOG")
    return 0

def cmd_clean(a):
    n = 0
    targets = [GEN_CMAKE]
    for ov in ([a.overlay] if a.overlay else OVERLAYS):
        targets += [os.path.join(ROOT,'src','pc','stubs_%s.c'%ov),
                    os.path.join(ROOT,'src','pc','harness_%s.c'%ov)]
    for t in targets:
        if os.path.exists(t): os.remove(t); print("removed", t); n += 1
    if a.build_dir and os.path.isdir(a.build_dir) and a.all:
        shutil.rmtree(a.build_dir); print("removed", a.build_dir); n += 1
    print("%d removed" % n); return 0

def cmd_list(a):
    print("%-9s %-22s %6s  %-20s %s" % ("overlay","sources","asm","entry","state"))
    for ov in sorted(OVERLAYS):
        if not os.path.isdir(os.path.join(ROOT, OVERLAYS[ov]['src'])): continue
        n, e = asm_left(ov), OVERLAYS[ov]['entry']
        # "no asm left" is not "runs": jet has none and still faults on a
        # port bug. Only build and run can tell you, so do not claim it here.
        if n == 0 and e:
            st = "no asm left"
        elif n == 0:
            st = "needs entry"
        elif e and entry_is_asm(ov):
            st = "%d asm left, entry is asm" % n
        else:
            st = "%d asm left" % n
        print("%-9s %-22s %6d  %-20s %s" % (ov, OVERLAYS[ov]['src'], n, e or "(unknown)", st))
    return 0

def main():
    global VERBOSE
    p = argparse.ArgumentParser(prog='overlay_runner.py', add_help=False,
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('-h','--help', action='store_true')
    p.add_argument('-d','--debug', action='store_true', help='verbose tool logging')
    p.add_argument('--build-dir', default=DEFAULT_BUILD)
    # Repeated on every subcommand so the flags work on either side of it.
    # SUPPRESS keeps an unset subcommand flag from clobbering the global one.
    common = argparse.ArgumentParser(add_help=False)
    common.add_argument('-d','--debug', action='store_true', default=argparse.SUPPRESS)
    common.add_argument('--build-dir', default=argparse.SUPPRESS)
    sub = p.add_subparsers(dest='cmd')
    sub.add_parser('help').add_argument('topic', nargs='?')
    sub.add_parser('list', parents=[common])
    for c in ('probe','gen','build','debug'):
        sp = sub.add_parser(c, parents=[common]); sp.add_argument('overlay')
    r = sub.add_parser('run', parents=[common]); r.add_argument('overlay')
    for f in ('--frame-step','--break-on-stub','--log-stubs','--trace','--gdb'):
        r.add_argument(f, action='store_true')
    c = sub.add_parser('clean', parents=[common]); c.add_argument('overlay', nargs='?')
    c.add_argument('--all', action='store_true', help='also remove the build dir')
    a = p.parse_args()
    VERBOSE = a.debug
    if a.help or not a.cmd or a.cmd == 'help':
        print(__doc__); return 0
    for k in ('frame_step','break_on_stub','log_stubs','trace','gdb','all','overlay'):
        if not hasattr(a, k): setattr(a, k, False if k != 'overlay' else None)
    return {'list':cmd_list,'probe':cmd_probe,'gen':cmd_gen,'build':cmd_build,
            'run':cmd_run,'debug':cmd_debug,'clean':cmd_clean}[a.cmd](a)

if __name__ == '__main__':
    sys.exit(main() or 0)
