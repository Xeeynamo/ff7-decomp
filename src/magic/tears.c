//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
// TEARS.BIN: Three falling sprites per update, distributed around the target in the x/z plane.

extern s32 rand(void);

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos, Velocity;
    s16 unk14, unk16, unk18, unk1A, TargetIndex, SourceIndex;
} TearsEffect;
extern TearsEffect g_BattleEffectSlots[];
extern BattleSpriteDesc D_801B0450;
extern u8 D_801B0460[];
extern void* D_801D0460;
static void func_tears_801B03E4(s32, s32);

void MAGIC_Tears(s32 arg0, s32 arg1) { func_tears_801B03E4(arg0, arg1); }

static void func_tears_801B0020(void) {
    TearsEffect* effect;
    s16 startFrame;
    u8 nextFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    startFrame = effect->AnimationFrame;
    if (startFrame < 5) {
        nextFrame = (startFrame << 7) / 5;
        D_801B0450.b = nextFrame;
        D_801B0450.g = nextFrame;
        D_801B0450.r = nextFrame;
    } else {
        *(u32*)&D_801B0450.r = 0x2E808080;
    }
    BattleSetBillboardMatrix(&effect->Pos, (s32)effect->unk14, 0);
    D_801B0450.u = ((effect->AnimationFrame + effect->StartFrame) & 7) * 32;
    D_801D0460 = BattleEffectSpriteAdd(&D_801B0450, g_cDb->unk70, 0xC, D_801D0460);
    if (D_80062D98 == 0) {
        effect->Pos.vy = (u16)effect->Pos.vy + (u16)effect->Velocity.vy;
        effect->AnimationFrame = (u16)effect->AnimationFrame + 1;
        effect->Velocity.vy = (s16)(u16)effect->Velocity.vy;
        if (effect->Pos.vy > 0) {
            effect->StartFrame = -1;
        }
    }
}

static void func_tears_801B0158(void) {
    TearsEffect* particle;
    TearsEffect* effect;
    s32 zOffset;
    s32 temp_s1;
    s32 temp_s2;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        i = 0;
        if (effect->AnimationFrame < 0x1E) {
        loop_2:
            i += 1;
            particle = &g_BattleEffectSlots[BattleEffectRegister(func_tears_801B0020)];
            temp_s1 = rand() & 0xFFF;
            temp_s2 = rand() % 1000;
            particle->Pos.vx = (u16)effect->Pos.vx + ((s32)(rsin(temp_s1) * temp_s2) >> 0xC);
            particle->Pos.vy = (rand() % 100) - 0x834;
            zOffset = rcos(temp_s1) * temp_s2;
            particle->Pos.vz = (u16)effect->Pos.vz + (zOffset >> 0xC);
            particle->Velocity.vy = 0x64;
            particle->unk14 = 0x1000;
            particle->StartFrame = rand() & 7;
            if (i < 3) {
                goto loop_2;
            }
        } else {
            func_800D5774((u32)effect->StartFrame);
            effect->StartFrame = -1;
        }
        effect->AnimationFrame = (u16)effect->AnimationFrame + 1;
    }
}

static void func_tears_801B0304(s32 arg0, s32 callbackArg) {
    TearsEffect* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_tears_801B0158)];
    effect->StartFrame = arg0;
    BattleGetPartPosition((s32)arg0, g_BattleModels[arg0].boneIndices[0], &effect->Pos);
}

static void func_tears_801B0384(void) {
    TearsEffect* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D0460 = (effect->AnimationFrame << 0x10) + D_801B0460;
    effect->AnimationFrame = (u16)effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

static void func_tears_801B03E4(s32 arg0, s32 arg1) {
    BattleEffectRegister(func_tears_801B0384);
    MagicAnimationRegister(arg0, arg1, 0, func_tears_801B0304);
    BattleAkaoCommand(0x20, BattleEntityGetStereoPan(arg0), 0x11E);
}
