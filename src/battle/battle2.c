//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"

static void func_800D67BC(s32 arg0);
static void func_800D67E8(s32 arg0);
void func_800D6814(s32 arg0);
static void func_800D58D0(s32 arg0, s16 arg1, s16 arg2);

extern Yamada D_800EEBB8[]; // MAGIC/*.BIN overlay
extern s16 D_800EF5B0[];
extern s16 D_800EF63C[];
extern s16 D_800EF6A8[];
extern s16 D_800EF6D8[];
extern s16 D_800EF6FC[];
extern s16 D_800EF838[];
extern s16 D_800EF8D8[];

s32 BattleMovementRegister(void (*f)(void));
s32 func_800BC04C(void (*f)());
void func_800C2928();
void func_800C328C();
void func_800C3578();
void func_800C3950();
void func_800C3CA8();
void func_800C40F4();
void func_800C44B4();
void func_800C4814();
static void func_800CFB14();
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
    D_800FA6D8[arg0].unk0 = func_800D376C(D_800FA6D8[arg0].unk8, D_800FA6D8[arg0].unk0, arg1, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800C7C4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD400);

static s16 func_800CD558(s16 arg0, u8* arg1) {
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
    if (g_modelScreenPos[D_801590CC].prevX < g_modelScreenPos[D_801621F0[D_801590D4].unk8].prevX) {
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
        ret = BattleEffectRegister(func_800C3578);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg2;
        break;
    case 8:
        ret = BattleEffectRegister(func_800C4814);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg2;
        break;
    case 1:
    case 10:
        ret = BattleMovementRegister(func_800C3950);
        g_BattleMovementSlots[ret].D_801620B2 = arg2;
        g_BattleMovementSlots[ret].D_801620B4 = arg0;
        g_BattleMovementSlots[ret].D_801620B6 = 0xF8;
        return;
    case 2:
        ret = BattleEffectRegister(func_800C3CA8);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg2;
        break;
    case 3:
        ret = BattleEffectRegister(func_800C328C);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg2;
        break;
    case 17:
    case 18:
        ret = BattleEffectRegister(func_800C40F4);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg2;
        break;
    case 7:
        ret = BattleEffectRegister(func_800C44B4);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg2;
        break;
    }
}

static void func_800CE21C(s16 arg0, u8 arg1) {
    s32 ret;

    func_800CE058(arg0);
    switch (D_801636B8[arg0].D_801636BC) {
    case 0:
        ret = BattleEffectRegister(func_800C3578);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg1;
        break;
    case 8:
        ret = BattleEffectRegister(func_800C4814);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg1;
        break;
    case 1:
    case 10:
        ret = BattleMovementRegister(func_800C3950);
        g_BattleMovementSlots[ret].D_801620B2 = arg1;
        g_BattleMovementSlots[ret].D_801620B4 = arg0;
        g_BattleMovementSlots[ret].D_801620B6 = 0xF8;
        break;
    case 2:
        ret = BattleEffectRegister(func_800C3CA8);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg1;
        break;
    case 3:
        ret = BattleEffectRegister(func_800C328C);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg1;
        break;
    case 17:
    case 18:
        ret = BattleEffectRegister(func_800C40F4);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg1;
        break;
    case 7:
        ret = BattleEffectRegister(func_800C44B4);
        g_BattleEffectSlots[ret].D_80162980 = arg0;
        g_BattleEffectSlots[ret].D_8016297E = arg1;
        break;
    }
}

static void func_800CE384(void) {
    u8* ptr;
    u8 do_work;
    u32 param;
    u32 param_hi;

    if (D_80151200[D_801621F0[D_801590D4].D_801621F6].D_80151234 != D_801621F0[D_801590D4].D_801621F2) {
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
            D_801621F0[D_801590D4].D_801621F4 = ptr[D_801621F0[D_801590D4].unk18++];
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
            func_800A3534(D_801621F0[D_801590D4].unkA, D_801621F0[D_801590D4].unk8);
        }
        D_801621F0[D_801590D4].D_801621F4--;
        break;
    }
}

// Shell over a covered unit; MBarrier wins when both masks are set. Masks
// named by Akari's q-gears_reverse (ffvii/address_battle.txt): D_800FA69C is
// the magic barrier mask, D_80163608 the physical one.
static void func_800CE75C(void) {
    if ((D_800FA69C >> g_BattleEffectSlots[g_BattleEffectCursor].D_80162980) & 1) {
        func_800D67E8(g_BattleEffectSlots[g_BattleEffectCursor].D_80162980); // MBarrier
    } else if ((D_80163608 >> g_BattleEffectSlots[g_BattleEffectCursor].D_80162980) & 1) {
        func_800D67BC(g_BattleEffectSlots[g_BattleEffectCursor].D_80162980); // Barrier
    }
}

static void func_800CE7E0(void) {
    s32 dst;

    if (!g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E != -1) {
            func_800CE75C();
            dst = func_800BC04C(func_800C2928);
            D_801621F0[dst].unk14 = g_BattleEffectSlots[g_BattleEffectCursor].unkE;
            D_801621F0[dst].unkE = g_BattleEffectSlots[g_BattleEffectCursor].D_80162982;
            D_801621F0[dst].unk10.ptr = (u8*)(u32)g_BattleEffectSlots[g_BattleEffectCursor].D_80162980;
            dst = func_800BC04C(func_800CE638);
            D_801621F0[dst].unkA = g_BattleEffectSlots[g_BattleEffectCursor].unk19;
            D_801621F0[dst].unk8 = g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E;
        }
        g_BattleEffectSlots[g_BattleEffectCursor].D_80162978 = -1;
        return;
    } else {
        g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C--;
    }
}

void func_800CEB48(void);
void func_800CE970(void) {
    s32 dst;

    if (!g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].unkE & 2) {
            D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        }
        if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E != -1 &&
            g_BattleEffectSlots[g_BattleEffectCursor].unk18 != 1) {
            func_800CE75C();
            dst = func_800BC04C(func_800C2928);
            D_801621F0[dst].unk14 = g_BattleEffectSlots[g_BattleEffectCursor].unkE;
            D_801621F0[dst].unkE = g_BattleEffectSlots[g_BattleEffectCursor].D_80162982;
            D_801621F0[dst].unk10.ptr = (u8*)(u32)g_BattleEffectSlots[g_BattleEffectCursor].D_80162980;
            dst = func_800BC04C(func_800CE638);
            D_801621F0[dst].unkA = g_BattleEffectSlots[g_BattleEffectCursor].unk19;
            D_801621F0[dst].unk8 = g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E;
        }
        func_800CEB48();
        return;
    } else {
        g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C--;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CEB48);

static void func_800CF2F0(void) {
    s16 index;

    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        return;
    }
    index = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    D_801518E4[index].D_80151A4C.vy += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    *(s32*)0x1F80000C = index;
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 =
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 - 1;
}

static void func_800CF3CC(void) {
    s16 index;

    index = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    *(s32*)0x1F80000C = index;
    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        return;
    }
    D_801518E4[index].unk160.vy += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
}

static void func_800CF4A8(void) {
    s16 index;

    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        return;
    }
    index = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    D_801518E4[index].D_80151A4C.vx += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    D_801518E4[index].D_80151A4C.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
    *(s32*)0x1F80000C = index;
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 =
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 - 1;
}

static void func_800CF5BC(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX1 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    IDX2 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 1;
        if (IDX1 >= 4) {
            if (D_801031F0 == 0) {
                g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 =
                    (MUL(D_801518E4[IDX2].D_80151A4C.vy, D_801518E4[IDX2].D_801518EA) -
                     MUL(D_801518E4[IDX1].D_80151A4C.vy, D_801518E4[IDX1].D_801518EA)) /
                    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0;
            } else {
                g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 = 0;
            }
        } else {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 = 0;
        }
    }
    if (!g_BattleMovementSlots[g_BattleMovementCursor].unk18) {
        D_80166F58 = 0;
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
        D_801518E4[IDX1].D_80151A4C.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
        D_801518E4[IDX1].D_80151A4C.vy += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        return;
    }
    g_BattleMovementSlots[g_BattleMovementCursor].unk18--;
#undef IDX2
#undef IDX1
#undef MUL
}

static void func_800CF8C0(s16 arg0, s16 arg1, u8 arg2) {
    u8 dst;

    dst = BattleMovementRegister(func_800CFB14);
    g_BattleMovementSlots[dst].D_801620B4 = arg0;
    g_BattleMovementSlots[dst].D_801620B0 = arg1;
    g_BattleMovementSlots[dst].D_801620B6 = D_800F99E8;
    g_BattleMovementSlots[dst].unk18 = arg2;
    if (D_800F99E8 == arg0) {
        g_BattleMovementSlots[dst].unkC = 0;
        g_BattleMovementSlots[dst].unkE = 0;
        g_BattleMovementSlots[dst].D_801620B2 = 0;
        return;
    }
    g_BattleMovementSlots[dst].unkC = (D_80163C80[arg0].vx - D_801518E4[arg0].D_80151A4C.vx) / arg1;
    g_BattleMovementSlots[dst].unkE = (D_80163C80[arg0].vz - D_801518E4[arg0].D_80151A4C.vz) / arg1;
    g_BattleMovementSlots[dst].D_801620B2 = (D_80163C80[arg0].vy - D_801518E4[arg0].D_80151A4C.vy) / arg1;
}

static void func_800CFB14(void) {
    s16 dst;

    dst = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    *(s32*)0x1F80000C = dst;
    *(s32*)0x1F800008 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    if (g_BattleMovementSlots[g_BattleMovementCursor].unk18 == 0) {
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
            return;
        }
        D_801518E4[dst].D_80151A4C.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
        D_801518E4[dst].D_80151A4C.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
        D_801518E4[dst].D_80151A4C.vy += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        return;
    }
    g_BattleMovementSlots[g_BattleMovementCursor].unk18--;
}

static void func_800CFCB0(void) {
    s32 temp_a3;
    s16 temp_a2;
    s32 temp_a1;

    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        return;
    }
    temp_a2 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    temp_a3 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    temp_a1 = g_BattleMovementSlots[g_BattleMovementCursor].unk10;
    *((s32*)0x1F80000C) = temp_a2;
    *((s32*)0x1F800008) = temp_a3;
    *((s32*)0x1F800010) = temp_a1;
    D_801518E4[temp_a2].D_80151A4C.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
    D_801518E4[temp_a2].D_80151A4C.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
    D_801518E4[temp_a2].D_80151A4C.vy += D_800EEB28[temp_a1][g_BattleMovementSlots[g_BattleMovementCursor].unk18++];
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
}

static void func_800CFE60(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX1 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    IDX2 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    switch (g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE) {
    case 0:
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 1;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 = g_BattleMovementSlots[g_BattleMovementCursor].unk19;
        break;
    case 1:
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 2;
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 =
                g_BattleMovementSlots[g_BattleMovementCursor].unk1A;
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 =
                MUL(D_801518E4[IDX1].D_80151A4C.vy - g_BattleMovementSlots[g_BattleMovementCursor].unk10,
                    D_801518E4[IDX2].D_801518EA) /
                g_BattleMovementSlots[g_BattleMovementCursor].unk1A;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vy += g_BattleMovementSlots[g_BattleMovementCursor].unk14;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        break;
    case 2:
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
        D_801518E4[IDX1].D_80151A4C.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
        D_801518E4[IDX1].D_80151A4C.vy -= g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        break;
    }
#undef IDX2
#undef IDX1
#undef MUL
}

static void func_800D01C0(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX2 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    IDX1 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    switch (g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE) {
    case 0:
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 = g_BattleMovementSlots[g_BattleMovementCursor].unk1A;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 =
            (g_BattleMovementSlots[g_BattleMovementCursor].unk14 - D_801518E4[IDX1].D_80151A4C.vy) /
            g_BattleMovementSlots[g_BattleMovementCursor].unk1A;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 1;
        break;
    case 1:
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 =
                g_BattleMovementSlots[g_BattleMovementCursor].unk19;
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 =
                MUL(g_BattleMovementSlots[g_BattleMovementCursor].unk14 -
                        g_BattleMovementSlots[g_BattleMovementCursor].unk10,
                    D_801518E4[g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6].D_801518EA) /
                g_BattleMovementSlots[g_BattleMovementCursor].unk19;
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 2;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
        D_801518E4[IDX1].D_80151A4C.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
        D_801518E4[IDX1].D_80151A4C.vy += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        break;
    case 2:
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vy -= g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        break;
    }
#undef IDX2
#undef IDX1
#undef MUL
}

static void func_800D0578(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        func_800D4D4C(D_801621F0[D_801590D4].unk8, D_801621F0[D_801590D4].D_801621F6);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

static void func_800D061C(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        func_800BBA84(D_801621F0[D_801590D4].D_801621F6, D_801590CC, 0);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

static void func_800D06B8(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        if (D_801518DC == 0) {
            func_800D0C80(D_801621F0[D_801590D4].D_801621F6);
            D_801621F0[D_801590D4].D_801621F0 = -1;
        }
    } else {
        D_801621F0[D_801590D4].D_801621F4--;
    }
}

static void func_800D0760(void) {
    if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E == 0) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C == 0) {
            g_BattleEffectSlots[g_BattleEffectCursor].D_80162978 = -1;
            return;
        }
        func_800DCF60(D_801518E4[D_801590CC].D_80151907, D_80151200[D_801590CC].D_8015123E);
        g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C--;
        return;
    }
    g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E--;
}

void func_800D088C(s32 loc, s32 len) {
    SystemLoadFileBySector(loc, len, (u_long*)0x801B0000, NULL);
    BattleCdromReadChain();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D08B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D0958);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D09D0);

static void func_800D0A44(void) {}

static void func_800D0AD4(void);
void func_800D0A4C(void) {
    s32 ret;
    s32 i;

    for (i = 0; i < 3; i++) {
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

static void func_800D0B4C(u8 arg0) {
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
            if (D_801590CC == D_800FA9D0[2].unk0) {
                D_80163A98 = 0;
            } else {
                D_80163A98 = 1;
            }
        }
        *(s32*)0x1F800000 = D_800EFEA0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
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

static void func_800D1110(u8 arg0) {
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

void BattleEnqueueLoadImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_LOAD_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

static void BattleEnqueueStoreImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_STORE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

static void BattleEnqueueMoveImage(RECT* rect, s32 x, s32 y) {
    D_800F01DC->method = QUEUE_MOVE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->x = x;
    D_800F01DC->y = y;
    D_800F01DC++;
}

static void BattleEnqueueClearImage(RECT* rect) {
    D_800F01DC->method = QUEUE_CLEAR_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC++;
}

void BattleFlushImageQueue(void) {
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

static void BattleResetImageQueue(void) { D_800F01DC = D_800F4BAC; }

static void func_800D2710(u_long* addr, s16 x, s16 y) {
    TIM_IMAGE tim;

    OpenTIM(addr);
    ReadTIM(&tim);
    if (tim.crect && tim.caddr) {
        D_800F4B2C[D_800F01E0] = *tim.crect;
        D_800F4B2C[D_800F01E0].x += x & ~15;
        D_800F4B2C[D_800F01E0].y = y + D_800F4B2C[D_800F01E0].y; // requires GCC 2.6.3
        BattleEnqueueLoadImage(&D_800F4B2C[D_800F01E0], tim.caddr);
        D_800F01E0 = (D_800F01E0 + 1) & 7;
    }
}

static void func_800D2828(u_long* addr, s32 xy) {
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
        D_800F4B6C[D_800F01E4].x = ((temp_a2 & 0x0F) * 0x40 + (D_800F4B6C[D_800F01E4].x - temp_a3)) & 0x3FF;
        temp_a3 = (temp_a1 & 0x30) * 0x10;
        D_800F4B6C[D_800F01E4].y = ((temp_a2 & 0x30) * 0x10 + (D_800F4B6C[D_800F01E4].y - temp_a3)) & 0x1FF;
        BattleEnqueueLoadImage(&D_800F4B6C[D_800F01E4], tim.paddr);
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

static s32 func_800D35D8(u8* arg0, s32* arg1, s32 arg2) {
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

void BattleGetPartPosition(s32 arg0, s32 arg1, void* arg2);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleGetPartPosition);

// Take the low 16 bits of each of arg0's translation components relative to the
// camera D_800FA63C, then rotate that offset by the camera's transposed
// orientation into arg1.
static void func_800D3A6C(MATRIX* arg0, SVECTOR* arg1) {
    MATRIX sp10;

    arg1->vx = (s16)(*(u16*)&arg0->t[0] - *(u16*)&D_800FA63C.m.t[0]);
    arg1->vy = (s16)(*(u16*)&arg0->t[1] - *(u16*)&D_800FA63C.m.t[1]);
    arg1->vz = (s16)(*(u16*)&arg0->t[2] - *(u16*)&D_800FA63C.m.t[2]);
    TransposeMatrix(&D_800FA63C.m, &sp10);
    ApplyMatrixSV(&sp10, arg1, arg1);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3AF0);

const s32 D_800A0D98[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00001000, 0x00000000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3BF0);

static void func_800D3D88(void) {
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
        BattleGetPartPosition(temp_s0, temp_s2, &temp_s0_2->D_801621F4);
        temp_s0_2->D_801621F6 = 0;
        temp_s0_2->unkE = temp_s1->unkE;
        temp_s0_2->unk10.unk.unk0 = temp_s1->unk10.unk.unk0;
    }
    temp_s1->D_801621F2++;
    if (temp_s1->D_801621F2 == 4) {
        temp_s1->D_801621F0 = -1;
    }
}

static void func_800D3E8C(s32 arg0) {
    Unk801621F0* temp_v0;

    temp_v0 = &D_801621F0[func_800BC04C(func_800D3D88)];
    temp_v0->D_801621F0 = arg0;
    temp_v0->unkE = *(s16*)& temp_v0->unk10 = D_801518E4[arg0].D_801518EA;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3F0C);

static void func_800D3F8C(void) {
    Unk801621F0* temp_s0;
    Unk801621F0* temp_s1;

    temp_s1 = &D_801621F0[D_801590D4];
    if (D_80062D98 == 0) {
        temp_s1->unkC--;
        if (temp_s1->unkC == -1) {
            temp_s0 = &D_801621F0[func_800BC04C(func_800D3AF0)];
            RotMatrixYXZ(&D_801518E4[temp_s1->unk10.unk.unk2].unk160, (MATRIX*)0x1F800008);
            ApplyMatrixSV((MATRIX*)0x1F800008, (SVECTOR*)&temp_s1->D_801621F4, (SVECTOR*)0x1F800000);
            temp_s0->D_801621F4 = D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vx + ((SVECTOR*)0x1F800000)->vx;
            temp_s0->D_801621F6 = D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vy + ((SVECTOR*)0x1F800000)->vy;
            temp_s0->unk8 = D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vz + ((SVECTOR*)0x1F800000)->vz;
            temp_s0->unkE = temp_s1->unkE;
            temp_s0->unk10.unk.unk0 = temp_s1->unk10.unk.unk0;
            temp_s1->D_801621F0 = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D415C);

static void func_800D41FC(MATRIX* arg0, MATRIX* arg1, MATRIX* arg2) {
    arg2->t[0] = arg1->t[0] - arg0->t[0];
    arg2->t[1] = arg1->t[1] - arg0->t[1];
    arg2->t[2] = arg1->t[2] - arg0->t[2];
    TransposeMatrix(arg0, arg2);
    ApplyMatrixLV(arg2, (VECTOR*)arg2->t, (VECTOR*)arg2->t);
    MulMatrix(arg2, arg1);
}

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

static void func_800D4D6C(void* arg0, s32 arg1, s32 arg2);
void func_800D4C08(void* arg0, s32 arg1, s32 arg2, s32 arg3);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4C08);

static void func_800D4CBC(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp10;

    BattleGetPartPosition(arg0, D_801518E4[arg0].D_8015190F, &sp10);
    func_800D4C08(&sp10, arg1, arg2, -D_801518E4[arg0].unk12);
}

static void func_800D4D4C(s32 arg0, s32 arg1) { func_800D4CBC(arg0, arg1, 0x1000); }

static void func_800D4D6C(void* arg0, s32 arg1, s32 arg2) { func_800D4C08(arg0, arg1, 0x1000, arg2); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4D90);

extern s32 D_800F10D8;
extern s32 D_800F4CEC[16];
extern s16 D_800F4D2C[16][10];

// Enqueue a value into the 16-entry circular battle queue and return
// the associated data slot for the newly queued entry.
static s16* BattleEventQueuePush(s32 arg0) {
    s32 idx = D_800F10D8;
    s32 next = (idx + 1) & 0xF;

    D_800F4CEC[idx] = arg0;
    D_800F10D8 = next;
    return D_800F4D2C[idx];
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4FF0);

void func_800D508C();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D508C);

extern Unk80162978* D_800F10E0;

// Reset the fixed-point ramp: zero the accumulator (0x04) and seed the
// countdown (0x0C) so it lasts arg0 ticks.
static void BattleFixedPointRampInit(s32 arg0) {
    if (D_800F10E0 == NULL) {
        D_800F10E0 = &g_BattleEffectSlots[BattleEffectRegister(func_800D508C)];
    }
    *(s32*)&D_800F10E0->D_8016297C = 0;
    *(s32*)&D_800F10E0->unkC = 0x10000 / arg0;
}

void func_800D51D4(s32 arg0);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D51D4);

extern s32 D_800F10E4;
extern s16 D_800F5B74;

// Step the ramp once: accumulate (0x04 += 0x08), publish the high word, and
// free the slot when the countdown (0x0C) reaches 0.
static void BattleFixedPointRampUpdate(void) {
    Unk80162978* slot = &g_BattleEffectSlots[g_BattleEffectCursor];
    s32 v0;
    s32 v1;

    if (D_80062D98 == 0) {
        v0 = *(s32*)&slot->D_8016297C + *(s32*)&slot->D_80162980;
        *(s32*)&slot->D_8016297C = v0;
        D_800F5B74 = v0 >> 0x10;
        v1 = *(s32*)&slot->unkC - 1;
        *(s32*)&slot->unkC = v1;
        if (v1 == 0) {
            D_800F10E4 = 0;
            slot->D_80162978 = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D52A0);

// Fan one magic animation out over its target mask. Each activation scans to
// the next target in the mask, fires the callback, and retires the slot once
// the mask is exhausted; FrameStep decides how often that happens.
void BattleAnimationUpdate(void) {
    MagicAnimationData* slot = (MagicAnimationData*)&g_BattleEffectSlots[g_BattleEffectCursor];
    s16 target;

    if (D_80062D98 != 0) { // global pause
        return;
    }
    if (slot->FrameCounter == 0) {
        do {
            target = slot->TargetCursor;
            while (((slot->TargetMask >> target) & 1) == 0) {
                target = target + 1;
                slot->TargetCursor = target;
            }
            slot->Callback(slot->TargetCursor, slot->CallbackArg);
            // These two fields are read back with lhu here and lh everywhere
            // else, so the u16 casts have to stay.
            slot->TargetCursor = (u16)slot->TargetCursor + 1;
            // No bit left at or above the cursor, so every target is done.
            if (slot->TargetMask < (1 << slot->TargetCursor)) {
                slot->TargetCursor = -1;
                return;
            }
        } while (slot->FrameStep == 0); // 0 fans out to every target at once
    }
    slot->FrameCounter = (u16)slot->FrameCounter + 1;
    if (slot->FrameCounter >= slot->FrameStep) {
        slot->FrameCounter = 0;
    }
}

// TODO: signature is a best guess. Certain: two args are passed, the target
// index and arg1 (offset 0x06). Guessed: the types -- s16 and s32
// compile identically, no overlay yet reads arg1, and editing this
// leaves every object byte-identical, so the build cannot check it.

void MagicAnimationRegister(s32 arg0, s32 arg1, s32 arg2, void (*func)(s32, s32)) {
    MagicAnimationData* temp_v0 =
        (MagicAnimationData*)&g_BattleEffectSlots[BattleEffectRegister(BattleAnimationUpdate)];
    temp_v0->TargetCursor = 0;
    temp_v0->TargetMask = arg0;
    temp_v0->CallbackArg = arg1;
    temp_v0->FrameStep = arg2;
    temp_v0->Callback = func;
}

static s32 func_800D54BC(s32 arg0) {
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

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleEntityGetCenter);

s32 func_800D55A4(s32 arg0) { return (D_801518E4[arg0].unk12 * 0x10) * D_801518E4[arg0].D_801518EA >> 0xC; }

// Generic AKAO sound-command dispatcher: the first vararg's low 16 bits are
// the command id, which selects how many trailing u32 params get copied into
// the D_8009A004 queue before calling SystemAkaoExecute.
void BattleCommandSend(s32 cmdId, ...) {
    void** args = (void**)&cmdId;
    u32* dst = (u32*)cmdId;
    u32* src;
    s32 cmd = *(u16*)args;
    s32 count;
    s32 nExtra;

    D_8009A000[0] = cmd;
    switch (cmd & 0xFFFF) {
    case 0x21:
        nExtra = 3;
        break;
    case 0x22:
        nExtra = 4;
        break;
    case 0x23:
        nExtra = 5;
        break;
    default:
        nExtra = 2;
        break;
    }
    count = 1;
    if (count <= nExtra) {
        dst = (u32*)D_8009A004;
        src = (u32*)args + 1;
        for (; count <= nExtra; count++) {
            *dst++ = *src++;
        }
    }
    SystemAkaoExecute();
}

// Project a point through the current view matrix and convert its clamped
// on-screen X (0..319) into a 0..127 stereo pan value.
s32 BattlePositionToStereoPan(SVECTOR* sv) {
    s16 sxy[2];
    s32 p;
    s32 flag;

    SetRotMatrix(&D_800FA63C.m);
    SetTransMatrix(&D_800FA63C.m);
    RotTransPers(sv, (long*)sxy, (long*)&p, (long*)&flag);
    if (sxy[0] < 0) {
        sxy[0] = 0;
    } else if (sxy[0] >= 0x140) {
        sxy[0] = 0x13F;
    }
    return (sxy[0] * 128) / 320;
}

s32 BattleEntityGetStereoPan(s32 arg0) {
    SVECTOR sv;

    BattleEntityGetCenter(arg0, &sv);
    return BattlePositionToStereoPan(&sv);
}

// Queue a popup carrying bit index arg0, using push type 6 if that bit is
// set in the D_800F836C flag word, else type 4.
void func_800D5774(u32 arg0) {
    s32 cond;
    s16* ptr;

    cond = (D_800F836C >> arg0) & 1;
    if (cond) {
        ptr = BattleEventQueuePush(6);
    } else {
        ptr = BattleEventQueuePush(4);
    }
    *ptr = arg0;
}

void func_800D57C0();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D57C0);

static void func_800D58D0(s32 arg0, s16 arg1, s16 arg2) {
    Unk80162978* temp_v0 = &g_BattleEffectSlots[BattleEffectRegister(func_800D57C0)];
    temp_v0->D_80162978 = 0;
    temp_v0->D_80162980 = arg0;
    temp_v0->D_8016297E = arg2;
    temp_v0->D_8016297C = arg1;
}

void func_800D5938();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5938);

static void func_800D5A68(s16 arg0, s16 arg1) {
    Unk80162978* temp_v0;

    temp_v0 = &g_BattleEffectSlots[BattleEffectRegister(func_800D5938)];
    temp_v0->D_80162978 = 0;
    temp_v0->D_8016297E = arg1;
    temp_v0->D_8016297C = arg0;
}

// Divide each byte lane of a packed color independently by a divisor,
// yielding a per-channel step (e.g. a color-fade increment).
static s32 func_800D5AC0(s32 arg0, s32 arg1) {
    return (((arg0 & 0xFF0000) / arg1) & 0xFF0000) | (((arg0 & 0xFF00) / arg1) & 0xFF00) |
           (((arg0 & 0xFF) / arg1) & 0xFF);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5B6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5D28);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D61AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6260);

extern ModelRenderDesc D_800F14D0;

// Draw a model 4 times through func_800D29D4 (same request-struct pattern as
// barrier.c's D_801B0C98/D_801B0CB0), toggling the 0x1/0x2 flag bits between
// passes. Those bits mirror on X and Y, so the four passes are the four
// quadrants of a symmetric model built from one quarter.
static void func_800D6394(s32* arg0, s16 arg1) {
    D_800F14D0.model = arg0;
    D_800F14D0.color = arg1;
    SetFarColor(0, 0, 0);
    PushMatrix();
    D_80163C74 = func_800D29D4(&D_800F14D0, g_cDb->unk70, 0xC, D_80163C74);
    PopMatrix();
    PushMatrix();
    D_800F14D0.flags |= MODEL_MIRROR_X;
    D_80163C74 = func_800D29D4(&D_800F14D0, g_cDb->unk70, 0xC, D_80163C74);
    PopMatrix();
    PushMatrix();
    D_800F14D0.flags |= MODEL_MIRROR_Y;
    D_80163C74 = func_800D29D4(&D_800F14D0, g_cDb->unk70, 0xC, D_80163C74);
    PopMatrix();
    D_800F14D0.flags &= ~MODEL_MIRROR_X;
    D_80163C74 = func_800D29D4(&D_800F14D0, g_cDb->unk70, 0xC, D_80163C74);
    D_800F14D0.flags &= ~MODEL_MIRROR_Y;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D650C);

extern u8 D_800F10EC[];
extern u8 D_800F11E8[];
extern u8 D_800F1304[];
u8* const D_800A0DC8[] = {D_800F10EC, D_800F11E8, D_800F1304};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6734);

void func_800D6734(s32, s32);

static void func_800D67BC(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS; // Barrier
    func_800D6734(arg0, 0);
}

static void func_800D67E8(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_NO_CULL | MODEL_SEMI_TRANS; // MBarrier
    func_800D6734(arg0, 1);
}

void func_800D6814(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
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

static void func_800D72B4(void) {
    Unk801621F0* elem = &D_801621F0[D_801590D4];

    if (D_80062D98 == 0) {
        // Advance this slot's per-tick state machine (field 0x2).
        if (elem->D_801621F2 == 0) {
            BattleFixedPointRampInit(1);
        }
        if (elem->D_801621F2 == 2) {
            func_800D51D4(1);
            elem->D_801621F0 = -1;
        }
        elem->D_801621F2++;
    }
}

static void func_800D7340(void) { BattleEffectRegister(func_800D72B4); }

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

typedef struct {
    /* 0x00 */ u16 unk0;
    /* 0x02 */ u16 unk2;
    /* 0x04 */ u16 unk4;
    /* 0x06 */ u16 unk6;
    /* 0x08 */ u16 unk8;
    /* 0x0A */ u16 unkA;
    /* 0x0C */ u16 unkC;
    /* 0x0E */ u16 unkE;
    /* 0x10 */ u16 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
} UnkStruct800D8468; // size:0x20

static void func_800D8468(UnkStruct800D8468* dst, UnkStruct800D8468* src) {
    dst->unk0 = src->unk0;
    dst->unk6 = src->unk6;
    dst->unkC = src->unkC;
    dst->unk2 = src->unk2;
    dst->unk8 = src->unk8;
    dst->unkE = src->unkE;
    dst->unk4 = src->unk4;
    dst->unkA = src->unkA;
    dst->unk10 = src->unk10;
    dst->unk14 = src->unk14;
    dst->unk18 = src->unk18;
    dst->unk1C = src->unk1C;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D84F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D85B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D87EC);
