#include "common.h"
#include "../battle/battle.h"

typedef struct Lv5DeathData {
    s16 StartFrame;
    s16 AnimationFrame;
    SVECTOR Pos;
    u16 Rotation;
    s16 TargetIndex;
    char pad10[0x10];
} Lv5DeathData;

/* Battle effect instance array. */
extern Lv5DeathData D_80162978[];

/* State used by the LV5 Death effect. */
extern u8 g_BattleEffectPaused;
extern s8 g_BattleFarColorRed;
extern s8 g_BattleFarColorGreen;
extern s8 g_BattleFarColorBlue;
extern s16 g_BattleModelFade; //3d model color related
extern s16 D_8015169C;
extern u8 D_8015190F[];
extern s16 D_80162080;
extern s32 lv5deth_tim; //tim for animation
extern s32 light_1_rdx; //glow effect
extern s8 g_Lv5DeathSpriteRed;
extern volatile s8 g_Lv5DeathSpriteGreen;
extern volatile s8 g_Lv5DeathSpriteBlue;
extern s16 g_Lv5DeathAnimFrame;
extern s32 g_Lv5DeathPrimBuffer0;
extern s32 g_Lv5DeathPrimBuffer1;
extern s32* g_Lv5DeathPrimPtr;
extern s32* g_Lv5DeathInitEffect;
extern s32 g_dbIndex;
extern s32 g_numberOfTargets;
extern s32 g_cDb;


/* Battle / magic engine functions. */
extern void BattleGetPartPosition(s32 arg0, s32 arg1, void* arg2);
extern s32 BattleEffectRegister(void (*func)(void));
extern void MagicAnimationRegister(s32, s32, s32, void (*func)(void));
extern void QueueTimLoad(u_long* addr, s16 imgXY, s16 clutX, s16 clutY);
extern s32* BattleBuildEffectPrimitives(void* effect,
    s32* orderingTable,
    s32 depthShift,
    s32* primitiveBuffer*);
extern void BattleSetEffectTransform(SVECTOR* pos, s16 arg1, s32 arg2);
extern s32 BattleBuildSpritePrimitives(s8*, s32, s32, s32);
extern void BattleQueueTargetEvent(s16);

/* LV5 Death functions. */
void MAGIC_Lv5DeathInit(void);
void Lv5DeathDrawGlow(void);
void Lv5DeathDrawSkull(void);
void Lv5DeathUpdateFade(void);
void Lv5DeathCreateTargetEffect(s32 arg0);
void Lv5DeathSetup(s32 arg0, s32 arg1);

void MAGIC_Lv5DeathInit(void) {
    s32* primPtr;

    primPtr = &g_Lv5DeathPrimBuffer1;

    if (g_dbIndex == 0) {
        primPtr = &g_Lv5DeathPrimBuffer0;
    }

    g_Lv5DeathPrimPtr = primPtr;

    if (D_80162080 < 2) {
        *g_Lv5DeathInitEffect = -1;
    }
}

void Lv5DeathStart(void) { ((void (*)(void))Lv5DeathSetup)(); }

void Lv5DeathDrawGlow(void) {
    char pad[0x34];

    Lv5DeathData* effect;
    s32 rotation;
    s32 animationFrame;
    s32* effectData;

    effect = &D_80162978[D_8015169C];

    rotation = effect->Rotation << 16;

    BattleSetEffectTransform(&effect->Pos, rotation >> 16, -(rotation >> 19));

    effectData = (s32*)0x1F800000;

    effectData[0] = (s32)&light_1_rdx;
    effectData[1] = 0x88;
    ((s16*)effectData)[4] = 0;
    ((s16*)effectData)[5] = 0x800;
    ((s16*)effectData)[6] = 0;
    ((s16*)effectData)[7] = 0;

    animationFrame = effect->AnimationFrame;
    if (animationFrame < 8) {
        animationFrame <<= 8;
        ((s16*)effectData)[5] = 0x1000 - animationFrame;
    } else if (animationFrame >= 0x25) {
        ((s16*)effectData)[5] = (animationFrame << 8) - 0x1D00;
    }
    SetFarColor(0, 0, 0);

    g_Lv5DeathPrimPtr = BattleBuildEffectPrimitives((s32)effectData, g_cDb + 0x70, 0xC, g_Lv5DeathPrimPtr);

    if (effect->AnimationFrame >= 0x2D) {
        effect->StartFrame = -1;
    }

    if (g_BattleEffectPaused == 0) {
        effect->AnimationFrame++;
    }
}

void Lv5DeathDrawSkull(void) {
    s16 animationFrame;
    u8 opacity;
    s32 rotation;
    s8* spriteColor;
    s16* animationFramePtr;
    Lv5DeathData* effect;

    effect = &D_80162978[D_8015169C];

    animationFramePtr = &effect->AnimationFrame;

    g_Lv5DeathAnimFrame = *animationFramePtr & 7;

    animationFrame = *animationFramePtr;

    if (animationFrame < 8) {
        opacity = animationFrame * 0x10;
    } else if (animationFrame >= 0x25) {
        opacity = -0x80 - ((animationFrame - 0x25) * 0x10);
    } else {
        opacity = 0x80;
    }

    spriteColor = &g_Lv5DeathSpriteRed;

    g_Lv5DeathSpriteBlue = opacity;
    g_Lv5DeathSpriteGreen = opacity;
    *(volatile s8*)spriteColor = opacity;

    rotation = effect->Rotation;

    BattleSetEffectTransform(&effect->Pos, (s16)rotation, -((s32)(rotation << 16) >> 18));

    g_Lv5DeathPrimPtr = BattleBuildSpritePrimitives(spriteColor - 4, g_cDb + 0x70, 0xC, g_Lv5DeathPrimPtr);

    if (effect->AnimationFrame >= 0x2D) {
        effect->StartFrame = -1;
        g_numberOfTargets -= 1;
    }

    if (g_BattleEffectPaused == 0) {
        if (effect->AnimationFrame == 0x23) {
            BattleQueueTargetEvent(effect->TargetIndex);
        }

        effect->AnimationFrame++;
    }
}

void Lv5DeathUpdateFade(void) {
    s16 animationFrame;
    s16 fadeValue;
    s16 fadeStartFrame;
    Lv5DeathData* effect;

    effect = &D_80162978[D_8015169C];
    animationFrame = effect->AnimationFrame;

    if (animationFrame < 8) {
        g_BattleFarColorBlue = 0;
        g_BattleFarColorGreen = 0;
        g_BattleFarColorRed = 0;

        __asm__ volatile("" : : : "memory");

        fadeStartFrame = effect->AnimationFrame;
        fadeValue = fadeStartFrame * 0x140;
    } else if (g_numberOfTargets <= 0) {
        if (effect->TargetIndex == 0) {
            effect->TargetIndex = animationFrame;
        }

        fadeValue = 0xA00 - ((effect->AnimationFrame - effect->TargetIndex) * 0x140);
    } else {
        fadeValue = 0xA00;
    }

    if (effect->AnimationFrame >= 0x35) {
        fadeValue = 0;
        effect->StartFrame = -1;
    }

    g_BattleModelFade = fadeValue;

    if (g_BattleEffectPaused == 0) {
        effect->AnimationFrame = (s16)((u16)effect->AnimationFrame + 1);
    }
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", Lv5DeathCreateTargetEffect);
#else

void Lv5DeathCreateTargetEffect(s32 arg0) {
    Lv5DeathData* skullEffect;
    Lv5DeathData* glowEffect;
    s32 targetIndex;
    s8* targetPosition;

    targetIndex = arg0;

    skullEffect = &D_80162978[BattleEffectRegister(Lv5DeathDrawSkull)];

    targetPosition = (s8*)&skullEffect->Pos;

    BattleGetPartPosition(targetIndex, D_8015190F[targetIndex * 0xB9C], targetPosition);

    skullEffect->Rotation = 0x1CCC;
    skullEffect->TargetIndex = targetIndex;

    glowEffect = &D_80162978[BattleEffectRegister(Lv5DeathDrawGlow)];

    glowEffect->Pos = skullEffect->Pos;

    glowEffect->Rotation = 0x13DC;

    BattleCommandSend(0x20, BattlePositionToStereoPan(targetPosition), 0xAA);
}
#endif

void Lv5DeathSetup(s32 arg0, s32 arg1) {
    s32 targetCount;
    s32 targetBit;
    Lv5DeathData* effect;

    g_Lv5DeathInitEffect = &D_80162978[BattleEffectRegister(MAGIC_Lv5DeathInit)];

    QueueTimLoad(&lv5deth_tim, 0, 0, 0);

    effect = &D_80162978[BattleEffectRegister(Lv5DeathUpdateFade)];

    effect->TargetIndex = 0;

    MagicAnimationRegister(arg0, arg1, 2, Lv5DeathCreateTargetEffect);

    targetBit = 0;
    targetCount = 0;

    do {
        if ((arg0 >> targetBit++) & 1) {
            targetCount++;
        }
    } while (targetBit < 0xA);

    g_numberOfTargets = targetCount;
}
