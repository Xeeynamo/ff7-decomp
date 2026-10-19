//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
// HAKAI.BIN: Random body-part sprites alternate between two frame sequences while the screen effect pulses.

extern s32 rand(void);

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos, Velocity;
    s16 unk14, unk16, unk18, unk1A, TargetIndex, SourceIndex;
} HakaiEffect;
extern HakaiEffect g_BattleEffectSlots[];
extern s32 D_80163C74;
extern u_long D_801B04C4[];
extern SpriteRenderDesc D_801B6754;
extern SpriteAnim* D_801B6760[];
extern u8 D_801B6768[];
extern void* D_801CE768;
void func_800BBA84(s32 arg0, s32 arg1, s32 arg2);
s32 func_800C4FC8(s32 arg0, s32 arg1, s16 arg2);
static void func_hakai_801B0448(s32 arg0, s32 arg1);

void MAGIC_Hakai(s32 targetMask, s32 callbackArg) { func_hakai_801B0448(targetMask, callbackArg); }

static void func_hakai_801B0020(void) {
    HakaiEffect* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801CE768 = (effect->AnimationFrame * 0xC000) + D_801B6768;
    effect->AnimationFrame = (u16)effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

static void func_hakai_801B008C(void) {
    HakaiEffect* effect;
    s16 nextFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, (s32)effect->unk18, (s32)effect->unk14);
    D_801B6754.frames = D_801B6760[effect->unk16];
    D_801B6754.frameIndex = (u16)effect->AnimationFrame;
    D_801CE768 = func_800D4D90(&D_801B6754, g_cDb->unk70, 0xC, D_801CE768);
    if (D_80062D98 != 0) {
        return;
    }
    nextFrame = (u16)effect->AnimationFrame + 1;
    effect->AnimationFrame = nextFrame;
    if (nextFrame == D_801B6760[effect->unk16]->frameCount) {
        effect->StartFrame = -1;
    }
}

static void func_hakai_801B0180(void) {
    HakaiEffect* effect;
    HakaiEffect* particle;
    s16 nextFrameValue;
    u16 randomScale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_80163C74 = func_800C4FC8(0, 0, (effect->AnimationFrame % 2) * 32 + 32);
    if (D_80062D98 != 0) {
        return;
    }
    if (effect->AnimationFrame < 0x16) {
        particle = &g_BattleEffectSlots[BattleEffectRegister(func_hakai_801B008C)];
        BattleGetPartPosition(
            effect->TargetIndex, rand() % g_BattleModels[effect->TargetIndex].numBones, &particle->Pos);
        particle->unk14 = -(s16)g_BattleModels[effect->TargetIndex].collisionRadius;
        randomScale = g_BattleModels[effect->TargetIndex].collisionRadius;
        if (g_BattleModels[effect->TargetIndex].collisionRadius >= 0xAAB) {
            randomScale = 0xAAA;
        }
        particle->unk18 = (s16)randomScale * 6 + rand() % ((s16)randomScale * 6);
        particle->unk16 = rand() % 2;
    }
    nextFrameValue = (u16)effect->AnimationFrame + 1;
    effect->AnimationFrame = nextFrameValue;
    if (nextFrameValue == 0x1E) {
        func_800D5774((u32)effect->TargetIndex);
        effect->StartFrame = -1;
    }
}

static void func_hakai_801B03F4(s32 arg0, s32 arg1) {
    HakaiEffect* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_hakai_801B0180)];
    effect->TargetIndex = arg0;
    effect->SourceIndex = arg1;
}

static void func_hakai_801B0448(s32 arg0, s32 arg1) {
    BattleEffectRegister(func_hakai_801B0020);
    BattleSetLoadTimToVram(D_801B04C4, 0, 0, 0);
    MagicAnimationRegister(arg0, arg1, 4, func_hakai_801B03F4);
    func_800BBA84(0x186, arg1, 0);
}
