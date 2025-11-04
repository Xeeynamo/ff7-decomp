#ifndef GAME_H
#define GAME_H

#include <common.h>
#include <libgpu.h>

#ifndef FF7_STR
#define _S(x) x       // check the usage of 'bin/str' to see how this works
#define _SL(len, x) x // same as _S, but for fixed-length strings with padding
#endif

#define MAX_PARTY_COUNT 9
#define MAX_INVENTORY_COUNT 320
#define MAX_MATERIA_COUNT 200

typedef unsigned char ff7s[];

typedef enum {
    LABEL_TIME,
    LABEL_GIL,
} Labels;

typedef enum {
    INIT_YAMADA,
    INIT_WINDOW,
    INIT_KERNEL,
    BATTLE_BROM,
    BATTLE_TITLE,
    BATTLE_BATTLE,
    BATTLE_BATINI,
    BATTLE_SCENE,
    BATTLE_BATRES,
    BATTLE_CO,
    YAMADA_FILE_NUM,
} YamadaFile;

typedef struct {
    s16 unk0;
    s16 unk2; // current page
    s16 unk4;
    s16 unk6; // total item count
    s16 unk8;
    s8 unkA;
    s8 unkB; // selected element
    s8 unkC;
    s8 unkD; // elements per page
    s8 unkE;
    s8 unkF;
    s8 unk10;
    s8 unk11;  // scroll type: 0=no wrap, 1/2:wrap, 3>:infinite
} Unk80026448; // size: 0x12

typedef union {
    void* poly;
    POLY_FT4* ft4;
    SPRT* sprt;
    TILE* tile;
    TILE_1* tile1;
    BLK_FILL* blk_fill;
} Gpu;

typedef struct {
    u32 checksum;
    u8 leader_level;
    u8 party[3];
    s8 leader_name[0x10];
    s32 unk18;
    s32 unk1C;
    s32 gil;
    s32 time;
    s8 place_name[0x20];
    s32 menu_color[3];
} SaveHeder; // size: 0x54

// partially inspired by Q-Gears 'VI. The Save game format'
typedef struct {
    u8 unk0;
    u8 level;
    u8 strength;
    u8 vitality;
    u8 magic;
    u8 spirit;
    u8 dexterity;
    u8 luck;
    u8 strength_bonus;
    u8 vitality_bonus;
    u8 magic_bonus;
    u8 spirit_bonus;
    u8 dexterity_bonus;
    u8 luck_bonus;
    u8 limit_level;
    u8 limit_charge;
    u8 name[12];
    u8 weapon;
    u8 armor;
    u8 accessory;
    u8 unk1F;
    u8 unk20;
    u8 unk21;
    u16 limit_learn;
    u16 kill_count;
    u16 limit_lv1_count;
    u16 limit_lv2_count;
    u16 limit_lv3_count;
    u16 hp_cur;
    u16 hp_base;
    u16 mp_cur;
    u16 mp_base;
    u32 unk34;
    u16 hp_max;
    u16 mp_max;
    u32 exp;
    u8 materia_weapon[8];
    u8 materia_armor[8];
    u8 unk50[0x34];
} SavePartyMember; // size:0x84
typedef struct {
    SaveHeder header;
    /* 0x54 */ SavePartyMember party[MAX_PARTY_COUNT];
    /* 0x4F8 */ u8 partyID[4];
    /* 0x4FC */ u16 inventory[MAX_INVENTORY_COUNT];
    /* 0x77C */ s32 materia[MAX_MATERIA_COUNT];
    /* 0xA9C */ u8 unkA9C[0xE0];
    /* 0xB7C */ s32 gil;
    /* 0xB80 */ s32 time;
    /* 0xB84 */ u8 D_8009D264[0x38];
    /* 0xBBC */ u16 D_8009D2A0;
    /* 0xBBE */ u8 D_8009D2A2;
    /* 0xBBF */ u8 D_8009D2A3;
    /* 0xBC0 */ u8 unkBC0[0x10];
    /* 0xBD0 */ u8 unkBD0[0x10];
    /* 0xBE0 */ u8 unkBE0[15];
    /* 0xBEF */ u8 unkBEF;
    /* 0xBF0 */ u8 unkBF0[0x420];
    /* 0x1010 */ u16 unk1010;
    /* 0x1012 */ u8 unk1012;
    /* 0x1013 */ u8 unk1013;
    /* 0x1014 */ u8 unk1014[0xC4];
    /* 0x10D8 */ u8 battle_speed;
    /* 0x10D9 */ u8 battle_msg_speed;
    /* 0x10DA */ u16 config;
    /* 0x10DC */ u8 button_config[16];
    /* 0x10EC */ u8 field_msg_speed;
    /* 0x10ED */ u8 D_8009D7D1;  // ??
    /* 0x10EE */ u16 D_8009D7D2; // ??
    /* 0x10F0 */ u32 D_8009D7D4;
} SaveWork; // size: 0x10F4

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    u8 unk34[4]; // character spacing array
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    s32 unk88;
    s32 unk8C;
    s32 unk90;
    s32 unk94;
    s32 unk98;
    s32 unk9C;
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    s32 unkAC;
    s32 unkB0;
    s32 unkB4;
    s32 unkB8;
    s32 unkBC;
    s32 unkC0;
    s32 unkC4;
    s32 unkC8;
    s32 unkCC;
    u8 unkD0[8];
    s32 unkD8;
    s32 unkDC;
    s32 unkE0;
    s32 unkE4;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    s32 unkF8;
    s32 unkFC;
    s32 unk100[0x40];
    s32 unk200;
    s32 unk204;
    s32 unk208;
    s32 unk20C;
    s32 unk210;
    s32 unk214;
    s32 unk218;
    s32 unk21C;
    s32 unk220;
    s32 unk224;
    s32 unk228;
    s32 unk22C;
    s32 unk230;
    s32 unk234;
    s32 unk238;
    s32 unk23C;
    s32 unk240;
    s32 unk244;
} Unk800A8D04; // size: ???

// seems to be related to a party member during battle
typedef struct {
    s32 unk0;
    s16 unk4;
    s8 unk6;
    s8 unk7;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    s32 unk18;
    s32 unk1C;
    s8 unk20;
    s8 unk21;
    s8 unk22;
    s8 unk23;
    u8 unk24[0x28];
    u8 un4C[4][6];
    u8 un64[0x3DC];
} Unk8009D84C; // size: 0x440

extern u8 D_80049208[12];   // window colors maybe??
extern u8 D_800492F0[][12]; // see Labels enum
extern u16 D_80062D78;      // pressed button?
extern u16 D_80062D7C;      // pressed button?
extern u16 D_80062D7E;      // pressed button?
extern u16 D_80062D80;      // tapped button
extern u16 D_80062D82;      // repeated button
extern s8 D_80062D99;
extern s32 D_80062DCC;
extern Gpu D_80062F24;
extern u16 D_80062F3C;
extern s32 D_80062F58;
extern u_long* D_80062FC4;
extern Unk800A8D04* D_80063014;
extern DRAWENV D_800706A4[2];
extern u8 D_800730DD[][0x14];
extern s32 D_800756F8[];
extern s8 D_80077F64[2][0x3400]; // polygon buffer
extern u8 D_80083184[0x40];
extern s16 D_8009A000;
extern s32 D_8009A004;
extern s32 D_8009A008;
extern s32 D_8009A024[8];
extern SaveWork _work; // 0x8009C6E4
extern Unk8009D84C D_8009D84C[3];

s32 func_80014B70(void);
s32 func_80014BA8(s32 arg0);
s32 func_8001521C(s32);
void func_800155A4(s32);
void func_8001726C(s16, u16);
void func_80021044(DRAWENV* draw_env, DISPENV* disp_env);
void func_80026448(Unk80026448* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4,
                   s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9, s32 arg10,
                   s32 arg11, s32 arg12, u16 arg13);
void func_800269C0(void* poly);
s32 func_80026B70(unsigned char* str);
s32 func_80034B44();

// from overlays
extern s32 D_8019DAA0;
extern u_long* D_8019D5E8; // otag array

#endif
