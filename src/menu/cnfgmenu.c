//! PSYQ=3.3 CC1=2.7.2
#include <game.h>

static s32 D_801D1AA8 = -1;
static u8 D_801D1AAC[] = {
    0x07, 0x07, 0x05, 0x07, 0x06, 0x07, 0x04, 0x07, 0x03, 0x0A, 0x00, 0x0A, 0x01, 0x0A, 0x02, 0x0A,
    0x08, 0x07, 0x00, 0x00, 0x00, 0x00, 0x09, 0x07, 0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00,
    0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
static u8 D_801D1ADB = 8;
static u8 D_801D1ADC[] = {0x05, 0x06, 0x04, 0x07, 0x02, 0x03, 0x00, 0x01, 0x08, 0x0B, 0x00, 0x00};
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
    {0x30, 0x52, 0x45, 0x53, 0x53, 0x00, 0xB2, 0x33, 0x34, 0x21, 0x32, 0x34, 0xB3, 0x00, 0x54, 0x4F,
     0x00, 0x43, 0x55, 0x53, 0x54, 0x4F, 0x4D, 0x49, 0x5A, 0x45, 0x0E, 0xFF, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
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
static s32 D_801D24C0[] = {4, 5, 6};

// from now on this is the BSS section; this can be moved up if necessary
static MenuTable D_801D24CC[4];
static s32 unused[5];
static s32 D_801D2528;
static u8 D_801D252C[NUM_MENU_COLOR];

static void func_801D0040(u16 arg0) {
    g_AkaoCmd.opcode = AKAO_PLAY_MENU_SOUND;
    g_AkaoCmd.params[0] = arg0;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
}

static void func_801D0080(s32 arg0) {
    switch (arg0) {
    case 0:
        g_AkaoCmd.opcode = AKAO_SET_MONO_MODE;
        g_AkaoCmd.params[0] = AKAO_SET_MONO_MODE;
        g_AkaoCmd.params[1] = AKAO_SET_MONO_MODE;
        break;
    case 1:
        g_AkaoCmd.opcode = AKAO_SET_STEREO_MODE;
        g_AkaoCmd.params[0] = AKAO_SET_STEREO_MODE;
        g_AkaoCmd.params[1] = AKAO_SET_STEREO_MODE;
        break;
    case 2:
        g_AkaoCmd.opcode = AKAO_RESET_VOLUME;
        g_AkaoCmd.params[0] = AKAO_RESET_VOLUME;
        g_AkaoCmd.params[1] = AKAO_RESET_VOLUME;
        break;
    }
    AkaoExec();
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

static void func_801D014C(x, y, value) s16 x;
s16 y;
s32 value;
{
    u8 uv = D_801D1AAC[value * 2 + 0];
    u8 page = D_801D1AAC[value * 2 + 1];
    SysMenuDrawTexturedRect(x, y, (uv & 3) * 16 + 96, (uv >> 2) * 16 + 64, 16, 16, page, 0);
}

static void func_801D01C8(void) {
    RECT rect;
    s32 temp_s4;
    s32 x;
    s32 y;
    s32 var_s2;
    s32 i;
    s32 _setting;
    u8* setting;

    y = D_801D24A0[1].y + 33;
    SysMenuDrawString(40, D_801D24A0[1].y + 14, D_801D1AE8[0], 5);
    for (i = 1; i < 9; i++) {
        SysMenuDrawString(40, y + (i - 1) * 18, D_801D1AE8[i], 5);
    }
    SysMenuDrawString(40, y + (i - 1) * 18, D_801D1AE8[33], 5);
    for (i = 0; i < 2; i++) {
        SysMenuDrawString(165 + i * 65, y, D_801D1AE8[15 + i], -((Savemap.config & 3) == i) & 7);
    }
    for (i = 0; i < 2; i++) {
        SysMenuDrawString(165 + i * 65, y + 18, D_801D1E48[i], -(((Savemap.config >> 2) & 3) == i) & 7);
    }
    for (i = 0; i < 2; i++) {
        SysMenuDrawString(165 + i * 65, y + 0x24, D_801D1EA8[i], -(((Savemap.config >> 4) & 3) == i) & 7);
    }
    temp_s4 = y + 54;
    SysMenuDrawString(165, temp_s4, D_801D1F08[0], -(((Savemap.config >> 6) & 3) == 0) & 7);
    x = SysGetSingleStringWidth(D_801D1F08[0]);
    SysMenuDrawString(x + 175, temp_s4, D_801D1F08[1], -(((Savemap.config >> 6) & 3) == 1) & 7);
    SysMenuDrawString(x + SysGetSingleStringWidth(D_801D1F08[1]) + 185, temp_s4, D_801D1F08[2],
                      -(((Savemap.config >> 6) & 3) == 2) & 7);
    for (i = 0; i < 2; i++) {
        SysMenuDrawString(165 + i * 65, y + 126, D_801D1F08[3 + i], -(((Savemap.config >> 8) & 3) == i) & 7);
    }
    for (i = 0; i < 3; i++) {
        _setting = D_801D248C[((Savemap.config >> 10) & 7) * 3 + i];
        setting = &D_801D248C[((Savemap.config >> 10) & 7) * 3];
        SysMenuDrawString(189 + i * 52, y + 0x90, D_801D1F08[12 + setting[i]], 7);
    }
    SysMenuDrawString(149, y + 0x90, D_801D1F08[16], 7);

    // battle speed value
    rect.y = y + 72;
    rect.w = 8;
    rect.h = 11;
    rect.x = (Savemap.battle_speed >> 1) + 184;
    SysMenuDrawScrollbarSlider(&rect);

    // battle message speed value
    rect.y = y + 0x5A;
    rect.w = 8;
    rect.h = 11;
    rect.x = (Savemap.battle_msg_speed >> 1) + 184;
    SysMenuDrawScrollbarSlider(&rect);

    // field message speed value
    rect.y = y + 108;
    rect.w = 8;
    rect.h = 11;
    rect.x = (Savemap.field_msg_speed >> 1) + 184;
    SysMenuDrawScrollbarSlider(&rect);

    // speed values
    for (i = 0; i < 3; i++) {
        SysMenuDrawString(157, y + 72 + i * 18 + 2, D_801D1F08[7], 7);
        SysMenuDrawString(324, y + 72 + i * 18 + 2, D_801D1F08[6], 7);
    }

    // speed bars
    for (i = 0; i < 3; i++) {
        rect.x = 184;
        rect.y = y + 72 + i * 18;
        rect.w = 136;
        rect.h = 11;
        SysMenuDrawScrollbarTrack(&rect);
    }

    // magic order ID
    SysMenuDrawDigitsWithLeadingZeroes(173, y + 146, ((Savemap.config >> 0xA) & 7) + 1, 1, 7);

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 0x100;
    SysMenuSetDrawMode(0, 1, 0x1F, &rect);
}

// exported, see 800493C8
void func_801D069C(void) {
    volatile s32 dummy;
    s32 i;
    SysMenuSetCursorMovement(&D_801D24CC[0], 0, 0, 1, 10, 0, 0, 1, 10, 0, 0, 0, 1, 0);
    SysMenuSetCursorMovement(&D_801D24CC[1], 0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 1, 1, 0);
    SysMenuSetCursorMovement(&D_801D24CC[2], 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 0, 1, 0);
    SysMenuSetCursorMovement(&D_801D24CC[3], 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0);
    D_801D1AA8 = 0;
    for (i = 0; i < NUM_MENU_COLOR; i++) {
        D_801D252C[i] = g_MenuColors[i];
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/us/menu/nonmatchings/cnfgmenu", func_801D080C);
#else
void func_801D080C(s32 arg0) {
    RECT sp18;
    RECT sp20;
    RECT padding[2];
    s16 temp_s0;
    s32 temp_s0_9;
    s32 temp_s3;
    s32 x;
    s32 y;
    s32 temp_v1;
    s32 var_a1_3;
    s32 i;
    u16 var_s2_3;
    s32* var_a3;

    D_801D24B8.press = InputReadPadsRaw();
    D_801D24BC = D_801D24B8.press & (D_801D24B8.prev ^ D_801D24B8.press);
    SysMenuDrawMenuList(g_MenuRenderBufferIndex);
    switch (D_801D1AA8) {
    case 0: // config menu
        SysMenuDrawString(16, 11, SysKernGetString(8, D_801D24CC[0].row + 0x37, 8), 7);
        if (D_801D24CC[0].row == 0) {
            SysMenuDrawCursor(12, D_801D24A0[1].y + 0x10);
        } else {
            SysMenuDrawCursor(12, D_801D24A0[1].y + 0x23 + (D_801D24CC[0].row - 1) * 18);
        }
        break;
    case 1: // window color sub-menu
        if (arg0 & 2) {
            if (D_801D24CC[0].row == 0) {
                SysMenuDrawCursor(12, D_801D24A0[1].y + 0x10);
            } else {
                SysMenuDrawCursor(12, D_801D24A0[1].y + 0x25 + (D_801D24CC[0].row - 1) * 18);
            }
        }
        SysMenuDrawCursor(D_801D24CC[1].column * 0x3D + 0x8D, D_801D24A0[1].y + 4 + D_801D24CC[1].row * 18);
        break;
    case 2: // RGB sub-menu
        if (arg0 & 2) {
            SysMenuDrawCursor(12, D_801D24A0[1].y + 0x10 + D_801D24CC[0].row * 18);
            SysMenuDrawCursor(D_801D24CC[1].column * 0x3D + 0x8D, D_801D24A0[1].y + 4 + D_801D24CC[1].row * 19);
        }
        SysMenuDrawCursor(0x93, D_801D24A0[1].y + 0x24 + D_801D24CC[2].row * 12);
        temp_s0 = D_801D24A0[1].y;
        temp_s3 = D_801D24A0[1].x + 0xAD;
        SysMenuDrawSingleFontLetter(temp_s3, temp_s0 + 0x23, D_801D2478[0][0], 2);
        SysMenuDrawSingleFontLetter(temp_s3, temp_s0 + 0x2F, D_801D2478[1][0], 4);
        SysMenuDrawSingleFontLetter(temp_s3, temp_s0 + 0x3B, D_801D2478[2][0], 1);

        for (i = 0; i < 3; i++) {
            SysMenuDrawDigitsWithLeadingZeroes(temp_s3 + 0x10, temp_s0 + 0x23 + i * 12,
                                               D_801D252C[D_801D24CC[1].column * 3 + D_801D24CC[1].row * 6 + i], 3, 7);
            sp20.x = D_801D252C[D_801D24CC[1].column * 3 + D_801D24CC[1].row * 6 + i] / 2 + 0xD5;
            sp20.y = D_801D24A0[1].y + 0x21 + i * 12;
            sp20.w = 8;
            sp20.h = 0xB;
            SysMenuDrawScrollbarSlider(&sp20);
            sp20.x = 0xD5;
            sp20.y = D_801D24A0[1].y + 0x21 + i * 12;
            sp20.w = 0x88;
            sp20.h = 0xB;
            SysMenuDrawScrollbarTrack(&sp20);
        }

        sp20.x = 0;
        sp20.y = 0;
        sp20.w = 0x100;
        sp20.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x1F, &sp20);
        SysMenuSetWindowRect(&sp18, D_801D24A0[1].x + 0xA5, D_801D24A0[1].y + 0x1D, 0xC0, 0x2B);
        SysMenuDrawWindow(&sp18);
        SysMenuSetWindowRect(&sp18, D_801D24A0[1].x + 0xF8, D_801D24A0[1].y + 7, 0x1A, 18);
        temp_v1 = D_801D24CC[1].row * 6 + D_801D24CC[1].column * 3;
        SysMenuDrawColoredRect(&sp18, D_801D252C[temp_v1], D_801D252C[temp_v1 + 1], D_801D252C[temp_v1 + 2]);
        SysMenuSetWindowRect(&sp18, D_801D24A0[1].x + 0xF5, D_801D24A0[1].y + 4, 0x20, 0x18);
        SysMenuDrawWindow(&sp18);
        break;
    case 3: // controller customization sub-menu
        if (arg0 & 2) {
            SysMenuDrawCursor(12, D_801D24A0[1].y + 10 + D_801D24CC[0].row * 0x15);
        }
        if (D_801D2528 == 0) {
            SysMenuDrawString(0x10, 0xB, D_801D23E8[0], 7);
        } else {
            SysMenuDrawString(0x10, 0xB, D_801D2418[0], 7);
        }
        x = D_801D24A0[1].x + 0x8C;
        y = D_801D24A0[1].y + 3;
        if (D_801D2528 != 0) {
            SysMenuDrawCursor(D_801D24A0[1].x + 0x78, y + 10 + (D_801D2528 - 1) * 18);
        }
        for (i = 0; i < 10; i++) {
            SysMenuDrawSingleFontLetter(x + 0x22, y + 10 + i * 18, 0xDA, 5);
        }
        SysMenuSetWindowRect(&sp18, x, y, 0x78, 0xBB);
        y += 6;
        x += 10;
        for (i = 0; i < 10; i++) {
            func_801D014C(x, y + i * 18, D_801D1ADC[i]);
            func_801D014C(x + 0x2C, y + i * 18, Savemap.button_config[D_801D1ADC[i]]);
            sp20.x = 0;
            sp20.y = 0;
            sp20.w = 0x100;
            sp20.h = 0x100;
            SysMenuSetDrawMode(0, 1, 0x1F, &sp20);
            SysMenuDrawString(x + 0x3E, y + 4 + i * 18 - 2, D_801D2238[Savemap.button_config[D_801D1ADC[i]]], 6);
        }
        SysMenuDrawWindow(&sp18);
        break;
    }
    func_801D01C8();
    sp20.x = 0;
    sp20.y = 0;
    sp20.w = 0xFF;
    sp20.h = 0xFF;
    SysMenuSetDrawMode(0, 1, 0x1F, &sp20);
    SysMenuStoreWindowColor();
    MENU_SetWindowColors(D_801D252C);
    SysMenuSetWindowRect(&sp18, D_801D24A0[1].x + 0xA5, D_801D24A0[1].y + 4, 0x40, 0x18);
    SysMenuDrawWindow(&sp18);
    SysMenuRestoreWindowColor();
    for (i = 0; i < 2; i++) {
        SysMenuDrawWindow(&D_801D24A0[i]);
    }
    if (SysMenuGetMenuListState() == 0) {
        SysMenuHandleButtons(&D_801D24CC[D_801D1AA8]);
        switch (D_801D1AA8) {
        case 4:
            break;
        case 0:
            if (g_Pad0KeysRepeat & 0x40) {
                func_801D0040(4);
                SysMenuSetMenuListAnimation(5, 0);
                SysMenuLoadMenuFileById(0);
            } else {
                switch (D_801D24CC[0].row) {
                case 0: // window color
                    if (g_Pad0KeysPressed & 0x20) {
                        D_801D1AA8 = 1;
                        func_801D0040(1);
                    }
                    break;
                case 1: // sound
                    var_s2_3 = Savemap.config & 3;
                    if (g_Pad0KeysRepeat & 0x2000) {
                        if (var_s2_3 != 1) {
                            func_801D0040(1);
                            var_s2_3++;
                            Savemap.config &= 0xFFFC;
                            Savemap.config |= var_s2_3 << 0;
                            func_801D0080(var_s2_3);
                        }
                    }
                    if (g_Pad0KeysRepeat & 0x8000) {
                        if (var_s2_3 != 0) {
                            func_801D0040(1);
                            var_s2_3--;
                            Savemap.config &= 0xFFFC;
                            Savemap.config |= var_s2_3 << 0;
                            func_801D0080(var_s2_3);
                        }
                    }
                    break;
                case 2: // controller
                    var_s2_3 = (Savemap.config >> 2) & 3;
                    if (g_Pad0KeysPressed & 0x20) {
                        if (var_s2_3 == 1) {
                            func_801D0040(1);
                            D_801D1AA8 = 3;
                            D_801D2528 = 0;
                        }
                    } else {
                        if (g_Pad0KeysRepeat & 0x2000) {
                            if (var_s2_3 != 1) {
                                func_801D0040(1);
                                var_s2_3++;
                                Savemap.config &= 0xFFF3;
                                Savemap.config |= var_s2_3 << 2;
                            }
                        }
                        if (g_Pad0KeysRepeat & 0x8000) {
                            if (var_s2_3 != 0) {
                                func_801D0040(1);
                                var_s2_3--;
                                Savemap.config &= 0xFFF3;
                                Savemap.config |= var_s2_3 << 2;
                            }
                        }
                    }
                    break;
                case 3: // cursor
                    var_s2_3 = (Savemap.config >> 4) & 3;
                    if (g_Pad0KeysRepeat & 0x2000) {
                        if (var_s2_3 != 1) {
                            func_801D0040(1);
                            var_s2_3++;
                            Savemap.config &= 0xFFCF;
                            Savemap.config |= var_s2_3 << 4;
                        }
                    }
                    if (g_Pad0KeysRepeat & 0x8000) {
                        if (var_s2_3 != 0) {
                            func_801D0040(1);
                            var_s2_3--;
                            Savemap.config &= 0xFFCF;
                            Savemap.config |= var_s2_3 << 4;
                        }
                    }
                    break;
                case 4: // ATB
                    var_s2_3 = (Savemap.config >> 6) & 3;
                    if (g_Pad0KeysRepeat & 0x2000) {
                        if (var_s2_3 != 2) {
                            var_s2_3++;
                            func_801D0040(1);
                            Savemap.config &= 0xFF3F;
                            Savemap.config |= var_s2_3 << 6;
                        }
                    }
                    if (g_Pad0KeysRepeat & 0x8000) {
                        if (var_s2_3 != 0) {
                            var_s2_3--;
                            func_801D0040(1);
                            Savemap.config &= 0xFF3F;
                            Savemap.config |= var_s2_3 << 6;
                        }
                    }
                    break;
                case 5: // battle speed
                    if (g_Pad0Keys & 0x2000) {
                        if (Savemap.battle_speed != 0xFF) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            Savemap.battle_speed += 1;
                        }
                    }
                    if (g_Pad0Keys & 0x8000) {
                        if (Savemap.battle_speed != 0) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            Savemap.battle_speed -= 1;
                        }
                    }
                    break;
                case 6: // battle message
                    if (g_Pad0Keys & 0x2000) {
                        if (Savemap.battle_msg_speed != 0xFF) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            Savemap.battle_msg_speed += 1;
                        }
                    }
                    if (g_Pad0Keys & 0x8000) {
                        if (Savemap.battle_msg_speed != 0) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            Savemap.battle_msg_speed -= 1;
                        }
                    }
                    break;
                case 7: // field message
                    if (g_Pad0Keys & 0x2000) {
                        if (Savemap.field_msg_speed != 0xFF) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            Savemap.field_msg_speed += 1;
                        }
                    }
                    if (g_Pad0Keys & 0x8000) {
                        if (Savemap.field_msg_speed != 0) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            Savemap.field_msg_speed -= 1;
                        }
                    }
                    break;
                case 8: // camera angle
                    var_s2_3 = (Savemap.config >> 8) & 3;
                    if (g_Pad0KeysRepeat & 0x2000) {
                        if (var_s2_3 != 1) {
                            func_801D0040(1);
                            var_s2_3++;
                            Savemap.config &= ~0x300;
                            Savemap.config |= var_s2_3 << 8;
                        }
                    }
                    if (g_Pad0KeysRepeat & 0x8000) {
                        if (var_s2_3 != 0) {
                            func_801D0040(1);
                            var_s2_3--;
                            Savemap.config &= ~0x300;
                            Savemap.config |= var_s2_3 << 8;
                        }
                    }
                    break;
                case 9: // magic order
                    var_s2_3 = (Savemap.config >> 10) & 7;
                    if (g_Pad0KeysRepeat & 0x2000) {
                        if (var_s2_3 != 5) {
                            func_801D0040(1);
                            var_s2_3++;
                            Savemap.config &= ~0x1C00;
                            Savemap.config |= var_s2_3 << 10;
                        }
                    }
                    if (g_Pad0KeysRepeat & 0x8000) {
                        if (var_s2_3 != 0) {
                            func_801D0040(1);
                            var_s2_3--;
                            Savemap.config &= ~0x1C00;
                            Savemap.config |= var_s2_3 << 10;
                        }
                    }
                    break;
                }
            }
            break;
        case 1:
            if (g_Pad0KeysPressed & 0x40) {
                for (i = 0; i < 12; i++) {
                    g_MenuColors[i] = D_801D252C[i];
                }
                func_801D0040(4);
                D_801D1AA8 = 0;
            } else if (g_Pad0KeysPressed & 0x20) {
                D_801D1AA8 = 2;
                func_801D0040(1);
            }
            break;
        case 2:
            temp_s0_9 = D_801D24CC[1].column * 3 + D_801D24CC[1].row * 6 + D_801D24CC[2].row;
            if (g_Pad0KeysPressed & 0x40) {
                func_801D0040(4);
                D_801D1AA8 = 1;
            } else if (g_Pad0Keys & 0x2000) {
                if (D_801D252C[temp_s0_9] != 0xFF) {
                    if (arg0 & 4) {
                        func_801D0040(1);
                    }
                    D_801D252C[temp_s0_9]++;
                }
            } else if (g_Pad0Keys & 0x8000) {
                if (D_801D252C[temp_s0_9] != 0) {
                    if (arg0 & 4) {
                        func_801D0040(1);
                    }
                    D_801D252C[temp_s0_9]--;
                }
            }
            break;
        case 3:
            if (D_801D24BC & 0x40) {
                if (D_801D2528 == 0) {
                    func_801D0040(4);
                    D_801D1AA8 = 0;
                    break;
                }
            } else if (D_801D2528 == 0) {
                if (D_801D24BC & 0x800) {
                    D_801D2528 = 1;
                }
                break;
            }
            if (D_801D24BC != 0) {
                if ((u8)func_801D0118(D_801D24BC) != 0xFF) {
                    Savemap.button_config[D_801D1ADC[D_801D2528 - 1]] = func_801D0118(D_801D24BC);
                } else if ((g_Pad0KeysRepeat & 0x1000) && D_801D2528 != 1) {
                    func_801D0040(1);
                    D_801D2528--;
                } else if ((g_Pad0KeysRepeat & 0x4000) && D_801D2528 != 10) {
                    func_801D0040(1);
                    D_801D2528++;
                } else {
                    if (g_Pad0KeysRepeat & 0xA000) {
                        var_a1_3 = 0;
                        var_s2_3 = 0;
                        var_a3 = D_801D24C0;
                        for (; var_s2_3 < 3; var_s2_3++) {
                            for (i = 0; i < 16; i++) {
                                if (var_a3[var_s2_3] == Savemap.button_config[i]) {
                                    var_a1_3 |= 1 << var_s2_3;
                                }
                            }
                        }
                        if (var_a1_3 == 7) {
                            func_801D0040(1);
                            D_801D2528 = 0;
                            D_801D1AA8 = 0;
                        } else {
                            func_801D0040(3);
                        }
                    }
                }
            }
            break;
        }
    }
    D_801D24B8.prev = D_801D24B8.press;
}
#endif

static void func_801D1AA0(void) {}
