//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"

void func_800D751C();
void func_800D7888();
void func_800D6F78();
void func_800D5D28();
void BattleHitFlashGrowTick();
void BattleHitFlashShrinkTick();

static void BattleTriggerActorFlashMode0(s32 arg0);
static void BattleTriggerActorFlashMode1(s32 arg0);
void BattleTriggerActorFlashMode2(s32 arg0);
static void BattleSpawnActorRampEffect(s32 arg0, s16 arg1, s16 arg2);

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
static void BattleModelMoveTick();
void func_800D1530();
s32 BattleModelReadAnimStream(BattleModelSub* arg0, s32 arg1, s16 nItems, u8* arg3);
void func_800D3AF0();
static void BattleSpawnFloatingIcon(s32 arg0, s32 arg1);

// MAGIC/ entrypoints
void func_801B037C(s16, u8);
void func_801B0000(s16, u8);
void func_801B0000_2(s16, u8);
void func_801B000C(s16, u8);
void func_801B0040(s16, u8);
void func_801B0054(s16, u8);
void func_801B0084(s16, u8);

static s32 BattleModelReadAnimIntoMatrix(s16 arg0, s16 nItems, u8* arg2) {
    BattleModelSub* var_a0;
    s32 var_a1;
    s32 temp_s0;
    s32 temp_v0;

    var_a0 = D_801518E4[arg0].D_80151A58;
    var_a1 = D_801518E4[arg0].D_80151958;
    D_801518E4[arg0].D_80151958 = BattleModelReadAnimStream(var_a0, var_a1, nItems, arg2);
    return D_801518E4[arg0].D_80151958 == 0;
}

static void BattleWeaponReadAnimIntoMatrix(s16 arg0, s16 arg1, u8* arg2) {
    D_800FA6D8[arg0].unk0 = BattleModelReadAnimStream(D_800FA6D8[arg0].unk8, D_800FA6D8[arg0].unk0, arg1, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleExecuteUnitAnimScript);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD400);

static s16 BattleUnitAnimScriptReadU16(s16 arg0, u8* arg1) {
    u32 val;
    val = arg1[D_801518E4[arg0].D_80151920++];
    return (arg1[D_801518E4[arg0].D_80151920++] << 8) + val;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD5E4);

static void BattleModelRestoreVelocity(u8 arg0) {
    D_801518E4[arg0].D_80151A4C.vx = D_80163C80[arg0].vx;
    D_801518E4[arg0].D_80151A4C.vy = D_80163C80[arg0].vy;
    D_801518E4[arg0].D_80151A4C.vz = D_80163C80[arg0].vz;
}

static void BattleClearEffectAndFlag(void) {
    D_801590DC = 1;
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD860);

static void BattleResetUnitAnimScript(s16 arg0) {
    D_801518E4[arg0].D_8015191F = 1;
    D_801518E4[arg0].D_80151920 = 0;
    D_801518E4[arg0].D_80151921 = 0;
}

void BattleApplyToggleFlag(void) {
    if (D_80166F68) {
        D_800F9DA4 = D_800F9DA4 | 1;
    } else {
        D_800F9DA4 = D_800F9DA4 & ~1;
    }
}

static void BattleSpawnPartEffectTick(void) {
    func_800BBA84(D_801621F0[D_801590D4].D_801621F4, D_801590CC, 0);
    BattleSpawnPartEffect(D_801590CC, D_801621F0[D_801590D4].D_801621F2);
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

static void BattleApplyFacingFlipTick(void) {
    if (g_modelScreenPos[D_801590CC].prevX < g_modelScreenPos[D_801621F0[D_801590D4].unk8].prevX) {
        D_801621F0[D_801590D4].unk14 |= 0x100;
    }
    BattleSpawnFloatingIcon(D_801621F0[D_801590D4].unk8, D_801621F0[D_801590D4].unk14);
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

void BattleQueueImpactEffect(s32 arg0, s16 arg1) {
    func_800BBA84(0x14, arg1, 0);
    BattleSpawnActorRampEffect(arg0, 1, 1);
    BattleSpawnFloatingIcon(D_800FA6D4, 0x2C);
}

static void BattleFadeToGreyTick(void) {
    if (!D_801621F0[D_801590D4].D_801621F4) {
        D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

static void BattleResetModelScale(s16 arg0) {
    Unk80151200* dst = &D_80151200[arg0];
    dst->D_8015123C = 0x1000;
    dst->D_8015123A = 0x1000;
    D_80151200[arg0].D_80151238 = 0x1000;
    D_80151200[arg0].D_8015120C |= 0x20;
}

static void BattleSpawnEffectByType(s16 arg0, u8 arg1, u8 arg2) {
    s32 ret;

    BattleResetModelScale(arg0);
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

static void BattleReapplyEffectFromState(s16 arg0, u8 arg1) {
    s32 ret;

    BattleResetModelScale(arg0);
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

static void BattleEffectScriptTick(void) {
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

static void BattleEffectDelayedCleanupTick(void) {
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
static void BattleDispatchFlagEffect(void) {
    if ((D_800FA69C >> g_BattleEffectSlots[g_BattleEffectCursor].D_80162980) & 1) {
        BattleTriggerActorFlashMode1(g_BattleEffectSlots[g_BattleEffectCursor].D_80162980); // MBarrier
    } else if ((D_80163608 >> g_BattleEffectSlots[g_BattleEffectCursor].D_80162980) & 1) {
        BattleTriggerActorFlashMode0(g_BattleEffectSlots[g_BattleEffectCursor].D_80162980); // Barrier
    }
}

static void BattleFixedPointRampSpawnChildEffects(void) {
    s32 dst;

    if (!g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E != -1) {
            BattleDispatchFlagEffect();
            dst = func_800BC04C(func_800C2928);
            D_801621F0[dst].unk14 = g_BattleEffectSlots[g_BattleEffectCursor].unkE;
            D_801621F0[dst].unkE = g_BattleEffectSlots[g_BattleEffectCursor].D_80162982;
            D_801621F0[dst].unk10.ptr = (u8*)(u32)g_BattleEffectSlots[g_BattleEffectCursor].D_80162980;
            dst = func_800BC04C(BattleEffectDelayedCleanupTick);
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
void BattleFixedPointRampSpawnChildEffectsWithFade(void) {
    s32 dst;

    if (!g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].unkE & 2) {
            D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        }
        if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E != -1 &&
            g_BattleEffectSlots[g_BattleEffectCursor].unk18 != 1) {
            BattleDispatchFlagEffect();
            dst = func_800BC04C(func_800C2928);
            D_801621F0[dst].unk14 = g_BattleEffectSlots[g_BattleEffectCursor].unkE;
            D_801621F0[dst].unkE = g_BattleEffectSlots[g_BattleEffectCursor].D_80162982;
            D_801621F0[dst].unk10.ptr = (u8*)(u32)g_BattleEffectSlots[g_BattleEffectCursor].D_80162980;
            dst = func_800BC04C(BattleEffectDelayedCleanupTick);
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

static void BattleShakeModelYTick(void) {
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

static void BattleShakeModelAltYTick(void) {
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

static void BattleShakeModelXZTick(void) {
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

static void BattleSyncModelPositionTick(void) {
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

static void BattleInitModelMoveEffect(s16 arg0, s16 arg1, u8 arg2) {
    u8 dst;

    dst = BattleMovementRegister(BattleModelMoveTick);
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

static void BattleModelMoveTick(void) {
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

static void BattleModelCurveMoveTick(void) {
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

static void BattleModelBounceTick(void) {
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

static void BattleModelSettleTick(void) {
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

static void BattleApplyDelayedFlagTick(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        BattleSpawnFloatingIcon(D_801621F0[D_801590D4].unk8, D_801621F0[D_801590D4].D_801621F6);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

static void BattleApplyDelayedActionTick(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        func_800BBA84(D_801621F0[D_801590D4].D_801621F6, D_801590CC, 0);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

static void BattleApplyGatedDelayedTick(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        if (D_801518DC == 0) {
            BattleDispatchModelCommand(D_801621F0[D_801590D4].D_801621F6);
            D_801621F0[D_801590D4].D_801621F0 = -1;
        }
    } else {
        D_801621F0[D_801590D4].D_801621F4--;
    }
}

static void BattleFixedPointRampRepeatTick(void) {
    if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E == 0) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C == 0) {
            g_BattleEffectSlots[g_BattleEffectCursor].D_80162978 = -1;
            return;
        }
        BattleSetPendingMarkerPos(D_801518E4[D_801590CC].D_80151907, D_80151200[D_801590CC].D_8015123E);
        g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C--;
        return;
    }
    g_BattleEffectSlots[g_BattleEffectCursor].D_8016297E--;
}

void BattleLoadOverlaySector(s32 loc, s32 len) {
    SystemLoadFileBySector(loc, len, (u_long*)0x801B0000, NULL);
    BattleCdromReadChain();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D08B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleStorePlayerAnimScriptsPtr);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleRestorePlayerAnimScriptsPtr);

static void BattleNoOpTick(void) {}

static void BattleEffectTimeoutTick(void);
void BattleInitMagicCastEffect(void) {
    s32 ret;
    s32 i;

    for (i = 0; i < 3; i++) {
        D_801518E4[i].D_80151909 |= 1;
    }
    func_801B0040(D_80151774, D_801590CC);
    ret = func_800BC04C(BattleEffectTimeoutTick);
    *(s32*)0x1F800000 = ret;
    D_801621F0[ret].D_801621F4 = 2;
}

static void BattleEffectTimeoutTick(void) {
    if (!D_801621F0[D_801590D4].D_801621F4) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        func_800BB978();
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

static void BattleDispatchModelRunScript(u8 arg0) {
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

void BattleDispatchModelCommand(u8 arg0) {
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

static void BattleDispatchModelLoadImage(u8 arg0) {
    s32 lba;
    s32 var_a1;
    s32 id;

    switch (D_801518E4[arg0].D_80151907) {
    case 2:
        if (D_801031F0 == 0) {
            id = D_800EF63C[D_801518E4[arg0].D_80151906];
            BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        } else {
            switch (D_801518E4[arg0].D_80151906) {
            case 29:
                BattleLoadOverlaySector(D_800EEBB8[77].loc, D_800EEBB8[77].len);
                break;
            case 41:
                BattleLoadOverlaySector(D_800EEBB8[32].loc, D_800EEBB8[32].len);
                break;
            case 44:
                BattleLoadOverlaySector(D_800EEBB8[26].loc, D_800EEBB8[26].len);
                break;
            case 32:
                BattleLoadOverlaySector(D_800EEBB8[255].loc, D_800EEBB8[255].len);
                break;
            case 35:
                BattleLoadOverlaySector(D_800EEBB8[6].loc, D_800EEBB8[6].len);
                break;
            default:
                id = D_800EF63C[D_801518E4[arg0].D_80151906];
                lba = D_800EEBB8[id].loc;
                var_a1 = D_800EEBB8[id].len;
                BattleLoadOverlaySector(lba, var_a1);
                break;
            }
        }
        break;
    case 7:
        BattleLoadOverlaySector(D_800EEBB8[221].loc, D_800EEBB8[221].len);
        break;
    case 8:
        id = D_800EF8D8[D_801518E4[arg0].D_80151906];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 13:
        id = D_800EF6A8[D_801518E4[arg0].D_80151906];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 20:
        id = D_800EF838[D_801518E4[arg0].D_80151906];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 4:
        id = D_800EF5B0[D_801518E4[arg0].D_80151906];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 32:
        id = D_800EF6FC[D_801518E4[arg0].D_80151906];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 3:
        id = D_800EF6D8[D_801518E4[arg0].D_80151906];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
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

static void BattleSetLoadClutToVram(u_long* addr, s16 x, s16 y) {
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

static void BattleSetLoadTextureToVram(u_long* addr, s32 xy) {
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

void BattleSetLoadTimToVram(u_long* addr, s16 imgXY, s16 clutX, s16 clutY) {
    BattleSetLoadClutToVram(addr, clutX, clutY);
    BattleSetLoadTextureToVram(addr, imgXY);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D29D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D32B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3354);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleModelFlipR11R21R31);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleModelFlipR12R22R32);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleModelFlipR13R23R33);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleModelUpdateBoneHeight);

s32 BattleModelAnimReadDynamicFrameOffsBits(u8* arg0, s32* arg1) {
    s32 pos;
    s32 tmp;
    u8* p;
    s32 bit;
    s32 window;
    s32 mask;

    pos = *arg1;
    tmp = pos;
    if (pos < 0) {
        tmp = pos + 7;
    }
    p = arg0 + (tmp >> 3);
    bit = pos & 7;
    window = (p[0] << 8) | p[1];
    mask = 1 << (0xF - bit);
    if ((window & mask) == 0) {
        *arg1 = pos + 8;
        return (s32)(window << (bit + 1) << 16) >> 0x19;
    } else {
        window = (window << 8) | p[2];
        *arg1 = pos + 0x11;
        return (s32)(window << (bit + 1) << 8) >> 0x10;
    }
}

static s32 BattleModelAnimReadBitStream(u8* arg0, s32* arg1, s32 arg2) {
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

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleModelAnimReadEncryptedRotBits);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleModelReadAnimStream);

void BattleGetPartPosition(s32 arg0, s32 arg1, void* arg2);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleGetPartPosition);

// Take the low 16 bits of each of arg0's translation components relative to the
// camera D_800FA63C, then rotate that offset by the camera's transposed
// orientation into arg1.
static void BattleTransformToCameraSpace(MATRIX* arg0, SVECTOR* arg1) {
    MATRIX sp10;

    arg1->vx = (s16)(*(u16*)&arg0->t[0] - *(u16*)&D_800FA63C.m.t[0]);
    arg1->vy = (s16)(*(u16*)&arg0->t[1] - *(u16*)&D_800FA63C.m.t[1]);
    arg1->vz = (s16)(*(u16*)&arg0->t[2] - *(u16*)&D_800FA63C.m.t[2]);
    TransposeMatrix(&D_800FA63C.m, &sp10);
    ApplyMatrixSV(&sp10, arg1, arg1);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3AF0);

const MATRIX D_800A0D98 = {{{0, 0, 0}, {0, 0, 0}, {0, 0, 4096}}, {0, 0, 0}};
typedef struct {
    s16 x;
    s16 y;
    u8 u;
    u8 v;
    u8 w;
    u8 h;
    u8 r;
    u8 g;
    u8 b;
    u8 code;
} BattleSpriteDesc;
extern BattleSpriteDesc D_800F0218;
void* func_800D6260(BattleSpriteDesc* desc, u_long** ot, int otLen, void* prim);

void BattleEffectSingleDustCloud(void) {
    MATRIX m = D_800A0D98;
    long flag;
    Unk801621F0* slot;

    slot = &D_801621F0[D_801590D4];
    D_800F0218.u = slot->D_801621F2 * 32;
    SetRotMatrix(&D_800FA63C.m);
    SetTransMatrix(&D_800FA63C.m);
    RotTrans((SVECTOR*)&slot->D_801621F4, (VECTOR*)m.t, &flag);
    m.t[2] -= (s16)slot->unk10.unk.unk0 >> 4;
    m.m[0][0] = slot->unkE + ((slot->unkE * slot->D_801621F2) >> 3);
    m.m[1][1] = (s16)slot->unk10.unk.unk0 + (((s16)slot->unk10.unk.unk0 * slot->D_801621F2) >> 3);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    D_80163C74 = BattleEffectSpriteAdd(&D_800F0218, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (slot->D_801621F2++ >= 7) {
            slot->D_801621F0 = -1;
        }
    }
}

static void BattleEffectDustClouds(void) {
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
        temp_s0_2 = &D_801621F0[func_800BC04C(BattleEffectSingleDustCloud)];
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

static void BattleSpawnPartFlickerEffect(s32 arg0) {
    Unk801621F0* temp_v0;

    temp_v0 = &D_801621F0[func_800BC04C(BattleEffectDustClouds)];
    temp_v0->D_801621F0 = arg0;
    temp_v0->unkE = *(s16*)& temp_v0->unk10 = D_801518E4[arg0].D_801518EA;
}

void BattleSpawnSparkleEffect(Pair16x2* arg0, s16 arg1, s16 arg2) {
    Unk801621F0* dst;

    dst = &D_801621F0[func_800BC04C(func_800D3AF0)];
    *(Pair16x2*)&dst->D_801621F4 = *arg0;
    dst->unkE = arg1;
    dst->unk10.unk.unk0 = arg2;
}

static void BattleDelayedRotatedSpawnTick(void) {
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

static void BattleComputeRelativeMatrix(MATRIX* arg0, MATRIX* arg1, MATRIX* arg2) {
    arg2->t[0] = arg1->t[0] - arg0->t[0];
    arg2->t[1] = arg1->t[1] - arg0->t[1];
    arg2->t[2] = arg1->t[2] - arg0->t[2];
    TransposeMatrix(arg0, arg2);
    ApplyMatrixLV(arg2, (VECTOR*)arg2->t, (VECTOR*)arg2->t);
    MulMatrix(arg2, arg1);
}

BattleModelSub* BattleModelAdvance(SVECTOR* arg0, s32 arg1, BattleModelSub* arg2) {
    VECTOR normal;
    long flag;

    SetRotMatrix(&D_800FA63C.m);
    SetTransMatrix(&D_800FA63C.m);
    RotTrans(arg0, (VECTOR*)arg2->m.t, &flag);
    if (arg1 != 0) {
        VectorNormal((VECTOR*)arg2->m.t, &normal);
        arg2->m.t[0] = ((arg1 * normal.vx) >> 12) + arg2->m.t[0];
        arg2->m.t[1] = ((arg1 * normal.vy) >> 12) + arg2->m.t[1];
        arg2->m.t[2] = ((arg1 * normal.vz) >> 12) + arg2->m.t[2];
    }
    return arg2;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4368);

static void BattleAddDrawModePrim(u_long* ot, u16 tpage) {
    DR_MODE* dr_mode;

    dr_mode = D_80163C74;
    SetDrawMode(dr_mode, 0, 1, tpage, NULL);
    AddPrim(ot, (void*)dr_mode);
    D_80163C74 = dr_mode + 1;
}

const VECTOR D_800A0DB8 = {0, -4096, 0, 0};
void BattleMatrixFromDirection(SVECTOR* dir, MATRIX* m) {
    VECTOR side;
    VECTOR up = D_800A0DB8;
    VECTOR fwd;

    fwd.vx = dir->vx;
    fwd.vy = dir->vy;
    fwd.vz = dir->vz;
    VectorNormal(&fwd, &fwd);
    side.vx = fwd.vz;
    side.vy = 0;
    side.vz = -fwd.vx;
    VectorNormal(&side, &side);
    OuterProduct12(&fwd, &side, &up);
    VectorNormal(&up, &up);
    m->m[0][0] = side.vx;
    m->m[1][0] = side.vy;
    m->m[2][0] = side.vz;
    m->m[0][1] = up.vx;
    m->m[1][1] = up.vy;
    m->m[2][1] = up.vz;
    m->m[0][2] = fwd.vx;
    m->m[1][2] = fwd.vy;
    m->m[2][2] = fwd.vz;
}

void BattleMatrixOrthonormalize(MATRIX* m) {
    VECTOR side;
    VECTOR up;
    VECTOR fwd;

    up.vx = m->m[0][1];
    up.vy = m->m[1][1];
    up.vz = m->m[2][1];
    VectorNormal(&up, &up);
    m->m[0][1] = up.vx;
    m->m[1][1] = up.vy;
    m->m[2][1] = up.vz;
    side.vx = up.vy;
    side.vy = -up.vx;
    side.vz = 0;
    VectorNormal(&side, &side);
    m->m[0][0] = side.vx;
    m->m[1][0] = side.vy;
    m->m[2][0] = side.vz;
    OuterProduct12(&side, &up, &fwd);
    VectorNormal(&fwd, &fwd);
    m->m[0][2] = fwd.vx;
    m->m[1][2] = fwd.vy;
    m->m[2][2] = fwd.vz;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4710);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleSpawnKeyframeParticle);

void BattleKeyframeEffectTick(void) {
    Unk801621F0* slot;
    s8* key;
    s8* sub;
    s32 alive;
    u8 frame;
    u8 subFrame;

    slot = &D_801621F0[D_801590D4];
    if (D_80062D98 == 0) {
        alive = 0;
        key = (s8*)*(s32*)&slot->unkC;
        while ((s8)(frame = *key++) != -1) {
            if (*key != -2) {
                if ((s8)frame == slot->D_801621F2) {
                    BattleSpawnKeyframeParticle(key, (Pair16x2*)&slot->D_801621F4, slot);
                } else if (slot->D_801621F2 < (s8)frame) {
                    alive = 1;
                }
                key += 5;
            } else {
                sub = D_800F0C44[key[1]];
                key += 2;
                while ((s8)(subFrame = *sub++) != -1) {
                    if ((s8)frame + (s8)subFrame == slot->D_801621F2) {
                        BattleSpawnKeyframeParticle(sub, (Pair16x2*)&slot->D_801621F4, slot);
                    } else if (slot->D_801621F2 < (s8)frame + (s8)subFrame) {
                        alive = 1;
                    }
                    sub += 5;
                }
            }
        }
        if (alive == 0) {
            slot->D_801621F0 = -1;
        }
        slot->D_801621F2++;
    }
}

static void BattleSpawnFloatingIconAt(void* arg0, s32 arg1, s32 arg2);
void func_800D4C08(void* arg0, s32 arg1, s32 arg2, s32 arg3);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4C08);

static void BattleSpawnFloatingIconAtPart(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp10;

    BattleGetPartPosition(arg0, D_801518E4[arg0].D_8015190F, &sp10);
    func_800D4C08(&sp10, arg1, arg2, -D_801518E4[arg0].unk12);
}

static void BattleSpawnFloatingIcon(s32 arg0, s32 arg1) { BattleSpawnFloatingIconAtPart(arg0, arg1, 0x1000); }

static void BattleSpawnFloatingIconAt(void* arg0, s32 arg1, s32 arg2) { func_800D4C08(arg0, arg1, 0x1000, arg2); }

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

s32 BattleEventQueuePop(s16** arg0) {
    s32 ret;

    if (D_800F10D8 == D_800F10DC) {
        *arg0 = D_800F4D2C[(D_800F10D8 - 1) & 0xF];
        return 0;
    } else {
        ret = D_800F4CEC[D_800F10DC];
        *arg0 = D_800F4D2C[D_800F10DC];
        D_800F10DC = (D_800F10DC + 1) & 0xF;
        return ret;
    }
}
extern Unk80162978* D_800F10E0;

void BattleFixedPointRampTick();
void BattleFixedPointRampTick(void) {
    Unk80162978* slot = &g_BattleEffectSlots[g_BattleEffectCursor];
    s32 v;
    u8 c;

    if (D_80062D98 == 0) {
        if (*(s32*)&slot->unkC != 0) {
            v = *(s32*)&slot->D_8016297C + *(s32*)&slot->unkC;
            *(s32*)&slot->D_8016297C = v;
            if (v <= 0) {
                slot->D_80162978 = -1;
                D_800F10E0 = NULL;
                return;
            }
            if (v > 0xFFFF) {
                *(s32*)&slot->D_8016297C = 0xFFFF;
                *(s32*)&slot->unkC = 0;
            }
        }
    }
    c = ((u8*)&slot->D_8016297C)[1];
    D_80163C74 = (DR_MODE*)func_800C4FC8(c, c, c);
}

// Reset the fixed-point ramp: zero the accumulator (0x04) and seed the
// countdown (0x0C) so it lasts arg0 ticks.
static void BattleFixedPointRampInit(s32 arg0) {
    if (D_800F10E0 == NULL) {
        D_800F10E0 = &g_BattleEffectSlots[BattleEffectRegister(BattleFixedPointRampTick)];
    }
    *(s32*)&D_800F10E0->D_8016297C = 0;
    *(s32*)&D_800F10E0->unkC = 0x10000 / arg0;
}

void BattleFixedPointRampReconfigure(s32 arg0);
void BattleFixedPointRampReconfigure(s32 arg0) {
    if (D_800F10E0 != NULL) {
        *(s32*)&D_800F10E0->unkC = -*(s32*)&D_800F10E0->D_8016297C / arg0;
    }
}

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

void BattleFixedPointRampUpdateInit(s32 arg0, s32 arg1) {
    Unk80162978* slot;
    s32 accum;

    if (D_800F10E4 == 0) {
        slot = &g_BattleEffectSlots[BattleEffectRegister(BattleFixedPointRampUpdate)];
        accum = D_800F5B74 << 0x10;
        D_800F10E4 = (s32)slot;
        *(s32*)&slot->unkC = arg1;
        *(s32*)&slot->D_8016297C = accum;
        *(s32*)&slot->D_80162980 = ((arg0 << 0x10) - accum) / arg1;
    }
}

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

static s32 BattleCountSetBits(s32 arg0) {
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

s32 BattleCalcModelPartScale(s32 arg0) { return (D_801518E4[arg0].unk12 * 0x10) * D_801518E4[arg0].D_801518EA >> 0xC; }

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
void BattleQueueStatusChangeEvent(u32 arg0) {
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

static void BattleSpawnActorRampEffect(s32 arg0, s16 arg1, s16 arg2) {
    Unk80162978* temp_v0 = &g_BattleEffectSlots[BattleEffectRegister(func_800D57C0)];
    temp_v0->D_80162978 = 0;
    temp_v0->D_80162980 = arg0;
    temp_v0->D_8016297E = arg2;
    temp_v0->D_8016297C = arg1;
}

void func_800D5938();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5938);

static void BattleSpawnGlobalRampEffect(s16 arg0, s16 arg1) {
    Unk80162978* temp_v0;

    temp_v0 = &g_BattleEffectSlots[BattleEffectRegister(func_800D5938)];
    temp_v0->D_80162978 = 0;
    temp_v0->D_8016297E = arg1;
    temp_v0->D_8016297C = arg0;
}

// Divide each byte lane of a packed color independently by a divisor,
// yielding a per-channel step (e.g. a color-fade increment).
static s32 BattleDivideColorChannels(s32 arg0, s32 arg1) {
    return (((arg0 & 0xFF0000) / arg1) & 0xFF0000) | (((arg0 & 0xFF00) / arg1) & 0xFF00) |
           (((arg0 & 0xFF) / arg1) & 0xFF);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5B6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5D28);

void BattleSpawnBlinkEffect(s32 arg0, s16 arg1, u32 arg2, s32 arg3, s32 arg4) {
    Unk801621F0* dst = &D_801621F0[func_800BC04C(func_800D5D28)];

    dst->D_801621F0 = (s16)(arg2 >> 24);
    *(s32*)&dst->D_801621F4 = arg0;
    ((s16*)&dst->unk14)[0] = -arg1;
    *(s32*)&dst->unk8 = 0;
    *(s32*)&dst->unkC = (arg2 & 0xFFFFFF) | 0x3A000000;
    dst->unk10.ptr = (u8*)BattleDivideColorChannels(arg2, arg3);
    ((s16*)&dst->unk14)[1] = (s16)arg4;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleEffectSpriteAdd);

extern ModelRenderDesc D_800F14D0;

// Draw a model 4 times through func_800D29D4 (same request-struct pattern as
// barrier.c's D_801B0C98/D_801B0CB0), toggling the 0x1/0x2 flag bits between
// passes. Those bits mirror on X and Y, so the four passes are the four
// quadrants of a symmetric model built from one quarter.
static void BattleDrawSelectionMarker(s32* arg0, s16 arg1) {
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

static void BattleTriggerActorFlashMode0(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS; // Barrier
    func_800D6734(arg0, 0);
}

static void BattleTriggerActorFlashMode1(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_NO_CULL | MODEL_SEMI_TRANS; // MBarrier
    func_800D6734(arg0, 1);
}

void BattleTriggerActorFlashMode2(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
    func_800D6734(arg0, 2);
}

void BattleDrawHitFlashModel(MATRIX* m) {
    SetFarColor(0, 0, 0);
    SetRotMatrix(m);
    SetTransMatrix(m);
    D_800F1698.flags &= ~(MODEL_MIRROR_X | MODEL_MIRROR_Z);
    D_80163C74 = func_800D29D4(&D_800F1698, g_cDb->unk70, 0xC, D_80163C74);
    SetRotMatrix(m);
    D_800F1698.flags |= MODEL_MIRROR_X;
    D_80163C74 = func_800D29D4(&D_800F1698, g_cDb->unk70, 0xC, D_80163C74);
    SetRotMatrix(m);
    D_800F1698.flags |= MODEL_MIRROR_Z;
    D_80163C74 = func_800D29D4(&D_800F1698, g_cDb->unk70, 0xC, D_80163C74);
    SetRotMatrix(m);
    D_800F1698.flags &= ~MODEL_MIRROR_X;
    D_80163C74 = func_800D29D4(&D_800F1698, g_cDb->unk70, 0xC, D_80163C74);
    D_800F1698.uvOffset = 0;
    D_800F1698.clut = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleHitFlashGrowTick);

void BattleHitFlashBurstTick(void) {
    Unk801621F0* slot = &D_801621F0[D_801590D4];
    u16 v;

    D_800F16CC.m[0][0] = D_800F16CC.m[2][2] = (slot->D_801621F2 * 3) << 9;
    if (slot->D_801621F2 < 8) {
        D_800F16CC.m[1][1] = (slot->D_801621F2 * 3) << 10;
        D_800F1698.color = 0;
    } else if (slot->D_801621F2 < 16) {
        D_800F16CC.m[1][1] = 0x6000;
        D_800F1698.color = (slot->D_801621F2 - 8) << 9;
    }
    D_800F16CC.t[0] = slot->D_801621F4;
    D_800F16CC.t[1] = 0;
    D_800F16CC.t[2] = slot->unk8;
    CompMatrix(&D_800FA63C.m, &D_800F16CC, D_800F16EC);
    D_800F1698.model = D_800F14E0;
    BattleDrawHitFlashModel(D_800F16EC);
    if (D_80062D98 == 0) {
        v = slot->D_801621F2 + 1;
        slot->D_801621F2 = v;
        if ((s16)v == 16) {
            slot->D_801621F0 = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleHitFlashShrinkTick);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6D8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6F78);

void BattleSpawnTrailEffect(void) {
    Unk801621F0* src = &D_801621F0[D_801590D4];
    Unk801621F0* dst;
    u16 v;

    if (D_80062D98 == 0) {
        if (!(src->D_801621F2 & 3)) {
            dst = &D_801621F0[func_800BC04C((void (*)())src->unk1C)];
            *(Pair16x2*)&dst->D_801621F4 = *(Pair16x2*)&src->D_801621F4;
        }
        v = src->D_801621F2 + 1;
        src->D_801621F2 = v;
        if ((s16)v == 0xD) {
            src->D_801621F0 = -1;
        }
    }
}

void BattleSpawnPartEffect(s32 arg0, s32 arg1) {
    Unk801621F0* dst = &D_801621F0[func_800BC04C(BattleSpawnTrailEffect)];
    Unk801621F0* dst2;

    BattleGetPartPosition(arg0, D_801518E4[arg0].D_8015190F, (u8*)dst + 4);
    switch (arg1) {
    case 0:
        dst->unk1C = BattleHitFlashGrowTick;
        return;
    case 1:
        dst->unk1C = BattleHitFlashBurstTick;
        return;
    case 2:
        dst->unk1C = BattleHitFlashShrinkTick;
        return;
    case 3:
        dst->unk1C = BattleHitFlashGrowTick;
        dst2 = &D_801621F0[func_800BC04C(func_800D6F78)];
        *(Pair16x2*)&dst2->D_801621F4 = *(Pair16x2*)&dst->D_801621F4;
        return;
    }
}

static void BattleFixedPointRampEffectTick(void) {
    Unk801621F0* elem = &D_801621F0[D_801590D4];

    if (D_80062D98 == 0) {
        // Advance this slot's per-tick state machine (field 0x2).
        if (elem->D_801621F2 == 0) {
            BattleFixedPointRampInit(1);
        }
        if (elem->D_801621F2 == 2) {
            BattleFixedPointRampReconfigure(1);
            elem->D_801621F0 = -1;
        }
        elem->D_801621F2++;
    }
}

static void BattleSpawnFixedPointRampEffect(void) { BattleEffectRegister(BattleFixedPointRampEffectTick); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7368);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D751C);

void BattleSpawnStreakEffect(Pair16x2* arg0) {
    Unk801621F0* dst;

    dst = &D_801621F0[func_800BC04C(func_800D751C)];
    *(Pair16x2*)&dst->unk8 = *arg0;
    dst->D_801621F0 = 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleSubModelFlashTick);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7888);

void BattleSpawnSpriteEffect(s32 arg0, s32 arg1, s32 arg2, s16 arg3, s32 arg4, s32 arg5) {
    Unk801621F0* slot = &D_801621F0[func_800BC04C(func_800D7888)];

    slot->D_801621F4 = arg0;
    slot->D_801621F6 = arg1;
    slot->unk1C = arg2;
    slot->unk1A = arg3;
    slot->unk8 = (s16)arg4;
    slot->unkA = (s16)arg5;
}

void BattleSpawnSpriteEffectAtSubModel(s32 arg0, s32 idx, s32 arg2, s32 arg3) {
    BattleSpawnSpriteEffect(
        arg0, idx, &D_801518E4[idx].D_80151A58[D_801518E4[idx].subModelIdx], D_801518E4[idx].D_801518FE, arg2, arg3);
}

void BattleSpawnSpriteEffectAtSubModel2(s32 arg0, s32 idx, s32 arg2, s32 arg3) {
    BattleSpawnSpriteEffect(
        arg0, idx, &D_801518E4[idx].D_80151A58[D_801518E4[idx].subModelIdx2], D_801518E4[idx].D_80151900, arg2, arg3);
}

void BattleSpawnSpriteEffectAtBothSubModels(s32 arg0, s32 idx, s32 arg2, s32 arg3) {
    BattleSpawnSpriteEffect(
        arg0, idx, &D_801518E4[idx].D_80151A58[D_801518E4[idx].subModelIdx], D_801518E4[idx].D_801518FE, arg2, arg3);
    BattleSpawnSpriteEffect(
        arg0, idx, &D_801518E4[idx].D_80151A58[D_801518E4[idx].subModelIdx2], D_801518E4[idx].D_80151900, arg2, arg3);
}

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

static void BattleCopyEffectState(UnkStruct800D8468* dst, UnkStruct800D8468* src) {
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
