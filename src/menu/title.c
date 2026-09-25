//! PSYQ=3.3 CC1=2.7.2
#include <libapi.h>
#include "savemenu.h"
#include <libetc.h>

extern s32 D_801E2CF4;

static void TitlePlaySfx(u16 soundId) {
    g_AkaoCmd.opcode = AKAO_PLAY_MENU_SOUND;
    g_AkaoCmd.params[0] = soundId;
    g_AkaoCmd.params[1] = soundId;
    AkaoExec();
}

static s32 TitleDoFade(s32 fadeDirection) {
    RECT rect;

    setTile(D_80062F24.tile);
    SetSemiTrans(D_80062F24.tile, 1);
    D_80062F24.tile->x0 = 0;
    D_80062F24.tile->y0 = 0;
    D_80062F24.tile->w = 384;
    D_80062F24.tile->h = 232;
    setRGB0(D_80062F24.tile, D_801E2CF4, D_801E2CF4, D_801E2CF4);
    AddPrim(D_80062FC4, D_80062F24.tile++);
    setRECT(&rect, 0, 0, 255, 255);
    SysMenuSetDrawMode(0, 1, 0x1F | 0x40, &rect);
    D_801E2CF4 += fadeDirection;
    if (D_801E2CF4 < 0) {
        D_801E2CF4 = 0;
    }
    if (D_801E2CF4 > 255) {
        D_801E2CF4 = 255;
    }
    return D_801E2CF4;
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
    TestEvent(D_8009A024[0]);
    TestEvent(D_8009A024[1]);
    TestEvent(D_8009A024[2]);
    TestEvent(D_8009A024[3]);
}

static void SavePeekHwCardStatus(void) {
    TestEvent(D_8009A024[4]);
    TestEvent(D_8009A024[5]);
    TestEvent(D_8009A024[6]);
    TestEvent(D_8009A024[7]);
}

static s32 SaveWaitSwCardStatus(void) {
    while (1) {
        if (TestEvent(D_8009A024[0]) == 1) {
            return 0;
        }
        if (TestEvent(D_8009A024[1]) == 1) {
            return 1;
        }
        if (TestEvent(D_8009A024[2]) == 1) {
            return 2;
        }
        if (TestEvent(D_8009A024[3]) == 1) {
            return 3;
        }
    }
}

static s32 SaveWaitHwCardStatus(void) {
    while (1) {
        if (TestEvent(D_8009A024[4]) == 1) {
            return 0;
        }
        if (TestEvent(D_8009A024[5]) == 1) {
            return 1;
        }
        if (TestEvent(D_8009A024[6]) == 1) {
            return 2;
        }
        if (TestEvent(D_8009A024[7]) == 1) {
            return 3;
        }
    }
}

#ifdef VERSION_PC
static void SaveFetchCardStatus(s32 cardId) {
#else
static s32 SaveFetchCardStatus(s32 cardId) {
#endif
    s32 chan;

    chan = (cardId != 0) * 0x10;
    _card_info(chan);
    switch (SaveWaitSwCardStatus()) {
    case 1:
        D_801E8F38[cardId][1] = 1;
        return;
    case 0:
        if (D_801E8F38[cardId][0]) {
            return;
        }
        D_801E8F38[cardId][0] = 1;
        break;
    case 2:
        D_801E8F38[cardId][0] = 0;
        D_801E8F38[cardId][1] = 0;
        D_801E8F38[cardId][2] = 0;
        return;
    case 3:
        SavePeekHwCardStatus();
        _card_clear(chan);
        SaveWaitHwCardStatus();
        break;
    default:
        D_801E8F38[cardId][1] = 1;
        return;
    }

    SavePeekSwCardStatus();
    _card_load(chan);
    switch (SaveWaitSwCardStatus()) {
    case 0:
        D_801E8F38[cardId][2] = 0;
        break;
    case 2:
        D_801E8F38[cardId][0] = 0;
        break;
    case 1:
        D_801E8F38[cardId][1] = 1;
        break;
    case 3:
        D_801E8F38[cardId][2] = 1;
        break;
    default:
        D_801E8F38[cardId][1] = 1;
        break;
    }
}

void SaveFetchAllCardStatus(s32 retryCounter) {
    if (!(retryCounter & 0x3F)) {
        SaveFetchCardStatus(0);
        SaveFetchCardStatus(1);
    }
}

s32 SaveFetchHeader(s32 cardId, s32 slotId) {
    s32 saveId;
    s32 i;
    s32 ret;

    for (i = 0; i < 20; i++) {
        saveId = slotId;
        if (cardId) {
            saveId |= 0x10;
        }
        if (LoadSaveHeader(saveId) == 0) {
            ret = 0;
            break;
        }
        ret = 1;
    }
    return ret;
}

void SaveDrawSlot(s32 x, s32 y, s32 slot_no) {
    RECT sp28;
    RECT rect;
    s32 i;
    s32 j;
    SaveHeader* save;
    u8* data;

    save = SaveGetHeader(slot_no);
    data = (u8*)save;
    SysMenuDrawString(192, y + 46, save->place_name, 7);
    for (i = 0; i < 3; i++) {
        if (data[i + 5] != 0xFF) {
            SysMenuDrawAvatar2(22 + i * 52, y + 6, 48, 48, (data[i + 5] >= 5) ? 48 : 0, (data[i + 5] % 5) * 48, 48, 48,
                               data[i + 5], 0);
        }
    }

    rect.x = 0;
    rect.y = 0;
    rect.w = 0xFF;
    rect.h = 0xFF;
    SysMenuSetDrawMode(0, 1, 127, &rect);
    SysMenuDrawDigitsWithoutLeadingZeroes(SysGetSingleStringWidth(D_801E3684) + 194, y + 28, save->leader_level, 2, 7);
    SysMenuDrawSingleFontLetter(338, y + 12, 213, 7); // prints the ':' symbol maybe?
    SysMenuDrawDigitsWithLeadingZeroes(324, y + 11, SysGetHoursFromSeconds(save->time), 2, 7);
    SysMenuDrawDigitsWithLeadingZeroes(345, y + 11, SysGetMinutesFromSeconds(save->time), 2, 7);
    SysMenuDrawDigitsWithoutLeadingZeroes(309, y + 25, save->gil, 7, 7);

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 0x100;
    SysMenuSetDrawMode(0, 1, 127, &rect);
    SysMenuDrawString(189, y + 26, D_801E3684, 5);
    SysMenuDrawString(184, y + 8, save->leader_name, 7);
    SysMenuDrawString(284, y + 9, g_Labels.labels[LABEL_TIME], 7);
    SysMenuDrawString(284, y + 23, g_Labels.labels[LABEL_GIL], 7);
    MENU_SetWindowColors(save->menu_color);

    for (j = 0; j < 3; j++) {
        SysMenuCopyWindowRect(&sp28, &D_801E3650[j]);
        SysMenuMoveWindowRect(&sp28, 0, y);
        SysMenuDrawWindow(&sp28);
    }
}

static void TitleInit(void) {
    D_801E3698 = 0;
    g_MenuStartMode = START_MENU_MODE_TITLE;
    MENU_SetWindowColors(D_801E368C);
    MENU_LoadTim(buster_tim, 0x380, 0, 0, 0x1E0);
    DrawSync(0);
    SysMenuSetCursorMovement(&D_801E3DEC[1], 0, 1, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
    SysMenuStoreCharacterClutToRam(D_801E3F2C);
    SysMenuStoreAvatarVram(D_801E8F44);
    SysMenuStoreFontVram(D_801E4538);
    SysMenuLoadAvatars();
    SaveInitCardEvents();
}

// title screen handler
static s32 TitleUpdate(s32 counter) {
    RECT sp38;
    RECT rect;
    s32 temp_v1_2;
    s32 var_a0_3;
    s32 var_s0;
    s32 var_s0_2;
    s32 temp_s1;
    s32 var_s1;
    s32 var_s2;
    s32 temp_s2;
    s32 var_s3;

    if ((g_MenuStartMode < START_MENU_MODE_CHECKING_FILES || g_MenuStartMode == START_MENU_MODE_TITLE) &&
        D_801E3D54 != 2 && D_801E3D54 != 0) {
        SaveFetchAllCardStatus(counter);
    }
    SysMenuDrawNoop(0x80);
    if (D_801E3D54 == 0) {
        if (TitleDoFade(-15) == 0) {
            D_801E3D54 = 1;
        }
    } else if (D_801E3D54 == 2) {
        if (TitleDoFade(15) == 255) {
            D_801E3D54 = -1;
        }
    }
    SysMenuIsWindowActive();
    switch (g_MenuStartMode) {
    case START_MENU_MODE_SELECT_SLOT:
        SysMenuDrawCursor(D_801E3668.x - 18, D_801E3668.y + 6 + D_801E3D80[0].row * 12);
        SysMenuDrawString(10, 11, D_801E2CFC[1], 7);
        SysMenuDrawString(D_801E3668.x + 12, D_801E3668.y + 5, D_801E2CFC[3], -(D_801E8F38[0][0] != 0) & 7);
        SysMenuDrawString(D_801E3668.x + 12, D_801E3668.y + 0x11, D_801E2CFC[4], -(D_801E8F38[1][0] != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SysMenuDrawWindow(&D_801E3668);
        break;
    case START_MENU_MODE_SELECT_FILE:
        if (!D_801E8F38[D_801E3D80[0].row][0]) {
            g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
        } else {
            SysMenuSavePoly();
            SysMenuSetPoly(D_801E3D58 * 0x5000 + buster_tim);
            SysMenuDrawCursor(8, (D_801E3D80[1].row * 64) | 0x38);
            var_s3 = !D_801E3D80[1].scrolling ? 3 : 4;
            for (var_s0 = 0; var_s0 < var_s3; var_s0++) {
                if ((D_80062F3C >> (var_s0 + D_801E3D80[1].rowOffset)) & 1) {
                    SysMenuStoreWindowColor();
                    SaveDrawSlot(0, var_s0 * 64 + 0x1D + D_801E3D80[1].unkF * 8, var_s0 + D_801E3D80[1].rowOffset);
                    SysMenuRestoreWindowColor();
                } else {
                    SysMenuDrawString(0x32, var_s0 * 64 + 55 + D_801E3D80[1].unkF * 8, D_801E2CFC[8], 6);
                    SysMenuCopyWindowRect(&sp38, &D_801E3650[2]);
                    SysMenuMoveWindowRect(&sp38, 0, var_s0 * 64 + 0x1D + D_801E3D80[1].unkF * 8);
                    SysMenuDrawWindow(&sp38);
                }
            }
            SysMenuDrawNoop(0x80);
            rect.y = 0x1D;
            rect.w = 0x16C;
            rect.h = 0xC3;
            rect.x = 0;
            SysMenuSetDrawenv(&D_801E3E34[D_801E3D58], &rect);
            SysMenuDrawString(10, 11, D_801E2CFC[2], 7);
            SysMenuDrawString(0xCE, 11, D_801E2CFC[9], 6);
            SysMenuDrawString(SysGetSingleStringWidth(D_801E2CFC[9]) + 0xD0, 11,
                              ((13 + D_801E3D80[1].row + D_801E3D80[1].rowOffset) * 36) + (D_801E2CFC[0]), 7);
            SysMenuSetWindowRect(&sp38, 200, 5, 0x4E, 0x18);
            SysMenuDrawWindow(&sp38);
            SysMenuRestorePoly();
        }
        break;
    case START_MENU_MODE_CHECKING_FILES:
    case START_MENU_MODE_CHECKING_WAIT:
        if (g_MenuStartMode == START_MENU_MODE_CHECKING_FILES) {
            var_s2 = 0x40;
            var_s1 = 0x20;
            var_s0_2 = 0xA0;
        } else {
            var_s2 = 0xE0;
            var_s1 = 0x80;
            var_s0_2 = 0;
        }
        SysMenuDrawString(10, 11, D_801E2CFC[12], 7);
        if (D_801E3F1C == 0) {
            SysMenuDrawProgressBar(122, 117, (D_801E3F20 + 1) * 8, 8, var_s2, var_s1, var_s0_2);
            rect.x = 0;
            rect.y = 0;
            rect.w = 0xFF;
            rect.h = 0xFF;
            SysMenuSetDrawMode(0, 1, 0x3F, &rect);
        }
        SysMenuSetWindowRect(&sp38, 112, 0x6D, 0x8C, 0x18);
        SysMenuDrawWindow(&sp38);
        break;
    case START_MENU_MODE_LOADING:
        if (D_801E3D54 != 2) {
            temp_s1 = SysGetSingleStringWidth(D_801E2CFC[6]) + 0x10;
            SysMenuDrawString(190 - temp_s1 / 2, 0x73, D_801E2CFC[6], 7);
            SysMenuSetWindowRect(&sp38, 0xB6 - temp_s1 / 2, 0x6D, temp_s1, 24);
            SysMenuDrawWindow(&sp38);
        }
        break;
    case START_MENU_MODE_FORMAT_PROMPT:
        if (counter & 2) {
            SysMenuDrawCursor(D_801E3668.x - 0x12, D_801E3668.y + 6 + D_801E3D80[0].row * 12);
        }
        SysMenuDrawString(D_801E3668.x + 12, D_801E3668.y + 5, D_801E2CFC[3], -(D_801E8F38[0][0] != 0) & 7);
        SysMenuDrawString(D_801E3668.x + 12, D_801E3668.y + 0x11, D_801E2CFC[4], -(D_801E8F38[1][0] != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SysMenuDrawWindow(&D_801E3668);
        SysMenuDrawString(10, 11, D_801E3260[4], 7);
        temp_s2 = SysGetSingleStringWidth(D_801E3260[5]) + 0x10;
        SysMenuDrawString(190 - temp_s2 / 2, D_801E3668.h + 99, D_801E3260[5], 7);
        SysMenuDrawString(228 - temp_s2 / 2, D_801E3668.h + 112, D_801E2CFC[34], 7);
        SysMenuDrawString(228 - temp_s2 / 2, D_801E3668.h + 124, D_801E2CFC[35], 7);
        SysMenuDrawCursor(200 - temp_s2 / 2, 0x73 + D_801E3DEC[0].row * 12 + D_801E3668.h);
        SysMenuSetWindowRect(&sp38, 0xB6 - temp_s2 / 2, D_801E3668.h + 0x5D, temp_s2, 0x30);
        SysMenuDrawWindow(&sp38);
        break;
    case START_MENU_MODE_TITLE:
        SysMenuDrawCursor(D_801E3668.x - 0x12, D_801E3668.y + 6 + D_801E3DEC[1].row * 12);
        SysMenuDrawString(D_801E3668.x + 8, D_801E3668.y + 6, D_801E2CFC[32], 7); // new game
        SysMenuDrawString(D_801E3668.x + 8, D_801E3668.y + 18, D_801E2CFC[10],
                          D_801E8F38[0][0] || D_801E8F38[1][0] ? 7 : 0); // continue?
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SetPolyFT4(D_80062F24.ft4);
        D_80062F24.ft4->r0 = 0x60;
        D_80062F24.ft4->g0 = 0x60;
        D_80062F24.ft4->b0 = 0x60;
        D_80062F24.ft4->x0 = 0x6F;
        D_80062F24.ft4->y0 = 0x54;
        D_80062F24.ft4->x1 = 0x119;
        D_80062F24.ft4->y1 = 0x54;
        D_80062F24.ft4->x2 = 0x6F;
        D_80062F24.ft4->y2 = 0xEA;
        D_80062F24.ft4->x3 = 0x119;
        D_80062F24.ft4->y3 = 0xEA;
        D_80062F24.ft4->u0 = 0;
        D_80062F24.ft4->v0 = 0;
        D_80062F24.ft4->u1 = 0x96;
        D_80062F24.ft4->v1 = 0;
        D_80062F24.ft4->u2 = 0;
        D_80062F24.ft4->v2 = 0x96;
        D_80062F24.ft4->u3 = 0x96;
        D_80062F24.ft4->v3 = 0x96;
        D_80062F24.ft4->clut = GetClut(0, 0x1E0);
        D_80062F24.ft4->tpage = GetTPage(1, 0, 0x380, 0);
        AddPrim(D_80062FC4, D_80062F24.ft4);
        D_80062F24.ft4++;
        break;
    }
    if (g_MenuStartMode != START_MENU_MODE_TITLE) {
        SysMenuDrawString(0x126, 11, D_801E2CFC[0], 7);
        SysMenuSetWindowRect(&sp38, 0x116, 5, 0x56, 0x18);
        SysMenuDrawWindow(&sp38);
        SysMenuSetWindowRect(&sp38, 0, 5, 0x16C, 0x18);
        SysMenuDrawWindow(&sp38);
    }
    if (!(SysMenuIsWindowActive() & 0xFF) && D_801E3D54 == 1) {
        switch (g_MenuStartMode) {
        case START_MENU_MODE_SELECT_SLOT:
            if (g_Pad1KeysPressed & PADRright) {
                temp_v1_2 = D_801E3D80[0].row;
                if (temp_v1_2 >= 2) {
                    break;
                }
                if (temp_v1_2 < 0) {
                    break;
                }
                if (D_801E8F38[temp_v1_2][0]) {
                    TitlePlaySfx(SFX_MENU_CURSOR_MOVE);
                    if (D_801E8F38[D_801E3D80[0].row][2]) {
                        g_MenuStartMode = START_MENU_MODE_FORMAT_PROMPT;
                        SysMenuSetCursorMovement(&D_801E3D80[6], 0, 1, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
                    } else {
                        D_801E3F18 = 10;
                        g_MenuStartMode = START_MENU_MODE_CHECKING_FILES;
                        D_801E3F20 = 0;
                        D_801E3F14 = 0;
                        D_80062F3C = 0;
                        D_801E3F1C = 1;
                        SysMenuSetCursorMovement(&D_801E3D80[1], 0, 0, 1, 3, 0, 0, 1, 15, 0, 0, 0, 0, 0);
                    }
                } else {
                    TitlePlaySfx(SFX_MENU_BAD);
                    SysMenuRequestAddWindow(D_801E33B0, 7);
                }
            } else if (g_Pad1KeysPressed & PADRdown) {
                TitlePlaySfx(SFX_MENU_BACK);
                g_MenuStartMode = START_MENU_MODE_TITLE;
            } else {
                SysMenuHandleButtons(&D_801E3D80[0]);
            }
            break;
        case START_MENU_MODE_SELECT_FILE:
            var_s1 = D_801E3D80[1].unkF;
            SaveHandleScrollCursor(&D_801E3D80[1]);
            if (!D_801E3D80[1].unkF && !var_s1) {
                if (g_Pad1KeysPressed & PADRright) {
                    if (((s32)D_80062F3C >> (D_801E3D80[1].row + D_801E3D80[1].rowOffset)) & 1) {
                        TitlePlaySfx(SFX_MENU_CURSOR_MOVE);
                        g_MenuStartMode = START_MENU_MODE_LOADING;
                        D_801E3F18 = 10;
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
            if (D_801E3F18 == 0) {
                if (D_801E3F1C) {
                    D_801E3F18 = 0;
                    D_801E3F1C = 0;
                    D_80062F3C = GetSaveSlotMask(D_801E3D80[0].row);
                } else {
                    var_s1 = 0;
                    if ((D_80062F3C >> D_801E3F20) & 1) {
                        var_s1 = SaveFetchHeader(D_801E3D80[0].row, D_801E3F20);
                    }
                    D_801E3F20++;
                    if (var_s1) {
                        g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                        SysMenuRequestAddWindow(D_801E33B0[8], 2);
                        TitlePlaySfx(SFX_MENU_BAD);
                    }
                    if (D_801E3F20 == 0xF) {
                        D_801E3F20 = 0xE;
                        g_MenuStartMode = START_MENU_MODE_CHECKING_WAIT;
                        D_801E3F18 = 10;
                        TitlePlaySfx(SFX_MENU_CONFIRMED);
                    }
                }
            } else {
                D_801E3F18--;
            }
            break;
        case START_MENU_MODE_CHECKING_WAIT:
            if (D_801E3F18 == 0) {
                g_MenuStartMode = START_MENU_MODE_SELECT_FILE;
            }
            D_801E3F18--;
            break;
        case START_MENU_MODE_LOADING:
            if (D_801E3F18 != 0) {
                D_801E3F18--;
                break;
            }
            g_SavemapBusy = 1;
            var_a0_3 = D_801E3D80[1].row + D_801E3D80[1].rowOffset;
            if (D_801E3D80[0].row) {
                var_a0_3 |= 0x10;
            }
            var_s1 = (s16)LoadSaveFile(var_a0_3);
            if (var_s1 == 0) {
                if (Savemap.header.checksum != (u16)SaveCalcChecksum(sizeof(SaveWork) - 4, &Savemap.header.leader_level)) {
                    g_MenuStartMode = START_MENU_MODE_SELECT_FILE;
                    TitlePlaySfx(SFX_MENU_BAD);
                    SysMenuRequestAddWindow(D_801E2CFC[31], 0);
                } else {
                    TitlePlaySfx(SFX_MEMCARD_LOADED);
                    D_801E3D54 = 2;
                    TitleApplySoundMode(Savemap.config & 3);
                }
            } else {
                g_MenuStartMode = START_MENU_MODE_SELECT_FILE;
                TitlePlaySfx(SFX_MENU_BAD);
                SysMenuRequestAddWindow(D_801E2CFC[11], var_s1);
            }
            g_SavemapBusy = 0;
            break;
        case START_MENU_MODE_FORMAT_PROMPT:
            SysMenuHandleButtons(&D_801E3DEC[0]);
            if (g_Pad1KeysPressed & PADRright) {
                if (D_801E3DEC[0].row == 0) {
                    if (D_801E3D80[0].row) {
                        temp_v1_2 = format("bu10:");
                    } else {
                        temp_v1_2 = format("bu00:");
                    }
                    g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                    if (temp_v1_2 == 1) {
                        D_801E8F38[D_801E3D80[0].row][2] = 0;
                        SysMenuRequestAddWindow(D_801E2CFC[41], 7);
                        TitlePlaySfx(SFX_MEMCARD_LOADED);
                    } else {
                        SysMenuRequestAddWindow(D_801E3260[3], 7);
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
                switch (D_801E3D80[7].row) {
                case 0:
                    TitlePlaySfx(SFX_MEMCARD_LOADED);
                    D_801E3698 = 1;
                    D_801E3D54 = 2;
                    break;
                case 1:
                    if (D_801E8F38[0][0] || D_801E8F38[1][0]) {
                        TitlePlaySfx(SFX_MENU_CURSOR_MOVE);
                        SysMenuSetCursorMovement(&D_801E3D80[0], 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
                        g_MenuStartMode = START_MENU_MODE_SELECT_SLOT;
                    } else {
                        TitlePlaySfx(SFX_MENU_BAD);
                    }
                    break;
                }
            } else {
                SysMenuHandleButtons(&D_801E3DEC[1]);
            }
            break;
        }
    }
    return D_801E3698;
}

static void TitleCleanup(void) {
    SysMenuLoadPartyPortraits();
    SysMenuStoreCharacterClutToRam(D_800756F8);
    SysMenuLoadCharacterClutFromRam(D_801E3F2C);
    SysMenuRestoreAvatarVram(D_801E8F44);
    SysMenuRestoreFontVram(D_801E4538);
    SaveCleanupCardEvents();
    PutDispEnv(D_801E3EEC);
    PutDrawEnv(D_801E3E34);
}

s32 TitleMain(void) {
    s32 i;
    s32 ret;

    SysMenuCreateDrawenvDispenv(D_801E3E34, D_801E3EEC);
    D_801E3D54 = 0;
    TitleInit();
    D_801E3D58 = 0;
    for (i = 0;; i++) {
        InputUpdateKeyStates();
        SysMenuSetPoly(D_80077F64[D_801E3D58]);
        D_801E3D5C = (u_long*)D_801E3D60[D_801E3D58];
        ClearOTag(D_801E3D5C, 1);
        SysMenuSetOtag(D_801E3D5C);
        SysMenuDrawAddWindow();
        ret = TitleUpdate(i);
        if (D_801E3D54 == -1) {
            break;
        }
        DrawSync(0);
        VSync(0);
        PutDispEnv(&D_801E3EEC[D_801E3D58]);
        PutDrawEnv(&D_801E3E34[D_801E3D58]);
        DrawOTag(D_801E3D5C);
        D_801E3D58 ^= 1; // flip back buffer ID?
    }
    TitleCleanup();
    VSync(0);
    PutDispEnv(&D_801E3EEC[1]);
    PutDrawEnv(&D_801E3E34[1]);
    for (i = 0; i < 3; i++) {
        if (Savemap.partyID[i] != 0xFF) {
            SysInitPlayerStatFromEquip(i);
            SysInitPlayerStatFromMateria((u8)i);
        }
    }
    SysCalcTotalLureGilPreempVal();
    return ret;
}
