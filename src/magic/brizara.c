//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "magic_private.h"
#include "../battle/battle.h"

// Ice2 (ブリザラ / Blizzara), tier 2.

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ s16 TargetIndex;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ SVECTOR Rot;
    /* 0x10 */ SVECTOR Pos;
    /* 0x18 */ char pad18[8];
} BrizaraData; // size:0x20

extern u_long g_BrizaraTexture[]; // 4bpp TIM + four 16-colour CLUTs, uploaded on setup
extern BrizaraData g_BattleEffectSlots[];
extern s32 g_BrizaraModel[];

static u8 brizara_prim_buffer[2][MAGIC_PAGE_SIZE];
static void* brizara_buffer_ptr;
static ModelRenderDesc brizara_render_desc0 = {g_BrizaraModel, MODEL_SEMI_TRANS, 0, 0x80, 0x20};
static ModelRenderDesc brizara_render_desc1 = {g_BrizaraModel, MODEL_SEMI_TRANS, 0, 0x80, 0x20};

static void BrizaraRenderModel0(void) {
    MATRIX* matrix;
    VECTOR* scale;
    BrizaraData* effect;
    s32 frame;

    matrix = (MATRIX*)0x1F800010;
    scale = (VECTOR*)0x1F800000;
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    scale->vx = scale->vy = scale->vz = (-(effect->AnimationFrame * 0xC00) / 15) + 0xC00;
    frame = effect->AnimationFrame;
    if (frame < 5) {
        brizara_render_desc0.color = (frame << 7) / 5;
    } else {
        frame -= 5;
        if (frame < 10) {
            brizara_render_desc0.color = 0x80;
        } else {
            effect->StartFrame = -1;
            return;
        }
    }
    RotMatrixYXZ(&effect->Rot, matrix);
    ScaleMatrix(matrix, scale);
    matrix->t[0] = effect->Pos.vx;
    matrix->t[1] = effect->Pos.vy;
    matrix->t[2] = effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, matrix, matrix);
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    brizara_buffer_ptr = func_800D29D4(&brizara_render_desc0, g_cDb->unk70, 0xC, brizara_buffer_ptr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        effect->Rot.vx += 0x100;
        effect->Rot.vy += 0x80;
        effect->Rot.vz += 0x40;
    }
}

static void BrizaraRenderModel1(void) {
    MATRIX* matrix;
    VECTOR* scale;
    BrizaraData* effect;
    s16 start;
    s32 frame;
    s32 fadeFrame;
    s32 temp;

    matrix = (MATRIX*)0x1F800010;
    scale = (VECTOR*)0x1F800000;
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    frame = effect->AnimationFrame;
    if (frame < 5) {
        brizara_render_desc1.color = 0x80;
        scale->vx = scale->vy = scale->vz = (frame << 11) / 5;
    } else {
        start = effect->StartFrame;
        frame -= 5;
        if (frame < (start + 5)) {
            brizara_render_desc1.color = 0x80;
            scale->vx = scale->vy = scale->vz = 0x800;
        } else {
            temp = frame - 5;
            fadeFrame = temp - start;
            if (fadeFrame < 5) {
                scale->vx = scale->vy = scale->vz = 0x800;
                brizara_render_desc1.color = (-(fadeFrame << 7) / 5) + 0x80;
            } else {
                effect->StartFrame = -1;
                return;
            }
        }
    }
    RotMatrixYXZ(&effect->Rot, matrix);
    ScaleMatrix(matrix, scale);
    matrix->t[0] = effect->Pos.vx;
    matrix->t[1] = effect->Pos.vy;
    matrix->t[2] = effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, matrix, matrix);
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    brizara_buffer_ptr = func_800D29D4(&brizara_render_desc1, g_cDb->unk70, 0xC, brizara_buffer_ptr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

static void BrizaraAnimationUpdate(void) {
    BrizaraData* next;
    BrizaraData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            next = &g_BattleEffectSlots[BattleEffectRegister(BrizaraRenderModel0)];
            next->Pos = effect->Pos;
            next->Rot.vx = next->Rot.vy = next->Rot.vz = 0;
            next = &g_BattleEffectSlots[BattleEffectRegister(BrizaraRenderModel0)];
            next->Pos = effect->Pos;
            next->Rot.vx = next->Rot.vy = 0x200;
            next->Rot.vz = -0x200;
            next = &g_BattleEffectSlots[BattleEffectRegister(BrizaraRenderModel0)];
            next->Pos = effect->Pos;
            next->Rot.vy = -0x200;
            next->Rot.vx = -0x200;
            next->Rot.vz = 0x200;
        }
        if (effect->AnimationFrame == 15) {
            next = &g_BattleEffectSlots[BattleEffectRegister(BrizaraRenderModel1)];
            next->StartFrame = 4;
            next->Pos = effect->Pos;
            next->Rot.vx = next->Rot.vy = next->Rot.vz = 0;
        }
        if (effect->AnimationFrame == 17) {
            next = &g_BattleEffectSlots[BattleEffectRegister(BrizaraRenderModel1)];
            next->StartFrame = 2;
            next->Pos = effect->Pos;
            next->Rot.vx = next->Rot.vy = 0x200;
            next->Rot.vz = -0x200;
        }
        if (effect->AnimationFrame == 19) {
            next = &g_BattleEffectSlots[BattleEffectRegister(BrizaraRenderModel1)];
            next->StartFrame = 0;
            next->Pos = effect->Pos;
            next->Rot.vx = next->Rot.vy = -0x200;
            next->Rot.vz = 0x200;
        }
        if (effect->AnimationFrame == 25) {
            func_800D5774(effect->TargetIndex);
            if (effect->AnimationFrame == 25) {
                effect->StartFrame = -1;
            }
        }
        effect->AnimationFrame++;
    }
}

static void BrizaraAttachToTarget(s32 target, s32 callbackArg) {
    BrizaraData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(BrizaraAnimationUpdate)];
    effect->TargetIndex = target;
    BattleGetPartPosition(target, g_BattleModels[target].battleModelRootBone, &effect->Pos);
}

static void BrizaraDoubleBufferFlip(void) {
    BrizaraData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    brizara_buffer_ptr = brizara_prim_buffer[effect->AnimationFrame];
    effect->AnimationFrame = effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void MAGIC_Brizara(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(g_BrizaraTexture, 0, 0, 0);
    BattleEffectRegister(BrizaraDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 0, BrizaraAttachToTarget);
    BattleAkaoCommand(0x20, BattleEntityGetStereoPan(g_BattleCurrentTargetMask), 0x1D);
}
