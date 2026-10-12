//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "kaen.h"
#include "../battle/battle.h"

// Effect shared by Fire Veil (火とん), Fireball (ファイアボール), Flame (火炎), Ghenghana (ゲヘナガナー)
// and Turk Light (タークス光線).

void func_801B0F6C(s32 targetMask, s32 callbackArg);

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 g_KaenPrimBuffer[2][0xC000];
extern void* g_KaenBufferPtr;

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
} KaenData; // size:0x20

extern ModelRenderDesc D_801D3EF4;
extern SVECTOR D_801D3EEC;
extern SpriteAnim* D_801D3ED4[];
extern SpriteRenderDesc D_801D3EC8;
extern SpriteRenderDesc D_801D3EE0;
extern SpriteRenderDesc D_801D3F04;
extern VECTOR D_801B0000;
extern KaenData g_BattleEffectSlots[];
extern KaenData g_BattleDetachedSlots[];
extern s16 g_BattleDetachedCursor;
extern s32* D_801D3F10[];
extern u_long D_801B0FE8[];
extern void* D_80163C74;
s32 BattleDetachedRegister(void (*func)(void));
void func_800BBA84(u16 arg0, s32 arg1, s32 arg2);
void KaenTargetTick(void);

// Same as MAGIC_Biora.
void MAGIC_Kaen(s32 targetMask, s32 callbackArg) { func_801B0F6C(targetMask, callbackArg); }

void KaenTargetCenterCalc(s16 targetMask, SVECTOR* out) {
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

void KaenSprayVectorCalc(s32 offset, s16 spread, SVECTOR* out) {
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

void KaenSpriteMatrixSetup(SVECTOR* pos, SVECTOR* rot, s16 scale, s32 offset) {
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

// Same as BioraDoubleBufferFlip.
static void KaenDoubleBufferFlip(void) {
    KaenData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_KaenBufferPtr = g_KaenPrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void KaenFlameSpriteTick(void) {
    KaenData* effect;

    effect = &g_BattleDetachedSlots[g_BattleDetachedCursor];
    BattleSetBillboardMatrix(&effect->Pos, effect->unk16 == 2 ? 0x2000 : 0x3000, 0);
    D_801D3EC8.frames = D_801D3ED4[effect->unk16];
    D_801D3EC8.frameIndex = effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801D3EC8, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == D_801D3ED4[effect->unk16]->frameCount) {
            effect->StartFrame = -1;
        }
    }
}

void KaenEmberTick(void) {
    MATRIX rotated;
    MATRIX matrix;
    SVECTOR rot;
    s32 flag;
    KaenData* effect;
    s16 fade;
    s16 scale;
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
    scale = 0x2000;
    if (effect->unk16 < 4) {
        scale = 0x1400;
    }
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = scale;
    RotMatrixYXZ(&rot, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    D_801D3EE0.frameIndex = effect->unk16;
    D_801D3EE0.color.cd = fade == 8 ? 0x2C : 0x2E;
    D_801D3EE0.color.r = D_801D3EE0.color.g = D_801D3EE0.color.b = fade << 4;
    D_80163C74 = func_800D4D90(&D_801D3EE0, (u_long**)&g_cDb->unk70, 12, D_80163C74);
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

void KaenFireballTick(void) {
    MATRIX dirMatrix;
    MATRIX matrix;
    MATRIX composed;
    s32 pad[2]; // match: unused, gives the original stack frame
    VECTOR scale;
    KaenData* effect;
    KaenData* child;

    scale = D_801B0000;
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleMatrixFromDirection(&effect->PartPos, &dirMatrix);
    RotMatrixYXZ(&D_801D3EEC, &matrix);
    MulMatrix0(&dirMatrix, &matrix, &matrix);
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D3EF4.model = D_801D3F10[effect->AnimationFrame % 8];
    g_KaenBufferPtr = func_800D29D4(&D_801D3EF4, g_cDb->unk70, 12, g_KaenBufferPtr);
    BattleSetBillboardMatrix(&effect->Pos, 0x1000, 0);
    D_80163C74 = func_800D4D90(&D_801D3F04, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->Pos.vx += effect->PartPos.vx;
        effect->Pos.vy += effect->PartPos.vy;
        effect->Pos.vz += effect->PartPos.vz;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 8) {
            child = &g_BattleDetachedSlots[BattleDetachedRegister(KaenFlameSpriteTick)];
            child->Pos.vx = effect->Pos.vx + rand() % 800 - 400;
            child->Pos.vy = effect->Pos.vy + rand() % 800 - 400;
            child->Pos.vz = effect->Pos.vz + rand() % 800 - 400;
            child->unk16 = 0;
            effect->StartFrame = -1;
        }
    }
}

void KaenTargetTick(void) {
    s32 pad[12]; // match: unused, gives the original stack frame
    KaenData* effect;
    KaenData* child;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            child = &g_BattleDetachedSlots[BattleDetachedRegister(KaenFlameSpriteTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child->unk16 = 2;
            child = &g_BattleEffectSlots[BattleEffectRegister(KaenFireballTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child->PartPos.vx = (effect->PartPos.vx - effect->Pos.vx) / 8;
            child->PartPos.vy = (effect->PartPos.vy - effect->Pos.vy) / 8;
            child->PartPos.vz = (effect->PartPos.vz - effect->Pos.vz) / 8;
        }
        if (effect->AnimationFrame == 8) {
            i = 0;
            do {
                child = &g_BattleEffectSlots[BattleEffectRegister(KaenEmberTick)];
                child->Pos.vx = effect->PartPos.vx + rand() % 400 - 200;
                child->Pos.vz = effect->PartPos.vz + rand() % 400 - 200;
                child->Pos.vy = 0;
                KaenSprayVectorCalc(-250, 100, &child->PartPos);
                child->unk16 = rand() % 20;
                i++;
            } while (i < 8);
            func_800BBA84(0xFA, effect->unk1C, 0);
        }
        if (effect->AnimationFrame >= 12 && effect->AnimationFrame < 19) {
            child = &g_BattleDetachedSlots[BattleDetachedRegister(KaenFlameSpriteTick)];
            child->Pos.vx = effect->PartPos.vx + rand() % 1200 - 600;
            child->Pos.vy = effect->PartPos.vy + rand() % 1200 - 600;
            child->Pos.vz = effect->PartPos.vz + rand() % 1200 - 600;
            child->unk16 = 1;
        }
        if (effect->AnimationFrame == 15) {
            func_800D5774(effect->unk1C);
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 30) {
            effect->StartFrame = -1;
        }
    }
}

void KaenTargetSetup(s32 target, s32 callbackArg) {
    KaenData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(KaenTargetTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[8], &effect->Pos);
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->PartPos);
    effect->unk1C = target;
}

void func_801B0F6C(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(KaenDoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B0FE8, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 4, KaenTargetSetup);
    func_800BBA84(0x163, callbackArg, 0);
}
