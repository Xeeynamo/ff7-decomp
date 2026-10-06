#ifndef HIGHWAY_PRIVATE_H
#define HIGHWAY_PRIVATE_H

#include "types.h"
#include <game.h>
#include <inline_o.h>
#include <libetc.h>
#include "../jet/jet_model.h"

typedef struct {
    /* 0x00 */ VECTOR position;
    /* 0x10 */ char pad10[8];
    /* 0x18 */ SVECTOR rotation;
    /* 0x20 */ char pad20[8];
    /* 0x28 */ s32 unk28;
    /* 0x2C */ s32 unk2C;
    /* 0x30 */ char pad30[4];
    /* 0x34 */ s32 unk34;
    /* 0x38 */ s32 unk38;
    /* 0x3C */ char pad3C[0x14];
    /* 0x50 */ s32 unk50;
    /* 0x54 */ s32 unk54;
    /* 0x58 */ s32 unk58;
    /* 0x5C */ s32 unk5C;
    /* 0x60 */ char pad60[0x18];
    /* 0x78 */ s32 unk78;
    /* 0x7C */ char pad7C[0x2C];
    /* 0xA8 */ s32 unkA8;
    /* 0xAC */ char padAC[0x28];
    /* 0xD4 */ JetNode* node;
    /* 0xD8 */ s16 index; // -1 when free
    /* 0xDA */ s16 active;
} HighwayObject; // size: 0xDC

typedef struct {
    /* 0x00 */ s16 triCount;
    /* 0x02 */ s16 quadCount;
    /* 0x04 */ SVECTOR boundsMin;
    /* 0x0C */ SVECTOR boundsMax;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 : 16;
} HighwayModelInfo; // size: 0x18

extern HighwayModelInfo* g_HighwayModelInfoAddr;
extern JetTriangle* g_HighwayTrianglesAddr;
extern JetQuad* g_HighwayQuadsAddr;
extern JetNode g_HighwayNodePool[400];
extern HighwayModelInfo* g_HighwayModelInfo;
extern JetModel* g_HighwayModelTable[181];
extern s32 g_HighwayTriangleCursor;
extern s16 g_HighwayNodeFreeList[400];
extern u32 g_HighwayModelCount;
extern JetNode g_HighwayNodeListHeads[10];
extern JetTriangle* g_HighwayTriangles;
extern JetModel g_HighwayModelPool[185];
extern s16 g_HighwayNextFreeNode;
extern JetNode g_HighwayRootNode;
extern s32 g_HighwayQuadCursor;
extern JetQuad* g_HighwayQuads;
extern s16 g_HighwayObjectFreeList[100];
extern HighwayObject g_HighwayObjectTemplate;
extern HighwayObject g_HighwayObjects[100];
extern s16 g_HighwayNextFreeObject;
extern JetNode g_HighwayNodeListTails[10];
extern s16 g_HighwayObjectCount;

void HighwayNodesInit(void);
void HighwayNodeInit(JetNode* node, s16 index);
JetNode* HighwayNodeAlloc(
    s16 modelId, s32 arg1, s32 arg2, s32 arg3, JetNode* parent, s32 x, s32 y, s32 z, s16 rotX, s16 rotY, s16 rotZ);
void func_800A3D24(JetNode* node, SVECTOR* rot, VECTOR* pos);
JetNode* HighwayNodeAllocYXZ(s16 modelId, JetNode* parent, VECTOR* pos, SVECTOR* rot);
void HighwayNodeFree(JetNode* node);
s16 HighwayNodeIndexAlloc(void);
void HighwayNodeIndexFree(s16 index);
void HighwayNodeLink(JetNode* node, JetNode* parent);
void HighwayNodeUnlink(JetNode* node);
void HighwayModelsReset(void);
JetModel* HighwayModelBuild(s32 infoIndex);
JetModel* HighwayModelAlloc(void);
JetTriangle* HighwayTrianglesAlloc(s32 count);
JetQuad* HighwayQuadsAlloc(s32 count);
void HighwayObjectsInit(void);
s16 HighwayObjectAlloc(HighwayObject* spawn, s16 parentIndex);
s16 HighwayObjectIndexAlloc(void);
void HighwayObjectIndexFree(s16 index);
HighwayObject* HighwayObjectSpawn(s16 x, s16 y, s16 z, s16 type, s16 modelId);

#endif
