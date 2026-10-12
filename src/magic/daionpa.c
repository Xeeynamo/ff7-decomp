//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "daionpa.h"
#include "../battle/battle.h"

// Effect shared by Big Sound (大音波) and Voice of Ages (ときのこえ).

void DaionpaStart(s32 targetMask, s32 callbackArg);
extern u_long D_801B0E98[];
void DaionpaRingSpriteTick(void);
void DaionpaImpactModelTick(void);
void DaionpaScreenWaveModelTick(void);
void DaionpaWaveSpriteTick(void);
void DaionpaMainTick(void);
extern s32 g_DaionpaTargetMask;
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 g_DaionpaPrimBuffer[2][0xC000];
extern void* g_DaionpaBufferPtr;

// Only StartFrame/AnimationFrame are common to every magic overlay.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR unkC;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ char pad1A[6];
} DaionpaData; // size:0x20

extern DaionpaData g_BattleEffectSlots[];

void MAGIC_Daionpa(s32 targetMask, s32 callbackArg) { DaionpaStart(targetMask, callbackArg); }

// Same as BioraDoubleBufferFlip.
void DaionpaDoubleBufferFlip(void) {
    DaionpaData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_DaionpaBufferPtr = g_DaionpaPrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

extern SpriteRenderDesc g_DaionpaSpriteDesc;
extern SpriteAnim* g_DaionpaSpriteAnims[];

extern ModelRenderDesc g_DaionpaModelDesc;
extern SVECTOR g_DaionpaRotation;
extern s16 g_DaionpaUnk4C;
extern VECTOR g_DaionpaScale;
extern s32 g_DaionpaGeomX;
extern s32 g_DaionpaGeomY;
extern ModelRenderDesc g_DaionpaModelDesc2;

extern SpriteRenderDesc g_DaionpaSpriteDesc2;
extern SpriteAnim* g_DaionpaSpriteAnims2[];

void DaionpaWaveSpriteTick(void) {
    DaionpaData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x1800, 0);
    g_DaionpaSpriteDesc.frameIndex = effect->AnimationFrame;
    g_DaionpaSpriteDesc.frames = g_DaionpaSpriteAnims[effect->unk14];
    g_DaionpaBufferPtr = func_800D4D90(&g_DaionpaSpriteDesc, g_cDb->unk70, 12, g_DaionpaBufferPtr);
    if (!D_80062D98) {
        effect->Pos.vx += effect->unkC.vx;
        effect->Pos.vy += effect->unkC.vy;
        effect->Pos.vz += effect->unkC.vz;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == g_DaionpaSpriteAnims[effect->unk14]->frameCount) {
            effect->StartFrame = -1;
        }
    }
}

void DaionpaImpactModelTick(void) {
    DaionpaData* effect;
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
    RotMatrixYXZ(&g_DaionpaRotation, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    g_DaionpaModelDesc.color = effect->AnimationFrame * 0x100;
    g_DaionpaBufferPtr = func_800D29D4(&g_DaionpaModelDesc, g_cDb->unk70, 12, g_DaionpaBufferPtr);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 15) {
            effect->StartFrame = -1;
        }
    }
}

void DaionpaScreenWaveModelTick(void) {
    DaionpaData* effect;
    MATRIX matrix;
    MATRIX unused;
    SVECTOR rot;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    ReadGeomOffset(&g_DaionpaGeomX, &g_DaionpaGeomY);
    SetGeomOffset(0xA0, 0x64);
    g_DaionpaScale.vy = effect->AnimationFrame * 300 + 0x2000;
    rot.vx = 0x400;
    rot.vy = effect->AnimationFrame << 5;
    rot.vz = 0;
    RotMatrix(&rot, &matrix);
    ScaleMatrix(&matrix, &g_DaionpaScale);
    matrix.t[0] = 0;
    matrix.t[1] = 0;
    matrix.t[2] = 0x1F40;
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    g_DaionpaModelDesc2.color = 0x1000 - (g_DaionpaUnk4C * 0x100);
    g_DaionpaBufferPtr = func_800D29D4(&g_DaionpaModelDesc2, g_cDb->unk70, 12, g_DaionpaBufferPtr);
    SetGeomOffset(g_DaionpaGeomX, g_DaionpaGeomY);
    if (!D_80062D98) {
        if (effect->AnimationFrame < 8) {
            g_DaionpaUnk4C++;
        }
        if (effect->AnimationFrame >= 0x17) {
            g_DaionpaUnk4C--;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x1E) {
            effect->StartFrame = -1;
        }
    }
}

void DaionpaRingSpriteTick(void) {
    DaionpaData* effect;
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
    g_DaionpaSpriteDesc2.frames = g_DaionpaSpriteAnims2[effect->unk14];
    g_DaionpaSpriteDesc2.color.r = g_DaionpaSpriteDesc2.color.g = g_DaionpaSpriteDesc2.color.b = shade;
    g_DaionpaBufferPtr = func_800D4D90(&g_DaionpaSpriteDesc2, g_cDb->unk70, 12, g_DaionpaBufferPtr);
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

void DaionpaMainTick(void) {
    DaionpaData* effect;
    DaionpaData* child;
    SVECTOR vec;
    SVECTOR rot;
    MATRIX matrix;
    s32 flag;
    s16 ang;
    s16 i;
    s16 j;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (!D_80062D98) {
        if (effect->AnimationFrame < 20) {
            for (i = 0; i < 2; i++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(DaionpaRingSpriteTick)];
                child->Pos.vx = effect->Pos.vx;
                child->Pos.vy = effect->Pos.vy;
                child->Pos.vz = effect->Pos.vz;
                child->unkC.vx = (effect->unkC.vx - effect->Pos.vx) / 15;
                child->unkC.vy = (effect->unkC.vy - effect->Pos.vy) / 15;
                child->unkC.vz = (effect->unkC.vz - effect->Pos.vz) / 15;
                ang = rand() % 4096;
                vec.vx = (u32)(rcos(ang) * 25) >> 10;
                vec.vy = (u32)(rsin(ang) * 25) >> 10;
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
        }
        if (effect->AnimationFrame == 30) {
            BattleEffectRegister(DaionpaScreenWaveModelTick);
            func_800BBA84(0xF6, -1, 0x40);
        }
        if (effect->AnimationFrame == 54) {
            for (i = 0; i < 10; i++) {
                if ((g_DaionpaTargetMask >> i) & 1) {
                    child = &g_BattleEffectSlots[BattleEffectRegister(DaionpaImpactModelTick)];
                    BattleGetPartPosition(i, g_BattleModels[i].boneIndices[0], &child->Pos);
                }
            }
            func_800BBA84(0x189, -1, 0x40);
        }
        if (effect->AnimationFrame >= 57 && effect->AnimationFrame < 64) {
            for (i = 0; i < 10; i++) {
                if ((g_DaionpaTargetMask >> i) & 1) {
                    for (j = 0; j < 2; j++) {
                        child = &g_BattleEffectSlots[BattleEffectRegister(DaionpaWaveSpriteTick)];
                        BattleGetPartPosition(i, g_BattleModels[i].boneIndices[0], &child->Pos);
                        child->Pos.vx += rand() % 1200 - 600;
                        child->Pos.vy += rand() % 1200 - 600;
                        child->Pos.vz += rand() % 1200 - 600;
                        child->unk14 = rand() % 6;
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
            }
        }
        if (effect->AnimationFrame == 60) {
            for (i = 0; i < 10; i++) {
                if ((g_DaionpaTargetMask >> i) & 1) {
                    func_800D5774(i);
                }
            }
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 70) {
            BattleFixedPointRampUpdateInit(0, 10);
            effect->StartFrame = -1;
        }
    }
}

void DaionpaStart(s32 targetMask, s32 callbackArg) {
    DaionpaData* effect;

    BattleEffectRegister(DaionpaDoubleBufferFlip);
    g_DaionpaTargetMask = targetMask;
    BattleSetLoadTimToVram(D_801B0E98, 0, 0, 0);
    effect = &g_BattleEffectSlots[BattleEffectRegister(DaionpaMainTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[8], &effect->Pos);
    BattleEntityGetCenter(targetMask, &effect->unkC);
    func_800BBA84(0x118, callbackArg, 0);
    BattleFixedPointRampUpdateInit(0xF00, 10);
    D_800F5B70 = 0;
    D_800F5B71 = 0;
    D_800F5B72 = 0x20;
}
