//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "magic_private.h"
#include "gravide.h"
#include "../battle/battle.h"

// Demi (グラビデ / Gravide), tier 1.

#define GRAVIDE_PAGE_SIZE 0xF000
#define TRAVEL_FRAMES 24 // the effect covers the caster-to-target distance in this many frames

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR Step; // per-frame movement, set from the target's position
    /* 0x14 */ s16 Scale;
    /* 0x16 */ s16 ScaleStep;
    /* 0x18 */ s16 TargetIndex;
    /* 0x1A */ char pad1A[6];
} GravideData; // size:0x20

extern SpriteRenderDesc g_GravideDescB; // 0x2C
extern SpriteRenderDesc g_GravideDescC; // 0x2C
extern SpriteRenderDesc g_GravideDescA; // 0x2E
extern GravideData g_BattleEffectSlots[];
extern u8 g_GravideBuffer[2][GRAVIDE_PAGE_SIZE];
extern u_long g_GravideTexture[];
extern void* g_GravideBufferPtr; // must land right after the buffer
void func_801B0000(void);
void func_801B00F0(void);
void func_801B01F4(void);
void GravideDoubleBufferFlip(void);
void GravideTargetSetup(s32 targetMask, s32 callbackArg);
void GravideTargetTick(void);

void func_801B0000(void) {
    GravideData* effect;
    MATRIX* matrix;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_GravideDescA.frameIndex = effect->AnimationFrame;
    matrix = BattleSetBillboardMatrix(&effect->Pos, 0x800, 0);
    if (matrix->t[2] > 0) {
        BattleAddDrawModePrim(g_cDb->unk70[matrix->t[2] >> 4], 0x20);
        g_GravideBufferPtr = func_800D4D90(&g_GravideDescA, g_cDb->unk70, 12, g_GravideBufferPtr);
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame += 3;
        if (effect->AnimationFrame >= 5) {
            effect->StartFrame = -1;
        }
    }
}

void func_801B00F0(void) {
    GravideData* effect;
    MATRIX* matrix;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    matrix = BattleSetBillboardMatrix(&effect->Pos, effect->Scale, -(effect->Scale >> 4));
    if (matrix->t[2] > 0) {
        BattleAddDrawModePrim(g_cDb->unk70[matrix->t[2] >> 4], 0x20);
        g_GravideBufferPtr = func_800D4D90(&g_GravideDescB, g_cDb->unk70, 12, g_GravideBufferPtr);
    }
    if (D_80062D98 == 0) {
        effect->Scale += effect->ScaleStep;
        effect->ScaleStep -= 0x28;
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 0x24) {
            effect->StartFrame = -1;
        }
    }
}

void func_801B01F4(void) {
    GravideData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_GravideDescC.frameIndex = effect->AnimationFrame % 12;
    BattleSetBillboardMatrix(&effect->Pos, 0x1000, -200);
    g_GravideBufferPtr = func_800D4D90(&g_GravideDescC, g_cDb->unk70, 12, g_GravideBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 0x24) {
            effect->StartFrame = -1;
        }
    }
}

void GravideTargetTick(void) {
    GravideData* next;
    GravideData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        next = &g_BattleEffectSlots[BattleEffectRegister(func_801B0000)];
        next->Pos = effect->Pos;
        next->AnimationFrame = 2;
        effect->Pos.vx += effect->Step.vx;
        effect->Pos.vy += effect->Step.vy;
        effect->Pos.vz += effect->Step.vz;
        next = &g_BattleEffectSlots[BattleEffectRegister(func_801B0000)];
        next->Pos = effect->Pos;
        next->AnimationFrame = 1;
        effect->Pos.vx += effect->Step.vx;
        effect->Pos.vy += effect->Step.vy;
        effect->Pos.vz += effect->Step.vz;
        next = &g_BattleEffectSlots[BattleEffectRegister(func_801B0000)];
        next->Pos = effect->Pos;
        effect->Pos.vx += effect->Step.vx;
        effect->Pos.vy += effect->Step.vy;
        effect->Pos.vz += effect->Step.vz;
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 8) {
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B01F4)];
            next->Pos = effect->Pos;
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B00F0)];
            next->Pos = effect->Pos;
            next->Scale = 0x200;
            next->ScaleStep = 0x2D0;
            func_800D5774(effect->TargetIndex);
            BattleAkaoCommand(
                AKAO_PLAY_SOUND, BattlePositionToStereoPan(&g_BattleModels[effect->TargetIndex].rootTrans),
                SFX_LEVIATHAN_LOW_CHAN);
            effect->StartFrame = -1;
        }
    }
}

void GravideTargetSetup(s32 target, s32 callbackArg) {
    GravideData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(GravideTargetTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[0], &effect->Pos);
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->Step);
    effect->TargetIndex = target;
    effect->Step.vx = (effect->Step.vx - effect->Pos.vx) / TRAVEL_FRAMES;
    effect->Step.vy = (effect->Step.vy - effect->Pos.vy) / TRAVEL_FRAMES;
    effect->Step.vz = (effect->Step.vz - effect->Pos.vz) / TRAVEL_FRAMES;
    BattleAkaoCommand(AKAO_PLAY_SOUND, BattlePositionToStereoPan(&g_BattleModels[callbackArg].rootTrans), SFX_GRAVIDE);
}

// Same as BarrierDoubleBufferFlip, with 0xF000 byte pages.
void GravideDoubleBufferFlip(void) {
    GravideData* flip;

    flip = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_GravideBufferPtr = g_GravideBuffer[flip->AnimationFrame];
    flip->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        flip->StartFrame = -1;
    }
}

void MAGIC_Gravide(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(g_GravideTexture, 0, 0, 0);
    BattleEffectRegister(GravideDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 8, GravideTargetSetup);
}
