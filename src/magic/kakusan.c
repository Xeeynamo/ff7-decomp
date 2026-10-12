//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "kakusan.h"
#include "../battle/battle.h"

// Effect shared by Molotov Cocktail (モロトフカクテル) and an unnamed attack (エサ喰い).

void KakusanStart(s32 targetMask, s32 callbackArg);

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 g_KakusanPrimBuffer[2][0xC000];
extern void* g_KakusanBufferPtr;

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
    /* 0x1A */ char pad1A[2];
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
} KakusanData; // size:0x20

extern MATRIX D_801E98F4;
extern ModelRenderDesc D_801D1890;
extern SVECTOR D_801D18B4;
extern SVECTOR D_801D18D8;
extern SVECTOR D_801E98EC;
extern SpriteAnim D_801D14EC;
extern SpriteAnim D_801D1674;
extern SpriteRenderDesc D_801D1804;
extern SpriteRenderDesc D_801D1810;
extern SpriteRenderDesc D_801D1820;
extern SpriteRenderDesc D_801D1884;
extern SpriteRenderDesc D_801D18CC;
extern VECTOR D_801D182C;
extern VECTOR D_801D18BC;
extern KakusanData g_BattleEffectSlots[];
extern KakusanData g_BattleDetachedSlots[];
extern s16 g_BattleDetachedCursor;
extern s16 D_801E98E8;
extern s16 D_801E9914;
extern s32 D_801D1340;
extern s32 D_801D1740;
extern s32 D_801D181C;
extern s32* D_801D18A0[];
extern u16 D_801D18B0;
extern u16* D_801D187C[];
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;
extern u_long D_801B1324[];
extern void* D_80163C74;
void KakusanTargetCenterCalc(s16 targetMask, SVECTOR* out);
void func_800BBA84(u16 arg0, s32 arg1, s32 arg2);
void KakusanMainTick(void);

// Same as MAGIC_Biora.
void MAGIC_Kakusan(s32 targetMask, s32 callbackArg) { KakusanStart(targetMask, callbackArg); }

void KakusanTargetCenterCalc(s16 targetMask, SVECTOR* out) {
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

void KakusanSprayVectorCalc(s32 offset, s16 spread, SVECTOR* out) {
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

void KakusanSpriteMatrixSetup(SVECTOR* pos, SVECTOR* rot, s16 scale, s32 offset) {
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
static void KakusanDoubleBufferFlip(void) {
    KakusanData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_KakusanBufferPtr = g_KakusanPrimBuffer[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void KakusanFlashSpriteTick(void) {
    KakusanData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2800, 0);
    D_801D1804.frameIndex = effect->AnimationFrame;
    g_KakusanBufferPtr = func_800D4D90(&D_801D1804, (u_long**)&g_cDb->unk70, 12, g_KakusanBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == D_801D1740) {
            effect->StartFrame = -1;
        }
    }
}

void func_801B0580(void) {
    MATRIX matrix;
    MATRIX rotated;
    s32 flag;
    KakusanData* effect;
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
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x2000;
    MulMatrix(&matrix, &g_BattleWorldView.m);
    RotMatrixYXZ(&effect->PartPos, &rotated);
    MulMatrix(&matrix, &rotated);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    D_801D1810.color.r = D_801D1810.color.g = D_801D1810.color.b = (u8)D_801D181C * 16;
    D_801D1810.frameIndex = effect->AnimationFrame % 3;
    g_KakusanBufferPtr = func_800D4D90(&D_801D1810, (u_long**)&g_cDb->unk70, 12, g_KakusanBufferPtr);
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame >= 33) {
            D_801D181C--;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 40) {
            effect->StartFrame = -1;
        }
    }
}

void func_801B075C(void) {
    // pad0 and pad1 are unused; they only reproduce the original 0x98 stack frame.
    VECTOR pad0;
    MATRIX matrix;
    MATRIX pad1;
    MATRIX composed;
    KakusanData* effect;
    s16 fade;
    s16 limit;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    fade = 8;
    if (effect->AnimationFrame >= 32) {
        fade = 40 - effect->AnimationFrame;
    }
    RotMatrix(&effect->PartPos, &matrix);
    MulMatrix0(&D_801E98F4, &matrix, &matrix);
    D_801D182C.vy = effect->unk18;
    D_801D182C.vx = D_801D182C.vz = 0x3000;
    ScaleMatrix(&matrix, &D_801D182C);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D1820.frames = effect->unk16 == 0 ? &D_801D14EC : &D_801D1674;
    D_801D1820.frameIndex = D_801D187C[effect->unk16][effect->unk14];
    D_801D1820.color.r = D_801D1820.color.g = D_801D1820.color.b = ((u32)fade << 16) >> 12;
    D_80163C74 = func_800D4D90(&D_801D1820, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        limit = 12;
        if (effect->unk16 == 0) {
            limit = 20;
        }
        if (++effect->unk14 == limit) {
            effect->unk14 -= 4;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 40) {
            effect->StartFrame = -1;
        }
    }
}

void func_801B0958(void) {
    MATRIX composed;
    MATRIX matrix;
    KakusanData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrixYXZ(&D_801D18B4, &matrix);
    ScaleMatrix(&matrix, &D_801D18BC);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D1890.color = D_801D18B0 * 16;
    D_801D1890.model = D_801D18A0[effect->AnimationFrame % 4];
    g_KakusanBufferPtr = func_800D29D4(&D_801D1890, g_cDb->unk70, 12, g_KakusanBufferPtr);
    BattleSetBillboardMatrix(&effect->Pos, 0x2000, 0);
    D_801D1884.color.r = D_801D1884.color.g = D_801D1884.color.b = (u8)D_801D18B0 * 16;
    D_801D1884.frameIndex = effect->AnimationFrame % 2;
    g_KakusanBufferPtr = func_800D4D90(&D_801D1884, (u_long**)&g_cDb->unk70, 12, g_KakusanBufferPtr);
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame >= 33) {
            D_801D18B0--;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 40) {
            effect->StartFrame = -1;
        }
    }
}

void func_801B0B5C(void) {
    MATRIX matrix;
    MATRIX composed;
    VECTOR scale;
    KakusanData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    RotMatrix(&D_801D18D8, &matrix);
    MulMatrix0(&D_801E98F4, &matrix, &matrix);
    scale.vx = D_801E9914 * 4096 / 2000;
    scale.vy = scale.vz = 0x4000;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = effect->Pos.vx;
    matrix.t[1] = effect->Pos.vy;
    matrix.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &matrix, &composed);
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    D_801D18CC.frameIndex = effect->AnimationFrame;
    D_80163C74 = func_800D4D90(&D_801D18CC, (u_long**)&g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == D_801D1340) {
            effect->StartFrame = -1;
        }
    }
}

void KakusanMainTick(void) {
    MATRIX matrix;
    SVECTOR move;
    SVECTOR rot;
    KakusanData* effect;
    KakusanData* child;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            for (i = 0; i < 20; i++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(func_801B075C)];
                child->Pos.vx = effect->Pos.vx;
                child->Pos.vy = effect->Pos.vy;
                child->Pos.vz = effect->Pos.vz;
                child->unk18 = D_801E9914 * 6144 / 2000;
                child->PartPos.vx = rand() % 400 - 200;
                child->PartPos.vy = rand() % 400 + 0x338;
                child->PartPos.vz = 0x400;
                child->unk14 = rand() % 8;
                child->unk16 = i % 2;
            }
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0B5C)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0958)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0580)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy;
            child->Pos.vz = effect->Pos.vz;
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = 0;
            child->Pos.vz = effect->Pos.vz;
            child->PartPos.vx = 0x400;
            child->PartPos.vy = ratan2(D_801E98EC.vx - effect->Pos.vx, D_801E98EC.vz - effect->Pos.vz);
            child->PartPos.vz = 0;
        }
        if (effect->AnimationFrame < 40) {
            for (i = 0; i < 10; i++) {
                if ((D_801E98E8 >> i) & 1) {
                    child = &g_BattleEffectSlots[BattleEffectRegister(KakusanFlashSpriteTick)];
                    BattleGetPartPosition(i, g_BattleModels[i].boneIndices[0], &child->Pos);
                    move.vx = 0;
                    move.vy = 0;
                    move.vz = rand() % 400 + 400;
                    rot.vx = rand() % 4096;
                    rot.vy = rand() % 4096;
                    rot.vz = 0;
                    RotMatrix(&rot, &matrix);
                    ApplyMatrixSV(&matrix, &move, &move);
                    child->Pos.vx += move.vx;
                    child->Pos.vy += move.vy;
                    child->Pos.vz += move.vz;
                    child->unk16 = 1;
                }
            }
        }
        if (effect->AnimationFrame == 40) {
            for (i = 0; i < 10; i++) {
                if ((D_801E98E8 >> i) & 1) {
                    BattleSpawnFloatingIconAtPart(i, 0x3A, 0x1000);
                    func_800D5774(i);
                }
            }
            func_800BBA84(0x95, -1, 0x40);
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 50) {
            BattleFixedPointRampUpdateInit(0, 20);
            effect->StartFrame = -1;
        }
    }
}

void KakusanStart(s32 targetMask, s32 callbackArg) {
    SVECTOR dir;
    KakusanData* effect;

    BattleEffectRegister(KakusanDoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B1324, 0, 0, 0);
    D_801E98E8 = targetMask;
    effect = &g_BattleEffectSlots[BattleEffectRegister(KakusanMainTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[9], &effect->Pos);
    KakusanTargetCenterCalc(targetMask, &D_801E98EC);
    dir.vx = D_801E98EC.vx - effect->Pos.vx;
    dir.vy = D_801E98EC.vy - effect->Pos.vy;
    dir.vz = D_801E98EC.vz - effect->Pos.vz;
    BattleMatrixFromDirection(&dir, &D_801E98F4);
    D_801E9914 = SquareRoot0(dir.vx * dir.vx + dir.vy * dir.vy + dir.vz * dir.vz);
    func_800BBA84(0x131, callbackArg, 0);
    D_800F5B70 = D_800F5B71 = D_800F5B72 = 0;
    BattleFixedPointRampUpdateInit(0xC00, 10);
}
