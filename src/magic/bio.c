//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "magic_private.h"
#include "bio.h"
#include "../battle/battle.h"

// Bio (バイオ / Bio), tier 1.

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ s16 TargetIndex;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ SVECTOR Pos;
    /* 0x10 */ s16 Scale;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ char pad14[0xC];
} BioData; // size:0x20

extern BioData g_BattleEffectSlots[];
extern u8 bio_prim_buffer[2][MAGIC_PAGE_SIZE];
extern void* bio_buffer_ptr;
extern u_long g_BioTexture[];
extern SpriteAnim g_BioRenderData0;
extern SpriteAnim g_BioRenderData1;

static SpriteRenderDesc bio_render_desc0 = {&g_BioRenderData0, {0x80, 0x80, 0x80, 0x2C}, 0, 0};
static SpriteRenderDesc bio_render_desc1 = {&g_BioRenderData1, {0x80, 0x80, 0x80, 0x2C}, 0, 0};

void BioCloudParticleTick(void);
void BioBubbleSpriteTick(void);
void BioTargetTick(void);
void BioTargetSetup(s32 target, s32 callbackArg);

void BioCloudParticleTick(void) {
    BioData* effect;
    s32 shade;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (effect->AnimationFrame < 10) {
        // written as one word; four byte stores do not match
        *(u32*)&bio_render_desc0.color = 0x2C808080;
    } else if (effect->AnimationFrame - 10 < 10) {
        shade = -((effect->AnimationFrame - 10) << 7) / 10 - 0x80;
        bio_render_desc0.color.cd = 0x2E;
        bio_render_desc0.color.r = bio_render_desc0.color.g = bio_render_desc0.color.b = shade;
    } else {
        effect->StartFrame = -1;
        return;
    }
    bio_render_desc0.frameIndex = effect->AnimationFrame % 7;
    BattleSetBillboardMatrix(&effect->Pos, effect->Scale, 0);
    bio_buffer_ptr = func_800D4D90(&bio_render_desc0, g_cDb->unk70, 12, bio_buffer_ptr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        effect->Pos.vy -= effect->unk12;
    }
}

void BioBubbleSpriteTick(void) {
    BioData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    bio_render_desc1.frameIndex = effect->AnimationFrame;
    BattleSetBillboardMatrix(&effect->Pos, effect->Scale, 0);
    bio_buffer_ptr = func_800D4D90(&bio_render_desc1, g_cDb->unk70, 12, bio_buffer_ptr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 6) {
            effect->StartFrame = -1;
        }
    }
}

void BioTargetTick(void) {
    BioData* effect;
    BioData* child;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if ((u16)effect->AnimationFrame < 15) {
            if ((effect->AnimationFrame & 3) == 0) {
                child = &g_BattleEffectSlots[BattleEffectRegister(BioCloudParticleTick)];
                child->Pos.vx = effect->Pos.vx + (rand() % 1000 - 500);
                child->Pos.vy = 0;
                child->Pos.vz = effect->Pos.vz + (rand() % 1000 - 500);
                child->Scale = rand() % 1280 + 0x1000;
                child->unk12 = rand() % 90 + 60;
            }
        }
        if ((u16)effect->AnimationFrame < 15) {
            if ((effect->AnimationFrame & 1) == 0) {
                child = &g_BattleEffectSlots[BattleEffectRegister(BioBubbleSpriteTick)];
                child->Pos.vx = effect->Pos.vx + rand() % 800 - 400;
                child->Pos.vy = effect->Pos.vy + rand() % 800 - 400;
                child->Pos.vz = effect->Pos.vz + rand() % 800 - 400;
                child->Scale = rand() % 2304 + 0x700;
            }
        }
        if (effect->AnimationFrame == 15) {
            func_800D5774(effect->TargetIndex);
            if (effect->AnimationFrame == 15) {
                effect->StartFrame = -1;
            }
        }
        effect->AnimationFrame++;
    }
}

void BioTargetSetup(s32 target, s32 callbackArg) {
    BioData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(BioTargetTick)];
    effect->TargetIndex = target;
    effect->unk6 = callbackArg;
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->Pos);
}

// Same as BarrierDoubleBufferFlip.
static void BioDoubleBufferFlip(void) {
    BioData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    bio_buffer_ptr = bio_prim_buffer[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;

    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void MAGIC_Bio(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(g_BioTexture, 0, 0, 0);
    BattleEffectRegister(BioDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 0, BioTargetSetup);
    BattleAkaoCommand(AKAO_PLAY_SOUND, BattleEntityGetStereoPan(targetMask), 0x3F);
}
