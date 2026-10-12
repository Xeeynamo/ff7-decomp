//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include <libetc.h>
#include "magic.h"
#include "magic_private.h"
#include "queik.h"
#include "../battle/battle.h"

// Quake (クエイク / Quake), tier 1.

extern void* g_QueikBufferPtr;
extern s32* g_QueikFlipEffect;
extern char g_QueikPrimBuffer0[];
extern char g_QueikPrimBuffer1[];

// Only StartFrame/AnimationFrame are common to every magic overlay; the
// rest is payload this effect lays out for itself.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ s16 unkC;
    /* 0x0E */ s16 unkE;
    /* 0x10 */ s16 TargetIndex;
    /* 0x12 */ char pad12[0xE];
} QueikData; // size:0x20

extern QueikData g_BattleEffectSlots[];
extern s32 g_QueikModel[];
extern u_long g_QueikTexture[];
void QueikStart(s32 targetMask, s32 callbackArg);
void QueikTargetSetup(s32 target, s32 callbackArg);
void QueikTargetTick(void);

// Same as Lv5DeathBufferFlip.
static void QueikBufferFlip(void) {
    g_QueikBufferPtr = g_dbIndex == 0 ? g_QueikPrimBuffer0 : g_QueikPrimBuffer1;
    if (g_BattleEffectCount < 2) {
        *g_QueikFlipEffect = -1;
    }
}

void MAGIC_Quake(s32 targetMask, s32 callbackArg) { QueikStart(targetMask, callbackArg); }

void QueikTargetTick(void) {
    SVECTOR rot;
    MATRIX matrix;
    VECTOR scale;
    QueikData* effect;
    ModelRenderDesc* desc;

    desc = (ModelRenderDesc*)getScratchAddr(0);
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    RotMatrix(&rot, &matrix);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    scale.vx = scale.vy = scale.vz = (effect->unkC << 12) / 600;
    ScaleMatrix(&matrix, &scale);
    CompMatrix(&g_BattleWorldView.m, &matrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);

    desc->model = g_QueikModel;
    desc->flags = 0;
    desc->uvOffset = 0;
    desc->color = 0x80;
    desc->tpage = 0;
    desc->clut = 0;
    g_QueikBufferPtr = func_800D29D4(desc, g_cDb->unk70, 12, g_QueikBufferPtr);
    if (effect->AnimationFrame >= 0x16) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0xE) {
            func_800D5774(effect->TargetIndex);
        }
        if (effect->AnimationFrame < 8) {
            effect->unkC += effect->unkE;
        } else if (effect->AnimationFrame >= 0xE) {
            effect->unkC -= effect->unkE;
        }
        effect->AnimationFrame++;
    }
}

void QueikTargetSetup(s32 target, s32 callbackArg) {
    QueikData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(QueikTargetTick)];
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->Pos);
    effect->Pos.vy = 0;
    effect->unkC = 0;
    effect->unkE = g_BattleModels[target].collisionRadius + 0xE0;
    if (effect->unkE > 0x76C) {
        effect->unkE = 0x76C;
    }
    effect->unkE /= 8;
    effect->TargetIndex = target;
}

void QueikStart(s32 targetMask, s32 callbackArg) {
    g_QueikFlipEffect = (s32*)&g_BattleEffectSlots[BattleEffectRegister(QueikBufferFlip)];
    BattleSetLoadTimToVram(g_QueikTexture, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 1, QueikTargetSetup);
    BattleAkaoCommand(AKAO_PLAY_SOUND, BattleEntityGetStereoPan(targetMask), 0x47);
}
