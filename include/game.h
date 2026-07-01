#ifndef GAME_H
#define GAME_H

#include "psxsdk/libgpu.h"
#include <common.h>
#include <libgte.h>
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
    s32 loc; // disk sector where the file can be found
    s32 len; // file size in bytes
} Yamada;

typedef enum {
    LBA_INIT_YAMADA = 614,
    LBA_ENEMY6_SEFFECT = 30046,
    LBA_ENEMY6_OVER2 = 30694,
    LBA_ENEMY6_FAN2 = 30695,
} Lba;

typedef enum {
    FIELDOP_NONE,
    FIELDOP_FIELD_CHANGE,
    FIELDOP_ENTERING_BATTLE,
    FIELDOP_LOAD_MOVIE,
    FIELDOP_PLAY_MOVIE,
    FIELDOP_PLAY_ENDING_FMV,
    FIELDOP_CHAR_NAME_ENTRY,
    FIELDOP_PARTY_SELECT,
    FIELDOP_SHOP,
    FIELDOP_PARTY_MENU,
    FIELDOP_BEAT_GAME,
    FIELDOP_UNKB,
    FIELDOP_LOAD_MINIGAME,
    FIELDOP_CD_CHANGE,
    FIELDOP_SAVE_SCREEN,
    FIELDOP_YUFFIE_STEALS_MATERIA,
    FIELDOP_YUFFIE_RETURNS_MATERIA,
    FIELDOP_REMOVE_CHARS_MATERIA_ACCESSORY,
    FIELDOP_UNK12,
    FIELDOP_UNK13,
    FIELDOP_UNK14,
    FIELDOP_UNK15,
    FIELDOP_MASTER_MATERIA_CHECK,
    FIELDOP_ADD_MASTER_MATERIA,
    FIELDOP_JENOVA_SYNTH_COPY_LEVELS,
    FIELDOP_UNK19,
    FIELDOP_GAME_OVER,
} FieldOpcode;

typedef enum {
    SCRL_OFF,
    // Scroll to center camera on given entity's 3D model.
    // Immediately sets camera to target coordinates.
    SCRL_TO_ENTITY_INSTANT,
    // Constant movement speed determined by number of steps.
    SCRL_TO_ENTITY_LINEAR,
    // Uses a precalculated sine table for a smoother start and stop.
    SCRL_TO_ENTITY_SMOOTH,
    // Scroll to center camera on given coordinates.
    SCRL_TO_COORDS_INSTANT,
    SCRL_TO_COORDS_LINEAR,
    SCRL_TO_COORDS_SMOOTH,
} ScrollMode;

typedef enum {
    SCRLST_INIT,
    SCRLST_ACTIVE,
    SCRLST_DONE,
} ScrollState;

// https://wiki.ffrtt.ru/index.php/FF7/Field/Script/Opcodes/6B_FADE
typedef enum {
    FFT_INSTANT,
    FFT_INV4_TO_FIELD_SUB,
    FFT_FIELD_TO_INV4_SUB,
    // Fade to black when changing field map.
    FFT_SYS_FADE_TO_BLACK_FIELD_CHANGE,
    FFT_INSTANT_BLACK,
    FFT_STANDARD_TO_FIELD_ADD,
    FFT_FIELD_TO_STANDARD_ADD,
    FFT_INSTANT_INV1_SUB_HOLD_FIELD,
    FFT_INSTANT_INV1_SUB_HOLD_COLOR,
    FFT_INSTANT_STANDARD_ADD_HOLD_FIELD,
    FFT_INSTANT_STANDARD_ADD_HOLD_COLOR,
    FFT_FIELD_TO_STANDARD_ADD_HOLD_COLOR,
    FFT_FIELD_TO_STANDARD_SUB_HOLD_COLOR,
    // Fade to black when opening menu.
    FFT_SYS_FADE_TO_BLACK_MENU,
} FieldFadeType;

typedef enum {
    OMODE_INSTANT,
    OMODE_LINEAR,
    OMODE_SMOOTH,
    OMODE_DONE,
} OffsetMode;

typedef enum {
    MOVCMD_IDLE,
    MOVCMD_ACTIVE,
    MOVCMD_DONE,
} MovieCommandState;

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
    LINE_F2* linef2;
} Gpu;

typedef struct {
    u32 checksum;
    u8 leader_level;
    u8 leader_portrait;
    u8 portrait2;
    u8 portrait3;
    s8 leader_name[0x10];
    u16 leader_hp;
    u16 leader_hp_max;
    u16 leader_mp;
    u16 leader_mp_max;
    s32 gil;
    s32 time;
    s8 place_name[0x20];
    s32 menu_color[3];
} SaveHeader; // size: 0x54

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
    /* 0x40 */ s32 materia_weapon[8];
    /* 0x60 */ s32 materia_armor[8];
    /* 0x80 */ u8 unk80[4];
} SavePartyMember; // size:0x84
typedef struct {
    SaveHeader header;
    /* 0x54 */ SavePartyMember party[MAX_PARTY_COUNT];
    /* 0x4F8 */ u8 partyID[4];
    /* 0x4FC */ u16 inventory[MAX_INVENTORY_COUNT];
    /* 0x77C */ s32 materia[MAX_MATERIA_COUNT];
    /* 0xA9C */ s32 yuffie_stolen_materia[48];
    /* 0xB5C */ u8 unk_b5c[32];
    /* 0xB7C */ s32 gil;
    /* 0xB80 */ s32 time;
    /* 0xB84 */ s32 countdown_timer_seconds;
    /* 0xB88 */ s32 game_timer_fraction;
    /* 0xB8C */ s32 countdown_timer_fraction;
    /* 0xB90 */ s32 worldmap_exit_action;
    /* 0xB94 */ u16 current_module;
    /* 0xB96 */ u16 current_location_id;
    /* 0xB98 */ u16 padding2;
    /* 0xB9A */ s16 field_x;
    /* 0xB9C */ s16 field_y;
    /* 0xB9E */ u16 field_triangle;
    /* 0xBA0 */ u8 field_direction;
    /* 0xBA1 */ u8 step_id;
    /* 0xBA2 */ u8 step_offset;
    /* 0xBA3 */ u8 padding3;
    /* 0xBA4 */ u8 memory_bank_1[256];
    /* 0xCA4 */ u8 memory_bank_2[256];
    /* 0xDA4 */ u8 memory_bank_3[256];
    /* 0xEA4 */ u8 memory_bank_4[256];
    /* 0xFA4 */ u8 memory_bank_5[256];
    /* 0x10A4 */ u16 phs_locking_mask;
    /* 0x10A6 */ u16 phs_visibility_mask;
    /* 0x10A8 */ u8 unk_10a8[48];
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
    s32 unk248;
    s32 unk24C;
    s32 unk250;
    s32 unk254;
    s32 unk258;
    s32 unk25C;
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
    u8 un64[0x48];
    u8 unAC[4];
    u8 unB0[0x390];
} Unk8009D84C; // size: 0x440

typedef struct {
    /* 00 */ u16 unk0;
    /* 02 */ u16 unk2;
    /* 04 */ u16 unk4[6];
    /* 10 */ u8 unk10;
    /* 11 */ u8 unk11;
    /* 12 */ u16 unk12;
} Unk800730CC;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u32 unk4;
} Unk80062F7C;

typedef struct {
    /* 0x00 */ u16 unk0;
    /* 0x02 */ u16 unk2;
    /* 0x04 */ u16 unk4;
    /* 0x06 */ u16 unk6;
    /* 0x08 */ u16 unk8;
    /* 0x0A */ u16 unkA;
    /* 0x0C */ u16 unkC;
    /* 0x0E */ u16 unkE;
    /* 0x10 */ u16 unk10;
    /* 0x12 */ u16 unk12;
    /* 0x14 */ u8 unk14;
    /* 0x15 */ u8 unk15;
    /* 0x16 */ u8 unk16;
    /* 0x17 */ u8 unk17;
} Unk8007E7AC; // size:0x18

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s8 unk1C[0x68];
} Unk80074EA4; // size:0x84

typedef struct {
    u8 enabled;
    u8 segmentActive;
    u8 rngId;
    s8 currentOffset;
    s16 amplitude;
    s16 start;
    s16 target;
    s16 numStepsPerSegment;
    s16 currentStep;
} FieldShakeData; // size:0xE

typedef struct {
    u8 unk0;
    // enum FieldOpcode.
    u8 opcode;
    // Used by some opcodes to carry extra info, ie. shop id
    // for shop menu, char id for name entry screen.
    s16 opcodeParam;
    // Stores player position when exiting field or jumping between field maps.
    s16 pcPosX;
    s16 pcPosY;
    s16 pcPosZ;
    // Used by field script opcodes SCR2D, SCRLC, and SCRLA to set target
    // coordinates when scrolling camera.
    s16 cameraScrollTargetX;
    s16 cameraScrollTargetY;
    s16 cameraScrollTargetZ; // Unused.
    // Scale of current field map. Affects 3D model sizes, movement speed, and
    // collision and interaction radius.
    s16 currentFieldScale;
    // viewOffset* are set by VWOFT opcode which applies viewOffset to
    // player's Z axis when camera is not scrolling.
    u8 viewOffsetNumSteps;
    u8 viewOffsetCurrentStep;
    u8 viewOffsetMode; // enum OffsetMode.
    u8 unk15;
    u16 viewOffset;
    s16 viewOffsetStart;
    s16 viewOffsetTarget;
    u8 unk1C;
    u8 cameraScrollMode; // enum ScrollMode.
    u8 cameraScrollTargetId;
    u8 cameraScrollState; // enum ScrollState.
    u16 cameraScrollNumSteps;
    // Following two variables are set when exiting from field to mini games,
    // world map, or another field map.
    u16 pcWalkMeshId;      // Walk mesh triangle id player is inside of.
    u16 pcDirection;       // Direction player is facing.
    u16 movieCommandState; // enum MovieCommandState.
    u16 modelCount;
    s16 pcModelId;
    u16 idleAnimId;
    u16 walkAnimId;
    u16 runAnimId;
    u8 characterLock;
    u8 suspendWalkAndAnim;
    u8 menuDisabled; // Set by MENU2.
    u8 unk35;
    u8 mapJumpDisabled; // Set by MPJPO. Disables gateways to other maps.
    u8 scrlo;           // Set by SCRLO. Unused(?)
    // Set by MPDSP in field map junbin5. Also set to 1 if
    // fadeType == FFT_INSTANT_BLACK.
    u8 mpdsp;
    // Set by MVCAM. Static field map camera is used instead of dynamic movie
    // camera.
    u8 moviecamDisabled;
    // Set by BGMOVIE. Enables movie camera if moviecamDisabled is not set.
    // Increases movement speed.
    u8 backgroundMovieEnabled;
    // Set by BTLON to disable or enable random encounters.
    u8 battlesDisabled;
    // Set by BTLTB.
    // Each field map has two sets of encounters BTLTB can switch between.
    u8 encounterTableId;
    // Set by BTLMD and BTMD2.
    u8 battleMode1;
    u16 battleMode2;
    u16 unk40;
    u8 unk42;
    u8 unk43;
    u32 nextBattleMusic;
    u32 nextFieldMusic;
    // Set by FADE or NFADE to start fades.
    u16 fadeType; // enum FieldFadeType.
    s16 fadeAdjust;
    s16 fadeSpeed;
    s16 fadeRed;
    s16 fadeGreen;
    s16 fadeBlue;
    u16 nFadeRedStart;
    u16 nFadeGreenStart;
    u16 nFadeBlueStart;
    s16 nFadeRedTarget;
    s16 nFadeGreenTarget;
    s16 nFadeBlueTarget;
    u16 prevFieldId;
    u8 unk66;
    u8 unk67;
    // Uses PADx macros in libetc.h
    u32 activeKeys;      // Currently active keys.
    u32 oldActiveKeys;   // activeKeys from last frame.
    u32 newActiveKeys;   // Was inactive last frame.
    u32 newInactiveKeys; // Was active last frame.
    u32 activeKeys2;
    u32 oldActiveKeys2;
    u32 newActiveKeys2;
    u32 newInactiveKeys2;
    s16 currentMovieFrame;
    // Set by SHAKE to enable a randomized camera shake effect.
    FieldShakeData shakeX;
    FieldShakeData shakeY;
    // Set by BGSCR. Affects parallax effect on camera movements.
    u16 layer2_bgScrollXSpeed;
    u16 layer2_bgScrollYSpeed;
    u16 layer3_bgScrollXSpeed;
    u16 layer3_bgScrollYSpeed;
    // Can be overridden by BGPDH.
    u16 layer3_depth; // Default: 1.
    u16 layer2_depth; // Default: 4095.
    // Bit fields that define which walk mesh triangles
    // the player can't travel between. IDLCK can override the accesses.
    u8 blockedAccesses[64];
    // Bit fields. Set by BGON, BGOFF, BGCLR, BGROL, and BGROL2.
    u8 backgroundLayerVisibility[64];
    u16 pad;  // Necessary with 4 byte alignment?
} FieldState; // size:0x134

extern u8 D_80049208[12];   // window colors maybe??
extern u8 D_800492F0[][12]; // see Labels enum
extern u16 D_80062D78;      // pressed button?
extern u16 D_80062D7C;      // pressed button?
extern u16 D_80062D7E;      // pressed button?
extern u16 D_80062D80;      // tapped button
extern u16 D_80062D82;      // repeated button
extern u8 D_80062D98;
extern u8 D_80062D99;
extern s32 D_80062DCC;
extern u8 D_80062F19; // Enemy Lure/Away Modifier
extern u8 D_80062F1A;
extern u8 D_80062F1B;
extern Gpu D_80062F24;
extern u16 D_80062F3C;
extern s32 D_80062F58;
extern u_long* D_80062FC4;
extern Unk800A8D04* D_80063014;
extern DRAWENV D_800706A4[2];
extern u8 g_FieldMusicLock; // MUSIC/FMUSC skip the sound engine while nonzero
                            // (set by the MULCK opcode)
extern u8 D_80070788;
extern u16 D_800707BE;
extern s16 D_80071A5C;
extern s8 D_80071C08;
extern u8 D_80071E24;
extern u8 D_80071E2C;
extern u8 D_80071E30;
extern MATRIX* D_80071E40;
extern u8 D_80071E34;
extern u8 D_800722C4; // entity owning the currently executing script
extern Unk800730CC D_800730CC[];
extern u8 D_800730DD[][0x14];
extern Unk80074EA4 D_80074EA4[2];
extern s32 D_800756F8[];
extern int D_80075DEC; // buffer index, either 0 or 1
extern u8 D_80075E24[];
extern s8 D_80077F64[2][0x3400]; // polygon buffer
extern Unk8007E7AC D_8007E7AC[];
extern DRAWENV D_8007EAAC[2];
extern DISPENV D_8007EB68[2];
extern s8 D_8007EBCC;
extern s8 D_8007EBDC;
extern u8 D_8007EBE0; // field debug mode
extern u8 D_80083184[0x40];
extern u16 D_800831FC[48]; // program counters for active entity scripts
extern u8 D_8008326C;
extern s32 D_80083274;
extern s16 D_800832A0;
extern s32 D_80083338;
extern s8 D_80095DCC;
extern volatile u16 D_80095DD4;
extern s16 D_800965E0;
extern s16 D_800965E8;
extern volatile s16 D_800965EC;
extern u8 D_80099FFC;
extern s16 D_8009A000[1];
extern u32 D_8009A004[1];
extern s32 D_8009A008[1];
extern s32 D_8009A00C;
extern s32 D_8009A024[8];
extern u8 D_8009A058;
extern FieldState D_8009ABF4;
extern u8 D_8009AC2F;
extern u8* D_8009C6DC;
extern FieldState* D_8009C6E0; // points to 0x8009abf4
extern SaveWork Savemap;       // 0x8009C6E4
extern u8 D_8009CBDC[];
extern s16 D_8009D288[];
extern u8 D_8009D2E7;
extern u8 D_8009D302;
extern u8 D_8009D391[1]; // part of a struct?
extern u8 D_8009D40D;
extern u8 D_8009D684;
extern u8 D_8009D685;
extern u8 D_8009D686;
extern u8 D_8009D60E;
extern u8 D_8009D820; // field debug related
extern u8 D_8009D8F8[];
extern u32 D_8009D260;
extern volatile s32 D_8009D268[];
extern Unk8009D84C D_8009D84C[3];
extern s8 D_8009FE8C;

// PSXSDK funcs
SVECTOR* ApplyMatrixSV(MATRIX* m, SVECTOR* v0, SVECTOR* v1);
MATRIX* RotMatrixYXZ(SVECTOR* r, MATRIX* m);
void SystemError(char c, long n);

s32 func_80014B70(void);
s32 func_80014BA8(s32 arg0);
s32 func_8001521C(s32);
const char* func_80015248(s32 arg0, s32 arg1, s32 arg2);
void func_800155A4(s32);
void func_8001726C(s16, u16);
void func_80021044(DRAWENV* draw_env, DISPENV* disp_env);
void func_800262D8();
void func_80026448(Unk80026448* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4,
                   s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9, s32 arg10,
                   s32 arg11, s32 arg12, u16 arg13);
void func_800269C0(void* poly);
s32 func_80026B70(unsigned char* str);
void func_80026F44(s32 x, s32 y, const char*, s32 color); // print FF7 string
void func_8002DA7C();

int func_80033DAC(int sector_no, void (*cb)());
int func_80033DE4(int sector_no);
int func_80033E34(int sector_no, size_t size, u_long* dst, void (*cb)());
int DS_read(int sector_no, size_t size, u_long* dst, void (*cb)());
int func_80033EDC(int sector_no, void (*cb)());
int func_80033F40(int sector_no, size_t size, u_long* dst, void (*cb)());
int func_80033FC4(int sector_no, size_t size, u_long* dst, void (*cb)());
u32 func_80034B44(void);

// from overlays
extern s32 D_8019DAA0;
extern u_long* D_8019D5E8; // otag array

#endif
