#include <game.h>

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s8 unkC;
    u8 unkD;
    s8 unkE;
    s8 unkF;
    s8 unk10;
    s8 unk11;
    s16 unk12;
    s8 unk14;
    s8 unk15;
    s16 unk16;
    s32 unk18;
    s32 unk1C;
    s16 unk20;
    s16 unk22;
    s32 unk24;
    s16 unk28;
    s16 unk2A;
    s16 unk2C;
    s16 unk2E;
    s32 unk30;
    u32 unk34[4];
    u32 unk44[9];
} Unk800F83E0; // size:0x68
typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    s32 unkC;
} UnkStruct; // size:0x10
typedef struct {
    s16 unk0;
    u8 unk2;
    u8 unk3;
    /* 0x04 */ u8 unk4[0x30];
    /* 0x34 */ UnkStruct unk34[6];
    /* 0x94 */ u8 unk94[6][0x10];
} Unk801B2308;

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/Enemy.cs
typedef struct {
    /* 0x00 */ u8 name[0x20];
    /* 0x20 */ u8 level;
    u8 speed;
    u8 luck;
    u8 evade;
    u8 strength;
    u8 defense;
    u8 magic;
    u8 magicDef;
    /* 0x28 */ u8 resist[8];
    /* 0x30 */ u8 elemResist[8];
    /* 0x38 */ u8 anim[16];
    /* 0x48 */ u8 attackID[16];
    /* 0x58 */ u8 cameraID[16];
    /* 0x68 */ u8 dropItem[4];
    /* 0x6C */ u16 dropItemCount[4];
    /* 0x74 */ u16 manipAttackID[4];
    /* 0x7C */ u16 mp;
    /* 0x7E */ u16 ap;
    /* 0x80 */ u16 morphItem;
    /* 0x82 */ u8 backAttackMul;
    /* 0x83 */ u8 unk83;
    /* 0x84 */ s32 exp;
    /* 0x88 */ s32 gil;
    /* 0x8C */ s32 statusImmunities;
    /* 0x90 */ u32 unk90[10];
} Unk800F5F44_1; // size:0xB8
typedef struct {
    u8 unk0[0x380];
} Unk800F5F44_2; // size:0x380
typedef struct {
    u8 unk0[0x40];
} Unk800F5F44_3; // size:0x40
typedef struct {
    u8 unk0[0x400];
} Unk800F5F44_4; // size:0x400
typedef struct {
    u8 unk0[0x200];
} Unk800F5F44_5; // size:0x200
typedef struct {
    u8 unk0[0x1000];
} Unk800F5F44_6; // size:0x1000
typedef struct {
    /* 0x0000 */ Unk800F5F44_1 enemy[3];
    /* 0x0228 */ Unk800F5F44_2 _2;
    /* 0x05A8 */ Unk800F5F44_3 _3;
    /* 0x05E8 */ Unk800F5F44_4 _4;
    /* 0x09E8 */ u8 a[0x278];
    /* 0x0C60 */ Unk800F5F44_5 _5;
    /* 0x0E60 */ Unk800F5F44_6 _6;
    /* 0x1E60 */ u16 D_800F7DA4;
    /* 0x1E62 */ u16 D_800F7DA6;
    /* 0x1E64 */ u16 D_800F7DA8;
    /* 0x1E66 */ u16 D_800F7DAA;
    /* 0x1E68 */ u16 D_800F7DAC;
    /* 0x1E6A */ u16 D_800F7DAE;
    /* 0x1E6C */ u16 D_800F7DB0;
    /* 0x1E6E */ u16 D_800F7DB2;
    /* 0x1E70 */ u16 D_800F7DB4;
    /* 0x1E72 */ u16 D_800F7DB6;
    /* 0x1E74 */ u16 D_800F7DB8;
    /* 0x1E76 */ u16 D_800F7DBA;
    /* 0x1E78 */ u16 D_800F7DBC;
    /* 0x1E7A */ u16 D_800F7DBE;
    /* 0x1E7B */ u16 D_800F7DC0;
    /* 0x1E7E */ u16 D_800F7DC2;
    /* 0x1E80 */ u16 D_800F7DC4;
    /* 0x1E72 */ u16 D_800F7DC6;
    /* 0x1E84 */ u16 D_800F7DC8;
    /* 0x1E84 */ u16 D_800F7DCA;
} Unk800F5F44; // size:0x1E88

typedef struct {
    s16 D_80163614;
    s16 D_80163616;
    s16 D_80163618;
    s16 D_8016361A;
    s32 D_8016361C;
    s32 D_80163620;
    u16 D_80163624;
    u8 D_80163626;
    s8 D_80163627;
} Unk8016360C_1; // size:0x14
typedef struct {
    u8 unk0[0x30];
} Unk8016360C_2; // size:0x30
typedef struct {
    u8 unk0[0x60];
} Unk8016360C_3; // size:0x60
typedef struct {
    u16 unk0[4];
    Unk8016360C_1 unk8;
    Unk8016360C_2 unk1C;
    Unk8016360C_3 unk4C;
} Unk8016360C; // size:0xAC
typedef struct {
    /* 0x000 */ u16 unk0[4];
    /* 0x008 */ Unk8016360C_1 unk8[4];
    /* 0x058 */ Unk8016360C_2 unk58[4];
    /* 0x118 */ Unk8016360C_3 unk118[4];
    /* 0x298 */ Unk800F5F44_1 sp3A8[3];
    /* 0x4C0 */ Unk800F5F44_2 sp5D0;
    /* 0x840 */ Unk800F5F44_3 sp950;
    /* 0x880 */ Unk800F5F44_4 sp990;
    /* 0xC80 */ Unk800F5F44_5 spD90;
    /* 0xE80 */ Unk800F5F44_6 spF90;
} UnkSceneStuff; // 0x1E80
#define BATTLE_SCENE 7

typedef struct {
    /* 0x000 */ u8 unk0[0x12];
    /* 0x012 */ s16 unk12;
    /* 0x014 */ u8 unk14[0x4];
    /* 0x018 */ s32 D_801518FC;
    /* 0x01C */ s32 D_80151900;
    /* 0x020 */ s16 D_80151904;
    /* 0x022 */ u8 D_80151906;
    /* 0x023 */ u8 D_80151907;
    /* 0x024 */ s8 D_80151908;
    /* 0x025 */ s8 D_80151909;
    /* 0x026 */ s8 D_8015190A;
    /* 0x027 */ s8 D_8015190B;
    /* 0x028 */ s16 D_8015190C;
    /* 0x02A */ s8 D_8015190E;
    /* 0x02B */ s8 D_8015190F;
    /* 0x02C */ s32 D_80151910;
    /* 0x030 */ s32 D_80151914;
    /* 0x034 */ s32 D_80151918;
    /* 0x038 */ s32 D_8015191C;
    /* 0x03C */ s32 D_80151920;
    /* 0x040 */ s32 D_80151924;
    /* 0x044 */ u8 unk44[0x11C];
    /* 0x160 */ SVECTOR unk160;
    /* 0x168 */ u8 unk168[0xA34];
} BattleModel; // size:0xB9C

extern u16 D_800F5BBC[10][0x22];
extern u8 D_800F5E66[3][0x34];
extern Unk800F5F44 D_800F5F44;
extern s8 D_800F6936[0x40][8];
extern u8 D_800F83A8;
extern s16 D_800F83AE[10][0x34]; // overlaps with D_800F83E0
extern s16 D_800F83CC;           // overlaps with D_800F83AE, sceneID
extern Unk800F83E0 D_800F83E0[3];
extern Unk8016360C D_8016360C;
extern u16 D_8016376A;
extern BattleModel D_801518E4[3];
