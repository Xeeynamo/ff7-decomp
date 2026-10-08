#ifndef HIGHWAY_PRIVATE_H
#define HIGHWAY_PRIVATE_H

#include "types.h"
#include <game.h>
#include <inline_o.h>
#include <libetc.h>
#include "../jet/jet_model.h"

typedef struct HighwayBuffer HighwayBuffer;

#define ABS(x) ((x) < 0 ? -(x) : (x))

typedef union {
    s32 raw[0x50];
    struct {
        /* 0x00 */ s32 status; // 0 normal, 1 hit, 2 dying, 5 inactive
        /* 0x04 */ s32 type;   // 0 player, 1 truck, 2 enemy bike
        /* 0x08 */ s32 unk8;
        /* 0x0C */ s32 unkC;
        /* 0x10 */ s32 nodeCount;
        /* 0x14 */ s32 unk14;
        /* 0x18 */ s32 x;
        /* 0x1C */ s32 y;
        /* 0x20 */ s32 z;
        /* 0x24 */ char pad24[0x8];
        /* 0x2C */ s32 maxZ;
        /* 0x30 */ char pad30[0x8];
        /* 0x38 */ s32 minZ;
        /* 0x3C */ s32 unk3C;
        /* 0x40 */ s32 unk40;
        /* 0x44 */ s32 unk44;
        /* 0x48 */ s32 unk48[6];
        /* 0x60 */ s32 unk60;
        /* 0x64 */ s32 unk64;
        /* 0x68 */ s32 unk68;
        /* 0x6C */ s32 unk6C;
        /* 0x70 */ char pad70[0x8];
        /* 0x78 */ s32 unk78;
        /* 0x7C */ s32 unk7C;
        /* 0x80 */ s32 unk80;
        /* 0x84 */ s32 unk84;
        /* 0x88 */ s32 hp;
        /* 0x8C */ s32 maxHp;
        /* 0x90 */ s32 unk90;
        /* 0x94 */ char pad94[0xC];
        /* 0xA0 */ s32 unkA0;
        /* 0xA4 */ s32 velX;
        /* 0xA8 */ s32 unkA8;
        /* 0xAC */ s32 velZ;
        /* 0xB0 */ s32 unkB0;
        /* 0xB4 */ s32 unkB4;
        /* 0xB8 */ s32 unkB8;
        /* 0xBC */ s32 unkBC;
        /* 0xC0 */ s32 unkC0;
        /* 0xC4 */ s32 unkC4;
        /* 0xC8 */ s32 unkC8;
        /* 0xCC */ s32 onPath;
        /* 0xD0 */ SVECTOR* path;
        /* 0xD4 */ s32 pathEnd;
        /* 0xD8 */ s32 pathPos;
        /* 0xDC */ char padDC[0x14];
        /* 0xF0 */ s32 unkF0;
        /* 0xF4 */ s32 unkF4;
        /* 0xF8 */ s32 unkF8;
        /* 0xFC */ s32 unkFC;
        /* 0x100 */ s32 unk100;
        /* 0x104 */ s32 attackTimer;
        /* 0x108 */ s32 unk108;
        /* 0x10C */ s32 unk10C;
        /* 0x110 */ s32 unk110;
        /* 0x114 */ s32 unk114;
        /* 0x118 */ s32 unk118;
        /* 0x11C */ s32 unk11C;
        /* 0x120 */ char pad120[0xC];
        /* 0x12C */ s32 unk12C;
    } common;
} HighwayRiderState; // size: 0x140

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 : 32;
    /* 0x10 */ SVECTOR unk10;
    /* 0x18 */ JetNode* nodes[10];
    /* 0x40 */ HighwayRiderState state;
} HighwayRider; // size: 0x180

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
typedef struct {
    /* 0x00 */ MATRIX m;
    /* 0x20 */ s32 lastFrame[20]; // per animation
    /* 0x70 */ char pad70[0x28];
    /* 0x98 */ s32 unk98;
    /* 0x9C */ s32 unk9C;
    /* 0xA0 */ s32 animation;
    /* 0xA4 */ s32 frame;
    /* 0xA8 */ s32 unkA8;
    /* 0xAC */ s16 flash; // set to 0xFF when hit
    /* 0xAE */ s16 unkAE;
    /* 0xB0 */ s16 unkB0;
    /* 0xB2 */ s16 : 16;
} HighwayKawaiState; // size: 0xB4

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

extern VECTOR g_HighwayCameraEye;    // listener for engine volume/pan
extern VECTOR g_HighwayCameraTarget; // on the player; the view rotates around it
extern VECTOR g_HighwayCameraTargetOffset;
extern VECTOR g_HighwayCameraEyeOffset;
extern s32* g_HighwayPathLengths;
extern s32* g_HighwayPathOffsets;
extern u8* g_HighwayPathData;
extern HighwayRider g_HighwayRiders[6]; // [0] is the player
extern s32 g_HighwayInputDisabled;
extern s32 g_HighwayEffectModels[4];
extern SVECTOR* g_HighwayPath;
extern HighwayKawaiState g_HighwayKawaiStates[20];
extern s32 g_HighwayPathLen;
extern s32 g_HighwayOtOffset;
extern s32 g_HighwayPadKeys;
extern s32 g_HighwayNearestEnemy;
extern s32 g_HighwayNearestEnemyDist;
extern u8 g_HighwayEnemiesDisabled;
extern FieldModelEntry* g_HighwayKawaiModels;
extern s32 g_HighwayRidersTrackPos;
extern SVECTOR* g_HighwayCameraPath;
extern s32 g_HighwayPadDir;
extern u32 g_HighwayScore;
extern u8 g_HighwayEnemyCount;
extern s32 g_HighwayCameraPathEnd;
extern s32 g_HighwayPadAction;
extern s32 g_HighwayCameraPathStep;
extern u8 g_HighwayEnemySpawnDelay;
extern HighwayBuffer* g_HighwayRidersBuffer;
extern SVECTOR g_HighwayCameraFixedOffset;
extern u32 g_HighwayCameraPathPos;
extern VECTOR g_HighwayCameraOffset;
extern s32* D_800BE550;
extern s32 g_HighwayCameraLift;
extern s32 g_HighwayNearestRiderDist;
extern s32 g_HighwayCameraYaw;
extern u8 g_HighwayCameraMode;
extern s32 D_80110ABC;
extern s32 g_HighwayEngineVolume;
extern s32 g_HighwayEnemyEngineVolume;
extern s32 g_HighwayCameraFixedPos;
extern s32* D_801163F8;
void HighwayRidersInit(void);
void HighwayRidersUpdate(s32 trackPos, HighwayBuffer* db);
void HighwayRidersSetup(void);
void HighwayEnemyInit(s32 index);
void HighwayRidersClamp(void);
void HighwayRiderClamp(s32 index);
void HighwayEnemiesSpawn(void);
void HighwayEnemySpawn(s32 index);
void HighwayRidersMove(void);
void HighwayRiderMove(s32 index);
void HighwayEnemyMove(s32 index);
void HighwayRidersAction(void);
void HighwayRiderAction(s32 index);
void HighwayRidersDraw(void);
void HighwayRidersDrawEffects(void);
void HighwayRiderEndpoints(s16 index, SVECTOR* front, SVECTOR* back, s16* outDiff);
void HighwayRidersCollide(void);
void HighwayRiderCollision(s16 a, s16 b, s32 angle);
void HighwayRiderDamage(s32 index, s32 damage);
void HighwayTruckHit(s32 index, s32 angle);
void HighwayRiderSetPath(s32 index, u8 pathIndex);
void HighwayEnemyAi(s32 index);
void HighwayCameraInit(void);
void HighwayCameraSetPath(s32 pathIndex, u8 mode, s32 position);
void HighwayPathLoad(u8 pathIndex);
void HighwayPathSample(u32 pathPosition, SVECTOR* path, VECTOR* position, u8 flag);
void HighwayInputReset(void);
void HighwayPlaySfx(s32 soundId, s32 slot, s32 timer);
void HighwaySetSlotPitch(s32 pitch, s32 slot);
void HighwaySetSlotVolume(s32 volume, u8 slot);
void HighwayGaugeDamage(u8 index, s16 damage);
void HighwayKawaiModelsUpdate(void);
#endif
