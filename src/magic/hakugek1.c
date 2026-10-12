//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "hakugek1.h"
#include "../battle/battle.h"

// Big Bomber (ビッグボンバー / Biggu Bonba).

void Hakugek1Start(s32 targetMask, s32 callbackArg);

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 g_Hakugek1PrimBuffer[2][0xC000];
extern void* g_Hakugek1BufferPtr;

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
} Hakugek1Data; // size:0x20

extern ModelRenderDesc D_801D2A54;
extern ModelRenderDesc D_801D2A64;
extern SpriteAnim* D_801D2A48[];
extern SpriteRenderDesc D_801D2A30;
extern SpriteRenderDesc D_801D2A3C;
extern Hakugek1Data g_BattleEffectSlots[];
extern Hakugek1Data g_BattleDetachedSlots[];
extern s16 g_BattleDetachedCursor;
extern s32 D_801D296C;
extern s32 D_801D139C[];
extern s32 D_801D1708[];
extern s32 D_801D1B14[];
extern s32 D_801D20E4[];
extern u16 D_801D2A74[];
extern u_long D_801B12FC[];
extern void* D_80163C74;
void Hakugek1TargetSetup(s32 target, s32 callbackArg);
void func_800BBA84(u16 arg0, s32 arg1, s32 arg2);
void Hakugek1TargetTick(void);

void MAGIC_Hakugek1(s32 targetMask, s32 callbackArg) { Hakugek1Start(targetMask, callbackArg); }

void Hakugek1TargetCenterCalc(s16 targetMask, SVECTOR* out) {
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

void Hakugek1SprayVectorCalc(s32 offset, s16 spread, SVECTOR* out) {
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

void Hakugek1SpriteMatrixSetup(SVECTOR* pos, SVECTOR* rot, s16 scale, s32 offset) {
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
static void Hakugek1DoubleBufferFlip(void) {
    Hakugek1Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_Hakugek1BufferPtr = g_Hakugek1PrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void Hakugek1FirePuffTick(void) {
    MATRIX matrix;
    MATRIX rotated;
    s32 flag;
    Hakugek1Data* effect;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    for (i = 0; i < 3; i++) {
        matrix.m[i][2] = 0;
        matrix.m[i][1] = 0;
        matrix.m[i][0] = 0;
    }
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(&effect->Pos, (VECTOR*)matrix.t, &flag);
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x3000;
    MulMatrix(&matrix, &g_BattleWorldView.m);
    RotMatrixYXZ(&effect->PartPos, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    D_801D2A30.frameIndex = effect->AnimationFrame;
    g_Hakugek1BufferPtr = func_800D4D90(&D_801D2A30, (u_long**)&g_cDb->unk70, 12, g_Hakugek1BufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == D_801D296C) {
            effect->StartFrame = -1;
        }
    }
}

void Hakugek1DebrisTick(void) {
    MATRIX rotated;
    MATRIX matrix;
    SVECTOR rot;
    s32 flag;
    Hakugek1Data* effect;
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
    D_801D2A3C.frameIndex = effect->unk16 % 3;
    D_801D2A3C.frames = D_801D2A48[effect->unk16 / 3];
    D_801D2A3C.color.r = D_801D2A3C.color.g = D_801D2A3C.color.b = fade << 4;
    D_80163C74 = func_800D4D90(&D_801D2A3C, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->Pos.vx += effect->PartPos.vx;
        effect->Pos.vy += effect->PartPos.vy;
        effect->Pos.vz += effect->PartPos.vz;
        effect->PartPos.vy += 0x14;
        effect->unk14 += 0x200;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x14) {
            effect->StartFrame = -1;
        }
    }
}
void Hakugek1PillarRiseTick(void) {
    SVECTOR rot;
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    Hakugek1Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vz = 0;
    rot.vx = 0;
    rot.vy = effect->unk14;
    RotMatrixYXZ(&rot, &matrix);
    scale.vy = 0x1000;
    scale.vx = scale.vz = effect->unk18;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    SetFarColor(0, 0, 0);
    D_801D2A54.color = effect->AnimationFrame * 200;
    g_Hakugek1BufferPtr = func_800D29D4(&D_801D2A54, g_cDb->unk70, 12, g_Hakugek1BufferPtr);
    if (D_80062D98 == 0) {
        effect->unk18 += 0x200;
        effect->unk14 += 0x80;
        effect->Pos.vy += effect->PartPos.vy;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x14) {
            effect->StartFrame = -1;
        }
    }
}

void Hakugek1ShockwaveTick(void) {
    SVECTOR rot;
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    Hakugek1Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vz = 0;
    rot.vx = 0;
    rot.vy = effect->AnimationFrame * 64;
    RotMatrixYXZ(&rot, &matrix);
    scale.vx = scale.vy = scale.vz = effect->AnimationFrame * 240 + 0x600;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = 0;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    SetFarColor(0, 0, 0);
    D_801D2A64.flags = MODEL_NO_CULL | MODEL_SEMI_TRANS;
    D_801D2A64.model = D_801D139C;
    D_801D2A64.color = 0x80 - effect->AnimationFrame * 6;
    g_Hakugek1BufferPtr = func_800D29D4(&D_801D2A64, g_cDb->unk70, 12, g_Hakugek1BufferPtr);
    RotMatrixYXZ(&rot, &matrix);
    scale.vx = scale.vy = scale.vz = effect->AnimationFrame * 320 + 0x800;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = 0;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D2A64.flags = MODEL_NO_CULL | MODEL_SEMI_TRANS;
    D_801D2A64.model = D_801D1B14;
    D_801D2A64.color = 0x80 - effect->AnimationFrame * 6;
    g_Hakugek1BufferPtr = func_800D29D4(&D_801D2A64, g_cDb->unk70, 12, g_Hakugek1BufferPtr);
    RotMatrixYXZ(&rot, &matrix);
    scale.vx = scale.vy = scale.vz = effect->AnimationFrame * 240 + 0x600;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = 0;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D2A64.flags = MODEL_DEPTH_CUE | MODEL_NO_CULL | MODEL_SEMI_TRANS;
    D_801D2A64.model = D_801D1708;
    D_801D2A64.color = effect->AnimationFrame * 48 + 0xC00;
    g_Hakugek1BufferPtr = func_800D29D4(&D_801D2A64, g_cDb->unk70, 12, g_Hakugek1BufferPtr);
    D_801D2A64.flags = MODEL_DEPTH_CUE | MODEL_NO_CULL | MODEL_SEMI_TRANS;
    D_801D2A64.model = D_801D20E4;
    D_801D2A64.color = effect->AnimationFrame * 96;
    g_Hakugek1BufferPtr = func_800D29D4(&D_801D2A64, g_cDb->unk70, 12, g_Hakugek1BufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void Hakugek1TargetTick(void) {
    SVECTOR pos;
    s32 pad[10]; // unused; gives the function its 0x60 byte stack frame (match)
    Hakugek1Data* effect;
    Hakugek1Data* child;
    s32 ang;
    s16 i;
    s16 j;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            ang = ratan2(effect->PartPos.vx - effect->Pos.vx, effect->PartPos.vz - effect->Pos.vz);
            child = &g_BattleEffectSlots[BattleEffectRegister(Hakugek1FirePuffTick)];
            BattleGetPartPosition(effect->unk1E, g_BattleModels[effect->unk1E].boneIndices[11], &child->Pos);
            child->PartPos.vx = 0;
            child->PartPos.vy = ang + 0x400;
            child->PartPos.vz = -0x200;
            child = &g_BattleEffectSlots[BattleEffectRegister(Hakugek1FirePuffTick)];
            BattleGetPartPosition(effect->unk1E, g_BattleModels[effect->unk1E].boneIndices[12], &child->Pos);
            child->PartPos.vx = 0;
            child->PartPos.vy = ang + 0x400;
            child->PartPos.vz = -0x200;
        }
        if (effect->AnimationFrame == 20) {
            i = 0;
            do {
                pos.vx = effect->PartPos.vx + (rcos((i << 16 >> 5) + 0x200) * 800 >> 12);
                pos.vy = effect->PartPos.vy;
                pos.vz = effect->PartPos.vz + (rsin((i << 16 >> 5) + 0x200) * 800 >> 12);
                j = 0;
                do {
                    child = &g_BattleEffectSlots[BattleEffectRegister(Hakugek1DebrisTick)];
                    child->Pos.vx = pos.vx + rand() % 400 - 200;
                    child->Pos.vz = pos.vz + rand() % 400 - 200;
                    child->Pos.vy = 0;
                    Hakugek1SprayVectorCalc(-300, 50, &child->PartPos);
                    child->unk16 = rand() % 9;
                    j++;
                } while (j < 24);
                child = &g_BattleEffectSlots[BattleEffectRegister(Hakugek1ShockwaveTick)];
                child->Pos.vx = pos.vx;
                child->Pos.vy = 0;
                child->Pos.vz = pos.vz;
                j = 0;
                do {
                    child = &g_BattleEffectSlots[BattleEffectRegister(Hakugek1PillarRiseTick)];
                    child->Pos.vx = pos.vx;
                    child->Pos.vy = pos.vy;
                    child->Pos.vz = pos.vz;
                    child->Pos.vy = (j - 4) * 80;
                    child->unk18 = D_801D2A74[j];
                    child->PartPos.vy = (j - 4) * 10;
                    child->unk14 = rand() % 4096;
                    j++;
                } while (j < 5);
                i++;
            } while (i < 2);
            func_800BBA84(0xFA, effect->unk1C, 0);
        }
        if (effect->AnimationFrame == 35) {
            func_800D5774(effect->unk1C);
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 45) {
            effect->StartFrame = -1;
        }
    }
}

void Hakugek1TargetSetup(s32 target, s32 callbackArg) {
    Hakugek1Data* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(Hakugek1TargetTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[0], &effect->Pos);
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->PartPos);
    effect->unk1C = target;
    effect->unk1E = callbackArg;
    func_800BBA84(0x14, callbackArg, 0);
}

void Hakugek1Start(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(Hakugek1DoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B12FC, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 4, Hakugek1TargetSetup);
}
