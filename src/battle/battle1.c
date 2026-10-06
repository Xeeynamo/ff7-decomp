//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"
#include <libetc.h>
#include <libgpu.h>

static void func_800B37A0(void);
static void func_800B37EC(void);
static void BattleLoadFirstEnemy(void);
static void BattleLoadSeffects(void);
static void BattleEnemyInitBonesAndAnims(void);
static void BattlePlayersInitBonesAndAnims(void);
static void func_800B3E2C(void);
static s32 func_800B3FAC(s32 arg0);
static void BattleQueue1ClearTargs(void);
static void BattleUpdateRender(void);
static void func_800B8360(s32);
static void func_800B85E0();
static void func_800B88CC(s32 arg0);
static void func_800B8E48(s32 arg0);
static void func_800BA24C(void);
static void func_800BA4C8(void);
void func_800BA598(s16);
static void func_800BB030(s16);
void BattleQueue1CameraInit(void);
static void func_800BB75C(BattleWorldView* view, MATRIX* camera, s16* cameraPos, s16* cameraTarget);
static void func_800BB804(void);
static void func_800BB864(void);
s32 func_800BC04C(void (*callback)(void));
s32 BattleMovementRegister(void (*callback)(void));
static s32 BattleCameraRegister(void (*callback)(void));
static void BattleCallbacksReset(void);
static void BattleCameraResetCallbacks(void);
static void func_800BC348(void);
static void BattleMovementUpdate(void);
static void func_800BC538(void);
static void BattleCameraUpdate(void);
static void func_800C0410(void);
static void func_800C0900(void);
static void BattleGet4DigitsFromValue(s16 arg0, s16* arg1);
static void func_800C4D10(void);
DR_MODE* func_800C4DC8(s16 x, s16 y, s16 w, s16 h, s32*);
static void BattleStoreUnitClut(u_long* arg0, s32 arg1);
static void func_800C627C(void);
void func_800C62F4(s32);
static void func_800BC81C(s16 arg0, s16 arg1);
static void BattleLoadSecondEnemy(void);
static void func_800B950C(void);
void BattleFadeOutUntargetedEnemies(void);
void func_800C679C(void);
void BattleLoadEffectModel(void);
void BattleFadeInAfterSummon(void);
void BattleFadeInUntargetedEnemies(void);
static void func_800C74A4(void);
void BattleUnitInitBonesAndMatrixes(s32 arg0, void* arg1, s32 arg2);
static void BattleLoadSecondPlayer(void);
void BattleLoadPlayerModel(s16);
void BattleLoadPlayerTexture(s16);
void BattleLoadThirdPlayer(void);
static void func_800C5BEC(void);
void BattleModelStartFades(s16);
void func_800B5FE8(s16);
void BattleModelFadeOutTick(void);
void BattleModelFadeInTick(void);
static void func_800BB89C(void);
void func_800BCA58(s32);
void func_800C1104();
void func_800BCB1C(u8, s16, s16);
void func_800BEA38(u8, s16, s16);
void func_800C0DD8(s16, s32, s32);
s32 func_800C0314(s32, s32);
static void func_800C5468(u8 arg0);
void func_800C5170(u8);
static u_long* func_800C5040(u8 r, u8 g, u8 b, s32 tpage, u_long* ot);
static void func_800C55B8(void);

// MAGIC/ summon entrypoints that are not named yet
EffectModel* func_801B0038(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_2(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_3(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_4(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_5(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_6(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_7(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_8(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_9(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_10(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_11(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_12(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0040_13(s32 targetMask, s32 callbackArg);
void func_801B0050(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0054(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0060(s32 targetMask, s32 callbackArg);

// MAGIC/ entrypoints of other effects with a model, not named yet
EffectModel* func_801B0054_9(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0054_2(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0054_3(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0054_4(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0054_5(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0054_6(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0054_7(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0054_8(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0084(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0084_2(s32 targetMask, s32 callbackArg);
EffectModel* func_801B00B4(s32 targetMask, s32 callbackArg);
EffectModel* func_801B00E8(s32 targetMask, s32 callbackArg);
EffectModel* func_801B04C0(s32 targetMask, s32 callbackArg);
EffectModel* func_801B0498(s32 targetMask, s32 callbackArg);

void BattleNormalStartSeq(void) {
    s32 i;

    g_cDb = &g_db;
    D_801031E4 = 0;
    g_dbIndex = 0;
    D_80162084 = 0x200;
    func_800B383C();
    func_800B430C();
    VSync(0);
    SetDispMask(0);
    D_800F9F34 = 0;
    *(s8*)&g_BattleWorldView.u.sub.unk34 = 0;
    D_800FA6A0 = 0;
    func_800B37A0();
    func_800B3E2C();
    BattleQueue1CameraInit();
    func_800BC04C(func_800C4D10);
    BattleUpdateRender();
    BattleUpdateRender();
    do {
    } while (D_80095DD4);
    func_800B37EC();
    SetDispMask(1);
    while (1) {
        switch (D_80163C7C) {
        case 0:
            D_801635FC = 0x3D;
            BattleLoadFirstEnemy();
            BattleUpdateRender();
            D_80163C7C = 1;
            break;
        case 1:
            BattleUpdateRender();
            if (D_800F7DF4 == (u8)D_80166F64 && D_801518DC == 0) {
                BattleLoadSeffects();
                BattleParseEnemyModels();
                D_80163C7C = 6;
            }
            break;
        case 6:
            BattleUpdateRender();
            BattleEnemyInitBonesAndAnims();
            for (i = 4; i < D_800F7E04[0] + 4; i++) {
                g_BattleModels[i].animControlFlags |= 4;
            }
            D_80163C7C = 2;
            break;
        case 2:
            BattleUpdateRender();
            if ((u8)D_80166F64 == 3 && D_801518DC == 0) {
                BattlePlayersInitBonesAndAnims();
                D_80163C7C = 3;
                g_BattleModels[0].animControlFlags |= 4;
                g_BattleModels[1].animControlFlags |= 4;
                g_BattleModels[2].animControlFlags |= 4;
            }
            break;
        case 3:
            BattleUpdateRender();
            if (D_801635FC == 0) {
                D_80163C7C = 4;
                func_800C61C0();
            }
            break;
        default:
            return;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleNextStartSeq);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleEnemyPlayInitAnims);

// one-shot setup call centered on the 320x240 screen
static void func_800B37A0(void) {
    func_800D91DC(0x140, 0xF0, D_80162084, D_800FA6A0, g_BattleWorldView.u.sub.unk34, D_800F9F34);
}

static void func_800B37EC(void) {
    D_80162094 = 4;
    BattleSetVsyncMode(4);
    BattleMenuInit();
    BattleMenuWidgetOpen(-1, -1, 0);
    D_80095DD4 = 2;
}

// Load stage files
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B383C);

// load stage entry i (D_800F7DF8[0]) into VRAM staging via SysCdromStartLoadLzs
static void BattleLoadFirstEnemy(void) {
    s32 i = D_800F7DF8[0];

    SysCdromStartLoadLzs(*&D_800E8050[i].loc, *&D_800E8050[i].len, (u_long*)0x801B0000, &BattleLoadSecondEnemy);
    BattleCdromReadChain();
}

static void BattleLoadEnemyFinish(void) {
    BattleLoadEnemyTexture(2);
    BattleLoadEnemyModel(2);
    D_80166F64 = 3;
}

// third link of the stage-load chain (BattleLoadFirstEnemy -> BattleLoadSecondEnemy ->
// here -> BattleLoadEnemyFinish): unpack the part just read into the staging buffer,
// record where the next part lands (D_800F8390[n+1] = D_800F8390[n] + size),
// advance the D_80166F64 phase counter BattleNormalStartSeq waits on, and queue the
// next part's read only while entries remain (D_800F7DF4 is the entry count)
static void BattleLoadThirdEnemy(void) {
    s32 size;
    s32 i;

    BattleLoadEnemyTexture(1);
    size = BattleLoadEnemyModel(1);
    D_80166F64 = 2;
    D_800F8390[2] = size + D_800F8390[1];
    if (D_800F7DF4 >= 3U) {
        i = D_800F7DF8[2];
        SysCdromStartLoadLzs(*&D_800E8050[i].loc, *&D_800E8050[i].len, (u_long*)0x801B0000, BattleLoadEnemyFinish);
        BattleCdromReadChain();
    }
}

// second link of the chain: unpack part 0 out of the staging buffer, then
// queue part 1's read
static void BattleLoadSecondEnemy(void) {
    s32 size;
    s32 i;

    D_800F8390[0] = D_80130200;
    BattleLoadEnemyTexture(0);
    size = BattleLoadEnemyModel(0);
    D_80166F64 = 1;
    D_800F8390[1] = size + D_800F8390[0];
    if (D_800F7DF4 >= 2U) {
        i = D_800F7DF8[1];
        SysCdromStartLoadLzs(*&D_800E8050[i].loc, *&D_800E8050[i].len, (u_long*)0x801B0000, BattleLoadThirdEnemy);
        BattleCdromReadChain();
    }
}

static void BattleLoadSecondPlayer(void) {
    s16* s0;
    u8** dst;
    s16 v1;
    s16 cmp;

    s0 = &D_800FA9C6;
    v1 = *s0;
    dst = &D_800F8384[v1];
    *dst = D_80103200 + v1 * 0xF000;
    BattleLoadPlayerTexture(*s0);
    BattleLoadPlayerModel(*s0);
    cmp = D_800FA9C8;
    if (cmp != 0xC8) {
        SysCdromStartLoadLzs(*&D_800E8068[cmp].loc, *&D_800E8068[cmp].len, (u_long*)0x801B0000, BattleLoadThirdPlayer);
        BattleCdromReadChain();
        return;
    }
    D_80166F64 = 3;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleLoadThirdPlayer);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleLoadPlayerFinish);

static void BattleLoadFirstPlayer(void) {
    Yamada* y;
    u_long* dst;

    dst = (u_long*)0x801B0000;
    BattleSetLoadTimToVram(dst, 0, 0, 0);
    y = &D_800E8068[D_800FA9C4];
    SysCdromStartLoadLzs(y->loc, *&D_800E8068[D_800FA9C4].len, dst, BattleLoadSecondPlayer);
    BattleCdromReadChain();
}

static void BattleLoadSeffects(void) {
    BattleSelectPlayerModelFiles();
    D_800F839C = D_800EA50C;
    SysCdromStartLoadLzs(LBA_ENEMY6_SEFFECT, 0xA800, (u_long*)0x801B0000, BattleLoadFirstPlayer);
    BattleCdromReadChain();
}

static void BattleEnemyInitBonesAndAnims(void) {
    BattleEnemyModelsUpdateBonesPosClut();
    BattleInitModelsAnimAndColor(4, 10);
    BattleEnemyPlayInitAnims();
}

static void BattlePlayersInitBonesAndAnims(void) {
    s32 i;

    BattlePlayerModelsUpdateBonesPos();
    BattleInitModelsAnimAndColor(0, 3);
    BattleInitModelsAnimAndColor(3, 3);
    if (g_BattleData.activeEncounter.setup.stageID == 57) {
        for (i = 0; i < 10; i++) {
            g_BattleModels[i].specialFlags |= BATTLE_MODEL_NO_SHADOW;
        }
    }
}

static void func_800B3E2C(void) {
    s32 i;
    u8 var_a0;

    D_80163C7C = 0;
    D_800F9D94 = 0;
    D_80162974 = 0;
    D_800F7DE4 = 1;
    D_800F837C = 0;
    D_801031E0 = 1;
    D_801590E0 = 0;
    D_801620A0 = 0;
    D_80163B38 = 0;
    D_801590CC = 0;
    D_800FA6D4 = 0;
    D_801517C4 = 0;
    D_801620A4 = 0;
    D_800FAFDC = 0;
    D_800F7ED4 = 0;
    D_800F9D9C = 0;
    D_800F9D98 = 0;
    D_801590D8 = 0;
    D_80166F58 = 0;
    D_801516A0 = 0;
    D_800F8380 = 0;
    for (i = 0; i < LEN(g_BattleModels); i++) {
        g_BattleModels[i].unk26 = 1;
    }
    for (i = 2; i >= 0; i--) {
        D_800F9F28[i] = 0;
    }
    var_a0 = D_801590CC;
    g_BattleModels[var_a0].attackEffectId = 0;
    g_BattleModelFadeFrames = 0xE;
    g_BattleActionQueue[D_801590E0].unk8 = -2;
    BattleCallbacksReset();
    func_800C5BEC();
}

// search the formation's 6 enemy slots for one whose enemyID matches arg0;
// if found, bump a counter and return 0, else return -1
static s32 func_800B3FAC(s32 arg0) {
    s32 i;
    u8* p = &D_800F7DF4;

    for (i = 0; i < (s32)sizeof(g_BattleData.activeEncounter.formation); i += sizeof(FormationEntry)) {
        if (((FormationEntry*)((u8*)g_BattleData.activeEncounter.formation + i))->enemyID == arg0) {
            *p += 1;
            return 0;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3FFC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B430C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattlePlayerModifyDefaultPosByFormation);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattlePlayerSetDefaultRot);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattlePlayerModelsUpdateBonesPos);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattlePlayerInitModelWithSettings);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleParseEnemyModels);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleEnemyInitModelWithSettings);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleEnemyModelsUpdateBonesPosClut);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5AAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleLoadPlayerModel);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleLoadEnemyModel);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleLoadEnemyTexture);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleLoadPlayerTexture);

static void func_800B5FC4(s16 arg0) { BattleModelStartFades(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5FE8);

void BattleModelStartFades(s16 actor) {
    if (g_BattleModels[actor].animControlFlags & ANIM_CTRL_FADE_OUT) {
        g_BattleModels[actor].animControlFlags &= ~ANIM_CTRL_FADE_OUT;
        *(s32*)0x1F800000 = BattleMovementRegister(BattleModelFadeOutTick);
        g_BattleMovementSlots[*(s32*)0x1F800000].D_801620B2 = actor;
        g_BattleMovementSlots[*(s32*)0x1F800000].D_801620B0 = g_BattleModelFadeFrames;
        g_BattleModels[actor].blendAlpha = 0;
        g_BattleMovementSlots[*(s32*)0x1F800000].D_801620B4 = g_BattleModels[actor].colorR / g_BattleModelFadeFrames;
        g_BattleMovementSlots[*(s32*)0x1F800000].D_801620B6 = g_BattleModels[actor].colorG / g_BattleModelFadeFrames;
        g_BattleMovementSlots[*(s32*)0x1F800000].unkC = g_BattleModels[actor].colorB / g_BattleModelFadeFrames;
        g_BattleMovementSlots[*(s32*)0x1F800000].unkE = 0x1000 / g_BattleModelFadeFrames;
        func_800B5FE8(actor);
    }
    if (g_BattleModels[actor].animControlFlags & ANIM_CTRL_FADE_IN) {
        g_BattleModels[actor].animControlFlags &= ~ANIM_CTRL_FADE_IN;
        *(s32*)0x1F800000 = BattleMovementRegister(BattleModelFadeInTick);
        g_BattleMovementSlots[*(s32*)0x1F800000].D_801620B2 = actor;
        g_BattleMovementSlots[*(s32*)0x1F800000].D_801620B0 = g_BattleModelFadeFrames;
        g_BattleModels[actor].blendAlpha = 0x1000;
        g_BattleMovementSlots[*(s32*)0x1F800000].D_801620B4 = g_BattleModels[actor].colorR / g_BattleModelFadeFrames;
        g_BattleMovementSlots[*(s32*)0x1F800000].D_801620B6 = g_BattleModels[actor].colorG / g_BattleModelFadeFrames;
        g_BattleMovementSlots[*(s32*)0x1F800000].unkC = g_BattleModels[actor].colorB / g_BattleModelFadeFrames;
        g_BattleMovementSlots[*(s32*)0x1F800000].unkE = 0x1000 / g_BattleModelFadeFrames;
        if (actor == EFFECT_MODEL_SLOT) {
            D_80153BDD &= ~BATTLE_MODEL_HIDDEN;
            func_800B5FE8(EFFECT_MODEL_SLOT);
        }
        if (actor < START_ENEMY || D_800F7E10[actor - START_ENEMY][0] & 1) {
            g_BattleModels[actor].specialFlags &= ~BATTLE_MODEL_HIDDEN;
        }
        func_800B5FE8(actor);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleModelFadeOutTick);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleModelFadeInTick);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleInitModelsAnimAndColor);

// drains g_BattleActionQueue (12-byte entries, -1-terminated, index D_801590E0), one
// entry per call, dispatched by a type byte (0-5, jtbl_800A05FC) via m2c
// structural read (not yet decompiled):
//   0 callback-driven step (func_800BC04C(&func_800C494C)), immediate
//   1 gated on D_800F7DE4: walks a linked status list (g_BattleQueueTargets/1/2),
//     looks like "hide next status icon" (sets D_800FA6D4/D_80161EEC/
//     D_800F99E8 icon slots, or 0xF when the list is exhausted)
//   2 func_800C5C18(4 entry fields), immediate -- shape matches a sound cue
//   3 gated on D_800F7DE4: same linked-list shape as case 1, opposite flag
//     direction -- looks like "show next status icon"
//   4 gated on D_800F7DE4: HP-counter tick-animation init -- writes to PS1
//     scratchpad (0x1F800004/8), computes abs(diff)/entryField, stores
//     start/target/increment into a g_BattleEffectSlots slot (allocated via
//     BattleEffectRegister)
//   5 immediate: sets a per-actor "step complete" flag, conditionally
//     copies animation-state fields
// D_800F7DE4 (the gate for cases 1/3/4) is set once per frame by
// BattleUpdateRender below, once all actor slots are ready -- so this function
// is a generic "process the next queued visual/counter effect, one per
// frame" drainer, not itself the source of any particular command's
// damage/effect. See BattleActionType14's comment in battle.c: opcode 0x14 just
// spins this to drain whatever's already queued
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleQueue1Execute);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleQueue1InitPlayAnim);

extern u8 D_801517F0[0x4E];

static void BattleQueue1ClearTargs(void) {
    s32 i;

    for (i = 0; i < LEN(D_801517F0); i += 1) {
        D_801517F0[i] = 0xFF;
        D_80163CC0[i].D_80163CC0 = 0;
        D_80163CC0[i].D_80163CC2 = 0;
        D_80163CC0[i].D_80163CC4 = 0;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleQueue1AddNewTarg);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleQueue1UpdateTargMasks);

static void func_800B7F6C(void) {
    volatile s32 padding;

    while (g_SavemapBusy) {
        BattleCdromReadChain();
    }
    D_80062D98 = 0;
}

void BattleCdromReadChain(void) { D_801518DC = SystemCdromReadChain(); }

// per-frame tick: pumps the GPU ordering-table draw lists, runs render/vsync,
// drains the action-queue ring buffer (func_800A3ED0 -- see the queue-push
// writeup), and sets D_800F7DE4 = 1 exactly once per frame once every actor
// slot is ready and g_BattleEffectCount (a per-frame counter) reaches 0. BattleQueue1Execute
// gates several of its event-queue steps on this flag, effectively waiting
// for "the next frame is ready" before consuming a queued effect
static void BattleUpdateRender(void) {
    s32 i;

    BattleCdromReadChain();
    ClearOTagR((u_long*)g_cDb->unk40A4, LEN(g_cDb->unk40A4));
    ClearOTag((u_long*)g_cDb->unk4070, LEN(g_cDb->unk4070));
    ClearOTag((u_long*)g_cDb->unk4078, LEN(g_cDb->unk4078));
    ClearOTagR((u_long*)g_cDb->unk70, LEN(g_cDb->unk70));
    ClearOTagR((u_long*)g_cDb->unk4080, LEN(g_cDb->unk4080));
    ClearOTag((u_long*)g_cDb->unk40E4, LEN(g_cDb->unk40E4));
    ClearOTag((u_long*)g_cDb->unk40EC, LEN(g_cDb->unk40EC));
    D_80163C74 = g_dbIndex == 0 ? (DR_MODE*)0x80168000 : (DR_MODE*)0x80184000;
    func_800B8360(1);
    func_800C5CC0();
    func_800B8438();
    for (i = 0; i < 10; i++) {
        if (g_BattleModels[i].unk26 == 0) {
            D_800F7DE4 = 0;
            break;
        }
        if (g_BattleEffectCount == 0) {
            D_800F7DE4 = 1;
        } else {
            D_800F7DE4 = 0;
        }
    }
    func_800A3ED0();
    func_800B8360(2);
    func_800DCFD4((u_long*)g_cDb->unk40E4);
    if (D_800F9D94 == 0) {
        ResetGraph(1);
        D_800F9D94 = 1;
    }
    if (g_BattleData.flags & 2) {
        func_800E16B8(g_cDb->unk40E4, 0x10, 0x10, Savemap.countdown_timer_seconds);
    }
    D_800FA9B8 = VSync(1);
    BattleFlushImageQueue();
    BattleCdromReadChain();
    D_80158D08 = BattleFlipDoubleBuffer();
    SetGeomScreen(D_80162084);
    D_801516F4++;
    func_800B7F6C();
    func_800B950C();
    D_801516A0 = D_800F198C;
}

static void func_800B8234(s32 arg0) {
    if (arg0) {
        func_800D0C80(D_801590CC);
        D_801517BC = 0;
    }
}

static void func_800B8268(void) {
    s32 i;
    u8* var_a1;
    s32 var_t1;

    i = 0;
    var_t1 = 1;
    var_a1 = D_80163784;
    while (i < 10) {
        *var_a1 = g_BattleData.actors[i].idleActionId;
        if (!(D_80151200[i].D_8015120C & 8) && g_BattleModels[i].animId != *var_a1 &&
            g_BattleModels[i].unk26 == var_t1) {
            g_BattleModels[i].animControlFlags |= 1;
            g_BattleModels[i].animId = *var_a1;
        }
        var_a1++;
        i += 1;
    }
    D_80163787 = 0;
}

// build a draw-mode prim (texture page selected by arg0) and add it to the OT
static void func_800B8360(s32 arg0) {
    DR_MODE* drMode;

    SetDrawMode(D_80163C74, 1, 1, (arg0 & 3) << 5, 0);
    drMode = D_80163C74;
    D_80163C74 = drMode + 1;
    AddPrim(g_cDb->unk4078, drMode);
}

static void func_800B83C4() {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_80151200[i].D_8015120C & 1) {
            func_800BA4C8();
            func_800BA40C();
            return;
        }
    }
    func_800BA40C();
    func_800BA4C8();
}

void func_800B8438(void) {
    func_800B9568();
    if (D_801635FC) {
        D_801635FC--;
    }
    switch (D_80163C7C) {
    case 2:
        func_800B905C();
        BattleMovementUpdate();
        func_800BA4C8();
        break;
    case 0:
    case 1:
    case 6:
        break;
    case 3:
    case 4:
    case 5:
    default:
        func_800B8EE4();
        func_800B905C();
        func_800B8234(D_801517BC);
        BattleMovementUpdate();
        BattleCdromReadChain();
        func_800B83C4();
        func_800B8B48();
        break;
    }
    BattleCdromReadChain();
    func_800B91CC();
    D_80151694 = g_BattleData.unitPresentMask;
    func_800B85E0();
    func_800BC81C(D_800F8370, g_BattleModels[D_801590CC].attackEffectId);
    func_800BC8B0(D_800F8370);
    func_800B8268();
    SetFarColor(0, 0, 0);
    func_800BC538();
    func_800BC348();
    func_800BB75C(&g_BattleWorldView, &D_800FA958, &g_BattleCameraPos, &g_BattleCameraTarget);
    func_800C627C();
}

static void func_800B85E0() {
    s32 i;

    if (D_800F7ED4 != 100 && D_800FA6B8) {
        func_800BB804();
        D_80163C7C = 5;
        BattlePlaySavemapDoneSound();
        D_800F7ED4 = 100;
        g_BattleActionQueue[D_801590E0].unk8 = -3;
        BattleQueue1CameraInit();
        for (i = 0; i < 3; i++) {
            g_BattleModels[i].animControlFlags |= 0x20;
            D_80151200[i].D_80151200 = g_BattleData.actors[i].D_801636C0;
        }
    }
    if (D_800F9D98 != 100 && (g_BattleMode & 1)) {
        D_80163C7C = 5;
        BattlePlaySavemapDoneSound();
        D_800F9D98 = 100;
        g_BattleActionQueue[D_801590E0].unk8 = -1;
        BattleQueue1CameraInit();
    }
    if (!D_801590D8 && D_80163B80) {
        func_800BB864();
        D_801590D8 = 1;
    }
    if (D_800F9D9C != 100) {
        i = 0;
        if (g_BattleMode & 8) {
            for (; i < 3; i++) {
                g_BattleModels[i].animControlFlags |= 1;
                g_BattleModels[i].animId = g_BattleData.actors[i].idleActionId;
                g_BattleModels[i].animControlFlags |= 0x20;
                D_80151200[i].D_80151200 = g_BattleData.actors[i].D_801636C0;
            }
            D_800F9D9C = 100;
            D_80163C7C = 5;
            BattlePlaySavemapDoneSound();
            g_BattleActionQueue[D_801590E0].unk8 = -1;
            BattleQueue1CameraInit();
        }
    }
}

extern u8 D_801517F0[0x4E];

s16 func_800B888C(s32 arg0) {
    s32 i;

    for (i = 0; i < LEN(D_801517F0); i++) {
        if (arg0 == D_801517F0[i]) {
            return i;
        }
    }
}

// initialize g_BattleEffectSlots slot v (registered via BattleEffectRegister) from arg0
// and dispatch
static void func_800B88CC(s32 arg0) {
    s32 v = BattleEffectRegister(&BattleFixedPointRampSpawnChildEffectsWithFade);

    g_BattleEffectSlots[v].D_8016297C = 0;
    g_BattleEffectSlots[v].D_80162980 = arg0;
    func_800B8A34(func_800B888C(arg0), v);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8944);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8A34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8B48);

static void func_800B8E48(s32 arg0) {
    s32 temp_a0;

    temp_a0 = arg0 & 0xFF;
    g_BattleModels[temp_a0].unk26 = 1;
    g_BattleModels[temp_a0].specialFlags &= 0x7F;
    D_80151200[temp_a0].D_8015120C &= 0xFFDF;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8EE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8FCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B905C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B91CC);

// promote each model's staged (x, y) into its committed (prevX, prevY)
static void func_800B950C(void) {
    s32 i;

    for (i = 0; i < LEN(g_modelScreenPos); i++) {
        g_modelScreenPos[i].prevX = g_modelScreenPos[i].x;
        g_modelScreenPos[i].prevY = g_modelScreenPos[i].y;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B9568);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA11C);

// advances two wrapping counters (mod 4, mod 32) and accumulates an offset
// each time one wraps to 0; func_800BA2BC (still undecompiled) reads/writes
// the same D_80163B44/D_800F8182 pair with an analogous mod-32 decrement, so
// this is one tick of a periodic effect shared with that function
static void func_800BA24C(void) {
    D_800F8182[0] = 0;
    if (D_80163B44[0] == 0) {
        D_800F8182[0] = -0x28;
    }
    if (D_80163B44[1] == 0) {
        D_800F8182[0] -= 0x50;
    }
    D_80163B44[0] = (D_80163B44[0] - 1) & 3;
    D_80163B44[1] = (D_80163B44[1] - 1) & 0x1F;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA2BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA360);

static void func_800BA40C(void) {
    s32 i;
    u8 param;

    for (i = 0; i < 3; i++) {
        if (!(g_BattleModels[i].specialFlags & BATTLE_MODEL_INACTIVE)) {
            param = i;
            func_800C1908(param);
            func_800BA598(i);
            if (g_BattleModels[i].deathType & 0x80) {
                func_800BB2A8(param);
                func_800BB030(i);
            }
        }
    }
}

static void func_800BA4C8(void) {
    s32 i;

    for (i = 4; i < D_800F7E04[0] + 4; i++) {
        if (!(g_BattleModels[i].specialFlags & 0x80)) {
            continue;
        }
        if (g_BattleModels[i].specialFlags & BATTLE_MODEL_INACTIVE) {
            continue;
        }
        func_800C1908(i);
        func_800BA598(i);
        if (g_BattleModels[i].deathType & 0x80) {
            func_800BB030(i);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA598);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleUpdateModelMatrixesWithSelfAndParent);

static void BattleUpdateMatrixWithSelfAndParentAndSetToGte(BattleModelSub* modelSub) {
    s32 flag;

    *(MATRIX**)0x1F800020 = modelSub->parentMatrix;
    *(MATRIX*)0x1F800024 = **(MATRIX**)0x1F800020;
    MulMatrix2((MATRIX*)0x1F800024, &modelSub->m);
    SetRotMatrix((MATRIX*)0x1F800024);
    SetTransMatrix((MATRIX*)0x1F800024);
    RotTrans(&modelSub->trans, (VECTOR*)modelSub->m.t, &flag);
    SetRotMatrix(&modelSub->m);
    SetTransMatrix(&modelSub->m);
}

static void BattleUpdateMatrixWithScaleAndSetToGte(MATRIX* m, VECTOR* v) {
    ScaleMatrix(m, v);
    SetRotMatrix(m);
    SetTransMatrix(m);
}

static void func_800BB030(s16 arg0) {
    s32 i;
    ModelRenderDesc* unk;

    unk = (ModelRenderDesc*)0x1F800020;
    SetFarColor(g_BattleModels[arg0].colorR, g_BattleModels[arg0].colorG, g_BattleModels[arg0].colorB);
    SetRotMatrix(&g_BattleModels[arg0].stageMatrix);
    SetTransMatrix(&g_BattleModels[arg0].stageMatrix);
    for (i = 0; i < D_800FA6D8[arg0].unk3C; i++) {
        RotMatrixYXZ(&D_800FA6D8[arg0].unk8[i].rot, &D_800FA6D8[arg0].unk8[i].m);
    }

    for (i = 0; i < D_800FA6D8[arg0].unk3C; i++) {
        BattleUpdateMatrixWithSelfAndParentAndSetToGte(&D_800FA6D8[arg0].unk8[i]);
        if (!D_800FA6D8[arg0].unk4[i])
            continue;
        unk->model = D_800FA6D8[arg0].unk4[i];
        unk->flags = D_800FA6D8[arg0].unk3E[i] | MODEL_PRIM_PACKET_BITS | MODEL_DEPTH_CUE;
        unk->uvOffset = 0;
        unk->color = g_BattleModels[arg0].blendAlpha;
        unk->tpage = 0x20;
        unk->clut = g_BattleModels[arg0].clutOffset;
        if (g_BattleModels[arg0].specialFlags & BATTLE_MODEL_HIDDEN) {
            continue;
        }
        D_80163C74 = func_800D29D4(unk, g_cDb->unk70, 12, D_80163C74);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB2A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleModelUpdateAllBonesHeight);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB4F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleUnitInitBonesAndMatrixes);

void BattleBoneSetParentMatrix(MATRIX* parent, BattleModelSub* bone) { bone->parentMatrix = parent; }

void BattleQueue1CameraInit(void) {
    s16 command = g_BattleActionQueue[D_801590E0].unk8;
    u8 category;

    if (command == -4) {
        return;
    }
    D_800F8370 = command;
    D_801590DC = 0;
    g_BattleQueue1CamWriteCursor[3].pos = 0xFF;
    g_BattleQueue1CamWriteCursor[2].pos = 0xFF;
    g_BattleQueue1CamWriteCursor[1].pos = 0xFF;
    g_BattleQueue1CamWriteCursor[0].pos = 0xFF;
    g_BattleQueue1CamReadCursor[3].pos = 0xFF;
    g_BattleQueue1CamReadCursor[2].pos = 0xFF;
    g_BattleQueue1CamReadCursor[1].pos = 0xFF;
    g_BattleQueue1CamReadCursor[0].pos = 0xFF;
    BattleCameraResetCallbacks();
    if (D_800F837C != 3) {
        category = D_801516F4 & 3;
        if (category != 3) {
            D_800F837C = category;
        }
    }
}

static void func_800BB75C(BattleWorldView* view, MATRIX* camera, s16* cameraPos, s16* cameraTarget) {
    s32 flag;

    func_800D85B0(camera, cameraPos, cameraTarget, &D_800E7D10);
    RotMatrixYXZ(&view->rot, &view->m);
    TransMatrix(&view->m, &view->u.v);
    MulMatrix2(camera, &view->m);
    SetRotMatrix(camera);
    SetTransMatrix(camera);
    RotTrans(&view->u.sub.trans, (VECTOR*)&view->m.t, &flag);
    BattleUpdateMatrixWithScaleAndSetToGte(&view->m, &D_800E7D20);
}

static void func_800BB804(void) {
    if (!(g_BattleData.flags & 0x20)) {
        SystemLoadFileBySector(LBA_ENEMY6_FAN2, 0x1000, (u_long*)0x801D0000, func_800BB89C);
        BattleCdromReadChain();
        return;
    }
    D_80163B80 = 0;
    D_800FA6B8 = 0;
}

static void func_800BB864(void) {
    SystemLoadFileBySector(LBA_ENEMY6_OVER2, 0x800, (u_long*)0x801D0000, func_800BB89C);
    BattleCdromReadChain();
}

static void func_800BB89C(void) {
    D_80163B80 = 0;
    D_800FA6B8 = 0;
    g_AkaoCmd.opcode = !(!(g_BattleData.flags & 0x10) && !g_AkaoPrevBgmLanes[0].activeMask)
                           ? AKAO_PLAY_MUSIC
                           : AKAO_PLAY_MUSIC_SAVE_CURR;
    g_AkaoCmd.params[0] = 0x801D0000;
    AkaoExec();
}

void func_800BB90C(void) {
    g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT2;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
}

// queue the 0xF1 sound command after the 0xA0 pair; called from batres
void func_800BB944(void) {
    func_800BB90C();
    g_AkaoCmd.opcode = AKAO_STOP_ALL_SOUNDS;
    AkaoExec();
}

// queue sound command 0xC1
void func_800BB978(void) {
    g_AkaoCmd.opcode = AKAO_VOL_SLIDE_FROM_CURR;
    g_AkaoCmd.params[0] = 0x12C;
    g_AkaoCmd.params[1] = 0;
    AkaoExec();
}

// queue sound command 0x30, dispatched directly via AkaoDispatchCommand (akao.c)
// rather than the g_AkaoCmd staging command used by the sibling functions below
void func_800BB9B8(s32 arg0) {
    s16* ptr;

    ptr = &D_800F4AD0;
    *ptr = AKAO_PLAY_MENU_SOUND;
    D_800F4AD4 = arg0 & 0xFFFF;
    D_800F4AD8 = arg0 & 0xFFFF;
    AkaoDispatchCommand(ptr);
}

// queue sound command 0x2B
void func_800BB9FC(s32 arg0) {
    s32 param;

    g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
    param = arg0 & 0xFFFF;
    g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
    g_AkaoCmd.params[1] = param;
    AkaoExec();
}

// queue sound command 0x20
static void func_800BBA40(s32 arg0) {
    s32 param;

    g_AkaoCmd.opcode = AKAO_PLAY_SOUND;
    param = arg0 & 0xFFFF;
    g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
    g_AkaoCmd.params[1] = param;
    AkaoExec();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBA84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBB20);

static void func_800BBDF8(void) {
    if (g_dbIndex == 0) {
        D_800F4AF4 = D_80163C74;
        if ((u32)D_80163C74 > (u32)0x80184000) {
            PadStop();
            ResetGraph(1);
            StopCallback();
            SystemError('b', 0);
        }
    } else {
        D_800F4AF8 = D_80163C74;
        if ((u32)D_80163C74 > (u32)0x801A0000) {
            PadStop();
            ResetGraph(1);
            StopCallback();
            SystemError('b', 1);
        }
    }
}

// the callback writes -1 over field 0 of its slot to have the Update free it
s32 BattleEffectRegister(void (*callback)(void)) {
    s16 i;

    for (i = 0; i < 100; i++) {
        if (!g_BattleEffectCallbacks[i]) {
            if (i >= g_BattleEffectCursor) {
                g_BattleEffectCallbacks[i] = callback;
                g_BattleEffectSlots[i].D_80162978 = g_BattleEffectCursor;
                g_BattleEffectCount++;
                return i;
            }
        }
    }

    PadStop();
    ResetGraph(1);
    StopCallback();
    SystemError('a', 1);
}

s32 BattleMovementRegister(void (*callback)(void)) {
    s16 i;

    for (i = 0; i < 10; i++) {
        if (!g_BattleMovementCallbacks[i]) {
            if (i >= g_BattleMovementCursor) {
                g_BattleMovementCallbacks[i] = callback;
                g_BattleMovementSlots[i].D_801620AC = g_BattleMovementCursor;
                g_BattleMovementCount++;
                return i;
            }
        }
    }

    PadStop();
    ResetGraph(1);
    StopCallback();
    SystemError('a', 2);
}

// q-gears: "add effect callback"
s32 func_800BC04C(void (*callback)(void)) {
    s16 i;

    for (i = 0; i < 60; i++) {
        if (!D_80163B84[i]) {
            if (i >= D_801590D4) {
                D_80163B84[i] = callback;
                D_801621F0[i].D_801621F0 = D_801590D4;
                D_80163C78++;
                return i;
            }
        }
    }

    PadStop();
    ResetGraph(1);
    StopCallback();
    SystemError('a', 4);
}

static s32 BattleCameraRegister(void (*callback)(void)) {
    s16 i;

    for (i = 0; i < 16; i++) {
        if (!g_BattleCameraCallbacks[i]) {
            g_BattleCameraCallbacks[i] = callback;
            g_BattleCameraSlots[i].D_800F7ED8 = g_BattleCameraCursor;
            g_BattleCameraCount++;
            return i;
        }
    }

    PadStop();
    ResetGraph(1);
    StopCallback();
    SystemError('a', 3);
}

// q-gears: "init damage, unit movement, effect and camera callback arrays"
static void BattleCallbacksReset(void) {
    s32 i;

    g_BattleEffectCount = g_BattleMovementCount = D_80163C78 = 0;

    for (i = 0; i < 100; i++) {
        g_BattleEffectCallbacks[i] = NULL;
        g_BattleEffectSlots[i].D_80162978 = g_BattleEffectSlots[i].D_8016297A = 0;
    }
    for (i = 0; i < 10; i++) {
        g_BattleMovementCallbacks[i] = NULL;
        g_BattleMovementSlots[i].D_801620AC = g_BattleMovementSlots[i].D_801620AE = 0;
    }
    for (i = 0; i < 60; i++) {
        D_80163B84[i] = NULL;
        D_801621F0[i].D_801621F0 = D_801621F0[i].D_801621F2 = 0;
    }

    BattleCameraResetCallbacks();
}

static void BattleCameraResetCallbacks(void) {
    s32 i;

    g_BattleCameraCount = 0;
    for (i = 0; i < 0x10; i++) {
        g_BattleCameraCallbacks[i] = 0;
        g_BattleCameraSlots[i].D_800F7ED8 = 0;
        g_BattleCameraSlots[i].D_800F7EDA = 0;
    }
}

// drives the 0x64 queue; q-gears calls that one the damage callbacks
static void func_800BC348(void) {
    void (*callback)(void);

    for (g_BattleEffectCursor = 0; g_BattleEffectCursor < 100; g_BattleEffectCursor++) {
        callback = g_BattleEffectCallbacks[g_BattleEffectCursor];
        if (callback) {
            callback();
            if (g_BattleEffectSlots[g_BattleEffectCursor].D_80162978 == -1) {
                g_BattleEffectSlots[g_BattleEffectCursor].D_80162978 = 0;
                g_BattleEffectSlots[g_BattleEffectCursor].D_8016297A = 0;
                g_BattleEffectCallbacks[g_BattleEffectCursor] = NULL;
                g_BattleEffectCount--;
            }
        }
    }
    g_BattleEffectCursor = 0;
}

static void BattleMovementUpdate(void) {
    void (*callback)(void);

    for (g_BattleMovementCursor = 0; g_BattleMovementCursor < 10; g_BattleMovementCursor++) {
        callback = g_BattleMovementCallbacks[g_BattleMovementCursor];
        if (callback) {
            callback();
            if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC == -1) {
                g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = 0;
                g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 0;
                g_BattleMovementCallbacks[g_BattleMovementCursor] = NULL;
                g_BattleMovementCount--;
            }
        }
    }
    g_BattleMovementCursor = 0;
}

// q-gears: "effects update"
static void func_800BC538(void) {
    void (*callback)(void);

    for (D_801590D4 = 0; D_801590D4 < 60; D_801590D4++) {
        callback = D_80163B84[D_801590D4];
        if (callback) {
            callback();
            if (D_801621F0[D_801590D4].D_801621F0 == -1) {
                D_801621F0[D_801590D4].D_801621F0 = 0;
                D_801621F0[D_801590D4].D_801621F2 = 0;
                D_80163B84[D_801590D4] = NULL;
                D_80163C78--;
            }
        }
    }
    D_801590D4 = 0;
}

static void BattleCameraUpdate(void) {
    void (*callback)(void);

    for (g_BattleCameraCursor = 0; g_BattleCameraCursor < 16; g_BattleCameraCursor++) {
        callback = g_BattleCameraCallbacks[g_BattleCameraCursor];
        if (callback) {
            callback();
            if (g_BattleCameraSlots[g_BattleCameraCursor].D_800F7ED8 == -1) {
                g_BattleCameraSlots[g_BattleCameraCursor].D_800F7ED8 = 0;
                g_BattleCameraSlots[g_BattleCameraCursor].D_800F7EDA = 0;
                g_BattleCameraCallbacks[g_BattleCameraCursor] = NULL;
                g_BattleCameraCount--;
            }
        }
    }
    g_BattleCameraCursor = 0;
}

static void func_800BC72C(void) {
    func_800C1104();
    func_800BCA58(3);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", CopyCameraStartEndFromBattleSetup);

// run both per-slot handlers for each of the three party slots, then the
// shared tail step; skipped entirely while D_801590DC is set
static void func_800BC81C(s16 arg0, s16 arg1) {
    s32 i;

    if (D_801590DC == 0) {
        for (i = 0; i < 3; i++) {
            func_800BEA38(i, arg1, arg0);
            func_800BCB1C(i, arg1, arg0);
        }
        BattleCameraUpdate();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC8B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BCA58);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BCB1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE49C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE69C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE86C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BEA38);

// Read the next u16 from arg0's byte stream via this category's read cursor.
static s16 func_800BFA98(u8* arg0, s32 arg1) {
    s32 category = arg1 & 0xFF;
    u16 pos = g_BattleQueue1CamReadCursor[category].pos;
    u32 lo;
    u8 hi;

    g_BattleQueue1CamReadCursor[category].pos = pos + 1;
    lo = arg0[pos];
    g_BattleQueue1CamReadCursor[category].pos = pos + 2;
    hi = arg0[(u16)(pos + 1)];
    return (hi << 8) + lo;
}

static s16 func_800BFB10(u8* arg0, s32 arg1) {
    s32 category = arg1 & 0xFF;
    u16 pos = g_BattleQueue1CamWriteCursor[category].pos;
    u32 lo;
    u8 hi;

    g_BattleQueue1CamWriteCursor[category].pos = pos + 1;
    lo = arg0[pos];
    g_BattleQueue1CamWriteCursor[category].pos = pos + 2;
    hi = arg0[(u16)(pos + 1)];
    return (hi << 8) + lo;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFB88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFDA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFF88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0088);

// Sample sp, then accumulate it into the scratchpad totals at 0x1F800000.
static void func_800C018C(s16 arg0, s16 arg1, s32 arg2, s32 arg3) {
    SVECTOR sp;

    if (arg0 == 0xF) {
        BattleEntityGetCenter(g_BattleCurrentTargetMask, &sp);
    } else {
        BattleGetPartPosition(arg0, arg1, &sp);
        func_800C0DD8(arg0, arg2 & 0xFF, arg3 & 0xFF);
    }
    *(s32*)0x1F800000 += sp.vx;
    *(s32*)0x1F800004 += sp.vy;
    *(s32*)0x1F800008 += sp.vz;
}

static void func_800C0254(s16 arg0, s16 arg1) {
    SVECTOR sp;

    if (arg0 == 0xF) {
        BattleEntityGetCenter(g_BattleCurrentTargetMask, &sp);
    } else {
        BattleGetPartPosition(arg0, arg1, &sp);
        *(s32*)0x1F800004 = func_800C0314(*(s32*)0x1F800004, (u8)arg0);
    }
    *(s32*)0x1F800000 += sp.vx;
    *(s32*)0x1F800004 += sp.vy;
    *(s32*)0x1F800008 += sp.vz;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0314);

// magnitude of (arg0 - arg1) via GTE sqrt
static s16 func_800C03B8(s16 arg0, s16 arg1) {
    s32 delta;

    delta = arg0 - arg1;
    return SquareRoot0(delta * delta);
}

static s32 func_800C03FC(s32 arg0, s32 arg1) { return arg0 < 0 ? -arg1 : arg1; }

void func_800C0480(s16); // TODO: mark as static once decompiled
void func_800C0630(s16); // TODO: mark as static once decompiled
static void func_800C0410(void) {
    switch (g_BattleCameraSlots[g_BattleCameraCursor].D_800F7EDA) {
    case 0:
        func_800C0480(g_BattleCameraCursor);
        func_800C0630(g_BattleCameraCursor);
        return;
    case 1:
        func_800C0630(g_BattleCameraCursor);
        return;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0630);

void func_800C0970(s16); // TODO: mark as static once decompiled
void func_800C0B20(s16); // TODO: mark as static once decompiled
static void func_800C0900(void) {
    switch (g_BattleCameraSlots[g_BattleCameraCursor].D_800F7EDA) {
    case 0:
        func_800C0970(g_BattleCameraCursor);
        func_800C0B20(g_BattleCameraCursor);
        return;
    case 1:
        func_800C0B20(g_BattleCameraCursor);
        return;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0970);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0B20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0DD8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1104);

// cosine-eased interpolation between arg0 and arg1
static s32 func_800C1304(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 val;
    s32 delta;

    delta = arg1 - arg0;
    val = (rcos((s16)(((arg3 << 0xB) / arg2) + 0x800)) + 0x1000) * delta;
    return arg0 + val / 0x2000;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C14C0);

static s32 func_800C169C(u8 arg0) {
    g_BattleModels[arg0].specialFlags |= 8;
    if (D_80151200[arg0].D_80151200 & 0x2000) {
        return 10;
    }
    if (D_80151200[arg0].D_80151200 & 0x4000) {
        return 5;
    }
    if (D_80151200[arg0].D_80151200 & 0x0008) {
        return 1;
    }
    if (D_80151200[arg0].D_80151200 & 0x800000) {
        return 3;
    }
    if (D_80151200[arg0].D_80151200 & 0x01000000) {
        return 6;
    }
    if (D_80151200[arg0].D_80151200 & 0x04000000) {
        return 8;
    }
    if (D_80151200[arg0].D_80151200 & 0x8000) {
        return 9;
    }
    if (D_80151200[arg0].D_80151200 & 0x400000) {
        return 7;
    }
    g_BattleModels[arg0].specialFlags &= ~8;
    return 0;
}

static void func_800C17A0(s32 arg0, s32 arg1) {
    switch (D_800EA19C[arg1][0]) {
    case 0:
        g_BattleModels[arg0].blendAlpha = 0;
        break;
    case 1:
        g_BattleModels[arg0].blendAlpha = 0x800;
        break;
    case 2:
        g_BattleModels[arg0].blendAlpha = 0xC00;
        break;
    }
    g_BattleModels[arg0].colorR = D_800EA19C[arg1][1];
    g_BattleModels[arg0].colorG = D_800EA19C[arg1][2];
    g_BattleModels[arg0].colorB = D_800EA19C[arg1][3];
    g_BattleModels[arg0].unk24 = 0;
}

static void func_800C1908(u8 arg0) {
    s32 temp_a1;
    s16 var_a0;
    u8 temp_s0;

    temp_s0 = arg0;
    if (g_BattleModels[temp_s0].animControlFlags & 0x20) {
        if (temp_s0 < 4) {
            D_800F9F28[temp_s0] = g_BattleData.actors[temp_s0].D_801636C0;
        }
        func_800C5170(temp_s0);
        func_800C5468(temp_s0);
        func_800C17A0(temp_s0, func_800C169C(temp_s0));
        g_BattleModels[temp_s0].animControlFlags &= 0xDF;
    }
    temp_a1 = arg0;
    if (D_80151200[temp_a1].D_80151235 == 0) {
        if (D_80151200[temp_a1].D_80151200 & 0x4000) {
            D_80151200[temp_a1].D_80151233 = 3;
            return;
        }
        D_80151200[temp_a1].D_80151233 = 0;
        if (D_80151200[temp_a1].D_80151200 & 0x100) {
            D_80151200[temp_a1].D_80151233 = 1;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x200) {
            D_80151200[temp_a1].D_80151233 = 2;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x400) {
            D_80151200[temp_a1].D_80151233 = 3;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x02000000) {
            D_80151200[temp_a1].D_80151233 = 3;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x40) {
            if (g_BattleModels[temp_a1].animId == D_80163784[temp_a1]) {
                g_BattleModels[temp_a1].rootRot.vy += 0x100;
            }
        }
        var_a0 = arg0;
        if (D_80151200[var_a0].D_80151200 & 0x400000 && g_BattleModels[var_a0].animId == D_80163784[var_a0]) {
            if (g_BattleModels[var_a0].defaultRotX == 0) {
                g_BattleModels[var_a0].rootRot.vy = 0x800;
            } else {
                g_BattleModels[var_a0].rootRot.vy = 0;
            }
        }
        var_a0 = arg0;
        if (g_BattleModels[var_a0].specialFlags & 8) {
            if (g_BattleModels[var_a0].unk24 < 0x10) {
                g_BattleModels[var_a0].blendAlpha += 0x80;
            } else {
                g_BattleModels[var_a0].blendAlpha -= 0x80;
            }
            g_BattleModels[arg0].unk24--;
            g_BattleModels[arg0].unk24 &= 0x1F;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleCreateStatusIconPacket);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleAddStatusIconToRender);

static void BattleGet4DigitsFromValue(s16 arg0, s16* arg1) {
    s32 i;

    for (i = 0; i < 4; i++) {
        arg1[3 - i] = arg0 % 10;
        arg0 /= 0xA;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleAddStatusDigitsToRender);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleCreateStatusDigitsPackets);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2704);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2928);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2F20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2FD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3068);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C328C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C33F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3578);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C36B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3950);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3AA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3CA8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3DE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3F44);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C40F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C428C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C44B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C45EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C4814);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C494C);

static void func_800C4B60(s16 arg0) {
    if (D_801621F0[arg0].D_801621F4 == 0) {
        D_801621F0[arg0].D_801621F0 = -1;
        return;
    }
    D_80163C74 = func_800C4DC8(0, D_801621F0[arg0].unkA, 320, 47, &D_800EA25C);
    D_80163C74 = func_800C4DC8(0, D_801621F0[arg0].unkA + 47, 320, 32, &D_800EA258);
    D_80163C74 = func_800C4DC8(0, D_801621F0[arg0].unk8, 320, 32, &D_800EA260);
    D_80163C74 = func_800C4DC8(0, D_801621F0[arg0].unk8 + 32, 320, 47, &D_800EA25C);
    D_801621F0[arg0].unk8 += 4;
    D_801621F0[arg0].unkA -= 4;
    D_801621F0[arg0].D_801621F4--;
}

static void func_800C4D10(void) {
    int arg0;

    arg0 = D_801590D4;
    switch (D_801621F0[arg0].D_801621F2) {
    case 0:
        D_801621F0[arg0].D_801621F4 = 21;
        D_801621F0[arg0].unk8 = 87;
        D_801621F0[arg0].unkA = 8;
        D_801621F0[arg0].D_801621F2++;
    case 1:
        func_800C4B60(arg0);
        break;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C4DC8);

u_long* func_800C4FC8(u8 r, u8 g, u8 b) { return func_800C5040(r, g, b, 1, (u_long*)&g_cDb->unk4080[1]); }

static u_long* func_800C5004(u8 r, u8 g, u8 b) { return func_800C5040(r, g, b, 2, (u_long*)&g_cDb->unk40EC); }

static u_long* func_800C5040(u8 r, u8 g, u8 b, s32 tpage, u_long* ot) {
    DR_MODE* drMode;
    POLY_F4* poly;

    drMode = D_80163C74;
    SetDrawMode(drMode, 1, 0, (tpage & 3) << 5, NULL);
    poly = (POLY_F4*)(drMode + 24);
    SetPolyF4(poly);
    SetSemiTrans(poly, 1);
    poly->r0 = r;
    poly->g0 = g;
    poly->b0 = b;
    poly->x0 = 0;
    poly->y0 = 8;
    poly->x1 = 320;
    poly->y1 = 8;
    poly->x2 = 0;
    poly->y2 = 166;
    poly->x3 = 320;
    poly->y3 = 166;
    addPrim(ot, poly);
    addPrim(ot, drMode);
    return (u_long*)(poly + 1);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5170);

static void func_800C5468(u8 arg0) {
    s32 var_v0_2;
    s32 var_v0;
    u16 temp_a1;

    var_v0 = arg0;
    if (D_80151200[var_v0].D_80151200 & 0x1000) {
        temp_a1 = D_80151200[var_v0].D_8015120C;
        if (!(temp_a1 & 0x80)) {
            D_80151200[var_v0].D_8015120C |= 0x80;
            var_v0_2 = func_800BC04C(func_800C55B8);
            D_801621F0[var_v0_2].D_801621F6 = arg0;
            D_801621F0[var_v0_2].D_801621F4 = 0x10;
            D_801621F0[var_v0_2].D_801621F2 = -0x80;
        }
    } else {
        temp_a1 = D_80151200[var_v0].D_8015120C;
        if (temp_a1 & 0x80) {
            D_80151200[var_v0].D_8015120C = temp_a1 & (~0x80);
            var_v0_2 = func_800BC04C(func_800C55B8);
            var_v0_2 = var_v0_2;
            D_801621F0[var_v0_2].D_801621F6 = arg0;
            D_801621F0[var_v0_2].D_801621F4 = 0x10;
            D_801621F0[var_v0_2].D_801621F2 = 0x80;
        }
    }
}

static void func_800C55B8(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    g_BattleModels[D_801621F0[D_801590D4].D_801621F6].scale += D_801621F0[D_801590D4].D_801621F2;
    D_801621F0[D_801590D4].D_801621F4--;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5694);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C57B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C59B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5ADC);

// reset each slot's first field to -1 (empty)
static void func_800C5BEC(void) {
    s32 fill;
    s32 i;

    fill = -1;
    for (i = 0x17A; i >= 0; i -= 6) {
        *(s16*)&D_800F9DA8[i] = fill;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5C18);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5CC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleSelectPlayerModelFiles);

s32 func_800C60F4(void) { return Savemap.battle_msg_speed / 4 + 4; }

static void func_800C610C(void) {
    while (D_801518DC) {
        BattleCdromReadChain();
    }
}

static void BattleStoreUnitClut(u_long* pTim, s32 palIndex) {
    TIM_IMAGE tim;
    u32* dst;
    s32 i;

    palIndex &= 0xFF;
    dst = D_800F8CF4[palIndex];
    OpenTIM(pTim);
    ReadTIM(&tim);
    for (i = 0; i < 0x18; i++) {
        *dst++ = *tim.caddr++;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C61C0);

// load an image into VRAM
static void func_800C627C(void) {
    s32 i;

    for (i = 0; i < 10; i++) {
        func_800C62F4(i & 0xFF);
    }
    g_BattleModelClutRect.x = 0;
    g_BattleModelClutRect.y = 480;
    g_BattleModelClutRect.w = 16;
    g_BattleModelClutRect.h = 30;
    BattleEnqueueLoadImage(&g_BattleModelClutRect, D_80158D0C);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C62F4);

void func_800C64AC(void) { BattleEffectRegister(func_800C679C); }

void BattleFadeOutUntargetedEnemies(void) {
    s32 i;
    s32 j;

    for (i = START_ENEMY; i < LEN(g_BattleModels); i++) {
        g_BattleSavedSpecialFlags[i] = g_BattleModels[i].specialFlags;
        g_BattleModels[i].animControlFlags |= ANIM_CTRL_FADE_OUT;
    }
    for (i = START_ENEMY; i < LEN(g_BattleModels); i++) {
        if (g_BattleSavedSpecialFlags[i] & BATTLE_MODEL_HIDDEN) {
            g_BattleModels[i].animControlFlags &= ~ANIM_CTRL_FADE_OUT;
        }
        if ((g_BattleCurrentTargetMask >> i) & 1) {
            for (j = START_ENEMY; j < LEN(g_BattleModels); j++) {
                if (D_80163C80[i].vx == D_80163C80[j].vx && D_80163C80[i].vz == D_80163C80[j].vz) {
                    g_BattleModels[j].animControlFlags &= ~ANIM_CTRL_FADE_OUT;
                }
            }
        }
    }
}

void BattleFadeInUntargetedEnemies(void) {
    s32 i;
    s32 j;

    for (i = START_ENEMY; i < LEN(g_BattleModels); i++) {
        if (!((g_BattleCurrentTargetMask >> i) & 1) && !(g_BattleSavedSpecialFlags[i] & BATTLE_MODEL_HIDDEN)) {
            g_BattleModels[i].animControlFlags |= ANIM_CTRL_FADE_IN;
            g_BattleModels[i].specialFlags &= ~BATTLE_MODEL_HIDDEN;
        }
    }
    for (i = START_ENEMY; i < LEN(g_BattleModels); i++) {
        if ((g_BattleCurrentTargetMask >> i) & 1) {
            for (j = START_ENEMY; j < LEN(g_BattleModels); j++) {
                if (i != j && D_80163C80[i].vx == D_80163C80[j].vx && D_80163C80[i].vz == D_80163C80[j].vz) {
                    g_BattleModels[j].animControlFlags &= ~ANIM_CTRL_FADE_IN;
                    g_BattleModels[j].specialFlags &= ~BATTLE_MODEL_HIDDEN;
                }
            }
        }
    }
}

void func_800C679C(void) {
    s32 i;

    g_BattleEffectModelNotSummon = 0;
    switch (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297A) {
    case 0:
        g_BattleEffectSlots[g_BattleEffectCursor].D_8016297A++;
        g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C = 14;
        g_BattleModelFadeFrames = 14;
        func_800BBA40(0x29);
        for (i = 0; i < NUM_PARTY; i++) {
            g_BattleModels[i].animControlFlags |= ANIM_CTRL_FADE_OUT;
        }
        if (D_800FA6D0 == 4) {
            BattleFadeOutUntargetedEnemies();
        }
        break;
    case 1:
        if (g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C == 0) {
            D_800FAFDC = 1;
            g_BattleModels[0].specialFlags |= BATTLE_MODEL_INACTIVE;
            g_BattleModels[1].specialFlags |= BATTLE_MODEL_INACTIVE;
            g_BattleModels[2].specialFlags |= BATTLE_MODEL_INACTIVE;
            g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C = 45;
            g_BattleEffectSlots[g_BattleEffectCursor].D_8016297A++;
            switch (g_BattleModels[D_801590CC].attackEffectId) {
            case SUMMON_VAHAMUT:
                D_800F57D0 = func_801B0040_5(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_ODIN2:
                D_800F57D0 = func_801B0040_3(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_ODIN1:
                D_800F57D0 = func_801B0040_13(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_HADES:
                D_800F57D0 = func_801B0040_10(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_CHOCO0:
                D_800F57D0 = MAGIC_Choco0(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_TITAN:
                D_800F57D0 = func_801B0040_2(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_RIVA:
                D_800F57D0 = func_801B0040_4(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_SIVA:
                D_800F57D0 = func_801B0054(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_IFLEET:
                D_800F57D0 = func_801B0040(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_LAMU:
                D_800F57D0 = func_801B0038(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_PHOENIX:
                D_800F57D0 = func_801B0040_8(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_KUJATA:
                D_800F57D0 = func_801B0040_6(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_TUPON:
                D_800F57D0 = func_801B0040_11(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_ALEX:
                D_800F57D0 = func_801B0040_7(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_VAHAMUT2:
                D_800F57D0 = func_801B0040_9(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_VAHAMUT0:
                D_800F57D0 = func_801B0060(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_DEBUCHO:
                D_800F57D0 = func_801B0040_12(g_BattleCurrentTargetMask, D_801590CC);
                break;
            case SUMMON_KNIGHTS:
                func_801B0050(g_BattleCurrentTargetMask, D_801590CC);
                g_BattleEffectSlots[g_BattleEffectCursor].D_80162978 = -1;
                return;
            }
            BattleLoadEffectModel();
            g_BattleModels[EFFECT_MODEL_SLOT].specialFlags |= BATTLE_MODEL_NO_SHADOW;
            g_BattleEffectSlots[g_BattleEffectCursor].D_80162978 = -1;
        } else {
            g_BattleEffectSlots[g_BattleEffectCursor].D_8016297C--;
        }
    }
}

void BattleStartEffectWithModel(s32 targetMask, s32 callbackArg) {
    g_BattleEffectModelNotSummon = 1;
    switch (g_BattleModels[D_801590CC].currentActionId) {
    case CMD_LIMIT:
        switch (g_BattleModels[D_801590CC].attackEffectId) {
        case 61: // DEATH.BIN
            D_800F57D0 = func_801B0054_9(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 58: // TSOL.BIN
            D_800F57D0 = func_801B0054_2(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 57: // MDANCE.BIN
            D_800F57D0 = func_801B0054_3(g_BattleCurrentTargetMask, D_801590CC);
            break;
        }
        break;
    case CMD_ENEMY_SKILL:
        switch (g_BattleModels[D_801590CC].attackEffectId) {
        case 20: // SENNKOKU.BIN
            D_800F57D0 = func_801B0054_4(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 21: // SENNKOKU.BIN
            D_800F57D0 = func_801B0084(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 18: // CONF.BIN
            D_800F57D0 = func_801B04C0(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 5: // RAISE.BIN
            D_800F57D0 = func_801B00B4(g_BattleCurrentTargetMask, D_801590CC);
            break;
        }
        break;
    case CMD_ENEMY_ATTACK:
        switch (g_BattleModels[D_801590CC].attackEffectId) {
        case 50: // JOKER1.BIN
            D_800F57D0 = func_801B0054_5(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 144: // RAISE.BIN
            D_800F57D0 = func_801B00E8(g_BattleCurrentTargetMask, D_801590CC);
            break;
        }
        break;
    case CMD_MAGIC:
        switch (g_BattleModels[D_801590CC].attackEffectId) {
        case 7: // RAISE.BIN
            D_800F57D0 = func_801B0054_6(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 8: // RAISE.BIN
            D_800F57D0 = func_801B0084_2(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 24: // DEATH.BIN
            D_800F57D0 = func_801B0054_7(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 12: // CONF.BIN
            D_800F57D0 = func_801B0498(g_BattleCurrentTargetMask, D_801590CC);
            break;
        }
        break;
    case CMD_ITEM:
        switch (g_BattleModels[D_801590CC].attackEffectId) {
        case 24: // DEATH.BIN
            D_800F57D0 = func_801B0054_8(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 29: // CONF.BIN
            D_800F57D0 = func_801B0498(g_BattleCurrentTargetMask, D_801590CC);
            break;
        }
        break;
    }
    g_BattleModels[EFFECT_MODEL_SLOT].unk26 = 0;
    BattleLoadEffectModel();
    g_BattleModels[EFFECT_MODEL_SLOT].specialFlags |= BATTLE_MODEL_NO_SHADOW;
}

void BattleFadeInAfterSummon(void) {
    s32 i;
    s16 timer;

    if (D_801621F0[D_801590D4].D_801621F2 == 0) {
        if (g_BattleEffectCount == 0) {
            for (i = 0; i < NUM_PARTY; i++) {
                g_BattleModels[i].animControlFlags |= ANIM_CTRL_FADE_IN;
                if (Savemap.partyID[i] != 0xFF) {
                    g_BattleModels[i].specialFlags &= ~BATTLE_MODEL_INACTIVE;
                }
            }
            if (D_800FA6D0 == 4) {
                BattleFadeInUntargetedEnemies();
            }
            g_BattleModelFadeFrames = 14;
            D_801621F0[D_801590D4].D_801621F2 = 1;
            D_801621F0[D_801590D4].D_801621F4 = 14;
        }
    } else {
        timer = D_801621F0[D_801590D4].D_801621F4;
        if (timer == 0) {
            D_801621F0[D_801590D4].D_801621F0 = -1;
            D_800FAFDC = 0;
            g_BattleModels[EFFECT_MODEL_SLOT].unk26 = 1;
        } else {
            D_801621F0[D_801590D4].D_801621F4 = timer - 1;
        }
    }
}

void BattleFadeOutEffectModel(void) {
    if (D_801621F0[D_801590D4].D_801621F2 == 0) {
        g_BattleModelFadeFrames = 14;
        g_BattleModels[EFFECT_MODEL_SLOT].animControlFlags |= ANIM_CTRL_FADE_OUT;
        D_801621F0[D_801590D4].D_801621F2 = 1;
        D_801621F0[D_801590D4].D_801621F4 = 14;
    } else if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        g_BattleModels[EFFECT_MODEL_SLOT].specialFlags |= BATTLE_MODEL_INACTIVE;
        g_BattleModels[EFFECT_MODEL_SLOT].specialFlags &= ~BATTLE_MODEL_NO_SHADOW;
        D_801621F0[D_801590D4].D_801621F0 = -1;
        g_BattleModels[EFFECT_MODEL_SLOT].unk26 = 1;
        return;
    } else {
        D_801621F0[D_801590D4].D_801621F4--;
    }
    BattleModelStartFades(EFFECT_MODEL_SLOT);
    func_800C74A4();
    if (!(D_80153BDD & BATTLE_MODEL_INACTIVE)) {
        func_800BA598(EFFECT_MODEL_SLOT);
    }
}

void BattleEffectModelTick(void) {
    if (g_BattleEffectModelState == EFFECT_MODEL_ENDING) {
        g_BattleEffectSlots[g_BattleEffectCursor].D_80162978 = -1;
        if (g_BattleEffectModelNotSummon == 0) {
            func_800BC04C(BattleFadeInAfterSummon);
        } else {
            func_800BC04C(BattleFadeOutEffectModel);
        }
    }
    if (g_BattleEffectModelState == EFFECT_MODEL_STARTING) {
        g_BattleEffectModelState = EFFECT_MODEL_RUNNING;
        D_801031E0 = 1;
        g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vx = g_BattleEffectModelStartPos.vx;
        g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vy = g_BattleEffectModelStartPos.vy;
        g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vz = g_BattleEffectModelStartPos.vz;
        g_BattleModels[EFFECT_MODEL_SLOT].rootRot.vy = g_BattleEffectModelStartRotY;
    }
    BattleModelStartFades(EFFECT_MODEL_SLOT);
    func_800C74A4();
    if (g_BattleEffectModelNotSummon && g_BattleEffectSlots[g_BattleEffectCursor].D_8016297A == 0) {
        g_BattleEffectSlots[g_BattleEffectCursor].D_8016297A = 1;
    } else if (!(D_80153BDD & BATTLE_MODEL_INACTIVE)) {
        func_800BA598(EFFECT_MODEL_SLOT);
    }
}

static void func_800C74A4(void) {
    if (!(g_BattleModels[EFFECT_MODEL_SLOT].specialFlags & BATTLE_MODEL_INACTIVE)) {
        BattleExecuteUnitAnimScript(
            EFFECT_MODEL_SLOT, D_800F57D0->header.offsets[1], &D_800F57D0->header.offsets[2], D_800F57D0);
    }
}

void BattleLoadEffectModel(void) {
    EffectModelHeader* header;
    s32* offsets;
    u8* bones;
    s32 i;

    header = &D_800F57D0->header;
    if (D_801517BC == 0) {
        header->offsets[1] += (s32)D_800F57D0;
        offsets = (s32*)header->offsets[1];
        for (i = 0; i < 8; i++) {
            *offsets++ += header->offsets[1];
        }
        BattleUnitInitBonesAndMatrixes(
            EFFECT_MODEL_SLOT, D_800F57D0->bytes + header->offsets[EFFECT_MODEL_SKELETON], 0);
        g_BattleModels[EFFECT_MODEL_SLOT].modelSetting1 = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].modelSetting2 = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].deathType = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].scale = 0x1000;
        D_80151360.unk2F = 0;
        D_80151360.unk2C = 0;
        D_80151360.unk0 = 0x1C0;
        D_80151360.unk2 = 0x200;
        D_80151360.unk4 = 0x1C0;
        D_80151360.unk6 = 0;
        for (i = 0; i < LEN(D_80151360.unkA); i++) {
            D_80151360.unkA[i] = D_800EA4F4[i];
            D_80151360.unk16[i] = D_800EA4F4[i + 6];
        }
        D_80153BCE = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].blendAlpha = 0;
        D_80151360.unk2A = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vz = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vy = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vx = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].rootRot.vz = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].rootRot.vx = 0;
        g_BattleModels[EFFECT_MODEL_SLOT].rootRot.vy = 0;
        bones = g_BattleModels[EFFECT_MODEL_SLOT].boneIndices;
        for (i = LEN(g_BattleModels[EFFECT_MODEL_SLOT].boneIndices) - 1; i >= 0; i--) {
            bones[i] = 0;
        }
        BattleSetLoadTimToVram((u_long*)(D_800F57D0->bytes + header->offsets[header->numOffsets - 1]), 0, 0, 0);
    }
    g_BattleModels[EFFECT_MODEL_SLOT].specialFlags = 0x80;
    g_BattleModels[EFFECT_MODEL_SLOT].animControlFlags = 1;
    D_80163787 = 0;
    g_BattleModels[EFFECT_MODEL_SLOT].animId = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleGetModelBoneNumAndInitBones);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleGetWeaponBoneNumAndInitBones);
