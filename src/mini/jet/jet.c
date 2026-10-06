//! PSYQ=3.3 FORCE_MEM=true FORCE_ADDR=true COMM=true

#include <game.h>
#include "jet_private.h"
#include <libc.h>

#define JET_ASSET_ADDR ((u_long*)0x800F0000)

// Argument block for the GTE renderers in jet_gte.s.
typedef struct {
    /* 0x0 */ JetTriangle* tris;
    /* 0x4 */ POLY_G3* prim;
    /* 0x8 */ OT_TYPE* ot;
    /* 0xC */ JetModel* model;
} JetModelDrawArgs; // size: 0x10

#define JET_LIST_END 0xFFFF

// prev and next index the array the draw list orders.
typedef struct {
    /* 0x0 */ u16 prev;
    /* 0x2 */ u16 next;
} JetListLink; // size: 0x4

u8 g_JetSfxChannel;
volatile s32 D_800A8A84;
void* D_800A891C;
void* D_800A8920;
s32 g_JetTrackSegmentsCrossed;
s16 g_JetBeam0Vertex2X;
s16 g_JetBeam0Vertex2Y;
s16 g_JetBeam1Vertex2X;
s16 g_JetBeam1Vertex2Y;
SVECTOR* g_JetTrackPath;
s32 g_JetFogNear;
s32 g_JetFogFar;
u16 g_JetTrackListHead;
s32 g_JetLaserPitch;
u_long* g_JetTexAdr[10]; // TEXADR.BIN: TIM pointers into TEX.BIN
u16 g_JetTriangleListHead;
s32 g_JetPadDir; // 1..9 keypad layout, 0 = none
s32 D_800A8A7C;
s32 g_JetStartHeldFrames;
JetNode* g_JetPopupNode[1];
s32* g_JetTrackPathOffsets;
u32 g_JetTrackListsPrevPos;
u8 g_JetPaused;
void* D_800D16D4;
s32 g_JetTrackPathLength;
u32 g_JetTrackListsPos;
u8 g_JetAimMode;
u16* g_JetTrackAddCursor;
void* D_800D1A38;
void* D_800D1A3C;
MATRIX* g_JetViewMatrix;
MATRIX* g_JetWorldMatrix;
u16 g_JetTrackListCount;
SVECTOR* g_JetTrackLeft;
u16* g_JetTriangleAddCursor;
u16 g_JetTriangleListCount;
u16 g_JetTrackListTail;
u16 g_JetTriangleListTail;
DR_MODE g_JetDrawMode;
JetListLink g_JetTrackLinks[9000];
u8 g_JetInitialTrackSegmentPending;
volatile s32 D_800E25FC;
s32* g_JetTrackPathLengths;
JetListLink g_JetTriangleLinks[12000];
u16* g_JetTrackRemoveCursor;
SVECTOR* g_JetTrackRight;
MATRIX g_JetCameraRot;
SVECTOR* g_JetTrackRot;
u16* g_JetTriangleRemoveCursor;
s16 g_JetBeam0OriginX;
s16 g_JetBeam0OriginY;
s16 g_JetBeam1OriginX;
s16 g_JetBeam1OriginY;
s32 g_JetSpeed;
u16 g_JetSpriteTPage[12];
u16 g_JetFadeClut;
s16 g_JetPopupTimer;
s16 g_JetPopupModelId;
u16 g_JetFadeTPage;
s32 g_JetScore;
s32 g_JetTrackSegment;
u8 g_JetBeamScroll;
JetModel* g_JetModelTable[100];
JetXbinAdr g_JetXbinAdr;
u8 g_JetFiring;
s32 g_JetCameraPathPos;
s16 g_JetShotPower;
u8 g_JetShotRepeatCounter;
s16 g_JetCursorX;
s16 g_JetCursorY;
u8 g_JetScorePopupAlternate;
u8 g_JetDrawEnabled;
u8 g_JetExit;
SVECTOR g_JetPopupRot;
u16 g_JetSpriteClut[12];
extern void* D_80110BB8;
void* JetDrawModelTris(JetModelDrawArgs* args);
void JetProject3Points(SVECTOR* points, u_long* screen);
void JetProject6Points(SVECTOR* points, u_long* screen);
void* JetDrawModelTrisUI(JetModelDrawArgs* args);
POLY_G3* JetDrawTriangle(JetTriangle* arg0, POLY_G3* arg1, OT_TYPE* arg2, JetTriangle* arg3);
POLY_FT4* JetDrawTrackQuad(SVECTOR* arg0, POLY_FT4* arg1, OT_TYPE* arg2, SVECTOR* arg3);

static void JetDrawEnergyGauge();
static void JetDrawNumber(s32 value, s32 x, s32 y, s16 zeroPad, u16 textureV);
static void JetDrawScorePopup(JetBuffer* db, s16 modelId, s32 rotationX, s32 rotationY, s32 rotationZ);
static void JetDrawSprite(
    s16 spriteId, s16 x, s16 y, s16 w, s16 h, u8 u, u8 v, u8 textureWidth, u8 textureHeight, u8 semiTrans);
static void JetDrawTrack(void);
static void JetDrawTriangleList(void);
static void JetSetWorldMatrix();
static void JetQueueTPageResets();
static void JetInitialize(void);
static void JetSpriteTablesInit(void);
static void JetLoadTim(u_long* tim);
static void JetAudioInit(void);
static void JetAudioUpdateVolumes(void);
static void JetTrackInit(void);
static void JetCameraUpdate(void);
static void JetTrackPathLoad(s32 pathIndex, s32 unused);
static void JetInputUpdate(void);
static void JetDrawListsInit(void);
static void JetTrackListsAdvance(s32 speed);
static void JetTrackListsClean(s32 unusedArg);
static void JetTriangleListAppend(u16 triangleId);
static void JetTriangleListRemove(u16 triangleId);
static void JetTrackListAppend(u16 trackId);
static void JetTrackListRemove(u16 trackId);
static void JetLoadAssets(void);

static const RECT D_800A0000 = {0, 0, 320, 200};

enum JetLba {
    LBA_MINI_TEXADR = 2520,  // MINI/TEXADR.BIN
    LBA_MINI_TEX = 2521,     // MINI/TEX.BIN
    LBA_MINI_XBINADR = 2531, // MINI/XBINADR.BIN
    LBA_MINI_XBIN2 = 2532,   // MINI/XBIN2.BIN
};

// MINI/ files read by JetLoadAssets.
static Yamada g_JetAssetFiles[4] = {
    {LBA_MINI_TEXADR, 0x28},
    {LBA_MINI_TEX, 0x4DE8},
    {LBA_MINI_XBINADR, 0x44},
    {LBA_MINI_XBIN2, 0xA7958},
};
static s32 D_800A8330 = 0x7F;
static s32 D_800A8334 = 0x7F;
static s32 g_JetSlot0Volume = 0;
static s32 g_JetLaserVolume = 0;
static MATRIX D_800A8340 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
static MATRIX D_800A8360 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
static MATRIX g_JetCameraRollMatrix = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
static SVECTOR g_JetCameraRoll = {0, 0, 0, 0};
static VECTOR g_JetCameraPos = {0, 0, 0, 0};
VECTOR g_JetCameraPosCopy = {0, 0, 0, 0};
static VECTOR D_800A83C8 = {0, 0, 0, 0};
static VECTOR D_800A83D8 = {0, 0, 0, 0};
static s32 D_800A83E8[2] = {0, 0};

u16 MINI_Jet(void) {
    s32 unused[2];
    JetBuffer* next;
    JetBuffer* current;

    JetInitialize();
    SetDrawMode(&g_JetDrawMode, 0, 1, GetTPage(1, 1, 768, 0), NULL);
    g_JetTrackRot = g_JetXbinAdr.trackRotations;
    JetTrackPathLoad(0, 0);
    g_JetTrackLeft = g_JetTrackPath;
    JetTrackPathLoad(1, 0);
    g_JetTrackRight = g_JetTrackPath;
    JetAudioInit();
    SetFogNearFar(g_JetFogNear, g_JetFogFar, 256);
    g_JetPopupNode[0] = JetNodeAlloc(JET_MODEL_BLUE_PLANE, 0, 0, 1, &g_JetRootNode, 1200, 50, 3000, 0, 1000, 0);
    while (1) {
        if ((g_JetTrackSegment * 4) > (g_JetTrackPathLength - 0x10) || g_JetExit == 1) {
            break;
        }
        JetInputUpdate();
        if (g_JetPaused == 0) {
            JetCameraUpdate();
            JetTrackListsAdvance(g_JetSpeed);
            JetSetWorldMatrix();
            JetDrawTrack();
            JetDrawTriangleList();
            JetDrawScorePopup(g_JetBufferPtr[0], g_JetPopupModelId, 5, 40, 0);
            JetTrackListsClean(g_JetSpeed);
            JetObjectsUpdate(g_JetBufferPtr[0]);
            JetDrawNumber(g_JetScore, 244, 200, 0, 0);
            JetDrawSprite(7, 204, 200, 39, 17, 0, 0, 0x27, 0x11, 0);
            JetDrawSprite(11, 18, 86, 12, 140, 0, 0x70, 0xC, 0x8C, 0);
            JetDrawEnergyGauge();
            if (g_JetSpeed < 16384) {
                g_JetSlot0Volume = 0;
            } else {
                g_JetSlot0Volume = AKAO_VOL_MAX;
            }
        } else {
            JetDrawSprite(9, 202, 192, 96, 32, 0, 0x50, 0x60, 0x20, 0);
            g_JetSlot0Volume = 0;
            g_JetLaserVolume = 0;
        }
        JetAudioUpdateVolumes();
        JetQueueTPageResets();
        JetDrawSprite(10, 200, 192, 111, 31, 0, 0x30, 0x70, 0x20, 0);
        DrawSync(0);
        VSync(0);
        ResetGraph(1);
        PutDrawEnv(&g_JetBufferPtr[0]->draw);
        PutDispEnv(&g_JetBufferPtr[0]->disp);
        ClearImage(&g_JetBufferPtr[0]->draw.clip, 0, 0, 0);
        if (g_JetDrawEnabled) {
            DrawOTag(&g_JetBufferPtr[0]->ot[LEN(g_JetBufferPtr[0]->ot) - 1]);
            DrawOTag(&g_JetBufferPtr[0]->ot2[LEN(g_JetBufferPtr[0]->ot2) - 1]);
        }
        next = g_JetBuffers;
        current = g_JetBufferPtr[0];
        D_800E25FC = 0;
        if (current == next) {
            next++;
        }
        g_JetBufferPtr[0] = next;
        ClearOTagR(g_JetBufferPtr[0]->ot, LEN(g_JetBufferPtr[0]->ot));
        ClearOTagR(g_JetBufferPtr[0]->ot2, LEN(g_JetBufferPtr[0]->ot2));
        JetPrimCursorsReset(&g_JetBufferPtr[0]->prims);
    }
    g_AkaoCmd.opcode = AKAO_SET_ALL_VOL_BALANCE;
    g_AkaoCmd.params[0] = 0;
    AkaoExec();
    return g_JetScore;
}

// Draw one object's model, project its bounding box and flag a cursor hit.
void JetDrawObjectAndCheckHit(JetBuffer* db, JetNode* node, s16 otIndex, s32 unusedArg, JetObject* object) {
    JetModelDrawArgs args;
    s16 xs[6];
    s16 ys[6];
    MATRIX* m;
    s16 minX;
    s16 maxX;
    s16 minY;
    s16 maxY;
    s16 i;

    m = g_JetWorldMatrix;
    m->m[0][0] = node->m.m[0][0];
    m->m[0][1] = node->m.m[0][1];
    m->m[0][2] = node->m.m[0][2];
    m->m[1][0] = node->m.m[1][0];
    m->m[1][1] = node->m.m[1][1];
    m->m[1][2] = node->m.m[1][2];
    m->m[2][0] = node->m.m[2][0];
    m->m[2][1] = node->m.m[2][1];
    m->m[2][2] = node->m.m[2][2];
    m->t[0] = node->m.t[0];
    m->t[1] = node->m.t[1];
    m->t[2] = node->m.t[2];
    if (node->parent != &g_JetRootNode) {
        CompMatrix(&node->parent->m, m, m);
    }
    g_JetWorldMatrix->t[0] -= g_JetCameraPos.vx;
    g_JetWorldMatrix->t[1] -= g_JetCameraPos.vy;
    g_JetWorldMatrix->t[2] -= g_JetCameraPos.vz;
    gte_SetRotMatrix(&g_JetCameraRot);
    gte_ldclmv(&g_JetWorldMatrix->m[0][0]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][0]);
    gte_ldclmv(&g_JetWorldMatrix->m[0][1]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][1]);
    gte_ldclmv(&g_JetWorldMatrix->m[0][2]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][2]);
    gte_SetTransMatrix(&g_JetCameraRot);
    gte_ldlv0(&g_JetWorldMatrix->t[0]);
    gte_rt();
    gte_stlvl(&g_JetWorldMatrix->t[0]);
    gte_SetRotMatrix(g_JetWorldMatrix);
    gte_SetTransMatrix(g_JetWorldMatrix);
    args.tris = node->model->tris;
    args.prim = db->prims.g3Cursor;
    args.ot = &db->ot[otIndex];
    args.model = node->model;
    db->prims.g3Cursor = JetDrawModelTris(&args);
    JetProject6Points(object->boxFaceCentres, object->boxFaceScreenXY);
    ys[0] = object->boxFaceScreenXY[0] >> 16;
    minY = ys[0];
    maxY = minY;
    xs[0] = object->boxFaceScreenXY[0];
    minX = xs[0];
    maxX = minX;
    for (i = 1; i < LEN(object->boxFaceScreenXY); i++) {
        ys[i] = (object->boxFaceScreenXY[i] & 0xFFFF0000) >> 16;
        xs[i] = object->boxFaceScreenXY[i];
        if (minX > xs[i]) {
            minX = xs[i];
        }
        if (maxX < xs[i]) {
            maxX = xs[i];
        }
        if (minY > ys[i]) {
            minY = ys[i];
        }
        if (maxY < ys[i]) {
            maxY = ys[i];
        }
    }
    if (JetVectorInsidePlanes((VECTOR*)g_JetWorldMatrix->t)) {
        object->state.hit = 0;
        if (g_JetCursorX < maxX && minX < g_JetCursorX && g_JetCursorY < maxY && minY < g_JetCursorY &&
            g_JetFiring == 1) {
            object->state.hit = g_JetFiring;
        }
    }
}

void JetDrawCartAndProjectBeams(JetBuffer* db, JetNode* node, s16 otIndex, s32 unusedArg, JetObject* object) {
    JetModelDrawArgs args;
    MATRIX unused;
    u_long screen[12];
    MATRIX* m;

    m = g_JetWorldMatrix;
    m->m[0][0] = node->m.m[0][0];
    m->m[0][1] = node->m.m[0][1];
    m->m[0][2] = node->m.m[0][2];
    m->m[1][0] = node->m.m[1][0];
    m->m[1][1] = node->m.m[1][1];
    m->m[1][2] = node->m.m[1][2];
    m->m[2][0] = node->m.m[2][0];
    m->m[2][1] = node->m.m[2][1];
    m->m[2][2] = node->m.m[2][2];
    m->t[0] = node->m.t[0];
    m->t[1] = node->m.t[1];
    m->t[2] = node->m.t[2];
    if (node->parent != &g_JetRootNode) {
        CompMatrix(&node->parent->m, m, m);
    }
    g_JetWorldMatrix->t[0] -= g_JetCameraPos.vx;
    g_JetWorldMatrix->t[1] -= g_JetCameraPos.vy;
    g_JetWorldMatrix->t[2] -= g_JetCameraPos.vz;
    gte_SetRotMatrix(&g_JetCameraRot);
    gte_ldclmv(&g_JetWorldMatrix->m[0][0]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][0]);
    gte_ldclmv(&g_JetWorldMatrix->m[0][1]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][1]);
    gte_ldclmv(&g_JetWorldMatrix->m[0][2]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][2]);
    gte_SetTransMatrix(&g_JetCameraRot);
    gte_ldlv0(&g_JetWorldMatrix->t[0]);
    gte_rt();
    gte_stlvl(&g_JetWorldMatrix->t[0]);
    gte_SetRotMatrix(g_JetWorldMatrix);
    gte_SetTransMatrix(g_JetWorldMatrix);
    args.tris = node->model->tris;
    args.prim = db->prims.g3Cursor;
    args.ot = &db->ot2[otIndex];
    args.model = node->model;
    db->prims.g3Cursor = JetDrawModelTris(&args);
    JetProject3Points(&g_JetModelTable[JET_MODEL_BEAM_ORIGINS]->tris[0].v0, screen);
    g_JetBeam0OriginY = screen[1] >> 16;
    g_JetBeam0OriginX = screen[1];
    g_JetBeam0Vertex2Y = screen[2] >> 16;
    g_JetBeam0Vertex2X = screen[2];
    JetProject3Points(&g_JetModelTable[JET_MODEL_BEAM_ORIGINS]->tris[1].v0, screen);
    g_JetBeam1OriginY = screen[1] >> 16;
    g_JetBeam1OriginX = screen[1];
    g_JetBeam1Vertex2Y = screen[2] >> 16;
    g_JetBeam1Vertex2X = screen[2];
}

// Load a node's matrix into the GTE and draw its model's triangles.
static void JetDrawNodeUI(JetBuffer* db, JetNode* node, s16 otIndex, s32 arg3, s32 arg4) {
    JetModelDrawArgs args;
    MATRIX* m;

    m = g_JetWorldMatrix;
    m->m[0][0] = node->m.m[0][0];
    m->m[0][1] = node->m.m[0][1];
    m->m[0][2] = node->m.m[0][2];
    m->m[1][0] = node->m.m[1][0];
    m->m[1][1] = node->m.m[1][1];
    m->m[1][2] = node->m.m[1][2];
    m->m[2][0] = node->m.m[2][0];
    m->m[2][1] = node->m.m[2][1];
    m->m[2][2] = node->m.m[2][2];
    m->t[0] = node->m.t[0];
    m->t[1] = node->m.t[1];
    m->t[2] = node->m.t[2];
    gte_SetRotMatrix(g_JetWorldMatrix);
    gte_SetTransMatrix(g_JetWorldMatrix);
    args.tris = node->model->tris;
    args.prim = db->prims.g3Cursor;
    args.ot = &db->ot2[otIndex];
    args.model = node->model;
    db->prims.g3Cursor = JetDrawModelTrisUI(&args);
}

// Draw every background triangle on the draw list, front to back.
static void JetDrawTriangleList(void) {
    JetListLink* list;
    JetTriangle* tris;
    u16 triId;
    POLY_G3* prim;

    prim = g_JetBufferPtr[0]->prims.g3Cursor;
    tris = g_JetXbinAdr.triangles;
    if (g_JetTriangleListCount) {
        triId = g_JetTriangleListHead;
        list = g_JetTriangleLinks;
        do {
            prim = JetDrawTriangle(&tris[triId], prim, g_JetBufferPtr[0]->ot, &tris[triId]);
            triId = list[triId].next;
        } while (triId != JET_LIST_END);
    }
    g_JetBufferPtr[0]->prims.g3Cursor = prim;
}

// Draw every track element on the draw list, front to back.
static void JetDrawTrack(void) {
    u16 trackId;
    POLY_FT4* prim;

    trackId = g_JetTrackListHead;
    prim = g_JetBufferPtr[0]->prims.ft4Cursor;
    do {
        prim = JetDrawTrackQuad(&g_JetTrackLeft[trackId], prim, g_JetBufferPtr[0]->ot, &g_JetTrackRight[trackId]);
        trackId = g_JetTrackLinks[trackId].next;
    } while (trackId != JET_LIST_END);
    g_JetBufferPtr[0]->prims.ft4Cursor = prim;
}

// Build the world matrix from the camera rotation and the view position.
static void JetSetWorldMatrix(void) {

    g_JetViewMatrix->t[0] = -g_JetCameraPos.vx;
    g_JetViewMatrix->t[1] = -g_JetCameraPos.vy;
    g_JetViewMatrix->t[2] = -g_JetCameraPos.vz;
    gte_SetRotMatrix(&g_JetCameraRot);
    gte_ldclmv(&g_JetViewMatrix->m[0][0]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][0]);
    gte_ldclmv(&g_JetViewMatrix->m[0][1]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][1]);
    gte_ldclmv(&g_JetViewMatrix->m[0][2]);
    gte_rtir();
    gte_stclmv(&g_JetWorldMatrix->m[0][2]);
    gte_SetTransMatrix(&g_JetCameraRot);
    gte_ldlv0(&g_JetViewMatrix->t[0]);
    gte_rt();
    gte_stlvl(&g_JetWorldMatrix->t[0]);
    gte_SetRotMatrix(g_JetWorldMatrix);
    gte_SetTransMatrix(g_JetWorldMatrix);
}

void JetTrackSample(u32 trackPosition, s32 heightOffset, VECTOR* position, SVECTOR* rotation) {
    VECTOR left;
    VECTOR right;
    VECTOR mid;
    VECTOR nextMid;
    VECTOR curMid;
    VECTOR across;
    VECTOR along;
    VECTOR normal;
    VECTOR unit;
    VECTOR dLeft;
    VECTOR dRight;
    SVECTOR* rotCur;
    SVECTOR* rotNext;
    SVECTOR* leftCur;
    SVECTOR* leftNext;
    SVECTOR* rightCur;
    SVECTOR* rightNext;
    s32 seg;
    s32 frac;
    s32 dx;
    s32 dy;
    s32 dz;

    seg = trackPosition >> 16;
    frac = trackPosition & 0xFFFF;
    rotCur = &g_JetTrackRot[seg];
    rotNext = &g_JetTrackRot[seg + 1];
    dx = rotNext->vx - rotCur->vx;
    dy = rotCur->vy - rotNext->vy;
    dz = rotNext->vz - rotCur->vz;
    if (dx > 0x800) {
        dx -= 0x1000;
    }
    if (dy > 0x800) {
        dy -= 0x1000;
    }
    if (dz > 0x800) {
        dz -= 0x1000;
    }
    if (dx < -0x800) {
        dx += 0x1000;
    }
    if (dy < -0x800) {
        dy += 0x1000;
    }
    if (dz < -0x800) {
        dz += 0x1000;
    }
    dx *= frac;
    dy *= frac;
    dz *= frac;
    dx >>= 16;
    dy >>= 16;
    dz >>= 16;

    leftCur = &g_JetTrackLeft[seg];
    leftNext = &g_JetTrackLeft[seg + 1];
    dLeft.vx = (leftNext->vx - leftCur->vx) * frac;
    dLeft.vy = (leftNext->vy - leftCur->vy) * frac;
    dLeft.vz = (leftNext->vz - leftCur->vz) * frac;

    rightCur = &g_JetTrackRight[seg];
    rightNext = &g_JetTrackRight[seg + 1];
    left.vx = leftCur->vx + (dLeft.vx >> 16);
    left.vy = leftCur->vy + (dLeft.vy >> 16);
    left.vz = leftCur->vz + (dLeft.vz >> 16);

    dRight.vx = (rightNext->vx - rightCur->vx) * frac;
    dRight.vy = (rightNext->vy - rightCur->vy) * frac;
    dRight.vz = (rightNext->vz - rightCur->vz) * frac;

    right.vx = rightCur->vx + (dRight.vx >> 16);
    right.vy = rightCur->vy + (dRight.vy >> 16);
    right.vz = rightCur->vz + (dRight.vz >> 16);
    mid.vx = (right.vx + left.vx) >> 1;
    mid.vy = (right.vy + left.vy) >> 1;
    mid.vz = (right.vz + left.vz) >> 1;

    curMid.vx = (rightCur->vx + leftCur->vx) >> 1;
    curMid.vy = (rightCur->vy + leftCur->vy) >> 1;
    curMid.vz = (rightCur->vz + leftCur->vz) >> 1;

    nextMid.vx = (rightNext->vx + leftNext->vx) >> 1;
    nextMid.vy = (rightNext->vy + leftNext->vy) >> 1;
    nextMid.vz = (rightNext->vz + leftNext->vz) >> 1;
    along.vx = nextMid.vx - curMid.vx;
    along.vy = nextMid.vy - curMid.vy;
    along.vz = nextMid.vz - curMid.vz;

    across.vx = right.vx - left.vx;
    across.vy = right.vy - left.vy;
    across.vz = right.vz - left.vz;

    OuterProduct0(&along, &across, &normal);
    VectorNormal(&normal, &unit);

    position->vx = (s16)mid.vx + ((unit.vx * heightOffset) >> 12);
    position->vy = (s16)mid.vy + ((unit.vy * heightOffset) >> 12);
    position->vz = (s16)mid.vz + ((unit.vz * heightOffset) >> 12);

    rotation->vx = rotCur->vx + dx;
    rotation->vy = dy - rotCur->vy;
    rotation->vz = rotCur->vz + dz;
}

static void JetDrawEnergyGauge(void) {
    JetBuffer** db;
    POLY_G4* poly;
    s16 power;
    s32 top;

    db = g_JetBufferPtr;
    poly = db[0]->prims.g4Cursor;
    power = g_JetShotPower;
    top = 220 - power;
    setXY4(poly, 20, top, 28, top, 20, 220, 28, 220);
    setRGB0(poly, -0x80 - power, power, 0);
    setRGB1(poly, -0x80 - power, power, 0);
    setRGB2(poly, 0x80, 0, 0);
    setRGB3(poly, 0x80, 0, 0);
    SetSemiTrans(poly, 0);
    addPrim(&db[0]->ot2[1], poly);
    poly++;
    db[0]->prims.g4Cursor = poly;
}

// Spin and draw the score model, alternating it with the title every so often.
static void JetDrawScorePopup(JetBuffer* db, s16 modelId, s32 rotationX, s32 rotationY, s32 rotationZ) {
    s32 unused;

    if (modelId == 0 || modelId == JET_MODEL_UFO) {
        return;
    }
    g_JetPopupNode[0]->model = g_JetModelTable[modelId];
    g_JetPopupRot.vx += rotationX;
    g_JetPopupRot.vy += rotationY;
    g_JetPopupRot.vz += rotationZ;
    if (g_JetScorePopupAlternate == 1) {
        RotMatrix(&g_JetPopupRot, &g_JetPopupNode[0]->m);
        JetDrawNodeUI(db, g_JetPopupNode[0], 0, 0, unused);
        JetDrawNumber(g_JetPopupPoints, 220, 160, 0, 0x18);
    }
    g_JetPopupTimer--;
    if (g_JetPopupTimer < 50) {
        if (g_JetScorePopupAlternate == 0) {
            g_JetScorePopupAlternate = 1;
        } else {
            g_JetScorePopupAlternate = 0;
        }
    }
    if (g_JetPopupTimer == 0) {
        g_JetPopupModelId = 0;
    }
}

static void JetDrawNumber(s32 value, s32 x, s32 y, s16 zeroPad, u16 textureV) {
    POLY_FT4* poly;
    JetBuffer* db;
    s32 digit;
    s32 power;
    s32 remain;
    s32 i;
    u16 startX;
    s32 w;
    s32 left;
    u8 leading;

    startX = x;
    power = 1000;
    leading = 1;
    remain = value + 1;
    poly = g_JetBufferPtr[0]->prims.ft4Cursor;
    for (i = 0; i < 4; i++) {
        digit = 0;
        while (remain > power) {
            remain -= power;
            digit++;
        }
        if (digit) {
            leading = 0;
        }
        if (value == 0 && power == 1) {
            leading = 0;
        }
        if (zeroPad == 1 || digit || leading == 0) {
            left = x + i * 14;
            w = i * 14 + 16;
            setXY4(poly, left, y, startX + w, y, left, y + 16, startX + w, y + 16);
            setRGB0(poly, 0x80, 0x80, 0x80);
            setUVWH(poly, digit * 0x10 + 0x30, textureV, 0x10, 0x12);
            poly->tpage = g_JetSpriteTPage[8];
            poly->clut = g_JetSpriteClut[8];
            SetSemiTrans(poly, 1);
            db = g_JetBufferPtr[0];
            addPrim(&db->ot2[1], poly);
            poly++;
        }
        power /= 10;
    }
    g_JetBufferPtr[0]->prims.ft4Cursor = poly;
}

// Draw one sprite from the HUD sprite table.
static void JetDrawSprite(
    s16 spriteId, s16 x, s16 y, s16 w, s16 h, u8 u, u8 v, u8 textureWidth, u8 textureHeight, u8 semiTrans) {
    JetBuffer** db;
    POLY_FT4* poly;

    db = g_JetBufferPtr;
    poly = db[0]->prims.ft4Cursor;
    setXYWH(poly, x, y, w, h);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUVWH(poly, u, v, textureWidth, textureHeight);
    poly->tpage = g_JetSpriteTPage[spriteId];
    poly->clut = g_JetSpriteClut[spriteId];
    SetSemiTrans(poly, semiTrans);
    addPrim(&db[0]->ot2[1], poly);
    poly++;
    db[0]->prims.ft4Cursor = poly;
}

static void JetQueueTPageResets(void) {
    JetBuffer** db;
    POLY_FT4* poly;

    db = g_JetBufferPtr;
    poly = db[0]->prims.ft4Cursor;
    setXY4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    poly->tpage = g_JetSpriteTPage[5];
    poly->clut = g_JetSpriteClut[5];
    SetSemiTrans(poly, 0);
    addPrim(&db[0]->ot[LEN(db[0]->ot) - 1], poly);
    poly++;
    setXY4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    poly->tpage = g_JetSpriteTPage[5];
    poly->clut = g_JetSpriteClut[5];
    SetSemiTrans(poly, 0);
    addPrim(&db[0]->ot[2], poly);
    poly++;
    db[0]->prims.ft4Cursor = poly;
}

// Point every matrix and vector at scratchpad, then build the world.
static void JetInitialize(void) {
    s32 i;

    D_800A8A84 = 0;
    D_80110BB8 = (void*)0x1F800000;
    D_800D16D4 = (void*)0x1F800000;
    g_JetPaused = 0;
    g_JetWorldMatrix = (MATRIX*)0x1F800010;
    g_JetViewMatrix = (MATRIX*)0x1F800030;
    D_800D1A38 = (void*)0x1F800050;
    D_800D1A3C = (void*)0x1F800058;
    D_800A891C = (void*)0x1F800060;
    D_800A8920 = (void*)0x1F800064;
    g_JetViewMatrix->m[0][0] = 0x1000;
    g_JetViewMatrix->m[0][1] = 0;
    g_JetViewMatrix->m[0][2] = 0;
    g_JetViewMatrix->m[1][0] = 0;
    g_JetViewMatrix->m[1][1] = 0x1000;
    g_JetViewMatrix->m[1][2] = 0;
    g_JetViewMatrix->m[2][0] = 0;
    g_JetViewMatrix->m[2][1] = 0;
    g_JetViewMatrix->m[2][2] = 0x1000;
    JetBuffersInit();
    JetLoadAssets();
    D_800A8A84 = 0x99;
    JetDrawListsInit();
    JetNodesInit();
    JetModelsReset();
    JetTrackInit();
    JetObjectsInit();
    JetFrustumInit();
    for (i = 0; i < LEN(g_JetModelTable); i++) {
        g_JetModelTable[i] = JetModelBuild(i);
    }
    g_JetSpeed = 10000;
    g_JetFogNear = 10410;
    g_JetFogFar = 14300;
    g_JetTrackSegment = 0;
    g_JetCameraPathPos = 0;
    g_JetScore = 0;
    g_JetDrawEnabled = 0;
    g_JetExit = 0;
    g_JetPopupModelId = 0;
    g_JetScorePopupAlternate = 0;
    g_JetPopupRot.vx = 0;
    g_JetPopupRot.vy = 0;
    g_JetPopupRot.vz = 0;
    g_JetSfxChannel = 0;
    D_800A8330 = 0x7F;
    D_800A8334 = 0x7F;
    g_JetSlot0Volume = 0;
    g_JetLaserVolume = 0;
    g_JetPopupTimer = 0;
}

static void JetLoadAssets(void) {
    RECT unused;

    unused = D_800A0000;

    SystemLoadFileBySector(g_JetAssetFiles[0].loc, g_JetAssetFiles[0].len, (u_long*)g_JetTexAdr, NULL);
    while (SystemCdromReadChain())
        ;
    SystemLoadFileBySector(g_JetAssetFiles[1].loc, g_JetAssetFiles[1].len, JET_ASSET_ADDR, NULL);
    while (SystemCdromReadChain())
        ;

    JetSpriteTablesInit();

    SystemLoadFileBySector(g_JetAssetFiles[2].loc, g_JetAssetFiles[2].len, &g_JetXbinAdr.musicData, NULL);
    while (SystemCdromReadChain())
        ;
    SysCdromStartLoadLzs(g_JetAssetFiles[3].loc, g_JetAssetFiles[3].len, JET_ASSET_ADDR, NULL);
    while (SystemCdromReadChain())
        ;
}

// Upload the nine loaded TIMs and build the sprite tpage/clut tables.
static void JetSpriteTablesInit(void) {
    TIM_IMAGE timimg;
    u_long** tims;
    s32 i;
    u_long* addr;

    // i is created before tims so the two take the registers the target uses.
    i = 0;
    tims = g_JetTexAdr;
    for (; i < 9; i++) {
        addr = *tims++;
        JetLoadTim(addr);
        OpenTIM(addr);
        ReadTIM(&timimg);
    }
    g_JetFadeTPage = GetTPage(0, 2, 0x280, 0);
    g_JetFadeClut = GetClut(0, 0x1E0);
    g_JetSpriteTPage[0] = GetTPage(0, 1, 0x280, 0);
    g_JetSpriteClut[0] = GetClut(0, 0x1E0);
    g_JetSpriteTPage[1] = GetTPage(1, 1, 0x2C0, 0);
    g_JetSpriteClut[1] = GetClut(0, 0x1E1);
    g_JetSpriteTPage[2] = GetTPage(1, 1, 0x2D0, 0);
    g_JetSpriteClut[2] = GetClut(0, 0x1E1);
    g_JetSpriteTPage[3] = GetTPage(1, 1, 0x2E0, 0);
    g_JetSpriteClut[3] = GetClut(0, 0x1E1);
    g_JetSpriteTPage[4] = GetTPage(0, 1, 0x280, 0x100);
    g_JetSpriteClut[4] = GetClut(0, 0x1FF);
    g_JetSpriteTPage[5] = GetTPage(0, 1, 0x280, 0x100);
    g_JetSpriteClut[5] = GetClut(0, 0x1FE);
    g_JetSpriteTPage[6] = GetTPage(0, 1, 0x300, 0);
    g_JetSpriteClut[6] = GetClut(0x10, 0x1E0);
    g_JetSpriteTPage[7] = GetTPage(0, 1, 0x240, 0);
    g_JetSpriteClut[7] = GetClut(0x40, 0x1E0);
    g_JetSpriteTPage[8] = GetTPage(0, 1, 0x240, 0x18);
    g_JetSpriteClut[8] = GetClut(0x30, 0x1E0);
    g_JetSpriteTPage[9] = GetTPage(0, 1, 0x240, 0x50);
    g_JetSpriteClut[9] = GetClut(0x50, 0x1E0);
    g_JetSpriteTPage[10] = GetTPage(0, 1, 0x240, 0x30);
    g_JetSpriteClut[10] = GetClut(0x20, 0x1E0);
    g_JetSpriteTPage[11] = GetTPage(0, 1, 0x240, 0);
    g_JetSpriteClut[11] = GetClut(0x60, 0x1E0);
}

static void JetLoadTim(u_long* tim) {
    TIM_IMAGE timimg;

    OpenTIM(tim);

    while (ReadTIM(&timimg)) {
        if (timimg.caddr) {
            LoadImage(timimg.crect, timimg.caddr);
        }
        if (timimg.paddr) {
            LoadImage(timimg.prect, timimg.paddr);
        }
    }
}

static void JetAudioInit(void) {
    g_AkaoCmd.opcode = AKAO_PLAY_MUSIC;
    g_AkaoCmd.params[0] = g_JetXbinAdr.musicData;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_VOLUME_SET;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_ALL_VOL_BALANCE;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_ALL_PITCH;
    g_AkaoCmd.params[0] = 0;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT0;
    g_AkaoCmd.params[0] = 0;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_PLAY_SLOT0;
    g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
    g_AkaoCmd.params[1] = SFX_JET_TRACK;
    AkaoExec();
}

void JetAudioFadeOut(void) {
    g_AkaoCmd.opcode = AKAO_VOL_SLIDE_FROM_CURR;
    g_AkaoCmd.params[0] = 0xF0;
    g_AkaoCmd.params[1] = 0;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SLIDE_ALL_VOL_BALANCE;
    g_AkaoCmd.params[0] = 0xF0;
    g_AkaoCmd.params[1] = 0;
    AkaoExec();
}

// Alternate the two laser channels on each shot.
void JetPlaySfx(s16 soundId) {
    g_JetSfxChannel = (g_JetSfxChannel + 1) & 1;
    if (g_JetSfxChannel == 0) {
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT2;
        g_AkaoCmd.params[0] = 0;
        AkaoExec();
        g_AkaoCmd.opcode = AKAO_PLAY_SLOT2;
        g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
        g_AkaoCmd.params[1] = soundId;
        AkaoExec();
    }
    if (g_JetSfxChannel == 1) {
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT1;
        g_AkaoCmd.params[0] = 0;
        AkaoExec();
        g_AkaoCmd.opcode = AKAO_PLAY_SLOT1;
        g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
        g_AkaoCmd.params[1] = soundId;
        AkaoExec();
    }
}

static void JetUpdateLaserSfx(s32 power) {
    if (g_JetLaserPitch == 0 && (power & 0xFF)) {
        g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
        g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
        g_AkaoCmd.params[1] = SFX_JET_LASER;
        AkaoExec();
    }
    if (power & 0xFF) {
        g_JetLaserVolume = power & 0xFF;
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT3;
        g_AkaoCmd.params[0] = power & 0xFF;
        AkaoExec();
        g_JetLaserPitch = power & 0xFF;
    } else {
        g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
        g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
        g_AkaoCmd.params[1] = SFX_NULL;
        AkaoExec();
        g_JetLaserPitch = 0;
    }
}

static void JetAudioUpdateVolumes(void) {
    g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT0;
    g_AkaoCmd.params[0] = g_JetSlot0Volume;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT3;
    g_AkaoCmd.params[0] = g_JetLaserVolume;
    AkaoExec();
}

static void JetTrackInit(void) {
    D_800A83C8.vy = -0x1B76;
    D_800A83C8.vx = 0;
    D_800A83C8.vz = 0xC8;
    g_JetTrackPathLengths = g_JetXbinAdr.trackPathLengths;
    g_JetTrackPathOffsets = g_JetXbinAdr.trackPathOffsets;
    JetTrackPathLoad(0, 3);
    g_JetAimMode = 1;
}

// Advance the camera along its path and rebuild the view matrices.
static void JetCameraUpdate(void) {
    VECTOR pos;
    SVECTOR rot;
    SVECTOR camRot;
    s32 step;

    JetTrackSample(g_JetCameraPathPos, -0x64, &pos, &rot);
    g_JetCameraPathPos += g_JetSpeed;
    g_JetCameraPosCopy.vx = pos.vx;
    g_JetCameraPosCopy.vy = pos.vy;
    g_JetCameraPosCopy.vz = pos.vz;
    if (rot.vx < 0) {
        rot.vx += 0x1000;
    }
    g_JetCameraRoll.vz = -rot.vz;
    step = rsin(rot.vx) / 15;
    if (step > 0) {
        if (g_JetSpeed > 43000) {
            g_JetSpeed -= step;
        }
    }
    if (step < 0) {
        if (g_JetSpeed < 120000) {
            g_JetSpeed -= step;
        }
    }
    g_JetCameraPos.vx = pos.vx;
    g_JetCameraPos.vy = pos.vy;
    g_JetCameraPos.vz = pos.vz;
    camRot.vx = -rot.vx;
    camRot.vy = -rot.vy;
    camRot.vz = 0;
    RotMatrix(&camRot, &g_JetCameraRot);
    RotMatrix(&g_JetCameraRoll, &g_JetCameraRollMatrix);
    CompMatrix(&g_JetCameraRollMatrix, &g_JetCameraRot, &g_JetCameraRot);
}

static void JetTrackPathLoad(s32 pathIndex, s32 unused) {
    s32 offset;
    u8* base;

    offset = g_JetTrackPathOffsets[pathIndex & 0xFF];
    base = g_JetXbinAdr.trackPaths;
    g_JetTrackPath = (SVECTOR*)(base + offset);
    g_JetTrackPathLength = *g_JetTrackPathLengths;
}

static void func_800A2E30(void) {}

// Read the pad and drive the cursor, the camera tweaks and the pause toggle.
static void JetInputUpdate(void) {
    u32 pad;

    pad = InputReadPadsRaw(1);
    if (g_JetPaused == 0) {
        g_JetPadDir = 0;
        D_800A8A7C = 0;
        if (pad & PAD_LEFT) {
            g_JetPadDir = 4;
        }
        if (pad & PAD_RIGHT) {
            g_JetPadDir = 6;
        }
        if (pad & PAD_UP) {
            g_JetPadDir = 8;
            if (pad & PAD_LEFT) {
                g_JetPadDir = 7;
            }
            if (pad & PAD_RIGHT) {
                g_JetPadDir = 9;
            }
        }
        if (pad & PAD_DOWN) {
            g_JetPadDir = 2;
            if (pad & PAD_LEFT) {
                g_JetPadDir = 1;
            }
            if (pad & PAD_RIGHT) {
                g_JetPadDir = 3;
            }
        }
        if (g_JetAimMode == 1) {
            if (pad & PAD_DOWN) {
                g_JetCursorY += 5;
            }
            if (pad & PAD_UP) {
                g_JetCursorY -= 5;
            }
            if (pad & PAD_LEFT) {
                g_JetCursorX -= 5;
            }
            if (pad & PAD_RIGHT) {
                g_JetCursorX += 5;
            }
            g_JetFiring = 0;
            if (pad & PAD_CIRCLE) {
                JetUpdateLaserSfx(g_JetShotPower & 0xFF);
                if (g_JetShotPower >= 9) {
                    g_JetShotPower--;
                }
                if (g_JetShotRepeatCounter == 0) {
                    g_JetShotRepeatCounter = 1;
                    g_JetFiring = 1;
                    g_JetBeamScroll = (u8)(g_JetBeamScroll + 3) % 15;
                } else {
                    g_JetShotRepeatCounter--;
                }
            } else {
                JetUpdateLaserSfx(0);
                if (g_JetShotPower < 128) {
                    g_JetShotPower++;
                }
            }
            if (g_JetCursorX > 320) {
                g_JetCursorX = 320;
            }
            if (g_JetCursorX < 0) {
                g_JetCursorX = 0;
            }
            if (g_JetCursorY > 240) {
                g_JetCursorY = 240;
            }
            if (g_JetCursorY < 0) {
                g_JetCursorY = 0;
            }
        }
        if (g_JetAimMode == 0) {
            if (pad & PAD_DOWN) {
                g_JetFogFar -= 10;
            }
            if (pad & PAD_UP) {
                g_JetFogFar += 10;
            }
            if (pad & PAD_LEFT) {
                g_JetFogNear -= 10;
            }
            if (pad & PAD_RIGHT) {
                g_JetFogNear += 10;
            }
            if (pad & PAD_CROSS) {
                D_800A83D8.vz -= 100;
            }
            if (pad & PAD_TRIANGLE) {
                D_800A83D8.vz += 100;
            }
            if (pad & PAD_SQUARE) {
                D_800A83D8.vx -= 100;
            }
            if (pad & PAD_CIRCLE) {
                D_800A83D8.vx += 100;
            }
            if (pad & PAD_R1) {
                D_800A83D8.vy -= 100;
            }
            if (pad & PAD_R2) {
                D_800A83D8.vy += 100;
            }
            if (pad & PAD_L1) {
                g_JetSpeed += 1024;
            }
            if (pad & PAD_L2) {
                if (g_JetSpeed >= 1024) {
                    g_JetSpeed -= 1024;
                }
            }
            if (pad & PAD_START) {
                g_JetSpeed = 0;
            }
        }
    }
    if (pad & PAD_START) {
        g_JetStartHeldFrames++;
    } else {
        g_JetStartHeldFrames = 0;
    }
    if (g_JetStartHeldFrames == 1) {
        if (g_JetPaused == 1) {
            g_JetPaused = 0;
        } else {
            g_JetPaused = 1;
        }
        JetPlaySfx(SFX_BUTTON);
    }
}

// Reset both draw lists and the object streams for a new run.
static void JetDrawListsInit(void) {
    JetListLink* list;
    s32 i;

    g_JetTrackListsPos = 0xFFFE;
    g_JetTrackListsPrevPos = 0;
    g_JetTriangleAddCursor = g_JetXbinAdr.triangleAdds;
    g_JetTriangleRemoveCursor = g_JetXbinAdr.triangleRemoves;
    g_JetTrackAddCursor = g_JetXbinAdr.trackAdds;
    g_JetTrackRemoveCursor = g_JetXbinAdr.trackRemoves;
    list = g_JetTriangleLinks;
    for (i = 0; i < LEN(g_JetTriangleLinks); i++) {
        list[i].prev = JET_LIST_END;
        list[i].next = JET_LIST_END;
    }
    g_JetTriangleListCount = 0;
    list = g_JetTrackLinks;
    for (i = 0; i < LEN(g_JetTrackLinks); i++) {
        list[i].prev = JET_LIST_END;
        list[i].next = JET_LIST_END;
    }
    g_JetTrackListCount = 0;
    g_JetInitialTrackSegmentPending = 1;
}

// Unused: JetTrackListsAdvance and JetTrackListsClean in one pass.
static void JetTrackListsAdvanceAndClean(s32 advance) {
    u32 prev;
    u32 next;
    u32 steps;
    u32 i;
    u32 id;

    g_JetTrackListsPrevPos = g_JetTrackListsPos;
    g_JetTrackListsPos = g_JetTrackListsPrevPos + advance;
    prev = g_JetTrackListsPrevPos >> 18;
    next = g_JetTrackListsPos >> 18;
    steps = next - prev;
    g_JetTrackSegment = g_JetTrackSegment + steps;
    for (i = 0; i < steps + g_JetInitialTrackSegmentPending; i++) {
        while (1) {
            id = *g_JetTriangleAddCursor++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTriangleListAppend(id);
        }
        while (1) {
            id = *g_JetTriangleRemoveCursor++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTriangleListRemove(id);
        }
    }
    for (i = 0; i < steps; i++) {
        while (1) {
            id = *g_JetTrackAddCursor++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTrackListAppend(id);
        }
        while (1) {
            id = *g_JetTrackRemoveCursor++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTrackListRemove(id);
        }
    }
    if (g_JetInitialTrackSegmentPending == 1) {
        g_JetInitialTrackSegmentPending = 0;
    }
}

static void JetTrackListsAdvance(s32 speed) {
    u32 prev;
    u32 next;
    u32 i;
    u32 id;
    u8* first;

    g_JetTrackListsPrevPos = g_JetTrackListsPos;
    g_JetTrackListsPos = g_JetTrackListsPrevPos + speed;
    prev = g_JetTrackListsPrevPos >> 18;
    next = g_JetTrackListsPos >> 18;
    g_JetTrackSegmentsCrossed = next - prev;
    g_JetTrackSegment = g_JetTrackSegment + g_JetTrackSegmentsCrossed;
    for (i = 0; i < g_JetTrackSegmentsCrossed + g_JetInitialTrackSegmentPending; i++) {
        while (1) {
            id = *g_JetTriangleAddCursor++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTriangleListAppend(id);
        }
    }
    for (i = 0; i < g_JetTrackSegmentsCrossed; i++) {
        while (1) {
            id = *g_JetTrackAddCursor++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTrackListAppend(id);
        }
    }
}

static void JetTrackListsClean(s32 unusedArg) {
    u32 i;
    u32 id;

    for (i = 0; i < g_JetTrackSegmentsCrossed + g_JetInitialTrackSegmentPending; i++) {
        while (1) {
            id = *g_JetTriangleRemoveCursor++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTriangleListRemove(id);
        }
    }
    for (i = 0; i < g_JetTrackSegmentsCrossed; i++) {
        while (1) {
            id = *g_JetTrackRemoveCursor++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTrackListRemove(id);
        }
    }
    if (g_JetInitialTrackSegmentPending == 1) {
        g_JetInitialTrackSegmentPending = 0;
    }
}

static void JetTriangleListAppend(u16 triangleId) {
    u16 prev;
    u16 next;

    prev = g_JetTriangleLinks[triangleId].prev;
    next = g_JetTriangleLinks[triangleId].next;
    if (g_JetTriangleListCount == 0) {
        g_JetTriangleListHead = triangleId;
        g_JetTriangleListTail = triangleId;
        g_JetTriangleListCount = 1;
    } else {
        g_JetTriangleLinks[triangleId].prev = g_JetTriangleListTail;
        g_JetTriangleLinks[g_JetTriangleListTail].next = triangleId;
        g_JetTriangleListTail = triangleId;
        g_JetTriangleListCount++;
    }
}

static void JetTriangleListRemove(u16 triangleId) {
    u16 prev;
    u16 next;

    prev = g_JetTriangleLinks[triangleId].prev;
    next = g_JetTriangleLinks[triangleId].next;
    if (prev != JET_LIST_END) {
        g_JetTriangleLinks[prev].next = next;
    } else {
        g_JetTriangleListHead = next;
    }
    if (next != JET_LIST_END) {
        g_JetTriangleLinks[next].prev = prev;
    } else {
        g_JetTriangleListTail = prev;
    }
    g_JetTriangleLinks[triangleId].prev = JET_LIST_END;
    g_JetTriangleLinks[triangleId].next = JET_LIST_END;
    g_JetTriangleListCount--;
}

static void JetTrackListAppend(u16 trackId) {
    if (g_JetTrackListCount == 0) {
        g_JetTrackLinks[trackId].prev = JET_LIST_END;
        g_JetTrackLinks[trackId].next = JET_LIST_END;
        g_JetTrackListHead = trackId;
        g_JetTrackListTail = trackId;
        g_JetTrackListCount = 1;
    } else {
        g_JetTrackLinks[trackId].prev = g_JetTrackListTail;
        g_JetTrackLinks[trackId].next = JET_LIST_END;
        g_JetTrackLinks[g_JetTrackListTail].next = trackId;
        g_JetTrackListTail = trackId;
        g_JetTrackListCount++;
    }
}

static void JetTrackListRemove(u16 trackId) {
    u16 prev;
    u16 next;

    prev = g_JetTrackLinks[trackId].prev;
    next = g_JetTrackLinks[trackId].next;
    if (prev != JET_LIST_END) {
        g_JetTrackLinks[prev].next = next;
    } else {
        g_JetTrackListHead = next;
    }
    if (next != JET_LIST_END) {
        g_JetTrackLinks[next].prev = prev;
    } else {
        g_JetTrackListTail = prev;
    }
    g_JetTrackListCount--;
}
