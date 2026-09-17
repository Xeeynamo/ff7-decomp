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
    KERNEL_TEXT_DESC_COMMAND,    // 0x00
    KERNEL_TEXT_DESC_MAGIC,      // 0x01
    KERNEL_TEXT_DESC_ITEM,       // 0x02
    KERNEL_TEXT_DESC_WEAPON,     // 0x03
    KERNEL_TEXT_DESC_ARMOR,      // 0x04
    KERNEL_TEXT_DESC_ACCESSORY,  // 0x05
    KERNEL_TEXT_DESC_MATERIA,    // 0x06
    KERNEL_TEXT_DESC_KEY_ITEM,   // 0x07
    KERNEL_TEXT_NAME_COMMAND,    // 0x08
    KERNEL_TEXT_NAME_MAGIC,      // 0x09
    KERNEL_TEXT_NAME_ITEM,       // 0x0A
    KERNEL_TEXT_NAME_WEAPON,     // 0x0B
    KERNEL_TEXT_NAME_ARMOR,      // 0x0C
    KERNEL_TEXT_NAME_ACCESSORY,  // 0x0D
    KERNEL_TEXT_NAME_MATERIA,    // 0x0E
    KERNEL_TEXT_NAME_KEY_ITEM,   // 0x0F
    KERNEL_TEXT_BATTLE_MESSAGES, // 0x10
    KERNEL_TEXT_NAME_SUMMON,     // 0x11
} KernelTextBlockID;

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

extern u16 g_Pad1KeysPrev;
extern u16 g_Pad2Keys;
extern u16 g_Pad2KeysPrev;
extern u16 g_Pad2KeysPressed;
extern u16 g_Pad2KeysRepeat;
extern u16 g_Pad1BattleKeys;
extern u16 g_Pad1BattleKeysPrev;
extern u16 g_Pad1BattleKeysPressed;
extern u16 g_Pad1BattleKeysRepeat;
extern u16 g_Pad2BattleKeys;
extern u16 g_Pad2BattleKeysPrev;
extern u16 g_Pad2BattleKeysPressed;
extern u16 g_Pad2BattleKeysRepeat;

extern Yamada D_80048F60[17];
extern Yamada D_80048FE8[15];
extern s32 D_80049474[6]; // play-clock divisors, see ovl.c
extern s32 D_80049500[8]; // party slot -> character id (endgame level snapshot)
extern u8 D_80049520[];
extern u8 D_80049528[];
extern s32 g_RewardMenuHasEarnedItems;
extern u_long* g_CurrentMenuOrderingTable;
extern s32 g_PartyMenuListState;
extern s32 g_PartyMenuPreviousMenuId;
extern s32 g_PartyMenuListTransitionFactor;
extern s32 g_PartyMenuSelectedMenuId;
extern u32 D_8006966C[16];
extern s16 g_RewardMenuState;
extern u8 D_80062DDA;
extern u8 D_80062F98;
extern s8 D_80062FFC;
extern s16 D_80062FF4;
extern u8 D_80062FEC;
extern u16 D_80062F34[3];
extern Unk80062F7C* D_80062F7C;
extern u8 g_KernelTextBuffer[];
extern TILE D_800696FC[5];
extern u_long g_MenuOrderingTables[2][20];
extern MenuTable g_RewardMenuTable;
extern s32 g_TutorialActive;
extern u32 D_80062FA4[2];
extern u16 g_KernelTextBlockOffsets[];
extern u8 D_800694B4[16];
extern u8 D_800694C4[16];
extern u8 D_800694D4[16];
extern s16 D_800694E4[12];
extern s16 D_800694FC[6];
extern DISPENV D_8007075C[2]; // active display environments (double-buffered)
extern struct {
    u16 battleId;
    u16 mode;
} D_800707BC;
extern u8 D_800716D0;
extern s32 D_80071744; // LBA loc for func_80014540
extern s16 D_80071A5C;
extern AccessoryRecord g_AccessoryTable[]; // accessory kernel table, by acc. id
extern ArmorRecord g_ArmorTable[];         // armor kernel table, indexed by armor id
extern u_long* D_800722C8;                 // LBA dst for func_80014540
extern WeaponRecord g_WeaponTable[];       // weapon kernel table, by weapon id
extern s32 D_80095DD8;                     // LBA len for func_80014540
void D_800A00CC(void);                     // battle/brom entrypoint
void D_800A1158(void);                     // battle/battle entrypoint

void SysGzipBinDecompress(GzHeader* src, u8* dst);
void SysGzipSetDataBlock(u8* arg0);
u16 SysGzipGetType(void);
u16 SysGzipGetSize(void);
u32 SysGzipPackDecompressNextBlock(u8* dst);
u16 func_801D1950(u16 len, u8* data);
u8* SysGetKernTextPtrByBlock(s32 arg0, s32 arg1, s32 arg2);
u8* SysGetKernBattleTextPtr(s32 arg0);
u8 func_8001F6B4(void);
void SysMenuSetPosAddWindow(s16 enabled, s16 x, s16 y); // PC: menu_setNotificationWindowPosition
void SysMenuRequestAddWindow(u8* text, s8 palette);     // PC: menu_setNotificationMessage
u8* func_80014C80(s32 arg0);
void SysMenuDrawScrollbarSlider(RECT* rect);
void SysMenuDrawScrollbarTrack(RECT* rect);
void SysMenuDrawSingleFontLetter(s16 x, s16 y, s32 ch, u8 color);
