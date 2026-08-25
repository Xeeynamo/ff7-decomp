//! PSYQ=3.3 CC1=2.7.2 G=0
#include "main_private.h"
#include "unzip.h"

void func_80015B44(u8* arg0);
u8* func_80014C80(s32 arg0);
u16 func_80015B50(void);
u16 func_80015B88(void);
extern u8 D_80083084[];

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
void func_8001155C(void);
void func_80014A00(s32* dst, s32* src, s32 len);
u16* func_80014D9C(s32, s32, s32);
s32 func_800150E4(u16*, u16*);
u16* func_800151F4(s32);
void func_80015CA0(GzHeader* src, s32* dst);
s32 func_8001AC9C(u8, s32);
void func_8001B834(s32);
void func_8001BD50(u8, u8, u8);
u8 func_8001F6B4();
void func_8001F6E4(s16, s16, s16);

void __main(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", __SN_ENTRY_POINT);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_8001117C);

void func_800111E4(void) {
    D_8009A000[0] = 0xF4;
    SystemAkaoExecute();
    if (!(Savemap.memory_bank_4[97] & 0x30)) {
        func_8001117C(0x2B);
    }
    D_800707BC = D_8009ABF4.eventCmdParam;
    g_BattleMode = D_8009ABF4.battleMode2;
    g_BattleMode = D_800716D0 | g_BattleMode;
    func_800146A4();
    D_800716D0 = 0;
}

void func_80011274(void) {
    SystemLoadFileBySector(
        D_80048CFC[4].loc, D_80048CFC[4].len, (u_long*)0x800E0000, NULL);

    while (1) {
        if (SystemCdromReadChain() == 0) {
            break;
        }
    }

    SystemLoadFileBySector(
        D_80048CFC[3].loc, D_80048CFC[3].len, (u_long*)0x800A0000, NULL);

    while (1) {
        if (SystemCdromReadChain() == 0) {
            break;
        }
    }

    func_80029818((u32*)0x800A0000, (u32*)0x800E0000);
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800112E8);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_8001146C);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_8001155C);

void func_8001171C(void) {
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    func_80036298();
    D_80095DD4 = 0;
    VSyncCallback(&func_8001155C);
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80011784);

void func_800A16CC(); // field loop
void func_800CF60C(); // field load

void func_80011860(void) {
    if (D_800965EC != 5 && D_800965EC != 13) {
        if (D_800965EC != 2) {
            SystemLoadFileBySector(D_80048CFC[5].loc, D_80048CFC[5].len,
                                   (u_long*)0x80180000, NULL);
            while (1) {
                if (SystemCdromReadChain() == 0) {
                    break;
                }
            }
            func_80015CA0((GzHeader*)0x80180000, (s32*)0x800A0000);
        } else {
            while (1) {
                if (SystemCdromReadChain() == 0) {
                    break;
                }
            }
            func_80015CA0((GzHeader*)0x801C0000, (s32*)0x800A0000);
        }
    }
    func_800CF60C();
    func_800A16CC();
}

void func_80011920(void) {
    g_isFieldLoading = 0;
    D_80071A5C = 0;
}

void func_80011938(void) {
    SystemLoadFileBySector(
        D_80048CFC[0].loc, D_80048CFC[0].len, (u_long*)0x800F0000, NULL);
    do {
    } while (SystemCdromReadChain());
    SystemLoadFileBySector(
        D_80048CFC[1].loc, D_80048CFC[1].len, (u_long*)0x801B0000, NULL);
    do {
    } while (SystemCdromReadChain());
    SystemLoadFileBySector(
        D_80048CFC[2].loc, D_80048CFC[2].len, (u_long*)0x801BC800, NULL);
    do {
    } while (SystemCdromReadChain());
    func_8002988C(0x800F0000, 0x801BC800);
    func_80029998(0x801B0000);
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800119E4);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80011AEC);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80011BB4);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", main);

const s16 D_80010020[4] = {0, 0, 0x1E0, 0x1D8};

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", jtbl_80010028);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", jtbl_80010068);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", jtbl_800100A0);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80012840);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800128B8);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800129D0);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80012A8C);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80012DB0);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800131B8);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800134F4);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80013564);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800135C0);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80013624);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80013800);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800138EC);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", D_80010100);

const u8 D_80010118[8] = {4, 10, 11, 12, 13, 0, 0, 0};

const u8 D_80010120[4] = {0, 0x38, 0x48, 0x80};

const u8 D_80010124[20] = {
    1, 1, 1, 1, 2, 0, 0xFF, 0xFF, 0xFF, 0xFF, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0,
};

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_80013C9C);

void func_800140A4(void) {
    D_8019DAA0++;
    if (!(D_8019DAA0 & 1)) {
        DrawOTag(D_8019D5E8);
        func_80013C9C();
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800140F4);
