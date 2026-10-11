//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"
#include "battle_gte_matrix.h"
#include <libetc.h>
#include <libgpu.h>

void BattleLoadPlayerFinish(void);
void BattleGetModelBoneNumAndInitBones(s16* count, u16* data, s16 actor);
void BattleGetWeaponBoneNumAndInitBones(s16* count, u16* data, s16 actor);
void BattleBoneSetParentMatrix(MATRIX* parent, BattleModelSub* bone);

void BattleQueue1Execute(void);
void func_800B8FCC(s32 actor);
void func_800BA11C(s32 index, s32 step, s16 initial, s16 reset);
void func_800BA2BC(s32 actor);
void BattleUpdateModelMatrixesWithSelfAndParent(s32 actor);
void func_800BBA84(u16 sound, s32 actor, s32 pan);
void CopyCameraStartEndFromBattleSetup(void);
void func_800BE49C(void);
void func_800BE69C(s16 actor, s32 categoryArg, u8* sequence, s32 flags);
void func_800BE86C(s16 actor, s32 categoryArg, u8* sequence);
void func_800BFB88(void);
void func_800BFDA0(void);
void func_800BFF88(s16 actor, s16 bone, s32 category, s32 flags);
void func_800C0088(s16 actor, s16 bone, s32 categoryArg);
void func_800C14C0(s32 actorArg);
void func_800C2864(s32 arg0);
void func_800C2928(void);
s32 func_800C2F20(s32 number, s16* digits);
void func_800C328C(void);
void func_800C3578(void);
void func_800C3950(void);
void func_800C3CA8(void);
void func_800C3DE4(void);
void func_800C40F4(void);
void func_800C44B4(void);
void func_800C4814(void);
void func_800C5864(void);

void func_800C494C(void);
static void func_800C5694(void);
static void BattleEffectModelTick(void);
void BattleNormalStartSeq(void);
void BattleEnemyPlayInitAnims(void);
void func_800B383C(void);
static void BattleLoadEnemyFinish(void);
static void BattleLoadThirdEnemy(void);
static void BattleLoadFirstPlayer(void);
void func_800B430C(void);
void BattlePlayerModifyDefaultPosByFormation(void);
void BattlePlayerInitModelWithSettings(u8 arg0);
void BattleEnemyInitModelWithSettings(u8 arg0);
void BattleEnemyModelsUpdateBonesPosClut(void);
void func_800B5AAC(s32 arg0);
static void func_800B5FC4(s16 arg0);
void BattleQueue1InitPlayAnim(void);
void BattleQueue1AddNewTarg(void);
void BattleQueue1UpdateTargMasks(void);
static void func_800B7F6C(void);
static void func_800B8234(s32 arg0);
static void func_800B8268(void);
static void func_800B83C4();
void func_800B8944(s32 actor, s32 target);
void func_800B8B48(void);
void func_800B8EE4(void);
void func_800B91CC(void);
void func_800BA360(s32 index, u32** ot, s32 depth, s32 flags);
static void BattleUpdateMatrixWithSelfAndParentAndSetToGte(BattleModelSub* modelSub);
static void BattleUpdateMatrixWithScaleAndSetToGte(MATRIX* m, VECTOR* v);
void BattleModelUpdateAllBonesHeight(s16 actor, s16 height);
void func_800BB4F8(void);
void func_800BB90C(void);
void func_800BB944(void);
void func_800BB978(void);
void func_800BB9FC(s32 arg0);
static void func_800BBA40(s32 arg0);
static void func_800BBDF8(void);
static void func_800BC72C(void);
static s16 func_800BFA98(u8* arg0, s32 arg1);
static s16 func_800BFB10(u8* arg0, s32 arg1);
static void func_800C018C(s16 arg0, s16 arg1, s32 arg2, s32 arg3);
static void func_800C0254(s16 arg0, s16 arg1);
static s16 func_800C03B8(s16 arg0, s16 arg1);
static s32 func_800C03FC(s32 arg0, s32 arg1);
static s32 func_800C1304(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_800C1394(s32 actorArg);
static s32 func_800C169C(u8 arg0);
static void func_800C17A0(s32 arg0, s32 arg1);
void* BattleCreateStatusIconPacket(u_long** ot, SVECTOR* pos, s16 u, s16 v);
void BattleAddStatusIconToRender(s32 arg0, s16 flags);
void BattleAddStatusDigitsToRender(s32 arg0, u8 arg1);
void func_800C2FD4(s32 actor, s16 sound, s32 enabled);
void func_800C33F0(s32 effectArg);
void func_800C3AA0(s32 effectArg);
void func_800C3F44(s32 effectArg);
void func_800C428C(s32 effectArg);
void func_800C45EC(s32 effectArg);
static void func_800C4B60(s16 arg0);
u_long* func_800C4FC8(u8 r, u8 g, u8 b);
void func_800C57B0(void);
void* func_800C59B8(u_long* ot, s16 texture);
void* func_800C5ADC(u_long** ot);
void func_800C5C18(s32 id, s32 delay, s32 duration, s32 playSound);
void func_800C5CC0(void);
s32 func_800C60F4(void);
static void func_800C610C(void);
void func_800C61C0(void);
void func_800C64AC(void);
void BattleStartEffectWithModel(s32 targetMask, s32 callbackArg);
void BattleFadeOutEffectModel(void);
void* BattleCreateStatusDigitsPackets(u_long** ot, SVECTOR* pos, u8 digit1, u8 digit2);
void* func_800C2704(u_long** ot, s16 x, s16 y, s16 u, u16 v, u16 w, u16 h, s32 flags);

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
s32 BattleDetachedRegister(void (*callback)(void));
s32 BattleMovementRegister(void (*callback)(void));
static s32 BattleCameraRegister(void (*callback)(void));
static void BattleCallbacksReset(void);
static void BattleCameraResetCallbacks(void);
static void BattleEffectUpdate(void);
static void BattleMovementUpdate(void);
static void BattleDetachedUpdate(void);
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
void BattleLoadPlayerModel(s32);
void BattleLoadPlayerTexture(s32);
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
static u_long* func_800C5004(u8 r, u8 g, u8 b);
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
    BattleDetachedRegister(func_800C4D10);
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
            if (D_800F7DF4.count == (u8)D_80166F64 && D_801518DC == 0) {
                BattleLoadSeffects();
                BattleParseEnemyModels();
                D_80163C7C = 6;
            }
            break;
        case 6:
            BattleUpdateRender();
            BattleEnemyInitBonesAndAnims();
            for (i = 4; i < D_800F7DF4.enemyCount + 4; i++) {
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

void BattleEnemyPlayInitAnims(void) {
    s32 actor;
    s32 steps;
    s32 i;
    EffectModel* data;
    if (!(g_BattleData.activeEncounter.setup.flags & 2)) {
        steps = 4;
        for (actor = 5; actor < D_800F7DF4.enemyCount + 4; actor++) {
            for (i = 0; i < steps; i++) {
                data = (EffectModel*)D_800F8384[D_800F7DF4.enemies[actor - 4].modelIndex];
                BattleExecuteUnitAnimScript(actor, data->header.offsets[1] + 0x68, &data->header.offsets[2], data);
            }
            steps += 4;
        }
    }
}

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
void func_800B383C(void) {
    func_800B3FFC();
    SysCdromLoadLzs(*&D_800E7D50[D_800FA6D0].loc, *&D_800E7D50[D_800FA6D0].len, (u32*)0x801A0000, NULL);
    SysCdromLoadLzs(*&D_800E7D98[g_BattleData.activeEncounter.setup.stageID].loc,
                    *&D_800E7D98[g_BattleData.activeEncounter.setup.stageID].len, (u32*)0x801B0000, NULL);
    func_800BB4F8();
    func_8001C3CC(&D_801590E4, (void*)0x801B0000, 0x8D04);
    DrawSync(0);
}

// load stage entry i (D_800F7DF4.modelIds[0]) into VRAM staging via SysCdromStartLoadLzs
static void BattleLoadFirstEnemy(void) {
    s32 i = D_800F7DF4.modelIds[0];

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
// record where the next part lands (D_800F8384[n + 4] = D_800F8384[n + 3] + size),
// advance the D_80166F64 phase counter BattleNormalStartSeq waits on, and queue the
// next part's read only while entries remain (D_800F7DF4 is the entry count)
static void BattleLoadThirdEnemy(void) {
    s32 size;
    s32 i;

    BattleLoadEnemyTexture(1);
    size = BattleLoadEnemyModel(1);
    D_80166F64 = 2;
    D_800F8384[5] = size + D_800F8384[4];
    if (D_800F7DF4.count >= 3U) {
        i = D_800F7DF4.modelIds[2];
        SysCdromStartLoadLzs(*&D_800E8050[i].loc, *&D_800E8050[i].len, (u_long*)0x801B0000, BattleLoadEnemyFinish);
        BattleCdromReadChain();
    }
}

// second link of the chain: unpack part 0 out of the staging buffer, then
// queue part 1's read
static void BattleLoadSecondEnemy(void) {
    s32 size;
    s32 i;

    D_800F8384[3] = D_80130200;
    BattleLoadEnemyTexture(0);
    size = BattleLoadEnemyModel(0);
    D_80166F64 = 1;
    D_800F8384[4] = size + D_800F8384[3];
    if (D_800F7DF4.count >= 2U) {
        i = D_800F7DF4.modelIds[1];
        SysCdromStartLoadLzs(*&D_800E8050[i].loc, *&D_800E8050[i].len, (u_long*)0x801B0000, BattleLoadThirdEnemy);
        BattleCdromReadChain();
    }
}

static void BattleLoadSecondPlayer(void) {
    s16* s0;
    u8** dst;
    s16 v1;
    s16 cmp;

    s0 = &D_800FA9C4[0].actor;
    v1 = *s0;
    dst = &D_800F8384[v1];
    *dst = D_80103200 + v1 * 0xF000;
    BattleLoadPlayerTexture(*s0);
    BattleLoadPlayerModel(*s0);
    cmp = D_800FA9C4[1].file;
    if (cmp != 0xC8) {
        SysCdromStartLoadLzs(*&D_800E8068[cmp].loc, *&D_800E8068[cmp].len, (u_long*)0x801B0000, BattleLoadThirdPlayer);
        BattleCdromReadChain();
        return;
    }
    D_80166F64 = 3;
}

void BattleLoadThirdPlayer(void) {
    s16* s0;
    u8** dst;
    s16 v1;
    s16 cmp;

    s0 = &D_800FA9C4[1].actor;
    v1 = *s0;
    dst = &D_800F8384[v1];
    *dst = D_80103200 + v1 * 0xF000;
    BattleLoadPlayerTexture(*s0);
    BattleLoadPlayerModel(*s0);
    cmp = D_800FA9C4[2].file;
    if (cmp != 0xC8) {
        SysCdromStartLoadLzs(*&D_800E8068[cmp].loc, *&D_800E8068[cmp].len, (u_long*)0x801B0000, BattleLoadPlayerFinish);
        BattleCdromReadChain();
        return;
    }
    D_80166F64 = 3;
}

void BattleLoadPlayerFinish(void) {
    s16* slot;
    u8** dst;
    s16 actor;

    slot = &D_800FA9C4[2].actor;
    actor = *slot;
    dst = &D_800F8384[actor];
    *dst = D_80103200 + actor * 0xF000;
    BattleLoadPlayerTexture(*slot);
    BattleLoadPlayerModel(*slot);
    D_80166F64 = 3;
}

static void BattleLoadFirstPlayer(void) {
    Yamada* y;
    u_long* dst;

    dst = (u_long*)0x801B0000;
    BattleSetLoadTimToVram(dst, 0, 0, 0);
    y = &D_800E8068[D_800FA9C4[0].file];
    SysCdromStartLoadLzs(y->loc, *&D_800E8068[D_800FA9C4[0].file].len, dst, BattleLoadSecondPlayer);
    BattleCdromReadChain();
}

static void BattleLoadSeffects(void) {
    BattleSelectPlayerModelFiles();
    D_800F8384[6] = (u8*)D_800EA50C;
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
        g_BattleModels[i].ready = 1;
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
    u8* p = (u8*)&D_800F7DF4;

    for (i = 0; i < (s32)sizeof(g_BattleData.activeEncounter.formation); i += sizeof(FormationEntry)) {
        if (((FormationEntry*)((u8*)g_BattleData.activeEncounter.formation + i))->enemyID == arg0) {
            *p += 1;
            return 0;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3FFC);

void func_800B430C(void) {
    BattleModelSub* bone;
    BattleModel* model;
    s32 i;
    u8 stageType;
    BattleModelSub* bone3;
    u8* offsets;

    i = 0;
    D_80163B44[0] = 0;
    D_80163B44[1] = 0x10;
    g_BattleWorldView.u.sub.trans.vz = 0;
    g_BattleWorldView.u.sub.trans.vy = 0;
    g_BattleWorldView.u.sub.trans.vx = 0;
    g_BattleWorldView.rot.vz = 0;
    g_BattleWorldView.rot.vy = 0;
    g_BattleWorldView.rot.vx = 0;
    for (; i < NUM_BATTLE_ACTOR; i++) {
        bone = &D_800F8158[i];
        bone->rot.vz = 0;
        bone->rot.vy = 0;
        D_800F8158[i].rot.vx = 0;
        bone->trans.vz = 0;
        bone->trans.vy = 0;
        D_800F8158[i].trans.vx = 0;
    }
    BattleBoneSetParentMatrix(&g_BattleWorldView.m, D_800F8158);
    offsets = (u8*)&D_801590E4 + sizeof(s32);
    offsets -= -D_801590E4.modelHeader.layoutOffset;
    stageType = offsets[-4];
    switch (stageType) {
    case 2:
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &D_800F8158[1]);
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &D_800F8158[2]);
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &D_800F8158[3]);
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &D_800F8158[4]);
        D_800F8158[4].trans.vy = 0x30D4;
        break;
    case 0:
        i = 0;
        for (; i < NUM_BATTLE_ACTOR; i++) {
            BattleBoneSetParentMatrix(&g_BattleWorldView.m, &D_800F8158[i]);
        }
        D_800F8158[2].trans.vz = 30000;
        D_800F8158[3].trans.vz = 20000;
        D_800F8158[4].trans.vz = 10000;
        D_800F8158[6].trans.vz = -10000;
        D_800F8158[7].trans.vz = -20000;
        D_800F8158[5].trans.vz = 0;
        D_800F8158[8].trans.vz = 25536;
        break;
    case 4:
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &D_800F8158[6]);
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &D_800F8158[5]);
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &D_800F8158[4]);
        /* fallthrough */
    case 3:
        bone3 = &D_800F8158[1];
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &bone3[0]);
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &bone3[1]);
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, &bone3[2]);
        break;
    }
    i = 0;
    for (; i < NUM_BATTLE_ACTOR; i++) {
        model = &g_BattleModels[i];
        model->rootTrans.vz = 0;
        model->rootTrans.vy = 0;
        g_BattleModels[i].rootTrans.vx = 0;
        D_80151200[i].D_80151234 = 0;
    }
    D_800F5B70.cd = 0x1F;
    D_800F5B70.b = 0;
    D_800F5B70.g = 0;
    D_800F5B70.r = 0;
    D_800F5B74 = 0;
    if (g_BattleData.activeEncounter.setup.stageID == 0x3A) {
        func_800C57B0();
    }
}

extern ShortVectorXYZ D_800E8E84[9][NUM_PARTY];
extern s16 D_800E8F94[9][NUM_PARTY];
void BattlePlayerModifyDefaultPosByFormation(void) {
    s32 i;
    s32 formation;
    i = 0;
    formation = D_800FA6D0;
    do {
        if (g_BattleData.actors[i].D_801636BE & 1) {
            if (D_800E8F94[formation][i] == 0) {
                D_800E8E84[formation][i].vz += 0x204;
            } else {
                D_800E8E84[formation][i].vz -= 0x204;
            }
        }
        i++;
    } while (i < NUM_PARTY);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattlePlayerSetDefaultRot);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattlePlayerModelsUpdateBonesPos);

typedef struct {
    u32 numOffsets;
    u32 skeletonOffset;
    u32 settingsAddress;
} BattleModelFileHeader;

typedef struct {
    u16 animDescOffset;
    u8 deathType;
    u8 unk3;
    u16 collisionRadius;
    u16 settings[3];
    u16 height;
    u16 unkE;
    u16 unk10;
    u8 boneIndices[16];
    u16 unk22;
    u32 dataOffsets[8];
    u16 values44[4];
    u16 values4C[6];
    u16 values58[6];
    u16 value64, unk66;
} BattleModelSettings;

void BattlePlayerInitModelWithSettings(u8 arg0) {
    s16 actor;
    u8** data;
    BattleModelSettings* settings;
    s32 i;
    s32 index;
    u16 height;
    actor = arg0 & 0xFF;
    if (g_BattleData.actors[actor].charId != -1) {
        index = D_80151200[actor].D_80151232;
        data = &D_800F8384[index];
        BattleUnitInitBonesAndMatrixes(
            arg0 & 0xFF, (void*)(((BattleModelFileHeader*)*data)->skeletonOffset + (u_long)*data), 0);
        settings = (BattleModelSettings*)(u_long)(u32)((BattleModelFileHeader*)*data)->settingsAddress;
        g_BattleModels[actor].deathType = settings->deathType;
        g_BattleModels[actor].collisionRadius = settings->collisionRadius;
        g_BattleModels[actor].modelSetting1 = settings->settings[0];
        g_BattleModels[actor].modelSetting2 = settings->settings[1];
        g_BattleModels[actor].modelSetting3 = settings->settings[2];
        g_BattleModels[actor].animDescOffset = settings->animDescOffset;
        D_80151200[actor].height = settings->height;
        D_80151200[actor].unk28 = settings->unkE;
        height = settings->unk10;
        D_80151200[actor].D_8015122E = 0;
        D_80151200[actor].unk2A = height;
        D_80151200[actor].D_80151230 = settings->value64;
        for (i = 0; i < LEN(settings->values44); i++) {
            D_80151200[actor].values4[i] = settings->values44[i];
        }
        for (i = 0; i < LEN(settings->values4C); i++) {
            D_80151200[arg0 & 0xFF].valuesE[i] = settings->values4C[i];
            D_80151200[arg0 & 0xFF].values1A[i] = settings->values58[i];
        }
        actor = arg0 & 0xFF;
        if (g_BattleModels[actor].deathType & 0x80) {
            BattleBoneSetParentMatrix(&g_BattleModels[actor].stageMatrix, D_800FA6D8[actor].unk8);
            BattleGetWeaponBoneNumAndInitBones(&D_800FA6D8[actor].unk3C, D_80163F34[actor], actor);
        }
        for (index = 0; index < LEN(settings->boneIndices); index++) {
            g_BattleModels[actor].boneIndices[index] = settings->boneIndices[index];
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleParseEnemyModels);

typedef struct {
    u32 numOffsets;
    // GCC 2.6 uses a zero-length array for the variable-length offset table.
    u32 offsets[0];
} BattleEnemyOffsets;

typedef struct {
    s16 animDescOffset;
    u8 deathType;
    u8 unk3;
    u16 collisionRadius;
    u16 settings[3];
    u16 height;
    u16 unkE;
    u16 unk10;
    u8 boneIndices[16];
    u16 unk22;
    u32 dataOffsets[8];
    u16 values44[4];
    u16 values4C[6];
    u16 values58[6];
    u16 value64, unk66;
} BattleEnemyModelSettings;

void BattleEnemyInitModelWithSettings(u8 arg0) {
    s16 actor;
    s32 model;
    u8** data;
    BattleEnemyModelSettings* settings;
    s32 i;
    u8* flags;
    BattleModel* modelState;
    actor = arg0 & 0xFF;
    if (D_80151200[actor].D_80151232 == 6) {
        model = 6;
    } else {
        model = D_800F7DF4.enemies[actor - START_ENEMY].modelIndex;
    }
    data = D_800F8384;
    data += model;
    BattleUnitInitBonesAndMatrixes(
        arg0 & 0xFF, (void*)(((BattleModelFileHeader*)*data)->skeletonOffset + (u_long)*data), 1);
    arg0 &= 0xFF;
    actor = arg0;
    settings = (BattleEnemyModelSettings*)(u_long)(u32)((BattleModelFileHeader*)*data)->settingsAddress;
    g_BattleModels[actor].deathType = settings->deathType;
    g_BattleModels[actor].collisionRadius = settings->collisionRadius;
    g_BattleModels[actor].modelSetting1 = settings->settings[0];
    g_BattleModels[actor].modelSetting2 = settings->settings[1];
    g_BattleModels[actor].modelSetting3 = settings->settings[2];
    modelState = &g_BattleModels[actor];
    modelState->colorB = 0;
    modelState->colorG = 0;
    g_BattleModels[actor].colorR = 0;
    g_BattleModels[actor].animDescOffset = *(u16*)((u8*)&D_800F7DF4 + (model - 2) * sizeof(s32));
    if (settings->animDescOffset != 0) {
        g_BattleModels[actor].deathType |= 0x40;
    }
    D_80151200[actor].height = settings->height;
    D_80151200[actor].unk28 = settings->unkE;
    D_80151200[actor].unk2A = settings->height - 0x384;
    D_80151200[actor].D_8015122E = 0;
    D_80151200[actor].D_80151230 = settings->value64;
    for (i = 0; i < LEN(settings->values44); i++) {
        D_80151200[actor].values4[i] = settings->values44[i];
    }
    for (i = 0; i < LEN(settings->values4C); i++) {
        D_80151200[arg0 & 0xFF].valuesE[i] = settings->values4C[i];
        D_80151200[arg0 & 0xFF].values1A[i] = settings->values58[i];
    }
    actor = arg0 & 0xFF;
    g_BattleData.actors[actor].D_801636BC = g_BattleModels[actor].deathType & 0x3F;
    if (g_BattleModels[actor].deathType & 0x80) {
        BattleBoneSetParentMatrix(&g_BattleModels[actor].stageMatrix, D_800FA6D8[actor].unk8);
        BattleGetWeaponBoneNumAndInitBones(
            &D_800FA6D8[actor].unk3C,
            (u16*)(((BattleEnemyOffsets*)D_800F8384[model])
                       ->offsets[((BattleEnemyOffsets*)D_800F8384[model])->numOffsets - 2] +
                   (u_long)D_800F8384[model]),
            actor);
    }
    model = 0;
    flags = g_BattleModels[actor].boneIndices;
    for (; model < LEN(settings->boneIndices); model++) {
        *flags++ = settings->boneIndices[model];
    }
}

void BattleEnemyModelsUpdateBonesPosClut(void) {
    s32 loaded;
    u8* loadCount;
    s32* modelIds;
    s32 normalHeight;
    s32 specialHeight;
    s32 currentSpecial;
    s32 currentNormal;
    s32 searchActor;
    s32 heightActor;
    s32 modelOffset;
    s32 searchOffset;
    s32 positionOffset;
    MATRIX* parent;
    BattleModelSub* bones;
    s16* clut;
    u16 z;
    s32 step;
    s32 count;
    loadCount = &D_800F7DF4.count;
    loaded = 0;
    count = *loadCount;
    if (loaded < count) {
        modelIds = (s32*)((u8*)&D_800F7DF4 + loaded * sizeof(s32));
        // Skip the count and padding word before the model IDs.
        modelIds++;
        normalHeight = (loaded + 18) * 0x10000;
        specialHeight = (loaded + 19) * 0x10000;
        for (; loaded < D_800F7DF4.count; loaded++) {
            searchActor = 4;
            currentSpecial = specialHeight;
            currentNormal = normalHeight;
            heightActor = 0x40000;
            searchOffset = 4 * sizeof(BattleModel);
        findModel:
            if (((BattleModel*)((u8*)g_BattleModels + searchOffset))->animDescOffset == *modelIds) {
                if (g_BattleData.activeEncounter.setup.stageID == 0x4E) {
                    BattleModelUpdateAllBonesHeight(heightActor >> 16, currentSpecial >> 16);
                    goto nextLoaded;
                } else {
                    BattleModelUpdateAllBonesHeight(heightActor >> 16, currentNormal >> 16);
                    goto nextLoaded;
                }
            }
            step = 0x10000;
            heightActor += step;
            searchActor++;
            searchOffset += sizeof(BattleModel);
            if (searchActor < 10) {
                goto findModel;
            }
        nextLoaded:
            step = 0x10000;
            normalHeight += step;
            specialHeight += step;
            modelIds++;
        }
    }
    g_BattleModels[3].clutOffset = 0x240;
    if (g_BattleData.activeEncounter.setup.stageID == 0x4E) {
        g_BattleModels[4].clutOffset = 0;
        loaded = 4;
    } else {
        g_BattleModels[4].clutOffset = 0x300;
        loaded = 4;
    }
    positionOffset = 4 * sizeof(ShortVectorXYZ);
    clut = &g_BattleModels[5].clutOffset;
    bones = &g_BattleModels[4].boneTransforms[0];
    parent = &g_BattleModels[4].stageMatrix;
    modelOffset = 4 * sizeof(BattleModel);
    *clut = 0x3C0;
    g_BattleModels[6].clutOffset = 0x480;
    g_BattleModels[7].clutOffset = 0x540;
    g_BattleModels[8].clutOffset = 0x600;
    g_BattleModels[9].clutOffset = 0x6C0;
    do {
        BattleBoneSetParentMatrix(parent, &bones[49]);
        bones = (BattleModelSub*)((u8*)bones + sizeof(BattleModel));
        parent = (MATRIX*)((u8*)parent + sizeof(BattleModel));
        ((BattleModel*)((u8*)g_BattleModels + modelOffset))->rootTrans.vx =
            ((ShortVectorXYZ*)((u8*)D_80163C80 + positionOffset))->vx;
        loaded++;
        ((BattleModel*)((u8*)g_BattleModels + modelOffset))->rootTrans.vy =
            ((ShortVectorXYZ*)((u8*)D_80163C80 + positionOffset))->vy;
        z = ((ShortVectorXYZ*)((u8*)D_80163C80 + positionOffset))->vz;
        positionOffset += sizeof(ShortVectorXYZ);
        ((BattleModel*)((u8*)g_BattleModels + modelOffset))->rootTrans.vz = z;
        modelOffset += sizeof(BattleModel);
    } while (loaded < 10);
}

void func_800B5AAC(s32 arg0) {
    u8 actor = arg0 & 0xFF;
    Unk80151200* state;
    s32 i;
    g_BattleModels[actor].rootTrans.vx = D_80163C80[actor].vx;
    g_BattleModels[actor].rootTrans.vy = D_80163C80[actor].vy;
    g_BattleModels[actor].rootTrans.vz = D_80163C80[actor].vz;
    g_BattleModels[actor].scale = 0x1000;
    state = &D_80151200[actor];
    state->D_8015123C = 0x1000;
    state->D_8015123A = 0x1000;
    D_80151200[actor].D_80151238 = 0x1000;
    D_80151200[actor].D_8015120C = 0;
    D_80151200[actor].D_80151200 = 0;
    for (i = 0; i < g_BattleModels[actor].numBones; i++) {
        g_BattleModels[actor].boneTransforms[i].trans.vy = 0;
    }
}

void BattleLoadPlayerModel(s32 actor) {
    u32* data = (u32*)0x801B0000;
    ActiveCharacterData* player;
    u32 count;

    func_8001C3CC(D_80103200 + actor * 0xF000, data, data[data[0] - 16]);
    player = SysGetPartyPlayerStructureAddressByPartyId(actor);
    count = data[0];
    func_8001C3CC(D_80163F34[actor], (u8*)data + data[count + (player->weapon.weaponModel & 0xF) - 15], 0x1000);
}

s32 BattleLoadEnemyModel(s32 slot) {
    u32* data = (u32*)0x801B0000;

    func_8001C3CC(D_800F8384[slot + 3], data, data[data[0]]);
    return data[data[0]];
}

void BattleLoadEnemyTexture(s32 slot) {
    u32* data = (u32*)0x801B0000;
    u32 offset;
    s32 i;
    if (g_BattleData.activeEncounter.setup.stageID == 0x4E) {
        BattleSetLoadTimToVram((u_long*)(data[data[0]] + (u8*)data), (s16)(slot + 0x13), 0, 0);
    } else {
        BattleSetLoadTimToVram((u_long*)(data[data[0]] + (u8*)data), (s16)(slot + 0x12), 0, (s16)((slot + 3) * 3));
    }
    for (i = 4; i < D_800F7DF4.enemyCount + 4; i++) {
        if (D_800F7DF4.enemies[i - 4].modelIndex == slot + 3) {
            offset = data[0] * 4;
            BattleStoreUnitClut((u_long*)((u8*)data + *(u32*)((u8*)data + offset)), i);
        }
    }
}

void BattleLoadPlayerTexture(s32 actor) {
    s16 texture;
    s32 clutSlot;
    s16 clutOffset;
    u32* data;
    switch (actor) {
    case 0:
        texture = 0;
        clutSlot = 0;
        clutOffset = 0;
        break;
    case 1:
        texture = 16;
        clutSlot = 1;
        clutOffset = 192;
        break;
    case 2:
        texture = 17;
        clutSlot = 2;
        clutOffset = 384;
        break;
    }
    if (g_BattleData.actors[actor].charId != -1) {
        data = (u32*)0x801B0000;
        BattleSetLoadTimToVram((u_long*)((u8*)data + data[data[0] - 16]), texture, 0, (s16)(actor * 3));
        BattleStoreUnitClut((u_long*)((u8*)data + data[data[0] - 16]), clutSlot);
        g_BattleModels[actor].clutOffset = clutOffset;
        BattleBoneSetParentMatrix(&g_BattleModels[actor].stageMatrix, &g_BattleModels[actor].boneTransforms[49]);
    }
}

static void func_800B5FC4(s16 arg0) { BattleModelStartFades(arg0); }

void func_800B5FE8(s16 actor) {
    s32 i;
    u8* flags;
    for (i = 0; i < g_BattleModels[actor].numBones; i++) {
        g_BattleModels[actor].boneFlags[i] |= 8;
    }
    for (i = 0; i < D_800FA6D8[actor].unk3C; i++) {
        flags = (u8*)&D_800FA6D8[actor] + ((u8*)D_800FA6D8[0].unk3E - (u8*)&D_800FA6D8[0]);
        flags[i] |= 8;
    }
}

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
        if (actor < START_ENEMY || D_800F7DF4.enemies[actor - START_ENEMY].flags & 1) {
            g_BattleModels[actor].specialFlags &= ~BATTLE_MODEL_HIDDEN;
        }
        func_800B5FE8(actor);
    }
}

void BattleModelFadeOutTick(void) {
    s16 actor = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
    *(s32*)0x1F800008 = actor;
    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleModels[actor].blendAlpha = 0;
        g_BattleModels[actor].specialFlags |= 4;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        D_80151200[*(s32*)0x1F800008].D_80151235 = 0;
        return;
    }
    g_BattleModels[actor].blendAlpha += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
    g_BattleModels[actor].colorR -= g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    g_BattleModels[*(s32*)0x1F800008].colorG -= g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    g_BattleModels[*(s32*)0x1F800008].colorB -= g_BattleMovementSlots[g_BattleMovementCursor].unkC;
    D_80151200[*(s32*)0x1F800008].D_80151235 = 1;
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
}

void BattleModelFadeInTick(void) {
    s16 actor = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
    s32 i;
    *(s32*)0x1F800008 = actor;
    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleModels[actor].blendAlpha = 0;
        for (i = 0; i < g_BattleModels[*(s32*)0x1F800008].numBones; i++) {
            g_BattleModels[*(s32*)0x1F800008].boneFlags[i] &= 0xF7;
        }
        for (i = 0; i < D_800FA6D8[*(s32*)0x1F800008].unk3C; i++) {
            D_800FA6D8[*(s32*)0x1F800008].unk3E[i] &= 0xF7;
        }
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        D_80151200[*(s32*)0x1F800008].D_80151235 = 0;
        g_BattleModels[*(s32*)0x1F800008].animControlFlags |= 0x20;
        return;
    }
    g_BattleModels[actor].blendAlpha -= g_BattleMovementSlots[g_BattleMovementCursor].unkE;
    g_BattleModels[actor].colorR += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    g_BattleModels[*(s32*)0x1F800008].colorG += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    g_BattleModels[*(s32*)0x1F800008].colorB += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
    D_80151200[*(s32*)0x1F800008].D_80151235 = 1;
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
}

void BattleInitModelsAnimAndColor(s32 first, s32 end) {
    s32 actor;
    Unk80151200* state;
    u8* animControl;
    s32 stateOffset;
    s32 modelIndex;
    s32 one;
    s16 scale;
    actor = first;
    if (actor < end) {
        one = 1;
        scale = 0x1000;
        stateOffset = actor * (s32)sizeof(Unk80151200);
        modelIndex = actor;
        animControl = (u8*)&g_BattleModels[modelIndex].animControlFlags;
        for (; actor < end; actor++) {
            g_BattleModels[modelIndex].unk24 = 31;
            g_BattleModels[modelIndex].specialFlags = 0x80;
            g_BattleModels[modelIndex].blendAlpha = 0;
            g_BattleModels[modelIndex].colorR = 0;
            g_BattleModels[modelIndex].colorG = 0;
            g_BattleModels[modelIndex].colorB = 0;
            if (actor >= START_ENEMY) {
                if (!(D_800F7DF4.enemies[modelIndex - START_ENEMY].flags & 4)) {
                    g_BattleModels[modelIndex].specialFlags = 0;
                }
                if (!(D_800F7DF4.enemies[modelIndex - START_ENEMY].flags & 1)) {
                    g_BattleModels[modelIndex].specialFlags |= 4;
                }
            }
            state = (Unk80151200*)((u8*)D_80151200 + stateOffset);
            *animControl = one;
            g_BattleModels[modelIndex].animId = 0;
            g_BattleModels[modelIndex].scale = scale;
            state->D_8015123C = scale;
            state->D_8015123A = scale;
            ((Unk80151200*)((u8*)D_80151200 + stateOffset))->D_80151238 = scale;
            g_BattleModels[modelIndex].ready = one;
            ((Unk80151200*)((u8*)D_80151200 + stateOffset))->D_80151233 = 0;
            stateOffset += sizeof(Unk80151200);
            animControl += sizeof(BattleModel);
            modelIndex++;
        }
    }
}

// drains g_BattleActionQueue (12-byte entries, -1-terminated, index D_801590E0), one
// entry per call, dispatched by a type byte (0-5, jtbl_800A05FC) via m2c
// structural read (not yet decompiled):
//   0 callback-driven step (BattleDetachedRegister(&func_800C494C)), immediate
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
void BattleQueue1Execute(void) {
    s32 actor;
    s32 actor2;
    s32 flags;
    s32 target;
    s32 current;
    s32 effect;
    s32 terminal;
    u8 index;
    u8 targetId;
    index = D_801590E0;
    if (g_BattleActionQueue[index].actionId != -1) {
        do {
            switch (g_BattleActionQueue[index].unk1) {
            case 5:
                g_BattleModels[g_BattleActionQueue[D_801590E0].actionId].animControlFlags |= 0x20;
                actor = g_BattleActionQueue[D_801590E0].actionId;
                D_80151200[actor].D_80151200 = g_BattleData.actors[actor].D_801636C0;
                actor2 = g_BattleActionQueue[D_801590E0].actionId;
                flags = D_80151200[actor2].D_80151200;
                if (flags != 0x10 && flags != 0x20)
                    g_BattleModels[actor2].rootRot.vy = g_BattleModels[actor2].defaultRotX;
                BattleUpdateRender();
                D_801590E0++;
                break;
            case 0:
                BattleDetachedRegister(func_800C494C);
                BattleUpdateRender();
                D_801590E0++;
                break;
            case 2:
                func_800C5C18(g_BattleActionQueue[D_801590E0].unk6, (u8)g_BattleActionQueue[D_801590E0].unk5,
                              (u8)g_BattleActionQueue[D_801590E0].unk3, (u8)g_BattleActionQueue[D_801590E0].unk2);
                BattleUpdateRender();
                D_801590E0++;
                break;
            case 4:
                if (D_800F7DE4 != 0) {
                    D_800F7DE4 = 0;
                    *(s32*)0x1F800004 = g_BattleActionQueue[D_801590E0].unk5 << 8;
                    *(s32*)0x1F800008 = g_BattleModels[g_BattleActionQueue[D_801590E0].actionId].scale;
                    effect = BattleEffectRegister(func_800C5694);
                    target = *(s32*)0x1F800004;
                    current = *(s32*)0x1F800008;
                    if (target < current)
                        *((s16*)((u8*)&g_BattleEffectSlots[0].raw.D_8016297A + (effect << 5))) =
                            (current - target) / g_BattleActionQueue[D_801590E0].unk6;
                    else
                        *((s16*)((u8*)&g_BattleEffectSlots[0].raw.D_8016297A + (effect << 5))) =
                            (target - current) / g_BattleActionQueue[D_801590E0].unk6;
                    g_BattleModels[g_BattleActionQueue[D_801590E0].actionId].ready = 0;
                    *((s16*)((u8*)&g_BattleEffectSlots[0].raw.D_8016297E + (effect << 5))) =
                        g_BattleActionQueue[D_801590E0].actionId;
                    *((s16*)((u8*)&g_BattleEffectSlots[0].raw.D_8016297C + (effect << 5))) =
                        g_BattleActionQueue[D_801590E0].unk6;
                    g_BattleActionQueue[D_801590E0].unk8 = -1;
                }
                goto render;
            case 3:
                if (D_800F7DE4 != 0) {
                    D_800F7DE4 = 0;
                    BattleQueue1CameraInit();
                    D_801590CC = g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].attackerId;
                    g_BattleModels[D_801590CC].ready = 1;
                    BattleQueue1InitPlayAnim();
                    D_800F9774 = 0;
                    if (g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].targetId != -1) {
                        terminal = -1;
                        do {
                            BattleQueue1AddNewTarg();
                            g_BattleActionQueue[D_801590E0].targetIndex++;
                        } while (
                            g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].targetId != terminal);
                    }
                    D_801517BC = 1;
                    BattleQueue1UpdateTargMasks();
                }
                goto render;
            case 1:
                if (D_800F7DE4 != 0) {
                    D_800F7DE4 = 0;
                    BattleQueue1CameraInit();
                    D_801590CC = (u8)g_BattleActionQueue[D_801590E0].actionId;
                    g_BattleModels[D_801590CC].animControlFlags |= 1;
                    g_BattleModels[D_801590CC].ready = 0;
                    BattleQueue1InitPlayAnim();
                    D_800F9774 = 0;
                    while (g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].targetId != -1) {
                        D_800FAFE0 = g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].attackerId;
                        BattleQueue1AddNewTarg();
                        if (D_800F9774 == 1) {
                            targetId = g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].targetId;
                            D_800FA6D4 = targetId;
                            D_800F99E8 = targetId;
                            D_80161EEC = targetId;
                            if (g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].hurtAnimScript == 9)
                                D_800F99E8 =
                                    g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].attackerId;
                            D_801031F0 = 0;
                        } else {
                            D_800FA6D4 = 15;
                            D_80161EEC = 15;
                            D_800F99E8 = 15;
                            D_801031F0 = 1;
                        }
                        g_BattleActionQueue[D_801590E0].targetIndex++;
                    }
                    BattleQueue1UpdateTargMasks();
                }
                D_801517BC = 0;
                goto render;
            }
            goto next;
        render:
            BattleUpdateRender();
            if (D_800F7DE4 == 0)
                goto next;
            D_801590E0++;
        next:
            index = D_801590E0;
        } while (g_BattleActionQueue[index].actionId != -1);
    }
    D_801590E0 = 0;
    BattleUpdateRender();
}

void BattleQueue1InitPlayAnim(void) {
    if (D_800FA6D0 == 2 && D_801590CC < 4) {
        g_BattleModels[D_801590CC].defaultRotX = 0x800;
        g_BattleModels[D_801590CC].rootRot.vy = 0x800;
    }
    if (D_800FA6D0 == 1 && D_801590CC >= 4) {
        g_BattleModels[D_801590CC].defaultRotX = 0x800;
        g_BattleModels[D_801590CC].rootRot.vy = 0x800;
    }
    g_BattleModels[D_801590CC].animId = g_BattleActionQueue[D_801590E0].unk5;
    g_BattleModels[D_801590CC].attackEffectId = g_BattleActionQueue[D_801590E0].unk2;
    g_BattleModels[D_801590CC].currentActionId = g_BattleActionQueue[D_801590E0].unk3;
    D_80163B38 = 0;
    D_80162090 = 0;
    D_80151200[D_801590CC].D_8015123E = g_BattleActionQueue[D_801590E0].unk6;
    BattleQueue1ClearTargs();
}

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

void BattleQueue1AddNewTarg(void) {
    u8 target;
    s32 queueIndex;
    s32 targetSlot;
    s32 firstSlot;
    s16 value;
    s32 payloadOffset;
    firstSlot = D_800F9774;
    target = g_BattleQueueTargets[g_BattleActionQueue[D_801590E0].targetIndex].targetId;
    D_80163CC0[firstSlot].D_80163CC0 = target;
    D_801517F0[firstSlot] = target;
    queueIndex = D_801590E0;
    targetSlot = D_800F9774;
    D_80163CC0[targetSlot].D_80163CC2 = g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].flags;
    D_80163CC0[targetSlot].D_80163CC4 = g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].targetStatus;
    g_BattleModels[D_80163CC0[targetSlot].D_80163CC0].queuedActionId =
        g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].hurtAnimScript;
    payloadOffset = D_800F9774 * (s32)sizeof(BattleTargetEffectSettings);
    (*(u16*)((u8*)&D_800F99EC[0].unk0 + payloadOffset)) =
        D_800F9F3C[g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].extraDataIndex].damage;
    (*(u16*)((u8*)&D_800F99EC[0].unk2 + payloadOffset)) =
        D_800F9F3C[g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].extraDataIndex].damageFlags;
    (*(u16*)((u8*)&D_800F99EC[0].actor + payloadOffset)) =
        g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].extraDataIndex;
    (*(u16*)((u8*)&D_800F99EC[0].targetIndex + payloadOffset)) = g_BattleActionQueue[queueIndex].targetIndex;
    if (g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].extraDataIndex == -1) {
        value = -1;
        (*(s16*)((u8*)&D_800F99EC[0].unk4 + payloadOffset)) = -1;
    } else {
        (*(s16*)((u8*)&D_800F99EC[0].unk4 + payloadOffset)) =
            D_800F9F3C[g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].extraDataIndex].impactSfxId;
        value =
            D_800F9F3C[g_BattleQueueTargets[g_BattleActionQueue[queueIndex].targetIndex].extraDataIndex].impactEffectId;
    }
    (*(s16*)((u8*)&D_800F99EC[0].unk6 + payloadOffset)) = value;
    D_800F9774++;
}

void BattleQueue1UpdateTargMasks(void) {
    Unk80163CC0* target;
    s32 index;
    u16* targetFlags;
    u32 bitBase;
    s32 count;
    s32 mask;
    u16 flags;

    D_800FA69C = 0;
    D_80163608 = 0;
    D_80162088 = 0;
    D_80161DE8 = 0;
    D_800F836C = 0;
    D_801517B4 = 0;
    g_BattleCurrentTargetMask = 0;
    index = 0;
    count = D_800F9774;
    if (index < count) {
        bitBase = 1;
        target = D_80163CC0;
        targetFlags = &D_80163CC0->D_80163CC2;
        for (; index < (s32)D_800F9774; index++) {
            mask = (bitBase << target->D_80163CC0) | (u16)g_BattleCurrentTargetMask;
            flags = *targetFlags & 1;
            g_BattleCurrentTargetMask = mask;
            if (flags) {
                D_801517B4 = (bitBase << target->D_80163CC0) | D_801517B4;
            }
            if (*targetFlags & 2) {
                D_800F836C = (bitBase << target->D_80163CC0) | (u16)D_800F836C;
            }
            if (*targetFlags & 4) {
                D_80161DE8 = (bitBase << target->D_80163CC0) | D_80161DE8;
            }
            if (*targetFlags & 8) {
                D_80162088 = (bitBase << target->D_80163CC0) | D_80162088;
            }
            if (*targetFlags & 0x10) {
                D_80163608 = (bitBase << target->D_80163CC0) | (u16)D_80163608;
            }
            flags = *targetFlags & 0x20;
            targetFlags = (u16*)((u8*)targetFlags + sizeof(*target));
            if (flags) {
                D_800FA69C = (bitBase << target->D_80163CC0) | (u16)D_800FA69C;
            }
            target++;
        }
    }
}

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
        if (g_BattleModels[i].ready == 0) {
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
            g_BattleModels[i].ready == var_t1) {
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
    BattleDetachedUpdate();
    BattleEffectUpdate();
    func_800BB75C(&g_BattleWorldView, &D_800FA958, &g_BattleCameraPos.vx, &g_BattleCameraTarget.vx);
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

    g_BattleEffectSlots[v].raw.D_8016297C = 0;
    g_BattleEffectSlots[v].raw.D_80162980 = arg0;
    func_800B8A34(func_800B888C(arg0), v);
}

void func_800B8944(s32 actor, s32 target) {
    s32 slot;
    s16 flags;
    s32 stateOffset;
    slot = BattleEffectRegister(BattleFixedPointRampSpawnChildEffectsWithFade);
    g_BattleEffectSlots[slot].raw.D_8016297C = 0;
    g_BattleEffectSlots[slot].raw.D_80162980 = actor;
    func_800B8A34(target, slot);
    if (D_80163CC0[target].D_80163CC2 & 4) {
        stateOffset = actor * (s32)sizeof(Unk80151200);
        flags = *(s16*)((u8*)&D_80151200[0].D_8015120C + stateOffset);
        flags |= 0x10;
    } else {
        stateOffset = actor * (s32)sizeof(Unk80151200);
        flags = *(s16*)((u8*)&D_80151200[0].D_8015120C + stateOffset);
        flags &= 0xFFEF;
    }
    *(s16*)((u8*)&D_80151200[0].D_8015120C + stateOffset) = flags;
}

void func_800B8A34(s32 target, s32 slot) {
    u16 next;
    g_BattleEffectSlots[slot].raw.D_80162982 = D_800F99EC[target].unk0;
    next = D_800F99EC[target].unk2;
    g_BattleEffectSlots[slot].raw.unkC = D_801590CC;
    g_BattleEffectSlots[slot].raw.unkE = next;
    g_BattleEffectSlots[slot].raw.unk10 = D_800F99EC[target].unk4;
    g_BattleEffectSlots[slot].raw.unk14 = D_800F99EC[target].unk6;
    next = D_800F99EC[target].actor;
    g_BattleEffectSlots[slot].raw.unk18 = (u8)D_800F8CF0;
    g_BattleEffectSlots[slot].raw.D_8016297E = next;
    g_BattleEffectSlots[slot].raw.unk19 = (u8)D_800F99EC[target].targetIndex;
}

void func_800B8B48(void) {
    s16* event;
    s32 actor;
    s32 actor9;
    s32 type;
    for (;;) {
        type = BattleEventQueuePop(&event);
        if (type == 0) {
            return;
        }
        switch (type) {
        case 3:
        case 8:
            break;
        case 4:
        case 5:
            actor = event[0];
            D_800F8378 = 0;
            func_800B88CC(actor);
            g_BattleModels[actor].animControlFlags |= 0x20;
            D_80151200[actor].D_80151200 = D_80163CC0[func_800B888C(actor)].D_80163CC4;
            break;
        case 9:
            actor9 = event[0];
            D_800F8378 = 0;
            func_800B88CC(actor9);
            break;
        case 10:
            D_800F8378 = 1;
            actor = event[0];
            func_800B8944(actor, event[8]);
            g_BattleModels[actor].animControlFlags |= 0x20;
            D_80151200[actor].D_80151200 = D_80163CC0[func_800B888C(actor)].D_80163CC4;
            D_80163B38++;
            break;
        case 1:
            g_BattleEffectModelStartPos.vx = event[2];
            g_BattleEffectModelStartPos.vy = event[3];
            g_BattleEffectModelStartPos.vz = event[4];
            g_BattleEffectModelStartRotY = event[8];
            BattleEffectRegister(BattleEffectModelTick);
            g_BattleEffectModelState = 0;
            break;
        case 2:
            g_BattleEffectModelState = 0xFF;
            break;
        case 6:
            actor = event[0];
            BattleTriggerActorFlashMode2(actor);
            g_BattleModels[actor].ready = 1;
            break;
        case 7:
            SysCdromStartLoadLzs(D_800E8C90[event[8]].loc, D_800E8C90[event[8]].len, *(void**)&event[6], NULL);
            BattleCdromReadChain();
            break;
        default:
            break;
        }
    }
}

static void func_800B8E48(s32 arg0) {
    s32 temp_a0;

    temp_a0 = arg0 & 0xFF;
    g_BattleModels[temp_a0].ready = 1;
    g_BattleModels[temp_a0].specialFlags &= 0x7F;
    D_80151200[temp_a0].D_8015120C &= 0xFFDF;
}

void func_800B8EE4(void) {
    s32 actor;
    EffectModel* data;
    s32* offsets;
    for (actor = 0; actor < NUM_PARTY; actor++) {
        if (g_BattleData.actors[actor].charId != -1) {
            func_800B5FC4(actor);
            if (!(g_BattleModels[actor].specialFlags & 2)) {
                data = (EffectModel*)D_800F8384[D_80151200[actor].D_80151232];
                offsets = (s32*)((u8*)data + sizeof(data->header.numOffsets));
                BattleExecuteUnitAnimScript(actor, offsets[1] + 0x68, &offsets[2], data);
            }
        }
    }
}

void func_800B8FCC(s32 actor) {
    s32 model;
    EffectModel* data;
    if (D_80151200[actor].D_80151232 == 6) {
        model = 6;
    } else {
        model = D_800F7DF4.enemies[actor - 4].modelIndex;
    }
    data = (EffectModel*)D_800F8384[model];
    BattleExecuteUnitAnimScript(actor, data->header.offsets[1] + 0x68, &data->header.offsets[2], data);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B905C);

void func_800B91CC(void) {
    union {
        s32 packed;
        DVECTOR point;
    } screen;
    s32 p, flag;
    SVECTOR* rotation;
    SVECTOR* rotationBase;
    BattleModelSub* bone;
    MATRIX* matrix;
    s32 actor;
    s32 modelOffset;
    s32 boneOffset;
    s32 screenOffset;
    u8* boneIndex;
    u16 transX;
    u16 x;
    s32 y;
    s32 partyScreenOffset;
    s32 partyModelOffset;
    s32 enemyScreenOffset;
    s32 enemyModelOffset;
    MATRIX* enemyMatrix;
    u8* enemyBoneIndex;
    if ((Savemap.config & 0x300) == 0x100) {
        actor = 0;
        partyScreenOffset = 0;
        rotationBase = &D_80163A9C[0].rot;
        bone = (BattleModelSub*)(void*)&D_80163A9C;
        rotation = rotationBase;
        partyModelOffset = 0;
        boneOffset = 0;
        for (; actor < 3; actor++) {
            ((BattleModelSub*)((u8*)D_80163A9C + boneOffset))->parentMatrix =
                ((BattleModel*)((u8*)g_BattleModels + partyModelOffset))->pCameraMatrix;
            transX = ((BattleModel*)((u8*)g_BattleModels + partyModelOffset))->rootTrans.vx;
            ((BattleModelSub*)((u8*)D_80163A9C + boneOffset))->trans.vy = -500;
            ((BattleModelSub*)((u8*)D_80163A9C + boneOffset))->trans.vx = transX;
            ((BattleModelSub*)((u8*)D_80163A9C + boneOffset))->trans.vz =
                ((BattleModel*)((u8*)g_BattleModels + partyModelOffset))->rootTrans.vz;
            ((BattleModelSub*)((u8*)D_80163A9C + boneOffset))->rot.vx =
                ((BattleModel*)((u8*)g_BattleModels + partyModelOffset))->rootRot.vx;
            ((BattleModelSub*)((u8*)D_80163A9C + boneOffset))->rot.vy =
                ((BattleModel*)((u8*)g_BattleModels + partyModelOffset))->rootRot.vy;
            ((BattleModelSub*)((u8*)D_80163A9C + boneOffset))->rot.vz =
                ((BattleModel*)((u8*)g_BattleModels + partyModelOffset))->rootRot.vz;
            partyModelOffset += sizeof(BattleModel);
            RotMatrixYXZ(rotation, &bone->m);
            rotation = (SVECTOR*)((u_long)rotation + sizeof(BattleModelSub));
            BattleUpdateMatrixWithSelfAndParentAndSetToGte(bone);
#ifdef PLATFORM_PSYZ
            RotTransPers(&D_800E7D08, &screen.packed, &p, &flag);
#else
            RotTransPers(&D_800E7D08, (long*)&screen.packed, (long*)&p, (long*)&flag);
#endif
            bone++;
            boneOffset += sizeof(BattleModelSub);
            x = (u16)screen.point.vx;
            y = screen.packed >> 16;
            ((ModelScreenPos*)((u8*)g_modelScreenPos + partyScreenOffset))->x = x;
            ((ModelScreenPos*)((u8*)g_modelScreenPos + partyScreenOffset))->y = y;
            partyScreenOffset += sizeof(ModelScreenPos);
        }
        actor = 4;
        enemyScreenOffset = 4 * sizeof(ModelScreenPos);
        enemyBoneIndex = &g_BattleModels[4].boneIndices[0];
        enemyModelOffset = 4 * sizeof(BattleModel);
        do {
            enemyMatrix =
                (MATRIX*)((u8*)&g_BattleModels + ((u8*)&g_BattleModels[0].boneTransforms[0].m - (u8*)&g_BattleModels) +
                          enemyModelOffset);
            enemyModelOffset += sizeof(BattleModel);
            SetRotMatrix((MATRIX*)((u_long)enemyMatrix + *enemyBoneIndex * sizeof(BattleModelSub)));
            actor++;
            SetTransMatrix((MATRIX*)((u_long)enemyMatrix + *enemyBoneIndex * sizeof(BattleModelSub)));
#ifdef PLATFORM_PSYZ
            RotTransPers(&D_800E7D08, &screen.packed, &p, &flag);
#else
            RotTransPers(&D_800E7D08, (long*)&screen.packed, (long*)&p, (long*)&flag);
#endif
            enemyBoneIndex = (u8*)((u_long)enemyBoneIndex + sizeof(BattleModel));
            x = (u16)screen.point.vx;
            y = screen.packed >> 16;
            ((ModelScreenPos*)((u8*)g_modelScreenPos + enemyScreenOffset))->x = x;
            ((ModelScreenPos*)((u8*)g_modelScreenPos + enemyScreenOffset))->y = y;
            enemyScreenOffset += sizeof(ModelScreenPos);
        } while (actor < 10);
        return;
    }
    actor = 0;
    screenOffset = 0;
    boneIndex = &g_BattleModels[0].boneIndices[0];
    modelOffset = 0;
    do {
        matrix = (MATRIX*)((u8*)&g_BattleModels + ((u8*)&g_BattleModels[0].boneTransforms[0].m - (u8*)&g_BattleModels) +
                           modelOffset);
        modelOffset += sizeof(BattleModel);
        SetRotMatrix((MATRIX*)((u_long)matrix + *boneIndex * sizeof(BattleModelSub)));
        actor++;
        SetTransMatrix((MATRIX*)((u_long)matrix + *boneIndex * sizeof(BattleModelSub)));
#ifdef PLATFORM_PSYZ
        RotTransPers(&D_800E7D08, &screen.packed, &p, &flag);
#else
        RotTransPers(&D_800E7D08, (long*)&screen.packed, (long*)&p, (long*)&flag);
#endif
        boneIndex = (u8*)((u_long)boneIndex + sizeof(BattleModel));
        x = (u16)screen.point.vx;
        y = screen.packed >> 16;
        ((ModelScreenPos*)((u8*)g_modelScreenPos + screenOffset))->x = x;
        ((ModelScreenPos*)((u8*)g_modelScreenPos + screenOffset))->y = y;
        screenOffset += sizeof(ModelScreenPos);
    } while (actor < 10);
}

// promote each model's staged (x, y) into its committed (prevX, prevY)
static void func_800B950C(void) {
    s32 i;

    for (i = 0; i < LEN(g_modelScreenPos); i++) {
        g_modelScreenPos[i].prevX = g_modelScreenPos[i].x;
        g_modelScreenPos[i].prevY = g_modelScreenPos[i].y;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B9568);

void func_800BA11C(s32 index, s32 step, s16 initial, s16 reset) {
    s32 boneOffset;
    s32 translationOffset;
    SVECTOR* rot;
    BattleModelSub* bone;
    boneOffset = (index & 0xFF) * sizeof(BattleModelSub);
    rot = (SVECTOR*)((u8*)&D_800F8158[0].rot + boneOffset);
    bone = (BattleModelSub*)((u8*)D_800F8158 + boneOffset);
    RotMatrixYXZ(rot, &bone->m);
    BattleUpdateMatrixWithSelfAndParentAndSetToGte(bone);
    translationOffset = boneOffset;
    if (((BattleModelSub*)((u8*)D_800F8158 + translationOffset))->trans.vz == reset) {
        ((BattleModelSub*)((u8*)D_800F8158 + translationOffset))->trans.vz = initial;
    }
    ((BattleModelSub*)((u8*)D_800F8158 + translationOffset))->trans.vz =
        (u32)step + (u16)((BattleModelSub*)((u8*)D_800F8158 + translationOffset))->trans.vz;
    func_800BA360(8, g_cDb->unk70, 12, 0);
    func_800BA360(9, g_cDb->unk70, 12, 0);
    func_800BA360(10, g_cDb->unk70, 12, 0);
}

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

void func_800BA2BC(s32 actor) {
    if (D_80163B44[actor] < 0x10) {
        D_800F8158[actor].trans.vy -= 25;
    } else {
        D_800F8158[actor].trans.vy += 25;
    }
    D_80163B44[actor] = (D_80163B44[actor] - 1) & 0x1F;
}

void func_800BA360(s32 index, u32** ot, s32 depth, s32 flags) {
    ModelRenderDesc* desc = (ModelRenderDesc*)0x1F800320;
    s32 offset;
    s32 renderFlags;
    u8 sign;
    s32* model;
    offset = D_801590E4.header.offsets[index];
    flags |= 0x180;
    desc->tpage = 0x20;
    model = (s32*)(D_801590E4.bytes + (offset & 0x7FFFFFFF));
    sign = offset < 0;
    renderFlags = (sign << 3) | flags;
    desc->clut = 0;
    desc->uvOffset = 0;
    desc->model = model;
    desc->flags = renderFlags;
    desc->color = D_800F5B74;
    D_80163C74 = func_800D29D4(desc, ot, depth, D_80163C74);
}

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

    for (i = 4; i < D_800F7DF4.enemyCount + 4; i++) {
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

void BattleUpdateModelMatrixesWithSelfAndParent(s32 actor) {
    BattleMatrixScratch* scratch = (BattleMatrixScratch*)0x1F800020;
    BattleModelSub* bone;
    MATRIX* parent;
    s32 i = 0;
    for (; i < g_BattleModels[actor].numBones; i++) {
        bone = &g_BattleModels[actor].boneTransforms[i];
        parent = bone->parentMatrix;
        scratch->parent = parent;
        scratch->matrix = *parent;
        gte_SetRotMatrix(&scratch->matrix);
        gte_ldclmv(&bone->m.m[0][0]);
        gte_rtir();
        gte_stclmv(&bone->m.m[0][0]);
        gte_ldclmv(&bone->m.m[0][1]);
        gte_rtir();
        gte_stclmv(&bone->m.m[0][1]);
        gte_ldclmv(&bone->m.m[0][2]);
        gte_rtir();
        gte_stclmv(&bone->m.m[0][2]);
        gte_SetRotMatrix(&scratch->matrix);
        gte_SetTransMatrix(&scratch->matrix);
        gte_ldv0(&bone->trans);
        gte_rt();
        gte_stlvnl(bone->m.t);
    }
}

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

void BattleModelUpdateAllBonesHeight(s16 actor, s16 height) {
    s32 i = 0;
    s32* model;
    s32 count;
    count = g_BattleModels[actor].numBones;
    if (i < count) {
        do {
            model = g_BattleModels[actor].boneModels[i];
            i++;
            if (model != NULL) {
                BattleModelUpdateBoneHeight(model, height);
            }
        } while (i < g_BattleModels[actor].numBones);
    }
}

void func_800BB4F8(void) {
    u32* data = (u32*)0x801B0000;

    BattleSetLoadTimToVram((u_long*)((u8*)data + data[data[0]]), 0, 0, 0);
}

void BattleUnitInitBonesAndMatrixes(s32 actorArg, void* data, s32 enemy) {
    s32* offsets = D_801590E4.header.offsets;
    s32 modelOffset;
    s16 actor = actorArg;
    if (D_801590E4.bytes[offsets[0]] == 2) {
        if (enemy == 0)
            BattleBoneSetParentMatrix(&D_800F8158[0].m, (BattleModelSub*)&g_BattleModels[actor].stageMatrix);
        else
            BattleBoneSetParentMatrix(&D_800F8158[1].m, (BattleModelSub*)&g_BattleModels[actor].stageMatrix);
    } else if (actor == 3) {
        BattleBoneSetParentMatrix(&g_BattleWorldView.m, (BattleModelSub*)&g_BattleModels[3].stageMatrix);
    } else {
        BattleBoneSetParentMatrix(&D_800F8158[0].m, (BattleModelSub*)&g_BattleModels[actor].stageMatrix);
    }
    modelOffset = actor * (s32)sizeof(BattleModel);
    BattleBoneSetParentMatrix((MATRIX*)((u8*)&g_BattleModels[0].stageMatrix + modelOffset),
                              (BattleModelSub*)((u8*)&g_BattleModels[0].boneTransforms[0] + modelOffset));
    BattleGetModelBoneNumAndInitBones((s16*)((u8*)&g_BattleModels[0].numBones + modelOffset), data, actor);
}

void BattleBoneSetParentMatrix(MATRIX* parent, BattleModelSub* bone) { bone->parentMatrix = parent; }

void BattleQueue1CameraInit(void) {
    s16 command = g_BattleActionQueue[D_801590E0].unk8;
    u8 category;

    if (command == -4) {
        return;
    }
    D_800F8370 = command;
    D_801590DC = 0;
    D_801518A4[3].cursor = 0xFF;
    D_801518A4[2].cursor = 0xFF;
    D_801518A4[1].cursor = 0xFF;
    D_801518A4[0].cursor = 0xFF;
    D_80151844[3].cursor = 0xFF;
    D_80151844[2].cursor = 0xFF;
    D_80151844[1].cursor = 0xFF;
    D_80151844[0].cursor = 0xFF;
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

    BattleLookAt(camera, cameraPos, cameraTarget, &D_800E7D10);
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

void func_800BBA84(u16 sound, s32 actor, s32 pan) {
    s32 value;

    if ((s16)actor == -1) {
        value = pan & 0xFF;
    } else {
        value = (((s16)g_modelScreenPos[(s16)actor].prevX / 5) * 2) & 0x7E;
    }
    g_AkaoCmd.params[0] = value;
    g_AkaoCmd.opcode = AKAO_PLAY_SOUND;
    g_AkaoCmd.params[1] = sound & 0xFFFF;
    AkaoExec();
}

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
                g_BattleEffectSlots[i].raw.D_80162978 = g_BattleEffectCursor;
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
s32 BattleDetachedRegister(void (*callback)(void)) {
    s16 i;

    for (i = 0; i < 60; i++) {
        if (!g_BattleDetachedCallbacks[i]) {
            if (i >= g_BattleDetachedCursor) {
                g_BattleDetachedCallbacks[i] = callback;
                g_BattleDetachedSlots[i].raw.D_801621F0 = g_BattleDetachedCursor;
                g_BattleDetachedCount++;
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

    g_BattleEffectCount = g_BattleMovementCount = g_BattleDetachedCount = 0;

    for (i = 0; i < 100; i++) {
        g_BattleEffectCallbacks[i] = NULL;
        g_BattleEffectSlots[i].raw.D_80162978 = g_BattleEffectSlots[i].raw.D_8016297A = 0;
    }
    for (i = 0; i < 10; i++) {
        g_BattleMovementCallbacks[i] = NULL;
        g_BattleMovementSlots[i].D_801620AC = g_BattleMovementSlots[i].D_801620AE = 0;
    }
    for (i = 0; i < 60; i++) {
        g_BattleDetachedCallbacks[i] = NULL;
        g_BattleDetachedSlots[i].raw.D_801621F0 = g_BattleDetachedSlots[i].raw.D_801621F2 = 0;
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
static void BattleEffectUpdate(void) {
    void (*callback)(void);

    for (g_BattleEffectCursor = 0; g_BattleEffectCursor < 100; g_BattleEffectCursor++) {
        callback = g_BattleEffectCallbacks[g_BattleEffectCursor];
        if (callback) {
            callback();
            if (g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162978 == -1) {
                g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162978 = 0;
                g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297A = 0;
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
static void BattleDetachedUpdate(void) {
    void (*callback)(void);

    for (g_BattleDetachedCursor = 0; g_BattleDetachedCursor < 60; g_BattleDetachedCursor++) {
        callback = g_BattleDetachedCallbacks[g_BattleDetachedCursor];
        if (callback) {
            callback();
            if (g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 == -1) {
                g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = 0;
                g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F2 = 0;
                g_BattleDetachedCallbacks[g_BattleDetachedCursor] = NULL;
                g_BattleDetachedCount--;
            }
        }
    }
    g_BattleDetachedCursor = 0;
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

void CopyCameraStartEndFromBattleSetup(void) {
    s32 camera = D_80163B3C;
    g_BattleCameraPos.vx = g_BattleData.activeEncounter.camera[camera].start.x;
    g_BattleCameraPos.vy = g_BattleData.activeEncounter.camera[camera].start.y;
    g_BattleCameraPos.vz = g_BattleData.activeEncounter.camera[camera].start.z;
    g_BattleCameraTarget.vx = g_BattleData.activeEncounter.camera[camera].direction.x;
    g_BattleCameraTarget.vy = g_BattleData.activeEncounter.camera[camera].direction.y;
    g_BattleCameraTarget.vz = g_BattleData.activeEncounter.camera[camera].direction.z;
}

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

void func_800BCA58(s32 arg0) {
    s32 index = (s16)arg0;
    g_BattleCameraPos.vx = D_80151844[index].x;
    g_BattleCameraPos.vy = D_80151844[index].y;
    g_BattleCameraPos.vz = D_80151844[index].z;
    g_BattleCameraTarget.vx = D_801518A4[index].x;
    g_BattleCameraTarget.vy = D_801518A4[index].y;
    g_BattleCameraTarget.vz = D_801518A4[index].z;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BCB1C);

void func_800BE49C(void) {
    s32 slotIndex;
    s32 category;
    s16 angle;
    slotIndex = g_BattleCameraCursor;
    category = (u8)g_BattleCameraSlots[slotIndex].unk18;
    if (g_BattleCameraSlots[slotIndex].unk4 == 1) {
        g_BattleCameraSlots[slotIndex].D_800F7ED8 = -1;
    }
    angle = (u16)g_BattleCameraSlots[slotIndex].unkC + (u16)g_BattleCameraSlots[slotIndex].unk8;
    g_BattleCameraSlots[slotIndex].unkC = angle;
    D_80151844[category].x = ((rsin(angle) * g_BattleCameraSlots[slotIndex].unkA) >> 9) + D_801518A4[category].x;
    D_80151844[category].z = ((rcos(g_BattleCameraSlots[slotIndex].unkC) * g_BattleCameraSlots[slotIndex].unkA) >> 9) +
                             D_801518A4[category].z;
    D_80151844[category].y += (u16)g_BattleCameraSlots[slotIndex].unk6;
    g_BattleCameraSlots[slotIndex].unkA += g_BattleCameraSlots[slotIndex].unkE;
    g_BattleCameraSlots[slotIndex].unk4--;
}

void func_800BE69C(s16 actor, s32 categoryArg, u8* sequence, s32 flags) {
    Unk800F7ED8* slot;
    s32 categoryIndex;
    s32 category;
    u16 cursor;
    u16 z;

    category = (u8)categoryArg;
    *(s32*)0x1F800000 = (s32)func_800BFA98(sequence, category);
    *(s32*)0x1F800004 = (s32)func_800BFA98(sequence, category);
    categoryIndex = category;
    *(s32*)0x1F800008 = (s32)func_800BFA98(sequence, category);
    cursor = D_80151844[category].cursor;
    D_80151844[category].cursor = cursor + 1;
    *(s32*)0x1F80000C = (s32)sequence[cursor];
    func_800C018C(actor, *(s16*)0x1F800014, category, flags & 0xFF);
    slot = &g_BattleCameraSlots[BattleCameraRegister(func_800C0410)];
    slot->unk6 = (u16) * (s32*)0x1F800000;
    slot->unk8 = (u16) * (s32*)0x1F800004;
    slot->unkA = (u16) * (s32*)0x1F800008;
    slot->unk4 = (u16) * (s32*)0x1F80000C;
    slot->unkC = (u16)D_80151844[categoryIndex].x;
    slot->unkE = (u16)D_80151844[categoryIndex].y;
    z = D_80151844[categoryIndex].z;
    slot->unk12 = (u8)categoryArg;
    slot->unk10 = z;
}

void func_800BE86C(s16 actor, s32 categoryArg, u8* sequence) {
    Unk800F7ED8* slot;
    s32 category;
    s32 savedCategory = categoryArg;
    s32 categoryIndex;
    u16 cursor;
    u16 z;

    category = (u8)savedCategory;
    *(s32*)0x1F800000 = (s32)func_800BFB10(sequence, category);
    *(s32*)0x1F800004 = (s32)func_800BFB10(sequence, category);
    *(s32*)0x1F800008 = (s32)func_800BFB10(sequence, category);
    cursor = D_801518A4[category].cursor;
    D_801518A4[category].cursor = cursor + 1;
    *(s32*)0x1F80000C = (s32)sequence[cursor];
    func_800C0254(actor, *(s16*)0x1F800014);
    slot = &g_BattleCameraSlots[BattleCameraRegister(func_800C0900)];
    slot->unk6 = (u16) * (s32*)0x1F800000;
    if (D_800F4B00 != 0) {
        slot->unk8 = 0U;
    } else {
        slot->unk8 = (u16) * (s32*)0x1F800004;
    }
    slot->unkA = (u16) * (s32*)0x1F800008;
    slot->unk4 = (u16) * (s32*)0x1F80000C;
    categoryIndex = savedCategory & 0xFF;
    slot->unkC = (u16)D_801518A4[categoryIndex].x;
    slot->unkE = (u16)D_801518A4[categoryIndex].y;
    z = D_801518A4[categoryIndex].z;
    slot->unk12 = (u8)savedCategory;
    slot->unk10 = z;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BEA38);

// Read the next u16 from arg0's byte stream via this category's read cursor.
static s16 func_800BFA98(u8* arg0, s32 arg1) {
    s32 category = arg1 & 0xFF;
    u16 pos = D_80151844[category].cursor;
    u32 lo;
    u8 hi;

    D_80151844[category].cursor = pos + 1;
    lo = arg0[pos];
    D_80151844[category].cursor = pos + 2;
    hi = arg0[(u16)(pos + 1)];
    return (hi << 8) + lo;
}

static s16 func_800BFB10(u8* arg0, s32 arg1) {
    s32 category = arg1 & 0xFF;
    u16 pos = D_801518A4[category].cursor;
    u32 lo;
    u8 hi;

    D_801518A4[category].cursor = pos + 1;
    lo = arg0[pos];
    D_801518A4[category].cursor = pos + 2;
    hi = arg0[(u16)(pos + 1)];
    return (hi << 8) + lo;
}

void func_800BFB88(void) {
    SVECTOR pos;
    s16 actor;
    s16 bone;
    s32 category;
    s32 slotIndex;
    s32 slotIndexAfterCall;
    s32 slotIndexAfterUpdate;

    if (g_BattleCameraSlots[g_BattleCameraCursor].unk4 == 1) {
        g_BattleCameraSlots[g_BattleCameraCursor].D_800F7ED8 = -1;
    }
    slotIndex = g_BattleCameraCursor;
    actor = g_BattleCameraSlots[slotIndex].unk8;
    *(s32*)0x1F80000C = actor;
    bone = g_BattleCameraSlots[slotIndex].unkA;
    *(s32*)0x1F800014 = bone;
    *(s32*)0x1F800018 = g_BattleCameraSlots[slotIndex].unkC;
    BattleGetPartPosition(actor, bone, &pos);
    slotIndexAfterCall = g_BattleCameraCursor;
    *(s32*)0x1F800000 = g_BattleCameraSlots[slotIndexAfterCall].unkE;
    *(s32*)0x1F800004 = g_BattleCameraSlots[slotIndexAfterCall].unk10;
    *(s32*)0x1F800008 = g_BattleCameraSlots[slotIndexAfterCall].unk6;
    func_800C0DD8((s16) * (s32*)0x1F80000C, (u8)g_BattleCameraSlots[slotIndexAfterCall].unkC,
                  (u8)g_BattleCameraSlots[slotIndexAfterCall].unk18);
    category = *(s32*)0x1F800018;
    D_80151844[category].x = (u16)pos.vx + (u16) * (s32*)0x1F800000;
    D_80151844[category].y = (u16)pos.vy + (u16) * (s32*)0x1F800004;
    slotIndexAfterUpdate = g_BattleCameraCursor;
    D_80151844[category].z = (u16)pos.vz + (u16) * (s32*)0x1F800008;
    g_BattleCameraSlots[slotIndexAfterUpdate].unk4 = g_BattleCameraSlots[slotIndexAfterUpdate].unk4 - 1;
}

void func_800BFDA0(void) {
    SVECTOR pos;
    s16 actor;
    s16 bone;
    s32 category;
    s32 slotIndexAfterCall;
    s32 slotIndex;

    if (g_BattleCameraSlots[g_BattleCameraCursor].unk4 == 1) {
        g_BattleCameraSlots[g_BattleCameraCursor].D_800F7ED8 = -1;
    }
    slotIndex = g_BattleCameraCursor;
    actor = g_BattleCameraSlots[slotIndex].unk8;
    *(s32*)0x1F80000C = actor;
    bone = g_BattleCameraSlots[slotIndex].unkA;
    *(s32*)0x1F800014 = bone;
    *(s32*)0x1F800018 = g_BattleCameraSlots[slotIndex].unkC;
    BattleGetPartPosition(actor, bone, &pos);
    *(s32*)0x1F800004 = func_800C0314(*(s32*)0x1F800004, (u8) * (s32*)0x1F80000C);
    slotIndexAfterCall = g_BattleCameraCursor;
    *(s32*)0x1F800000 = g_BattleCameraSlots[slotIndexAfterCall].unkE;
    *(s32*)0x1F800004 = g_BattleCameraSlots[slotIndexAfterCall].unk10;
    *(s32*)0x1F800008 = g_BattleCameraSlots[slotIndexAfterCall].unk6;
    category = *(s32*)0x1F800018;
    D_801518A4[category].x = (u16)pos.vx + (u16) * (s32*)0x1F800000;
    D_801518A4[category].y = (u16)pos.vy + (u16) * (s32*)0x1F800004;
    D_801518A4[category].z = (u16)pos.vz + (u16) * (s32*)0x1F800008;
    g_BattleCameraSlots[slotIndexAfterCall].unk4 = g_BattleCameraSlots[slotIndexAfterCall].unk4 - 1;
}

void func_800BFF88(s16 actor, s16 bone, s32 category, s32 flags) {
    SVECTOR pos;
    s32 index;
    if (actor == 15) {
        BattleEntityGetCenter(g_BattleCurrentTargetMask, &pos);
    } else {
        BattleGetPartPosition(actor, bone, &pos);
        flags &= 0xFF;
        func_800C0DD8(actor, category & 0xFF, flags);
    }
    index = (s16)category;
    D_80151844[index].x = (u16)pos.vx + *(u16*)0x1F800000;
    D_80151844[index].y = (u16)pos.vy + *(u16*)0x1F800004;
    D_80151844[index].z = (u16)pos.vz + *(u16*)0x1F800008;
}

void func_800C0088(s16 actor, s16 bone, s32 categoryArg) {
    SVECTOR pos;
    s32 index;
    s32 category = categoryArg;
    if (actor == 15) {
        BattleEntityGetCenter(g_BattleCurrentTargetMask, &pos);
    } else {
        BattleGetPartPosition(actor, bone, &pos);
        *(s32*)0x1F800004 = func_800C0314(*(s32*)0x1F800004, (u8)actor);
    }
    index = (s16)category;
    D_801518A4[index].x = (u16)pos.vx + *(u16*)0x1F800000;
    D_801518A4[index].y = (u16)pos.vy + *(u16*)0x1F800004;
    D_801518A4[index].z = (u16)pos.vz + *(u16*)0x1F800008;
}

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

s32 func_800C0314(s32 value, s32 arg1) {
    u8 actor = arg1;
    if (g_BattleModels[actor].animId == 1 && (s16)D_80151200[actor].D_80151230 > 0) {
        return value;
    }
    return value + (s16)D_80151200[actor].D_80151230;
}

// magnitude of (arg0 - arg1) via GTE sqrt
static s16 func_800C03B8(s16 arg0, s16 arg1) {
    s32 delta;

    delta = arg0 - arg1;
    return SquareRoot0(delta * delta);
}

static s32 func_800C03FC(s32 arg0, s32 arg1) { return arg0 < 0 ? -arg1 : arg1; }

static void func_800C0480(s16);
static void func_800C0630(s16);
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

static void func_800C0480(s16 arg0) {
    Unk800F7ED8* slot;

    slot = &g_BattleCameraSlots[arg0];
    slot->D_800F7EDA = (u16)slot->D_800F7EDA + 1;
    if (D_80151844[slot->unk12].unkD == 0) {
        slot->unk1E = 0;
        slot->unk1A = (u16)slot->unk4;
        return;
    }
    slot->unk14 =
        ((func_800C03FC(slot->unk6 - slot->unkC, func_800C03B8(slot->unk6, slot->unkC)) + 1) * 0x10) / slot->unk4;
    slot->unk16 =
        ((func_800C03FC(slot->unk8 - slot->unkE, func_800C03B8(slot->unk8, slot->unkE)) + 1) * 0x10) / slot->unk4;
    slot->unk18 =
        ((func_800C03FC(slot->unkA - slot->unk10, func_800C03B8(slot->unkA, slot->unk10)) + 1) * 0x10) / slot->unk4;
}

static void func_800C0630(s16 slotIndex) {
    Unk800F7ED8* slot;
    s32 z;
    s32 y;
    s32 x;
    s32 categoryIndex;

    slot = &g_BattleCameraSlots[slotIndex];
    if (slot->unk4 == 1) {
        slot->D_800F7ED8 = -1;
        D_80151844[slot->unk12].x = slot->unk6;
        D_80151844[slot->unk12].y = slot->unk8;
        D_80151844[slot->unk12].z = slot->unkA;
        return;
    }
    categoryIndex = slot->unk12;
    if (D_80151844[categoryIndex].unkD == 0) {
        D_80151844[slot->unk12].x = func_800C1304(slot->unkC, slot->unk6, slot->unk1A, slot->unk1E);
        D_80151844[slot->unk12].y = func_800C1304(slot->unkE, slot->unk8, slot->unk1A, slot->unk1E);
        D_80151844[slot->unk12].z = func_800C1304(slot->unk10, slot->unkA, slot->unk1A, slot->unk1E);
        slot->unk1E = (s16)((u16)slot->unk1E + 1);
    } else {
        x = D_80151844[categoryIndex].x * 0x10;
        *(s32*)0x1F800000 = x;
        y = D_80151844[slot->unk12].y * 0x10;
        *(s32*)0x1F800004 = y;
        z = D_80151844[slot->unk12].z * 0x10;
        *(s32*)0x1F800008 = z;
        D_80151844[slot->unk12].x = (slot->unk14 + x) >> 4;
        D_80151844[slot->unk12].y = (slot->unk16 + y) >> 4;
        D_80151844[slot->unk12].z = (slot->unk18 + z) >> 4;
    }
    slot->unk4 = (s16)((u16)slot->unk4 - 1);
}

static void func_800C0970(s16);
static void func_800C0B20(s16);
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

static void func_800C0970(s16 arg0) {
    Unk800F7ED8* slot;

    slot = &g_BattleCameraSlots[arg0];
    slot->D_800F7EDA = (u16)slot->D_800F7EDA + 1;
    if (D_801518A4[slot->unk12].flags == 0) {
        slot->unk1E = 0;
        slot->unk1A = (u16)slot->unk4;
        return;
    }
    slot->unk14 =
        ((func_800C03FC(slot->unk6 - slot->unkC, func_800C03B8(slot->unk6, slot->unkC)) + 1) * 0x10) / slot->unk4;
    slot->unk16 =
        ((func_800C03FC(slot->unk8 - slot->unkE, func_800C03B8(slot->unk8, slot->unkE)) + 1) * 0x10) / slot->unk4;
    slot->unk18 =
        ((func_800C03FC(slot->unkA - slot->unk10, func_800C03B8(slot->unkA, slot->unk10)) + 1) * 0x10) / slot->unk4;
}

static void func_800C0B20(s16 slotIndex) {
    Unk800F7ED8* slot;
    s32 z;
    s32 y;
    s32 x;
    s32 categoryIndex;

    slot = &g_BattleCameraSlots[slotIndex];
    if (slot->unk4 == 1) {
        slot->D_800F7ED8 = -1;
        D_801518A4[slot->unk12].x = slot->unk6;
        D_801518A4[slot->unk12].y = slot->unk8;
        D_801518A4[slot->unk12].z = slot->unkA;
        return;
    }
    categoryIndex = slot->unk12;
    if (D_801518A4[categoryIndex].flags == 0) {
        D_801518A4[slot->unk12].x = func_800C1304(slot->unkC, slot->unk6, slot->unk1A, slot->unk1E);
        D_801518A4[slot->unk12].y = func_800C1304(slot->unkE, slot->unk8, slot->unk1A, slot->unk1E);
        D_801518A4[slot->unk12].z = func_800C1304(slot->unk10, slot->unkA, slot->unk1A, slot->unk1E);
    } else {
        x = D_801518A4[categoryIndex].x * 0x10;
        *(s32*)0x1F800000 = x;
        y = D_801518A4[slot->unk12].y * 0x10;
        *(s32*)0x1F800004 = y;
        z = D_801518A4[slot->unk12].z * 0x10;
        *(s32*)0x1F800008 = z;
        D_801518A4[slot->unk12].x = (slot->unk14 + x) >> 4;
        D_801518A4[slot->unk12].y = (slot->unk16 + y) >> 4;
        D_801518A4[slot->unk12].z = (slot->unk18 + z) >> 4;
    }
    slot->unk1E = (s16)((u16)slot->unk1E + 1);
    slot->unk4 = (s16)((u16)slot->unk4 - 1);
}

void func_800C0DD8(s16 actor, s32 categoryArg, s32 flagsArg) {
    s32 z;
    s32 radius;
    s32 x;
    s32 angle;
    u32 scale;
    s32 category = categoryArg;
    s32 flags = flagsArg;

    scale = D_80151200[actor].values4[0];
    if (g_BattleModels[actor].scale < 0x1000) {
        scale >>= 1;
        if ((s32)scale < 0x100) {
            scale = 0x100;
        }
    }
    *(s32*)0x1F800000 = (s32)(scale * *(s32*)0x1F800000) >> 8;
    scale = D_80151200[actor].values4[1];
    if (g_BattleModels[actor].scale < 0x1000) {
        scale >>= 1;
        if ((s32)scale < 0x100) {
            scale = 0x100;
        }
    }
    *(s32*)0x1F800004 = (s32)(scale * *(s32*)0x1F800004) >> 8;
    scale = D_80151200[actor].values4[2];
    if (g_BattleModels[actor].scale < 0x1000) {
        scale >>= 1;
        if ((s32)scale < 0x100) {
            scale = 0x100;
        }
    }
    z = (s32)(scale * *(s32*)0x1F800008) >> 8;
    *(s32*)0x1F800008 = z;
    if (D_80151844[category & 0xFF].flags != 0) {
        x = *(s32*)0x1F800000;
        radius = SquareRoot0((x * x) + (z * z));
        if (flags & 0xFF) {
            angle = ratan2(*(s32*)0x1F800000, *(s32*)0x1F800008) + D_80151200[actor].D_8015122C;
        } else {
            angle = ratan2(*(s32*)0x1F800000, *(s32*)0x1F800008) + g_BattleModels[actor].rootRot.vy;
        }
        *(s32*)0x1F800000 = (s32)((s32)(rsin(angle) * radius) >> 0xC);
        *(s32*)0x1F800008 = (s32)((s32)(rcos(angle) * radius) >> 0xC);
    }
}

void func_800C1104(void) {
    s32 cameraIndex;
    s32 dx;
    s32 targetDy;
    s32 targetDx;
    s32 dz;
    s32 dy;
    s32 targetDz;

    D_80151844[3].x = (u16)g_BattleCameraPos.vx;
    D_80151844[3].y = g_BattleCameraPos.vy;
    D_80151844[3].z = g_BattleCameraPos.vz;
    D_801518A4[3].x = (u16)g_BattleCameraTarget.vx;
    D_801518A4[3].y = g_BattleCameraTarget.vy;
    D_801518A4[3].z = g_BattleCameraTarget.vz;
    if (D_801031E0 != 0) {
        cameraIndex = D_80163B3C;
        dx = (s32)(g_BattleData.activeEncounter.camera[D_80163B3C].start.x - g_BattleCameraPos.vx) / 2;
        D_80151844[3].x = (u16)g_BattleCameraPos.vx + dx;
        dy = (s32)(g_BattleData.activeEncounter.camera[cameraIndex].start.y - g_BattleCameraPos.vy) / 2;
        D_80151844[3].y = g_BattleCameraPos.vy + dy;
        dz = (s32)(g_BattleData.activeEncounter.camera[cameraIndex].start.z - g_BattleCameraPos.vz) / 2;
        D_80151844[3].z = g_BattleCameraPos.vz + dz;
        targetDx = (s32)(g_BattleData.activeEncounter.camera[cameraIndex].direction.x - g_BattleCameraTarget.vx) / 2;
        D_801518A4[3].x = (u16)g_BattleCameraTarget.vx + targetDx;
        targetDy = (s32)(g_BattleData.activeEncounter.camera[cameraIndex].direction.y - g_BattleCameraTarget.vy) / 2;
        D_801518A4[3].y = g_BattleCameraTarget.vy + targetDy;
        targetDz = (s32)(g_BattleData.activeEncounter.camera[cameraIndex].direction.z - g_BattleCameraTarget.vz) / 2;
        D_801518A4[3].z = g_BattleCameraTarget.vz + targetDz;
        if ((dx == 0) && (dy == 0) && (dz == 0) && (targetDx == 0) && (targetDy == 0) && (targetDz == 0)) {
            D_801031E0 = 0;
        }
    }
}

// cosine-eased interpolation between arg0 and arg1
static s32 func_800C1304(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 val;
    s32 delta;

    delta = arg1 - arg0;
    val = (rcos((s16)(((arg3 << 0xB) / arg2) + 0x800)) + 0x1000) * delta;
    return arg0 + val / 0x2000;
}

void func_800C1394(s32 actorArg) {
    s32 actor = actorArg & 0xFF;
    BattleModelSub* bones;
    BattleModelSub* flash;
    bones = g_BattleModels[actor].boneTransforms;
    g_BattleModels[actor].boneTransforms[49].trans.vy = D_80151200[actor].height;
    g_BattleModels[actor].boneTransforms[49].rot.vy += 0x80;
    flash = &bones[49];
    RotMatrixYXZ(&bones[49].rot, &flash->m);
    BattleUpdateMatrixWithSelfAndParentAndSetToGte(flash);
    *(s32**)0x1F800020 = D_800EA470;
    *(s32*)0x1F800024 = 0x90;
    *(s16*)0x1F80002E = 0;
    *(s16*)0x1F80002A = 0;
    *(s16*)0x1F800028 = 0;
    *(s16*)0x1F80002C = 0x20;
    D_80163C74 = func_800D29D4((ModelRenderDesc*)0x1F800020, g_cDb->unk70, 0xC, D_80163C74);
}

void func_800C14C0(s32 actorArg) {
    u8 actor;
    s32 status;
    s32 flags;
    s32 selected;
    s32 savedActor;

    actor = actorArg;
    if (actor != 3) {
        if (!(g_BattleModels[actor].specialFlags & 4)) {
            status = g_BattleData.actors[actor].D_801636C4;
            if (status & 0x200000) {
                BattleAddStatusDigitsToRender(actor, g_BattleData.actors[actor].D_801636BA);
            } else if (status & 0x2000) {
                BattleAddStatusDigitsToRender(actor, g_BattleData.actors[actor].D_801636BB);
            } else {
                flags = D_80151200[actor].D_80151200;
                if (flags & 4) {
                    BattleAddStatusIconToRender(actor, 0xE0);
                } else if (flags & 0x80) {
                    BattleAddStatusIconToRender(actor, 0xEF);
                }
            }
        }
        savedActor = actorArg & 0xFF;
        if (!(g_BattleModels[savedActor].specialFlags & 1)) {
            if (BattleGetStatusFlashState(1) == 2 && D_800F38A0 == savedActor) {
                func_800C1394(D_800F38A0);
            }
            if (BattleGetStatusFlashState(0x13) == 2) {
                selected = D_80158CFC + 4;
                if (selected == (actorArg & 0xFF)) {
                    func_800C1394(selected);
                }
            }
        }
    }
}

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

void* BattleCreateStatusIconPacket(u_long** ot, SVECTOR* pos, s16 u, s16 v) {
    MATRIX* matrix;
    s32* widthFlag;
    s32* heightFlag;
    POLY_FT4* poly;
    s16 top;
    s32 depth;
    s32 height;
    s32 width;
    s32 adjustedWidth;
    s32 otIndex;
    widthFlag = (s32*)0x1F800010;
    heightFlag = (s32*)0x1F800014;
    poly = D_80163C74;
    SetPolyFT4((POLY_FT4*)poly);
    poly->tpage = 0x3F;
    poly->clut = GetClut(0x100, 0x1E1);
    poly->r0 = 0x80;
    poly->g0 = 0x80;
    poly->b0 = 0x80;
    RotTrans(pos, (VECTOR*)D_800E7D30.t, (s32*)0x1F800008);
    matrix = &D_800E7D30;
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    *heightFlag = 0;
    *widthFlag = 0;
    depth = RotAverage4(&D_800EA1C8[0], &D_800EA1C8[1], &D_800EA1C8[2], &D_800EA1C8[3], (s32*)&poly->x0,
                        (s32*)&poly->x1, (s32*)&poly->x2, (s32*)&poly->x3, (s32*)0x1F800000, (s32*)0x1F800008);
    width = poly->x1 - poly->x0;
    *(s32*)0x1F800000 = width;
    if (width < 0x10) {
        poly->x0 = (s16)((u16)poly->x1 - 0xF);
        poly->x2 = (s16)(poly->x3 - 0xF);
        *widthFlag = 1;
    }
    height = poly->y2 - poly->y0;
    *(s32*)0x1F800000 = height;
    if (height < 0x10) {
        top = (u16)poly->y2 - 0x10;
        poly->y1 = top;
        poly->y0 = top;
        *heightFlag = 1;
    }
    adjustedWidth = *widthFlag;
    if ((adjustedWidth == 1) && (*heightFlag == adjustedWidth)) {
        setUV4(poly, u, v, u + 0xF, v, u, v + 0x10, u + 0xF, v + 0x10);
    } else {
        setUV4(poly, u, v, u + 0xF, v, u, v + 0xF, u + 0xF, v + 0xF);
    }
    depth >>= 2;
    if (depth >= 2) {
        if (depth > 0x1000)
            otIndex = 0x1000;
        else
            otIndex = depth;
    } else
        otIndex = 2;
    addPrim(ot[otIndex], poly);
    return poly + 1;
}

void BattleAddStatusIconToRender(s32 arg0, s16 flags) {
    u8 actor = arg0;
    D_800F4B04->vx = g_BattleModels[actor].boneTransforms[0].trans.vx;
    D_800F4B04->vy = (s16)D_80151200[actor].unk2A;
    D_800F4B04->vz = g_BattleModels[actor].boneTransforms[0].trans.vz;
    D_80163C74 = BattleCreateStatusIconPacket(g_cDb->unk70, D_800F4B04, D_800EA1E8[(D_800EA1EC >> 3) & 3], flags);
    D_800EA1EC++;
}

static void BattleGet4DigitsFromValue(s16 arg0, s16* arg1) {
    s32 i;

    for (i = 0; i < 4; i++) {
        arg1[3 - i] = arg0 % 10;
        arg0 /= 0xA;
    }
}

void BattleAddStatusDigitsToRender(s32 arg0, u8 arg1) {
    u8 actor = arg0;

    D_800F4B08->vx = g_BattleModels[actor].boneTransforms[0].trans.vx;
    D_800F4B08->vy = (s16)D_80151200[actor].unk2A;
    D_800F4B08->vz = g_BattleModels[actor].boneTransforms[0].trans.vz;
    if ((arg1 & 0xFF) != 0xFF) {
        BattleGet4DigitsFromValue(arg1, D_800F4B0C);
        D_80163C74 =
            BattleCreateStatusDigitsPackets(g_cDb->unk70, D_800F4B08, ((u8*)D_800F4B0C)[4], ((u8*)D_800F4B0C)[6]);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleCreateStatusDigitsPackets);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2704);

void func_800C2864(s32 arg0) {
    u8 actor = arg0;
    if (D_80163C7C == 4 && D_800FAFDC == 0 && D_801620A4 == 0) {
        D_80163C74 = func_800C2704(&g_cDb->unk4080[1], (s16)((s16)g_modelScreenPos[actor].x + 3),
                                   (s16)(g_modelScreenPos[actor].y - 14), 0, 0xD0, 0x30, 0x10, 0);
    }
}

void func_800C2928(void) {
#ifdef PLATFORM_PSYZ
    s32 scratch[5];
#endif
    Unk801621F0* slots = g_BattleDetachedSlots;
    s32 slotOffset = g_BattleDetachedCursor * sizeof(Unk801621F0);
    s16 stateId = *(s16*)((u8*)&g_BattleDetachedSlots[0].raw.D_801621F2 + slotOffset);
    Unk801621F0* slot = (Unk801621F0*)((u8*)slots + slotOffset);
    BattleModelSub* bone;
    Unk80151200* state;
    s32 depth;
#ifndef PLATFORM_PSYZ
    s16* screen;
#endif
    s32 y;
    s32 actorOffset;
    s32 actor, boneActor, boneIndex;
    u16 frame;
    s32 updatedOffset;
    switch (stateId) {
    case 0:
        slot->D_801621F2 = 1;
        actor = slot->unk10.value;
        slot->D_801621F4 = 11;
        slot->unkC = 0;
        boneActor = slot->unk10.value;
        boneIndex = g_BattleModels[boneActor].boneIndices[0];
#ifdef PLATFORM_PSYZ
        scratch[0] = boneIndex;
#else
        *(s32*)0x1F800000 = boneIndex;
#endif
        BattleGetPartPosition(actor, boneIndex, &D_80151200[actor].bone.trans);
        actorOffset = slot->unk10.value * sizeof(Unk80151200);
        state = (Unk80151200*)((u8*)D_80151200 + actorOffset);
        state->bone.rot.vz = 0;
        state->bone.rot.vy = 0;
        *(s16*)((u8*)&D_80151200[0].bone.rot.vx + actorOffset) = 0;
        D_80151200[slot->unk10.value].bone.parentMatrix = &D_800F8158[0].m;
        return;
    case 1:
        if (slot->D_801621F4 == 0) {
            slot->D_801621F0 = -1;
            return;
        }
        bone = &D_80151200[slot->unk10.value].bone;
        RotMatrixYXZ(&bone->rot, &bone->m);
        BattleUpdateMatrixWithSelfAndParentAndSetToGte(&D_80151200[slot->unk10.value].bone);
#ifdef PLATFORM_PSYZ
        depth = RotTransPers(&D_800E7D08, &scratch[1], &scratch[2], &scratch[3]);
        slot->D_801621F6 = (u16)scratch[1];
        y = (s16)((u32)scratch[1] >> 16);
#else
        depth = RotTransPers(&D_800E7D08, (long*)0x1F800004, (long*)0x1F800008, (long*)0x1F80000C);
        screen = (s16*)0x1F800004;
#endif
#ifndef PLATFORM_PSYZ
        slot->D_801621F6 = (u16)screen[0];
        y = *(s32*)screen >> 16;
#endif
        slot->unkA = y;
#ifdef PLATFORM_PSYZ
        scratch[4] = depth;
#else
        *(s32*)0x1F800010 = depth;
#endif
        slot->unk8 = y - D_800EA230[slot->unkC];
        if (slot->D_801621F6 < 24)
            slot->D_801621F6 = 24;
        if (slot->D_801621F6 >= 297)
            slot->D_801621F6 = 296;
        if (slot->unk8 < 16)
            slot->unk8 = 16;
        if (slot->unk8 >= 151)
            slot->unk8 = 150;
        frame = slot->unkC;
        slot->unkC = frame + 1;
        func_800C2C1C(slot->D_801621F6, slot->unk8, slot->unkE, D_800EA230[(s16)frame], (s16)slot->unk14);
        updatedOffset = g_BattleDetachedCursor * sizeof(Unk801621F0);
        slots[g_BattleDetachedCursor].D_801621F4 =
            *(u16*)((u8*)&g_BattleDetachedSlots[0].raw.D_801621F4 + updatedOffset) - 1;
        return;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2C1C);

s32 func_800C2F20(s32 number, s16* digits) {
    s16 value = number;
    s32 i = 0;
    s32 quotient;
    s32 previous;
    s32 digitIndex;

    for (; i < 4; i++) {
        digitIndex = 3 - i;
        previous = (s16)value;
        quotient = previous / 10;
        value = quotient;
        digits[digitIndex] = (previous - quotient * 10) * 8 + 0x98;
    }
    for (i = 0; i < 3; i++) {
        if (digits[i] != 0x98) {
            return (4 - i) & 0xFF;
        }
        digits[i] = 0;
    }
    return 1;
}

void func_800C2FD4(s32 actor, s16 sound, s32 enabled) {
    if (enabled & 0xFF) {
        g_AkaoCmd.opcode = 0x2A;
        g_AkaoCmd.params[0] = (((s16)g_modelScreenPos[actor & 0xFF].prevX / 5) * 2) & 0x7E;
        g_AkaoCmd.params[1] = sound;
        AkaoExec();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3068);

void func_800C328C(void) {
    s16 phase;
    s32 actor;
    s32 modelIndex;
    s32 effectIndex;

    effectIndex = g_BattleEffectCursor;
    phase = g_BattleEffectSlots[effectIndex].raw.D_8016297A;
    switch (phase) {
    case 0:
        g_BattleEffectSlots[effectIndex].raw.D_8016297C = 0xE;
        g_BattleEffectSlots[effectIndex].raw.D_8016297A++;
        actor = g_BattleEffectSlots[effectIndex].raw.D_80162980;
        func_800C2FD4(actor & 0xFF, 0x16, (u8)g_BattleEffectSlots[effectIndex].raw.D_8016297E);
        D_80151200[actor].D_80151233 = 3;
        modelIndex = actor;
        g_BattleModels[modelIndex].colorR = 0xF8;
        g_BattleModels[modelIndex].colorG = 0;
        g_BattleModels[modelIndex].colorB = 0;
        g_BattleModels[modelIndex].blendAlpha = 0x800;
        func_800B5FE8(actor);
        /* fallthrough */
    case 1:
        func_800C3068((u8)g_BattleEffectCursor);
        return;
    }
}

void func_800C33F0(s32 effectArg) {
    s32 actor;
    s32 effectIndex;
    s32 modelIndexOnFinish;
    s32 effectIndexForUpdate;
    s32 stateIndexOnFinish;
    s32 modelIndex;

    effectIndex = effectArg & 0xFF;
    effectIndexForUpdate = effectIndex;
    actor = g_BattleEffectSlots[effectIndexForUpdate].raw.D_80162980;
    if (g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C == 0) {
        g_BattleEffectSlots[effectIndex].raw.D_80162978 = -1;
        modelIndexOnFinish = actor;
        g_BattleModels[modelIndexOnFinish].specialFlags = g_BattleModels[modelIndexOnFinish].specialFlags & 0x7F;
        stateIndexOnFinish = actor;
        D_80151200[stateIndexOnFinish].D_8015120C = D_80151200[stateIndexOnFinish].D_8015120C & 0xFFDF;
        g_BattleModels[modelIndexOnFinish].ready = 1;
        func_800B5AAC(actor & 0xFF);
        return;
    }
    modelIndex = actor;
    g_BattleModels[modelIndex].blendAlpha = g_BattleModels[modelIndex].blendAlpha + 0x80;
    g_BattleModels[modelIndex].colorR = g_BattleModels[modelIndex].colorR + 0xF0;
    g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C =
        g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C - 1;
}

void func_800C3578(void) {
    s16 phase;
    s32 actor;
    s32 modelIndex;
    s32 effectIndex;

    effectIndex = g_BattleEffectCursor;
    phase = g_BattleEffectSlots[effectIndex].raw.D_8016297A;
    switch (phase) {
    case 0:
        g_BattleEffectSlots[effectIndex].raw.D_8016297C = 0xE;
        g_BattleEffectSlots[effectIndex].raw.D_8016297A++;
        actor = g_BattleEffectSlots[effectIndex].raw.D_80162980;
        func_800C2FD4(actor & 0xFF, 0x16, (u8)g_BattleEffectSlots[effectIndex].raw.D_8016297E);
        modelIndex = actor;
        g_BattleModels[modelIndex].colorR = 0xF8;
        g_BattleModels[modelIndex].colorG = 0;
        g_BattleModels[modelIndex].colorB = 0;
        g_BattleModels[modelIndex].blendAlpha = 0x800;
        func_800B5FE8(actor);
        /* fallthrough */
    case 1:
        func_800C33F0((u8)g_BattleEffectCursor);
        return;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C36B4);

void func_800C3950(void) {
    s16 phase;
    s32 actor;
    s32 movementIndex;
    s32 modelIndex;
    u8 color;

    movementIndex = g_BattleMovementCursor;
    phase = g_BattleMovementSlots[movementIndex].D_801620AE;
    switch (phase) {
    case 0:
        g_BattleMovementSlots[movementIndex].D_801620B0 = 0xE;
        g_BattleMovementSlots[movementIndex].D_801620AE++;
        actor = g_BattleMovementSlots[movementIndex].D_801620B4;
        func_800C2FD4(actor & 0xFF, 0x16, (u8)g_BattleMovementSlots[movementIndex].D_801620B2);
        modelIndex = actor;
        g_BattleModels[modelIndex].blendAlpha = 0x800;
        color = (u8)g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
        g_BattleModels[modelIndex].colorG = 0;
        g_BattleModels[modelIndex].colorB = 0;
        g_BattleModels[modelIndex].colorR = color;
        func_800B5FE8(actor);
        /* fallthrough */
    case 1:
        func_800C36B4((u8)g_BattleMovementCursor);
        return;
    }
}

void func_800C3AA0(s32 effectArg) {
    s32 actor;
    s32 effectIndex;
    s32 modelIndexOnFinish;
    s32 effectIndexForUpdate;
    s32 stateIndexOnFinish;
    s32 modelIndex;
    s32 stateIndex;

    effectIndex = effectArg & 0xFF;
    effectIndexForUpdate = effectIndex;
    actor = g_BattleEffectSlots[effectIndexForUpdate].raw.D_80162980;
    if (g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C == 0) {
        g_BattleEffectSlots[effectIndex].raw.D_80162978 = -1;
        modelIndexOnFinish = actor;
        g_BattleModels[modelIndexOnFinish].specialFlags = g_BattleModels[modelIndexOnFinish].specialFlags & 0x7F;
        stateIndexOnFinish = actor;
        D_80151200[stateIndexOnFinish].D_8015120C = D_80151200[stateIndexOnFinish].D_8015120C & 0xFFDF;
        g_BattleModels[modelIndexOnFinish].ready = 1;
        func_800B5AAC(actor & 0xFF);
        return;
    }
    modelIndex = actor;
    g_BattleModels[modelIndex].blendAlpha = g_BattleModels[modelIndex].blendAlpha + 0x80;
    stateIndex = actor;
    g_BattleModels[modelIndex].colorR = g_BattleModels[modelIndex].colorR + 0xF0;
    D_80151200[stateIndex].D_80151238 = D_80151200[stateIndex].D_80151238 + 0x100;
    D_80151200[stateIndex].D_8015123A = D_80151200[stateIndex].D_8015123A - 0x100;
    D_80151200[stateIndex].D_8015123C = D_80151200[stateIndex].D_8015123C + 0x100;
    g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C =
        g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C - 1;
}

void func_800C3CA8(void) {
    s16 phase;
    s32 actor;
    s32 modelIndex;
    s32 effectIndex;

    effectIndex = g_BattleEffectCursor;
    phase = g_BattleEffectSlots[effectIndex].raw.D_8016297A;
    switch (phase) {
    case 0:
        g_BattleEffectSlots[effectIndex].raw.D_8016297C = 0xE;
        g_BattleEffectSlots[effectIndex].raw.D_8016297A++;
        actor = g_BattleEffectSlots[effectIndex].raw.D_80162980;
        func_800C2FD4(actor & 0xFF, 0x16, (u8)g_BattleEffectSlots[effectIndex].raw.D_8016297E);
        modelIndex = actor;
        g_BattleModels[modelIndex].colorR = 0xF8;
        g_BattleModels[modelIndex].colorG = 0;
        g_BattleModels[modelIndex].colorB = 0;
        g_BattleModels[modelIndex].blendAlpha = 0x800;
        func_800B5FE8(actor);
        /* fallthrough */
    case 1:
        func_800C3AA0((u8)g_BattleEffectCursor);
        return;
    }
}

void func_800C3DE4(void) {
    s16 frames;
    s32 actor;
    s32 slotIndex;
    s32 slotIndexBeforeCall;
    s32 slotIndexAfterCall;

    slotIndexBeforeCall = g_BattleDetachedCursor;
    frames = g_BattleDetachedSlots[slotIndexBeforeCall].raw.D_801621F4;
    actor = g_BattleDetachedSlots[slotIndexBeforeCall].raw.unk8;
    slotIndex = g_BattleDetachedCursor;
    if (frames == 0) {
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        return;
    }
    if ((frames == 0x3A) || (frames == 0x40)) {
        D_80163C74 = func_800C4FC8(0xFAU, 0xFAU, 0xFAU);
    }
    if (g_BattleDetachedSlots[slotIndex].raw.D_801621F4 & 1) {
        g_BattleModels[actor].rootTrans.vz = g_BattleDetachedSlots[slotIndex].raw.unkA + 0x40;
    } else {
        g_BattleModels[actor].rootTrans.vz = g_BattleDetachedSlots[slotIndex].raw.unkA - 0x40;
    }
    slotIndexAfterCall = slotIndex;
    g_BattleDetachedSlots[slotIndexAfterCall].raw.D_801621F4 =
        g_BattleDetachedSlots[slotIndexAfterCall].raw.D_801621F4 - 1;
}

void func_800C3F44(s32 effectArg) {
    s32 actor;
    s32 effectIndex;
    s32 modelIndexOnFinish;
    s32 effectIndexForUpdate;
    s32 stateIndexOnFinish;
    s32 modelIndex;

    effectIndex = effectArg & 0xFF;
    effectIndexForUpdate = effectIndex;
    actor = g_BattleEffectSlots[effectIndexForUpdate].raw.D_80162980;
    if (g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C == 1) {
        g_BattleEffectSlots[effectIndex].raw.D_80162978 = -1;
        modelIndexOnFinish = actor;
        g_BattleModels[modelIndexOnFinish].specialFlags = g_BattleModels[modelIndexOnFinish].specialFlags & 0x7F;
        stateIndexOnFinish = actor;
        D_80151200[stateIndexOnFinish].D_8015120C = D_80151200[stateIndexOnFinish].D_8015120C & 0xFFDF;
        g_BattleModels[modelIndexOnFinish].ready = 1;
        g_BattleModels[modelIndexOnFinish].colorR = 0;
        g_BattleModels[modelIndexOnFinish].blendAlpha = 0x1000;
        func_800B5AAC(actor & 0xFF);
        return;
    }
    modelIndex = actor;
    g_BattleModels[modelIndex].blendAlpha = g_BattleModels[modelIndex].blendAlpha + 0x20;
    g_BattleModels[modelIndex].colorR = g_BattleModels[modelIndex].colorR + 0xFC;
    g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C =
        g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C - 1;
}

void func_800C40F4(void) {
    s16 phase;
    s32 actor;
    s32 modelIndex;
    s32 slotIndex;
    s32 effectIndex;

    effectIndex = g_BattleEffectCursor;
    phase = g_BattleEffectSlots[effectIndex].raw.D_8016297A;
    switch (phase) {
    case 0:
        g_BattleEffectSlots[effectIndex].raw.D_8016297C = 0x40;
        g_BattleEffectSlots[effectIndex].raw.D_8016297A++;
        actor = g_BattleEffectSlots[effectIndex].raw.D_80162980;
        func_800C2FD4(actor & 0xFF, 0x1C0, (u8)g_BattleEffectSlots[effectIndex].raw.D_8016297E);
        slotIndex = BattleDetachedRegister(func_800C3DE4);
        modelIndex = actor;
        g_BattleDetachedSlots[slotIndex].raw.unk8 = actor;
        g_BattleDetachedSlots[slotIndex].raw.D_801621F4 = 0x40;
        g_BattleDetachedSlots[slotIndex].raw.unkA = g_BattleModels[modelIndex].rootTrans.vz;
        g_BattleModels[modelIndex].colorR = 0xFF;
        g_BattleModels[modelIndex].colorG = 0;
        g_BattleModels[modelIndex].colorB = 0;
        g_BattleModels[modelIndex].blendAlpha = 0x800;
        func_800B5FE8(actor);
        /* fallthrough */
    case 1:
        func_800C3F44((u8)g_BattleEffectCursor);
        return;
    }
}

void func_800C428C(s32 effectArg) {
    s32 actor;
    s32 effectIndex;
    s32 modelIndexOnFinish;
    s32 effectIndexForUpdate;
    s32 stateIndexOnFinish;
    s32 stateIndex;
    s32 modelIndex;

    effectIndex = effectArg & 0xFF;
    effectIndexForUpdate = effectIndex;
    actor = g_BattleEffectSlots[effectIndexForUpdate].raw.D_80162980;
    if (g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C == 0) {
        g_BattleEffectSlots[effectIndex].raw.D_80162978 = -1;
        modelIndexOnFinish = actor;
        g_BattleModels[modelIndexOnFinish].specialFlags = g_BattleModels[modelIndexOnFinish].specialFlags & 0x7F;
        stateIndexOnFinish = actor;
        D_80151200[stateIndexOnFinish].D_8015120C = D_80151200[stateIndexOnFinish].D_8015120C & 0xFFDF;
        g_BattleModels[modelIndexOnFinish].ready = 1;
        return;
    }
    modelIndex = actor;
    g_BattleModels[modelIndex].blendAlpha = g_BattleModels[modelIndex].blendAlpha + 0x80;
    g_BattleModels[modelIndex].colorR = g_BattleModels[modelIndex].colorR + 0xF8;
    g_BattleModels[modelIndex].colorG = g_BattleModels[modelIndex].colorG + 0xF8;
    stateIndex = actor;
    g_BattleModels[modelIndex].colorB = g_BattleModels[modelIndex].colorB + 0xF8;
    D_80151200[stateIndex].D_80151238 = 0;
    D_80151200[stateIndex].D_8015123A = 0x1000;
    D_80151200[stateIndex].D_8015123C = 0x1000;
    g_BattleModels[modelIndex].rootRot.vy = g_BattleModels[modelIndex].rootRot.vy + 0x100;
    g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C =
        g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C - 1;
}

void func_800C44B4(void) {
    s16 phase;
    s32 actor;
    s32 modelIndex;
    s32 effectIndex;

    effectIndex = g_BattleEffectCursor;
    phase = g_BattleEffectSlots[effectIndex].raw.D_8016297A;
    switch (phase) {
    case 0:
        g_BattleEffectSlots[effectIndex].raw.D_8016297C = 0x10;
        g_BattleEffectSlots[effectIndex].raw.D_8016297A++;
        actor = g_BattleEffectSlots[effectIndex].raw.D_80162980;
        func_800C2FD4(actor & 0xFF, 0x16, (u8)g_BattleEffectSlots[effectIndex].raw.D_8016297E);
        modelIndex = actor;
        g_BattleModels[modelIndex].colorR = 0x80;
        g_BattleModels[modelIndex].colorG = 0x80;
        g_BattleModels[modelIndex].colorB = 0x80;
        g_BattleModels[modelIndex].blendAlpha = 0;
        func_800B5FE8(actor);
        /* fallthrough */
    case 1:
        func_800C428C((u8)g_BattleEffectCursor);
        return;
    }
}

void func_800C45EC(s32 effectArg) {
    s32 actor;
    s32 effectIndex;
    s32 modelIndexOnFinish;
    s32 effectIndexForUpdate;
    s32 stateIndexOnFinish;
    s32 stateIndex;
    s32 modelIndex;

    effectIndex = effectArg & 0xFF;
    effectIndexForUpdate = effectIndex;
    actor = g_BattleEffectSlots[effectIndexForUpdate].raw.D_80162980;
    if (g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C == 0) {
        g_BattleEffectSlots[effectIndex].raw.D_80162978 = -1;
        modelIndexOnFinish = actor;
        g_BattleModels[modelIndexOnFinish].specialFlags = g_BattleModels[modelIndexOnFinish].specialFlags & 0x7F;
        stateIndexOnFinish = actor;
        D_80151200[stateIndexOnFinish].D_8015120C = D_80151200[stateIndexOnFinish].D_8015120C & 0xFFDF;
        g_BattleModels[modelIndexOnFinish].ready = 1;
        return;
    }
    modelIndex = actor;
    g_BattleModels[modelIndex].blendAlpha = g_BattleModels[modelIndex].blendAlpha + 0x80;
    g_BattleModels[modelIndex].colorR = g_BattleModels[modelIndex].colorR + 0xF8;
    g_BattleModels[modelIndex].colorG = g_BattleModels[modelIndex].colorG + 0xF8;
    stateIndex = actor;
    g_BattleModels[modelIndex].colorB = g_BattleModels[modelIndex].colorB + 0xF8;
    D_80151200[stateIndex].D_80151238 = 0x1000;
    D_80151200[stateIndex].D_8015123A = 0x1000;
    D_80151200[stateIndex].D_8015123C = 0;
    g_BattleModels[modelIndex].rootTrans.vy = g_BattleModels[modelIndex].rootTrans.vy - 0x80;
    g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C =
        g_BattleEffectSlots[effectIndexForUpdate].raw.D_8016297C - 1;
}

void func_800C4814(void) {
    s16 phase;
    s32 actor;
    s32 modelIndex;
    s32 effectIndex;

    effectIndex = g_BattleEffectCursor;
    phase = g_BattleEffectSlots[effectIndex].raw.D_8016297A;
    switch (phase) {
    case 0:
        g_BattleEffectSlots[effectIndex].raw.D_8016297C = 0x10;
        g_BattleEffectSlots[effectIndex].raw.D_8016297A++;
        actor = g_BattleEffectSlots[effectIndex].raw.D_80162980;
        func_800C2FD4(actor & 0xFF, 0x16, (u8)g_BattleEffectSlots[effectIndex].raw.D_8016297E);
        modelIndex = actor;
        g_BattleModels[modelIndex].colorR = 0x80;
        g_BattleModels[modelIndex].colorG = 0x80;
        g_BattleModels[modelIndex].colorB = 0x80;
        g_BattleModels[modelIndex].blendAlpha = 0;
        func_800B5FE8(actor);
        /* fallthrough */
    case 1:
        func_800C45EC((u8)g_BattleEffectCursor);
        return;
    }
}

void func_800C494C(void) {
    switch (g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.state) {
    case 0:
        g_BattleScreenFadeR = 0;
        g_BattleScreenFadeG = 0;
        g_BattleScreenFadeB = 0;
        g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.state = 1;
        g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.framesLeft = 15;
        g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.colorStep = 16;
        D_80162974 = 1;
        D_80163C74 = func_800C5004(g_BattleScreenFadeR, g_BattleScreenFadeG, g_BattleScreenFadeB);
        g_BattleScreenFadeG = g_BattleScreenFadeB = g_BattleScreenFadeR +=
            g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.colorStep;
        g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.framesLeft--;
        break;
    case 1:
        if (g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.framesLeft == 0) {
            g_BattleScreenFadeR = 0xFF;
            g_BattleScreenFadeG = 0xFF;
            g_BattleScreenFadeB = 0xFF;
            g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.state = 2;
            D_80163C74 = func_800C5004(g_BattleScreenFadeR, g_BattleScreenFadeG, g_BattleScreenFadeB);
            SetDispMask(0);
        } else {
            D_80163C74 = func_800C5004(g_BattleScreenFadeR, g_BattleScreenFadeG, g_BattleScreenFadeB);
            g_BattleScreenFadeG = g_BattleScreenFadeB = g_BattleScreenFadeR +=
                g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.colorStep;
            g_BattleDetachedSlots[g_BattleDetachedCursor].screenFade.framesLeft--;
        }
        break;
    case 2:
        D_80163C74 = func_800C5004(g_BattleScreenFadeR, g_BattleScreenFadeG, g_BattleScreenFadeB);
        SetDispMask(0);
        break;
    }
}

static void func_800C4B60(s16 arg0) {
    if (g_BattleDetachedSlots[arg0].bands.framesLeft == 0) {
        g_BattleDetachedSlots[arg0].raw.D_801621F0 = -1;
        return;
    }
    D_80163C74 = func_800C4DC8(0, g_BattleDetachedSlots[arg0].bands.upperY, 320, 47, &D_800EA25C);
    D_80163C74 = func_800C4DC8(0, g_BattleDetachedSlots[arg0].bands.upperY + 47, 320, 32, &D_800EA258);
    D_80163C74 = func_800C4DC8(0, g_BattleDetachedSlots[arg0].bands.lowerY, 320, 32, &D_800EA260);
    D_80163C74 = func_800C4DC8(0, g_BattleDetachedSlots[arg0].bands.lowerY + 32, 320, 47, &D_800EA25C);
    g_BattleDetachedSlots[arg0].bands.lowerY += 4;
    g_BattleDetachedSlots[arg0].bands.upperY -= 4;
    g_BattleDetachedSlots[arg0].bands.framesLeft--;
}

static void func_800C4D10(void) {
    int arg0;

    arg0 = g_BattleDetachedCursor;
    switch (g_BattleDetachedSlots[arg0].bands.state) {
    case 0:
        g_BattleDetachedSlots[arg0].bands.framesLeft = 21;
        g_BattleDetachedSlots[arg0].bands.lowerY = 87;
        g_BattleDetachedSlots[arg0].bands.upperY = 8;
        g_BattleDetachedSlots[arg0].bands.state++;
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
            var_v0_2 = BattleDetachedRegister(func_800C55B8);
            g_BattleDetachedSlots[var_v0_2].modelScale.actor = arg0;
            g_BattleDetachedSlots[var_v0_2].modelScale.framesLeft = 0x10;
            g_BattleDetachedSlots[var_v0_2].modelScale.scaleStep = -0x80;
        }
    } else {
        temp_a1 = D_80151200[var_v0].D_8015120C;
        if (temp_a1 & 0x80) {
            D_80151200[var_v0].D_8015120C = temp_a1 & (~0x80);
            var_v0_2 = BattleDetachedRegister(func_800C55B8);
            var_v0_2 = var_v0_2;
            g_BattleDetachedSlots[var_v0_2].modelScale.actor = arg0;
            g_BattleDetachedSlots[var_v0_2].modelScale.framesLeft = 0x10;
            g_BattleDetachedSlots[var_v0_2].modelScale.scaleStep = 0x80;
        }
    }
}

static void func_800C55B8(void) {
    if (g_BattleDetachedSlots[g_BattleDetachedCursor].modelScale.framesLeft == 0) {
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        return;
    }
    g_BattleModels[g_BattleDetachedSlots[g_BattleDetachedCursor].modelScale.actor].scale +=
        g_BattleDetachedSlots[g_BattleDetachedCursor].modelScale.scaleStep;
    g_BattleDetachedSlots[g_BattleDetachedCursor].modelScale.framesLeft--;
}

static void func_800C5694(void) {
    if (g_BattleEffectSlots[g_BattleEffectCursor].modelScale.framesLeft == 0) {
        g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162978 = -1;
        g_BattleModels[g_BattleEffectSlots[g_BattleEffectCursor].modelScale.actor].ready = 1;
        return;
    }
    g_BattleModels[g_BattleEffectSlots[g_BattleEffectCursor].modelScale.actor].scale +=
        g_BattleEffectSlots[g_BattleEffectCursor].modelScale.scaleStep;
    g_BattleEffectSlots[g_BattleEffectCursor].modelScale.framesLeft--;
}

void func_800C57B0(void) {
    StageParticles* particles = &D_801590E4.stage.particles;
    BattleModelSub* bone = particles->bones;
    u8* state = (u8*)&particles->state;
    s32 i;
    for (i = 0; i < 256; i++) {
        bone->trans.vx = (BattleGetRndU16() & 0x3FFF) - 0x2000;
        bone->trans.vy = -10000 - BattleGetRndU16() / 2;
        bone->trans.vz = (BattleGetRndU16() & 0x3FFF) - 0x2000;
        bone->rot.vx = 0;
        bone->rot.vy = 0;
        bone->rot.vz = 0;
        bone->parentMatrix = &g_BattleWorldView.m;
        bone++;
        state[0] = 0;
        state[1] = 0;
        state += 2;
    }
}

void func_800C5864(void) {
    StageParticles* particles = &D_801590E4.stage.particles;
    BattleModelSub* bone = particles->bones;
    u8* state = (u8*)&particles->state;
    s32 i = 0;
    u8* texture = (u8*)&particles->state + 1;
    BattleModelSub* transform = bone;
    do {
        switch (*state) {
        case 0:
            if (transform->trans.vy > 0) {
                *state = 1;
            } else {
                transform->trans.vy += 400;
            }
            break;
        case 8:
            *state = 0;
            transform->trans.vy = -10000 - BattleGetRndU16() / 2;
            break;
        default:
            transform->trans.vy = 0;
            break;
        }
        RotMatrixYXZ(&transform->rot, &bone->m);
        BattleUpdateMatrixWithSelfAndParentAndSetToGte(bone);
        if (*state == 0) {
            D_80163C74 = func_800C5ADC(g_cDb->unk70);
        } else {
            (*state)++;
            *texture += 32;
            D_80163C74 = func_800C59B8(g_cDb->unk4078, *texture);
        }
        transform++;
        bone++;
        texture += 2;
        i++;
        state += 2;
    } while (i < 256);
}

void* func_800C59B8(u_long* ot, s16 texture) {
    POLY_FT4* poly = D_80163C74;
    SetPolyFT4(poly);
    poly->tpage = 0x28A;
    poly->clut = GetClut(0x140, 0x1FE);
    poly->r0 = 0x80;
    poly->g0 = 0x80;
    poly->b0 = 0x80;
    RotTransPers4(&D_800EA264[0], &D_800EA264[1], &D_800EA264[2], &D_800EA264[3], (s32*)&poly->x0, (s32*)&poly->x1,
                  (s32*)&poly->x2, (s32*)&poly->x3, (s32*)0x1F800000, (s32*)0x1F800008);
    setUV4(poly, texture, 0xA0, texture + 32, 0xA0, texture, 0xC0, texture + 32, 0xC0);
    addPrim(ot, poly);
    return poly + 1;
}

void* func_800C5ADC(u_long** ot) {
    s32 p;
    s32 flag;
    LINE_G2* line = D_80163C74;
    s32 depth;
    s32 result;
    u8 color;
    SetLineG2(line);
    RotTransPers(&D_800EA284[0], (s32*)&line->x0, &p, &flag);
    result = RotTransPers(&D_800EA284[1], (s32*)&line->x1, &p, &flag);
    color = 0x80;
    line->r1 = color;
    line->g1 = color;
    line->b1 = color;
    color = 1;
    depth = result >> 2;
    line->r0 = color;
    line->g0 = color;
    line->b0 = color;
    if (depth >= 2) {
        if (depth >= 0x1001)
            depth = 0x1000;
    } else {
        depth = 2;
    }
    addPrim(ot[depth], line);
    return line + 1;
}

// reset each slot's first field to -1 (empty)
static void func_800C5BEC(void) {
    s32 fill;
    s32 i;

    fill = -1;
    for (i = 0x17A; i >= 0; i -= 6) {
        *(s16*)&((u8*)D_800F9DA8)[i] = fill;
    }
}

void func_800C5C18(s32 id, s32 delay, s32 duration, s32 playSound) {
    s32 i = 0;
    s32 empty;
    u8* target;
    BattleBannerMessage* slot;

    empty = -1;
    delay += 0xFF;
    target = &D_800F9DA8[0].duration;
    slot = (BattleBannerMessage*)(target - 5);
    for (; i < 64; i++) {
        if (slot->stringId == empty) {
            slot->stringId = id;
            D_800F9DA8[i].delay = delay;
            D_800F9DA8[i].playSound = playSound;
            if (!(u8)duration) {
                *target = func_800C60F4();
            } else {
                *target = duration;
            }
            return;
        }
        target += 6;
        slot++;
    }
}

void func_800C5CC0(void) {
    s32 i;
    s32 j;
    D_800F9DA4 |= 2;
    if (D_800F9DA8[0].stringId != -1) {
        if (D_800F9DA8[0].delay == 0) {
            if (D_800F9DA8[0].playSound != 0) {
                func_800BBA40(0x2D7);
                D_800F9DA8[0].playSound = 0;
            }
            if (D_800F9DA8[0].duration == 0) {
                D_800F9DA8[0].stringId = -1;
                for (i = 0; i < 64; i++) {
                    if (D_800F9DA8[0].stringId == -1) {
                        for (j = 0; j < 63; j++) {
                            D_800F9DA8[j].stringId = D_800F9DA8[j + 1].stringId;
                            D_800F9DA8[j].delay = D_800F9DA8[j + 1].delay;
                            D_800F9DA8[j].duration = D_800F9DA8[j + 1].duration;
                            D_800F9DA8[j].playSound = D_800F9DA8[j + 1].playSound;
                        }
                        D_800F9DA8[63].stringId = -1;
                    }
                }
            } else {
                BattleBannerSetEncounterString(D_800F9DA8[0].stringId);
                D_800F9DA8[0].duration--;
            }
        } else {
            D_800F9DA8[0].delay--;
            goto clear;
        }
    } else {
    clear:
        BattleBannerSetEncounterString(-1);
        D_800F9DA4 &= 0xFFFD;
    }
}

void BattleSelectPlayerModelFiles(void) {
    s32 i;
    s32 pass;
    s32 weapon;
    u16 tmpFile;
    u16 tmpActor;
    s32 leftFile;
    s32 leftActor;

    for (i = 0; i < 3; i++) {
        if (g_BattleData.actors[i].charId == -1) {
            D_800FA9C4[i].file = 200;
        } else {
            switch (g_BattleData.actors[i].charId) {
            case 1:
                weapon = SysGetPartyPlayerStructureAddressByPartyId(i)->weapon.weaponModel & 0xF0;
                switch (weapon) {
                case 0x10:
                    D_800FA9C4[i].file = 11;
                    break;
                case 0x20:
                    D_800FA9C4[i].file = 12;
                    break;
                case 0x30:
                    D_800FA9C4[i].file = 13;
                    break;
                default:
                    D_800FA9C4[i].file = 1;
                    break;
                }
                break;
            case 7:
                weapon = SysGetPartyPlayerStructureAddressByPartyId(i)->weapon.weaponModel & 0xF0;
                switch (weapon) {
                case 0x10:
                    D_800FA9C4[i].file = 14;
                    break;
                case 0x20:
                    D_800FA9C4[i].file = 15;
                    break;
                default:
                    D_800FA9C4[i].file = 7;
                    break;
                }
                break;
            default:
                if (g_BattleData.activeEncounter.setup.stageID == 0x4E && g_BattleData.actors[i].charId == 0) {
                    D_800FA9C4[i].file = 16;
                } else {
                    D_800FA9C4[i].file = g_BattleData.actors[i].charId;
                }
                break;
            }
        }
        D_800FA9C4[i].actor = i;
    }

    for (pass = 0; pass < 2; pass++) {
        for (i = 0; i < 2; i++) {
            leftFile = D_800FA9C4[i].file;
            tmpFile = D_800FA9C4[i + 1].file;
            if ((s16)tmpFile < leftFile) {
                D_800FA9C4[i].file = tmpFile;
                tmpActor = D_800FA9C4[i + 1].actor;
                D_800FA9C4[i + 1].file = leftFile;
                leftActor = D_800FA9C4[i].actor;
                D_800FA9C4[i].actor = tmpActor;
                D_800FA9C4[i + 1].actor = leftActor;
            }
        }
    }
}

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

void func_800C61C0(void) {
    s32 i;
    for (i = 0; i < NUM_PARTY; i++) {
        if (g_BattleData.actors[i].charId == 6) {
            D_800F4B1C.x = 0x10;
            D_800F4B1C.y = 0x1E0;
            D_800F4B1C.w = 0x10;
            D_800F4B1C.h = 3;
            BattleEnqueueLoadImage(&D_800F4B1C, D_800F8CF4[i]);
        }
    }
}

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
    switch (g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297A) {
    case 0:
        g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297A++;
        g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297C = 14;
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
        if (g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297C == 0) {
            D_800FAFDC = 1;
            g_BattleModels[0].specialFlags |= BATTLE_MODEL_INACTIVE;
            g_BattleModels[1].specialFlags |= BATTLE_MODEL_INACTIVE;
            g_BattleModels[2].specialFlags |= BATTLE_MODEL_INACTIVE;
            g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297C = 45;
            g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297A++;
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
                g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162978 = -1;
                return;
            }
            BattleLoadEffectModel();
            g_BattleModels[EFFECT_MODEL_SLOT].specialFlags |= BATTLE_MODEL_NO_SHADOW;
            g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162978 = -1;
        } else {
            g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297C--;
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
    g_BattleModels[EFFECT_MODEL_SLOT].ready = 0;
    BattleLoadEffectModel();
    g_BattleModels[EFFECT_MODEL_SLOT].specialFlags |= BATTLE_MODEL_NO_SHADOW;
}

void BattleFadeInAfterSummon(void) {
    s32 i;
    s16 timer;

    if (g_BattleDetachedSlots[g_BattleDetachedCursor].fade.state == 0) {
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
            g_BattleDetachedSlots[g_BattleDetachedCursor].fade.state = 1;
            g_BattleDetachedSlots[g_BattleDetachedCursor].fade.framesLeft = 14;
        }
    } else {
        timer = g_BattleDetachedSlots[g_BattleDetachedCursor].fade.framesLeft;
        if (timer == 0) {
            g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
            D_800FAFDC = 0;
            g_BattleModels[EFFECT_MODEL_SLOT].ready = 1;
        } else {
            g_BattleDetachedSlots[g_BattleDetachedCursor].fade.framesLeft = timer - 1;
        }
    }
}

void BattleFadeOutEffectModel(void) {
    if (g_BattleDetachedSlots[g_BattleDetachedCursor].fade.state == 0) {
        g_BattleModelFadeFrames = 14;
        g_BattleModels[EFFECT_MODEL_SLOT].animControlFlags |= ANIM_CTRL_FADE_OUT;
        g_BattleDetachedSlots[g_BattleDetachedCursor].fade.state = 1;
        g_BattleDetachedSlots[g_BattleDetachedCursor].fade.framesLeft = 14;
    } else if (g_BattleDetachedSlots[g_BattleDetachedCursor].fade.framesLeft == 0) {
        g_BattleModels[EFFECT_MODEL_SLOT].specialFlags |= BATTLE_MODEL_INACTIVE;
        g_BattleModels[EFFECT_MODEL_SLOT].specialFlags &= ~BATTLE_MODEL_NO_SHADOW;
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        g_BattleModels[EFFECT_MODEL_SLOT].ready = 1;
        return;
    } else {
        g_BattleDetachedSlots[g_BattleDetachedCursor].fade.framesLeft--;
    }
    BattleModelStartFades(EFFECT_MODEL_SLOT);
    func_800C74A4();
    if (!(D_80153BDD & BATTLE_MODEL_INACTIVE)) {
        func_800BA598(EFFECT_MODEL_SLOT);
    }
}

void BattleEffectModelTick(void) {
    if (g_BattleEffectModelState == EFFECT_MODEL_ENDING) {
        g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162978 = -1;
        if (g_BattleEffectModelNotSummon == 0) {
            BattleDetachedRegister(BattleFadeInAfterSummon);
        } else {
            BattleDetachedRegister(BattleFadeOutEffectModel);
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
    if (g_BattleEffectModelNotSummon && g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297A == 0) {
        g_BattleEffectSlots[g_BattleEffectCursor].raw.D_8016297A = 1;
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

void BattleGetModelBoneNumAndInitBones(s16* count, u16* data, s16 actor) {
    s32 offset;
    BattleModelSub* bones;
    BattleModel* boneModel;
    BattleModel* model;
    s32 modelOffset;
    s32 boneIndex;
    s32 updatedBoneIndex;
    s32 boneOffset;
    BattleModelSub* actorBones;
    model = &g_BattleModels[actor];
    model->boneTransforms[0].trans.vz = 0;
    model->boneTransforms[0].trans.vy = 0;
    g_BattleModels[actor].boneTransforms[0].trans.vx = 0;
    *count = *data + 1;
    *(s32*)0x1F800000 = 0;
    data += 2;
    while (*(s32*)0x1F800000 < *count) {
        g_BattleModels[actor].boneFlags[*(s32*)0x1F800000] = 0;
        offset = ((BattleModelBoneEntry*)data)[*(s32*)0x1F800000].modelOffset;
        if (offset != 0) {
            if (offset < 0) {
                g_BattleModels[actor].boneFlags[*(s32*)0x1F800000] = 1;
                g_BattleModels[actor].boneModels[*(s32*)0x1F800000] =
                    (s32*)((u8*)data +
                           ((((BattleModelBoneEntry*)data)[*(s32*)0x1F800000].modelOffset & 0x7FFFFFFF) - 4));
            } else {
                g_BattleModels[actor].boneModels[*(s32*)0x1F800000] = (s32*)((u8*)data + (offset - 4));
            }
        } else {
            g_BattleModels[actor].boneModels[*(s32*)0x1F800000] = NULL;
        }
        boneIndex = *(s32*)0x1F800000;
        if (boneIndex != 0) {
            modelOffset = actor * sizeof(BattleModel);
            bones = g_BattleModels[0].boneTransforms;
            actorBones = (BattleModelSub*)((u8*)bones + modelOffset);
            BattleBoneSetParentMatrix(
                &actorBones[((BattleModelBoneEntry*)data)[boneIndex].parent].m, &actorBones[boneIndex]);
            updatedBoneIndex = *(s32*)0x1F800000;
            bones = (BattleModelSub*)((u8*)bones - ((u8*)g_BattleModels[0].boneTransforms - (u8*)g_BattleModels));
            boneOffset = updatedBoneIndex * sizeof(BattleModelSub) + modelOffset;
            boneModel = (BattleModel*)(boneOffset + (u8*)bones);
            boneModel->boneTransforms[0].trans.vy = 0;
            ((BattleModel*)((u8*)g_BattleModels + boneOffset))->boneTransforms[0].trans.vx = 0;
            ((BattleModel*)((u8*)g_BattleModels + boneOffset))->boneTransforms[0].trans.vz =
                ((BattleModelBoneEntry*)data)[((BattleModelBoneEntry*)data)[updatedBoneIndex].parent].z;
        }
        (*(s32*)0x1F800000)++;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", BattleGetWeaponBoneNumAndInitBones);
