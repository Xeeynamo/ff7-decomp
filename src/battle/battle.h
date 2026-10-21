#ifndef BATTLE_H
#define BATTLE_H

#include <game.h>

#define EFFECT_MODEL_SLOT (NUM_PARTY) // holds the model an effect overlay supplies
#define START_ENEMY (NUM_PARTY + 1)
#define NUM_ENEMY (6)
#define NUM_BATTLE_ACTOR (START_ENEMY + NUM_ENEMY) // 10
#define NUM_ZONES (3)                              // battlefield zones, see BattleData.unitZoneMask

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/BattleFlags.cs#L4
typedef enum {
    SETUP_UNK2 = 0x2,
    SETUP_CANNOT_ESCAPE = 0x4,
    SETUP_NO_VICTORY_POSE = 0x8,
    SETUP_NO_PREEMPTIVE_STRIKE = 0x10,
} BattleSetupFlags;

// attackEffectId of a summon (currentActionId 3), named after its MAGIC/*.BIN.
typedef enum {
    SUMMON_CHOCO0,   // Choco/Mog
    SUMMON_SIVA,     // Shiva
    SUMMON_IFLEET,   // Ifrit
    SUMMON_LAMU,     // Ramuh
    SUMMON_TITAN,    // Titan
    SUMMON_ODIN2,    // Odin
    SUMMON_RIVA,     // Leviathan
    SUMMON_VAHAMUT,  // Bahamut
    SUMMON_KUJATA,   // Kujata
    SUMMON_ALEX,     // Alexander
    SUMMON_PHOENIX,  // Phoenix
    SUMMON_VAHAMUT2, // Neo Bahamut
    SUMMON_HADES,    // Hades
    SUMMON_TUPON,    // Typhon
    SUMMON_VAHAMUT0, // Bahamut ZERO
    SUMMON_KNIGHTS,  // Knights of the Round
    SUMMON_DEBUCHO,  // Fat Chocobo
    SUMMON_ODIN1,    // Odin
} Summon;

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/BattleType.cs#L3
typedef enum {
    SETUP_DEFAULT,
    SETUP_PREEMPTIVE,
    SETUP_BACK_ATTACK,
    SETUP_SIDE_ATTACK,
    SETUP_PINCER,
    SETUP_PINCER_2,
    SETUP_SIDE_ATTACK_2,
    SETUP_SIDE_ATTACK_3,
    SETUP_FRONT_ROW_ONLY,
    NUM_SETUP,
} BattleSetupType;

typedef enum {
    EVENT_BATTLE_SQUARE = 0x40,
} BattleEventType;

// Battle message IDs, the entries of KERNEL_TEXT_BATTLE_MESSAGES.
typedef enum {
    BATTLE_TEXT_LIMIT_BREAK = 0x26,
    BATTLE_TEXT_STOLE_ITEM = 0x2F,
    BATTLE_TEXT_STEAL_FAILED = 0x30,
    BATTLE_TEXT_NOTHING_TO_STEAL = 0x31,
    BATTLE_TEXT_PREEMPTIVE_ATTACK = 0x32,
    BATTLE_TEXT_BACK_ATTACK = 0x33,
    BATTLE_TEXT_SIDE_ATTACK = 0x34,
    BATTLE_TEXT_PINCER_ATTACK = 0x35,
    BATTLE_TEXT_CANNOT_ESCAPE = 0x36,
    BATTLE_TEXT_RECEIVED_ITEM = 0x52,
    BATTLE_TEXT_ITEM_STOLEN = 0x53,
    BATTLE_TEXT_RECEIVED_GIL = 0x54,
    BATTLE_TEXT_MORPH_ITEM = 0x56,
    BATTLE_TEXT_CANNOT_MIMIC = 0x57,
    BATTLE_TEXT_USELESS_IMITATION = 0x58,
    BATTLE_TEXT_CANNOT_MANIPULATE = 0x59,
    BATTLE_TEXT_MANIPULATED = 0x5A,
    BATTLE_TEXT_NOT_ENOUGH_MP = 0x5B,
    BATTLE_TEXT_SKILL_POWER_USED_UP = 0x5C,
    BATTLE_TEXT_ELEMENT_NAMES_START = 0x5D,
    BATTLE_TEXT_WEAK_AGAINST_ELEMENT = 0x6D,
    BATTLE_TEXT_DATA_ERROR_START = 0x6E,
    BATTLE_TEXT_SENSE_HP = 0x72,
    BATTLE_TEXT_LEARNED_ENEMY_SKILL = 0x73,
    BATTLE_TEXT_SENSE_NAME_LEVEL = 0x74,
    BATTLE_TEXT_SENSE_HP_MP = 0x75,
    BATTLE_TEXT_SENSE_FAILED = 0x76,
    BATTLE_TEXT_OFFSET_ENTRY = 0x7E,
} BattleTextId;

typedef enum {
    FORMULA_MODIFIER_NONE = 0x0,
    FORMULA_MODIFIER_FORCE_PHYSICAL = 0x1, // clears attackFlags 0x04 (magical attack)
    FORMULA_MODIFIER_CUSTOM_DMG = 0x2,     // adds 16 to dmgFormulaId (custom formula slots)
    FORMULA_MODIFIER_SPECIAL_EFFECT = 0x4, // selects effect from table, forces physical damage formula
} BattleFormulaModifierFlags;

// Used to index BattleTurnWork.statusTimers; g_StatusBitTable[index] is the bit
// number (0-31) of the BattleUnit.status flag that timer belongs to, and also
// used to index g_StatusTimerInitValues
typedef enum {
    TIMER_STOP,
    TIMER_PARALYSIS,
    TIMER_D_SENTENCE,
    TIMER_SLOW_NUMB,
    TIMER_BARRIER,
    TIMER_M_BARRIER,
    TIMER_POISON,
    TIMER_SLEEP,
    TIMER_REGEN,
    TIMER_DUAL_DRAIN,
    TIMER_SHIELD,
    TIMER_PEERLESS,
    NUM_STATUS_TIMERS = 16,
} BattleStatusTimerIndex;

// Used to index BattleTurnWork.statMults
typedef enum {
    STAT_MULT_PHYS_ATTACK,
    STAT_MULT_MAG_ATTACK,
    STAT_MULT_PHYS_DEFENCE,
    STAT_MULT_MAG_DEFENCE,
    STAT_MULT_PHYS_EVADE,
    STAT_MULT_DEXTERITY,
    NUM_STAT_MULTS = 8,
} BattleStatMultIndex;

typedef enum {
    AFFINITY_DEATH = 0x1,
    AFFINITY_UNK2 = 0x2,
    AFFINITY_DOUBLE = 0x4,
    AFFINITY_UNK8 = 0x8, // Unused?
    AFFINITY_HALF = 0x10,
    AFFINITY_NULLIFY = 0x20,
    AFFINITY_ABSORB = 0x40,
    AFFINITY_FULL_CURE = 0x80,
} BattleAffinityFlags;

typedef enum {
    SPRITE_QUAD_FLIP_U = 0x1,
    SPRITE_QUAD_FLIP_V = 0x2,
    SPRITE_QUAD_SEMI_TRANS = 0x100,
    SPRITE_QUAD_TEX_SIZE = 0x200,
} SpriteQuadFlags;

enum CombatantStateFlags {
    COMBATANT_DEFENDING = 0x20,
    COMBATANT_BACK_ROW = 0x40,
};

typedef enum {
    BATTLE_MSG_ARG_CHAR_NAME = 0xEA,
    BATTLE_MSG_ARG_ITEM_NAME = 0xEB,
    BATTLE_MSG_ARG_NUMBER = 0xEC,
    BATTLE_MSG_ARG_UNIT_NAME = 0xED,
    BATTLE_MSG_ARG_MAGIC_NAME = 0xEE,
    BATTLE_MSG_ARG_ENEMY_LETTER = 0xEF,
    BATTLE_MSG_ARG_BATTLE_TEXT = 0xF0,
    BATTLE_MSG_ARG_KERNEL_TEXT = 0xF1,
    BATTLE_MSG_ARG_START = BATTLE_MSG_ARG_CHAR_NAME,
    BATTLE_MSG_ARG_END = BATTLE_MSG_ARG_KERNEL_TEXT,
} BattleMessageArgType;

typedef struct {
    // condition/status bitmask; see BattleStatusFlags above for the bits
    // confirmed live here
    /* 0x00 */ s32 status;
    // CombatantStateFlags; also 0x10 = Limit transformation active (set by
    // BattleApplyVincentLimitTransform)
    /* 0x04 */ u32 stateFlags;
    /* 0x08 */ s8 actorId;
    /* 0x09 */ u8 level;
    /* 0x0A */ s8 targetSubSlot;
    /* 0x0B */ s8 minElemInfluence;
    /* 0x0C */ s8 formationIndex;
    /* 0x0D */ u8 physAttack;
    /* 0x0E */ u8 magAttack;
    /* 0x0F */ u8 physEvade;
    /* 0x10 */ s8 idleActionId;
    /* 0x11 */ u8 hurtActionId;
    /* 0x12 */ u8 backDamageMult;
    /* 0x13 */ u8 rowFlags;
    /* 0x14 */ u8 dexterity;
    /* 0x15 */ u8 luck;
    /* 0x16 */ s8 unk16;
    /* 0x17 */ u8 coverTargetSlot;
    /* 0x18 */ u16 attackMask;
    /* 0x1A */ u16 attackerMask;
    /* 0x1C */ s32 statusAppliedMask;
    /* 0x20 */ u16 physDefence;
    /* 0x22 */ u16 magDefence;
    /* 0x24 */ u16 enemyId;
    /* 0x26 */ u16 elemAbsorbExtra;
    /* 0x28 */ u16 curMP;
    /* 0x2A */ u16 maxMP;
    /* 0x2C */ u32 curHP;
    /* 0x30 */ u32 maxHP;
    /* 0x34 */ u32 elemHalve;
    /* 0x38 */ u32 elemNullify;
    /* 0x3C */ u32 elemAbsorb;
    /* 0x40 */ u32 elemWeakness;
    /* 0x44 */ u32 prevStatus;
    /* 0x48 */ u32 unk48;
    /* 0x4C */ u8 unk4C;
    /* 0x4D */ u8 magEvade;
    /* 0x4E */ u8 formationRow;
    /* 0x4F */ u8 unk4F;
    /* 0x50 */ u16 unk50; // Stolen gil?
    /* 0x52 */ u16 unk52; // Stolen item?
    /* 0x54 */ u16 elemImmuneExtra;
    /* 0x56 */ u8 unk56;
    /* 0x57 */ u8 unk57;
    /* 0x58 */ u32 gil;
    /* 0x5C */ u32 exp;
    /* 0x60 */ u32 unk60[2];
} BattleUnit; // size:0x68

typedef struct {
    /* 0x000 */ u16 unk0;
    /* 0x002 */ u16 presentMask;
    /* 0x004 */ u16 scriptSelfMask;
    /* 0x006 */ u16 scriptOpponentNonPetrifiedMask;
    /* 0x008 */ u16 scriptAllyAliveMask;
    /* 0x00A */ u16 scriptAllyDeadMask;
    /* 0x00C */ u16 scriptOpponentAliveMask;
    /* 0x00E */ u16 scriptOpponentDeadMask;
    /* 0x010 */ u16 playerUnitMask;
    /* 0x012 */ u16 enemyUnitMask;
    /* 0x014 */ u16 allUnitsMask;
    /* 0x016 */ u16 pad16;
    /* 0x018 */ u16 unk18;
    /* 0x01A */ u16 cycleFlags;
    /* 0x01C */ u16 unk1C;
    /* 0x01E */ u16 unk1E;
    /* 0x020 */ u16 sceneID;
    /* 0x022 */ u16 pad22;
    /* 0x024 */ u16 setupFlags;
    /* 0x026 */ u16 attackCategoryFlags;
    /* 0x028 */ u16 arenaRound;
    /* 0x02A */ u16 pad2A;
    /* 0x02C */ u16 unk2C;
    /* 0x02E */ u16 pad2E;
    /* 0x030 */ u32 partyGil;
    /* 0x034 */ BattleUnit combatant[NUM_BATTLE_ACTOR];
} BattleState; // size:0x444

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/BattleSetupData.cs
typedef struct {
    /* 0x00 0x80163614 */ u16 stageID; // load STAGE/ files
    /* 0x02 0x80163616 */ u16 nextStageID;
    /* 0x04 0x80163618 */ s16 escapeCounter;
    /* 0x06 0x8016361A */ s16 D_8016361A;
    /* 0x08 0x8016361C */ u16 D_8016361C[4];
    /* 0x10 0x80163624 */ u16 flags; // BattleSetupFlags
    /* 0x12 0x80163626 */ u8 type;   // BattleSetupType
    /* 0x13 0x80163627 */ s8 cameraID;
} BattleSetup; // size:0x14

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/CameraPlacementData.cs
typedef struct {
    short x, y, z;
} CameraPos;
typedef struct {
    CameraPos start;
    CameraPos direction;
} CameraPlacement; // size:0xC

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/EnemyLocation.cs
typedef struct {
    /* 0x00 */ s16 enemyID;
    /* 0x02 */ u16 x, y;
    /* 0x06 */ s16 z;
    /* 0x08 */ u16 row;
    /* 0x0A */ u16 coverFlags;
    /* 0x0C */ u32 flags;
} FormationEntry; // size:0x10

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/Enemy.cs
typedef struct {
    /* 0x00 */ u8 name[0x20];
    /* 0x20 */ u8 level;
    /* 0x21 */ u8 speed;
    /* 0x22 */ u8 luck;
    /* 0x23 */ u8 evade;
    /* 0x24 */ u8 strength;
    /* 0x25 */ u8 defense;
    /* 0x26 */ u8 magic;
    /* 0x27 */ u8 magicDef;
    /* 0x28 */ u8 affinityIds[8];   // which element or status bit (0-63, 0xFF empty)
    /* 0x30 */ u8 affinityTypes[8]; // 4 = half, 5 = nullify, 6 = absorb
    /* 0x38 */ u8 actionAnimeIdx[16];
    /* 0x48 */ u16 attackID[16];
    /* 0x68 */ u16 cameraMovementIDs[16];
    /* 0x88 */ u8 dropItemRates[4];
    /* 0x8C */ u16 dropItemIDs[4];
    /* 0x94 */ u16 manipAttackIDs[3];
    /* 0x9A */ u16 unk9A;
    /* 0x9C */ u16 mp;
    /* 0x9E */ u16 ap;
    /* 0xA0 */ u16 morphItem;
    /* 0xA2 */ u8 backAttackMultiplier;
    /* 0xA3 */ u8 padA3;
    /* 0xA4 */ u32 hp;
    /* 0xA8 */ u32 exp;
    /* 0xAC */ u32 gil;
    /* 0xB0 */ u32 statusImmunities;
    /* 0xB4 */ u32 padB4;
} SceneEnemy; // size:0xB8

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/Shared/DataParser.cs

typedef struct {
    /* 0x000 */ u16 scriptOffsets[4]; // Offsets for formations 1 through 4 (0x00 - 0x07)
    /* 0x008 */ u8 scriptData[504];   // Bytecode stream for formation scripts (0x08 - 0x1FF)
} FormationAIScripts;                 // size:0x200

// an uncompressed chunk from SCENE.BIN
typedef struct {
    /* 0x000 */ s16 enemyModelIDs[4];
    /* 0x008 */ BattleSetup setup[4];
    /* 0x058 */ CameraPlacement camera[4][4];
    /* 0x118 */ FormationEntry formation[4][6];
    /* 0x298 */ SceneEnemy enemy[3];
    /* 0x4C0 */ AttackData attacks[0x20];
    /* 0x840 */ u16 attackIDs[0x20];
    /* 0x880 */ u8 attackNames[0x20][0x20];
    /* 0xC80 */ FormationAIScripts formationAI;
    /* 0xE80 */ u8 script[0x1000];
} SceneContainer; // 0x1E80

typedef struct {
    /* 0x00 */ s16 enemyModelIDs[4];
    /* 0x08 */ BattleSetup setup;
    /* 0x1C */ CameraPlacement camera[4];
    /* 0x4C */ FormationEntry formation[NUM_ENEMY];
} ActiveEncounterData; // size:0xAC

typedef struct {
    /* 0x00 */ s8 charId;
    /* 0x01 */ u8 idleActionId;
    /* 0x02 */ s8 D_801636BA;
    /* 0x03 */ s8 D_801636BB;
    /* 0x04 */ u8 D_801636BC;
    /* 0x05 */ s8 D_801636BD;
    /* 0x06 */ s16 D_801636BE;
    /* 0x08 */ s32 status;
    /* 0x0C */ s32 D_801636C4;
} BattleActor; // size:0x10

// Layout follows a 0x178-byte struct found in the PC port via memset; no such
// evidence exists yet for PSX but so far everything in the PSX version lines up
// Treat the size as probable but unconfirmed
typedef struct {
    /* 0x000 */ ActiveEncounterData activeEncounter;
    /* 0x0AC */ BattleActor actors[NUM_BATTLE_ACTOR];
    /* 0x14C */ u16 unk14C;
    /* 0x14E */ u16 unitPresentMask;
    /* 0x150 */ u16 unk150;
    /* 0x152 */ u16 unk152;
    /* 0x154 */ u16 unk154;
    /* 0x156 */ u16 limitReadyMask;
    /* 0x158 */ u16 unk158;
    /* 0x15A */ u16 downedActors;
    /* 0x15C */ u16 unk15C;
    /* 0x15E */ u16 flags;
    /* 0x160 */ u16 isMultiBattle;           // This is actually a counter for consecutive battles (battle square?)
    /* 0x162 */ u16 unitZoneMask[NUM_ZONES]; // set by BattleInitFormation; [1] is the middle in pincer/side attacks
    /* 0x168 */ u8 caitSithRolls[4];         // Dice: packed die values, 2 per byte; Slots: the 3 landed reel symbols
    /* 0x16C */ u8 unk16C[7];
    /* 0x173 */ u8 unk173;
    /* 0x174 */ u16 unk174;
    /* 0x176 */ u16 unk176;
} BattleData; // size:0x178

// Action entry, matches
// https://wiki.ffrtt.ru/index.php/FF7/Battle/Battle_Mechanics action-queue
// layout exactly (priority/queue-pos/slot/cmdIndex/attackIndex/targetMask).
// Entries reach g_BattleWork.actionQueue (a 64-slot priority table) through BattleScheduleAction,
// either directly (BattleQueueImmediateAction) or via the g_BattlePendingActions ring
// (BattleQueuePendingAction). BattleBattleActionQueueExecute drains the table in priority
// order into BattleCmdScriptDispatch, which runs the command as a byte-coded sequence
// of opcodes (g_BattleCmdOpcodeOffs/g_BattleCmdOpcodeStream/g_BattleCmdOpcodeJmpTbl), not a single switch on
// cmdIndex. Full writeup: ff7-re/reference/BATTLE_COMMAND_QUEUE.md
typedef struct {
    /* 0x0 */ u8 priority;        // BattleQueuePendingAction: 5 = CMD_LIMIT, 6 = other commands
    /* 0x1 */ s8 orderInPriority; // position within the priority band, set by BattleScheduleAction
    /* 0x2 */ s8 unitSlot;
    /* 0x3 */ s8 cmdIndex;     // BattleCommand, stored raw (not the enum type -- keeps the 0x8-byte layout);
                               // -1 = the AI picks the command
    /* 0x4 */ s16 attackIndex; // command-dependent: spell id for CMD_MAGIC,
                               // damage dealt for CMD_COIN, skill id for
                               // CMD_ENEMY_SKILL, etc -- not a uniform lookup
    /* 0x6 */ s16 targetMask;
} BattleActionEntry; // size:0x8

#define BATTTLE_SCENE 7

typedef struct {
    MATRIX m;
    SVECTOR rot;
    SVECTOR trans;
    MATRIX* parentMatrix;
} BattleModelSub; // size:0x34

enum BattleModelSpecialFlags {
    BATTLE_MODEL_INACTIVE = 0x2,
    BATTLE_MODEL_HIDDEN = 0x4,
    BATTLE_MODEL_NO_SHADOW = 0x10,
};

enum BattleModelAnimControlFlags {
    ANIM_CTRL_FADE_OUT = 0x2,
    ANIM_CTRL_FADE_IN = 0x4,
};

typedef struct {
    /* 0x000 */ s16 animDescOffset;
    /* 0x002 */ s16 animId;
    /* 0x004 */ s16 queuedActionId;
    /* 0x006 */ s16 scale;
    /* 0x008 */ u16 modelSetting1;
    /* 0x00A */ u16 modelSetting2;
    /* 0x00C */ s16 modelSetting3;
    /* 0x00E */ s16 nextAnimId;
    /* 0x010 */ s16 numBones;
    /* 0x012 */ s16 collisionRadius;
    /* 0x014 */ s16 blendAlpha;
    /* 0x016 */ s16 clutOffset;
    /* 0x018 */ s16 defaultRotX;
    /* 0x01A */ s16 defaultRotY;
    /* 0x01C */ s16 defaultRotZ;
    /* 0x01E */ s16 pad1E;
    /* 0x020 */ s16 modelFlags;
    /* 0x022 */ u8 attackEffectId;
    /* 0x023 */ u8 currentActionId;
    /* 0x024 */ u8 unk24;
    /* 0x025 */ u8 specialFlags;
    /* 0x026 */ u8 ready;
    /* 0x027 */ u8 deathType;
    /* 0x028 */ u8 colorR;
    /* 0x029 */ u8 colorG;
    /* 0x02A */ u8 colorB;
    /* 0x02B */ u8 boneIndices[16]; // [0] root, [11] and [12] feet

    /* 0x03B */ s8 scriptEnabled;
    /* 0x03C */ u8 scriptPc;
    /* 0x03D */ s8 scriptWaitFrames;
    /* 0x03E */ u8 animControlFlags;
    /* 0x03F */ u8 boneFlags[53];
    /* 0x074 */ s32 animInProgress;
    /* 0x078 */ s32* boneModels[50];
    /* 0x140 */ MATRIX stageMatrix;
    /* 0x160 */ SVECTOR rootRot;
    /* 0x168 */ SVECTOR rootTrans;
    /* 0x170 */ MATRIX* pCameraMatrix;
    /* 0x174 */ BattleModelSub boneTransforms[50];
} BattleModel; // size:0xB9C

typedef struct {
    /* 0x0000 */ u_long* unk0[0x1C];
    /* 0x0070 */ u_long* unk70[0x1000];
    /* 0x4070 */ u_long* unk4070[2];
    /* 0x4078 */ u_long* unk4078[2];
    /* 0x4080 */ u_long* unk4080[9];
    /* 0x40A4 */ u_long* unk40A4[0x10];
    /* 0x40E4 */ u_long* unk40E4[2];
    /* 0x40EC */ u_long* unk40EC[2];
} DB; // size:0x40F4

typedef struct {
    /* 0x00 */ MATRIX m;
    /* 0x20 */ SVECTOR rot;
    union {
        /* 0x28 */ VECTOR v;
        struct {
            /* 0x28 */ SVECTOR trans;
            /* 0x30 */ s32 unk30;
            /* 0x34 */ u8 unk34;
            /* 0x35 */ s8 unk35;
            /* 0x36 */ s16 unk36;
        } sub;
    } u;
} BattleWorldView; // size:0x38

// Flag word at offset 4, as func_800D29D4 tests it. Each mirror bit negates
// one rotation column, reversing polygon winding, so the cull test XORs their
// parity back out. Readings agree with Akari's q-gears_reverse
// (ffvii/DISC/BATTLE/BATTLE.X_model.cpp, "funcd29d4").
enum ModelRenderFlags {
    MODEL_MIRROR_X = 0x1,           // negates R11/R21/R31
    MODEL_MIRROR_Y = 0x2,           // negates R12/R22/R32
    MODEL_MIRROR_Z = 0x4,           // negates R13/R23/R33
    MODEL_SEMI_TRANS = 0x8,         // sets bit 1 of the GPU command byte
    MODEL_NO_DEPTH_CUE = 0x10,      // skips dpcs/dpct under MODEL_DEPTH_CUE
    MODEL_NO_CULL = 0x20,           // skips nclip; both faces emitted
    MODEL_PRIM_TPAGE = 0x40,        // tpage from primitive byte 7, not tpage
    MODEL_DEPTH_CUE = 0x80,         // offset 0xA feeds GTE IR0; see below
    MODEL_PRIM_PACKET_BITS = 0x100, // primitive byte 7 << 18 ORed into colour
};

// Model descriptor read by func_800D29D4. Field readings agree with Akari's
// q-gears_reverse (ffvii/DISC/BATTLE/BATTLE.X_model.cpp, "funcd29d4").
typedef struct {
    /* 0x0 */ s32* model;
    /* 0x4 */ s32 flags;    // ModelRenderFlags
    /* 0x8 */ u16 uvOffset; // added to every UV halfword
    /* 0xA */ s16 color;    // grey level ORed into the colour word; under
                            // MODEL_DEPTH_CUE it feeds GTE IR0 instead, so
                            // 0x1000 blends the model fully into SetFarColor
    /* 0xC */ s16 tpage;    // packet +0x16; Akari: "blending option"
    /* 0xE */ s16 clut;     // packet clut halfword
} ModelRenderDesc;          // size:0x10

typedef struct {
    /* 0x00 */ u32 flags;
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y;
    /* 0x08 */ u16 u;
    /* 0x0A */ u16 v;
    /* 0x0C */ u16 tpage;
    /* 0x0E */ u16 clut;
    /* 0x10 */ u8 w;
    /* 0x11 */ u8 uw;
    /* 0x12 */ u8 h;
    /* 0x13 */ u8 vh;
} SpriteQuad; // size:0x14

typedef struct {
    /* 0x0 */ s16 unk0;
    /* 0x2 */ s16 quadCount;
    /* 0x4 */ SpriteQuad quads[1];
} SpriteFrame;

typedef struct {
    /* 0x0 */ s32 unk0;
    /* 0x4 */ s32 frameCount;
    /* 0x8 */ SpriteFrame frames[1];
} SpriteAnim;

// Textured-quad descriptor read by func_800D4D90; ROM instances are packed
// 0xC apart. Akari: BATTLE.X_units_functions.cpp, "funcd4d90".
typedef struct {
    /* 0x0 */ SpriteAnim* frames;
    /* 0x4 */ CVECTOR color;  // packet colour word; cd is 0x2C or 0x2E (POLY_FT4)
    /* 0x8 */ u16 frameIndex; // blocks skipped; bit 15 enables clutBias
    /* 0xA */ s16 clutBias;   // added to each quad's clut halfword
} SpriteRenderDesc;           // size:0xC

// Sprite descriptor read by BattleEffectSpriteAdd.
typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ u8 u;
    /* 0x5 */ u8 v;
    /* 0x6 */ u8 w;
    /* 0x7 */ u8 h;
    /* 0x8 */ u8 r;
    /* 0x9 */ u8 g;
    /* 0xA */ u8 b;
    /* 0xB */ u8 code;
    /* 0xC */ u16 tpage; // POLY_FT4 tpage halfword
    /* 0xE */ u16 clut;  // POLY_FT4 clut halfword
} BattleSpriteDesc;      // size:0x10

typedef struct {
    /* 0x00 */ u16 varSpeed;
    /* 0x02 */ u16 atbIncrement;
    /* 0x04 */ u16 atbGauge;     // ATB fill gauge, saturates/compares at 0xFFFF -- unsigned
    /* 0x06 */ s16 hpChangeRate; // HP per 4 var ticks: +maxHP/32 Regen, -maxHP/32 Dual Drain
    /* 0x08 */ s16 limitSpeedFlag;
    /* 0x0A */ u16 constSpeed;
    /* 0x0C */ u8 senseTargetMask;
    /* 0x0D */ u8 accessoryEffectId;
    /* 0x0E */ u8 menuListFlags;
    /* 0x0F */ u8 formationIndex;
    /* 0x10 */ u8 statusTimers[NUM_STATUS_TIMERS];
    /* 0x20 */ s8 statMults[NUM_STAT_MULTS];
    /* 0x28 */ u8 unk28;
    /* 0x29 */ u8 turnFlags;
    /* 0x2A */ u8 unk2A;
    /* 0x2B */ u8 deathEffectState;
    /* 0x2C */ s32 action09Data1;
    /* 0x30 */ s32 action09Data2;
    /* 0x34 */ s32 statusProtectionMask;
    /* 0x38 */ SceneEnemy* enemy;
    /* 0x3C */ u16 prevHP;
    /* 0x3E */ u16 prevMP;
    /* 0x40 */ s32 unk40;
} BattleTurnWork; // 0x44

/* one battle-usable item in the in-battle item list (built from the inventory
   by BATINI; counts are committed back when the battle ends) */
typedef struct {
    /* 0x0 */ u16 id;
    /* 0x2 */ u8 count;
    /* 0x3 */ u8 targetFlags;
    /* 0x4 */ u8 unk4;
    /* 0x5 */ u8 unk5;
} BattleItemEntry; /* size: 0x6 */

// Written by BattleInitEnemyUnits, seems to track attacks usable by enemies under manipulation
typedef struct {
    /* 0x0 */ u8 attackIndex;
    /* 0x1 */ u8 unk1;
    /* 0x2 */ u8 targetFlags;
    /* 0x3 */ u8 unk3;
    /* 0x4 */ u8 unk4[2];
} Unk80166F78; // size: 0x6

typedef struct {
    /* 0x00 */ SavePartyMember* partyMember;
    /* 0x04 */ u8 limitCount; // inferred: bumped when a Limit Break executes
    /* 0x05 */ u8 limitLevel;
    /* 0x06 */ u8 unk6;
    /* 0x07 */ u8 killCount; // enemy kills this battle; reconciled into
                             // SavePartyMember.kill_count after battle
                             // (BATRES.X, undecompiled)
    /* 0x08 */ u16 limitBar;
    /* 0x0A */ u16 limitBarUI;
    /* 0x0C */ u16 curMP;
    /* 0x0E */ u16 curHP;
    /* 0x10 */ u16 maxMP;
    /* 0x12 */ u16 maxHP;
    /* 0x14 */ u16 capMP;
    /* 0x16 */ u16 capHP;
    /* 0x18 */ u16 unk18[2];
    /* 0x1C */ u32 limitBreakHPDivisor;
    /* 0x20 */ u32 accessoryStatusMask; // status mask granted by the equipped accessory
    /* 0x24 */ u32 enemySkillMateriaData;
    /* 0x28 */ u32 enemySkillMateriaData2;
    /* 0x2C */ u16 unk2C;
    /* 0x2E */ u16 unk2E;
    /* 0x30 */ u16 unk30;
    /* 0x32 */ u16 unk32;
} BattlePartyWork; // size:0x34

typedef struct {
    /* 0x00 */ u8 targetFlags;
    /* 0x01 */ u8 attackEffectId;
    /* 0x02 */ u8 damageFormulaId;
    /* 0x03 */ u8 hitChance;
    /* 0x04 */ u8 impactEffectId;
    /* 0x05 */ u8 criticalHitChance;
    /* 0x06 */ u8 unk06;
    /* 0x07 */ u8 unk07;
    /* 0x08 */ s16 attackSound[3]; // [0] normal hit, [1] critical, [2] miss; 0x100 set from WeaponRecord.soundIdMask
    /* 0x0E */ u16 attackElement;
    /* 0x10 */ u16 cameraMovementId;
    /* 0x12 */ u16 specialAttackFlags;
    /* 0x14 */ s32 attackStatusMask;
} BattleUnitAttackSetup; // size:0x18

typedef struct {
    /* 0x0000 */ BattleTurnWork turn[NUM_BATTLE_ACTOR];
    /* 0x02A8 */ BattlePartyWork party[NUM_PARTY];
    /* 0x0344 */ BattleUnitAttackSetup setup[NUM_PARTY];
    /* 0x038C */ SceneEnemy enemy[3];
    /* 0x05B4 */ AttackData attacks[0x20];
    /* 0x0934 */ u16 attackIDs[0x20];
    /* 0x0974 */ u8 attackNames[0x20][0x20];
    /* 0x0D74 */ BattleActionEntry currentAction;
    /* 0x0D7C */ BattleActionEntry actionQueue[64];
    /* 0x0F7C */ BattleActionEntry subActionSlots[10];
    /* 0x0FCC */ BattleActionEntry postExecAction[2];
    /* 0x0FDC */ u8 nextOrderToExecute[7];
    /* 0x0FE3 */ u8 nextOrderToAssign[7];
    /* 0x0FEA */ u8 unkFEA[2];
    /* 0x0FEC */ FormationAIScripts formationAI;
    /* 0x11EC */ u8 aiScriptBuffer[0x1000];
    /* 0x21EC */ u16 configFlags;
    /* 0x21EE */ u16 battleSpeed;
    /* 0x21F0 */ u16 avgPartyDexterity;
    /* 0x21F2 */ u16 atbWaitMode;
    /* 0x21F4 */ u16 activeUnitCmdMask;
    /* 0x21F6 */ u16 escapeHoldTicks;
    /* 0x21F8 */ u16 escapeTicks;
    /* 0x21FA */ u16 escapeCounter1;
    /* 0x21FC */ u16 escapeSubCounter;
    /* 0x21FE */ u16 escapeCounter2;
    /* 0x2200 */ u16 isQueueExecuting;
    /* 0x2202 */ u16 currentQueuePriority;
    /* 0x2204 */ u16 activeScriptMask;
    /* 0x2206 */ u16 lucky7777StringID;
    /* 0x2208 */ u16 lucky7777ActionParam;
    /* 0x220A */ u16 turnReadyUnitMask;
    /* 0x220C */ u16 disabledUnitMask;
    /* 0x220E */ u16 imprisonedType;
    /* 0x2210 */ u16 encounterType;
    /* 0x2212 */ u16 manipulatedUnitMask;
    /* 0x2214 */ u16 unk2214;
    /* 0x2216 */ u16 petrifiedMask;
    /* 0x2218 */ u16 frameRnd; // Random u16 roll every frame, used for steal and escape
    /* 0x221A */ u16 reviveMask;
    /* 0x221C */ u8 enemyTypeCount[3];
    /* 0x221F */ u8 pad221F;
    /* 0x2220 */ u16 attackerUnitMask;
    /* 0x2222 */ u16 playerContextOffset[2];
    /* 0x2226 */ s16 pendingActionPriority;
} BattleWork; // size:0x2228

typedef struct Unk800A8D04 {
    /* 0x000 */ s32 actorId;
    /* 0x004 */ s32 characterLevel;
    /* 0x008 */ s32 unk8;
    /* 0x00C */ s32 queuedCmdIndex;
    /* 0x010 */ s32 relativeActionIndex; // index within its own category (spell #, summon
                                         // #, etc) - see MAGIC_START in game.h for range defines
    /* 0x014 */ s32 enabledMagicIndex;   // -1 means unset
    /* 0x018 */ s32 allowedTargetsMask;
    /* 0x01C */ s32 unk1C;
    /* 0x020 */ s32 unk20; // pending message/animation id, -1 = none
    /* 0x024 */ s32 unk24;
    /* 0x028 */ s32 cmdIndex;
    /* 0x02C */ s32 absoluteActionIndex; // relativeActionIndex remapped into the single
                                         // shared spell/summon/enemy-skill/limit name
                                         // table (kernel.bin section 18) - see MAGIC_START
                                         // in game.h for range defines
    /* 0x030 */ s32 unk30;
    /* 0x034 */ u8 unk34[4]; // character spacing array
    /* 0x038 */ s32 mpCost;
    /* 0x03C */ s32 attackPercent;
    /* 0x040 */ s32 formulaIds;
    /* 0x044 */ s32 elements;
    /* 0x048 */ s32 power;
    /* 0x04C */ s32 attackStat;
    /* 0x050 */ s32 targetFlags;
    /* 0x054 */ s32 impactSfxIds[3];
    /* 0x060 */ s32 unk60;
    /* 0x064 */ s32 unk64;
    /* 0x068 */ s32 impactEffectId;
    /* 0x06C */ s32 attackFlags;
    /* 0x070 */ s32 unk70;
    /* 0x074 */ s32 unk74;
    /* 0x078 */ s32 unk78;
    /* 0x07C */ s32 unk7C;
    /* 0x080 */ s32 unk80[3];
    /* 0x08C */ s32 statusChangeChance;
    /* 0x090 */ s32 unk90;
    /* 0x094 */ s32 unk94;
    /* 0x098 */ s32 unk98;
    /* 0x09C */ s32 unk9C;
    /* 0x0A0 */ s32 hitFormulaId;
    /* 0x0A4 */ s32 dmgFormulaId;
    /* 0x0A8 */ s32 unkA8;
    /* 0x0AC */ s32 unkAC;
    /* 0x0B0 */ s32 unkB0;
    /* 0x0B4 */ s32 unkB4;
    /* 0x0B8 */ s32 unkB8;
    /* 0x0BC */ s32 effectId;
    /* 0x0C0 */ s32 effectModifier;
    /* 0x0C4 */ s32 unkC4;
    /* 0x0C8 */ s32 attackerStatus;
    /* 0x0CC */ s32 unkCC;
    /* 0x0D0 */ u8 unkD0[8];
    /* 0x0D8 */ s32 unkD8;
    /* 0x0DC */ s32 unkDC;
    /* 0x0E0 */ s32 landedHitCount;
    /* 0x0E4 */ s32 unkE4;
    /* 0x0E8 */ s32 unkE8;
    /* 0x0EC */ s32 unkEC;
    /* 0x0F0 */ s32 unkF0;
    /* 0x0F4 */ s32 unkF4;
    /* 0x0F8 */ s32 unkF8;
    /* 0x0FC */ s32 unkFC;
    /* 0x100 */ s32 unk100[0x40];
    /* 0x200 */ BattleTurnWork* turnWork;
    /* 0x204 */ BattlePartyWork* partyWork;
    /* 0x208 */ s32 targetId;
    /* 0x20C */ s32 targetEnemyId;
    /* 0x210 */ s32 targetDefense;
    /* 0x214 */ s32 tmpDamage;
    /* 0x218 */ s32 unk218;
    /* 0x21C */ s32 unk21C;
    /* 0x220 */ s32 damageFlags;
    /* 0x224 */ s32 hurtAnimScript;
    /* 0x228 */ u32 targetStatus;
    /* 0x22C */ s32 targetProtectionStatus;
    /* 0x230 */ s32 affinityFlags;
    /* 0x234 */ s32 unk234;
    /* 0x238 */ s32 unk238[3];
    /* 0x244 */ s32 unk244;
    /* 0x248 */ s32 targetImpactSfxId;
    /* 0x24C */ s32 targetImpactEffectId;
    /* 0x250 */ s32 unk250;
    /* 0x254 */ s32 targetLevel;
    /* 0x258 */ s32 targetHP;
    /* 0x25C */ s32 targetMP;
    /* 0x260 */ s32 targetAttackPercent;
} Unk800A8D04; // size: 0x264

extern s16 D_800F5B74;
extern BattleWork g_BattleWork;
extern u16 D_800F7DE8;
extern u8 g_EncounterType;
extern BattleState g_BattleState;
extern BattleWorldView g_BattleWorldView;
extern DB* g_cDb;
extern s32 g_dbIndex;
extern short g_BattleEffectCursor;
extern s16 g_BattleCurrentTargetMask;
extern BattleModel g_BattleModels[NUM_BATTLE_ACTOR];
extern short g_BattleEffectCount;
extern s32 D_801620A8;
extern BattleData g_BattleData;
// pending battle-start status flags, one bit per entry of D_801B001C / D_801B002C (bit 4 = full-heal)
extern u8 g_BattleStartFXFlags;
extern s32 D_800F87F0[NUM_BATTLE_ACTOR][32]; // per-combatant battle-script variable bank, 0x80 B
                                             // each (BattleOpcodeValOffs)
extern Unk80166F78 D_80166F78[NUM_ENEMY][16];

// Scratch copy of a party member's save record, taken when g_BattleData.flags bit EVENT_BATTLE_SQUARE is set.
extern SavePartyMember D_80167938;

s32 BattleEffectRegister(void (*callback)(void));
void BattleSetLoadTimToVram(u_long* addr, s16 imgXY, s16 clutX, s16 clutY);
void BattleEnqueueClearImage(RECT* rect, s32 arg1, s32 arg2, s32 arg3);
void* func_800D29D4(ModelRenderDesc*, u_long**, int, void*);
// Build the model matrix for a battle effect: `scale` goes on the matrix
// diagonal, `pos` is transformed into view space to become the translation,
// and `depthBias` nudges it along that view vector (negative pulls it toward
// the camera). Leaves the result installed as the rot/trans matrix.
MATRIX* BattleSetBillboardMatrix(SVECTOR* pos, s32 scale, s32 depthBias);
void* func_800D4D90(SpriteRenderDesc* desc, u_long** ot, int otLen, void* prim);
void* BattleEffectSpriteAdd(BattleSpriteDesc* desc, u_long** ot, int otLen, void* prim);
void func_800D5444(int, int, int, void (*func)(int));
// Returns a scale derived from the target's model size.
s32 func_800D55A4(s32 target);
void BattleAkaoCommand(s32 cmdId, ...);
void BattleGetPartPosition(s32 actor, s32 bone, SVECTOR* pos);
SVECTOR* BattleEntityGetCenter(s32 targetMask, SVECTOR* center);
enum BattleEventType {
    BATTLE_EVENT_EFFECT_MODEL_START = 1,
    BATTLE_EVENT_EFFECT_MODEL_END = 2,
};
s16* BattleEventQueuePush(s32 type);
// Runs `func` once per set bit in targetMask, frameStep frames apart.
void MagicAnimationRegister(s32 targetMask, s32 callbackArg, s32 frameStep, void (*func)(s32, s32));
s32 BattlePositionToStereoPan(SVECTOR* sv);
s32 BattleEntityGetStereoPan(s32 arg0);
void func_800D5774(u32 targetIndex);
void BattleRunFrame(void);
void BattleQueueEvent(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void BattleUnitCalcSpeed(s32 index);
void BattleUpdateUnitMasks(void);
void BattleCheckAllLucky7s(void);
void BattleActionQueueReset(void);
void BattleCmdScriptInitTbl(void);
void BattleHitFormulaInit(void);
void BattleEventQueueInit(void);
void BattleBannerSetEncounterString(s16 stringId);
void BattleResetReservedItems(void);
void BattleExecFormationAIScripts(void);
void BattleInitTurnWorkHPMP(void);
void BattleAddAutoBattleActionByChance(s32 arg0, s32 mode);
void BattleInitUnitAction(s32 index);
void BattleEnableLimitToPlayerWithSpeed(s32 index);
s32 BattleCopyMessageWithArgs(u8* dst, const u8* src, const u16* args);
s8* BattleGetStringPtrFromStringBuffer(s32 arg0);

#endif
