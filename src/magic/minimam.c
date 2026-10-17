//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "minimam_assets.h"
// MINIMAM.BIN: A circular pool of 444 position/scale records feeds groups of three billboard sprites.

extern s32 rand(void);

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos;
    s16 Radius, Width, HeightBias, HeightRange, Scale, unk16, TargetIndex;
    char pad1A[6];
} MinimamEffect;
extern MinimamEffect g_BattleEffectSlots[];
static void func_minimam_801B0514(s32 arg0, s32 arg1);

static void func_minimam_801B0000(void) {
    u8* offset;

    offset = D_801C08AC;
    if (g_dbIndex == 0) {
        offset = D_801B48AC;
    }
    D_801CC8AC = offset;
    if (g_BattleEffectCount < 2) {
        *D_801CC8B0 = -1;
    }
}

void MAGIC_Minimam(s32 targetMask, s32 callbackArg) { func_minimam_801B0514(targetMask, callbackArg); }

static void func_minimam_801B0074(void) {
    MinimamEffect* effect;
    SVECTOR* position;
    s32 randomScale;
    s32 i;
    s32 temp_a2;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801B48A0.frameIndex = (u16)effect->AnimationFrame;
    randomScale = effect->TargetIndex * 8;
    for (i = 0; i < 3; i++) {
        position = (SVECTOR*)((u8*)g_MinimamParticles + randomScale);
        temp_a2 = *(s16*)((u8*)&g_MinimamParticles[0].pad + randomScale);
        randomScale += 8;
        BattleSetBillboardMatrix(position, temp_a2, -(temp_a2 >> 2));
        D_801CC8AC = func_800D4D90(&D_801B48A0, g_cDb->unk70, 0xC, D_801CC8AC);
        if (randomScale >= 0xDE0) {
            randomScale = 0;
        }
    }
    if (effect->AnimationFrame >= 7) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 != 0) {
        return;
    }
    effect->AnimationFrame = (u16)effect->AnimationFrame + 1;
}

static void func_minimam_801B0190(void) {
    MinimamEffect* effect;
    MinimamEffect* particle;
    s32 randomScale;
    s32 i;
    s32 iValue;
    s32 iValueValue;

    if (D_80062D98 != 0) {
        return;
    }
    particle = g_BattleEffectSlots;
    effect = &particle[g_BattleEffectCursor];
    if ((u32)((u16)effect->AnimationFrame - 2) < 0x1AU) {
        iValue = D_801CC8B4;
        i = iValue * 8;
        particle[BattleEffectRegister(func_minimam_801B0074)].TargetIndex = (s16)iValue;
        for (iValueValue = 0; iValueValue < 3; iValueValue++) {
            randomScale = rand() % 4096 + 2048;
            *(s16*)((u8*)g_MinimamParticles + i) =
                ((u16)effect->Pos.vx + (rand() % (s16)effect->Width)) - (u16)effect->Radius;
            iValue += 1;
            *(s16*)((u8*)&g_MinimamParticles[0].vy + i) =
                ((u16)effect->Pos.vy - (rand() % (s16)effect->HeightRange)) + (u16)effect->HeightBias;
            *(s16*)((u8*)&g_MinimamParticles[0].vz + i) =
                ((u16)effect->Pos.vz + (rand() % (s16)effect->Width)) - (u16)effect->Radius;
            *(s16*)((u8*)&g_MinimamParticles[0].pad + i) = (s16)((s32)(effect->Scale * randomScale) >> 0xC);
            i += 8;
            if (iValue >= 0x1BC) {
                i = 0;
                iValue = 0;
            }
        }
        D_801CC8B4 = iValue;
    }
    if (effect->AnimationFrame == 0x1C) {
        func_800D5774((u32)effect->TargetIndex);
        BattleAkaoCommand(0x20, BattlePositionToStereoPan(&effect->Pos), 0xB1);
    }
    if (effect->AnimationFrame >= 0x23) {
        effect->StartFrame = -1;
    }
    effect->AnimationFrame = (u16)effect->AnimationFrame + 1;
}

static void func_minimam_801B0414(s32 arg0, s32 arg1) {
    MinimamEffect* effect;
    s32 offset;
    s32 modelOffset;
    s32 nextFrame;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_minimam_801B0190)];
    // Keep a byte offset across the part-position call for the model-field views.
    modelOffset = arg0 * 0xB9C;
    BattleGetPartPosition(arg0, *(u8*)((u8*)g_BattleModels + modelOffset + 0x2B), &effect->Pos);
    offset = *(s16*)((u8*)g_BattleModels + modelOffset + 0x12) + 0xE0;
    if (offset >= 0x4F3) {
        offset = 0x4F2;
    }
    effect->Radius = offset;
    effect->Width = offset * 2;
    effect->TargetIndex = (s16)arg0;
    nextFrame = -(effect->Pos.vy >> 1);
    effect->HeightBias = nextFrame;
    effect->HeightRange = nextFrame * 4;
    effect->Scale = (offset << 0xC) / 540;
}

static void func_minimam_801B0514(s32 arg0, s32 arg1) {
    D_801CC8B0 = (s32*)&g_BattleEffectSlots[BattleEffectRegister(func_minimam_801B0000)];
    BattleSetLoadTimToVram(minimam_texture_tim, 0, 0, 0);
    MagicAnimationRegister(arg0, arg1, 1, func_minimam_801B0414);
    D_801CC8B4 = 0;
    BattleAkaoCommand(0x20, BattleEntityGetStereoPan(arg0), 0x134);
}
