//! PSYQ=3.3 CC1=2.7.2 G=8
#include "main_private.h"
#include <libetc.h>

typedef struct PadRepeatState {
    s16 enabled[2];
    s16 counter[2];
} PadRepeatState;

static PadRepeatState s_PadRepeatState = {{0, 0}, {0, 0}};
static PadRepeatState s_PadFastRepeatState = {{0, 0}, {0, 0}};
static u8 s_PadsInitialized = 0;
static u8 s_TutorialMessageVisible = 0;
static s16 s_TutorialMessageX = 0;
static s16 s_TutorialMessageY = 0;
static s16 D_80062D76 = 0; // Unused

static u32 s_TutorialDelay;
static s32 D_80062E98; // Unused
static u8* s_Tutorial;

/*
Receive buffer data format for 16 button pad:
Offset: Contents:
0       Received result, 0x00: Success, 0xFF: Failure
1       Upper 4 bits: 4 (terminal type)
        Lower 4 bits: Number of received bytes/2 (always 1 for 16 button pad)
2,3     Key status bits, 1: Release, 0: Push

Max possible number of received bytes is 32 (when offset 1, lower 4 bits = 0)
so Psy-Q recommends allocating 34 bytes.

Macros inspired by CTRLLER.H from SDK samples.
*/
#define GOOD_DATA(x) ((x)[0] != 0xff)
#define GOOD_FORMAT(x) ((x)[1] == 0x41)
#define GET_KEYS(x) (~((x)[3] | ((x)[2] << 8)))

static u8 s_PadBuffers[2][34];

u8 func_8001F6B4(void);
// PC: menu_setNotificationMessage
void SysMenuRequestAddWindow(u8* text, s8 palette);
// PC: menu_setNotificationWindowPosition
void SysMenuSetPosAddWindow(s16 enabled, s16 x, s16 y);

void InputInit(void) {
    if (!s_PadsInitialized) {
        s_PadsInitialized = 1;
        StartPAD();
        InitPAD(s_PadBuffers[0], 4, s_PadBuffers[1], 4);
    }
    g_TutorialActive = 0;
}

void TutorialInit(u8* tutorial) {
    s_Tutorial = tutorial;
    s_TutorialDelay = 20;
}

// Returns key states as read directly from pad 1.
// Tutorial override and player-configured remapping are ignored.
u16 InputReadPad1Raw(void) {
    u16 input;

    if (GOOD_DATA(s_PadBuffers[0]) && GOOD_FORMAT(s_PadBuffers[0])) {
        input = GET_KEYS(s_PadBuffers[0]);
    } else {
        input = 0;
    }
    return input;
}

static u8* TutorialShowMessage(u8* txt) {
    s32 i, c;

    s_TutorialMessageVisible = 1;
    SysMenuSetPosAddWindow(1, s_TutorialMessageX, s_TutorialMessageY);
    SysMenuRequestAddWindow(txt, 7);

    for (i = 0; i < 256; i++) {
        c = *txt;
        if (c < 0xFF) {
            if (c < 0xFA) {
                txt++;
            } else {
                txt += 2;
            }
        } else {
            txt++;
        }
        if (*txt == 0xFF) {
            txt++;
            break;
        }
    }
    return txt;
}

static void TutorialUpdateMessageState(void) {
    if (!func_8001F6B4()) {
        s_TutorialMessageVisible = 0;
    }
}

static u16 TutorialDoNextEvent(void) {
    u16 key = 0;

    if (s_TutorialMessageVisible) {
        TutorialUpdateMessageState();
    } else {
        switch (*s_Tutorial++) {
        case 0:
            s_TutorialDelay = *s_Tutorial++;
            s_TutorialDelay += *s_Tutorial << 8;
            s_Tutorial++;
            break;
        case 2:
            key = PADLup;
            s_TutorialDelay = 20;
            break;
        case 3:
            key = PADLdown;
            s_TutorialDelay = 20;
            break;
        case 4:
            key = PADLleft;
            s_TutorialDelay = 20;
            break;
        case 5:
            key = PADLright;
            s_TutorialDelay = 20;
            break;
        case 8:
            key = PADRleft;
            s_TutorialDelay = 20;
            break;
        case 9:
            key = PADRright;
            s_TutorialDelay = 20;
            break;
        case 6:
            key = PADRup;
            s_TutorialDelay = 20;
            break;
        case 7:
            key = PADRdown;
            s_TutorialDelay = 20;
            break;
        case 10:
            key = PADR1;
            s_TutorialDelay = 20;
            break;
        case 11:
            key = PADR2;
            s_TutorialDelay = 20;
            break;
        case 12:
            key = PADL1;
            s_TutorialDelay = 20;
            break;
        case 13:
            key = PADL2;
            s_TutorialDelay = 20;
            break;
        case 16:
            s_Tutorial = TutorialShowMessage(s_Tutorial);
            s_TutorialDelay = 80;
            break;
        case 18:
            s_TutorialMessageX = *s_Tutorial++;
            s_TutorialMessageX += *s_Tutorial << 8;
            s_Tutorial++;
            s_TutorialMessageY = *s_Tutorial++;
            s_TutorialMessageY += *s_Tutorial << 8;
            s_Tutorial++;
            s_TutorialDelay = 60;
            break;
        case 17:
            SysMenuSetPosAddWindow(0, 0, 0);
            g_TutorialActive = 0;
            s_TutorialDelay = 0;
            break;
        }
    }
    return key;
}

static u16 TutorialFrameUpdate(void) {
    u16 input = 0;

    if (s_TutorialDelay != 0) {
        s_TutorialDelay -= 1;
    } else if ((SysMenuGetMenuListState() == 0 || SysMenuGetMenuListState() == 1) && SystemCdromReadChain() == 0) {
        input = TutorialDoNextEvent();
    }
    return input;
}

// Returns key states directly from both pads.
// If tutorial is active, its synthetic key state replaces pad 1.
u32 InputReadPadsRaw(void) {
    u16 inputA, inputB;

    if (!g_TutorialActive) {
        if (GOOD_DATA(s_PadBuffers[0]) && GOOD_FORMAT(s_PadBuffers[0])) {
            inputA = GET_KEYS(s_PadBuffers[0]);
        } else {
            inputA = 0;
        }
    } else {
        inputA = TutorialFrameUpdate();
    }

    if (GOOD_DATA(s_PadBuffers[1]) && GOOD_FORMAT(s_PadBuffers[1])) {
        inputB = GET_KEYS(s_PadBuffers[1]);
    } else {
        inputB = 0;
    }

    return inputA | (inputB << 16);
}

// Same as InputReadPadsRaw, but player configured remapping is applied to
// pad 1.
u32 InputReadPads(void) {
    u32 inputs, inputA, inputB;
    s32 i;

    inputs = InputReadPadsRaw();
    inputB = inputs & 0xFFFF0000;
    if (((Savemap.config >> 2) & 3) && !g_TutorialActive) {
        inputA = 0;
        for (i = 0; i < 16; i++) {
            if (inputs & (1 << i)) {
                inputA |= (1 << Savemap.button_config[i]);
            }
        }
    } else {
        inputA = inputs & 0xFFFF;
    }
    return inputA | inputB;
}

// Called by battle to update key states at 15 fps.
// Uses states previously read from pads by InputUpdateKeyStates.
void InputUpdateBattleKeyStates(void) {
    g_Pad2BattleKeys = g_Pad2Keys;
    g_Pad1BattleKeys = g_Pad1Keys;

    g_Pad1BattleKeysPressed = g_Pad1BattleKeys ^ g_Pad1BattleKeysPrev;
    g_Pad1BattleKeysPressed &= g_Pad1BattleKeys;

    /*
    State machine for repeating keys.
    First repeat is triggered 10 frames (~667 ms) after key press,
    then every 2 frames (~133 ms) after that.
    g_Pad1BattleKeysRepeat is not cleared per call so if a release
    immediately follows a repeat, that repeat state can survive through the
    next call to this function.
    */
    if (g_Pad1BattleKeys != g_Pad1BattleKeysPrev) {
        s_PadFastRepeatState.enabled[0] = 0;
        s_PadFastRepeatState.counter[0] = 0;
    } else if (s_PadFastRepeatState.enabled[0]) {
        if (s_PadFastRepeatState.counter[0] == 1) {
            g_Pad1BattleKeysRepeat = g_Pad1BattleKeys;
            s_PadFastRepeatState.counter[0] = 0;
        } else {
            s_PadFastRepeatState.counter[0]++;
            g_Pad1BattleKeysRepeat = 0;
        }
    } else {
        if (s_PadFastRepeatState.counter[0] == 7) {
            s_PadFastRepeatState.enabled[0] = 1;
            s_PadFastRepeatState.counter[0] = 0;
        } else {
            s_PadFastRepeatState.counter[0]++;
        }
        g_Pad1BattleKeysRepeat = 0;
    }

    g_Pad2BattleKeysPressed = g_Pad2BattleKeys ^ g_Pad2BattleKeysPrev;
    g_Pad2BattleKeysPressed &= g_Pad2BattleKeys;
    if (g_Pad2BattleKeys != g_Pad2BattleKeysPrev) {
        s_PadFastRepeatState.enabled[1] = 0;
        s_PadFastRepeatState.counter[1] = 0;
    } else if (s_PadFastRepeatState.enabled[1]) {
        if (s_PadFastRepeatState.counter[1] == 1) {
            g_Pad2BattleKeysRepeat = g_Pad2BattleKeys;
            s_PadFastRepeatState.counter[1] = 0;
        } else {
            s_PadFastRepeatState.counter[1]++;
            g_Pad2BattleKeysRepeat = 0;
        }
    } else {
        if (s_PadFastRepeatState.counter[1] == 7) {
            s_PadFastRepeatState.enabled[1] = 1;
            s_PadFastRepeatState.counter[1] = 0;
        } else {
            s_PadFastRepeatState.counter[1]++;
        }
        g_Pad2BattleKeysRepeat = 0;
    }
    g_Pad1BattleKeysRepeat |= g_Pad1BattleKeysPressed;
    g_Pad2BattleKeysRepeat |= g_Pad2BattleKeysPressed;
    g_Pad1BattleKeysPrev = g_Pad1BattleKeys;
    g_Pad2BattleKeysPrev = g_Pad2BattleKeys;
}

// The main key state update function. Called normally at each v-sync.
// Reads key states from pads, applies remapping (if tutorial is inactive),
// and updates state globals.
void InputUpdateKeyStates(void) {
    s32 i;
    u32 inputs;

    inputs = InputReadPadsRaw();
    g_Pad2Keys = inputs >> 16;
    if (((Savemap.config >> 2) & 3) && !g_TutorialActive) {
        g_Pad1Keys = 0;
        i = 0;
        do {
            if (inputs & (1 << i)) {
                g_Pad1Keys |= 1 << Savemap.button_config[i];
            }
            i++;
        } while (i < 16);
    } else {
        g_Pad1Keys = inputs;
    }

    g_Pad1KeysRepeat = 0;
    g_Pad1KeysPressed = g_Pad1Keys ^ g_Pad1KeysPrev;
    g_Pad1KeysPressed &= g_Pad1Keys;

    // State machine for repeating keys.
    // First repeat is triggered 20 frames (~330 ms) after key press,
    // then every 4 frames (~67 ms) after that.
    // State machine works on the entire key state so pressing or releasing
    // any key will reset repeat state for all keys.
    if (g_Pad1Keys != g_Pad1KeysPrev) {
        s_PadRepeatState.counter[0] = 0;
        s_PadRepeatState.enabled[0] = 0;
    } else if (s_PadRepeatState.enabled[0]) {

        if (s_PadRepeatState.counter[0] == 3) {
            g_Pad1KeysRepeat = g_Pad1Keys;
            s_PadRepeatState.counter[0] = 0;
        } else {
            s_PadRepeatState.counter[0]++;
        }
    } else {

        if (s_PadRepeatState.counter[0] == 15) {
            s_PadRepeatState.enabled[0] = 1;
            s_PadRepeatState.counter[0] = 0;
        } else {
            s_PadRepeatState.counter[0]++;
        }
    }

    g_Pad2KeysPressed = g_Pad2Keys ^ g_Pad2KeysPrev;
    g_Pad2KeysPressed &= g_Pad2Keys;
    if (g_Pad2Keys != g_Pad2KeysPrev) {
        s_PadRepeatState.enabled[1] = 0;
        s_PadRepeatState.counter[1] = 0;
    } else if (s_PadRepeatState.enabled[1]) {
        if (s_PadRepeatState.counter[1] == 3) {
            g_Pad2KeysRepeat = g_Pad2Keys;
            s_PadRepeatState.counter[1] = 0;
        } else {
            s_PadRepeatState.counter[1]++;
            g_Pad2KeysRepeat = 0;
        }
    } else {
        if (s_PadRepeatState.counter[1] == 15) {
            s_PadRepeatState.enabled[1] = 1;
            s_PadRepeatState.counter[1] = 0;
        } else {
            s_PadRepeatState.counter[1]++;
        }
        g_Pad2KeysRepeat = 0;
    }

    // Pressed keys are added to repeat globals so they can be used
    // to detect both new key presses and repeats.
    g_Pad1KeysRepeat |= g_Pad1KeysPressed;
    g_Pad2KeysRepeat |= g_Pad2KeysPressed;
    g_Pad1KeysPrev = g_Pad1Keys;
    g_Pad2KeysPrev = g_Pad2Keys;
}
