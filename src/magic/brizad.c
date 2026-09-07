//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"
#include "magic_private.h"

// Ice (ブリザド / Blizzard), tier 1. Structurally a sibling of barrier.c:
// a 3D model effect built through func_800D29D4 with the PSYQ matrix helpers,
// double-buffered into a 0x20000 primitive page per frame.
//
// The model lives in this overlay's data segment at 0x801B03F0: 84 vertices
// and 120 Gouraud triangles, no textures, forming a twelve-spike burst whose
// tips point at the vertices of an icosahedron.

// .bss -- laid out so BrizadBufferPtr lands immediately after the buffer, at
// 0x801B1014 + 0x20000. Matches the BarrierPrimBuffer/BarrierBufferPtr pair.
static char BrizadPrimBuffer[2 * MAGIC_PAGE_SIZE];
static void* BrizadBufferPtr; // current write pointer into the above

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ s16 TargetIndex;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ SVECTOR Rot; // vx/vy/vz cleared together; 0x0E pad untouched
    /* 0x10 */ SVECTOR Pos;
    /* 0x18 */ s16 Scale;
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
} BrizadData; // size:0x20

// Frames 0..14; the model grows and fades out. func_800D29D4 loads the fade
// as the GTE's depth-cue factor, driving the vertex colour toward
// SetFarColor, black here. Every primitive is emitted semi-transparent and
// the GPU blends additively, so black adds nothing and the model fades to
// invisible rather than to a dark shape. Both rates are per (BRIZAD_LIFETIME
// - 1) frames and the divisions truncate exactly; the compiler emits them as
// shift-add chains. By the last frame growth reaches 0x2FF6 (2.998x), fade
// 0xFF8.
#define BRIZAD_LIFETIME 15
#define GROWTH_PER_FRAME (3 * FIXED_ONE / (BRIZAD_LIFETIME - 1)) // 0x36D
#define FADE_PER_FRAME (FIXED_ONE / (BRIZAD_LIFETIME - 1))       // 0x124

// ScaleMatrix writes into MATRIX.m, which is s16, so the scale clamps here.
#define SCALE_MAX 0x7FFF

extern ModelRenderDesc BrizadRenderDesc;
extern BrizadData D_80162978[];
extern s16 D_80151774;

// Render pass; registered by BrizadSpawnIce, so it takes no arguments.
static void BrizadRenderIce(void) {
    MATRIX matrix;
    VECTOR scaleVec;
    BrizadData* effect = &D_80162978[D_8015169C];
    s16 nextFrame;
    s16 frame;
    s16 fade;
    s32 growth = (effect->Scale * GROWTH_PER_FRAME);
    s32 scale = (s32)(effect->AnimationFrame * growth) >> FIXED_SHIFT;

    if (scale > SCALE_MAX) {
        scale = SCALE_MAX;
    }
    scaleVec.vx = scaleVec.vy = scaleVec.vz = scale;
    frame = effect->AnimationFrame;
    if (frame < 0) {
        fade = 0;
    } else {
        fade = frame * FADE_PER_FRAME;
    }
    RotMatrixYXZ(&effect->Rot, &matrix);
    ScaleMatrix(&matrix, &scaleVec);
    matrix.t[0] = (s32)effect->Pos.vx;
    matrix.t[1] = (s32)effect->Pos.vy;
    matrix.t[2] = (s32)effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, &matrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    BrizadRenderDesc.color = fade;
    BrizadBufferPtr = func_800D29D4(&BrizadRenderDesc, g_cDb->unk70, 12, BrizadBufferPtr);
    if (D_80062D98 == 0) {
        nextFrame = (u16)effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame >= BRIZAD_LIFETIME) {
            effect->StartFrame = -1;
        }
    }
}

static void BrizadSpawnIce(void) {
    BrizadData* next;
    BrizadData* effect = &D_80162978[D_8015169C];

    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            next = &D_80162978[BattleEffectRegister(BrizadRenderIce)];
            BattleGetPartPosition(effect->TargetIndex, D_801518E4[effect->TargetIndex].D_8015190F, &next->Pos);
            next->Rot.vz = 0;
            next->Rot.vy = 0;
            next->Rot.vx = 0;
            next->Scale = func_800D55A4(effect->TargetIndex);
            func_800D5774(effect->TargetIndex);
            if (effect->AnimationFrame == 0) {
                effect->StartFrame = -1;
            }
        }
        effect->AnimationFrame++;
    }
}

// Byte-identical twin of BrizadAttachToTarget below, present in the original
// and never registered by this overlay. Kept so the layout matches.
static void BrizadAttachToTargetUnused(s32 target) {
    D_80162978[BattleEffectRegister(BrizadSpawnIce)].TargetIndex = target;
}

// Registered as the MagicAnimationRegister per-target callback by MAGIC_Brizad.
// Spawns a BrizadSpawnIce slot and tells it which target it belongs to.
static void BrizadAttachToTarget(s32 target) { D_80162978[BattleEffectRegister(BrizadSpawnIce)].TargetIndex = target; }

// Points the write pointer at this frame's page, then toggles to the other one.
// This slot uses AnimationFrame as a 0/1 page index, not as a frame counter.
// Compare BarrierDoubleBufferFlip in barrier.c and Lv5DeathBufferFlip.
static void BrizadDoubleBufferFlip(void) {
    BrizadData* flip = &D_80162978[D_8015169C];

    BrizadBufferPtr = &BrizadPrimBuffer[flip->AnimationFrame * MAGIC_PAGE_SIZE];
    flip->AnimationFrame = (u16)flip->AnimationFrame ^ 1;
    if (D_80162080 < 2) {
        flip->StartFrame = -1;
    }
}

// Overlay entry, at 0x801B037C; the D_800EFAF0 dispatch table sends the ice
// magic id here. Registers the buffer flip and the per-target callback, then
// fires the SFX. Unlike barrier and lv5deth this is not a trampoline -- it is
// the last function in the overlay, so it must stay last in this file.
void MAGIC_Brizad(s32 targetMask, s32 arg1) {
    BattleEffectRegister(BrizadDoubleBufferFlip);
    MagicAnimationRegister(targetMask, arg1, 4, BrizadAttachToTarget);
    BattleCommandSend(0x20, BattleEntityGetStereoPan((s32)D_80151774), 0x18);
}
