//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>

extern u8 D_800E08C0[];
extern char D_800E0628[];
extern char D_800E0630[];
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
extern u16 g_FieldDebugRb;
extern s16 g_FieldDebugRChars;
extern s16 g_FieldDebugRLines;
extern s16 g_FieldDebugRRect;
extern s16 g_FieldDebugRDm;
extern u16 g_FieldDebugTransp;
extern char g_DebugText[];
extern char g_DebugMessageBuffer[];

void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind);
void FieldWindowReset(s16 window);
void AddStrNextDebugRow(s32 val, const char* msg_out);
void FieldDebugStringCopy(char* dst, const char* src);
void FieldDebugStringConcat(char* dest, char* src);
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
    D_80095DCC = 1;
    D_80099FFC = 4;
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

void FieldDialogSetWindowStyleCbc(s16 window, s8 style, s16 preventClose) {
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

void FieldDialogSetWindowHeight(s16 window, s16 height) { g_WindowData[window].height = height; }

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
                (g_WindowData[window].height - 9) / 16 - 1 + g_WindowExtraRows[window]) {
                g_WindowData[window].state = WSTATE_SCROLL_ROW;
                g_WindowData[window].textScrolling -= 2;
                g_WindowExtraRows[window]++;
            }
        }
        break;
    case WSTATE_TXT_DONE:
        if (!(g_WindowData[window].preventClose & 1) && (g_pFieldState->pressedKeys & PADRright)) {
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
            g_WindowTotalRowsHeight[window] = g_WindowData[window].currentRow * 16 + 17;
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

s32 FieldDialogAskUpdateStates(u8 window, u8 message, u8 first, u8 last, s16* selectedLine) {
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
                (g_WindowData[window].height - 9) / 16 - 1 + g_WindowExtraRows[window]) {
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
            g_WindowTotalRowsHeight[window] = g_WindowData[window].currentRow * 16 + 17;
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

    g_WindowTextBudget[window] += characterCost * (g_WindowFastForwardLevel[window] >> 4) + baseCredit;

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
                (g_WindowData[window].height - 9) / 16 - 1 + g_WindowExtraRows[window]) {
                g_WindowData[window].state = WSTATE_WAIT_ROW;
                g_WindowFastForwardLevel[window] = 1;
                g_WindowTextBudget[window] = 0;
                goto end;
            }
            g_WindowString[window][g_WindowData[window].stringByteLength] = *g_WindowStringPtr[window];
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
                g_WindowString[window][g_WindowData[window].stringByteLength] = 0;
                g_WindowData[window].stringByteLength++;
                g_WindowData[window].stringLength++;
            }
            continue;

        // Write 10 spaces.
        case 0xE0:
            g_WindowStringPtr[window]++;
            for (i = 0; i < 10; i++) {
                g_WindowString[window][g_WindowData[window].stringByteLength] = 0;
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
            g_WindowString[window][g_WindowData[window].stringByteLength] = 0xA9;
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
            if (name[g_WindowNameCopyCount[window]] == 0xFF || g_WindowNameCopyCount[window] >= 9) {
                g_WindowStringPtr[window]++;
                g_WindowNameCopyCount[window] = 0;
            } else {
                g_WindowString[window][g_WindowData[window].stringByteLength] = name[g_WindowNameCopyCount[window]];
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
                    g_WindowString[window][g_WindowData[window].stringByteLength] = 0xA9;
                    g_WindowData[window].stringByteLength++;
                    g_WindowNameCopyCount[window]++;
                    g_WindowData[window].stringLength++;
                    g_WindowTextBudget[window] -= characterCost;
                }
            } else {
                // Party member exists. Get player-chosen name from savemap.
                name = GetCharacterName(value);
                if (name[g_WindowNameCopyCount[window]] == 0xFF || g_WindowNameCopyCount[window] >= 9) {
                    g_WindowStringPtr[window]++;
                    g_WindowNameCopyCount[window] = 0;
                } else {
                    g_WindowString[window][g_WindowData[window].stringByteLength] = name[g_WindowNameCopyCount[window]];
                    g_WindowData[window].stringByteLength++;
                    g_WindowNameCopyCount[window]++;
                    g_WindowData[window].stringLength++;
                    g_WindowTextBudget[window] -= characterCost;
                }
            }
            continue;

        // Opcode prefix.
        case 0xFE:
            g_WindowString[window][g_WindowData[window].stringByteLength] = *g_WindowStringPtr[window];
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
                    g_WindowBuffer[window][g_WindowBufferPos[window]] == 0xFF || g_WindowBufferPos[window] >= 16) {
                    // Last converted character has been copied.
                    g_WindowStringPtr[window] += 2;
                    g_WindowBufferPos[window] = -1;
                    g_WindowReplaceParam[window]++;
                } else {
                    // Copy the next character of the converted integer.
                    g_WindowString[window][g_WindowData[window].stringByteLength] =
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
                        g_WindowBuffer[window][i] = Savemap.memory_bank_1[value + i];
                    }
                    g_WindowBuffer[window][i] = 0xFF;
                    g_WindowBufferPos[window]++;
                } else if (g_WindowBuffer[window][g_WindowBufferPos[window]] == 0xFF) {
                    g_WindowStringPtr[window] += 6;
                    g_WindowBufferPos[window] = -1;
                } else {
                    g_WindowString[window][g_WindowData[window].stringByteLength] =
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
                g_WindowString[window][g_WindowData[window].stringByteLength] = *g_WindowStringPtr[window];
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
                g_WindowString[window][g_WindowData[window].stringByteLength] = *g_WindowStringPtr[window];
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
            g_WindowString[window][g_WindowData[window].stringByteLength] = *g_WindowStringPtr[window];
            g_WindowStringPtr[window]++;
            g_WindowData[window].stringByteLength++;

        // Fall through to copy the second byte of the character.
        // Also used to copy all other characters directly.
        default:
            g_WindowString[window][g_WindowData[window].stringByteLength] = *g_WindowStringPtr[window];
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

    if (g_WindowData[window].textScrolling + g_WindowTotalRowsHeight[window] > 0) {
        g_WindowData[window].textScrolling -= g_WindowFastForwardLevel[window] >> 2;
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

    if (g_WindowData[window].currentWidth < 9 && g_WindowData[window].currentHeight < 9) {
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
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] + 0x100;
        value = Savemap.memory_bank_1[offset];
        break;
    case 4:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] + 0x100;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 11:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] + 0x200;
        value = Savemap.memory_bank_1[offset];
        break;
    case 12:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] + 0x200;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 13:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] + 0x300;
        value = Savemap.memory_bank_1[offset];
        break;
    case 15:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] + 0x400;
        value = Savemap.memory_bank_1[offset];
        break;
    case 14:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] + 0x300;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 7:
        offset = g_WindowReplaceBankAddr[window][g_WindowReplaceParam[window]] + 0x400;
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

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugInitBuffers);

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
    AddStrNextDebugRow(3, D_800E0630);
    FieldDebugPageHide(3);
    FieldDebugPageInit(1, 0, 0, 0x6C, 0xCA);
    AddStrNextDebugRow(1, D_800E0628);
    FieldDebugPageHide(1);
    D_80099FFC = 3;
    D_8007EBCC = 4;
    D_8007EBDC = 8;
    D_80071E24 = 0;
    g_DebugLevel = 0;
    D_80070788 = 0;
    g_FieldDebugCurPage = 5;
    FieldDebugPageSetHeadRow(5, 4);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPagesResetPosSize);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageSetPosSize);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageAddPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageAddSize);

bool FieldDebugPageIsRender(s16 arg0) { return D_800E08C0[arg0 * 378] == 0; }

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageResetStrings);

static void FieldDebugRenderClear(void) {
    g_FieldDebugRChars = 0;
    g_FieldDebugRLines = 0;
    g_FieldDebugRRect = 0;
    g_FieldDebugRDm = 0;
    g_FieldDebugRb ^= 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugRenderPage);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugRenderString);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", AddStrNextDebugRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", AddColorStrNextDebugRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", SetStrToDebugRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", SetDebugStrRowColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageSetHeadRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageSetColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageNotInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugPageHide);

static void FieldDebugTranspSwitch(void) { g_FieldDebugTransp = (g_FieldDebugTransp + 1) & 3; }

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

INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldDebugIntToString);
