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
    /* 0x0000 */ char pad[MAGIC_PAGE_SIZE];
} MabariaPrimPage;

extern s32 D_801B0CA0;
extern s32 D_801B0CA4;
extern MabariaData D_80162978[];
extern ModelRenderDesc MabariaRenderDesc;
extern MabariaPrimPage MabariaPrimBuffer[];
extern void* MabariaBufferPtr;

static void MabariaMainSetup(s32 arg0, s32 arg1);

void MAGIC_MBarrier(s32 arg0, s32 arg1) { MabariaMainSetup(arg0, arg1); }

// Grows over the 16 frames, fading out over the last 8.
static void MabariaRenderModel(void) {
    MabariaData* effect = &D_80162978[D_8015169C];
    // D_801B0CA0 is always 0x2000: scale runs 0.5x on frame 0 to 7.5x on 15.
    s32 growth = effect->AnimationFrame * GROWTH_TOTAL / MABARIA_LIFETIME;
    MATRIX matrix;
    s32 fade;
    s16 nextFrame;
    VECTOR scale;

    scale.vx = scale.vy = scale.vz = ((growth + SCALE_BASE) * D_801B0CA0) >> FIXED_SHIFT;
    // D_801B0CA4 holds 0, so the lerp passes the fade through.
    if (effect->AnimationFrame < FADE_START_FRAME) {
        fade = 0;
    } else {
        fade = (effect->AnimationFrame - FADE_START_FRAME) * FADE_PER_FRAME;
    }
    fade += ((FIXED_ONE - fade) * D_801B0CA4) >> FIXED_SHIFT;
    RotMatrixYXZ(&effect->Rot, &matrix);
    matrix.t[0] = (s32)effect->Pos.vx;
    matrix.t[1] = (s32)effect->Pos.vy;
    matrix.t[2] = (s32)effect->Pos.vz;
    ScaleMatrix(&matrix, &scale);
    CompMatrix(&D_800FA63C.m, &matrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    MabariaRenderDesc.color = fade;
    MabariaBufferPtr = func_800D29D4(&MabariaRenderDesc, g_cDb->unk70, 0xC, MabariaBufferPtr);
    if (D_80062D98 == 0) {
        nextFrame = effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame >= MABARIA_LIFETIME) {
            effect->StartFrame = -1;
        }
    }
}

// Spawns a render instance on frames 0, 4 and 8; retires on 16.
static void MabariaAnimationUpdate(void) {
    MabariaData* effect = &D_80162978[D_8015169C];
    MabariaData* next;

    if (D_80062D98 != 0) {
        return;
    }

    if (effect->AnimationFrame == 0) {
        next = &D_80162978[BattleEffectRegister(MabariaRenderModel)];
        next->Pos = effect->Pos;
        next->Rot = effect->Rot;
    }

    if (effect->AnimationFrame == 4) {
        next = &D_80162978[BattleEffectRegister(MabariaRenderModel)];
        next->Pos = effect->Pos;
        next->Rot = effect->Rot;
    }

    if (effect->AnimationFrame == 8) {
        next = &D_80162978[BattleEffectRegister(MabariaRenderModel)];
        next->Pos = effect->Pos;
        next->Rot = effect->Rot;
    }

    if (effect->AnimationFrame == 16) {
        func_800D5774(effect->TargetIndex);
        effect->StartFrame = -1;
    }

    effect->AnimationFrame++;
}

static void MabariaAttachToTarget(s32 target) {
    MabariaData* effect = &D_80162978[BattleEffectRegister(MabariaAnimationUpdate)];

    BattleGetPartPosition(target, D_801518E4[target].D_8015190F, &effect->Pos);
    effect->Pos.vx = effect->Pos.vx - ((rsin(D_801518E4[target].unk160.vy) * D_801518E4[target].unk12) >> FIXED_SHIFT);
    effect->Pos.vz = effect->Pos.vz - ((rcos(D_801518E4[target].unk160.vy) * D_801518E4[target].unk12) >> FIXED_SHIFT);
    effect->Rot = D_801518E4[target].unk160;
    effect->TargetIndex = target;
}

static void MabariaDoubleBufferFlip(void) {
    MabariaData* effect = &D_80162978[D_8015169C];

    MabariaBufferPtr = &MabariaPrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame = effect->AnimationFrame ^ 1;
    if (D_80162080 < 2) {
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
