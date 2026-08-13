// should be imported only by the BATTLE overlay, not BATINI or similar
#include "battle.h"

enum QueueMethod {
    QUEUE_LOAD_IMAGE,
    QUEUE_STORE_IMAGE,
    QUEUE_MOVE_IMAGE,
    QUEUE_CLEAR_IMAGE,
};

typedef struct {
    u8 unk[0x30];
    s32 unk30;
} Unk800BB67C;

typedef struct {
    u16 unk0;
    u8 pad[0x43E];
} Unk8009D866; // 0x440

typedef struct {
    s16 unk0;
    s16 unk2;
    u16 unk4; // ATB fill gauge, saturates/compares at 0xFFFF -- unsigned
    s16 unk6;
    s32 unk8;
    s16 unkC;
    u8 unkE;
    u8 unkF;
    s32 unk10;
    u8 unk14[4];
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    u8 unk28;
    s8 unk29;
    s16 unk2A;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    u16 unk3C;
    u16 unk3E;
    s32 unk40;
} Unk800AF470; // 0x44

typedef struct {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
    s8 unk4;
    s8 unk5;
    s16 unk6;
    s16 unk8;
    s16 unkA;
} Unk800A2F4C; // size: 0xC (confirmed by D_80163A98 - D_80163798 == 0x40 * 0xC)

typedef struct {
    s16 D_801621AC;
    s16 D_801621AE;
    s16 D_801621B0;
    s16 D_801621B2;
    s16 D_801621B4;
    s16 D_801621B6;
    s16 unk8;
    s16 unkA;
    s32 unkC;
    s32 unk10;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    s16 unk18;
    s16 unk1A;
} Unk801620AC; // size:0x20

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ s16 TargetActorIndex;
    /* 0x06 */ s16 unk6;
    // SVECTOR Pos
    /* 0x08 */ s16 D_80162980; // Pos.vx
    /* 0x0A */ s16 D_80162982; // Pos.vy
    /* 0x0C */ s16 unk8;       // Pos.vz (Which unk8 do I change?)
                               // SVECTOR Rot
    /* 0x0E */ s16 unkA;       // Rot.vx
    /* 0x10 */ s16 unkC;       // Rot.vx
    /* 0x12 */ s16 unkE;       // Rot.vx
    /* 0x14 */ s16 unk10;
    /* 0x16 */ s16 unk12;
    /* 0x18 */ u8 unk14;
    /* 0x19 */ u8 unk15;
    /* 0x1A */ s16 unk16;
    /* 0x1C */ s16 unk18;
    /* 0x1E */ s16 unk1A;
} BattleEffectInstance; // size:0x20

typedef struct {
    u16 unk0;
    s16 unk2;
} Unk80162200;
typedef union {
    u8* ptr;
    Unk80162200 unk;
} Union80162200;

typedef struct {
    s16 D_801621F0;
    s16 D_801621F2;
    s16 D_801621F4;
    s16 D_801621F6; // player idx? 0, 1 or 2. See func_800D0C80
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    Union80162200 unk10;
    s32 unk14;
    u8 unk18;
    s8 unk19;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
} Unk801621F0; // size:0x20

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
    s32 method; // enum QueueMethod
    RECT* rect;
    u_long* ptr;
    s32 x;
    s32 y;
} Unk800F01DC; // size:0x14

typedef struct {
    /* 0x00 */ s32 unk0; // frame counter?
    /* 0x04 */ s32* unk4[1];
    /* 0x08 */ BattleModelSub unk8[1];
    /* 0x3C */ s16 unk3C;
    /* 0x3E */ u8 unk3E[1];
    /* 0x3F */ u8 unk3F;
} Unk800FA6D8;

typedef struct {
    short vx, vy, vz;
} ShortVectorXYZ;

typedef struct {
    /* 0x00 */ s32 D_80151200;
    /* 0x04 */ s32 D_80151204;
    /* 0x08 */ s32 D_80151208;
    /* 0x0C */ s16 D_8015120C;
    /* 0x0E */ s16 D_8015120E;
    /* 0x10 */ s32 D_80151210;
    /* 0x14 */ s32 D_80151214;
    /* 0x18 */ s32 D_80151218;
    /* 0x1C */ s32 D_8015121C;
    /* 0x20 */ s32 D_80151220;
    /* 0x24 */ s32 D_80151224;
    /* 0x28 */ s32 D_80151228;
    /* 0x2C */ s16 D_8015122C;
    /* 0x2E */ s16 D_8015122E;
    /* 0x30 */ u16 D_80151230;
    /* 0x32 */ u8 D_80151232;
    /* 0x33 */ u8 D_80151233;
    /* 0x34 */ u8 D_80151234;
    /* 0x35 */ u8 D_80151235;
    /* 0x36 */ s16 D_80151236;
    /* 0x38 */ s16 D_80151238;
    /* 0x3A */ s16 D_8015123A;
    /* 0x3C */ s16 D_8015123C;
    /* 0x3E */ s16 D_8015123E;
    /* 0x40 */ s32 D_80151240;
    /* 0x44 */ s32 D_80151244;
    /* 0x48 */ s32 D_80151248;
    /* 0x4C */ s32 D_8015124C;
    /* 0x50 */ s32 D_80151250;
    /* 0x54 */ s32 D_80151254;
    /* 0x58 */ s32 D_80151258;
    /* 0x5C */ s32 D_8015125C;
    /* 0x60 */ s32 D_80151260;
    /* 0x64 */ s32 D_80151264;
    /* 0x68 */ s32 D_80151268;
    /* 0x6C */ s32 D_8015126C;
    /* 0x70 */ s32 D_80151270;
} Unk80151200; // size:0x74

// Confirmed live via PCSX-Redux (exec breakpoint on func_800A4350, one command
// at a time, plus direct cmdIndex injection for the remaining gaps). "All"-
// linked materia (Steal-All, Sense-All, etc) reuse their base command's
// cmdIndex -- targetMask changes, not cmdIndex. 0x0E-0x10 and past 0x1B are
// unused/no-op (injected directly, no visible effect and not reachable via
// any known menu path). 0x11 forces a melee attack ignoring weapon range
// (injected on Barret with a long-range weapon -- he closed to melee instead
// of shooting), not reachable via any menu path either -- possibly an
// internal command for a status-forced attack (Berserk?), unconfirmed.
typedef enum {
    CMD_ATTACK = 0x01,
    CMD_MAGIC = 0x02,
    CMD_SUMMON = 0x03,
    CMD_ITEM = 0x04,
    CMD_STEAL = 0x05,
    CMD_SENSE = 0x06,
    CMD_COIN = 0x07,
    CMD_THROW = 0x08,
    CMD_MORPH = 0x09,
    CMD_DEATHBLOW = 0x0A,
    CMD_MANIPULATE = 0x0B,
    CMD_MIME = 0x0C,
    CMD_ENEMY_SKILL = 0x0D,
    CMD_MELEE_ATTACK = 0x11, // ignores weapon range; not player-menu-reachable?
    CMD_CHANGE = 0x12,
    CMD_DEFEND = 0x13,
    CMD_LIMIT = 0x14, // priority-5 special case in func_800A4350
    CMD_W_MAGIC = 0x15,
    CMD_W_SUMMON = 0x16,
    CMD_W_ITEM = 0x17,
    CMD_SLASH_ALL = 0x18, // materia-granted Attack-command replacement
    CMD_2X_CUT = 0x19,    // materia-granted Attack-command replacement
    CMD_FLASH = 0x1A,     // materia-granted Attack-command replacement
    CMD_4X_CUT = 0x1B,    // materia-granted Attack-command replacement
    CMD_NONE = 0xFF,      // enemy attack / not a player-menu command
} BattleCommand;

// Queued-action entry, matches
// https://wiki.ffrtt.ru/index.php/FF7/Battle/Battle_Mechanics action-queue
// layout exactly (priority/queue-pos/actorId/cmdIndex/attackIndex/targetMask).
// D_800F3958 is a 16-entry ring buffer of these (D_800F39D8 read idx,
// D_800F39DC write idx); the wiki describes up to 64 queued actions, so this
// may be a smaller staging ring rather than the full logical queue --
// unconfirmed. Drain chain: func_800A3ED0 drains this ring into a 64-slot
// priority table (func_800A3D4C), which func_800A23E0 drains in priority
// order into func_800A1798, which runs the command as a byte-coded sequence
// of opcodes (D_800F38AC/D_800A0098/D_800E7B28), not a single switch on
// cmdIndex. Full writeup: ff7-re/reference/BATTLE_COMMAND_QUEUE.md
typedef struct {
    /* 0x0 */ u8
        priority; // 0=limits/counters, 6=player spells (see func_800A4350)
    /* 0x1 */ u8
        queuePos; // position within priority band; not set by func_800A4350
    /* 0x2 */ u8 actorId;
    /* 0x3 */ s8 cmdIndex; // BattleCommand, stored raw (not the enum type --
                           // keeps this struct's confirmed 0x8-byte layout)
    /* 0x4 */ s16 attackIndex; // command-dependent: spell id for CMD_MAGIC,
                               // damage dealt for CMD_COIN, skill id for
                               // CMD_ENEMY_SKILL, etc -- not a uniform lookup
    /* 0x6 */ u16 targetMask;
} QueuedAction; // size:0x8

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk800F57D0;

typedef struct {
    /* 0x00 */ s32 D_800F83E4;
    /* 0x04 */ u8 unk4[0x64];
} Unk800F83E4; // size:0x68

extern u8 D_800708C8[];       // kernel-region table, 0x1C-byte rows, indexed by
                              // attack/effect id
extern u8 D_800708D0[][0x1C]; // kernel-region table, indexed by
                              // attack/effect id
extern s16 D_8009D85C[];      // record fields, stride 0x440
extern s16 D_8009D85E[];
extern Unk8009D866 D_8009D866[];
extern u8 D_8009D954[]; // per-actor sub-table, 0x440 stride, 8-byte rows keyed
                        // by effect id
extern s32 D_800E7A38;
extern u8 D_800E7A48[0x10];
extern s8 D_800E7A58[];
// Cait Sith's "Slots" limit: 7 three-symbol combos (one row per combo)
// checked in order against the 3 landed reel symbols (D_80163774) -- see
// BATTLE_ResolveCaitSithSlotsResult in battle.c
extern u8 D_800E7BA4[7][3];
extern Yamada D_800E8050[];
extern VECTOR D_800E7D10;
extern VECTOR D_800E7D20;
extern Yamada D_800E8068[];
extern u8 D_800EA19C[][4];
extern s32 D_800EA258;
extern s32 D_800EA25C;
extern s32 D_800EA260;
extern s32 D_800EA50C[];
extern short D_800EEB28[9][8];
extern void (*D_800EF9D8[])(s16, u8);
extern void (*D_800EFFE0[])(s16, u8);
extern void (*D_800EFAF0[])(s16, u8);
extern void (*D_800EFBC8[])(s16, u8);
extern void (*D_800EFC28[])(s16, u8);
extern s32 (*D_800EFEA0[])(s16, u8);
extern Unk800F01DC* D_800F01DC;
extern s32 D_800F01E0;
extern s32 D_800F01E4;
extern u16 D_800F198C;
extern s32 D_800F199C;
extern u8 D_800F19A4;
extern s8 D_800F1E4F;
extern s16 D_800F1E50;
extern s32 D_800F311C;
extern s16 D_800F3122; // part of a struct?
extern s32 D_800F3138;
extern s32 D_800F313C;
extern s32 D_800F3140;
extern u8 D_800F3184[];
extern u8 D_800F332C[3][0x10];
extern s16 D_800F338C[];
extern u8 D_800F33A0[3];
extern u8 D_800F33AA;
extern s8 D_800F3468;
extern u8 D_800F381C[];
extern u8 D_800F3828[];
extern unsigned char D_800F384A[];
extern s32 D_800F38AC[];
extern u8 D_800F38A0;
extern u8 D_800F38A1;
extern s16 D_800F38A2;
extern u8 D_800F7E04[]; // part of a struct
extern u8 D_800F7ED4;
extern u8 D_800F38A7;
extern u8 D_800F389C;
extern s16 D_800F389E;
extern s16 D_800F3896;
extern s32 D_800F3944;
extern s32 D_800F3948;
extern s32 D_800F394C;
extern s32 D_800F3950;
extern s32 D_800F3954;
extern QueuedAction D_800F3958[16];
extern s32 D_800F39D8; // read index into D_800F3958
extern s32 D_800F39DC; // write index into D_800F3958
extern s32 D_800F39E0;
extern s32 D_800F39E4;
extern s32 D_800F39EC;
extern u8 D_800F39F0[][6];
extern s32 D_800F3A1C;     // write index into D_800F3A20
extern s16 D_800F3A20[16]; // ring buffer, see func_800A56B0
extern s8 D_800F3A80[];
extern u16 D_800F4280[];
typedef struct {
    u8 unk0;
    s8 unk1;
    s16 unk2;
} Unk800F4308;
extern Unk800F4308 D_800F4308[][128];
extern s32 D_800F4914[];
extern s32 D_800F4920;
extern u16 D_800F4938[];
extern s8 D_800F494C[];
extern u16 D_800F4958;
extern s16 D_800F4AD0;
extern s32 D_800F4AD4;
extern s32 D_800F4AD8;
extern DR_MODE* D_800F4AF4;
extern DR_MODE* D_800F4AF8;
extern RECT D_800F4B24;
extern RECT D_800F4B2C[];
extern RECT D_800F4B6C[];
extern Unk800F01DC D_800F4BAC[];
extern u8 D_800F514C[];
extern s8 D_800F5760;
extern u8 D_800F5764;
extern u8 D_800F5774;
extern s32 D_800F57CC;
extern Unk800F57D0* D_800F57D0;
extern u8 D_800F57D4;
extern Unk800AF470 g_CombatantTurnState[10]; // per-party-slot turn/effect state
                                             // (flags, countdown timers)
extern s8 D_800F7DE4;
extern u8 D_800F7DF4;
extern s32 D_800F7DF8[3];
typedef struct {
    /* 0x00 */ s16 D_800F7ED8;
    /* 0x02 */ s16 D_800F7EDA;
    /* 0x04 */ u8 unk4[0x24];
} Unk800F7ED8; // size:0x28

extern Unk800F7ED8 D_800F7ED8[];
extern s16 D_800F8182[];
extern s16 D_800F8360;
extern s32 g_dbIndex;
extern s16 D_800F8370;
extern u8 D_800F8374;
extern u8 D_800F837C;
extern u8 D_800F8380;
extern u8* D_800F8384[3];
extern u8* D_800F8390[3];
extern s32* D_800F839C; // CD offset?
extern u8 D_800F83A4[]; // shared battle-script variable bank (func_800B13B0)
extern u8 D_800F83A6;
extern Unk800F83E4 D_800F83E4[];
extern s32 D_800F8408;
extern u8 D_800F87F0[]; // per-combatant battle-script variable bank, 0x80 B
                        // each (func_800B13B0)
extern s8 D_800F8CF0;
extern u32 D_800F8CF4[][0x18];
extern Unk80026448 D_800F9132;
extern s32 D_800F9144;
extern s8 D_800F914E;
extern s32 D_800F9F28[]; // size is either 4 or 5
extern u8 D_800F9F34;
typedef struct {
    /* 0x0 */ s16 unk0;
    /* 0x2 */ s16 unk2;
    /* 0x4 */ s16 unk4;
    /* 0x6 */ u16 unk6;
    /* 0x8 */ u16 unk8;
    /* 0xA */ s16 unkA;
    /* 0xC */ s16 unkC;
} Unk800F9F3C; // size:0xE

extern Unk800F9F3C D_800F9F3C[];
extern u8 D_800F99E8;
extern s32 D_800F99E4;
extern u8 D_800F9D94;
extern u8 D_800F9D98;
extern u8 D_800F9D9C;
extern u16 D_800F9DA4;
extern u8 D_800F9DA8[];
extern Unk800BB75C D_800FA63C;
extern s16 D_800FA69C;
extern u8 D_800FA6A0;
extern u16 D_800FA6B8;
extern u8 D_800FA6D4;
extern Unk800FA6D8 D_800FA6D8[];
extern MATRIX D_800FA958;
extern s32 D_800FA978[];
extern s32 D_800FA9B8;
extern u16 D_800FA9BC;
extern s16 D_800FA9C4;
extern s16 D_800FA9C6;
extern s16 D_800FA9C8;
// queued-action-ish record, allocated by func_800A2FD0 (unk3 set to -1,
// marking it unassigned) and searched by func_800A34CC. Traced through
// func_800ABA68's callers (func_800AB830/func_800ABB0C, still undecompiled):
// unk0 is very likely an actorId (0-2) -- its source value independently
// indexes D_800F83E0 with the same 0x68 stride confirmed elsewhere, in both
// callers. unk1 is a second actor-related value (not always equal to unk0).
// unk3 becomes a real D_800F9F3C slot index (0-0x7F) once func_800A311C
// activates the record. unk4's bit 0x4 is checked by func_800A34CC.
typedef struct {
    /* 0x0 */ s8 unk0;
    /* 0x1 */ s8 unk1;
    /* 0x2 */ s8 unk2;
    /* 0x3 */ s8 unk3;
    /* 0x4 */ u16 unk4;
    /* 0x6 */ u8 unk6[2];
    /* 0x8 */ u32 unk8;
} Unk800FA9D0; // size:0xC

extern Unk800FA9D0 D_800FA9D0[0x80];
extern s8 D_800FA9E8;
extern u8 D_800FAFDC;
extern s16 D_800FAFD4;
extern s32 D_800FAFEC;
extern s32 D_800FAFF0;
extern DB g_db;
extern u8 D_801031E0;
extern s32 D_801031E4;
extern s16 D_801031E8;
extern u8 D_801031F0;
extern u8 D_80103200[];
extern u8 D_80130200[];
extern Unk80151200 D_80151200[3];
extern u16 D_80151694;
extern s16 D_8015169C;
extern u16 D_801516A0;
extern u8 D_801516F4;
extern u16 D_801516F8;
// per-on-screen-model position cache (10 slots). func_800B91CC writes a
// fresh (x, y) into the staging pair each update; func_800B950C promotes it
// into the committed (prevX, prevY) pair. Two decoupled consumers then read
// the COMMITTED pair on their own schedule: func_800BBA84/func_800C2FD4
// derive a positional-audio parameter from prevX (feeds a sound-queue call
// via SystemAkaoExecute), and func_800DBC18 folds prevY (low bit masked) into
// limit-gauge draw positioning. func_800C2864 reads the staging pair
// directly (with small centering offsets) for an on-screen draw call.
typedef struct {
    /* 0x0 */ s16 prevX;
    /* 0x2 */ s16 prevY;
    /* 0x4 */ u16 x;
    /* 0x6 */ s16 y;
} ModelScreenPos; // size:0x8

extern ModelScreenPos g_modelScreenPos[10];
extern s16 D_80151774;
extern u8 D_801517BC;
extern u8 D_801517C4;
extern s16 D_80158D00;
extern s32 D_80158D08;
extern u_long D_80158D0C[];
extern u8 D_801518DC;
extern s32 D_800F9780[];
extern u8 D_80153BDD;
extern u32 D_80151840;
extern u8 D_801590CC;
extern s16 D_801590D0;
extern s16 D_801590D4;
extern u8 D_801590D8;
extern u8 D_801590DC;
extern u8 D_801590E0;
extern s16 D_80162080;
extern s16 D_80162084;
extern s8 D_80162094;
extern u8 D_801620A0;
extern u8 D_801620A4;
extern Unk801620AC D_801620AC[10];
extern Unk801621F0 D_801621F0[60];
extern u8 D_80162974;
extern BattleEffectInstance g_BattleEffectInstances[100];
extern u8 D_801635FC;
extern u8 D_80163604;
extern s16 D_80163608;
extern Unk801636B8 D_801636B8[5];
extern u16 D_80163758[]; // part of a struct
extern u16 D_8016375C;
extern u16 D_8016375E;
extern u16 D_80163762;
// Cait Sith's 3 landed Slots reel symbols (see func_800E5358, and
// BATTLE_ResolveCaitSithSlotsResult in battle.c)
extern u8 D_80163774[3];
extern u16 D_8016376E[];
extern u8 D_80163784[3];
extern s8 D_80163787; // suspicious, very likely part of a struct
extern u8 D_8016378C[];
extern Unk800A2F4C D_80163798[0x40];
extern s8 D_80163A98;
extern u8 D_80163B38;
extern s16 D_80163B44[];
extern u16 D_80163B80;
extern DR_MODE* D_80163C74; // TODO might be a generic u_long*, not DR_MODE*
extern u8 D_80163C7C;
extern ShortVectorXYZ D_80163C80[];
typedef struct {
    /* 0x00 */ u8 D_80163CC0;
    /* 0x01 */ u8 unk1;
    /* 0x02 */ u16 D_80163CC2;
    /* 0x04 */ u32 D_80163CC4;
} Unk80163CC0; // size:0x08

extern Unk80163CC0 D_80163CC0[];
extern s8 D_80166F58;
extern s8 D_80166F64;
extern u8 D_80166F68;

void func_800A4350(s16, s16, s16, u16);
void func_800A8E84(s32);
void func_800AA950(Unk800FA9D0*);
void func_800AB308(void);
void func_800AB480(void);
void func_800AB788(void);
void func_800ABA68(Unk800FA9D0*, s16, u16, s16, s16);
void func_800AC6B4(s32);
void func_800AC73C(s32);
void func_800ACA24(void);
s32 func_800ACD88(s32);
s32 BATTLE_IsDamageNullified(s32);
void BATTLE_QueueUnassignedResultDisplay(Unk800FA9D0*);
void func_800AD0FC(void);
void func_800AD324(s32, s32, s32, s32);
void BATTLE_ApplyDefaultAbsorbEffect(void);
void func_800AD4EC(void);
void func_800AE82C(void);
s32 func_800B3030(s32);
void func_800B4794(void);
s32 func_800B5CD4(s32);
void func_800B5D38(s32);
void func_800B6B98(s32, s32);
void func_800B7FB4(void);
s16 func_800B888C(s32);
void func_800B8438(void);
void func_800B8A34(s16, s32);
static void func_800BA40C();
static void func_800BB030(s16 arg0);
void func_800BB2A8(u8);
void func_800BB9B8(s32);
void func_800BBA84(u16 arg0, s32 arg1, s32 arg2);
static void func_800C0480(s16);
static void func_800C0630(s16);
static void func_800C0970(s16);
static void func_800C0B20(s16);
static void func_800C1908(u8 arg0);
void func_800C5E94(void);
void func_8002DF88(s16*);
void func_800D088C(s32 loc, s32 len);
void func_800D0C80(u8 arg0);
void func_800D2980(u_long* addr, s16 imgXY, s16 clutX, s16 clutY);
void func_800D3BF0();
void func_800D8A78(s8);
int func_800D8A88(void);
void func_800D91DC(s32, s32, s16, u8, s32, s32);
void func_800D9E0C(s32, s32, s32);
void func_800DCFD4(u_long*);
void func_800DDFEC(void);
void func_800E15D8(void);
void func_800E5814(void);
void func_800E6B94(void);
void BATTLE_EnqueueLoadImage(RECT* rect, u_long* ptr);

/* one battle-usable item in the in-battle item list (built from the inventory
   at battle start; counts are committed back when the battle ends) */
typedef struct {
    /* 0x0 */ u16 id;
    /* 0x2 */ u8 count;
    /* 0x3 */ u8 targetFlags;
    /* 0x4 */ u8 unk4;
    /* 0x5 */ u8 unk5;
} BattleItemEntry; /* size: 0x6 */

/* battle menu widget block (one per widget id, 0x240 apart) -- partial */
typedef struct {
    /* 0x0 */ u16 unk0;
    /* 0x2 */ s16 scroll;
    /* 0x4 */ u8 unk4[2];
    /* 0x6 */ u16 unk6;
    /* 0x8 */ u16 unk8;
    /* 0xA */ u8 unkA;
    /* 0xB */ s8 cursorRow;
    /* 0xC */ u8 unkC;
    /* 0xD */ u8 unkD;
    /* 0xE */ u8 unkE;
    /* 0xF */ u8 unkF;
    /* 0x10 */ u8 unk10;
    /* 0x11 */ u8 unk11;
    /* 0x12 */ u8 unk12[0x22E];
} BattleMenuWidget; /* size: 0x240 */

/* State of the battle-script VM interpreted by func_800B1D48. Operands are
   fetched from the script buffer D_800F4AC0 at `pc` and evaluated on `stack`,
   which grows downwards: a push predecrements `sp` before storing, a pop reads
   at `sp` then postincrements it. Instructions address two operand slots by
   index, hence the [2] arrays. */
typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 pc;
    /* 0x08 */ s32 sp;
    /* 0x0C */ s32 opcode;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18[2];
    /* 0x20 */ s32 unk20[2];
    /* 0x28 */ u16 unk28[2];
    /* 0x2C */ s32 var[2][10];
    /* variable length: indexed by `sp`, extent unconfirmed */
    /* 0x7C */ u8 stack[1];
} BattleScriptVm;

extern u8* D_800F4AC0;
extern BattleScriptVm* D_800F4AC4;

s32 func_800B18A8(s32);

void func_800A4E40(void);
void func_800DE2B4(void);
void func_800E08C4(s32);
void func_800E0BE0(s32);
void func_800E7170(void);
extern void func_800CE970(void);
extern void (*D_800F300C[])();
extern s16 D_800F310E;
extern s16 D_800F3120;
extern u16 D_800F314E;
extern u16 D_800F3894;
extern u8 D_800F389D;
extern u8 D_800F38A4;
extern u8 D_800F38A5;
extern u8 D_800F38A6;
extern u8 D_800F38A9;
extern u8 D_800F514D;
extern u8 D_800F515F;
extern u8 D_800F5161;
extern u8 D_800F5166;
extern u8 D_800F5167;
extern u8 D_800F5168;
extern u8 D_800F5628;
extern u16 D_800F562C;
extern u8 D_800F5630;
extern u16 D_800F5634;
extern u8 D_800F5638;
extern u8 D_800F563C;
extern u8 D_800F5EFC[]; // per-slot formation-setup config, 0x18 B stride; byte
                        // 0 -> func_800A8D18
extern BattleMenuWidget D_800F90C6[];
extern u8 D_80151698;
extern u8 D_80166F74;
extern u8 D_80166F75;
extern BattleItemEntry D_801671B8[];
