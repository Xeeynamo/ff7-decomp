//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include <libetc.h>
#include "magic.h"
#include "sailess.h"
#include "../battle/battle.h"

// Effect SAILESS.

extern u_long D_801B0498[];
void func_801B02E0(s32 target, s32 callbackArg);

// Only StartFrame/AnimationFrame are common to every magic overlay.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ s16 unkC;
    /* 0x0E */ s16 unkE;
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ char pad1A[6];
} SailessData; // size:0x20

extern SailessData g_BattleEffectSlots[];
extern char D_801B8E7C[];
extern char D_801C4E7C[];
extern s32 D_801B84D8[];
extern s32* D_801D0E80; // slot running func_801B0000
extern void* D_801D0E7C;
void func_801B03FC(s32 targetMask, s32 callbackArg);

// Same as Dia2BufferFlip.
static void func_801B0000(void) {
    D_801D0E7C = g_dbIndex == 0 ? D_801B8E7C : D_801C4E7C;
    if (g_BattleEffectCount < 2) {
        *D_801D0E80 = -1;
    }
}

void func_801B0054(s32 targetMask, s32 callbackArg) { func_801B03FC(targetMask, callbackArg); }

// Same as func_801B0F9C.
void func_801B0074(void) {
    SVECTOR rotA;
    SVECTOR rotB;
    SVECTOR offset;
    MATRIX matA;
    MATRIX matB;
    VECTOR scale;
    SailessData* effect;
    ModelRenderDesc* desc;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rotA.vx = 0;
    rotA.vy = effect->unkE;
    rotA.vz = effect->unkC;
    RotMatrix(&rotA, &matA);
    rotB.vx = 0;
    rotB.vy = effect->unkE;
    rotB.vz = 0;
    RotMatrix(&rotB, &matB);
    offset.vx = 0;
    offset.vy = 0;
    offset.vz = -effect->unk10;
    ApplyMatrix(&matB, &offset, (VECTOR*)matA.t);
    matA.t[0] += effect->Pos.vx;
    matA.t[1] += effect->Pos.vy;
    matA.t[2] += effect->Pos.vz;
    scale.vx = scale.vy = scale.vz = effect->unk14;
    ScaleMatrix(&matA, &scale);
    CompMatrix(&g_BattleWorldView.m, &matA, &matA);
    SetRotMatrix(&matA);
    SetTransMatrix(&matA);

    desc = (ModelRenderDesc*)getScratchAddr(0);
    desc->model = D_801B84D8;
    desc->flags = 0;
    desc->uvOffset = 0;
    desc->color = 0x80;
    desc->tpage = 0x20;
    desc->clut = 0;
    if (effect->AnimationFrame >= 0xC) {
        desc->flags = MODEL_SEMI_TRANS;
        desc->color -= (effect->AnimationFrame - 0xC) * 8;
    }
    D_801D0E7C = func_800D29D4(desc, (u_long**)&g_cDb->unk70, 12, D_801D0E7C);

    if (effect->AnimationFrame >= 0x1C) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0x14) {
            func_800D5774(effect->unk18);
        }
        if (effect->AnimationFrame < 8) {
            effect->unkC += 0x200;
            effect->unk14 += effect->unk16;
        } else if (effect->AnimationFrame >= 0xC) {
            effect->unk10 -= effect->unk12;
        }
        effect->AnimationFrame++;
    }
}

void func_801B02E0(s32 target, s32 callbackArg) {
    SailessData* effect;
    s32 radius;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_801B0074)];
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &effect->Pos);
    effect->unkC = 0;
    effect->unkE = g_BattleModels[target].rootRot.vy;
    radius = g_BattleModels[target].collisionRadius + 0xE0;
    if (radius >= 0x76D) {
        radius = 0x76C;
    }
    effect->unk10 = radius;
    effect->unk12 = radius / 16;
    effect->unk14 = 0;
    if (radius >= 0x4F3) {
        radius = 0x4F2;
    }
    effect->unk16 = (radius << 12) / 2000;
    effect->unk18 = target;
}

// Same as QueikStart.
void func_801B03FC(s32 targetMask, s32 callbackArg) {
    D_801D0E80 = (s32*)&g_BattleEffectSlots[BattleEffectRegister(func_801B0000)];
    BattleSetLoadTimToVram(D_801B0498, 0, 0, 0);
    MagicAnimationRegister(targetMask, callbackArg, 1, func_801B02E0);
    BattleAkaoCommand(AKAO_PLAY_SOUND, BattleEntityGetStereoPan(targetMask), 0x54);
}
