//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
// BOOST.BIN: A timed sequence of radial sprites followed by a target-position burst.

extern s32 rand(void);

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos, Velocity;
    s16 unk14, unk16, unk18, unk1A, TargetIndex, SourceIndex;
} BoostEffect;
extern BoostEffect g_BattleEffectSlots[];
extern void* D_80163C74;
extern u_long D_801B04CC[];
extern s32 D_801B8530;
extern s32 D_801B85F8;
extern EffectModel D_801B868C;
extern SpriteRenderDesc D_801BA264;
extern SpriteRenderDesc D_801BA270;
void func_800BBA84(s32 arg0, s32 arg1, s32 arg2);
static void func_boost_801B046C(s32 arg0, s32 arg1);

EffectModel* MAGIC_Boost(s32 targetMask, s32 callbackArg) {
    func_boost_801B046C(targetMask, callbackArg);
    return &D_801B868C;
}

static void func_boost_801B0028(void) {
    BoostEffect* effect;
    s16 nextFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2000, 0);
    D_801BA264.frameIndex = (u16)effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801BA264, g_cDb->unk70, 0xC, D_80163C74);
    if (D_80062D98 != 0) {
        return;
    }
    nextFrame = (u16)effect->AnimationFrame + 1;
    effect->AnimationFrame = nextFrame;
    if (nextFrame == D_801B85F8) {
        effect->StartFrame = -1;
    }
}

static void func_boost_801B00E0(void) {
    BoostEffect* effect;
    s16 nextFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2000, 0);
    D_801BA270.frameIndex = (u16)effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801BA270, g_cDb->unk70, 0xC, D_80163C74);
    if (D_80062D98 != 0) {
        return;
    }
    effect->Pos.vx += effect->Velocity.vx;
    effect->Pos.vy += effect->Velocity.vy;
    effect->Pos.vz += effect->Velocity.vz;
    nextFrame = ++effect->AnimationFrame;
    if (nextFrame == D_801B8530) {
        effect->StartFrame = -1;
    }
}

static void func_boost_801B01C8(void) {
    BoostEffect* particle;
    BoostEffect* effect;
    s16 nextIndex;
    u16 nextX;
    s16 nextFrame;
    s16 i;
    s32 modelOffset;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 != 0) {
        return;
    }
    if (effect->AnimationFrame == 0x20) {
        func_800BBA84(0x30, (s32)effect->SourceIndex, 0);
    }
    if (effect->AnimationFrame == 0x3A) {
        for (i = 0; i < 0x10;) {
            particle = &g_BattleEffectSlots[BattleEffectRegister(func_boost_801B00E0)];
            modelOffset = (s32)(i << 0x10) >> 8;
            nextX = effect->Velocity.vx;
            particle->Pos.vy = 0;
            particle->Pos.vx = nextX;
            particle->Pos.vz = (s16)(u16)effect->Velocity.vz;
            particle->Velocity.vx = (s16)((u32)(rsin(modelOffset) * 0x19) >> 9);
            particle->Velocity.vz = (s16)((u32)(rcos(modelOffset) * 0x19) >> 9);
            nextIndex = i + 1;
            i = nextIndex;
            particle->Velocity.vy = -rand() % 200;
        }
        BattleSpawnFloatingIconAtPart((s32)effect->TargetIndex, 0x21, 0x1000);
        func_800BBA84(0x15A, (s32)effect->SourceIndex, 0);
    }
    if (effect->AnimationFrame == 0x44) {
        particle = &g_BattleEffectSlots[BattleEffectRegister(func_boost_801B0028)];
        particle->Pos.vx = (s16)(u16)effect->Velocity.vx;
        particle->Pos.vy = (s16)(u16)effect->Velocity.vy;
        particle->Pos.vz = (s16)(u16)effect->Velocity.vz;
        func_800D5774((u32)effect->TargetIndex);
        func_800BBA84(0x141, (s32)effect->SourceIndex, 0);
    }
    nextFrame = (u16)effect->AnimationFrame + 1;
    effect->AnimationFrame = nextFrame;
    if (nextFrame == 0x64) {
        effect->StartFrame = -1;
    }
}

static void func_boost_801B03D8(s32 arg0, s32 arg1) {
    BoostEffect* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_boost_801B01C8)];
    BattleGetPartPosition(arg0, g_BattleModels[arg0].boneIndices[0], &effect->Velocity);
    effect->TargetIndex = (s16)arg0;
    effect->SourceIndex = arg1;
}

static void func_boost_801B046C(s32 arg0, s32 arg1) {
    BattleSetLoadTimToVram(D_801B04CC, 0, 0, 0);
    MagicAnimationRegister(arg0, arg1, 4, func_boost_801B03D8);
}
