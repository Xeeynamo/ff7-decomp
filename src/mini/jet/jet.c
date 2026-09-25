//! PSYQ=3.3 FORCE_MEM=true

#include "jet_private.h"
#include <libc.h>

// Argument block for the GTE renderers in jet_gte.s.
typedef struct {
    /* 0x0 */ JetTriangle* tris;
    /* 0x4 */ POLY_G3* prim;
    /* 0x8 */ OT_TYPE* ot;
    /* 0xC */ JetModel* model;
} JetModelDrawArgs; // size: 0x10

#define JET_LIST_END 0xFFFF

typedef struct {
    /* 0x0 */ u16 prev;
    /* 0x2 */ u16 next;
} JetListLink; // size: 0x4

extern struct {
    s32 near;
    s32 far;
} g_JetFog;
extern u16 g_JetTrackListHead;
extern u16 g_JetTriangleListHead;
extern JetNode* g_JetPopupNode[1];
extern MATRIX* g_JetViewMatrix;
extern MATRIX* g_JetWorldMatrix;
extern SVECTOR* g_JetTrackLeft;
extern u16 g_JetTriangleListCount;
extern JetListLink g_JetTrackLinks[9000];
extern JetListLink g_JetTriangleLinks[12000];
extern SVECTOR* g_JetTrackRight;
extern MATRIX g_JetCameraRot;
extern SVECTOR* g_JetTrackRot;
void* JetDrawModelTris(JetModelDrawArgs* args);
void JetProject6Points(SVECTOR* points, u_long* screen);
void* JetDrawModelTrisUI(JetModelDrawArgs* args);
POLY_G3* JetDrawTriangle(JetTriangle* arg0, POLY_G3* arg1, OT_TYPE* arg2, JetTriangle* arg3);
POLY_FT4* JetDrawTrackQuad(SVECTOR* arg0, POLY_FT4* arg1, OT_TYPE* arg2, SVECTOR* arg3);

static void JetDrawEnergyGauge();
void JetDrawNumber(s32 value, s32 x, s32 y, s16 zeroPad, u16 textureV);
static void JetDrawScorePopup(JetBuffer* buffer, s16 modelId, s32 rotationX, s32 rotationY, s32 rotationZ);
static void JetDrawSprite(
    s16 spriteId, s16 x, s16 y, s16 w, s16 h, u8 u, u8 v, u8 textureWidth, u8 textureHeight, u8 semiTrans);
static void JetDrawTrack(void);
static void JetDrawTriangleList(void);
static void JetSetWorldMatrix();
static void JetQueueTPageResets();

static const RECT D_800A0000 = {0, 0, 320, 200};

typedef struct {
    s32 sector;
    u32 size;
} JetDiscFile;

// MINI/ files read by JetLoadAssets.
static JetDiscFile g_JetAssetFiles[4] = {
    {0x9D8, 0x28},    // TEXADR.BIN
    {0x9D9, 0x4DE8},  // TEX.BIN
    {0x9E3, 0x44},    // XBINADR.BIN
    {0x9E4, 0xA7958}, // XBIN2.BIN
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

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", MINI_Jet);

// Draw one object's model, project its bounding box and flag a cursor hit.
void JetDrawObjectAndCheckHit(JetBuffer* drawBuffer, JetNode* node, s16 otIndex, s32 unusedArg, JetObject* object) {
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
    args.prim = drawBuffer->prims.g3Cursor;
    args.ot = &drawBuffer->ot[otIndex];
    args.model = node->model;
    drawBuffer->prims.g3Cursor = JetDrawModelTris(&args);
    JetProject6Points(object->unkDC, object->unk11C);
    // Both reads have to stay: -fforce-mem is what keeps the second one a
    // word load instead of letting gcc narrow it to lhu.
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
        object->unk28.hit = 0;
        if (g_JetCursorX < maxX && minX < g_JetCursorX && g_JetCursorY < maxY && minY < g_JetCursorY &&
            g_JetFiring == 1) {
            object->unk28.hit = g_JetFiring;
        }
    }
}

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetDrawCartAndProjectBeams);

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

// Sample the track at a fractional segment index, giving a point lifted along
// the surface normal and the interpolated banking rotation.
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
static void JetDrawScorePopup(JetBuffer* buffer, s16 modelId, s32 rotationX, s32 rotationY, s32 rotationZ) {
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
        JetDrawNodeUI(buffer, g_JetPopupNode[index], 0, 0, unused);
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

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetDrawNumber);

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
INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetInitialize);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetLoadAssets);

// Upload the nine loaded TIMs and build the sprite tpage/clut tables.
INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetSpriteTablesInit);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetLoadTim);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetAudioInit);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetAudioFadeOut);

// Alternate the two laser channels on each shot.
INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetPlaySfx);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetSetLaserVolume);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetAudioUpdateVolumes);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTrackInit);

// Advance the camera along its path and rebuild the view matrices.
INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetCameraUpdate);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTrackPathLoad);

static void func_800A2E30(void) {}

// Read the pad and drive the cursor, the camera tweaks and the pause toggle.
INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetInputUpdate);

// Reset both draw lists and the object streams for a new run.
INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetDrawListsInit);

// Unused: JetTrackListsAdvance and JetTrackListsClean in one pass.
INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTrackListsAdvanceAndClean);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTrackListsAdvance);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTrackListsClean);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTriangleListAppend);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTriangleListRemove);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTrackListAppend);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", JetTrackListRemove);
