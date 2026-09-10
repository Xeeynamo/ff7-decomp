//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"
#include "magic_private.h"

// Ice (ブリザド / Blizzard), tier 1.
//
// The model at 0x801B03F0 is 84 vertices and 120 untextured Gouraud
// triangles: a twelve-spike burst whose tips point at the vertices of an
// icosahedron.

// brizad_buffer_ptr must land immediately after the buffer, at 0x801B1014 +
// 0x20000.
typedef struct {
    /* 0x00 */ char pad[MAGIC_PAGE_SIZE];
} BrizadPrimPage; // size:0x10000

static BrizadPrimPage brizad_prim_buffer[2];
static void* brizad_buffer_ptr;

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

// The fade is the GTE depth-cue factor, driving vertex colour toward
// SetFarColor -- black here. Primitives are semi-transparent and blended
// additively, so the model fades to invisible rather than to a dark shape.
#define BRIZAD_LIFETIME 15
#define GROWTH_PER_FRAME (3 * 4096 / (BRIZAD_LIFETIME - 1)) // 0x36D
#define FADE_PER_FRAME (4096 / (BRIZAD_LIFETIME - 1))       // 0x124

// ScaleMatrix writes into MATRIX.m, which is s16, so the scale clamps here.
#define SCALE_MAX 0x7FFF

extern ModelRenderDesc g_BrizadRenderDesc;
extern BrizadData g_BattleEffectSlots[];
extern s16 D_80151774;

static void BrizadRenderIce(void) {
    MATRIX matrix;
    VECTOR scaleVec;
    BrizadData* effect;
    s16 frame;
    s16 fade;
    s32 growth;
    s32 scale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    growth = (effect->Scale * GROWTH_PER_FRAME);
    scale = (effect->AnimationFrame * growth) >> 12;
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
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, &matrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    g_BrizadRenderDesc.color = fade;
    brizad_buffer_ptr = func_800D29D4(&g_BrizadRenderDesc, g_cDb->unk70, 12, brizad_buffer_ptr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= BRIZAD_LIFETIME) {
            effect->StartFrame = -1;
        }
    }
}

static void BrizadSpawnIce(void) {
    BrizadData* next;
    BrizadData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            next = &g_BattleEffectSlots[BattleEffectRegister(BrizadRenderIce)];
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
static void BrizadAttachToTargetUnused(s32 target, s32 arg1) {
    g_BattleEffectSlots[BattleEffectRegister(BrizadSpawnIce)].TargetIndex = target;
}

static void BrizadAttachToTarget(s32 target, s32 arg1) {
    g_BattleEffectSlots[BattleEffectRegister(BrizadSpawnIce)].TargetIndex = target;
}

// This slot uses AnimationFrame as a 0/1 page index, not as a frame counter.
static void BrizadDoubleBufferFlip(void) {
    BrizadData* flip;

    flip = &g_BattleEffectSlots[g_BattleEffectCursor];
    brizad_buffer_ptr = &brizad_prim_buffer[flip->AnimationFrame];
    flip->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        flip->StartFrame = -1;
    }
}

// Overlay entry, at 0x801B037C. This is the last function in the overlay, so
// it must stay last in this file.
void MAGIC_Brizad(s32 targetMask, s32 arg1) {
    BattleEffectRegister(BrizadDoubleBufferFlip);
    MagicAnimationRegister(targetMask, arg1, 4, BrizadAttachToTarget);
    BattleCommandSend(0x20, BattleEntityGetStereoPan(D_80151774), 0x18);
}
