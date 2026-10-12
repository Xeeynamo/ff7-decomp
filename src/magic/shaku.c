//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "shaku.h"
#include "../battle/battle.h"

u_long* func_800C4FC8();

// Effect shared by Wave Artillery (波動砲) and Golem Laser (ゴーレムレーザー).

void ShakuStart(s32 targetMask, s32 callbackArg);

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 g_ShakuPrimBuffer[2][0xC000];
extern void* g_ShakuBufferPtr;

// Only StartFrame/AnimationFrame are common to every magic overlay; the
// rest is payload this effect lays out for itself.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR PartPos;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
} ShakuData; // size:0x20

extern MATRIX D_801D573C;
extern ModelRenderDesc D_801D567C;
extern ModelRenderDesc D_801D56A4;
extern ModelRenderDesc D_801D56BC;
extern ModelRenderDesc D_801D56E8;
extern ModelRenderDesc D_801D5714;
extern SVECTOR D_801D569C;
extern SVECTOR D_801D56B4;
extern SVECTOR D_801D56F8;
extern SVECTOR D_801D5730;
extern SVECTOR D_801D5760;
extern SpriteRenderDesc D_801D5654;
extern SpriteRenderDesc D_801D5660;
extern SpriteRenderDesc D_801D56CC;
extern SpriteRenderDesc D_801D56D8;
extern SpriteRenderDesc D_801D5724;
extern VECTOR D_801D566C;
extern VECTOR D_801D5704;
extern ShakuData g_BattleEffectSlots[];
extern ShakuData g_BattleDetachedSlots[];
extern s16 g_BattleDetachedCursor;
extern s16 D_801D5650;
extern s16 D_801D56E4;
extern s16 D_801D5700;
extern s16 D_801D5738;
extern s16 D_801D575C;
extern s32 D_801D5294;
extern s32 D_801D54CC;
extern s32 D_801D1C38[];
extern s32 D_801D223C[];
extern s32 D_801D2840[];
extern s32 D_801D37E4[];
extern s32 D_801D4788[];
extern s32* D_801D568C[];
extern u16 D_801031EA;
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;
extern u_long D_801B1B38[];
extern void* D_80163C74;
s32 BattleDetachedRegister(void (*func)(void));
void func_800BBA84(u16 arg0, s32 arg1, s32 arg2);
void ShakuViewportFlashTick(void);
void ShakuEmberTick(void);
void ShakuRisingModelTick(void);
void ShakuShrinkingModelTick(void);
void ShakuBeamHeadTick(void);
void ShakuMainTick(void);

void MAGIC_Shaku(s32 targetMask, s32 callbackArg) { ShakuStart(targetMask, callbackArg); }

void ShakuTargetCenterCalc(s16 targetMask, SVECTOR* out) {
    VECTOR sum;
    SVECTOR pos;
    s16 i;
    s16 count;

    count = 0;
    sum.vx = sum.vy = sum.vz = 0;
    for (i = 0; i < 10; i++) {
        if ((targetMask >> i) & 1) {
            BattleGetPartPosition(i, g_BattleModels[i].boneIndices[0], &pos);
            count++;
            sum.vx += pos.vx;
            sum.vy += pos.vy;
            sum.vz += pos.vz;
        }
    }
    out->vx = sum.vx / count;
    out->vy = sum.vy / count;
    out->vz = sum.vz / count;
}

void ShakuSprayVectorCalc(s32 offset, s16 spread, SVECTOR* out) {
    SVECTOR in;
    SVECTOR rot;
    MATRIX matrix;

    in.vz = 0;
    in.vx = 0;
    in.vy = offset + (rand() % spread * 2 - spread);
    rot.vy = 0;
    rot.vx = rand() % 800 - 400;
    rot.vz = rand() % 800 - 400;
    RotMatrix(&rot, &matrix);
    ApplyMatrixSV(&matrix, &in, out);
}

void ShakuSpriteMatrixSetup(SVECTOR* pos, SVECTOR* rot, s16 scale, s32 offset) {
    MATRIX matrix;
    MATRIX rotated;
    VECTOR dir;
    s32 flag;
    s16 i;

    for (i = 0; i < 3; i++) {
        matrix.m[2][i] = 0;
        matrix.m[1][i] = 0;
        matrix.m[0][i] = 0;
    }
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(pos, (VECTOR*)matrix.t, &flag);
    if (offset != 0) {
        VectorNormal((VECTOR*)matrix.t, &dir);
        matrix.t[0] += offset * dir.vx >> 12;
        matrix.t[1] += offset * dir.vy >> 12;
        matrix.t[2] += offset * dir.vz >> 12;
    }
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = scale;
    RotMatrixYXZ(rot, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
}

// Same as BarrierDoubleBufferFlip.
static void ShakuDoubleBufferFlip(void) {
    ShakuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_ShakuBufferPtr = g_ShakuPrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void ShakuViewportFlashTick(void) {
    ShakuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_80163C74 = func_800C4FC8(D_801D5650, 0, 0);
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame < 16) {
            D_801D5650 += 15;
        }
        if (effect->AnimationFrame >= 35) {
            D_801D5650 -= 15;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 50) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuSparkSpriteTick(void) {
    ShakuData* effect;

    effect = &g_BattleDetachedSlots[g_BattleDetachedCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2800, 0);
    D_801D5654.frameIndex = effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801D5654, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801D5294) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuEmberTick(void) {
    MATRIX rotated;
    MATRIX matrix;
    SVECTOR rot;
    s32 flag;
    ShakuData* effect;
    s16 fade;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    for (i = 0; i < 3; i++) {
        matrix.m[2][i] = 0;
        matrix.m[1][i] = 0;
        matrix.m[0][i] = 0;
    }
    fade = 8;
    if (effect->AnimationFrame >= 12) {
        fade = 20 - effect->AnimationFrame;
    }
    rot.vy = 0;
    rot.vx = 0;
    rot.vz = effect->unk14;
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(&effect->Pos, (VECTOR*)matrix.t, &flag);
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x2000;
    RotMatrixYXZ(&rot, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    D_801D5660.frameIndex = effect->unk16;
    D_801D5660.color.cd = fade == 8 ? 0x2C : 0x2E;
    D_801D5660.color.r = D_801D5660.color.g = D_801D5660.color.b = fade << 4;
    D_80163C74 = func_800D4D90(&D_801D5660, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->Pos.vx += effect->PartPos.vx;
        effect->Pos.vy += effect->PartPos.vy;
        effect->Pos.vz += effect->PartPos.vz;
        if (effect->Pos.vy > 0) {
            effect->Pos.vy = 0;
            effect->PartPos.vy *= -1;
        }
        effect->PartPos.vy += 0x14;
        effect->unk14 += 0x200;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x14) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuRisingModelTick(void) {
    MATRIX matrix;
    MATRIX composed;
    SVECTOR rot;
    ShakuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vy = 0;
    rot.vx = 0;
    rot.vz = effect->unk14;
    RotMatrix(&rot, &matrix);
    ScaleMatrix(&matrix, &D_801D566C);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D567C.color = 0x80 - effect->AnimationFrame * 6;
    D_801D567C.model = D_801D568C[effect->unk16];
    g_ShakuBufferPtr = func_800D29D4(&D_801D567C, g_cDb->unk70, 12, g_ShakuBufferPtr);
    if (D_80062D98 == 0) {
        effect->Pos.vy -= 200;
        effect->unk14 += 0x100;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuGrowingModelTick(void) {
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    ShakuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrixYXZ(&D_801D569C, &matrix);
    MulMatrix0(&D_801D573C, &matrix, &matrix);
    scale.vx = scale.vy = scale.vz = effect->AnimationFrame * 1500;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    SetFarColor(0, 0, 0);
    D_801D56A4.color = effect->AnimationFrame * 256;
    g_ShakuBufferPtr = func_800D29D4(&D_801D56A4, g_cDb->unk70, 12, g_ShakuBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuShrinkingModelTick(void) {
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    ShakuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (effect->AnimationFrame >= 3) {
        RotMatrixYXZ(&D_801D56B4, &matrix);
        MulMatrix0(&D_801D573C, &matrix, &matrix);
        scale.vx = scale.vy = scale.vz = 0x4000 - effect->AnimationFrame * 400;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = effect->Pos.vx;
        matrix.t[1] = effect->Pos.vy;
        matrix.t[2] = effect->Pos.vz;
        CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
        SetRotMatrix(&composed);
        SetTransMatrix(&composed);
        SetFarColor(0, 0, 0);
        D_801D56BC.color = effect->AnimationFrame * 200;
        g_ShakuBufferPtr = func_800D29D4(&D_801D56BC, g_cDb->unk70, 12, g_ShakuBufferPtr);
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuDriftSpriteTick(void) {
    ShakuData* effect;

    effect = &g_BattleDetachedSlots[g_BattleDetachedCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x4000, 0);
    D_801D56CC.frameIndex = effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801D56CC, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->Pos.vx += effect->PartPos.vx;
        effect->Pos.vy += effect->PartPos.vy;
        effect->Pos.vz += effect->PartPos.vz;
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801D54CC) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuPulseSpriteTick(void) {
    ShakuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, (s16)(0x1000 + ((rsin(effect->AnimationFrame * 51) << 11) >> 16 << 7)), 0);
    D_801D56D8.frameIndex = effect->AnimationFrame % 2;
    g_ShakuBufferPtr = func_800D4D90(&D_801D56D8, (u_long**)&g_cDb->unk70, 12, g_ShakuBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 40) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuBeamColumnTick(void) {
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    ShakuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrixYXZ(&D_801D56F8, &matrix);
    MulMatrix0(&D_801D573C, &matrix, &matrix);
    scale.vx = scale.vz = 0x4000 - effect->AnimationFrame * 300;
    scale.vy = effect->AnimationFrame * 900;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    SetFarColor(0, 0, 0);
    D_801D56E8.color = 0x1000 - D_801D56E4 * 256;
    D_801D56E8.model = D_801D1C38;
    g_ShakuBufferPtr = func_800D29D4(&D_801D56E8, g_cDb->unk70, 12, g_ShakuBufferPtr);
    D_801D56E8.model = D_801D223C;
    g_ShakuBufferPtr = func_800D29D4(&D_801D56E8, g_cDb->unk70, 12, g_ShakuBufferPtr);
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame >= 13) {
            D_801D56E4 -= 1;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuBeamHeadTick(void) {
    MATRIX matrix;
    MATRIX composed;
    ShakuData* effect;
    ShakuData* child;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrixYXZ(&D_801D5730, &matrix);
    MulMatrix0(&D_801D573C, &matrix, &matrix);
    ScaleMatrix(&matrix, &D_801D5704);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D5714.color = (u16)D_801D5700 * 16;
    D_801D5714.model = D_801D2840;
    g_ShakuBufferPtr = func_800D29D4(&D_801D5714, g_cDb->unk70, 12, g_ShakuBufferPtr);
    D_801D5714.model = D_801D37E4;
    g_ShakuBufferPtr = func_800D29D4(&D_801D5714, g_cDb->unk70, 12, g_ShakuBufferPtr);
    D_801D5714.model = D_801D4788;
    g_ShakuBufferPtr = func_800D29D4(&D_801D5714, g_cDb->unk70, 12, g_ShakuBufferPtr);
    BattleSetBillboardMatrix(&effect->Pos, 0x4000, 0);
    D_801D5724.color.r = D_801D5724.color.g = D_801D5724.color.b = D_801D5700 * 20;
    D_801D5724.frameIndex = effect->AnimationFrame % 2;
    g_ShakuBufferPtr = func_800D4D90(&D_801D5724, (u_long**)&g_cDb->unk70, 12, g_ShakuBufferPtr);
    if (D_80062D98 == 0) {
        effect->Pos.vx += effect->PartPos.vx;
        effect->Pos.vy += effect->PartPos.vy;
        effect->Pos.vz += effect->PartPos.vz;
        if ((effect->AnimationFrame & 1) == 0) {
            child = &g_BattleEffectSlots[BattleEffectRegister(ShakuShrinkingModelTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
        }
        if (effect->Pos.vy < 0) {
            child = &g_BattleEffectSlots[BattleEffectRegister(ShakuEmberTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vz = effect->Pos.vz;
            child->Pos.vy = 0;
            D_801D5738 = rand() % 4096;
            child->PartPos.vx = (u32)(rcos(D_801D5738) * 15) >> 9;
            child->PartPos.vz = (u32)(rsin(D_801D5738) * 15) >> 9;
            child->PartPos.vy = -rand() % 300 - 50;
            child->unk16 = rand() % 7;
            if (effect->AnimationFrame % 3 == 0) {
                child = &g_BattleEffectSlots[BattleEffectRegister(ShakuRisingModelTick)];
                child->Pos.vx = effect->Pos.vx + rand() % 4000 - 2000;
                child->Pos.vz = effect->Pos.vz + rand() % 4000 - 2000;
                child->Pos.vy = 0;
                child->unk16 = rand() % 4;
            }
        }
        if (effect->AnimationFrame >= 13) {
            D_801D5700 -= 1;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuMainTick(void) {
    SVECTOR in;
    SVECTOR rot;
    MATRIX matrix;
    s32 flag;
    ShakuData* effect;
    ShakuData* child;
    s16 i;
    u16* u16p;
    u16 t;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            child = &g_BattleEffectSlots[BattleEffectRegister(ShakuGrowingModelTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child = &g_BattleEffectSlots[BattleEffectRegister(ShakuPulseSpriteTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            func_800BBA84(0x174, effect->unk1E, 0x40);
        }
        if (effect->AnimationFrame == 20) {
            child = &g_BattleEffectSlots[BattleEffectRegister(ShakuBeamColumnTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child = &g_BattleEffectSlots[BattleEffectRegister(ShakuBeamHeadTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child->PartPos.vx = effect->PartPos.vx;
            child->PartPos.vy = effect->PartPos.vy;
            child->PartPos.vz = effect->PartPos.vz;
            func_800BBA84(0x176, effect->unk1E, 0x40);
        }
        if (effect->AnimationFrame == 20) {
            BattleEffectRegister(ShakuViewportFlashTick);
        }
        if (effect->AnimationFrame == 54) {
            BattleFixedPointRampUpdateInit(0, 16);
        }
        if (effect->AnimationFrame >= 37 && effect->AnimationFrame < 44) {
            for (i = 0; i < 10; i++) {
                if ((D_801D575C >> i) & 1) {
                    child = &g_BattleDetachedSlots[BattleDetachedRegister(ShakuDriftSpriteTick)];
                    BattleGetPartPosition(i, g_BattleModels[i].boneIndices[0], &child->Pos);
                    in.vy = 0x50;
                    in.vz = 0;
                    in.vx = 0;
                    rot.vx = rand() % 4096;
                    rot.vz = rand() % 4096;
                    rot.vy = 0;
                    RotMatrixYXZ(&rot, &matrix);
                    matrix.t[2] = 0;
                    matrix.t[1] = 0;
                    matrix.t[0] = 0;
                    SetRotMatrix(&matrix);
                    SetTransMatrix(&matrix);
                    RotTransSV(&in, &child->PartPos, &flag);
                }
            }
        }
        if (effect->AnimationFrame == 40) {
            for (i = 0; i < 10; i++) {
                if ((D_801D575C >> i) & 1) {
                    func_800D5774(i);
                }
            }
            func_800BBA84(0xFA, -1, 0x40);
        }
        if (effect->AnimationFrame >= 21) {
            u16p = &D_801031EA;
            t = *u16p + 0x10;
            *u16p = t - (effect->AnimationFrame % 2) * 32;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 70) {
            effect->StartFrame = -1;
        }
    }
}

void ShakuStart(s32 targetMask, s32 callbackArg) {
    VECTOR vec;
    SVECTOR dir;
    ShakuData* effect;

    BattleEffectRegister(ShakuDoubleBufferFlip);
    D_801D575C = targetMask;
    effect = &g_BattleEffectSlots[BattleEffectRegister(ShakuMainTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[8], &effect->Pos);
    ShakuTargetCenterCalc(targetMask, &D_801D5760);
    vec.vx = dir.vx = D_801D5760.vx - effect->Pos.vx;
    vec.vy = dir.vy = D_801D5760.vy - effect->Pos.vy;
    vec.vz = dir.vz = D_801D5760.vz - effect->Pos.vz;
    BattleMatrixFromDirection(&dir, &D_801D573C);
    VectorNormal(&vec, &vec);
    effect->PartPos.vx = (u32)(vec.vx * 75) >> 9;
    effect->PartPos.vy = (u32)(vec.vy * 75) >> 9;
    effect->PartPos.vz = (u32)(vec.vz * 75) >> 9;
    effect->unk1E = callbackArg;
    BattleSetLoadTimToVram(D_801B1B38, 0, 0, 0);
    D_800F5B70 = D_800F5B71 = D_800F5B72 = 0;
    BattleFixedPointRampUpdateInit(0x1000, 0x10);
}
