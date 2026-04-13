//! PSYQ=3.3 CC1=2.6.3
#include "world.h"

typedef struct {
    /* 0x00 */ s32 unk0; // aligns with Unk8010AD3C->unkC
    /* 0x04 */ s32 unk4; // aligns with Unk8010AD3C->unk10
    /* 0x08 */ s32 unk8; // aligns with Unk8010AD3C->unk14
    /* 0x0C */ s32 unkC; // aligns with Unk8010AD3C->unk18
} Unk800AA098;

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
    /* 0x2C */ s32 unk2C;
    /* 0x30 */ s32 unk30;
    /* 0x34 */ s32 unk34;
    /* 0x38 */ s32 unk38;
    /* 0x3C */ s32 unk3C;
    /* 0x40 */ s32 unk40;
    /* 0x44 */ s32 unk44;
    /* 0x48 */ s16 unk48;
    /* 0x4A */ s16 unk4A;
    /* 0x4C */ s16 unk4C;
    /* 0x4E */ s16 unk4E;
    /* 0x50 */ s32 unk50;
    /* 0x54 */ s32 unk54;
    /* 0x58 */ s32 unk58;
    /* 0x5C */ s8 unk5C;
    /* 0x5D */ s8 unk5D;
} Unk8010AD3C; // size:???

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
    /* 0x2C */ s32 unk2C;
    /* 0x30 */ s32 unk30;
    /* 0x34 */ s32 unk34;
    /* 0x38 */ s32 unk38;
    /* 0x3C */ s32 unk3C;
    /* 0x40 */ s32 unk40;
    /* 0x44 */ s32 unk44;
    /* 0x48 */ s16 unk48;
    /* 0x4A */ s16 unk4A;
    /* 0x4C */ s16 unk4C;
    /* 0x4E */ s16 unk4E;
    /* 0x50 */ s8 unk50;
} Unk8010AD40; // size:???

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ s32 unk20;
} Unk8010B3B8; // size:???

void func_800A31C0(s16);
void func_800A368C(s32);
void func_800A6994(s32*, s32);
void func_800A8CE4();
void func_800A8F74();
void func_800A8FCC();
void func_800A9110();
s32 func_800A9154();
s32 func_800A9174();
s32 func_800A929C();
s16 func_800A97A8(void);
void func_800A98A4(s32);
s32 func_800A98E4(void);
s32 func_800A99BC();
s32 func_800A9A44(void);
void func_800AB8EC(s32);
void func_800ABA18(s32);
void func_800B5274();
void func_800B63F0(s32);
void func_800B65E0(s32);
s32 func_800B7200();
void func_800BB9A0(u8);
static void func_800BBA5C(void);
s32 func_800BBBB0(void);

extern s32 D_800D05E8;
extern s32 D_800E5608;
extern s32 D_800E560C;
extern s32 D_800E5618;
extern s32 D_800E5628;
extern s32 D_800E5630;
extern s32 D_800E5634;
extern s32 D_800E5648;
extern s32 D_800E5654;
extern s32 D_800E5678;
extern MATRIX D_800E5698;
extern MATRIX D_800E56B8;
extern s32 D_800E5828;
extern s32 D_800E55EC;
extern s32 D_800E55FC;
extern s32 D_800E561C;
extern s32 D_800E5620;
extern s32 D_800E5624;
extern s32 D_800E5638;
extern s32 D_800E563C;
extern s32 D_800E5644;
extern s32 D_800E5658;
extern s32 D_800E566C;
extern s32 D_800E5674;
extern s16 D_800E56D8;
extern s32 D_800E56F4;
extern s32 D_800E5824;
extern s32 D_80109D58;
extern s32 D_8010C808;
extern s32 D_8010CAF0;
extern s16 D_8010CAFC;
extern s32 D_80109D54;
extern s32 D_80109D6C;
extern Unk8010AD3C* D_8010AD3C;
extern Unk8010AD40* D_8010AD40;
extern s32 D_8010AE54;
extern s32 D_8010B080;
extern s32 D_8010B174;
extern Unk8010B3B8* D_8010B3B8;
extern s32 D_8010B47C;
extern s32 D_80115A68;
extern s32 D_801159DC;
extern s32 D_801159E0;
extern s32 D_80115A60;
extern s32 D_8011626C;
extern s32 D_80116270;
extern s32 D_80116278;
extern s32 D_8011627C;
extern s32 D_80116280;
extern s32 D_80116284;
extern s8 D_801163E0;
extern s8* D_801163E8;
extern s32 D_801163D4;
extern s32 D_801163EC;
extern s16 D_801164F8;

const char D_800A0000[] = "NEW  ";
static const char D_800A0008[] = "OLD  ";
static const char D_800A0010[] = "JUMP ";
static const char D_800A0018[] = "FROM ";
static const char D_800A0020[] = "SCR-H";

static void func_800A0B40(void) {}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A0B48);

static s32 func_800A0BD4(void) { return D_800D05E8; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A0BE4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A0C54);

static void func_800A0D1C(void) { D_800E55EC = 0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A0D2C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A12AC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A1370);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A141C);

static void func_800A16D0(s32 arg0) { D_800E5630 = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A16E0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A1710);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A19FC);

static s32 func_800A1D04(void) { return D_800E5648; }

s32 func_800A1D14(void) { return D_800E560C & 0xFFF; }

static void func_800A1D24(s32 arg0) { D_800E5608 = arg0 & 0xFFF; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A1D38);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A1D54);

static s32 func_800A1DB0(void) { return D_800E5618; }

static s32 func_800A1DC0(void) { return D_800E5654; }

static void func_800A1DD0(s32 arg0) { D_800E5654 = arg0; }

static s32 func_800A1DE0(void) { return D_800E5634; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A1DF0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A1FAC);

void func_800A2040(void) {
    SetRotMatrix(&D_800E5698);
    SetTransMatrix(&D_800E56B8);
}

static s16 func_800A2078(void) { return D_800E56D8; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A2088);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A2108);

s32 func_800A21A4(void) { return D_800E55FC; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A21B4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A31C0);

s32 func_800A31E8(void) { return !D_800E5628; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A31F8);

s32 func_800A32F4(void) { return D_800E5678; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3304);

void func_800A368C(s32 arg0) { D_800E5658 = arg0; }

static s32 func_800A369C(void) { return D_800E5658; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A36AC);

void func_800A38C8(void) {
    if (D_80071E34 == 1) {
        func_800260DC();
        func_80026090();
        D_80071E34 = 0;
    }
}

void func_800A3908(void) {
    s32 ret;

    do {
        ret = func_800484A8();
        if (ret == -1) {
            VSync(0);
        }
    } while (ret == -1);
    if (ret) {
        do {
        } while (func_80048540(1));
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3964);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3C74);

void func_800A3DFC(void) {
    D_800E566C = 5;
    D_800E5644 = 0x14;
    func_800B7714(0);
    func_800A2108(0, 0);
    func_800B0098(0x10, 1);
}

void func_800A3E4C(void) {
    D_800E566C = 4;
    D_800E5644 = -0x14;
    func_800B7714(0);
    func_800A2108(0, 0);
    func_800B0098(0x10, 1);
}

void func_800A3E9C(s32 arg0) {
    func_800B7714(arg0);
    D_800E566C = 8;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3EC8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3F4C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4008);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4080);

void func_800A40B8(s32 arg0) {
    func_800A2088(arg0 & 3);
    func_800BC9E8((arg0 >> 2) & 3);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A40F0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4138);

void func_800A41E8(s32 arg0) {
    switch (arg0) {
    case 1:
        func_800A2108(0, 4);
        func_800B0098(4, 1);
        break;
    case 4:
        func_800A2108(1, 4);
        func_800AFFBC(0x10, 1);
        arg0 = 0;
        break;
    }
    D_800E56F4 = arg0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4268);

void func_800A4494(s32 arg0) { D_800E561C = arg0; }

void func_800A44A4(s32 arg0) { D_800E5620 = arg0; }

void func_800A44B4(s32 arg0) { D_800E5624 = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A44C4);

void func_800A45C4(s32 arg0) { D_800E5638 = arg0; }

void func_800A45D4(s32 arg0) { D_800E563C = arg0; }

void func_800A45E4(s32 arg0) { D_800E5674 = arg0; }

s32 func_800A45F4(void) { return D_800E5674; }

static void func_800A4604(void) {}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A460C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4DDC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4F08);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4F78);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5208);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A52A4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5348);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A53A8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A54F0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A57C8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5924);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5970);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A59A0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5A20);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5A94);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5AD8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5B88);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5C08);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5D00);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5E28);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5FB4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A60D8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6168);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A63FC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A64AC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A67A8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6884);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A692C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6994);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6B8C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6BCC);

void func_800A6C00(s32 arg0) {
    if (func_800A1DB0() != 2) {
        func_800A1FAC(arg0);
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6C3C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6FC0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A71E8);

static s32 func_800A7E7C(void) { return D_80109D6C; }

static void func_800A7E8C(s32 arg0) { D_80109D6C = arg0; }

static void func_800A7E9C(void) {}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A7EA4);

void func_800A7F18(void) { func_800A7F38(); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A7F38);

static void func_800A8048(void) { D_800E5828 = 1; }

static s32 func_800A805C(void) { return D_800E5828; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A806C);

static void func_800A82DC(void) { D_80109D54 = 1; }

s32 func_800A82F0(void) { return D_80109D58; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8300);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A835C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A86C4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A886C);

void func_800A8888(s32 arg0) { D_800E5824 = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8898);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A891C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8A1C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8A88);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8ABC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8AF4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8B30);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8C70);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8CA4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8CE4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8D58);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8E50);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8F48);

void func_800A8F74(void) {
    if (D_8010AD40) {
        func_800A8AF4(D_8010AD40);
    }
}

void func_800A8FA0(void) {
    if (D_8010AD3C) {
        func_800A8AF4(D_8010AD3C);
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8FCC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9018);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9064);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A90EC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9110);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9134);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9154);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9174);

Unk8010AD3C* func_800A9194(void) { return D_8010AD3C; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A91A4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A91E0);

s32 func_800A921C(s32 arg0, u8 arg1) {
    if (arg1 >= 0x20) {
        return 0;
    }
    return (arg0 >> arg1) & 1;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9240);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A929C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A92F8);

static const s32 D_800A01D8[] = {0, 0xF000};
static const s32 D_800A01E0[] = {0, 0};
static const s32 D_800A01E8[] = {0, 0, 0, 0};

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9334);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9480);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A94A8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A94D0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A94F4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9520);

void func_800A9678(s16 arg0) { func_800A9520(D_8010AD3C, arg0); }

void func_800A96A4(s16 arg0) { func_800A9520(D_8010AD40, arg0); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A96D0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A97A8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A97E4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9820);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A984C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9878);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A98A4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A98E4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9910);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A993C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9988);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A99BC);

void func_800A9A04(s8 arg0) {
    if (D_8010AD40) {
        D_8010AD40->unk50 = arg0;
    }
}

void func_800A9A24(s16 arg0) {
    if (D_8010AD40) {
        D_8010AD40->unk4A = arg0;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9A44);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9A70);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9AA4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9AD0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9B04);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9C64);

void func_800A9D5C(s32 arg0) { func_800A9C64(D_8010AD3C, arg0); }

void func_800A9D88(s32 arg0) { func_800A9C64(D_8010AD40, arg0); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9DB4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9E14);

static void func_800AA02C(s32 arg0) {
    if (D_8010AD40) {
        D_8010AD40->unk10 = arg0;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA04C);

static void func_800AA078(s16 arg0) {
    if (D_8010AD40) {
        D_8010AD40->unk4E = arg0;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA098);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA0E0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA128);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA170);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA1B8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA238);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA2B8);

static void func_800AA2E4(s8 arg0) {
    if (D_8010AD3C) {
        D_8010AD3C->unk5D = arg0;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA304);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA580);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA640);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA684);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA6A4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA6D0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA7DC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA8D8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA8F8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AAA00);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AAB18);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB36C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB398);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB48C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB4F4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB570);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB5E4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB6E4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB8EC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB92C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AB988);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ABA18);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ABA78);

static const s32 D_800A0260[] = {0, 0};

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ABB24);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ABE58);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ABFC0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AC3C0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AC484);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AC700);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AD63C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AD788);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AD804);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AD928);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AD970);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADA08);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADA64);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADB30);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADC3C);

void func_800ADC70(void) { D_8010AE54 = 0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADC80);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADD4C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADE30);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADEA8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADFC0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE024);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE0BC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE180);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE47C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE4B8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE5B8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE5F0);

s32 func_800AE628(void) { return D_8010B080; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE638);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE8AC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AEA48);

void func_800AF0A0(s32 arg0) { D_8010B174 = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF0B0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF110);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF1A8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF1E8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF24C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF2A4);

static void func_800AF2E4(s32 arg0) {
    if (D_8010B3B8) {
        D_8010B3B8->unk1C = arg0;
    }
}

static void func_800AF304(s32 arg0) {
    if (D_8010B3B8) {
        D_8010B3B8->unk20 = arg0;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF324);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF364);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF3A4);

s32 func_800AF96C(s32 arg0) {
    if (D_8010B3B8) {
        return func_800AE024(arg0, D_8010B3B8);
    }
    return 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF9A0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF9DC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AFCC8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AFFBC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0098);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B017C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B01C4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0200);

s32 func_800B0240(void) { return !D_8010B47C; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0250);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0334);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B04AC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0670);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B075C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0794);

static s32 func_800B0800(void) { return D_8010C808; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0810);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0BF4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0D98);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0E84);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B104C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B10AC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B11C4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B1650);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B190C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B1C80);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B21E4);

static void func_800B22E4(void) { func_800B190C(); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B2304);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B2638);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B271C);

static void func_800BBD0C(void);
void func_800B28CC(s32 arg0) {
    s32 param;

    if (D_8010CAF0 != 3) {
        if (arg0 != 1) {
            param = 0x26;
            if (arg0 == 2) {
                param = 0x2A;
            }
        } else {
            param = 0x29;
        }
    } else {
        param = 4;
    }
    func_800A993C(param, arg0);
    switch (D_8010CAF0) {
    case 3:
        func_800A8FA0();
        D_8010CAF0 = 0;
        break;
    case 1:
        if (func_800A8F48() != 0) {
            func_800A9110();
            func_800BBD0C();
        } else {

            func_800AB988(func_800A9154(), 5);
        }
        break;
    case 2:
        func_800A9018();
        func_800A8FA0();
        break;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B29CC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B2E90);

static void func_800B2F94(s32 arg0) { D_8010CAF0 = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B2FA4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B2FD0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3018);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3044);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B307C);

static s16 func_800B32F0(void) { return D_8010CAFC; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3300);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3350);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B338C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3418);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B37E0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3828);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B392C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B39B4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3C40);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B40B4);

static const s32 D_800A0768[] = {0, 0x28};

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B4244);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B45DC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5274);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5314);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B579C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B57C0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B57DC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B58F8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B59F4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5C7C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5DD8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5E28);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B624C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6348);

static void func_800B63E0(s32 arg0) { D_801159DC = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B63F0);

void func_800B64A0(void) { func_800B63F0(D_801159E0); }

static s32 func_800B64C8(void) { return D_801159E0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B64D8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B650C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6570);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B65A4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B65E0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B667C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6724);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B69A4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6A4C);

void func_800B6AEC(void) {
    if (D_80115A60 != 0) {
        D_80115A60 = 0;
        func_800ADD4C(2);
        func_800B6A4C();
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6B28);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6C84);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6D10);

void func_800B6DCC(void) {
    if (D_80115A60 != 0) {
        D_80115A60 = 0;
        func_800ADD4C(2);
        func_800B6D10();
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6E08);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6E78);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6EFC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7104);

void func_800B7134(void) {
    func_8003408C();
    D_80115A60 = 0;
}

static void func_800B715C(s32 arg0) { D_80115A68 = arg0; }

static s32 func_800B716C(void) { return D_80115A68; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B717C);

s32 func_800B7200(void) {
    if (D_8009D288[0] >= 0x3E8 && D_8009D288[0] < 0x4B0) {
        return 1;
    }
    return 0;
}

static u8 func_800B7218(void) { return D_8009D686; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7228);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7480);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7620);

void func_800B76A8(void) {
    u32 var_a0;

    var_a0 = D_8009D685;
    if (var_a0 >= 3) {
        if (var_a0 >= 0x2B) {
            func_800A993C(0x13);
            func_800BBA5C();
            var_a0 = 3;
        }
        func_800A993C(var_a0);
        func_800BBA5C();
        func_800A31C0(func_800A97A8());
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7714);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B77A8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B77F4);

void func_800B7820(void) {
    D_80116278 = 0;
    D_8009D2E7 = 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7838);

s32 func_800B785C(void) { return D_8011626C; }

s32 func_800B786C(void) { return D_80116270; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B787C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B79B8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7A40);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7AC0);

void func_800B7B1C(u8 arg0) { D_8009D684 = arg0; }

static u8 func_800B7B2C(void) { return D_8009D684; }

u16 func_800B7B3C(void) { return (D_800707BE >> 3) & 1; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7B54);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7B78);

u8 func_800B7BA0(void) { return D_80062F1B >> 7; }

static u8 func_800B7BB0(void) { return D_80062F1A; }

u8 func_800B7BC0(void) { return D_8009D40D & 1; }

static s32 func_800B7BD0(void) { return 1; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7BD8);

static s32 func_800B7C14(void) { return 1; }

static void func_800B7C44(void);

void func_800B7C1C(void) {
    D_80116284 = 0;
    func_800B7C44();
}

static void func_800B7C44(void) { D_8011627C = -0x8C; }

static void func_800B7C58(void) { D_8011627C = -0x1E; }

static void func_800B7C6C(s32 arg0) { D_80116280 = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7C7C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B832C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8488);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B84D8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B851C);

static s32 func_800B857C(void) { return D_800832A0 != 0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B858C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B85D4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B86C4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B86E8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8720);

static void func_800B8750(void) {}

static void func_800B8758(void) {}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8760);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B87D8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B89C4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8A5C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8A98);

static const char D_800A090C[] = "win limit x=";
static const char D_800A091C[] = "win limit y=";

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8B00);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8CBC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8D20);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B8D4C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B90C0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B95E8);

static const char D_800A09AC[] = "No mes data!";
static const char D_800A09BC[] = "mes busy=";

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B962C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B98F0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B9B2C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BA938);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BAA00);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BAB60);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BAC70);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BAE60);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB350);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB450);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB568);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB650);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB7DC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB8B0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB8E8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB9A0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB9D0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BBA0C);

static void func_800BBA34(s8 arg0) { D_801163E0 = arg0; }

s32 func_800BBA44(void) { return (u32)&D_801163E0 < (u32)D_801163E8; }

static void func_800BBA5C(void) {
    s32 sp10[3];
    s32 var_a1;
    s32 var_s0;

    if (func_800A9174() == 5 && D_801163EC) {
        func_800A98A4(1);
        func_800A368C(1);
        return;
    }
    if (func_800A99BC()) {
        func_800AA098(sp10);
        func_800A6994(sp10, func_800A9154() == 3 ? -1 : 1);
        func_800BB9A0(func_800A9174());
        if (func_800A929C()) {
            func_800A8CE4();
            func_800B63F0(2);
            return;
        }
        func_800A8FCC();
        func_800A8F74();
        func_800A9110();
        switch (func_800A9174()) {
        case 3:
            func_800A98A4(1);
            func_800A368C(1);
            func_800B5274();
            if (func_800B64C8() < 6) {
                func_800B63F0(func_800B7200() ? 1 : 3);
            }
            break;
        case 6:
            func_800B65E0(0x1EC);
            break;
        }
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BBBB0);

void func_800BBC4C(void) {
    s32 temp_s0;

    temp_s0 = func_800A9174();
    if (D_801163D4 == 0) {
        if (func_800BBBB0() != 0) {
            if ((temp_s0 == 3) || ((temp_s0 == 5) && (func_800A98E4() != 0))) {
                func_800A368C(-1);
            } else {
                func_800A2108(0, 2);
            }
            D_801163D4 = 1;
            if ((u32)(temp_s0 - 3) < 2U) {
                func_800AB8EC(1);
            }
        }
        if (func_800A9A44() == 0x1B) {
            func_800ABA18(9);
        }
    }
}

static void func_800BBD0C(void) { D_801163D4 = 1; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BBD20);

static s32 func_800BC1AC(void) { return D_801163D4; }

static void func_800BC1BC(s32 arg0) { D_801163EC = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BC1CC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BC420);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BC9E8);

s16 func_800BCA38(void) { return D_801164F8; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCA48);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCA78);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCB2C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCBE8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCECC);
