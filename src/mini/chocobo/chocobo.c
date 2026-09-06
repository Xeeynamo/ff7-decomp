//! PSYQ=4.0 CC1=2.7.2

#include "common.h"
#include "game.h"
#include "libgte.h"

typedef struct {
    u32 unk0;
    u32 unk4;
} UnkRectData;

extern UnkRectData D_800A0020;
extern UnkRectData D_800A0028;
extern u32 D_80079F64;
extern u32 D_8007AF64;
extern u32 D_8007BF64;
extern u32 D_8007C764;
extern s32 D_800B7598;
extern s32 D_800F507C;
extern s32* D_800F5084;

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A02D0);

void func_800A1260(void) {
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_s2;

    temp_s0 = *D_800F5084;
    temp_s2 = (D_800B7598 + temp_s0) % temp_s0;
    temp_s1 = (D_800F507C + temp_s0) % temp_s0;

    PushMatrix();

    if (temp_s1 < temp_s2) {
        func_800A1354(0, temp_s1);
        func_800A1354(temp_s2, temp_s0);
    } else {
        func_800A1354(temp_s2, temp_s1);
    }

    PopMatrix();
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A1354);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A157C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A1630);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A17F0);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A18BC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A1F40);

void func_800A272C(s32 arg0, s32 arg1) {
    RECT sp10;
    RECT sp18;
    s32 var_a0;
    u32 var_a1;

    sp10 = *(RECT*)&D_800A0020;
    sp18 = *(RECT*)&D_800A0028;

    var_a0 = 0x32C;
    if (arg0 != 0) {
        var_a0 = 0x3C1;
    }

    SysCdromStartLoadLzs(var_a0, 0x20000, (u32*)0x80110000, 0);

    while (SystemCdromReadChain() != 0) {
    }

    LoadImage(&sp10, (u32*)0x80110000);
    DrawSync(0);

    if (arg0 != 0) {
        var_a0 = 0x3F1;
        var_a1 = 0x1E000;
    } else {
        var_a0 = 0x3CE;
        var_a1 = 0x1E800;
    }

    SysCdromStartLoadLzs(var_a0, var_a1, (u32*)0x80190000, 0);

    while (SystemCdromReadChain() != 0) {
    }

    switch (arg1) {
    case 0:
        var_a0 = 0x459;
        SysCdromStartLoadLzs(var_a0, 0x30000, (u32*)0x80110000, 0);
        break;

    case 1:
        var_a0 = 0x433;
        SysCdromStartLoadLzs(var_a0, 0x30000, (u32*)0x80110000, 0);
        break;

    case 2:
        var_a0 = 0x417;
        SysCdromStartLoadLzs(var_a0, 0x30000, (u32*)0x80110000, 0);
        break;

    case 3:
        var_a0 = 0x49C;
        SysCdromStartLoadLzs(var_a0, 0x30000, (u32*)0x80110000, 0);
        break;
    }

    while (SystemCdromReadChain() != 0) {
    }

    LoadImage(&sp18, (u32*)0x80110000);
    DrawSync(0);

    if (arg0 != 0) {
        var_a0 = 0x33E;
        var_a1 = 0x6A000;
    } else {
        var_a0 = 0x293;
        var_a1 = 0x7D000;
    }

    SysCdromStartLoadLzs(var_a0, var_a1, (u32*)0x80110000, 0);

    while (SystemCdromReadChain() != 0) {
    }
}

void func_800A28D8(void) {
    SysCdromStartLoadLzs(0x4C9, 0x1000U, &D_80079F64, NULL);
    do {

    } while (SystemCdromReadChain() != 0);
    SysCdromStartLoadLzs(0x4CA, 0x1000U, &D_8007AF64, NULL);
    do {

    } while (SystemCdromReadChain() != 0);
    SysCdromStartLoadLzs(0x4C8, 0x800U, &D_8007BF64, NULL);
    do {

    } while (SystemCdromReadChain() != 0);
    SysCdromStartLoadLzs(0x4C7, 0x800U, &D_8007C764, NULL);
    do {

    } while (SystemCdromReadChain() != 0);
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A2984);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A2AFC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A2BD4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A3308);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A34A8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A44E4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A4888);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A500C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A68D4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A6B9C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A6E50);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A7840);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A7924);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A7AB8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A7CA4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A869C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A8940);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A89A0);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A8A18);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A8AE8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A9828);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A9A94);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A9D94);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AAC00);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AAF1C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AB410);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800ABABC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AC554);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AD52C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AD7B8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AD7E8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AD91C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AD9D8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AE534);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AE7D4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AF11C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AF9E4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AFC64);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800AFDBC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800B00DC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800B01B0);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800B0E7C);
