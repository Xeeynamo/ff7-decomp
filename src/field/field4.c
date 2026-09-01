//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern u8 D_800E08C0[];
extern u8 g_DialogDigitCharacters[16];
extern u8 g_WindowReplaceBank[4][8];
extern u16 g_WindowReplaceBankAddr[4][8];
extern u32 g_FieldKeyState;
extern s16 g_WindowWaitTime[4];
extern u8* g_WindowStringPtr[4];
extern u8 g_WindowString[4][256];
extern s16 g_WindowNameCopyCount[4];
extern s16 g_WindowReplaceParam[4];
extern s16 g_WindowExtraRows[4];
extern s16 g_WindowTextBudget[4];
extern s16 g_WindowFastForwardLevel[4];
extern s16 g_WindowBufferPos[4];
extern u8 g_WindowBuffer[4][16];
extern s16 g_WindowTotalRowsHeight[4];
extern char g_FieldDebugDigits[16];
extern char D_800E0628[];
extern s8 D_800E0630;
extern s16 g_FieldDebugRChars;
extern s16 g_FieldDebugRLines;
extern s16 g_FieldDebugRRect;
extern s16 g_FieldDebugRDm;
extern u16 g_FieldDebugTransp;
extern char g_DebugText[];
extern char g_DebugMessageBuffer[];

void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind);
void FieldWindowReset(s16 window);
void FieldDebugStringCopy(char* dst, const char* src);
void FieldDebugStringU8hex(s32 val, char* msg_out);
void FieldDebugStringU16hex(s32 val, char* msg_out);
void FieldDebugStringU32hex(s32 val, char* msg_out);
static void PlayWindowPointerClickSound(void);
s32 FieldDialogWindowInit(s16 window, s16 stringId);
void FieldDialogWindowGrowth(s16 window);
void FieldDialogCopyTextFromField(s16 window);
void DialogScrollText(s16 window);
void DialogScrollTextDuringOk(s16 window);
void FieldDialogWindowInitNext(s16 window);
s32 FieldDialogWindowDecrease(s16 window);
u16 FieldDialogGetVariableFromBank(s16 window);
void ConvertDigitToString(u16 value, u8* dst);
void ConvertNumToStrWithSpace(u16 value, u8* dst);
void ConvertHexToString(u16 value, u8* dst);

/////////////////////////////////////////////////
// Begin of field_dialog.c
/////////////////////////////////////////////////

static void DebugDummyFunc(void) {}

void DebugPrintToFieldWindow(const char* str) {
    // used to print debug messages -- dummied out on release
}

void FieldEventDebugError(const char* errmsg) {
    FieldDebugPageInit(0, 100, 100, 150, 12);
    FieldDebugPageSetColor(0, 0x7F, 0, 0);
    AddStrNextDebugRow(0, errmsg);
    g_FieldScriptHalted = 1;
    g_FieldScriptRunState = 4;
}

void FieldWindowResetAll(void) {
    s32 i;

    g_WindowCount = 0;
    for (i = 0; i < 4; i++) {
        FieldWindowReset(i);
    }
    if (g_FieldScripts->stringOffset != 0) {
        g_FieldText = (u8*)g_FieldScripts + g_FieldScripts->stringOffset;
    } else {
        g_FieldText = NULL;
    }
}

void FieldWindowReset(s16 window) {
    s32 i;

    if (window == 1) {
        g_WindowData[window].y = 8;
    } else {
        g_WindowData[window].y = 149;
    }

    g_WindowData[window].x = 8;
    g_WindowData[window].width = 304;
    g_WindowData[window].height = 73;
    g_WindowData[window].currentWidth = 1;
    g_WindowData[window].currentHeight = 1;
    g_WindowData[window].state = WSTATE_INIT;
    g_WindowData[window].style = WSTYLE_NORMAL;
    g_WindowData[window].numDisplayType = WNDT_OFF;
    g_WindowData[window].unk1C = 0;
    g_WindowData[window].numDisplayLength = 6;
    g_WindowData[window].numDisplayX = 0;
    g_WindowData[window].numDisplayY = 0;
    g_WindowData[window].preventClose = 0;
    g_WindowToEntity[window] = 0xFF;

    for (i = 0; i < 8; i++) {
        g_WindowReplaceBank[window][i] = 0;
        g_WindowReplaceBankAddr[window][i] = 0;
    }

    g_WindowWaitTime[window] = 0;
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("mes reset=", window, 1);
    }
}

s32 FieldWindowSetStateToClose(s16 window) {
    switch (g_WindowData[window].state) {
    case WSTATE_SHOW:
        return 0;
    case WSTATE_TXT:
    case WSTATE_WAIT_ROW:
    case WSTATE_TXT_DONE:
    case WSTATE_SCROLL_ROW:
    case WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK:
    case WSTATE_PAUSE_TXT_UNTIL_OK:
        g_WindowData[window].state = WSTATE_CLOSING;
    }
    return 1;
}

void FieldDialogSetWindowStyleCbc(s16 window, u8 style, s16 preventClose) {
    g_WindowData[window].style = style;
    g_WindowData[window].preventClose = preventClose;
}

void FieldWindowResetTextAll(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        g_WindowData[i].state = WSTATE_INIT;
        g_WindowData[i].stringLength = 0;
        g_WindowToEntity[i] = 0xFF;
        g_WindowWaitTime[i] = 0;
    }
    g_WindowCount = 0;
}

void FieldDialogSetSize(s16 window, s16 x, s16 y, s16 width, s16 height) {
    if (x < 8) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("win limit x=", x, 2);
        }
        x = 8;
    }
    if (x + width > 312) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("win limit x=", x + width, 3);
        }
        x = 312 - width;
    }
    if (y < 8) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("win limit y=", y, 2);
        }
        y = 8;
    }
    if (y + height > 224) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("win limit y=", y + height, 3);
        }
        y = 224 - height;
    }

    g_WindowData[window].x = x;
    g_WindowData[window].y = y;
    g_WindowData[window].width = width;
    g_WindowData[window].height = height;
}

void FieldDialogMove(s16 window, s16 dx, s16 dy) {
    g_WindowData[window].x += dx;
    g_WindowData[window].y += dy;
}

void FieldDialogSetWindowHeight(s16 window, s16 height) {
    g_WindowData[window].height = height;
}

s32 FieldDialogMessageUpdateStates(u8 window, u8 message) {
    switch (g_WindowData[window].state) {
    case WSTATE_INIT:
        if (FieldDialogWindowInit(window, message)) {
            return 1;
        }
        break;
    case WSTATE_SHOW:
        FieldDialogWindowGrowth(window);
        break;
    case WSTATE_TXT:
        FieldDialogCopyTextFromField(window);
        break;
    case WSTATE_SCROLL_ROW:
        DialogScrollText(window);
        break;
    case WSTATE_SCROLL_TXT_WHILE_OK:
        DialogScrollTextDuringOk(window);
        break;
    case WSTATE_PAUSE_TXT_UNTIL_OK:
        if (g_pFieldState->pressedKeys & PADRright) {
            g_WindowData[window].state = WSTATE_TXT;
        }
        break;
    case WSTATE_PAUSE_TXT:
        if (g_WindowWaitTime[window] == 0) {
            g_WindowData[window].state = WSTATE_TXT;
        } else {
            g_WindowWaitTime[window]--;
        }
        break;
    case WSTATE_WAIT_ROW:
        if (g_pFieldState->pressedKeys & PADRright) {
            if (g_WindowData[window].currentRow ==
                (g_WindowData[window].height - 9) / 16 - 1 +
                    g_WindowExtraRows[window]) {
                g_WindowData[window].state = WSTATE_SCROLL_ROW;
                g_WindowData[window].textScrolling -= 2;
                g_WindowExtraRows[window]++;
            }
        }
        break;
    case WSTATE_TXT_DONE:
        if (!(g_WindowData[window].preventClose & 1) &&
            (g_pFieldState->pressedKeys & PADRright)) {
            g_WindowData[window].state = WSTATE_CLOSING;
            FieldDialogWindowDecrease(window);
        }
        break;
    case WSTATE_WAIT_NEXT_WINDOW:
        if (g_pFieldState->pressedKeys & PADRright) {
            FieldDialogWindowInitNext(window);
        }
        break;
    case WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK:
        if (g_pFieldState->pressedKeys & PADRright) {
            g_WindowData[window].state = WSTATE_SCROLL_TXT_WHILE_OK;
            g_WindowTotalRowsHeight[window] =
                g_WindowData[window].currentRow * 16 + 17;
            g_WindowData[window].textScrolling -= 2;
        }
        break;
    case WSTATE_INIT_NEXT:
        FieldDialogWindowInitNext(window);
        break;
    case WSTATE_UNK5:
    case WSTATE_CLOSING:
        if (FieldDialogWindowDecrease(window)) {
            return 1;
        }
        break;
    }

    return 0;
}

s32 FieldDialogAskUpdateStates(
    u8 window, u8 message, u8 first, u8 last, s16* selectedLine) {
    switch (g_WindowData[window].state) {

    // Clears window, sets width/height to 1/4 of what was previously set
    // with FieldDialogSetSize, and sets state = WSTATE_SHOW.
    case WSTATE_INIT:
        if (FieldDialogWindowInit(window, message)) {
            return 1;
        }
        break;

    // Increases window to full size and sets state WSTATE_TXT.
    case WSTATE_SHOW:
        FieldDialogWindowGrowth(window);
        break;

    // Renders text gradually. Can transition to other states to pause or scroll
    // text or open new window. Sets state = WSTATE_TXT_DONE when all text is
    // displayed.
    case WSTATE_TXT:
        FieldDialogCopyTextFromField(window);
        break;
    case WSTATE_SCROLL_ROW:
        DialogScrollText(window);
        break;
    case WSTATE_SCROLL_TXT_WHILE_OK:
        DialogScrollTextDuringOk(window);
        break;
    case WSTATE_PAUSE_TXT_UNTIL_OK:
        if (g_pFieldState->pressedKeys & PADRright) {
            g_WindowData[window].state = WSTATE_TXT;
        }
        break;
    case WSTATE_PAUSE_TXT:
        if (g_WindowWaitTime[window] == 0) {
            g_WindowData[window].state = WSTATE_TXT;
        } else {
            g_WindowWaitTime[window]--;
        }
        break;
    case WSTATE_WAIT_ROW:
        if (g_pFieldState->pressedKeys & PADRright) {
            if (g_WindowData[window].currentRow ==
                (g_WindowData[window].height - 9) / 16 - 1 +
                    g_WindowExtraRows[window]) {
                g_WindowData[window].state = WSTATE_SCROLL_ROW;
                g_WindowData[window].textScrolling -= 2;
                g_WindowExtraRows[window]++;
            }
        }
        break;

    // Displays all text and pointer for user to choose an option.
    case WSTATE_TXT_DONE:
        if (!(g_WindowData[window].preventClose & 1)) {
            g_WindowData[window].pointerEnabled = 1;

            if (g_pFieldState->pressedKeysRaw & PADLup) {
                if (first < *selectedLine) {
                    PlayWindowPointerClickSound();
                }
                (*selectedLine)--;
            }
            if (g_pFieldState->pressedKeysRaw & PADLdown) {
                if (*selectedLine < last) {
                    PlayWindowPointerClickSound();
                }
                (*selectedLine)++;
            }
            if (*selectedLine < first) {
                *selectedLine = first;
            }
            if (last < *selectedLine) {
                *selectedLine = last;
            }

            g_WindowData[window].pointerX = 5;
            g_WindowData[window].pointerY = *selectedLine * 16 + 6;

            // User has pressed OK to choose an option.
            if (g_pFieldState->pressedKeys & PADRright) {
                PlayWindowPointerClickSound();
                g_WindowData[window].state = WSTATE_CLOSING;
                FieldDialogWindowDecrease(window);
            }
        }
        break;
    case WSTATE_WAIT_NEXT_WINDOW:
        if (g_pFieldState->pressedKeys & PADRright) {
            FieldDialogWindowInitNext(window);
        }
        break;
    case WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK:
        if (g_pFieldState->pressedKeys & PADRright) {
            g_WindowData[window].state = WSTATE_SCROLL_TXT_WHILE_OK;
            g_WindowTotalRowsHeight[window] =
                g_WindowData[window].currentRow * 16 + 17;
            g_WindowData[window].textScrolling -= 2;
        }
        break;
    case WSTATE_INIT_NEXT:
        FieldDialogWindowInitNext(window);
        break;
    case WSTATE_UNK5:
    case WSTATE_CLOSING:
        if (FieldDialogWindowDecrease(window)) {
            g_WindowData[window].pointerEnabled = 0;
            return 1;
        }
        // Make pointer blink while window is closing.
        g_WindowData[window].pointerEnabled ^= 1;
        break;
    }

    return 0;
}

static void PlayWindowPointerClickSound(void) {
    FieldEventClearAkaoStruct();
    D_8009A000[0] = 0x30;
    D_8009A004[0] = 1;
    D_8009A008[0] = 0x40;
    SystemAkaoExecute();
}

s32 FieldDialogWindowInit(s16 window, s16 stringId) {
    if (g_FieldText == NULL) {
        FieldEventDebugError("No mes data!");
        return 1;
    }

    if (g_WindowToEntity[window] != 0xFF) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("mes busy=", window, 1);
        }
        return 0;
    }

    g_WindowToEntity[window] = g_CurrentEntity;
    g_WindowData[window].currentWidth = g_WindowData[window].width / 4;
    g_WindowData[window].currentHeight = g_WindowData[window].height / 4;
    if (g_WindowData[window].currentHeight < 8) {
        g_WindowData[window].currentHeight = 8;
    }
    if (g_WindowData[window].currentWidth < 8) {
        g_WindowData[window].currentWidth = 8;
    }

    g_WindowData[window].text = g_WindowString[window];
    g_WindowData[window].textScrolling = 0;
    g_WindowData[window].stringLength = 0;
    g_WindowData[window].stringByteLength = 0;
    g_WindowData[window].currentRow = 0;
    g_WindowData[window].pointerEnabled = 0;
    g_WindowString[window][0] = 0xFF;

    g_WindowStringPtr[window] = g_FieldText;
    g_WindowStringPtr[window] += g_FieldText[stringId * 2 + 2];
    g_WindowStringPtr[window] += g_FieldText[stringId * 2 + 3] << 8;

    g_WindowCount++;
    g_WindowFastForwardLevel[window] = 1;
    g_WindowTextBudget[window] = 0;
    g_WindowExtraRows[window] = 0;
    g_WindowNameCopyCount[window] = 0;
    g_WindowReplaceParam[window] = 0;
    g_WindowBufferPos[window] = -1;
    g_WindowData[window].state = WSTATE_SHOW;
    return 0;
}

void FieldDialogWindowGrowth(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("mes busy=", window, 1);
        }
        return;
    }

    g_WindowData[window].currentWidth += g_WindowData[window].width / 4;
    if (g_WindowData[window].currentWidth < 8) {
        g_WindowData[window].currentWidth = 8;
    }
    if (g_WindowData[window].width < g_WindowData[window].currentWidth) {
        g_WindowData[window].currentWidth = g_WindowData[window].width;
    }

    g_WindowData[window].currentHeight += g_WindowData[window].height / 4;
    if (g_WindowData[window].currentHeight < 8) {
        g_WindowData[window].currentHeight = 8;
    }
    if (g_WindowData[window].height < g_WindowData[window].currentHeight) {
        g_WindowData[window].currentHeight = g_WindowData[window].height;
    }

    if (g_WindowData[window].currentWidth == g_WindowData[window].width &&
        g_WindowData[window].currentHeight == g_WindowData[window].height) {
        g_WindowData[window].state = WSTATE_TXT;
    }
}

void FieldDialogCopyTextFromField(s16 window) {
    u8 opcode;
    u16 len;
    s16 i;
    s16 baseCredit;
    s16 characterCost;
    u8* name;
    u16 value;

    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("mes busy=", window, 1);
        }
        return;
    }

    /*
     * To render text gradually, the game implements a text-writing credit
     * system. The > comparison in the while-loop means it can emit one
     * fewer character on the first update.
     * g_WindowFastForwardLevel can add 0-8 characters per update and increase
     * scrolling speed. It's ramped up or down based on the state of OK.
     * field_msg_speed  baseCredit  characterCost   chars/update
     * 0                6           1               6
     * 1-32             5           1               5
     * 33-64            4           1               4
     * 65-96            3           1               3
     * 97-159           2           1               2
     * 160-191          2           2               1
     * 192-223          2           3               2/3
     * 224-255          2           4               1/2
     */

    if (g_WindowData[window].preventClose & 2) {
        baseCredit = 256;
        characterCost = 1;
    } else {
        SaveWork* save;

        // Holding OK down increases text and scrolling speed.
        if (g_pFieldState->activeKeys & PADRright) {
            g_WindowFastForwardLevel[window]++;
            if (g_WindowFastForwardLevel[window] > 128) {
                g_WindowFastForwardLevel[window] = 128;
            }
        } else {
            g_WindowFastForwardLevel[window]--;
            if (g_WindowFastForwardLevel[window] < 2) {
                g_WindowFastForwardLevel[window] = 1;
            }
        }

        save = &Savemap;
        if (save->field_msg_speed < 128) {
            baseCredit = ((128 - save->field_msg_speed) >> 5) + 2;
            characterCost = 1;
        } else {
            baseCredit = 2;
            characterCost = ((save->field_msg_speed - 128) >> 5) + 1;
        }
    }

    g_WindowTextBudget[window] +=
        characterCost * (g_WindowFastForwardLevel[window] >> 4) + baseCredit;

    while (g_WindowTextBudget[window] > characterCost) {
        switch (*g_WindowStringPtr[window]) {
        // End of string.
        case 0xFF:
            g_WindowData[window].state = WSTATE_TXT_DONE;
            g_WindowTextBudget[window] = 0;
            goto end;

        // Next row.
        // If we haven't reached max rows, add new row character and continue.
        // If we have, set state to WSTATE_WAIT_ROW.
        case 0xE7:
            if (g_WindowData[window].currentRow ==
                (g_WindowData[window].height - 9) / 16 - 1 +
                    g_WindowExtraRows[window]) {
                g_WindowData[window].state = WSTATE_WAIT_ROW;
                g_WindowFastForwardLevel[window] = 1;
                g_WindowTextBudget[window] = 0;
                goto end;
            }
            g_WindowString[window][g_WindowData[window].stringByteLength] =
                *g_WindowStringPtr[window];
            g_WindowStringPtr[window]++;
            g_WindowData[window].stringByteLength++;
            g_WindowData[window].currentRow++;
            continue;

        // Wait for next window. Stop string copying and return.
        case 0xE8:
        case 0xE9:
            g_WindowStringPtr[window]++;
            g_WindowData[window].state = WSTATE_WAIT_NEXT_WINDOW;
            g_WindowFastForwardLevel[window] = 1;
            g_WindowTextBudget[window] = 0;
            goto end;

        // Write 4 spaces.
        case 0xE1:
            g_WindowStringPtr[window]++;
            for (i = 0; i < 4; i++) {
                g_WindowString[window][g_WindowData[window].stringByteLength] =
                    0;
                g_WindowData[window].stringByteLength++;
                g_WindowData[window].stringLength++;
            }
            continue;

        // Write 10 spaces.
        case 0xE0:
            g_WindowStringPtr[window]++;
            for (i = 0; i < 10; i++) {
                g_WindowString[window][g_WindowData[window].stringByteLength] =
                    0;
                g_WindowData[window].stringByteLength++;
                g_WindowData[window].stringLength++;
            }
            continue;

        // Write ', '
        case 0xE2:
            g_WindowStringPtr[window]++;
            g_WindowString[window][g_WindowData[window].stringByteLength] = 0xC;
            g_WindowData[window].stringByteLength++;
            g_WindowString[window][g_WindowData[window].stringByteLength] = 0;
            g_WindowData[window].stringByteLength++;
            g_WindowData[window].stringLength += 2;
            continue;

        // Write '."'
        case 0xE3:
            g_WindowStringPtr[window]++;
            g_WindowString[window][g_WindowData[window].stringByteLength] = 0xE;
            g_WindowData[window].stringByteLength++;
            g_WindowString[window][g_WindowData[window].stringByteLength] = 2;
            g_WindowData[window].stringByteLength++;
            g_WindowData[window].stringLength += 2;
            continue;

        // Write '…"'
        case 0xE4:
            g_WindowStringPtr[window]++;
            g_WindowString[window][g_WindowData[window].stringByteLength] =
                0xA9;
            g_WindowData[window].stringByteLength++;
            g_WindowString[window][g_WindowData[window].stringByteLength] = 2;
            g_WindowData[window].stringByteLength++;
            g_WindowData[window].stringLength += 2;
            continue;

        // Write player-chosen character name from savemap.
        case 0xEA: // Cloud
        case 0xEB: // Barret
        case 0xEC: // Tifa
        case 0xED: // Aerith
        case 0xEE: // Red XIII
        case 0xEF: // Yuffie
        case 0xF0: // Cait Sith
        case 0xF1: // Vincent
        case 0xF2: // Cid
            value = *g_WindowStringPtr[window] - 0xEA;
            name = GetCharacterName(value);
            if (name[g_WindowNameCopyCount[window]] == 0xFF ||
                g_WindowNameCopyCount[window] >= 9) {
                g_WindowStringPtr[window]++;
                g_WindowNameCopyCount[window] = 0;
            } else {
                g_WindowString[window][g_WindowData[window].stringByteLength] =
                    name[g_WindowNameCopyCount[window]];
                g_WindowData[window].stringByteLength++;
                g_WindowNameCopyCount[window]++;
                g_WindowData[window].stringLength++;
                g_WindowTextBudget[window] -= characterCost;
            }
            continue;

        // Write name of party member.
        case 0xF3:
        case 0xF4:
        case 0xF5:
            value = Savemap.memory_bank_1[22 + *g_WindowStringPtr[window]];
            // Empty party slot. Write 9 ellipsis characters in place of name.
            if (value == 0xFF) {
                if (g_WindowNameCopyCount[window] >= 9) {
                    g_WindowStringPtr[window]++;
                    g_WindowNameCopyCount[window] = 0;
                } else {
                    g_WindowString[window]
                                  [g_WindowData[window].stringByteLength] =
                                      0xA9;
                    g_WindowData[window].stringByteLength++;
                    g_WindowNameCopyCount[window]++;
                    g_WindowData[window].stringLength++;
                    g_WindowTextBudget[window] -= characterCost;
                }
            } else {
                // Party member exists. Get player-chosen name from savemap.
                name = GetCharacterName(value);
                if (name[g_WindowNameCopyCount[window]] == 0xFF ||
                    g_WindowNameCopyCount[window] >= 9) {
                    g_WindowStringPtr[window]++;
                    g_WindowNameCopyCount[window] = 0;
                } else {
                    g_WindowString[window]
                                  [g_WindowData[window].stringByteLength] =
                                      name[g_WindowNameCopyCount[window]];
                    g_WindowData[window].stringByteLength++;
                    g_WindowNameCopyCount[window]++;
                    g_WindowData[window].stringLength++;
                    g_WindowTextBudget[window] -= characterCost;
                }
            }
            continue;

        // Opcode prefix.
        case 0xFE:
            g_WindowString[window][g_WindowData[window].stringByteLength] =
                *g_WindowStringPtr[window];
            g_WindowStringPtr[window]++;
            g_WindowData[window].stringByteLength++;
            switch (*g_WindowStringPtr[window]) {
            // Pause writing text until the player presses OK.
            case 0xDC:
                g_WindowData[window].stringByteLength--;
                g_WindowStringPtr[window]++;
                g_WindowData[window].state = WSTATE_PAUSE_TXT_UNTIL_OK;
                g_WindowFastForwardLevel[window] = 1;
                g_WindowTextBudget[window] = 0;
                goto end;

            // Pause writing and wait for OK before scrolling the text.
            case 0xE0:
                g_WindowData[window].stringByteLength--;
                g_WindowStringPtr[window]++;
                g_WindowData[window].state = WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK;
                g_WindowFastForwardLevel[window] = 1;
                g_WindowTextBudget[window] = 0;
                goto end;

            // Copy an integer from a memory bank.
            case 0xDE:
            case 0xDF:
            case 0xE1:
                g_WindowData[window].stringByteLength--;
                g_WindowStringPtr[window]--;
                if (g_WindowBufferPos[window] == -1) {
                    // First iteration. Fetch and convert the value.
                    value = FieldDialogGetVariableFromBank(window);
                    if (g_DebugLevel & 3) {
                        FieldDebugAddParseValueToPage2("mpara=", value, 4);
                    }
                    opcode = g_WindowStringPtr[window][1];
                    switch (opcode) {
                    // Integer to decimal string.
                    case 0xDE:
                        ConvertDigitToString(value, g_WindowBuffer[window]);
                        break;
                    // Integer to decimal string with space fill.
                    case 0xE1:
                        ConvertNumToStrWithSpace(value, g_WindowBuffer[window]);
                        break;
                    // Integer to hexadecimal string.
                    case 0xDF:
                        ConvertHexToString(value, g_WindowBuffer[window]);
                        break;
                    }
                    g_WindowBufferPos[window]++;
                } else if (
                    g_WindowBuffer[window][g_WindowBufferPos[window]] == 0xFF ||
                    g_WindowBufferPos[window] >= 16) {
                    // Last converted character has been copied.
                    g_WindowStringPtr[window] += 2;
                    g_WindowBufferPos[window] = -1;
                    g_WindowReplaceParam[window]++;
                } else {
                    // Copy the next character of the converted integer.
                    g_WindowString[window][g_WindowData[window]
                                               .stringByteLength] =
                        g_WindowBuffer[window][g_WindowBufferPos[window]];
                    g_WindowData[window].stringByteLength++;
                    g_WindowBufferPos[window]++;
                    g_WindowData[window].stringLength++;
                    g_WindowTextBudget[window] -= characterCost;
                }
                continue;

            // Copy a string from a memory bank.
            case 0xE2:
                g_WindowData[window].stringByteLength--;
                g_WindowStringPtr[window]--;
                if (g_WindowBufferPos[window] == -1) {
                    value = g_WindowStringPtr[window][2];
                    value |= g_WindowStringPtr[window][3] << 8;
                    len = g_WindowStringPtr[window][4];
                    len |= g_WindowStringPtr[window][5] << 8;
                    if (g_DebugLevel & 3) {
                        FieldDebugAddParseValueToPage2("gstr=", value, 4);
                        if (g_DebugLevel & 3) {
                            FieldDebugAddParseValueToPage2("glen=", len, 4);
                        }
                    }
                    for (i = 0; i < len; i++) {
                        g_WindowBuffer[window][i] =
                            Savemap.memory_bank_1[value + i];
                    }
                    g_WindowBuffer[window][i] = 0xFF;
                    g_WindowBufferPos[window]++;
                } else if (
                    g_WindowBuffer[window][g_WindowBufferPos[window]] == 0xFF) {
                    g_WindowStringPtr[window] += 6;
                    g_WindowBufferPos[window] = -1;
                } else {
                    g_WindowString[window][g_WindowData[window]
                                               .stringByteLength] =
                        g_WindowBuffer[window][g_WindowBufferPos[window]];
                    g_WindowData[window].stringByteLength++;
                    g_WindowBufferPos[window]++;
                    g_WindowData[window].stringLength++;
                    g_WindowTextBudget[window] -= characterCost;
                }
                continue;

            // Font colors.
            case 0xD2: // Gray
            case 0xD3: // Blue
            case 0xD4: // Red
            case 0xD5: // Purple
            case 0xD6: // Green
            case 0xD7: // Cyan
            case 0xD8: // Yellow
            case 0xD9: // White
            // Special global colors.
            case 0xDA: // Flash colors
            case 0xDB: // Rainbow colors, changes color for each character
            // Toggle left padding of characters.
            case 0xE9:
                g_WindowString[window][g_WindowData[window].stringByteLength] =
                    *g_WindowStringPtr[window];
                g_WindowStringPtr[window]++;
                g_WindowData[window].stringByteLength++;
                continue;

            // Wait until g_WindowWaitTime reaches 0 before resuming.
            case 0xDD:
                g_WindowData[window].state = WSTATE_PAUSE_TXT;
                g_WindowStringPtr[window]++;
                g_WindowData[window].stringByteLength++;
                g_WindowWaitTime[window] = *g_WindowStringPtr[window];
                g_WindowStringPtr[window]++;
                g_WindowData[window].stringByteLength++;
                g_WindowWaitTime[window] |= *g_WindowStringPtr[window] << 8;
                g_WindowStringPtr[window]++;
                g_WindowData[window].stringByteLength++;
                goto end;

            default:
                g_WindowString[window][g_WindowData[window].stringByteLength] =
                    *g_WindowStringPtr[window];
                g_WindowStringPtr[window]++;
                g_WindowData[window].stringByteLength++;
                g_WindowData[window].stringLength++;
                g_WindowTextBudget[window] -= characterCost;
                continue;
            }

        // Two byte characters used in Japanese extended font(?)
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
            g_WindowString[window][g_WindowData[window].stringByteLength] =
                *g_WindowStringPtr[window];
            g_WindowStringPtr[window]++;
            g_WindowData[window].stringByteLength++;

        // Fall through to copy the second byte of the character.
        // Also used to copy all other characters directly.
        default:
            g_WindowString[window][g_WindowData[window].stringByteLength] =
                *g_WindowStringPtr[window];
            g_WindowStringPtr[window]++;
            g_WindowData[window].stringByteLength++;
            g_WindowData[window].stringLength++;
            g_WindowTextBudget[window] -= characterCost;
            continue;
        }
    }

end:
    g_WindowString[window][g_WindowData[window].stringByteLength] = 0xFF;
}

void DialogScrollText(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("mes busy=", window, 1);
        }
        return;
    }

    if (g_WindowData[window].textScrolling & 0xF) {
        g_WindowData[window].textScrolling -= 2;
    } else {
        g_WindowData[window].state = WSTATE_TXT;
    }
}

void DialogScrollTextDuringOk(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("mes busy=", window, 1);
        }
        return;
    }

    if (g_WindowData[window].textScrolling + g_WindowTotalRowsHeight[window] >
        0) {
        g_WindowData[window].textScrolling -=
            g_WindowFastForwardLevel[window] >> 2;
        if (g_pFieldState->activeKeys & PADRright) {
            g_WindowFastForwardLevel[window]++;
            if (g_WindowFastForwardLevel[window] > 128) {
                g_WindowFastForwardLevel[window] = 128;
            }
        } else {
            g_WindowFastForwardLevel[window]--;
            if (g_WindowFastForwardLevel[window] < 2) {
                g_WindowFastForwardLevel[window] = 1;
            }
        }
    } else {
        g_WindowData[window].state = WSTATE_INIT_NEXT;
    }
}

void FieldDialogWindowInitNext(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("mes busy=", window, 1);
        }
        return;
    }

    g_WindowData[window].state = WSTATE_TXT;
    g_WindowData[window].stringByteLength = 0;
    g_WindowData[window].stringLength = 0;
    g_WindowData[window].textScrolling = 0;
    g_WindowData[window].currentRow = 0;
    g_WindowString[window][0] = 0xFF;
    g_WindowExtraRows[window] = 0;
    g_WindowFastForwardLevel[window] = 1;
}

s32 FieldDialogWindowDecrease(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("mes busy=", window, 1);
        }
        return;
    }

    if (g_WindowData[window].currentWidth >= 8) {
        g_WindowData[window].currentWidth -= g_WindowData[window].width / 4;
    } else {
        g_WindowData[window].currentWidth = 8;
    }

    if (g_WindowData[window].currentHeight >= 8) {
        g_WindowData[window].currentHeight -= g_WindowData[window].height / 4;
    } else {
        g_WindowData[window].currentHeight = 8;
    }

    if (g_WindowData[window].currentWidth < 9 &&
        g_WindowData[window].currentHeight < 9) {
        g_WindowData[window].stringLength = 0;
        g_WindowData[window].state = WSTATE_INIT;
        g_WindowToEntity[window] = 0xFF;
        g_WindowCount--;
        return 1;
    }
    return 0;
}

u16 FieldDialogGetVariableFromBank(s16 window) {
    u16 value;
    u16 offset;

    switch (g_WindowReplaceBank[window][g_WindowReplaceParam[window]]) {
    case 0:
        value = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]];
        break;
    case 1:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]];
        value = Savemap.memory_bank_1[offset];
        break;
    case 2:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]];
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 3:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] +
                 0x100;
        value = Savemap.memory_bank_1[offset];
        break;
    case 4:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] +
                 0x100;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 11:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] +
                 0x200;
        value = Savemap.memory_bank_1[offset];
        break;
    case 12:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] +
                 0x200;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 13:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] +
                 0x300;
        value = Savemap.memory_bank_1[offset];
        break;
    case 15:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] +
                 0x400;
        value = Savemap.memory_bank_1[offset];
        break;
    case 14:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] +
                 0x300;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 7:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] +
                 0x400;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 5:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]];
        value = g_FieldMapVars[offset];
        break;
    case 6:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]];
        value = g_FieldMapVars[offset];
        value |= g_FieldMapVars[offset + 1] << 8;
        break;
    default:
        value = 0;
        break;
    }

    return value;
}

void ConvertDigitToString(u16 value, u8* dst) {
    u32 foundDigit;
    s16 i;
    s16 divisor;
    s16 digit;

    foundDigit = 0;
    divisor = 10000;
    i = 0;
    while (divisor > 1) {
        digit = value / divisor;
        if (foundDigit || digit) {
            foundDigit = 1;
            dst[i] = g_DialogDigitCharacters[digit];
            i++;
        }
        value -= digit * divisor;
        divisor /= 10;
    }
    dst[i] = g_DialogDigitCharacters[value];
    dst[i + 1] = 0xFF;
}

void ConvertNumToStrWithSpace(u16 value, u8* dst) {
    s32 foundDigit;
    s16 i;
    s16 divisor;
    s16 digit;

    foundDigit = 0;
    divisor = 10000;
    i = 0;
    while (divisor > 1) {
        digit = value / divisor;
        if (foundDigit || digit) {
            foundDigit = 1;
            dst[i] = g_DialogDigitCharacters[digit];
            i++;
        } else {
            dst[i] = 0;
            i++;
        }
        value -= digit * divisor;
        divisor /= 10;
    }
    dst[i] = g_DialogDigitCharacters[value];
    dst[i + 1] = 0xFF;
}

void ConvertHexToString(u16 value, u8* dst) {
    u32 foundDigit;
    s16 i;
    s16 divisor;
    s16 digit;

    foundDigit = 0;
    divisor = 0x1000;
    i = 0;
    while (divisor > 1) {
        digit = value / divisor;
        if (foundDigit || digit) {
            foundDigit = 1;
            dst[i] = g_DialogDigitCharacters[digit];
            i++;
        }
        value -= digit * divisor;
        divisor /= 16;
    }
    dst[i] = g_DialogDigitCharacters[value];
    dst[i + 1] = 0xFF;
}

s32 CopyDialogToMapName(s16 stringId) {
    s16 i;
    s16 j;
    u8* str;
    u8* charName;
    u8 value;

    if (g_FieldText == NULL) {
        FieldEventDebugError("No mes data!");
        return 0;
    }

    str = g_FieldText;
    j = 0;
    i = 0;
    // Field text section starts with an array of 16-bit offsets to each string.
    str += g_FieldText[stringId * 2 + 2];
    str += g_FieldText[stringId * 2 + 3] << 8;

    do {
        switch (*str) {
        // End of string.
        case 0xFF:
            goto end;

        // Write ', '
        case 0xE2:
            str++;
            Savemap.memory_bank_4[104 + i] = 0xC;
            i++;
            Savemap.memory_bank_4[104 + i] = 0;
            i++;
            break;

        // Write '."'
        case 0xE3:
            str++;
            Savemap.memory_bank_4[104 + i] = 0xE;
            i++;
            Savemap.memory_bank_4[104 + i] = 2;
            i++;
            break;

        // Write '…"'
        case 0xE4:
            str++;
            Savemap.memory_bank_4[104 + i] = 0xA9;
            i++;
            Savemap.memory_bank_4[104 + i] = 2;
            i++;
            break;

        // Write player-chosen character name from savemap.
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
        case 0xF0:
        case 0xF1:
        case 0xF2:
            charName = GetCharacterName((s16)(*str - 0xEA)) + j;
            if (*charName == 0xFF || j >= 9) {
                str++;
                j = 0;
            } else {
                j++;
                Savemap.memory_bank_4[104 + i] = *charName;
                i++;
            }
            break;

        // Two byte characters used in Japanese extended font(?)
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
            value = *str;
            str++;
            Savemap.memory_bank_4[104 + i] = value;
            i++;

        // Fall through to copy the second byte of the character.
        // Also used to copy all other characters directly.
        default:
            value = *str;
            str++;
            Savemap.memory_bank_4[104 + i] = value;
            i++;
            break;
        }
    } while (i < 23);

end:
    Savemap.memory_bank_4[104 + i] = 0xFF;
    return 1;
}

void SystemMessageSetCharName(s16 battleCharId, s16 stringId) {
    u8* newName;
    s16 len;
    u8* charName;

    if (g_FieldText == NULL) {
        FieldEventDebugError("No mes data!");
        return;
    }

    newName = g_FieldText;
    newName += g_FieldText[stringId * 2 + 2];
    newName += g_FieldText[stringId * 2 + 3] << 8;
    len = 0;
    charName = GetCharacterName(battleCharId);

    while (*newName != 0xFF) {
        *charName++ = *newName++;
        len++;
    }

    if (len < 9) {
        *charName = 0xFF;
    }
}

/////////////////////////////////////////////////
// Begin of field_debug.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_debug.c
/////////////////////////////////////////////////

/* Initialise the debug renderer's GPU buffers: hide all six pages, then build
 * both framebuffers' primitive arrays (sprites, lines, tiles) with their packet
 * codes and blend bits, the CLUT table, and the two draw-mode blocks. */
void FieldDebugInitBuffers(void) {
    s32 i;
    s32 off;
    u8* sprt0;
    u8* sprt1;
    u8* line0;
    u8* line1;
    u8* tile0;
    u8* tile1;
    u8* dm0;
    u8* dm1;
    u16 tpage;
    s32 hide;
    u8 unusedLocals[0x20];

    hide = 1;
    for (off = 0x762; off >= 0; off -= 0x17A) {
        g_FieldDebugPageHidden[off] = hide;
    }
    g_FieldDebugPagesDirty = 1;
    g_FieldDebugRb = 0;
    g_FieldDebugCurPage = 0;
    g_FieldDebugTransp = 0;

    sprt0 = (u8*)&D_800E1028[0];
    sprt1 = sprt0 + 0x1580;
    for (i = 0; i < 0x158; i++) {
        setlen(&sprt0[i * 0x10], 3);
        setcode(&sprt0[i * 0x10], 0x74);
        setlen(&sprt1[i * 0x10], 3);
        setcode(&sprt1[i * 0x10], 0x74);
        setShadeTex(&sprt0[i * 0x10], 1);
        setShadeTex(&sprt1[i * 0x10], 1);
    }

    for (i = 0; i < 8; i++) {
        D_800E4200[i] = ((0x1E7 - i) << 6) | 0x10;
    }

    line0 = (u8*)&D_800E3FA8[0];
    line1 = line0 + 0xC0;
    for (i = 0; i < 0xC; i++) {
        setlen(&line0[i * 0x10], 3);
        setcode(&line0[i * 0x10], 0x60);
        setlen(&line1[i * 0x10], 3);
        setcode(&line1[i * 0x10], 0x60);
        setSemiTrans(&line0[i * 0x10], 1);
        setSemiTrans(&line1[i * 0x10], 1);
    }

    tile0 = (u8*)&D_800E3B28[0];
    tile1 = tile0 + 0x240;
    for (i = 0; i < 0x18; i++) {
        setlen(&tile0[i * 0x18], 5);
        setcode(&tile0[i * 0x18], 0x48);
        *(u32*)((u8*)&tile0[i * 0x18] + 0x14) = 0x55555555;
        setlen(&tile1[i * 0x18], 5);
        setcode(&tile1[i * 0x18], 0x48);
        *(u32*)((u8*)&tile1[i * 0x18] + 0x14) = 0x55555555;
    }

    if (GetGraphType() == 1 || GetGraphType() == 2) {
        tpage = 0x2F;
    } else {
        tpage = 0x1F;
    }
    dm0 = (u8*)&D_800E4128[0];
    dm1 = dm0 + 0x48;
    for (i = 0; i < 6; i++) {
        SetDrawMode((DR_MODE*)&dm0[i * 0xC], 0, 0, tpage, NULL);
        SetDrawMode((DR_MODE*)&dm1[i * 0xC], 0, 0, tpage, NULL);
    }
}

void InitFieldDebugPages(void) {
    FieldDebugPageInit(5, 0x6C, 0, 0x6C, 0x52);
    FieldDebugStringCopy(g_DebugText, "Authr:");
    FieldDebugStringConcat(g_DebugText, g_FieldScripts->author);
    AddStrNextDebugRow(5, g_DebugText);
    FieldDebugStringCopy(g_DebugText, "Event:");
    FieldDebugStringConcat(g_DebugText, g_FieldScripts->name);
    AddStrNextDebugRow(5, g_DebugText);
    AddStrNextDebugRow(5, "  Go");
    AddStrNextDebugRow(5, "  Stop");
    AddStrNextDebugRow(5, "  Step");
    SetStrToDebugRow(5, 5, "  Actor OFF");
    SetStrToDebugRow(5, 6, "  Info  OFF");
    FieldDebugPageHide(5);
    FieldDebugPageInit(4, 0x6C, 0x52, 0x6C, 0x52);
    AddStrNextDebugRow(4, D_800E0628);
    FieldDebugPageHide(4);
    FieldDebugPageInit(3, 0x6C, 0xA4, 0x6C, 0x5C);
    AddStrNextDebugRow(3, &D_800E0630);
    FieldDebugPageHide(3);
    FieldDebugPageInit(1, 0, 0, 0x6C, 0xCA);
    AddStrNextDebugRow(1, D_800E0628);
    FieldDebugPageHide(1);
    g_FieldScriptRunState = 3;
    D_8007EBCC = 4;
    D_8007EBDC = 8;
    g_FieldScriptDebugFlags = 0;
    g_DebugLevel = 0;
    g_FieldDebugStepRequest = 0;
    g_FieldDebugCurPage = 5;
    FieldDebugPageSetHeadRow(5, 4);
}

/* Move the first hidden debug page to (x, y, w, h) and clear its text, falling
 * back to page 0 when every page is currently being rendered.
 *
 * The element address has to go through `page` rather than being indexed
 * inline: as a bare `g_FieldDebugPageHidden[i * 378]` gcc hoists the symbol's
 * %hi/%lo out of the loop, where the original rematerialises it each iteration.
 */
s16 FieldDebugPagesResetPosSize(s16 x, s16 y, s16 w, s16 h) {
    s16 i;

    for (i = 0; i < 6; i++) {
        u8* page;

        page = &g_FieldDebugPageHidden[i * 378];
        if (*page) {
            FieldDebugPageSetPosSize(i, x, y, w, h);
            FieldDebugPageResetStrings(i);
            return i;
        }
    }
    FieldDebugPageSetPosSize(0, x, y, w, h);
    FieldDebugPageResetStrings(0);
    return 0;
}

/* `offClear` is deliberately a second copy of `off`: with one variable feeding
 * both the test and the store gcc coalesces the multiply into $v0 and needs an
 * extra `move` to get it into the address register. */
void FieldDebugPageInit(s16 page, s16 x, s16 y, s16 w, s16 h) {
    s32 off;
    s32 offClear;

    FieldDebugPageSetPosSize(page, x, y, w, h);
    off = page * 378;
    offClear = off;
    if (g_FieldDebugPageHidden[off] != 2) {
        FieldDebugPageResetStrings(page);
    } else {
        g_FieldDebugPageHidden[offClear] = 0;
        g_FieldDebugPagesDirty = 1;
    }
}

void FieldDebugPageSetPosSize(s16 page, s16 x, s16 y, s16 w, s16 h) {
    D_800E0748[page * 189] = x;
    g_FieldDebugPageY[page * 189] = y;
    D_800E074C[page * 189] = w;
    g_FieldDebugPageH[page * 189] = h;
    g_FieldDebugPagesDirty = 1;
}

/* The byte offset has to be a named local and the elements reached through a
 * `u8*` cast: written as `D_800E0748[page * 189] += x` the index needs a
 * scaling `sll`, and gcc folds the symbol's %hi/%lo into that same `addu`, so
 * one base register serves both the load and the store. With `off` already
 * holding the byte offset the address stays `(symbol)(reg)` in all four mems
 * and the assembler rematerialises it through $at each time, which is what the
 * original does. Splitting the `+=` into a load and a store is needed too --
 * a compound assignment computes the address once by construction. */
void FieldDebugPageAddPos(s16 page, s16 x, s16 y) {
    s32 off;
    s16 px;
    s16 py;

    g_FieldDebugPagesDirty = 1;
    off = page * 378;
    px = *(s16*)((u8*)D_800E0748 + off);
    py = *(s16*)((u8*)g_FieldDebugPageY + off);
    *(s16*)((u8*)D_800E0748 + off) = px + x;
    *(s16*)((u8*)g_FieldDebugPageY + off) = py + y;
}

/* Same shape as FieldDebugPageAddPos above; see the note there. */
void FieldDebugPageAddSize(s16 page, s16 w, s16 h) {
    s32 off;
    s16 pw;
    s16 ph;

    g_FieldDebugPagesDirty = 1;
    off = page * 378;
    pw = *(s16*)((u8*)D_800E074C + off);
    ph = *(s16*)((u8*)g_FieldDebugPageH + off);
    *(s16*)((u8*)D_800E074C + off) = pw + w;
    *(s16*)((u8*)g_FieldDebugPageH + off) = ph + h;
}

bool FieldDebugPageIsRender(s16 arg0) { return D_800E08C0[arg0 * 378] == 0; }

/* Blank all 24 rows of a debug page and restore its default colour. The row
 * text is a 14-byte record per row, the per-row colour a single byte, so the
 * two arrays walk the page at different strides. */
void FieldDebugPageResetStrings(s16 page) {
    s32 i;
    s32 off;
    u8* colors;

    i = 0;
    colors = &D_800E08A8[page * 378];
    off = page * 378;
    while (i < 24) {
        g_FieldDebugRowText[off] = 0;
        *colors++ = 0;
        i++;
        off += 14;
    }
    g_FieldDebugPageR[page * 378] = 7;
    g_FieldDebugPageG[page * 378] = 0xF;
    g_FieldDebugPageB[page * 378] = 0x1F;
    g_FieldDebugPageRow[page * 189] = 0;
    g_FieldDebugPageHeadRow[page * 189] = 0;
    g_FieldDebugPageHidden[page * 378] = 0;
    g_FieldDebugPagesDirty = 1;
}

static void FieldDebugRenderClear(void) {
    g_FieldDebugRChars = 0;
    g_FieldDebugRLines = 0;
    g_FieldDebugRRect = 0;
    g_FieldDebugRDm = 0;
    g_FieldDebugRb ^= 1;
}

/* Rebuilds the debug overlay into this frame's ordering table when anything
 * marked it dirty, then links that table into the caller's OT.
 *
 * The page counter and the byte offset have to be two independent induction
 * variables, and the visibility byte reached by bare subscript. Anything that
 * derives the offset from the counter (`g_FieldDebugPageHidden[page * 378]`,
 * with or without a pointer local) lets gcc strength-reduce the two into one
 * walking pointer with the symbol folded into its start value; giving it a
 * plain register index instead leaves the `symbol(reg)` addressing the original
 * has, which maspsx rematerialises through $at every iteration. */
void FieldDebugRender(u_long* ot) {
    s32 page;
    s32 off;

    if (g_FieldDebugPagesDirty) {
        FieldDebugRenderClear();
        page = 0;
        off = 0;
        ClearOTag(D_800E41C8[g_FieldDebugRb], 7);
        do {
            if (g_FieldDebugPageHidden[off] == 0) {
                FieldDebugRenderPage(page);
            }
            off += 378;
            page++;
        } while (page < 6);
        g_FieldDebugPagesDirty = 0;
    }
    addPrims(ot, D_800E41C8[g_FieldDebugRb], &D_800E41C8[g_FieldDebugRb][6]);
}

/* Draw one debug page's frame and fill into the debug ordering table. */
INCLUDE_ASM("asm/us/field/nonmatchings/field4", FieldDebugRenderPage);

/* Render one debug-page string's characters as GPU sprites into this frame's
 * half of the debug render buffer, advancing `x` by 8 per glyph and stopping
 * at the page's right edge or when the 0x158-sprite buffer is full. The
 * switch is the jump table at .rodata+0x3D8; the default arm maps digits,
 * upper case and lower case by three additive ranges. */
void FieldDebugRenderString(s16 page, s16 row, u8* str, s32 x, s32 y) {
    s32 off;
    s32 rowIdx;
    s32 colOff;
    s32 colBase;
    s32 rb;
    s16 chars;
    s32 rbOff;
    s32 charOff;
    s16 glyph;
    u8 ch;
    SPRT_16* sprite;

    while (*str != 0) {
        off = page * 378;
        if ((*(s16*)((u8*)D_800E0748 + off) + *(s16*)((u8*)D_800E074C + off) -
             8) < x) {
            break;
        }
        if (g_FieldDebugRChars >= 0x158) {
            break;
        }
        switch (*str) {
        case 0x20:
            glyph = 0x3F;
            break;
        case 0x3A:
            glyph = 0xD5;
            break;
        case 0x2E:
            glyph = 0xB2;
            break;
        case 0x2B:
            glyph = 0xB3;
            break;
        case 0x2F:
            glyph = 0xD4;
            break;
        case 0x2D:
            glyph = 0xD0;
            break;
        case 0x2A:
            glyph = 0xCF;
            break;
        case 0x21:
            glyph = 0xAE;
            break;
        case 0x3F:
            glyph = 0xAF;
            break;
        case 0x3D:
            glyph = 0xDA;
            break;
        case 0x23:
            glyph = 0xD6;
            break;
        case 0x3E:
            glyph = 0xD9;
            break;
        default:
            ch = *str;
            if (ch < 0x3A) {
                glyph = *str + 3;
            } else if (ch >= 0x61) {
                glyph = *str + 0x53;
            } else {
                glyph = *str + 0x73;
            }
            break;
        }
        str++;
        ((SPRT_16*)&D_800E1028[g_FieldDebugRChars * 0x10 +
                               g_FieldDebugRb * 0x1580])
            ->u0 = (glyph & 0xF) * 8 - 0x80;
        ((SPRT_16*)&D_800E1028[g_FieldDebugRChars * 0x10 +
                               g_FieldDebugRb * 0x1580])
            ->v0 = ((glyph >> 1) & 0x78) - 0x80;
        rb = g_FieldDebugRb;
        chars = g_FieldDebugRChars;
        rbOff = rb * 0x1580;
        charOff = chars * 0x10;
        sprite = (SPRT_16*)&D_800E1028[rbOff + charOff];
        sprite->x0 = x;
        sprite->y0 = y;
        rowIdx = row;
        colOff = page * 378;
        colBase = (s32)D_800E08A8;
        *(u16*)(D_800E1036 + rbOff + charOff) =
            D_800E4200[*(u8*)(colOff + colBase + rowIdx)];
        g_FieldDebugRChars = chars + 1;
        addPrim(
            &D_800E41C8[rb][page], (u8*)(rbOff + (s32)D_800E1028 + charOff));
        x += 8;
    }
}

/* Append a line to a debug page (no colour), wrapping back to the top row once
 * the page's pixel height can no longer hold another 10-pixel row. */
s32 AddStrNextDebugRow(s16 page, const char* str) {
    s32 off;
    char* rows;
    FieldDebugPageHdr* hdr;

    off = page * 378;
    rows = g_FieldDebugRowText + off;
    FieldDebugStringCopy(
        &rows[*(s16*)((u8*)g_FieldDebugPageHeadRow + off) * 14], str);
    hdr = (FieldDebugPageHdr*)(g_FieldDebugRowText - 0x10 + off);
    hdr->headRow = *(s16*)((u8*)g_FieldDebugPageHeadRow + off) + 1;
    if ((*(s16*)((u8*)g_FieldDebugPageH + off) - 8) / 10 <
        *(s16*)((u8*)g_FieldDebugPageHeadRow + off)) {
        *(s16*)((u8*)g_FieldDebugPageHeadRow + off) = 0;
    }
    g_FieldDebugPagesDirty = 1;
    return 1;
}

/* Append a coloured line to a debug page, wrapping back to the top row once the
 * page's pixel height can no longer hold another 10-pixel row. Same two
 * spellings as AddStrNextDebugRow above, plus the colour byte reached as
 * `g_FieldDebugRowText + 0x150 + off` so that its base is the live `rows`
 * register too. */
s32 AddColorStrNextDebugRow(s16 page, const char* str, u8 color) {
    s32 off;
    char* rows;
    u8* colors;
    FieldDebugPageHdr* hdr;
    s16 next;

    off = page * 378;
    rows = g_FieldDebugRowText + off;
    FieldDebugStringCopy(
        &rows[*(s16*)((u8*)g_FieldDebugPageHeadRow + off) * 14], str);
    colors = (u8*)(g_FieldDebugRowText + 0x150 + off);
    colors[*(s16*)((u8*)g_FieldDebugPageHeadRow + off)] = color;
    next = *(s16*)((u8*)g_FieldDebugPageHeadRow + off) + 1;
    hdr = (FieldDebugPageHdr*)(g_FieldDebugRowText - 0x10 + off);
    hdr->headRow = next;
    if ((*(s16*)((u8*)g_FieldDebugPageH + off) - 8) / 10 <
        *(s16*)((u8*)g_FieldDebugPageHeadRow + off)) {
        *(s16*)((u8*)g_FieldDebugPageHeadRow + off) = 0;
    }
    g_FieldDebugPagesDirty = 1;
    return 1;
}

s32 SetStrToDebugRow(s16 page, s16 row, const char* str) {
    char* rows = g_FieldDebugRowText + page * 378;

    FieldDebugStringCopy(&rows[row * 14], str);
    g_FieldDebugPagesDirty = 1;
    return 1;
}

s32 SetDebugStrRowColor(s16 page, s16 row, u8 color) {
    s32 offset = page * 378;
    s32 index = row;
    u8* colors = D_800E08A8 + offset;

    colors[index] = color;
    return 1;
}

void FieldDebugPageSetHeadRow(s16 page, s16 row) {
    g_FieldDebugPageRow[page * 189] = row;
    g_FieldDebugPagesDirty = 1;
}

void FieldDebugPageSetColor(s16 page, u8 r, u8 g, u8 b) {
    if (g_FieldDebugPageHidden[page * 378] == 0) {
        g_FieldDebugPageR[page * 378] = r;
        g_FieldDebugPageG[page * 378] = g;
        g_FieldDebugPageB[page * 378] = b;
        g_FieldDebugPagesDirty = 1;
    }
}

void FieldDebugPageNotInit(s16 page) {
    g_FieldDebugPageHidden[page * 378] = 1;
    g_FieldDebugPagesDirty = 1;
}

void FieldDebugPageHide(s16 page) {
    g_FieldDebugPageHidden[page * 378] = 2;
    g_FieldDebugPagesDirty = 1;
}

static void FieldDebugTranspSwitch(void) {
    g_FieldDebugTransp = (g_FieldDebugTransp + 1) & 3;
}

void FieldDebugStringCopy(char* dst, const char* src) {
    if (*src) {
        do {
            *dst++ = *src++;
        } while (*src != '\0');
    }
    *dst = '\0';
}

void FieldDebugStringConcat(char* dest, char* src) {
    if (*dest != '\0') {
        while (*++dest != '\0') {
        }
    }
    if (*src != '\0') {
        do {
            *dest++ = *src++;
        } while (*src != '\0');
    }
    *dest = '\0';
}

static s32 FieldDebugStringSize(char* src) {
    s32 len = 0;

    while (*src != '\0') {
        src++;
        len++;
    }
    return len;
}

static void FieldDebugStringPartCopy(char* dst, char* src, s32 len) {
    s32 i;
    for (i = len - 1; i != -1; i--) {
        *dst = *src;
        src++;
        dst++;
    }
}

void FieldDebugStringU8hex(s32 val, char* msg_out) {
    msg_out[1] = '\0';
    msg_out[0] = g_FieldDebugDigits[val & 0xF];
}

void FieldDebugStringU16hex(s32 val, char* msg_out) {
    msg_out[2] = '\0';
    msg_out[0] = g_FieldDebugDigits[(val & 0xF0) >> 4];
    msg_out[1] = g_FieldDebugDigits[val & 0xF];
}

void FieldDebugStringU32hex(s32 val, char* msg_out) {
    msg_out[4] = '\0';
    msg_out[0] = g_FieldDebugDigits[(val & 0xF000) >> 0xC];
    msg_out[1] = g_FieldDebugDigits[(val & 0xF00) >> 8];
    msg_out[2] = g_FieldDebugDigits[(val & 0xF0) >> 4];
    msg_out[3] = g_FieldDebugDigits[val & 0xF];
}

/* Writes value as decimal into out, suppressing leading zeros. Five digits
 * plus the units place, so the range is 0..99999. */
void FieldDebugIntToString(s32 value, char* out) {
    char* end;
    s32 started;
    s32 divisor;
    s32 count;
    s32 digit;
    u8 lastDigit;

    started = 0;
    divisor = 10000;
    count = 0;
    do {
        digit = value / divisor;
        if (started || digit != 0) {
            started = 1;
            out[count] = g_FieldDebugDigits[digit];
            count++;
        }
        value -= digit * divisor;
        divisor /= 10;
    } while (divisor >= 2);
    end = out + count;
    digit = g_FieldDebugDigits[value];
    lastDigit = digit;
    end[1] = '\0';
    end[0] = lastDigit;
}
