#ifndef WORLD_H
#define WORLD_H

#include <game.h>

#define ABS(x) ((x <= 0) ? -(x) : (x))

typedef struct {
    /* 0x00 */ u8 vert[3];
    /* 0x03 */ u8 walkabilityAndScript; // packed of 5 bits for walkabilty and 3
                                        // bits for script id
    /* 0x04 */ u8 u0;
    /* 0x05 */ u8 v0;
    /* 0x06 */ u8 u1;
    /* 0x07 */ u8 v1;
    /* 0x08 */ u8 u2;
    /* 0x09 */ u8 v2;
    /* 0x0A */ u16 textureAndLocationAndFlags; // packed 9 bits texture, 5 bits world area,
                                               // and some bits for some flags?
} WorldTriangle;                               // size: 0xC

typedef struct {
    /* 0x00 */ WorldTriangle* tri;
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 z;
} WorldStoredTriangle; // size

typedef struct WorldChunkHeader {
    /* 0x00 */ struct WorldChunkHeader* next;
    /* 0x04 */ WorldTriangle* tris;
    /* 0x08 */ SVECTOR* verts;
    /* 0x0C */ SVECTOR* norms;
    /* 0x10 */ s16 x;
    /* 0x12 */ s16 z;
    /* 0x14 */ s16 numTris;
    /* 0x16 */ s16 numVerts;
} WorldChunkHeader; // size: 0x18

typedef struct {
    /* 0x00 */ s16 scriptIdx;
    /* 0x02 */ u8 waitFrames;
    /* 0x03 */ u8 scriptPriority;
} WorldScriptFrame; // size: 0x4

typedef struct WorldActor {
    /* 0x00 */ struct WorldActor* next;
    /* 0x04 */ struct WorldActor* collide;
    /* 0x08 */ struct WorldActor* riding;
    /* 0x0C */ VECTOR pos;
    /* 0x1C */ VECTOR altPos;
    /* 0x2C */ WorldScriptFrame scriptStack[3]; // may be [4]?
    /* 0x38 */ s32 unk38;
    /* 0x3C */ u16 unk3C;
    /* 0x3E */ u16 unk3E;
    /* 0x40 */ s16 direction;
    /* 0x42 */ s16 unk42;
    /* 0x44 */ s16 yOffset;
    /* 0x46 */ s16 scriptIdx;
    /* 0x48 */ s16 unk48;
    /* 0x4A */ s16 walkmesh;
    /* 0x4C */ s16 facing;
    /* 0x4E */ s16 unk4E;
    /* 0x50 */ u8 actorType;
    /* 0x51 */ u8 flags1;
    /* 0x52 */ u8 scriptCallModel;
    /* 0x53 */ s8 unk53;
    /* 0x54 */ u8 scriptCallDepth;
    /* 0x55 */ u8 horizontalSpeed;
    /* 0x56 */ u8 waitFrames;
    /* 0x57 */ u8 scriptPriority;
    /* 0x58 */ u8 unk58;
    /* 0x59 */ s8 unk59[3];
    /* 0x5C */ s8 verticalSpeed;
    /* 0x5D */ s8 animId;
    /* 0x5E */ s8 unk5E;
    /* 0x5F */ s8 unk5F;
    /* 0x60 */ WorldStoredTriangle storedTris[6];
    /* 0x90 */ u8 unk90[0x50];
} WorldActor; // size: 0xE0

typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 z;
    /* 0x04 */ s16 x2;
    /* 0x06 */ s16 z2;
} WorldZolomSegment; // size: 0x8

typedef struct {
    /* 0x00 */ u8 unk00[0x13];
    /* 0x13 */ u8 unk13;
    /* 0x14 */ u8 unk14[0x10];
} Unk8010B178; // size: 0x24

typedef struct {
    /* 0x00 */ VECTOR unk0;
    /* 0x10 */ u8 unk10;
    /* 0x11 */ u8 unk11;
    /* 0x12 */ u8 unk12;
    /* 0x13 */ u8 unk13;
    /* 0x14 */ u8 unk14;
    /* 0x15 */ u8 unk15;
    /* 0x16 */ u8 unk16;
    /* 0x17 */ u8 unk17;
    /* 0x18 */ u8 unk18;
    /* 0x19 */ u8 unk19;
    /* 0x1A */ u8 unk1A;
    /* 0x1B */ u8 unk1B;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ s32 unk20;
} Unk8010B3B8; // size:???

// entry of binary search table to find world map scripts
typedef struct {
    /* 0x00 */ u16 unk0;
    /* 0x02 */ u16 unk2;
} WorldScriptBSTEntry; // size: 4

typedef struct {
    /* 0x000 */ WorldScriptBSTEntry bst[0x100];
    /* 0x400 */ u16 scr[0x3600];
} WorldScriptData;

typedef struct {
    /* 0x0 */ s32 unk0;
    /* 0x4 */ u16 unk4;
    /* 0x6 */ s16 unk6;
} Unk8010AD70;

void WmSetRenderBuffers();
void func_800A19FC(WorldChunkHeader*, SVECTOR*, WorldStoredTriangle*, s16*, s32, s16*, s32);
void func_800A31C0(s16);
void func_800A368C(s32);
void WmExtractLoopCoordsTopBottomParts(VECTOR*, SVECTOR*, s16*, s16*);
void WmLoopCoordsAroundWorld(VECTOR*);
void func_800A6994(VECTOR*, s32);
void WmCleanEntityStruct(WorldActor*);
void WmUnlinkEntityFromAll(WorldActor*);
void WmInsertStructInEntityStructList(WorldActor*);
void WmLinkPcToActiveEntity();
void WmUnlinkPcEntityFromAll();
void WmUnlinkPcLinkedEntityFromAll();
void WmSetActiveEntityAsPcEntity();
s32 WmGetModelIdFromActiveEntity();
s32 WmGetModelIdFromPcEntity();
s32 func_800A929C();
s16 func_800A97A8(void);
void func_800A98A4(s32);
s32 WmIsPcEntityPosNeedRecalculation(void);
s32 func_800A99BC();
s32 WmGetPcEntityTerrainId(void);
s32 func_800A9B04(s16, u8);
void func_800A9C64(WorldActor*, VECTOR*);
s32 func_800AA304(WorldActor*, WorldActor*);
s32 func_800AA580(WorldActor*);
void func_800AAB18(WorldActor*);
void func_800AB398(WorldActor*);
void func_800AB48C(WorldActor*);
void WmScriptDisableForPcEntity(s32);
void func_800ABA18(s32);
void WmScriptOpcode000Handle(u16);
void WmScriptOpcode100Handle(u16);
s32 WmScriptOpcode200Handle(u16);
s32 WmScriptOpcode300Handle(u16);
void WmScriptRunOne(WorldActor*);
void func_800AD970(WorldActor*);
void func_800ADD4C(s16);
s16 func_800AE180(s32, s32, s32);
void func_800B0D98(WorldChunkHeader*);
void func_800B1C80(WorldChunkHeader*);
void func_800B5274();
void func_800B5C7C(WorldActor*);
void func_800B624C(u16, s32);
void func_800B63F0(s32);
void func_800B65E0(s32);
void func_800B6B28(s16);
void func_800B6E08();
s32 func_800B7200();
void func_800B7714(s32);
void func_800B77A8(s32);
s32 func_800B79B8();
static void func_800B7C44(void);
s32 func_800B7C7C();
void func_800B7838();
s16 func_800B86C4();
void func_800B8760();
void func_800BB9A0(u8);
static void func_800BBA5C(void);
s32 func_800BBBB0(void);
static void func_800BBD0C(void);

extern u32* D_800BD130;
extern s32 D_800BD144;
extern u16 D_800BD9E8[16][4][16]; // world map encounter data, size: 0x800
extern s32 D_800BE1E8[1];         // TODO: size unknown
extern s32 D_800C65EC;
extern s32 D_800C6628;
extern s32 D_800C6638;
extern u8 D_800C6770[1]; // TODO: size unknown
extern s16 D_800C68EE;
extern s16 D_800C6902;
extern s16 D_800C6916;
extern u8 D_800C72B4[16][4]; // size: 0x40
extern u8 D_800C72F4[16];    // yuffie spawn chances per area, size: 0x10
extern s8 D_800C752D;
extern u32* D_800C7530;
extern s32 D_800D05E8;
extern WorldScriptData D_800D05EC;
extern s32 D_800E55EC;
extern s32 D_800E55F0;
extern s32 D_800E55F4;
extern s32 D_800E55FC;
extern s32 D_800E5600;
extern s32 D_800E5604;
extern s32 D_800E5608;
extern s32 D_800E560C;
extern s32 D_800E5618;
extern s32 D_800E561C;
extern s32 D_800E5620;
extern s32 D_800E5624;
extern s32 D_800E5628;
extern s32 D_800E5630; // WM earthquake
extern s32 D_800E5634;
extern s32 D_800E5638;
extern s32 D_800E563C;
extern s32 D_800E5644;
extern s32 D_800E5648;
extern s32 D_800E564C;
extern s32 D_800E5650;
extern s32 D_800E5654;
extern s32 D_800E5658;
extern s32 D_800E5668;
extern s32 D_800E566C;
extern s32 D_800E5670;
extern s32 D_800E5674;
extern s32 D_800E5678;
extern MATRIX D_800E5698;
extern MATRIX D_800E56B8;
extern s16 D_800E56D8;
extern s32 D_800E56F4;
extern s32 D_800E56F8;
extern s32 D_800E5814;
extern s32 D_800E5820;
extern s32 D_800E5824;
extern s32 D_800E5828;
extern s32 D_800E5A34;
extern VECTOR D_80109D44;
extern s32 D_80109D54;
extern s32 D_80109D58;
extern s32 D_80109D6C;
extern WorldActor D_80109D74[0x10]; // World map actor heap, TODO: Confirm size
extern WorldActor D_80109E54;
extern WorldActor* D_8010AD34;
extern WorldActor* D_8010AD38;
extern WorldActor* D_8010AD3C; // Active Actor
extern WorldActor* D_8010AD40; // Player Actor
extern WorldActor* D_8010ADE4; // World current script context object?
// 8010ADF4 appears to maybe only be read from in an unused world script opcode
extern s32 D_8010ADF4;
extern s32 D_8010ADE8;
extern s16 D_8010AD44;
extern s16 D_8010AD48;
extern s16 D_8010AD4C;
extern u16 D_8010AD54; // possibly a svec?
extern u16 D_8010AD58;
extern s32 D_8010AD5C;
extern WorldScriptData* D_8010AD68;
extern u16* D_8010AD6C;
extern Unk8010AD70 D_8010AD70[1]; // todo: size
extern Unk8010AD70* D_8010AD90;
extern u8* D_8010AD94[4];
extern s32 D_8010ADEC;
extern s32 D_8010AE24;
extern s32 D_8010AE28;
extern s32 D_8010AE2C;
extern s32 D_8010AE30;
extern VECTOR D_8010AE34;
extern s32 D_8010AE54;
extern s32 D_8010AE58;     // WM RNG index
extern u8 D_8010AE5C[521]; // WM RNG Buffer
extern u8 D_8010B068[1];   // TODO: size unknown
extern s32 D_8010B080;
extern s32 D_8010B174;
extern Unk8010B178 D_8010B178[1]; // TODO: determine size
extern Unk8010B3B8* D_8010B3B8;
extern s32 D_8010B47C;
extern WorldZolomSegment D_8010C2AC[0x30];
extern WorldZolomSegment* D_8010C42C;
extern s16 D_8010C7F0;
extern s32 D_8010C804;
extern s32 D_8010C808;
extern s32 D_8010CA1C;
extern s32 D_8010CA20;
extern s32 D_8010CA74;
extern s32 D_8010CA78;
extern s32 D_8010CA8C;
extern s32 D_8010CAC0;
extern s32 D_8010CAC4;
extern s32 D_8010CAC8;
extern s32 D_8010CACC;
extern s32 D_8010CAD0;
extern s32 D_8010CAD4;
extern s32 D_8010CAF0;
extern s32 D_8010CAF4;
extern s16 D_8010CAFC;
extern s16 D_8010CB00;
extern s16 D_8010CB04;
extern s16 D_8010CB08;
extern s16 D_8010CB0C;
extern s16 D_8010CB10;
extern u32 D_8010CB14;
extern s32 D_8010CB18;
extern s32 D_8010CB1C;
extern s32 D_801159DC;
extern s32 D_801159E0;
extern s32 D_80115A50;
extern s32 D_80115A58;
extern s32 D_80115A60;
extern s32 D_80115A64;
extern s32 D_80115A68;
typedef struct WorldListNode {
    /* 0x0 */ struct WorldListNode* next;
    /* 0x4 */ s16 unk4;
} WorldListNode;

extern WorldListNode* D_800E5768;
extern s16 D_800BE5F0[];
extern s32 D_8010B488[];
extern s32 D_8010B494[];
extern s32 D_8010B4A0[];
extern u8 D_8010B18B[];
extern u8 D_800C8564[];
extern u8 D_800CC564[];
extern u8* D_800D05E4;
extern u32* D_800D05DC;
extern u32* D_800D05E0;
extern u8 D_800C752C;
extern u8* D_80109D60;
extern u8* D_80109D5C;
extern u8 D_800BF5F0[];
extern s16 D_8009ABF6;
extern s16 D_8009ABF8;
extern s16 D_8009ABFA;
extern s16 D_8009AC16;
extern s16 D_8009AC18;
extern u8* D_8009C6DC;
extern s32 D_800E5810;
extern void* D_800E580C;
extern u16 D_80083278[];
extern u16 D_8008327A[];
extern s16 D_80083286[];
extern u8 D_80115A14[];
extern s8 D_801159E8[];
extern u8 D_8010B17C[];
extern u8 D_8008328D[];
extern s16 D_800832A2[];
extern s32 D_80109D64;
extern s32 D_80109D68;
extern u8 D_8010D9B8[];
extern u8 D_8010D9BA[];
extern s32 D_80116274;
extern s16 D_80116288;
extern s16 D_8011628C;
extern s32 D_8011626C;
extern s32 D_80116270;
extern s32 D_80116278;
extern s32 D_8011627C;
extern s32 D_80116280;
extern s32 D_80116284; // World Danger
extern u8* D_80116298;
extern s16 D_8011629C[1];
extern s16 D_801162A0[1];
extern s16 D_801162A4[1];
extern s16 D_801162A8[1];
extern s16 D_801162AC[1];
extern u8* D_801162B0[1];
extern u8 D_801162B4[1][256];
extern u8 D_801163B4[1][4];
extern u16 D_801163B8[1][4];
extern s16 D_801163C0[1];
extern s16 D_801163C4[1];
extern u8 D_801163C8[1][8];
extern s16 D_801163D0[1];
extern s32 D_801163D8;
extern s32 D_801163DC;
extern s8 D_801163E0;
extern s8* D_801163E8;
extern s32 D_801163D4;
extern s32 D_801163EC;
extern s32 D_801164F8;
extern s32 D_801164FC;
extern s32 D_8011650C;

#endif
