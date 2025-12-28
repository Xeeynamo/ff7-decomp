//! PSYQ=3.3 CC1=2.7.2
#include <game.h>

static s32 D_801D1AA8 = -1;
static u8 D_801D1AAC[] = {
    0x07, 0x07, 0x05, 0x07, 0x06, 0x07, 0x04, 0x07, 0x03, 0x0A, 0x00, 0x0A,
    0x01, 0x0A, 0x02, 0x0A, 0x08, 0x07, 0x00, 0x00, 0x00, 0x00, 0x09, 0x07,
    0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00,
    0x40, 0x00, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
static u8 D_801D1ADB = 8;
static u8 D_801D1ADC[] = {
    0x05, 0x06, 0x04, 0x07, 0x02, 0x03, 0x00, 0x01, 0x08, 0x0B, 0x00, 0x00};
static unsigned char D_801D1AE8[][0x30] = {
    _S("Window color"),   // 0
    _S("Sound"),          // 1
    _S("Controller"),     // 2
    _S("Cursor"),         // 3
    _S("ATB"),            // 4
    _S("Battle speed"),   // 5
    _S("Battle message"), // 6
    _S("Field message"),  // 7
    _S("Camera angle"),   // 8
    _S("Select"),         // 9
    _S("Cancel"),         // 10
    _S("Menu"),           // 11
    _S("Window OFF"),     // 12
    _S("Pause"),          // 13
    _S("Battle help"),    // 14
    _S("Mono"),           // 15
    _S("Stereo"),         // 16
    _S("Wide"),           // 17
};
static unsigned char D_801D1E48[][0x30] = {
    _S("Normal"),
    _S("Customize"),
};
static unsigned char D_801D1EA8[][0x30] = {
    _S("Initial"),
    _S("Memory"),
};
static unsigned char D_801D1F08[][0x30] = {
    _S("Active"),      // 0
    _S("Recommended"), // 1
    _S("Wait"),        // 2
    _S("Auto"),        // 3
    _S("Fix"),         // 4
    _S(""),            // 5
    _S("Slow"),        // 6
    _S("Fast"),        // 7
    _S("RED"),         // 8
    _S("GREEN"),       // 9
    _S("BLUE"),        // 10
    _S("Magic order"), // 11
    _S("restore"),     // 12
    _S("attack"),      // 13
    _S("indirect"),    // 14
    _S("forbidden"),   // 15
    _S("No."),         // 16
};
static unsigned char D_801D2238[][0x30] = {
    _S("Left2"),  // 0
    _S("Right2"), // 1
    _S("Left1"),  // 2
    _S("Right1"), // 3
    _S("Menu"),   // 4
    _S("Select"), // 5
    _S("Cancel"), // 6
    _S("EXT"),    // 7
    _S("Help"),   // 8
};
static unsigned char D_801D23E8[][0x30] = {
    {0x30, 0x52, 0x45, 0x53, 0x53, 0x00, 0xB2, 0x33, 0x34, 0x21, 0x32, 0x34,
     0xB3, 0x00, 0x54, 0x4F, 0x00, 0x43, 0x55, 0x53, 0x54, 0x4F, 0x4D, 0x49,
     0x5A, 0x45, 0x0E, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};
static unsigned char D_801D2418[][0x30] = {
    _S("Press Directional button Left or Right to end."),
    _S("Pause"),
};
static u8 D_801D2478[][6] = {
    {0xC5, 0xB8, 0xB7, 0xFF, 0x00, 0x00}, // RED
    {0xBA, 0xC5, 0xB8, 0xB8, 0xC1, 0xFF}, // BLUE
    {0xB5, 0xBF, 0xC8, 0xB8, 0xFF, 0x00}, // GREEN
};
static u8 D_801D248C[] = {
    0x00, 0x01, 0x02, 0x00, 0x02, 0x01, 0x01, 0x02, 0x00, 0x01,
    0x00, 0x02, 0x02, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00,
};
static RECT D_801D24A0[2] = {
    {0, 5, 364, 24},
    {0, 29, 364, 195},
};
static u8 D_801D24B0[] = {
    // maybe this is an unused string?
    0x53, 0x69, 0x17, 0x61, 0xA1, 0x69, 0xFF,
};
struct Pad {
    u16 press;
    u16 prev;
};
static struct Pad D_801D24B8 = {0, 0};
static u16 D_801D24BC = 0;
static u8 D_801D24C0[] = {4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0};

// from now on this is the BSS section; this can be moved up if necessary
static CursorPicker D_801D24CC[4];
static s32 unused[5];
static s32 D_801D2528;
static u8 D_801D252C[LEN(D_80049208)];

static void PlaySound(u16 arg0) {
    D_8009A000 = 0x30;
    D_8009A004 = arg0;
    D_8009A008 = arg0;
    func_8002DA7C(arg0);
}

static void func_801D0080(s32 arg0) {
    switch (arg0) {
    case 0:
        D_8009A000 = 0x81;
        D_8009A004 = 0x81;
        D_8009A008 = 0x81;
        break;
    case 1:
        D_8009A000 = 0x80;
        D_8009A004 = 0x80;
        D_8009A008 = 0x80;
        break;
    case 2:
        D_8009A000 = 0x82;
        D_8009A004 = 0x82;
        D_8009A008 = 0x82;
        break;
    }
    func_8002DA7C();
}

static u8 func_801D0118(u16 arg0) {
    s32 i;
    for (i = 0; i < 12; i++) {
        if ((arg0 >> i) & 1) {
            return i;
        }
    }
    return 0xFF;
}

static void func_801D014C(s16 x, s16 y, s32 value) {
    u8 uv = D_801D1AAC[value * 2 + 0];
    u8 page = D_801D1AAC[value * 2 + 1];
    MenuDrawBtlImage(
        x, y, (uv & 3) * 16 + 96, (uv >> 2) * 16 + 64, 16, 16, page, 0);
}

static void func_801D01C8(void) {
    RECT rect;
    s32 temp_s4;
    s32 y;
    s32 x;
    s32 var_s2;
    s32 i;
    s32 _setting;
    u8* setting;

    y = D_801D24A0[1].y + 33;
    MenuDrawText(40, D_801D24A0[1].y + 14, D_801D1AE8[0], 5);
    for (i = 1; i < 9; i++) {
        MenuDrawText(40, y + (i - 1) * 18, D_801D1AE8[i], 5);
    }
    MenuDrawText(40, y + (i - 1) * 18, D_801D1AE8[33], 5);
    for (i = 0; i < 2; i++) {
        MenuDrawText(165 + i * 65, y, D_801D1AE8[15 + i],
                     -((Savemap.config & 3) == i) & 7);
    }
    for (i = 0; i < 2; i++) {
        MenuDrawText(165 + i * 65, y + 18, D_801D1E48[i],
                     -(((Savemap.config >> 2) & 3) == i) & 7);
    }
    for (i = 0; i < 2; i++) {
        MenuDrawText(165 + i * 65, y + 0x24, D_801D1EA8[i],
                     -(((Savemap.config >> 4) & 3) == i) & 7);
    }
    temp_s4 = y + 54;
    MenuDrawText(
        165, temp_s4, D_801D1F08[0], -(((Savemap.config >> 6) & 3) == 0) & 7);
    x = MenuGetTextWidth(D_801D1F08[0]);
    MenuDrawText(x + 175, temp_s4, D_801D1F08[1],
                 -(((Savemap.config >> 6) & 3) == 1) & 7);
    MenuDrawText(x + MenuGetTextWidth(D_801D1F08[1]) + 185, temp_s4,
                 D_801D1F08[2], -(((Savemap.config >> 6) & 3) == 2) & 7);
    for (i = 0; i < 2; i++) {
        MenuDrawText(165 + i * 65, y + 126, D_801D1F08[3 + i],
                     -(((Savemap.config >> 8) & 3) == i) & 7);
    }
    for (i = 0; i < 3; i++) {
        _setting = D_801D248C[((Savemap.config >> 10) & 7) * 3 + i];
        setting = &D_801D248C[((Savemap.config >> 10) & 7) * 3];
        MenuDrawText(189 + i * 52, y + 0x90, D_801D1F08[12 + setting[i]], 7);
    }
    MenuDrawText(149, y + 0x90, D_801D1F08[16], 7);

    // battle speed value
    rect.y = y + 72;
    rect.w = 8;
    rect.h = 11;
    rect.x = (Savemap.battle_speed >> 1) + 184;
    MenuDrawSlider(&rect);

    // battle message speed value
    rect.y = y + 0x5A;
    rect.w = 8;
    rect.h = 11;
    rect.x = (Savemap.battle_msg_speed >> 1) + 184;
    MenuDrawSlider(&rect);

    // field message speed value
    rect.y = y + 108;
    rect.w = 8;
    rect.h = 11;
    rect.x = (Savemap.field_msg_speed >> 1) + 184;
    MenuDrawSlider(&rect);

    // speed values
    for (i = 0; i < 3; i++) {
        MenuDrawText(157, y + 72 + i * 18 + 2, D_801D1F08[7], 7);
        MenuDrawText(324, y + 72 + i * 18 + 2, D_801D1F08[6], 7);
    }

    // speed bars
    for (i = 0; i < 3; i++) {
        rect.x = 184;
        rect.y = y + 72 + i * 18;
        rect.w = 136;
        rect.h = 11;
        MenuDrawSliderBg(&rect);
    }

    // magic order ID
    MenuDrawNumberPadded(173, y + 146, ((Savemap.config >> 0xA) & 7) + 1, 1, 7);

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 0x100;
    MenuSetDrawMode(0, 1, 0x1F, &rect);
}

// exported, see 800493C8
void func_801D069C(void) {
    volatile s32 dummy;
    s32 i;
    MenuSetCursorPickerObj(
        &D_801D24CC[0], 0, 0, 1, 10, 0, 0, 1, 10, 0, 0, 0, 1, 0);
    MenuSetCursorPickerObj(
        &D_801D24CC[1], 0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 1, 1, 0);
    MenuSetCursorPickerObj(
        &D_801D24CC[2], 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 0, 1, 0);
    MenuSetCursorPickerObj(
        &D_801D24CC[3], 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0);
    D_801D1AA8 = 0;
    for (i = 0; i < LEN(D_80049208); i++) {
        D_801D252C[i] = D_80049208[i];
    }
}

// exported, see 8004941C
INCLUDE_ASM("asm/us/menu/nonmatchings/cnfgmenu", MenuConfig);

static void func_801D1AA0(void) {}
