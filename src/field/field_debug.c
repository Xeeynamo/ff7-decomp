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
extern s16 g_FieldDebugRb;
extern s16 g_FieldDebugRChars;
extern s16 g_FieldDebugRLines;
extern s16 g_FieldDebugRRect;
extern s16 g_FieldDebugRDm;
extern s16 g_FieldDebugTransp;

void FieldDebugPageSetPosSize(s16 pageId, s16 x, s16 y, s16 width, s16 height);
void FieldDebugPageResetStrings(s16 pageId);
static void FieldDebugPageSetHeadRow(s16 pageId, s16 row);
static void FieldDebugPageHide(s16 pageId);

extern SPRT_8 D_800E1028[2][0x158];
extern LINE_F3 D_800E3B28[2][24];
extern TILE D_800E3FA8[2][12];
extern DR_MODE D_800E4128[2][6];
extern u16 D_800E4200[8];
extern u32 D_800E41C8[2][7];

void FieldDebugInitBuffers(void) {
    s32 unused[8]; // unknown locals
    s32 i;
    u16 tpage;

    for (i = 0; i < LEN(g_FieldDebugPages); i++) {
        g_FieldDebugPages[i].state = 1;
    }

    D_8009D824 = 1;
    g_FieldDebugRb = 0;
    g_FieldDebugCurPage = 0;
    g_FieldDebugTransp = 0;

    for (i = 0; i < LEN(D_800E1028[0]); i++) {
        setSprt8(&D_800E1028[0][i]);
        setSprt8(&D_800E1028[1][i]);
        setShadeTex(&D_800E1028[0][i], 1);
        setShadeTex(&D_800E1028[1][i], 1);
    }

    for (i = 0; i < LEN(D_800E4200); i++) {
        D_800E4200[i] = ((0x1E7 - i) << 6) | 0x10;
    }

    for (i = 0; i < LEN(D_800E3FA8[0]); i++) {
        setTile(&D_800E3FA8[0][i]);
        setTile(&D_800E3FA8[1][i]);
        setSemiTrans(&D_800E3FA8[0][i], 1);
        setSemiTrans(&D_800E3FA8[1][i], 1);
    }

    for (i = 0; i < LEN(D_800E3B28[0]); i++) {
        setLineF3(&D_800E3B28[0][i]);
        setLineF3(&D_800E3B28[1][i]);
    }

    tpage = getTPage(0, 0, 960, 256);

    for (i = 0; i < LEN(D_800E4128[0]); i++) {
        SetDrawMode(&D_800E4128[0][i], 0, 0, tpage, NULL);
        SetDrawMode(&D_800E4128[1][i], 0, 0, tpage, NULL);
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

s16 FieldDebugPagesResetPosSize(s16 x, s16 y, s16 width, s16 height) {
    s16 i;

    for (i = 0; i < LEN(g_FieldDebugPages); i++) {
        if (!g_FieldDebugPages[i].state) {
            continue;
        }
        FieldDebugPageSetPosSize(i, x, y, width, height);
        FieldDebugPageResetStrings(i);
        return i;
    }
    FieldDebugPageSetPosSize(0, x, y, width, height);
    FieldDebugPageResetStrings(0);
    return 0;
}

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

void FieldDebugPageSetPosSize(s16 pageId, s16 x, s16 y, s16 width, s16 height) {
    g_FieldDebugPages[pageId].x = x;
    g_FieldDebugPages[pageId].y = y;
    g_FieldDebugPages[pageId].width = width;
    g_FieldDebugPages[pageId].height = height;
    D_8009D824 = 1;
}

void FieldDebugPageAddPos(s16 pageId, s16 dx, s16 dy) {
    g_FieldDebugPages[pageId].x += dx;
    g_FieldDebugPages[pageId].y += dy;
    D_8009D824 = 1;
}

void FieldDebugPageAddSize(s16 pageId, s16 dw, s16 dh) {
    g_FieldDebugPages[pageId].width += dw;
    g_FieldDebugPages[pageId].height += dh;
    D_8009D824 = 1;
}

static s32 FieldDebugPageIsRender(s16 pageId) { return g_FieldDebugPages[pageId].state == 0; }

void FieldDebugPageResetStrings(s16 pageId) {
    s32 i;

    for (i = 0; i < LEN(g_FieldDebugPages[pageId].rowColor); i++) {
        g_FieldDebugPages[pageId].text[i][0] = 0;
        g_FieldDebugPages[pageId].rowColor[i] = 0;
    }
    g_FieldDebugPages[pageId].r = 7;
    g_FieldDebugPages[pageId].g = 15;
    g_FieldDebugPages[pageId].b = 31;
    g_FieldDebugPages[pageId].headRow = 0;
    g_FieldDebugPages[pageId].rowCount = 0;
    g_FieldDebugPages[pageId].state = 0;
    D_8009D824 = 1;
}

static void FieldDebugRenderClear(void) {
    g_FieldDebugRChars = 0;
    g_FieldDebugRLines = 0;
    g_FieldDebugRRect = 0;
    g_FieldDebugRDm = 0;
    g_FieldDebugRb ^= 1;
}

void FieldDebugRenderPage(s16 pageId);
void FieldDebugRenderString(s16 pageId, s16 row, char* str, s32 x, s32 y);

void FieldDebugRender(OT_TYPE* ot) {
    s32 i;

    if (D_8009D824) {
        FieldDebugRenderClear();
        ClearOTag(D_800E41C8[g_FieldDebugRb], 7);
        for (i = 0; i < LEN(g_FieldDebugPages); i++) {
            if (!g_FieldDebugPages[i].state) {
                FieldDebugRenderPage(i);
            }
        }
        D_8009D824 = 0;
    }
    addPrims(ot, &D_800E41C8[g_FieldDebugRb][0], &D_800E41C8[g_FieldDebugRb][6]);
}

void FieldDebugRenderPage(s16 pageId) {
    s16 y;
    s16 i;

    if (g_FieldDebugRRect >= 0x18) {
        return;
    }
    for (i = 0, y = g_FieldDebugPages[pageId].y + 2;
         y < g_FieldDebugPages[pageId].y + g_FieldDebugPages[pageId].height - 8; i++, y += 10) {
        FieldDebugRenderString(pageId, i, g_FieldDebugPages[pageId].text[i], g_FieldDebugPages[pageId].x + 2, y);
    }
    if (g_FieldDebugPages[pageId].headRow) {
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x0 = g_FieldDebugPages[pageId].x + 2;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y0 =
            g_FieldDebugPages[pageId].y + (g_FieldDebugPages[pageId].headRow - 1) * 10 + 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x1 =
            g_FieldDebugPages[pageId].x - 2 + g_FieldDebugPages[pageId].width;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y1 =
            g_FieldDebugPages[pageId].y + (g_FieldDebugPages[pageId].headRow - 1) * 10 + 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x2 =
            g_FieldDebugPages[pageId].x - 2 + g_FieldDebugPages[pageId].width;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y2 =
            g_FieldDebugPages[pageId].y + (g_FieldDebugPages[pageId].headRow - 1) * 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->r0 = g_FieldDebugPages[pageId].r | 0x3F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->g0 = g_FieldDebugPages[pageId].g >> 1;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->b0 = g_FieldDebugPages[pageId].b;
        addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect]);
        g_FieldDebugRRect++;

        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x0 = g_FieldDebugPages[pageId].x + 2;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y0 =
            g_FieldDebugPages[pageId].y + (g_FieldDebugPages[pageId].headRow - 1) * 10 + 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x1 = g_FieldDebugPages[pageId].x + 2;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y1 =
            g_FieldDebugPages[pageId].y + (g_FieldDebugPages[pageId].headRow - 1) * 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x2 =
            g_FieldDebugPages[pageId].x - 2 + g_FieldDebugPages[pageId].width;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y2 =
            g_FieldDebugPages[pageId].y + (g_FieldDebugPages[pageId].headRow - 1) * 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->r0 = (g_FieldDebugPages[pageId].r * 4) | 0x7F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->g0 = g_FieldDebugPages[pageId].g | 0x3F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->b0 = (g_FieldDebugPages[pageId].b * 2) | 0x3F;
        addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect]);
        g_FieldDebugRRect++;

        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->x0 = g_FieldDebugPages[pageId].x + 2;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->y0 =
            g_FieldDebugPages[pageId].y + (g_FieldDebugPages[pageId].headRow - 1) * 10;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->w = g_FieldDebugPages[pageId].width - 4;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->h = 10;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->r0 = (g_FieldDebugPages[pageId].r * 2) | 0x7F;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->g0 = g_FieldDebugPages[pageId].g >> 1;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->b0 = g_FieldDebugPages[pageId].b | 0x3F;
        addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines]);
        g_FieldDebugRLines++;
    } else {
        if (g_FieldDebugPages[pageId].rowCount) {
            i = g_FieldDebugPages[pageId].rowCount - 1;
        } else {
            i = (g_FieldDebugPages[pageId].height + 2) / 10 - 1;
        }
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x0 = g_FieldDebugPages[pageId].x + 2;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y0 = g_FieldDebugPages[pageId].y + i * 10 + 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x1 =
            g_FieldDebugPages[pageId].x - 2 + g_FieldDebugPages[pageId].width;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y1 = g_FieldDebugPages[pageId].y + i * 10 + 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x2 =
            g_FieldDebugPages[pageId].x - 2 + g_FieldDebugPages[pageId].width;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y2 = g_FieldDebugPages[pageId].y + i * 10;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->r0 = (g_FieldDebugPages[pageId].r >> 1) | 0x3F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->g0 = (g_FieldDebugPages[pageId].g * 4) | 0x7F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->b0 = g_FieldDebugPages[pageId].b | 0x3F;
        addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect]);
        g_FieldDebugRRect++;
    }
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x0 = g_FieldDebugPages[pageId].x;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y0 =
        g_FieldDebugPages[pageId].y + g_FieldDebugPages[pageId].height;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x1 =
        g_FieldDebugPages[pageId].x + g_FieldDebugPages[pageId].width;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y1 =
        g_FieldDebugPages[pageId].y + g_FieldDebugPages[pageId].height;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x2 =
        g_FieldDebugPages[pageId].x + g_FieldDebugPages[pageId].width;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y2 = g_FieldDebugPages[pageId].y;
    if (pageId == (u8)g_FieldDebugCurPage) {
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->r0 = g_FieldDebugPages[pageId].r >> 1;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->g0 = g_FieldDebugPages[pageId].g >> 1;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->b0 = g_FieldDebugPages[pageId].b >> 1;
    } else {
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->r0 = g_FieldDebugPages[pageId].r;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->g0 = g_FieldDebugPages[pageId].g;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->b0 = g_FieldDebugPages[pageId].b;
    }
    addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect]);
    g_FieldDebugRRect++;

    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x0 = g_FieldDebugPages[pageId].x;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y0 =
        g_FieldDebugPages[pageId].y + g_FieldDebugPages[pageId].height;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x1 = g_FieldDebugPages[pageId].x;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y1 = g_FieldDebugPages[pageId].y;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->x2 =
        g_FieldDebugPages[pageId].x + g_FieldDebugPages[pageId].width;
    (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->y2 = g_FieldDebugPages[pageId].y;
    if (pageId == (u8)g_FieldDebugCurPage) {
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->r0 = (g_FieldDebugPages[pageId].r * 4) | 0x7F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->g0 = (g_FieldDebugPages[pageId].g * 4) | 0x7F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->b0 = (g_FieldDebugPages[pageId].b * 4) | 0x7F;
    } else {
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->r0 = (g_FieldDebugPages[pageId].r * 2) | 0x3F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->g0 = (g_FieldDebugPages[pageId].g * 2) | 0x3F;
        (&D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect])->b0 = (g_FieldDebugPages[pageId].b * 2) | 0x3F;
    }
    addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E3B28[g_FieldDebugRb][g_FieldDebugRRect]);
    g_FieldDebugRRect++;

    (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->x0 = g_FieldDebugPages[pageId].x;
    (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->y0 = g_FieldDebugPages[pageId].y;
    (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->w = g_FieldDebugPages[pageId].width;
    (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->h = g_FieldDebugPages[pageId].height;
    if (pageId == (u8)g_FieldDebugCurPage) {
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->r0 = g_FieldDebugPages[pageId].r * 2;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->g0 = g_FieldDebugPages[pageId].g * 2;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->b0 = g_FieldDebugPages[pageId].b * 2;
    } else {
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->r0 = g_FieldDebugPages[pageId].r;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->g0 = g_FieldDebugPages[pageId].g;
        (&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines])->b0 = g_FieldDebugPages[pageId].b;
    }
    switch (g_FieldDebugTransp) {
    case 1:
        setSemiTrans(&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines], 1);
        addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines]);
        g_FieldDebugRLines++;
        break;
    case 2:
        setSemiTrans(&D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines], 0);
        addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E3FA8[g_FieldDebugRb][g_FieldDebugRLines]);
        g_FieldDebugRLines++;
        break;
    case 3:
        break;
    }
    addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E4128[g_FieldDebugRb][g_FieldDebugRDm]);
    g_FieldDebugRDm++;
}

void FieldDebugRenderString(s16 pageId, s16 row, char* str, s32 x, s32 y) {
    s16 code;

    while (*str) {
        if (x > g_FieldDebugPages[pageId].x + g_FieldDebugPages[pageId].width - 8) {
            break;
        }
        if (g_FieldDebugRChars >= 0x158) {
            break;
        }
        switch (*str) {
        case ' ':
            code = 0x3F;
            break;
        case ':':
            code = 0xD5;
            break;
        case '.':
            code = 0xB2;
            break;
        case '+':
            code = 0xB3;
            break;
        case '/':
            code = 0xD4;
            break;
        case '-':
            code = 0xD0;
            break;
        case '*':
            code = 0xCF;
            break;
        case '!':
            code = 0xAE;
            break;
        case '?':
            code = 0xAF;
            break;
        case '=':
            code = 0xDA;
            break;
        case '#':
            code = 0xD6;
            break;
        case '>':
            code = 0xD9;
            break;
        default:
            if (*str < ':') {
                code = *str + 3;
            } else if (*str >= 'a') {
                code = *str + 0x53;
            } else {
                code = *str + 0x73;
            }
            break;
        }
        str++;
        setUV0(&D_800E1028[g_FieldDebugRb][g_FieldDebugRChars], (code & 0xF) * 8 - 0x80, ((code >> 1) & 0x78) - 0x80);
        setXY0(&D_800E1028[g_FieldDebugRb][g_FieldDebugRChars], x, y);
        D_800E1028[g_FieldDebugRb][g_FieldDebugRChars].clut = D_800E4200[g_FieldDebugPages[pageId].rowColor[row]];
        addPrim(&D_800E41C8[g_FieldDebugRb][pageId], &D_800E1028[g_FieldDebugRb][g_FieldDebugRChars]);
        g_FieldDebugRChars++;
        x += 8;
    }
}

s32 AddStrNextDebugRow(s16 pageId, const char* msg) {
    FieldDebugStringCopy(g_FieldDebugPages[pageId].text[g_FieldDebugPages[pageId].rowCount], msg);
    g_FieldDebugPages[pageId].rowCount++;
    if (g_FieldDebugPages[pageId].rowCount > (g_FieldDebugPages[pageId].height - 8) / 10) {
        g_FieldDebugPages[pageId].rowCount = 0;
    }
    D_8009D824 = 1;
    return 1;
}

s32 AddColorStrNextDebugRow(s32 pageId, const char* msg, u8 color) {
    s16 page;

    page = pageId;
    FieldDebugStringCopy(g_FieldDebugPages[page].text[g_FieldDebugPages[page].rowCount], msg);
    g_FieldDebugPages[page].rowColor[g_FieldDebugPages[page].rowCount] = color;
    g_FieldDebugPages[page].rowCount++;
    if (g_FieldDebugPages[page].rowCount > (g_FieldDebugPages[page].height - 8) / 10) {
        g_FieldDebugPages[page].rowCount = 0;
    }
    D_8009D824 = 1;
    return 1;
}

s32 SetStrToDebugRow(s16 pageId, s16 row, const char* str) {
    FieldDebugStringCopy(g_FieldDebugPages[pageId].text[row], str);
    D_8009D824 = 1;
    return 1;
}

s32 SetDebugStrRowColor(s16 pageId, s16 row, u8 color) {
    g_FieldDebugPages[pageId].rowColor[row] = color;
    return 1;
}

static void FieldDebugPageSetHeadRow(s16 pageId, s16 row) {
    g_FieldDebugPages[pageId].headRow = row;
    D_8009D824 = 1;
}

void FieldDebugPageSetColor(s16 pageId, u8 r, u8 g, u8 b) {
    if (!g_FieldDebugPages[pageId].state) {
        g_FieldDebugPages[pageId].r = r;
        g_FieldDebugPages[pageId].g = g;
        g_FieldDebugPages[pageId].b = b;
        D_8009D824 = 1;
    }
}

void FieldDebugPageNotInit(s16 pageId) {
    g_FieldDebugPages[pageId].state = 1;
    D_8009D824 = 1;
}

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

void FieldDebugStringU8hex(s32 val, char* msgOut) {
    msgOut[1] = '\0';
    msgOut[0] = g_FieldDebugDigits[val & 0xF];
}

void FieldDebugStringU16hex(s32 val, char* msgOut) {
    msgOut[2] = '\0';
    msgOut[0] = g_FieldDebugDigits[(val & 0xF0) >> 4];
    msgOut[1] = g_FieldDebugDigits[val & 0xF];
}

void FieldDebugStringU32hex(s32 val, char* msgOut) {
    msgOut[4] = '\0';
    msgOut[0] = g_FieldDebugDigits[(val & 0xF000) >> 0xC];
    msgOut[1] = g_FieldDebugDigits[(val & 0xF00) >> 8];
    msgOut[2] = g_FieldDebugDigits[(val & 0xF0) >> 4];
    msgOut[3] = g_FieldDebugDigits[val & 0xF];
}

void FieldDebugIntToString(s32 val, char* msgOut) {
    s32 started;
    s32 len;
    s32 div;
    s32 digit;

    started = 0;
    div = 10000;
    len = 0;
    for (; div > 1; div /= 10) {
        digit = val / div;
        if (started || digit) {
            started = 1;
            msgOut[len++] = g_FieldDebugDigits[digit];
        }
        val -= digit * div;
    }
    msgOut[len] = g_FieldDebugDigits[val];
    msgOut[len + 1] = '\0';
}
