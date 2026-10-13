//! PSYQ=3.3 CC1=2.7.2
#include <game.h>
#include <libapi.h>
#include <libgpu.h>
#include <kernel.h>

#include "savemenu.h"
#include <libetc.h>

static s32 g_SaveFadeOpacity = 0xFF;
static MenuRect g_SaveSlotSelectWindow = {150, 93, 64, 32};
static MenuRect g_SaveConfirmWindow = {82, 86, 201, 47};
static u32 D_801D4ED8_unused = 0;
INCLUDE_DATA("menu/data/buster_tim");
static char g_SaveMenuTitleText[8] = _S("Save");
static RECT g_SaveSubWindowRects[3] = {
    {278, 4, 86, 36},
    {184, 40, 180, 24},
    {0, 0, 364, 64},
};
static RECT g_SaveTitleWindowRect = {278, 5, 86, 24};
static u8 D_801DEF04_unused[4] = {0x81, 0x96, 0x00, 0x00};
static u8 shiftJis_table[0x200] = {
    // FF7 char code -> 2-byte Shift-JIS, byte-indexed; digits start at 0x20
    0x81, 0x40, 0x81, 0x49, 0x81, 0x68, 0x81, 0x94, 0x81, 0x90, 0x81, 0x93, 0x81, 0x95, 0x81, 0x66, 0x81, 0x69, 0x81,
    0x6A, 0x81, 0x96, 0x81, 0x7B, 0x81, 0x43, 0x81, 0x7C, 0x81, 0x44, 0x81, 0x5E, 0x82, 0x4F, 0x82, 0x50, 0x82, 0x51,
    0x82, 0x52, 0x82, 0x53, 0x82, 0x54, 0x82, 0x55, 0x82, 0x56, 0x82, 0x57, 0x82, 0x58, 0x81, 0x46, 0x81, 0x47, 0x81,
    0x83, 0x81, 0x81, 0x81, 0x84, 0x81, 0x48, 0x81, 0x97, 0x82, 0x60, 0x82, 0x61, 0x82, 0x62, 0x82, 0x63, 0x82, 0x64,
    0x82, 0x65, 0x82, 0x66, 0x82, 0x67, 0x82, 0x68, 0x82, 0x69, 0x82, 0x6A, 0x82, 0x6B, 0x82, 0x6C, 0x82, 0x6D, 0x82,
    0x6E, 0x82, 0x6F, 0x82, 0x70, 0x82, 0x71, 0x82, 0x72, 0x82, 0x73, 0x82, 0x74, 0x82, 0x75, 0x82, 0x76, 0x82, 0x77,
    0x82, 0x78, 0x82, 0x79, 0x81, 0x6D, 0x81, 0x8F, 0x81, 0x6E, 0x81, 0x4F, 0x81, 0x51, 0x81, 0xA6, 0x82, 0x81, 0x82,
    0x82, 0x82, 0x83, 0x82, 0x84, 0x82, 0x85, 0x82, 0x86, 0x82, 0x87, 0x82, 0x88, 0x82, 0x89, 0x82, 0x8A, 0x82, 0x8B,
    0x82, 0x8C, 0x82, 0x8D, 0x82, 0x8E, 0x82, 0x8F, 0x82, 0x90, 0x82, 0x91, 0x82, 0x92, 0x82, 0x93, 0x82, 0x94, 0x82,
    0x95, 0x82, 0x96, 0x82, 0x97, 0x82, 0x98, 0x82, 0x99, 0x82, 0x9A, 0x81, 0x6F, 0x81, 0x62, 0x81, 0x70, 0x81, 0x60,
    0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81,
    0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6,
    0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81,
    0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6,
    0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x83, 0x58, 0x83, 0x58, 0x83, 0x58, 0x83, 0x58, 0x83, 0x58, 0x83, 0x58, 0x81,
    0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6,
    0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81,
    0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0x63, 0x81, 0xA6,
    0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0x75, 0x81, 0x76, 0x81,
    0x77, 0x81, 0x78, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0x63, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6,
    0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81,
    0xA6, 0x81, 0xA6, 0x81, 0x63, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6,
    0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0x63, 0x81,
    0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6,
    0x81, 0xA6, 0x81, 0xA6, 0x85, 0xAB, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0x63, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81,
    0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6,
    0x81, 0xA6, 0x81, 0xA6, 0x81, 0x63, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6, 0x81, 0xA6,
};
static u32 D_801DF108_unused = 0;
// Card icons, SAVE_ICON_SIZE each: CLUT at 0x00, bitmap at 0x2C
extern u8 g_SaveIcons[];
INCLUDE_DATA("menu/data/saveicons");

// bss starts at g_SaveCharClutBackup; the earlier part lives in title.c, both ordered by savemenu.h
u_long g_SaveCharClutBackup[0x183];
u_long g_SaveFontVramBackup[0xA00];
MemcardFileHeader g_SaveFileHeader;
MemcardSaveFile g_SaveFile;
u8 g_MemCardSlotStatus[2][3];
u8 g_SaveSlotUnkFlags[2];
s32 g_SaveWriteRemaining;
u_long g_SaveAvatarVramBackup[0xB40];

static void PlaySfx(u16 arg0) {
    g_AkaoCmd.opcode = AKAO_PLAY_MENU_SOUND;
    g_AkaoCmd.params[0] = arg0;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
}

static s32 SaveUpdateFade(s32 fadeDirection) {
    RECT rect;

    setTile(g_PolyPtr.tile);
    SetSemiTrans(g_PolyPtr.tile, 1);
    g_PolyPtr.tile->x0 = 0;
    g_PolyPtr.tile->y0 = 0;
    g_PolyPtr.tile->w = SAVE_SCREEN_WIDTH;
    g_PolyPtr.tile->h = SAVE_SCREEN_HEIGHT;
    g_PolyPtr.tile->r0 = g_SaveFadeOpacity;
    g_PolyPtr.tile->g0 = g_SaveFadeOpacity;
    g_PolyPtr.tile->b0 = g_SaveFadeOpacity;
    AddPrim(g_CurrentOT, g_PolyPtr.tile++);
    setRECT(&rect, 0, 0, 255, 255);
    SysMenuSetDrawMode(0, 1, 0x5F, &rect);
    g_SaveFadeOpacity += fadeDirection;
    if (g_SaveFadeOpacity < 0) {
        g_SaveFadeOpacity = 0;
    }
    if (g_SaveFadeOpacity >= 0x100) {
        g_SaveFadeOpacity = 0xFF;
    }
    return g_SaveFadeOpacity;
}

void SAVEMENU_Init(u8 arg0) {
    g_SaveCardCheckWaitTimer = SAVE_CHECK_WAIT_FRAMES;
    g_SaveMenuMode = arg0;
    g_SaveMenuState = SAVE_MENU_STATE_SELECT_SLOT;
    SysMenuSetCursorMovement(menus.tables, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
    SysMenuStoreAvatarVram(g_SaveAvatarVramBackup);
    SysMenuStoreFontVram(g_SaveFontVramBackup);
    SysMenuLoadAvatars();
    SaveInitCardEvents();
}

static void SAVEMENU_Cleanup(void) {
    SysMenuRestoreAvatarVram(g_SaveAvatarVramBackup);
    SysMenuRestoreFontVram(g_SaveFontVramBackup);
    SaveCleanupCardEvents();
}

int SAVEMENU_HandleSave(s32 counter) {
    RECT sp38;
    RECT rect;
    s32 temp_s1;
    s32 temp_s2;
    s32 temp_v1;
    s32 var_s0;
    s32 var_s1;
    s32 var_s2;
    s32 var_s3;
    s32 var_v0_6;
    s8 temp_s0_2;

    if (g_SaveMenuMode == 0) {
        SysMenuDrawMenuList(g_MenuRenderBufferIndex);
    } else if (g_SaveFadeState == SAVE_FADE_IN) {
        if (SaveUpdateFade(-TITLE_FADE_STEP) == 0) {
            g_SaveFadeState = SAVE_FADE_ACTIVE;
        }
    } else if (g_SaveFadeState == SAVE_FADE_OUT) {
        if (SaveUpdateFade(TITLE_FADE_STEP) == 0xFF) {
            g_SaveFadeState = SAVE_FADE_DONE;
        }
    }
    if (!SysMenuGetMenuListState() || (g_SaveMenuMode && g_SaveFadeState == SAVE_FADE_ACTIVE)) {
        if (!(u8)SysMenuIsWindowActive()) {
            if (g_SaveMenuState >= SAVE_MENU_STATE_SELECT_SLOT && g_SaveMenuState < SAVE_MENU_STATE_CHECKING_FILES) {
                SaveFetchAllCardStatus(counter);
            }
            if (g_SaveCardCheckWaitTimer) {
                g_SaveCardCheckWaitTimer--;
            }
        }
    }
    SysMenuUnkNoop(0x80);
    switch (g_SaveMenuState) {
    case SAVE_MENU_STATE_SELECT_SLOT:
        SysMenuDrawCursor(g_SaveSlotSelectWindow.x - 18, g_SaveSlotSelectWindow.y + 6 + (menus.tables[0].row * 12));
        SysMenuDrawString(10, 11, g_SaveLabels[SAVE_STR_SELECT_SLOT], 7);
        SysMenuDrawString(g_SaveSlotSelectWindow.x + 12, g_SaveSlotSelectWindow.y + 5, g_SaveLabels[SAVE_STR_SLOT_1], -(g_MemCardSlotStatus[CARD_SLOT_1][CARD_STATUS_INSERTED] != 0) & 7);
        SysMenuDrawString(g_SaveSlotSelectWindow.x + 12, g_SaveSlotSelectWindow.y + 17, g_SaveLabels[SAVE_STR_SLOT_2], -(g_MemCardSlotStatus[CARD_SLOT_2][CARD_STATUS_INSERTED] != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 256;
        rect.h = 256;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SysMenuDrawWindow(&g_SaveSlotSelectWindow);
        break;
    case SAVE_MENU_STATE_CONFIRM:
        SysMenuDrawCursor(g_SaveConfirmWindow.x + 22, g_SaveConfirmWindow.y + 21 + menus.promptTables[1].row * 12);
        rect.x = 0;
        rect.y = 0;
        rect.w = 256;
        rect.h = 256;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SysMenuDrawString(g_SaveConfirmWindow.x + 10, g_SaveConfirmWindow.y + 6, g_SaveLabels[SAVE_STR_CONFIRM_SAVE], 7);
        SysMenuDrawString(g_SaveConfirmWindow.x + 48, g_SaveConfirmWindow.y + 19, g_SaveLabels[SAVE_STR_YES], 7);
        SysMenuDrawString(g_SaveConfirmWindow.x + 48, g_SaveConfirmWindow.y + 31, g_SaveLabels[SAVE_STR_NO], 7);
        SysMenuDrawWindow(&g_SaveConfirmWindow);
        /* fallthrough */
    case SAVE_MENU_STATE_SELECT_FILE:
        if (!g_MemCardSlotStatus[menus.tables[0].row][CARD_STATUS_INSERTED]) {
            g_SaveMenuState = SAVE_MENU_STATE_SELECT_SLOT;
        } else {
            SysMenuSavePoly();
            if (g_SaveMenuMode == 0) {
                SysMenuSetPoly(g_MenuRenderBufferIndex * 0x5000 + buster_tim);
            } else {
                SysMenuSetPoly(g_SaveBufferIndex * 0x5000 + buster_tim);
            }
            if (g_SaveMenuState != SAVE_MENU_STATE_CONFIRM || (counter & 2)) {
                SysMenuDrawCursor(8, (menus.tables[1].row << 6) | 56);
            }
            var_s3 = !menus.tables[1].scrolling ? 3 : 4;
            for (var_s0 = 0; var_s0 < var_s3; var_s0++) {
                if ((g_SaveSlotMask >> (var_s0 + menus.tables[1].rowOffset)) & 1) {
                    SysMenuStoreWindowColor();
                    SaveDrawSlot(0, var_s0 * 64 + 29 + menus.tables[1].scrollAnimY * 8,
                                 var_s0 + menus.tables[1].rowOffset);
                    SysMenuRestoreWindowColor();
                } else {
                    SysMenuDrawString(50, var_s0 * 64 + 55 + menus.tables[1].scrollAnimY * 8, g_SaveLabels[SAVE_STR_EMPTY], 6);
                    SysMenuCopyWindowRect(&sp38, &g_SaveSubWindowRects[2]);
                    SysMenuMoveWindowRect(&sp38, 0, var_s0 * 64 + 29 + menus.tables[1].scrollAnimY * 8);
                    SysMenuDrawWindow(&sp38);
                }
            }
            SysMenuUnkNoop(0x80);
            rect.y = 29;
            rect.w = 364;
            rect.x = 0;
            rect.h = 195;
            if (g_SaveMenuMode == 0) {
                SysMenuSetDrawenv(&g_MenuDrawEnv[g_MenuRenderBufferIndex], &rect);
            } else {
                SysMenuSetDrawenv(&g_SaveDrawEnv[g_SaveBufferIndex], &rect);
            }
            SysMenuDrawString(10, 11, g_SaveLabels[SAVE_STR_SELECT_FILE], 7);
            SysMenuDrawString(206, 11, g_SaveLabels[SAVE_STR_FILE], 6);
            SysMenuDrawString(
                SysGetSingleStringWidth(g_SaveLabels[SAVE_STR_FILE]) + 208, 11,
                ((13 + menus.tables[1].row + menus.tables[1].rowOffset) * 36) + g_SaveLabels[LABEL_LOAD], 7);
            SysMenuSetWindowRect(&sp38, 200, 5, 78, 24);
            SysMenuDrawWindow(&sp38);
            SysMenuRestorePoly();
        }
        break;
    case SAVE_MENU_STATE_CHECKING_FILES:
    case SAVE_MENU_STATE_CHECKING_WAIT:
        if (g_SaveMenuState == SAVE_MENU_STATE_CHECKING_FILES) {
            var_s2 = 64;
            var_s1 = 32;
            var_s0 = 160;
        } else {
            var_s2 = 224;
            var_s1 = 128;
            var_s0 = 0;
        }
        SysMenuDrawString(10, 11, g_SaveLabels[SAVE_STR_CHECKING_CARD], 7);
        if (g_SaveScanInitialCheck == 0) {
            SysMenuDrawProgressBar(122, 117, (g_SaveScanSlotIndex + 1) * 8, 8, var_s2, var_s1, var_s0);
            rect.x = 0;
            rect.y = 0;
            rect.w = 255;
            rect.h = 255;
            SysMenuSetDrawMode(0, 1, 0x3F, &rect);
        }
        SysMenuSetWindowRect(&sp38, 112, 109, 140, 24);
        SysMenuDrawWindow(&sp38);
        break;
    case SAVE_MENU_STATE_SAVING:
        temp_s1 = SysGetSingleStringWidth(g_SaveLabels[SAVE_STR_SAVING]) + 16;
        SysMenuDrawString(190 - temp_s1 / 2, 115, g_SaveLabels[SAVE_STR_SAVING], 7);
        SysMenuSetWindowRect(&sp38, 182 - temp_s1 / 2, 109, temp_s1, 24);
        SysMenuDrawWindow(&sp38);
        break;
    case SAVE_MENU_STATE_FORMAT_PROMPT:
        if (counter & 2) {
            SysMenuDrawCursor(g_SaveSlotSelectWindow.x - 18, g_SaveSlotSelectWindow.y + 6 + menus.tables[0].row * 12);
        }
        SysMenuDrawString(g_SaveSlotSelectWindow.x + 12, g_SaveSlotSelectWindow.y + 5, g_SaveLabels[SAVE_STR_SLOT_1], -(g_MemCardSlotStatus[CARD_SLOT_1][CARD_STATUS_INSERTED] != 0) & 7);
        SysMenuDrawString(
            g_SaveSlotSelectWindow.x + 12, g_SaveSlotSelectWindow.y + 17, g_SaveLabels[SAVE_STR_SLOT_2], -(g_MemCardSlotStatus[CARD_SLOT_2][CARD_STATUS_INSERTED] != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 256;
        rect.h = 256;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SysMenuDrawWindow(&g_SaveSlotSelectWindow);
        SysMenuDrawString(10, 11, g_SaveFormatStrings[SAVE_FORMAT_STR_UNFORMATTED], 7);
        temp_s2 = SysGetSingleStringWidth(g_SaveFormatStrings[SAVE_FORMAT_STR_PROMPT]) + 16;
        SysMenuDrawString(190 - temp_s2 / 2, g_SaveSlotSelectWindow.h + 99, g_SaveFormatStrings[SAVE_FORMAT_STR_PROMPT], 7);
        SysMenuDrawString(228 - temp_s2 / 2, g_SaveSlotSelectWindow.h + 112, g_SaveLabels[SAVE_STR_YES], 7);
        SysMenuDrawString(228 - temp_s2 / 2, g_SaveSlotSelectWindow.h + 124, g_SaveLabels[SAVE_STR_NO], 7);
        SysMenuDrawCursor(200 - temp_s2 / 2, 115 + (menus.promptTables[0].row * 12) + g_SaveSlotSelectWindow.h);
        SysMenuSetWindowRect(&sp38, 182 - temp_s2 / 2, g_SaveSlotSelectWindow.h + 93, temp_s2, 48);
        SysMenuDrawWindow(&sp38);
        break;
    }
    if (g_SaveMenuMode != 0) {
        SysMenuUnkNoop(0x80);
        SysMenuDrawString(294, 11, g_SaveMenuTitleText, 7);
        SysMenuDrawWindow(&g_SaveTitleWindowRect);
    }
    SysMenuSetWindowRect(&sp38, 0, 5, 364, 24);
    SysMenuDrawWindow(&sp38);
    if (!(g_SaveMenuMode == 0 && !SysMenuGetMenuListState()) && (g_SaveMenuMode == 0 || g_SaveFadeState != SAVE_FADE_ACTIVE)) {
        return;
    }
    if (SysMenuIsWindowActive() & 0xFF) {
        return;
    }
    switch (g_SaveMenuState) {
    case SAVE_MENU_STATE_SELECT_SLOT:
        if (g_Pad0KeysPressed & PAD_CIRCLE) {
            if (g_MemCardSlotStatus[menus.tables[0].row][CARD_STATUS_INSERTED]) {
                PlaySfx(SFX_MENU_CURSOR_MOVE);
                if (g_MemCardSlotStatus[menus.tables[0].row][CARD_STATUS_UNFORMATTED]) {
                    g_SaveMenuState = SAVE_MENU_STATE_FORMAT_PROMPT;
                    SysMenuSetCursorMovement(&menus.promptTables[0], 0, 1, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
                } else {
                    g_SaveMenuState = SAVE_MENU_STATE_CHECKING_FILES;
                    g_SaveScanSlotIndex = 0;
                    g_SaveScanStatus = 0;
                    g_SaveSlotMask = 0;
                    g_SaveScanInitialCheck = 1;
                    g_SaveMenuDelayTimer = SAVE_SCAN_DELAY_FRAMES;
                    SysMenuSetCursorMovement(&menus.tables[1], 0, 0, 1, 3, 0, 0, 1, 15, 0, 0, 0, 0, 0);
                }
            } else {
                PlaySfx(SFX_MENU_BAD);
                SysMenuRequestAddWindow(!g_SaveCardCheckWaitTimer ? g_SaveErrorStrings[SAVE_ERR_NO_CARD] : g_SaveFormatStrings[SAVE_FORMAT_STR_NO_CARD], 7);
            }
        } else {
            SysMenuHandleButtons(&menus.tables[0]);
            if (g_SaveMenuMode != 0) {
                if (g_Pad0KeysPressed & PAD_CROSS) {
                    PlaySfx(SFX_MENU_BACK);
                    g_SaveFadeState = SAVE_FADE_OUT;
                }
            } else if (g_Pad0KeysPressed & PAD_CROSS) {
                PlaySfx(SFX_MENU_BACK);
                SAVEMENU_Cleanup();
                SysMenuSetMenuListAnimation(5, 0);
                SysMenuLoadMenuFileById(0);
            }
        }
        break;
    case SAVE_MENU_STATE_SELECT_FILE:
        var_s0 = menus.tables[1].scrollAnimY;
        SaveHandleScrollCursor(&menus.tables[1]);
        if ((menus.tables[1].scrollAnimY == 0) && (var_s0 == 0)) {
            if (g_Pad0KeysPressed & PAD_CIRCLE) {
                g_SaveMenuState = SAVE_MENU_STATE_CONFIRM;
                SysMenuSetCursorMovement(&menus.promptTables[1], 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
                PlaySfx(SFX_MENU_CURSOR_MOVE);
            } else if (g_Pad0KeysPressed & PAD_CROSS) {
                PlaySfx(SFX_MENU_BACK);
                g_SaveMenuState = SAVE_MENU_STATE_SELECT_SLOT;
            }
        }
        break;
    case SAVE_MENU_STATE_CHECKING_FILES:
        if (g_SaveMenuDelayTimer == 0) {
            if (g_SaveScanInitialCheck) {
                g_SaveMenuDelayTimer = 0;
                g_SaveScanInitialCheck = 0;
                g_SaveSlotMask = GetSaveSlotMask(menus.tables[0].row);
            } else {
                var_s0 = 0;
                if ((g_SaveSlotMask >> g_SaveScanSlotIndex) & 1) {
                    var_s0 = SaveFetchHeader(menus.tables[0].row, g_SaveScanSlotIndex);
                }
                g_SaveScanSlotIndex++;
                if (var_s0) {
                    g_SaveMenuState = SAVE_MENU_STATE_SELECT_SLOT;
                    SysMenuRequestAddWindow(g_SaveErrorStrings[SAVE_ERR_READ_FAILED], 2);
                }
                if (g_SaveScanSlotIndex == NUM_SAVE_FILES_PER_CARD) {
                    g_SaveScanSlotIndex = NUM_SAVE_FILES_PER_CARD - 1;
                    g_SaveMenuState = SAVE_MENU_STATE_CHECKING_WAIT;
                    g_SaveMenuDelayTimer = SAVE_TRANSITION_DELAY;
                    PlaySfx(SFX_MENU_CONFIRMED);
                }
            }
        } else {
            g_SaveMenuDelayTimer--;
        }
        break;
    case SAVE_MENU_STATE_CHECKING_WAIT:
        if (g_SaveMenuDelayTimer == 0) {
            g_SaveMenuState = SAVE_MENU_STATE_SELECT_FILE;
        }
        g_SaveMenuDelayTimer--;
        break;
    case SAVE_MENU_STATE_SAVING:
        if (g_SaveMenuDelayTimer != 0) {
            g_SaveMenuDelayTimer--;
            return;
        }
        g_SaveMenuState = SAVE_MENU_STATE_SELECT_FILE;
        var_v0_6 = menus.tables[1].row + menus.tables[1].rowOffset;
        if (menus.tables[0].row != 0) {
            var_v0_6 |= CARD_PORT_STRIDE;
        }
        if (!SaveCheckFile(var_v0_6)) {
            PlaySfx(SFX_MEMCARD_LOADED);
            SysMenuRequestAddWindow(g_SaveLabels[SAVE_STR_SAVED], 7);
            g_SaveSlotMask |= 1 << (menus.tables[1].row + menus.tables[1].rowOffset);
        } else {
            PlaySfx(SFX_MENU_BAD);
            SysMenuRequestAddWindow(g_SaveErrorStrings[SAVE_ERR_OUT_OF_MEMORY], 7);
        }
        break;
    case SAVE_MENU_STATE_FORMAT_PROMPT:
        SysMenuHandleButtons(&menus.promptTables[0]);
        if (g_Pad0KeysPressed & PAD_CIRCLE) {
            if (menus.promptTables[0].row) {
                g_SaveMenuState = SAVE_MENU_STATE_SELECT_SLOT;
                PlaySfx(SFX_MENU_BACK);
            } else {
                if (menus.tables[0].row) {
                    temp_v1 = format("bu10:");
                } else {
                    temp_v1 = format("bu00:");
                }
                g_SaveMenuState = SAVE_MENU_STATE_SELECT_SLOT;
                if (temp_v1 == 1) {
                    g_MemCardSlotStatus[menus.tables[0].row][CARD_STATUS_UNFORMATTED] = 0;
                    SysMenuRequestAddWindow(g_SaveLabels[SAVE_STR_COMPLETED], 7);
                    PlaySfx(SFX_MEMCARD_LOADED);
                } else {
                    SysMenuRequestAddWindow(g_SaveFormatStrings[SAVE_FORMAT_STR_FAILED], 7);
                    PlaySfx(SFX_MENU_BAD);
                }
            }
        } else if (g_Pad0KeysPressed & PAD_CROSS) {
            g_SaveMenuState = SAVE_MENU_STATE_SELECT_SLOT;
            PlaySfx(SFX_MENU_BACK);
        }
        break;
    case SAVE_MENU_STATE_CONFIRM:
        if (g_Pad0KeysPressed & PAD_CIRCLE) {
            temp_s0_2 = menus.promptTables[1].row;
            switch (menus.promptTables[1].row) {
            case FORMAT_OPTION_YES:
                PlaySfx(SFX_MENU_CURSOR_MOVE);
                g_SaveMenuState = SAVE_MENU_STATE_SAVING;
                g_SaveMenuDelayTimer = SAVE_TRANSITION_DELAY;
                break;
            case FORMAT_OPTION_NO:
                PlaySfx(SFX_MENU_BACK);
                g_SaveMenuState = temp_s0_2;
                break;
            }
        } else if (g_Pad0KeysPressed & PAD_CROSS) {
            g_SaveMenuState = SAVE_MENU_STATE_SELECT_FILE;
            PlaySfx(SFX_MENU_BACK);
        } else {
            SysMenuHandleButtons(&menus.promptTables[1]);
        }
        break;
    }
}

static const char* g_SaveFileNames[] = {
    "BASCUS-94163FF7-S01", "BASCUS-94163FF7-S02", "BASCUS-94163FF7-S03", "BASCUS-94163FF7-S04", "BASCUS-94163FF7-S05",
    "BASCUS-94163FF7-S06", "BASCUS-94163FF7-S07", "BASCUS-94163FF7-S08", "BASCUS-94163FF7-S09", "BASCUS-94163FF7-S10",
    "BASCUS-94163FF7-S11", "BASCUS-94163FF7-S12", "BASCUS-94163FF7-S13", "BASCUS-94163FF7-S14", "BASCUS-94163FF7-S15",
};
static s32 g_SaveStringCopyLimit = 0;

s32 SAVEMENU_Main(void) {
    s32 ret;
    s32 i;

    SysMenuCreateDrawenvDispenv(g_SaveDrawEnv, g_SaveDispEnv);
    i = 0;
    g_SaveFadeState = SAVE_FADE_IN;
    SAVEMENU_Init(1);
    g_SaveBufferIndex = 0;
    while (1) {
        InputUpdateKeyStates();
        SysMenuSetPoly(g_MenuPolygonBuffer[g_SaveBufferIndex]);
        g_SaveOrderingTable = (u_long*)g_SaveOrderingTableBuffers[g_SaveBufferIndex];
        ClearOTag(g_SaveOrderingTable, 1);
        SysMenuSetOtag(g_SaveOrderingTable);
        SysMenuDrawAddWindow();
        ret = SAVEMENU_HandleSave(i);
        if (g_SaveFadeState == SAVE_FADE_DONE) {
            break;
        }
        DrawSync(0);
        VSync(0);
        PutDispEnv(&g_SaveDispEnv[g_SaveBufferIndex]);
        PutDrawEnv(&g_SaveDrawEnv[g_SaveBufferIndex]);
        DrawOTag(g_SaveOrderingTable);
        g_SaveBufferIndex ^= 1;
        i++;
    }
    SAVEMENU_Cleanup();
    VSync(0);
    PutDispEnv(&g_SaveDispEnv[0]);
    PutDrawEnv(&g_SaveDrawEnv[0]);
    VSync(0);
    PutDispEnv(&g_SaveDispEnv[1]);
    PutDrawEnv(&g_SaveDrawEnv[1]);
    return ret;
}

u16 SaveCalcChecksum(u16 len, u8* data) {
    u16 i, j;
    s32 sum = 0xFFFF;
    for (i = 0; i < len; i++) {
        sum ^= *(data + i) << 8;
        for (j = 0; j < 8; j++) {
            if (sum & 0x8000) {
                sum = (sum * 2) ^ 0x1021;
            } else {
                sum *= 2;
            }
        }
    }
    return ~sum;
}

void SaveInitCardEvents(void) {
    s32 i;

    if (D_80062DCC == 0) {
        EnterCriticalSection();
        g_MemcardEvents[0] = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL);
        g_MemcardEvents[1] = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL);
        g_MemcardEvents[2] = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
        g_MemcardEvents[3] = OpenEvent(SwCARD, EvSpNEW, EvMdNOINTR, NULL);
        g_MemcardEvents[4] = OpenEvent(HwCARD, EvSpIOE, EvMdNOINTR, NULL);
        g_MemcardEvents[5] = OpenEvent(HwCARD, EvSpERROR, EvMdNOINTR, NULL);
        g_MemcardEvents[6] = OpenEvent(HwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
        g_MemcardEvents[7] = OpenEvent(HwCARD, EvSpNEW, EvMdNOINTR, NULL);
        InitCARD(1);
        StartCARD();
        ChangeClearPAD(0);
        _bu_init();
        _card_auto(0);
        for (i = 0; i < NUM_MEMCARD_EVENTS; i++) {
            EnableEvent(g_MemcardEvents[i]);
        }
        ExitCriticalSection();
        D_80062DCC = 1;
    }
    for (i = 0; i < NUM_CARD_SLOTS; i++) {
        g_MemCardSlotStatus[i][CARD_STATUS_INSERTED] = 0;
        g_MemCardSlotStatus[i][CARD_STATUS_ERROR] = 0;
        g_MemCardSlotStatus[i][CARD_STATUS_UNFORMATTED] = 0;
    }
}

void SaveCleanupCardEvents(void) {}

static void SaveTestCardEvent(s32 arg0, s32 arg1) { TestEvent(g_MemcardEvents[arg1]); }

// strcmp?
static s32 SaveStrcmp(u8* arg0, u8* arg1) {
    while (1) {
        if (*arg0++ != *arg1++) {
            return 0;
        }
        if (!*arg0 && !*arg1) {
            return 1;
        }
    }
}

// "bu10:" is memory card slot 2, "bu00:" slot 1; the wildcard lists every
// file on the card.
static const char SaveSearchCard2[] = "bu10:*"; // D_801D017C
static const char SaveSearchCard1[] = "bu00:*"; // D_801D0184

// Returns a bitmask of which of the 15 FF7 save slots exist on the memory card
// in `cardSlot`, bit N set when BASCUS-94163FF7-S(N+1) is present. Retries the
// directory open up to 100 times, and gives up with an empty mask.
u16 GetSaveSlotMask(s32 cardSlot) {
    struct DIRENTRY dirEntry;
    s32 i;
    struct DIRENTRY* entry;
    u16 slotMask;

    slotMask = 0;
    for (i = 0; i < 100; i++) {
        if (cardSlot) {
            entry = firstfile((char*)SaveSearchCard2, &dirEntry);
        } else {
            entry = firstfile((char*)SaveSearchCard1, &dirEntry);
        }
        if (entry) {
            break;
        }
    }

    if (entry) {
        while (entry) {
            for (i = 0; i < NUM_SAVE_FILES_PER_CARD; i++) {
                if (SaveStrcmp(entry->name, (u8*)g_SaveFileNames[i]) != 0) {
                    slotMask |= 1 << i;
                }
            }
            entry = nextfile(entry);
        }
    } else {
        slotMask = 0;
    }
    return slotMask;
}

static const char SaveSlotCard2PathFmt[] = "bu10:%s";
static const char SaveSlotCard1PathFmt[] = "bu00:%s";

SaveHeader* SaveGetHeader(s32 arg0) { return &g_SaveSlotHeaders[arg0]; }

s32 LoadSaveHeader(s32 saveId) {
    // Declared but never used, like the one GetSaveSlotMask passes to
    // firstfile(); it still occupies its stack slot.
    struct DIRENTRY dirEntry;
    char path[0x20];
    s32 fd;
    s32 readCount;
    s32 readRetry;
    u8* headerSrc;
    u8* headerDst;
    SaveHeader* headers;
    s32 slot;

    // Only the header page is needed to build the slot preview.
    g_SaveWriteRemaining = SAVE_HEADER_PAGE_SIZE;
    if (saveId & CARD_PORT_STRIDE) {
        sprintf(path, SaveSlotCard2PathFmt, g_SaveFileNames[saveId & 15]);
    } else {
        sprintf(path, SaveSlotCard1PathFmt, g_SaveFileNames[saveId & 15]);
    }
    fd = open(path, 1);
    if (fd == -1) {
        return 1;
    }
    readRetry = SAVE_RETRY_COUNT;
    do {
        readCount = read(fd, &g_SaveFile, g_SaveWriteRemaining);
        if (readCount == g_SaveWriteRemaining) {
            goto read_ok;
        }
        readRetry--;
        if (readCount != -1) {
            g_SaveWriteRemaining -= readCount;
        }
    } while (readRetry != 0);
    if (readRetry != 0) {
        goto read_ok;
    }
    close(fd);
    return 2;
read_ok:
    close(fd);
    slot = saveId & 15;
    headers = g_SaveSlotHeaders;
    headerDst = (u8*)&headers[slot];
    headerSrc = (u8*)&g_SaveFile.save;
    memcpy(headerDst, headerSrc, sizeof(SaveHeader));
    return 0;
}

s32 LoadSaveFile(s32 saveId) {
    // Declared but never used, like the one GetSaveSlotMask passes to
    // firstfile(); it still occupies its stack slot.
    struct DIRENTRY dirEntry;
    char path[0x20];
    s32 fd;
    s32 readCount;
    s32 readRetry;
    s32 i;
    u8* saveSrc;
    u8* saveDst;

    // readCount doubles as a scratch copy of saveId here; the overlay
    // stops matching if either reuse below is given its own variable.
    readCount = saveId;
    g_SaveWriteRemaining = sizeof(MemcardSaveFile);
    if (readCount & CARD_PORT_STRIDE) {
        fd = readCount;
        sprintf(path, SaveSlotCard2PathFmt, g_SaveFileNames[fd & 15]);
    } else {
        sprintf(path, SaveSlotCard1PathFmt, g_SaveFileNames[readCount & 15]);
    }
    fd = open(path, 1);
    if (fd == -1) {
        return 1;
    }
    readRetry = SAVE_RETRY_COUNT;
    do {
        readCount = read(fd, &g_SaveFile, g_SaveWriteRemaining);
        if (readCount == g_SaveWriteRemaining) {
            goto read_ok;
        }
        // Counts up from 0x1E, so a persistent error spins until it
        // wraps rather than giving up after 30 tries. LoadSaveHeader
        // decrements here; this looks like a bug in the original.
        readRetry++;
        if (readCount != -1) {
            g_SaveWriteRemaining -= readCount;
        }
    } while (readRetry != 0);
    if (readRetry != 0) {
        goto read_ok;
    }
    close(fd);
    return 2;
read_ok:
    close(fd);
    saveDst = (u8*)&Savemap;
    saveSrc = (u8*)&g_SaveFile.save;
    memcpy(saveDst, saveSrc, sizeof(SaveWork));
    for (i = 0; i < NUM_MENU_COLOR; i++) {
        g_MenuColors[i] = Savemap.header.menu_color[i];
    }
    return 0;
}

static s32 IsHalfwidthKatakana(s8 arg0) {
    if (arg0 > -0x68 && arg0 < -0x60 || arg0 > -32 && arg0 < -3) {
        return 1;
    }
    return 0;
}

static s32 IsShiftJisLeadByte(s8 arg0) {
    if (arg0 > -0x80 && arg0 < -0x60 || arg0 > -33 && arg0 < -3) {
        return 1;
    }
    return 0;
}

static void SaveCopyTitle(u8* arg0, u8* arg1) {
    s32 i;
    for (i = 0; i < MEMCARD_TITLE_SIZE; i++) {
        *arg0++ = *arg1++;
    }
}

static void SaveSetStringLimit(s32 arg0) { g_SaveStringCopyLimit = arg0; }

static void SaveCopyString(u8* arg0, u8* arg1) {
    s32 i;
    for (i = 0; i < g_SaveStringCopyLimit; i++) {
        *arg0++ = *arg1;
        if (*arg1 == 0xFF) {
            break;
        }
        arg1++;
    }
}

static void UpdateSaveHeader(void) {
    s32 i;
    u8 id;

    for (i = 0; i < NUM_PARTY; i++) {
        Savemap.header.party_portraits[i] = Savemap.partyID[i];
    }
    SaveSetStringLimit(SAVE_LEADER_NAME_LIMIT);
    for (i = 0; i < NUM_PARTY; i++) {
        id = Savemap.partyID[i];
        if (id != CHAR_EMPTY) {
            SaveCopyString(Savemap.header.leader_name, Savemap.party[id].name);
            Savemap.header.leader_level = Savemap.party[id].level;
            Savemap.header.leader_hp = g_ActiveCharacters[i].hp;
            Savemap.header.leader_hp_max = g_ActiveCharacters[i].baseHp;
            Savemap.header.leader_mp = g_ActiveCharacters[i].mp;
            Savemap.header.leader_mp_max = g_ActiveCharacters[i].baseMp;
            break;
        }
    }
    for (i = 0; i < NUM_MENU_COLOR; i++) {
        Savemap.header.menu_color[i] = g_MenuColors[i];
    }
    Savemap.header.gil = Savemap.gil;
    Savemap.header.time = Savemap.time;
    SaveSetStringLimit(SAVE_LOCATION_NAME_LIMIT);
    SaveCopyString(Savemap.header.place_name, &Savemap.memory_bank_4[104]);
}

static s32 WriteSaveFile(s8* path, u8* title) {
    u8 hour;
    u8 minute;
    u8 digit;
    s32 existingFd;
    s32 createdFd;
    s32 fd;
    s32 written;
    s32 writeComplete;
    s32 deleteRetry;
    s32 createRetry;
    s32 openRetry;
    s32 writeRetry;
    s32 i;
    u8* reserved;
    u8* savemapSrc;
    u8* headerDst;
    u8* iconClut;
    // Never read, but the target reserves its 0x200 bytes on the stack.
    MemcardFileHeader unused;

    UpdateSaveHeader();
    g_SaveWriteRemaining = sizeof(MemcardSaveFile);
    g_SaveFileHeader.magic[0] = 'S';
    g_SaveFileHeader.magic[1] = 'C';
    g_SaveFileHeader.iconFlag = 0x11;
    g_SaveFileHeader.blockCount = 1;
    SaveCopyTitle(g_SaveFileHeader.title, title);
    i = sizeof(g_SaveFileHeader.reserved) - 1;
    reserved = &g_SaveFileHeader.reserved[i];
    for (; i >= 0; i--) {
        *reserved-- = 0;
    }
    hour = SysGetHoursFromSeconds(Savemap.header.time);
    digit = ((hour / 10) * 2) + SJIS_DIGITS_OFFSET;
    g_SaveFileHeader.title[22] = shiftJis_table[digit];
    g_SaveFileHeader.title[23] = shiftJis_table[digit + 1];
    digit = ((hour % 10) * 2) + SJIS_DIGITS_OFFSET;
    g_SaveFileHeader.title[24] = shiftJis_table[digit];
    g_SaveFileHeader.title[25] = shiftJis_table[digit + 1];
    minute = SysGetMinutesFromSeconds(Savemap.header.time);
    digit = ((minute / 10) * 2) + SJIS_DIGITS_OFFSET;
    g_SaveFileHeader.title[28] = shiftJis_table[digit];
    g_SaveFileHeader.title[29] = shiftJis_table[digit + 1];
    digit = ((minute % 10) * 2) + SJIS_DIGITS_OFFSET;
    g_SaveFileHeader.title[30] = shiftJis_table[digit];
    g_SaveFileHeader.title[31] = shiftJis_table[digit + 1];
    iconClut = &g_SaveIcons[g_SaveSlot * SAVE_ICON_SIZE];
    memcpy(g_SaveFileHeader.iconPalette, iconClut, sizeof(g_SaveFileHeader.iconPalette));
    memcpy(g_SaveFileHeader.iconFrame[0], &g_SaveIcons[(g_SaveSlot * SAVE_ICON_SIZE) + SAVE_ICON_BITMAP_OFFSET],
           sizeof(g_SaveFileHeader.iconFrame[0]));
    headerDst = (u8*)&g_SaveFile.header;
    memcpy(headerDst, (u8*)&g_SaveFileHeader, sizeof(MemcardFileHeader));
    g_SavemapBusy = 1;
    Savemap.header.checksum = SaveCalcChecksum(SAVE_CHECKSUM_SIZE, (u8*)&Savemap.header.leader_level);
    savemapSrc = (u8*)&Savemap;
    memcpy((u8*)&g_SaveFile.save, savemapSrc, sizeof(SaveWork));
    g_SavemapBusy = 0;

    for (deleteRetry = 0; deleteRetry < SAVE_DELETE_RETRY_COUNT; deleteRetry++) {
        // The store into fd is dead; the overlay stops matching without it.
        existingFd = fd = open(path, 1);
        if (existingFd != -1) {
            delete(path);
            close(existingFd);
            break;
        }
    }
    for (createRetry = SAVE_RETRY_COUNT; createRetry != 0; createRetry--) {
        createdFd = open(path, (g_SaveFileHeader.blockCount << 0x10) | 0x200);
        if (createdFd != -1) {
            goto created;
        }
    }
    return 1;
created:
    close(createdFd);
    for (openRetry = SAVE_RETRY_COUNT; openRetry != 0; openRetry--) {
        fd = open(path, 2);
        if (fd != -1) {
            goto opened;
        }
    }
    return 2;
opened:
    writeRetry = SAVE_RETRY_COUNT;
    do {
        // Same: this copy is redundant but the overlay needs it.
        createdFd = fd;
        written = write(createdFd, &g_SaveFile, g_SaveWriteRemaining);
        writeComplete = written == g_SaveWriteRemaining;
        if (writeComplete) {
            goto written_ok;
        }
        writeRetry--;
        if (written != -1) {
            g_SaveWriteRemaining -= written;
        }
    } while (writeRetry != 0);
    close(createdFd);
    return 3;
written_ok:
    close(createdFd);
    return 0;
}

static const char* g_SaveCardTitleFormats[] = {
    "ＦＦ７／ＳＡＶＥ０１／００：００", "ＦＦ７／ＳＡＶＥ０２／００：００", "ＦＦ７／ＳＡＶＥ０３／００：００",
    "ＦＦ７／ＳＡＶＥ０４／００：００", "ＦＦ７／ＳＡＶＥ０５／００：００", "ＦＦ７／ＳＡＶＥ０６／００：００",
    "ＦＦ７／ＳＡＶＥ０７／００：００", "ＦＦ７／ＳＡＶＥ０８／００：００", "ＦＦ７／ＳＡＶＥ０９／００：００",
    "ＦＦ７／ＳＡＶＥ１０／１１：１１", "ＦＦ７／ＳＡＶＥ１１／１１：１１", "ＦＦ７／ＳＡＶＥ１２／１１：１１",
    "ＦＦ７／ＳＡＶＥ１３／１１：１１", "ＦＦ７／ＳＡＶＥ１４／１１：１１", "ＦＦ７／ＳＡＶＥ１５／１１：１１",
};

static s16 SaveCheckFile(s32 saveId) {
    char path[0x40];
    s32 ret;
    s32 slot;

    if (saveId & CARD_PORT_STRIDE) {
        sprintf(path, SaveSlotCard2PathFmt, g_SaveFileNames[saveId & 15]);
    } else {
        sprintf(path, SaveSlotCard1PathFmt, g_SaveFileNames[saveId & 15]);
    }
    slot = saveId & 15;
    g_SaveSlot = slot;
    ret = WriteSaveFile(path, (u8*)g_SaveCardTitleFormats[slot]);
    if (!(s16)ret) {
        memcpy(&g_SaveSlotHeaders[slot], &Savemap.header, sizeof(SaveHeader));
    }
    return ret;
}

s32 g_TitleFadeBrightness = 0xFF;                            // used by title.c
StartMenuMode g_MenuStartMode = START_MENU_MODE_SELECT_SLOT; // used by title.c

unsigned char g_SaveLabels[][0x24] = {
    _S("Load"),
    _S("Select a slot."),
    _S("Select a file."),
    _S("SLOT 1"),
    _S("SLOT 2"),
    _S("Are you sure?"),
    _S("Loading. Do not remove Memory card."),
    _S("Saving. Do not remove Memory card."),
    _S("EMPTY"),
    _S("FILE"),
    _S("Continue?"),
    _S("/15"),
    _S("Checking Memory card."),
    _S("01"),
    _S("02"),
    _S("03"),
    _S("04"),
    _S("05"),
    _S("06"),
    _S("07"),
    _S("08"),
    _S("09"),
    _S("10"),
    _S("11"),
    _S("12"),
    _S("13"),
    _S("14"),
    _S("15"),
    _S("Saved."),
    _S("Could not save."),
    _S("Could not load."),
    _S("File is ruined."),
    _S("NEW GAME"),
    _S("Are you sure you want to save?"),
    _S("Yes"),
    _S("No"),
    _S(""),
    _S("Completed."),
};
static u32 _padding[] = {0, 0, 0};
unsigned char g_SaveFormatStrings[][0x30] = {
    _S(""),
    _S(""),
    _S("Formatted."),
    _S("Could not format."),
    _S("Not formatted."),
    _S("Want to format it now?"),
    _S("No Memory card."),
};
unsigned char g_SaveErrorStrings[][0x30] = {
    _S("No Memory card."),
    _S("This Memory card is damaged and cannot be used."),
    _S("Please insert another Memory card."),
    _S("No enough memory left on Memory card."),
    _S("Use another Memory card,"),
    _S("or erase 1 block of saved data."),
    _S(""),
    _S(""),
    _S("Couldn't read it."),
    _S("Still want to begin the game?"),
    _S("‘’"),
    _S("‘’"),
    _S("‘’"),
    _S(""),
};
