//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"
#include "magic_private.h"

// Bolt (サンダー / Thunder), tier 1.

#define MODEL_LIFETIME 16
#define DIM_START_FRAME 8
#define GREY_FULL 0x80
#define GREY_PER_FRAME 0x10

#define BOLT_LIFETIME 16
#define SPARK_START_FRAME 2

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR unk0C;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 Scale;     // 0x1000 == 1.0
    /* 0x18 */ u16 ScaleStep; // seeded 0x200
    /* 0x1A */ u16 unk1A;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ char pad1E[2];
} ThunderData; // size:0x20

typedef struct {
    /* 0x00 */ char pad[MAGIC_PAGE_SIZE];
} ThunderPrimPage; // size:0x10000

extern void* g_ThunderBufferPtr;
extern ThunderData g_BattleEffectSlots[];
extern ThunderPrimPage g_ThunderPrimBuffer[];
extern u_long g_ThunderTexture[]; // 8bpp TIM + CLUT, uploaded on setup
extern SpriteRenderDesc g_ThunderRenderDesc0;
extern SpriteRenderDesc g_ThunderRenderDesc1;
extern MATRIX g_ThunderModelMatrix;
extern ModelRenderDesc g_ThunderModelDesc;

static void ThunderMainSetup(s32 arg0, s32 arg1);

void MAGIC_Thunder(s32 arg0, s32 arg1) { ThunderMainSetup(arg0, arg1); }

static void ThunderRenderModel(void) {
    MATRIX matrix;
    ThunderData* effect;
    s16 frame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    frame = effect->AnimationFrame;
    if (frame < DIM_START_FRAME) {
        g_ThunderModelDesc.color = GREY_FULL;
    } else if (frame < MODEL_LIFETIME) {
        g_ThunderModelDesc.color = GREY_FULL - ((frame - DIM_START_FRAME) * GREY_PER_FRAME);
    } else {
        effect->StartFrame = -1;
        return;
    }

    g_ThunderModelMatrix.m[0][0] = g_ThunderModelMatrix.m[2][1] = effect->Scale;
    g_ThunderModelMatrix.m[1][2] = -effect->Scale;
    g_ThunderModelMatrix.t[0] = effect->Pos.vx;
    g_ThunderModelMatrix.t[1] = effect->Pos.vy;
    g_ThunderModelMatrix.t[2] = effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, &g_ThunderModelMatrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    g_ThunderBufferPtr = func_800D29D4(&g_ThunderModelDesc, g_cDb->unk70, 0xC, g_ThunderBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        effect->Scale += effect->ScaleStep;
    }
}

static void func_801B0180(void) {
    ThunderData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    func_800D4368(&effect->Pos, 0x2000, effect->unk1C);
    g_ThunderRenderDesc0.frameIndex = effect->AnimationFrame >> 1;
    g_ThunderBufferPtr = func_800D4D90(&g_ThunderRenderDesc0, g_cDb->unk70, 0xC, g_ThunderBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 9) {
            effect->StartFrame = -1;
        }
    }
}

static void func_801B023C(void) {
    MATRIX* matrix;
    ThunderData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    matrix = func_800D4368(&effect->Pos, 0x2000, effect->unk1C);
    if (effect->unk1A & 1) {
        matrix->m[0][0] = -matrix->m[0][0];
    }
    if (effect->unk1A & 2) {
        matrix->m[1][1] = -matrix->m[1][1];
    }
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    g_ThunderRenderDesc1.frameIndex = effect->AnimationFrame;
    g_ThunderBufferPtr = func_800D4D90(&g_ThunderRenderDesc1, g_cDb->unk70, 0xC, g_ThunderBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 8) {
            effect->StartFrame = -1;
        }
    }
}

static void ThunderSpawnBolt(void) {
    ThunderData* next;
    ThunderData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B0180)];
            next->Pos = effect->Pos;
            next->Pos.vy = 0;
            next->unk1C = effect->unk1C;
            func_800D5774(effect->unk14);
            if (effect->AnimationFrame == 0) {
                next = &g_BattleEffectSlots[BattleEffectRegister(ThunderRenderModel)];
                next->Pos = effect->Pos;
                next->Scale = 4096;
                next->Pos.vy = 0;
                next->ScaleStep = 0x200;
                next->unk1C = effect->unk1C;
            }
        }
        if (effect->AnimationFrame >= SPARK_START_FRAME) {
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B023C)];
            next->Pos.vx = (effect->Pos.vx + rand() % 1000) - 500;
            next->Pos.vy = (effect->Pos.vy + rand() % 1000) - 500;
            next->Pos.vz = (effect->Pos.vz + rand() % 1000) - 500;
            next->unk1A = rand() & 3;
            next->unk1C = effect->unk1C;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == BOLT_LIFETIME) {
            effect->StartFrame = -1;
        }
    }
}

static void ThunderAttachToTarget(s32 target, s32 arg1) {
    ThunderData* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(ThunderSpawnBolt)];
    BattleGetPartPosition(target, D_801518E4[target].D_8015190F, &effect->Pos);
    effect->unk14 = target;
    effect->unk1C = -D_801518E4[target].unk12;
    BattleCommandSend(0x20, BattlePositionToStereoPan(&effect->Pos), 0xB);
}

static void ThunderDoubleBufferFlip(void) {
    ThunderData* data;

    data = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_ThunderBufferPtr = &g_ThunderPrimBuffer[data->AnimationFrame];
    data->AnimationFrame = data->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        data->StartFrame = -1;
    }
}

static void ThunderMainSetup(s32 arg0, s32 arg1) {
    func_800D2980(g_ThunderTexture, 0, 0, 0);
    BattleEffectRegister(ThunderDoubleBufferFlip);
    MagicAnimationRegister(arg0, arg1, 2, ThunderAttachToTarget);
}
