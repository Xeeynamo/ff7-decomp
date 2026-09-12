//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic_private.h"
#include "../battle/battle.h"

// Three rings spawn 8 frames apart, each growing a little faster than the
// last; Scale starts at RING_START_SCALE and gains ScaleStep every frame.
#define RING_LIFETIME 16
#define RING_START_SCALE 0x200

// Fade is the GTE depth cue toward SetFarColor, black here, so a ring
// dissolves into the background as it approaches 0x1000.
#define FADE_START_FRAME 8
#define FADE_PER_FRAME 0x200

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR Rot;
    /* 0x14 */ s16 Scale;
    /* 0x16 */ s16 ScaleStep;
    /* 0x18 */ s16 TargetIndex;
    /* 0x1A */ char pad1A[6]; // untouched by the overlay
} RefrecData;                 // size:0x20

extern s32 g_RefrecBaseScale;
extern s32 D_801B0C14;
extern u8 g_RefrecPrimBuffer[2][MAGIC_PAGE_SIZE];
extern void* g_RefrecBufferPtr;
extern RefrecData g_BattleEffectSlots[];
extern ModelRenderDesc g_RefrecRenderDesc;

static void RefrecMainSetup(s32 targetMask, s32 callbackArg);

void MAGIC_Refrec(s32 targetMask, s32 callbackArg) { RefrecMainSetup(targetMask, callbackArg); }

static void RefrecRenderModel(void) {
    VECTOR scale;
    MATRIX matrix;
    RefrecData* effect;
    s32 growth;
    s32 fade;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    growth = (effect->Scale * g_RefrecBaseScale) >> 12;
    scale.vx = scale.vy = scale.vz = growth;
    if (effect->AnimationFrame < FADE_START_FRAME) {
        fade = 0;
    } else {
        fade = (effect->AnimationFrame - FADE_START_FRAME) * FADE_PER_FRAME;
    }
    // D_801B0C14 holds 0, so the lerp passes the fade through.
    fade += ((4096 - fade) * D_801B0C14) >> 12;
    RotMatrixYXZ(&effect->Rot, &matrix);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    ScaleMatrix(&matrix, &scale);
    CompMatrix(&D_800FA63C.m, &matrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    g_RefrecRenderDesc.color = fade;
    g_RefrecBufferPtr = func_800D29D4(&g_RefrecRenderDesc, g_cDb->unk70, 0xC, g_RefrecBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= RING_LIFETIME) {
            effect->StartFrame = -1;
        } else {
            effect->Scale = effect->Scale + effect->ScaleStep;
        }
    }
}

static void RefrecAnimationUpdate(void) {
    RefrecData* next;
    RefrecData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            next = &g_BattleEffectSlots[BattleEffectRegister(RefrecRenderModel)];
            next->Pos = effect->Pos;
            next->Rot = effect->Rot;
            next->Scale = RING_START_SCALE;
            next->ScaleStep = 0x160;
        }
        if (effect->AnimationFrame == 8) {
            next = &g_BattleEffectSlots[BattleEffectRegister(RefrecRenderModel)];
            next->Pos = effect->Pos;
            next->Rot = effect->Rot;
            next->Scale = RING_START_SCALE;
            next->ScaleStep = 0x180;
        }
        if (effect->AnimationFrame == 0x10) {
            next = &g_BattleEffectSlots[BattleEffectRegister(RefrecRenderModel)];
            next->Pos = effect->Pos;
            next->Rot = effect->Rot;
            next->Scale = RING_START_SCALE;
            next->ScaleStep = 0x1A0;
        }
        if (effect->AnimationFrame == 0x18) {
            func_800D5774(effect->TargetIndex);
            effect->StartFrame = -1;
        }
        effect->AnimationFrame++;
    }
}

static void RefrecAttachToTarget(s32 target, s32 callbackArg) {
    BattleModel* model;
    RefrecData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(RefrecAnimationUpdate)];
    model = &D_801518E4[target];
    BattleGetPartPosition(target, model->D_8015190F, &effect->Pos);
    effect->Pos.vx = effect->Pos.vx - ((rsin(model->unk160.vy) * model->unk12) >> 12);
    effect->Pos.vz -= (rcos(model->unk160.vy) * model->unk12) >> 12;
    effect->Rot = model->unk160;
    effect->TargetIndex = target;
}

static void RefrecDoubleBufferFlip(void) {
    RefrecData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_RefrecBufferPtr = g_RefrecPrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame = effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

static void RefrecMainSetup(s32 targetMask, s32 callbackArg) {
    g_RefrecBaseScale = 0x2000;
    D_801B0C14 = 0;
    BattleEffectRegister(RefrecDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 0, RefrecAttachToTarget);
    BattleCommandSend(0x20, 0x40, 0x48);
}
