//! PSYQ=3.3 FORCE_MEM=true

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

extern u8 g_JetSfxChannel;
extern s32 D_800A8A84;
extern void* D_800A891C;
extern void* D_800A8920;
extern s32 g_JetTrackSegmentsCrossed;
extern s16 D_800A8960;
extern s16 D_800A896C;
extern s16 D_800A8974;
extern s16 D_800A8980;
extern SVECTOR* g_JetTrackPath;
extern struct {
    s32 near;
    s32 far;
} g_JetFog;
extern u16 g_JetTrackListHead;
extern s32 g_JetLaserPitch;
extern u_long* g_JetTexAdr[10]; // TEXADR.BIN: TIM pointers into TEX.BIN
extern u16 g_JetTriangleListHead;
extern s32 g_JetPadDir; // 1..9 keypad layout, 0 = none
extern s32 D_800A8A7C;
extern s32 g_JetStartHeldFrames;
extern JetNode* g_JetPopupNode[1];
extern s32* g_JetTrackPathOffsets;
extern u32 g_JetTrackListsPrevPos;
extern u8 g_JetPaused;
extern void* D_800D16D4;
extern s32 g_JetTrackPathLength;
extern u32 g_JetTrackListsPos;
extern u8 g_JetAimMode;
extern u16* g_JetTrackAddCursor;
extern void* D_800D1A38;
extern void* D_800D1A3C;
extern MATRIX* g_JetViewMatrix;
extern MATRIX* g_JetWorldMatrix;
extern u16 g_JetTrackListCount;
extern SVECTOR* g_JetTrackLeft;
extern u16* g_JetTriangleAddCursor;
extern u16 g_JetTriangleListCount;
extern u16 g_JetTrackListTail;
extern u16 g_JetTriangleListTail;
extern DR_MODE D_800D9934;
extern JetListLink g_JetTrackLinks[9000];
extern u8 g_JetInitialTrackSegmentPending;
extern s32 D_800E25FC;
extern s32* g_JetTrackPathLengths;
extern JetListLink g_JetTriangleLinks[12000];
extern u16* g_JetTrackRemoveCursor;
extern SVECTOR* g_JetTrackRight;
extern MATRIX g_JetCameraRot;
extern SVECTOR* g_JetTrackRot;
extern u16* g_JetTriangleRemoveCursor;
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
static s32 D_800A8338 = 0;
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
    s32* speed;
    volatile s32* ptr;
    SVECTOR** path;

    JetInitialize();
    SetDrawMode(&D_800D9934, 0, 1, GetTPage(1, 1, 768, 0) & 0xFFFF, NULL);
    g_JetTrackRot = g_JetXbinAdr.trackRotations;
    JetTrackPathLoad(0, 0);
    path = &g_JetTrackPath;
    g_JetTrackLeft = *path;
    JetTrackPathLoad(1, 0);
    g_JetTrackRight = *path;
    JetAudioInit();
    SetFogNearFar(g_JetFog.near, g_JetFog.far, 256);
    g_JetPopupNode[0] = JetNodeAlloc(30, 0, 0, 1, &g_JetRootNode, 1200, 50, 3000, 0, 1000, 0);
    for (;;) {
        speed = &g_JetSpeed;
        if ((g_JetTrackSegment * 4) > (g_JetTrackPathLength - 0x10) || g_JetExit == 1) {
            break;
        }
        JetInputUpdate();
        if (g_JetPaused == 0) {
            JetCameraUpdate();
            JetTrackListsAdvance(*speed);
            JetSetWorldMatrix();
            JetDrawTrack();
            JetDrawTriangleList();
            JetDrawScorePopup(g_JetBufferPtr[0], g_JetPopupModelId, 5, 40, 0);
            JetTrackListsClean(*speed);
            JetObjectsUpdate(g_JetBufferPtr[0]);
            JetDrawNumber(g_JetScore, 244, 200, 0, 0);
            JetDrawSprite(7, 204, 200, 39, 17, 0, 0, 0x27, 0x11, 0);
            JetDrawSprite(11, 18, 86, 12, 140, 0, 0x70, 0xC, 0x8C, 0);
            JetDrawEnergyGauge();
            if (*speed < 16384) {
                D_800A8338 = 0;
            } else {
                D_800A8338 = AKAO_VOL_MAX;
            }
        } else {
            JetDrawSprite(9, 202, 192, 96, 32, 0, 0x50, 0x60, 0x20, 0);
            D_800A8338 = 0;
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
        ptr = &D_800E25FC;
        *ptr = 0;
        if (current == next) {
            next++;
        }
        g_JetBufferPtr[0] = next;
        ClearOTagR(next->ot, 0x1000);
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
    MATRIX** world;
    MATRIX* m;
    MATRIX* wm;
    MATRIX* cam;
    s16 minX;
    s16 maxX;
    s16 minY;
    s16 maxY;
    s16 i;

    world = &g_JetWorldMatrix;
    m = world[0];
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
    wm = world[0];
    wm->t[0] -= g_JetCameraPos.vx;
    wm->t[1] -= g_JetCameraPos.vy;
    wm->t[2] -= g_JetCameraPos.vz;
    cam = &g_JetCameraRot;
    gte_SetRotMatrix(cam);
    gte_ldclmv(&world[0]->m[0][0]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][0]);
    gte_ldclmv(&world[0]->m[0][1]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][1]);
    gte_ldclmv(&world[0]->m[0][2]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][2]);
    gte_SetTransMatrix(cam);
    gte_ldlv0(&world[0]->t[0]);
    gte_rt();
    gte_stlvl(&world[0]->t[0]);
    gte_SetRotMatrix(world[0]);
    gte_SetTransMatrix(world[0]);
    args.tris = node->model->tris;
    args.prim = db->prims.g3Cursor;
    args.ot = &db->ot[otIndex];
    args.model = node->model;
    db->prims.g3Cursor = JetDrawModelTris(&args);
    JetProject6Points(object->unkDC, object->unk11C);
    ys[0] = object->unk11C[0] >> 16;
    minY = ys[0];
    maxY = minY;
    xs[0] = object->unk11C[0];
    minX = xs[0];
    maxX = minX;
    for (i = 1; i < LEN(object->unk11C); i++) {
        ys[i] = (object->unk11C[i] & 0xFFFF0000) >> 16;
        xs[i] = object->unk11C[i];
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
    MATRIX** world;
    MATRIX* m;
    MATRIX* wm;
    MATRIX* cam;
    JetModel** shadow;
    u_long xy1;
    u_long xy2;
    s32 index;

    world = &g_JetWorldMatrix;
    m = world[0];
    index = 0;
    m->m[0][0] = node->m.m[index][index];
    m->m[index][1] = node->m.m[index][1];
    m->m[index][2] = node->m.m[index][2];
    m->m[1][index] = node->m.m[1][index];
    m->m[1][1] = node->m.m[1][1];
    m->m[1][2] = node->m.m[1][2];
    m->m[2][index] = node->m.m[2][index];
    m->m[2][1] = node->m.m[2][1];
    m->m[2][2] = node->m.m[2][2];
    m->t[index] = node->m.t[index];
    m->t[1] = node->m.t[1];
    m->t[2] = node->m.t[2];
    if (node->parent != &g_JetRootNode) {
        CompMatrix(&node->parent->m, m, m);
    }
    wm = world[index];
    wm->t[index] -= g_JetCameraPos.vx;
    wm->t[1] -= g_JetCameraPos.vy;
    wm->t[2] -= g_JetCameraPos.vz;
    cam = &g_JetCameraRot;
    gte_SetRotMatrix(cam);
    gte_ldclmv(&world[0]->m[0][0]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][0]);
    gte_ldclmv(&world[0]->m[0][1]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][1]);
    gte_ldclmv(&world[0]->m[0][2]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][2]);
    gte_SetTransMatrix(cam);
    gte_ldlv0(&world[0]->t[0]);
    gte_rt();
    gte_stlvl(&world[0]->t[0]);
    gte_SetRotMatrix(world[0]);
    gte_SetTransMatrix(world[0]);
    args.tris = node->model->tris;
    args.prim = db->prims.g3Cursor;
    args.ot = &db->ot2[otIndex];
    args.model = node->model;
    db->prims.g3Cursor = JetDrawModelTris(&args);
    shadow = &g_JetModelTable[79];
    JetProject3Points(&shadow[index]->tris[0].v0, screen);
    g_JetBeam0OriginY = screen[1] >> 16;
    g_JetBeam0OriginX = screen[1];
    D_800A896C = screen[2] >> 16;
    D_800A8960 = screen[2];
    JetProject3Points(&shadow[index]->tris[1].v0, screen);
    xy1 = screen[1];
    xy2 = screen[2];
    g_JetBeam1OriginY = xy1 >> 16;
    g_JetBeam1OriginX = xy1;
    D_800A8980 = xy2 >> 16;
    D_800A8974 = xy2;
}

// Load a node's matrix into the GTE and draw its model's triangles.
static void JetDrawNodeUI(JetBuffer* db, JetNode* node, s16 otIndex, s32 arg3, s32 arg4) {
    JetModelDrawArgs args;
    MATRIX** world;
    MATRIX* m;

    world = &g_JetWorldMatrix;
    m = world[0];
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
    gte_SetRotMatrix(world[0]);
    gte_SetTransMatrix(world[0]);
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
    JetListLink* list;
    SVECTOR* left;
    SVECTOR* right;
    u16 trackId;
    POLY_FT4* prim;

    trackId = g_JetTrackListHead;
    prim = g_JetBufferPtr[0]->prims.ft4Cursor;
    list = g_JetTrackLinks;
loop:
    left = g_JetTrackLeft;
    right = g_JetTrackRight;
    prim = JetDrawTrackQuad(&left[trackId], prim, g_JetBufferPtr[0]->ot, &right[trackId]);
    trackId = list[trackId].next;
    if (trackId != JET_LIST_END) {
        goto loop;
    }
    g_JetBufferPtr[0]->prims.ft4Cursor = prim;
}

// Build the world matrix from the camera rotation and the view position.
static void JetSetWorldMatrix(void) {
    MATRIX** view;
    MATRIX** world;
    MATRIX* cam;

    view = &g_JetViewMatrix;
    view[0]->t[0] = -g_JetCameraPos.vx;
    view[0]->t[1] = -g_JetCameraPos.vy;
    view[0]->t[2] = -g_JetCameraPos.vz;
    cam = &g_JetCameraRot;
    gte_SetRotMatrix(cam);
    gte_ldclmv(&view[0]->m[0][0]);
    gte_rtir();
    world = &g_JetWorldMatrix;
    gte_stclmv(&world[0]->m[0][0]);
    gte_ldclmv(&view[0]->m[0][1]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][1]);
    gte_ldclmv(&view[0]->m[0][2]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][2]);
    gte_SetTransMatrix(cam);
    gte_ldlv0(&view[0]->t[0]);
    gte_rt();
    gte_stlvl(&world[0]->t[0]);
    gte_SetRotMatrix(world[0]);
    gte_SetTransMatrix(world[0]);
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
    JetNode* node;
    u8* alternate;
    s16* counter;
    s32 index;
    s32 unused;

    if (modelId == 0 || modelId == 0x5B) {
        return;
    }
    alternate = &g_JetScorePopupAlternate;
    index = 0;
    node = g_JetPopupNode[index];
    node->model = g_JetModelTable[modelId];
    g_JetPopupRot.vx += rotationX;
    g_JetPopupRot.vy += rotationY;
    g_JetPopupRot.vz += rotationZ;
    if (alternate[0] == 1) {
        RotMatrix(&g_JetPopupRot, &g_JetPopupNode[index]->m);
        JetDrawNodeUI(db, g_JetPopupNode[index], 0, 0, unused);
        JetDrawNumber(g_JetPopupPoints, 220, 160, 0, 0x18);
    }
    counter = &g_JetPopupTimer;
    (*counter)--;
    if (*counter < 50) {
        if (alternate[0] == 0) {
            alternate[0] = 1;
        } else {
            alternate[0] = 0;
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
    volatile s32* state;
    s32 i;

    state = &D_800A8A84;
    *state = 0;
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
    *state = 0x99;
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
    g_JetFog.near = 10410;
    g_JetFog.far = 14300;
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
    D_800A8338 = 0;
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

    SystemLoadFileBySector(g_JetAssetFiles[2].loc, g_JetAssetFiles[2].len, &g_JetXbinAdr.unk0, NULL);
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
    g_AkaoCmd.params[0] = g_JetXbinAdr.unk0;
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
    g_AkaoCmd.params[1] = SFX_177;
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
    u8* pChannel;
    s32 channel;

    pChannel = &g_JetSfxChannel;
    channel = (*pChannel + 1) & 1;
    *pChannel = channel;
    if (channel == 0) {
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT2;
        g_AkaoCmd.params[0] = 0;
        AkaoExec();
        g_AkaoCmd.opcode = AKAO_PLAY_SLOT2;
        g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
        g_AkaoCmd.params[1] = soundId;
        AkaoExec();
    }
    if (*pChannel == 1) {
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
    s32* lastPitch;
    s32 param;

    lastPitch = &g_JetLaserPitch;
    if (*lastPitch == 0) {
        if (power & 0xFF) {
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = SFX_22B;
            AkaoExec();
        } else {
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = SFX_NULL;
            AkaoExec();
            g_JetLaserPitch = 0;
            return;
        }
    }
    param = power & 0xFF;
    if (param) {
        g_JetLaserVolume = param;
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT3;
        g_AkaoCmd.params[0] = param;
        AkaoExec();
        *lastPitch = param;
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
    g_AkaoCmd.params[0] = D_800A8338;
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
    s32* pathPos;
    s32* speed;
    s32* limit;
    s32 step;

    pathPos = &g_JetCameraPathPos;
    JetTrackSample(pathPos[0], -0x64, &pos, &rot);
    speed = &g_JetSpeed;
    pathPos[0] += speed[0];
    g_JetCameraPosCopy.vx = pos.vx;
    g_JetCameraPosCopy.vy = pos.vy;
    g_JetCameraPosCopy.vz = pos.vz;
    if (rot.vx < 0) {
        rot.vx += 0x1000;
    }
    g_JetCameraRoll.vz = -rot.vz;
    step = rsin(rot.vx) / 15;
    if (step > 0) {
        if (speed[0] > 43000) {
            speed[0] -= step;
        }
    }
    if (step < 0) {
        limit = &g_JetSpeed;
        if (limit[0] <= 119999) {
            limit[0] -= step;
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
    s32* dir;
    s16* cursorX;
    s16* cursorY;
    u8* shoot;
    s16* power;
    s16* powerRegen;
    u8* repeat;
    u8* scroll;
    s32* speed;
    s32* brake;
    s32* held;
    u8* paused;
    u8 next;
    VECTOR* cam;
    s32 count;

    pad = InputReadPadsRaw(1);
    if (g_JetPaused == 0) {
        dir = &g_JetPadDir;
        *dir = 0;
        D_800A8A7C = 0;
        if (pad & PADLleft) {
            *dir = 4;
        }
        if (pad & PADLright) {
            *dir = 6;
        }
        if (pad & PADLup) {
            *dir = 8;
            if (pad & PADLleft) {
                *dir = 7;
            }
            if (pad & PADLright) {
                *dir = 9;
            }
        }
        if (pad & PADLdown) {
            dir = &g_JetPadDir;
            *dir = 2;
            if (pad & PADLleft) {
                *dir = 1;
            }
            if (pad & PADLright) {
                *dir = 3;
            }
        }
        if (g_JetAimMode == 1) {
            if (pad & PADLdown) {
                cursorY = &g_JetCursorY;
                *cursorY += 5;
            }
            if (pad & PADLup) {
                cursorY = &g_JetCursorY;
                *cursorY -= 5;
            }
            if (pad & PADLleft) {
                cursorX = &g_JetCursorX;
                *cursorX -= 5;
            }
            if (pad & PADLright) {
                cursorX = &g_JetCursorX;
                *cursorX += 5;
            }
            shoot = &g_JetFiring;
            *shoot = 0;
            if (pad & PADRright) {
                power = &g_JetShotPower;
                JetUpdateLaserSfx(*power & 0xFF);
                if (*power >= 9) {
                    (*power)--;
                }
                repeat = &g_JetShotRepeatCounter;
                count = *repeat;
                if (count == 0) {
                    scroll = &g_JetBeamScroll;
                    next = *scroll + 3;
                    *repeat = 1;
                    *shoot = 1;
                    *scroll = next % 15;
                } else {
                    *repeat = count - 1;
                }
            } else {
                JetUpdateLaserSfx(0);
                powerRegen = &g_JetShotPower;
                if (*powerRegen < 128) {
                    (*powerRegen)++;
                }
            }
            cursorX = &g_JetCursorX;
            if (*cursorX > 320) {
                *cursorX = 320;
            }
            if (*cursorX < 0) {
                *cursorX = 0;
            }
            cursorY = &g_JetCursorY;
            if (*cursorY > 240) {
                *cursorY = 240;
            }
            if (*cursorY < 0) {
                *cursorY = 0;
            }
        }
        if (g_JetAimMode == 0) {
            if (pad & PADLdown) {
                g_JetFog.far -= 10;
            }
            if (pad & PADLup) {
                g_JetFog.far += 10;
            }
            if (pad & PADLleft) {
                g_JetFog.near -= 10;
            }
            if (pad & PADLright) {
                g_JetFog.near += 10;
            }
            if (pad & PADRdown) {
                cam = &D_800A83D8;
                cam->vz -= 100;
            }
            if (pad & PADRup) {
                cam = &D_800A83D8;
                cam->vz += 100;
            }
            if (pad & PADRleft) {
                D_800A83D8.vx -= 100;
            }
            if (pad & PADRright) {
                D_800A83D8.vx += 100;
            }
            if (pad & PADR1) {
                cam = &D_800A83D8;
                cam->vy -= 100;
            }
            if (pad & PADR2) {
                cam = &D_800A83D8;
                cam->vy += 100;
            }
            if (pad & PADL1) {
                speed = &g_JetSpeed;
                *speed += 1024;
            }
            if (pad & PADL2) {
                brake = &g_JetSpeed;
                if (*brake >= 1024) {
                    *brake -= 1024;
                }
            }
            if (pad & PADstart) {
                g_JetSpeed = 0;
            }
        }
    }
    if (pad & PADstart) {
        held = &g_JetStartHeldFrames;
        *held = *held + 1;
    } else {
        g_JetStartHeldFrames = 0;
    }
    if (g_JetStartHeldFrames == 1) {
        paused = &g_JetPaused;
        if (*paused == 1) {
            *paused = 0;
        } else {
            *paused = 1;
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
    u32* pos;
    s32* segment;
    u32 prev;
    u32 next;
    u32 steps;
    u32 i;
    u32 id;
    u8* first;

    pos = &g_JetTrackListsPos;
    g_JetTrackListsPrevPos = pos[0];
    pos[0] = g_JetTrackListsPrevPos + advance;
    prev = g_JetTrackListsPrevPos >> 18;
    next = pos[0] >> 18;
    steps = next - prev;
    segment = &g_JetTrackSegment;
    segment[0] = segment[0] + steps;
    for (i = 0; i < steps + g_JetInitialTrackSegmentPending; i++) {
        u16** add;
        u16** remove;
        u32 end;

        add = &g_JetTriangleAddCursor;
        end = JET_LIST_END;
        remove = &g_JetTriangleRemoveCursor;
        while (1) {
            id = *add[0]++;
            if (id == end) {
                break;
            }
            JetTriangleListAppend(id);
        }
        while (1) {
            id = *remove[0]++;
            if (id == end) {
                break;
            }
            JetTriangleListRemove(id);
        }
    }
    for (i = 0; i < steps; i++) {
        u16** add;
        u16** remove;
        u32 end;

        add = &g_JetTrackAddCursor;
        end = JET_LIST_END;
        remove = &g_JetTrackRemoveCursor;
        while (1) {
            id = *add[0]++;
            if (id == end) {
                break;
            }
            JetTrackListAppend(id);
        }
        while (1) {
            id = *remove[0]++;
            if (id == end) {
                break;
            }
            JetTrackListRemove(id);
        }
    }
    first = &g_JetInitialTrackSegmentPending;
    if (*first == 1) {
        *first = 0;
    }
}

static void JetTrackListsAdvance(s32 speed) {
    u32* pos;
    s32* segment;
    u32 prev;
    u32 next;
    u32 i;
    u16** tri;
    u16** track;
    u32 id;
    u8* first;

    pos = &g_JetTrackListsPos;
    g_JetTrackListsPrevPos = pos[0];
    pos[0] = g_JetTrackListsPrevPos + speed;
    prev = g_JetTrackListsPrevPos >> 18;
    next = pos[0] >> 18;
    g_JetTrackSegmentsCrossed = next - prev;
    segment = &g_JetTrackSegment;
    segment[0] = segment[0] + g_JetTrackSegmentsCrossed;
    for (i = 0; i < g_JetTrackSegmentsCrossed + g_JetInitialTrackSegmentPending; i++) {
        tri = &g_JetTriangleAddCursor;
        while (1) {
            id = *tri[0]++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTriangleListAppend(id);
        }
    }
    for (i = 0; i < g_JetTrackSegmentsCrossed; i++) {
        track = &g_JetTrackAddCursor;
        while (1) {
            id = *track[0]++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTrackListAppend(id);
        }
    }
}

static void JetTrackListsClean(s32 unusedArg) {
    u32 i;
    u16** tri;
    u16** track;
    u32 id;
    u8* first;

    for (i = 0; i < g_JetTrackSegmentsCrossed + g_JetInitialTrackSegmentPending; i++) {
        tri = &g_JetTriangleRemoveCursor;
        while (1) {
            id = *tri[0]++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTriangleListRemove(id);
        }
    }
    for (i = 0; i < g_JetTrackSegmentsCrossed; i++) {
        track = &g_JetTrackRemoveCursor;
        while (1) {
            id = *track[0]++;
            if (id == JET_LIST_END) {
                break;
            }
            JetTrackListRemove(id);
        }
    }
    first = &g_JetInitialTrackSegmentPending;
    if (*first == 1) {
        *first = 0;
    }
}

static void JetTriangleListAppend(u16 triangleId) {
    JetListLink* node;
    u16* pCount;
    u16* pTail;
    u16 newCount;
    u16 count;
    u16 tail;

    node = &g_JetTriangleLinks[triangleId];
    pCount = &g_JetTriangleListCount;
    count = *pCount;
    if (count == 0) {
        g_JetTriangleListHead = triangleId;
        g_JetTriangleListTail = triangleId;
        *pCount = 1;
    } else {
        pTail = &g_JetTriangleListTail;
        tail = *pTail;
        newCount = count + 1;
        node->prev = tail;
        (g_JetTriangleLinks + tail)->next = triangleId;
        *pTail = triangleId;
        *pCount = newCount;
    }
}

static void JetTriangleListRemove(u16 triangleId) {
    JetListLink* list;
    JetListLink* node;
    u16* pCount;
    u16 prev;
    u16 next;

    node = &g_JetTriangleLinks[triangleId];
    prev = node->prev;
    next = node->next;
    list = g_JetTriangleLinks;
    if (prev != JET_LIST_END) {
        list[prev].next = next;
    } else {
        g_JetTriangleListHead = next;
    }
    if (next != JET_LIST_END) {
        g_JetTriangleLinks[next].prev = prev;
    } else {
        g_JetTriangleListTail = prev;
    }
    {
        JetListLink* links;

        links = g_JetTriangleLinks;
        links[triangleId].prev = JET_LIST_END;
        links[triangleId].next = JET_LIST_END;
    }
    pCount = &g_JetTriangleListCount;
    *pCount = *pCount - 1;
}

static void JetTrackListAppend(u16 trackId) {
    u16* pCount;
    u16 count;

    pCount = &g_JetTrackListCount;
    count = *pCount;
    if (count == 0) {
        JetListLink* list;
        JetListLink* node;

        list = g_JetTrackLinks;
        node = &list[trackId];
        node->prev = JET_LIST_END;
        node->next = JET_LIST_END;
        g_JetTrackListHead = trackId;
        g_JetTrackListTail = trackId;
        *pCount = 1;
    } else {
        JetListLink* list;
        JetListLink* node;
        u16* pTail;
        u16 newCount;
        u16 tail;

        newCount = count + 1;
        list = g_JetTrackLinks;
        node = &list[trackId];
        pTail = &g_JetTrackListTail;
        tail = *pTail;
        node->prev = tail;
        node->next = JET_LIST_END;
        list[tail].next = trackId;
        *pTail = trackId;
        *pCount = newCount;
    }
}

static void JetTrackListRemove(u16 trackId) {
    JetListLink* list;
    JetListLink* node;
    u16* pCount;
    u16 prev;
    u16 next;

    node = &g_JetTrackLinks[trackId];
    prev = node->prev;
    next = node->next;
    list = g_JetTrackLinks;
    if (prev != JET_LIST_END) {
        list[prev].next = next;
    } else {
        g_JetTrackListHead = next;
    }
    if (next != JET_LIST_END) {
        g_JetTrackLinks[next].prev = prev;
    } else {
        g_JetTrackListTail = prev;
    }
    pCount = &g_JetTrackListCount;
    *pCount = *pCount - 1;
}
