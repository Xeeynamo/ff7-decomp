//! PSYQ=3.3

#include "highway_private.h"

// Same as JetNodeInit.
void HighwayNodeInit(JetNode* node, s16 index) {
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
    node->parent = &g_HighwayRootNode;
    node->index = index;
    node->prev = NULL;
    node->next = NULL;
}

// Same as JetNodeAlloc.
JetNode* HighwayNodeAlloc(
    s16 modelId, s32 arg1, s32 arg2, s32 arg3, JetNode* parent, s32 x, s32 y, s32 z, s16 rotX, s16 rotY, s16 rotZ) {
    SVECTOR rot;
    JetNode* node;
    s16 index;

    index = HighwayNodeIndexAlloc();
    node = &g_HighwayNodePool[index];
    HighwayNodeLink(node, parent);
    node->model = g_HighwayModelTable[modelId];
    node->index = index;
    node->modelId = modelId;
    setVector(&rot, rotX, rotY, rotZ);
    RotMatrix(&rot, &node->m);
    node->m.t[0] = x;
    node->m.t[1] = y;
    node->m.t[2] = z;
    return node;
}

void func_800A3D24(JetNode* node, SVECTOR* rot, VECTOR* pos) {
    RotMatrixYXZ(rot, &node->m);
    node->m.t[0] = pos->vx;
    node->m.t[1] = pos->vy;
    node->m.t[2] = pos->vz;
}

// Like HighwayNodeAlloc, but with a VECTOR position and YXZ rotation.
JetNode* HighwayNodeAllocYXZ(s16 modelId, JetNode* parent, VECTOR* pos, SVECTOR* rot) {
    SVECTOR unused;
    JetNode* node;
    s16 index;

    index = HighwayNodeIndexAlloc();
    node = &g_HighwayNodePool[index];
    HighwayNodeLink(node, parent);
    node->model = g_HighwayModelTable[modelId];
    node->modelId = modelId;
    node->index = index;
    RotMatrixYXZ(rot, &node->m);
    node->m.t[0] = pos->vx;
    node->m.t[1] = pos->vy;
    node->m.t[2] = pos->vz;
    return node;
}

// Same as JetNodeFree.
void HighwayNodeFree(JetNode* node) {
    HighwayNodeUnlink(node);
    HighwayNodeIndexFree(node->index);
}

// Same as JetNodeIndexAlloc.
s16 HighwayNodeIndexAlloc(void) {
    s16 index;

    index = g_HighwayNextFreeNode;
    g_HighwayNextFreeNode = g_HighwayNodeFreeList[index];

    return index;
}

// Same as JetNodeIndexFree.
void HighwayNodeIndexFree(s16 index) {
    s16 next;

    next = g_HighwayNextFreeNode;
    g_HighwayNextFreeNode = index;
    g_HighwayNodeFreeList[index] = next;
}

// Same as JetNodeLink.
void HighwayNodeLink(JetNode* node, JetNode* parent) {
    s16 depth;

    node->parent = parent;
    depth = parent->depth + 1;
    node->depth = depth;
    node->prev = g_HighwayNodeListTails[depth].prev;
    node->next = node->prev->next;
    g_HighwayNodeListTails[depth].prev->next = node;
    g_HighwayNodeListTails[depth].prev = node;
}

// Same as JetNodeUnlink.
void HighwayNodeUnlink(JetNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

// Same as JetModelsReset.
void HighwayModelsReset(void) {
    g_HighwayTriangleCursor = 0;
    g_HighwayQuadCursor = 0;
    g_HighwayModelCount = 0;
    g_HighwayTriangles = g_HighwayTrianglesAddr;
    g_HighwayQuads = g_HighwayQuadsAddr;
    g_HighwayModelInfo = g_HighwayModelInfoAddr;
}

// Like JetModelBuild, with a 0x18-byte model info record.
JetModel* HighwayModelBuild(s32 infoIndex) {
    JetModel* model;
    s32 numTri;
    s32 numQua;

    model = HighwayModelAlloc();
    numTri = g_HighwayModelInfo[infoIndex].triCount;
    numQua = g_HighwayModelInfo[infoIndex].quadCount;
    model->boundsMinX = g_HighwayModelInfo[infoIndex].boundsMin.vx;
    model->boundsMaxX = g_HighwayModelInfo[infoIndex].boundsMax.vx;
    model->boundsMinZ = g_HighwayModelInfo[infoIndex].boundsMin.vz;
    model->boundsMaxZ = g_HighwayModelInfo[infoIndex].boundsMax.vz;
    model->unk2 = 0;
    model->triCount = numTri;
    model->quadCount = numQua;
    model->unk8 = 0;
    model->polyCount = numTri + numQua;
    model->tris = HighwayTrianglesAlloc(numTri);
    model->quads = HighwayQuadsAlloc(numQua);
    model->unk1C = g_HighwayModelInfo[infoIndex].unk14;
    return model;
}

JetModel* HighwayModelAlloc(void) { return &g_HighwayModelPool[g_HighwayModelCount++]; }

JetTriangle* HighwayTrianglesAlloc(s32 count) {
    s32 index;

    index = g_HighwayTriangleCursor;
    g_HighwayTriangleCursor += count;
    return &g_HighwayTriangles[index];
}

JetQuad* HighwayQuadsAlloc(s32 count) {
    s32 index;

    index = g_HighwayQuadCursor;
    g_HighwayQuadCursor += count;
    return &g_HighwayQuads[index];
}
