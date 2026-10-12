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
static u8 D_801D24C0[] = {4, 0, 0, 0, 5, 0, 0, 0, 6, 0, 0, 0};

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

static void func_801D014C(s16 x, s16 y, s32 value) {
    u8 uv = D_801D1AAC[value * 2 + 0];
    u8 page = D_801D1AAC[value * 2 + 1];
    SysMenuDrawTexturedRect(x, y, (uv & 3) * 16 + 96, (uv >> 2) * 16 + 64, 16, 16, page, 0);
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

? SysMenuDrawColoredRect(RECT*, u8, u8, u8);                // extern
? SysMenuDrawCursor(s32, s32);                              // extern
? SysMenuDrawDigitsWithLeadingZeroes(s32, s32, u8, ?, s32); // extern
? SysMenuDrawMenuList(s32);                                 // extern
? SysMenuDrawScrollbarSlider(s16*, s8);                     // extern
? SysMenuDrawScrollbarTrack(s16*);                          // extern
? SysMenuDrawSingleFontLetter(s32, s32, u8, ?);             // extern
? SysMenuDrawWindow(RECT*);                                 // extern
? SysMenuLoadMenuFileById(?);                               // extern
? SysMenuRestoreWindowColor();                              // extern
? SysMenuSetWindowRect(RECT*, s32, s32, ?, s32);            // extern
? SysMenuStoreWindowColor();                                // extern
extern Unk8009D7BC D_8009D7BD;
extern u16 D_8009D7BE;
extern /*?*/ s32 D_8009D7C0;
extern Unk8009D7BC D_8009D7D0;
extern /*?*/ s32 D_801D252D;
extern /*?*/ s32 D_801D252E;

void func_801D080C(s32 arg0) {
    RECT sp18;
    s16 sp20;
    s16 sp22;
    s16 sp24;
    s16 sp26;
    RECT* var_s0_3;
    Unk8009D7BC* var_s0_4;
    s16 temp_a1_2;
    s16 temp_s0;
    s16 temp_s3_3;
    s16 temp_s4_2;
    s16 var_a1;
    s16 var_s2_2;
    s16 var_v0;
    s32 temp_a1;
    s32 temp_a1_3;
    s32 temp_s0_2;
    s32 temp_s0_3;
    s32 temp_s0_4;
    s32 temp_s0_5;
    s32 temp_s0_6;
    s32 temp_s0_7;
    s32 temp_s0_8;
    s32 temp_s0_9;
    s32 temp_s3;
    s32 temp_s3_2;
    s32 temp_s4;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 temp_v1_5;
    s32 temp_v1_6;
    s32 temp_v1_7;
    s32 temp_v1_8;
    s32 var_a0;
    s32 var_a1_2;
    s32 var_a1_3;
    s32 var_s0;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s1_3;
    s32 var_s1_4;
    s32 var_s1_5;
    s32 var_s1_6;
    s32 var_s2;
    s32 var_s2_3;
    s32 var_s2_4;
    s32 var_s2_5;
    s32 var_s2_6;
    s32 var_s2_7;
    s32 var_s2_8;
    s32 var_s2_9;
    s32 var_s4;
    s32 var_s5;
    s32 var_v0_2;
    s32 var_v1;
    u16 temp_v0;
    u16 var_a0_2;
    u8(*var_a2)[0x30];
    u8 temp_v0_2;
    u8* var_s0_2;

    temp_v0 = InputReadPadsRaw();
    D_801D24B8.press = temp_v0;
    D_801D24BC = temp_v0 & (D_801D24B8.prev ^ temp_v0);
    SysMenuDrawMenuList(g_MenuRenderBufferIndex);
    switch (D_801D1AA8) { // switch 1; irregular
    case 0:               // switch 1
        SysMenuDrawString(0x10, 0xB, SysKernGetString(8, D_801D24CC->row + 0x37, 8), 7);
        if (D_801D24CC->row == 0) {
            SysMenuDrawCursor(0xC, D_801D24A0[1].y + 0x10);
        } else {
            var_a0 = 0xC;
            var_a1 = D_801D24A0[1].y;
            var_v0 = ((D_801D24CC->row - 1) * 0x12) + 0x23;
        block_16:
            SysMenuDrawCursor(var_a0, var_v0 + var_a1);
        }
        break;
    case 1: // switch 1
        if (arg0 & 2) {
            if (D_801D24CC->row == 0) {
                var_a1_2 = D_801D24A0[1].y + 0x10;
            } else {
                var_a1_2 = D_801D24A0[1].y + (((D_801D24CC->row - 1) * 0x12) + 0x25);
            }
            SysMenuDrawCursor(0xC, var_a1_2);
        }
        var_a0 = (D_801D24CC[1].column * 0x3D) + 0x8D;
        var_v0 = D_801D24A0[1].y;
        var_a1 = (D_801D24CC[1].row * 0x12) + 4;
        goto block_16;
    case 2: // switch 1
        if (arg0 & 2) {
            SysMenuDrawCursor(0xC, D_801D24A0[1].y + ((D_801D24CC->row * 0x12) + 0x10));
            SysMenuDrawCursor((D_801D24CC[1].column * 0x3D) + 0x8D, D_801D24A0[1].y + ((D_801D24CC[1].row * 0x13) + 4));
        }
        var_s1 = 0;
        var_s2 = 0x21;
        SysMenuDrawCursor(0x93, D_801D24A0[1].y + ((D_801D24CC[2].row * 0xC) + 0x24));
        temp_s0 = D_801D24A0[1].y;
        temp_s3 = D_801D24A0[1].x + 0xAD;
        var_s4 = temp_s0 + 0x23;
        SysMenuDrawSingleFontLetter(temp_s3, var_s4, D_801D2478[0][0], 2);
        SysMenuDrawSingleFontLetter(temp_s3, temp_s0 + 0x2F, D_801D2478->unk6, 4);
        SysMenuDrawSingleFontLetter(temp_s3, temp_s0 + 0x3B, D_801D2478->unkC, 1);
        do {
            temp_a1 = var_s4;
            var_s4 += 0xC;
            SysMenuDrawDigitsWithLeadingZeroes(
                temp_s3 + 0x10, temp_a1,
                D_801D252C[((&D_801D24CC[2].row)[-0x13].unk1 * 6) + (D_801D24CC[1].column * 3) + var_s1], 3, 7);
            temp_v1 = ((&D_801D24CC[2].row)[-0x13].unk1 * 6) + (D_801D24CC[1].column * 3) + var_s1;
            var_s1 += 1;
            sp26 = 0xB;
            sp22 = (u16)D_801D24A0[1].y + var_s2;
            sp24 = 8;
            sp20 = ((u8)D_801D252C[temp_v1] >> 1) + 0xD5;
            SysMenuDrawScrollbarSlider(&sp20, D_801D24CC[1].column);
            sp20 = 0xD5;
            sp24 = 0x88;
            sp26 = 0xB;
            sp22 = (u16)D_801D24A0[1].y + var_s2;
            SysMenuDrawScrollbarTrack(&sp20);
            var_s2 += 0xC;
        } while (var_s1 < 3);
        sp20 = 0;
        sp22 = 0;
        sp24 = 0x100;
        sp26 = 0x100;
        SysMenuSetDrawMode(0, 1, 0x1F, (RECT*)&sp20);
        SysMenuSetWindowRect(&sp18, D_801D24A0[1].x + 0xA5, D_801D24A0[1].y + 0x1D, 0xC0, 0x2B);
        SysMenuDrawWindow(&sp18);
        SysMenuSetWindowRect(&sp18, D_801D24A0[1].x + 0xF8, D_801D24A0[1].y + 7, 0x1A, 0x12);
        temp_v1_2 = (D_801D24CC[1].row * 6) + (D_801D24CC[1].column * 3);
        SysMenuDrawColoredRect(&sp18, D_801D252C[temp_v1_2], *(&D_801D252D + temp_v1_2), *(&D_801D252E + temp_v1_2));
        SysMenuSetWindowRect(&sp18, D_801D24A0[1].x + 0xF5, D_801D24A0[1].y + 4, 0x20, 0x18);
    block_33:
        SysMenuDrawWindow(&sp18);
        break;
    case 3: // switch 1
        if (arg0 & 2) {
            SysMenuDrawCursor(0xC, D_801D24A0[1].y + ((D_801D24CC->row * 0x15) + 0xA));
        }
        if (D_801D2528 == 0) {
            var_a2 = D_801D23E8;
        } else {
            var_a2 = D_801D2418;
        }
        SysMenuDrawString(0x10, 0xB, (s8*)var_a2, 7);
        temp_s3_2 = D_801D24A0[1].x + 0x8C;
        temp_s4 = D_801D24A0[1].y + 3;
        if (D_801D2528 != 0) {
            SysMenuDrawCursor(D_801D24A0[1].x + 0x78, temp_s4 + (((D_801D2528 - 1) * 0x12) + 0xA));
        }
        var_s1_2 = 0;
        var_s0 = 0xA;
        do {
            SysMenuDrawSingleFontLetter(temp_s3_2 + 0x22, temp_s4 + var_s0, 0xDAU, 5);
            var_s1_2 += 1;
            var_s0 += 0x12;
        } while (var_s1_2 < 0xA);
        SysMenuSetWindowRect(&sp18, temp_s3_2, temp_s4, 0x78, 0xBB);
        temp_s4_2 = temp_s4 + 6;
        temp_s3_3 = temp_s3_2 + 0xA;
        var_s1_3 = 0;
        var_s0_2 = D_801D1ADC;
        var_s5 = 4;
        var_s2_2 = temp_s4_2;
        do {
            var_s1_3 += 1;
            func_801D014C(temp_s3_3, var_s2_2, (s32)*var_s0_2);
            temp_a1_2 = var_s2_2;
            var_s2_2 += 0x12;
            func_801D014C(temp_s3_3 + 0x2C, temp_a1_2, (s32) * (&D_8009D7C0 + *var_s0_2));
            sp20 = 0;
            sp22 = 0;
            sp24 = 0x100;
            sp26 = 0x100;
            SysMenuSetDrawMode(0, 1, 0x1F, (RECT*)&sp20);
            temp_a1_3 = (temp_s4_2 + var_s5) - 2;
            temp_v0_2 = *var_s0_2;
            var_s0_2 += 1;
            var_s5 += 0x12;
            SysMenuDrawString(temp_s3_3 + 0x3E, temp_a1_3, (s8*)D_801D2238[*(&D_8009D7C0 + temp_v0_2)], 6);
        } while (var_s1_3 < 0xA);
        goto block_33;
    }
    var_s1_4 = 0;
    func_801D01C8();
    sp20 = 0;
    sp22 = 0;
    sp24 = 0xFF;
    sp26 = 0xFF;
    SysMenuSetDrawMode(0, 1, 0x1F, (RECT*)&sp20);
    SysMenuStoreWindowColor();
    MENU_SetWindowColors(D_801D252C);
    var_s0_3 = &D_801D24A0[1] - 8;
    SysMenuSetWindowRect(&sp18, D_801D24A0[1].x + 0xA5, D_801D24A0[1].y + 4, 0x40, 0x18);
    SysMenuDrawWindow(&sp18);
    SysMenuRestoreWindowColor();
    do {
        SysMenuDrawWindow(var_s0_3);
        var_s1_4 += 1;
        var_s0_3 += 8;
    } while (var_s1_4 < 2);
    if (SysMenuGetMenuListState() == 0) {
        SysMenuHandleButtons(&D_801D24CC[D_801D1AA8]);
        switch (D_801D1AA8) { // switch 2
        case 4:               // switch 2
            break;
        case 0: // switch 2
            if (g_Pad0KeysRepeat & 0x40) {
                func_801D0040(4U);
                SysMenuSetMenuListAnimation(5, 0);
                SysMenuLoadMenuFileById(0);
            } else {
                switch (D_801D24CC->row) { // switch 3
                case 0:                    // switch 3
                    if (g_Pad0KeysPressed & 0x20) {
                        D_801D1AA8 = 1;
                        var_a0_2 = 1;
                    block_151:
                        func_801D0040(var_a0_2);
                    }
                    break;
                case 1: // switch 3
                    temp_s0_2 = D_8009D7BE & 3;
                    var_s2_3 = temp_s0_2;
                    if ((g_Pad0KeysRepeat & 0x2000) && ((var_s2_3 & 0xFFFF) != 1)) {
                        func_801D0040(1U);
                        temp_v1_3 = temp_s0_2 + 1;
                        var_s2_3 = temp_v1_3;
                        D_8009D7BE = (D_8009D7BE & 0xFFFC) | temp_v1_3;
                        func_801D0080(var_s2_3);
                    }
                    if ((g_Pad0KeysRepeat & 0x8000) && (var_s2_3 & 0xFFFF)) {
                        func_801D0040(1U);
                        temp_v0_3 = var_s2_3 - 1;
                        D_8009D7BE = (D_8009D7BE & 0xFFFC) | temp_v0_3;
                        func_801D0080(temp_v0_3 & 0xFFFF);
                    }
                    break;
                case 2: // switch 3
                    temp_s0_3 = ((u16)D_8009D7BE >> 2) & 3;
                    var_s2_4 = temp_s0_3;
                    if (g_Pad0KeysPressed & 0x20) {
                        if ((var_s2_4 & 0xFFFF) == 1) {
                            func_801D0040(1U);
                            D_801D1AA8 = 3;
                            D_801D2528 = 0;
                        }
                    } else {
                        if ((g_Pad0KeysRepeat & 0x2000) && ((var_s2_4 & 0xFFFF) != 1)) {
                            func_801D0040(1U);
                            temp_v1_4 = temp_s0_3 + 1;
                            var_s2_4 = temp_v1_4;
                            D_8009D7BE = (D_8009D7BE & 0xFFF3) | (temp_v1_4 * 4);
                        }
                        if ((g_Pad0KeysRepeat & 0x8000) && (var_s2_4 & 0xFFFF)) {
                            func_801D0040(1U);
                            var_v0_2 = (var_s2_4 - 1) * 4;
                            var_v1 = D_8009D7BE & 0xFFF3;
                        block_109:
                            D_8009D7BE = var_v1 | var_v0_2;
                        }
                    }
                    break;
                case 3: // switch 3
                    temp_s0_4 = ((u16)D_8009D7BE >> 4) & 3;
                    var_s2_5 = temp_s0_4;
                    if ((g_Pad0KeysRepeat & 0x2000) && ((var_s2_5 & 0xFFFF) != 1)) {
                        func_801D0040(1U);
                        temp_v1_5 = temp_s0_4 + 1;
                        var_s2_5 = temp_v1_5;
                        D_8009D7BE = (D_8009D7BE & 0xFFCF) | (temp_v1_5 * 0x10);
                    }
                    if ((g_Pad0KeysRepeat & 0x8000) && (var_s2_5 & 0xFFFF)) {
                        func_801D0040(1U);
                        var_v0_2 = (var_s2_5 - 1) * 0x10;
                        var_v1 = D_8009D7BE & 0xFFCF;
                        goto block_109;
                    }
                    break;
                case 4: // switch 3
                    temp_s0_5 = ((u16)D_8009D7BE >> 6) & 3;
                    var_s2_6 = temp_s0_5;
                    if ((g_Pad0KeysRepeat & 0x2000) && ((var_s2_6 & 0xFFFF) != 2)) {
                        temp_s0_6 = temp_s0_5 + 1;
                        func_801D0040(1U);
                        var_s2_6 = temp_s0_6;
                        D_8009D7BE = (D_8009D7BE & 0xFF3F) | (temp_s0_6 << 6);
                    }
                    if ((g_Pad0KeysRepeat & 0x8000) && (var_s2_6 & 0xFFFF)) {
                        func_801D0040(1U);
                        var_v0_2 = (var_s2_6 - 1) << 6;
                        var_v1 = D_8009D7BE & 0xFF3F;
                        goto block_109;
                    }
                    break;
                case 5: // switch 3
                    if ((g_Pad0Keys & 0x2000) && (D_8009D7BC.unk0 != 0xFF)) {
                        if (arg0 & 4) {
                            func_801D0040(1U);
                        }
                        D_8009D7BC.unk0 += 1;
                    }
                    if (g_Pad0Keys & 0x8000) {
                        var_s0_4 = &D_8009D7BC;
                    block_93:
                        if (var_s0_4->unk0 != 0) {
                            if (arg0 & 4) {
                                func_801D0040(1U);
                            }
                            var_s0_4->unk0 -= 1;
                        }
                    }
                    break;
                case 6: // switch 3
                    if ((g_Pad0Keys & 0x2000) && (D_8009D7BD.unk0 != 0xFF)) {
                        if (arg0 & 4) {
                            func_801D0040(1U);
                        }
                        D_8009D7BD.unk0 += 1;
                    }
                    if (g_Pad0Keys & 0x8000) {
                        var_s0_4 = &D_8009D7BD;
                        goto block_93;
                    }
                    break;
                case 7: // switch 3
                    if ((g_Pad0Keys & 0x2000) && (D_8009D7D0.unk0 != 0xFF)) {
                        if (arg0 & 4) {
                            func_801D0040(1U);
                        }
                        D_8009D7D0.unk0 += 1;
                    }
                    if (g_Pad0Keys & 0x8000) {
                        var_s0_4 = &D_8009D7D0;
                        goto block_93;
                    }
                    break;
                case 8: // switch 3
                    temp_s0_7 = ((u16)D_8009D7BE >> 8) & 3;
                    var_s2_7 = temp_s0_7;
                    if ((g_Pad0KeysRepeat & 0x2000) && ((var_s2_7 & 0xFFFF) != 1)) {
                        func_801D0040(1U);
                        temp_v1_6 = temp_s0_7 + 1;
                        var_s2_7 = temp_v1_6;
                        D_8009D7BE = (D_8009D7BE & 0xFCFF) | (temp_v1_6 << 8);
                    }
                    if ((g_Pad0KeysRepeat & 0x8000) && (var_s2_7 & 0xFFFF)) {
                        func_801D0040(1U);
                        var_v0_2 = (var_s2_7 - 1) << 8;
                        var_v1 = D_8009D7BE & 0xFCFF;
                        goto block_109;
                    }
                    break;
                case 9: // switch 3
                    temp_s0_8 = ((u16)D_8009D7BE >> 0xA) & 7;
                    var_s2_8 = temp_s0_8;
                    if ((g_Pad0KeysRepeat & 0x2000) && ((var_s2_8 & 0xFFFF) != 5)) {
                        func_801D0040(1U);
                        temp_v1_7 = temp_s0_8 + 1;
                        var_s2_8 = temp_v1_7;
                        D_8009D7BE = (D_8009D7BE & 0xE3FF) | (temp_v1_7 << 0xA);
                    }
                    if ((g_Pad0KeysRepeat & 0x8000) && (var_s2_8 & 0xFFFF)) {
                        func_801D0040(1U);
                        var_v0_2 = (var_s2_8 - 1) << 0xA;
                        var_v1 = D_8009D7BE & 0xE3FF;
                        goto block_109;
                    }
                    break;
                }
            }
            break;
        case 1: // switch 2
            var_s1_5 = 0;
            if (g_Pad0KeysPressed & 0x40) {
            loop_111:
                g_MenuColors[var_s1_5] = D_801D252C[var_s1_5];
                var_s1_5 += 1;
                if (var_s1_5 < 0xC) {
                    goto loop_111;
                }
            block_129:
                func_801D0040(4U);
                D_801D1AA8 = 0;
            } else if (g_Pad0KeysPressed & 0x20) {
                D_801D1AA8 = 2;
                var_a0_2 = 1;
                goto block_151;
            }
            break;
        case 2: // switch 2
            temp_s0_9 = (D_801D24CC[1].column * 3) + (D_801D24CC[1].row * 6) + D_801D24CC[2].row;
            if (g_Pad0KeysPressed & 0x40) {
                func_801D0040(4U);
                D_801D1AA8 = 1;
            } else if (g_Pad0Keys & 0x2000) {
                if (D_801D252C[temp_s0_9] != 0xFF) {
                    if (arg0 & 4) {
                        func_801D0040(1U);
                    }
                    D_801D252C[temp_s0_9] = D_801D252C[temp_s0_9] + 1;
                }
            } else if ((g_Pad0Keys & 0x8000) && (D_801D252C[temp_s0_9] != 0)) {
                if (arg0 & 4) {
                    func_801D0040(1U);
                }
                D_801D252C[temp_s0_9] = D_801D252C[temp_s0_9] - 1;
            }
            break;
        case 3: // switch 2
            if (D_801D24BC & 0x40) {
                if (D_801D2528 == 0) {
                    goto block_129;
                }
                goto block_133;
            }
            if (D_801D2528 == 0) {
                if (D_801D24BC & 0x800) {
                    D_801D2528 = 1;
                }
            } else {
            block_133:
                if (D_801D24BC != 0) {
                    if ((func_801D0118(D_801D24BC) & 0xFF) != 0xFF) {
                        *(&D_8009D7C0 + (&D_801D1ADB)[D_801D2528]) = func_801D0118(D_801D24BC);
                    } else if ((g_Pad0KeysRepeat & 0x1000) && (D_801D2528 != 1)) {
                        func_801D0040(1U);
                        D_801D2528 -= 1;
                    } else if ((g_Pad0KeysRepeat & 0x4000) && (D_801D2528 != 0xA)) {
                        func_801D0040(1U);
                        D_801D2528 += 1;
                    } else {
                        var_a1_3 = 0;
                        if (g_Pad0KeysRepeat & 0xA000) {
                            var_s2_9 = 0;
                            do {
                                var_s1_6 = 0;
                                temp_v1_8 = var_s2_9 & 0xFFFF;
                            loop_145:
                                if (*((temp_v1_8 * 4) + D_801D24C0) == *(&D_8009D7C0 + var_s1_6)) {
                                    var_a1_3 |= 1 << temp_v1_8;
                                }
                                var_s1_6 += 1;
                                if (var_s1_6 < 0x10) {
                                    goto loop_145;
                                }
                                var_s2_9 += 1;
                            } while ((u32)(var_s2_9 & 0xFFFF) < 3U);
                            var_a0_2 = 3;
                            if (var_a1_3 == 7) {
                                func_801D0040(1U);
                                D_801D2528 = 0;
                                D_801D1AA8 = 0;
                            } else {
                                goto block_151;
                            }
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
