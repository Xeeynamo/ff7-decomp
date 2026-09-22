//! PSYQ=3.3

#include "types.h"
#include <game.h>

// One pool per primitive type, each with the write cursor that walks it.
typedef struct {
    /* 0x0000 */ void* f3Cursor;
    /* 0x0004 */ void* f4Cursor;
    /* 0x0008 */ void* g3Cursor;
    /* 0x000C */ void* g4Cursor;
    /* 0x0010 */ void* ft3Cursor;
    /* 0x0014 */ void* ft4Cursor;
    /* 0x0018 */ void* gt3Cursor;
    /* 0x001C */ void* gt4Cursor;
    /* 0x0020 */ void* lineCursor;
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

// Offsets 0x00 and 0x5C are fixed by SetDefDrawEnv/SetDefDispEnv; both
// ordering tables are sized by their ClearOTagR calls.
typedef struct {
    /* 0x0000 */ DRAWENV draw;
    /* 0x005C */ DISPENV disp;
    /* 0x0070 */ u_long ot[0x1000];
    /* 0x4070 */ u_long unk4070[10];
    /* 0x4098 */ u_long ot2[0xB4];
    /* 0x4368 */ JetPrimBuffer prims;
} JetBuffer; // size: 0x1265C

typedef struct {
    /* 0x00 */ s16 triCount;
    /* 0x02 */ s16 quadCount;
    /* 0x04 */ SVECTOR unk4;
    /* 0x0C */ SVECTOR unkC;
} JetModelInfo; // size: 0x14

typedef struct {
    /* 0x00 */ s16 polyCount;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ s16 triCount;
    /* 0x06 */ s16 quadCount;
    /* 0x08 */ s16 unk8;
    /* 0x0A */ char padA[2];
    /* 0x0C */ s32* tris;
    /* 0x10 */ s32* quads;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ char pad1C[4];
} JetModel; // size: 0x20

// Scene graph node. The prev/next pair chains every node of one depth
// between the head and tail sentinels g_JetNodeListHeads/g_JetNodeListTails.
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

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ char pad4[0x20];
} JetTriangle; // size: 0x24

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ char pad4[0x24];
} JetQuad; // size: 0x28

// The two view frustum side planes: an unnormalised normal, a plane distance,
// and the normal's length, which scales a half-space value to a world distance.
extern s32 g_JetLeftPlaneNormalX;
extern s32 g_JetLeftPlaneNormalY;
extern s32 g_JetLeftPlaneNormalZ;
extern s32 g_JetRightPlaneNormalX;
extern s32 g_JetRightPlaneNormalY;
extern s32 g_JetRightPlaneNormalZ;
extern s32 D_800A8950; // the sign the left plane's inside half-space has
extern s32 D_800A8968; // the same, for the right plane
extern s32 g_JetLeftPlaneDistance;
extern s32 g_JetRightPlaneDistance;
extern s32 g_JetLeftNormalLength;
extern s32 g_JetRightNormalLength;

// Models and their primitives are carved out of three streams by bump
// cursors that are never rewound; g_Jet*Base holds each stream's start.
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

JetModel* func_800A7BF4(void);
s32* func_800A7C20(s32 count);
s32* func_800A7C54(s32 count);
void func_800A7E70(JetPrimBuffer* arg0);
void func_800A7FAC(JetPrimBuffer* arg0);
void func_800A80A8(JetNode* arg0, s16 arg1);
s16 func_800A8238(void);
void func_800A8264(s16 arg0);
void func_800A8290(JetNode* arg0, JetNode* arg1);
void func_800A82F0(JetNode* arg0);

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

s32 func_800A7414(VECTOR* arg0) {
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
    hsLeft = (lx * (arg0->vx >> 2)) + (ly * (arg0->vy >> 2)) + (lz * (arg0->vz >> 2)) + g_JetLeftPlaneDistance;
    rx = g_JetRightPlaneNormalX;
    ry = g_JetRightPlaneNormalY;
    rz = g_JetRightPlaneNormalZ;
    hsRight = (rx * (arg0->vx >> 2)) + (ry * (arg0->vy >> 2)) + (rz * (arg0->vz >> 2)) + g_JetRightPlaneDistance;
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

s32 func_800A7544(SVECTOR* arg0) {
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
    hsLeft = (lx * (arg0->vx >> 2)) + (ly * (arg0->vy >> 2)) + (lz * (arg0->vz >> 2)) + g_JetLeftPlaneDistance;
    rx = g_JetRightPlaneNormalX;
    ry = g_JetRightPlaneNormalY;
    rz = g_JetRightPlaneNormalZ;
    hsRight = (rx * (arg0->vx >> 2)) + (ry * (arg0->vy >> 2)) + (rz * (arg0->vz >> 2)) + g_JetRightPlaneDistance;
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

s32 func_800A7688(s32 arg0, s32 arg1, s32 arg2) {
    s32 a;
    s32 b;
    s32 c;

    a = g_JetLeftPlaneNormalX;
    b = g_JetLeftPlaneNormalY;
    c = g_JetLeftPlaneNormalZ;

    return (a * (arg0 >> 2)) + (b * (arg1 >> 2)) + (c * (arg2 >> 2)) + g_JetLeftPlaneDistance;
}

s32 func_800A76DC(s32 arg0, s32 arg1, s32 arg2) {
    s32 a;
    s32 b;
    s32 c;

    a = g_JetRightPlaneNormalX;
    b = g_JetRightPlaneNormalY;
    c = g_JetRightPlaneNormalZ;

    return (a * (arg0 >> 2)) + (b * (arg1 >> 2)) + (c * (arg2 >> 2)) + g_JetRightPlaneDistance;
}

s32 func_800A7730(VECTOR* arg0, s16 arg1) {
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
    hsLeft = (lx * (arg0->vx >> 2)) + (ly * (arg0->vy >> 2)) + (lz * (arg0->vz >> 2)) + g_JetLeftPlaneDistance;
    if (D_800A8950 > 0 && hsLeft >= 0) {
        leftOk = 1;
    }
    if (D_800A8950 < 0 && hsLeft <= 0) {
        leftOk = 1;
    }
    if (leftOk == 0) {
        len = g_JetLeftNormalLength;
        planeDistance = ((hsLeft < 0) ? -hsLeft : hsLeft) / len;
        if (planeDistance < arg1) {
            leftOk = 1;
        }
    }
    rx = g_JetRightPlaneNormalX;
    ry = g_JetRightPlaneNormalY;
    rz = g_JetRightPlaneNormalZ;
    hsRight = (rx * (arg0->vx >> 2)) + (ry * (arg0->vy >> 2)) + (rz * (arg0->vz >> 2)) + g_JetRightPlaneDistance;
    if (D_800A8968 > 0 && hsRight >= 0) {
        rightOk = 1;
    }
    if (D_800A8968 < 0 && hsRight <= 0) {
        rightOk = 1;
    }
    if (rightOk == 0) {
        len = g_JetRightNormalLength;
        planeDistance = ((hsRight < 0) ? -hsRight : hsRight) / len;
        if (planeDistance < arg1) {
            rightOk = 1;
        }
    }
    return leftOk & rightOk;
}

s32 func_800A7928(s32 arg0, s32 arg1, s32 arg2, s16 arg3) {
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
    hs = (a * (arg0 >> 2)) + (b * (arg1 >> 2)) + (c * (arg2 >> 2)) + g_JetLeftPlaneDistance;
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
        if (hs / len < arg3) {
            ok = 1;
        }
    }
    return ok;
}

s32 func_800A7A10(s32 arg0, s32 arg1, s32 arg2, s16 arg3) {
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
    hs = (a * (arg0 >> 2)) + (b * (arg1 >> 2)) + (c * (arg2 >> 2)) + g_JetRightPlaneDistance;
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
        if (hs / len < arg3) {
            ok = 1;
        }
    }
    return ok;
}

void func_800A7AF8(void) {
    g_JetTriangleCursor = 0;
    g_JetQuadCursor = 0;
    g_JetModelCount = 0;
    g_JetTriangles = g_JetTrianglesBase;
    g_JetQuads = g_JetQuadsBase;
    g_JetModelInfo = g_JetModelInfoBase;
}

JetModel* func_800A7B48(s32 arg0) {
    JetModel* model;
    s32 numTri;
    s32 numQua;

    model = func_800A7BF4();
    numTri = g_JetModelInfo[arg0].triCount;
    numQua = g_JetModelInfo[arg0].quadCount;
    model->unk16 = g_JetModelInfo[arg0].unk4.vx;
    model->unk14 = g_JetModelInfo[arg0].unkC.vx;
    model->unk1A = g_JetModelInfo[arg0].unk4.vz;
    model->unk18 = g_JetModelInfo[arg0].unkC.vz;
    model->unk2 = 0;
    model->triCount = numTri;
    model->quadCount = numQua;
    model->unk8 = 0;
    model->polyCount = numTri + numQua;
    model->tris = func_800A7C20(numTri);
    model->quads = func_800A7C54(numQua);
    return model;
}

JetModel* func_800A7BF4(void) {
    u32* counter;
    JetModel* base;
    s32 index;

    counter = &g_JetModelCount;
    index = *counter;
    base = g_JetModelPool;
    *counter = index + 1;
    return &base[index];
}

s32* func_800A7C20(s32 count) {
    s32* cursor;
    JetTriangle* base;
    s32 index;

    cursor = &g_JetTriangleCursor;
    index = *cursor;
    *cursor = index + count;
    base = g_JetTriangles;
    return &base[index].unk0;
}

s32* func_800A7C54(s32 count) {
    s32* cursor;
    JetQuad* base;
    s32 index;

    cursor = &g_JetQuadCursor;
    index = *cursor;
    *cursor = index + count;
    base = g_JetQuads;
    return &base[index].unk0;
}

void func_800A7C88(void) {
    JetPrimBuffer* temp_s1;
    JetBuffer* db;
    u_char* isbg;

    SetDefDrawEnv(&g_JetBuffers[0].draw, 0, 0, 0x140, 0xF0);
    SetDefDispEnv(&g_JetBuffers[0].disp, 0, 0xF0, 0x140, 0xF0);
    SetDefDrawEnv(&g_JetBuffers[1].draw, 0, 0xF0, 0x140, 0xF0);
    SetDefDispEnv(&g_JetBuffers[1].disp, 0, 0, 0x140, 0xF0);
    db = g_JetBuffers;
    g_JetBuffers[0].draw.isbg = 0;
    // Stored off the buffer base register; a direct field store folds to an absolute address.
    isbg = &db[1].draw.isbg;
    *isbg = 0;
    setRGB0(&g_JetBuffers[0].draw, 0, 0, 8);
    setRGB0(&g_JetBuffers[1].draw, 0, 0, 8);
    SetGeomOffset(0xA0, 0xA0);
    SetGeomScreen(0x100);
    SetDispMask(1);
    SetBackColor(0x80, 0x80, 0x80);
    SetFarColor(0, 0, 8);
    temp_s1 = &g_JetBuffers[0].prims;
    func_800A7E70(temp_s1);
    func_800A7E70(&g_JetBuffers[1].prims);
    func_800A7FAC(temp_s1);
    func_800A7FAC(&g_JetBuffers[1].prims);
    ClearOTagR(g_JetBuffers[0].ot, LEN(g_JetBuffers[0].ot));
    ClearOTagR(g_JetBuffers[1].ot, LEN(g_JetBuffers[1].ot));
    ClearOTagR(g_JetBuffers[0].ot2, LEN(g_JetBuffers[0].ot2));
    ClearOTagR(g_JetBuffers[1].ot2, LEN(g_JetBuffers[1].ot2));
    *g_JetBufferPtr = &g_JetBuffers[0];
}

void func_800A7E1C(void) {
    ClearOTagR(g_JetBufferPtr[0]->ot, LEN(g_JetBufferPtr[0]->ot));
    ClearOTagR(g_JetBufferPtr[0]->ot2, LEN(g_JetBufferPtr[0]->ot2));
    func_800A7FAC(&g_JetBufferPtr[0]->prims);
}

// Tag every primitive in the nine pools with its type and length.
void func_800A7E70(JetPrimBuffer* arg0) {
    s32 i;

    for (i = 0; i < LEN(arg0->f3); i++) {
        SetPolyF3(&arg0->f3[i]);
    }
    for (i = 0; i < LEN(arg0->f4); i++) {
        SetPolyF4(&arg0->f4[i]);
    }
    for (i = 0; i < LEN(arg0->g3); i++) {
        SetPolyG3(&arg0->g3[i]);
    }
    for (i = 0; i < LEN(arg0->g4); i++) {
        SetPolyG4(&arg0->g4[i]);
    }
    for (i = 0; i < LEN(arg0->ft3); i++) {
        SetPolyFT3(&arg0->ft3[i]);
    }
    for (i = 0; i < LEN(arg0->ft4); i++) {
        SetPolyFT4(&arg0->ft4[i]);
    }
    for (i = 0; i < LEN(arg0->gt3); i++) {
        SetPolyGT3(&arg0->gt3[i]);
    }
    for (i = 0; i < LEN(arg0->gt4); i++) {
        SetPolyGT4(&arg0->gt4[i]);
    }
    for (i = 0; i < LEN(arg0->line); i++) {
        SetLineF2(&arg0->line[i]);
    }
}

void func_800A7FAC(JetPrimBuffer* arg0) {
    arg0->f3Cursor = arg0->f3;
    arg0->f4Cursor = arg0->f4;
    arg0->g3Cursor = arg0->g3;
    arg0->g4Cursor = arg0->g4;
    arg0->ft3Cursor = arg0->ft3;
    arg0->ft4Cursor = arg0->ft4;
    arg0->gt3Cursor = arg0->gt3;
    arg0->gt4Cursor = arg0->gt4;
    arg0->lineCursor = arg0->line;
}

void func_800A8010(void) {
    JetNode* a;
    JetNode* b;
    s32 i;

    func_800A80A8(&g_JetRootNode, 0);
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

void func_800A80A8(JetNode* arg0, s16 arg1) {
    arg0->m.m[0][0] = 0x1000;
    arg0->m.m[1][1] = 0x1000;
    arg0->m.m[2][2] = 0x1000;
    arg0->m.t[0] = 0;
    arg0->m.t[1] = 0;
    arg0->m.t[2] = 0;
    arg0->m.m[0][1] = 0;
    arg0->m.m[0][2] = 0;
    arg0->m.m[1][0] = 0;
    arg0->m.m[1][2] = 0;
    arg0->m.m[2][0] = 0;
    arg0->m.m[2][1] = 0;
    arg0->parent = &g_JetRootNode;
    arg0->index = arg1;
    arg0->prev = 0;
    arg0->next = 0;
}

JetNode* func_800A80F8(s16 arg0, s32 arg1, s32 arg2, s32 arg3, JetNode* arg4, s32 arg5, s32 arg6, s32 arg7, s16 arg8,
                       s16 arg9, s16 arg10) {
    SVECTOR sp10;
    JetNode* temp_s0;
    JetNode* temp_v1;
    s16 temp_v0;

    temp_v0 = func_800A8238();
    temp_v1 = g_JetNodePool;
    temp_s0 = &temp_v1[temp_v0];
    func_800A8290(temp_s0, arg4);
    temp_s0->model = g_JetModelTable[arg0];
    temp_s0->modelId = arg0;
    temp_s0->index = temp_v0;
    setVector(&sp10, arg8, arg9, arg10);
    RotMatrix(&sp10, &temp_s0->m);
    temp_s0->m.t[0] = arg5;
    temp_s0->m.t[1] = arg6;
    temp_s0->m.t[2] = arg7;
    return temp_s0;
}

void func_800A8204(JetNode* arg0) {
    func_800A82F0(arg0);
    func_800A8264(arg0->index);
}

s16 func_800A8238(void) {
    s16* head;
    s16 result;

    head = &g_JetNextFreeNode;
    result = *head;
    *head = g_JetNodeFreeList[result];

    return result;
}

void func_800A8264(s16 arg0) {
    s16* temp;
    s16* temp2;

    temp2 = &g_JetNodeFreeList[arg0];
    temp = &g_JetNextFreeNode;
    *temp2 = *temp;
    *temp = arg0;
}

void func_800A8290(JetNode* arg0, JetNode* arg1) {
    JetNode* temp_v0_2;
    JetNode* temp_v1;
    s16 temp_v0;

    arg0->parent = arg1;
    temp_v0 = arg1->depth + 1;
    arg0->depth = temp_v0;
    temp_v1 = &g_JetNodeListTails[temp_v0];
    temp_v0_2 = temp_v1->prev;
    arg0->prev = temp_v0_2;
    arg0->next = temp_v0_2->next;
    temp_v1->prev->next = arg0;
    temp_v1->prev = arg0;
}

void func_800A82F0(JetNode* arg0) {
    arg0->prev->next = arg0->next;
    arg0->next->prev = arg0->prev;
}
