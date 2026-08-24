#include "common.h"
#include "../battle/battle.h"

typedef struct Lv5DeathData {
    s16 unk0;
    s16 AnimationFrame;
    SVECTOR Pos;
    u16 Rotation;
    s16 TargetIndex;
    char pad10[0x10];
} Lv5DeathData;

/* Battle effect instance array. */
extern Lv5DeathData D_80162978[];

/* State used by the LV5 Death effect. */
extern u8 D_80062D98;
extern s8 g_BattleFarColorRed;
extern s8 g_BattleFarColorGreen;
extern s8 g_BattleFarColorBlue;
extern s16 D_800F5B74; //3d model color related
extern s16 D_8015169C;
extern u8 D_8015190F[];
extern s16 D_80162080;
extern s32 lv5deth_tim; //tim for animation
extern s32 light_1_rdx; //glow effect
extern s8 D_801C0E48;
extern volatile s8 D_801C0E49;
extern volatile s8 D_801C0E4A;
extern s16 g_Lv5DeathAnimFrame;
extern s32 g_Lv5DeathPrimBuffer0;
extern s32 g_Lv5DeathPrimBuffer1;
extern s32* g_Lv5DeathPrimPtr;
extern s32* D_801D8E54;
extern s32 g_dbIndex;
extern s32 D_801D8E58;
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
extern s32 func_800D4D90(s8*, s32, s32, s32);
extern void func_800D5774(s16);

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
        *D_801D8E54 = -1;
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
        effect->unk0 = -1;
    }

    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

void Lv5DeathDrawSkull(void) {
    s16 temp_v1;
    u8 var_v1;
    s32 temp_a2;
    s8* temp_s0;
    s16* temp_AnimationFrame;
    Lv5DeathData* temp_s1;

    temp_s1 = &D_80162978[D_8015169C];

    temp_AnimationFrame = &temp_s1->AnimationFrame;

    g_Lv5DeathAnimFrame = *temp_AnimationFrame & 7;

    temp_v1 = *temp_AnimationFrame;

    if (temp_v1 < 8) {
        var_v1 = temp_v1 * 0x10;
    } else if (temp_v1 >= 0x25) {
        var_v1 = -0x80 - ((temp_v1 - 0x25) * 0x10);
    } else {
        var_v1 = 0x80;
    }

    temp_s0 = &D_801C0E48;

    D_801C0E4A = var_v1;
    D_801C0E49 = var_v1;
    *(volatile s8*)temp_s0 = var_v1;

    temp_a2 = temp_s1->Rotation;

    BattleSetEffectTransform(&temp_s1->Pos, (s16)temp_a2, -((s32)(temp_a2 << 16) >> 18));

    g_Lv5DeathPrimPtr = func_800D4D90(temp_s0 - 4, g_cDb + 0x70, 0xC, g_Lv5DeathPrimPtr);

    if (temp_s1->AnimationFrame >= 0x2D) {
        temp_s1->unk0 = -1;
        D_801D8E58 -= 1;
    }

    if (D_80062D98 == 0) {
        if (temp_s1->AnimationFrame == 0x23) {
            func_800D5774(temp_s1->TargetIndex);
        }

        temp_s1->AnimationFrame++;
    }
}

void Lv5DeathUpdateFade(void) {
    s16 temp_v0;
    s16 var_v1;
    s16 temp_v1;
    Lv5DeathData* temp_a0;

    temp_a0 = &D_80162978[D_8015169C];
    temp_v0 = temp_a0->AnimationFrame;

    if (temp_v0 < 8) {
        g_BattleFarColorBlue = 0;
        g_BattleFarColorGreen = 0;
        g_BattleFarColorRed = 0;

        __asm__ volatile("" : : : "memory");

        temp_v1 = temp_a0->AnimationFrame;
        var_v1 = temp_v1 * 0x140;
    } else if (D_801D8E58 <= 0) {
        if (temp_a0->TargetIndex == 0) {
            temp_a0->TargetIndex = temp_v0;
        }

        var_v1 = 0xA00 - ((temp_a0->AnimationFrame - temp_a0->TargetIndex) * 0x140);
    } else {
        var_v1 = 0xA00;
    }

    if (temp_a0->AnimationFrame >= 0x35) {
        var_v1 = 0;
        temp_a0->unk0 = -1;
    }

    D_800F5B74 = var_v1;

    if (D_80062D98 == 0) {
        temp_a0->AnimationFrame = (s16)((u16)temp_a0->AnimationFrame + 1);
    }
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", Lv5DeathCreateTargetEffect);
#else

void Lv5DeathCreateTargetEffect(s32 arg0) {
    Lv5DeathData* temp_s0;
    Lv5DeathData* temp_v0;
    s32 temp_s1;
    s8* temp_s3;

    temp_s1 = arg0;

    temp_s0 = &D_80162978[BattleEffectRegister(Lv5DeathDrawSkull)];

    temp_s3 = (s8*)&temp_s0->Pos;

    BattleGetPartPosition(temp_s1, D_8015190F[temp_s1 * 0xB9C], temp_s3);

    temp_s0->Rotation = 0x1CCC;
    temp_s0->TargetIndex = temp_s1;

    temp_v0 = &D_80162978[BattleEffectRegister(Lv5DeathDrawGlow)];

    temp_v0->Pos = temp_s0->Pos;

    temp_v0->Rotation = 0x13DC;

    BattleCommandSend(0x20, BattlePositionToStereoPan(temp_s3), 0xAA);
}
#endif

void Lv5DeathSetup(s32 arg0, s32 arg1) {
    s32 var_a0;
    s32 var_v1;
    Lv5DeathData* var_v0;

    D_801D8E54 = &D_80162978[BattleEffectRegister(MAGIC_Lv5DeathInit)];

    QueueTimLoad(&lv5deth_tim, 0, 0, 0);

    var_v0 = &D_80162978[BattleEffectRegister(Lv5DeathUpdateFade)];

    var_v0->TargetIndex = 0;

    MagicAnimationRegister(arg0, arg1, 2, Lv5DeathCreateTargetEffect);

    var_v1 = 0;
    var_a0 = 0;

    do {
        if ((arg0 >> var_v1++) & 1) {
            var_a0++;
        }
    } while (var_v1 < 0xA);

    D_801D8E58 = var_a0;
}
