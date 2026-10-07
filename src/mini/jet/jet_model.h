#ifndef JET_MODEL_H
#define JET_MODEL_H

#include "types.h"
#include <game.h>

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
    /* 0x14 */ s16 boundsMaxX;
    /* 0x16 */ s16 boundsMinX;
    /* 0x18 */ s16 boundsMaxZ;
    /* 0x1A */ s16 boundsMinZ;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 : 16;
} JetModel; // size: 0x20

// Argument block for the GTE renderers in jet_gte.s.
typedef struct {
    /* 0x0 */ JetTriangle* tris;
    /* 0x4 */ POLY_G3* prim;
    /* 0x8 */ OT_TYPE* ot;
    /* 0xC */ JetModel* model;
} JetModelDrawArgs; // size: 0x10

// Doubly linked list node, chained by JetNodesInit with a 0x38 stride.
typedef struct JetNode {
    /* 0x00 */ JetModel* model;
    /* 0x04 */ MATRIX m;
    /* 0x24 */ struct JetNode* parent;
    /* 0x28 */ s16 modelId;
    /* 0x2A */ s16 index;
    /* 0x2C */ u16 depth;
    /* 0x2E */ char pad2E[2];
    /* 0x30 */ struct JetNode* prev;
    /* 0x34 */ struct JetNode* next;
} JetNode; // size: 0x38

#endif
