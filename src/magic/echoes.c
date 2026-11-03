//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
// ECHOES.BIN: Seven billboard sprites per update, with random offsets bounded by the target radius.

extern s32 rand(void);

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos, Velocity;
    s16 unk14, unk16, unk18, unk1A, TargetIndex, SourceIndex;
} EchoesEffect;
extern EchoesEffect g_BattleEffectSlots[];
extern u_long D_801B0474[];
extern u8 D_801B64B4[];
extern SpriteRenderDesc D_801B6870;
extern u8 D_801B687C[];
extern void* D_801D687C;
static void func_echoes_801B0404(s32 arg0, s32 arg1);

void MAGIC_Echoes(s32 targetMask, s32 callbackArg) { func_echoes_801B0404(targetMask, callbackArg); }

static void func_echoes_801B0020(void) {
    EchoesEffect* effect;
    MATRIX* matrix;
    u16 nextFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    matrix = BattleSetBillboardMatrix(&effect->Pos, (s32)effect->Velocity.vz, (s32)effect->Velocity.pad);
    matrix->t[0] += effect->Velocity.vx;
    matrix->t[1] = effect->Velocity.vy + matrix->t[1];
    SetTransMatrix(matrix);
    D_801B6870.frameIndex = (u16)effect->AnimationFrame;
    D_801D687C = func_800D4D90(&D_801B6870, g_cDb->unk70, 0xC, D_801D687C);
    if (D_80062D98 != 0) {
        return;
    }
    nextFrame = (u16)effect->AnimationFrame + 1;
    effect->AnimationFrame = nextFrame;
    if ((s16)nextFrame >= (s32)D_801B64B4[4]) {
        effect->StartFrame = -1;
    }
}

static void func_echoes_801B0108(void) {
    EchoesEffect* particle;
    EchoesEffect* effect;
    s32 angle;
    s32 angleY;
    s32 i;
    s32 fullTurn;
    s32 randomValue;
    s32 offset;
    s32 offsetY;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 != 0) {
        return;
    }
    if (effect->AnimationFrame < 0xF) {
        for (i = 0, fullTurn = 0x1000; i < 7;) {
            particle = &g_BattleEffectSlots[BattleEffectRegister(func_echoes_801B0020)];
            particle->Pos = effect->Pos;
            angle = rand() & 0x7FF;
            if (angle < 0x400) {
                offset = rsin(angle) - 0x1000;
            } else {
                offset = fullTurn - rsin(angle);
            }
            particle->Velocity.vx = (s16)((s32)(effect->Velocity.vx * offset) >> 0xC);
            angleY = rand() & 0x7FF;
            if (angleY < 0x400) {
                offsetY = rsin(angleY) - 0x1000;
            } else {
                offsetY = fullTurn - rsin(angleY);
            }
            particle->Velocity.vy = (s16)((s32)(effect->Velocity.vy * offsetY) >> 0xC);
            randomValue = rand();
            particle->Velocity.vz = (randomValue - (randomValue / 4096) * 4096) + 0x2000;
            i += 1;
            particle->Velocity.pad = (s16)(u16)effect->Velocity.pad;
        }
    } else {
        func_800D5774((u32)effect->StartFrame);
        effect->StartFrame = -1;
    }
    effect->AnimationFrame = (u16)effect->AnimationFrame + 1;
}

static void func_echoes_801B02BC(s32 arg0, s32 callbackArg) {
    EchoesEffect* effect;
    SVECTOR* position;
    s32 modelOffset;
    u32 nextFrame;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_echoes_801B0108)];
    // Keep a byte offset across the part-position call for the model-field views.
    modelOffset = arg0 * sizeof(BattleModel);
    position = &effect->Pos;
    effect->StartFrame = arg0;
    BattleGetPartPosition((s32)arg0, *(u8*)((u8*)g_BattleModels + modelOffset + 0x2B), position);
    nextFrame = (u32)(*(s16*)((u8*)g_BattleModels + modelOffset + 0x12) * 3) >> 1;
    effect->Velocity.vy = (s16)nextFrame;
    effect->Velocity.vx = (s16)nextFrame;
    // match: division preserves the signed halfword load used for the depth bias.
    effect->Velocity.pad = *(s16*)((u8*)g_BattleModels + modelOffset + 0x12) / -1;
    BattleAkaoCommand(0x20, BattlePositionToStereoPan(position), 0x134);
}

static void func_echoes_801B03A4(void) {
    EchoesEffect* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D687C = (effect->AnimationFrame << 0x10) + D_801B687C;
    effect->AnimationFrame = (u16)effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

static void func_echoes_801B0404(s32 arg0, s32 arg1) {
    BattleSetLoadTimToVram(D_801B0474, 0, 0, 0);
    BattleEffectRegister(func_echoes_801B03A4);
    MagicAnimationRegister(arg0, arg1, 0, func_echoes_801B02BC);
}
