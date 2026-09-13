//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ s16 TargetIndex;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ SVECTOR Pos;
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ char pad14[0xC];
} FireData; // size:0x20

extern FireData g_BattleEffectSlots[];
static u8 fire_prim_buffer[2][0x4000];
static void* fire_buffer_ptr;
extern u_long g_FireTexture[]; // 4bpp TIM + four 16-colour CLUTs, uploaded on setup
extern SpriteRenderDesc g_FireRenderDesc;

static void FireRenderSprite(void) {
    FireData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    func_800D4368(&effect->Pos, 0x1000, effect->unk12);
    g_FireRenderDesc.frameIndex = effect->AnimationFrame;
    fire_buffer_ptr = func_800D4D90(&g_FireRenderDesc, g_cDb->unk70, 0xC, fire_buffer_ptr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 14) {
            effect->StartFrame = -1;
        }
    }
}

static void FireAnimationUpdate(void) {
    char pad[0x10]; // never read, but the frame reserves it
    FireData* next;
    FireData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            func_800D5774(effect->TargetIndex);
        }
        if (!(effect->AnimationFrame & 1)) {
            next = &g_BattleEffectSlots[BattleEffectRegister(FireRenderSprite)];
            effect->unk6 = (effect->unk6 + (rand() & 0xF) + 1) % D_801518E4[effect->TargetIndex].unk10;
            BattleGetPartPosition(effect->TargetIndex, effect->unk6, &next->Pos);
            next->unk12 = effect->unk12;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 5) {
            effect->StartFrame = -1;
        }
    }
}

static void FireAttachToTarget(s32 target, s32 callbackArg) {
    FireData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(FireAnimationUpdate)];
    effect->TargetIndex = target;
    effect->unk6 = 0;
    effect->unk12 = -D_801518E4[target].unk12;
}

static void FireDoubleBufferFlip(void) {
    FireData* effect;
    u8* buf;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    buf = fire_prim_buffer[1];
    if (g_dbIndex != 0) {
        buf = fire_prim_buffer[0];
    }
    fire_buffer_ptr = buf;

    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void MAGIC_Fire(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(g_FireTexture, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 0, FireAttachToTarget);
    BattleEffectRegister(FireDoubleBufferFlip);
    BattleCommandSend(0x20, BattleEntityGetStereoPan(D_80151774), 9);
}

void func_801B037C(void) { func_8001C3C4(); }
