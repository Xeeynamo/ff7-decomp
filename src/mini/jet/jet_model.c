//! PSYQ=3.3 FORCE_MEM=true

#include "jet_private.h"

extern s32 g_JetTriangleCursor;
extern u32 g_JetModelCount;
extern JetTriangle* g_JetTriangles;
extern JetModel g_JetModelPool[];
extern s32 g_JetQuadCursor;
extern JetQuad* g_JetQuads;

static JetModel* JetModelAlloc(void);
static JetQuad* JetQuadsAlloc(s32 count);
static JetTriangle* JetTrianglesAlloc(s32 count);

void JetModelsReset(void) {
    g_JetTriangleCursor = 0;
    g_JetQuadCursor = 0;
    g_JetModelCount = 0;
    g_JetTriangles = g_JetXbinAdr.triangles;
    g_JetQuads = g_JetXbinAdr.quads;
    g_JetModelInfo = g_JetXbinAdr.modelInfo;
}

JetModel* JetModelBuild(s32 infoIndex) {
    JetModel* model;
    s32 numTri;
    s32 numQua;

    model = JetModelAlloc();
    numTri = g_JetModelInfo[infoIndex].triCount;
    numQua = g_JetModelInfo[infoIndex].quadCount;
    model->boundsMinX = g_JetModelInfo[infoIndex].boundsMin.vx;
    model->boundsMaxX = g_JetModelInfo[infoIndex].boundsMax.vx;
    model->boundsMinZ = g_JetModelInfo[infoIndex].boundsMin.vz;
    model->boundsMaxZ = g_JetModelInfo[infoIndex].boundsMax.vz;
    model->unk2 = 0;
    model->triCount = numTri;
    model->quadCount = numQua;
    model->unk8 = 0;
    model->polyCount = numTri + numQua;
    model->tris = JetTrianglesAlloc(numTri);
    model->quads = JetQuadsAlloc(numQua);
    return model;
}

static JetModel* JetModelAlloc(void) {
    u32* counter;
    JetModel* base;
    s32 index;

    counter = &g_JetModelCount;
    index = *counter;
    base = g_JetModelPool;
    *counter = index + 1;
    return &base[index];
}

static JetTriangle* JetTrianglesAlloc(s32 count) {
    s32* cursor;
    JetTriangle* base;
    s32 index;

    cursor = &g_JetTriangleCursor;
    index = *cursor;
    *cursor = index + count;
    base = g_JetTriangles;
    return &base[index];
}

static JetQuad* JetQuadsAlloc(s32 count) {
    s32* cursor;
    JetQuad* base;
    s32 index;

    cursor = &g_JetQuadCursor;
    index = *cursor;
    *cursor = index + count;
    base = g_JetQuads;
    return &base[index];
}
