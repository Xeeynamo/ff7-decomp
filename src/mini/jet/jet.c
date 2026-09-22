//! PSYQ=3.3

#include "types.h"
#include <game.h>

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

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", MINI_Jet);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A0874);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A0D78);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1198);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A12EC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A13AC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1450);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A16A4);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1A64);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1B64);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1CD8);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1F18);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2058);

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
    s32 a;
    s32 b;
    s32 c;
    s32 hs;
    s32 ok;
    s32 len;

    a = g_JetLeftPlaneNormalX;
    b = g_JetLeftPlaneNormalY;
    c = g_JetLeftPlaneNormalZ;
    ok = 0;
    hs = (a * (x >> 2)) + (b * (y >> 2)) + (c * (z >> 2)) + g_JetLeftPlaneDistance;
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
    s32 a;
    s32 b;
    s32 c;
    s32 hs;
    s32 ok;
    s32 len;

    a = g_JetRightPlaneNormalX;
    b = g_JetRightPlaneNormalY;
    c = g_JetRightPlaneNormalZ;
    ok = 0;
    hs = (a * (x >> 2)) + (b * (y >> 2)) + (c * (z >> 2)) + g_JetRightPlaneDistance;
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
    JetPrimBuffer* temp_s1;
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
    temp_s1 = &g_JetBuffers[0].prims;
    JetPrimsInit(temp_s1);
    JetPrimsInit(&g_JetBuffers[1].prims);
    JetPrimCursorsReset(temp_s1);
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
    JetNode* a;
    JetNode* b;
    s32 i;

    JetNodeInit(&g_JetRootNode, 0);
    g_JetRootNode.depth = 0;
    g_JetNextFreeNode = 0;
    for (i = 0; i < LEN(g_JetNodeFreeList); i++) {
        g_JetNodeFreeList[i] = i + 1;
    }
    for (i = 0; i < LEN(g_JetNodeListHeads); i++) {
        a = &g_JetNodeListHeads[i];
        b = &g_JetNodeListTails[i];
        a->prev = NULL;
        a->next = b;
        b->prev = a;
        b->next = NULL;
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
    SVECTOR sp10;
    JetNode* temp_s0;
    JetNode* temp_v1;
    s16 temp_v0;

    temp_v0 = JetNodeIndexAlloc();
    temp_v1 = g_JetNodePool;
    temp_s0 = &temp_v1[temp_v0];
    JetNodeLink(temp_s0, parent);
    temp_s0->model = g_JetModelTable[modelId];
    temp_s0->modelId = modelId;
    temp_s0->index = temp_v0;
    setVector(&sp10, rotX, rotY, rotZ);
    RotMatrix(&sp10, &temp_s0->m);
    temp_s0->m.t[0] = x;
    temp_s0->m.t[1] = y;
    temp_s0->m.t[2] = z;
    return temp_s0;
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
    s16* temp;
    s16* temp2;

    temp2 = &g_JetNodeFreeList[index];
    temp = &g_JetNextFreeNode;
    *temp2 = *temp;
    *temp = index;
}

void JetNodeLink(JetNode* node, JetNode* parent) {
    JetNode* temp_v0_2;
    JetNode* temp_v1;
    s16 temp_v0;

    node->parent = parent;
    temp_v0 = parent->depth + 1;
    node->depth = temp_v0;
    temp_v1 = &g_JetNodeListTails[temp_v0];
    temp_v0_2 = temp_v1->prev;
    node->prev = temp_v0_2;
    node->next = temp_v0_2->next;
    temp_v1->prev->next = node;
    temp_v1->prev = node;
}

void JetNodeUnlink(JetNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}
