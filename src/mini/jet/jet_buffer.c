//! PSYQ=3.3 FORCE_MEM=true

#include "jet_private.h"

static void JetPrimsInit(JetPrimBuffer* prims);

void JetBuffersInit(void) {
    JetPrimBuffer* prims;
    JetBuffer* db;
    u_char* isbg;

    SetDefDrawEnv(&g_JetBuffers[0].draw, 0, 0, 320, 240);
    SetDefDispEnv(&g_JetBuffers[0].disp, 0, 240, 320, 240);
    SetDefDrawEnv(&g_JetBuffers[1].draw, 0, 240, 320, 240);
    SetDefDispEnv(&g_JetBuffers[1].disp, 0, 0, 320, 240);
    db = g_JetBuffers;
    g_JetBuffers[0].draw.isbg = 0;
    // do not fold into a direct store; it stops matching.
    isbg = &db[1].draw.isbg;
    *isbg = 0;
    setRGB0(&g_JetBuffers[0].draw, 0, 0, 8);
    setRGB0(&g_JetBuffers[1].draw, 0, 0, 8);
    SetGeomOffset(160, 160);
    SetGeomScreen(0x100);
    SetDispMask(1);
    SetBackColor(0x80, 0x80, 0x80);
    SetFarColor(0, 0, 8);
    prims = &g_JetBuffers[0].prims;
    JetPrimsInit(prims);
    JetPrimsInit(&g_JetBuffers[1].prims);
    JetPrimCursorsReset(prims);
    JetPrimCursorsReset(&g_JetBuffers[1].prims);
    ClearOTagR(g_JetBuffers[0].ot, LEN(g_JetBuffers[0].ot));
    ClearOTagR(g_JetBuffers[1].ot, LEN(g_JetBuffers[1].ot));
    ClearOTagR(g_JetBuffers[0].ot2, LEN(g_JetBuffers[0].ot2));
    ClearOTagR(g_JetBuffers[1].ot2, LEN(g_JetBuffers[1].ot2));
    *g_JetBufferPtr = &g_JetBuffers[0];
}

static void JetBufferReset(void) {
    ClearOTagR(g_JetBufferPtr[0]->ot, LEN(g_JetBufferPtr[0]->ot));
    ClearOTagR(g_JetBufferPtr[0]->ot2, LEN(g_JetBufferPtr[0]->ot2));
    JetPrimCursorsReset(&g_JetBufferPtr[0]->prims);
}

// Tag every primitive in the nine pools with its type and length.
static void JetPrimsInit(JetPrimBuffer* prims) {
    s32 i;

    for (i = 0; i < LEN(prims->f3); i++) {
        SetPolyF3(&prims->f3[i]);
    }
    for (i = 0; i < LEN(prims->f4); i++) {
        SetPolyF4(&prims->f4[i]);
    }
    for (i = 0; i < LEN(prims->g3); i++) {
        SetPolyG3(&prims->g3[i]);
    }
    for (i = 0; i < LEN(prims->g4); i++) {
        SetPolyG4(&prims->g4[i]);
    }
    for (i = 0; i < LEN(prims->ft3); i++) {
        SetPolyFT3(&prims->ft3[i]);
    }
    for (i = 0; i < LEN(prims->ft4); i++) {
        SetPolyFT4(&prims->ft4[i]);
    }
    for (i = 0; i < LEN(prims->gt3); i++) {
        SetPolyGT3(&prims->gt3[i]);
    }
    for (i = 0; i < LEN(prims->gt4); i++) {
        SetPolyGT4(&prims->gt4[i]);
    }
    for (i = 0; i < LEN(prims->line); i++) {
        SetLineF2(&prims->line[i]);
    }
}

void JetPrimCursorsReset(JetPrimBuffer* prims) {
    prims->f3Cursor = prims->f3;
    prims->f4Cursor = prims->f4;
    prims->g3Cursor = prims->g3;
    prims->g4Cursor = prims->g4;
    prims->ft3Cursor = prims->ft3;
    prims->ft4Cursor = prims->ft4;
    prims->gt3Cursor = prims->gt3;
    prims->gt4Cursor = prims->gt4;
    prims->lineCursor = prims->line;
}
