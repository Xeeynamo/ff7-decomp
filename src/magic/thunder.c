//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "magic_private.h"
#include "../battle/battle.h"
#include <libc.h>

// Bolt (サンダー / Thunder), tier 1.

#define MODEL_LIFETIME 16
#define DIM_START_FRAME 8
#define GREY_FULL 0x80
#define GREY_PER_FRAME 0x10

#define BOLT_LIFETIME 16
#define SPARK_START_FRAME 2

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR unk0C;
    /* 0x14 */ s16 TargetIndex;
    /* 0x16 */ s16 Scale;     // 0x1000 == 1.0
    /* 0x18 */ u16 ScaleStep; // seeded 0x200
    /* 0x1A */ u16 Flags;
    /* 0x1C */ s16 DepthBias;
    /* 0x1E */ char pad1E[2];
} ThunderData; // size:0x20

extern void* g_ThunderBufferPtr;
extern ThunderData g_BattleEffectSlots[];
extern u8 g_ThunderPrimBuffer[2][MAGIC_PAGE_SIZE];
extern u_long g_ThunderTexture[]; // 8bpp TIM + CLUT, uploaded on setup
extern s32 g_ThunderModel[];
extern s32 g_ThunderRenderData0[];
extern s32 g_ThunderRenderData1[];

static MATRIX thunder_model_matrix = {0};
static ModelRenderDesc thunder_model_desc = {g_ThunderModel, MODEL_SEMI_TRANS, 0, GREY_FULL, 0x20};
static SpriteRenderDesc thunder_render_desc0 = {g_ThunderRenderData0, {0x80, 0x80, 0x80, 0x2C}, 0, 0};
static SpriteRenderDesc thunder_render_desc1 = {g_ThunderRenderData1, {0x80, 0x80, 0x80, 0x2C}, 0, 0};

static void ThunderMainSetup(s32 targetMask, s32 callbackArg);

void MAGIC_Thunder(s32 targetMask, s32 callbackArg) { ThunderMainSetup(targetMask, callbackArg); }

static void ThunderRenderModel(void) {
    MATRIX matrix;
    ThunderData* effect;
    s16 frame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    frame = effect->AnimationFrame;
    if (frame < DIM_START_FRAME) {
        thunder_model_desc.color = GREY_FULL;
    } else if (frame < MODEL_LIFETIME) {
        thunder_model_desc.color = GREY_FULL - ((frame - DIM_START_FRAME) * GREY_PER_FRAME);
    } else {
        effect->StartFrame = -1;
        return;
    }

    thunder_model_matrix.m[0][0] = thunder_model_matrix.m[2][1] = effect->Scale;
    thunder_model_matrix.m[1][2] = -effect->Scale;
    thunder_model_matrix.t[0] = effect->Pos.vx;
    thunder_model_matrix.t[1] = effect->Pos.vy;
    thunder_model_matrix.t[2] = effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, &thunder_model_matrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    g_ThunderBufferPtr = func_800D29D4(&thunder_model_desc, g_cDb->unk70, 0xC, g_ThunderBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        effect->Scale += effect->ScaleStep;
    }
}

static void ThunderRenderImpact(void) {
    ThunderData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    func_800D4368(&effect->Pos, 0x2000, effect->DepthBias);
    thunder_render_desc0.frameIndex = effect->AnimationFrame >> 1;
    g_ThunderBufferPtr = func_800D4D90(&thunder_render_desc0, g_cDb->unk70, 0xC, g_ThunderBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 9) {
            effect->StartFrame = -1;
        }
    }
}

static void ThunderRenderSpark(void) {
    MATRIX* matrix;
    ThunderData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    matrix = func_800D4368(&effect->Pos, 0x2000, effect->DepthBias);
    if (effect->Flags & 1) {
        matrix->m[0][0] = -matrix->m[0][0];
    }
    if (effect->Flags & 2) {
        matrix->m[1][1] = -matrix->m[1][1];
    }
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    thunder_render_desc1.frameIndex = effect->AnimationFrame;
    g_ThunderBufferPtr = func_800D4D90(&thunder_render_desc1, g_cDb->unk70, 0xC, g_ThunderBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 8) {
            effect->StartFrame = -1;
        }
    }
}

static void ThunderSpawnBolt(void) {
    ThunderData* next;
    ThunderData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            next = &g_BattleEffectSlots[BattleEffectRegister(ThunderRenderImpact)];
            next->Pos = effect->Pos;
            next->Pos.vy = 0;
            next->DepthBias = effect->DepthBias;
            func_800D5774(effect->TargetIndex);
            if (effect->AnimationFrame == 0) {
                next = &g_BattleEffectSlots[BattleEffectRegister(ThunderRenderModel)];
                next->Pos = effect->Pos;
                next->Scale = 4096;
                next->Pos.vy = 0;
                next->ScaleStep = 0x200;
                next->DepthBias = effect->DepthBias;
            }
        }
        if (effect->AnimationFrame >= SPARK_START_FRAME) {
            next = &g_BattleEffectSlots[BattleEffectRegister(ThunderRenderSpark)];
            next->Pos.vx = (effect->Pos.vx + rand() % 1000) - 500;
            next->Pos.vy = (effect->Pos.vy + rand() % 1000) - 500;
            next->Pos.vz = (effect->Pos.vz + rand() % 1000) - 500;
            next->Flags = rand() & 3;
            next->DepthBias = effect->DepthBias;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == BOLT_LIFETIME) {
            effect->StartFrame = -1;
        }
    }
}

static void ThunderAttachToTarget(s32 target, s32 callbackArg) {
    ThunderData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(ThunderSpawnBolt)];
    BattleGetPartPosition(target, g_BattleModels[target].battleModelRootBone, &effect->Pos);
    effect->TargetIndex = target;
    effect->DepthBias = -g_BattleModels[target].collisionRadius;
    BattleAkaoCommand(AKAO_PLAY_SOUND, BattlePositionToStereoPan(&effect->Pos), SFX_THUNDER);
}

static void ThunderDoubleBufferFlip(void) {
    ThunderData* data;

    data = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_ThunderBufferPtr = g_ThunderPrimBuffer[data->AnimationFrame];
    data->AnimationFrame = data->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        data->StartFrame = -1;
    }
}

static void ThunderMainSetup(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(g_ThunderTexture, 0, 0, 0);
    BattleEffectRegister(ThunderDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 2, ThunderAttachToTarget);
}
