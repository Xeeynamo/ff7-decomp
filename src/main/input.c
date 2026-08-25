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
static s16 D_80062D76 = 0;

static u32 s_TutorialDelay;
static s32 D_80062E98;
static u8* s_Tutorial;

u8 func_8001F6B4(void);
void func_8001F6C0(u8* text, s8 palette); // PC: menu_setNotificationMessage
void func_8001F6E4(
    s16 enabled, s16 x, s16 y); // PC: menu_setNotificationWindowPosition

void SetupGamepad(void) {
    if (!s_PadsInitialized) {
        s_PadsInitialized = 1;
        StartPAD();
        InitPAD(&D_800696AC.padABuffer, 4, &D_800696AC.padBBuffer, 4);
    }
    g_TutorialActive = 0;
}

void func_8001C484(u8* tutorial) {
    s_Tutorial = tutorial;
    s_TutorialDelay = 20;
}

u16 func_8001C498(void) {
    u16 input;

    if (D_800696AC.padABuffer != 0xFF && D_800696AC.unk1 == 0x41) {
        input = ~(D_800696AC.unk3[0] | (D_800696AC.unk2 << 8));
    } else {
        input = 0;
    }
    return input;
}

static u8* func_8001C4E8(u8* txt) {
    s32 i, c;

    s_TutorialMessageVisible = 1;
    func_8001F6E4(1, s_TutorialMessageX, s_TutorialMessageY);
    func_8001F6C0(txt, 7);

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

static void func_8001C58C(void) {
    if (!func_8001F6B4()) {
        s_TutorialMessageVisible = 0;
    }
}

static u16 func_8001C5BC(void) {
    u16 button = 0;

    if (s_TutorialMessageVisible) {
        func_8001C58C();
    } else {
        switch (*s_Tutorial++) {
        case 0:
            s_TutorialDelay = *s_Tutorial++;
            s_TutorialDelay += *s_Tutorial << 8;
            s_Tutorial++;
            break;
        case 2:
            button = PADLup;
            s_TutorialDelay = 20;
            break;
        case 3:
            button = PADLdown;
            s_TutorialDelay = 20;
            break;
        case 4:
            button = PADLleft;
            s_TutorialDelay = 20;
            break;
        case 5:
            button = PADLright;
            s_TutorialDelay = 20;
            break;
        case 8:
            button = PADRleft;
            s_TutorialDelay = 20;
            break;
        case 9:
            button = PADRright;
            s_TutorialDelay = 20;
            break;
        case 6:
            button = PADRup;
            s_TutorialDelay = 20;
            break;
        case 7:
            button = PADRdown;
            s_TutorialDelay = 20;
            break;
        case 10:
            button = PADR1;
            s_TutorialDelay = 20;
            break;
        case 11:
            button = PADR2;
            s_TutorialDelay = 20;
            break;
        case 12:
            button = PADL1;
            s_TutorialDelay = 20;
            break;
        case 13:
            button = PADL2;
            s_TutorialDelay = 20;
            break;
        case 16:
            s_Tutorial = func_8001C4E8(s_Tutorial);
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
            func_8001F6E4(0, 0, 0);
            g_TutorialActive = 0;
            s_TutorialDelay = 0;
            break;
        }
    }
    return button;
}

static u16 func_8001C788(void) {
    u16 input = 0;

    if (s_TutorialDelay != 0) {
        s_TutorialDelay -= 1;
    } else if ((func_80023050() == 0 || func_80023050() == 1) &&
               SystemCdromReadChain() == 0) {
        input = func_8001C5BC();
    }
    return input;
}

u32 func_8001C808(void) {
    u16 inputA, inputB;

    if (!g_TutorialActive) {
        if (D_800696AC.padABuffer != 0xFF && D_800696AC.unk1 == 0x41) {
            inputA = ~(D_800696AC.unk3[0] | (D_800696AC.unk2 << 8));
        } else {
            inputA = 0;
        }
    } else {
        inputA = func_8001C788();
    }

    if (D_800696AC.padBBuffer != 0xFF && D_800696AC.unk23 == 0x41) {
        inputB = ~(D_800696AC.unk25[0] | (D_800696AC.unk24 << 8));
    } else {
        inputB = 0;
    }

    return inputA | (inputB << 16);
}

u32 func_8001C8D4(void) {
    u32 inputs, inputA, inputB;
    s32 i;

    inputs = func_8001C808();
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

void func_8001C980(void) {
    g_Pad2FastButtons = g_Pad2Buttons;
    g_Pad1FastButtons = g_Pad1Buttons;

    g_Pad1FastButtonsPressed = g_Pad1FastButtons ^ g_Pad1FastButtonsPrev;
    g_Pad1FastButtonsPressed &= g_Pad1FastButtons;
    if (g_Pad1FastButtons != g_Pad1FastButtonsPrev) {
        s_PadFastRepeatState.enabled[0] = 0;
        s_PadFastRepeatState.counter[0] = 0;
    } else if (s_PadFastRepeatState.enabled[0]) {
        if (s_PadFastRepeatState.counter[0] == 1) {
            g_Pad1FastButtonsRepeat = g_Pad1FastButtons;
            s_PadFastRepeatState.counter[0] = 0;
        } else {
            s_PadFastRepeatState.counter[0]++;
            g_Pad1FastButtonsRepeat = 0;
        }
    } else {
        if (s_PadFastRepeatState.counter[0] == 7) {
            s_PadFastRepeatState.enabled[0] = 1;
            s_PadFastRepeatState.counter[0] = 0;
        } else {
            s_PadFastRepeatState.counter[0]++;
        }
        g_Pad1FastButtonsRepeat = 0;
    }

    g_Pad2FastButtonsPressed = g_Pad2FastButtons ^ g_Pad2FastButtonsPrev;
    g_Pad2FastButtonsPressed &= g_Pad2FastButtons;
    if (g_Pad2FastButtons != g_Pad2FastButtonsPrev) {
        s_PadFastRepeatState.enabled[1] = 0;
        s_PadFastRepeatState.counter[1] = 0;
    } else if (s_PadFastRepeatState.enabled[1]) {
        if (s_PadFastRepeatState.counter[1] == 1) {
            g_Pad2FastButtonsRepeat = g_Pad2FastButtons;
            s_PadFastRepeatState.counter[1] = 0;
        } else {
            s_PadFastRepeatState.counter[1]++;
            g_Pad2FastButtonsRepeat = 0;
        }
    } else {
        if (s_PadFastRepeatState.counter[1] == 7) {
            s_PadFastRepeatState.enabled[1] = 1;
            s_PadFastRepeatState.counter[1] = 0;
        } else {
            s_PadFastRepeatState.counter[1]++;
        }
        g_Pad2FastButtonsRepeat = 0;
    }
    g_Pad1FastButtonsRepeat |= g_Pad1FastButtonsPressed;
    g_Pad2FastButtonsRepeat |= g_Pad2FastButtonsPressed;
    g_Pad1FastButtonsPrev = g_Pad1FastButtons;
    g_Pad2FastButtonsPrev = g_Pad2FastButtons;
}

void func_8001CB48(void) {
    s32 i;
    u32 inputs;

    inputs = func_8001C808();
    g_Pad2Buttons = inputs >> 16;
    if (((Savemap.config >> 2) & 3) && !g_TutorialActive) {
        g_Pad1Buttons = 0;
        i = 0;
        do {
            if (inputs & (1 << i)) {
                g_Pad1Buttons |= 1 << Savemap.button_config[i];
            }
            i++;
        } while (i < 16);
    } else {
        g_Pad1Buttons = inputs;
    }

    g_Pad1ButtonsRepeat = 0;
    g_Pad1ButtonsPressed = g_Pad1Buttons ^ g_Pad1ButtonsPrev;
    g_Pad1ButtonsPressed &= g_Pad1Buttons;
    if (g_Pad1Buttons != g_Pad1ButtonsPrev) {
        s_PadRepeatState.counter[0] = 0;
        s_PadRepeatState.enabled[0] = 0;
    } else if (s_PadRepeatState.enabled[0]) {
        if (s_PadRepeatState.counter[0] == 3) {
            g_Pad1ButtonsRepeat = g_Pad1Buttons;
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

    g_Pad2ButtonsPressed = g_Pad2Buttons ^ g_Pad2ButtonsPrev;
    g_Pad2ButtonsPressed &= g_Pad2Buttons;
    if (g_Pad2Buttons != g_Pad2ButtonsPrev) {
        s_PadRepeatState.enabled[1] = 0;
        s_PadRepeatState.counter[1] = 0;
    } else if (s_PadRepeatState.enabled[1]) {
        if (s_PadRepeatState.counter[1] == 3) {
            g_Pad2ButtonsRepeat = g_Pad2Buttons;
            s_PadRepeatState.counter[1] = 0;
        } else {
            s_PadRepeatState.counter[1]++;
            g_Pad2ButtonsRepeat = 0;
        }
    } else {
        if (s_PadRepeatState.counter[1] == 15) {
            s_PadRepeatState.enabled[1] = 1;
            s_PadRepeatState.counter[1] = 0;
        } else {
            s_PadRepeatState.counter[1]++;
        }
        g_Pad2ButtonsRepeat = 0;
    }

    g_Pad1ButtonsRepeat |= g_Pad1ButtonsPressed;
    g_Pad2ButtonsRepeat |= g_Pad2ButtonsPressed;
    g_Pad1ButtonsPrev = g_Pad1Buttons;
    g_Pad2ButtonsPrev = g_Pad2Buttons;
}
