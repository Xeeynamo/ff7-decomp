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
    SYNC_NONE = 0x0,
    SYNC_WAITING = 0x1,
    SYNC_DONE = 0x2,
} ScriptSyncState;

// Script controlled movement modes.
typedef enum {
    SMODE_NONE = 0x0,
    SMODE_WALK = 0x1,
    SMODE_JUMP = 0x3,
    SMODE_LADDER_V = 0x4,
    SMODE_LADDER_H = 0x5,
} ScriptedMoveMode;

typedef enum {
    EVTCMD_NONE,
    EVTCMD_FIELD_MAP_CHANGE,
    EVTCMD_ENTERING_BATTLE,
    EVTCMD_LOAD_MOVIE,
    EVTCMD_PLAY_MOVIE,
    EVTCMD_PLAY_ENDING_FMV,
    EVTCMD_CHAR_NAME_ENTRY,
    EVTCMD_PARTY_SELECT,
    EVTCMD_SHOP,
    EVTCMD_PARTY_MENU,
    EVTCMD_BEAT_GAME,
    EVTCMD_UNKB,
    EVTCMD_LOAD_MINIGAME,
    EVTCMD_CD_CHANGE,
    EVTCMD_SAVE_SCREEN,
    EVTCMD_YUFFIE_STEALS_MATERIA,
    EVTCMD_YUFFIE_RETURNS_MATERIA,
    EVTCMD_REMOVE_CHARS_MATERIA_ACCESSORY,
    EVTCMD_UNK12,
    EVTCMD_UNK13,
    EVTCMD_UNK14,
    EVTCMD_UNK15,
    EVTCMD_MASTER_MATERIA_CHECK,
    EVTCMD_ADD_MASTER_MATERIA,
    EVTCMD_JENOVA_SYNTH_COPY_LEVELS,
    EVTCMD_UNK19,
    EVTCMD_GAME_OVER,
} FieldEventCmd;

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

typedef enum {
    WSTYLE_NORMAL,
    WSTYLE_BACKGROUND_BORDER_OFF,
    WSTYLE_TRANS_BACKGROUND,
} WindowStyle;

typedef enum {
    WNDT_OFF,
    WNDT_CLOCK,
    WNDT_NUMERICAL,
} WindowNumDispType;

typedef enum {
    WSTATE_INIT,
    WSTATE_SHOW,
    WSTATE_TXT,
    WSTATE_PAUSE_TXT,
    WSTATE_WAIT_ROW,
    WSTATE_UNK5,
    WSTATE_TXT_DONE,
    WSTATE_CLOSING,
    WSTATE_SCROLL_ROW,
    WSTATE_INIT_NEXT,
    WSTATE_UNKA,
    WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK,
    WSTATE_SCROLL_TXT_WHILE_OK,
    WSTATE_PAUSE_TXT_UNTIL_OK,
    WSTATE_WAIT_NEXT_WINDOW,
} WindowState;

typedef struct {
    s16 x1;
    s16 y1;
    s16 z1;
    s16 x2;
    s16 y2;
    s16 z2;
} LinePos;

typedef struct {
    s16 unk0;
    s16 rowOffset; // Top visible row on the page of a scrollable table.
    s16 unk4;
    s16 numTotalRows; // Total rows in table.
    s16 scrolling;
    s8 column; // Selected column.
    s8 row;    // Selected row.
    s8 numColumns;
    s8 numRowsPerPage; // Visible rows per page of table.
    s8 unkE;           // Scrolling offset?
    s8 unkF;           // Horizontal wrap behaviour?
    s8 unk10;          // Vertical wrap behaviour?
    s8 unk11;          // scroll type: 0=no wrap, 1/2:wrap, 3>:infinite
} MenuTable;           // size: 0x12

typedef struct {
    s16 id;
    s16 quantity;
    s16 enabled;
} BattleItemReward; // size: 0x6

typedef struct {
    s32 xpNextLevel;
    s32 xp;
    u8 levelProgressBar;
    u8 level;
    s16 newLimitBreaks;
} CharacterLevelData; // size: 0xC

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
    u8 party_portraits[3]; // lead, 2nd and 3rd character ids, 0xFF = empty
    s8 leader_name[0x10];
    u16 leader_hp;
    u16 leader_hp_max;
    u16 leader_mp;
    u16 leader_mp_max;
    s32 gil;
    s32 time;
    s8 place_name[0x20];
    u8 menu_color[12]; // 4 corners x RGB
} SaveHeader;          // size: 0x54

// partially inspired by Q-Gears 'VI. The Save game format'
typedef struct {
    u8 char_id;
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
    u8 status_flags; // Status effects that remain after battle. 0x10 = Sadness,
                     // 0x20 = Fury.
    u8 order;        // 0xFF = front row, 0xFE = back row
    u8 level_progress_bar; // ui related
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
    /* 0x40 */ u32 materia_weapon[8];
    /* 0x60 */ u32 materia_armor[8];
    /* 0x80 */ u32 exp_to_next_level;
} SavePartyMember; // size:0x84

// https://ff7-mods.github.io/ff7-flat-wiki/FF7/Savemap
typedef struct {
    SaveHeader header;
    /* 0x54 */ SavePartyMember party[MAX_PARTY_COUNT];
    /* 0x4F8 */ u8 partyID[4];
    /* 0x4FC */ u16 inventory[MAX_INVENTORY_COUNT];
    /* 0x77C */ s32 materia[MAX_MATERIA_COUNT];
    /* 0xA9C */ s32 yuffie_stolen_materia[48];
    /* 0xB5C */ u8 unk_b5c[32];
    /* 0xB7C */ s32 gil;
    /* 0xB80 */ volatile u32 time;
    /* 0xB84 */ volatile u32 countdown_timer_seconds;
    /* 0xB88 */ volatile u32 game_timer_fraction;
    /* 0xB8C */ volatile u32 countdown_timer_fraction;
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
    s32 actorId;
    s32 characterLevel;
    s32 unk8;
    s32 unkC;
    s32 relativeActionIndex; // index within its own category (spell #, summon
                             // #, etc) -- see D_800A0290 in battle.c
    s32 unk14;
    s32 allowedTargetsMask;
    s32 unk1C;
    s32 unk20; // pending message/animation id, -1 = none
    s32 unk24;
    s32 unk28;
    s32 absoluteActionIndex; // relativeActionIndex remapped into the single
                             // shared spell/summon/enemy-skill/limit name
                             // table (kernel.bin section 18) via
                             // D_800A0290's per-category base offset
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
    void* unk200;
    void* unk204;
    s32 unk208;
    s32 unk20C;
    s32 unk210;
    s32 unk214;
    s32 unk218;
    s32 unk21C;
    s32 unk220;
    s32 unk224;
    u32 unk228;
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

typedef struct {
    u8 id;
    u8 mpCost;
    u8 quadraAttacksLeft;
    u8 quadEnabled;
    u8 allAttacksLeft;
    u8 targetFlags;
    u8 menuflags;
    u8 costModifier;
} MagicRecord; // size: 0x8

typedef struct {
    u8 value;
    u8 selected;
} Unk80062F7CMateriaAttribute;

typedef struct {
    u8 id;
    u8 quadCount;
    u8 quadEnabled;
    u8 allCount;
    u8 costModifier;
} CurrentCharMagicCommand; // size: 0x5

typedef struct {
    u8 id;
    u8 allCount;
    u8 materiaEffectFlags;
} CurrentCharBattleMenuCommand; // size: 0x3

typedef struct {
    u8 initialCursorAction;
    u8 targetFlags;
    u16 unknown;
    u16 cameraMovementSingleTarget;
    u16 cameraMovementMultipleTargets;
} BattleCommandData; // size: 0x8

typedef struct {
    u8 unk0[4];
    u8 mpCost;
    u8 unk5[7];
    u8 targetFlags;
    u8 unkD[15];
} AttackData; // size: 0x1C

// Kernel weapon record, one per weapon id (g_WeaponTable), 0x2C-byte stride.
// Combat fields verified by dumping the live table and matching each field
// against published weapon stats (same method as ArmorRecord); the remaining
// fields follow the standard kernel weapon-data layout.
typedef struct {
    u8 targetFlags;     // 0x23 = melee, 0x03 = long-range (hits back row)
    u8 attackEffectId;  // always 0xFF (unused by weapons)
    u8 damageFormula;   // 0x11 = physical; 0xA0-0xA8 select a special formula
                        // (HP/MP/AP/Limit/kills/status/dead-allies), shared by
                        // formula across weapons
    u8 unk3;            // always 0xFF (unused)
    u8 attack;          // attack power
    u8 statusAttack;    // index of the status this attack inflicts; 0xFF (none)
                        // on every weapon (cf. ArmorRecord.statusDefense)
    u8 materiaGrowth;   // 0=None, 1=Normal, 2=Double, 3=Triple
    u8 criticalPercent; // bonus critical-hit %
    u8 attackPercent;   // hit rate
    u8 weaponModel;     // lo nibble = model index, hi nibble = animation mod
    u8 alignmentA;      // always 0xFF (alignment padding)
    u8 soundIdMask;     // mask to reach the high (0x100+) sound-effect ids
    u8 cameraMovementId[2]; // attack camera; always 0xFFFF
    u8 equipMask[2];      // equippable-by-character bitmask (see ArmorRecord);
                          // Cloud weapons add bit9 (Young Cloud) = 0x0201
    u16 attackElement;    // 0x0400=Cut,0x0800=Hit,0x1000=Punch,0x2000=Shoot
    u8 unk12[2];          // unknown, always 0xFFFF
    u8 statBonusId[4];    // stat each slot boosts: 0=Str,1=Vit,2=Mag,3=Spr,
                          // 4=Dex,5=Lck; 0xFF = unused (the Mag column is id 2)
    u8 statBonusValue[4]; // bonus amount, paired with statBonusId; 0xFF unused
    u8 materiaSlot[8];    // one byte per slot; same encoding as ArmorRecord
                          // (5=single/6,7=linked-pair when materiaGrowth!=None;
                          //  1=single/2,3=linked-pair when materiaGrowth==None)
    u8 hitSound;      // sound-effect id for a normal hit (constant per weapon
                      // class)
    u8 criticalSound; // sound-effect id for a critical hit
    u8 missSound;    // sound-effect id for a miss (0x2F on firearms, else 0x05)
    u8 impactEffect; // impact-effect id (varies per weapon)
    u8 specialAttackFlags[2]; // always 0xFFFF
    u8 restrictionMask[2]; // a set bit forbids: 0x01 sell, 0x02 use in battle,
                           // 0x04 use in menu, 0x08 throw (0xFFF6 base; the
                           // initial weapons add sell+throw -> 0xFFFF)
} WeaponRecord;

typedef struct {
    u16 levelUpApLimits[4];
    u8 equipEffect;
    u8 statusEffects[3];
    u8 elementIndex;
    u8 materiaType;
    u8 materiaAttributes[6];
} MateriaData; // size: 0x14

typedef struct {
    u8 counterType;
    u8 battleCommand;
    u8 materiaAttribute;
} ActiveCharEnabledCounter; // size: 0x3

typedef struct {
    u8 id;
    u8 initialCursorAction;
    u8 targetFlags;
    u8 unk4;
    u8 allCount;
    u8 materiaEffectFlags;
} ActiveCharCommandMenu; // size: 0x6

// Runtime data for a battle participant.
typedef struct {
    u8 id;
    u8 coverChance;
    u8 strength;
    u8 vitality;
    u8 magic;
    u8 spirit;
    u8 dexterity;
    u8 luck;
    u16 physAttack;
    u16 physDefence;
    u16 magAttack;
    u16 magDefence;
    s16 hp;
    s16 baseHp;
    s16 mp;
    s16 baseMp;
    s32 unk18;
    s32 unk1C;
    s8 unk20;
    s8 unk21;
    s8 unk22;
    u8 characterFlags;
    ActiveCharEnabledCounter enabledCounters[8];
    u16 physicalAttackElements;
    u16 halvedElements;
    u16 nullifiedElements;
    u16 absorbedElements;
    u32 physicalAttackStatuses;
    u32 immuneStatuses;
    ActiveCharCommandMenu commandMenu[16];
    u8 unkAC[92];
    MagicRecord enabledMagic[96];
    WeaponRecord weapon;
    s16 unk434;
    u8 unk436;
    u8 encounterDownRate;
    s16 unk438;
    s16 unk43A;
    u8 gilBonus;
    u8 encounterRate;
    u8 chocoboChance;
    u8 preemptiveChance;
} ActiveCharacterData; // size: 0x440

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
    u16 unk8;
    u16 unkA[8];
    Unk80062F7CMateriaAttribute materiaAttributes[5];
} Unk80062F7C;

typedef struct {
    /* 0x00 */ LinePos pos;
    /* 0x0C */ u8 isActive;
    /* 0x0D */ u8 entityId;
    /* 0x0E */ u8 touch;
    /* 0x0F */ u8 across;

    /* 0x10 */ u8 requestPushScript;
    /* 0x11 */ u8 requestTalkScript;
    /* 0x12 */ u8 requestTouchOnScript;
    /* 0x13 */ u8 requestTouchOffScript;
    /* 0x14 */ u8 proximityAngle;
    /* 0x15 */ u8 isOnLine;
    /* 0x16 */ u8 slipDisabled;
    /* 0x17 */ u8 unk17;

} FieldLine; // size:0x18

typedef struct {
    s16 KawaiOp1;         // 0x00
    u16 KawaiOp0;         // 0x02
    u8* KawaiDataOffset;  // 0x04
    u8 BlinkOn;           // 0x08
    u8 KawaiA;            // 0x09
    u8 KawaiB;            // 0x0A
    u8 KawaiC;            // 0x0B
    s32 PosX;             // 0x0C
    s32 PosY;             // 0x10
    s32 PosZ;             // 0x14
    s32 MoveStartX;       // 0x18
    s32 MoveStartY;       // 0x1C
    s32 MoveStartZ;       // 0x20
    s8 Unk24[8];          // 0x24-0x2B
    s16 MoveB;            // 0x2C
    u8 Unk2E[2];          // 0x2E-0x2F
    s16 MoveSteps;        // 0x30
    s16 MoveStep;         // 0x32
    u8 Unk34;             // 0x34
    u8 MoveDirAdd;        // 0x35
    u8 MoveDir;           // 0x36
    u8 DirLock;           // 0x37
    u8 Dir;               // 0x38
    u8 TurnSteps;         // 0x39
    u8 TurnStep;          // 0x3A
    u8 TurnType;          // 0x3B
    s16 TurnStart;        // 0x3C
    s16 TurnEnd;          // 0x3E
    s16 OffsetX;          // 0x40
    s16 OffsetStartX;     // 0x42
    s16 OffsetEndX;       // 0x44
    s16 OffsetY;          // 0x46
    s16 OffsetStartY;     // 0x48
    s16 OffsetEndY;       // 0x4A
    s16 OffsetZ;          // 0x4C
    s16 OffsetStartZ;     // 0x4E
    s16 OffsetEndZ;       // 0x50
    u16 OffsetSteps;      // 0x52
    u16 OffsetStep;       // 0x54
    u8 OfsType;           // 0x56
    u8 entityId;          // 0x57 - entity model is attached to
    u8 requestPushScript; // 0x58
    u8 SolidOff;          // 0x59
    u8 requestTalkScript; // 0x5A
    u8 TalkOff;           // 0x5B
    u8 visible;           // 0x5C
    u8 scriptedMoveMode;  // 0x5D - enum ScriptedMoveMode
    u8 activeAnimId;      // 0x5E
    s8 unk5F;             // 0x5F
    s16 animSpeed;        // 0x60
    s16 animCurrentFrame; // 0x62
    s16 animLastFrame;    // 0x64
    u16 charId;           // 0x66 - model id
    s16 ActionArg;        // 0x68
    s16 ActionState;      // 0x6A
    u16 SolidRange;       // 0x6C
    u16 TalkRange;        // 0x6E
    u16 MoveSpeed;        // 0x70
    u16 PosI;             // 0x72
    s16 MoveEndI;         // 0x74
    u16 Pad76;
    s32 MoveEndX; // 0x78
    s32 MoveEndY; // 0x7C
    s32 MoveEndZ; // 0x80
} FieldEntity;    // size:0x84

typedef struct {
    /* 0x00 */ u8 faceId;          // texture face/palette id
    /* 0x01 */ u8 boneCount;       // number of bones
    /* 0x02 */ u8 partCount;       // number of model parts
    /* 0x03 */ u8 animationCount;  // number of animations
    /* 0x04 */ u8 modelEntryIndex; // index into FieldModelData->modelEntries
    /* 0x05 */ u8 npcFlag;         // NPC/model type flag?
    /* 0x06 */ u8 unk6;
    /* 0x07 */ u8 globalModelId; // BCX/global model lookup id
} FieldModelLoaderData;          // size:0x8

typedef struct {
    /* 0x00 */ u8 flags;     // initialized to 1, later cleared
    /* 0x01 */ u8 kawaiType; // KAWAI second byte
    /* 0x02 */ u8 boneCount;
    /* 0x03 */ u8 partCount;
    /* 0x04 */ u8 animationCount;
    /* 0x05 */ s8 rotationX;
    /* 0x06 */ s8 rotationY;
    /* 0x07 */ s8 rotationZ;
    /* 0x08 */ s32 translationX;
    /* 0x0C */ s32 translationY;
    /* 0x10 */ s32 translationZ;
    /* 0x14 */ u8 globalModelId;
    /* 0x15 */ u8 textureFaceId;
    /* 0x16 */ u16 scale;
    /* 0x18 */ u16 partsOffset;
    /* 0x1A */ u16 animationOffset;
    /* 0x1C */ u8* modelData;
    /* 0x20 */ u8* partMatrices; // part matrix data
} FieldModelEntry;               // size:0x24

typedef struct {
    /* 0x00 */ u8 modelCount;
    /* 0x01 */ u8 unk1;                       // (initialized to 0)
    /* 0x02 */ u16 unk2;                      // (initialized to 0)
    /* 0x04 */ FieldModelEntry* modelEntries; // per-model-file records
} FieldModelData;

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
    u8 renderBuffer;
    // enum FieldEventCmd.
    u8 eventCmd;
    // Used by some event commands to carry extra info, ie. shop id
    // for shop menu, char id for name entry screen.
    s16 eventCmdParam;
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
    s16 pcDirection;       // Direction player is facing.
    s16 movieCommandState; // enum MovieCommandState.
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
    u8 scrloSet;        // Set by SCRLO. Unused(?)
    // Set by MPDSP in field map junbin5. Also set to 1 if
    // fadeType == FFT_INSTANT_BLACK.
    u8 mpdspSet;
    // Set by MVCAM. Static field map camera is used instead of dynamic movie
    // camera.
    u8 movieCamDisabled;
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
    // Raw states ignore custom key mapping set by player.
    u32 activeKeysRaw;     // Currently active keys.
    u32 activeKeysPrevRaw; // activeKeysRaw from last frame.
    u32 pressedKeysRaw;    // Was inactive last frame.
    u32 releasedKeysRaw;   // Was active last frame.
    u32 activeKeys;
    u32 activeKeysPrev;
    u32 pressedKeys;
    u32 releasedKeys;
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

typedef struct {
    u8 eventDataVersion;
    u8 eventVersion;
    u8 numEntities;
    u8 numModels;
    u16 stringOffset; // Offset to strings
    u16 numExtras;    // Akao and tutorials
    u16 scale;
    u16 pad[3];
    char author[8];
    char name[8];
    /*
    char entityNames[numEntities][8];
    u32 extras[numExtras]; // Offsets to akao/tutorial blocks
    u16 entityScripts[numEntities][32]; // Offsets to entity scripts
    */
} FieldScriptHeader; // size:Varies

typedef struct {
    s32 unk0;
    s32 unk4;
    u32 unk8;
} Unk80075D00;

typedef struct WindowData {
    u8* text;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 currentWidth;
    s16 currentHeight;
    s16 textScrolling;
    s16 stringLength;
    s16 stringByteLength;
    s16 currentRow;
    u8 isFull;
    u8 style; // enum WindowStyle
    u8 pointerEnabled;
    u8 numDisplayType; // enum WindowNumDispType
    u8 unk1C;
    s8 numDisplayLength;
    s16 unk1E;
    s32 numDisplayValue;
    s16 pointerX;
    s16 pointerY;
    s16 numDisplayX;
    s16 numDisplayY;
    s16 state; // enum WindowState
    u16 preventClose;
} WindowData; // size:0x30

extern u16 g_Pad1Keys;
extern u16 g_Pad1KeysPressed;
extern u16 g_Pad1KeysRepeat;

// Map between battle character IDs and index into character record array.
// Battle characters have IDs 0-10. 9 and 10 are young Cloud and Sephiroth from
// flashback sequence and they use same character records as Cait Sith and
// Vincent.
extern s32 g_BattleCharIdToCharId[11];
extern u8 g_MenuColors[12]; // 4 corners x RGB
extern u8 D_800492F0[][12]; // see Labels enum
extern FieldModelData* g_FieldModelData;
extern u8 D_80062D98; // battle_clearRenderList
// Set while a memory-card transfer is in flight and the savemap must not be
// touched; battle code spin-waits on it.
extern volatile u8 g_SavemapBusy;
extern s32 D_80062DCC;
extern s8 _D_80062DFD;
extern u8 D_80062F19; // Enemy Lure/Away Modifier
extern u8 D_80062F1A;
extern u8 D_80062F1B;
extern Gpu D_80062F24;
extern u16 D_80062F3C;
extern s32 g_MenuRenderBufferIndex;
extern u_long* D_80062FC4;
extern Unk800A8D04* g_CurrentAction;
extern DRAWENV D_800706A4[2];
extern u8 g_FieldMusicLock; // MUSIC/FMUSC skip the sound engine while nonzero
                            // (set by the MULCK opcode)
extern u8 D_80070788;
extern u8 g_EntityToLine[48];
extern u16 g_BattleMode;
extern u16 g_FieldWaitCounter[48];      // Used by WAIT opcode to pause script
extern u16 g_SavedFieldScriptPC[48][8]; // Program counters of paused scripts
extern s16 D_80071A5C;
extern u8 g_FieldScriptSyncWaitEntity[48][8];
extern s8 g_FieldDebugCurPage;
extern u8 D_80071E24;
extern u8 g_WindowCount;
extern u8 D_80071E30;
extern MATRIX* D_80071E40;
extern u8 g_PartyUpdatedByFieldScript;
extern u8 g_CurrentEntity; // entity owning the currently executing script
extern Unk800730CC D_800730CC[];
extern MateriaData g_MateriaData[100];
extern CurrentCharBattleMenuCommand D_80069508[16];
extern CurrentCharMagicCommand D_80069554[56];
extern u8* D_800707C0;
extern BattleCommandData D_800707C4[32];
extern AttackData D_800708C4[];
extern FieldEntity g_FieldEntity[];
extern u8 D_800756E8[]; // per-model flags, indexed by field model id
extern s32 D_800756F8[];
extern Unk80075D00* D_80075D00;
extern int D_80075DEC;           // buffer index, either 0 or 1
extern u8 g_FieldMapVars[256];   // map-local memory bank for field scripts
extern s8 D_80077F64[2][0x3400]; // polygon buffer
extern u8* g_FieldText;
extern FieldLine g_FieldLines[32];
extern DRAWENV D_8007EAAC[2];
extern DISPENV D_8007EB68[2];
extern u8 g_EntityToModel[48]; // entity id -> model id (0xFF: none)
extern s8 D_8007EBCC;
extern s8 D_8007EBDC;
extern u8 D_8007EBE0;      // field debug mode
extern u8 g_CharacterLock; // mirror of the UC opcode's control-lock flag
extern u8 g_EntitySplitJoinState[48]; // states for SPLIT and JOIN opcodes
extern s16 D_80082248[]; // per-model current animation playback speed
extern u8 D_80083184[0x40];
extern u8 D_800831C4[];         // Magic Order table from kernel.bin section 3.
extern u16 g_FieldScriptPC[48]; // program counters for active entity scripts
extern u8 D_8008325C[];         // per-model default animation id (DFANM)
extern u8 g_WindowToEntity[4];
extern WindowData g_WindowData[4];
extern u8 D_8008326C[4];
extern s32 D_80083338;
extern u8 g_FieldScriptSyncState[48][8]; // sync states of entity scripts per
                                         // priority level
extern FieldModelLoaderData* g_FieldModelLoaderData;
extern s16 g_FieldLineCount;
extern s8 D_80095DCC;
extern volatile u16 D_80095DD4;
extern s16 g_PlayerModelId;
extern s16 g_isFieldLoading;
extern volatile s16 D_800965EC;
extern u8 D_80099FFC;
extern s16 D_8009A000[1];
extern u32 D_8009A004[1];
extern s32 D_8009A008[1];
extern s32 D_8009A00C;
extern s32 D_8009A024[8];
extern u8 g_FieldCurrentOpcode;
extern s32 D_8009A064;
extern MenuTable g_PartyMenuTables[3];
extern u8 g_FieldScriptPriority[48]; // active scripts execution priority
extern FieldState g_FieldState;
extern u8 D_8009AC2F;
extern u8 g_CharIdToEntity[9];
extern FieldEntity* g_FieldModels; // loaded field models
extern u8 g_FieldModelCount;       // number of allocated field models
extern FieldScriptHeader* g_FieldScripts;
extern FieldState* g_pFieldState; // points to g_FieldState
extern SaveWork Savemap;          // 0x8009C6E4
extern u8 D_8009CBDC[];
extern u16 D_8009D288[];
extern u8 D_8009D2E7;
extern u8 D_8009D302;
extern u8 D_8009D391[1]; // part of a struct?
extern u8 D_8009D40D;
extern u8 D_8009D588; // disc number requested by the DSKCG opcode
extern u8 D_8009D684;
extern u8 D_8009D685;
extern u8 D_8009D686;
extern u8 D_8009D60E;
extern u8 g_DebugLevel; // field debug related
extern CharacterLevelData g_CharacterLevelData[3];
extern u8 D_8009D824;
extern s16 D_8009D828[]; // per-model base animation speed
extern BattleItemReward g_BattleItemsEarned[4];
extern u8 D_8009D8F8[];
extern u32 D_8009D260;
extern volatile s32 D_8009D268[];
extern ActiveCharacterData g_ActiveCharacters[9];
extern u8 D_8009FE8C;
extern u8 D_800C7304[16];

// PSXSDK funcs
SVECTOR* ApplyMatrixSV(MATRIX* m, SVECTOR* v0, SVECTOR* v1);
MATRIX* RotMatrixYXZ(SVECTOR* r, MATRIX* m);
void SystemError(char c, long n);

void func_80014B54(void);
s32 func_8001521C(s32);
const char* func_80015248(s32 arg0, s32 arg1, s32 arg2);
void func_800155A4(s32, ...);
void func_8001726C(s16, u16);
u32 InputReadPadsRaw(void);
u32 InputReadPads(void);
void func_80021044(DRAWENV* draw_env, DISPENV* disp_env);
s32 func_80023050(void);
void func_8002305C(s32 state, s32 menuId);
u8* GetCharacterName(s32 battleCharId);
void func_800262D8();
void func_80026448(
    MenuTable* table, s32 column, s32 row, s32 numColumns, s32 numRowsPerPage,
    s32 unk0, s32 rowOffset, s32 unk4, s32 numTotalRows, s32 unkE, s32 unkF,
    s32 unk10, s32 unk11, u16 scrolling);
void func_800269C0(void* poly);
s32 func_80026B70(unsigned char* str);
void func_80026F44(s32 x, s32 y, const char*, s32 color); // print FF7 string
int SystemAkaoExecute();

int func_80033DAC(int sector_no, void (*cb)());
int func_80033DE4(int sector_no);
int SystemLoadFileBySector(
    int sector_no, size_t size, u_long* dst, void (*cb)());
int DS_read(int sector_no, size_t size, u_long* dst, void (*cb)());
int func_80033EDC(int sector_no, void (*cb)());
int func_80033F40(int sector_no, size_t size, u_long* dst, void (*cb)());
int func_80033FC4(int sector_no, size_t size, u_long* dst, void (*cb)());
u32 SystemCdromReadChain(void);

// from overlays
extern u8 SavedScriptIds[48][8]; // script ids of latest queued scripts
extern u_long* D_8019D5E8;
extern s32 D_8019DAA0;

#endif
