//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern struct FieldRenderData g_FieldRenderData[2];
extern u32 g_FieldKeyState;
extern s16 g_CameraScrollX;
extern s16 g_CameraScrollY;
extern FieldTriggers* g_FieldTriggers;
extern volatile s16 g_FieldMoviePlayed;
extern s8 D_80071C0C;
extern OT_TYPE D_8007E7A0[2];
extern s32 D_8007EB90;
extern s32 D_8007EB94;
extern DRAWENV* D_8007EBD0;
extern DISPENV* D_8007EBD8;
extern s32 D_800965E4;
extern s32** D_8009A044;
extern s32 D_8009A060;
extern SVECTOR (*D_800E4274)[3];
extern u_long* D_800E4D90;
extern u32 D_800E4D94;
extern u16 D_800E4D98;
extern u16 D_800E4D9A;
extern u16 D_800E4D9C;
extern u16 D_800E4D9E;
extern u_long* D_800E4DA4;
extern u32 D_800E4DA8;
extern s16 D_800E4DAC;
extern s16 D_800E4DAE;
extern u16 D_800E4DB0;
extern u16 D_800E4DB2;
extern u16 D_800E4DB4;
extern u_long* D_800E4DD4;
extern u32 D_800E4DD8;
extern s16 D_800E4DDC;
extern s16 D_800E4DDE;
extern u16 D_800E4DE0;
extern u16 D_800E4DE2;
extern u16 D_800E4DE4;
extern DRAWENV D_80113F2C[2];
extern s16 D_801142C8;
extern s16 (*D_80114458)[3];
extern s32 D_80114478;
extern s32 D_8011447C;
extern volatile s16 D_80114488;
extern volatile s16 g_GameState;

void FieldEntityLineInteract(FieldEntity* arg0, FieldLine* arg1);
void HandleKawaiDataInModel(struct FieldRenderData* renderData);
void FieldModelLoadAndInit(void);
void FieldEntityInitPos(void);
void FieldCameraAssign(void);
void FieldEventUpdate(OT_TYPE* ot);
void FieldBGScrollInit(void);
void FieldBGScrollUpdate(void);
void FieldBGShakeUpdate(FieldShakeData* data);
void FieldBGUpdateDrawenv(struct FieldRenderData* renderData);
void FieldEntityMovementUpdate(u32 keys);
void FieldEntityCheckTalk(void);
void FieldRainUpdate(void);
void FieldArrowsAddToRender(struct FieldRenderData* renderData, MATRIX* matrix, FieldGateway* gateways);
void SysFadeUpdate(void);
void SysMovieAbortPlay(void);
void FieldUpdateMovieStream(void);

static u32 FieldButtonsUpdate(s16* scrollX, s16* scrollY);

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
    D_800E4274 = (SVECTOR(*)[3])(*D_8009A044 + 1);
    D_80114458 = (s16(*)[3])(D_800E4274 + *(u16*)*D_8009A044);
    if (g_PrevGameState != GAMESTATE_MENU && g_PrevGameState != GAMESTATE_BATTLE &&
        g_PrevGameState != GAMESTATE_MENU_COMMANND) {
        FieldEntityInitPos();
    }
    FieldBackgroundInitPackets(
        g_FieldRenderData[0].bg1, g_FieldRenderData[0].bg2, g_FieldRenderData[0].bgAnim, g_FieldRenderData[0].bgDm);
    FieldBackgroundInitPackets(
        g_FieldRenderData[1].bg1, g_FieldRenderData[1].bg2, g_FieldRenderData[1].bgAnim, g_FieldRenderData[1].bgDm);
    FieldRainInit(&g_FieldRenderData[0]);
    FieldRainInit(&g_FieldRenderData[1]);
    displayDelay = 1;
    D_80114488 = 0;
    D_801142C8 = 0;
    g_FieldMoviePlayed = 0;
    D_80071C0C = 0;
    g_IsFieldLoading = 0;

    while (1) {
        if (!displayDelay) {
            D_80075DEC++;
        }
        D_80075DEC &= 1;
        g_FieldState.renderBuffer = D_80075DEC;
        renderData = &g_FieldRenderData[D_80075DEC];
        ClearOTagR(renderData->ot, 4096);
        ClearOTagR(&renderData->otUi, 1);
        FieldCameraAssign();
        g_FieldKeyState = FieldButtonsUpdate(&g_CameraScrollX, &g_CameraScrollY);
        g_FieldState.currentMovieFrame = D_80075D00->unk8;
        FieldEventUpdate(&renderData->otUi);
        g_PlayerModelId = g_FieldState.pcModelId;
        FieldBGScrollInit();
        FieldBGScrollUpdate();
        FieldBGShakeUpdate(&g_FieldState.shakeX);
        FieldBGShakeUpdate(&g_FieldState.shakeY);
        FieldBGUpdateDrawenv(renderData);
        PreloadNextFieldMap(&g_FieldEntity[g_PlayerModelId], g_FieldTriggers->gateways);
        if ((g_FieldState.activeKeysRaw & (PADstart | PADselect | PADR1 | PADR2 | PADL1 | PADL2)) ==
            (PADstart | PADselect | PADR1 | PADR2 | PADL1 | PADL2)) {
            g_FieldState.eventCmd = EVTCMD_TITLE_SCREEN;
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
        if (g_FieldState.eventCmd == EVTCMD_YUFFIE_STEALS_MATERIA || g_FieldState.eventCmd == EVTCMD_YUFFIE_RETURNS_MATERIA ||
            g_FieldState.eventCmd == EVTCMD_REMOVE_CHARS_MATERIA_ACCESSORY || g_FieldState.eventCmd == EVTCMD_UNK15 ||
            g_FieldState.eventCmd == EVTCMD_MASTER_MATERIA_CHECK || g_FieldState.eventCmd == EVTCMD_ADD_MASTER_MATERIA ||
            g_FieldState.eventCmd == EVTCMD_JENOVA_SYNTH_COPY_LEVELS) {
            g_GameState = GAMESTATE_MENU_COMMANND;
            StopFieldMapPreload();
            return;
        }
        if (g_FieldState.eventCmd == EVTCMD_CHAR_NAME_ENTRY || g_FieldState.eventCmd == EVTCMD_PARTY_SELECT ||
            g_FieldState.eventCmd == EVTCMD_PARTY_MENU || g_FieldState.eventCmd == EVTCMD_SAVE_SCREEN ||
            g_FieldState.eventCmd == EVTCMD_SHOP || g_FieldState.eventCmd == EVTCMD_UNK12 || g_FieldState.eventCmd == EVTCMD_UNK13) {
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
        FieldRainAddToRender(renderData->ot, renderData->Rain, D_80071E40, &renderData->rainDm);
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
        if(!g_FieldState.mpdspSet) {
            DrawOTag(&renderData->otSceneDrenv);
            DrawOTag(&renderData->ot[4095]);
            DrawOTag(&renderData->otFadeDrenv);
            if (g_FieldState.fadeType != FFT_INSTANT) {
                DrawOTag(&D_8007E7A0[D_80075DEC]);
            }
        }
        DrawOTag(&renderData->otUi);
    }
}

void FieldLoadMimToVram(s32 arg0, void* mimData) {
    RECT rect;
    u8 unused[40];
    u_long* data = mimData;

    D_800E4D94 = *data++;
    D_800E4D98 = *data & 0xFFFF;
    D_800E4D9A = *data++ >> 16;
    D_800E4D9C = *data & 0xFFFF;
    D_800E4D9E = *data++ >> 16;
    D_800E4D90 = data;
    data += D_800E4D94 / 4 - 3;

    D_800E4DA8 = *data++;
    D_800E4DAC = *data & 0xFFFF;
    D_800E4DAE = *data++ >> 16;
    D_800E4DB0 = (*data & 0xFFFF) * 2;
    D_800E4DB2 = *data++ >> 16;
    D_800E4DA4 = data;
    data += D_800E4DA8 / 4 - 3;

    D_800E4DD8 = *data++;
    D_800E4DDC = *data & 0xFFFF;
    D_800E4DDE = *data++ >> 16;
    D_800E4DE0 = (*data & 0xFFFF) * 2;
    D_800E4DE2 = *data++ >> 16;
    D_800E4DD4 = data;

    setRECT(&rect, 0, 480, 256, 16);
    DrawSync(0);
    LoadImage(&rect, D_800E4D90);
    DrawSync(0);
    D_800E4DB4 = LoadTPage(D_800E4DA4, 1, 0, D_800E4DAC, D_800E4DAE, D_800E4DB0, D_800E4DB2);
    if (D_800E4DD8) {
        DrawSync(0);
        D_800E4DE4 = LoadTPage(D_800E4DD4, 1, 0, D_800E4DDC, D_800E4DDE, D_800E4DE0, D_800E4DE2);
    }
    DrawSync(0);
}

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
