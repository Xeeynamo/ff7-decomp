//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "magic_private.h"
#include "../battle/battle.h"

// Fire2 (ファイラ / Fira), tier 2.

#define FADE_IN_FRAMES 8
#define FADE_OUT_START 16
#define LIFETIME 24

#define SCALE_MAX 0x7FFF

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ s16 Scale;
    /* 0x0E */ char padE[0x12];
} FairaData; // size:0x20

extern u_long g_FairaTexture[];
extern u8 faira_prim_buffer[2][MAGIC_PAGE_SIZE];
extern void* faira_buffer_ptr;
extern FairaData g_BattleEffectSlots[];
extern s32 g_FairaModel[];
extern MATRIX faira_matrix;

static SVECTOR faira_rotation = {0x400, 0x400, 0x400, 0};
static u16 faira_uv_offsets[4] = {0, 0x7F, 0x6800, 0x687F};

static void FairaMainSetup(s32 targetMask, s32 callbackArg);

void MAGIC_Faira(s32 targetMask, s32 callbackArg) { FairaMainSetup(targetMask, callbackArg); }

static void FairaRenderModel(void) {
    MATRIX composedMatrix;
    char pad[0x10];
    VECTOR scale;
    FairaData* effect;
    ModelRenderDesc* desc;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    desc = (ModelRenderDesc*)0x1F800000;
    RotMatrixYXZ(&faira_rotation, &faira_matrix);
    scale.vx = scale.vy = scale.vz = effect->Scale;
    ScaleMatrix(&faira_matrix, &scale);
    faira_matrix.t[0] = effect->Pos.vx;
    faira_matrix.t[1] = effect->Pos.vy;
    faira_matrix.t[2] = effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, &faira_matrix, &composedMatrix);
    SetRotMatrix(&composedMatrix);
    SetTransMatrix(&composedMatrix);

    desc->model = g_FairaModel;
    desc->flags = MODEL_SEMI_TRANS;
    desc->uvOffset = faira_uv_offsets[effect->AnimationFrame & 3];
    if (effect->AnimationFrame < FADE_IN_FRAMES) {
        desc->color = (effect->AnimationFrame + 1) * 14;
    } else if (effect->AnimationFrame < FADE_OUT_START) {
        desc->color = 0x80;
    } else if (effect->AnimationFrame < LIFETIME) {
        desc->color = (0x80 - ((effect->AnimationFrame - FADE_OUT_START + 1) * 14));
    } else {
        effect->StartFrame = -1;
        return;
    }

    desc->tpage = ((effect->AnimationFrame >> 3) & 1) | 0x20;
    desc->clut = 0;
    faira_buffer_ptr = func_800D29D4(desc, g_cDb->unk70, 0xC, faira_buffer_ptr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

static void FairaAttachToTarget(s32 target, s32 callbackArg) {
    FairaData* effect;
    s32 scale;

    effect = &g_BattleEffectSlots[BattleEffectRegister(FairaRenderModel)];
    BattleGetPartPosition(target, g_BattleModels[target].battleModelRootBone, &effect->Pos);
    effect->Pos.vy = 0;
    scale = func_800D55A4(target);
    if (scale > SCALE_MAX) {
        scale = SCALE_MAX;
    }
    effect->Scale = scale;
    func_800D5774(target);
}

// This slot uses AnimationFrame as a 0/1 page index, not as a frame counter.
static void FairaDoubleBufferFlip(void) {
    FairaData* flip;

    flip = &g_BattleEffectSlots[g_BattleEffectCursor];
    faira_buffer_ptr = &faira_prim_buffer[flip->AnimationFrame];
    flip->AnimationFrame = flip->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        flip->StartFrame = -1;
    }
}

static void FairaMainSetup(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(g_FairaTexture, 0, 0, 0);
    BattleEffectRegister(FairaDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 0, FairaAttachToTarget);
    BattleCommandSend(0x20, BattleEntityGetStereoPan(targetMask), 0xA);
}
