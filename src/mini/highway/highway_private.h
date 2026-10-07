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

typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 unk4;
    /* 0x06 */ u16 unk6;
    /* 0x08 */ u8 unk8[4];
    /* 0x0C */ u8 unkC[4];
    /* 0x10 */ u8 w;
    /* 0x11 */ u8 h;
    /* 0x12 */ u8 unk12;
    /* 0x13 */ u8 unk13;
} HighwayGaugeLayout; // size: 0x14

// Texture coordinates of a POLY_FT4, packed without the vertex positions.
typedef struct {
    /* 0x0 */ u8 u0;
    /* 0x1 */ u8 v0;
    /* 0x2 */ u16 clut;
    /* 0x4 */ u8 u1;
    /* 0x5 */ u8 v1;
    /* 0x6 */ u16 tpage;
    /* 0x8 */ u8 u2;
    /* 0x9 */ u8 v2;
    /* 0xA */ u16 : 16;
    /* 0xC */ u8 u3;
    /* 0xD */ u8 v3;
    /* 0xE */ u16 : 16;
} HighwayQuadUv; // size: 0x10

// One track segment of the 80-entry ring buffer.
typedef struct {
    /* 0x000 */ VECTOR pos;
    /* 0x010 */ SVECTOR rot;
    /* 0x018 */ MATRIX m;
    /* 0x038 */ s32 unk38;
    /* 0x03C */ s32 radius;
    /* 0x040 */ s32 turn;
    /* 0x044 */ s16 width;
    /* 0x046 */ s16 unk46[10];
    /* 0x05A */ s16 : 16;
    /* 0x05C */ SVECTOR unk5C[4];
    /* 0x07C */ char pad7C[0x74];
    /* 0x0F0 */ JetNode* nodes[20];
    /* 0x140 */ u8 nodeCount;
    /* 0x141 */ char pad141[3];
} HighwayRoadSegment; // size: 0x144

typedef struct {
    /* 0x00 */ s32 sxy[4];
    /* 0x10 */ s32 sz[4];
} HighwayScratchpadProj; // size: 0x20

// Lives in the scratchpad.
typedef struct {
    /* 0x000 */ char pad0[0x30];
    /* 0x030 */ VECTOR unk30;
    /* 0x040 */ VECTOR unk40;
    /* 0x050 */ VECTOR unk50;
    /* 0x060 */ VECTOR unk60;
    /* 0x070 */ VECTOR unk70;
    /* 0x080 */ char pad80[0x88];
    /* 0x108 */ SVECTOR unk108[11];
    /* 0x160 */ s32 unk160[11];
    /* 0x18C */ HighwayScratchpadProj unk18C;
    /* 0x1AC */ char pad1AC[0x20];
    /* 0x1CC */ HighwayScratchpadProj unk1CC;
} HighwayScratchpad; // size: 0x1EC

typedef struct {
    /* 0x0 */ s16 pattern; // into g_HighwayPropPatterns
    /* 0x2 */ u16 repeat;
    /* 0x4 */ u16 offset;
    /* 0x6 */ u16 height;
    /* 0x8 */ u16 flags; // 1: height from the anchor, 2: record the anchor
    /* 0xA */ u16 yaw;
} HighwayPropScript; // size: 0xC

typedef struct {
    /* 0x0 */ s32 segment; // runs once g_HighwayTrackSegment is 45 segments past it
    /* 0x4 */ s32 code;
} HighwayEvent; // size: 0x8

typedef struct {
    /* 0x0 */ u8 turn; // 0 straight, 1 and 2 turn one way or the other
    /* 0x1 */ u8 radius;
    /* 0x2 */ u16 length; // segments
    /* 0x4 */ s16 roll;
    /* 0x6 */ s16 pitch;
    /* 0x8 */ u16 width;
    /* 0xA */ s16 pattern;
} HighwayTrackCommand; // size: 0xC

typedef struct {
    /* 0x00 */ u8* propData;
    /* 0x04 */ HighwayPropScript* propScripts[10];
    /* 0x2C */ HighwayTrackCommand* trackCommands;
    /* 0x30 */ u8* unk30;
    /* 0x34 */ HighwayEvent* events;
} HighwayCourse; // size: 0x38

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

extern HighwayCourse g_HighwayCourses[2]; // [0] story, [1] Gold Saucer G-Bike (g_HighwayArcadeMode)
extern u_long* g_HighwayTimAddr[52];
extern u8 g_HighwayArcadeMode; // Savemap bank 2 [0x73]: Gold Saucer G-Bike, shows score instead of HP bars
extern s32 g_HighwaySegmentsCrossed;
extern u16 g_HighwayRoadTPage[11];
extern s32 g_HighwaySpeed;
extern u16 g_HighwayWallTPage[8];
extern u8 g_HighwayPropPatternCount;
extern u8* g_HighwayPropData;
extern s32 g_HighwayTrackRadius;
extern u16 D_800BD590;
extern VECTOR g_HighwayPropAnchors[10];
extern s16 g_HighwayPropRepeat[10];
extern u16 g_HighwaySpriteTPage[3];
extern u8* g_HighwayPropPatterns[256];
extern HighwayQuadUv g_HighwayWallUv[10];
extern u16 g_HighwayTrackCommandIndex;
extern HighwayRoadSegment g_HighwayRoad[80];
extern u16 D_800C4A98;
extern HighwayPropScript* g_HighwayPropCurrent;
extern HighwayTrackCommand* g_HighwayTrackCommand;
extern HighwayQuadUv g_HighwayNearUvLeft[8];
extern s32 g_HighwayTrackRollAcc;
extern HighwayQuadUv g_HighwayNearUvRight[8];
extern s32 g_HighwayTrackSegment; // newest segment; g_HighwayRoad is indexed modulo 80
extern s32 g_HighwayTrackRollStep;
extern u8 D_8010EBC8;
extern s32 g_HighwayTrackRollFrom;
extern s16 g_HighwayTrackCmdLeft;
extern u16 g_HighwayPropYaw[10];
extern HighwayQuadUv g_HighwayRoadUv[11];
extern u8 g_HighwayPropPatternLen[10];
extern u8 g_HighwayTrackNextCommand;
extern u8 g_HighwayPropNeedNext[10];
extern HighwayPropScript* g_HighwayPropScripts[10];
extern u16 D_8010FD7C;
extern s16 g_HighwayPropRecord[10];
extern u16 g_HighwayPropOffset[10];
extern VECTOR g_HighwayTrackGenPos;
extern s32 g_HighwaySegmentFrac; // 8.8 progress into the current segment
extern s16 g_HighwayTrackPattern;
extern HighwayQuadUv g_HighwayRoadUvHalves[40];
extern HighwayQuadUv g_HighwayRoadUvEighths[161];
extern u16 g_HighwayGaugeClut;
extern s32 g_HighwayTrackRollTo;
extern s32 g_HighwayTrackGenYaw;
extern u16 g_HighwayPropFlags[10];
extern s32 g_HighwayTrackPitchAcc;
extern u8* g_HighwayRoadPatternPtr;
extern HighwayScratchpad* g_HighwayScratchpad;
extern s32 g_HighwayTrackPitchStep;
extern u8* D_80110BC0;
extern u8 g_HighwayRoadPatternPos;
extern u8 g_HighwayRoadPatternLen;
extern u8 g_HighwayPropPatternPos[10];
extern s32 g_HighwayTrackPitchFrom;
extern u8 D_80110CC8;
extern u8 D_80110CCC;
extern u8 g_HighwayTrackTurn;
extern s32 g_HighwayDistance;
extern HighwayGaugeLayout g_HighwayGaugeLayout[5];
extern u16 g_HighwayGaugeTPage;
extern s32 g_HighwayRoadHead;
extern u16 g_HighwayRoadClut[11];
extern u8* g_HighwayRoadPatterns;
extern s32 D_801163EC;
extern s32 g_HighwayTrackTurnStep;
extern u16 g_HighwayWallClut[8];
extern u16 g_HighwayPropPattern[10];
extern u16 g_HighwayPropHeight[10];
extern s32 g_HighwayTrackPitchTo;
extern u16 D_80116674;
extern u16 g_HighwayTrackWidth;
extern s32 g_HighwayTrackPos; // g_HighwayDistance + 0x2300
extern u16 g_HighwaySpriteClut[3];

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

long csqrt(long a);

void HighwayTexturesInit(void);
void HighwayLoadTim(u_long* tim);
void HighwayRoadUvInit(void);
void HighwayQuadUvSplitV(HighwayQuadUv* src, HighwayQuadUv* top, HighwayQuadUv* bottom);
void HighwayQuadUvSplit8(HighwayQuadUv* src, s32 index);
void HighwayQuadUvSplitH(HighwayQuadUv* src, s32 index);
void HighwayTrackReset(void);
void HighwayTrackGenerateSegment(void);
void HighwayTrackFreeSegment(void);
void HighwayTrackSamplePos(s32 pos, s32 offset, VECTOR* out);
void HighwayTrackSample(s32 pos, s32 offset, VECTOR* out, SVECTOR* rot);
void HighwayTrackSampleNoRoll(s32 pos, s32 offset, VECTOR* out, SVECTOR* rot);
void HighwayTrackSpawnProps(void);
void HighwayPropsInit(void);
void HighwayPropScriptStep(u8 index, u8* modelId, s16* offset, s16* height, u16* flags, u16* yaw);
void HighwayPropPatternGet(u8 table, u8 index, u8* first, u8* second, u8* unused);
void HighwayTrackAdvance(void);

#endif
