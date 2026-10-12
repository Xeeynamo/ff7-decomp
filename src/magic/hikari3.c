//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "hikari3.h"
#include "../battle/battle.h"

// Effect shared by Red Light (赤い光), Petrif-Eye (石化にらみ), Heatwing (ヒートウィング)
// and Eyesight (眼力).

void Hikari3Start(s32 targetMask, s32 callbackArg);
extern s16 D_801D0AA8[];
extern s16 D_800F5B74;
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;
void Hikari3ViewportFlashTick(void);
void Hikari3TargetTick(void);
void Hikari3DoubleBufferFlip(void);
void Hikari3TargetSetup(s32 target, s32 callbackArg);

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 D_801B8AA4[2][0xC000];
extern void* D_801D0AA4;
extern void* D_80163C74;
extern s16 D_801B8A88;
extern s16 D_801B8A8A;
extern SpriteRenderDesc D_801B8A8C;
extern SpriteRenderDesc D_801B8A98;
extern s32 D_801B48EC;
extern s32 D_801B89C4;
extern u_long D_801B08A8[];
extern u_long D_801B4980[];

// Only StartFrame/AnimationFrame are common to every magic overlay.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR unkC;
    /* 0x14 */ char pad14[6];
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ char pad1C[4];
} Hikari3Data; // size:0x20

extern Hikari3Data g_BattleEffectSlots[];

void MAGIC_Hikari3(s32 targetMask, s32 callbackArg) { Hikari3Start(targetMask, callbackArg); }

// Same as BarrierDoubleBufferFlip.
void Hikari3DoubleBufferFlip(void) {
    Hikari3Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D0AA4 = D_801B8AA4[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void Hikari3ViewportFlashTick(void) {
    Hikari3Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_80163C74 = func_800C4FC8(D_801B8A88, 0, 0);
    if (!D_80062D98) {
        D_801B8A88 += D_801B8A8A;
        if (!D_801B8A88) {
            effect->StartFrame = -1;
        }
    }
}

void Hikari3FlashFadeInSetup(s16 frames) {
    D_801B8A88 = 0;
    D_801B8A8A = 0xFF / frames;
    BattleEffectRegister(Hikari3ViewportFlashTick);
}

void Hikari3FlashFadeOutSetup(s16 frames) {
    D_801B8A88 = 0xFF;
    D_801B8A8A = -(0xFF / frames);
}

void Hikari3SparkSpriteTick(void) {
    Hikari3Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x1800, 0);
    D_801B8A8C.frameIndex = effect->AnimationFrame;
    D_801D0AA4 = func_800D4D90(&D_801B8A8C, g_cDb->unk70, 12, D_801D0AA4);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == D_801B48EC) {
            effect->StartFrame = -1;
        }
    }
}

void Hikari3HitSpriteTick(void) {
    Hikari3Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x1800, 0);
    D_801B8A98.frameIndex = effect->AnimationFrame;
    D_801D0AA4 = func_800D4D90(&D_801B8A98, g_cDb->unk70, 12, D_801D0AA4);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801B89C4) {
            effect->StartFrame = -1;
        }
    }
}

void Hikari3TargetTick(void) {
    Hikari3Data* effect;
    Hikari3Data* child;
    SVECTOR vec;
    MATRIX matrix;
    s32 flag;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (!D_80062D98) {
        if (!effect->AnimationFrame) {
            child = &g_BattleEffectSlots[BattleEffectRegister(Hikari3SparkSpriteTick)];
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
                D_801D0AA8[i] = g_BattleModels[i].specialFlags;
                g_BattleModels[i].specialFlags |= 2;
            }
        }
        if (effect->AnimationFrame == 4) {
            D_800F5B74 = 0;
            Hikari3FlashFadeInSetup(15);
            for (i = 0; i < 10; i++) {
                g_BattleModels[i].specialFlags = D_801D0AA8[i];
            }
        }
        if (effect->AnimationFrame == 20) {
            Hikari3FlashFadeOutSetup(15);
        }
        if (effect->AnimationFrame == 30) {
            func_800D5774(effect->unk1A);
            func_800BBA84(0x95, effect->unk1A, 0x40);
        }
        if (effect->AnimationFrame >= 31 && effect->AnimationFrame < 35) {
            for (i = 0; i < 2; i++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(Hikari3HitSpriteTick)];
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

void Hikari3TargetSetup(s32 target, s32 callbackArg) {
    Hikari3Data* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(Hikari3TargetTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[9], &effect->Pos);
    effect->unk1A = target;
}

void Hikari3Start(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(Hikari3DoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B08A8, 0, 0, 0);
    BattleSetLoadTimToVram(D_801B4980, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 4, Hikari3TargetSetup);
    func_800BBA84(0x6B, callbackArg, 0x40);
}
