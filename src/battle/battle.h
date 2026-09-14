#include <game.h>

#define NUM_PARTY (3)
#define START_ENEMY (NUM_PARTY + 1)
#define NUM_ENEMY (6)
#define NUM_BATTLE_ACTOR (START_ENEMY + NUM_ENEMY) // 10

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/BattleFlags.cs#L4
typedef enum {
    SETUP_CANNOT_ESCAPE = 4,
    SETUP_NO_VICTORY_POSE = 8,
    SETUP_NO_PREEMPTIVE_STRIKE = 0x10,
} BattleSetupFlags;

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

// full standard FF7 status bitmask (wiki.ffrtt.ru/FF7/Battle/Status_Effects)
// for BattleUnit.status. NOTE: the standard table's bit 0x2 (Near Death)
// does not appear to be set here -- this engine computes Near Death live
// from curHP/maxHP instead (see func_800B10B4), not via this flag. Bits
// actually confirmed live in decompiled code so far: Death, Confu, Silence,
// Frog, Small, Petrify, D.Sentence, Manipulate, Berserk, Peerless -- the
// rest are the documented table entries, not yet independently confirmed
// in this engine's own code.
typedef enum {
    STATUS_DEATH = 0x1,
    STATUS_NEAR_DEATH = 0x2,
    STATUS_SLEEP = 0x4,
    STATUS_POISON = 0x8,
    STATUS_SADNESS = 0x10,
    STATUS_FURY = 0x20,
    STATUS_CONFU = 0x40,
    STATUS_SILENCE = 0x80,
    STATUS_HASTE = 0x100,
    STATUS_SLOW = 0x200,
    STATUS_STOP = 0x400,
    STATUS_FROG = 0x800,
    STATUS_SMALL = 0x1000,
    STATUS_SLOW_NUMB = 0x2000,
    STATUS_PETRIFY = 0x4000,
    STATUS_REGEN = 0x8000,
    STATUS_BARRIER = 0x10000,
    STATUS_M_BARRIER = 0x20000,
    STATUS_REFLECT = 0x40000,
    STATUS_DUAL = 0x80000,
    STATUS_SHIELD = 0x100000,
    STATUS_D_SENTENCE = 0x200000,
    STATUS_MANIPULATE = 0x400000,
    STATUS_BERSERK = 0x800000,
    STATUS_PEERLESS = 0x1000000,
    STATUS_PARALYSIS = 0x2000000,
    STATUS_DARKNESS = 0x4000000,
    STATUS_DUAL_DRAIN = 0x8000000,
    STATUS_DEATH_FORCE = 0x10000000,
    STATUS_RESIST = 0x20000000,
    STATUS_LUCKY_GIRL = 0x40000000,
    STATUS_IMPRISONED = 0x80000000,
} BattleStatusFlags;

typedef struct {
    /* 0x0 */ u16 isMultiBattle;
    /* 0x2 */ u16 characterMask[NUM_PARTY];
} BattleMultiInfo; /* size = 0x8 */

typedef struct {
    // condition/status bitmask; see BattleStatusFlags above for the bits
    // confirmed live here
    /* 0x00 */ s32 status;
    /* 0x04 */ u32 stateFlags; 
    /* 0x08 */ s8 actorId;
    /* 0x09 */ u8 level;
    /* 0x0A */ s8 targetSubSlot;
    /* 0x0B */ s8 minElemInfluence;
    /* 0x0C */ s8 formationIndex;
    /* 0x0D */ u8 physAttack;
    /* 0x0E */ s8 magAttack;
    /* 0x0F */ s8 physEvade;
    /* 0x10 */ s8 idleActionId; 
    /* 0x11 */ u8 hurtActionId;
    /* 0x12 */ s8 backDamageMult;
    /* 0x13 */ s8 rowFlags;
    /* 0x14 */ s8 dexterity;
    /* 0x15 */ u8 luck;
    /* 0x16 */ s8 unk16;
    /* 0x17 */ u8 coverTargetSlot;
    /* 0x18 */ u16 attackMask;
	/* 0x1A */ u16 attackerMask;
    /* 0x1C */ s32 statusAppliedMask;
    /* 0x20 */ s16 physDefence;
    /* 0x22 */ s16 magDefence;
    /* 0x24 */ s16 enemyId;
	/* 0x26 */ u16 elemAbsorbExtra;
    /* 0x28 */ s16 curMP;
    /* 0x2A */ s16 maxMP;
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
	/* 0x50 */ u16 unk50;
    /* 0x52 */ u16 unk52;
    /* 0x54 */ u16 elemImmuneExtra;
    /* 0x56 */ u16 ap;
    /* 0x58 */ u32 gil;
    /* 0x5C */ u32 exp;
    /* 0x60 */ u32 unk60[2];
} BattleUnit; // size:0x68

typedef struct {
    /* 0x000 */ u16 unk0;
    /* 0x002 */ u16 presentMask; // D_800F83AE: bit per combatant present
    /* 0x004 */ u8 unk4[2];      // D_800F83B0
    /* 0x006 */ u16 unk6;        // D_800F83B2
    /* 0x008 */ u8 unk8[8];      // D_800F83B4..D_800F83BC
    /* 0x010 */ u16 unk10;       // D_800F83BC
    /* 0x012 */ u16 unk12;       // D_800F83BE
    /* 0x014 */ u8 unk14[4];     // D_800F83C0
    /* 0x018 */ u16 unk18;       // D_800F83C4
    /* 0x01A */ u16 unk1A;       // D_800F83C6
    /* 0x01C */ u16 unk1C;       // D_800F83C8
    /* 0x01E */ u16 unk1E;       // D_800F83CA
    /* 0x020 */ u16 sceneID;     // D_800F83CC
    /* 0x022 */ u16 unk22;       // D_800F83CE
    /* 0x024 */ u16 setupFlags;  // D_800F83D0: BattleSetupFlags
    /* 0x026 */ u16 unk26;       // D_800F83D2
    /* 0x028 */ u16 unk28;       // D_800F83D4
    /* 0x02A */ u8 unk2A[0xA];   // D_800F83D8..D_800F83DC
    /* 0x034 */ BattleUnit combatant[NUM_BATTLE_ACTOR];
} BattleState; // size:0x444

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/SceneEditor/BattleSetupData.cs
typedef struct {
    /* 0x00 0x80163614 */ u16 stageID; // load STAGE/ files
    /* 0x02 0x80163616 */ s16 nextStageID;
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
    /* 0x28 */ u8 elementTypes[8];
    /* 0x30 */ u8 elementRates[8];
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
    /* 0x000 */ u16 enemyModelIDs[4];
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
    /* 0x00 */ u16 enemyModelIDs[4];
    /* 0x08 */ BattleSetup setup;
    /* 0x1C */ CameraPlacement camera[4];
    /* 0x4C */ FormationEntry formation[NUM_ENEMY];
} ActiveEncounterData; // size:0xAC

typedef struct {
    u8 priority;
    s8 orderInPriority;
    s8 unitID;
    u8 actionType;
    s16 attackIndex;
    s16 targetMask;
} BattleActionEntry; // size:8

typedef struct {
    /* 0x0000 */ SceneEnemy enemy[3];
    /* 0x0228 */ AttackData attacks[0x20];
    /* 0x05A8 */ u16 attackIDs[0x20];
    /* 0x05E8 */ u8 attackNames[0x20][0x20];
    /* 0x09E8 */ BattleActionEntry currentAction;
    /* 0x09F0 */ BattleActionEntry actionQueue[64];
    /* 0x0BF0 */ BattleActionEntry subActionSlots[10];
    /* 0x0C40 */ BattleActionEntry postExecAction[2];
    /* 0x0C50 */ u8 partySlotMap[6];
    /* 0x0C56 */ u8 activeTargetSlot;
    /* 0x0C57 */ u8 enemySlotMap[6];
    /* 0x0C5D */ u8 cursorFocusSlot;
    /* 0x0C5E */ u8 unkC5E[2];
    /* 0x0C60 */ FormationAIScripts formationAI;
    /* 0x0E60 */ u8 aiScriptBuffer[0x1000];
    /* 0x1E60 */ u16 configFlags;
    /* 0x1E62 */ u16 battleSpeed;
    /* 0x1E64 */ u16 avgPartyDexterity;
    /* 0x1E66 */ u16 atbWaitMode;
    /* 0x1E68 */ u16 activeUnitCmdMask;
    /* 0x1E6A */ u16 pad1E6A;
    /* 0x1E6C */ u16 pad1E6C;
    /* 0x1E6E */ u16 escapeCounter1;
    /* 0x1E70 */ u16 escapeSubCounter;
    /* 0x1E72 */ u16 escapeCounter2;
    /* 0x1E74 */ u16 isQueueExecuting;
    /* 0x1E76 */ u16 currentQueuePriority;
    /* 0x1E78 */ u16 activeScriptMask;
    /* 0x1E7A */ u16 lucky7777StringID;
    /* 0x1E7C */ u16 lucky7777ActionParam;
    /* 0x1E7E */ u16 turnReadyUnitMask;
    /* 0x1E80 */ u16 disabledUnitMask;
    /* 0x1E82 */ u16 imprisonedType;
    /* 0x1E84 */ u16 encounterType;
    /* 0x1E86 */ u16 manipulatedUnitMask;
    /* 0x1E88 */ u16 unk1E88;
    /* 0x1E8A */ u16 petrifiedMask;
    /* 0x1E8C */ u16 unk1E8C;
    /* 0x1E8E */ u16 reviveMask;
    /* 0x1E90 */ u8 enemyTypeCount[3];
    /* 0x1E93 */ u8 pad1E93;
    /* 0x1E94 */ u16 attackerUnitMask;
    /* 0x1E96 */ u16 playerContextOffset;
    /* 0x1E98 */ u16 pad1E98;
    /* 0x1E9A */ s16 pendingActionPriority;
} BattleSceneContext; // size:0x1E9C

#define BATTTLE_SCENE 7

typedef struct {
    MATRIX m;
    SVECTOR sv1;
    SVECTOR sv2;
    MATRIX* pm;
} BattleModelSub; // size:0x34

typedef struct {
    /* 0x000 */ s16 D_801518E4;
    /* 0x002 */ s16 D_801518E6;
    /* 0x004 */ s16 D_801518E8;
    /* 0x006 */ s16 D_801518EA;
    /* 0x008 */ u8 unk8[8];
    /* 0x010 */ s16 unk10;
    /* 0x012 */ s16 unk12;
    /* 0x014 */ u16 unk14[2];
    /* 0x018 */ s16 D_801518FC;
    /* 0x01A */ s16 D_801518FE;
    /* 0x01C */ s32 D_80151900;
    /* 0x020 */ s16 D_80151904;
    /* 0x022 */ u8 D_80151906;
    /* 0x023 */ u8 D_80151907;
    /* 0x024 */ u8 D_80151908;
    /* 0x025 */ u8 D_80151909;
    /* 0x026 */ u8 D_8015190A;
    /* 0x027 */ u8 D_8015190B;
    /* 0x028 */ u8 D_8015190C;
    /* 0x029 */ u8 D_8015190D;
    /* 0x02A */ u8 D_8015190E;
    /* 0x02B */ u8 D_8015190F;
    /* 0x02C */ s32 D_80151910;
    /* 0x030 */ s32 D_80151914;
    /* 0x034 */ s16 D_80151918;
    /* 0x036 */ s16 D_8015191A;
    /* 0x038 */ s16 D_8015191C;
    /* 0x03A */ s8 D_8015191E;
    /* 0x03B */ s8 D_8015191F;
    /* 0x03C */ u8 D_80151920;
    /* 0x03D */ s8 D_80151921;
    /* 0x03E */ s8 D_80151922;
    /* 0x03F */ s8 D_80151923;
    /* 0x040 */ s32 D_80151924;
    /* 0x044 */ s32 D_80151928;
    /* 0x048 */ s32 D_8015192C;
    /* 0x04C */ s32 D_80151930;
    /* 0x050 */ s32 D_80151934;
    /* 0x054 */ s32 D_80151938;
    /* 0x058 */ s32 D_8015193C;
    /* 0x05C */ s32 D_80151940;
    /* 0x060 */ s32 D_80151944;
    /* 0x064 */ s32 D_80151948;
    /* 0x068 */ s32 D_8015194C;
    /* 0x06C */ s32 D_80151950;
    /* 0x070 */ s32 D_80151954;
    /* 0x074 */ s32 D_80151958;
    /* 0x078 */ u8 unk5C[0xC8];
    /* 0x140 */ MATRIX m;
    /* 0x160 */ SVECTOR unk160;
    /* 0x168 */ SVECTOR D_80151A4C;
    /* 0x170 */ s32 D_80151A54;
    /* 0x174 */ BattleModelSub D_80151A58[50];
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
    /* 0x20 */ SVECTOR sv;
    union {
        /* 0x28 */ VECTOR v;
        struct {
            /* 0x28 */ SVECTOR sv2;
            /* 0x30 */ s32 unk30;
            /* 0x34 */ u8 unk34;
            /* 0x35 */ s8 unk35;
            /* 0x36 */ s16 unk36;
        } sub;
    } u;

} Unk800BB75C; // size:0x38

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
    /* 0x8 */ u16 uvOffset; // added to every UV halfword; all callers pass 0
    /* 0xA */ s16 color;    // grey level ORed into the colour word; under
                            // MODEL_DEPTH_CUE it feeds GTE IR0 instead, so
                            // 0x1000 blends the model fully into SetFarColor
    /* 0xC */ s16 tpage;    // packet +0x16; Akari: "blending option"
    /* 0xE */ s16 clut;     // packet clut halfword
} ModelRenderDesc;          // size:0x10

// Textured-quad descriptor read by func_800D4D90; ROM instances are packed
// 0xC apart. Akari: BATTLE.X_units_functions.cpp, "funcd4d90".
typedef struct {
    /* 0x0 */ s32* frames;    // per-frame quad blocks, count in each header
    /* 0x4 */ CVECTOR color;  // packet colour word; cd is 0x2C or 0x2E (POLY_FT4)
    /* 0x8 */ u16 frameIndex; // blocks skipped; bit 15 enables clutBias
    /* 0xA */ s16 clutBias;   // added to each quad's clut halfword
} SpriteRenderDesc;           // size:0xC

typedef struct {
    s16 unk0;
    s16 unk2;
    u16 unk4; // ATB fill gauge, saturates/compares at 0xFFFF -- unsigned
    s16 unk6;
    s32 unk8;
    u8 unkC;
    u8 unkD; // effect id of the equipped accessory (0xFF = none)
    u8 unkE;
    u8 unkF;
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;
    u8 unk14[4];
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    u8 unk28;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    u16 unk3C;
    u16 unk3E;
    s32 unk40;
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
    /* 0x18 */ u16 unk18;
    /* 0x1A */ u16 unk1A;
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
    /* 0x00 */ s8 D_801636B8;
    /* 0x01 */ u8 D_801636B9;
    /* 0x02 */ s8 D_801636BA;
    /* 0x03 */ s8 D_801636BB;
    /* 0x04 */ u8 D_801636BC;
    /* 0x05 */ s8 D_801636BD;
    /* 0x06 */ s16 D_801636BE;
    /* 0x08 */ s32 D_801636C0;
    /* 0x0C */ s32 D_801636C4;
} Unk801636B8; // size:0x10

typedef struct {
    /* 0x00 */ u8 targetFlags;
    /* 0x01 */ u8 attackEffectId;
    /* 0x02 */ u8 damageFormulaId;
    /* 0x03 */ u8 hitChance;
    /* 0x04 */ u8 impactEffectId;
    /* 0x05 */ u8 criticalHitChance;
    /* 0x06 */ u8 unk06;
    /* 0x07 */ u8 unk07;
    /* 0x08 */ u16 normalAttackSound;
    /* 0x0A */ u16 criticalAttackSound;
    /* 0x0C */ u16 missAttackSound;
    /* 0x0E */ u16 attackElement;
    /* 0x10 */ u16 cameraMovementId;
    /* 0x12 */ u16 specialAttackFlags;
    /* 0x14 */ s32 attackStatusMask;
} BattleUnitAttackSetup; // size:0x18

typedef struct {
    /* 0x000 */ BattleTurnWork turn[NUM_BATTLE_ACTOR];
    /* 0x2A8 */ BattlePartyWork party[NUM_PARTY];
    /* 0x344 */ BattleUnitAttackSetup setup[NUM_PARTY];
} BattleWork; // size:0x38C

extern s16 D_800F5B74;
extern BattleWork g_BattleWork;
extern BattleSceneContext g_BattleSceneContext;
extern u16 D_800F7DE8;
extern u8 g_EncounterType;
extern BattleState g_BattleState;
extern Unk800BB75C D_800FA63C;
extern DB* g_cDb;
extern s32 g_dbIndex;
extern short g_BattleEffectCursor;
extern s16 D_80151774;
extern BattleModel D_801518E4[NUM_BATTLE_ACTOR];
extern short g_BattleEffectCount;
extern s32 D_801620A8;
extern ActiveEncounterData g_ActiveEncounter;
extern Unk801636B8 D_801636B8[NUM_BATTLE_ACTOR];
extern u16 D_8016376A;

extern BattleMultiInfo g_BattleMultiInfo;

s32 BattleEffectRegister(void (*func)(void));
void BattleSetLoadTimToVram(u_long* addr, s16 imgXY, s16 clutX, s16 clutY);
void* func_800D29D4(ModelRenderDesc*, u_long**, int, void*);
// Build the model matrix for a battle effect: `scale` goes on the matrix
// diagonal, `pos` is transformed into view space to become the translation,
// and `depthBias` nudges it along that view vector (negative pulls it toward
// the camera). Leaves the result installed as the rot/trans matrix.
MATRIX* func_800D4368(SVECTOR* pos, s32 scale, s32 depthBias);
void* func_800D4D90(SpriteRenderDesc* desc, u_long** ot, int otLen, void* prim);
void func_800D5444(int, int, int, void (*func)(int));
// Returns a scale derived from the target's model size.
s32 func_800D55A4(s32 target);
void BattleCommandSend(s32 cmdId, ...);
void BattleGetPartPosition(s32 arg0, s32 arg1, void* arg2);
// Runs `func` once per set bit in targetMask, frameStep frames apart.
void MagicAnimationRegister(s32 targetMask, s32 callbackArg, s32 frameStep, void (*func)(s32, s32));
s32 BattlePositionToStereoPan(SVECTOR* sv);
s32 BattleEntityGetStereoPan(s32 arg0);
void func_800D5774(u32 targetIndex);
void BattleRunFrame(void);
void BattleQueueEvent(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void BattleRecalcUnitSpeed(s32 index);
void BattleUpdateUnitMasks(void);
void BattleCheckAllLucky7s(void);
void BattleActionQueueReset(void);
void BattleCmdScriptInitTbl(void);
void BattleHitFormulaInit(void);
void BattleEventQueueInit(void);
void BattleBannerSetEncounterString(s16 arg0);
void BattleResetReservedItems(void);
void BattleExecFormationAIScripts(void);
void BattleInitTurnWorkHPMP(void);
void BattleAddAutoBattleActionByChance(s32 arg0, s32 arg1);
