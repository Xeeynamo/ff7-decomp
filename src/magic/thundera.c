//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "magic_private.h"
#include "../battle/battle.h"
#include <libc.h>

// Bolt2 (サンダラ / Thundara), tier 2.

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ s16 TargetIndex;
    /* 0x0E */ u16 Scale;
    /* 0x10 */ s16 ScaleStep;
    /* 0x12 */ u16 Flags; // two mirror bits and a sprite-set bit, seeded at random
    /* 0x14 */ s16 DepthBias;
    /* 0x16 */ char pad16[0xA];
} ThunderaData; // size:0x20

extern void* g_ThunderaBufferPtr;
extern ThunderaData g_BattleEffectSlots[];
extern u8 g_ThunderaPrimBuffer[2][MAGIC_PAGE_SIZE];
extern u_long g_ThunderaTexture[];
extern s32 g_ThunderaModel[];
extern s32 g_ThunderaRenderData0[];
extern s32 g_ThunderaRenderData1[];
extern s32 g_ThunderaRenderData2[];
extern s32 g_ThunderaRenderData3[];

static SpriteRenderDesc thundera_render_desc = {g_ThunderaRenderData3, {0x80, 0x80, 0x80, 0x2C}, 0, 0};
static MATRIX thundera_matrix = {{{0x2000, 0, 0}, {0, 0x2000, 0}, {0, 0, 0x2000}}, {0, 0, 0}};

static void ThunderaMainSetup(s32 targetMask, s32 callbackArg);

void MAGIC_Thundera(s32 targetMask, s32 callbackArg) { ThunderaMainSetup(targetMask, callbackArg); }

static void ThunderaRenderModel(void) {
    MATRIX matrix;
    ModelRenderDesc* desc;
    ThunderaData* effect;
    u16 frame;
    u16 boltFrame;
    s32 grey;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    frame = effect->AnimationFrame;
    grey = 0x80 - (frame * 0x10);
    matrix.m[0][0] = matrix.m[2][1] = effect->Scale;
    matrix.m[1][2] = -effect->Scale;
    matrix.m[0][1] = matrix.m[0][2] = matrix.m[1][0] = matrix.m[1][1] = matrix.m[2][0] = matrix.m[2][2] = 0;
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, &matrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    desc = (ModelRenderDesc*)0x1F800000;
    desc->model = g_ThunderaModel;
    desc->flags = MODEL_SEMI_TRANS;
    desc->uvOffset = 0;
    desc->color = grey;
    desc->tpage = 0x20;
    desc->clut = 0;
    g_ThunderaBufferPtr = func_800D29D4(desc, g_cDb->unk70, 0xC, g_ThunderaBufferPtr);
    if (D_80062D98 == 0) {
        effect->Scale += effect->ScaleStep;
        effect->ScaleStep -= effect->ScaleStep >> 2;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 8) {
            effect->StartFrame = -1;
        }
    }
}

static void func_801B01A0(void) {
    SpriteRenderDesc* desc;
    ThunderaData* effect;
    u8 frame;
    s32 shade;

    desc = (SpriteRenderDesc*)0x1F800000;
    desc->frames = g_ThunderaRenderData0;
    desc->frameIndex = 0;
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    frame = effect->AnimationFrame;
    shade = ~(frame << 6);
    desc->color.cd = 0x2C;
    desc->color.r = desc->color.g = desc->color.b = shade;
    func_800D4368(&effect->Pos, 0x2000, effect->DepthBias);
    g_ThunderaBufferPtr = func_800D4D90(desc, g_cDb->unk70, 0xC, g_ThunderaBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 4) {
            effect->StartFrame = -1;
        }
    }
}

static void func_801B028C(void) {
    ThunderaData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    func_800D4368(&effect->Pos, 0x3000, effect->DepthBias);
    thundera_render_desc.frameIndex = effect->AnimationFrame >> 1;
    g_ThunderaBufferPtr = func_800D4D90(&thundera_render_desc, g_cDb->unk70, 0xC, g_ThunderaBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 16) {
            effect->StartFrame = -1;
        }
    }
}

static void func_801B0348(void) {
    int flag;
    SpriteRenderDesc* desc;
    ThunderaData* effect;

    desc = (SpriteRenderDesc*)0x1F800000;
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    SetRotMatrix(&D_800FA63C.m);
    SetTransMatrix(&D_800FA63C.m);
    RotTrans(&effect->Pos, (VECTOR*)thundera_matrix.t, &flag);
    thundera_matrix.t[2] += effect->DepthBias;
    if (effect->Flags & 1) {
        thundera_matrix.m[0][0] = -0x2000;
    } else {
        thundera_matrix.m[0][0] = 0x2000;
    }
    if (effect->Flags & 2) {
        thundera_matrix.m[1][1] = -0x2000;
    } else {
        thundera_matrix.m[1][1] = 0x2000;
    }
    SetRotMatrix(&thundera_matrix);
    SetTransMatrix(&thundera_matrix);
    if (effect->Flags & 4) {
        desc->frames = g_ThunderaRenderData1;
    } else {
        desc->frames = g_ThunderaRenderData2;
    }
    // written as one word; four byte stores do not match
    *(u32*)&desc->color = 0x2C808080;
    desc->frameIndex = effect->AnimationFrame;
    g_ThunderaBufferPtr = func_800D4D90(desc, g_cDb->unk70, 0xC, g_ThunderaBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 8) {
            effect->StartFrame = -1;
        }
    }
}

static void ThunderaSpawnBolt(void) {
    ThunderaData* next;
    ThunderaData* effect;
    u16 frame;
    u16 boltFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 1) {
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B01A0)];
            next->Pos = effect->Pos;
            next->Pos.vy = 0;
            next->DepthBias = effect->DepthBias;
            func_800D5774(effect->TargetIndex);
        }
        frame = effect->AnimationFrame;
        if (frame < 21 && !(frame & 1)) {
            next = &g_BattleEffectSlots[BattleEffectRegister(ThunderaRenderModel)];
            next->Pos.vx = (effect->Pos.vx + rand() % 2000) - 1000;
            next->Pos.vy = 0;
            next->Pos.vz = (effect->Pos.vz + rand() % 2000) - 1000;
            next->Scale = 0x1000;
            next->ScaleStep = 0x800;
        }
        if (effect->AnimationFrame == 8) {
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B028C)];
            next->Pos = effect->Pos;
            next->DepthBias = effect->DepthBias;
        }
        boltFrame = effect->AnimationFrame;
        if (boltFrame < 16) {
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B0348)];
            next->Pos.vx = (effect->Pos.vx + rand() % 1000) - 500;
            next->Pos.vy = (effect->Pos.vy + rand() % 1000) - 500;
            next->Pos.vz = (effect->Pos.vz + rand() % 1000) - 500;
            next->Flags = rand() & 7;
            next->DepthBias = effect->DepthBias;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 21) {
            effect->StartFrame = -1;
        }
    }
}

static void ThunderaAttachToTarget(s32 target, s32 callbackArg) {
    ThunderaData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(ThunderaSpawnBolt)];
    BattleGetPartPosition(target, g_BattleModels[target].battleModelRootBone, &effect->Pos);
    effect->DepthBias = -g_BattleModels[target].collisionRadius;
    effect->TargetIndex = target;
}

static void ThunderaDoubleBufferFlip(void) {
    ThunderaData* data;

    data = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_ThunderaBufferPtr = g_ThunderaPrimBuffer[data->AnimationFrame];
    data->AnimationFrame = data->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        data->StartFrame = -1;
    }
}

static void ThunderaMainSetup(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(g_ThunderaTexture, 0, 0, 0);
    BattleEffectRegister(ThunderaDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 2, ThunderaAttachToTarget);
    BattleAkaoCommand(AKAO_PLAY_SOUND, BattleEntityGetStereoPan(targetMask),SFX_THUNDARA);
}
