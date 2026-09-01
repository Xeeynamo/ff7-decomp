//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

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
        DS_read(g_FieldLzsInfo[g_CurrentFieldIndex * 6],
                g_FieldLzsInfo[g_CurrentFieldIndex * 6 + 1], (u32*)0x80128000,
                NULL);
        while (SystemCdromReadChain() != 0) {
        }
    } else {
        while (SystemCdromReadChain() != 0) {
        }
        SystemLzsDecompress((void*)0x801B0000, (void*)0x80128000);
    }
    DS_read(((u32*)g_FieldFileInfo)[g_CurrentFieldIndex * 6],
            ((u32*)g_FieldFileInfo)[g_CurrentFieldIndex * 6 + 1],
            (u32*)0x80114FE4, NULL);
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

extern u16 g_FieldPreloadMapId;
extern s32 g_WmPreSector;
extern u32 g_WmPreSize;

// D_8009ABF5 = g_pFieldState -> command

void PreloadNextFieldMap(FieldEntity* Player, u16* gateway) {
    s32* scratchpad;
    s32 minDist;
    s32 i;
    s32 diffX, diffY, dist;
    s32 sector;
    u32 size;

    minDist = 0x7FFFFFFF;

    scratchpad = (s32*)0x1F800000;
    scratchpad[0] = Player->PosX >> 12;
    scratchpad[1] = Player->PosY >> 12;
    scratchpad[2] = Player->PosZ >> 12;

    if (g_FieldAnimLock == 0) {
        for (i = 0; i < 12; i++, gateway += 12) {
            if (gateway[9] != 0x7FFF) {
                diffX = ((s16*)gateway)[0] - scratchpad[0];
                diffY = ((s16*)gateway)[1] - scratchpad[1];
                dist = diffX * diffX + diffY * diffY;
                if (dist < minDist) {
                    minDist = dist;
                    g_FieldPreloadMapId = gateway[9];
                }
            }
        }
    }

    if (D_8009ABF5 == 3 || (s16)g_FieldMoviePlayed == 1 || D_8009ABF5 == 2) {
        StopFieldMapPreload();
        return;
    }

    if (D_80071A5C == (s16)g_FieldPreloadMapId) {
        return;
    }

    if (0x4DFFF < g_FieldFileTable[(s16)g_FieldPreloadMapId * 6]) {
        return;
    }

    StopFieldMapPreload();
    D_80071A5C = g_FieldPreloadMapId;

    if (D_80071A5C >= 0x41) {
        SystemLoadFileBySector(
            g_FieldFileTable[D_80071A5C * 6 - 1],
            g_FieldFileTable[D_80071A5C * 6], 0x801B0000, NULL);
    } else {
        SystemLoadFileBySector(g_WmPreSector, g_WmPreSize, 0x801B0000, NULL);
    }
    g_isFieldLoading = 1;
}

void FieldMain(void) {
    RECT clip = {0, 0, 480, 472};
    s8* fill;
    s32 fillVal;
    s32 i;
    s16 fieldId;
    s32 preloadId;
    s16 exitId;
    u8 exitKind;

    ClearOTagR(&g_FieldRenderData[0].OtFadeDrenv, 1);
    ClearOTagR(&g_FieldRenderData[1].OtFadeDrenv, 1);
    SetDrawEnv(&g_FieldRenderData[0].FadeDrenv, &g_FieldDrawEnv[0]);
    SetDrawEnv(&g_FieldRenderData[1].FadeDrenv, &g_FieldDrawEnv[1]);
    addPrim(&g_FieldRenderData[0].OtFadeDrenv, &g_FieldRenderData[0].FadeDrenv);
    addPrim(&g_FieldRenderData[1].OtFadeDrenv, &g_FieldRenderData[1].FadeDrenv);
    SetDefDrawEnv(&g_FieldDrawEnvBg[0], 0, 8, 0x140, 0xE0);
    SetDefDrawEnv(&g_FieldDrawEnvBg[1], 0, 0xF0, 0x140, 0xE0);
    SetDefDrawEnv(&D_80114154[0], 0, 8, 0x140, 0xE0);
    SetDefDrawEnv(&D_80114154[1], 0, 0xF0, 0x140, 0xE0);
    SetDefDrawEnv(&D_8011420C[0], 0, 8, 0x140, 0xE0);
    SetDefDrawEnv(&D_8011420C[1], 0, 0xF0, 0x140, 0xE0);
    g_FieldDrawEnvBg[0].dtd = 1;
    g_FieldDrawEnvBg[1].dtd = 1;
    D_80114154[0].dtd = 1;
    D_80114154[1].dtd = 1;
    D_8011420C[0].dtd = 1;
    D_8011420C[1].dtd = 1;
    g_FieldDrawEnvBg[0].isbg = 0;
    g_FieldDrawEnvBg[1].isbg = 0;
    D_80114154[0].isbg = 0;
    D_80114154[1].isbg = 0;
    D_8011420C[0].isbg = 0;
    D_8011420C[1].isbg = 0;
    ClearOTagR(&g_FieldRenderData[0].OtSceneDrenv, 1);
    ClearOTagR(&g_FieldRenderData[1].OtSceneDrenv, 1);
    SetDrawEnv(&g_FieldRenderData[0].SceneDrenv, &g_FieldDrawEnvBg[0]);
    SetDrawEnv(&g_FieldRenderData[1].SceneDrenv, &g_FieldDrawEnvBg[1]);
    addPrim(
        &g_FieldRenderData[0].OtSceneDrenv, &g_FieldRenderData[0].SceneDrenv);
    addPrim(
        &g_FieldRenderData[1].OtSceneDrenv, &g_FieldRenderData[1].SceneDrenv);
    SetDefDrawEnv(&D_80113FE4[0], 0, 8, 0x140, 0xE0);
    SetDefDrawEnv(&D_80113FE4[1], 0, 0xF0, 0x140, 0xE0);
    D_80113FE4[0].isbg = 0;
    D_80113FE4[1].isbg = 0;
    D_80113FE4[0].dtd = 1;
    D_80113FE4[1].dtd = 1;
    SetDefDrawEnv(&D_8011409C[0], 0, 8, 0x140, 0xE0);
    SetDefDrawEnv(&D_8011409C[1], 0, 0xF0, 0x140, 0xE0);
    D_8011409C[0].isbg = 0;
    D_8011409C[1].isbg = 0;
    D_8011409C[0].dtd = 1;
    D_8011409C[1].dtd = 1;
    func_800128B8();
    *(volatile u16*)&g_FieldStateData.fadeType = 0;
    if (D_800965EC != 1 && D_800965EC != 2 && D_800965EC != 3 &&
        D_800965EC != 5 && D_800965EC != 0xD) {
        ClearImage(&clip, 0, 0, 0);
    }

    for (;;) {
        DebugRunEveryLoop();
        D_80071A5C = 0;
        g_FieldPreloadMapId = 0;
        if ((D_800965EC == 1 || D_800965EC == 3) &&
            g_FieldStateData.fadeType == 0) {
            func_800129D0();
            g_FieldStateData.fadeType = 3;
            D_80071A58 = 3;
            g_FieldStateData.fadeAdjust = 0;
            D_8007E768 = 0;
            D_80095DD4 = 1;
        }
        if (D_800965EC != 5 && D_800965EC != 0xD) {
            D_8007EB64 = (s32*)0x80114FE4;
            D_8009A044 = (FieldWalkmesh**)0x80114FE8;
            D_8009D848 = (FieldBgData**)0x80114FEC;
            D_80083578 = (MATRIX**)0x80114FF0;
            g_FieldTriggersP = (s32*)0x80114FF4;
            g_FieldEncountersP = (s32*)0x80114FF8;
            g_FieldModelsP = (s32*)0x80114FFC;
            FieldLoadMimDatFiles();
        }
        if (D_800965EC == 2) {
            D_8007EBE0 = 1;
            if (D_8007EBC8 == 1) {
                D_8007EBC8 = 0;
                D_8009C6D8 = 0;
                D_8007173C = 0;
                *((volatile u8*)&g_FieldStateData.fadeType - 0x4B) = 0;
            }
        }
        while (D_80095DD4 != 0) {
        }
        while (DrawSync(1) != 0) {
        }
        if (D_800965EC != 0xD) {
            *(volatile u16*)&g_FieldStateData.fadeType = 1;
            *(volatile s16*)&g_FieldStateData.fadeSpeed = 0x10;
            *(volatile s16*)&g_FieldStateData.fadeAdjust = 0x100;
            *(volatile s16*)&g_FieldStateData.fadeRed = 0;
            *(volatile s16*)&g_FieldStateData.fadeGreen = 0;
            *(volatile s16*)&g_FieldStateData.fadeBlue = 0;
        }
        if (D_800965EC == 0 || D_800965EC == 1 || D_800965EC == 3 ||
            D_800965EC == 6 || D_800965EC == 8 || D_800965EC == 7 ||
            D_800965EC == 9 || D_800965EC == 0xB || D_800965EC == 0xA) {
            g_FieldStateData.layer2_bgScrollXSpeed = 0;
            g_FieldStateData.layer2_bgScrollYSpeed = 0;
            g_FieldStateData.layer3_bgScrollXSpeed = 0;
            g_FieldStateData.layer3_bgScrollYSpeed = 0;
            g_FieldStateData.layer3_depth = 1;
            g_FieldStateData.layer2_depth = 0xFFF;
            D_8009A100 = 0;
            D_80071E38 = 0;
            D_80071E3C = 0;
            g_FieldBGCameraHeightBias =
                ((FieldTriggerHeader*)g_FieldTriggers)->camHeightBias;
            FieldEventInit(&g_FieldStateData, g_FieldEntity, *D_8007EB64);
            g_FieldEntity[D_8009AC1E].Dir = D_8009AC18;
            fillVal = -1;
            if ((g_RainControl & 0x80) == 0) {
                g_RainForce = 0;
            } else {
                g_RainForce = 0xFF;
            }
            /* Emits nothing. It is here to stop reorg copying `i = 0xF` into
             * the delay slot of the jump that reaches this join -- see the
             * `stop_search_p` paragraph in the note above. Whatever the
             * original wrote here is not recoverable; only that it reached
             * RTL as an ASM_INPUT is. */
            __asm__("");
            i = 0xF;
            fill = &D_8009A057;
            do {
                *fill-- = fillVal;
            } while (--i >= 0);
            FieldEntityBgTriggerInit((void*)(g_FieldTriggers + 0x158));
        } else {
            D_8009AC1A[0] = 2;
        }
        FieldEnablePartyModels();
        FieldEntityLineClear(&D_8007E7AC);
        D_800716D0 = 0;
        FieldArrowsInit(
            g_FieldRenderData[0].Arrows, &g_FieldRenderData[0].ArrowsDm);
        FieldArrowsInit(
            g_FieldRenderData[1].Arrows, &g_FieldRenderData[1].ArrowsDm);
        if (D_800965EC != 5 && D_800965EC != 0xD) {
            FieldLoadMimToVram(0, (u8*)0x80128000);
        }
        if (D_800965EC == 2) {
            D_8009A000[0] = 0xF5;
            SystemAkaoExecute();
            D_8009A000[0] = 0x18;
            D_8009A008[0] = 4;
            D_8009A004[0] = D_8009AC3C[0];
            SystemAkaoExecute();
        }
        FieldMainLoop();
        while (DrawSync(1) != 0) {
        }
        VSync(1);
        g_FieldDispEnv[0].isrgb24 = 0;
        g_FieldDispEnv[1].isrgb24 = 0;
        PutDispEnv(&g_FieldDispEnv[(s16)D_80075DEC]);
        PutDrawEnv(&g_FieldDrawEnv[(s16)D_80075DEC]);
        D_800965EC = 1;
        if (*((volatile u8*)&g_FieldStateData.fadeType - 0x4B) == 0xA ||
            *((volatile u8*)&g_FieldStateData.fadeType - 0x4B) == 0x1A ||
            *((volatile u8*)&g_FieldStateData.fadeType - 0x4B) == 5) {
            break;
        }
        if (*((volatile u8*)&g_FieldStateData.fadeType - 0x4B) == 1) {
            preloadId = D_80071A5C;
            ((volatile FieldU16Slot*)(((volatile u8*)&g_FieldStateData
                                           .fadeType -
                                       0x4B) +
                                      0x63))
                ->v = (u16)g_CurrentFieldIndex;
            fieldId =
                *(volatile u16*)(((volatile u8*)&g_FieldStateData.fadeType -
                                  0x4B) +
                                 1);
            g_CurrentFieldIndex = fieldId;
            if (fieldId != preloadId) {
                StopFieldMapPreload();
            }
            if ((u32)((u16)g_CurrentFieldIndex - 1) < 0x40) {
                g_FieldNextModule = 3;
                func_800129D0();
                *(volatile u16*)(((volatile u8*)&g_FieldStateData.fadeType -
                                  0x4B) +
                                 0x4B) = 3;
                D_80071A58 = 3;
                *(volatile u16*)(((volatile u8*)&g_FieldStateData.fadeType -
                                  0x4B) +
                                 0x4D) = 0;
                D_8007E768 = 0;
                D_80095DD4 = 1;
                break;
            }
        }
        if (*((volatile u8*)&g_FieldStateData.fadeType - 0x4B) == 0xC) {
            *(volatile u16*)(((volatile u8*)&g_FieldStateData.fadeType - 0x4B) +
                             0x63) = (u16)g_CurrentFieldIndex;
            exitId =
                *(volatile u16*)(((volatile u8*)&g_FieldStateData.fadeType -
                                  0x4B) +
                                 1);
            exitKind = ((volatile u8*)&g_FieldStateData.fadeType - 0x4B)[0xF1];
            g_CurrentFieldIndex = exitId;
            switch (exitKind) {
            case 0:
                g_FieldNextModule = 6;
                break;
            case 1:
                g_FieldNextModule = 7;
                break;
            case 2:
                g_FieldNextModule = 8;
                break;
            case 3:
                g_FieldNextModule = 9;
                break;
            case 4:
                g_FieldNextModule = 0xA;
                break;
            case 5:
                g_FieldNextModule = 0xB;
                break;
            case 6:
                g_FieldNextModule = 0xE;
                break;
            }
            break;
        }
        if (*((volatile u8*)&g_FieldStateData.fadeType - 0x4B) == 2 ||
            *((volatile u8*)&g_FieldStateData.fadeType - 0x4B) == 0xD) {
            break;
        }
        if (g_FieldNextModule == 5) {
            func_800129D0();
            *(volatile u16*)(((volatile u8*)&g_FieldStateData.fadeType - 0x4B) +
                             0x4B) = 0xD;
            D_80071A58 = 0xD;
            *(volatile u16*)(((volatile u8*)&g_FieldStateData.fadeType - 0x4B) +
                             0x4D) = 0;
            D_8007E768 = 0;
            D_80095DD4 = 1;
            break;
        }
        if (g_FieldNextModule == 0xD) {
            break;
        }
        if (g_FieldNextModule == 0x10) {
            break;
        }
    }
    VSync(0);
}

s32 FieldMainLoop(void) {
    RECT clip24Top = {0, 0, 480, 8};
    RECT clip24Mid = {0, 232, 480, 8};
    RECT clip24Bot = {0, 464, 480, 8};
    RECT clip16Top = {0, 0, 320, 8};
    RECT clip16Mid = {0, 232, 320, 8};
    RECT clip16Bot = {0, 464, 320, 8};
    struct FieldRenderData* buf;
    s16* tris;
    s16 first;

    g_FieldScreenCenterX = 160;
    g_FieldScreenCenterY = 120;
    if (D_800965EC != 5 && D_800965EC != 0xD) {
        FieldModelLoadAndInit();
    }
    tris = (*D_8009A044)->tris;
    D_800E4274 = tris;
    D_80114458 = (s16*)((*D_8009A044)->triCount * 24 + (s32)tris);
    if (D_800965EC != 5 && D_800965EC != 2 && D_800965EC != 0xD) {
        FieldEntityInitPos();
    }
    FieldBackgroundInitPackets(
        g_FieldRenderData[0].Bg1, g_FieldRenderData[0].Bg2,
        (u8*)g_FieldRenderData[0].BgAnim, g_FieldRenderData[0].BgDm);
    first = 1;
    FieldBackgroundInitPackets(
        g_FieldRenderData[1].Bg1, g_FieldRenderData[1].Bg2,
        (u8*)g_FieldRenderData[1].BgAnim, g_FieldRenderData[1].BgDm);
    FieldRainInit(&g_FieldRenderData[0]);
    FieldRainInit(&g_FieldRenderData[1]);
    g_FieldMovieStreamActive = 0;
    g_FieldMovieStreamDone = 0;
    g_FieldMoviePlayed = 0;
    g_FieldLineCheckResult = 0;
    g_isFieldLoading = 0;

    for (;;) {
        if (first == 0) {
            D_80075DEC++;
        }
        D_80075DEC = D_80075DEC & 1;
        g_FieldStateData.renderBuffer = D_80075DEC;
        buf = &g_FieldRenderData[(s16)D_80075DEC];
        ClearOTagR(buf->ot, 0x1000);
        ClearOTagR(&buf->OtUi, 1);
        FieldCameraAssign();
        g_FieldKeyState = FieldButtonsUpdate(&D_80071E38, &D_80071E3C);
        g_FieldStateData.currentMovieFrame = g_MovieStream->currentFrame;
        FieldEventUpdate((s32)&buf->OtUi);
        g_PlayerModelId = g_FieldStateData.pcModelId;
        FieldBGScrollInit();
        FieldBGScrollUpdate();
        FieldBGShakeUpdate(&g_FieldStateData.shakeX);
        FieldBGShakeUpdate(&g_FieldStateData.shakeY);
        FieldBGUpdateDrawenv(buf);
        PreloadNextFieldMap(&g_FieldEntity[g_PlayerModelId],
                            (FieldLine*)(g_FieldTriggers + 0x38));
        if ((g_FieldStateData.activeKeysRaw & 0x90F) == 0x90F) {
            g_FieldStateData.eventCmd = 0xA;
            func_80035658();
            StopFieldMapPreload();
            return;
        }
        if (g_FieldStateData.eventCmd == 1) {
            return;
        }
        if (g_FieldStateData.eventCmd == 0xC) {
            StopFieldMapPreload();
            return;
        }
        if (g_FieldStateData.eventCmd == 0xD) {
            StopFieldMapPreload();
            g_FieldNextModule = 0xC;
            return;
        }
        if (g_FieldStateData.eventCmd == 0x19) {
            g_FieldNextModule = 0x10;
            StopFieldMapPreload();
            return;
        }
        if (g_FieldStateData.eventCmd == 0xF ||
            g_FieldStateData.eventCmd == 0x10 ||
            g_FieldStateData.eventCmd == 0x11 ||
            g_FieldStateData.eventCmd == 0x15 ||
            g_FieldStateData.eventCmd == 0x16 ||
            g_FieldStateData.eventCmd == 0x17 ||
            g_FieldStateData.eventCmd == 0x18) {
            g_FieldNextModule = 0xD;
            StopFieldMapPreload();
            return;
        }
        if (g_FieldStateData.eventCmd == 6 || g_FieldStateData.eventCmd == 7 ||
            g_FieldStateData.eventCmd == 9 ||
            g_FieldStateData.eventCmd == 0xE ||
            g_FieldStateData.eventCmd == 8 ||
            g_FieldStateData.eventCmd == 0x12 ||
            g_FieldStateData.eventCmd == 0x13) {
            g_FieldNextModule = 5;
            StopFieldMapPreload();
            return;
        }
        if ((g_FieldKeyState & 0x10) && g_FieldStateData.menuDisabled == 0 &&
            g_FieldMoviePlayed == 0 && g_FieldMovieStreamActive == 0) {
            g_FieldNextModule = 5;
            g_FieldStateData.eventCmd = 9;
            g_FieldStateData.eventCmdParam = 0;
            StopFieldMapPreload();
            return;
        }
        if (g_FieldStateData.eventCmd == 5 ||
            g_FieldStateData.eventCmd == 0x1A) {
            StopFieldMapPreload();
            return;
        }
        if (g_FieldStateData.eventCmd == 2) {
            g_FieldStateData.pcPosX =
                g_FieldEntity[g_PlayerModelId].PosX / 4096;
            g_FieldStateData.pcPosY =
                g_FieldEntity[g_PlayerModelId].PosY / 4096;
            g_FieldNextModule = 2;
            g_FieldStateData.pcWalkMeshId = g_FieldEntity[g_PlayerModelId].PosI;
            StopFieldMapPreload();
            return;
        }
        FieldEntityMovementUpdate(g_FieldKeyState);
        FieldEntityLineInteract(&g_FieldEntity[g_PlayerModelId], &D_8007E7AC);
        FieldEntityCheckTalk();
        if (g_FieldMovieStreamActive == 0 || g_FieldMovieDrawBg == 1) {
            AddBackgroundToRender(buf);
        }
        HandleKawaiDataInModel(buf);
        FieldRainUpdate();
        FieldRainAddToRender(buf->ot, buf->Rain, D_80071E40, &buf->RainDm);
        FieldArrowsAddToRender(buf, D_80071E40, g_FieldTriggers + 0x38);
        func_800138EC();
        g_FieldVSyncBeforeDraw = VSync(1);
        while (DrawSync(1) != 0) {
        }
        g_FieldVSyncAfterDraw = VSync(1);
        if (g_FieldMovieStreamActive != 0 && g_FieldMovieVSyncMode != 1) {
            VSync(3);
        } else {
            VSync(2);
        }
        if (first != 0) {
            first--;
            if (first == 0) {
                SetDispMask(1);
            }
        }
        ResetGraph(1);
        if (g_FieldMovieStreamActive == 0) {
            if (g_FieldMovieStreamDone == 0) {
                g_FieldDispEnv[(s16)D_80075DEC].isrgb24 = 0;
            } else {
                g_FieldMovieStreamDone = 0;
            }
        }
        PutDispEnv(&g_FieldDispEnv[(s16)D_80075DEC]);
        PutDrawEnv(&g_FieldDrawEnv[(s16)D_80075DEC]);
        if (g_FieldMovieStreamActive == 0) {
            ClearImage(&g_FieldDrawEnv[(s16)D_80075DEC].clip, 0, 0, 0);
        } else if (g_FieldDispEnv[(s16)D_80075DEC].isrgb24 == 0) {
            ClearImage(&clip16Top, 0, 0, 0);
            ClearImage(&clip16Mid, 0, 0, 0);
            ClearImage(&clip16Bot, 0, 0, 0);
        } else {
            ClearImage(&clip24Top, 0, 0, 0);
            ClearImage(&clip24Mid, 0, 0, 0);
            ClearImage(&clip24Bot, 0, 0, 0);
        }
        g_FieldCurDispEnv = &g_FieldDispEnv[(s16)D_80075DEC];
        g_FieldCurDrawEnv = &g_FieldDrawEnvBg[(s16)D_80075DEC];
        FieldUpdateMovieStream();
        if (g_FieldStateData.mpdspSet == 0) {
            DrawOTag(&buf->OtSceneDrenv);
            DrawOTag(&buf->ot[0xFFF]);
            DrawOTag(&buf->OtFadeDrenv);
            if (D_8009AC40[0] != 0) {
                DrawOTag(&g_FieldOTHead[(s16)D_80075DEC]);
            }
        }
        DrawOTag(&buf->OtUi);
    }
}

/* Parse a MIM (field background map image) header and upload its palette and
 * two texture pages to VRAM. `mim` points at the loaded file; three
 * variable-length records follow one another, each opening with a 32-bit byte
 * length, and each seeds a slice of the state block at g_FieldMimPalData. The
 * palette goes up with LoadImage, the two pages with LoadTPage, with a DrawSync
 * between every step. */
void FieldLoadMimToVram(s32 arg0, u8* mim) {
    RECT rect;
    u8 unusedLocals[0x28];
    u32 next;
    u16 unk0A;

    *(u32*)&g_FieldMimPalSize[0] = *(u32*)mim;
    next = (*(u32*)&g_FieldMimPalSize[0] >> 2) * 4 - 0xC;
    *(u16*)&g_FieldMimPalX[0] = *(u16*)(mim + 4);
    *(u16*)&g_FieldMimPalY[0] = *(u16*)(mim + 6);
    *(u16*)&g_FieldMimPalW[0] = *(u16*)(mim + 8);
    unk0A = *(u16*)(mim + 0xA);
    mim += 0xC;
    *(u8**)&g_FieldMimPalData[0] = mim;
    *(u16*)&g_FieldMimPalH[0] = unk0A;
    mim += next;

    /* First texture page block. */
    *(u32*)&g_FieldMimTex0Size[0] = *(u32*)mim;
    next = (*(u32*)&g_FieldMimTex0Size[0] >> 2) * 4 - 0xC;
    mim += 4;
    *(u16*)&g_FieldMimTex0X[0] = *(u16*)mim;
    *(u16*)&g_FieldMimTex0Y[0] = *(u16*)(mim + 2);
    mim += 4;
    *(u16*)&g_FieldMimTex0Rect[0] = *(u16*)mim * 2;
    *(u16*)((u8*)g_FieldMimTex0Rect + 2) = *(u16*)(mim + 2);
    mim += 4;
    *(u8**)&g_FieldMimTex0Data[0] = mim;
    mim += next;

    /* Second texture page block. */
    *(u32*)&g_FieldMimTex1Size[0] = *(u32*)mim;
    mim += 4;
    *(u16*)&g_FieldMimTex1X[0] = *(u16*)mim;
    *(u16*)&g_FieldMimTex1Y[0] = *(u16*)(mim + 2);
    mim += 4;
    *(u16*)&g_FieldMimTex1Rect[0] = *(u16*)mim * 2;
    *(u16*)((u8*)g_FieldMimTex1Rect + 2) = *(u16*)(mim + 2);
    mim += 4;
    *(u8**)&g_FieldMimTex1Data[0] = mim;

    rect.x = 0;
    rect.y = 0x1E0;
    rect.w = 0x100;
    rect.h = 0x10;
    DrawSync(0);
    LoadImage(&rect, *(u_long**)((u8*)g_FieldMimPalSize - 4));
    DrawSync(0);
    *(u16*)&g_FieldMimTex0Tpage[0] = LoadTPage(
        *(u_long**)((u8*)g_FieldMimPalData + 0x14), 1, 0,
        *(s16*)((u8*)g_FieldMimPalData + 0x1C),
        *(s16*)((u8*)g_FieldMimPalData + 0x1E),
        *(u16*)((u8*)g_FieldMimPalData + 0x20),
        *(u16*)((u8*)g_FieldMimPalData + 0x22));
    if (*(u32*)((u8*)g_FieldMimPalData + 0x48) != 0) {
        DrawSync(0);
        *(u16*)&g_FieldMimTex1Tpage[0] = LoadTPage(
            *(u_long**)((u8*)g_FieldMimPalData + 0x44), 1, 0,
            *(s16*)((u8*)g_FieldMimPalData + 0x4C),
            *(s16*)((u8*)g_FieldMimPalData + 0x4E),
            *(u16*)((u8*)g_FieldMimPalData + 0x50),
            *(u16*)((u8*)g_FieldMimPalData + 0x52));
    }
    DrawSync(0);
}

u32 FieldButtonsUpdate(void) {
    g_FieldKeyState = InputReadPadsRaw();
    g_FieldState.activeKeysPrevRaw = g_FieldState.activeKeysRaw;
    g_FieldState.activeKeysRaw = g_FieldKeyState;
    g_FieldState.pressedKeysRaw =
        (g_FieldKeyState ^ g_FieldState.activeKeysPrevRaw) & g_FieldKeyState;
    g_FieldState.releasedKeysRaw =
        (g_FieldKeyState ^ g_FieldState.activeKeysPrevRaw) & ~g_FieldKeyState;

    g_FieldKeyState = InputReadPads();
    g_FieldState.activeKeysPrev = g_FieldState.activeKeys;
    g_FieldState.activeKeys = g_FieldKeyState;
    g_FieldState.pressedKeys =
        (g_FieldKeyState ^ g_FieldState.activeKeysPrev) & g_FieldKeyState;
    g_FieldState.releasedKeys =
        (g_FieldKeyState ^ g_FieldState.activeKeysPrev) & ~g_FieldKeyState;

    return g_FieldKeyState;
}

/* Build the sprite packets for the field background's four layers. Layers 1
 * and 2 are 16x16 sprites, layers 3 and 4 are 32x32; each layer walks the run
 * list from where the previous one stopped, emitting one packet per tile and a
 * DR_MODE whenever a run asks for a different texture page. `pairs` collects
 * the two per-sprite parameter bytes the animation code later edits in
 * place. */
void FieldBackgroundInitPackets(
    SPRT_16* sprt16, SPRT* sprt, u8* pairs, DR_MODE* modes) {
    FieldBgData* data1;
    FieldBgData* data2;
    FieldBgData* data3;
    FieldBgTile1* tile1;
    FieldBgTile2* tile2;
    u16* tpages;
    s16* run;
    s16 count;
    u8 white;
    u16 spriteCount;
    u16 sprite34Count;

    spriteCount = 0;
    sprite34Count = 0;
    D_8011448C = 0;
    D_801144D0 = 0;
    data1 = *D_8009D848;
    run = data1->runs;
    tile1 = (FieldBgTile1*)((u8*)data1 + data1->layer1Offset);
    tpages = (u16*)((u8*)data1 + data1->tpageOffset);

    for (;;) {
        if (run[0] == 0x7FFF) {
            run++;
            goto layer2;
        }
        if (run[0] == 0x7FFE) {
            SetDrawMode(modes, 0, 1, tpages[0], NULL);
            tpages++;
            D_8011448C++;
            modes++;
        } else {
            count = run[2];
            if (count != 0) {
                do {
                    SetSprt16(sprt16);
                    SetShadeTex(sprt16, 1);
                    SetSemiTrans(sprt16, 0);
                    sprt16->r0 = 0x80;
                    sprt16->g0 = 0x80;
                    sprt16->b0 = 0x80;
                    sprt16->x0 = tile1->x;
                    sprt16->y0 = tile1->y;
                    sprt16->u0 = tile1->u;
                    sprt16->v0 = tile1->v;
                    sprt16->clut = tile1->clut;
                    tile1++;
                    sprt16++;
                    pairs += 2;
                    count--;
                    spriteCount++;
                } while (count != 0);
            }
        }
        run += 3;
    }

layer2:
    D_8011448C = spriteCount - D_8011448C;
    data2 = *D_8009D848;
    tile2 = (FieldBgTile2*)((u8*)data2 + data2->layer2Offset);

    for (;;) {
        if (run[0] == 0x7FFF) {
            run++;
            goto layer3;
        }
        count = run[2];
        if (count != 0) {
            do {
                SetDrawMode(modes, 0, 1, tile2->tpage, NULL);
                modes++;
                D_801144D0++;
                SetSprt16(sprt16);
                SetShadeTex(sprt16, 1);
                if ((tile2->flags & 0x80) == 0) {
                    SetSemiTrans(sprt16, 0);
                } else {
                    SetSemiTrans(sprt16, 1);
                }
                sprt16->r0 = tile2->rg;
                sprt16->g0 = tile2->rg >> 8;
                sprt16->b0 = 0x80;
                sprt16->x0 = tile2->x;
                sprt16->y0 = tile2->y;
                sprt16->u0 = tile2->u;
                sprt16->v0 = tile2->v;
                sprt16->clut = tile2->clut;
                pairs[0] = tile2->flags;
                pairs[1] = tile2->param;
                tile2++;
                sprt16++;
                pairs += 2;
                count--;
                spriteCount++;
            } while (count != 0);
        }
        run += 3;
    }

layer3:
    white = 0x80;
    D_801144C8 = spriteCount;
    data3 = *D_8009D848;
    D_8007EBD4 = (FieldBgTile3*)((u8*)data3 + data3->layer34Offset);

    for (;;) {
        if (run[0] == 0x7FFF) {
            run++;
            goto layer4;
        }
        if (run[0] == 0x7FFE) {
            SetDrawMode(modes, 0, 1, tpages[0], NULL);
            tpages++;
            modes++;
        } else {
            run[1] = sprite34Count;
            count = run[2];
            if (count != 0) {
                do {
                    SetSprt(sprt);
                    SetShadeTex(sprt, 1);
                    if ((D_8007EBD4->flags & 0x80) == 0) {
                        SetSemiTrans(sprt, 0);
                    } else {
                        SetSemiTrans(sprt, 1);
                    }
                    sprt->r0 = white;
                    sprt->g0 = white;
                    sprt->b0 = white;
                    sprt->x0 = D_8007EBD4->x;
                    sprt->y0 = D_8007EBD4->y;
                    sprt->u0 = D_8007EBD4->u;
                    sprt->v0 = D_8007EBD4->v;
                    sprt->w = 0x20;
                    sprt->h = 0x20;
                    sprt->clut = D_8007EBD4->clut;
                    pairs[0] = D_8007EBD4->flags;
                    pairs[1] = D_8007EBD4->param;
                    D_8007EBD4++;
                    sprt++;
                    pairs += 2;
                    count--;
                    sprite34Count++;
                } while (count != 0);
            }
        }
        run += 3;
    }

layer4:
    white = 0x80;
    for (;;) {
        if (run[0] == 0x7FFF) {
            return;
        }
        if (run[0] == 0x7FFE) {
            SetDrawMode(modes, 0, 1, tpages[0], NULL);
            tpages++;
            modes++;
        } else {
            run[1] = sprite34Count;
            count = run[2];
            if (count != 0) {
                do {
                    SetSprt(sprt);
                    SetShadeTex(sprt, 1);
                    if ((D_8007EBD4->flags & 0x80) == 0) {
                        SetSemiTrans(sprt, 0);
                    } else {
                        SetSemiTrans(sprt, 1);
                    }
                    sprt->r0 = white;
                    sprt->g0 = white;
                    sprt->b0 = white;
                    sprt->x0 = D_8007EBD4->x;
                    sprt->y0 = D_8007EBD4->y;
                    sprt->u0 = D_8007EBD4->u;
                    sprt->v0 = D_8007EBD4->v;
                    sprt->w = 0x20;
                    sprt->h = 0x20;
                    sprt->clut = D_8007EBD4->clut;
                    pairs[0] = D_8007EBD4->flags;
                    pairs[1] = D_8007EBD4->param;
                    D_8007EBD4++;
                    sprt++;
                    pairs += 2;
                    count--;
                    sprite34Count++;
                } while (count != 0);
                do {
                } while (0);
            }
        }
        run += 3;
    }
}

/* Link this frame's visible background tiles into the field's ordering
 * table. */
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

s32 FieldCalcEaseInOut(s32 from, s32 to, s32 total, s32 step) {
    s32 angle;
    s32 diff;

    angle = ((step << 12) / total) / 32 - 0x80;
    diff = to - from;
    return from +
           ((FieldEntityGetDirVectorY(angle & 0xFF) + 0x1000) * diff) / 0x2000;
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

/* Advance one axis of the SHAKE camera effect. The shake runs as a chain of
 * segments: each one eases the background offset from where the previous
 * segment left off to a fresh random target, negated relative to the last so
 * the image swings either side of centre. Clearing `enabled` does not stop it
 * dead -- it eases one final segment back to zero first.
 *
 * Every instruction matches except the four that increment currentStep: gcc
 * coalesces `step + 1` into a3 (the argument register) where the original
 * keeps it in v0, which lets the original schedule the store into the call's
 * delay slot. */
void FieldBGShakeUpdate(FieldShakeData* shake) {
    s16 step;
    s16 target;

    if (shake->enabled == 1) {
        if (shake->segmentActive == 0) {
            shake->currentStep = 0;
            shake->start = 0;
            shake->target =
                (s16)(g_RandomTable[shake->rngId] * shake->amplitude) / 256;
            shake->segmentActive = 1;
            shake->rngId++;
            return;
        }
        step = shake->currentStep;
        if (shake->numStepsPerSegment < step) {
            target = shake->target;
            shake->currentStep = 0;
            shake->start = target;
            if (target < 0) {
                shake->target =
                    (s16)(g_RandomTable[shake->rngId] * shake->amplitude) / 256;
            } else {
                shake->target =
                    -(s16)(g_RandomTable[shake->rngId] * shake->amplitude) /
                    256;
            }
            shake->rngId++;
            return;
        }
    } else if (shake->segmentActive == 1) {
        step = shake->currentStep;
        if (shake->numStepsPerSegment < step) {
            shake->currentStep = 0;
            shake->start = shake->target;
            shake->target = 0;
            shake->segmentActive = 0;
            shake->rngId++;
            return;
        }
    } else {
        step = shake->currentStep;
        if (shake->numStepsPerSegment == step) {
            shake->currentOffset = 0;
            return;
        }
    }
    /* The two arms are deliberately identical. gcc cross-jumps them back into
     * one block, but only after it has allocated `step + 1` to its own
     * register instead of coalescing it into a3 -- which is what lets the
     * store of currentStep be scheduled into the call's delay slot, as the
     * original does. Written once, the tail is four instructions off. */
    if (step != 0) {
        step = step + 1;
        shake->currentStep = step;
        shake->currentOffset = FieldCalcEaseInOut(
            shake->start, shake->target, shake->numStepsPerSegment, step);
    } else {
        step = step + 1;
        shake->currentStep = step;
        shake->currentOffset = FieldCalcEaseInOut(
            shake->start, shake->target, shake->numStepsPerSegment, step);
    }
}
