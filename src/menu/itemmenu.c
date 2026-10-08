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


//#ifndef NON_MATCHING
//INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", ITEMMENU_Main);
//#else 

/////////////////////////////////
// begin decomp work here
/////////////////////////////////


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




void SysMenuSetDrawMode(s32, s32, s32, RECT*);
void SysMenuDrawDigitsWithoutLeadingZeroes(s32, s32, s32, s32, s32);

s32 SysMenuGetMenuListState(void);


void SysMenuSetMenuListAnimation(s32, s32);
void SysInitPlayerStatFromEquip(s32);
void SysInitPlayerStatFromMateria(s32);
void ArrangeItems(s32);
void SysMenuUnkNoop(s32);


extern void SysMenuClose(void);
extern void SysMenuDrawAvatar(u8, s32, u8, u8, s32, s32, s32, s32, s32, s32);
extern void SysMenuDrawCharNameLvHpMpByPartyId(u8, s32, s32);
extern void SysMenuDrawCursor(s32, s32);
extern void SysMenuDrawMenuList(s32);
extern void SysMenuDrawScrollbar(s16*, u16);
extern void SysMenuDrawSingleFontLetter(u8, s32, u8, s32);
extern void SysMenuDrawWindow(void*);
extern void SysMenuLoadMenuFileById(u8);
extern void SysMenuRemoveItem(s32);
extern void SysMenuRequestAddWindow(u8*, u8);
extern s32 SysMenuSearchItem(u32);
extern void SysMenuSetDrawenv(DRAWENV*, s16*);
extern void SysMenuSetWindowRect(RECT*, u8, u8, u8, s32);
extern void SystemMenuAddHpByPartyId(s8, s32);
extern void SystemMenuAddMpByPartyId(s8, s32);
extern s32 func_801D0CAC(s8);
extern s32 func_801D0CE8(s8);
extern s32 func_801D0D24(u8);
extern s32 func_801D0DCC(s32);
extern void func_801D0E4C(void*);

extern u8 D_8009C740;
extern u8 D_8009C744;
extern u8 D_8009C757;
extern u8 D_8009C75A;
extern s8 D_8009CA50;
extern s8 D_8009CA51;
extern s8 D_8009CA5E;
extern s8 D_8009CA5F;
extern s32 D_8009CA8C;
extern s8 D_8009CAD4;
extern s8 D_8009CAD5;
extern s8 D_8009CAE2;
extern s8 D_8009CAE3;
extern s32 D_8009CB10;
extern u8 D_8009CBCF;
extern u8 D_8009CBDC;
extern u8 D_8009CBE0;
extern u8 D_8009D5E8;
extern u8 D_8009D85C;
extern u8 D_8009D85E;
extern u8 D_801D3260;
extern u8 D_801D3282;
extern u8 D_801D3590;
extern s8 D_801D3CD4;
extern s8 D_801D3CF8;
extern u8 D_801D3D25;
extern u8 D_801D3D5C;
extern UnkWindowRect D_801D3D74;
extern s16 D_801D3D76;
extern s32 D_801D3D84;
extern s32 D_801D3D88;
extern s32 D_801D3D8C;
extern u8 D_801D3DE4;
extern s8 D_801D3DE6;
extern u8 D_801D3DEB;
extern s16 D_801D3DF0;
extern s16 D_801D3DF6;
extern s8 D_801D3DF9;
extern s8 D_801D3E0B;
extern s16 D_801D3E14;
extern s8 D_801D3E1C;
extern s8 D_801D3E1D;
extern s8 D_801D3E21;
extern s8 D_801D3E2F;
extern s16 D_801D3E38;
extern s8 D_801D3E40;
extern s8 D_801D3E41;
extern s8 D_801D3E45;
extern s16 D_801D3E4C;
extern s16 D_801D3E4E;
extern u16 D_801D3E50;
extern s16 D_801D3E52;
extern s16 D_801D3E54;
extern s16 D_801D3E56;
extern s16 D_801D3E58;
extern s32 D_801D3E5C;

// --- Cleaned Function Body ---
void ITEMMENU_Main(s32 arg0) {
    RECT sp38;
    s16 sp40;
    s16 sp42;
    s16 sp44;
    s16 sp46;
    void* var_a0_2;
    void* var_s1_4;
    s16 var_a1_2;
    s32 temp_a0_10;
    s32 temp_a0_11;
    s32 temp_a0_4;
    s32 temp_a0_5;
    s32 temp_a0_6;
    s32 temp_a0_7;
    s32 temp_a0_8;
    s32 temp_a0_9;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s1_2;
    s32 temp_s3;
    s32 temp_s3_2;
    s32 temp_s4;
    s32 temp_s4_2;
    s32 temp_s4_3;
    s32 temp_s5;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_6;
    s32 var_a0;
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s0_3;
    s32 var_s0_4;
    s32 var_s0_5;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s1_5;
    s32 var_s1_6;
    s32 var_s1_7;
    s32 var_s1_8;
    s32 var_s2_2;
    s32 var_s2_3;
    s32 var_s2_4;
    s32 var_s2_5;
    s32 var_s2_6;
    s32 var_s3;
    s32 var_s3_2;
    s32 var_s4;
    s32 var_s4_2;
    s32 var_s5;
    s32 var_s6;
    s32 var_v0;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v0_7;
    s32 var_v0_8;
    s8 var_s0_6;
    s8 var_s0_7;
    s8 var_s0_8;
    s8 var_s0_9;
    s8 var_v0_2;
    s8 var_v0_5;
    s8 var_v0_6;
    s8* temp_a2;
    s8* var_s1_3;
    s8* var_s2;
    u16 temp_a0;
    u16 temp_a0_12;
    u16 temp_a0_2;
    u16 temp_a1;
    u16 temp_a1_2;
    u16 temp_v1_3;
    u16* temp_v1_13;
    u32 temp_s1;
    u32 temp_s4_4;
    u8 temp_a0_3;
    u8 temp_a1_3;
    u8 temp_v1_10;
    u8 temp_v1_11;
    u8 temp_v1_12;
    u8 temp_v1_4;
    u8 temp_v1_5;
    u8 temp_v1_7;
    u8 temp_v1_8;
    u8 temp_v1_9;
    u8 var_a1;

    SysMenuDrawMenuList(g_MenuRenderBufferIndex);
    if (g_ItemMenuCurrentScreen == 2) {
        var_v0 = arg0 & 2;
        if (D_801D3E5C == 0) {
            temp_v1 = *(&D_8009CBE0 + ((D_801D3DF9 + D_801D3DF0) * 2)) & 0x1FF;
            if (temp_v1 != 6) {
                if (temp_v1 == 0x46) {
                    goto block_5;
                }
                var_v0_2 = D_801D3E0B;
            } else {
block_5:
                var_v0_2 = arg0 % 3;
            }
            SysMenuDrawCursor(0, (var_v0_2 * 0x38) + 0x4B);
            var_v0 = arg0 & 2;
        }
        if (var_v0 != 0) {
            SysMenuDrawCursor(0xA9, (D_801D3DF9 * 0x10) + 0x3C);
        }
        if (D_801D3E5C != 0) {
            D_801D3E5C -= 1;
        }
    }
    SysMenuUnkNoop(0x80);
    switch (g_ItemMenuCurrentScreen) {              /* switch 1 */
    case 0:                                         /* switch 1 */
        SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        break;
    case 1:                                         /* switch 1 */
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        SysMenuDrawCursor(0xA9, (D_801D3DF9 * 0x10) + 0x3C);
        var_s4 = D_801D3DF9 + D_801D3DF0;
block_33:
        temp_a1 = *(&D_8009CBE0 + (var_s4 * 2));
        var_a0 = 4;
        if ((temp_a1 & 0xFFFF) != 0xFFFF) {
            var_a1 = temp_a1 & 0x1FF;
block_35:
            SysMenuDrawString(0x10, 0x23, SysKernGetString(var_a0, (s32) var_a1, 0), 7);
        }
        break;
    case 2:                                         /* switch 1 */
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        var_s4 = D_801D3DF9 + D_801D3DF0;
        goto block_33;
    case 3:                                         /* switch 1 */
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        SysMenuDrawCursor((D_801D3E1C * 0xA6) + 3, (D_801D3E1D * 0x10) + 0x3C);
        var_a1 = g_KeyItemList[((D_801D3E1D + D_801D3E14) * 2) + D_801D3E1C];
        var_a0 = 0xE;
        if (var_a1 != 0xFF) {
            goto block_35;
        }
        break;
    case 4:                                         /* switch 1 */
        var_s0 = 0;
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
            var_s0 = 0;
        }
        var_s2 = &D_801D3CF8;
        var_s1 = 6;
        SysMenuDrawCursor(D_801D3D74.unk0 - 0x12, D_801D3D76 + ((D_801D3E2F * 0xC) + 8));
        do {
            temp_a2 = var_s2;
            var_s2 += 0xC;
            var_s0 += 1;
            SysMenuDrawString(D_801D3D74.unk0 + 8, D_801D3D74.unk2 + var_s1, (u8*)temp_a2, 7);
            var_s1 += 0xC;
        } while (var_s0 < 8);
        sp40 = 0;
        sp42 = 0;
        sp44 = 0x100;
        sp46 = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, (RECT* ) &sp40);
        SysMenuDrawWindow(&D_801D3D74);
        break;
    case 5:                                         /* switch 1 */
        if (arg0 & 2) {
            SysMenuDrawCursor((D_801D3DE6 * 0x38) + 8, 0xC);
        }
        var_s4 = D_801D3E41 + D_801D3E38;
        goto block_33;
    }
    SysMenuUnkNoop(8);
    sp40 = 0;
    sp42 = 0;
    sp44 = 0x100;
    sp46 = 0x100;
    SysMenuSetDrawMode(0, 1, 0x7F, (RECT* ) &sp40);
    var_s1_2 = 0xD;
    if (D_801D3DE6 != 2) {
        var_s0_2 = 0;
        var_s4_2 = 0x38;
        var_s3 = 0x36;
        var_s2_2 = 0x3B;
        do {
            if (*(&D_8009CBCF + var_s1_2) != 0xFF) {
                SysMenuDrawCharNameLvHpMpByPartyId(0x50, var_s2_2, var_s0_2);
                SysMenuDrawAvatar(0x16, var_s3, 0x30, 0x30, 0, var_s4_2, 0x30, 0x30, var_s1_2, 0);
                sp40 = 0;
                sp42 = 0;
                sp44 = 0x100;
                sp46 = 0x100;
                SysMenuSetDrawMode(0, 1, 0x7F, (RECT* ) &sp40);
            }
            var_s1_2 += 1;
            var_s4_2 += 0x30;
            var_s3 += 0x38;
            var_s0_2 += 1;
            var_s2_2 += 0x38;
        } while (var_s0_2 < 3);
        SysMenuSetWindowRect(&sp38, 0, 0x32, 0xAA, 0xAB);
        SysMenuDrawWindow(&sp38);
    }
    var_s2_3 = 0;
    var_s1_3 = &D_801D3CD4;
    var_s0_3 = 0x22;
    do {
        SysMenuDrawString(var_s0_3, 0xD, (u8*)var_s1_3, 7);
        var_s1_3 += 0xC;
        var_s2_3 += 1;
        var_s0_3 += 0x38;
    } while (var_s2_3 < 3);
    sp44 = 0x16C;
    sp46 = 0xE0;
    sp40 = 0;
    sp42 = 0;
    SysMenuSetDrawenv(&D_800706A4[g_MenuRenderBufferIndex], &sp40);
    if (D_801D3DE6 != 2) {
        if (g_ItemMenuCurrentScreen == 5) {
            if ((D_801D3D84 != 0) && (arg0 & 2)) {
                temp_v1_2 = ((D_801D3D8C - D_801D3E38) * 0x10) + (D_801D3E45 * 4);
                if ((u32) (temp_v1_2 + 0xB) < 0x10FU) {
                    SysMenuDrawCursor(0xA5, temp_v1_2 + 0x38);
                }
            }
            var_s5 = 5;
            SysMenuDrawCursor(0xA9, (D_801D3E41 * 0x10) + 0x3C);
        } else {
            var_s5 = 1;
        }
        D_801D3E4C = 0xA;
        D_801D3E4E = 0x140;
        temp_s0 = var_s5 * 0x12;
        temp_a1_2 = *(&g_ItemMenuWidgets->rowOffset + temp_s0);
        D_801D3E52 = 0x160;
        D_801D3E54 = 0x35;
        D_801D3E56 = 0xA;
        D_801D3E58 = 0xA5;
        D_801D3E50 = temp_a1_2;
        var_s6 = 0xA;
        SysMenuDrawScrollbar(&D_801D3E4C, temp_a1_2);
        if (*(&D_801D3DE4 + temp_s0) != 0) {
            var_s6 = 0xB;
        }
        SysMenuUnkNoop(9);
        var_s2_4 = 0;
        if (var_s6 != 0) {
            do {
                temp_a0 = *(&D_8009CBE0 + ((*(&g_ItemMenuWidgets->rowOffset + temp_s0) + var_s2_4) * 2));
                temp_s4 = temp_a0 & 0x1FF;
                if ((temp_a0 & 0xFFFF) != 0xFFFF) {
                    temp_s3 = -((func_801D0DCC(temp_s4) & 4) == 0) & 7;
                    SysMenuDrawString(0xD6, (var_s2_4 * 0x10) + ((*(&D_801D3DEB + temp_s0) * 4) + 0x3A), SysKernGetString(4, temp_s4, 8), temp_s3);
                }
                var_s2_4 += 1;
            } while (var_s2_4 < var_s6);
        }
        var_s2_5 = 0;
        if (var_s6 != 0) {
            temp_s5 = var_s5 * 0x12;
            do {
                temp_v1_3 = *(&D_8009CBE0 + ((*(&g_ItemMenuWidgets->rowOffset + temp_s5) + var_s2_5) * 2));
                temp_s1 = temp_v1_3 & 0xFFFF;
                temp_s4_2 = temp_v1_3 & 0x1FF;
                if (temp_s1 != 0xFFFF) {
                    temp_s3_2 = -((func_801D0DCC(temp_s4_2) & 4) == 0) & 7;
                    temp_s0_2 = var_s2_5 * 0x10;
                    ITEMMENU_DrawItemTypeIcon(0xC4, temp_s0_2 + ((*(&D_801D3DEB + temp_s5) * 4) + 0x38), temp_s4_2);
                    SysMenuDrawSingleFontLetter(0x13F, temp_s0_2 + ((*(&D_801D3DEB + temp_s5) * 4) + 0x3C), 0xD5, temp_s3_2);
                    SysMenuDrawDigitsWithoutLeadingZeroes(0x140, temp_s0_2 + ((*(&D_801D3DEB + temp_s5) * 4) + 0x3B), (s32) (temp_s1 >> 9), 3, temp_s3_2);
                }
                var_s2_5 += 1;
            } while (var_s2_5 < var_s6);
        }
    } else {
        D_801D3E4C = 0xA;
        D_801D3E56 = 0xA;
        D_801D3E4E = 0x20;
        D_801D3E52 = 0x160;
        D_801D3E54 = 0x35;
        D_801D3E58 = 0xA5;
        D_801D3E50 = (u16) D_801D3E14;
        var_s2_6 = 0;
        SysMenuDrawScrollbar(&D_801D3E4C, 0); // Presumed 0 to match args if missing
        SysMenuUnkNoop(9);
        var_s0_4 = 0;
        do {
            var_s3_2 = 0x20;
            temp_s1_2 = (D_801D3E14 + var_s2_6) * 2;
loop_66:
            temp_a1_3 = g_KeyItemList[temp_s1_2 + var_s0_4];
            if (temp_a1_3 != 0xFF) {
                SysMenuDrawString(var_s3_2, (var_s2_6 * 0x10) + ((D_801D3E21 * 4) + 0x3A), SysKernGetString(0xE, (s32) temp_a1_3, 8), 7);
            }
            var_s0_4 += 1;
            var_s3_2 += 0xA6;
            if (var_s0_4 < 2) {
                goto loop_66;
            }
            var_s2_6 += 1;
            var_s0_4 = 0;
        } while (var_s2_6 < 0xC);
    }
    var_s0_5 = 0;
    var_s1_4 = &D_801D3D5C;
    sp42 = 0x35;
    sp44 = 0x16C;
    sp46 = 0xA5;
    sp40 = 0;
    SysMenuSetDrawenv(&D_800706A4[g_MenuRenderBufferIndex], &sp40);
    do {
        SysMenuDrawWindow(var_s1_4);
        var_s0_5 += 1;
        var_s1_4 = (void*)((u8*)var_s1_4 + 8);
    } while (var_s0_5 < 3);
    if (SysMenuGetMenuListState() == 0) {
        SysMenuHandleButtons(&g_ItemMenuWidgets[g_ItemMenuCurrentScreen]);
        switch (g_ItemMenuCurrentScreen) {          /* switch 2 */
        case 0:                                     /* switch 2 */
            if (g_Pad0KeysPressed & 0x20) {
                PlayItemMenuSfx(1U);
                switch (D_801D3DE6) {               /* switch 3; irregular */
                case 0:                             /* switch 3 */
                    g_ItemMenuCurrentScreen = 1;
                    return;
                case 1:                             /* switch 3 */
                    SysMenuSetCursorMovement((MenuTable* ) (&D_801D3DE6 + 0x3E), 0, 0, 1, 8, 0, 0, (s32) D_801D3DE6, 8, 0, 0, 0, (s32) D_801D3DE6, 0U);
                    g_ItemMenuCurrentScreen = 4;
                    return;
                case 2:                             /* switch 3 */
                    SysMenuSetCursorMovement((MenuTable* ) (&D_801D3DE6 + 0x2C), 0, 0, 2, 0xA, 0, 0, (s32) D_801D3DE6, 0x20, 0, 0, (s32) D_801D3DE6, 0, 0U);
                    g_ItemMenuCurrentScreen = 3;
                    return;
                }
            } else if (g_Pad0KeysRepeat & 0x40) {
                PlayItemMenuSfx(4U);
                SysMenuSetMenuListAnimation(5, 0);
                SysMenuLoadMenuFileById(0);
                return;
            }
            break;
        case 1:                                     /* switch 2 */
            if (D_801D3DF6 == 0) {
                var_v0_3 = g_Pad0KeysPressed & 0x40;
                if (g_Pad0KeysPressed & 0x20) {
                    temp_a0_2 = *(&D_8009CBE0 + ((D_801D3DF9 + D_801D3DF0) * 2));
                    temp_s4_3 = temp_a0_2 & 0x1FF;
                    if (((temp_a0_2 & 0xFFFF) != 0xFFFF) && !(func_801D0DCC(temp_s4_3) & 4)) {
                        if (temp_s4_3 != 0x62) {
                            if (temp_s4_3 == 0x67) {
                                PlayItemMenuSfx(0x107U);
                                D_8009CA50 = 6;
                                D_8009CA51 = 1;
                                D_8009CA5E = 1;
                                D_8009CA5F = 0xFF;
                                D_8009CA8C = 0xFFFFFF;
                                D_8009CAD5 = 1;
                                D_8009CAD4 = 7;
                                D_8009CAE2 = 1;
                                D_8009CAE3 = 0xFF;
                                D_8009CB10 = 0xFFFFFF;
                                return;
                            }
                            PlayItemMenuSfx(1U);
                            D_801D3E5C = 0;
                            g_ItemMenuCurrentScreen = 2;
                            return;
                        }
                        PlayItemMenuSfx(0x107U);
                        D_8009D5E8 |= 1;
                        SysMenuSetMenuListAnimation(5, 0);
                        SysMenuLoadMenuFileById(0);
                        SysMenuClose();
                        return;
                    }
block_201:
                    PlayItemMenuSfx(3U);
                    return;
                }
block_217:
                if (var_v0_3 != 0) {
                    PlayItemMenuSfx(4U);
block_219:
                    g_ItemMenuCurrentScreen = 0;
                }
            }
            break;
        case 2:                                     /* switch 2 */
            if (D_801D3E5C == 0) {
                if (g_Pad0KeysPressed & 0x20) {
                    temp_s4_4 = *(&D_8009CBE0 + ((D_801D3DF9 + D_801D3DF0) * 2)) & 0x1FF;
                    temp_a0_3 = *(&D_8009CBDC + D_801D3E0B);
                    var_v0_4 = temp_s4_4 < 0x5FU;
                    if (temp_a0_3 == 0xFF) {
                        if ((temp_s4_4 == 6) || (temp_s4_4 == 0x46)) {
                            var_v0_4 = temp_s4_4 < 0x5FU;
                            goto block_103;
                        }
                        goto block_201;
                    }
block_103:
                    if (var_v0_4 != 0) {
                        switch (temp_s4_4) {        /* switch 4 */
                        case 0xD:                   /* switch 4 */
                            temp_a0_4 = temp_a0_3 * 0x84;
                            temp_v1_4 = *(&D_8009C757 + temp_a0_4);
                            if (!(temp_v1_4 & 0x20)) {
                                var_v0_5 = temp_v1_4 & 0xEF;
                                if (!(temp_v1_4 & 0x10)) {
                                    var_v0_5 = temp_v1_4 | 0x20;
                                }
                                *(&D_8009C757 + temp_a0_4) = var_v0_5;
                                PlayItemMenuSfx(0x107U);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0xE:                   /* switch 4 */
                            temp_a0_5 = temp_a0_3 * 0x84;
                            temp_v1_5 = *(&D_8009C757 + temp_a0_5);
                            var_v0_6 = temp_v1_5 & 0xDF;
                            if ((temp_v1_5 & 0x20) || (var_v0_6 = temp_v1_5 | 0x10, ((temp_v1_5 & 0x10) == 0))) {
                                *(&D_8009C757 + temp_a0_5) = var_v0_6;
                                PlayItemMenuSfx(0x107U);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x57:                  /* switch 4 */
                        case 0x58:                  /* switch 4 */
                        case 0x59:                  /* switch 4 */
                        case 0x5A:                  /* switch 4 */
                        case 0x5B:                  /* switch 4 */
                        case 0x5C:                  /* switch 4 */
                        case 0x5D:                  /* switch 4 */
                        case 0x5E:                  /* switch 4 */
                            if (temp_a0_3 == *(&D_801D3D25 + temp_s4_4)) {
                                if (func_801D0D24(temp_a0_3) != 0) {
                                    PlayItemMenuSfx(0x180U);
                                    temp_v1_6 = *(&D_801D3D25 + temp_s4_4) * 0x84;
                                    *(&D_8009C75A + temp_v1_6) = *(&D_8009C75A + temp_v1_6) | 0x200;
                                    SysMenuRemoveItem(temp_s4_4 | 0x200);
                                    var_v0_7 = temp_s4_4 - 0x57;
                                    if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                        g_ItemMenuCurrentScreen = 1;
                                        var_v0_7 = temp_s4_4 - 0x57;
                                    }
                                    func_801D0E4C((void*)((var_v0_7 * 0x66) + &D_801D3260));
                                    SysMenuRequestAddWindow(g_ItemMenuNotificationText, 7);
                                    return;
                                }
                                var_a0_2 = (void*)(((temp_s4_4 - 0x57) * 0x66) + &D_801D3282);
                                goto block_120;
                            }
                            if (temp_a0_3 == 6) {
                                var_a0_2 = &D_801D3590;
                            } else {
                                if ((s32) temp_a0_3 >= 6) {
                                    var_v0_8 = (temp_a0_3 - 1) * 3;
                                } else {
                                    var_v0_8 = temp_a0_3 * 3;
                                }
                                var_a0_2 = (void*)(((var_v0_8 + 2) * 0x22) + &D_801D3260);
                            }
block_120:
                            func_801D0E4C(var_a0_2);
                            SysMenuRequestAddWindow(g_ItemMenuNotificationText, 7);
                            goto block_201;
                        case 0x47:                  /* switch 4 */
                        case 0x48:                  /* switch 4 */
                        case 0x49:                  /* switch 4 */
                        case 0x4A:                  /* switch 4 */
                        case 0x4B:                  /* switch 4 */
                        case 0x4C:                  /* switch 4 */
                            switch (temp_s4_4) {    /* switch 5 */
                            case 0x47:              /* switch 5 */
                                temp_a0_6 = temp_a0_3 * 0x84;
                                temp_v1_7 = *(&D_8009C740 + temp_a0_6);
                                if (temp_v1_7 < 0xFFU) {
                                    *(&D_8009C740 + temp_a0_6) = temp_v1_7 + 1;
                                default:            /* switch 5 */
block_141:
                                    PlayItemMenuSfx(0x107U);
                                    SysInitPlayerStatFromEquip((s32) D_801D3E0B);
                                    SysInitPlayerStatFromMateria((s32) (u8) D_801D3E0B);
                                    SysMenuRemoveItem(temp_s4_4 | 0x200);
                                    if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                        goto block_204;
                                    }
                                } else {
                                    goto block_201;
                                }
                                break;
                            case 0x48:              /* switch 5 */
                                temp_a0_7 = temp_a0_3 * 0x84;
                                temp_v1_8 = *(&D_8009C740 + 1 + temp_a0_7);
                                if (temp_v1_8 < 0xFFU) {
                                    *(&D_8009C740 + 1 + temp_a0_7) = temp_v1_8 + 1;
                                    goto block_141;
                                }
                                goto block_201;
                            case 0x49:              /* switch 5 */
                                temp_a0_8 = temp_a0_3 * 0x84;
                                temp_v1_9 = *(&D_8009C740 + 2 + temp_a0_8);
                                if (temp_v1_9 < 0xFFU) {
                                    *(&D_8009C740 + 2 + temp_a0_8) = temp_v1_9 + 1;
                                    goto block_141;
                                }
                                goto block_201;
                            case 0x4A:              /* switch 5 */
                                temp_a0_9 = temp_a0_3 * 0x84;
                                temp_v1_10 = *(&D_8009C740 + 3 + temp_a0_9);
                                if (temp_v1_10 < 0xFFU) {
                                    *(&D_8009C740 + 3 + temp_a0_9) = temp_v1_10 + 1;
                                    goto block_141;
                                }
                                goto block_201;
                            case 0x4B:              /* switch 5 */
                                temp_a0_10 = temp_a0_3 * 0x84;
                                temp_v1_11 = *(&D_8009C744 + temp_a0_10);
                                if (temp_v1_11 < 0xFFU) {
                                    *(&D_8009C744 + temp_a0_10) = temp_v1_11 + 1;
                                    goto block_141;
                                }
                                goto block_201;
                            case 0x4C:              /* switch 5 */
                                temp_a0_11 = temp_a0_3 * 0x84;
                                temp_v1_12 = *(&D_8009C744 + 1 + temp_a0_11);
                                if (temp_v1_12 < 0xFFU) {
                                    *(&D_8009C744 + 1 + temp_a0_11) = temp_v1_12 + 1;
                                    goto block_141;
                                }
                                goto block_201;
                            }
                            break;
                        case 0x0:                   /* switch 4 */
                            if ((func_801D0CAC(D_801D3E0B) == 0) && (*(&D_8009D85C + (D_801D3E0B * 0x440)) != 0)) {
                                PlayItemMenuSfx(0x107U);
                                SystemMenuAddHpByPartyId(D_801D3E0B, 0x64);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x1:                   /* switch 4 */
                            if ((func_801D0CAC(D_801D3E0B) == 0) && (*(&D_8009D85C + (D_801D3E0B * 0x440)) != 0)) {
                                PlayItemMenuSfx(0x107U);
                                SystemMenuAddHpByPartyId(D_801D3E0B, 0x1F4);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x3:                   /* switch 4 */
                            if ((func_801D0CE8(D_801D3E0B) == 0) && (*(&D_8009D85C + (D_801D3E0B * 0x440)) != 0)) {
                                PlayItemMenuSfx(0x107U);
                                SystemMenuAddMpByPartyId(D_801D3E0B, 0x64);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x4:                   /* switch 4 */
                            if ((func_801D0CE8(D_801D3E0B) == 0) && (*(&D_8009D85C + (D_801D3E0B * 0x440)) != 0)) {
                                PlayItemMenuSfx(0x107U);
                                SystemMenuAddMpByPartyId(D_801D3E0B, 0x2710);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x7:                   /* switch 4 */
                            if (*(&D_8009D85C + (D_801D3E0B * 0x440)) == 0) {
                                PlayItemMenuSfx(0x107U);
                                var_a1_2 = *(&D_8009D85E + (D_801D3E0B * 0x440));
                                if (var_a1_2 < 0) {
                                    var_a1_2 += 3;
                                }
                                SystemMenuAddHpByPartyId(D_801D3E0B, var_a1_2 >> 2);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x46:                  /* switch 4 */
                            var_s0_6 = 0;
                            var_s1_5 = 0;
                            do {
                                if ((*(&D_8009CBDC + var_s0_6) != 0xFF) && ((func_801D0CAC(var_s0_6) == 0) || (func_801D0CE8(var_s0_6) == 0))) {
                                    var_s1_5 = 1;
                                }
                                var_s0_6 += 1;
                            } while (var_s0_6 < 3);
                            var_s0_7 = 0;
                            if (var_s1_5 != 0) {
                                var_s1_6 = 0;
                                do {
                                    if ((*(&D_8009D85C + var_s1_6) != 0) && (*(&D_8009CBDC + var_s0_7) != 0xFF)) {
                                        SystemMenuAddHpByPartyId(var_s0_7, 0x2710);
                                        SystemMenuAddMpByPartyId(var_s0_7, 0x2710);
                                    }
                                    var_s0_7 += 1;
                                    var_s1_6 += 0x440;
                                } while (var_s0_7 < 3);
                                PlayItemMenuSfx(0x107U);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x2:                   /* switch 4 */
                            if ((func_801D0CAC(D_801D3E0B) == 0) && (*(&D_8009D85C + (D_801D3E0B * 0x440)) != 0)) {
                                PlayItemMenuSfx(0x107U);
                                SystemMenuAddHpByPartyId(D_801D3E0B, 0x2710);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x5:                   /* switch 4 */
                            if (((func_801D0CAC(D_801D3E0B) == 0) || (func_801D0CE8(D_801D3E0B) == 0)) && (*(&D_8009D85C + (D_801D3E0B * 0x440)) != 0)) {
                                PlayItemMenuSfx(0x107U);
                                SystemMenuAddHpByPartyId(D_801D3E0B, 0x2710);
                                SystemMenuAddMpByPartyId(D_801D3E0B, 0x2710);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        case 0x6:                   /* switch 4 */
                            var_s0_8 = 0;
                            var_s1_7 = 0;
                            do {
                                if ((*(&D_8009CBDC + var_s0_8) != 0xFF) && ((func_801D0CAC(var_s0_8) == 0) || (func_801D0CE8(var_s0_8) == 0))) {
                                    var_s1_7 = 1;
                                }
                                var_s0_8 += 1;
                            } while (var_s0_8 < 3);
                            var_s0_9 = 0;
                            if (var_s1_7 != 0) {
                                var_s1_8 = 0;
                                do {
                                    if ((*(&D_8009D85C + var_s1_8) != 0) && (*(&D_8009CBDC + var_s0_9) != 0xFF)) {
                                        SystemMenuAddHpByPartyId(var_s0_9, 0x2710);
                                        SystemMenuAddMpByPartyId(var_s0_9, 0x2710);
                                    }
                                    var_s0_9 += 1;
                                    var_s1_8 += 0x440;
                                } while (var_s0_9 < 3);
                                PlayItemMenuSfx(0x107U);
                                SysMenuRemoveItem(temp_s4_4 | 0x200);
                                if ((SysMenuSearchItem(temp_s4_4) & 0xFFFF) == 0xFFFF) {
                                    goto block_204;
                                }
                            } else {
                                goto block_201;
                            }
                            break;
                        }
                    }
                } else if (g_Pad0KeysPressed & 0x40) {
                    PlayItemMenuSfx(4U);
block_204:
                    g_ItemMenuCurrentScreen = 1;
                    return;
                }
            }
            break;
        case 3:                                     /* switch 2 */
            var_v0_3 = g_Pad0KeysPressed & 0x40;
            goto block_217;
        case 4:                                     /* switch 2 */
            var_v0_3 = g_Pad0KeysPressed & 0x40;
            if (g_Pad0KeysPressed & 0x20) {
                PlayItemMenuSfx(1U);
                if (D_801D3E2F == 0) {
                    SysMenuSetCursorMovement((MenuTable* ) (&D_801D3E2F + 7), 0, 0, 1, 0xA, 0, 0, 1, 0x140, 0, 0, 0, 0, 0U);
                    D_801D3D84 = 0;
                    D_801D3D88 = 0;
                    D_801D3D8C = 0;
                    g_ItemMenuCurrentScreen = 5;
                    return;
                }
                ArrangeItems((s32) D_801D3E2F);
                goto block_219;
            }
            goto block_217;
        case 5:                                     /* switch 2 */
            if (g_Pad0KeysPressed & 0x20) {
                switch (D_801D3D84) {               /* switch 6; irregular */
                case 0:                             /* switch 6 */
                    PlayItemMenuSfx(1U);
                    D_801D3D88 = (s32) D_801D3E40;
                    D_801D3D84 += 1;
                    D_801D3D8C = D_801D3E41 + D_801D3E38;
                    return;
                case 1:                             /* switch 6 */
                    PlayItemMenuSfx(1U);
                    temp_v1_13 = (u16*)((D_801D3D8C * 2) + &D_8009CBE0);
                    temp_a0_12 = *temp_v1_13;
                    *temp_v1_13 = *(((D_801D3E41 + D_801D3E38) * 2) + &D_8009CBE0);
                    D_801D3D84 = 0;
                    *(((D_801D3E41 + D_801D3E38) * 2) + &D_8009CBE0) = temp_a0_12;
                    return;
                }
            } else {
                var_v0_3 = g_Pad0KeysPressed & 0x40;
                goto block_217;
            }
            break;
        default:                                        /* switch 2 */
            break;
        }
    } else {
    }
}
//////////////////////////////////
//End decomp work here. 
/////////////////////////////////
//#endif


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
