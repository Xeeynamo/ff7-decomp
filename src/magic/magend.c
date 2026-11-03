//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
// MAGEND.BIN: One particle per update; random rotation determines its velocity and gravity pulls it down.

extern s32 rand(void);

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos, Velocity;
    s16 unk14, unk16, unk18, unk1A, TargetIndex, SourceIndex;
} MagendEffect;
extern MagendEffect g_BattleEffectSlots[];
extern u_long D_801B04B4[];
extern s16 D_801B1730;
extern SpriteRenderDesc D_801B1734;
extern SVECTOR D_801B1740;
extern u8 D_801B1748[];
extern void* D_801C1748;
void RotTransSV(SVECTOR* input, SVECTOR* output, s32* flags);
static void func_magend_801B0448(s32 arg0, s32 arg1);

void MAGIC_Magend(s32 targetMask, s32 callbackArg) { func_magend_801B0448(targetMask, callbackArg); }

static void func_magend_801B0020(void) {
    MagendEffect* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801C1748 = (effect->AnimationFrame << 0xF) + D_801B1748;
    effect->AnimationFrame = (u16)effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

static void func_magend_801B0080(void) {
    MagendEffect* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x200, 0);
    D_801C1748 = func_800D4D90(&D_801B1734, g_cDb->unk70, 0xC, D_801C1748);
    if (D_80062D98 != 0) {
        return;
    }
    effect->Velocity.vy = (u16)effect->Velocity.vy + 3;
    effect->Pos.vx = (u16)effect->Pos.vx + (u16)effect->Velocity.vx;
    effect->Pos.vy = (u16)effect->Pos.vy + (u16)effect->Velocity.vy;
    effect->Pos.vz = (u16)effect->Pos.vz + (u16)effect->Velocity.vz;
    if ((effect->Pos.vy > 0) || (D_801B1730 == 1)) {
        effect->StartFrame = -1;
    }
    effect->AnimationFrame = (u16)effect->AnimationFrame + 1;
}

static void func_magend_801B0174(void) {
    SVECTOR rotation;
    MATRIX matrix;
    s32 flags;
    MagendEffect* particle;
    MagendEffect* effect;
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 != 0) {
        return;
    }
    if (effect->AnimationFrame < 56) {
        particle = &g_BattleEffectSlots[BattleEffectRegister(func_magend_801B0080)];
        BattleGetPartPosition(effect->SourceIndex, g_BattleModels[effect->SourceIndex].boneIndices[0], &particle->Pos);
        particle->Pos.vx += rand() % 200 - 100;
        particle->Pos.vy += rand() % 200 - 100;
        particle->Pos.vz += rand() % 200 - 100;
        rotation.vx = rand() % 500 + 350;
        rotation.vy = rand() % 500 - 250;
        rotation.vz = 0;
        RotMatrixYXZ(&rotation, &matrix);
        matrix.t[0] = matrix.t[1] = matrix.t[2] = 0;
        SetRotMatrix(&matrix);
        SetTransMatrix(&matrix);
        RotTransSV(&D_801B1740, &particle->Velocity, &flags);
    }
    effect->AnimationFrame++;
    if (effect->AnimationFrame == 56) {
        func_800D5774(effect->TargetIndex);
        D_801B1730 = 1;
        effect->StartFrame = -1;
    }
}

static void func_magend_801B03F4(s32 arg0, s32 arg1) {
    MagendEffect* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_magend_801B0174)];
    effect->TargetIndex = arg0;
    effect->SourceIndex = arg1;
}

static void func_magend_801B0448(s32 arg0, s32 arg1) {
    BattleEffectRegister(func_magend_801B0020);
    BattleSetLoadTimToVram(D_801B04B4, 0, 0, 0);
    MagicAnimationRegister(arg0, arg1, 4, func_magend_801B03F4);
}
