//! PSYQ=3.3 G=0
#include "main_private.h"
#include "unzip.h"

extern u8 g_KernRndTable[];
extern u8 D_80062D98;
extern u8 D_80062D99;
extern s32 D_80062D9C;
extern s32 D_80062DA0;
extern s32 D_80062DA4;
extern s32 D_80062DA8;
extern s32 D_80062DAC;
extern s32 D_80062DB0;
extern s16 D_80062DB4;
extern s16 D_80062DB6;
extern s16 D_80062DB8;
extern s16 D_80062DBA;
extern s16 D_80062DBC;
extern s16 D_80062DBE;
extern s32 D_80062DC0;
extern s32 D_80062DC4;
extern s32 D_80062DC8;
extern s32 D_80062DCC;
extern s32 D_80062DD0;
extern s32 D_80062DD4;
extern u8 D_80062DDB;
extern u8 D_80062DDC;
extern s32 D_80062DE0;
extern u8 D_80062DE4;
extern u8 D_80062DE5;
extern s16 D_80062DE6;
extern s16 D_80062DE8;
extern s16 D_80062DEA;
extern s32 D_80062DEC;
extern s32 D_80062DF0;
extern s32 D_80062DF4;
extern s8 D_80062DFC;
extern s8 _D_80062DFD;
extern s32 D_80062E00;
extern s32 D_80062E04;
extern s16 D_80062E08;
extern s16 D_80062E0A;
extern s32 D_80062E0C;
void SysBgRender(void);
s32 SysGetMateriaActivatedStars(u8, s32);

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
    SystemLoadFileBySector(D_80048CFC[4].loc, D_80048CFC[4].len, (u_long*)0x800E0000, NULL);

    while (1) {
        if (SystemCdromReadChain() == 0) {
            break;
        }
    }

    SystemLoadFileBySector(D_80048CFC[3].loc, D_80048CFC[3].len, (u_long*)0x800A0000, NULL);

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
            SystemLoadFileBySector(D_80048CFC[5].loc, D_80048CFC[5].len, (u_long*)0x80180000, NULL);
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
    SystemLoadFileBySector(D_80048CFC[0].loc, D_80048CFC[0].len, (u_long*)0x800F0000, NULL);
    do {
    } while (SystemCdromReadChain());
    SystemLoadFileBySector(D_80048CFC[1].loc, D_80048CFC[1].len, (u_long*)0x801B0000, NULL);
    do {
    } while (SystemCdromReadChain());
    SystemLoadFileBySector(D_80048CFC[2].loc, D_80048CFC[2].len, (u_long*)0x801BC800, NULL);
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
