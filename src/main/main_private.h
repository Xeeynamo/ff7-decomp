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
    u8 attackElement[2];  // 0x0400=Cut,0x0800=Hit,0x1000=Punch,0x2000=Shoot
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

extern s32 D_80010100[];
extern s32 D_80048CFC;
extern s32 D_80048D00;
extern s32 D_80048D04;
extern s32 D_80048D08;
extern s32 D_80048D0C;
extern s32 D_80048D10;
extern s32 D_80048D14;
extern s32 D_80048D18;
extern s32 D_80048D1C;
extern s32 D_80048D20;
extern Yamada D_80048D84[];
extern s32 D_80048DD4[];
extern PortraitEntry D_80048FE8[15];
extern u32 D_800491D0[];
extern u8 D_80049208[12]; // menu color RGB-quadruplet
extern s32 D_80049224[8];
extern s32 D_80049474;    // play-clock divisor: 36000 (seconds per 10 hours)
extern s32 D_80049478;    // play-clock divisor: 3600 (seconds per hour)
extern s32 D_80049500[8]; // party slot -> character id (endgame level snapshot)
extern u8 D_80049520[];
extern u8 D_80049528[];
extern u8 D_80063690[];
extern Unk800696AC D_800696AC;
extern u32 D_80062FA0; // Some sort of pad state
extern u16 D_80069490[];
extern DRAWENV D_80070700; // active draw environment (double-buffered)
extern DISPENV D_8007075C; // active display environment (double-buffered)
extern u16 D_800707BC;
extern u16 D_800707BE;
extern u8 D_800708D4[];
extern u8 D_800716D0;
extern s32 D_80071744; // LBA loc for func_80014540
extern s16 D_80071A5C;
extern AccessoryRecord g_AccessoryTable[]; // accessory kernel table, by acc. id
extern ArmorRecord g_ArmorTable[]; // armor kernel table, indexed by armor id
extern u_long* D_800722C8;         // LBA dst for func_80014540
extern u8 D_800722DC[];
extern u8 D_800730DC[][0x14];
extern WeaponRecord g_WeaponTable[]; // weapon kernel table, by weapon id
extern s32 D_80095DD8;               // LBA len for func_80014540
extern s16 D_800965E8;
extern u16 D_8009C560; // refer to Subsystem enum
extern SavePartyMember D_8009C738[8];
extern u8 D_8009C748[];  // character record, 0x84-byte stride; +0xC = equipped
                         // weapon id, +0xD = equipped armor id
extern u8 D_8009C754[];  // D_8009C748 + 0xC (equipped weapon id field),
                         // decompiler-split alias
extern u8 D_8009C755[];  // D_8009C748 + 0xD (equipped armor id field),
                         // decompiler-split alias
extern u8 D_8009D44C[8]; // party-level snapshot for endgame battle AI
extern u16 D_8009ABF6;
extern u16 D_8009AC32;
extern u8 D_8009D5E9;
extern u16 D_8009D78A;           // party-present bitmask
extern void (*D_800A00CC)(void); // battle/brom entrypoint
extern void (*D_800A1158)(void); // battle/battle entrypoint
