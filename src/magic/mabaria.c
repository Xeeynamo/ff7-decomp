//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"
#include "magic_private.h"

// Frame 16 would land on 0x7FFE, just under ScaleMatrix's 0x7FFF s16 ceiling.
#define GROWTH_TOTAL 0x3BFF
#define SCALE_BASE 0x400

// Fade is the GTE depth cue toward SetFarColor, black here, reaching 0xE00 on
// frame 15. Drawn semi-transparent and blended additively, so black vanishes.
#define MABARIA_LIFETIME 16
#define FADE_START_FRAME 8
#define FADE_PER_FRAME 0x200

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR Rot;
    /* 0x14 */ s16 TargetIndex;
    /* 0x16 */ char pad16[0xA]; // untouched by the overlay
} MabariaData;                  // size:0x20

typedef struct {
    /* 0x00 */ char pad[MAGIC_PAGE_SIZE];
} MabariaPrimPage; // size:0x10000

extern s32 D_801B0CA0;
extern s32 D_801B0CA4;
extern MabariaData g_BattleEffectSlots[];
extern ModelRenderDesc g_MabariaRenderDesc;
extern MabariaPrimPage g_MabariaPrimBuffer[];
extern void* g_MabariaBufferPtr;

static void MabariaMainSetup(s32 arg0, s32 arg1);

void MAGIC_MBarrier(s32 arg0, s32 arg1) { MabariaMainSetup(arg0, arg1); }

static void MabariaRenderModel(void) {
    MabariaData* effect;
    s32 growth;
    MATRIX matrix;
    s32 fade;
    VECTOR scale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    growth = effect->AnimationFrame * GROWTH_TOTAL / MABARIA_LIFETIME;
    scale.vx = scale.vy = scale.vz = ((growth + SCALE_BASE) * D_801B0CA0) >> 12;
    // D_801B0CA4 holds 0, so the lerp passes the fade through.
    if (effect->AnimationFrame < FADE_START_FRAME) {
        fade = 0;
    } else {
        fade = (effect->AnimationFrame - FADE_START_FRAME) * FADE_PER_FRAME;
    }
    fade += ((4096 - fade) * D_801B0CA4) >> 12;
    RotMatrixYXZ(&effect->Rot, &matrix);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    ScaleMatrix(&matrix, &scale);
    CompMatrix(&D_800FA63C.m, &matrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    g_MabariaRenderDesc.color = fade;
    g_MabariaBufferPtr = func_800D29D4(&g_MabariaRenderDesc, g_cDb->unk70, 0xC, g_MabariaBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= MABARIA_LIFETIME) {
            effect->StartFrame = -1;
        }
    }
}

static void MabariaAnimationUpdate(void) {
    MabariaData* effect;
    MabariaData* next;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 != 0) {
        return;
    }

    if (effect->AnimationFrame == 0) {
        next = &g_BattleEffectSlots[BattleEffectRegister(MabariaRenderModel)];
        next->Pos = effect->Pos;
        next->Rot = effect->Rot;
    }

    if (effect->AnimationFrame == 4) {
        next = &g_BattleEffectSlots[BattleEffectRegister(MabariaRenderModel)];
        next->Pos = effect->Pos;
        next->Rot = effect->Rot;
    }

    if (effect->AnimationFrame == 8) {
        next = &g_BattleEffectSlots[BattleEffectRegister(MabariaRenderModel)];
        next->Pos = effect->Pos;
        next->Rot = effect->Rot;
    }

    if (effect->AnimationFrame == 16) {
        func_800D5774(effect->TargetIndex);
        effect->StartFrame = -1;
    }

    effect->AnimationFrame++;
}

static void MabariaAttachToTarget(s32 target, s32 arg1) {
    MabariaData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(MabariaAnimationUpdate)];
    BattleGetPartPosition(target, D_801518E4[target].D_8015190F, &effect->Pos);
    effect->Pos.vx = effect->Pos.vx - ((rsin(D_801518E4[target].unk160.vy) * D_801518E4[target].unk12) >> 12);
    effect->Pos.vz = effect->Pos.vz - ((rcos(D_801518E4[target].unk160.vy) * D_801518E4[target].unk12) >> 12);
    effect->Rot = D_801518E4[target].unk160;
    effect->TargetIndex = target;
}

static void MabariaDoubleBufferFlip(void) {
    MabariaData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_MabariaBufferPtr = &g_MabariaPrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame = effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

static void MabariaMainSetup(s32 arg0, s32 arg1) {
    D_801B0CA0 = 0x2000;
    D_801B0CA4 = 0;
    BattleEffectRegister(MabariaDoubleBufferFlip);
    MagicAnimationRegister(arg0, arg1, 0, MabariaAttachToTarget);
    BattleCommandSend(0x20, 0x40, 0x43);
}
