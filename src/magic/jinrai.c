//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
// JINRAI.BIN: Two target-position sprite bursts, scheduled at frames 22 and 55.

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos, TargetPos;
    s16 unk14, unk16, unk18, unk1A, TargetIndex, SourceIndex;
} JinraiEffect;
extern JinraiEffect g_BattleEffectSlots[];
extern SpriteRenderDesc D_801B5220;
extern EffectModel D_801B3470;
extern u_long D_801B0310[];
extern s32 D_801B3354;
extern void* D_80163C74;
static void func_jinrai_801B02B0(s32, s32);

void func_800BBA84(s32, s32, s32);

EffectModel* MAGIC_Jinrai(s32 arg0, s32 arg1) {
    func_jinrai_801B02B0(arg0, arg1);
    return &D_801B3470;
}

static void func_jinrai_801B0028(void) {
    JinraiEffect* effect;
    s16 nextFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x3000, 0);
    D_801B5220.frameIndex = (u16)effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801B5220, g_cDb->unk70, 0xC, D_80163C74);
    if (D_80062D98 == 0) {
        nextFrame = (u16)effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame == D_801B3354) {
            effect->StartFrame = -1;
        }
    }
}

static void func_jinrai_801B00E0(void) {
    JinraiEffect* effect;
    JinraiEffect* particle;
    s16 nextFrame;
    s16 startFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        startFrame = effect->AnimationFrame;
        if ((startFrame == 0x16) || (startFrame == 0x37)) {
            particle = &g_BattleEffectSlots[BattleEffectRegister(func_jinrai_801B0028)];
            particle->Pos.vx = (s16)(u16)effect->TargetPos.vx;
            particle->Pos.vy = (s16)(u16)effect->TargetPos.vy;
            particle->Pos.vz = (s16)(u16)effect->TargetPos.vz;
            func_800BBA84(0x155, (s32)effect->SourceIndex, 0);
        }
        if (effect->AnimationFrame == 0x1E) {
            func_800BBA84(0x30, (s32)effect->SourceIndex, 0);
        }
        if (effect->AnimationFrame == 0x37) {
            func_800D5774((u32)effect->TargetIndex);
        }
        nextFrame = (u16)effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame == 0x46) {
            effect->StartFrame = -1;
        }
    }
}

static void func_jinrai_801B01E4(s32 arg0, s32 arg1) {
    JinraiEffect* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_jinrai_801B00E0)];
    BattleGetPartPosition(arg1, g_BattleModels[arg1].boneIndices[0], &effect->Pos);
    BattleGetPartPosition(arg0, g_BattleModels[arg0].boneIndices[0], &effect->TargetPos);
    effect->TargetIndex = (s16)arg0;
    effect->SourceIndex = (s16)arg1;
}

static void func_jinrai_801B02B0(s32 arg0, s32 arg1) {
    BattleSetLoadTimToVram(D_801B0310, 0, 0, 0);
    MagicAnimationRegister(arg0, arg1, 4, func_jinrai_801B01E4);
}
