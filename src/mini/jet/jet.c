//! PSYQ=3.3 FORCE_MEM=true

#include "types.h"
#include <game.h>
#include <inline_o.h>

typedef struct {
    /* 0x0000 */ POLY_F3* f3Cursor;
    /* 0x0004 */ POLY_F4* f4Cursor;
    /* 0x0008 */ POLY_G3* g3Cursor;
    /* 0x000C */ POLY_G4* g4Cursor;
    /* 0x0010 */ POLY_FT3* ft3Cursor;
    /* 0x0014 */ POLY_FT4* ft4Cursor;
    /* 0x0018 */ POLY_GT3* gt3Cursor;
    /* 0x001C */ POLY_GT4* gt4Cursor;
    /* 0x0020 */ LINE_F2* lineCursor;
    /* 0x0024 */ POLY_F3 f3[1];
    /* 0x0038 */ POLY_F4 f4[1];
    /* 0x0050 */ POLY_G3 g3[0x640];
    /* 0xAF50 */ POLY_G4 g4[0x1E];
    /* 0xB388 */ POLY_FT3 ft3[1];
    /* 0xB3A8 */ POLY_FT4 ft4[0x12C];
    /* 0xE288 */ POLY_GT3 gt3[1];
    /* 0xE2B0 */ POLY_GT4 gt4[1];
    /* 0xE2E4 */ LINE_F2 line[1];
} JetPrimBuffer; // size: 0xE2F4

typedef struct {
    /* 0x0000 */ DRAWENV draw;
    /* 0x005C */ DISPENV disp;
    /* 0x0070 */ OT_TYPE ot[0x1000];
    /* 0x4070 */ u_long unk4070[10];
    /* 0x4098 */ OT_TYPE ot2[0xB4];
    /* 0x4368 */ JetPrimBuffer prims;
} JetBuffer; // size: 0x1265C

typedef struct {
    /* 0x00 */ s16 triCount;
    /* 0x02 */ s16 quadCount;
    /* 0x04 */ SVECTOR unk4;
    /* 0x0C */ SVECTOR unkC;
} JetModelInfo; // size: 0x14

typedef struct {
    /* 0x00 */ SVECTOR v0;
    /* 0x08 */ SVECTOR v1;
    /* 0x10 */ SVECTOR v2;
    /* 0x18 */ CVECTOR c0;
    /* 0x1C */ CVECTOR c1;
    /* 0x20 */ CVECTOR c2;
} JetTriangle; // size: 0x24

typedef struct {
    /* 0x00 */ s32 unk0[10];
} JetQuad; // size: 0x28

typedef struct {
    /* 0x00 */ s16 polyCount;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ s16 triCount;
    /* 0x06 */ s16 quadCount;
    /* 0x08 */ s16 unk8;
    /* 0x0A */ s16 : 16;
    /* 0x0C */ JetTriangle* tris;
    /* 0x10 */ JetQuad* quads;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ s32 : 32;
} JetModel; // size: 0x20

typedef struct JetNode {
    /* 0x00 */ JetModel* model;
    /* 0x04 */ MATRIX m;
    /* 0x24 */ struct JetNode* parent;
    /* 0x28 */ s16 modelId;
    /* 0x2A */ s16 index;
    /* 0x2C */ u16 depth;
    /* 0x2E */ s16 unk2E;
    /* 0x30 */ struct JetNode* prev;
    /* 0x34 */ struct JetNode* next;
} JetNode; // size: 0x38

// The behaviour state an object's type handler drives.
typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 hit;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ char pad20[8];
    /* 0x28 */ s32 unk28;
    /* 0x2C */ s32 unk2C;
    /* 0x30 */ s32 unk30;
    /* 0x34 */ s32 unk34;
    /* 0x38 */ char pad38[0x18];
    /* 0x50 */ s32 unk50[0x14];
} Unk800D1CAC; // size: 0xA0

typedef struct {
    /* 0x00 */ VECTOR unk0;
    /* 0x10 */ char pad10[8];
    /* 0x18 */ SVECTOR unk18; // spawn rotation
    /* 0x20 */ char pad20[8];
    /* 0x28 */ Unk800D1CAC unk28;
    /* 0xC8 */ s32 unkC8;      // the object path's length
    /* 0xCC */ SVECTOR* unkCC; // the object path itself
    /* 0xD0 */ s32 : 32;
    /* 0xD4 */ JetNode* unkD4;
    /* 0xD8 */ s16 unkD8;
    /* 0xDA */ s16 unkDA;
    /* 0xDC */ SVECTOR unkDC[6]; // the model bounding box's six face centres
    /* 0x10C */ char pad10C[0x10];
    /* 0x11C */ u_long unk11C[6]; // the same six points projected to the screen
    /* 0x134 */ char pad134[8];
} Unk800A4390; // size: 0x13C

// Argument block for the GTE renderers in jet_gte.s.
typedef struct {
    /* 0x0 */ JetTriangle* tris;
    /* 0x4 */ POLY_G3* prim;
    /* 0x8 */ OT_TYPE* ot;
    /* 0xC */ JetModel* model;
} Unk800A8604; // size: 0x10

// A doubly-linked draw-list entry, parallel to the array it orders. Both
// links are indices into that array, with 0xFFFF for the ends.
typedef struct {
    /* 0x0 */ u16 prev;
    /* 0x2 */ u16 next;
} JetListLink; // size: 0x4

extern s32 g_JetLeftPlaneNormalX;
extern s32 g_JetLeftPlaneNormalY;
extern s32 g_JetLeftPlaneNormalZ;
extern s32 g_JetRightPlaneNormalX;
extern s32 g_JetRightPlaneNormalY;
extern s32 g_JetRightPlaneNormalZ;
extern s32 D_800A8950;
extern s32 D_800A8968;
extern s32 g_JetLeftPlaneDistance;
extern s32 g_JetRightPlaneDistance;
extern s32 g_JetLeftNormalLength;
extern s32 g_JetRightNormalLength;

extern JetModelInfo* g_JetModelInfo;
extern JetModelInfo* g_JetModelInfoBase;
extern JetTriangle* g_JetTriangles;
extern JetTriangle* g_JetTrianglesBase;
extern JetQuad* g_JetQuads;
extern JetQuad* g_JetQuadsBase;
extern s32 g_JetTriangleCursor;
extern s32 g_JetQuadCursor;
extern u32 g_JetModelCount;
extern JetModel g_JetModelPool[];
extern JetModel* g_JetModelTable[];

extern JetBuffer g_JetBuffers[2];
extern JetBuffer* g_JetBufferPtr[1];

extern JetNode g_JetNodePool[0xC8];
extern s16 g_JetNodeFreeList[0xC8];
extern s16 g_JetNextFreeNode;
extern JetNode g_JetRootNode;
extern JetNode g_JetNodeListHeads[10];
extern JetNode g_JetNodeListTails[10];

extern u16 g_JetSpriteTPage[];
extern s16 g_JetPopupTimer;
extern u16 g_JetTrackListHead;
extern u16 g_JetTriangleListHead;
extern JetNode* D_800A8A74[1];
extern s16 g_JetPopupModelId;
extern MATRIX* g_JetViewMatrix;
extern MATRIX* g_JetWorldMatrix;
extern u8 g_JetFiring;
extern SVECTOR* g_JetTrackLeft;
extern u16 g_JetTriangleListCount;
extern JetListLink g_JetTrackLinks[];
extern u8 D_800E25E8;
extern s16 g_JetCursorX;
extern s16 g_JetCursorY;
extern JetListLink g_JetTriangleLinks;
extern SVECTOR g_JetPopupRot;
extern SVECTOR* g_JetTrackRight;
extern u16 g_JetSpriteClut[];
extern MATRIX g_JetCameraRot;
extern SVECTOR* g_JetTrackRot;
extern s16 g_JetPopupPoints;
extern u16 g_JetShotPower;

s32 D_800A8310 = 0x9D8;
u32 D_800A8314 = 0x28;
s32 D_800A8318 = 0x9D9;
u32 D_800A831C = 0x4DE8;
s32 D_800A8320 = 0x9E3;
u32 D_800A8324 = 0x44;
s32 D_800A8328 = 0x9E4;
u32 D_800A832C = 0xA7958;
s32 D_800A8330 = 0x7F;
s32 D_800A8334 = 0x7F;
s32 D_800A8338 = 0;
s32 D_800A833C = 0;
MATRIX D_800A8340 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
MATRIX D_800A8360 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
MATRIX D_800A8380 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
SVECTOR D_800A83A0 = {0, 0, 0, 0}; // world rotation
VECTOR g_JetCameraPos = {0, 0, 0, 0};
VECTOR D_800A83B8 = {0, 0, 0, 0};
VECTOR D_800A83C8 = {0, 0, 0, 0};
VECTOR D_800A83D8 = {0, 0, 0, 0};
s32 D_800A83E8[2] = {0, 0};

JetModel* JetModelAlloc(void);
JetTriangle* JetTrianglesAlloc(s32 count);
JetQuad* JetQuadsAlloc(s32 count);
void JetPrimsInit(JetPrimBuffer* prims);
void JetPrimCursorsReset(JetPrimBuffer* prims);
void JetNodeInit(JetNode* node, s16 index);
s16 JetNodeIndexAlloc(void);
void JetNodeIndexFree(s16 index);
void JetNodeLink(JetNode* node, JetNode* parent);
void JetNodeUnlink(JetNode* node);
s32 JetVectorInsidePlanes(VECTOR* arg0);
void JetProject6Points(SVECTOR* arg0, u_long* arg1);
void* JetDrawModelTris(Unk800A8604* arg0);
void* JetDrawModelTrisUI(Unk800A8604* arg0);
POLY_G3* JetDrawTriangle(JetTriangle* arg0, POLY_G3* arg1, OT_TYPE* arg2, JetTriangle* arg3);
POLY_FT4* JetDrawTrackQuad(SVECTOR* arg0, POLY_FT4* arg1, OT_TYPE* arg2, SVECTOR* arg3);
void func_800A1CD8(s32 value, s32 x, s16 y, s16 padWithZero, u16 v);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", MINI_Jet);

// Draw one object's model, project its bounding box and flag a cursor hit.
void func_800A0874(JetBuffer* db, JetNode* node, s16 otIndex, s32 arg3, Unk800A4390* obj) {
    Unk800A8604 args;
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
    JetProject6Points(obj->unkDC, obj->unk11C);
    // Both reads have to stay: -fforce-mem is what keeps the second one a
    // word load instead of letting gcc narrow it to lhu.
    ys[0] = obj->unk11C[0] >> 16;
    minY = ys[0];
    maxY = minY;
    xs[0] = obj->unk11C[0];
    minX = xs[0];
    maxX = minX;
    for (i = 1; i < 6; i++) {
        ys[i] = (obj->unk11C[i] & 0xFFFF0000) >> 16;
        xs[i] = obj->unk11C[i];
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
        obj->unk28.hit = 0;
        if (g_JetCursorX < maxX && minX < g_JetCursorX && g_JetCursorY < maxY && minY < g_JetCursorY &&
            g_JetFiring == 1) {
            obj->unk28.hit = g_JetFiring;
        }
    }
}

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A0D78);

void JetDrawNodeUI(JetBuffer* db, JetNode* node, s16 otIndex, s32 arg3, s32 arg4) {
    Unk800A8604 args;
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

void JetDrawTriangleList(void) {
    JetListLink* list;
    JetTriangle* tris;
    u16 triId;
    POLY_G3* prim;

    prim = g_JetBufferPtr[0]->prims.g3Cursor;
    tris = g_JetTrianglesBase;
    if (g_JetTriangleListCount) {
        triId = g_JetTriangleListHead;
        list = &g_JetTriangleLinks;
        do {
            prim = JetDrawTriangle(&tris[triId], prim, g_JetBufferPtr[0]->ot, &tris[triId]);
            triId = list[triId].next;
        } while (triId != 0xFFFF);
    }
    g_JetBufferPtr[0]->prims.g3Cursor = prim;
}

void JetDrawTrack(void) {
    JetListLink* list;
    SVECTOR* left;
    SVECTOR* right;
    u16 trackId;
    POLY_FT4* prim;

    trackId = g_JetTrackListHead;
    prim = g_JetBufferPtr[0]->prims.ft4Cursor;
    list = g_JetTrackLinks;
// A loop keyword hoists the 0xFFFF test into a register; the goto keeps it inline.
loop:
    left = g_JetTrackLeft;
    right = g_JetTrackRight;
    prim = JetDrawTrackQuad(&left[trackId], prim, g_JetBufferPtr[0]->ot, &right[trackId]);
    trackId = list[trackId].next;
    if (trackId != 0xFFFF) {
        goto loop;
    }
    g_JetBufferPtr[0]->prims.ft4Cursor = prim;
}

void JetSetWorldMatrix(void) {
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
void JetTrackSample(u32 at, s32 lift, VECTOR* pos, SVECTOR* rot) {
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

    seg = at >> 16;
    frac = at & 0xFFFF;
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

    pos->vx = (s16)mid.vx + ((unit.vx * lift) >> 12);
    pos->vy = (s16)mid.vy + ((unit.vy * lift) >> 12);
    pos->vz = (s16)mid.vz + ((unit.vz * lift) >> 12);

    rot->vx = rotCur->vx + dx;
    rot->vy = dy - rotCur->vy;
    rot->vz = rotCur->vz + dz;
}

void JetDrawEnergyGauge(void) {
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
void JetDrawScorePopup(JetBuffer* arg0, s16 arg1, s32 arg2, s32 arg3, s32 arg4) {
    JetNode* node;
    u8* alternate;
    s16* counter;
    s32 index;
    s32 unused;

    if (arg1 == 0 || arg1 == 0x5B) {
        return;
    }
    alternate = &D_800E25E8;
    index = 0;
    node = D_800A8A74[index];
    node->model = g_JetModelTable[arg1];
    g_JetPopupRot.vx += arg2;
    g_JetPopupRot.vy += arg3;
    g_JetPopupRot.vz += arg4;
    if (alternate[0] == 1) {
        RotMatrix(&g_JetPopupRot, &D_800A8A74[index]->m);
        JetDrawNodeUI(arg0, D_800A8A74[index], 0, 0, unused);
        func_800A1CD8(g_JetPopupPoints, 0xDC, 0xA0, 0, 0x18);
    }
    counter = &g_JetPopupTimer;
    (*counter)--;
    if (*counter < 0x32) {
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

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1CD8);

void JetDrawSprite(s16 spriteId, s16 x, s16 y, s16 w, s16 h, u8 u, u8 v, u8 uw, u8 vh, u8 semiTrans) {
    JetBuffer** db;
    POLY_FT4* poly;

    db = g_JetBufferPtr;
    poly = db[0]->prims.ft4Cursor;
    setXYWH(poly, x, y, w, h);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUVWH(poly, u, v, uw, vh);
    poly->tpage = g_JetSpriteTPage[spriteId];
    poly->clut = g_JetSpriteClut[spriteId];
    SetSemiTrans(poly, semiTrans);
    addPrim(&db[0]->ot2[1], poly);
    poly++;
    db[0]->prims.ft4Cursor = poly;
}

// Queue two blank textured quads, one at each end of the background OT.
void func_800A2058(void) {
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
    addPrim(&db[0]->ot[0xFFF], poly);
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

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2214);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2420);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2518);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A27F0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2860);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2938);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A29AC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2AA0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2B78);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2BE0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2C50);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2DE4);

void func_800A2E30(void) {}

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2E38);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A334C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3414);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A35DC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A372C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A385C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A38D4);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3980);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3A20);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3AAC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3B58);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3C04);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3D50);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3E58);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A40F4);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A4390);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A4400);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A442C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A4458);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A45C0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A4650);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A46E8);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A6B08);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A6BD8);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A70D4);

s32 JetVectorInsidePlanes(VECTOR* point) {
    s32 hsLeft;
    s32 rightOk;
    s32 leftOk;
    s32 hsRight;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;

    leftOk = 0;
    rightOk = 0;
    lx = g_JetLeftPlaneNormalX;
    ly = g_JetLeftPlaneNormalY;
    lz = g_JetLeftPlaneNormalZ;
    hsLeft = (lx * (point->vx >> 2)) + (ly * (point->vy >> 2)) + (lz * (point->vz >> 2)) + g_JetLeftPlaneDistance;
    rx = g_JetRightPlaneNormalX;
    ry = g_JetRightPlaneNormalY;
    rz = g_JetRightPlaneNormalZ;
    hsRight = (rx * (point->vx >> 2)) + (ry * (point->vy >> 2)) + (rz * (point->vz >> 2)) + g_JetRightPlaneDistance;
    if (hsLeft > 0 && D_800A8950 > 0) {
        leftOk = 1;
    }
    if (hsLeft < 0 && D_800A8950 < 0) {
        leftOk = 1;
    }
    if (hsRight > 0 && D_800A8968 > 0) {
        rightOk = 1;
    }
    if (hsRight < 0 && D_800A8968 < 0) {
        rightOk = 1;
    }
    return leftOk & rightOk;
}

s32 JetSVectorInsidePlanes(SVECTOR* point) {
    s32 hsLeft;
    s32 rightOk;
    s32 leftOk;
    s32 hsRight;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;

    leftOk = 0;
    rightOk = 0;
    lx = g_JetLeftPlaneNormalX;
    ly = g_JetLeftPlaneNormalY;
    lz = g_JetLeftPlaneNormalZ;
    hsLeft = (lx * (point->vx >> 2)) + (ly * (point->vy >> 2)) + (lz * (point->vz >> 2)) + g_JetLeftPlaneDistance;
    rx = g_JetRightPlaneNormalX;
    ry = g_JetRightPlaneNormalY;
    rz = g_JetRightPlaneNormalZ;
    hsRight = (rx * (point->vx >> 2)) + (ry * (point->vy >> 2)) + (rz * (point->vz >> 2)) + g_JetRightPlaneDistance;
    if (hsLeft > 0 && D_800A8950 > 0) {
        leftOk = 1;
    }
    if (hsLeft < 0 && D_800A8950 < 0) {
        leftOk = 1;
    }
    if (hsRight > 0 && D_800A8968 > 0) {
        rightOk = 1;
    }
    if (hsRight < 0 && D_800A8968 < 0) {
        rightOk = 1;
    }
    return leftOk & rightOk;
}

s32 JetLeftPlaneHalfSpace(s32 x, s32 y, s32 z) {
    s32 nx;
    s32 ny;
    s32 nz;

    nx = g_JetLeftPlaneNormalX;
    ny = g_JetLeftPlaneNormalY;
    nz = g_JetLeftPlaneNormalZ;

    return (nx * (x >> 2)) + (ny * (y >> 2)) + (nz * (z >> 2)) + g_JetLeftPlaneDistance;
}

s32 JetRightPlaneHalfSpace(s32 x, s32 y, s32 z) {
    s32 nx;
    s32 ny;
    s32 nz;

    nx = g_JetRightPlaneNormalX;
    ny = g_JetRightPlaneNormalY;
    nz = g_JetRightPlaneNormalZ;

    return (nx * (x >> 2)) + (ny * (y >> 2)) + (nz * (z >> 2)) + g_JetRightPlaneDistance;
}

s32 JetSphereInsidePlanes(VECTOR* center, s16 radius) {
    s32 leftOk;
    s32 hsLeft;
    s32 rightOk;
    s32 hsRight;
    s32 planeDistance;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;
    s32 len;

    leftOk = 0;
    rightOk = 0;
    lx = g_JetLeftPlaneNormalX;
    ly = g_JetLeftPlaneNormalY;
    lz = g_JetLeftPlaneNormalZ;
    hsLeft = (lx * (center->vx >> 2)) + (ly * (center->vy >> 2)) + (lz * (center->vz >> 2)) + g_JetLeftPlaneDistance;
    if (D_800A8950 > 0 && hsLeft >= 0) {
        leftOk = 1;
    }
    if (D_800A8950 < 0 && hsLeft <= 0) {
        leftOk = 1;
    }
    if (leftOk == 0) {
        len = g_JetLeftNormalLength;
        planeDistance = ((hsLeft < 0) ? -hsLeft : hsLeft) / len;
        if (planeDistance < radius) {
            leftOk = 1;
        }
    }
    rx = g_JetRightPlaneNormalX;
    ry = g_JetRightPlaneNormalY;
    rz = g_JetRightPlaneNormalZ;
    hsRight = (rx * (center->vx >> 2)) + (ry * (center->vy >> 2)) + (rz * (center->vz >> 2)) + g_JetRightPlaneDistance;
    if (D_800A8968 > 0 && hsRight >= 0) {
        rightOk = 1;
    }
    if (D_800A8968 < 0 && hsRight <= 0) {
        rightOk = 1;
    }
    if (rightOk == 0) {
        len = g_JetRightNormalLength;
        planeDistance = ((hsRight < 0) ? -hsRight : hsRight) / len;
        if (planeDistance < radius) {
            rightOk = 1;
        }
    }
    return leftOk & rightOk;
}

s32 JetSphereInsideLeftPlane(s32 x, s32 y, s32 z, s16 radius) {
    s32 nx;
    s32 ny;
    s32 nz;
    s32 hs;
    s32 ok;
    s32 len;

    nx = g_JetLeftPlaneNormalX;
    ny = g_JetLeftPlaneNormalY;
    nz = g_JetLeftPlaneNormalZ;
    ok = 0;
    hs = (nx * (x >> 2)) + (ny * (y >> 2)) + (nz * (z >> 2)) + g_JetLeftPlaneDistance;
    if (D_800A8950 > 0 && hs >= 0) {
        ok = 1;
    }
    if (D_800A8950 < 0 && hs <= 0) {
        ok = 1;
    }
    if (ok == 0) {
        len = g_JetLeftNormalLength;
        if (hs < 0) {
            hs = -hs;
        }
        if (hs / len < radius) {
            ok = 1;
        }
    }
    return ok;
}

s32 JetSphereInsideRightPlane(s32 x, s32 y, s32 z, s16 radius) {
    s32 nx;
    s32 ny;
    s32 nz;
    s32 hs;
    s32 ok;
    s32 len;

    nx = g_JetRightPlaneNormalX;
    ny = g_JetRightPlaneNormalY;
    nz = g_JetRightPlaneNormalZ;
    ok = 0;
    hs = (nx * (x >> 2)) + (ny * (y >> 2)) + (nz * (z >> 2)) + g_JetRightPlaneDistance;
    if (D_800A8968 > 0 && hs >= 0) {
        ok = 1;
    }
    if (D_800A8968 < 0 && hs <= 0) {
        ok = 1;
    }
    if (ok == 0) {
        len = g_JetRightNormalLength;
        if (hs < 0) {
            hs = -hs;
        }
        if (hs / len < radius) {
            ok = 1;
        }
    }
    return ok;
}

void JetModelsReset(void) {
    g_JetTriangleCursor = 0;
    g_JetQuadCursor = 0;
    g_JetModelCount = 0;
    g_JetTriangles = g_JetTrianglesBase;
    g_JetQuads = g_JetQuadsBase;
    g_JetModelInfo = g_JetModelInfoBase;
}

JetModel* JetModelBuild(s32 infoIndex) {
    JetModel* model;
    s32 numTri;
    s32 numQua;

    model = JetModelAlloc();
    numTri = g_JetModelInfo[infoIndex].triCount;
    numQua = g_JetModelInfo[infoIndex].quadCount;
    model->unk16 = g_JetModelInfo[infoIndex].unk4.vx;
    model->unk14 = g_JetModelInfo[infoIndex].unkC.vx;
    model->unk1A = g_JetModelInfo[infoIndex].unk4.vz;
    model->unk18 = g_JetModelInfo[infoIndex].unkC.vz;
    model->unk2 = 0;
    model->triCount = numTri;
    model->quadCount = numQua;
    model->unk8 = 0;
    model->polyCount = numTri + numQua;
    model->tris = JetTrianglesAlloc(numTri);
    model->quads = JetQuadsAlloc(numQua);
    return model;
}

JetModel* JetModelAlloc(void) {
    u32* counter;
    JetModel* base;
    s32 index;

    counter = &g_JetModelCount;
    index = *counter;
    base = g_JetModelPool;
    *counter = index + 1;
    return &base[index];
}

JetTriangle* JetTrianglesAlloc(s32 count) {
    s32* cursor;
    JetTriangle* base;
    s32 index;

    cursor = &g_JetTriangleCursor;
    index = *cursor;
    *cursor = index + count;
    base = g_JetTriangles;
    return &base[index];
}

JetQuad* JetQuadsAlloc(s32 count) {
    s32* cursor;
    JetQuad* base;
    s32 index;

    cursor = &g_JetQuadCursor;
    index = *cursor;
    *cursor = index + count;
    base = g_JetQuads;
    return &base[index];
}

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

void JetBufferReset(void) {
    ClearOTagR(g_JetBufferPtr[0]->ot, LEN(g_JetBufferPtr[0]->ot));
    ClearOTagR(g_JetBufferPtr[0]->ot2, LEN(g_JetBufferPtr[0]->ot2));
    JetPrimCursorsReset(&g_JetBufferPtr[0]->prims);
}

void JetPrimsInit(JetPrimBuffer* prims) {
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

void JetNodesInit(void) {
    JetNode* head;
    JetNode* tail;
    s32 i;

    JetNodeInit(&g_JetRootNode, 0);
    g_JetRootNode.depth = 0;
    g_JetNextFreeNode = 0;
    for (i = 0; i < LEN(g_JetNodeFreeList); i++) {
        g_JetNodeFreeList[i] = i + 1;
    }
    for (i = 0; i < LEN(g_JetNodeListHeads); i++) {
        head = &g_JetNodeListHeads[i];
        tail = &g_JetNodeListTails[i];
        head->prev = NULL;
        head->next = tail;
        tail->prev = head;
        tail->next = NULL;
    }
}

void JetNodeInit(JetNode* node, s16 index) {
    node->m.m[0][0] = 0x1000;
    node->m.m[1][1] = 0x1000;
    node->m.m[2][2] = 0x1000;
    node->m.t[0] = 0;
    node->m.t[1] = 0;
    node->m.t[2] = 0;
    node->m.m[0][1] = 0;
    node->m.m[0][2] = 0;
    node->m.m[1][0] = 0;
    node->m.m[1][2] = 0;
    node->m.m[2][0] = 0;
    node->m.m[2][1] = 0;
    node->parent = &g_JetRootNode;
    node->index = index;
    node->prev = 0;
    node->next = 0;
}

JetNode* JetNodeAlloc(
    s16 modelId, s32 arg1, s32 arg2, s32 arg3, JetNode* parent, s32 x, s32 y, s32 z, s16 rotX, s16 rotY, s16 rotZ) {
    SVECTOR rot;
    JetNode* node;
    JetNode* pool;
    s16 index;

    index = JetNodeIndexAlloc();
    pool = g_JetNodePool;
    node = &pool[index];
    JetNodeLink(node, parent);
    node->model = g_JetModelTable[modelId];
    node->modelId = modelId;
    node->index = index;
    setVector(&rot, rotX, rotY, rotZ);
    RotMatrix(&rot, &node->m);
    node->m.t[0] = x;
    node->m.t[1] = y;
    node->m.t[2] = z;
    return node;
}

void JetNodeFree(JetNode* node) {
    JetNodeUnlink(node);
    JetNodeIndexFree(node->index);
}

s16 JetNodeIndexAlloc(void) {
    s16* head;
    s16 result;

    head = &g_JetNextFreeNode;
    result = *head;
    *head = g_JetNodeFreeList[result];

    return result;
}

void JetNodeIndexFree(s16 index) {
    s16* head;
    s16* slot;

    slot = &g_JetNodeFreeList[index];
    head = &g_JetNextFreeNode;
    *slot = *head;
    *head = index;
}

void JetNodeLink(JetNode* node, JetNode* parent) {
    JetNode* last;
    JetNode* tail;
    s16 depth;

    node->parent = parent;
    depth = parent->depth + 1;
    node->depth = depth;
    tail = &g_JetNodeListTails[depth];
    last = tail->prev;
    node->prev = last;
    node->next = last->next;
    tail->prev->next = node;
    tail->prev = node;
}

void JetNodeUnlink(JetNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}
