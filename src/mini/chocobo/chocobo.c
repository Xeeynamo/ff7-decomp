//! PSYQ=4.0 CC1=2.6.3

#include "common.h"

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A02D0);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A1260);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A1354);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A157C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A1630);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A17F0);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A18BC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A1F40);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A272C);

//INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo", func_800A28D8);


extern u32 D_80079F64;
extern u32 D_8007AF64;
extern u32 D_8007BF64;
extern u32 D_8007C764;

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
