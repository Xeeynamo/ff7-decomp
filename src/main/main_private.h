#include <game.h>

typedef enum {
    KERNEL_COMMAND,
    KERNEL_ATTACK,
    KERNEL_GROWTH,
    KERNEL_INIT,
    KERNEL_ITEM,
    KERNEL_WEAPON,
    KERNEL_ARMOR,
    KERNEL_ACCESSORY,
    KERNEL_MATERIA,
    KERNEL_DESC_COMMAND,
    KERNEL_DESC_MAGIC,
    KERNEL_DESC_ITEM,
    KERNEL_DESC_WEAPON,
    KERNEL_DESC_ARMOR,
    KERNEL_DESC_ACCESSORY,
    KERNEL_DESC_MATERIA,
    KERNEL_DESC_KEY_ITEM,
    KERNEL_NAME_COMMAND,
    KERNEL_NAME_MAGIC,
    KERNEL_NAME_ITEM,
    KERNEL_NAME_WEAPON,
    KERNEL_NAME_ARMOR,
    KERNEL_NAME_ACCESSORY,
    KERNEL_NAME_MATERIA,
    KERNEL_NAME_KEY_ITEM,
    KERNEL_TEXT_BATTLE,
    KERNEL_NAME_SUMMON,
} KernelID;

typedef enum {
    SUBSYSTEM_FIELD = 1,
    SUBSYSTEM_BATTLE = 2,
    SUBSYSTEM_WORLD = 3, // also used for snowfield
    SUBSYSTEM_UNK = 4,   // similar to battle?
    SUBSYSTEM_MENU = 5,
    SUBSYSTEM_BIKE = 6,
    SUBSYSTEM_RACE = 7,
    SUBSYSTEM_SNOWBOARD = 8,
    SUBSYSTEM_FORTCONDOR = 9,
    SUBSYSTEM_SUBMARIME = 10,
    SUBSYSTEM_SHOOTING = 11,
    SUBSYSTEM_CHANGE_DISK,
    SUBSYSTEM_SNOWBOARD_GOLDSAURCER = 14,
} Subsystem;

typedef struct {
    s32 len; // decompressed length
    s32 unk4;
} GzHeader;

typedef struct {
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
} Unk8001DE0C;

typedef struct {
    u8 padABuffer;
    u8 unk1;
    u8 unk2;
    u8 unk3[31];
    u8 padBBuffer;
    u8 unk23;
    u8 unk24;
    u8 unk25[31];
} Unk800696AC;

typedef struct {
    s32 sector_off;
    s32 length;
} PortraitEntry;

// Kernel armor record, one per armor id (g_ArmorTable). Field meanings were
// verified by dumping the live table and matching each field against
// published stats for all 32 armors.
typedef struct {
    u8 unk0;            // 0 on every armor except Wizard Bracelet (0xFF)
    u8 elementalEffect; // "damage type": 0xFF=none, 0=absorb, 1=nullify,
                        // 2=halve
    u8 defense;
    u8 magicDefense;
    u8 defensePercent;
    u8 magicDefensePercent;
    u8 statusDefense; // index of the status bit this armor guards against;
                      // 0xFF (none) on every armor (a mostly-accessory field)
    u8 unk7;
    u8 unk8;           // 0 on every armor except Four Slots (0xFF)
    u8 materiaSlot[8]; // one byte per possible slot; 0=none, else slot present
                       // (5=single/6,7=linked-pair when materiaGrowth!=None;
                       //  1=single/2,3=linked-pair when materiaGrowth==None)
    u8 materiaGrowth;  // 0=None, 1=Normal, 2=Double
    u8 equipMask[2];   // equippable-by-character bitmask (bit0=Cloud,1=Barret,
                     // 2=Tifa,3=Aeris,4=RedXIII,5=Yuffie,6=CaitSith,7=Vincent,
                     // 8=Cid,9=Young Cloud). 0x01FF=all; Minerva=0x002C
                     // (women), Escort Guard=0x03D3 (men + Young Cloud).
    u8 elementalMask
        [2];     // bit0=Fire,1=Ice,2=Lightning,3=Earth,4=Poison,5=Gravity,
                 // 6=Water,7=Wind,8=Holy,10=Cut,11=Hit,12=Punch,13=Shoot
    u8 unk16[2]; // unknown, always 0x00FF
    u8 statBonusId[4];     // stat each slot boosts: 0=Str,1=Vit,2=Mag,3=Spr,
                           // 4=Dex,5=Lck; unused slot when paired value==0
    u8 statBonusValue[4];  // bonus amount; 0 = slot unused
    u8 restrictionMask[2]; // usage flags (sellability / battle-use / menu-use);
                           // 0xFFFE on armor
    u8 unk22[2];           // unknown, always 0xFFFF
} ArmorRecord;

// Kernel accessory record, one per accessory id (g_AccessoryTable), 0x10 bytes.
// Field meanings verified by dumping the live table and matching each field
// against published stats for all 32 accessories (same method as ArmorRecord).
typedef struct {
    u8 statBonusId[2];    // stat each slot boosts: 0=Str,1=Vit,2=Mag,3=Spr,
                          // 4=Dex,5=Lck; 0xFF = unused
    u8 statBonusValue[2]; // bonus amount, paired with statBonusId
    u8 elementalStrength; // 0=absorb, 1=nullify, 2=halve; 0xFF = none
    u8 specialEffect;     // 0xFF none; 0=Haste, 1=Berserk, 2=Curse, 3=Reflect,
                          // 4=raise steal rate, 5=raise manipulate rate,
                          // 6=Barrier/MBarrier
    u8 elementMask[2];   // elements the elementalStrength applies to (u16 mask,
                         // same element bits as ArmorRecord.elementalMask)
    u8 statusProtect[4]; // status-immunity bitmask (u32); e.g. Ribbon sets most
    u8 equipMask[2];     // equippable-by-character bitmask (see ArmorRecord);
                         // 0x01FF (all nine) on every accessory
    u8 restrictionMask[2]; // a set bit forbids: 0x01 sell, 0x02 use in battle,
                           // 0x04 use in menu (0xFFFE on every accessory)
} AccessoryRecord;

extern u16 g_Pad1ButtonsPrev;
extern u16 g_Pad2Buttons;
extern u16 g_Pad2ButtonsPrev;
extern u16 g_Pad2ButtonsPressed;
extern u16 g_Pad2ButtonsRepeat;
extern u16 g_Pad1FastButtons;
extern u16 g_Pad1FastButtonsPrev;
extern u16 g_Pad1FastButtonsPressed;
extern u16 g_Pad1FastButtonsRepeat;
extern u16 g_Pad2FastButtons;
extern u16 g_Pad2FastButtonsPrev;
extern u16 g_Pad2FastButtonsPressed;
extern u16 g_Pad2FastButtonsRepeat;

extern s32 D_80010100[];
extern Yamada D_80048CFC[];
extern Yamada D_80048D84[];
extern s32 D_80048DD4[];
extern PortraitEntry D_80048FE8[15];
// Map between battle character IDs and index into character record array.
// Battle characters have IDs 0-10. 9 and 10 are young Cloud and Sephiroth from
// flashback sequence and they use same character records as Cait Sith and
// Vincent.
extern s32 g_BattleCharIdToCharId[11];
extern s32 D_80049224[8];
extern s32 D_80049474[];  // play-clock divisors: 36000 (seconds per 10 hours),
                          // 3600 (seconds per hour)
extern s32 D_80049500[8]; // party slot -> character id (endgame level snapshot)
extern u8 D_80049520[];
extern u8 D_80049528[];
extern u8 D_80062E54[8];
extern u8 D_80062E5C; // Pre-emptive materia is at maximum level.
extern ActiveCharacterData* D_80062E60; // Current active character.
extern u16 g_ElementIdToBitmask[16];
extern u32 D_80062E64;
extern u32 D_80062E68;
extern s16 D_80062E6C[4];
extern u32 D_80062E74;
extern u32 D_80062E78;
extern s32 D_80062E7C;
extern s32 D_80062E80;
extern s32 D_80062E84;
extern u32 D_80062E88;
extern u32 D_80062E8C;
extern u32 D_80062E90;
extern s32 g_RewardMenuHasEarnedItems;
extern u_long* g_CurrentMenuOrderingTable;
extern s32 g_PartyMenuListState;
extern s32 g_PartyMenuPreviousMenuId;
extern s32 g_PartyMenuListTransitionFactor;
extern s32 g_PartyMenuSelectedMenuId;
extern u32 D_8006966C[16];
extern u32 D_8006968C[8];
extern s16 g_RewardMenuState;
extern u8 D_80062DDA;
extern u8 D_80062F98;
extern s8 D_80062FFC;
extern s16 D_80062FF4;
extern u8 D_80062FEC;
extern u16 D_80062F34[3];
extern Unk80062F7C* D_80062F7C;
extern u8 D_80063690[];
extern Unk800696AC D_800696AC;
extern TILE D_800696FC[5];
extern u_long g_MenuOrderingTables[2][20];
extern MenuTable g_RewardMenuTable;
extern s32 g_TutorialActive;
extern u32 D_80062FA4[2];
extern u16 D_80069490[];
extern u8 D_800694B4[16];
extern u8 D_800694C4[16];
extern u8 D_800694D4[16];
extern s16 D_800694E4[12];
extern u16 D_800694F4[];
extern u16 D_800694F6;
extern u8 D_800694FA;
extern s16 D_800694FC[6];
extern s16 D_800694FE;
extern s16 D_80069500;
extern s16 D_80069502;
extern s16 D_80069504;
extern s16 D_80069506;
extern DRAWENV D_80070700;    // active draw environment (double-buffered)
extern DISPENV D_8007075C[2]; // active display environments (double-buffered)
extern u16 D_800707BC;
extern u8 D_800708D4[];
extern u8 D_800716D0;
extern s32 D_80071744; // LBA loc for func_80014540
extern s16 D_80071A5C;
extern AccessoryRecord g_AccessoryTable[]; // accessory kernel table, by acc. id
extern ArmorRecord g_ArmorTable[]; // armor kernel table, indexed by armor id
extern u_long* D_800722C8;         // LBA dst for func_80014540
extern u8 D_800722DC[];
extern WeaponRecord g_WeaponTable[]; // weapon kernel table, by weapon id
extern s32 D_80095DD8;               // LBA len for func_80014540
extern s16 g_isFieldLoading;
extern volatile s16 D_8009C560; // refer to Subsystem enum
extern SavePartyMember D_8009C738[8];
extern u8 D_8009C754[]; // Savemap.party[0].weapon, with a 0x84-byte stride
extern u8 D_8009C755[]; // Savemap.party[0].armor, with a 0x84-byte stride
extern s16 D_8009ABF6;
extern u16 D_8009AC32;
extern u8 D_8009D44C[8]; // party-level snapshot for endgame battle AI
extern u8 D_8009D5E9;
extern u16 D_8009D78A; // party-present bitmask
void D_800A00CC(void); // battle/brom entrypoint
void D_800A1158(void); // battle/battle entrypoint
