//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"

static void func_800D67BC(s32 arg0);
static void func_800D67E8(s32 arg0);
void func_800D6814(s32 arg0);

extern Yamada D_800EEBB8[]; // MAGIC/*.BIN overlay
extern s16 D_800EF5B0[];
extern s16 D_800EF63C[];
extern s16 D_800EF6A8[];
extern s16 D_800EF6D8[];
extern s16 D_800EF6FC[];
extern s16 D_800EF838[];
extern s16 D_800EF8D8[];

s32 func_800BBF7C(void (*f)(void));
s32 func_800BC04C(void (*f)());
void func_800C2928();
void func_800C328C();
void func_800C3578();
void func_800C3950();
void func_800C3CA8();
void func_800C40F4();
void func_800C44B4();
void func_800C4814();
void func_800CFB14();
void func_800D1530();
s32 func_800D376C(BattleModelSub* arg0, s32 arg1, s16 nItems, u8* arg3);
void func_800D3AF0();
static void func_800D4D4C(s32 arg0, s32 arg1);

// MAGIC/ entrypoints
void func_801B037C(s16, u8);
void func_801B0000(s16, u8);
void func_801B0000_2(s16, u8);
void func_801B000C(s16, u8);
void func_801B0040(s16, u8);
void func_801B0054(s16, u8);
void func_801B0084(s16, u8);

static s32 func_800C7B60(s16 arg0, s16 nItems, u8* arg2) {
    BattleModelSub* var_a0;
    s32 var_a1;
    s32 temp_s0;
    s32 temp_v0;

    var_a0 = D_801518E4[arg0].D_80151A58;
    var_a1 = D_801518E4[arg0].D_80151958;
    D_801518E4[arg0].D_80151958 = func_800D376C(var_a0, var_a1, nItems, arg2);
    return D_801518E4[arg0].D_80151958 == 0;
}

static void func_800C7BE8(s16 arg0, s16 arg1, u8* arg2) {
    D_800FA6D8[arg0].unk0 = func_800D376C(
        &D_800FA6D8[arg0].unk8, D_800FA6D8[arg0].unk0, arg1, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800C7C4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD400);

s16 func_800CD558(s16 arg0, u8* arg1) {
    u32 val;
    val = arg1[D_801518E4[arg0].D_80151920++];
    return (arg1[D_801518E4[arg0].D_80151920++] << 8) + val;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD5E4);

static void func_800CD798(u8 arg0) {
    D_801518E4[arg0].D_80151A4C.vx = D_80163C80[arg0].vx;
    D_801518E4[arg0].D_80151A4C.vy = D_80163C80[arg0].vy;
    D_801518E4[arg0].D_80151A4C.vz = D_80163C80[arg0].vz;
}

static void func_800CD82C(void) {
    D_801590DC = 1;
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD860);

static void func_800CDD44(s16 arg0) {
    D_801518E4[arg0].D_8015191F = 1;
    D_801518E4[arg0].D_80151920 = 0;
    D_801518E4[arg0].D_80151921 = 0;
}

void func_800CDDA4(void) {
    if (D_80166F68) {
        D_800F9DA4 = D_800F9DA4 | 1;
    } else {
        D_800F9DA4 = D_800F9DA4 & ~1;
    }
}

static void func_800CDDE8(void) {
    func_800BBA84(D_801621F0[D_801590D4].D_801621F4, D_801590CC, 0);
    func_800D7178(D_801590CC, D_801621F0[D_801590D4].D_801621F2);
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

static void func_800CDE78(void) {
    if (D_801516FC[D_801590CC][0] <
        D_801516FC[D_801621F0[D_801590D4].unk8][0]) {
        D_801621F0[D_801590D4].unk14 |= 0x100;
    }
    func_800D4D4C(D_801621F0[D_801590D4].unk8, D_801621F0[D_801590D4].unk14);
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

void func_800CDF6C(s32 arg0, s16 arg1) {
    func_800BBA84(0x14, arg1, 0);
    func_800D58D0(arg0, 1, 1);
    func_800D4D4C(D_800FA6D4, 0x2C);
}

static void func_800CDFC4(void) {
    if (!D_801621F0[D_801590D4].D_801621F4) {
        D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

static void func_800CE058(s16 arg0) {
    Unk80151200* dst = &D_80151200[arg0];
    dst->D_8015123C = 0x1000;
    dst->D_8015123A = 0x1000;
    D_80151200[arg0].D_80151238 = 0x1000;
    D_80151200[arg0].D_8015120C |= 0x20;
}

static void func_800CE0C8(s16 arg0, u8 arg1, u8 arg2) {
    s32 ret;

    func_800CE058(arg0);
    switch (arg1) {
    case 0:
        ret = func_800BBEAC(func_800C3578);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg2;
        break;
    case 8:
        ret = func_800BBEAC(func_800C4814);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg2;
        break;
    case 1:
    case 10:
        ret = func_800BBF7C(func_800C3950);
        D_801620AC[ret].D_801621B2 = arg2;
        D_801620AC[ret].D_801621B4 = arg0;
        D_801620AC[ret].D_801621B6 = 0xF8;
        return;
    case 2:
        ret = func_800BBEAC(func_800C3CA8);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg2;
        break;
    case 3:
        ret = func_800BBEAC(func_800C328C);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg2;
        break;
    case 17:
    case 18:
        ret = func_800BBEAC(func_800C40F4);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg2;
        break;
    case 7:
        ret = func_800BBEAC(func_800C44B4);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg2;
        break;
    }
}

static void func_800CE21C(s16 arg0, u8 arg1) {
    s32 ret;

    func_800CE058(arg0);
    switch (D_801636B8[arg0].D_801636BC) {
    case 0:
        ret = func_800BBEAC(func_800C3578);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg1;
        break;
    case 8:
        ret = func_800BBEAC(func_800C4814);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg1;
        break;
    case 1:
    case 10:
        ret = func_800BBF7C(func_800C3950);
        D_801620AC[ret].D_801621B2 = arg1;
        D_801620AC[ret].D_801621B4 = arg0;
        D_801620AC[ret].D_801621B6 = 0xF8;
        break;
    case 2:
        ret = func_800BBEAC(func_800C3CA8);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg1;
        break;
    case 3:
        ret = func_800BBEAC(func_800C328C);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg1;
        break;
    case 17:
    case 18:
        ret = func_800BBEAC(func_800C40F4);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg1;
        break;
    case 7:
        ret = func_800BBEAC(func_800C44B4);
        D_80162978[ret].D_80162980 = arg0;
        D_80162978[ret].D_8016297E = arg1;
        break;
    }
}

static void func_800CE384(void) {
    u8* ptr;
    u8 do_work;
    u32 param;
    u32 param_hi;

    if (D_80151200[D_801621F0[D_801590D4].D_801621F6].D_80151234 !=
        D_801621F0[D_801590D4].D_801621F2) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    if (D_801621F0[D_801590D4].D_801621F4) {
        D_801621F0[D_801590D4].D_801621F4--;
        return;
    }
    do_work = 1;
    while (do_work) {
        ptr = D_801621F0[D_801590D4].unk10.ptr;
        switch (ptr[D_801621F0[D_801590D4].unk18++]) {
        case 0xFD:
            param = ptr[D_801621F0[D_801590D4].unk18++];
            param_hi = ptr[D_801621F0[D_801590D4].unk18++];
            param_hi <<= 8;
            param |= param_hi;
            D_801621F0[D_801590D4].D_801621F4 =
                ptr[D_801621F0[D_801590D4].unk18++];
            D_80151200[D_801621F0[D_801590D4].D_801621F6].D_8015122E = param;
            do_work = 0;
            break;
        case 0xFE:
            D_801621F0[D_801590D4].unk18 = 0;
            break;
        case 0xFF:
            D_801621F0[D_801590D4].D_801621F0 = -1;
            do_work = 0;
            break;
        default:
            do_work = 0;
            break;
        }
    }
}

static void func_800CE638(void) {
    switch (D_801621F0[D_801590D4].D_801621F2) {
    case 0:
        D_801621F0[D_801590D4].D_801621F4 = 3;
        D_801621F0[D_801590D4].D_801621F2++;
    case 1:
        if (D_801621F0[D_801590D4].D_801621F4 == 0) {
            D_801621F0[D_801590D4].D_801621F0 = -1;
            func_800A3534(
                D_801621F0[D_801590D4].unkA, D_801621F0[D_801590D4].unk8);
        }
        D_801621F0[D_801590D4].D_801621F4--;
        break;
    }
}

static void func_800CE75C(void) {
    if ((D_800FA69C >> D_80162978[D_8015169C].D_80162980) & 1) {
        func_800D67E8(D_80162978[D_8015169C].D_80162980);
    } else if ((D_80163608 >> D_80162978[D_8015169C].D_80162980) & 1) {
        func_800D67BC(D_80162978[D_8015169C].D_80162980);
    }
}

static void func_800CE7E0(void) {
    s32 dst;

    if (!D_80162978[D_8015169C].D_8016297C) {
        if (D_80162978[D_8015169C].D_8016297E != -1) {
            func_800CE75C();
            dst = func_800BC04C(func_800C2928);
            D_801621F0[dst].unk14 = D_80162978[D_8015169C].unkA;
            D_801621F0[dst].unkE = D_80162978[D_8015169C].D_80162982;
            D_801621F0[dst].unk10.ptr = (u8*)D_80162978[D_8015169C].D_80162980;
            dst = func_800BC04C(func_800CE638);
            D_801621F0[dst].unkA = D_80162978[D_8015169C].unk15;
            D_801621F0[dst].unk8 = D_80162978[D_8015169C].D_8016297E;
        }
        D_80162978[D_8015169C].D_80162978 = -1;
        return;
    } else {
        D_80162978[D_8015169C].D_8016297C--;
    }
}

void func_800CEB48(void);
void func_800CE970(void) {
    s32 dst;

    if (!D_80162978[D_8015169C].D_8016297C) {
        if (D_80162978[D_8015169C].unkA & 2) {
            D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        }
        if (D_80162978[D_8015169C].D_8016297E != -1 &&
            D_80162978[D_8015169C].unk14 != 1) {
            func_800CE75C();
            dst = func_800BC04C(func_800C2928);
            D_801621F0[dst].unk14 = D_80162978[D_8015169C].unkA;
            D_801621F0[dst].unkE = D_80162978[D_8015169C].D_80162982;
            D_801621F0[dst].unk10.ptr = (u8*)D_80162978[D_8015169C].D_80162980;
            dst = func_800BC04C(func_800CE638);
            D_801621F0[dst].unkA = D_80162978[D_8015169C].unk15;
            D_801621F0[dst].unk8 = D_80162978[D_8015169C].D_8016297E;
        }
        func_800CEB48();
        return;
    } else {
        D_80162978[D_8015169C].D_8016297C--;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CEB48);

static void func_800CF2F0(void) {
    s16 index;

    if (D_801620AC[D_801590D0].D_801621B0 == 0) {
        D_801620AC[D_801590D0].D_801621AC = -1;
        return;
    }
    index = D_801620AC[D_801590D0].D_801621B4;
    D_801518E4[index].D_80151A4C.vy += D_801620AC[D_801590D0].D_801621B6;
    *(s32*)0x1F80000C = index;
    D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].D_801621B0 - 1;
}

static void func_800CF3CC(void) {
    s16 index;

    index = D_801620AC[D_801590D0].D_801621B4;
    *(s32*)0x1F80000C = index;
    if (D_801620AC[D_801590D0].D_801621B0 == 0) {
        D_801620AC[D_801590D0].D_801621AC = -1;
        return;
    }
    D_801518E4[index].unk160.vy += D_801620AC[D_801590D0].unkA;
    D_801620AC[D_801590D0].D_801621B0--;
}

static void func_800CF4A8(void) {
    s16 index;

    if (D_801620AC[D_801590D0].D_801621B0 == 0) {
        D_801620AC[D_801590D0].D_801621AC = -1;
        return;
    }
    index = D_801620AC[D_801590D0].D_801621B4;
    D_801518E4[index].D_80151A4C.vx += D_801620AC[D_801590D0].D_801621B6;
    D_801518E4[index].D_80151A4C.vz += D_801620AC[D_801590D0].unk8;
    *(s32*)0x1F80000C = index;
    D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].D_801621B0 - 1;
}

void func_800CF5BC(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX1 = D_801620AC[D_801590D0].D_801621B4;
    IDX2 = D_801620AC[D_801590D0].D_801621B6;
    if (D_801620AC[D_801590D0].D_801621AE == 0) {
        D_801620AC[D_801590D0].D_801621AE = 1;
        if (IDX1 >= 4) {
            if (D_801031F0 == 0) {
                D_801620AC[D_801590D0].D_801621B2 =
                    (MUL(D_801518E4[IDX2].D_80151A4C.vy,
                         D_801518E4[IDX2].D_801518EA) -
                     MUL(D_801518E4[IDX1].D_80151A4C.vy,
                         D_801518E4[IDX1].D_801518EA)) /
                    D_801620AC[D_801590D0].D_801621B0;
            } else {
                D_801620AC[D_801590D0].D_801621B2 = 0;
            }
        } else {
            D_801620AC[D_801590D0].D_801621B2 = 0;
        }
    }
    if (!D_801620AC[D_801590D0].unk14) {
        D_80166F58 = 0;
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
        D_801518E4[IDX1].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
        D_801518E4[IDX1].D_80151A4C.vy += D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        return;
    }
    D_801620AC[D_801590D0].unk14--;
#undef IDX2
#undef IDX1
#undef MUL
}

void func_800CF8C0(s16 arg0, s16 arg1, u8 arg2) {
    u8 dst;

    dst = func_800BBF7C(func_800CFB14);
    D_801620AC[dst].D_801621B4 = arg0;
    D_801620AC[dst].D_801621B0 = arg1;
    D_801620AC[dst].D_801621B6 = D_800F99E8;
    D_801620AC[dst].unk14 = arg2;
    if (D_800F99E8 == arg0) {
        D_801620AC[dst].unk8 = 0;
        D_801620AC[dst].unkA = 0;
        D_801620AC[dst].D_801621B2 = 0;
        return;
    }
    D_801620AC[dst].unk8 =
        (D_80163C80[arg0].vx - D_801518E4[arg0].D_80151A4C.vx) / arg1;
    D_801620AC[dst].unkA =
        (D_80163C80[arg0].vz - D_801518E4[arg0].D_80151A4C.vz) / arg1;
    D_801620AC[dst].D_801621B2 =
        (D_80163C80[arg0].vy - D_801518E4[arg0].D_80151A4C.vy) / arg1;
}

void func_800CFB14(void) {
    s16 dst;

    dst = D_801620AC[D_801590D0].D_801621B4;
    *(s32*)0x1F80000C = dst;
    *(s32*)0x1F800008 = D_801620AC[D_801590D0].D_801621B6;
    if (D_801620AC[D_801590D0].unk14 == 0) {
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AC = -1;
            return;
        }
        D_801518E4[dst].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
        D_801518E4[dst].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
        D_801518E4[dst].D_80151A4C.vy += D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        return;
    }
    D_801620AC[D_801590D0].unk14--;
}

void func_800CFCB0(void) {
    s32 temp_a3;
    s16 temp_a2;
    s32 temp_a1;

    if (D_801620AC[D_801590D0].D_801621B0 == 0) {
        D_801620AC[D_801590D0].D_801621AC = -1;
        return;
    }
    temp_a2 = D_801620AC[D_801590D0].D_801621B4;
    temp_a3 = D_801620AC[D_801590D0].D_801621B6;
    temp_a1 = D_801620AC[D_801590D0].unkC;
    *((s32*)0x1F80000C) = temp_a2;
    *((s32*)0x1F800008) = temp_a3;
    *((s32*)0x1F800010) = temp_a1;
    D_801518E4[temp_a2].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
    D_801518E4[temp_a2].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
    D_801518E4[temp_a2].D_80151A4C.vy +=
        D_800EEB28[temp_a1][D_801620AC[D_801590D0].unk14++];
    D_801620AC[D_801590D0].D_801621B0--;
}

void func_800CFE60(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX1 = D_801620AC[D_801590D0].D_801621B4;
    IDX2 = D_801620AC[D_801590D0].D_801621B6;
    switch (D_801620AC[D_801590D0].D_801621AE) {
    case 0:
        D_801620AC[D_801590D0].D_801621AE = 1;
        D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].unk15;
        break;
    case 1:
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AE = 2;
            D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].unk16;
            D_801620AC[D_801590D0].D_801621B2 =
                MUL(D_801518E4[IDX1].D_80151A4C.vy -
                        D_801620AC[D_801590D0].unkC,
                    D_801518E4[IDX2].D_801518EA) /
                D_801620AC[D_801590D0].unk16;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vy += D_801620AC[D_801590D0].unk10;
        D_801620AC[D_801590D0].D_801621B0--;
        break;
    case 2:
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
        D_801518E4[IDX1].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
        D_801518E4[IDX1].D_80151A4C.vy -= D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        break;
    }
#undef IDX2
#undef IDX1
#undef MUL
}

void func_800D01C0(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX2 = D_801620AC[D_801590D0].D_801621B6;
    IDX1 = D_801620AC[D_801590D0].D_801621B4;
    switch (D_801620AC[D_801590D0].D_801621AE) {
    case 0:
        D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].unk16;
        D_801620AC[D_801590D0].D_801621B2 =
            (D_801620AC[D_801590D0].unk10 - D_801518E4[IDX1].D_80151A4C.vy) /
            D_801620AC[D_801590D0].unk16;
        D_801620AC[D_801590D0].D_801621AE = 1;
        break;
    case 1:
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].unk15;
            D_801620AC[D_801590D0].D_801621B2 =
                MUL(D_801620AC[D_801590D0].unk10 - D_801620AC[D_801590D0].unkC,
                    D_801518E4[D_801620AC[D_801590D0].D_801621B6].D_801518EA) /
                D_801620AC[D_801590D0].unk15;
            D_801620AC[D_801590D0].D_801621AE = 2;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
        D_801518E4[IDX1].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
        D_801518E4[IDX1].D_80151A4C.vy += D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        break;
    case 2:
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vy -= D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        break;
    }
#undef IDX2
#undef IDX1
#undef MUL
}

void func_800D0578(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        func_800D4D4C(
            D_801621F0[D_801590D4].unk8, D_801621F0[D_801590D4].D_801621F6);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

void func_800D061C(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        func_800BBA84(D_801621F0[D_801590D4].D_801621F6, D_801590CC, 0);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

void func_800D06B8(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        if (D_801518DC == 0) {
            func_800D0C80(D_801621F0[D_801590D4].D_801621F6);
            D_801621F0[D_801590D4].D_801621F0 = -1;
        }
    } else {
        D_801621F0[D_801590D4].D_801621F4--;
    }
}

void func_800D0760(void) {
    if (D_80162978[D_8015169C].D_8016297E == 0) {
        if (D_80162978[D_8015169C].D_8016297C == 0) {
            D_80162978[D_8015169C].D_80162978 = -1;
            return;
        }
        func_800DCF60(D_801518E4[D_801590CC].D_80151907,
                      D_80151200[D_801590CC].D_8015123E);
        D_80162978[D_8015169C].D_8016297C--;
        return;
    }
    D_80162978[D_8015169C].D_8016297E--;
}

void func_800D088C(s32 loc, s32 len) {
    func_80033E34(loc, len, (u_long*)0x801B0000, NULL);
    func_800B7FB4();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D08B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D0958);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D09D0);

void func_800D0A44(void) {}

static void func_800D0AD4(void);
void func_800D0A4C(void) {
    s32 ret;
    s32 i;

    for (i = 0; i < LEN(D_801518E4); i++) {
        D_801518E4[i].D_80151909 |= 1;
    }
    func_801B0040(D_80151774, D_801590CC);
    ret = func_800BC04C(func_800D0AD4);
    *(s32*)0x1F800000 = ret;
    D_801621F0[ret].D_801621F4 = 2;
}

static void func_800D0AD4(void) {
    if (!D_801621F0[D_801590D4].D_801621F4) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        func_800BB978();
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

void func_800D0B4C(u8 arg0) {
    D_800F8CF0 = 0;
    func_800D1530();
    switch (D_801518E4[arg0].D_80151907) {
    case 4:
        D_800EF9D8[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 7:
        func_801B037C(D_80151774, D_801590CC);
        break;
    case 8:
        D_800EFFE0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    }
}

void func_800D0C80(u8 arg0) {
    D_800F8CF0 = 0;
    func_800D1530();
    switch (D_801518E4[arg0].D_80151907) {
    case 2:
        if (D_801031F0 == 0) {
            if (D_801518E4[arg0].D_80151906 == 25) {
                D_801518E4[0].D_8015190A = 1;
                D_801518E4[1].D_8015190A = 1;
                D_801518E4[2].D_8015190A = 1;
            }
            D_800EFAF0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
            return;
        }
        switch (D_801518E4[arg0].D_80151906) {
        case 41:
            func_801B0000(D_80151774, D_801590CC);
            break;
        case 44:
            func_801B0000_2(D_80151774, D_801590CC);
            break;
        case 35:
            func_801B000C(D_80151774, D_801590CC);
            break;
        case 32:
            func_801B0054(D_80151774, D_801590CC);
            break;
        case 29:
            func_801B0084(D_80151774, D_801590CC);
            break;
        default:
            D_800EFAF0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
            break;
        }
        break;
    case 13:
        D_800EFBC8[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 20:
        if (D_801518E4[arg0].D_80151906 == 2) {
            if (D_801590CC == D_800FA9E8) {
                D_80163A98 = 0;
            } else {
                D_80163A98 = 1;
            }
        }
        *(s32*)0x1F800000 =
            D_800EFEA0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        switch (D_801518E4[arg0].D_80151906) {
        case 0x2D:
        case 0x2E:
        case 0x2F:
        case 0x30:
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4F:
            *(s32*)0x1F800000 = 0;
            break;
        }
        func_800D08B8(arg0, *(s32*)0x1F800000);
        break;
    case 32:
        D_800EFC28[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 3:
        func_800C64AC();
        break;
    }
}

void func_800D1110(u8 arg0) {
    s32 lba;
    s32 var_a1;
    s32 id;

    switch (D_801518E4[arg0].D_80151907) {
    case 2:
        if (D_801031F0 == 0) {
            id = D_800EF63C[D_801518E4[arg0].D_80151906];
            func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        } else {
            switch (D_801518E4[arg0].D_80151906) {
            case 29:
                func_800D088C(D_800EEBB8[77].loc, D_800EEBB8[77].len);
                break;
            case 41:
                func_800D088C(D_800EEBB8[32].loc, D_800EEBB8[32].len);
                break;
            case 44:
                func_800D088C(D_800EEBB8[26].loc, D_800EEBB8[26].len);
                break;
            case 32:
                func_800D088C(D_800EEBB8[255].loc, D_800EEBB8[255].len);
                break;
            case 35:
                func_800D088C(D_800EEBB8[6].loc, D_800EEBB8[6].len);
                break;
            default:
                id = D_800EF63C[D_801518E4[arg0].D_80151906];
                lba = D_800EEBB8[id].loc;
                var_a1 = D_800EEBB8[id].len;
                func_800D088C(lba, var_a1);
                break;
            }
        }
        break;
    case 7:
        func_800D088C(D_800EEBB8[221].loc, D_800EEBB8[221].len);
        break;
    case 8:
        id = D_800EF8D8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 13:
        id = D_800EF6A8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 20:
        id = D_800EF838[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 4:
        id = D_800EF5B0[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 32:
        id = D_800EF6FC[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 3:
        id = D_800EF6D8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D1530);

void BATTLE_EnqueueLoadImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_LOAD_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

void BATTLE_EnqueueStoreImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_STORE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

void BATTLE_EnqueueMoveImage(RECT* rect, s32 x, s32 y) {
    D_800F01DC->method = QUEUE_MOVE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->x = x;
    D_800F01DC->y = y;
    D_800F01DC++;
}

void BATTLE_EnqueueClearImage(RECT* rect) {
    D_800F01DC->method = QUEUE_CLEAR_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC++;
}

void BATTLE_FlushImageQueue(void) {
    Unk800F01DC* item;

    for (item = D_800F4BAC; item < D_800F01DC; item++) {
        switch (item->method) {
        case QUEUE_LOAD_IMAGE:
            LoadImage(item->rect, item->ptr);
            break;
        case QUEUE_STORE_IMAGE:
            StoreImage(item->rect, item->ptr);
            break;
        case QUEUE_MOVE_IMAGE:
            MoveImage(item->rect, item->x, item->y);
            break;
        case QUEUE_CLEAR_IMAGE:
            ClearImage(item->rect, 0, 0, 0);
            break;
        }
    }
    D_800F01DC = D_800F4BAC;
}

void BATTLE_ResetImageQueue(void) { D_800F01DC = D_800F4BAC; }

void func_800D2710(u_long* addr, s16 x, s16 y) {
    TIM_IMAGE tim;

    OpenTIM(addr);
    ReadTIM(&tim);
    if (tim.crect && tim.caddr) {
        D_800F4B2C[D_800F01E0] = *tim.crect;
        D_800F4B2C[D_800F01E0].x += x & ~15;
        D_800F4B2C[D_800F01E0].y =
            y + D_800F4B2C[D_800F01E0].y; // requires GCC 2.6.3
        BATTLE_EnqueueLoadImage(&D_800F4B2C[D_800F01E0], tim.caddr);
        D_800F01E0 = (D_800F01E0 + 1) & 7;
    }
}

void func_800D2828(u_long* addr, s32 xy) {
    TIM_IMAGE tim;
    s32 temp_a1;
    s32 temp_a3;
    s32 temp_a2;

    OpenTIM(addr);
    ReadTIM(&tim);
    if (tim.prect && tim.paddr) {
        D_800F4B6C[D_800F01E4] = *tim.prect;
        temp_a1 = (tim.prect->y & 0x300) >> 4 | (tim.prect->x & 0x3FF) >> 6;
        temp_a2 = temp_a1 + xy;
        temp_a3 = (temp_a1 & 0x0F) * 0x40;
        D_800F4B6C[D_800F01E4].x =
            ((temp_a2 & 0x0F) * 0x40 + (D_800F4B6C[D_800F01E4].x - temp_a3)) &
            0x3FF;
        temp_a3 = (temp_a1 & 0x30) * 0x10;
        D_800F4B6C[D_800F01E4].y =
            ((temp_a2 & 0x30) * 0x10 + (D_800F4B6C[D_800F01E4].y - temp_a3)) &
            0x1FF;
        BATTLE_EnqueueLoadImage(&D_800F4B6C[D_800F01E4], tim.paddr);
        D_800F01E4 = (D_800F01E4 + 1) & 7;
    }
}

void func_800D2980(u_long* addr, s16 imgXY, s16 clutX, s16 clutY) {
    func_800D2710(addr, clutX, clutY);
    func_800D2828(addr, imgXY);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D29D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D32B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3354);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3418);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3474);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D34C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3520);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3548);

s32 func_800D35D8(u8* arg0, s32* arg1, s32 arg2) {
    s32 bits;
    s32 i;

    bits = 0;
    for (i = 0; i < arg2; i++) {
        bits <<= 1;
        if ((arg0[*arg1 / 8] >> (7 - (*arg1 & 7))) & 1) {
            bits++;
        }
        *arg1 = *arg1 + 1;
    }
    bits <<= 32 - arg2;
    bits >>= 32 - arg2;
    return bits;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3658);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D376C);

void func_800D3994(s32 arg0, s32 arg1, void* arg2);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3994);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3A6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3AF0);

const s32 D_800A0D98[] = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                          0x00001000, 0x00000000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3BF0);

void func_800D3D88(void) {
    Unk801621F0* temp_s0_2;
    Unk801621F0* temp_s1;
    s32 temp_s0;
    u16 temp_s2;

    temp_s1 = &D_801621F0[D_801590D4];
    temp_s0 = temp_s1->D_801621F0;
    temp_s2 = ((u8*)&D_801518E4[temp_s0].D_8015191A)[temp_s1->D_801621F2 & 1];
    temp_s0++; // !FAKE
    temp_s0--; // !FAKE
    if (temp_s2 != 0xFF) {
        temp_s0_2 = &D_801621F0[func_800BC04C(func_800D3BF0)];
        func_800D3994(temp_s0, temp_s2, &temp_s0_2->D_801621F4);
        temp_s0_2->D_801621F6 = 0;
        temp_s0_2->unkE = temp_s1->unkE;
        temp_s0_2->unk10.unk.unk0 = temp_s1->unk10.unk.unk0;
    }
    temp_s1->D_801621F2++;
    if (temp_s1->D_801621F2 == 4) {
        temp_s1->D_801621F0 = -1;
    }
}

void func_800D3E8C(s32 arg0) {
    Unk801621F0* temp_v0;

    temp_v0 = &D_801621F0[func_800BC04C(func_800D3D88)];
    temp_v0->D_801621F0 = arg0;
    temp_v0->unkE = *(s16*)& temp_v0->unk10 = D_801518E4[arg0].D_801518EA;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3F0C);

void func_800D3F8C(void) {
    Unk801621F0* temp_s0;
    Unk801621F0* temp_s1;

    temp_s1 = &D_801621F0[D_801590D4];
    if (D_80062D98 == 0) {
        temp_s1->unkC--;
        if (temp_s1->unkC == -1) {
            temp_s0 = &D_801621F0[func_800BC04C(func_800D3AF0)];
            RotMatrixYXZ(&D_801518E4[temp_s1->unk10.unk.unk2].unk160,
                         (MATRIX*)0x1F800008);
            ApplyMatrixSV((MATRIX*)0x1F800008, (SVECTOR*)&temp_s1->D_801621F4,
                          (SVECTOR*)0x1F800000);
            temp_s0->D_801621F4 =
                D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vx +
                ((SVECTOR*)0x1F800000)->vx;
            temp_s0->D_801621F6 =
                D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vy +
                ((SVECTOR*)0x1F800000)->vy;
            temp_s0->unk8 = D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vz +
                            ((SVECTOR*)0x1F800000)->vz;
            temp_s0->unkE = temp_s1->unkE;
            temp_s0->unk10.unk.unk0 = temp_s1->unk10.unk.unk0;
            temp_s1->D_801621F0 = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D415C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D41FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4284);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4368);

static void func_800D4484(u_long* ot, u16 tpage) {
    DR_MODE* dr_mode;

    dr_mode = D_80163C74;
    SetDrawMode(dr_mode, 0, 1, tpage, NULL);
    AddPrim(ot, (void*)dr_mode);
    D_80163C74 = dr_mode + 1;
}

const s32 D_800A0DB8[] = {0x00000000, 0xFFFFF000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D44E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D461C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4710);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D491C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4A64);

static void func_800D4D6C(s32 arg0, s32 arg1, s32 arg2);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4C08);

void func_800D4CBC(s32, s32, s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4CBC);

static void func_800D4D4C(s32 arg0, s32 arg1) {
    func_800D4CBC(arg0, arg1, 0x1000);
}

static void func_800D4D6C(s32 arg0, s32 arg1, s32 arg2) {
    func_800D4C08(arg0, arg1, 0x1000, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4D90);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4FA8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4FF0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D508C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5138);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D51D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5230);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D52A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5350);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5444);

s32 func_800D54BC(s32 arg0) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 10; i++) {
        if ((arg0 >> i) & 1) {
            count++;
        }
    }
    return count;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D54EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D55A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D55F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D56A8);

s32 func_800D574C(s32 arg0) {
    s32 sp10;

    func_800D54EC(arg0, &sp10);
    return func_800D56A8(&sp10);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5774);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D57C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D58D0);

void func_800D5938();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5938);

static void func_800D5A68(s16 arg0, s16 arg1) {
    Unk80162978* temp_v0;

    temp_v0 = &D_80162978[func_800BBEAC(func_800D5938)];
    temp_v0->D_80162978 = 0;
    temp_v0->D_8016297E = arg1;
    temp_v0->D_8016297C = arg0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5AC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5B6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5D28);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D61AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6260);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D650C);

extern u8 D_800F10EC[];
extern u8 D_800F11E8[];
extern u8 D_800F1304[];
u8* const D_800A0DC8[] = {D_800F10EC, D_800F11E8, D_800F1304};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6734);

void func_800D6734(s32, s32);
extern s32 D_800F14D4;

static void func_800D67BC(s32 arg0) {
    D_800F14D4 = 0x88;
    func_800D6734(arg0, 0);
}

static void func_800D67E8(s32 arg0) {
    D_800F14D4 = 0xA8;
    func_800D6734(arg0, 1);
}

void func_800D6814(s32 arg0) {
    D_800F14D4 = 0x88;
    func_800D6734(arg0, 2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6840);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6998);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6ACC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6C20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6D8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6F78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D70C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7178);

void func_800D72B4();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D72B4);

static void func_800D7340(void) { func_800BBEAC(func_800D72B4); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7368);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D751C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D76B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7724);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7888);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7A88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7B1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7BA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7C2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7D3C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D83A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8468);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D84F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D85B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D87EC);
