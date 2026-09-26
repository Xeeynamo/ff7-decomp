//! PSYQ=3.3 CC1=2.7.2
#include <libapi.h>
#include "savemenu.h"
#include <libetc.h>

static void TitlePlaySfx(u16 soundId) {
    g_AkaoCmd.opcode = AKAO_PLAY_MENU_SOUND;
    g_AkaoCmd.params[0] = soundId;
    g_AkaoCmd.params[1] = soundId;
    AkaoExec();
}

static s32 TitleDoFade(s32 fadeDirection) {
    RECT clipRect;

    setTile(g_GpuPacketPtr.tile);
    SetSemiTrans(g_GpuPacketPtr.tile, 1);
    g_GpuPacketPtr.tile->x0 = 0;
    g_GpuPacketPtr.tile->y0 = 0;
    g_GpuPacketPtr.tile->w = 384;
    g_GpuPacketPtr.tile->h = 232;
    setRGB0(g_GpuPacketPtr.tile, g_TitleFadeBrightness, g_TitleFadeBrightness, g_TitleFadeBrightness);
    AddPrim(g_CurrentOT, g_GpuPacketPtr.tile++);
    setRECT(&clipRect, 0, 0, 255, 255);
    SysMenuSetDrawMode(0, 1, 0x1F | 0x40, &clipRect);
    g_TitleFadeBrightness += fadeDirection;
    if (g_TitleFadeBrightness < 0) {
        g_TitleFadeBrightness = 0;
    }
    if (g_TitleFadeBrightness > 255) {
        g_TitleFadeBrightness = 255;
    }
    return g_TitleFadeBrightness;
}

static void TitleApplySoundMode(s32 mode) {
    switch (mode) {
    case MONO:
        g_AkaoCmd.opcode = AKAO_SET_MONO_MODE;
        g_AkaoCmd.params[0] = AKAO_SET_MONO_MODE;
        g_AkaoCmd.params[1] = AKAO_SET_MONO_MODE;
        break;
    case STEREO:
        g_AkaoCmd.opcode = AKAO_SET_STEREO_MODE;
        g_AkaoCmd.params[0] = AKAO_SET_STEREO_MODE;
        g_AkaoCmd.params[1] = AKAO_SET_STEREO_MODE;
        break;
    case VOL_RESET:
        g_AkaoCmd.opcode = AKAO_RESET_VOLUME;
        g_AkaoCmd.params[0] = AKAO_RESET_VOLUME;
        g_AkaoCmd.params[1] = AKAO_RESET_VOLUME;
        break;
    }
    AkaoExec();
}

INCLUDE_ASM("asm/us/menu/nonmatchings/title", SaveHandleScrollCursor);

static void SavePeekSwCardStatus(void) {
    TestEvent(g_MemcardEvents[0]);
    TestEvent(g_MemcardEvents[1]);
    TestEvent(g_MemcardEvents[2]);
    TestEvent(g_MemcardEvents[3]);
}

static void SavePeekHwCardStatus(void) {
    TestEvent(g_MemcardEvents[4]);
    TestEvent(g_MemcardEvents[5]);
    TestEvent(g_MemcardEvents[6]);
    TestEvent(g_MemcardEvents[7]);
}

static s32 SaveWaitSwCardStatus(void) {
    while (1) {
        if (TestEvent(g_MemcardEvents[0]) == 1) {
            return CARD_EVENT_SUCCESS;
        }
        if (TestEvent(g_MemcardEvents[1]) == 1) {
            return CARD_EVENT_ERROR;
        }
        if (TestEvent(g_MemcardEvents[2]) == 1) {
            return CARD_EVENT_TIMEOUT;
        }
        if (TestEvent(g_MemcardEvents[3]) == 1) {
            return CARD_EVENT_NEW_CARD;
        }
    }
}

static s32 SaveWaitHwCardStatus(void) {
    while (1) {
        if (TestEvent(g_MemcardEvents[4]) == 1) {
            return CARD_EVENT_SUCCESS;
        }
        if (TestEvent(g_MemcardEvents[5]) == 1) {
            return CARD_EVENT_ERROR;
        }
        if (TestEvent(g_MemcardEvents[6]) == 1) {
            return CARD_EVENT_TIMEOUT;
        }
        if (TestEvent(g_MemcardEvents[7]) == 1) {
            return CARD_EVENT_NEW_CARD;
        }
    }
}

#ifdef VERSION_PC
static void SaveFetchCardStatus(s32 cardId) {
#else
static s32 SaveFetchCardStatus(s32 cardId) {
#endif
    s32 chan;

    chan = (cardId != 0) * CARD_PORT_STRIDE;
    _card_info(chan);
    switch (SaveWaitSwCardStatus()) {
    case CARD_EVENT_ERROR:
        g_SaveCardSlotStatus[cardId][CARD_STATUS_ERROR] = 1;
        return;
    case CARD_EVENT_SUCCESS:
        if (g_SaveCardSlotStatus[cardId][CARD_STATUS_INSERTED]) {
            return;
        }
        g_SaveCardSlotStatus[cardId][CARD_STATUS_INSERTED] = 1;
        break;
    case CARD_EVENT_TIMEOUT:
        g_SaveCardSlotStatus[cardId][CARD_STATUS_INSERTED] = 0;
        g_SaveCardSlotStatus[cardId][CARD_STATUS_ERROR] = 0;
        g_SaveCardSlotStatus[cardId][CARD_STATUS_UNFORMATTED] = 0;
        return;
    case CARD_EVENT_NEW_CARD:
        SavePeekHwCardStatus();
        _card_clear(chan);
        SaveWaitHwCardStatus();
        break;
    default:
        g_SaveCardSlotStatus[cardId][CARD_STATUS_ERROR] = 1;
        return;
    }

    SavePeekSwCardStatus();
    _card_load(chan);
    switch (SaveWaitSwCardStatus()) {
    case CARD_EVENT_SUCCESS:
        g_SaveCardSlotStatus[cardId][CARD_STATUS_UNFORMATTED] = 0;
        break;
    case CARD_EVENT_TIMEOUT:
        g_SaveCardSlotStatus[cardId][CARD_STATUS_INSERTED] = 0;
        break;
    case CARD_EVENT_ERROR:
        g_SaveCardSlotStatus[cardId][CARD_STATUS_ERROR] = 1;
        break;
    case CARD_EVENT_NEW_CARD:
        g_SaveCardSlotStatus[cardId][CARD_STATUS_UNFORMATTED] = 1;
        break;
    default:
        g_SaveCardSlotStatus[cardId][CARD_STATUS_ERROR] = 1;
        break;
    }
}

void SaveFetchAllCardStatus(s32 retryCounter) {
    if (!(retryCounter & 0x3F)) {
        SaveFetchCardStatus(CARD_SLOT_1);
        SaveFetchCardStatus(CARD_SLOT_2);
    }
}

s32 SaveFetchHeader(s32 cardId, s32 slotId) {
    s32 saveId;
    s32 i;
    s32 ret;

    for (i = 0; i < SAVE_HEADER_MAX_RETRIES; i++) {
        saveId = slotId;
        if (cardId) {
            saveId |= CARD_PORT_STRIDE;
        }
        if (LoadSaveHeader(saveId) == 0) {
            ret = 0;
            break;
        }
        ret = 1;
    }
    return ret;
}

void SaveDrawSlot(s32 x, s32 y, s32 slotIndex) {
    RECT slotRect;
    RECT clipRect;
    s32 i;
    s32 j;
    SaveHeader* save;
    u8* saveBytes;

    save = SaveGetHeader(slotIndex);
    saveBytes = (u8*)save;
    SysMenuDrawString(192, y + 46, save->place_name, 7);
    for (i = 0; i < 3; i++) {
        if (saveBytes[i + 5] != 0xFF) {
            SysMenuDrawAvatar2(22 + i * 52, y + 6, 48, 48, (saveBytes[i + 5] >= 5) ? 48 : 0,
                               (saveBytes[i + 5] % 5) * 48, 48, 48, saveBytes[i + 5], 0);
        }
    }

    clipRect.x = 0;
    clipRect.y = 0;
    clipRect.w = 0xFF;
    clipRect.h = 0xFF;
    SysMenuSetDrawMode(0, 1, 127, &clipRect);
    SysMenuDrawDigitsWithoutLeadingZeroes(
        SysGetSingleStringWidth(g_SaveLevelLabel) + 194, y + 28, save->leader_level, 2, 7);
    SysMenuDrawSingleFontLetter(338, y + 12, 213, 7); // prints the ':' symbol maybe?
    SysMenuDrawDigitsWithLeadingZeroes(324, y + 11, SysGetHoursFromSeconds(save->time), 2, 7);
    SysMenuDrawDigitsWithLeadingZeroes(345, y + 11, SysGetMinutesFromSeconds(save->time), 2, 7);
    SysMenuDrawDigitsWithoutLeadingZeroes(309, y + 25, save->gil, 7, 7);

    clipRect.x = 0;
    clipRect.y = 0;
    clipRect.w = 0x100;
    clipRect.h = 0x100;
    SysMenuSetDrawMode(0, 1, 127, &clipRect);
    SysMenuDrawString(189, y + 26, g_SaveLevelLabel, 5);
    SysMenuDrawString(184, y + 8, save->leader_name, 7);
    SysMenuDrawString(284, y + 9, g_Labels.labels[LABEL_TIME], 7);
    SysMenuDrawString(284, y + 23, g_Labels.labels[LABEL_GIL], 7);
    MENU_SetWindowColors(save->menu_color);

    for (j = 0; j < 3; j++) {
        SysMenuCopyWindowRect(&slotRect, &g_SaveSlotWindowRects[j]);
        SysMenuMoveWindowRect(&slotRect, 0, y);
        SysMenuDrawWindow(&slotRect);
    }
}

static void TitleInit(void) {
    g_TitleResult = TITLE_RESULT_CONTINUE;
    g_MenuStartMode = START_MENU_MODE_TITLE;
    MENU_SetWindowColors(g_TitleDefaultWindowColors);
    MENU_LoadTim(buster_tim, 0x380, 0, 0, 0x1E0);
    DrawSync(0);
    SysMenuSetCursorMovement(&g_TitleButtonTables[TITLE_BTN_TITLE], 0, 1, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
    SysMenuStoreCharacterClutToRam(g_SaveCharClutBackup);
    SysMenuStoreAvatarVram(g_SaveAvatarVramBackup);
    SysMenuStoreFontVram(g_SaveFontVramBackup);
    SysMenuLoadAvatars();
    SaveInitCardEvents();
}

// title screen handler
static s32 TitleUpdate(s32 counter) {
    RECT windowRect;
    RECT clipRect;
    s32 slotIndex;
    s32 fileSlotId;
    s32 rowIdx;
    s32 blue;
    s32 loadingWidth;
    s32 status;
    s32 red;
    s32 promptWidth;
    s32 visibleSlotCount;

    if ((g_MenuStartMode < START_MENU_MODE_CHECKING_FILES || g_MenuStartMode == START_MENU_MODE_TITLE) &&
        g_TitleFadeState != TITLE_FADE_OUT && g_TitleFadeState != TITLE_FADE_IN) {
        SaveFetchAllCardStatus(counter);
    }
    SysMenuDrawNoop(0x80);
    if (g_TitleFadeState == TITLE_FADE_IN) {
        if (TitleDoFade(-TITLE_FADE_STEP) == 0) {
            g_TitleFadeState = TITLE_FADE_ACTIVE;
        }
    } else if (g_TitleFadeState == TITLE_FADE_OUT) {
        if (TitleDoFade(TITLE_FADE_STEP) == 255) {
            g_TitleFadeState = TITLE_FADE_DONE;
        }
    }
    SysMenuIsWindowActive();
    switch (g_MenuStartMode) {
    case START_MENU_MODE_SELECT_SLOT:
        SysMenuDrawCursor(
            g_TitleWindowRect.x - 18, g_TitleWindowRect.y + 6 + g_TitleMenuTables[TITLE_TABLE_SLOTS].row * 12);
        SysMenuDrawString(10, 11, g_SaveMenuStrings[SAVE_STR_SELECT_SLOT], 7);
        SysMenuDrawString(g_TitleWindowRect.x + 12, g_TitleWindowRect.y + 5, g_SaveMenuStrings[SAVE_STR_SLOT_1],
                          -(g_SaveCardSlotStatus[CARD_SLOT_1][CARD_STATUS_INSERTED] != 0) & 7);
        SysMenuDrawString(g_TitleWindowRect.x + 12, g_TitleWindowRect.y + 0x11, g_SaveMenuStrings[SAVE_STR_SLOT_2],
                          -(g_SaveCardSlotStatus[CARD_SLOT_2][CARD_STATUS_INSERTED] != 0) & 7);
        clipRect.x = 0;
        clipRect.y = 0;
        clipRect.w = 0x100;
        clipRect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &clipRect);
        SysMenuDrawWindow(&g_TitleWindowRect);
        break;
    case START_MENU_MODE_SELECT_FILE:
        if (!g_SaveCardSlotStatus[g_TitleMenuTables[TITLE_TABLE_SLOTS].row][CARD_STATUS_INSERTED]) {
            g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
        } else {
            SysMenuSavePoly();
            SysMenuSetPoly(g_TitleBufferIndex * 0x5000 + buster_tim);
            SysMenuDrawCursor(8, (g_TitleMenuTables[TITLE_TABLE_FILES].row * 64) | 0x38);
            visibleSlotCount = !g_TitleMenuTables[TITLE_TABLE_FILES].scrolling ? 3 : 4;
            for (rowIdx = 0; rowIdx < visibleSlotCount; rowIdx++) {
                if ((g_SaveSlotMask >> (rowIdx + g_TitleMenuTables[TITLE_TABLE_FILES].rowOffset)) & 1) {
                    SysMenuStoreWindowColor();
                    SaveDrawSlot(0, rowIdx * 64 + 0x1D + g_TitleMenuTables[TITLE_TABLE_FILES].scrollAnimY * 8,
                                 rowIdx + g_TitleMenuTables[TITLE_TABLE_FILES].rowOffset);
                    SysMenuRestoreWindowColor();
                } else {
                    SysMenuDrawString(0x32, rowIdx * 64 + 55 + g_TitleMenuTables[TITLE_TABLE_FILES].scrollAnimY * 8,
                                      g_SaveMenuStrings[SAVE_STR_EMPTY], 6);
                    SysMenuCopyWindowRect(&windowRect, &g_SaveSlotWindowRects[2]);
                    SysMenuMoveWindowRect(
                        &windowRect, 0, rowIdx * 64 + 0x1D + g_TitleMenuTables[TITLE_TABLE_FILES].scrollAnimY * 8);
                    SysMenuDrawWindow(&windowRect);
                }
            }
            SysMenuDrawNoop(0x80);
            clipRect.y = 0x1D;
            clipRect.w = 0x16C;
            clipRect.h = 0xC3;
            clipRect.x = 0;
            SysMenuSetDrawenv(&g_TitleDrawEnv[g_TitleBufferIndex], &clipRect);
            SysMenuDrawString(10, 11, g_SaveMenuStrings[SAVE_STR_SELECT_FILE], 7);
            SysMenuDrawString(0xCE, 11, g_SaveMenuStrings[SAVE_STR_FILE], 6);
            SysMenuDrawString(
                SysGetSingleStringWidth(g_SaveMenuStrings[SAVE_STR_FILE]) + 0xD0, 11,
                ((13 + g_TitleMenuTables[TITLE_TABLE_FILES].row + g_TitleMenuTables[TITLE_TABLE_FILES].rowOffset) *
                 36) +
                    (g_SaveMenuStrings[SAVE_STR_LOAD]),
                7);
            SysMenuSetWindowRect(&windowRect, 200, 5, 0x4E, 0x18);
            SysMenuDrawWindow(&windowRect);
            SysMenuRestorePoly();
        }
        break;
    case START_MENU_MODE_CHECKING_FILES:
    case START_MENU_MODE_CHECKING_WAIT:
        if (g_MenuStartMode == START_MENU_MODE_CHECKING_FILES) {
            red = 0x40;
            status = 0x20;
            blue = 0xA0;
        } else {
            red = 0xE0;
            status = 0x80;
            blue = 0;
        }
        SysMenuDrawString(10, 11, g_SaveMenuStrings[SAVE_STR_CHECKING_CARD], 7);
        if (g_TitleScanInitial == 0) {
            SysMenuDrawProgressBar(122, 117, (g_TitleScanFileIndex + 1) * 8, 8, red, status, blue);
            clipRect.x = 0;
            clipRect.y = 0;
            clipRect.w = 0xFF;
            clipRect.h = 0xFF;
            SysMenuSetDrawMode(0, 1, 0x3F, &clipRect);
        }
        SysMenuSetWindowRect(&windowRect, 112, 0x6D, 0x8C, 0x18);
        SysMenuDrawWindow(&windowRect);
        break;
    case START_MENU_MODE_LOADING:
        if (g_TitleFadeState != TITLE_FADE_OUT) {
            loadingWidth = SysGetSingleStringWidth(g_SaveMenuStrings[SAVE_STR_LOADING]) + 0x10;
            SysMenuDrawString(190 - loadingWidth / 2, 0x73, g_SaveMenuStrings[SAVE_STR_LOADING], 7);
            SysMenuSetWindowRect(&windowRect, 0xB6 - loadingWidth / 2, 0x6D, loadingWidth, 24);
            SysMenuDrawWindow(&windowRect);
        }
        break;
    case START_MENU_MODE_FORMAT_PROMPT:
        if (counter & 2) {
            SysMenuDrawCursor(
                g_TitleWindowRect.x - 0x12, g_TitleWindowRect.y + 6 + g_TitleMenuTables[TITLE_TABLE_SLOTS].row * 12);
        }
        SysMenuDrawString(g_TitleWindowRect.x + 12, g_TitleWindowRect.y + 5, g_SaveMenuStrings[SAVE_STR_SLOT_1],
                          -(g_SaveCardSlotStatus[CARD_SLOT_1][CARD_STATUS_INSERTED] != 0) & 7);
        SysMenuDrawString(g_TitleWindowRect.x + 12, g_TitleWindowRect.y + 0x11, g_SaveMenuStrings[SAVE_STR_SLOT_2],
                          -(g_SaveCardSlotStatus[CARD_SLOT_2][CARD_STATUS_INSERTED] != 0) & 7);
        clipRect.x = 0;
        clipRect.y = 0;
        clipRect.w = 0x100;
        clipRect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &clipRect);
        SysMenuDrawWindow(&g_TitleWindowRect);
        SysMenuDrawString(10, 11, g_SaveFormatStrings[SAVE_FORMAT_STR_UNFORMATTED], 7);
        promptWidth = SysGetSingleStringWidth(g_SaveFormatStrings[SAVE_FORMAT_STR_PROMPT]) + 0x10;
        SysMenuDrawString(
            190 - promptWidth / 2, g_TitleWindowRect.h + 99, g_SaveFormatStrings[SAVE_FORMAT_STR_PROMPT], 7);
        SysMenuDrawString(228 - promptWidth / 2, g_TitleWindowRect.h + 112, g_SaveMenuStrings[SAVE_STR_YES], 7);
        SysMenuDrawString(228 - promptWidth / 2, g_TitleWindowRect.h + 124, g_SaveMenuStrings[SAVE_STR_NO], 7);
        SysMenuDrawCursor(
            200 - promptWidth / 2, 0x73 + g_TitleButtonTables[TITLE_BTN_FORMAT].row * 12 + g_TitleWindowRect.h);
        SysMenuSetWindowRect(&windowRect, 0xB6 - promptWidth / 2, g_TitleWindowRect.h + 0x5D, promptWidth, 0x30);
        SysMenuDrawWindow(&windowRect);
        break;
    case START_MENU_MODE_TITLE:
        SysMenuDrawCursor(
            g_TitleWindowRect.x - 0x12, g_TitleWindowRect.y + 6 + g_TitleButtonTables[TITLE_BTN_TITLE].row * 12);
        SysMenuDrawString(
            g_TitleWindowRect.x + 8, g_TitleWindowRect.y + 6, g_SaveMenuStrings[SAVE_STR_NEW_GAME], 7); // new game
        SysMenuDrawString(g_TitleWindowRect.x + 8, g_TitleWindowRect.y + 18, g_SaveMenuStrings[SAVE_STR_CONTINUE],
                          g_SaveCardSlotStatus[CARD_SLOT_1][CARD_STATUS_INSERTED] ||
                                  g_SaveCardSlotStatus[CARD_SLOT_2][CARD_STATUS_INSERTED]
                              ? 7
                              : 0); // continue?
        clipRect.x = 0;
        clipRect.y = 0;
        clipRect.w = 0x100;
        clipRect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &clipRect);
        SetPolyFT4(g_GpuPacketPtr.ft4);
        g_GpuPacketPtr.ft4->r0 = 0x60;
        g_GpuPacketPtr.ft4->g0 = 0x60;
        g_GpuPacketPtr.ft4->b0 = 0x60;
        g_GpuPacketPtr.ft4->x0 = 0x6F;
        g_GpuPacketPtr.ft4->y0 = 0x54;
        g_GpuPacketPtr.ft4->x1 = 0x119;
        g_GpuPacketPtr.ft4->y1 = 0x54;
        g_GpuPacketPtr.ft4->x2 = 0x6F;
        g_GpuPacketPtr.ft4->y2 = 0xEA;
        g_GpuPacketPtr.ft4->x3 = 0x119;
        g_GpuPacketPtr.ft4->y3 = 0xEA;
        g_GpuPacketPtr.ft4->u0 = 0;
        g_GpuPacketPtr.ft4->v0 = 0;
        g_GpuPacketPtr.ft4->u1 = 0x96;
        g_GpuPacketPtr.ft4->v1 = 0;
        g_GpuPacketPtr.ft4->u2 = 0;
        g_GpuPacketPtr.ft4->v2 = 0x96;
        g_GpuPacketPtr.ft4->u3 = 0x96;
        g_GpuPacketPtr.ft4->v3 = 0x96;
        g_GpuPacketPtr.ft4->clut = GetClut(0, 0x1E0);
        g_GpuPacketPtr.ft4->tpage = GetTPage(1, 0, 0x380, 0);
        AddPrim(g_CurrentOT, g_GpuPacketPtr.ft4);
        g_GpuPacketPtr.ft4++;
        break;
    }
    if (g_MenuStartMode != START_MENU_MODE_TITLE) {
        SysMenuDrawString(0x126, 11, g_SaveMenuStrings[SAVE_STR_LOAD], 7);
        SysMenuSetWindowRect(&windowRect, 0x116, 5, 0x56, 0x18);
        SysMenuDrawWindow(&windowRect);
        SysMenuSetWindowRect(&windowRect, 0, 5, 0x16C, 0x18);
        SysMenuDrawWindow(&windowRect);
    }
    if (!(SysMenuIsWindowActive() & 0xFF) && g_TitleFadeState == TITLE_FADE_ACTIVE) {
        switch (g_MenuStartMode) {
        case START_MENU_MODE_SELECT_SLOT:
            if (g_Pad1KeysPressed & PADRright) {
                slotIndex = g_TitleMenuTables[TITLE_TABLE_SLOTS].row;
                if (slotIndex >= NUM_CARD_SLOTS) {
                    break;
                }
                if (slotIndex < 0) {
                    break;
                }
                if (g_SaveCardSlotStatus[slotIndex][CARD_STATUS_INSERTED]) {
                    TitlePlaySfx(SFX_MENU_CURSOR_MOVE);
                    if (g_SaveCardSlotStatus[g_TitleMenuTables[TITLE_TABLE_SLOTS].row][CARD_STATUS_UNFORMATTED]) {
                        g_MenuStartMode = START_MENU_MODE_FORMAT_PROMPT;
                        SysMenuSetCursorMovement(
                            &g_TitleMenuTables[TITLE_TABLE_FORMAT], 0, 1, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
                    } else {
                        g_TitleTimer = TITLE_TRANSITION_DELAY;
                        g_MenuStartMode = START_MENU_MODE_CHECKING_FILES;
                        g_TitleScanFileIndex = 0;
                        g_TitleScanUnk = 0;
                        g_SaveSlotMask = 0;
                        g_TitleScanInitial = 1;
                        SysMenuSetCursorMovement(&g_TitleMenuTables[TITLE_TABLE_FILES], 0, 0, 1, 3, 0, 0, 1,
                                                 NUM_SAVE_FILES_PER_CARD, 0, 0, 0, 0, 0);
                    }
                } else {
                    TitlePlaySfx(SFX_MENU_BAD);
                    SysMenuRequestAddWindow(g_SaveErrorStrings, 7);
                }
            } else if (g_Pad1KeysPressed & PADRdown) {
                TitlePlaySfx(SFX_MENU_BACK);
                g_MenuStartMode = START_MENU_MODE_TITLE;
            } else {
                SysMenuHandleButtons(&g_TitleMenuTables[TITLE_TABLE_SLOTS]);
            }
            break;
        case START_MENU_MODE_SELECT_FILE:
            status = g_TitleMenuTables[TITLE_TABLE_FILES].scrollAnimY;
            SaveHandleScrollCursor(&g_TitleMenuTables[TITLE_TABLE_FILES]);
            if (!g_TitleMenuTables[TITLE_TABLE_FILES].scrollAnimY && !status) {
                if (g_Pad1KeysPressed & PADRright) {
                    if (((s32)g_SaveSlotMask >>
                         (g_TitleMenuTables[TITLE_TABLE_FILES].row + g_TitleMenuTables[TITLE_TABLE_FILES].rowOffset)) &
                        1) {
                        TitlePlaySfx(SFX_MENU_CURSOR_MOVE);
                        g_MenuStartMode = START_MENU_MODE_LOADING;
                        g_TitleTimer = TITLE_TRANSITION_DELAY;
                    } else {
                        TitlePlaySfx(SFX_MENU_BAD);
                    }
                } else if (g_Pad1KeysPressed & PADRdown) {
                    TitlePlaySfx(SFX_MENU_BACK);
                    g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                }
            }
            break;
        case START_MENU_MODE_CHECKING_FILES:
            if (g_TitleTimer == 0) {
                if (g_TitleScanInitial) {
                    g_TitleTimer = 0;
                    g_TitleScanInitial = 0;
                    g_SaveSlotMask = GetSaveSlotMask(g_TitleMenuTables[TITLE_TABLE_SLOTS].row);
                } else {
                    status = 0;
                    if ((g_SaveSlotMask >> g_TitleScanFileIndex) & 1) {
                        status = SaveFetchHeader(g_TitleMenuTables[TITLE_TABLE_SLOTS].row, g_TitleScanFileIndex);
                    }
                    g_TitleScanFileIndex++;
                    if (status) {
                        g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                        SysMenuRequestAddWindow(g_SaveErrorStrings[8], 2);
                        TitlePlaySfx(SFX_MENU_BAD);
                    }
                    if (g_TitleScanFileIndex == NUM_SAVE_FILES_PER_CARD) {
                        g_TitleScanFileIndex = NUM_SAVE_FILES_PER_CARD - 1;
                        g_MenuStartMode = START_MENU_MODE_CHECKING_WAIT;
                        g_TitleTimer = TITLE_TRANSITION_DELAY;
                        TitlePlaySfx(SFX_MENU_CONFIRMED);
                    }
                }
            } else {
                g_TitleTimer--;
            }
            break;
        case START_MENU_MODE_CHECKING_WAIT:
            if (g_TitleTimer == 0) {
                g_MenuStartMode = START_MENU_MODE_SELECT_FILE;
            }
            g_TitleTimer--;
            break;
        case START_MENU_MODE_LOADING:
            if (g_TitleTimer != 0) {
                g_TitleTimer--;
                break;
            }
            g_SavemapBusy = 1;
            fileSlotId = g_TitleMenuTables[TITLE_TABLE_FILES].row + g_TitleMenuTables[TITLE_TABLE_FILES].rowOffset;
            if (g_TitleMenuTables[TITLE_TABLE_SLOTS].row) {
                fileSlotId |= CARD_PORT_STRIDE;
            }
            status = (s16)LoadSaveFile(fileSlotId);
            if (status == 0) {
                if (Savemap.header.checksum !=
                    (u16)SaveCalcChecksum(sizeof(SaveWork) - 4, &Savemap.header.leader_level)) {
                    g_MenuStartMode = START_MENU_MODE_SELECT_FILE;
                    TitlePlaySfx(SFX_MENU_BAD);
                    SysMenuRequestAddWindow(g_SaveMenuStrings[SAVE_STR_FILE_RUINED], 0);
                } else {
                    TitlePlaySfx(SFX_MEMCARD_LOADED);
                    g_TitleFadeState = TITLE_FADE_OUT;
                    TitleApplySoundMode(Savemap.config & 3);
                }
            } else {
                g_MenuStartMode = START_MENU_MODE_SELECT_FILE;
                TitlePlaySfx(SFX_MENU_BAD);
                SysMenuRequestAddWindow(g_SaveMenuStrings[SAVE_STR_COULD_NOT_LOAD], status);
            }
            g_SavemapBusy = 0;
            break;
        case START_MENU_MODE_FORMAT_PROMPT:
            SysMenuHandleButtons(&g_TitleButtonTables[TITLE_BTN_FORMAT]);
            if (g_Pad1KeysPressed & PADRright) {
                if (g_TitleButtonTables[TITLE_BTN_FORMAT].row == FORMAT_OPTION_YES) {
                    if (g_TitleMenuTables[TITLE_TABLE_SLOTS].row) {
                        slotIndex = format("bu10:");
                    } else {
                        slotIndex = format("bu00:");
                    }
                    g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                    if (slotIndex == 1) {
                        g_SaveCardSlotStatus[g_TitleMenuTables[TITLE_TABLE_SLOTS].row][CARD_STATUS_UNFORMATTED] = 0;
                        SysMenuRequestAddWindow(g_SaveMenuStrings[SAVE_STR_COMPLETED], 7);
                        TitlePlaySfx(SFX_MEMCARD_LOADED);
                    } else {
                        SysMenuRequestAddWindow(g_SaveFormatStrings[SAVE_FORMAT_STR_FAILED], 7);
                        TitlePlaySfx(SFX_MENU_BAD);
                    }
                } else {
                    g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                    TitlePlaySfx(SFX_MENU_BACK);
                }
            } else if (g_Pad1KeysPressed & PADRdown) {
                g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                TitlePlaySfx(SFX_MENU_BACK);
            }
            break;
        case START_MENU_MODE_TITLE:
            if (g_Pad1KeysPressed & PADRright) {
                switch (g_TitleMenuTables[TITLE_TABLE_TITLE].row) {
                case TITLE_OPTION_NEW_GAME:
                    TitlePlaySfx(SFX_MEMCARD_LOADED);
                    g_TitleResult = TITLE_RESULT_NEW_GAME;
                    g_TitleFadeState = TITLE_FADE_OUT;
                    break;
                case TITLE_OPTION_CONTINUE:
                    if (g_SaveCardSlotStatus[CARD_SLOT_1][CARD_STATUS_INSERTED] ||
                        g_SaveCardSlotStatus[CARD_SLOT_2][CARD_STATUS_INSERTED]) {
                        TitlePlaySfx(SFX_MENU_CURSOR_MOVE);
                        SysMenuSetCursorMovement(
                            &g_TitleMenuTables[TITLE_TABLE_SLOTS], 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
                        g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                    } else {
                        TitlePlaySfx(SFX_MENU_BAD);
                    }
                    break;
                }
            } else {
                SysMenuHandleButtons(&g_TitleButtonTables[TITLE_BTN_TITLE]);
            }
            break;
        }
    }
    return g_TitleResult;
}

static void TitleCleanup(void) {
    SysMenuLoadPartyPortraits();
    SysMenuStoreCharacterClutToRam(g_PartyPortraitClut);
    SysMenuLoadCharacterClutFromRam(g_SaveCharClutBackup);
    SysMenuRestoreAvatarVram(g_SaveAvatarVramBackup);
    SysMenuRestoreFontVram(g_SaveFontVramBackup);
    SaveCleanupCardEvents();
    PutDispEnv(g_TitleDispEnv);
    PutDrawEnv(g_TitleDrawEnv);
}

s32 TitleMain(void) {
    s32 i;
    s32 ret;

    SysMenuCreateDrawenvDispenv(g_TitleDrawEnv, g_TitleDispEnv);
    g_TitleFadeState = TITLE_FADE_IN;
    TitleInit();
    g_TitleBufferIndex = 0;
    for (i = 0;; i++) {
        InputUpdateKeyStates();
        SysMenuSetPoly(D_80077F64[g_TitleBufferIndex]);
        g_TitleActiveOT = (u_long*)g_TitleOrderingTable[g_TitleBufferIndex];
        ClearOTag(g_TitleActiveOT, 1);
        SysMenuSetOtag(g_TitleActiveOT);
        SysMenuDrawAddWindow();
        ret = TitleUpdate(i);
        if (g_TitleFadeState == TITLE_FADE_DONE) {
            break;
        }
        DrawSync(0);
        VSync(0);
        PutDispEnv(&g_TitleDispEnv[g_TitleBufferIndex]);
        PutDrawEnv(&g_TitleDrawEnv[g_TitleBufferIndex]);
        DrawOTag(g_TitleActiveOT);
        g_TitleBufferIndex ^= 1; // flip back buffer ID?
    }
    TitleCleanup();
    VSync(0);
    PutDispEnv(&g_TitleDispEnv[1]);
    PutDrawEnv(&g_TitleDrawEnv[1]);
    for (i = 0; i < 3; i++) {
        if (Savemap.partyID[i] != 0xFF) {
            SysInitPlayerStatFromEquip(i);
            SysInitPlayerStatFromMateria((u8)i);
        }
    }
    SysCalcTotalLureGilPreempVal();
    return ret;
}
