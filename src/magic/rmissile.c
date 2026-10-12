//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "rmissile.h"
#include "../battle/battle.h"

// Effect RMISSILE.

extern void* D_80163C74;
extern SpriteRenderDesc D_801D47DC;

extern u_long D_801B178C[];
void RmissileTargetSetup(s32 target, s32 callbackArg);

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
} RmissileData; // size:0x20

extern SpriteRenderDesc D_801D47C4;
extern SpriteRenderDesc D_801D47D0;
extern RmissileData g_BattleEffectSlots[];
extern RmissileData g_BattleDetachedSlots[];
extern s16 g_BattleDetachedCursor;
extern s32 D_801D4638;
extern s32 D_801D4700;
extern u8 D_801D4850[2][0xC000];
extern void* D_801EC850;
void RmissileStart(s32 targetMask, s32 callbackArg);
void RmissileBlastTick(void);
void RmissileMissileTick(void);
void RmissileTargetTick(void);

void MAGIC_Rmissile(s32 targetMask, s32 callbackArg) { RmissileStart(targetMask, callbackArg); }

void RmissileTargetCenterCalc(s16 targetMask, SVECTOR* out) {
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

void RmissileSprayVectorCalc(s32 offset, s16 spread, SVECTOR* out) {
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

void RmissileSpriteMatrixSetup(SVECTOR* pos, SVECTOR* rot, s16 scale, s32 offset) {
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
static void RmissileDoubleBufferFlip(void) {
    RmissileData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801EC850 = D_801D4850[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void RmissileSparkSpriteTick(void) {
    RmissileData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2000, 0);
    D_801D47C4.frameIndex = effect->AnimationFrame;
    D_801EC850 = func_800D4D90(&D_801D47C4, (u_long**)&g_cDb->unk70, 12, D_801EC850);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801D4700) {
            effect->StartFrame = -1;
        }
    }
}

void RmissileDustSpriteTick(void) {
    RmissileData* effect;

    effect = &g_BattleDetachedSlots[g_BattleDetachedCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2800, 0);
    D_801D47D0.frameIndex = effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801D47D0, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801D4638) {
            effect->StartFrame = -1;
        }
    }
}

// Same as RmissileDustSpriteTick.
void RmissileDebrisTick(void) {
    MATRIX rotated;
    MATRIX matrix;
    SVECTOR rot;
    s32 flag;
    RmissileData* effect;
    s16 fade;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    for (i = 0; i < 3; i++) {
        matrix.m[2][i] = 0;
        matrix.m[1][i] = 0;
        matrix.m[0][i] = 0;
    }
    fade = 8;
    if (effect->AnimationFrame >= 22) {
        fade = 30 - effect->AnimationFrame;
    }
    rot.vy = 0;
    rot.vx = 0;
    rot.vz = effect->unk16;
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(&effect->Pos, (VECTOR*)matrix.t, &flag);
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x2000;
    RotMatrixYXZ(&rot, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    D_801D47DC.frameIndex = effect->unk18;
    D_801D47DC.color.cd = fade == 8 ? 0x2C : 0x2E;
    D_801D47DC.color.r = D_801D47DC.color.g = D_801D47DC.color.b = fade << 4;
    D_80163C74 = func_800D4D90(&D_801D47DC, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->Pos.vx += effect->PartPos.vx;
        effect->Pos.vy += effect->PartPos.vy;
        effect->Pos.vz += effect->PartPos.vz;
        if (effect->Pos.vy > 0) {
            effect->Pos.vy = 0;
            effect->PartPos.vy *= -1;
        }
        effect->PartPos.vy += 0x14;
        effect->unk16 += 0x200;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x1E) {
            effect->StartFrame = -1;
        }
    }
}

extern SVECTOR D_801D47E8;
extern ModelRenderDesc D_801D47F0;
extern ModelRenderDesc D_801D4804;
extern VECTOR D_801D4814;
extern ModelRenderDesc D_801D4824;
extern SpriteRenderDesc D_801D4834;
extern SpriteRenderDesc D_801D4840;
extern s32 D_801D44A0;
extern u16 D_801D4800;
extern u16 D_801031EA;
extern s32 D_801D2524[];
extern s32 D_801D42C8[];

// Same as func_801B07A0.
void RmissileFlashModelTick(void) {
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    RmissileData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrix(&D_801D47E8, &matrix);
    scale.vx = scale.vy = scale.vz = effect->AnimationFrame * 1500;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D47F0.color = effect->AnimationFrame * 256;
    D_801EC850 = func_800D29D4(&D_801D47F0, g_cDb->unk70, 12, D_801EC850);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 15) {
            effect->StartFrame = -1;
        }
    }
}

void RmissileBlastTick(void) {
    MATRIX matrix;
    MATRIX composed;
    SVECTOR rot;
    s32 pad[6]; // frame size only, needed for the match
    VECTOR scale;
    RmissileData* effect;
    RmissileData* chip;
    s32 radius;
    s16 i;
    s16 angle;
    u16* u16p;
    s32 t;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vy = effect->AnimationFrame * 64;
    rot.vx = rot.vz = 0;
    RotMatrix(&rot, &matrix);
    scale.vx = scale.vy = scale.vz = effect->AnimationFrame * 300 + 0x1000;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D4804.flags = 0x28;
    D_801D4804.model = D_801D2524;
    D_801D4804.color = D_801D4800 << 3;
    D_801EC850 = func_800D29D4(&D_801D4804, g_cDb->unk70, 12, D_801EC850);
    D_801D4804.flags = 0xA8;
    D_801D4804.model = D_801D42C8;
    D_801D4804.color = 0x1000 - (s16)D_801D4800 * 512;
    D_801EC850 = func_800D29D4(&D_801D4804, g_cDb->unk70, 12, D_801EC850);
    rot.vy = -effect->AnimationFrame * 64;
    rot.vx = rot.vz = 0;
    RotMatrix(&rot, &matrix);
    scale.vx = scale.vy = scale.vz = effect->AnimationFrame * 280 + 0x800;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D4804.flags = 0x28;
    D_801D4804.model = D_801D2524;
    D_801D4804.color = D_801D4800 << 3;
    D_801EC850 = func_800D29D4(&D_801D4804, g_cDb->unk70, 12, D_801EC850);
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame < 0x16) {
            for (i = 0, radius = (s16)(effect->AnimationFrame * 5 << 4); i < 6; i++) {
                angle = rand() % 0x1000;
                chip = &g_BattleDetachedSlots[BattleDetachedRegister(RmissileDustSpriteTick)];
                chip->Pos.vx = effect->Pos.vx + (rcos(angle) * radius >> 12);
                chip->Pos.vz = effect->Pos.vz + (rsin(angle) * radius >> 12);
                chip->Pos.vy = -0x96;
                chip->unk18 = 2;
            }
        }
        u16p = &D_801031EA;
        t = *u16p - 8;
        *u16p = t + (effect->AnimationFrame % 2) * 16;
        if (effect->AnimationFrame >= 0xF) {
            D_801D4800--;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x1E) {
            effect->StartFrame = -1;
        }
    }
}

void RmissileMissileTick(void) {
    MATRIX matrix;
    MATRIX composed;
    SVECTOR rot;
    SVECTOR pos;
    SVECTOR ring;
    SVECTOR step;
    long flag;
    long xy;
    RmissileData* effect;
    TILE_1* prim;
    s16 i;
    long depth;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vx = -0x400;
    rot.vy = -0x400;
    rot.vz = 0;
    RotMatrix(&rot, &matrix);
    RotMatrixY(effect->unk14, &matrix);
    ScaleMatrix(&matrix, &D_801D4814);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801EC850 = func_800D29D4(&D_801D4824, g_cDb->unk70, 12, D_801EC850);
    rot.vy = effect->unk14;
    rot.vz = 0;
    rot.vx = 0;
    pos.vy = 0;
    pos.vx = 0;
    pos.vz = -0x64;
    RotMatrixYXZ(&rot, &matrix);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    RotTransSV(&pos, &pos, &flag);
    BattleSetBillboardMatrix(&pos, 0x2000, 0);
    D_801D4834.frameIndex = effect->AnimationFrame % D_801D44A0;
    D_801EC850 = func_800D4D90(&D_801D4834, (u_long**)&g_cDb->unk70, 12, D_801EC850);
    if (effect->AnimationFrame >= 6) {
        ring.vx = pos.vx;
        ring.vy = pos.vy;
        ring.vz = pos.vz;
        step.vx = -rsin(effect->unk14) * ((effect->AnimationFrame % 2) * 80 + 0x50) >> 12;
        step.vz = -rcos(effect->unk14) * ((effect->AnimationFrame % 2) * 80 + 0x50) >> 12;
        for (i = 0; i < 12; i++) {
            BattleSetBillboardMatrix(&ring, 0x2000, 0);
            D_801D4840.frameIndex = i >> 1;
            D_801EC850 = func_800D4D90(&D_801D4840, (u_long**)&g_cDb->unk70, 12, D_801EC850);
            ring.vx += step.vx;
            ring.vz += step.vz;
        }
    }
    if (D_80062D98 == 0) {
        SetRotMatrix(&g_BattleWorldView.m);
        SetTransMatrix(&g_BattleWorldView.m);
        for (i = 0; i < 10; i++) {
            ring.vx = pos.vx + rand() % 400 - 200;
            ring.vy = pos.vy + rand() % 400 - 200;
            ring.vz = pos.vz + rand() % 400 - 200;
            prim = D_801EC850;
            SetTile1(prim);
            prim->code = 0x6A;
            depth = RotTransPers(&ring, (long*)&prim->x0, &xy, &flag);
            if (depth > 0) {
                prim->r0 = 0x80;
                prim->g0 = 0x40;
                prim->b0 = 0x20;
                AddPrim(&g_cDb->unk70[depth >> 2], prim);
                D_801EC850 = prim + 1;
            }
        }
        if (effect->AnimationFrame < 6) {
            effect->Pos.vx += effect->PartPos.vx >> 2;
            effect->Pos.vy += (effect->PartPos.vy >> 2) + effect->unk1A;
            effect->Pos.vz += effect->PartPos.vz >> 2;
            effect->unk1A -= 5;
        }
        if (effect->AnimationFrame >= 6) {
            effect->Pos.vx += effect->PartPos.vx;
            effect->Pos.vy += effect->PartPos.vy - 10;
            effect->Pos.vz += effect->PartPos.vz;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x14) {
            effect->StartFrame = -1;
        }
    }
}

void RmissileTargetTick(void) {
    s32 pad[14]; // frame size only, needed for the match
    RmissileData* effect;
    RmissileData* next;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame < 6 && effect->AnimationFrame % 5 == 0) {
            next = &g_BattleEffectSlots[BattleEffectRegister(RmissileMissileTick)];
            next->Pos.vx = effect->Pos.vx;
            next->Pos.vy = effect->Pos.vy - effect->AnimationFrame * 30;
            next->Pos.vz = effect->Pos.vz;
            next->unk14 = ratan2(effect->PartPos.vx - effect->Pos.vx, effect->PartPos.vz - effect->Pos.vz);
            next->PartPos.vx = (effect->PartPos.vx - effect->Pos.vx) / 15;
            next->PartPos.vy = (effect->PartPos.vy - effect->Pos.vy) / 15;
            next->PartPos.vz = (effect->PartPos.vz - effect->Pos.vz) / 15;
            next->unk1A = 0x32;
            func_800BBA84(0x82, effect->unk1E, 0x40);
        }
        if (effect->AnimationFrame == 0x32) {
            func_800D5774(effect->unk1C);
        }
        if (effect->AnimationFrame == 0x14) {
            next = &g_BattleEffectSlots[BattleEffectRegister(RmissileBlastTick)];
            next->Pos.vx = effect->PartPos.vx;
            next->Pos.vy = 0;
            next->Pos.vz = effect->PartPos.vz;
            next->unk18 = 0;
            next = &g_BattleEffectSlots[BattleEffectRegister(RmissileFlashModelTick)];
            next->Pos.vx = effect->PartPos.vx;
            next->Pos.vy = -0x12C;
            next->Pos.vz = effect->PartPos.vz;
            for (i = 0; i < 20; i++) {
                next = &g_BattleEffectSlots[BattleEffectRegister(RmissileDebrisTick)];
                next->Pos.vx = effect->PartPos.vx;
                next->Pos.vz = effect->PartPos.vz;
                next->Pos.vy = 0;
                RmissileSprayVectorCalc(-0xFA, 0x64, &next->PartPos);
                next->unk18 = rand() % 7;
            }
            func_800BBA84(0x176, effect->unk1C, 0x40);
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x3C) {
            effect->StartFrame = -1;
        }
    }
}

void RmissileTargetSetup(s32 target, s32 callbackArg) {
    RmissileData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(RmissileTargetTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[9], &effect->Pos);
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->PartPos);
    effect->unk1C = target;
    effect->unk1E = callbackArg;
}

// Same as func_801B1990.
void RmissileStart(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(RmissileDoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B178C, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 4, RmissileTargetSetup);
}
