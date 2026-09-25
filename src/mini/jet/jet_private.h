#ifndef JET_PRIVATE_H
#define JET_PRIVATE_H

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

#endif
