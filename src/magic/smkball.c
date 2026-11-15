//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
// SMKBALL.BIN: Seven screen-space sprites per update, with random x/y positions and double buffering.

extern s32 rand(void);

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos, Velocity;
    s16 unk14, unk16, unk18, unk1A, TargetIndex, SourceIndex;
} SmkballEffect;
extern SmkballEffect g_BattleEffectSlots[];
extern u_long D_801B0310[];
extern u8 D_801B6350[];
extern MATRIX D_801B670C;
extern SpriteRenderDesc D_801B672C;
extern u8 D_801B6738[];
extern void* D_801D6738;
static void func_smkball_801B02B0(s32 arg0, s32 arg1);

void MAGIC_Smkball(s32 targetMask, s32 callbackArg) { func_smkball_801B02B0(targetMask, callbackArg); }

static void func_smkball_801B0020(void) {
    SmkballEffect* effect;
    s16 nextFrame;
    MATRIX* matrix;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801B670C.t[0] = effect->Pos.vx * 4;
    D_801B670C.t[1] = effect->Pos.vy * 4;
    matrix = &D_801B670C;
    D_801B670C.t[2] = ReadGeomScreen() * 4;
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    D_801B672C.frameIndex = (u16)effect->AnimationFrame;
    D_801D6738 = func_800D4D90(&D_801B672C, &g_cDb->unk4080[1], 0, D_801D6738);
    if (D_80062D98 != 0) {
        return;
    }
    effect->AnimationFrame++;
    nextFrame = effect->AnimationFrame;
    if ((s16)nextFrame >= (s32)D_801B6350[4]) {
        effect->StartFrame = -1;
    }
}

static void func_smkball_801B0124(void) {
    SmkballEffect* particle;
    s32 i;
    SmkballEffect* effect;
    s32 randomValue;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 != 0) {
        return;
    }
    if (effect->AnimationFrame < 0x1E) {
        for (i = 0; i < 7;) {
            i += 1;
            particle = &g_BattleEffectSlots[BattleEffectRegister(func_smkball_801B0020)];
            randomValue = rand();
            particle->Pos.vx = (randomValue % 320) - 0xA0;
            particle->Pos.vy = (rand() % 166) - 0x78;
        }
    } else {
        effect->StartFrame = -1;
    }
    effect->AnimationFrame = (u16)effect->AnimationFrame + 1;
}

static void func_smkball_801B0250(void) {
    SmkballEffect* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D6738 = (effect->AnimationFrame << 0x10) + D_801B6738;
    effect->AnimationFrame = (u16)effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

static void func_smkball_801B02B0(s32 arg0, s32 arg1) {
    BattleSetLoadTimToVram(D_801B0310, 0, 0, 0);
    BattleEffectRegister(func_smkball_801B0250);
    BattleEffectRegister(func_smkball_801B0124);
    BattleAkaoCommand(0x20, 0x40, 0x261);
}
