//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

s32 g_WmPreSector = 2903;
u32 g_WmPreSize = 280222;
extern FieldModelLoaderHeader** g_FieldModelsP;
extern FieldTriggers** g_FieldTriggersP;
extern s16 g_FieldPreloadMapId;
extern u8 D_800716D0;
extern u16 D_8007173C;
extern s8 D_80071A58;
extern FieldScriptHeader** D_8007EB64;
extern u8 D_8007EBC8;
extern s32** D_8009A044;
extern s8 D_8009A048[16];
extern volatile s16 g_GameState;
extern u8 D_8009C6D8;
extern u8** D_8009D848;
extern DRAWENV D_80113FE4[2];
extern DRAWENV D_8011409C[2];
extern DRAWENV D_80114154[2];
extern DRAWENV D_8011420C[2];

void FieldArrowsInit(SPRT_16* arrows, DR_MODE* drawMode);
void FieldEnablePartyModels(void);
void FieldEntityBgTriggerInit(FieldBgTrigger* triggers);
void FieldEventInit(FieldState* fieldState, FieldEntity* fieldModels, FieldScriptHeader* fieldScripts);
void SysFadeCopyScreen(void);
void SysFadeInitPoly(void);
void SystemCdromAbortLoading(void);
static void FieldLoadMimDatFiles(void) {

    if (g_IsFieldLoading == 0) {
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
    if (g_IsFieldLoading == 1) {
        SystemCdromAbortLoading();
    }
    D_80071A5C = 0; // needs to be called g_preloadedFieldMapId;
    g_IsFieldLoading = 0;
}

void PreloadNextFieldMap(FieldEntity* player, FieldGateway* gateways) {
    VECTOR* pos;
    s32 minDistance = 2147483647;
    s32 i;
    s32 dx;
    s32 dy;
    s32 distance;

    pos = (VECTOR*)getScratchAddr(0);
    pos->vx = player->PosX >> 12;
    pos->vy = player->PosY >> 12;
    pos->vz = player->PosZ >> 12;

    if (!g_FieldState.characterLock) {
        for (i = 0; i < 12; i++, gateways++) {
            if (gateways->fieldId != 32767) {
                dx = gateways->pos.x1 - pos->vx;
                dy = gateways->pos.y1 - pos->vy;
                distance = dx * dx + dy * dy;
                if (distance < minDistance) {
                    minDistance = distance;
                    g_FieldPreloadMapId = gateways->fieldId;
                }
            }
        }
    }

    if (g_FieldState.eventCmd == EVTCMD_LOAD_MOVIE || g_FieldMoviePlayed == 1 ||
        g_FieldState.eventCmd == EVTCMD_ENTERING_BATTLE) {
        StopFieldMapPreload();
        return;
    }
    if (D_80071A5C == g_FieldPreloadMapId) {
        return;
    }
    if (g_FieldFileInfo[g_FieldPreloadMapId * 6 + 3] >= 319488) {
        return;
    }

    StopFieldMapPreload();
    D_80071A5C = g_FieldPreloadMapId;
    if (D_80071A5C >= 65) {
        SystemLoadFileBySector(
            g_FieldFileInfo[D_80071A5C * 6 + 2], g_FieldFileInfo[D_80071A5C * 6 + 3], (u_long*)0x801B0000, NULL);
    } else {
        SystemLoadFileBySector(g_WmPreSector, g_WmPreSize, (u_long*)0x801B0000, NULL);
    }
    g_IsFieldLoading = 1;
}

void FIELD_Main(void) {
    RECT clearRect = {0, 0, 480, 472};
    s32 i;

    ClearOTagR(&g_FieldRenderData[0].otFadeDrenv, 1);
    ClearOTagR(&g_FieldRenderData[1].otFadeDrenv, 1);
    SetDrawEnv(&g_FieldRenderData[0].fadeDrenv, &D_8007EAAC[0]);
    SetDrawEnv(&g_FieldRenderData[1].fadeDrenv, &D_8007EAAC[1]);
    addPrim(&g_FieldRenderData[0].otFadeDrenv, &g_FieldRenderData[0].fadeDrenv);
    addPrim(&g_FieldRenderData[1].otFadeDrenv, &g_FieldRenderData[1].fadeDrenv);
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
    ClearOTagR(&g_FieldRenderData[0].otSceneDrenv, 1);
    ClearOTagR(&g_FieldRenderData[1].otSceneDrenv, 1);
    SetDrawEnv(&g_FieldRenderData[0].sceneDrenv, &D_80113F2C[0]);
    SetDrawEnv(&g_FieldRenderData[1].sceneDrenv, &D_80113F2C[1]);
    addPrim(&g_FieldRenderData[0].otSceneDrenv, &g_FieldRenderData[0].sceneDrenv);
    addPrim(&g_FieldRenderData[1].otSceneDrenv, &g_FieldRenderData[1].sceneDrenv);
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
    if (g_PrevGameState != GAMESTATE_FIELD && g_PrevGameState != GAMESTATE_BATTLE &&
        g_PrevGameState != GAMESTATE_WORLD && g_PrevGameState != GAMESTATE_MENU &&
        g_PrevGameState != GAMESTATE_MENU_COMMANND) {
        ClearImage(&clearRect, 0, 0, 0);
    }
    while (1) {
        DebugRunEveryLoop();
        D_80071A5C = 0;
        g_FieldPreloadMapId = 0;
        if ((g_PrevGameState == GAMESTATE_FIELD || g_PrevGameState == GAMESTATE_WORLD) &&
            g_FieldState.fadeType == FFT_INSTANT) {
            SysFadeCopyScreen();
            g_FieldState.fadeType = FFT_SYS_FADE_TO_BLACK_FIELD_CHANGE;
            D_80071A58 = 3;
            g_FieldState.fadeAdjust = 0;
            D_8007E768 = 0;
            D_80095DD4 = 1;
        }
        if (g_PrevGameState != GAMESTATE_MENU && g_PrevGameState != GAMESTATE_MENU_COMMANND) {
            D_8007EB64 = (FieldScriptHeader**)0x80114FE4;
            D_8009A044 = (s32**)0x80114FE8;
            D_8009D848 = (u8**)0x80114FEC;
            D_80083578 = (MATRIX**)0x80114FF0;
            g_FieldTriggersP = (FieldTriggers**)0x80114FF4;
            g_FieldEncountersP = (FieldEncounterTable**)0x80114FF8;
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
        if (g_PrevGameState == 0 || g_PrevGameState == GAMESTATE_FIELD || g_PrevGameState == GAMESTATE_WORLD ||
            g_PrevGameState == GAMESTATE_HIGHWAY || g_PrevGameState == GAMESTATE_SNOWBOARD1 ||
            g_PrevGameState == GAMESTATE_CHOCOBO || g_PrevGameState == GAMESTATE_FORTCONDOR ||
            g_PrevGameState == GAMESTATE_JET || g_PrevGameState == GAMESTATE_SUBMARIME) {
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
            if (!(Savemap.memory_bank_5[0x83] & 0x80)) {
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
        FieldArrowsInit(g_FieldRenderData[0].arrows, &g_FieldRenderData[0].arrowsDm);
        FieldArrowsInit(g_FieldRenderData[1].arrows, &g_FieldRenderData[1].arrowsDm);
        if (g_PrevGameState != GAMESTATE_MENU && g_PrevGameState != GAMESTATE_MENU_COMMANND) {
            FieldLoadMimToVram(0, (void*)0x80128000);
        }
        if (g_PrevGameState == GAMESTATE_BATTLE) {
            *D_8009A000 = 0xF5;
            AkaoExec();
            *D_8009A000 = 0x18;
            *D_8009A008 = 4;
            *D_8009A004 = g_FieldState.nextFieldMusic;
            AkaoExec();
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
        if (g_FieldState.eventCmd == EVTCMD_TITLE_SCREEN || g_FieldState.eventCmd == EVTCMD_GAME_OVER ||
            g_FieldState.eventCmd == EVTCMD_PLAY_ENDING_FMV) {
            break;
        }
        if (g_FieldState.eventCmd == EVTCMD_FIELD_MAP_CHANGE) {
            g_FieldState.prevFieldId = g_CurrentFieldIndex;
            g_CurrentFieldIndex = g_FieldState.eventCmdParam;
            if (g_CurrentFieldIndex != D_80071A5C) {
                StopFieldMapPreload();
            }
            if (((u16)g_CurrentFieldIndex - 1) < 64U) {
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
        if (g_FieldState.eventCmd == EVTCMD_ENTERING_BATTLE || g_FieldState.eventCmd == EVTCMD_CD_CHANGE) {
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
