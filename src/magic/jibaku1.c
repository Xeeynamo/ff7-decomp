//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "jibaku1.h"
#include "../battle/battle.h"

// Effect shared by Suicide (自爆), Self-destruct (自爆), Suicide Yang (ヤン自爆) and Destroyed (副砲破壊,
// 主砲破壊).

// Only StartFrame/AnimationFrame are common to every magic overlay; the
// rest is payload this effect lays out for itself.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ char pad0C[8];
    /* 0x14 */ s16 unk14;
    /* 0x16 */ char pad16[2];
    /* 0x18 */ s16 unk18;
    /* 0x1A */ char pad1A[6];
} Jibaku1Data; // size:0x20

extern SpriteRenderDesc D_801B857C;
extern Jibaku1Data g_BattleEffectSlots[];
extern s32 D_801B83F8;
extern u8 D_801B8588[2][0xC000];
extern u_long D_801B03B4[];
extern void* D_801D0588;

// Same as ChikasuiDoubleBufferFlip.
static void Jibaku1DoubleBufferFlip(void) {
    Jibaku1Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D0588 = D_801B8588[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void Jibaku1ExplosionSpriteTick(void) {
    Jibaku1Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2000, 0);
    D_801B857C.frameIndex = effect->AnimationFrame;
    D_801D0588 = func_800D4D90(&D_801B857C, (u_long**)&g_cDb->unk70, 12, D_801D0588);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801B83F8) {
            effect->StartFrame = -1;
        }
    }
}

void Jibaku1TargetTick(void) {
    Jibaku1Data* effect;
    Jibaku1Data* child;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if ((u16)effect->AnimationFrame < 10) {
            child = &g_BattleEffectSlots[BattleEffectRegister(Jibaku1ExplosionSpriteTick)];
            child->Pos.vx = effect->Pos.vx + rand() % 1000 - 500;
            child->Pos.vy = effect->Pos.vy + rand() % 1000 - 500;
            child->Pos.vz = effect->Pos.vz + rand() % 1000 - 500;
        }
        if (effect->AnimationFrame == 0) {
            func_800D5774(effect->unk14);
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void Jibaku1TargetSetup(s32 target, s32 callbackArg) {
    Jibaku1Data* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(Jibaku1TargetTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[0], &effect->Pos);
    effect->unk14 = target;
}

void func_801B0338(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(Jibaku1DoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B03B4, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 4, Jibaku1TargetSetup);
    func_800BBA84(0x7C, callbackArg, 0);
}
