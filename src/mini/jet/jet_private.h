#ifndef JET_PRIVATE_H
#define JET_PRIVATE_H

#include "types.h"
#include <game.h>
#include <inline_o.h>
#include <libetc.h>

// Nine write cursors, each reset to the start of its own buffer below.
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
    /* 0x04 */ SVECTOR boundsMin;
    /* 0x0C */ SVECTOR boundsMax;
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
    /* 0x14 */ s16 boundsMaxX;
    /* 0x16 */ s16 boundsMinX;
    /* 0x18 */ s16 boundsMaxZ;
    /* 0x1A */ s16 boundsMinZ;
    /* 0x1C */ s32 : 32;
} JetModel; // size: 0x20

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

// The behaviour state an object's type handler drives.
typedef struct {
    /* 0x00 */ s32 type;
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
    /* 0x50 */ s32 unk50[0x14]; // shootable types: [0] points, [0xD] hit points, [18] death sfx
} JetObjectState;               // size: 0xA0

typedef struct {
    /* 0x00 */ VECTOR position;
    /* 0x10 */ char pad10[8];
    /* 0x18 */ SVECTOR rotation;
    /* 0x20 */ char pad20[8];
    /* 0x28 */ JetObjectState unk28;
    /* 0xC8 */ s32 pathLen;
    /* 0xCC */ SVECTOR* path;
    /* 0xD0 */ s32 : 32;
    /* 0xD4 */ JetNode* unkD4;
    /* 0xD8 */ s16 unkD8;
    /* 0xDA */ s16 unkDA;
    /* 0xDC */ SVECTOR unkDC[6]; // the model bounding box's six face centres
    /* 0x10C */ char pad10C[0x10];
    /* 0x11C */ u_long unk11C[6]; // the same six points projected to the screen
    /* 0x134 */ char pad134[8];
} JetObject; // size: 0x13C

// One scheduled object spawn, read from xbin stream 0xE.
typedef struct {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 : 16;
    /* 0x04 */ s16 modelId;
    /* 0x06 */ s16 : 16;
    /* 0x08 */ s32 pathIndex;
    /* 0x0C */ s32 speed;
    /* 0x10 */ s32 params[0x14];
} JetObjectSpawn; // size: 0x60

// XBINADR.BIN: pointers to the xbin streams in the decompressed XBIN2.BIN.
typedef struct {
    /* 0x00 */ u_long unk0; // sound data, handed to Akao opcode 0x10
    /* 0x04 */ JetModelInfo* modelInfo;
    /* 0x08 */ u16* trackAdds;
    /* 0x0C */ u16* trackRemoves;
    /* 0x10 */ u8* trackPaths;
    /* 0x14 */ s32* trackPathOffsets;
    /* 0x18 */ s32* trackPathLengths;
    /* 0x1C */ SVECTOR* trackRotations;
    /* 0x20 */ u16* triangleAdds;
    /* 0x24 */ u16* triangleRemoves;
    /* 0x28 */ JetTriangle* triangles;
    /* 0x2C */ u8* objectPaths;
    /* 0x30 */ s32* objectPathOffsets;
    /* 0x34 */ s32* objectPathLengths;
    /* 0x38 */ JetObjectSpawn* spawns;
    /* 0x3C */ u8* spawnCounts;
    /* 0x40 */ JetQuad* quads;
} JetXbinAdr; // size: 0x44

extern VECTOR g_JetCameraPosCopy;
extern s16 g_JetBeam0OriginX;
extern s16 g_JetBeam0OriginY;
extern s16 g_JetBeam1OriginX;
extern s16 g_JetBeam1OriginY;
extern s32 g_JetSpeed;
extern u16 g_JetSpriteTPage[];
extern JetModelInfo* g_JetModelInfo;
extern u16 g_JetFadeClut;
extern s16 g_JetPopupTimer;
extern s16 g_JetPopupPoints;
extern s16 g_JetPopupModelId;
extern u16 g_JetFadeTPage;
extern JetBuffer g_JetBuffers[2];
extern s32 g_JetScore;
extern s32 g_JetTrackSegment;
extern JetNode g_JetRootNode;
extern u8 g_JetBeamScroll;
extern JetModel* g_JetModelTable[100];
extern JetBuffer* g_JetBufferPtr[1];
extern JetXbinAdr g_JetXbinAdr;
extern u8 g_JetFiring;
extern s32 g_JetCameraPathPos;
extern s16 g_JetShotPower;
extern u8 g_JetShotRepeatCounter;
extern s16 g_JetCursorX;
extern s16 g_JetCursorY;
extern u8 g_JetScorePopupAlternate;
extern u8 g_JetTransitionDrawEnabled;
extern u8 g_JetExit;
extern SVECTOR g_JetPopupRot;
extern u16 g_JetSpriteClut[];

void JetPrimCursorsReset(JetPrimBuffer* prims);
void JetNodeFree(JetNode* node);
void JetPlaySfx(s16 soundId);
void JetDrawObjectAndCheckHit(JetBuffer* drawBuffer, JetNode* node, s16 otIndex, s32 unusedArg, JetObject* object);
s32 JetVectorInsidePlanes(VECTOR* arg0);
void JetObjectsInit(void);
void JetFrustumInit(void);
void JetModelsReset(void);
JetModel* JetModelBuild(s32 infoIndex);
void JetBuffersInit(void);
void JetNodesInit(void);
void JetTrackSample(u32 trackPosition, s32 heightOffset, VECTOR* position, SVECTOR* rotation);
void JetAudioFadeOut(void);
void JetObjectsUpdate(JetBuffer* db);
void JetDrawCartAndProjectBeams(JetBuffer* drawBuffer, JetNode* node, s16 otIndex, s32 unusedArg, JetObject* object);
JetNode* JetNodeAlloc(
    s16 modelId, s32 arg1, s32 arg2, s32 arg3, JetNode* parent, s32 x, s32 y, s32 z, s16 rotX, s16 rotY, s16 rotZ);

#endif
