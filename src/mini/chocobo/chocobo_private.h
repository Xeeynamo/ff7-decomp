#ifndef CHOCOBO_PRIVATE_H
#define CHOCOBO_PRIVATE_H
#include <game.h>

#define NUM_CHOCOBO 6

#define ChocoboPrizeTable(name, n, ...)                                                                                \
    struct {                                                                                                           \
        s32 count;                                                                                                     \
        ChocoboPrize prizes[n];                                                                                        \
    } name = {n, {__VA_ARGS__}}

typedef struct {
    /* 0x0 */ u32 flags; // bit 0 is set once the vertices got scaled
    /* 0x4 */ SVECTOR verts[1];
} ChocoboPartVerts;

typedef struct {
    /* 0x00 */ u8 unk0[0x2];
    /* 0x02 */ u8 nVerts;
    /* 0x03 */ u8 unk3[0x15];
    /* 0x18 */ ChocoboPartVerts* verts;
    /* 0x1C */ u8 unk1C[0x4];
} ChocoboModelPart; // size:0x20

typedef struct {
    /* 0x00 */ u16 nValues; // values per frame
    /* 0x02 */ u8 unk2;
    /* 0x03 */ u8 nFrames;
    /* 0x04 */ u8 nExtra;
    /* 0x05 */ u8 unk5;
    /* 0x06 */ u16 framesOffset;
    /* 0x08 */ u16 extraOffset;
    /* 0x0A */ u8 unkA[0x2];
    /* 0x0C */ s32 data; // the first word is set once the values got scaled
} ChocoboModelAnim;      // size:0x10

typedef struct {
    /* 0x00 */ u8 unk0[0x2];
    /* 0x02 */ u8 unk2;
    /* 0x03 */ u8 nParts;
    /* 0x04 */ u8 nAnims;
    /* 0x05 */ u8 r;
    /* 0x06 */ u8 g;
    /* 0x07 */ u8 b;
    /* 0x08 */ s32 x;
    /* 0x0C */ s32 y;
    /* 0x10 */ s32 z;
    /* 0x14 */ u8 unk14[0x2];
    /* 0x16 */ s16 unk16;
    /* 0x18 */ u16 partsOffset;
    /* 0x1A */ u16 animOffset;
    /* 0x1C */ u8* data;
    /* 0x20 */ u8* unk20;
} ChocoboModel; // size:0x24

typedef struct {
    /* 0x0 */ s32 unk0;
    /* 0x4 */ ChocoboModel* models;
} ChocoboModels;

typedef struct {
    /* 0x00 */ s16 unk0;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ s16 speed;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ u8 unk8[0x8];
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ u8 unk16[0x12];
    /* 0x28 */ s16 unk28;
    /* 0x2A */ s16 unk2A;
    /* 0x2C */ s16 unk2C;
    /* 0x2E */ u8 unk2E[0xC];
    /* 0x3A */ s16 unk3A;
    /* 0x3C */ u8 unk3C[0xC];
    /* 0x48 */ s16 unk48;
    /* 0x4A */ u8 unk4A[0x2];
    /* 0x4C */ s16 unk4C;
    /* 0x4E */ u8 unk4E[0x4];
    /* 0x52 */ s16 unk52;
    /* 0x54 */ u8 unk54[0xC];
    /* 0x60 */ s16 unk60;
    /* 0x62 */ u8 unk62[0x2];
    /* 0x64 */ s16 unk64;
    /* 0x66 */ u8 unk66[0x2];
    /* 0x68 */ s32 unk68;
    /* 0x6C */ s32 unk6C;
    /* 0x70 */ s16 unk70;
    /* 0x72 */ s16 unk72;
    /* 0x74 */ u8 unk74[0x8];
    /* 0x7C */ s16 unk7C;
    /* 0x7E */ s16 unk7E;
    /* 0x80 */ s16 unk80;
    /* 0x82 */ s16 rank;
    /* 0x84 */ u8 unk84[0x2];
    /* 0x86 */ s16 unk86;
    /* 0x88 */ u8 unk88[0x8];
    /* 0x90 */ s16 unk90;
    /* 0x92 */ s16 unk92;
    /* 0x94 */ s16 unk94;
    /* 0x96 */ u8 unk96[0x2];
    /* 0x98 */ s16 unk98;
    /* 0x9A */ s16 unk9A;
    /* 0x9C */ s16 unk9C;
    /* 0x9E */ u8 unk9E[0x2];
    /* 0xA0 */ s16 unkA0;
    /* 0xA2 */ s16 unkA2;
} Chocobo; // size:0xA4

typedef struct {
    /* 0x00 */ SVECTOR p0; // one edge of the track
    /* 0x08 */ SVECTOR p1; // the other edge of the track
    /* 0x10 */ u8 unk10;
    /* 0x11 */ u8 prop; // 1-based index of the ChocoboTrack prop placed on this segment, 0 for none
    /* 0x12 */ u8 unk12[0x2];
    /* 0x14 */ u8 r;
    /* 0x15 */ u8 g;
    /* 0x16 */ u8 b;
    /* 0x17 */ u8 flags;
} ChocoboTrackSegment; // size:0x18

typedef struct {
    /* 0x0 */ ChocoboModels* unk0;
} Unk800B1254;

typedef struct {
    /* 0x0 */ u8 strIndex;
    /* 0x1 */ u8 unk1;
    /* 0x2 */ u8 unk2;
    /* 0x3 */ u8 unk3;
} ChocoboPrize;

typedef struct {
    /* 0x00000 */ OT_TYPE ot[0x1000];
    /* 0x04000 */ u8 unk4000[0x10];
    /* 0x04010 */ POLY_FT4 prims[2507];
    /* 0x1C7C8 */ u8 unk1C7C8[0x20];
    /* 0x1C7E8 */ OT_TYPE ot2[4];
    /* 0x1C7F8 */ POLY_G3 polys[250];
    /* 0x1E350 */ TILE bg;
    /* 0x1E360 */ u8 unk1E360[0x8];
    /* 0x1E368 */ POLY_FT4 unk1E368[5][3];
    /* 0x1E5C0 */ POLY_FT4 unk1E5C0[5][3];
    /* 0x1E818 */ POLY_FT4 unk1E818[5][3];
    /* 0x1EA70 */ DRAWENV draw;
    /* 0x1EACC */ DISPENV disp;
} GpuBuffer; // size:0x1EAE0

typedef struct {
    /* 0x0 */ u8 index;
    /* 0x1 */ u8 unk1[3];
    /* 0x4 */ GpuBuffer* cur;
} GpuBufferPtr; // size:0x8

typedef struct {
    /* 0x0 */ s32 unk0;
    /* 0x4 */ u32 pressed;
    /* 0x8 */ s32 unk8;
    /* 0xC */ s32 unkC;
} Unk800B7530; // size:0x10

typedef struct {
    /* 0x00 */ s8 type;
    /* 0x01 */ s8 unk1;
    /* 0x02 */ u8 unk2[0x8];
    /* 0x0A */ s16 unkA;
    /* 0x0C */ u8 unkC[0x8];
} ChocoboTrackEvent; // size:0x14

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ ChocoboTrackEvent event;
    /* 0x28 */ s16 unk28;
    /* 0x2A */ s16 unk2A;
    /* 0x2C */ s32 unk2C;
    /* 0x30 */ s32 fadeSpeed; // added to D_800B7514 every frame
} Unk800F5040;                // size:0x34

typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
    /* 0x6 */ u16 flags; // bit 1 loops the animation
    /* 0x8 */ s16 step;
    /* 0xA */ s16 frame;
    /* 0xC */ u8 r;
    /* 0xD */ u8 g;
    /* 0xE */ u8 b;
    /* 0xF */ u8 model;
} ChocoboTrackNode; // size:0x10

typedef struct {
    /* 0x00 */ s32 count;
    /* 0x04 */ u8 unk4[0x4];
    /* 0x08 */ ChocoboTrackSegment* segments;
    /* 0x0C */ u8 unkC[0x18];
    /* 0x24 */ s16 nTris;
    /* 0x26 */ u8 unk26[0x2];
    /* 0x28 */ struct ChocoboTri* tris;
    /* 0x2C */ u8 unk2C[0xC];
    /* 0x38 */ ChocoboTrackEvent* events;
    /* 0x3C */ u8 unk3C[0x4];
    /* 0x40 */ ChocoboTrackNode* nodes;
} ChocoboTrack;

typedef struct ChocoboTri {
    /* 0x00 */ SVECTOR v[3];
    /* 0x18 */ u32 rgb[3]; // CVECTOR with the primitive code slot
} ChocoboTri;              // size:0x24

typedef struct {
    /* 0x00 */ s32 count;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ u32 unk8;
    /* 0x0C */ ChocoboTrack* track;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s16 unk24;
} Unk800F5078; // size:0x28

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ u8 unk4[0x18];
    /* 0x1C */ s32 unk1C;
} Unk800B7A48; // size:0x20

typedef struct {
    ChocoboPrize* entries;
} ChocoboPrizePtr;

typedef struct {
    /* 0x0 */ s16 unk0;
    /* 0x2 */ u8 unk2;
    /* 0x3 */ u8 unk3;
    /* 0x4 */ u8 unk4;
    /* 0x5 */ u8 unk5;
    /* 0x6 */ u8 unk6;
    /* 0x7 */ u8 unk7;
} Unk800B7480;

extern SVECTOR D_800A0000;
extern VECTOR D_800A0008;
extern VECTOR D_800A0068;
extern Unk800B1254 D_800B1254;
extern s32 D_800B7478;
extern s32 D_800B733C; // id of the last chocobo
extern ChocoboTrackSegment* D_800B7500;
extern s32 D_800B7514;     // screen fade level, 0 is fully visible and 0x100 is black
extern MATRIX D_800B7544;  // world to screen matrix
extern SVECTOR D_800B7340; // camera eye
extern SVECTOR D_800B7348; // camera target
extern SVECTOR D_800B1348; // eye position the camera moves towards
extern SVECTOR D_800B1350; // target position the camera moves towards
extern s32 D_800B7564[6];
extern Chocobo D_800B75CC[NUM_CHOCOBO + 1];
extern Unk800B7A48 D_800B7A48;
extern GpuBufferPtr D_800B66CC;
extern Unk800B7530 D_800B7530;
extern Unk800F5040 D_800F5040;
extern GpuBuffer* D_800F5074;
extern GpuBuffer D_800B7A68[2];
extern Unk800F5078 D_800F5078;
extern ChocoboModel* D_800F50A0[];
extern s32 D_800B7598;
extern s32 D_800B74F8;
extern s32 D_800B74FC;
extern s32 D_800B747C;
extern s32 D_800B7594;
extern s32 D_800B759C;
extern s32 D_800F5124;
extern s32 D_800F5034; // next free entry of GpuBuffer::prims
extern s32 D_800B745C[3];
extern s16 D_800F5028;
extern s32 D_800B1358;
extern POLY_F4 D_800B14B4;

void ChocoboResetRacerColors(void);
void ChocoboRaceInit(void);
void ChocoboInitMusic(void);
void func_800A18BC(void);
void ChocoboDrawTrackTris(void);
void ChocoboDrawTrackSegments(void);
void func_800A2BD4(s32 start, s32 end);
void func_800A34A8(void);
void ChocoboUpdateRanking(void);
void ChocoboSelectRacer(s32);
void func_800A4888(s32 id);
void ChocoboSelectRacerAtSegment(s32 id, s32 speed, s32 seg);
void func_800A6E50(s32);
void func_800A7CA4(void);
void func_800A8AE8(void);
void func_800A9D94(void);
void func_800AC554(void);
void ChocoboDrawText(const char* str, s32 len, s32 x, s32 y);
void ChocoboDrawFade(void);
u8* ChocoboModelSetupParts(ChocoboModel* model, u8* buf, s32 arg2);
u8* func_800AD9D8(ChocoboModelPart*, u8*, s32, s32);
void func_800AE534(ChocoboModel*);
void func_800AF11C(ChocoboModel*, MATRIX*, s32, s32);
void func_800AF9E4(ChocoboModel* model, s16 scale, s32 force);
void ChocoboScalePartVerts(ChocoboModelPart* part, s16 scale, s32 force);
void func_800AFDBC(ChocoboModelAnim* anim, s16 scale, s32 force);
s32 ChocoboModelApplyPartRotation(ChocoboModel* model, u8* data);
void func_800B01B0(ChocoboModelPart*, s32, s32, s32);

#endif
