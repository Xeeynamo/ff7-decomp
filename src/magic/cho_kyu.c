//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "cho_kyu.h"
#include "../battle/battle.h"

u_long* func_800C4FC8();

// Effect shared by Ultima Beam (アルテマビーム), Beam Cannon (ビームキャノン) and Pale Horse (ペイルホース).

extern void* D_80163C74;
extern SpriteRenderDesc D_801D7348;

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
} ChoKyuData; // size:0x20

extern ChoKyuData g_BattleEffectSlots[];
extern ChoKyuData g_BattleDetachedSlots[];
extern MATRIX D_801D746C;
extern ModelRenderDesc D_801D7330;
extern ModelRenderDesc D_801D7364;
extern ModelRenderDesc D_801D738C;
extern ModelRenderDesc D_801D73E8;
extern SVECTOR D_801D7340;
extern SVECTOR D_801D7384;
extern SVECTOR D_801D73F8;
extern SVECTOR D_801D7490;
extern SpriteRenderDesc D_801D7324;
extern SpriteRenderDesc D_801D73B4;
extern SpriteRenderDesc D_801D7438;
extern VECTOR D_801D7354;
extern s16 g_BattleDetachedCursor;
extern s16 D_801D7320;
extern s16 D_801D73E4;
extern s16 D_801D748C;
extern s32 D_801D6F2C;
extern s32 D_801D7164;
extern s32 D_801D2430[];
extern s32 D_801D2A34[];
extern s32* D_801D7374[];
extern u16 D_801031EA;
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;
extern u8 D_801D7498[2][0xC000];
extern u_long D_801B21F0[];
extern void* D_801EF498;
s32 BattleDetachedRegister(void (*func)(void));
void ChoKyuStart(s32 targetMask, s32 callbackArg);
void func_800BBA84(u16 arg0, s32 arg1, s32 arg2);
void ChoKyuViewportFlashTick(void);
void ChoKyuFlashShellTick(void);
void ChoKyuGrowingModelTick(void);
void ChoKyuDriftSpriteTick(void);
void ChoKyuPulseSpriteTick(void);
void ChoKyuBeamColumnTick(void);
void ChoKyuBeamHeadTick(void);
void ChoKyuChargeSparkTick(void);

void MAGIC_ChoKyu(s32 targetMask, s32 callbackArg) { ChoKyuStart(targetMask, callbackArg); }

void ChoKyuTargetCenterCalc(s16 targetMask, SVECTOR* out) {
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

void ChoKyuSprayVectorCalc(s32 offset, s16 spread, SVECTOR* out) {
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

void ChoKyuSpriteMatrixSetup(SVECTOR* pos, SVECTOR* rot, s16 scale, s32 offset) {
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

// Same as EnergyDoubleBufferFlip.
static void ChoKyuDoubleBufferFlip(void) {
    ChoKyuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801EF498 = D_801D7498[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void ChoKyuViewportFlashTick(void) {
    ChoKyuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_80163C74 = func_800C4FC8(0, 0, D_801D7320);
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame < 16) {
            D_801D7320 += 15;
        }
        if (effect->AnimationFrame >= 35) {
            D_801D7320 -= 15;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 50) {
            effect->StartFrame = -1;
        }
    }
}

void ChoKyuSparkSpriteTick(void) {
    ChoKyuData* effect;

    effect = &g_BattleDetachedSlots[g_BattleDetachedCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2800, 0);
    D_801D7324.frameIndex = effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801D7324, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801D6F2C) {
            effect->StartFrame = -1;
        }
    }
}

void ChoKyuFlashShellTick(void) {
    MATRIX matrix;
    MATRIX rotated;
    s32 flag;
    ChoKyuData* effect;
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
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x1000 + effect->AnimationFrame * 800;
    RotMatrixYXZ(&D_801D7340, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetFarColor(0, 0, 0);
    D_801D7330.color = effect->AnimationFrame * 0x100;
    D_801EF498 = func_800D29D4(&D_801D7330, g_cDb->unk70, 12, D_801EF498);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

// Same as func_801B0584.
void ChoKyuEmberTick(void) {
    MATRIX rotated;
    MATRIX matrix;
    SVECTOR rot;
    s32 flag;
    ChoKyuData* effect;
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
    D_801D7348.frameIndex = effect->unk16;
    D_801D7348.color.cd = fade == 8 ? 0x2C : 0x2E;
    D_801D7348.color.r = D_801D7348.color.g = D_801D7348.color.b = fade << 4;
    D_80163C74 = func_800D4D90(&D_801D7348, (u_long**)&g_cDb->unk70, 12, D_80163C74);
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

void ChoKyuRisingModelTick(void) {
    MATRIX matrix;
    MATRIX composed;
    SVECTOR rot;
    ChoKyuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vy = 0;
    rot.vx = 0;
    rot.vz = effect->unk14;
    RotMatrix(&rot, &matrix);
    ScaleMatrix(&matrix, &D_801D7354);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D7364.color = 0x80 - effect->AnimationFrame * 6;
    D_801D7364.model = D_801D7374[effect->unk16];
    D_801EF498 = func_800D29D4(&D_801D7364, g_cDb->unk70, 12, D_801EF498);
    if (D_80062D98 == 0) {
        effect->Pos.vy -= 200;
        effect->unk14 += 0x100;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void ChoKyuGrowingModelTick(void) {
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    ChoKyuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrixYXZ(&D_801D7384, &matrix);
    MulMatrix0(&D_801D746C, &matrix, &matrix);
    scale.vx = scale.vy = scale.vz = effect->AnimationFrame * 1500;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    SetFarColor(0, 0, 0);
    D_801D738C.color = effect->AnimationFrame * 0x100;
    D_801EF498 = func_800D29D4(&D_801D738C, g_cDb->unk70, 12, D_801EF498);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

extern SVECTOR D_801D739C;
extern ModelRenderDesc D_801D73A4;

void ChoKyuShrinkingModelTick(void) {
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    ChoKyuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (effect->AnimationFrame >= 3) {
        RotMatrixYXZ(&D_801D739C, &matrix);
        MulMatrix0(&D_801D746C, &matrix, &matrix);
        scale.vx = scale.vy = scale.vz = 0x4000 - effect->AnimationFrame * 400;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = effect->Pos.vx;
        matrix.t[1] = effect->Pos.vy;
        matrix.t[2] = effect->Pos.vz;
        CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
        SetRotMatrix(&composed);
        SetTransMatrix(&composed);
        D_801D73A4.color = effect->AnimationFrame * 200;
        SetFarColor(0, 0, 0);
        D_801EF498 = func_800D29D4(&D_801D73A4, g_cDb->unk70, 12, D_801EF498);
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void ChoKyuDriftSpriteTick(void) {
    ChoKyuData* effect;

    effect = &g_BattleDetachedSlots[g_BattleDetachedCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x6000, 0);
    D_801D73B4.frameIndex = effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801D73B4, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->Pos.vx += effect->PartPos.vx;
        effect->Pos.vy += effect->PartPos.vy;
        effect->Pos.vz += effect->PartPos.vz;
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801D7164) {
            effect->StartFrame = -1;
        }
    }
}

extern SpriteRenderDesc D_801D73C0;
extern ModelRenderDesc D_801D73CC;
extern SVECTOR D_801D73DC;
extern VECTOR D_801D7458;
extern s32 D_801D61A8[];
extern s32 D_801D684C[];

void ChoKyuPulseSpriteTick(void) {
    MATRIX matrix;
    MATRIX composed;
    ChoKyuData* effect;
    s32 scale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    scale = ((rsin(effect->AnimationFrame * 51) << 11) >> 16) * 0x80 + 0x1000;
    BattleSetBillboardMatrix(&effect->Pos, (s16)scale, 0);
    D_801D73C0.frameIndex = effect->AnimationFrame % 2;
    D_801EF498 = func_800D4D90(&D_801D73C0, (u_long**)&g_cDb->unk70, 12, D_801EF498);
    D_801D7458.vx = D_801D7458.vy = D_801D7458.vz = scale;
    D_801D73DC.vz = effect->AnimationFrame * 0x100;
    RotMatrixYXZ(&D_801D73DC, &matrix);
    MulMatrix0(&D_801D746C, &matrix, &matrix);
    ScaleMatrix(&matrix, &D_801D7458);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D73CC.model = D_801D61A8;
    SetFarColor(0, 0, 0);
    D_801EF498 = func_800D29D4(&D_801D73CC, g_cDb->unk70, 12, D_801EF498);
    D_801D73DC.vz = -effect->AnimationFrame * 0x100;
    RotMatrixYXZ(&D_801D73DC, &matrix);
    MulMatrix0(&D_801D746C, &matrix, &matrix);
    ScaleMatrix(&matrix, &D_801D7458);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D73CC.model = D_801D684C;
    D_801EF498 = func_800D29D4(&D_801D73CC, g_cDb->unk70, 12, D_801EF498);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 40) {
            effect->StartFrame = -1;
        }
    }
}

void ChoKyuBeamColumnTick(void) {
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    ChoKyuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrixYXZ(&D_801D73F8, &matrix);
    MulMatrix0(&D_801D746C, &matrix, &matrix);
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
    D_801D73E8.color = 0x1000 - D_801D73E4 * 0x100;
    D_801D73E8.model = D_801D2430;
    D_801EF498 = func_800D29D4(&D_801D73E8, g_cDb->unk70, 12, D_801EF498);
    D_801D73E8.model = D_801D2A34;
    D_801EF498 = func_800D29D4(&D_801D73E8, g_cDb->unk70, 12, D_801EF498);
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame >= 13) {
            D_801D73E4 -= 1;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

extern s16 D_801D7400;
extern VECTOR D_801D7404;
extern ModelRenderDesc D_801D7414;
extern SpriteRenderDesc D_801D7424;
extern SVECTOR D_801D7430;
extern s16 D_801D7468;
extern s32 D_801D3038[];
extern s32 D_801D3FDC[];
extern s32 D_801D4F80[];

void ChoKyuBeamHeadTick(void) {
    MATRIX matrix;
    MATRIX composed;
    SVECTOR pos;
    ChoKyuData* effect;
    ChoKyuData* child;
    s32 flag;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrixYXZ(&D_801D7430, &matrix);
    MulMatrix0(&D_801D746C, &matrix, &matrix);
    ScaleMatrix(&matrix, &D_801D7404);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D7414.color = D_801D7400 << 4;
    D_801D7414.model = D_801D3038;
    D_801EF498 = func_800D29D4(&D_801D7414, g_cDb->unk70, 12, D_801EF498);
    D_801D7414.model = D_801D3FDC;
    D_801EF498 = func_800D29D4(&D_801D7414, g_cDb->unk70, 12, D_801EF498);
    D_801D7414.model = D_801D4F80;
    D_801EF498 = func_800D29D4(&D_801D7414, g_cDb->unk70, 12, D_801EF498);
    BattleSetBillboardMatrix(&effect->Pos, 0x4000, 0);
    D_801D7424.color.r = D_801D7424.color.g = D_801D7424.color.b = D_801D7400 * 20;
    D_801D7424.frameIndex = effect->AnimationFrame % 2;
    D_801EF498 = func_800D4D90(&D_801D7424, (u_long**)&g_cDb->unk70, 12, D_801EF498);
    if (D_80062D98 == 0) {
        effect->Pos.vx += effect->PartPos.vx;
        effect->Pos.vy += effect->PartPos.vy;
        effect->Pos.vz += effect->PartPos.vz;
        D_801D746C.t[0] = effect->Pos.vx;
        D_801D746C.t[1] = effect->Pos.vy;
        D_801D746C.t[2] = effect->Pos.vz;
        SetRotMatrix(&D_801D746C);
        SetTransMatrix(&D_801D746C);
        D_801D7468 = rand() % 4096;
        pos.vx = (u32)(rcos(D_801D7468) * 75) >> 8;
        pos.vy = (u32)(rsin(D_801D7468) * 75) >> 8;
        pos.vz = 0;
        RotTransSV(&pos, &pos, &flag);
        BattleSpawnFloatingIconAt(&pos, 0x44, 0);
        if ((effect->AnimationFrame & 1) == 0) {
            child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuShrinkingModelTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
        }
        if (effect->Pos.vy < 0) {
            child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuEmberTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vz = effect->Pos.vz;
            child->Pos.vy = 0;
            D_801D7468 = rand() % 4096;
            child->PartPos.vx = (u32)(rcos(D_801D7468) * 15) >> 9;
            child->PartPos.vz = (u32)(rsin(D_801D7468) * 15) >> 9;
            child->PartPos.vy = -rand() % 300 - 50;
            child->unk16 = rand() % 7;
            if (effect->AnimationFrame % 3 == 0) {
                child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuRisingModelTick)];
                child->Pos.vx = effect->Pos.vx + rand() % 4000 - 2000;
                child->Pos.vz = effect->Pos.vz + rand() % 4000 - 2000;
                child->Pos.vy = 0;
                child->unk16 = rand() % 4;
            }
        }
        if (effect->AnimationFrame >= 13) {
            D_801D7400--;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void ChoKyuChargeSparkTick(void) {
    SVECTOR pos;
    s32 flag;
    ChoKyuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D746C.t[0] = effect->Pos.vx;
    D_801D746C.t[1] = effect->Pos.vy;
    D_801D746C.t[2] = effect->Pos.vz;
    SetRotMatrix(&D_801D746C);
    SetTransMatrix(&D_801D746C);
    RotTransSV(&effect->PartPos, &pos, &flag);
    BattleSetBillboardMatrix(&pos, 0x2000, 0);
    D_801D7438.frameIndex = effect->AnimationFrame % 2;
    D_801EF498 = func_800D4D90(&D_801D7438, (u_long**)&g_cDb->unk70, 12, D_801EF498);
    if (D_80062D98 == 0) {
        effect->PartPos.vx += -effect->PartPos.vx / 4;
        effect->PartPos.vy += -effect->PartPos.vy / 4;
        effect->PartPos.vz -= 200;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 10) {
            effect->StartFrame = -1;
        }
    }
}

void ChoKyuMainTick(void) {
    SVECTOR in;
    SVECTOR rot;
    MATRIX matrix;
    s32 flag;
    ChoKyuData* effect;
    ChoKyuData* child;
    s16 i;
    s16 angle;
    u16* u16p;
    u16 t;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuGrowingModelTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuPulseSpriteTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuFlashShellTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            func_800BBA84(0x18B, effect->unk1E, 0x40);
        }
        if (effect->AnimationFrame < 16) {
            for (i = 0; i < 4; i++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuChargeSparkTick)];
                child->Pos.vx = effect->Pos.vx;
                child->Pos.vy = effect->Pos.vy;
                child->Pos.vz = effect->Pos.vz;
                angle = rand() % 4096;
                child->PartPos.vx = (u32)(rcos(angle) * 125) >> 8;
                child->PartPos.vy = (u32)(rsin(angle) * 125) >> 8;
                child->PartPos.vz = 2000;
            }
        }
        if (effect->AnimationFrame == 20) {
            child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuBeamColumnTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuBeamHeadTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child->PartPos.vx = effect->PartPos.vx;
            child->PartPos.vy = effect->PartPos.vy;
            child->PartPos.vz = effect->PartPos.vz;
            func_800BBA84(0x176, effect->unk1E, 0x40);
        }
        if (effect->AnimationFrame == 20) {
            BattleEffectRegister(ChoKyuViewportFlashTick);
        }
        if (effect->AnimationFrame == 54) {
            BattleFixedPointRampUpdateInit(0, 16);
        }
        if (effect->AnimationFrame >= 37 && effect->AnimationFrame < 44) {
            for (i = 0; i < 10; i++) {
                if ((D_801D748C >> i) & 1) {
                    child = &g_BattleDetachedSlots[BattleDetachedRegister(ChoKyuDriftSpriteTick)];
                    BattleGetPartPosition(i, g_BattleModels[i].boneIndices[0], &child->Pos);
                    in.vy = 0x64;
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
                if ((D_801D748C >> i) & 1) {
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

void ChoKyuStart(s32 targetMask, s32 callbackArg) {
    VECTOR vec;
    SVECTOR dir;
    ChoKyuData* effect;

    BattleEffectRegister(ChoKyuDoubleBufferFlip);
    D_801D748C = targetMask;
    effect = &g_BattleEffectSlots[BattleEffectRegister(ChoKyuMainTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[15], &effect->Pos);
    ChoKyuTargetCenterCalc(targetMask, &D_801D7490);
    vec.vx = dir.vx = D_801D7490.vx - effect->Pos.vx;
    vec.vy = dir.vy = D_801D7490.vy - effect->Pos.vy;
    vec.vz = dir.vz = D_801D7490.vz - effect->Pos.vz;
    BattleMatrixFromDirection(&dir, &D_801D746C);
    VectorNormal(&vec, &vec);
    effect->PartPos.vx = (u32)(vec.vx * 75) >> 9;
    effect->PartPos.vy = (u32)(vec.vy * 75) >> 9;
    effect->PartPos.vz = (u32)(vec.vz * 75) >> 9;
    effect->unk1E = callbackArg;
    BattleSetLoadTimToVram(D_801B21F0, 0, 0, 0);
    D_800F5B70 = D_800F5B71 = D_800F5B72 = 0;
    BattleFixedPointRampUpdateInit(0x1000, 0x10);
}
