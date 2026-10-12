//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "jammer.h"
#include "../battle/battle.h"

// Magic Extinguish (魔法消滅 / Maho Shometsu).

void JammerStart(s32 targetMask, s32 callbackArg);
void JammerTargetTick(void);
void JammerTargetSetup(s32 target, s32 callbackArg);
extern u_long D_801B0BB0[];

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 g_JammerPrimBuffer[2][0xC000];
extern void* g_JammerBufferPtr;

// Only StartFrame/AnimationFrame are common to every magic overlay.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR unkC;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ char pad1A[2];
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
} JammerData; // size:0x20

extern JammerData g_BattleEffectSlots[];

void MAGIC_Jammer(s32 targetMask, s32 callbackArg) { JammerStart(targetMask, callbackArg); }

// Same as BarrierDoubleBufferFlip.
void JammerDoubleBufferFlip(void) {
    JammerData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_JammerBufferPtr = g_JammerPrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

extern SpriteRenderDesc g_JammerSpriteDesc;
extern SpriteAnim* g_JammerSpriteAnims[];

extern ModelRenderDesc g_JammerModelDesc;
extern SVECTOR g_JammerRotation;

extern SpriteRenderDesc g_JammerSpriteDesc2;
extern SpriteAnim* g_JammerSpriteAnims2[];

void JammerWaveSpriteTick(void) {
    JammerData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x1800, 0);
    g_JammerSpriteDesc.frameIndex = effect->AnimationFrame;
    g_JammerSpriteDesc.frames = g_JammerSpriteAnims[effect->unk14];
    g_JammerBufferPtr = func_800D4D90(&g_JammerSpriteDesc, g_cDb->unk70, 12, g_JammerBufferPtr);
    if (!D_80062D98) {
        effect->Pos.vx += effect->unkC.vx;
        effect->Pos.vy += effect->unkC.vy;
        effect->Pos.vz += effect->unkC.vz;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == g_JammerSpriteAnims[effect->unk14]->frameCount) {
            effect->StartFrame = -1;
        }
    }
}

void JammerImpactModelTick(void) {
    JammerData* effect;
    MATRIX matrix;
    MATRIX rotated;
    s32 flag;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    for (i = 0; i < 3; i++) {
        matrix.m[2][i] = 0;
        matrix.m[1][i] = 0;
        matrix.m[0][i] = 0;
    }
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(&effect->Pos, (VECTOR*)matrix.t, &flag);
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = effect->AnimationFrame * 600 + 0x800;
    RotMatrixYXZ(&g_JammerRotation, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    g_JammerModelDesc.color = effect->AnimationFrame * 0x100;
    g_JammerBufferPtr = func_800D29D4(&g_JammerModelDesc, g_cDb->unk70, 12, g_JammerBufferPtr);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 15) {
            effect->StartFrame = -1;
        }
    }
}

void JammerRingSpriteTick(void) {
    JammerData* effect;
    MATRIX matrix;
    MATRIX rotated;
    SVECTOR rot;
    s32 flag;
    s16 shade;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vx = rot.vy = 0;
    rot.vz = effect->AnimationFrame << 8;
    for (i = 0; i < 3; i++) {
        matrix.m[i][0] = matrix.m[i][1] = matrix.m[i][2] = 0;
    }
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(&effect->Pos, (VECTOR*)matrix.t, &flag);
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x1400;
    RotMatrixYXZ(&rot, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    shade = effect->AnimationFrame < 12 ? 8 : 0x14 - effect->AnimationFrame;
    shade <<= 4;
    g_JammerSpriteDesc2.frames = g_JammerSpriteAnims2[effect->unk14];
    g_JammerSpriteDesc2.color.r = g_JammerSpriteDesc2.color.g = g_JammerSpriteDesc2.color.b = shade;
    g_JammerBufferPtr = func_800D4D90(&g_JammerSpriteDesc2, g_cDb->unk70, 12, g_JammerBufferPtr);
    if (!D_80062D98) {
        effect->Pos.vx += effect->unkC.vx;
        effect->Pos.vy += effect->unkC.vy;
        effect->Pos.vz += effect->unkC.vz;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x14) {
            effect->StartFrame = -1;
        }
    }
}

void JammerTargetTick(void) {
    JammerData* effect;
    JammerData* child;
    SVECTOR vec;
    SVECTOR rot;
    MATRIX matrix;
    s32 flag;
    s16 ang;
    s16 j;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (!D_80062D98) {
        if (effect->AnimationFrame < 20) {
            child = &g_BattleEffectSlots[BattleEffectRegister(JammerRingSpriteTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child->unkC.vx = (effect->unkC.vx - effect->Pos.vx) / 15;
            child->unkC.vy = (effect->unkC.vy - effect->Pos.vy) / 15;
            child->unkC.vz = (effect->unkC.vz - effect->Pos.vz) / 15;
            ang = rand() % 4096;
            vec.vx = (u32)(rcos(ang) * 15) >> 10;
            vec.vy = (u32)(rsin(ang) * 15) >> 10;
            vec.vz = 0;
            rot.vz = 0;
            rot.vx = 0;
            rot.vy = child->unk16 = ratan2(effect->unkC.vx - effect->Pos.vx, effect->unkC.vz - effect->Pos.vz);
            RotMatrixYXZ(&rot, &matrix);
            matrix.t[0] = child->unkC.vx;
            matrix.t[1] = child->unkC.vy;
            matrix.t[2] = child->unkC.vz;
            SetRotMatrix(&matrix);
            SetTransMatrix(&matrix);
            RotTransSV(&vec, &child->unkC, &flag);
            child->unk14 = rand() % 7;
        }
        if (effect->AnimationFrame == 24) {
            child = &g_BattleEffectSlots[BattleEffectRegister(JammerImpactModelTick)];
            child->Pos.vx = effect->unkC.vx;
            child->Pos.vy = effect->unkC.vy;
            child->Pos.vz = effect->unkC.vz;
            func_800BBA84(0x189, effect->unk1C, 0);
        }
        if (effect->AnimationFrame >= 25 && effect->AnimationFrame < 36) {
            for (j = 0; j < 2; j++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(JammerWaveSpriteTick)];
                BattleGetPartPosition(effect->unk1C, g_BattleModels[effect->unk1C].boneIndices[0], &child->Pos);
                child->Pos.vx += rand() % 1200 - 600;
                child->Pos.vy += rand() % 1200 - 600;
                child->Pos.vz += rand() % 1200 - 600;
                child->unk14 = rand() % 3;
                child->unk18 = (rand() % 3 << 11) + 0x1000;
                ang = rand() % 4096;
                vec.vx = (u32)(rcos(ang) * 15) >> 10;
                vec.vy = (u32)(rsin(ang) * 15) >> 10;
                vec.vz = 0;
                rot.vz = 0;
                rot.vx = 0;
                rot.vy = rand() % 4096;
                RotMatrixYXZ(&rot, &matrix);
                matrix.t[0] = matrix.t[1] = matrix.t[2] = 0;
                SetRotMatrix(&matrix);
                SetTransMatrix(&matrix);
                RotTransSV(&vec, &child->unkC, &flag);
            }
        }
        if (effect->AnimationFrame == 30) {
            func_800D5774(effect->unk1C);
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 50) {
            effect->StartFrame = -1;
        }
    }
}

void JammerTargetSetup(s32 target, s32 callbackArg) {
    JammerData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(JammerTargetTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[9], &effect->Pos);
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->unkC);
    effect->unk1C = target;
    effect->unk1E = callbackArg;
    func_800BBA84(0x11E, callbackArg, 0);
}

void JammerStart(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(JammerDoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B0BB0, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 4, JammerTargetSetup);
}
