//! PSYQ=3.3 G=0
#include "main_private.h"
#include "unzip.h"

void SysBgRender(void);

enum {
    YAMA_SOUND_INSTR_ALL,
    YAMA_SOUND_EFFECT,
    YAMA_SOUND_INSTR_DAT,
    YAMA_SOUND_INSTR2_ALL,
    YAMA_SOUND_INSTR2_DAT,
    YAMA_FIELD_FIELD,
    YAMA_WORLD_WORLD,
    YAMA_MINI_CONDOR,
    YAMA_MINI_SNOBO,
    YAMA_MINI_SNOBO2,
    YAMA_FIELD_DSCHANGE,
    YAMA_FIELD_ENDING,
    YAMA_MINI_CHOCOBO,
    YAMA_MINI_JET,
    YAMA_MINI_SUBMAR,
    YAMA_MINI_HIGHWAY,
};

static Yamada yama_files[16] = {
    {LBA_SOUND_INSTR_ALL, 483232},  // YAMA_SOUND_INSTR_ALL
    {LBA_SOUND_EFFECT, 51200},      // YAMA_SOUND_EFFECT
    {LBA_SOUND_INSTR_DAT, 8192},    // YAMA_SOUND_INSTR_DAT
    {LBA_SOUND_INSTR2_ALL, 251120}, // YAMA_SOUND_INSTR2_ALL
    {LBA_SOUND_INSTR2_DAT, 8192},   // YAMA_SOUND_INSTR2_DAT
    {LBA_FIELD_FIELD, 85435},       // YAMA_FIELD_FIELD
    {LBA_WORLD_WORLD, 66715},       // YAMA_WORLD_WORLD
    {LBA_MINI_CONDOR, 39600},       // YAMA_MINI_CONDOR
    {LBA_MINI_SNOBO, 70075},        // YAMA_MINI_SNOBO
    {LBA_MINI_SNOBO2, 81441},       // YAMA_MINI_SNOBO2
    {LBA_FIELD_DSCHANGE, 6004},     // YAMA_FIELD_DSCHANGE
    {LBA_FIELD_ENDING, 62484},      // YAMA_FIELD_ENDING
    {LBA_MINI_CHOCOBO, 36521},      // YAMA_MINI_CHOCOBO
    {LBA_MINI_JET, 14067},          // YAMA_MINI_JET
    {LBA_MINI_SUBMAR, 31341},       // YAMA_MINI_SUBMAR
    {LBA_MINI_HIGHWAY, 34138},      // YAMA_MINI_HIGHWAY
};

// likely a left-over from a debug build that used to load sparse files instead from the Yamada LBA.
static char unk_signature[8] = {'Y', 'A', 'M', 'A', '@', 'F', 'F', '7'};

void __main(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", __SN_ENTRY_POINT);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_8001117C);

static void func_800111E4(void) {
    D_8009A000[0] = 0xF4;
    SystemAkaoExecute();
    if (!(Savemap.memory_bank_4[97] & 0x30)) {
        func_8001117C(0x2B);
    }
    D_800707BC = g_FieldState.eventCmdParam;
    g_BattleMode = g_FieldState.battleMode2;
    g_BattleMode = D_800716D0 | g_BattleMode;
    func_800146A4();
    D_800716D0 = 0;
}

static void func_80011274(void) {
    SystemLoadFileBySector(
        yama_files[YAMA_SOUND_INSTR2_DAT].loc, yama_files[YAMA_SOUND_INSTR2_DAT].len, (u_long*)0x800E0000, NULL);

    while (1) {
        if (SystemCdromReadChain() == 0) {
            break;
        }
    }

    SystemLoadFileBySector(
        yama_files[YAMA_SOUND_INSTR2_ALL].loc, yama_files[YAMA_SOUND_INSTR2_ALL].len, (u_long*)0x800A0000, NULL);

    while (1) {
        if (SystemCdromReadChain() == 0) {
            break;
        }
    }

    func_80029818((u32*)0x800A0000, (u32*)0x800E0000);
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800112E8);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysBgFadeRender);

static void VSyncCallbackFunc(void) {
    switch (D_80095DD4) {
    case 0:
        break;
    case 1:
        SysBgFadeRender();
        break;
    case 2:
        func_800D8D78();
        break;
    case 3:
        SysBattleSwirlRender();
        break;
    case 4:
        SysMenuDrawBattleResult();
        break;
    }
    if (!D_80062D98 && !D_80062D99) {
        Savemap.game_timer_fraction += 1092; // 65536 / 1092 = ~60
        if (Savemap.game_timer_fraction >> 16) {
            Savemap.time++;
            Savemap.game_timer_fraction &= 0xFFFF;
        }
        Savemap.countdown_timer_fraction += 1092;
        if (Savemap.countdown_timer_fraction >> 16) {
            if (!(Savemap.memory_bank_1[95] & 2)) {
                if (Savemap.countdown_timer_seconds != 0) {
                    Savemap.countdown_timer_seconds--;
                }
            } else {
                Savemap.countdown_timer_seconds++;
            }
            Savemap.countdown_timer_fraction &= 0xFFFF;
        }
    }
    D_8007E768 = 1;
}

static void SysInitBase(void) {
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    func_80036298();
    D_80095DD4 = 0;
    VSyncCallback(VSyncCallbackFunc);
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysInitDispenvDrawenv);

void func_800A16CC(); // field loop
void func_800CF60C(); // field load

static void SysFieldRun(void) {
    if (D_800965EC != 5 && D_800965EC != 13) {
        if (D_800965EC != 2) {
            SystemLoadFileBySector(
                yama_files[YAMA_FIELD_FIELD].loc, yama_files[YAMA_FIELD_FIELD].len, (u_long*)0x80180000, NULL);
            while (1) {
                if (SystemCdromReadChain() == 0) {
                    break;
                }
            }
            SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
        } else {
            while (1) {
                if (SystemCdromReadChain() == 0) {
                    break;
                }
            }
            SysGzipBinDecompress((GzHeader*)0x801C0000, (u8*)0x800A0000);
        }
    }
    func_800CF60C();
    func_800A16CC();
}

static void func_80011920(void) {
    g_isFieldLoading = 0;
    D_80071A5C = 0;
}

static void SysInitAkaoEngine(void) {
    SystemLoadFileBySector(
        yama_files[YAMA_SOUND_INSTR_ALL].loc, yama_files[YAMA_SOUND_INSTR_ALL].len, (u_long*)0x800F0000, NULL);
    do {
    } while (SystemCdromReadChain());
    SystemLoadFileBySector(
        yama_files[YAMA_SOUND_EFFECT].loc, yama_files[YAMA_SOUND_EFFECT].len, (u_long*)0x801B0000, NULL);
    do {
    } while (SystemCdromReadChain());
    SystemLoadFileBySector(
        yama_files[YAMA_SOUND_INSTR_DAT].loc, yama_files[YAMA_SOUND_INSTR_DAT].len, (u_long*)0x801BC800, NULL);
    do {
    } while (SystemCdromReadChain());
    func_8002988C(0x800F0000, 0x801BC800);
    func_80029998(0x801B0000);
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800119E4);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysInitFieldFromSavemap);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysInitNewGame);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", main);

const s16 D_80010020[4] = {0, 0, 0x1E0, 0x1D8};

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", jtbl_80010028);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", jtbl_80010068);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", jtbl_800100A0);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetDrawMode);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeInitPoly);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeCopyScreen);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetPolyMonochrome);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetPolyRgbGradual);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetPolyRgbDirect);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeStepsDec);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeStepsInc);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeColorInterpolate);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetPolyRgbInterpolate);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeBgUpdate);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeUpdate);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", D_80010100);

const u8 D_80010118[8] = {4, 10, 11, 12, 13, 0, 0, 0};

const u8 D_80010120[4] = {0, 0x38, 0x48, 0x80};

const u8 D_80010124[20] = {
    1, 1, 1, 1, 2, 0, 0xFF, 0xFF, 0xFF, 0xFF, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0,
};
