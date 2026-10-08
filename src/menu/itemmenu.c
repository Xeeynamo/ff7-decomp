//! PSYQ=3.3 CC1=2.7.2
#include <game.h>

// Item-menu screen/sub-state selector. Confirmed via live RAM trace (PCSX-Redux
// write-watch, PSX retail build) while stepping through the menu:
//   0 = Use/Arrange/Key-Items tab selector
//   1 = Use (item list)
//   2 = item selected within Use (target/confirm step)
//   3 = Key Items
//   4 = Arrange
// Written by func_801D131C (initial tab pick: 1/3/4) and func_801D1A6C
// (cancel back to selector: 0; cancel out of item-select: back to 1) -
// NOT func_801D0E80, which is a separate on-enter routine unrelated to this
// state (see D_800493A8 in src/main/ovl.c, where func_801D0E80 is reached
// from 8 different screen-entry slots).
typedef enum {
    ITEMMENU_SCREEN_SELECTOR = 0,
    ITEMMENU_SCREEN_USE = 1,
    ITEMMENU_SCREEN_ITEM_SELECTED = 2,
    ITEMMENU_SCREEN_KEY_ITEMS = 3,
    ITEMMENU_SCREEN_ARRANGE = 4,
} ItemMenuScreen;

typedef enum {
    WEAPON_INDEX_CLOUD_MAX = 0x10,
    WEAPON_INDEX_BARRET_MAX = 0x20,
    WEAPON_INDEX_TIFA_MAX = 0x30,
    WEAPON_INDEX_AERIS_MAX = 0x3E,
    WEAPON_INDEX_RED_XIII_MAX = 0x49,
    WEAPON_INDEX_YUFFIE_MAX = 0x57,
    WEAPON_INDEX_CAIT_SITH_MAX = 0x65,
    WEAPON_INDEX_VINCENT_MAX = 0x72
} WeaponIndex;

typedef enum {
    ITEM_ICON_ITEM = 0,
    ITEM_ICON_SWORD = 1,
    ITEM_ICON_GLOVE = 2,
    ITEM_ICON_GUN_ARM = 3,
    ITEM_ICON_CLIP = 4,
    ITEM_ICON_STAFF = 5,
    ITEM_ICON_MEGAPHONE = 6,
    ITEM_ICON_GUN = 7,
    ITEM_ICON_SPEAR = 8,
    ITEM_ICON_SHURIKEN = 9,
    ITEM_ICON_ARMOR = 0xA,
    ITEM_ICON_ACCESSORY = 0xB
} ItemIcon;

typedef enum {
    ITEM_ARRANGE_CUSTOMIZE = 0,
    ITEM_ARRANGE_FIELD = 1,
    ITEM_ARRANGE_BATTLE = 2,
    ITEM_ARRANGE_THROW = 3,
    ITEM_ARRANGE_TYPE = 4,
    ITEM_ARRANGE_NAME = 5,
    ITEM_ARRANGE_MOST = 6,
    ITEM_ARRANGE_LEAST = 7,
} ItemArrangeMode;

#define ITEM_TYPE_WEAPON_BASE 0x80
#define ITEM_TYPE_ARMOR_BASE 0x100
#define ITEM_TYPE_ACCESSORY_BASE 0x120
#define ITEM_ICON_BASE_U 0x60
#define ITEM_ICON_BASE_V 0x70
#define ITEM_ICON_SIZE 0x10
#define ITEM_ICON_CLUT 1

#define ITEM_ID_MASK 0x1FF
#define ITEM_QTY_SHIFT 9
#define ITEM_EMPTY_SLOT 0xFFFF
#define SORT_KEY_EMPTY_SLOT 0x4E20

#define ITEM_USAGE_FLAG_BATTLE 0x2
#define ITEM_USAGE_FLAG_FIELD 0x4
#define ITEM_USAGE_FLAG_THROW 0x8

#define ITEM_ID_TENT 0x46
#define ITEM_ID_SAVE_CRYSTAL 0x62
#define MENU_LOCATION_TENT_ALLOWED 0x200
#define SAVE_CRYSTAL_USED_FLAG 0x2

#define MAX_KEY_ITEMS 0x40
#define MAX_STOLEN_MATERIA 0x30
#define NOTIFICATION_TEXT_SIZE 0x50
#define EMPTY_MATERIA_SLOT (-1)
#define EMPTY_ACCESSORY_SLOT 0xFF
#define EMPTY_LIMIT_COMMAND 0x7F
#define NUM_LIMIT_SLOTS 10

// [0]: single-slot, non-scrolling widget (total=1, 1/page) - purpose not yet
//      identified.
// [1]: the Use tab's item list - total=0x140 (320) matches the item
//      inventory Savemap.inventory exactly, 10/page.
// [2]: the Use/Arrange/Key-Items tab selector itself - total=3, wraps.
extern MenuTable g_ItemMenuWidgets[];
extern u8 g_MateriaPriority[];
extern s32 g_MateriaStealLoot[];
extern u16 g_ItemNameSortKeys[]; // per-item-id sort order for the "Name" arrange option
extern u16 g_MenuLocationFlags;
extern u8 g_CoinTextureTim[];
extern s32 g_ItemMenuCurrentScreen;
extern u8 g_ItemMenuNotificationText[];
extern u8 g_KeyItemList[]; // Key Items menu list: obtained key-item IDs in
                           // ascending order, 0xFF-padded to 64 entries.

s32 SysMenuGetInventoryRestrictionMask(s32); // returns an item's usage flags (0x2 battle, 0x4 field, 0x8 throw)
typedef s32 (*SortCmp)(s32, s32, s32*);
typedef void (*SortSwap)(s32, s32, s32*);
static s32 Quicksort(s32, s32, SortCmp, SortSwap);
s32 SysGetLimitCmdId(s32, s32);
void SysMenuDrawTexturedRect(s16, s16, s32, s32, s32, s32, s32, s32);

// Plays a menu sound effect: uses AKAO_PLAY_MENU_SOUND to play the sound
// id (soundEffectId) into the sound-request globals, then dispatches via
// AkaoExec.
void PlayItemMenuSfx(u16 soundEffectId) {
    g_AkaoCmd.opcode = AKAO_PLAY_MENU_SOUND;
    g_AkaoCmd.params[0] = soundEffectId;
    g_AkaoCmd.params[1] = soundEffectId;
    AkaoExec();
}

// Draws the type icon for an item at (x, y): maps the item id to
// one of several icon cells, then blits a 16x16 sprite via
// SysMenuDrawTexturedRect.
void ITEMMENU_DrawItemTypeIcon(s16 x, s16 y, s32 itemId) {
    s32 icon;
    if (itemId < ITEM_TYPE_WEAPON_BASE) {
        icon = ITEM_ICON_ITEM;
    } else if (itemId < ITEM_TYPE_ARMOR_BASE) {
        itemId -= ITEM_TYPE_WEAPON_BASE;
        if (itemId < WEAPON_INDEX_CLOUD_MAX) {
            icon = ITEM_ICON_SWORD;
        } else if (itemId < WEAPON_INDEX_BARRET_MAX) {
            icon = ITEM_ICON_GUN_ARM;
        } else if (itemId < WEAPON_INDEX_TIFA_MAX) {
            icon = ITEM_ICON_GLOVE;
        } else if (itemId < WEAPON_INDEX_AERIS_MAX) {
            icon = ITEM_ICON_STAFF;
        } else if (itemId < WEAPON_INDEX_RED_XIII_MAX) {
            icon = ITEM_ICON_CLIP;
        } else if (itemId < WEAPON_INDEX_YUFFIE_MAX) {
            icon = ITEM_ICON_SHURIKEN;
        } else if (itemId < WEAPON_INDEX_CAIT_SITH_MAX) {
            icon = ITEM_ICON_MEGAPHONE;
        } else if (itemId < WEAPON_INDEX_VINCENT_MAX) {
            icon = ITEM_ICON_GUN;
        } else {
            icon = ITEM_ICON_SPEAR;
        }
    } else if (itemId < ITEM_TYPE_ACCESSORY_BASE) {
        icon = ITEM_ICON_ARMOR;
    } else {
        icon = ITEM_ICON_ACCESSORY;
    }
    {
        s32 texU = ((icon & 1) << 4) | ITEM_ICON_BASE_U;
        s32 texV = (((u32)icon >> 1) << 4) + ITEM_ICON_BASE_V;
        SysMenuDrawTexturedRect(x, y, texU, texV, ITEM_ICON_SIZE, ITEM_ICON_SIZE, ITEM_ICON_CLUT, 0);
    }
}

// Builds the Key Items menu list: scans the 64-bit "key items obtained" bitmask
// in the savemap (Savemap + 0xBE4, i.e. memory_bank_1[0x40]) and appends the ID
// of each owned key item to g_KeyItemList in ascending order, then pads the
// remaining entries with 0xFF.
static void BuildKeyItemList(void) {
    s32 count;
    u8* keyItemPtr;
    s32 keyItemId;

    for (keyItemId = 0, count = 0, keyItemPtr = g_KeyItemList; keyItemId < MAX_KEY_ITEMS; keyItemId++) {
        if ((Savemap.memory_bank_1[0x40 + keyItemId / 8] >> (keyItemId & 7)) & 1) {
            *keyItemPtr = keyItemId;
            keyItemPtr += 1;
            count += 1;
        }
    }
    while (count < MAX_KEY_ITEMS) {
        g_KeyItemList[count] = -1;
        count += 1;
    }
}

// Swap the two 32-bit values pointed to by left and right.
static void SwapS32(s32* left, s32* right) {
    s32 valRight = *right;
    s32 valLeft = *left;
    *left = valRight;
    *right = valLeft;
}

// Iterative Hoare quicksort over item-slot indices [0, count), driving the
// cmp/swap callbacks. Explicit 64-deep bounds stack (lo half / hi half of one
// 128-word array), recursing into the smaller partition first (SwapS32 swaps
// the bounds pairs). Returns 1 on completion, 0 on bounds-stack overflow.
// NOTE: the do{}while(0) wrapper, the va1 register copy of j, the duplicated
// cont computation and the tmp* temporaries are all required for the
// byte-perfect match (they reproduce the original register allocation).
static s32 Quicksort(s32 base, s32 count, SortCmp cmp, SortSwap swap) {
    s32 stack[128];
    s32 tmp4;
    s32 tmp5;
    s32 j;
    s32 lo;
    s32 i;
    int tmp;
    s32 tmp3;
    s32* stackPtr;
    s32 depth;
    s32 cont;
    int tmp2;
    s32 va1;

    if (((u32)count) >= 2U) {
        goto body;
    }
    return 1;
ret0:
    return 0;

    do {
    body:
        depth = 0;
        stackPtr = stack;
        stack[0] = 0;
        stack[64] = count - 1;
    loop_4:
        lo = stackPtr[0];
        tmp = (i = lo + 1);
        j = stackPtr[64];
        count = j;
        if (((u32)i) < ((u32)j)) {
        loop_5:
            if (cmp(i, lo, &base) <= 0) {
                i += 1;
                if (((u32)i) < ((u32)j)) {
                    goto loop_5;
                }
            }
            va1 = j;
            if (((u32)va1) >= ((u32)i)) {
            loop_8:
                if (cmp(lo, va1, &base) <= 0) {
                    j -= 1;
                    va1 = j;
                    if (((u32)va1) >= ((u32)i)) {
                        goto loop_8;
                    }
                }
            }
            if (((u32)i) < ((u32)j)) {
                s32 oi = i;
                s32 oj = j;
                i += 1;
                tmp3 = oi;
                j -= 1;
                swap(tmp3, oj, &base);
                if (((u32)i) < ((u32)j)) {
                    goto loop_5;
                }
            }
        }
        if (cmp(lo, j, &base) > 0) {
            swap(lo, j, &base);
        }
        if (((u32)lo) < ((u32)j)) {
            j -= 1;
            if (((u32)lo) < ((u32)j)) {
                if ((((u32)i) < (va1 = (u32)count)) && (((u32)(j - lo)) < ((u32)(count - i)))) {
                    SwapS32(&j, &count);
                    SwapS32(&lo, &i);
                }
                tmp5 = j;
                if (((u32)lo) < ((u32)tmp5)) {
                    stackPtr[0] = lo;
                    stackPtr[64] = tmp5;
                    stackPtr += 1;
                    depth += 1;
                }
            }
        }
        cont = ((u32)depth) < 0x40U;
        tmp4 = count;
        if (((u32)i) < tmp4) {
            stackPtr[0] = i;
            stackPtr[64] = tmp4;
            stackPtr += 1;
            depth += 1;
        }
        cont = ((u32)depth) < 0x40U;
        depth -= 1;
    } while (0);
    if (cont != 0) {
        stackPtr -= 1;
        if (depth == (-1)) {
            return 1;
        }
        goto loop_4;
    }
    goto ret0;
}

// Swap the two 16-bit values pointed to by left and right.
static void SwapU16(u16* left, u16* right) {
    u16 valRight = *right;
    u16 valLeft = *left;
    *left = valRight;
    *right = valLeft;
}

// Returns the sign of value: -1, 0, or 1.
static s32 Sign(s32 value) {
    if (value != 0) {
        if (value < 0) {
            return -1;
        }
        return 1;
    }
    return 0;
}

// Sort comparator for the "Type" arrange option: orders inventory slots slotA
// and slotB by item id (low 9 bits; the item id space is grouped by type).
static s32 CompareItemsByType(s16 slotA, s16 slotB, s32* inventoryBase) {
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    u16 itemB = *(u16*)(*inventoryBase + slotB * 2);
    return Sign((itemA & ITEM_ID_MASK) - (itemB & ITEM_ID_MASK));
}

// Sort comparator for the "Most" arrange option: orders inventory slots by
// quantity (high 7 bits) descending, sending empty slots (0xFFFF) first.
static s32 CompareItemsByMost(s16 slotA, s16 slotB, s32* inventoryBase) {
    s32 qtyA;
    s32 qtyB;
    u16 itemB;
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    if (itemA == ITEM_EMPTY_SLOT) {
        qtyA = 0;
    } else {
        qtyA = itemA >> ITEM_QTY_SHIFT;
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    qtyB = itemB >> ITEM_QTY_SHIFT;
    if (itemB == ITEM_EMPTY_SLOT) {
        qtyB = 0;
    }
    return Sign(qtyB - qtyA);
}

// Sort comparator for the "Least" arrange option: orders inventory slots by
// quantity (high 7 bits) ascending, sending empty slots (0xFFFF) to the end.
static s32 CompareItemsByLeast(s16 slotA, s16 slotB, s32* inventoryBase) {
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    s32 qtyA = (itemA == ITEM_EMPTY_SLOT) ? SORT_KEY_EMPTY_SLOT : (itemA >> ITEM_QTY_SHIFT);
    u16 itemB = *(u16*)(*inventoryBase + slotB * 2);
    s32 qtyB = (itemB == ITEM_EMPTY_SLOT) ? SORT_KEY_EMPTY_SLOT : (itemB >> ITEM_QTY_SHIFT);
    return Sign(qtyA - qtyB);
}

// Sort comparator for the "Name" arrange option: orders inventory slots by a
// per-item sort-order table, sending empty slots (0xFFFF) to the end.
static s32 CompareItemsByName(s16 slotA, s16 slotB, s32* inventoryBase) {
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    s16 sortKeyA;
    u16 itemB;
    s16 sortKeyB;
    if (itemA == ITEM_EMPTY_SLOT) {
        sortKeyA = SORT_KEY_EMPTY_SLOT;
    } else {
        sortKeyA = g_ItemNameSortKeys[itemA & ITEM_ID_MASK];
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    if (itemB == ITEM_EMPTY_SLOT) {
        sortKeyB = SORT_KEY_EMPTY_SLOT;
    } else {
        sortKeyB = g_ItemNameSortKeys[itemB & ITEM_ID_MASK];
    }
    return Sign(sortKeyA - sortKeyB);
}

// Sort comparator for the "Field" arrange option: groups items usable in the
// field (usage flag 0x4) ahead of others; empty slots (0xFFFF) sort first.
static s32 CompareItemsByField(s16 slotA, s16 slotB, s32* inventoryBase) {
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    s32 priorityA;
    u16 itemB;
    s32 priorityB;
    if (itemA == ITEM_EMPTY_SLOT) {
        priorityA = 0;
    } else {
        priorityA = (SysMenuGetInventoryRestrictionMask(itemA & ITEM_ID_MASK) & ITEM_USAGE_FLAG_FIELD) ? 1 : 2;
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    if (itemB == ITEM_EMPTY_SLOT) {
        priorityB = 0;
    } else {
        priorityB = (SysMenuGetInventoryRestrictionMask(itemB & ITEM_ID_MASK) & ITEM_USAGE_FLAG_FIELD) ? 1 : 2;
    }
    return Sign(priorityB - priorityA);
}

// Sort comparator for the "Battle" arrange option: groups items usable in
// battle (usage flag 0x2) ahead of others; empty slots (0xFFFF) sort first.
static s32 CompareItemsByBattle(s16 slotA, s16 slotB, s32* inventoryBase) {
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    s32 priorityA;
    u16 itemB;
    s32 priorityB;
    if (itemA == ITEM_EMPTY_SLOT) {
        priorityA = 0;
    } else {
        priorityA = (SysMenuGetInventoryRestrictionMask(itemA & ITEM_ID_MASK) & ITEM_USAGE_FLAG_BATTLE) ? 1 : 2;
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    if (itemB == ITEM_EMPTY_SLOT) {
        priorityB = 0;
    } else {
        priorityB = (SysMenuGetInventoryRestrictionMask(itemB & ITEM_ID_MASK) & ITEM_USAGE_FLAG_BATTLE) ? 1 : 2;
    }
    return Sign(priorityB - priorityA);
}

// Sort comparator for the "Throw" arrange option: groups throwable items
// (usage flag 0x8) ahead of others; empty slots (0xFFFF) sort first.
static s32 CompareItemsByThrow(s16 slotA, s16 slotB, s32* inventoryBase) {
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    s32 priorityA;
    u16 itemB;
    s32 priorityB;
    if (itemA == ITEM_EMPTY_SLOT) {
        priorityA = 0;
    } else {
        priorityA = (SysMenuGetInventoryRestrictionMask(itemA & ITEM_ID_MASK) & ITEM_USAGE_FLAG_THROW) ? 1 : 2;
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    if (itemB == ITEM_EMPTY_SLOT) {
        priorityB = 0;
    } else {
        priorityB = (SysMenuGetInventoryRestrictionMask(itemB & ITEM_ID_MASK) & ITEM_USAGE_FLAG_THROW) ? 1 : 2;
    }
    return Sign(priorityB - priorityA);
}

// Swap two item inventory slots (indices slotA and slotB in the u16 array at
// *inventoryBase). Used by the item menu's "Customize" manual swap and as the swap
// callback for the inventory sort.
static void SwapItemSlots(s16 slotA, s16 slotB, s32* inventoryBase) {
    SwapU16((u16*)(*inventoryBase + slotA * 2), (u16*)(*inventoryBase + slotB * 2));
}

// Re-sorts the item inventory in place for the menu's "Arrange" command.
// Picks one of the seven comparison orders by `mode` (1=Field, 2=Battle,
// 3=Throw, 4=Type, 5=Name, 6=Most, 7=Least) and runs the sort over the 320
// inventory slots with SwapItemSlots. mode 0 (Customize) and out-of-range
// values do nothing.
static void ArrangeItems(s32 mode) {
    switch (mode) {
    case ITEM_ARRANGE_CUSTOMIZE:
        break;
    case ITEM_ARRANGE_FIELD:
        Quicksort((s32)Savemap.inventory, MAX_INVENTORY_COUNT, (SortCmp)CompareItemsByField, (SortSwap)SwapItemSlots);
        break;
    case ITEM_ARRANGE_BATTLE:
        Quicksort((s32)Savemap.inventory, MAX_INVENTORY_COUNT, (SortCmp)CompareItemsByBattle, (SortSwap)SwapItemSlots);
        break;
    case ITEM_ARRANGE_THROW:
        Quicksort((s32)Savemap.inventory, MAX_INVENTORY_COUNT, (SortCmp)CompareItemsByThrow, (SortSwap)SwapItemSlots);
        break;
    case ITEM_ARRANGE_TYPE:
        Quicksort((s32)Savemap.inventory, MAX_INVENTORY_COUNT, (SortCmp)CompareItemsByType, (SortSwap)SwapItemSlots);
        break;
    case ITEM_ARRANGE_NAME:
        Quicksort((s32)Savemap.inventory, MAX_INVENTORY_COUNT, (SortCmp)CompareItemsByName, (SortSwap)SwapItemSlots);
        break;
    case ITEM_ARRANGE_MOST:
        Quicksort((s32)Savemap.inventory, MAX_INVENTORY_COUNT, (SortCmp)CompareItemsByMost, (SortSwap)SwapItemSlots);
        break;
    case ITEM_ARRANGE_LEAST:
        Quicksort((s32)Savemap.inventory, MAX_INVENTORY_COUNT, (SortCmp)CompareItemsByLeast, (SortSwap)SwapItemSlots);
        break;
    }
}

// exported, see 800493A8
// Configures 3 widgets (g_ItemMenuWidgets[0..2], see MenuTable and the comment on
// its extern decl for what each backs) and defaults the item-menu to the Use
// tab, then continues in BuildKeyItemList. That default is later overwritten by
// func_801D131C if the player picks Arrange or Key Items instead, or by
// func_801D1A6C if they back out to the tab selector (see ItemMenuScreen).
// Reached from src/main/ovl.c's D_800493A8 per-screen entry table for
// several item-menu pages, called out of SysMenuDrawMenuList in
// src/main/1F6B4.c.
void ITEMMENU_Init(void) {
    g_ItemMenuCurrentScreen = ITEMMENU_SCREEN_USE;
    SysMenuSetCursorMovement(&g_ItemMenuWidgets[0], 0, 0, 3, 1, 0, 0, 3, 1, 0, 0, 1, 0, 0);
    SysMenuSetCursorMovement(&g_ItemMenuWidgets[1], 0, 0, 1, 0xA, 0, 0, 1, MAX_INVENTORY_COUNT, 0, 0, 0, 0, 0);
    SysMenuSetCursorMovement(&g_ItemMenuWidgets[2], 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 0, 1, 0);
    BuildKeyItemList();
}

// True if the two adjacent record fields for entry charIdx are equal.
static s32 IsCharacterHpFull(s32 charIdx) {
    return g_ActiveCharacters[charIdx].baseHp == g_ActiveCharacters[charIdx].hp;
}

// True if the two adjacent record fields for entry charIdx are equal.
static s32 IsCharacterMpFull(s32 charIdx) {
    return g_ActiveCharacters[charIdx].baseMp == g_ActiveCharacters[charIdx].mp;
}

// Builds a 10-bit mask of which of character charIdx's slots are occupied (slot
// value != 0x7F), clears bit 9, and returns whether it matches the stored
// value.
static s32 HasLearnedAllLimits(s32 charIdx) {
    s32 mask;
    s32 limitIdx;
    for (limitIdx = 0, mask = 0; limitIdx < NUM_LIMIT_SLOTS; limitIdx++) {
        if (SysGetLimitCmdId(charIdx, limitIdx) != EMPTY_LIMIT_COMMAND) {
            mask |= 1 << limitIdx;
        }
    }
    mask &= ~0x200;
    return (Savemap.party[charIdx].limit_learn ^ mask) == 0;
}

// Returns an item's usage flags (SysMenuGetInventoryRestrictionMask), with two
// context-dependent overrides: item 0x46 (the Tent) becomes field-usable while
// a location flag permits resting, and item 0x62 (the Save Crystal) while its
// one-time-use save flag is still clear.
static s32 GetContextualItemUsageFlags(s32 itemId) {
    s32 flags = SysMenuGetInventoryRestrictionMask(itemId);
    if (itemId != ITEM_ID_TENT) {
        if (itemId == ITEM_ID_SAVE_CRYSTAL) {
            if (!(Savemap.memory_bank_4[0x60] & SAVE_CRYSTAL_USED_FLAG)) {
                flags |= ITEM_USAGE_FLAG_FIELD;
            }
        }
    } else {
        if (g_MenuLocationFlags & MENU_LOCATION_TENT_ALLOWED) {
            flags |= ITEM_USAGE_FLAG_FIELD;
        }
    }
    return flags;
}

// Copies 0x50 bytes from text into the g_ItemMenuNotificationText buffer.
static void SetNotificationText(u8* text) {
    s32 byteIdx;
    for (byteIdx = 0; byteIdx < NOTIFICATION_TEXT_SIZE; byteIdx++) {
        g_ItemMenuNotificationText[byteIdx] = *text;
        text++;
    }
}

// Currently sitting at 99.93% matching
#ifndef NON_MATCHING
INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", ITEMMENU_Main);
#else

typedef struct {
    s16 unk0;
    s16 unk2;
} UnkWindowRect;

typedef struct {
    u16 rowOffset;
} ItemMenuWidget;

// --- Missing Globals & Inferred Function Prototypes ---
extern s32 g_MenuRenderBufferIndex;
extern s32 g_ItemMenuCurrentScreen;
extern u8 g_KeyItemList[];
extern DRAWENV D_800706A4[];

extern u16 g_Pad0KeysPressed;
extern u16 g_Pad0KeysRepeat;

void ArrangeItems(s32);
const char* SysKernGetString(s32, s32, s32);
void SysMenuDrawAvatar(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void SysMenuSetWindowRect(s32*, s32, s32, s32, s32);
void SysMenuDrawWindow(s32*);
void SysMenuDrawCursor(s32, s32);
void SysMenuRequestAddWindow(s32*, s32);
void SysMenuDrawCharNameLvHpMpByPartyId(s32, s32, s32);
void SysMenuLoadMenuFileById(s32);
s32 SysMenuGetMenuListState();
void SysMenuDrawMenuList(s32);
void SysMenuClose();
void SysMenuRemoveItem(s32);
s32 SysMenuSearchItem(u32);
void SystemMenuAddHpByPartyId(s32, s32);
void SystemMenuAddMpByPartyId(s32, s32);
void SysMenuSetDrawenv(void*, s16*);
void SysMenuDrawSingleFontLetter(s32, s32, s32, s32);
void SysMenuDrawScrollbar();
s32 func_801D0CAC(s32);
s32 func_801D0CE8(s32);
s32 func_801D0D24(u8);
s32 func_801D0DCC(s32);
void func_801D0E4C(s32*);

extern u8 D_8009C740[];
extern u8 D_8009C744[];
extern u8 D_8009C757[];
extern u16 D_8009C75A[];
extern s8 D_8009CA50;
extern s8 D_8009CA51;
extern s8 D_8009CA5E;
extern u8 D_8009CA5F;
extern s32 D_8009CA8C;
extern s8 D_8009CAD4;
extern s8 D_8009CAD5;
extern s8 D_8009CAE2;
extern u8 D_8009CAE3;
extern s32 D_8009CB10;
extern u8 D_8009CBCF[];
extern u8 D_8009CBDC[];
extern u16 D_8009CBE0[];
extern u8 D_8009D5E8;
extern s16 D_8009D85C[];
extern s16 D_8009D85E[];
extern s32 D_801D3282;
extern s32 D_801D3590;
extern s32 D_801D3CD4;
extern s32 D_801D3CF8;
extern unsigned char D_801D3D25[];
extern s32 D_801D3D5C;
extern RECT D_801D3D74;
extern s16 D_801D3D76;
extern s32 D_801D3D84;
extern s32 D_801D3D88;
extern s32 D_801D3D8C;
extern unsigned char D_801D3DE4[];
extern s8 D_801D3DE6;
extern s8 D_801D3DEB[];
extern s16 D_801D3DF0;
extern s16 D_801D3DF6;
extern s8 D_801D3DF9[];
extern s8 D_801D3E0B[];
extern s16 D_801D3E14;
extern s8 D_801D3E1C[];
extern s8 D_801D3E1D;
extern s8 D_801D3E21;
extern s8 D_801D3E2F[];
extern s16 D_801D3E38;
extern s8 D_801D3E40;
extern s8 D_801D3E41[];
extern s8 D_801D3E45;
extern s16 D_801D3E4C[];
extern s16 D_801D3E4E;
extern u16 D_801D3E50;
extern s16 D_801D3E52;
extern s16 D_801D3E54;
extern s16 D_801D3E56;
extern s16 D_801D3E58;
extern s32 D_801D3E5C;
void D_801D3260();

char ITEMMENU_Main(s32 arg0) {
    unsigned char temp_s3;
    s16* new_var3;
    s32 sp38[2];
    s16 sp40[12];
    s32* temp_a2;
    s32* var_a0_2;
    s8* var_s1_3;
    s8* var_s1_4;
    s32 temp_a0_10;
    s8* var_s2;
    int new_var6;
    unsigned char new_var11;
    short new_var13;
    s32 var_a0;
    s32 temp_a0_11;
    s32 temp_a0_4;
    s32 temp_a0_5;
    s32 temp_a0_6;
    s32 temp_a0_7;
    s32 temp_a0_8;
    s32 temp_a0_9;
    s8* new_var16;
    s32 var_s0_2;
    s32 temp_s1_2;
    s32 temp_s3_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_6;
    s8* var_s0;
    // s32 temp_e5c;
    MenuTable* new_var5;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s1_5;
    s32 var_s1_7;
    int new_var4;
    s32 var_s2_4;
    int new_var17;
    s32 var_s2_5;
    int new_var8;
    s32 var_s2_6;
    s32 var_s3;
    s32 var_s4;
    s32* new_var10;
    s32 temp_loopval;
    int new_var19;
    s32 var_loopc;
    s32 var_s5;
    s32 temp_bool2;
    s32 var_s6;
    s32 var_v0;
    int new_var9;
    int region_e_cond;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v0_7;
    s32 var_v0_8;
    s32 temp_s1_b;
    int new_var2;
    s32 temp_bool;
    MenuTable* new_var18;
    s32 temp_s2_b;
    int new_var15;
    s32 var_v0_2;
    s8 var_v0_5;
    s8 var_v0_6;
    u16* temp_v1_13;
    u16 temp_a0;
    u16 temp_a0_12;
    u16 temp_a0_2;
    u16 temp_a1;
    u32 temp_a1_2;
    u16 temp_v1_3;
    int new_var14;
    u32 temp_s1;
    unsigned int temp_a0_3;
    u8 temp_a1_3;
    u8 temp_v1_10;
    int new_var12;
    u8 temp_v1_11;
    u8 temp_v1_12;
    u8 temp_v1_4;
    int new_var7;
    u8 temp_v1_5;
    u8 temp_v1_7;
    u8 temp_v1_8;
    u8 temp_v1_9;
    u16* new_var;
    unsigned short var_a1;
    SysMenuDrawMenuList(g_MenuRenderBufferIndex);
    if (g_ItemMenuCurrentScreen == 2) {
        // temp_e5c = D_801D3E5C;
        if (D_801D3E5C == 0) {
            temp_v1 = D_8009CBE0[D_801D3DF9[0] + D_801D3DF0] & 0x1FF;
            if (((temp_v1 == 6) || (temp_v1 == 0x46)) != 0) {
                var_v0_2 = arg0;
                var_v0_2 = var_v0_2 % 3;
                SysMenuDrawCursor(0, (var_v0_2 * 0x38) + 0x4B);
            } else {
                var_v0_2 = D_801D3E0B[0];
                SysMenuDrawCursor(0, (var_v0_2 * 0x38) + 0x4B);
            }
        }
        if ((arg0 & 2) != 0) {
            SysMenuDrawCursor(0xA9, (D_801D3DF9[0] * 0x10) + 0x3C);
        }
        if (D_801D3E5C) {
            D_801D3E5C -= 1;
        }
    }
    SysMenuUnkNoop(0x80);
    switch (g_ItemMenuCurrentScreen) {
    case 0:
        SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        break;

    case 1:
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        SysMenuDrawCursor(0xA9, (D_801D3DF9[0] * 0x10) + 0x3C);
        var_s4 = D_801D3DF9[0] + D_801D3DF0;
        goto block_33;

    case 2:
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        var_s4 = D_801D3DF9[0] + D_801D3DF0;
        goto block_33;

    case 3:
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        SysMenuDrawCursor((D_801D3E1C[0] * 0xA6) + 3, (D_801D3E1D * 0x10) + 0x3C);
        new_var14 = 0xFF;
        var_s4 = ((D_801D3E1D + D_801D3E14) * 2) + D_801D3E1C[0];
        var_a1 = g_KeyItemList[var_s4];
        var_a0 = 0xE;
        if (var_a1 != new_var14) {
            do {
            } while (0);
            goto block_35;
        }
        break;

    case 4:
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        var_s0 = 0;
        temp_a0_4 = D_801D3D74.x;
        var_s3 = (s32)&D_801D3D74;
        var_s2 = (s8*)&D_801D3CF8;
        var_s1 = 6;
        SysMenuDrawCursor(temp_a0_4 - 0x12, (D_801D3D74.y + 8) + D_801D3E2F[0] * 12);
        do {
            SysMenuDrawString(((RECT*)var_s3)->x + 8, ((RECT*)var_s3)->y + var_s1, (const char*)var_s2, 7);
            var_s2 += 0xC;
            var_s0 += 1;
            var_s1 += 0xC;
        } while (((s32)var_s0) < 8);
        sp40[0] = 0;
        sp40[1] = 0;
        sp40[2] = 0x100;
        sp40[3] = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, sp40);
        SysMenuDrawWindow((s32*)(&D_801D3D74));

        break;

    case 5:
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        var_s4 = D_801D3E41[0] + D_801D3E38;
    block_33:
        var_a1 = D_8009CBE0[var_s4];

        var_a0 = 4;
        if ((var_a1 & 0xFFFF) != 0xFFFF) {
            var_a1 = var_a1 & 0x1FF;
        block_35:
            SysMenuDrawString(0x10, 0x23, SysKernGetString(var_a0, var_a1, 0), 7);
        }
    }

    SysMenuUnkNoop(8);
    sp40[0] = 0;
    sp40[1] = 0;
    sp40[2] = 0x100;
    sp40[3] = 0x100;
    SysMenuSetDrawMode(0, 1, 0x7F, sp40);
    var_s1_2 = 0xD;
    if (D_801D3DE6 != 2) {
        var_s0 = 0;
        var_s6 = 0x30;
        var_s5 = 0x100;
        var_s4 = 0x38;
        var_s3 = 0x36;
        var_s2 = (s8*)0x3B;
        do {
            if (D_8009CBCF[var_s1_2] != 0xFF) {
                SysMenuDrawCharNameLvHpMpByPartyId(0x50, (s32)var_s2, (s32)var_s0);
                SysMenuDrawAvatar(0x16, var_s3, 0x30, 0x30, 0, var_s4, var_s6, var_s6, var_s1_2, 0);
                sp40[0] = 0;
                sp40[1] = 0;
                sp40[2] = var_s5;
                sp40[3] = var_s5;
                SysMenuSetDrawMode(0, 1, 0x7F, sp40);
            }
            var_s1_2 = var_s1_2 + 1;
            var_s4 += 0x30;
            var_s3 += 0x38;
            var_s0 += 1;
            var_s2 += 0x38;
        } while (((s32)var_s0) < 3);
        SysMenuSetWindowRect(sp38, 0, 0x32, 0xAA, 0xAB);
        SysMenuDrawWindow(sp38);
    }
    var_s2 = (s8*)0;
    var_s1_3 = (s8*)(&D_801D3CD4);
    var_s0 = (s8*)0x22;
    do {
        SysMenuDrawString((s32)var_s0, 0xD, var_s1_3, 7);
        var_s1_3 += 0xC;
        var_s2 += 1;
        var_s0 += 0x38;
    } while (((s32)var_s2) < 3);
    sp40[2] = 0x16C;
    sp40[3] = 0xE0;
    sp40[0] = 0;
    sp40[1] = 0;
    SysMenuSetDrawenv((void*)(((u8*)D_800706A4) + (g_MenuRenderBufferIndex * 0x5C)), sp40);
    if (D_801D3DE6 != 2) {
        if (g_ItemMenuCurrentScreen == 5) {
            if ((D_801D3D84 != 0) && (arg0 & 2)) {
                temp_v1_2 = ((D_801D3D8C - D_801D3E38) * 0x10) + (D_801D3E45 * 4);
                if (((u32)(temp_v1_2 + 0xB)) < 0x10FU) {
                    SysMenuDrawCursor(0xA5, temp_v1_2 + 0x38);
                }
            }
            SysMenuDrawCursor(0xA9, (D_801D3E41[0] * 0x10) + 0x3C);
            var_s5 = 5;
        } else {
            var_s5 = 1;
        }
        do {
            D_801D3E4C[0] = 0xA;
            D_801D3E4E = 0x140;
        } while (0);
        var_s0 = (s8*)(var_s5 * 0x12);
        temp_a1_2 = *((u16*)((((u8*)g_ItemMenuWidgets) + 2) + ((s32)var_s0)));
        D_801D3E52 = 0x160;
        D_801D3E54 = 0x35;
        D_801D3E56 = 0xA;
        D_801D3E58 = 0xA5;
        D_801D3E50 = temp_a1_2;
        var_s6 = 0xA;
        SysMenuDrawScrollbar(D_801D3E4C, temp_a1_2);
        temp_bool2 = *((s16*)(((u8*)D_801D3DE4) + ((s32)var_s0)));
        if (temp_bool2 != 0) {
            var_s6 = 0xB;
        }
        SysMenuUnkNoop(9);
        var_s2_4 = 0;
        if ((s16)var_s6 != 0) {
            do {
                new_var6 = 0x3A;
                var_s1 = (*((s16*)((((u8*)g_ItemMenuWidgets) + 2) + ((s32)var_s0)))) + var_s2_4;
                temp_a0 = *(D_8009CBE0 - (-var_s1));
                if ((temp_a0 & 0xFFFF) != 0xFFFF) {
                    var_s4 = temp_a0 & 0x1FF;
                    var_s3 = (-((func_801D0DCC(var_s4) & 4) == 0)) & 7;
                    SysMenuDrawString(0xD6, (var_s2_4 * 0x10) + ((D_801D3DEB[(s32)var_s0] * 4) + new_var6),
                                      SysKernGetString(4, var_s4, 8), var_s3);
                }
                var_s2_4 += 1;
            } while (var_s2_4 < var_s6);
        }
        var_s2_4 = 0;
        if (var_s6 != 0) {
            var_s5 *= 0x12;
            do {
                var_s1_2 = (*((s16*)((((u8*)g_ItemMenuWidgets) + 2) + var_s5))) + var_s2_4;
                temp_v1_3 = *(D_8009CBE0 + var_s1_2);
                var_s1_2 = temp_v1_3 & 0xFFFF;
                if (var_s1_2 != 0xFFFF) {
                    var_s4 = temp_v1_3 & 0x1FF;
                    var_s1_2 = (s32)(((u32)var_s1_2) >> 9);
                    new_var19 = func_801D0DCC(var_s4) & 4;
                    var_s3 = (-(new_var19 == 0)) & 7;
                    ((void (*)())ITEMMENU_DrawItemTypeIcon)(
                        0xC4, ((s8*)(var_s2_4 * 0x10)) + ((D_801D3DEB[var_s5] * 4) + 0x38), var_s4, 0);
                    SysMenuDrawSingleFontLetter(
                        0x13F, (s32)(((s8*)(var_s2_4 * 0x10)) + ((D_801D3DEB[var_s5] * 4) + 0x3C)), 0xD5, var_s3);
                    SysMenuDrawDigitsWithoutLeadingZeroes(
                        0x140, (s32)(((s8*)(var_s2_4 * 0x10)) + ((D_801D3DEB[var_s5] * 4) + 0x3B)), var_s1_2, 3,
                        var_s3);
                }
                var_s2_4 += 1;
            } while (var_s2_4 < var_s6);
        }
    } else {
        var_s6 = 0xA;
        D_801D3E4C[0] = 0xA;
        D_801D3E4E = 0x20;
        D_801D3E56 = 0xA;
        D_801D3E52 = 0x160;
        D_801D3E54 = 0x35;
        D_801D3E58 = 0xA5;
        D_801D3E50 = (u16)D_801D3E14;
        var_s2_6 = 0;
        var_s4 = 0x38;
        do {
        } while (0);
        ((void (*)())SysMenuDrawScrollbar)(D_801D3E4C);
        SysMenuUnkNoop(9);
        var_s0 = 0;
        do {
            var_s5 = var_s2_6 * 0x10;
            var_s3 = 0x20;
            var_s1 = (D_801D3E14 + var_s2_6) * 2;
        loop_66:
            var_s4 = var_s1 + ((s32)var_s0);

            temp_a1_3 = g_KeyItemList[var_s4];
            if (temp_a1_3 != 0xFF) {
                SysMenuDrawString(
                    var_s3, var_s5 + (new_var17 = (D_801D3E21 * 4) + 0x3A), SysKernGetString(0xE, temp_a1_3, 8), 7);
            }
            var_s0 += 1;
            var_s3 += 0xA6;
            if (((s32)var_s0) < 2) {
                goto loop_66;
            }
            var_s2_6 += 1;
            var_s0 = 0;
        } while (((s32)var_s2_6) < 0xC);
    }
    temp_s3 = 0x35;
    sp40[1] = temp_s3;
    sp40[2] = 0x16C;
    sp40[3] = 0xA5;
    sp40[0] = 0;
    SysMenuSetDrawenv((void*)(((u8*)D_800706A4) + (g_MenuRenderBufferIndex * 0x5C)), sp40);
    var_s0 = 0;
    var_s1_4 = (s8*)(&D_801D3D5C);
    do {
        SysMenuDrawWindow(var_s1_4);
        var_s0 += 1;
        var_s1_4 = var_s1_4 + 8;
    } while (((s32)var_s0) < 3);
    if (SysMenuGetMenuListState() == 0) {
        SysMenuHandleButtons((MenuTable*)(((u8*)g_ItemMenuWidgets) + (g_ItemMenuCurrentScreen * 0x12)));
        switch (g_ItemMenuCurrentScreen) {
        case 0:
            if (g_Pad0KeysPressed & 0x20) {
                PlayItemMenuSfx(1);
                new_var5 = (MenuTable*)(&D_801D3DE6);
                switch (*((s8*)new_var5)) {
                case 0:
                    g_ItemMenuCurrentScreen = 1;
                    return;

                case 1:
                    SysMenuSetCursorMovement((MenuTable*)(((s8*)new_var5) + 0x3E), 0, 0, 1, 8, 0, 0,
                                             (s32)(*((s8*)new_var5)), 8, 0, 0, 0, (s32)(*((s8*)new_var5)), 0);
                    g_ItemMenuCurrentScreen = 4;
                    return;

                case 2:
                    SysMenuSetCursorMovement((MenuTable*)(((s8*)new_var5) + 0x2C), 0, 0, 2, 0xA, 0, 0,
                                             (s32)(*((s8*)new_var5)), 0x20, 0, 0, (s32)(*((s8*)new_var5)), 0, 0);
                    g_ItemMenuCurrentScreen = 3;
                    return;
                }

            } else if (g_Pad0KeysRepeat & 0x40) {
                PlayItemMenuSfx(4);
                SysMenuSetMenuListAnimation(5, 0);
                SysMenuLoadMenuFileById(0);
                return;
            }
            break;

        case 1:
            if (D_801D3DF6 == 0) {
                if (g_Pad0KeysPressed & 0x20) {
                    var_s4 = D_801D3DF9[0] + D_801D3DF0;
                    temp_a0_2 = D_8009CBE0[var_s4];
                    if (((temp_a0_2 & 0xFFFF) != 0xFFFF) && (!(func_801D0DCC(var_s4 = temp_a0_2 & 0x1FF) & 4))) {
                        if (var_s4 != 0x62) {
                            if (var_s4 == 0x67) {
                                PlayItemMenuSfx(0x107);
                                D_8009CA51 = 1;
                                D_8009CA5E = 1;
                                D_8009CA50 = 6;
                                D_8009CA5F = 0xFF;
                                D_8009CA8C = 0xFFFFFF;
                                D_8009CAD5 = 1;
                                D_8009CAD4 = 7;
                                D_8009CAE2 = 1;
                                D_8009CAE3 = 0xFF;
                                D_8009CB10 = 0xFFFFFF;
                                return;
                            }
                            goto block_e48;
                        }
                        PlayItemMenuSfx(0x107);
                        D_8009D5E8 |= 1;
                        SysMenuSetMenuListAnimation(5, 0);
                        SysMenuLoadMenuFileById(0);
                        SysMenuClose();
                        return;
                    block_e48:
                        PlayItemMenuSfx(1);

                        D_801D3E5C = 0;
                        g_ItemMenuCurrentScreen = 2;
                        return;
                    }
                    PlayItemMenuSfx(3);
                    return;
                }
                var_v0_3 = g_Pad0KeysPressed & 0x40;
                goto block_217;
            }
            break;

        case 2:
            if (D_801D3E5C == 0) {
                if (g_Pad0KeysPressed & 0x20) {
                    temp_a0_4 = D_8009CBDC[D_801D3E0B[0]];
                    var_s4 = D_8009CBE0[D_801D3DF9[0] + D_801D3DF0] & 0x1FF;
                    new_var4 = var_s4 < 0x5FU;
                    temp_a0_3 = temp_a0_4;
                    var_v0_4 = new_var4;
                    if (temp_a0_3 == 0xFF) {
                        if ((var_s4 != 6) && (var_s4 != 0x46)) {
                            PlayItemMenuSfx(3);
                            return;
                        }
                    }
                    {
                        switch (var_s4) {
                        case 0xD:
                            temp_a0_4 = temp_a0_3 * 0x84;
                            temp_v1_4 = D_8009C757[temp_a0_4];
                            if (!(temp_v1_4 & 0x20)) {
                                if (!(temp_v1_4 & 0x10)) {
                                    var_v0_5 = temp_v1_4 | 0x20;
                                } else {
                                    var_v0_5 = temp_v1_4 & 0xEF;
                                }
                                D_8009C757[temp_a0_4] = var_v0_5;
                                PlayItemMenuSfx(0x107);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0xE:
                            temp_a0_5 = temp_a0_3;
                            temp_a0_5 *= 0x84;
                            temp_v1_5 = D_8009C757[temp_a0_5];
                            region_e_cond = 0x20;
                            region_e_cond = (temp_v1_5 & region_e_cond) != 0;
                            if (region_e_cond || ((temp_v1_5 & 0x10) == 0)) {
                                var_v0_6 = (region_e_cond) ? (temp_v1_5 & 0xDF) : (temp_v1_5 | 0x10);
                                D_8009C757[temp_a0_5] = var_v0_6;
                                PlayItemMenuSfx(0x107);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x57:

                        case 0x58:

                        case 0x59:

                        case 0x5A:

                        case 0x5B:

                        case 0x5C:

                        case 0x5D:

                        case 0x5E:
                            if (temp_a0_3 == D_801D3D25[var_s4]) {
                                if (func_801D0D24(temp_a0_3) != 0) {
                                    PlayItemMenuSfx(0x180);
                                    temp_v1_6 = D_801D3D25[var_s4] * 0x84;
                                    *((u16*)(((u8*)D_8009C75A) + temp_v1_6)) =
                                        (*((u16*)(((u8*)D_8009C75A) + temp_v1_6))) | 0x200;
                                    SysMenuRemoveItem(var_s4 | 0x200);
                                    ;
                                    if ((SysMenuSearchItem(var_s4) & 0xFFFF) == 0xFFFF) {
                                        g_ItemMenuCurrentScreen = 1;
                                    }
                                    func_801D0E4C(((var_s4 - 0x57) * 0x66) + D_801D3260);
                                    SysMenuRequestAddWindow((s32*)g_ItemMenuNotificationText, 7);
                                    return;
                                }
                                var_a0_2 = (s32*)(((var_s4 - 0x57) * 0x66) + ((s8*)(&D_801D3282)));
                                func_801D0E4C(var_a0_2);
                                SysMenuRequestAddWindow((s32*)g_ItemMenuNotificationText, 7);
                                PlayItemMenuSfx(3);
                                return;
                            }
                            if (temp_a0_3 == 6) {
                                var_a0_2 = &D_801D3590;
                            } else {
                                new_var6 = 6;
                                if (((s32)temp_a0_3) >= new_var6) {
                                    var_v0_8 = (temp_a0_3 - 1) * 3;
                                } else {
                                    var_v0_8 = temp_a0_3 * 3;
                                }
                                var_a0_2 = (s32*)(((var_v0_8 + 2) * 0x22) + ((s8*)D_801D3260));
                            }
                            func_801D0E4C(var_a0_2);
                            SysMenuRequestAddWindow((s32*)g_ItemMenuNotificationText, 7);
                            PlayItemMenuSfx(3);
                            return;

                        case 0x47:

                        case 0x48:

                        case 0x49:

                        case 0x4A:

                        case 0x4B:

                        case 0x4C:
                            switch (var_s4) {
                            case 0x47:
                                temp_a0_6 = temp_a0_3 * 0x84;
                                temp_v1_7 = D_8009C740[temp_a0_6];
                                if (temp_v1_7 < 0xFFU) {
                                    D_8009C740[temp_a0_6] = temp_v1_7 - (-1);
                                default:
                                    goto src_tail;

                                } else {
                                    PlayItemMenuSfx(3);
                                    return;
                                }
                                break;

                            case 0x48:
                                temp_a0_7 = 33 * (4 * temp_a0_3);
                                temp_v1_8 = D_8009C740[1 + temp_a0_7];
                                if (temp_v1_8 < 0xFFU) {
                                    D_8009C740[1 + temp_a0_7] = temp_v1_8 + 1;
                                    goto src_tail;
                                    PlayItemMenuSfx(3);
                                }
                                PlayItemMenuSfx(3);
                                return;

                            case 0x49:
                                temp_a0_8 = temp_a0_3 * 0x84;
                                temp_v1_9 = D_8009C740[2 + temp_a0_8];
                                if (temp_v1_9 < 0xFFU) {
                                    D_8009C740[2 + temp_a0_8] = temp_v1_9 + 1;
                                    goto src_tail;
                                }
                                PlayItemMenuSfx(3);
                                return;

                            case 0x4A:
                                temp_a0_9 = temp_a0_3 * 0x84;
                                temp_v1_10 = D_8009C740[3 + temp_a0_9];
                                if (temp_v1_10 < 0xFFU) {
                                    D_8009C740[3 + temp_a0_9] = temp_v1_10 + 1;
                                    goto src_tail;
                                }
                                PlayItemMenuSfx(3);
                                return;

                            case 0x4B:
                                temp_a0_10 = temp_a0_3 * 0x84;
                                temp_v1_11 = D_8009C744[temp_a0_10];
                                if (temp_v1_11 < 0xFFU) {
                                    D_8009C744[temp_a0_10] = temp_v1_11 + 1;
                                    goto src_tail;
                                }
                                PlayItemMenuSfx(3);
                                return;

                            case 0x4C:
                                temp_a0_11 = temp_a0_3 * 0x84;
                                temp_v1_12 = D_8009C744[1 + temp_a0_11];
                                if (temp_v1_12 < 0xFFU) {
                                    D_8009C744[1 + temp_a0_11] = temp_v1_12 + 1;
                                src_tail:
                                    PlayItemMenuSfx(0x107);

                                    SysInitPlayerStatFromEquip(D_801D3E0B[0]);
                                    SysInitPlayerStatFromMateria(*((u8*)D_801D3E0B));
                                    SysMenuRemoveItem(var_s4 | 0x200);
                                    if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                        return;
                                    }
                                    g_ItemMenuCurrentScreen = 1;
                                    return;
                                }
                                PlayItemMenuSfx(3);
                                return;
                            }

                            break;

                        case 0x0:
                            if ((func_801D0CAC(D_801D3E0B[0]) == 0) &&
                                ((*((s16*)(((u8*)D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0)) {
                                PlayItemMenuSfx(0x107);
                                SystemMenuAddHpByPartyId(D_801D3E0B[0], 0x64);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x1:
                            if ((func_801D0CAC(D_801D3E0B[0]) == 0) &&
                                ((*((s16*)(((u8*)D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0)) {
                                PlayItemMenuSfx(0x107);
                                SystemMenuAddHpByPartyId(D_801D3E0B[0], 0x1F4);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x3:
                            temp_s3 = func_801D0CE8(D_801D3E0B[0]) == 0;
                            var_s2_4 = (new_var7 = 0);
                            if (temp_s3 && ((*((s16*)(((u8*)D_8009D85C) + (D_801D3E0B[var_s2_4] * 0x440)))) != 0)) {
                                PlayItemMenuSfx(0x107);
                                SystemMenuAddMpByPartyId(D_801D3E0B[0], 0x64);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if (0xFFFF != (SysMenuSearchItem(var_s4) & 0xFFFF)) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x4:
                            if ((func_801D0CE8(D_801D3E0B[0]) == 0) &&
                                ((*((s16*)(((u8*)D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0)) {
                                PlayItemMenuSfx(0x107);
                                SystemMenuAddMpByPartyId(D_801D3E0B[0], 0x2710);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x7:
                            if ((*((s16*)(((u8*)D_8009D85C) + (0x440 * D_801D3E0B[0])))) == 0) {
                                PlayItemMenuSfx(0x107);
                                SystemMenuAddHpByPartyId(
                                    D_801D3E0B[0], (*((s16*)(((u8*)D_8009D85E) + (D_801D3E0B[0] * 0x440)))) / 4);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x46:
                            var_s0 = 0;
                            var_s1_5 = 0;
                            if (var_s4) {
                            }
                            do {
                                if ((D_8009CBDC[(s32)var_s0] != 0xFF) &&
                                    ((func_801D0CAC((s32)var_s0) == 0) || (func_801D0CE8((s32)var_s0) == 0))) {
                                    var_s1_5 = 1;
                                }
                                var_s0 += 1;
                            } while (((s32)var_s0) < 3);
                            var_s0 = 0;
                            if (var_s1_5 != 0) {
                                new_var15 = 0xFF;
                                var_s1_5 = 0;
                                do {
                                    if (((*((s16*)(((u8*)D_8009D85C) + var_s1_5))) != 0) &&
                                        (D_8009CBDC[(s32)var_s0] != new_var15)) {
                                        SystemMenuAddHpByPartyId((s32)var_s0, 0x2710);
                                        SystemMenuAddMpByPartyId((s32)var_s0, 0x2710);
                                    }
                                    var_s0 += 1;
                                    var_s1_5 += 0x440;
                                } while (((s32)var_s0) < 3);
                                PlayItemMenuSfx(0x107);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                temp_s3_2 = SysMenuSearchItem(var_s4);
                                if ((temp_s3_2 & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x2:
                            if ((func_801D0CAC(D_801D3E0B[0]) == 0) &&
                                ((*((s16*)(((u8*)D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0)) {
                                PlayItemMenuSfx(0x107);
                                SystemMenuAddHpByPartyId(D_801D3E0B[0], 0x2710);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x5:
                            if (((func_801D0CAC(D_801D3E0B[0]) == 0) || (func_801D0CE8(D_801D3E0B[0]) == 0)) &&
                                ((*((s16*)(((u8*)D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0)) {
                                PlayItemMenuSfx(0x107);
                                SystemMenuAddHpByPartyId(D_801D3E0B[0], 0x2710);
                                SystemMenuAddMpByPartyId(D_801D3E0B[0], 0x2710);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                temp_v1_5 = D_8009C757[temp_a0_5];
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                if (!D_801D3DE6) {
                                }
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;

                        case 0x6:
                            var_s0 = 0;
                            var_s1_7 = 0;
                            do {
                                if ((D_8009CBDC[(s32)var_s0] != 0xFF) &&
                                    ((func_801D0CAC((s32)var_s0) == 0) || (func_801D0CE8((s32)var_s0) == 0))) {
                                    var_s1_7 = 1;
                                }
                                var_s0 += 1;
                            } while (((s32)var_s0) < 3);
                            var_s0 = 0;
                            if (var_s1_7 != 0) {
                                new_var12 = 0xFF;
                                var_loopc = 0;
                                do {
                                    if (((*((s16*)(((u8*)D_8009D85C) + var_loopc))) != 0) &&
                                        (D_8009CBDC[(s32)var_s0] != new_var12)) {
                                        SystemMenuAddHpByPartyId((s32)var_s0, 0x2710);
                                        SystemMenuAddMpByPartyId((s32)var_s0, 0x2710);
                                    }
                                    var_s0 += 1;
                                    var_loopc += 0x440;
                                } while (((s32)var_s0) < 3);
                                PlayItemMenuSfx(0x107);
                                SysMenuRemoveItem(var_s4 | 0x200);
                                if ((SysMenuSearchItem(var_s4) & 0xFFFF) != 0xFFFF) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            } else {
                                PlayItemMenuSfx(3);
                                return;
                            }
                            break;
                        }
                    }
                } else if (g_Pad0KeysPressed & 0x40) {
                    PlayItemMenuSfx(4);
                    g_ItemMenuCurrentScreen = 1;
                    return;
                }
            }
            break;

        case 3:
            var_v0_3 = g_Pad0KeysPressed & 0x40;
            goto block_217;

        case 4:
            if (g_Pad0KeysPressed & 0x20) {
                PlayItemMenuSfx(1);
                temp_bool = D_801D3E2F[0] == 0;
                if (temp_bool) {
                    new_var18 = (MenuTable*)((&D_801D3E2F[0]) + 7);
                    SysMenuSetCursorMovement(new_var18, 0, 0, 1, 0xA, 0, 0, 1, 0x140, 0, 0, 0, 0, 0);
                    D_801D3D84 = 0;
                    D_801D3D88 = 0;
                    D_801D3D8C = 0;
                    g_ItemMenuCurrentScreen = 5;
                    return;
                }
                ArrangeItems(D_801D3E2F[0]);
                goto block_219;
            }
            var_v0_3 = g_Pad0KeysPressed & 0x40;
            goto block_217;

        case 5:
            if (g_Pad0KeysPressed & 0x20) {
                switch (D_801D3D84) {
                case 0:
                    PlayItemMenuSfx(1);
                    D_801D3D88 = (s32)D_801D3E40;
                    D_801D3D8C = D_801D3E41[0] + D_801D3E38;
                    D_801D3D84 += 1;
                    return;

                case 1:
                    PlayItemMenuSfx(1);
                    temp_v1_13 = &D_8009CBE0[D_801D3D8C];
                    new_var16 = D_801D3E41;
                    new_var = temp_v1_13;
                    temp_a0_12 = *new_var;
                    *temp_v1_13 = D_8009CBE0[new_var16[0] + D_801D3E38];
                    D_801D3D84 = 0;
                    D_8009CBE0[new_var16[0] + D_801D3E38] = temp_a0_12;
                    return;
                }

            } else {
                var_v0_3 = g_Pad0KeysPressed & 0x40;
            block_217:
                if (var_v0_3 != 0) {
                    PlayItemMenuSfx(4);
                block_219:
                    g_ItemMenuCurrentScreen = 0;
                }
            }
            break;
        }
    }
}
#endif

static void ITEMMENU_Noop(void) {}

static void EvictWeakestStolenMateria(s32 newMateria, s32 priority) {
    s32 slotIdx;
    s32* lootPtr;

    slotIdx = 0;
    lootPtr = g_MateriaStealLoot;
    do {
        if (g_MateriaPriority[*(u8*)lootPtr] == priority) {
            *lootPtr = newMateria;
            return;
        }
        slotIdx += 1;
        lootPtr += 1;
    } while (slotIdx < MAX_STOLEN_MATERIA);
}

static s32 GetLowestStealPriority(void) {
    s32 slotIdx;
    s32 lowestPriority;
    u8* lootPtr;

    lowestPriority = 0xFF;
    slotIdx = 0;
    lootPtr = (u8*)g_MateriaStealLoot;
    do {
        u8 materiaId = *lootPtr;
        s32 priority = g_MateriaPriority[materiaId];
        if (priority < lowestPriority) {
            lowestPriority = priority;
        }
        slotIdx += 1;
        lootPtr += 4;
    } while (slotIdx < MAX_STOLEN_MATERIA);
    return lowestPriority;
}

static void OfferMateriaToSteal(s32* materiaPtr) {
    s32 slotIdx;
    s32 lowestPriority;

    if (*materiaPtr == EMPTY_MATERIA_SLOT) {
        return;
    }
    slotIdx = 0;
    do {
        if (g_MateriaStealLoot[slotIdx] == EMPTY_MATERIA_SLOT) {
            g_MateriaStealLoot[slotIdx] = *materiaPtr;
            return;
        }
        slotIdx += 1;
    } while (slotIdx < MAX_STOLEN_MATERIA);

    lowestPriority = GetLowestStealPriority();
    if (g_MateriaPriority[*materiaPtr & 0xFF] < lowestPriority) {
        return;
    }
    EvictWeakestStolenMateria(*materiaPtr, lowestPriority);
}

// Re-equip a returned materia into the first free, unlocked weapon then armor
// slot of any visible party member. Returns 0 if placed, 1 if no slot was free.
static s32 ReequipReturnedMateria(s32 materia) {
    s32 charIdx;

    for (charIdx = NUM_CHARACTERS - 1; charIdx != -1; charIdx--) {
        if ((Savemap.phs_visibility_mask >> charIdx) & 1) {
            {
                s32 slotIdx;
                for (slotIdx = 0; slotIdx < NUM_MATERIA_ROW; slotIdx++) {
                    if (Savemap.party[charIdx].materia_weapon[slotIdx] == EMPTY_MATERIA_SLOT &&
                        g_WeaponTable[Savemap.party[charIdx].weapon].materiaSlot[slotIdx]) {
                        Savemap.party[charIdx].materia_weapon[slotIdx] = materia;
                        return 0;
                    }
                }
            }
            {
                s32 slotIdx;
                for (slotIdx = 0; slotIdx < NUM_MATERIA_ROW; slotIdx++) {
                    if (Savemap.party[charIdx].materia_armor[slotIdx] == EMPTY_MATERIA_SLOT &&
                        g_ArmorTable[Savemap.party[charIdx].armor].materiaSlot[slotIdx]) {
                        Savemap.party[charIdx].materia_armor[slotIdx] = materia;
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

static void RemoveMateriaFromPlayer(s32 materia) {
    s32 charIdx;
    s32 slotIdx;

    for (charIdx = 0; charIdx < NUM_CHARACTERS; charIdx++) {
        if ((Savemap.phs_visibility_mask >> charIdx) & 1) {
            for (slotIdx = 0; slotIdx < NUM_MATERIA_ROW; slotIdx++) {
                if (Savemap.party[charIdx].materia_weapon[slotIdx] == materia) {
                    Savemap.party[charIdx].materia_weapon[slotIdx] = EMPTY_MATERIA_SLOT;
                    return;
                }
            }
            for (slotIdx = 0; slotIdx < NUM_MATERIA_ROW; slotIdx++) {
                if (Savemap.party[charIdx].materia_armor[slotIdx] == materia) {
                    Savemap.party[charIdx].materia_armor[slotIdx] = EMPTY_MATERIA_SLOT;
                    return;
                }
            }
        }
    }
    for (slotIdx = 0; slotIdx < MAX_MATERIA_COUNT; slotIdx++) {
        if (Savemap.materia[slotIdx] == materia) {
            Savemap.materia[slotIdx] = EMPTY_MATERIA_SLOT;
            return;
        }
    }
}

static void FinalizeMateriaSteal(void) {
    s32 slotIdx;
    s32 materia;

    for (slotIdx = 0; slotIdx < MAX_STOLEN_MATERIA; slotIdx++) {
        materia = g_MateriaStealLoot[slotIdx];
        if (materia != EMPTY_MATERIA_SLOT) {
            RemoveMateriaFromPlayer(materia);
        }
    }
    for (slotIdx = 0; slotIdx < MAX_STOLEN_MATERIA; slotIdx++) {
        Savemap.yuffie_stolen_materia[slotIdx] = g_MateriaStealLoot[slotIdx];
    }
}

void ITEMMENU_StealAllMateria(void) {
    s32 lootIdx;
    s32 charIdx;
    s32 slotIdx;

    for (lootIdx = 0; lootIdx < MAX_STOLEN_MATERIA; lootIdx++) {
        g_MateriaStealLoot[lootIdx] = EMPTY_MATERIA_SLOT;
    }
    for (charIdx = 0; charIdx < NUM_CHARACTERS; charIdx++) {
        if ((Savemap.phs_visibility_mask >> charIdx) & 1) {
            for (slotIdx = 0; slotIdx < NUM_MATERIA_ROW; slotIdx++) {
                do {
                    OfferMateriaToSteal(&Savemap.party[charIdx].materia_weapon[slotIdx]);
                } while (0);
            }
            for (slotIdx = 0; slotIdx < NUM_MATERIA_ROW; slotIdx++) {
                OfferMateriaToSteal(&Savemap.party[charIdx].materia_armor[slotIdx]);
            }
        }
    }
    for (slotIdx = 0; slotIdx < MAX_MATERIA_COUNT; slotIdx++) {
        OfferMateriaToSteal(&Savemap.materia[slotIdx]);
    }
    FinalizeMateriaSteal();
}

// Give back every materia that was stolen: try to re-equip each one, and if no
// equip slot is free, return it to the materia inventory instead.
void ITEMMENU_ReturnStolenMateria(void) {
    s32 slotIdx;

    for (slotIdx = 0; slotIdx < MAX_STOLEN_MATERIA; slotIdx++) {
        if (Savemap.yuffie_stolen_materia[slotIdx] != EMPTY_MATERIA_SLOT) {
            if (ReequipReturnedMateria(Savemap.yuffie_stolen_materia[slotIdx]) != 0) {
                // no free equip slot - add it to the materia inventory
                SysMenuAddMateria(Savemap.yuffie_stolen_materia[slotIdx]);
            }
        }
    }
}

// Unequip a party member: move their 16 equipped materia into the materia
// inventory and their accessory into the item inventory.
void ITEMMENU_UnequipCharacterMateria(s32 charIdx) {
    u8 accessory;
    {
        s32 slotIdx = 0;
        s32 emptySlot = EMPTY_MATERIA_SLOT;
        s32* materiaSlotPtr = Savemap.party[charIdx].materia_weapon;
        do {
            if (*materiaSlotPtr != emptySlot) {
                SysMenuAddMateria(*materiaSlotPtr);
                *materiaSlotPtr = emptySlot;
            }
            slotIdx += 1;
            materiaSlotPtr += 1;
        } while (slotIdx < NUM_MATERIA_ROW);
    }
    {
        s32 slotIdx = 0;
        s32 emptySlot = EMPTY_MATERIA_SLOT;
        s32* materiaSlotPtr = Savemap.party[charIdx].materia_armor;
        do {
            if (*materiaSlotPtr != emptySlot) {
                SysMenuAddMateria(*materiaSlotPtr);
                *materiaSlotPtr = emptySlot;
            }
            slotIdx += 1;
            materiaSlotPtr += 1;
        } while (slotIdx < NUM_MATERIA_ROW);
    }
    accessory = Savemap.party[charIdx].accessory;
    if (accessory != EMPTY_ACCESSORY_SLOT) {
        SysMenuAddItem((accessory + ITEM_TYPE_ACCESSORY_BASE) | (1 << ITEM_QTY_SHIFT));
        Savemap.party[charIdx].accessory = EMPTY_ACCESSORY_SLOT;
    }
}

// Save the current party lineup, a party member's weapon/armor ids, the first
// three materia inventory slots and the member's 16 equipped materia into the
// stolen-materia buffer (reused as scratch space), clearing each source slot.
void ITEMMENU_BackupCharacterMateria(s32 charIdx) {
    s32 i = 0;
    u8* backupBuffer = (u8*)Savemap.yuffie_stolen_materia;
    {
        u8* destPtr = backupBuffer;
        do {
            *destPtr = Savemap.partyID[i];
            i += 1;
            destPtr += 1;
        } while (i < NUM_PARTY);
    }
    {
        s32 emptySlot;
        s32* materiaInvPtr;
        u8* destPtr;
        i = 0;
        emptySlot = EMPTY_MATERIA_SLOT;
        materiaInvPtr = Savemap.materia;
        backupBuffer[4] = Savemap.party[charIdx].weapon;
        destPtr = backupBuffer;
        backupBuffer[5] = Savemap.party[charIdx].armor;
        do {
            s32 materiaId = *materiaInvPtr;
            i += 1;
            *(s32*)(destPtr + 0x48) = materiaId;
            *materiaInvPtr = emptySlot;
            materiaInvPtr += 1;
            destPtr += 4;
        } while (i < NUM_PARTY);
    }
    {
        s32 emptySlot;
        s32 partyMemberOffset;
        s32* armorMateriaPtr;
        s32* weaponMateriaPtr;
        u8* destPtr;
        u8* weaponMateriaBase;
        u8* armorMateriaBase;
        i = 0;
        emptySlot = EMPTY_MATERIA_SLOT;
        partyMemberOffset = charIdx * sizeof(SavePartyMember);
        weaponMateriaBase = (u8*)Savemap.party[0].materia_weapon;
        armorMateriaBase = weaponMateriaBase + 0x20;
        armorMateriaPtr = (s32*)(armorMateriaBase + partyMemberOffset);
        weaponMateriaPtr = (s32*)(weaponMateriaBase + partyMemberOffset);
        destPtr = backupBuffer;
        do {
            s32 materiaId;
            materiaId = *weaponMateriaPtr;
            i += 1;
            *(s32*)(destPtr + 8) = materiaId;
            *weaponMateriaPtr = emptySlot;
            weaponMateriaPtr += 1;
            materiaId = *armorMateriaPtr;
            *(s32*)(destPtr + 0x28) = materiaId;
            *armorMateriaPtr = emptySlot;
            armorMateriaPtr += 1;
            destPtr += 2;
            destPtr += 2;
        } while (i < NUM_MATERIA_ROW);
    }
    Savemap.party[charIdx].weapon = 0;
}

// Restore everything saved by ITEMMENU_BackupCharacterMateria: party lineup, the
// member's weapon/armor ids, the first three materia inventory slots and
// their 16 equipped materia.
void ITEMMENU_RestoreCharacterMateria(s32 charIdx) {
    s32 i = 0;
    u8* backupBuffer = (u8*)Savemap.yuffie_stolen_materia;
    {
        u8* srcPtr = backupBuffer;
        do {
            Savemap.partyID[i] = *srcPtr;
            i += 1;
            srcPtr += 1;
        } while (i < NUM_PARTY);
    }
    {
        s32* materiaInvPtr;
        u8* srcPtr;
        i = 0;
        materiaInvPtr = Savemap.materia;
        Savemap.party[charIdx].weapon = backupBuffer[4];
        srcPtr = backupBuffer;
        Savemap.party[charIdx].armor = backupBuffer[5];
        do {
            s32 materiaId = *(s32*)(srcPtr + 0x48);
            srcPtr += 4;
            i += 1;
            *materiaInvPtr = materiaId;
            materiaInvPtr += 1;
        } while (i < NUM_PARTY);
    }
    {
        s32 partyMemberOffset;
        s32* armorMateriaPtr;
        s32* weaponMateriaPtr;
        u8* srcPtr;
        u8* weaponMateriaBase;
        u8* armorMateriaBase;
        i = 0;
        partyMemberOffset = charIdx * sizeof(SavePartyMember);
        weaponMateriaBase = (u8*)Savemap.party[0].materia_weapon;
        armorMateriaBase = weaponMateriaBase + 0x20;
        armorMateriaPtr = (s32*)(armorMateriaBase + partyMemberOffset);
        weaponMateriaPtr = (s32*)(weaponMateriaBase + partyMemberOffset);
        srcPtr = backupBuffer;
        do {
            s32 materiaId;
            materiaId = *(s32*)(srcPtr + 8);
            i += 1;
            *weaponMateriaPtr = materiaId;
            weaponMateriaPtr += 1;
            materiaId = *(s32*)(srcPtr + 0x28);
            *armorMateriaPtr = materiaId;
            armorMateriaPtr += 1;
            srcPtr += 2;
            srcPtr += 2;
        } while (i < NUM_MATERIA_ROW);
    }
}

// Uploads the coin-pattern texture at g_CoinTextureTim (64x32, 4bpp, seamlessly
// tileable) into VRAM: pixel data to (0x3F0, 0x120), CLUT to (0x110, 0x1E0).
// Runs once at boot/menu init (main -> func_80026258 -> HandleLoadCoinTexture); the
// texture stays resident so the battle UI can scroll it as the animated
// backdrop behind the coin-throw amount prompt.
void ITEMMENU_LoadCoinTexture(void) { MENU_LoadTim((u_long*)g_CoinTextureTim, 0x3F0, 0x120, 0x110, 0x1E0); }
