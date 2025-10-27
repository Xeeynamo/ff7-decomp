#include "battle.h"

typedef struct {
    u8 unk[0x30];
    s32 unk30;
} Unk800BB67C;
typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    u8 raw[0x3C];
} Unk800AF470;

typedef struct {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
    s8 unk4;
    s8 unk5;
    s16 unk6;
    s16 unk8;
    s16 unkA;
} Unk800A2F4C; // size: 0x12

extern s32 D_800E7A38;
extern u8 D_800E7A48[0x10];
extern s8 D_800E7A58[];
extern s32* D_800F01DC;
extern s8 D_800F19A4;
extern s32 D_800F311C;
extern s16 D_800F3122; // part of a struct?
extern s8 D_800F3468;
extern unsigned char D_800F384A[];
extern s16 D_800F3896;
extern s32 D_800F3944;
extern s32 D_800F3948;
extern s32 D_800F3950;
extern s32 D_800F3954;
extern s32 D_800F39E0;
extern s32 D_800F39E4;
extern s32 D_800F39EC;
extern u8 D_800F39F0[][6];
extern s8 D_800F3A80[];
extern u16 D_800F4280[];
extern s32 D_800F4BAC[];
extern u8 D_800F514C[];
extern s8 D_800F5760;
extern Unk800AF470 D_800F5BB8[];
extern u16 D_800F7DC2[1]; // part of a struct
extern u16 D_800F7DC4[1]; // 100% part of a struct
extern u16 D_800F83C6;    // part of struct?
extern u16 D_800F83D0;
extern u16 D_800F9DA4;
extern u8 D_800FAFDC;
extern s32 D_800FAFEC;
extern s32 D_800FAFF0;
extern s8 D_801518DC;
extern s8 D_801518FC[][0xB9C]; // part of a struct
extern u16 D_80163758[];       // part of a struct
extern Unk800A2F4C D_80163798[0x40];
extern u8 D_80163C7C;

const u8 D_800A0000 = 0;
const u8 D_800A0001 = 0;
const u16 D_800A0002 = 0;
const u8 D_800A0004[] = {
    0x14, 0x11, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
    0x21, 0x37, 0x00, 0x00, 0x22, 0x00, 0x06, 0x00, 0x26, 0x00, 0x12, 0x04,
    0x21, 0x0A, 0x00, 0x00, 0x21, 0x09, 0x00, 0x00, 0x28, 0xB1, 0x00, 0x20,
    0x2A, 0x11, 0x00, 0x00, 0x2C, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x24, 0x11, 0x04, 0x00, 0x00, 0x11, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00,
    0x1F, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x15, 0x11, 0x00, 0x00,
    0x16, 0x11, 0x00, 0x10, 0x1C, 0x11, 0x02, 0x00, 0x18, 0x11, 0x00, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x2E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00};
const u8 D_800A0098[] = {
    0x1F, 0x0E, 0x09, 0x1F, 0x00, 0x0C, 0x09, 0x1F, 0x01, 0x0C, 0x09, 0x1F,
    0x02, 0x0D, 0x09, 0x1F, 0x1E, 0x09, 0x1F, 0x0A, 0x16, 0x09, 0x1F, 0x1D,
    0x09, 0x1F, 0x19, 0x09, 0x1F, 0x0E, 0x1C, 0x09, 0x1F, 0x0E, 0x1B, 0x09,
    0x1F, 0x1A, 0x09, 0x1F, 0x17, 0x1F, 0x03, 0x0C, 0x09, 0x1F, 0x1F, 0x1F,
    0x1F, 0x0E, 0x09, 0x1F, 0x04, 0x0B, 0x0F, 0x1F, 0x05, 0x1F, 0x06, 0x0C,
    0x09, 0x1F, 0x00, 0x0C, 0x09, 0x1F, 0x01, 0x0C, 0x09, 0x1F, 0x02, 0x0D,
    0x09, 0x1F, 0x0E, 0x09, 0x1F, 0x12, 0x0E, 0x09, 0x1F, 0x0E, 0x18, 0x09,
    0x1F, 0x10, 0x0E, 0x09, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x0C, 0x09,
    0x1F, 0x08, 0x1F, 0x11, 0x1F, 0x13, 0x09, 0x1F, 0x14, 0x1F, 0x15, 0x0F,
    0x1F, 0x00, 0x00, 0x00};
const s32 D_800A0108 = 21;
const s32 D_800A010C[] = {2, 22, 3, 23, 4};

// entrypoint
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1158);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1798);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A22C0);

void func_800A23BC(s32 arg0) {
    if (D_800E7A38 == arg0) {
        D_800E7A38 = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A23E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A283C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2894);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2974);

void func_800A2B28(s32 arg0) {
    if (arg0 & 1) {
        D_80063014->unk90 |= 0x80;
    }
    if (arg0 & 2) {
        D_80063014->unk90 |= 0x40;
    }
    if (arg0 & 8) {
        D_80063014->unk90 |= 0x04;
    }
    if (arg0 & 0x10) {
        D_80063014->unk90 |= 0x800;
    }
    if (arg0 & 0xE0) {
        D_80063014->unkE8 = (arg0 >> 5) * 10;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2BF4);

void func_800B10F0(s32, s32, s32, s32, s32, s32, s32);
void func_800A2CC4(s32 arg0) {
    func_800B10F0(D_80063014->unk0, arg0, D_80063014->unk28, D_80063014->unk24,
                  D_80063014->unk98, 0, 0);
}

const u8 D_800A01A8[] = {0x05, 0x06, 0x07, 0x12, 0x0F, 0x00, 0x03, 0xA6};
u8 func_800A2D0C(void) {
    s32 temp_v1;

    if (D_80063014->unk208 >= 3) {
        return D_800F83E0[D_80063014->unk208].unk11;
    }
    return D_800A01A8[D_80063014->unkCC];
}

void func_800A2D68(u8 arg0) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (D_80063014->unkD0[i] == 0xFF) {
            D_80063014->unkD0[i] = arg0;
            return;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2DB0);

void func_800A2EFC(void) {
    D_800F3950 = D_800F3944;
    D_800F3954 = D_800F3948;
}

void func_800A2F24(void) {
    D_800F3944 = D_800F3950;
    D_800F3948 = D_800F3954;
}

Unk800A2F4C* func_800A2F4C(void) {
    Unk800A2F4C* unk = &D_80163798[D_800F3944];
    unk->unk3 = 0;
    unk->unk2 = 0;
    unk->unkA = D_800F3948;
    if (D_800F3944 < LEN(D_80163798)) {
        D_800F3944++;
    } else {
        func_800155A4(40);
    }
    return unk;
}

s8* func_800A2FD0();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2FD0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A304C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A311C);

void func_800A317C(void) {
    s8* ret = func_800A2FD0();
    *ret = -1;
}

void func_800A31A0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Unk800A2F4C* unk = func_800A2F4C();
    unk->unk0 = arg0;
    unk->unk1 = arg1;
    unk->unk5 = arg2;
    unk->unk6 = arg3;
    unk->unk8 = -1;
    unk->unkA = -1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3208);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3240);

void func_800A3278(void) {
    D_800F3944 = 0;
    D_800F3948 = 0;
    D_80163798[0].unk0 = -1;
}

void func_800A329C(void) {
    if (D_800F3944) {
        D_800F3944--;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A32C0);

void func_800A3354();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3354);

void func_800A345C(void) {
    if (D_800F3944) {
        func_800A3354();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3488);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A34CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3534);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A35F8);

s32 func_800A37F8(s32 arg0) {
    if (arg0 >= 0) {
        D_800F39E0 = arg0;
        D_800F39E4 = 0;
        return 0;
    }
    return D_800F39E0;
}

s32 func_800A3828(void) {
    s32 ret = 0;
    if (D_800F39E0 == 3) {
        D_800F39E4 += D_800F7DA6[0];
    }
    if (D_800F7DA6[2] == 2) {
        switch (D_800F3896) {
        case 0:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 24:
        case 26:
        case 27:
            ret = 1;
            break;
        }
    }
    ret |= func_800A4A80();
    ret |= (D_800F83D0 & 3) ? 1 : 0;
    if (D_800F39E4 > 0x4000) {
        ret = 1;
    }
    return ret;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A38FC);

typedef struct {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
    s16 unk4;
    s16 unk6;
} Unk800A3D4C; // size:8
void func_800A3D4C(Unk800A3D4C*);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3D4C);

void func_800A3E98(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    Unk800A3D4C sp;

    sp.unk2 = arg0;
    sp.unk0 = arg1;
    sp.unk3 = arg2;
    sp.unk4 = arg3;
    sp.unk6 = arg4;
    func_800A3D4C(&sp);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3ED0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4350);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4480);

s32 func_800A44D8(s32 arg0);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A44D8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4540);

void func_800A4844(s32 arg0) {
    s32 var_v0 = arg0 ? 3 : 1;
    D_800F39EC = var_v0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4860);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4954);

s32 func_800A4A80(void) {
    s32 ret;

    ret = 1;
    if (D_80163C7C > 3 && D_80163C7C < 6 && !(D_800F9DA4 & 1)) {
        if (D_800FAFDC) {
            ret = 1;
        } else {
            ret = 0;
        }
    }
    return ret;
}

void func_800A4ACC(s16 arg0, u16 arg1) { func_8001726C(arg0, arg1); }

void func_800B6D6C();
void func_800A4AF4(void) {
    while (D_800F9DA4 & 2) {
        func_800B6D6C();
    }
}

u8 func_800A4B3C(s32 index, s32 arg1) {
    if (arg1 != -1) {
        D_801518FC[index][19] = arg1;
    }
    return D_801518FC[index][19];
}

void func_800A4B9C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4BA4);

u8 func_800A4CA8(s32 arg0) { return D_800F39F0[arg0][0]; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4CC8);

void func_800A4D2C(s32 arg0) {
    u32 i;

    if (D_800F83C6) {
        return;
    }
    for (i = 0; i < LEN(D_800E7A48); i++) {
        if (D_800E7A48[i] == arg0) {
            return;
        }
        if (D_800E7A48[i] == 0xFF) {
            D_800E7A48[i] = arg0;
            return;
        }
    }
}

void func_800A4D88(s32 arg0) {
    u32 i;

    for (i = 0; i < LEN(D_800E7A48); i++) {
        if (D_800E7A48[i] == arg0) {
            for (; i < LEN(D_800E7A48) - 1; i++) {
                D_800E7A48[i] = D_800E7A48[i + 1];
                if (D_800E7A48[i] == 0xFF) {
                    break;
                }
            }
            return;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4E00);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4E40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4E80);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4F14);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4F60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A50E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5250);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A555C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A55BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A55F4);

void func_800A5660(s32, s16);
;
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5660);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A56B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5750);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5990);

const u8 D_800A0240[] = {
    0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0x54, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0xA8, 0x54, 0x0A, 0x80, 0x94, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0x14, 0x54, 0x0A, 0x80,
    0x34, 0x54, 0x0A, 0x80, 0x74, 0x54, 0x0A, 0x80};
const u8 D_800A0278[] = {
    0x05, 0x06, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x00, 0x5C, 0x5A,
    0x0A, 0x80, 0x88, 0x5A, 0x0A, 0x80, 0xA8, 0x5A, 0x0A, 0x80};
u8 func_800A5A5C(void) { return D_800A0278[func_80014BA8(7)]; }

s32 func_800A5A88(void) { return func_80014BA8(54); }

s32 func_800A5AA8(void) { return func_80014BA8(16) + 56; }

const u8 D_800A028C[] = {0x02, 0xFF, 0x01, 0x86};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5AC8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5BC8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5E0C);

s32 func_800A5EB0(s32, s16*);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5EB0);

s8* func_800A5F90(s32 arg0) { return &D_800F3A80[D_800F4280[arg0]]; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5FB0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6000);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A61D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6278);

void func_800A64A0(s32 arg0, s8 arg1) { D_800E7A58[arg0] = arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A64B4);

void func_800A653C(s32 arg0) {
    s32 index = func_800A44D8(arg0);
    D_800F5BB8[index].unk4 = 0;
    D_800F7DC2[0] &= ~(1 << index);
}

void func_800A6590(s32 arg0) { func_800A4D88(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A65B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A661C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A66A4);

void func_800A6720(s32 arg0, s16 arg1) { func_800A5660(10, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6748);

void func_800A6798(s32 arg0, s32 arg1) { func_800A37F8(arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A67B8);

void func_800A6834(s32 arg0) { D_800F7DC4[0] &= ~(1 << arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6858);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A68FC);

void func_800A6A3C(s32 arg0, s32 arg1) { D_800F5BB8[arg0].raw[6] |= arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6A70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6AC4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6B1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6B88);

void func_800A6BFC(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6C04);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6C5C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6CC0);

void func_800A6D10(s32 arg0) { func_800A3E98(arg0, 3, 2, 54, 0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6D3C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6D88);

void func_800A6DFC(void) {}

void func_800A6E04(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6E0C);

void func_800A7254(s32, s32, s32, s32);
void func_800A6E6C(s32 arg0, s32 arg1) { func_800A7254(0, arg0, 13, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6E9C);

void func_800B0FFC(s32, s32, s32, s16*);
void func_800A7034(s32 arg0, s16 arg1) {
    s16 out = arg1;
    func_800B0FFC(arg0, 0x53, 1, &out);
}

void func_800A7060(s32 arg0, s32 arg1) { func_800A7254(0, arg0, 12, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7090);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A70C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7130);

void func_800A71E0(void) {}

s32 func_800A71E8(s32 arg0) { return (arg0 + 1) & 0x7F; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A71F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7254);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A72C8);

void func_800A73C0(void) { D_80063014->unk2C = D_80063014->unk10; }

void func_800A73D8(void) { D_80063014->unk2C = D_80063014->unk10 + 56; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A73F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7458);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7560);

void func_800A76AC(void) { D_80063014->unk2C = D_80063014->unk10 + 72; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A76CC);

void func_800A7784(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A778C);

const s16 D_800A0290[] = {0, 56, 72, 96, 256};
const s32 D_800A029C[] = {
    0x140D0302, 0x3D3CFFFF, 0x41403F3E, 0xFFFFFF42, 0xFFFFFFFF,
    0x43424140, 0x47464544, 0xFF444843, 0xFFFFFFFF};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A784C);

void func_800A7940(void) {
    D_80063014->unk80 = 0x400000;
    D_80063014->unkE4 = 0x59;
}

void func_800B1060(s32);
void func_800A795C(void) { func_800B1060(D_80063014->unk10); }

void func_800AF9C8();
void func_800A7988(void) { func_800AF9C8(); }

void func_800A79A8(void) {
    D_80063014->unk50 = 0;
    D_80063014->unk18 = 1 << D_80063014->unk0;
}

void func_800A79CC();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A79CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A80DC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A81B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8424);

void func_800A8528(void) { D_80063014->unkB4 = 4; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A853C);

void func_800A85A0(void) { D_80063014->unkB4 = 2; }

void func_800A85B4(void) {
    D_80063014->unk44 = 0x10;
    D_80063014->unk48 = 1;
    D_80063014->unk50 = 0;
    if (!((D_80163758[1] >> D_80063014->unk0) & 1)) {
        D_80063014->unk20 = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A85FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A866C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8A6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8C94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8CC8);

void func_800A8D04(void) { D_80063014->unk48 = 2; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8D18);

void func_800A8D60(s32 arg0) {
    if (D_80063014->unk50 == 0xFF) {
        D_80063014->unk50 = arg0;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8D88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8DCC);

void func_800A8E34(void) { func_800A79CC(); }

void func_800A8E54(s32 arg0) {
    D_80063014->unkF8 = arg0;
    D_80063014->unkAC = arg0 + 3;
    if (D_80063014->unkAC > 8) {
        D_80063014->unkAC = 8;
    }
}

const s16 D_800A02C0[] = {
    0x04, 0x3C, 0x04, 0x20, 0x01, 0x24, 0x10, 0x10, 0x04, 0x02, 0x02, 0x02,
    0x02, 0x01, 0x20, 0x04, 0x24, 0x10, 0x10, 0x04, 0x20, 0x10, 0x10, 0x10,
    0x30, 0x10, 0x20, 0x10, 0x10, 0x14, 0x01, 0x01, 0x01, 0x01, 0x01, 0x18};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8E84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A9C24);

const u8 D_800A0398[] = {0x64, 0x14, 0x14, 0x14, 0xEC, 0xCE, 0xCE, 0x00};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A9DA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA1C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA468);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA4FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA574);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA688);

s32 func_800AA6E8(s32 arg0, s32 arg1) {
    arg0 = arg0 < 4 ? 1 : 0;
    if (arg1 < 4) {
        arg0++;
    }
    return arg0 & 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA700);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA738);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA950);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AABBC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB308);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB674);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB788);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB830);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB9C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ABA68);

const s32 D_800A03A0[] = {0x200, 0x100, 0x010, 0x020};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ABB0C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AC6B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AC73C);

void func_800ACA24(void) {
    D_80063014->unk238 = 0;
    D_80063014->unk23C = 0;
    D_80063014->unk240 = 0;
    D_80063014->unk244 = 0;
    D_80063014->unk230 = 0;
    D_80063014->unk214 = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACA4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACB98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACD88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACE14);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACE88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD0FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD324);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD420);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD4EC);

const s8 D_800A04B0[] = {
    0x0A, 0x0B, 0x0C, 0x0D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x7F, 0x03, 0x34};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD5E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD73C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD804);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD890);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD8DC);

void func_800AD924(void) { D_80063014->unk218 |= 2; }

void func_800AD944();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD944);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADBBC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADC70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADD2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADDE8);

void func_800ADE5C(void) { D_80063014->unk214 = D_80063014->unk48 * 20; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADE84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADED8);

void func_800ADF04(void) {
    D_80063014->unk4C = D_80063014->unkD8 * 2;
    func_800AD944();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADF38);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADFC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADFF4);

void func_800AE050(void) {}

void func_800AE058(void) {}

void func_800AE060(void) {}

void func_800AE068(void) {}

void func_800AE070(void) {}

void func_800AE078(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE080);

void func_800AE234(void) {
    D_80063014->unk214 = _work.D_8009D2A2 + _work.D_8009D2A3 * 256;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE25C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE2A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE318);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE378);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE42C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE6C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE764);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE82C);

void func_800AE954();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE954);

const s8 D_800A04BC[] = {
    0x1E, 0x14, 0x3C, 0x1E, 0x7F, 0x7F, 0x0A, 0x64, 0x7F, 0x7F,
    0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x0D, 0x00, 0x00};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEB20);

// this data belong to functions located above:
const s8 D_800A04D0[] = {0x0A, 0x19, 0x15, 0x0D, 0x10, 0x11, 0x03, 0x02,
                         0x0F, 0x1B, 0x14, 0x18, 0xFF, 0xFF, 0xFF, 0xFF};
int func_800B0378();
int func_800B062C();
int func_800B079C();
int func_800B0B8C();
int func_800B0910();
int func_800B089C();
int (* const D_800A04E0[])() = {
    func_800B0378, func_800B062C, func_800B079C, func_800B0B8C,
    func_800B0B8C, func_800B0B8C, func_800B0910, func_800B089C,
};
// ___end

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEB80);

void func_800AEBF0(void) { func_800AE954(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEC10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEF68);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF0C4);

void func_800A7254(s32, s32, s32, s32);
void func_800AF1A8(s32 arg0) { func_800A7254(0, arg0, 8, 0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF1D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF264);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF320);

void func_800AF380(s32 arg0) { func_800A7254(2, arg0, 0x15, 0xF); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF3AC);

void func_800AF470(s32 arg0) { D_800F5BB8[arg0].raw[0x20] = 3; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF494);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF594);

void func_800B108C(s32 arg0);
void func_800AF63C(s32 arg0) { func_800B108C(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF65C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF834);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF874);

void func_800AF9C8();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF9C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AFE98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AFECC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0170);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0234);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0378);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B062C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B079C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B089C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0910);

int func_800B0B8C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0B94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0C14);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0DF8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0E5C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0EB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0F04);

void func_800B0FFC(s32 arg0, s32 arg1, s32 arg2, s16* arg3) {
    func_800A31A0(
        arg0, 2, arg2, func_800A5EB0(func_8001521C(arg1), arg3) + 0x100);
}

void func_800B1060(s32 arg0) { func_800A31A0(10, 2, 1, arg0); }

void func_800B108C(s32 arg0) { func_800A31A0(arg0, 5, 0, 0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B10B4);

void func_800B10F0(s32, s32, s32, s32, s32, s32, s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B10F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B11B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1218);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1268);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B12DC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1368);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B13B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B141C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B153C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1624);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B16D0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B17F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B18A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1A5C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1AA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1B64);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1C94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1D48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2A2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2B5C);

u16* func_800B2EBC();
s32 func_800B2C60(s32 arg0) {
    s32 var_s0;
    u16* ret;

    var_s0 = 0;
    if (arg0 <= 0xFFFE) {
        ret = func_800B2EBC();
        if (ret) {
            var_s0 = ret[2];
        }
    }
    return var_s0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2CAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2CFC);

u16* func_800B2EBC();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2EBC);

u8 func_800B2F30(void) { return func_80014B70(); }

u16 func_800B2F50(void) { return func_80014BE4(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2F70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2FC4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3000);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3030);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B30E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B33A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B36B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B37A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B37EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B383C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B38E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3934);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3968);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3A04);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3AB8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3B84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3C50);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3CD0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3D38);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3D88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3DBC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3E2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3FAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3FFC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B430C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B45F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B46B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B4794);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B4E30);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B5138);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B54B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B588C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B5AAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B5C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B5CD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B5D38);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B5E64);

void func_800B60E0(s16);
void func_800B5FC4(s16 arg0) { func_800B60E0(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B5FE8);

void func_800B60E0(s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B60E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B64CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B677C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B6B98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B6D6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B7764);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B798C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B79F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B7DB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B7F6C);

void func_800B7FB4(void) { D_801518DC = func_80034B44(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B7FDC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8234);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8268);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8360);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B83C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8438);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B85E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B888C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B88CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8944);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8A34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8B48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8E48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8EE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B8FCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B905C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B91CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B950C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B9568);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BA11C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BA24C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BA2BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BA360);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BA40C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BA4C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BA598);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BACEC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BAF34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BAFF8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB030);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB2A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB430);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB4F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB538);

void func_800BB67C(s32 arg0, Unk800BB67C* arg1) { arg1->unk30 = arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB684);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB75C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB804);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB89C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB90C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB944);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB978);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB9B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BB9FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BBA40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BBA84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BBB20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BBDF8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BBEAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BBF7C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC04C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC11C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC1E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC2F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC348);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC440);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC538);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC630);

void func_800BCA58(s32);
void func_800C1104();
void func_800BC72C(void) {
    func_800C1104();
    func_800BCA58(3);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC754);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC81C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BC8B0);

void func_800BCA58(s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BCA58);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BCB1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BE49C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BE69C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BE86C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BEA38);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BFA98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BFB10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BFB88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BFDA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800BFF88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C018C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0254);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0314);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C03B8);

s32 func_800C03FC(s32 arg0, s32 arg1) { return arg0 < 0 ? -arg1 : arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0410);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0630);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0900);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0970);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0B20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C0DD8);

void func_800C1104();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C1104);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C1304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C1394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C14C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C169C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C17A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C1908);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C1D8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C2000);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C20E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C2150);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C223C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C2704);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C2864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C2928);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C2C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C2F20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C2FD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C3068);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C328C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C33F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C3578);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C36B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C3950);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C3AA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C3CA8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C3DE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C3F44);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C40F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C428C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C44B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C45EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C4814);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C494C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C4B60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C4D10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C4DC8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C4FC8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5004);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5040);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5170);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5468);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C55B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5694);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C57B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C59B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5ADC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5BEC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5C18);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5CC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C5E94);

s32 func_800C60F4(void) { return _work.battle_msg_speed / 4 + 4; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C610C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C614C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C61C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C627C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C62F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C64AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C64D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C6628);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C679C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C6CB8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C70AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C7220);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C7340);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C74A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C74E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C76C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C7924);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C7B60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C7BE8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800C7C4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CD400);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CD558);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CD5E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CD798);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CD82C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CD860);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CDD44);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CDDA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CDDE8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CDE78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CDF6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CDFC4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CE058);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CE0C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CE21C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CE384);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CE638);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CE75C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CE7E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CE970);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CEB48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CF2F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CF3CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CF4A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CF5BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CF8C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CFB14);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CFCB0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800CFE60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D01C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D0578);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D061C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D06B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D0760);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D088C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D08B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D0958);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D09D0);

void func_800D0A44(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D0A4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D0AD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D0B4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D0C80);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D1110);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D1530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D2538);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D2564);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D2590);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D25C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D25E8);

void func_800D26F8(void) { D_800F01DC = D_800F4BAC; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D2710);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D2828);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D2980);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D29D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D32B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3354);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3418);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3474);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D34C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3520);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3548);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D35D8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3658);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D376C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3994);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3A6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3AF0);

const s32 D_800A0D98[] = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                          0x00001000, 0x00000000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3BF0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3D88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3E8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3F0C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D3F8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D415C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D41FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4284);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4368);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4484);

const s32 D_800A0DB8[] = {0x00000000, 0xFFFFF000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D44E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D461C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4710);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D491C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4A64);

void func_800D4D6C(s32 arg0, s32 arg1, s32 arg2);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4C08);

void func_800D4CBC(s32, s32, s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4CBC);

void func_800D4D4C(s32 arg0, s32 arg1) { func_800D4CBC(arg0, arg1, 0x1000); }

void func_800D4D6C(s32 arg0, s32 arg1, s32 arg2) {
    func_800D4C08(arg0, arg1, 0x1000, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4D90);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4FA8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D4FF0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D508C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5138);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D51D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5230);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D52A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5350);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5444);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D54BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D54EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D55A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D55F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D56A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D574C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5774);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D57C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D58D0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5938);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5A68);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5AC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5B6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D5D28);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D61AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6260);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D650C);

extern u8 D_800F10EC[];
extern u8 D_800F11E8[];
extern u8 D_800F1304[];
u8* const D_800A0DC8[] = {D_800F10EC, D_800F11E8, D_800F1304};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6734);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D67BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D67E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6814);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6840);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6998);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6ACC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6C20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6D8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D6F78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D70C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7178);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D72B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7340);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7368);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D751C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D76B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7724);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7888);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7A88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7B1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7BA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7C2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D7D3C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D8304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D83A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D8468);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D84F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D85B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D87EC);

void func_800D8A04(void) {}

s32 func_800D8A0C(s32 arg0) { return arg0 < 0 ? -arg0 : arg0; }

void func_800D8A24(void) {}

extern s32 D_800F199C;

void func_800D8A2C(void) { D_800F199C = 0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D8A3C);

void func_800D8A70(void) {}

void func_800D8A78(s8 arg0) { D_800F19A4 = arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D8A88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D8AF0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D8B2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D8B60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D8D78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D91DC);

void func_800D93DC(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D93E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D9BF4);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D9DEC);
#else
//! PSYQ=3.3 CC1=2.7.2
u8 func_800D9DEC(s16 arg0) { return D_800F514C[arg0]; }
#endif

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D9E0C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D9F5C);

void func_800D9F80(void) {
    D_800F311C = 0;
    D_800FAFEC = -0x100;
    D_800FAFF0 = -0x100;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800D9FA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DA380);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DB818);

const s32 D_800A0E48[] = {0, 0, 0};
void func_800DBC18(s32, s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DBC18);

void func_800DBEA4(s32 arg0, s16 arg1) { func_800DBC18(arg0, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DBEC8);

void func_800DBF54(void) { func_800269C0(D_80077F64); }

void func_800DBF7C(void) { D_800F3122 = 0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DBF8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DC0CC);

void func_800DCF58(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DCF60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DCF94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DCFD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DD690);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DD85C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DDAD8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DDC34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DDCE8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DDE90);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DDFEC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DE2B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DE3CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DE46C);

void func_800DE5D8(void) {}

void func_800DE5E0(void) {}

void func_800DE5E8(void) {}

void func_800DE5F0(void) {}

void func_800DE5F8(void) {}

void func_800DE600(void) {}

void func_800DE608(void) {}

void func_800DE610(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DE618);

void func_800DE910(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DE918);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DE94C);

void func_800DEB10(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DEB18);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DEC10);

void func_800DF244(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DF24C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DF2CC);

void func_800DF528(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DF530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DF5C8);

void func_800DF7BC(void) {}

void func_800DF7C4(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DF7CC);

void func_800DF8F0(void) {}

void func_800DF8F8(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DF900);

void func_800DF9F0(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DF9F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DFA94);

void func_800DFC38(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DFC40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DFE34);

void func_800DFFDC(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800DFFE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E010C);

void func_800E026C(void) {}

void func_800E53C8();
void func_800E0274(void) { func_800E53C8(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E0294);

void func_800E03C8(void) {}

void func_800E4B88();
void func_800E03D0(void) { func_800E4B88(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E03F0);

void func_800E0528(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E0530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E05E4);

void func_800E078C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E0794);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E084C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E08C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E0BE0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E0DF4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E0E34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E15D8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E16B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E1938);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E1A2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E1AC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E1C40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E2054);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E2098);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E2C6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E3088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E33A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E368C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E3B64);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E3E10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E3FB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E4180);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E4394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E4A64);

void func_800E4B88();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E4B88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E4BCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E4C08);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E5358);

void func_800E53C8();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E53C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E54EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E5530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E5814);

void func_800E58B0(void) {
    D_800F3468 = 0;
    D_800F5760 = 10;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E58CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E593C);

s32 func_800E5960(s32 arg0) { return arg0 < 0 ? -arg0 : arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E5978);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E5F30);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E5F70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E5FB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E6018);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E60F8);

s32 func_800E6820(void) { return func_80026B70(D_800F384A); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E6848);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E68B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E6904);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E6B40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E6B94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E6DCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800E7170);
