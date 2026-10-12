//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "magic_private.h"
#include "biora.h"
#include "../battle/battle.h"

// Bio2 (バイオラ / Biora), tier 2.

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ s16 TargetIndex;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ SVECTOR Pos;
    /* 0x10 */ s16 Scale;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ char pad14[0xC];
} BioraData; // size:0x20

extern BioraData g_BattleEffectSlots[];
extern u8 biora_prim_buffer[2][MAGIC_PAGE_SIZE];
extern void* biora_buffer_ptr;
extern u_long g_BioraTexture[];
extern s32 g_BioraModelA0[], g_BioraModelA1[], g_BioraModelA2[], g_BioraModelA3[];
extern s32 g_BioraModelB0[], g_BioraModelB1[], g_BioraModelB2[], g_BioraModelB3[];
extern SpriteAnim g_BioraRenderData0;
extern SpriteAnim g_BioraRenderData1;

static ModelRenderDesc biora_model_desc = {0, MODEL_SEMI_TRANS, 0, 0x80, 0x20, 0};
static s32* biora_models_a[4] = {g_BioraModelA0, g_BioraModelA1, g_BioraModelA2, g_BioraModelA3};
static s32* biora_models_b[4] = {g_BioraModelB0, g_BioraModelB1, g_BioraModelB2, g_BioraModelB3};
static MATRIX biora_matrix = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
static SpriteRenderDesc biora_render_desc0 = {&g_BioraRenderData0, {0x80, 0x80, 0x80, 0x2C}, 0, 0};
static SpriteRenderDesc biora_render_desc1 = {&g_BioraRenderData1, {0x80, 0x80, 0x80, 0x2C}, 0, 0};

void BioraBubbleTick(void);
void BioraBubbleSpriteTick(void);
void BioraCloudParticleTick(void);
void BioraCloudEmitterTick(void);
void BioraTargetTick(void);
void BioraTargetSetup(s32 target, s32 callbackArg);
void BioraStart(s32 targetMask, s32 callbackArg);

void MAGIC_Biora(s32 targetMask, s32 callbackArg) { BioraStart(targetMask, callbackArg); }

void BioraBubbleTick(void) {
    BioraData* effect;
    SVECTOR* dst;
    SVECTOR* src;
    s32* modelA;
    s32* modelB;
    s32 count;
    s32 i;
    s32 frame;
    s32 y;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    biora_matrix.m[0][0] = (rsin((effect->AnimationFrame + effect->StartFrame * 3) * 273) * 80 >> 12) + 0x1000;
    biora_matrix.m[1][1] = (rsin((effect->AnimationFrame + effect->StartFrame * 3) * 409) * 80 >> 12) + 0x1000;
    frame = effect->AnimationFrame;
    if (frame < 5) {
        biora_matrix.m[0][0] = biora_matrix.m[0][0] * frame / 5;
        biora_matrix.m[1][1] = biora_matrix.m[1][1] * effect->AnimationFrame / 5;
    } else {
        frame -= 5;
        if (frame >= 20) {
            frame -= 20;
            if (frame < 5) {
                biora_matrix.m[0][0] = biora_matrix.m[0][0] * (5 - frame) / 5;
                biora_matrix.m[1][1] = biora_matrix.m[1][1] * (5 - frame) / 5;
            } else {
                effect->StartFrame = -1;
                return;
            }
        }
    }
    modelA = biora_models_a[effect->StartFrame];
    modelB = biora_models_b[effect->StartFrame];
    dst = (SVECTOR*)(modelA + 1);
    src = (SVECTOR*)(modelB + 1);
    count = *modelA / 8;
    for (i = 0; i < count; i++) {
        dst[i].vx = src[i].vx + (rsin(i * 500 + effect->AnimationFrame * 75 * 4) * 50 >> 12);
        rcos(0);
        y = src[i].vy;
        dst[i].vy = y;
    }
    BattleSetMatrixPosition(&effect->Pos, 0, &biora_matrix);
    SetRotMatrix(&biora_matrix);
    SetTransMatrix(&biora_matrix);
    biora_model_desc.model = biora_models_a[effect->StartFrame];
    biora_buffer_ptr = func_800D29D4(&biora_model_desc, g_cDb->unk70, 12, biora_buffer_ptr);
    if (!D_80062D98) {
        effect->AnimationFrame++;
    }
}

void BioraBubbleSpriteTick(void) {
    BioraData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    biora_render_desc0.frameIndex = effect->AnimationFrame;
    BattleSetBillboardMatrix(&effect->Pos, effect->Scale, 0);
    biora_buffer_ptr = func_800D4D90(&biora_render_desc0, g_cDb->unk70, 12, biora_buffer_ptr);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 6) {
            effect->StartFrame = -1;
        }
    }
}

void BioraCloudParticleTick(void) {
    BioraData* effect;
    s32 shade;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (effect->AnimationFrame < 10) {
        // written as one word; four byte stores do not match
        *(u32*)&biora_render_desc1.color = 0x2C808080;
    } else if (effect->AnimationFrame - 10 < 10) {
        shade = -((effect->AnimationFrame - 10) << 7) / 10 - 0x80;
        biora_render_desc1.color.cd = 0x2E;
        biora_render_desc1.color.r = biora_render_desc1.color.g = biora_render_desc1.color.b = shade;
    } else {
        effect->StartFrame = -1;
        return;
    }
    biora_render_desc1.frameIndex = effect->AnimationFrame % 7;
    BattleSetBillboardMatrix(&effect->Pos, effect->Scale, 0);
    biora_buffer_ptr = func_800D4D90(&biora_render_desc1, g_cDb->unk70, 12, biora_buffer_ptr);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        effect->Pos.vy -= effect->unk12;
    }
}

void BioraCloudEmitterTick(void) {
    BioraData* effect;
    BioraData* child;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (!D_80062D98) {
        if ((effect->AnimationFrame & 3) == 0) {
            child = &g_BattleEffectSlots[BattleEffectRegister(BioraCloudParticleTick)];
            child->Pos.vx = effect->Pos.vx + rand() % 2000 - 1000;
            child->Pos.vy = 0;
            child->Pos.vz = effect->Pos.vz + rand() % 2000 - 1000;
            child->Scale = rand() % 1280 + 0x1000;
            child->unk12 = rand() % 50 + 50;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 20) {
            effect->StartFrame = -1;
        }
    }
}

void BioraTargetTick(void) {
    char pad[8]; // match: stack pad keeps the original frame size
    BioraData* effect;
    BioraData* child;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (!D_80062D98) {
        if ((u16)effect->AnimationFrame < 8) {
            if ((effect->AnimationFrame & 1) == 0) {
                child = &g_BattleEffectSlots[BattleEffectRegister(BioraBubbleTick)];
                child->StartFrame = effect->AnimationFrame / 2;
                child->Pos.vx = effect->Pos.vx + rand() % 1600 - 800;
                child->Pos.vy = effect->Pos.vy + rand() % 1600 - 800;
                child->Pos.vz = effect->Pos.vz + rand() % 1600 - 800;
            }
        }
        if ((u16)effect->AnimationFrame < 20) {
            if ((effect->AnimationFrame & 1) == 0) {
                child = &g_BattleEffectSlots[BattleEffectRegister(BioraBubbleSpriteTick)];
                child->Pos.vx = effect->Pos.vx + rand() % 1200 - 600;
                child->Pos.vy = effect->Pos.vy + rand() % 1200 - 600;
                child->Pos.vz = effect->Pos.vz + rand() % 1200 - 600;
                child->Scale = rand() % 0x1000 + 0x1000;
            }
        }
        if (!effect->AnimationFrame) {
            g_BattleEffectSlots[BattleEffectRegister(BioraCloudEmitterTick)].Pos = effect->Pos;
        }
        if (effect->AnimationFrame == 20) {
            func_800D5774(effect->TargetIndex);
            if (effect->AnimationFrame == 20) {
                effect->StartFrame = -1;
            }
        }
        effect->AnimationFrame++;
    }
}

void BioraTargetSetup(s32 target, s32 callbackArg) {
    BioraData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(BioraTargetTick)];
    effect->TargetIndex = target;
    effect->unk6 = callbackArg;
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->Pos);
    BattleAkaoCommand(AKAO_PLAY_SOUND, 0x40, 0x5A);
}

// Same as BarrierDoubleBufferFlip.
static void BioraDoubleBufferFlip(void) {
    BioraData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    biora_buffer_ptr = biora_prim_buffer[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;

    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void BioraStart(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(g_BioraTexture, 0, 0, 0);
    BattleEffectRegister(BioraDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 4, BioraTargetSetup);
}
