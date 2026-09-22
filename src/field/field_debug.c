//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

typedef struct {
    /* 0x000 */ s16 x;
    /* 0x002 */ s16 y;
    /* 0x004 */ s16 width;
    /* 0x006 */ s16 height;
    /* 0x008 */ u8 r;
    /* 0x009 */ u8 g;
    /* 0x00A */ u8 b;
    /* 0x00B */ u8 pad0B;
    /* 0x00C */ s16 rowCount;
    /* 0x00E */ s16 headRow;
    /* 0x010 */ char text[24][14];
    /* 0x160 */ u8 rowColor[24];
    /* 0x178 */ u8 state;
} FieldDebugPage; /* size = 0x17A */

extern FieldDebugPage g_FieldDebugPages[6];
extern char g_FieldDebugDigits[16];
extern u16 g_FieldDebugRb;
extern s16 g_FieldDebugRChars;
extern s16 g_FieldDebugRLines;
extern s16 g_FieldDebugRRect;
extern s16 g_FieldDebugRDm;
extern u16 g_FieldDebugTransp;

void FieldDebugPageSetPosSize(s16 pageId, s16 x, s16 y, s16 width, s16 height);
void FieldDebugPageResetStrings(s16 pageId);
static void FieldDebugPageSetHeadRow(s16 pageId, s16 row);
static void FieldDebugPageHide(s16 pageId);

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugInitBuffers);

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
    AddStrNextDebugRow(4, g_FieldDebugActorLabel);
    FieldDebugPageHide(4);
    FieldDebugPageInit(3, 0x6C, 0xA4, 0x6C, 0x5C);
    AddStrNextDebugRow(3, g_FieldDebugWordLabel);
    FieldDebugPageHide(3);
    FieldDebugPageInit(1, 0, 0, 0x6C, 0xCA);
    AddStrNextDebugRow(1, g_FieldDebugActorLabel);
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugPagesResetPosSize);

void FieldDebugPageInit(s16 pageId, s16 x, s16 y, s16 width, s16 height) {
    s32 offset;

    FieldDebugPageSetPosSize(pageId, x, y, width, height);
    offset = pageId * 378;
    if ((&g_FieldDebugPages[0].state)[offset] != 2) {
        FieldDebugPageResetStrings(pageId);
        return;
    }
    g_FieldDebugPages[pageId].state = 0;
    D_8009D824 = 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugPageSetPosSize);

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugPageAddPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugPageAddSize);

static s32 FieldDebugPageIsRender(s16 pageId) { return g_FieldDebugPages[pageId].state == 0; }

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugPageResetStrings);

static void FieldDebugRenderClear(void) {
    g_FieldDebugRChars = 0;
    g_FieldDebugRLines = 0;
    g_FieldDebugRRect = 0;
    g_FieldDebugRDm = 0;
    g_FieldDebugRb ^= 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugRenderPage);

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugRenderString);

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", AddStrNextDebugRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", AddColorStrNextDebugRow);

s32 SetStrToDebugRow(s16 pageId, s16 row, const char* str) {
    FieldDebugStringCopy(g_FieldDebugPages[pageId].text[row], str);
    D_8009D824 = 1;
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", SetDebugStrRowColor);

static void FieldDebugPageSetHeadRow(s16 pageId, s16 row) {
    g_FieldDebugPages[pageId].headRow = row;
    D_8009D824 = 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugPageSetColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugPageNotInit);

static void FieldDebugPageHide(s16 pageId) {
    g_FieldDebugPages[pageId].state = 2;
    D_8009D824 = 1;
}

static void FieldDebugTranspSwitch(void) { g_FieldDebugTransp = (g_FieldDebugTransp + 1) & 3; }

void FieldDebugStringCopy(char* dst, const char* src) {
    if (*src) {
        do {
            *dst++ = *src++;
        } while (*src != '\0');
    }
    *dst = '\0';
}

void FieldDebugStringConcat(char* dest, const char* src) {
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_debug", FieldDebugIntToString);
