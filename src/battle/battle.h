#include <game.h>

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
// for Unk800F83E0.status. NOTE: the standard table's bit 0x2 (Near Death)
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
    // condition/status bitmask; see BattleStatusFlags above for the bits
    // confirmed live here
    /* 0x00 */ s32 status;
    /* 0x04 */ s32 unk4; // battle-state flags (e.g. bit 0x40 = back row, bit
                         // 0x20 = defending)
    /* 0x08 */ s8 unk8;
    /* 0x09 */ s8 unk9;
    /* 0x0A */ s16 unkA;
    /* 0x0C */ s8 unkC;
    /* 0x0D */ u8 unkD;
    /* 0x0E */ s8 unkE;
    /* 0x0F */ s8 unkF;
    /* 0x10 */ s8 unk10; // cached "Near Death" display flag; see func_800B10B4
                         // for the live check
    /* 0x11 */ u8 unk11;
    /* 0x12 */ s8 unk12;
    /* 0x13 */ s8 unk13;
    /* 0x14 */ s8 unk14;
    /* 0x15 */ s8 unk15;
    /* 0x16 */ s8 unk16;
    /* 0x17 */ u8 unk17;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ s16 unk20;
    /* 0x22 */ s16 unk22;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s16 unk28;
    /* 0x2A */ s16 unk2A;
    /* 0x2C */ u32 curHP;
    /* 0x30 */ u32 maxHP;
    /* 0x34 */ u32 unk34[4];
    /* 0x44 */ u32 unk44[9];
} Unk800F83E0; // size:0x68

typedef struct {
    /* 0x000 */ u16 unk0;
    /* 0x002 */ u16 presentMask; // D_800F83AE: bit per combatant present
    /* 0x004 */ u8 unk4[0x14];   // D_800F83B0..D_800F83C0
    /* 0x018 */ u16 unk18;       // D_800F83C4
    /* 0x01A */ u16 unk1A;       // D_800F83C6
    /* 0x01C */ u16 unk1C;       // D_800F83C8
    /* 0x01E */ u16 unk1E;       // D_800F83CA
    /* 0x020 */ s16 sceneID;     // D_800F83CC
    /* 0x022 */ u16 unk22;       // D_800F83CE
    /* 0x024 */ u16 setupFlags;  // D_800F83D0: BattleSetupFlags
    /* 0x026 */ u16 unk26;       // D_800F83D2
    /* 0x028 */ u16 unk28;       // D_800F83D4
    /* 0x02A */ u8 unk2A[0xA];   // D_800F83D8..D_800F83DC
    /* 0x034 */ Unk800F83E0 combatant[10];
} BattleState; // size:0x444

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
    /* 0x02 */ u16 x, y, z;
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
} SceneEnemy; // size:0xB8

// https://github.com/petfriendamy/ff7-scarlet/blob/main/src/Shared/DataParser.cs
typedef struct {
    u8 accuracyRate;
    u8 impactEffectID;
    u8 impactAnimID;
    u8 unk3;
    u16 mpCost;
    u16 impactSfxID;
    u16 cameraSingleID;
    u16 cameraMultiID;
    u8 targetFlags;
    u8 attackEffectID;
    u8 damageCalcID;
    u8 strength;
    u8 conditionSubmenu;
    u8 statusChange;
    u8 additionalEffects;
    u8 effectsModifier;
    u32 statuses;
    u16 elements;
    u16 flags;
} AttackEntry; // size:0x1C

typedef struct {
    u16 unk0[0x100];
} Unk800F5F44_5; // size:0x200

// an uncompressed chunk from SCENE.BIN
typedef struct {
    /* 0x000 */ u16 enemyModelIDs[4];
    /* 0x008 */ BattleSetup setup[4];
    /* 0x058 */ CameraPlacement camera[4][4];
    /* 0x118 */ FormationEntry formation[4][6];
    /* 0x298 */ SceneEnemy enemy[3];
    /* 0x4C0 */ AttackEntry attacks[0x20];
    /* 0x840 */ u16 attackIDs[0x20];
    /* 0x880 */ char attackNames[0x20][0x20];
    /* 0xC80 */ Unk800F5F44_5 unkC80;
    /* 0xE80 */ u8 script[0x1000];
} SceneContainer; // 0x1E80

typedef struct {
    /* 0x00 */ u16 enemyModelIDs[4];
    /* 0x08 */ BattleSetup setup;
    /* 0x1C */ CameraPlacement camera[4];
    /* 0x4C */ FormationEntry formation[6];
} Unk8016360C; // size:0xAC

typedef struct {
    u8 unk0;
    s8 unk1;
    s8 unk2;
    u8 unk3;
    s16 unk4;
    s16 unk6;
} Unk800A3D4C; // size:8

typedef struct {
    /* 0x0000 */ SceneEnemy enemy[3];
    /* 0x0228 */ AttackEntry attacks[0x20];
    /* 0x05A8 */ u16 attackIDs[0x20];
    /* 0x05E8 */ char attackNames[0x20][0x20];
    /* 0x09E8 */ u8 unk9E8[8];
    /* 0x09F0 */ Unk800A3D4C messageQueue[64];
    /* 0x0BF0 */ u8 unkBF0[0x66];
    /* 0x0C56 */ u8 D_800F6B9A;
    /* 0x0C57 */ u8 unkC57[6];
    /* 0x0C5D */ u8 D_800F6BA1;
    /* 0x0C5E */ u8 unkC5E[2];
    /* 0x0C60 */ Unk800F5F44_5 _5;
    /* 0x0E60 */ u8 script[0x1000];
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
    /* 0x1E88 */ u8 unk1E88[0x12];
    /* 0x1E9A */ s16 D_800F7DDE;
} Unk800F5F44; // size:0x1E9C

#define BATTLE_SCENE 7

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

typedef struct {
    s32* unk0;
    s32 unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
} Unk801B0C98;

typedef struct {
    /* 0x00 */ SavePartyMember* partyMember;
    /* 0x04 */ u8 limitCount; // inferred: bumped when a Limit Break executes
    /* 0x05 */ u8 unk5;
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
    /* 0x1C */ u16 unk1C;
    /* 0x1E */ u16 unk1E;
    /* 0x20 */ u16 unk20;
    /* 0x22 */ u16 unk22;
    /* 0x24 */ u16 unk24;
    /* 0x26 */ u16 unk26;
    /* 0x28 */ u16 unk28;
    /* 0x2A */ u16 unk2A;
    /* 0x2C */ u16 unk2C;
    /* 0x2E */ u16 unk2E;
    /* 0x30 */ u16 unk30;
    /* 0x32 */ u16 unk32;
} Unk800F5E60; // size:0x34

extern u16 D_800F5BBC[10][0x22];
extern Unk800F5E60 D_800F5E60[3];
extern Unk800F5F44 D_800F5F44;
extern s8 D_800F6936[0x40][8];
extern u8 D_800F83A8;
extern BattleState g_BattleState;
extern s8 D_800F90B4[][0x240];
extern Unk800BB75C D_800FA63C;
extern DB* g_cDb;
extern short D_8015169C;
extern BattleModel D_801518E4[10];
extern short D_80162080;
extern Unk8016360C D_8016360C;
extern u16 D_8016376A;

// battle.c
int BattleEffectRegister(void (*func)(void));

// battle2.c
void* func_800D29D4(Unk801B0C98*, u_long**, int, void*);
void func_800D3994(s32 arg0, s32 arg1, void* arg2);
void func_800D5444(int, int, int, void (*func)(int));
void BattleCommandSend(void* arg0, ...);
int func_800D574C(int);
