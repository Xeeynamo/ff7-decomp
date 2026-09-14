//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern struct FieldRenderData g_FieldRenderData[2]; // double buffered

extern s32 (*g_FieldOpcodes[256])(void);
extern u8 g_EntityForSplitJoin;
extern char g_DebugMessageBuffer[]; // debug value transformed into text
extern u32 g_FieldKeyState;
extern u8 g_RandomTable[256];

extern s16 g_CameraScrollX;
extern s16 g_CameraScrollY;
extern s16 g_CameraScrollCurrentStep;
extern s16 g_CameraScrollStartX;
extern s16 g_CameraScrollTargetX;
extern s16 g_CameraScrollStartY;
extern s16 g_CameraScrollTargetY;
extern s16 g_CameraScrollEnabled;
extern s16 g_CameraScrollNumSteps;

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
void FieldDebugStringConcat(char* arg0, const char* arg1);

/////////////////////////////////////////////////
// Begin of field_main.c
/////////////////////////////////////////////////

extern u32 g_FieldFileInfo[];
extern FieldModelLoaderHeader** g_FieldModelsP;
extern FieldTriggers* g_FieldTriggers;
extern u16* g_FieldEncounters;
extern s16 g_CurrentFieldIndex;
extern FieldTriggers** g_FieldTriggersP;
extern u16** g_FieldEncountersP;

static void FieldLoadMimDatFiles(void) {
    if (g_isFieldLoading == 0) {
        SysCdromStartLoadLzs(g_FieldFileInfo[g_CurrentFieldIndex * 6 + 2], g_FieldFileInfo[g_CurrentFieldIndex * 6 + 3],
                             (u_long*)0x80128000, NULL);
        while (SystemCdromReadChain() != 0) {
        }
    } else {
        while (SystemCdromReadChain() != 0) {
        }
        SystemLzsDecompress((u8*)0x801B0000, (u8*)0x80128000);
    }
    SysCdromStartLoadLzs(g_FieldFileInfo[g_CurrentFieldIndex * 6 + 0], g_FieldFileInfo[g_CurrentFieldIndex * 6 + 1],
                         (u_long*)0x80114FE4, NULL);
    while (SystemCdromReadChain() != 0) {
    }
    g_FieldTriggers = *g_FieldTriggersP;
    g_FieldEncounters = *g_FieldEncountersP;
    D_8007E770 = *g_FieldModelsP;
    g_FieldModelLoaderData = (FieldModelLoaderData*)(D_8007E770 + 1);
}

void StopFieldMapPreload(void) {
    if (g_isFieldLoading == 1) {
        SystemCdromAbortLoading();
    }
    D_80071A5C = 0; // needs to be called g_preloadedFieldMapId;
    g_isFieldLoading = 0;
}

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

    table = g_FieldFileInfo;
    if (0x4DFFF < table[g_FieldPreloadMapId].mimSize) {
        return;
    }

    StopFieldMapPreload();
    D_80071A5C = g_FieldPreloadMapId;

    if (D_80071A5C >= 0x41) {
        sector = table[D_80071A5C].mimSector;
        size = table[D_80071A5C].mimSize;
    } else {
        sector = g_WmPreSector;
        size = g_WmPreSize;
    }

    SystemLoadFileBySector(sector, size, 0x801B0000, NULL);
    g_isFieldLoading = 1;
}

#endif

void FieldArrowsInit(SPRT_16* arrows, DR_MODE* drawMode);
void FieldEnablePartyModels(void);
void FieldEntityBgTriggerInit(FieldBgTrigger* triggers);
void FieldEventInit(FieldState* fieldState, FieldEntity* fieldModels, FieldScriptHeader* fieldScripts);
void FieldLoadMimToVram(s32 arg0, void* mimData);
s32 FieldMainLoop(void);
void SysFadeCopyScreen(void);
void SysFadeInitPoly(void);

extern u8 D_800716D0;
extern s16 D_8007173C;
extern s8 D_80071A58;
extern FieldScriptHeader** D_8007EB64;
extern u8 D_8007EBC8;
extern MATRIX** D_80083578;
extern s32** D_8009A044;
extern s8 D_8009A048[16];
extern volatile s16 g_GameState;
extern s8 D_8009C6D8;
extern u8** D_8009D848;
extern DRAWENV D_80113F2C[2];
extern DRAWENV D_80113FE4[2];
extern DRAWENV D_8011409C[2];
extern DRAWENV D_80114154[2];
extern DRAWENV D_8011420C[2];
extern u8 g_RainControl;
extern u8 g_RainForce;

#define NON_MATCHINGS
#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldMain);
#else
void FieldMain(void) {
    RECT clearRect = {0, 0, 480, 472};
    s32 i;

    ClearOTagR(&g_FieldRenderData[0].OtFadeDrenv, 1);
    ClearOTagR(&g_FieldRenderData[1].OtFadeDrenv, 1);
    SetDrawEnv(&g_FieldRenderData[0].FadeDrenv, &D_8007EAAC[0]);
    SetDrawEnv(&g_FieldRenderData[1].FadeDrenv, &D_8007EAAC[1]);
    addPrim(&g_FieldRenderData[0].OtFadeDrenv, &g_FieldRenderData[0].FadeDrenv);
    addPrim(&g_FieldRenderData[1].OtFadeDrenv, &g_FieldRenderData[1].FadeDrenv);
    SetDefDrawEnv(&D_80113F2C[0], 0, 8, 320, 224);
    SetDefDrawEnv(&D_80113F2C[1], 0, 240, 320, 224);
    SetDefDrawEnv(&D_80114154[0], 0, 8, 320, 224);
    SetDefDrawEnv(&D_80114154[1], 0, 240, 320, 224);
    SetDefDrawEnv(&D_8011420C[0], 0, 8, 320, 224);
    SetDefDrawEnv(&D_8011420C[1], 0, 240, 320, 224);
    D_80113F2C[0].dtd = 1;
    D_80113F2C[1].dtd = 1;
    D_80114154[0].dtd = 1;
    D_80114154[1].dtd = 1;
    D_8011420C[0].dtd = 1;
    D_8011420C[1].dtd = 1;
    D_80113F2C[0].isbg = 0;
    D_80113F2C[1].isbg = 0;
    D_80114154[0].isbg = 0;
    D_80114154[1].isbg = 0;
    D_8011420C[0].isbg = 0;
    D_8011420C[1].isbg = 0;
    ClearOTagR(&g_FieldRenderData[0].OtSceneDrenv, 1);
    ClearOTagR(&g_FieldRenderData[1].OtSceneDrenv, 1);
    SetDrawEnv(&g_FieldRenderData[0].SceneDrenv, &D_80113F2C[0]);
    SetDrawEnv(&g_FieldRenderData[1].SceneDrenv, &D_80113F2C[1]);
    addPrim(&g_FieldRenderData[0].OtSceneDrenv, &g_FieldRenderData[0].SceneDrenv);
    addPrim(&g_FieldRenderData[1].OtSceneDrenv, &g_FieldRenderData[1].SceneDrenv);
    SetDefDrawEnv(&D_80113FE4[0], 0, 8, 320, 224);
    SetDefDrawEnv(&D_80113FE4[1], 0, 240, 320, 224);
    D_80113FE4[0].isbg = 0;
    D_80113FE4[1].isbg = 0;
    D_80113FE4[0].dtd = 1;
    D_80113FE4[1].dtd = 1;
    SetDefDrawEnv(&D_8011409C[0], 0, 8, 320, 224);
    SetDefDrawEnv(&D_8011409C[1], 0, 240, 320, 224);
    D_8011409C[0].isbg = 0;
    D_8011409C[1].isbg = 0;
    D_8011409C[0].dtd = 1;
    D_8011409C[1].dtd = 1;
    SysFadeInitPoly();
    g_FieldState.fadeType = FFT_INSTANT;
    if ((g_PrevGameState != GAMESTATE_FIELD) && (g_PrevGameState != GAMESTATE_BATTLE) &&
        (g_PrevGameState != GAMESTATE_WORLD) && (g_PrevGameState != GAMESTATE_MENU) &&
        (g_PrevGameState != GAMESTATE_MENU_COMMANND)) {
        ClearImage(&clearRect, 0, 0, 0);
    }
    for (;;) {
        DebugRunEveryLoop();
        D_80071A5C = 0;
        g_FieldPreloadMapId = 0;
        if (((g_PrevGameState == GAMESTATE_FIELD) || (g_PrevGameState == GAMESTATE_WORLD)) &&
            (g_FieldState.fadeType == FFT_INSTANT)) {
            SysFadeCopyScreen();
            g_FieldState.fadeType = FFT_SYS_FADE_TO_BLACK_FIELD_CHANGE;
            D_80071A58 = 3;
            g_FieldState.fadeAdjust = 0;
            D_8007E768 = 0;
            D_80095DD4 = 1;
        }
        if ((g_PrevGameState != GAMESTATE_MENU) && (g_PrevGameState != GAMESTATE_MENU_COMMANND)) {
            D_8007EB64 = (FieldScriptHeader**)0x80114FE4;
            D_8009A044 = (s32**)0x80114FE8;
            D_8009D848 = (u8**)0x80114FEC;
            D_80083578 = (MATRIX**)0x80114FF0;
            g_FieldTriggersP = (FieldTriggers**)0x80114FF4;
            g_FieldEncountersP = (u16**)0x80114FF8;
            g_FieldModelsP = (FieldModelLoaderHeader**)0x80114FFC;
            FieldLoadMimDatFiles();
        }
        if (g_PrevGameState == GAMESTATE_BATTLE) {
            D_8007EBE0 = 1;
            if (D_8007EBC8 == 1) {
                D_8007EBC8 = 0;
                D_8009C6D8 = 0;
                D_8007173C = 0;
                g_FieldState.eventCmd = EVTCMD_NONE;
            }
        }
        while (D_80095DD4) {
        }
        while (DrawSync(1)) {
        }
        if (g_PrevGameState != GAMESTATE_MENU_COMMANND) {
            g_FieldState.fadeType = FFT_INV4_TO_FIELD_SUB;
            g_FieldState.fadeSpeed = 16;
            g_FieldState.fadeAdjust = 256;
            g_FieldState.fadeRed = 0;
            g_FieldState.fadeGreen = 0;
            g_FieldState.fadeBlue = 0;
        }
        if ((g_PrevGameState == 0) || (g_PrevGameState == GAMESTATE_FIELD) || (g_PrevGameState == GAMESTATE_WORLD) ||
            (g_PrevGameState == GAMESTATE_HIGHWAY) || (g_PrevGameState == GAMESTATE_SNOWBOARD1) ||
            (g_PrevGameState == GAMESTATE_CHOCOBO) || (g_PrevGameState == GAMESTATE_FORTCONDOR) ||
            (g_PrevGameState == GAMESTATE_JET) || (g_PrevGameState == GAMESTATE_SUBMARIME)) {
            g_FieldState.layer2_bgScrollXSpeed = 0;
            g_FieldState.layer2_bgScrollYSpeed = 0;
            g_FieldState.layer3_bgScrollXSpeed = 0;
            g_FieldState.layer3_bgScrollYSpeed = 0;
            g_FieldState.layer3_depth = 1;
            g_FieldState.layer2_depth = 4095;
            g_CameraScrollEnabled = 0;
            g_CameraScrollX = 0;
            g_CameraScrollY = 0;
            g_FieldState.viewOffset = g_FieldTriggers->viewOffset;
            FieldEventInit(&g_FieldState, g_FieldEntity, *D_8007EB64);
            g_FieldEntity[g_FieldState.pcModelId].Dir = g_FieldState.pcDirection;
            if (!(g_RainControl & 0x80)) {
                g_RainForce = 0;
            } else {
                g_RainForce = 0xFF;
            }
            for (i = 0; i < 16; i++) {
                D_8009A048[i] = -1;
            }
            FieldEntityBgTriggerInit(g_FieldTriggers->triggers);
        } else {
            g_FieldState.movieCommandState = MOVCMD_DONE;
        }
        FieldEnablePartyModels();
        FieldEntityLineClear(g_FieldLines);
        D_800716D0 = 0;
        FieldArrowsInit(g_FieldRenderData[0].Arrows, &g_FieldRenderData[0].ArrowsDm);
        FieldArrowsInit(g_FieldRenderData[1].Arrows, &g_FieldRenderData[1].ArrowsDm);
        if ((g_PrevGameState != GAMESTATE_MENU) && (g_PrevGameState != GAMESTATE_MENU_COMMANND)) {
            FieldLoadMimToVram(0, (void*)0x80128000);
        }
        if (g_PrevGameState == GAMESTATE_BATTLE) {
            *D_8009A000 = 0xF5;
            SystemAkaoExecute();
            *D_8009A000 = 0x18;
            *D_8009A008 = 4;
            *D_8009A004 = g_FieldState.nextFieldMusic;
            SystemAkaoExecute();
        }
        FieldMainLoop();
        while (DrawSync(1)) {
        }
        VSync(1);
        D_8007EB68[0].isrgb24 = 0;
        D_8007EB68[1].isrgb24 = 0;
        PutDispEnv(&D_8007EB68[D_80075DEC]);
        PutDrawEnv(&D_8007EAAC[D_80075DEC]);
        g_PrevGameState = GAMESTATE_FIELD;
        if ((g_FieldState.eventCmd == EVTCMD_BEAT_GAME) || (g_FieldState.eventCmd == EVTCMD_GAME_OVER) ||
            (g_FieldState.eventCmd == EVTCMD_PLAY_ENDING_FMV)) {
            break;
        }
        if (g_FieldState.eventCmd == EVTCMD_FIELD_MAP_CHANGE) {
            g_FieldState.prevFieldId = g_CurrentFieldIndex;
            g_CurrentFieldIndex = g_FieldState.eventCmdParam;
            if (g_CurrentFieldIndex != D_80071A5C) {
                StopFieldMapPreload();
            }
            if ((u32)((u16)g_CurrentFieldIndex - 1) < 64U) {
                g_GameState = GAMESTATE_WORLD;
                SysFadeCopyScreen();
                g_FieldState.fadeType = FFT_SYS_FADE_TO_BLACK_FIELD_CHANGE;
                D_80071A58 = 3;
                g_FieldState.fadeAdjust = 0;
                D_8007E768 = 0;
                D_80095DD4 = 1;
                break;
            }
        }
        if (g_FieldState.eventCmd == EVTCMD_LOAD_MINIGAME) {
            g_FieldState.prevFieldId = g_CurrentFieldIndex;
            g_CurrentFieldIndex = g_FieldState.eventCmdParam;
            switch (g_FieldState.backgroundLayerVisibility[0]) {
            case 0:
                g_GameState = GAMESTATE_HIGHWAY;
                break;
            case 1:
                g_GameState = GAMESTATE_CHOCOBO;
                break;
            case 2:
                g_GameState = GAMESTATE_SNOWBOARD1;
                break;
            case 3:
                g_GameState = GAMESTATE_FORTCONDOR;
                break;
            case 4:
                g_GameState = GAMESTATE_SUBMARIME;
                break;
            case 5:
                g_GameState = GAMESTATE_JET;
                break;
            case 6:
                g_GameState = GAMESTATE_SNOWBOARD2;
                break;
            }
            break;
        }
        if ((g_FieldState.eventCmd == EVTCMD_ENTERING_BATTLE) || (g_FieldState.eventCmd == EVTCMD_CD_CHANGE)) {
            break;
        }
        if (g_GameState == GAMESTATE_MENU) {
            SysFadeCopyScreen();
            g_FieldState.fadeType = FFT_SYS_FADE_TO_BLACK_MENU;
            D_80071A58 = 13;
            g_FieldState.fadeAdjust = 0;
            D_8007E768 = 0;
            D_80095DD4 = 1;
            break;
        }
        if (g_GameState == GAMESTATE_MENU_COMMANND || g_GameState == GAMESTATE_LOAD_INSTR2) {
            break;
        }
    }
    VSync(0);
}
#endif

void FieldModelLoadAndInit(void);
void FieldEntityInitPos(void);
void FieldBackgroundInitPackets(SPRT_16* bg1, SPRT* bg2, u16* animation, DR_MODE* drawMode);
void FieldCameraAssign(void);
void FieldEventUpdate(OT_TYPE* ot);
void FieldBGScrollInit(void);
void FieldBGScrollUpdate(void);
void FieldBGShakeUpdate(FieldShakeData* data);
void FieldBGUpdateDrawenv(struct FieldRenderData* renderData);
void PreloadNextFieldMap(FieldEntity* player, FieldGateway* gateways);
void FieldEntityMovementUpdate(u32 keys);
void FieldEntityCheckTalk(void);
void FieldRainUpdate(void);
void FieldArrowsAddToRender(struct FieldRenderData* renderData, MATRIX* matrix, FieldGateway* gateways);
void SysFadeUpdate(void);
void SysMovieAbortPlay(void);
void FieldUpdateMovieStream(void);
static u32 FieldButtonsUpdate(s16* scrollX, s16* scrollY);

extern s8 D_80071C0C;
extern OT_TYPE D_8007E7A0[2];
extern s32 D_8007EB90;
extern s32 D_8007EB94;
extern DRAWENV* D_8007EBD0;
extern DISPENV* D_8007EBD8;
extern s32 D_800965E4;
extern s32 D_8009A060;
extern SVECTOR (*D_800E4274)[3];
extern s16 D_801142C8;
extern s16 (*D_80114458)[3];
extern s32 D_80114478;
extern s32 D_8011447C;
extern u16 D_80114488;

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldMainLoop);
#else
s32 FieldMainLoop(void) {
    RECT wideTop = {0, 0, 480, 8};
    RECT wideMiddle = {0, 232, 480, 8};
    RECT wideBottom = {0, 464, 480, 8};
    RECT top = {0, 0, 320, 8};
    RECT middle = {0, 232, 320, 8};
    RECT bottom = {0, 464, 320, 8};
    s16 displayDelay;
    struct FieldRenderData* renderData;

    D_8007EB90 = 160;
    D_8007EB94 = 120;
    if (g_PrevGameState != GAMESTATE_MENU && g_PrevGameState != GAMESTATE_MENU_COMMANND) {
        FieldModelLoadAndInit();
    }
    D_800E4274 = (SVECTOR (*)[3])(*D_8009A044 + 1);
    D_80114458 = (s16 (*)[3])(D_800E4274 + *(u16*)*D_8009A044);
    if (g_PrevGameState != GAMESTATE_MENU && g_PrevGameState != GAMESTATE_BATTLE &&
        g_PrevGameState != GAMESTATE_MENU_COMMANND) {
        FieldEntityInitPos();
    }
    FieldBackgroundInitPackets(g_FieldRenderData[0].Bg1, g_FieldRenderData[0].Bg2,
                               g_FieldRenderData[0].BgAnim, g_FieldRenderData[0].BgDm);
    FieldBackgroundInitPackets(g_FieldRenderData[1].Bg1, g_FieldRenderData[1].Bg2,
                               g_FieldRenderData[1].BgAnim, g_FieldRenderData[1].BgDm);
    FieldRainInit(&g_FieldRenderData[0]);
    FieldRainInit(&g_FieldRenderData[1]);
    displayDelay = 1;
    D_80114488 = 0;
    D_801142C8 = 0;
    g_FieldMoviePlayed = 0;
    D_80071C0C = 0;
    g_isFieldLoading = 0;

    for (;;) {
        if (!displayDelay) {
            D_80075DEC++;
        }
        D_80075DEC &= 1;
        g_FieldState.renderBuffer = D_80075DEC;
        renderData = &g_FieldRenderData[D_80075DEC];
        ClearOTagR(renderData->ot, 4096);
        ClearOTagR(&renderData->OtUi, 1);
        FieldCameraAssign();
        g_FieldKeyState = FieldButtonsUpdate(&g_CameraScrollX, &g_CameraScrollY);
        g_FieldState.currentMovieFrame = D_80075D00->unk8;
        FieldEventUpdate(&renderData->OtUi);
        g_PlayerModelId = g_FieldState.pcModelId;
        FieldBGScrollInit();
        FieldBGScrollUpdate();
        FieldBGShakeUpdate(&g_FieldState.shakeX);
        FieldBGShakeUpdate(&g_FieldState.shakeY);
        FieldBGUpdateDrawenv(renderData);
        PreloadNextFieldMap(&g_FieldEntity[g_PlayerModelId], g_FieldTriggers->gateways);
        if ((g_FieldState.activeKeysRaw & (PADstart | PADselect | PADR1 | PADR2 | PADL1 | PADL2)) ==
            (PADstart | PADselect | PADR1 | PADR2 | PADL1 | PADL2)) {
            g_FieldState.eventCmd = EVTCMD_BEAT_GAME;
            SysMovieAbortPlay();
            StopFieldMapPreload();
            return;
        }
        if (g_FieldState.eventCmd == EVTCMD_FIELD_MAP_CHANGE) {
            break;
        }
        if (g_FieldState.eventCmd == EVTCMD_LOAD_MINIGAME) {
            StopFieldMapPreload();
            return;
        }
        if (g_FieldState.eventCmd == EVTCMD_CD_CHANGE) {
            StopFieldMapPreload();
            g_GameState = GAMESTATE_CHANGE_DISK;
            return;
        }
        if (g_FieldState.eventCmd == EVTCMD_UNK19) {
            g_GameState = GAMESTATE_LOAD_INSTR2;
            StopFieldMapPreload();
            return;
        }
        if (g_FieldState.eventCmd == EVTCMD_YUFFIE_STEALS_MATERIA ||
            g_FieldState.eventCmd == EVTCMD_YUFFIE_RETURNS_MATERIA ||
            g_FieldState.eventCmd == EVTCMD_REMOVE_CHARS_MATERIA_ACCESSORY ||
            g_FieldState.eventCmd == EVTCMD_UNK15 ||
            g_FieldState.eventCmd == EVTCMD_MASTER_MATERIA_CHECK ||
            g_FieldState.eventCmd == EVTCMD_ADD_MASTER_MATERIA ||
            g_FieldState.eventCmd == EVTCMD_JENOVA_SYNTH_COPY_LEVELS) {
            g_GameState = GAMESTATE_MENU_COMMANND;
            StopFieldMapPreload();
            return;
        }
        if (g_FieldState.eventCmd == EVTCMD_CHAR_NAME_ENTRY || g_FieldState.eventCmd == EVTCMD_PARTY_SELECT ||
            g_FieldState.eventCmd == EVTCMD_PARTY_MENU || g_FieldState.eventCmd == EVTCMD_SAVE_SCREEN ||
            g_FieldState.eventCmd == EVTCMD_SHOP || g_FieldState.eventCmd == EVTCMD_UNK12 ||
            g_FieldState.eventCmd == EVTCMD_UNK13) {
            g_GameState = GAMESTATE_MENU;
            StopFieldMapPreload();
            return;
        }
        if ((g_FieldKeyState & PADRup) && !g_FieldState.menuDisabled && !g_FieldMoviePlayed && !D_80114488) {
            g_GameState = GAMESTATE_MENU;
            g_FieldState.eventCmd = EVTCMD_PARTY_MENU;
            g_FieldState.eventCmdParam = 0;
            StopFieldMapPreload();
            return;
        }
        if (g_FieldState.eventCmd == EVTCMD_PLAY_ENDING_FMV || g_FieldState.eventCmd == EVTCMD_GAME_OVER) {
            StopFieldMapPreload();
            return;
        }
        if (g_FieldState.eventCmd == EVTCMD_ENTERING_BATTLE) {
            g_FieldState.pcPosX = g_FieldEntity[g_PlayerModelId].PosX / 4096;
            g_FieldState.pcPosY = g_FieldEntity[g_PlayerModelId].PosY / 4096;
            g_FieldState.pcWalkMeshId = g_FieldEntity[g_PlayerModelId].PosI;
            g_GameState = GAMESTATE_BATTLE;
            StopFieldMapPreload();
            return;
        }
        FieldEntityMovementUpdate(g_FieldKeyState);
        FieldEntityLineInteract(&g_FieldEntity[g_PlayerModelId], g_FieldLines);
        FieldEntityCheckTalk();
        if (!D_80114488 || D_8009A060 == 1) {
            AddBackgroundToRender(renderData);
        }
        HandleKawaiDataInModel(renderData);
        FieldRainUpdate();
        FieldRainAddToRender((u32*)renderData->ot, renderData->Rain, D_80071E40, &renderData->RainDm);
        FieldArrowsAddToRender(renderData, D_80071E40, g_FieldTriggers->gateways);
        SysFadeUpdate();
        D_80114478 = VSync(1);
        while (DrawSync(1)) {
        }
        D_8011447C = VSync(1);
        VSync(D_80114488 && D_800965E4 != 1 ? 3 : 2);
        if (displayDelay) {
            if (!--displayDelay) {
                SetDispMask(1);
            }
        }
        ResetGraph(1);
        if (!D_80114488) {
            if (!D_801142C8) {
                D_8007EB68[D_80075DEC].isrgb24 = 0;
            } else {
                D_801142C8 = 0;
            }
        }
        PutDispEnv(&D_8007EB68[D_80075DEC]);
        PutDrawEnv(&D_8007EAAC[D_80075DEC]);
        if (!D_80114488) {
            ClearImage(&D_8007EAAC[D_80075DEC].clip, 0, 0, 0);
        } else if (!D_8007EB68[D_80075DEC].isrgb24) {
            ClearImage(&top, 0, 0, 0);
            ClearImage(&middle, 0, 0, 0);
            ClearImage(&bottom, 0, 0, 0);
        } else {
            ClearImage(&wideTop, 0, 0, 0);
            ClearImage(&wideMiddle, 0, 0, 0);
            ClearImage(&wideBottom, 0, 0, 0);
        }
        D_8007EBD8 = &D_8007EB68[D_80075DEC];
        D_8007EBD0 = &D_80113F2C[D_80075DEC];
        FieldUpdateMovieStream();
        if (!g_FieldState.mpdspSet) {
            DrawOTag(&renderData->OtSceneDrenv);
            DrawOTag(&renderData->ot[4095]);
            DrawOTag(&renderData->OtFadeDrenv);
            if (g_FieldState.fadeType) {
                DrawOTag(&D_8007E7A0[D_80075DEC]);
            }
        }
        DrawOTag(&renderData->OtUi);
    }
}
#endif
#undef NON_MATCHINGS

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldLoadMimToVram);

static u32 FieldButtonsUpdate(s16* scrollX, s16* scrollY) {
    FieldState* state = &g_FieldState;

    g_FieldKeyState = InputReadPadsRaw();
    state->activeKeysPrevRaw = state->activeKeysRaw;
    state->activeKeysRaw = g_FieldKeyState;
    state->pressedKeysRaw = (g_FieldKeyState ^ state->activeKeysPrevRaw) & g_FieldKeyState;
    state->releasedKeysRaw = (g_FieldKeyState ^ state->activeKeysPrevRaw) & ~g_FieldKeyState;

    g_FieldKeyState = InputReadPads();
    state->activeKeysPrev = state->activeKeys;
    state->activeKeys = g_FieldKeyState;
    state->pressedKeys = (g_FieldKeyState ^ state->activeKeysPrev) & g_FieldKeyState;
    state->releasedKeys = (g_FieldKeyState ^ state->activeKeysPrev) & ~g_FieldKeyState;

    return g_FieldKeyState;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBackgroundInitPackets);

INCLUDE_ASM("asm/us/field/nonmatchings/field", AddBackgroundToRender);

s32 FieldCalcLinearStep(s32 start, s32 target, s32 duration, s32 step) {
    s32 delta = target - start;

    if (delta > -524288 && delta < 524288) {
        start += (delta * step) / duration;
    } else {
        start += (delta / duration) * step;
    }
    return start;
}

s32 FieldCalcEaseInOut(s32 start, s32 target, s32 duration, s32 step) {
    u8 angle;
    s32 delta;

    angle = (step << 12) / duration / 32 + 128;
    delta = target - start;
    start += (FieldEntityGetDirVectorY(angle) + 4096) * delta / 8192;
    return start;
}

s32 FieldCalcWorldToScreenPos(SVECTOR* worldPos, long* screenPos) {
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

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGShakeUpdate);
#else
void FieldBGShakeUpdate(FieldShakeData* data) {
    if (data->enabled == 1) {
        if (!data->segmentActive) {
            data->currentStep = 0;
            data->start = 0;
            data->target = (s16)(g_RandomTable[data->rngId] * data->amplitude) / 256;
            data->rngId++;
            data->segmentActive = 1;
        } else if (data->numStepsPerSegment < data->currentStep) {
            data->currentStep = 0;
            data->start = data->target;
            if (data->start < 0) {
                data->target = (s16)(g_RandomTable[data->rngId] * data->amplitude) / 256;
            } else {
                data->target = -(s16)(g_RandomTable[data->rngId] * data->amplitude) / 256;
            }
            data->rngId++;
        } else {
            data->currentStep++;
            data->currentOffset =
                FieldCalcEaseInOut(data->start, data->target, data->numStepsPerSegment, data->currentStep);
        }
    } else if (data->segmentActive == 1) {
        if (data->numStepsPerSegment < data->currentStep) {
            data->currentStep = 0;
            data->start = data->target;
            data->target = 0;
            data->rngId++;
            data->segmentActive = 0;
        } else {
            data->currentStep++;
            data->currentOffset =
                FieldCalcEaseInOut(data->start, data->target, data->numStepsPerSegment, data->currentStep);
        }
    } else if (data->numStepsPerSegment == data->currentStep) {
        data->currentOffset = 0;
    } else {
        data->currentStep++;
        data->currentOffset =
            FieldCalcEaseInOut(data->start, data->target, data->numStepsPerSegment, data->currentStep);
    }
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGScrollInit);
#else
void FieldBGScrollInit(void) {
    if (g_FieldState.cameraScrollState == SCRLST_INIT) {
        switch (g_FieldState.cameraScrollMode) {
        case SCRL_OFF:
            g_CameraScrollEnabled = 0;
            g_CameraScrollX = 0;
            g_CameraScrollY = 0;
            g_FieldState.cameraScrollState = SCRLST_DONE;
            break;
        case SCRL_TO_ENTITY_INSTANT:
            g_CameraScrollEnabled = 1;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case SCRL_TO_ENTITY_LINEAR:
            g_CameraScrollEnabled = 1;
            g_CameraScrollNumSteps = g_FieldState.cameraScrollNumSteps;
            g_CameraScrollCurrentStep = 0;
            g_CameraScrollStartX = g_CameraScrollX;
            g_CameraScrollStartY = g_CameraScrollY;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case SCRL_TO_ENTITY_SMOOTH:
            g_CameraScrollEnabled = 1;
            g_CameraScrollNumSteps = g_FieldState.cameraScrollNumSteps;
            g_CameraScrollCurrentStep = 0;
            g_CameraScrollStartX = g_CameraScrollX;
            g_CameraScrollStartY = g_CameraScrollY;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case SCRL_TO_COORDS_INSTANT:
            g_CameraScrollEnabled = 1;
            g_CameraScrollX = g_FieldState.cameraScrollTargetX;
            g_CameraScrollY = g_FieldState.cameraScrollTargetY;
            g_FieldState.cameraScrollState = SCRLST_DONE;
            break;
        case SCRL_TO_COORDS_LINEAR:
            g_CameraScrollEnabled = 1;
            g_CameraScrollNumSteps = g_FieldState.cameraScrollNumSteps;
            g_CameraScrollCurrentStep = 0;
            g_CameraScrollStartX = g_CameraScrollX;
            g_CameraScrollStartY = g_CameraScrollY;
            g_CameraScrollTargetX = g_FieldState.cameraScrollTargetX;
            g_CameraScrollTargetY = g_FieldState.cameraScrollTargetY;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case SCRL_TO_COORDS_SMOOTH:
            g_CameraScrollEnabled = 1;
            g_CameraScrollNumSteps = g_FieldState.cameraScrollNumSteps;
            g_CameraScrollCurrentStep = 0;
            g_CameraScrollStartX = g_CameraScrollX;
            g_CameraScrollStartY = g_CameraScrollY;
            g_CameraScrollTargetX = g_FieldState.cameraScrollTargetX;
            g_CameraScrollTargetY = g_FieldState.cameraScrollTargetY;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case 7:
        case 8:
        case 9:
            break;
        }
    }
}
#endif

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldCalcPointOnLine);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGClampPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGGetEntityScreenPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGScrollUpdate);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldBGUpdateDrawenv);
