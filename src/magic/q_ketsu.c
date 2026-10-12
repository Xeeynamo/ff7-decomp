//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "q_ketsu.h"
#include "../battle/battle.h"

// Effect shared by Drain (ドレイン), Dance (おどり) and Eyesight (眼力).

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 D_801BDAE8[2][0xC000];
extern void* D_801D5AE8;
extern SpriteRenderDesc D_801BDA90;
extern SpriteRenderDesc D_801BDA9C;
extern SpriteRenderDesc D_801BDABC;
extern SpriteAnim* D_801BDAA8[];
extern s32 D_801BD9CC;
extern s32 D_801BD84C;
extern SpriteRenderDesc D_801BDAB0;
extern s32 D_801BD8E4;
extern u8 D_801B10A4[];
extern ModelRenderDesc D_801B0000;
extern s16 D_801BDAD4;
extern VECTOR D_801BDAD8;
extern SpriteRenderDesc D_801BDAC8;
extern s32 D_801BD9AC;

void func_800BBA84(u16 arg0, s32 arg1, s32 arg2);
void QKetsuOrbSpriteTick(void);

// Only StartFrame/AnimationFrame are common to every magic overlay.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR Pos2;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
} QKetsuData; // size:0x20

extern QKetsuData g_BattleEffectSlots[];

// Same as BioraDoubleBufferFlip.
void QKetsuDoubleBufferFlip(void) {
    QKetsuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D5AE8 = D_801BDAE8[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void QKetsuHitSparkleSpriteTick(void) {
    QKetsuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2000, 0);
    D_801BDA90.frameIndex = effect->AnimationFrame;
    D_801D5AE8 = func_800D4D90(&D_801BDA90, g_cDb->unk70, 12, D_801D5AE8);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801BD9CC) {
            effect->StartFrame = -1;
        }
    }
}

void QKetsuRisingSparkleSpriteTick(void) {
    QKetsuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2000, 0);
    D_801BDA9C.frameIndex = effect->AnimationFrame % D_801BDAA8[effect->unk14]->frameCount;
    D_801BDA9C.frames = D_801BDAA8[effect->unk14];
    D_801D5AE8 = func_800D4D90(&D_801BDA9C, g_cDb->unk70, 12, D_801D5AE8);
    if (!D_80062D98) {
        effect->Pos.vy += 0x64;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 10) {
            effect->StartFrame = -1;
        }
    }
}

void QKetsuSpiralSparkleSpriteTick(void) {
    QKetsuData* effect;
    SVECTOR vec;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    vec.vx = effect->Pos.vx + (rsin(effect->unk14) * effect->unk18 >> 12);
    vec.vz = effect->Pos.vz + (rcos(effect->unk14) * effect->unk18 >> 12);
    vec.vy = effect->Pos.vy + effect->unk1A;
    BattleSetBillboardMatrix(&vec, 0x2000, 0);
    D_801BDAB0.frameIndex = effect->AnimationFrame % D_801BD8E4;
    D_801D5AE8 = func_800D4D90(&D_801BDAB0, g_cDb->unk70, 12, D_801D5AE8);
    if (!D_80062D98) {
        effect->unk1A -= 0x32;
        effect->unk14 += 0x80;
        effect->unk18 -= effect->unk18 / 10;
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 20) {
            effect->StartFrame = -1;
        }
    }
}

void QKetsuTrailSparkleSpriteTick(void) {
    QKetsuData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, 0x2000, 0);
    D_801BDABC.frameIndex = effect->AnimationFrame;
    D_801D5AE8 = func_800D4D90(&D_801BDABC, g_cDb->unk70, 12, D_801D5AE8);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == D_801BD84C) {
            effect->StartFrame = -1;
        }
    }
}

void QKetsuOrbSpriteTick(void) {
    QKetsuData* effect;
    QKetsuData* child;
    SVECTOR vec;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    vec.vx = effect->Pos.vx + (rsin(effect->unk14) * effect->unk18 >> 12);
    vec.vy = effect->Pos.vy + (rcos(effect->unk14) * effect->unk18 >> 12);
    vec.vz = effect->Pos.vz;
    BattleSetBillboardMatrix(&vec, (s16)(effect->unk16 * 5 / effect->unk1C), 0);
    D_801BDAC8.frameIndex = effect->AnimationFrame % D_801BD9AC;
    D_801D5AE8 = func_800D4D90(&D_801BDAC8, g_cDb->unk70, 12, D_801D5AE8);
    if (!D_80062D98) {
        if (effect->AnimationFrame < 0x1E) {
            effect->unk16 += 0xC0;
        }
        if ((u16)(effect->AnimationFrame - 0x1E) < 10) {
            effect->Pos.vx += effect->Pos2.vx;
            effect->Pos.vy += effect->Pos2.vy;
            effect->Pos.vz += effect->Pos2.vz;
            effect->unk18 = (u32)(rsin(effect->unk1A += 0xCC) * 25) >> 8;
            effect->unk14 += 0x200;
            child = &g_BattleEffectSlots[BattleEffectRegister(QKetsuTrailSparkleSpriteTick)];
            child->Pos.vx = effect->Pos.vx + rand() % 800 - 400;
            child->Pos.vy = effect->Pos.vy + rand() % 800 - 400;
            child->Pos.vz = effect->Pos.vz + rand() % 800 - 400;
        }
        if (effect->AnimationFrame >= 0x29) {
            effect->unk16 -= 0x100;
            child = &g_BattleEffectSlots[BattleEffectRegister(QKetsuRisingSparkleSpriteTick)];
            child->Pos.vx = effect->Pos.vx + rand() % 600 - 300;
            child->Pos.vy = effect->Pos.vy + rand() % 600 - 300;
            child->Pos.vz = effect->Pos.vz + rand() % 600 - 300;
            child->unk14 = rand() & 1;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x3C) {
            effect->StartFrame = -1;
        }
    }
}

void QKetsuModelTick(void) {
    QKetsuData* effect;
    QKetsuData* child;
    MATRIX mat;
    MATRIX view;
    ModelRenderDesc desc = D_801B0000;
    SVECTOR rot;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vz = 0;
    rot.vx = 0;
    rot.vy = effect->AnimationFrame << 7;
    RotMatrixYXZ(&rot, &mat);
    ScaleMatrix(&mat, &D_801BDAD8);
    mat.t[0] = effect->Pos.vx;
    mat.t[1] = effect->Pos.vy;
    mat.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView, &mat, &view);
    SetRotMatrix(&view);
    SetTransMatrix(&view);
    desc.color = D_801BDAD4;
    desc.clut = effect->AnimationFrame % 5 * 0xC0;
    D_801D5AE8 = func_800D29D4(&desc, (u_long**)&g_cDb->unk70, 12, D_801D5AE8);
    if (!D_80062D98) {
        if (effect->AnimationFrame < 0x14) {
            effect->Pos.vx += effect->Pos2.vx;
            effect->Pos.vy += effect->Pos2.vy;
            effect->Pos.vz += effect->Pos2.vz;
            D_801BDAD4 += 6;
        }
        if ((u16)(effect->AnimationFrame - 0x14) < 0x1E) {
            for (i = 0; i < 1; i++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(QKetsuSpiralSparkleSpriteTick)];
                child->Pos.vx = effect->Pos.vx;
                child->Pos.vy = effect->Pos.vy;
                child->Pos.vz = effect->Pos.vz;
                child->unk14 = rand() & 0xFFF;
                child->unk18 = 1000;
                child->unk1A = 1000;
            }
        }
        if ((u16)(effect->AnimationFrame - 0x26) < 6) {
            if (!(effect->AnimationFrame % 2)) {
                child = &g_BattleEffectSlots[BattleEffectRegister(QKetsuOrbSpriteTick)];
                child->Pos.vx = effect->Pos.vx;
                child->Pos.vy = effect->Pos.vy - 0x1F4;
                child->Pos.vz = effect->Pos.vz;
                child->Pos2.vx = -(u16)effect->Pos2.vx * 2;
                child->Pos2.vy = -(u16)effect->Pos2.vy * 2;
                child->Pos2.vz = -(u16)effect->Pos2.vz * 2;
                child->unk16 = 0x100;
                child->unk1A = 0;
                child->unk18 = 0;
                child->unk1C = (effect->AnimationFrame - 0x26) / 2 + 5;
            }
        }
        if (effect->AnimationFrame >= 0x33) {
            if (!(D_801BDAD4 -= 6)) {
                effect->StartFrame = -1;
            }
        }
        effect->AnimationFrame++;
    }
}

void QKetsuTargetTick(void) {
    QKetsuData* effect;
    QKetsuData* child;
    s32 y;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (!D_80062D98) {
        if (!effect->AnimationFrame) {
            child = &g_BattleEffectSlots[BattleEffectRegister(QKetsuModelTick)];
            child->Pos.vx = effect->Pos.vx;
            child->Pos.vy = effect->Pos.vy - 0x190;
            child->Pos.vz = effect->Pos.vz;
            child->Pos2.vx = (effect->Pos2.vx - child->Pos.vx) / 20;
            y = child->Pos.vy + 0x320;
            child->Pos2.vy = (effect->Pos2.vy - y) / 20;
            child->Pos2.vz = (effect->Pos2.vz - child->Pos.vz) / 20;
        }
        if (effect->AnimationFrame == 0x5A) {
            func_800D5774(effect->unk1C);
            func_800BBA84(7, effect->unk1E, 0x40);
        }
        if (effect->AnimationFrame >= 0x57 && effect->AnimationFrame < 0x66) {
            for (i = 0; i < 4; i++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(QKetsuHitSparkleSpriteTick)];
                BattleGetPartPosition(effect->unk1E, rand() % g_BattleModels[effect->unk1E].numBones, &child->Pos);
                child->Pos.vx += rand() % 600 - 300;
                child->Pos.vy += rand() % 600 - 300;
                child->Pos.vz += rand() % 600 - 300;
            }
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x6E) {
            effect->StartFrame = -1;
        }
    }
}

void QKetsuTargetSetup(s32 target, s32 target2) {
    QKetsuData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(QKetsuTargetTick)];
    BattleGetPartPosition(target2, g_BattleModels[target2].boneIndices[0], &effect->Pos);
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->Pos2);
    effect->unk1C = target;
    effect->unk1E = target2;
}

void MAGIC_QKetsu(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(QKetsuDoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B10A4, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 4, QKetsuTargetSetup);
    func_800BBA84(0x7E, callbackArg, 0x40);
}
