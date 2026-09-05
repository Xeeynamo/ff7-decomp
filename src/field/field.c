//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>

typedef struct FieldRenderData {
    OT_TYPE ot[0x1000];   // 0x00000: Main scene ordering table
    SPRT_16 Arrows[0x18]; // 0x04000: Field arrow sprite packets
    DR_MODE ArrowsDm;     // 0x04180: Arrow sprite draw mode

    OT_TYPE OtFadeDrenv;  // 0x0418c: Fade draw environment OT entry
    OT_TYPE OtSceneDrenv; // 0x04190: Scene draw environment OT entry

    DR_ENV FadeDrenv;  // 0x04194: Screen fade draw environment
    DR_ENV SceneDrenv; // 0x041d4: Main scene draw environment

    DR_ENV BgDrenv3S; // 0x04214: Background layer 3 start env
    DR_ENV BgDrenv4S; // 0x04254: Background layer 4 start env
    DR_ENV BgDrenv3E; // 0x04294: Background layer 3 end env
    DR_ENV BgDrenv4E; // 0x042d4: Background layer 4 end env

    u8 unk4314[0x600]; // 0x04314: Unknown render data

    SPRT_16 Bg1[0x9c4]; // 0x04914: Background layer 1/2 sprites
    SPRT Bg2[0x200];    // 0x0e554: Background layer 3/4 sprites

    u16 BgAnim[0xbc4];   // 0x10d54: Background animation data
    DR_MODE BgDm[0x6a4]; // 0x124dc: Background draw mode packets

    OT_TYPE OtUi;       // 0x1748c: UI ordering table
    DR_MODE RainDm;     // 0x17490: Rain draw mode
    LINE_F2 Rain[0x40]; // 0x1749c: Rain line primitives
};

extern struct FieldRenderData g_FieldRenderData[2]; // double buffered

const u32 D_800A0000[] = {0, 0x01D801E0};
extern s32 (*g_FieldOpcodes[256])(void);
extern u8 g_EntityForSplitJoin;
extern s16 D_800DF120[][2];
extern char g_DebugMessageBuffer[]; // debug value transformed into text

extern u8 D_80114498[];
extern u32 g_FieldKeyState;

void AddBackgroundToRender(struct FieldRenderData* buf);
s32 FieldEntitySqrDistToLine(FieldLine*, u_long*, u_long*);
void FieldEntityLineInteract(FieldEntity* arg0, FieldLine* arg1);
void HandleKawaiDataInModel(struct FieldRenderData* buf);
void FieldEventOpcodeCycle(void);
void FieldUpdateAnimationState(void);
u8 FieldEventRequestRun(s16 entityId, s16 priority, s16 scriptId);
void DebugUpdateActor(s32 arg0, u8 actorId);
void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind);
void FieldWindowResetTextAll(void);
void SetStrToDebugRow(s32 page, s16 row, const char* str);
void FieldDebugStringCopy(char* dst, const char* src);
void FieldDebugStringConcat(char* arg0, char* arg1);

/////////////////////////////////////////////////
// Begin of field_main.c
/////////////////////////////////////////////////

typedef struct {
    u32 datSector; // +0x00
    u32 datSize;   // +0x04
    u32 mimSector; // +0x08
    u32 mimSize;   // +0x0C
    u32 bsxSector; // +0x10
    u32 bsxSize;   // +0x14
} FieldFileInfo;

extern FieldFileInfo g_FieldFileInfo[];
extern void SystemLzsDecompress(void* dst, void* src);
extern s32* g_FieldModelsP;
extern s32 g_FieldTriggers;
extern s32 g_FieldEncounters;
extern s32 D_8007E770;
extern s16 g_CurrentFieldIndex;
extern s32* g_FieldTriggersP;
extern s32* g_FieldEncountersP;
extern u32 g_FieldLzsInfo[];

void FieldLoadMimDatFiles(void) {
    s32 temp;

    if (g_isFieldLoading == 0) {
        SysCdromStartLoadLzs(g_FieldLzsInfo[g_CurrentFieldIndex * 6], g_FieldLzsInfo[g_CurrentFieldIndex * 6 + 1],
                             (u32*)0x80128000, NULL);
        while (SystemCdromReadChain() != 0) {
        }
    } else {
        while (SystemCdromReadChain() != 0) {
        }
        SystemLzsDecompress((void*)0x801B0000, (void*)0x80128000);
    }
    SysCdromStartLoadLzs(((u32*)g_FieldFileInfo)[g_CurrentFieldIndex * 6],
                         ((u32*)g_FieldFileInfo)[g_CurrentFieldIndex * 6 + 1], (u32*)0x80114FE4, NULL);
    while (SystemCdromReadChain() != 0) {
    }
    g_FieldTriggers = *g_FieldTriggersP;
    g_FieldEncounters = *g_FieldEncountersP;
    temp = *g_FieldModelsP;
    D_8007E770 = temp;
    g_FieldModelLoaderData = temp + 4;
}

void StopFieldMapPreload(void) {
    if (g_isFieldLoading == 1) {
        SystemCdromAbortLoading();
    }
    D_80071A5C = 0; // needs to be called g_preloadedFieldMapId;
    g_isFieldLoading = 0;
}

extern FieldFileInfo g_FieldFileTable[];
extern u16 g_FieldMoviePlayed;
extern u16 g_FieldPreloadMapId;
extern s32 g_WmPreSector;
extern u32 g_WmPreSize;

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", PreloadNextFieldMap);
#else

// External Declarations
extern u8 D_8009ABF5;
extern u8 D_8009AC26;
extern s16 D_80071A5C;

// D_8009ABF5 = g_pFieldState -> command

void PreloadNextFieldMap(FieldEntity* Player, FieldLine* gateway) {
    s16* ptr_a3;
    s32* scratchpad;
    s32 min_dist;
    s32 counter;
    s16* ptr_a1;
    s32 term_val;
    s32 diff_x, diff_y, dist;
    s16 map_id;
    FieldFileInfo* table;
    s32 sector;
    u32 size;

    ptr_a3 = gateway;
    min_dist = 0x7FFFFFFF;

    scratchpad = 0x1F800000;
    scratchpad[0] = Player->PosX >> 12;
    scratchpad[1] = Player->PosY >> 12;
    scratchpad[2] = Player->PosZ >> 12;

    if (D_8009AC26 == 0) {
        counter = 0;
        term_val = 0x7FFF;
        ptr_a1 = (gateway + 0x12);

        do {
            map_id = ptr_a1[0];
            if (map_id != term_val) {
                diff_x = ptr_a3[0] - scratchpad[0];
                diff_y = ptr_a1[-8] - scratchpad[1];
                dist = (diff_x * diff_x) + (diff_y * diff_y);

                if (dist < min_dist) {
                    min_dist = dist;
                    g_FieldPreloadMapId = map_id;
                }
            }

            counter++;
            ptr_a1 = (ptr_a1 + 0x18);
            ptr_a3 = (ptr_a3 + 0x18);
        } while (counter < 12);
    }

    if (D_8009ABF5 == 3 || (g_FieldMoviePlayed == 1) || D_8009ABF5 == 2) {
        StopFieldMapPreload();
        return;
    }

    if (D_80071A5C == g_FieldPreloadMapId) {
        return;
    }

    table = g_FieldFileTable;
    if (0x4DFFF < table[g_FieldPreloadMapId].datSize) {
        return;
    }

    StopFieldMapPreload();
    D_80071A5C = g_FieldPreloadMapId;

    if (D_80071A5C >= 0x41) {
        sector = table[D_80071A5C].datSector;
        size = table[D_80071A5C].datSize;
    } else {
        sector = g_WmPreSector;
        size = g_WmPreSize;
    }

    SystemLoadFileBySector(sector, size, 0x801B0000, NULL);
    g_isFieldLoading = 1;
}

#endif

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldMain);

const u32 D_800A0024[] = {0x00000000, 0x000801E0};
const u32 D_800A002C[] = {0x00E80000, 0x000801E0};
const u32 D_800A0034[] = {0x01D00000, 0x000801E0};
const u32 D_800A003C[] = {0x00000000, 0x00080140};
const u32 D_800A0044[] = {0x00E80000, 0x00080140};
const u32 D_800A004C[] = {0x01D00000, 0x00080140};
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldMainLoop);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldLoadMimToVram);

u32 FieldButtonsUpdate(void) {
    g_FieldKeyState = InputReadPadsRaw();
    g_FieldState.activeKeysPrevRaw = g_FieldState.activeKeysRaw;
    g_FieldState.activeKeysRaw = g_FieldKeyState;
    g_FieldState.pressedKeysRaw = (g_FieldKeyState ^ g_FieldState.activeKeysPrevRaw) & g_FieldKeyState;
    g_FieldState.releasedKeysRaw = (g_FieldKeyState ^ g_FieldState.activeKeysPrevRaw) & ~g_FieldKeyState;

    g_FieldKeyState = InputReadPads();
    g_FieldState.activeKeysPrev = g_FieldState.activeKeys;
    g_FieldState.activeKeys = g_FieldKeyState;
    g_FieldState.pressedKeys = (g_FieldKeyState ^ g_FieldState.activeKeysPrev) & g_FieldKeyState;
    g_FieldState.releasedKeys = (g_FieldKeyState ^ g_FieldState.activeKeysPrev) & ~g_FieldKeyState;

    return g_FieldKeyState;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBackgroundInitPackets);

INCLUDE_ASM("asm/us/field/nonmatchings/field", AddBackgroundToRender);

s32 FieldCalcLinearStep(s32 start, s32 target, s32 duration, s32 step) {
    s32 delta = target - start;

    if ((u32)(delta + 0x7FFFF) <= 0xFFFFE) {
        start += (delta * step) / duration;
    } else {
        start += (delta / duration) * step;
    }

    return start;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldCalcEaseInOut);

static s32 FieldCalcWorldToScreenPos(SVECTOR* worldPos, long* screenPos) {
    long flag;
    long depth;
    s32 ret;

    PushMatrix();
    SetRotMatrix(D_80071E40);
    SetTransMatrix(D_80071E40);
    SetGeomOffset(0, 0);
    ret = RotTransPers(worldPos, screenPos, &flag, &depth);
    PopMatrix();
    return ret;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGShakeUpdate);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGScrollInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldCalcPointOnLine);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGClampPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGGetEntityScreenPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGScrollUpdate);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGUpdateDrawenv);

/////////////////////////////////////////////////
// Begin of field_entity.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityInitPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityAddRotate);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityAnimationUpdate);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityMovementUpdate);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityGatewayMapLoad);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityCheckTalk);

s16 FieldEntityGetDirVectorX(u8 arg0) { return D_800DF120[arg0][0]; }

s16 FieldEntityGetDirVectorY(u8 arg0) { return D_800DF120[arg0][1]; }

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityDirByVec);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityAutoMove);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityWalkmechCross);

static void FieldEntityVectorSub(s32* arg0, s16* arg1, s16* arg2) {
    arg0[0] = arg1[0] - arg2[0];
    arg0[1] = arg1[1] - arg2[1];
    arg0[2] = arg1[2] - arg2[2];
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityCalculateZ);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityMove);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityCollisionCheck);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntitySqrDistToLine);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityLineCheck);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityLineInteract);

static void FieldEntityLineClear(FieldLine* lines) {
    s32 i;

    for (i = 0; i < LEN(g_FieldLines); i++) {
        lines->isOnLine = 0;
        lines++;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityGatewayCheck);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityBgTriggerActivate);

const u32 D_800A00BC[] = {0x00360000, 0x012A007A};
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityTriggerCheck);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEntityBgTriggerInit);

/////////////////////////////////////////////////
// Begin of field_camera.c
/////////////////////////////////////////////////

const u32 D_800A00DC[] = {0x00000000};
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelLoadAndInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field", HandleKawaiDataInModel);

// Possable Debug routine. Ran at beginning of every main field loop. (FPS?)
void DebugRunEveryLoop(void) {}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldCameraAssign);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldUpdateMovieStream);

/////////////////////////////////////////////////
// Begin of field_rain.c
/////////////////////////////////////////////////

struct FieldRain {
    /* 0x00 */ SVECTOR p1;
    /* 0x08 */ SVECTOR p2;
    /* 0x10 */ s16 wait;
    /* 0x12 */ s16 rndSeed;
    /* 0x14 */ s16 z;
    /* 0x16 */ s16 render;
};

extern struct FieldRain g_FieldRain[64];
extern u8 g_RainForce;
extern s16 D_800E42EE[0x40][12];

void FieldRainInit(struct FieldRenderData* renderData) {
    LINE_F2* line;
    s32 i;
    s32 adjustedIndex;

    for (i = 0; i < LEN(g_FieldRain); i++) {
        g_FieldRain[i].render = 0;
        g_FieldRain[i].rndSeed = i * 4;
        g_FieldRain[i].wait = i % 8;

        line = &renderData->Rain[i];

        SetLineF2(line);
        SetSemiTrans(line, 1);

        renderData->Rain[i].r0 = 0x10;
        renderData->Rain[i].g0 = 0x10;
        renderData->Rain[i].b0 = 0x10;
    }

    SetDrawMode(&renderData->RainDm, 0, 0, GetTPage(0, 1, 0, 0) & 0xffff, NULL);
}

void FieldRainAddToRender(u32* ot, LINE_F2* rain, MATRIX* matrix, DR_MODE* rainDm) {
    long p;
    long flag;
    s32 i;
    s32 j;

    PushMatrix();
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);

    for (i = 0, j = 0; i < LEN(g_FieldRain); i++) {
        // 12 * sizeof(s16) = 24 bytes (0x18), the exact size of FieldRain
        if (D_800E42EE[i][0] == 1) {
            RotTransPers(&g_FieldRain[i].p1, &rain->x0, &p, &flag);
            RotTransPers(&g_FieldRain[i].p2, &rain->x1, &p, &flag);
            AddPrim(ot, rain);
        }
        rain++;
    }

    PopMatrix();

    *(u32*)rainDm = (*(u32*)rainDm & 0xFF000000) | (*ot & 0xFFFFFF);

    *ot = (*ot & 0xFF000000) | ((u32)rainDm & 0xFFFFFF);
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldRainUpdate);
#else

extern u8 g_RainControl;
extern s16 g_PlayerModelId;

extern FieldEntity g_FieldEntities[];
extern u8 g_RandomTable[];
extern struct FieldRain g_FieldRain[];

void FieldRainUpdate(void) {
    s32 i;
    s32 limit;
    s32 player;
    s32 max = 255;
    s32 vz;

    if ((g_RainControl & 0x80) == 0) {
        if (g_RainForce != 0) {
            g_RainForce--;
        }
    } else {
        if (g_RainForce != max) {
            g_RainForce++;
        }
    }

    limit = g_RainForce / 4;
    player = g_PlayerModelId;

    for (i = 0; i < 0x40; i++) {
        if (g_FieldRain[i].wait == 0) {
            if (i < limit) {

                u8 seed3;

                g_FieldRain[i].render = 1;
                g_FieldRain[i].rndSeed++;
                g_FieldRain[i].wait = 7;

                g_FieldRain[i].p2.vx =
                    (g_FieldEntities[player].PosX >> 12) + g_RandomTable[g_FieldRain[i].rndSeed & 0xFF] * 12 - 0x600;

                seed3 = g_FieldRain[i].rndSeed * 3;
                g_FieldRain[i].p2.vy = (g_FieldEntities[player].PosY >> 12) + g_RandomTable[seed3] * 12 - 0x600;

                g_FieldRain[i].p1.vx = g_FieldRain[i].p2.vx;
                g_FieldRain[i].p1.vy = g_FieldRain[i].p2.vy;

                g_FieldRain[i].z = (g_FieldEntities[player].PosZ >> 12) - 0x300;
            } else {
                g_FieldRain[i].wait = 1;
                g_FieldRain[i].render = 0;
            }
        }

        g_FieldRain[i].p2.vz = g_FieldRain[i].z + (g_FieldRain[i].wait & 0x7) * 0x80;

        vz = (g_FieldRain[i].wait & 0x7) * 0x80;
        vz += 0x100;

        g_FieldRain[i].p1.vz = g_FieldRain[i].z + vz;

        g_FieldRain[i].wait--;
    }
}
#endif

/////////////////////////////////////////////////
// Begin of field_battle.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldGetRandomU8FromList);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldGetNextRandomU8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBattleCheck);

/////////////////////////////////////////////////
// Begin of field_arrow.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldArrowsInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldArrowsAddToRender);

/////////////////////////////////////////////////
// Begin of field_model.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field", LoadLocalFieldModelAndInitAll);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelCreatePktsAndScale);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelCreatePktsForPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelLoadBsxTexToVram);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelBsxTdbModify);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelStructInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelLoadGlobalModels);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelLoadBcx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelPrepareRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelAddToRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelAnimCalcMtrxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelScaleModel);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelScalePartVrtxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldModelScaleAnimTranslat);

/////////////////////////////////////////////////
// Begin of field_kawai_char_model.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiClearData);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiExecute);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetCustomLightToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetVertexColorFromLighting);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetColorToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetColorToPartPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiLoadEyesMouthTexToVram);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiLightingApplyToModel);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiLightingApplyToPolyColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetModelTransparency);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetColorToPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetColorToPartPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiFadeModelColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetCustomLighting);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiColorFadeBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetLightingToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetLightingToPartPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetSplashToPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiInitSplashPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiSetPartAttribute);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiApplyBoneTransform);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiRenderClippedPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiDirectionalColorGradient);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiGradientColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field", KawaiAnimatedPointLight);
