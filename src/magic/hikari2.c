//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "hikari2.h"
#include "../battle/battle.h"

// Effect shared by Blue Light (青い光), Smog Alert (超有害スモッグ) and Eyesight (眼力).

void Hikari2Start(s32 targetMask, s32 callbackArg);
void Hikari2SparkSpriteTick(void);
void Hikari2HitSpriteTick(void);

void Hikari2FlashFadeOutSetup(s16 frames);
extern s16 D_801D0AE8[];
extern s16 D_800F5B74;
void Hikari2ViewportFlashTick(void);
void Hikari2TargetTick(void);
void Hikari2DoubleBufferFlip(void);
void Hikari2TargetSetup(s32 target, s32 callbackArg);

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 D_801B8AE4[2][0xC000];
extern void* D_801D0AE4;
extern void* D_80163C74;
extern s16 D_801B8AC8;
extern s16 D_801B8ACA;
extern SpriteRenderDesc D_801B8ACC;
extern SpriteRenderDesc D_801B8AD8;
extern s32 D_801B492C;
extern s32 D_801B8A04;
extern u_long D_801B08E8[];
extern u_long D_801B49C0[];
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;

// Only StartFrame/AnimationFrame are common to every magic overlay.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR unkC;
    /* 0x14 */ char pad14[6];
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ char pad1C[4];
} Hikari2Data; // size:0x20

extern Hikari2Data g_BattleEffectSlots[];

void MAGIC_Hikari2(s32 targetMask, s32 callbackArg) { Hikari2Start(targetMask, callbackArg); }

// Same as BarrierDoubleBufferFlip.
void Hikari2DoubleBufferFlip(void) {
    Hikari2Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D0AE4 = D_801B8AE4[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void Hikari2ViewportFlashTick(void) {
    Hikari2Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_80163C74 = func_800C4FC8(0, 0, D_801B8AC8);
    if (!D_80062D98) {
        D_801B8AC8 += D_801B8ACA;
        if (!D_801B8AC8) {
            effect->StartFrame = -1;
        }
    }
}

void Hikari2FlashFadeInSetup(s16 frames) {
    D_801B8AC8 = 0;
    D_801B8ACA = 0xFF / frames;
    BattleEffectRegister(Hikari2ViewportFlashTick);
    D_800F5B70 = D_800F5B71 = D_800F5B72 = 0;
    BattleFixedPointRampUpdateInit(0x1000, 15);
}

void Hikari2FlashFadeOutSetup(s16 frames) {
    D_801B8AC8 = 0xFF;
    D_801B8ACA = -(0xFF / frames);
    BattleFixedPointRampUpdateInit(0, 15);
}

void Hikari2SparkSpriteTick(void) {
    Hikari2Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x1800, 0);
    D_801B8ACC.frameIndex = effect->AnimationFrame;
    D_801D0AE4 = func_800D4D90(&D_801B8ACC, g_cDb->unk70, 12, D_801D0AE4);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == D_801B492C) {
            effect->StartFrame = -1;
        }
    }
}

void Hikari2HitSpriteTick(void) {
    Hikari2Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x1800, 0);
    D_801B8AD8.frameIndex = effect->AnimationFrame;
    D_801D0AE4 = func_800D4D90(&D_801B8AD8, g_cDb->unk70, 12, D_801D0AE4);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801B8A04) {
            effect->StartFrame = -1;
        }
    }
}

void Hikari2TargetTick(void) {
    Hikari2Data* effect;
    Hikari2Data* child;
    SVECTOR vec;
    MATRIX matrix;
    s32 flag;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (!D_80062D98) {
        if (!effect->AnimationFrame) {
            child = &g_BattleEffectSlots[BattleEffectRegister(Hikari2SparkSpriteTick)];
            child->unkC.vx = 0;
            child->unkC.vy = ratan2(effect->unkC.vx - effect->Pos.vx, effect->unkC.vz - effect->Pos.vz);
            vec.vx = vec.vy = 0;
            vec.vz = 100;
            RotMatrixYXZ(&child->unkC, &matrix);
            matrix.t[0] = matrix.t[1] = matrix.t[2] = 0;
            SetRotMatrix(&matrix);
            SetTransMatrix(&matrix);
            RotTransSV(&vec, &vec, &flag);
            child->Pos.vx = effect->Pos.vx + vec.vx;
            child->Pos.vy = effect->Pos.vy + vec.vy;
            child->Pos.vz = effect->Pos.vz + vec.vz;
        }
        if (effect->AnimationFrame == 2) {
            D_800F5B70 = D_800F5B71 = D_800F5B72 = 0;
            D_800F5B74 = 0x1000;
            for (i = 0; i < 10; i++) {
                D_801D0AE8[i] = g_BattleModels[i].specialFlags;
                g_BattleModels[i].specialFlags |= 2;
            }
        }
        if (effect->AnimationFrame == 4) {
            D_800F5B74 = 0;
            Hikari2FlashFadeInSetup(15);
            for (i = 0; i < 10; i++) {
                g_BattleModels[i].specialFlags = D_801D0AE8[i];
            }
        }
        if (effect->AnimationFrame == 20) {
            Hikari2FlashFadeOutSetup(15);
        }
        if (effect->AnimationFrame == 30) {
            func_800D5774(effect->unk1A);
            func_800BBA84(0x95, effect->unk1A, 0x40);
        }
        if (effect->AnimationFrame >= 31 && effect->AnimationFrame < 35) {
            for (i = 0; i < 2; i++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(Hikari2HitSpriteTick)];
                BattleGetPartPosition(effect->unk1A, g_BattleModels[effect->unk1A].boneIndices[0], &child->Pos);
                child->Pos.vx += rand() % 1000 - 500;
                child->Pos.vy += rand() % 1000 - 500;
                child->Pos.vz += rand() % 1000 - 500;
            }
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 40) {
            effect->StartFrame = -1;
        }
    }
}

void Hikari2TargetSetup(s32 target, s32 callbackArg) {
    Hikari2Data* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(Hikari2TargetTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[9], &effect->Pos);
    effect->unk1A = target;
}

void Hikari2Start(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(Hikari2DoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B08E8, 0, 0, 0);
    BattleSetLoadTimToVram(D_801B49C0, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 4, Hikari2TargetSetup);
    func_800BBA84(0x6B, callbackArg, 0x40);
}
