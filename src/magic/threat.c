//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
// THREAT.BIN: An emitter travels from the source to the target over sixteen updates, leaving a sprite trail.

typedef struct {
    s16 StartFrame, AnimationFrame;
    SVECTOR Pos, Velocity;
    s16 unk14, unk16, unk18, unk1A, TargetIndex, SourceIndex;
} ThreatEffect;
extern ThreatEffect g_BattleEffectSlots[];
extern u_long D_801B0410[];
extern s32 D_801B8454;
extern SpriteRenderDesc D_801B8548;
extern u8 D_801B8554[];
extern void* D_801D8554;

void MAGIC_Threat(s32 arg0, s32 arg1) { MAGIC_ThreatSetup(arg0, arg1); }

static void func_threat_801B0020(void) {
    ThreatEffect* effect;
    s16 nextFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, (s32)effect->unk14, 0);
    D_801B8548.frameIndex = (u16)effect->AnimationFrame;
    D_801D8554 = func_800D4D90(&D_801B8548, g_cDb->unk70, 0xC, D_801D8554);
    if (D_80062D98 == 0) {
        nextFrame = (u16)effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame >= D_801B8454) {
            effect->StartFrame = -1;
        }
    }
}

static void func_threat_801B00DC(void) {
    ThreatEffect* effect;
    ThreatEffect* particle;
    s16 nextFrame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        effect->Pos.vx = (u16)effect->Pos.vx + (u16)effect->Velocity.vx;
        effect->Pos.vy = (u16)effect->Pos.vy + (u16)effect->Velocity.vy;
        effect->Pos.vz = (u16)effect->Pos.vz + (u16)effect->Velocity.vz;
        particle = &g_BattleEffectSlots[BattleEffectRegister(func_threat_801B0020)];
        particle->Pos = effect->Pos;
        particle->unk14 = 0x2000;
        nextFrame = (u16)effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame >= 0x10) {
            BattleSpawnFloatingIcon(effect->unk16, 0x32);
            func_800D5774((u32)effect->unk16);
            effect->StartFrame = -1;
        }
    }
}

static void func_threat_801B01D8(s32 arg0, s32 arg1) {
    SVECTOR sp10;
    ThreatEffect* effect;
    s32 randomValue;
    s32 var_v0_2;
    s32 var_v0_3;

    effect = &g_BattleEffectSlots[BattleEffectRegister(func_threat_801B00DC)];
    BattleGetPartPosition(arg1, g_BattleModels[arg1].boneIndices[8], &effect->Pos);
    BattleGetPartPosition(arg0, g_BattleModels[arg0].boneIndices[0], &sp10);
    randomValue = sp10.vx - effect->Pos.vx;
    if (randomValue < 0) {
        randomValue += 0xF;
    }
    effect->Velocity.vx = (s16)(randomValue >> 4);
    var_v0_2 = sp10.vy - effect->Pos.vy;
    if (var_v0_2 < 0) {
        var_v0_2 += 0xF;
    }
    effect->Velocity.vy = (s16)(var_v0_2 >> 4);
    var_v0_3 = sp10.vz - effect->Pos.vz;
    if (var_v0_3 < 0) {
        var_v0_3 += 0xF;
    }
    effect->Velocity.vz = (s16)(var_v0_3 >> 4);
    effect->unk16 = (s16)arg0;
}

static void func_threat_801B0304(void) {
    ThreatEffect* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D8554 = (effect->AnimationFrame << 0x10) + D_801B8554;
    effect->AnimationFrame = (u16)effect->AnimationFrame ^ 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void MAGIC_ThreatSetup(s32 arg0, s32 arg1) {
    BattleSetLoadTimToVram(D_801B0410, 0, 0, 0);
    BattleEffectRegister(func_threat_801B0304);
    MagicAnimationRegister(arg0, arg1, 4, func_threat_801B01D8);
    BattleAkaoCommand(0x20, BattlePositionToStereoPan(&g_BattleModels[arg1].rootTrans), 0x31);
}
