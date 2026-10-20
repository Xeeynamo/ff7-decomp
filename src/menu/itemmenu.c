//! PSYQ=3.3 CC1=2.7.2
#include <game.h>

// Item-menu screen/sub-state selector.
typedef enum {
    ITEMMENU_SCREEN_SELECTOR = 0,
    ITEMMENU_SCREEN_USE = 1,
    ITEMMENU_SCREEN_ITEM_SELECTED = 2,
    ITEMMENU_SCREEN_KEY_ITEMS = 3,
    ITEMMENU_SCREEN_ARRANGE_SELECT = 4,
    ITEMMENU_SCREEN_ARRANGE_CUSTOM = 5
} ItemMenuScreen;

typedef enum {
    USE_ITEM_MENU_SELECTED = 0,
    ARRANGE_MENU_SELECTED = 1,
    KEY_ITEMS_MENU_SELECTED = 2,
} ItemMenuSelected;

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
#define ITEM_TYPE_REGUALR_BASE 0x5FU

#define ITEM_ICON_BASE_U 0x60
#define ITEM_ICON_BASE_V 0x70
#define ITEM_ICON_SIZE 0x10
#define ITEM_ICON_CLUT 1

#define ITEM_ID_MASK 0x1FF
#define ITEM_CONSUMED 0x200
#define ITEM_QTY_SHIFT 9
#define ITEM_EMPTY_SLOT 0xFFFF
#define SORT_KEY_EMPTY_SLOT 0x4E20

#define CLEAR_SADNESS 0xEF
#define CLEAR_FURY 0xDF

#define ITEM_USAGE_FLAG_BATTLE 0x2
#define ITEM_USAGE_FLAG_FIELD 0x4
#define ITEM_USAGE_FLAG_THROW 0x8

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
static s32 Quicksort(s32 base, s32 count, SortCmp cmp, SortSwap swap);
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
static s32 GetItemUseFlagsContext(s32 itemId) {
    s32 flags = SysMenuGetInventoryRestrictionMask(itemId);
    if (itemId != ITEM_TENT) {
        if (itemId == ITEM_SAVE_CRYSTAL) {
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

void ArrangeItems(s32 mode);
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
s32 IsCharacterHpFull(s32 charIdx);
s32 IsCharacterMpFull(s32 charIdx);

extern s32 g_LimitManualAlreadyLearnedMsg;
extern s32 g_LimitManualNoUserMsg;
extern s32 g_ItemMenuTabLabels;
extern s32 g_ItemArrangeLabels;
extern u8 g_LimitManualCharIds[];
extern s32 g_ItemMenuWindows;
extern RECT g_ItemArrangeWindow;
extern s32 g_ItemSwapState;
extern s32 g_ItemSwapSourceColumn;
extern s32 g_ItemSwapSourceSlot;
extern s16 g_ItemListOffet;
extern s16 g_ItemListScrolling;
extern s8 g_ItemUseListCursorPos[];
extern s8 g_PartySlotSelected[];
extern s16 g_KeyItemListOffset;
extern s8 g_KeyItemCursorColumn[];
extern s8 g_KeyItemCursorRow;
extern s8 g_KeyItemScrollAnimY;
extern s8 g_ArrangeMenuCursorRow[];
extern s16 g_ItemSwapListOffset;
extern s8 g_ItemSwapCursorColumn;
extern s8 g_ItemSwapCursorRow[];
extern s8 g_ItemSwapScrollAnimY;
extern s16 g_ItemMenuScrollbar[];
extern s16 g_ItemMenuScrollbarTotalRows;
extern u16 g_ItemMenuScrollbarTopRow;
extern s16 g_ItemMenuScrollbarTrackX;
extern s16 g_ItemMenuScrollbarTrackY;
extern s16 g_ItemMenuScrollbarTrackW;
extern s16 g_ItemMenuScrollbarTrackH;
extern s32 g_ItemMenuTargetSelectDelay;
void g_LimitManualMsgs();

char ITEMMENU_Main(s32 frameCount) {
    u8 canRestoreMp;
    s32 subWindowRect[2];
    s16 drawRect[12];
    s32* msgPtr;
    s8* tabNamePtr;
    s8* windowDefPtr;
    s8* menuStrPtr;
    s32 yBaseOffset;
    s32 stringCategory;
    s32 cursorPosX;
    s32 charStructOffset;
    s8* cursorRowPtr;
    s32 itemSearchRes;
    s32 cursorItemId;
    s32 cursorYOffset;
    s8* slotIdx;
    MenuTable* menuWidget;
    s32 itemSlot;
    s32 charSlot;
    s32 canHealAnyParty;
    s32 canHealPartyMember;
    s32 isRegularItem;
    s32 rowIdx;
    s32 scrollAnimOffset;
    s32 keyRowIdx;
    s32 textColor;
    s32 selectedItemId;
    s32 itemUsableFlag;
    s32 charHpOffset;
    s32 widgetScreen;
    s32 hasScrollAnimation;
    s32 numVisibleRows;
    s32 hasFuryCondition;
    s32 cancelButtonPressed;
    s32 isItemUsable;
    s32 charMsgIdx;
    s32 isCustomizeMode;
    MenuTable* arrangeWidget;
    s32 emptySlot;
    s32 targetPartySlot;
    s8 newStatusFlags;
    s8 newStatusFlagsTranquilizer;
    u16* sourceSlotPtr;
    u16 invItem;
    u16 swappedItem;
    u16 selectedItemEntry;
    u32 scrollPos;
    s32 emptyKeyItem;
    u32 characterId;
    u8 keyItemId;
    u8 spiritBonus;
    s32 emptyPartySlot;
    u8 dexterityBonus;
    u8 luckBonus;
    u8 statusFlags;
    u8 statusFlagsTranquilizer;
    u8 strengthBonus;
    u8 vitalityBonus;
    u8 magicBonus;
    u16* swapItemPtr;
    u16 rawItemData;
    SysMenuDrawMenuList(g_MenuRenderBufferIndex);
    if (g_ItemMenuCurrentScreen == ITEMMENU_SCREEN_ITEM_SELECTED) {
        if (g_ItemMenuTargetSelectDelay == 0) {
            cursorItemId = Savemap.inventory[g_ItemUseListCursorPos[0] + g_ItemListOffet] & ITEM_ID_MASK;
            // Selecting all with one item (Tent & Megaelixer heal all)
            if (((cursorItemId == ITEM_MEGALIXIR) || (cursorItemId == ITEM_TENT)) != 0) {
                targetPartySlot = frameCount;
                targetPartySlot = targetPartySlot % 3;
                SysMenuDrawCursor(0, (targetPartySlot * 0x38) + 0x4B);
            } else {
                targetPartySlot = g_PartySlotSelected[0];
                SysMenuDrawCursor(0, (targetPartySlot * 0x38) + 0x4B);
            }
        }
        if ((frameCount & 2) != 0) {
            SysMenuDrawCursor(0xA9, (g_ItemUseListCursorPos[0] * 0x10) + 0x3C);
        }
        if (g_ItemMenuTargetSelectDelay) {
            g_ItemMenuTargetSelectDelay -= 1;
        }
    }
    SysMenuUnkNoop(0x80);
    switch (g_ItemMenuCurrentScreen) {
    case ITEMMENU_SCREEN_SELECTOR:
        SysMenuDrawCursor((g_ItemMenuWidgets[0].column * 0x38) + 8, 0xC);
        break;

    case ITEMMENU_SCREEN_USE:
        if (frameCount & 2) {
            SysMenuDrawCursor((g_ItemMenuWidgets[0].column * 0x38) + 8, 0xC);
        }
        SysMenuDrawCursor(0xA9, (g_ItemUseListCursorPos[0] * 0x10) + 0x3C);
        selectedItemId = g_ItemUseListCursorPos[0] + g_ItemListOffet;
        goto block_33;

    case ITEMMENU_SCREEN_ITEM_SELECTED:
        if (frameCount & 2) {
            SysMenuDrawCursor((g_ItemMenuWidgets[0].column * 0x38) + 8, 0xC);
        }
        selectedItemId = g_ItemUseListCursorPos[0] + g_ItemListOffet;
        goto block_33;

    case ITEMMENU_SCREEN_KEY_ITEMS:
        if (frameCount & 2) {
            SysMenuDrawCursor((g_ItemMenuWidgets[0].column * 0x38) + 8, 0xC);
        }
        SysMenuDrawCursor((g_KeyItemCursorColumn[0] * 0xA6) + 3, (g_KeyItemCursorRow * 0x10) + 0x3C);
        emptyKeyItem = 0xFF;
        selectedItemId = ((g_KeyItemCursorRow + g_KeyItemListOffset) * 2) + g_KeyItemCursorColumn[0];
        rawItemData = g_KeyItemList[selectedItemId];
        stringCategory = 0xE;
        if (rawItemData != emptyKeyItem) {
            do {
            } while (0);
            goto block_35;
        }
        break;

    case ITEMMENU_SCREEN_ARRANGE_SELECT:
        if (frameCount & 2) {
            SysMenuDrawCursor((g_ItemMenuWidgets[0].column * 0x38) + 8, 0xC);
        }
        slotIdx = 0;
        cursorPosX = g_ItemArrangeWindow.x;
        textColor = (s32)&g_ItemArrangeWindow;
        menuStrPtr = (s8*)&g_ItemArrangeLabels;
        itemSlot = 6;
        SysMenuDrawCursor(cursorPosX - 0x12, (g_ItemArrangeWindow.y + 8) + g_ArrangeMenuCursorRow[0] * 12);
        do {
            SysMenuDrawString(((RECT*)textColor)->x + 8, ((RECT*)textColor)->y + itemSlot, (const char*)menuStrPtr, 7);
            menuStrPtr += 0xC;
            slotIdx += 1;
            itemSlot += 0xC;
        } while (((s32)slotIdx) < 8);
        drawRect[0] = 0;
        drawRect[1] = 0;
        drawRect[2] = 0x100;
        drawRect[3] = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, drawRect);
        SysMenuDrawWindow((s32*)(&g_ItemArrangeWindow));

        break;

    case ITEMMENU_SCREEN_ARRANGE_CUSTOM:
        if (frameCount & 2) {
            SysMenuDrawCursor((g_ItemMenuWidgets[0].column * 0x38) + 8, 0xC);
        }
        selectedItemId = g_ItemSwapCursorRow[0] + g_ItemSwapListOffset;
    block_33:
        rawItemData = Savemap.inventory[selectedItemId];

        stringCategory = 4;
        if ((rawItemData & 0xFFFF) != 0xFFFF) {
            rawItemData = rawItemData & ITEM_ID_MASK;
        block_35:
            SysMenuDrawString(0x10, 0x23, SysKernGetString(stringCategory, rawItemData, 0), 7);
        }
    }

    SysMenuUnkNoop(8);
    drawRect[0] = 0;
    drawRect[1] = 0;
    drawRect[2] = 0x100;
    drawRect[3] = 0x100;
    SysMenuSetDrawMode(0, 1, 0x7F, drawRect);
    charSlot = 0xD;
    if (g_ItemMenuWidgets[0].column != 2) {
        slotIdx = 0;
        numVisibleRows = 0x30;
        widgetScreen = 0x100;
        selectedItemId = 0x38;
        textColor = 0x36;
        menuStrPtr = (s8*)0x3B;
        do {
            if (((u8*)&Savemap.partyID[0] - 0xD)[charSlot] != 0xFF) {
                SysMenuDrawCharNameLvHpMpByPartyId(0x50, (s32)menuStrPtr, (s32)slotIdx);
                SysMenuDrawAvatar(
                    0x16, textColor, 0x30, 0x30, 0, selectedItemId, numVisibleRows, numVisibleRows, charSlot, 0);
                drawRect[0] = 0;
                drawRect[1] = 0;
                drawRect[2] = widgetScreen;
                drawRect[3] = widgetScreen;
                SysMenuSetDrawMode(0, 1, 0x7F, drawRect);
            }
            charSlot = charSlot + 1;
            selectedItemId += 0x30;
            textColor += 0x38;
            slotIdx += 1;
            menuStrPtr += 0x38;
        } while (((s32)slotIdx) < 3);
        SysMenuSetWindowRect(subWindowRect, 0, 0x32, 0xAA, 0xAB);
        SysMenuDrawWindow(subWindowRect);
    }
    menuStrPtr = (s8*)0;
    tabNamePtr = (s8*)(&g_ItemMenuTabLabels);
    slotIdx = (s8*)0x22;
    do {
        SysMenuDrawString((s32)slotIdx, 0xD, tabNamePtr, 7);
        tabNamePtr += 0xC;
        menuStrPtr += 1;
        slotIdx += 0x38;
    } while (((s32)menuStrPtr) < 3);
    drawRect[2] = 0x16C;
    drawRect[3] = 0xE0;
    drawRect[0] = 0;
    drawRect[1] = 0;
    SysMenuSetDrawenv((void*)(((u8*)D_800706A4) + (g_MenuRenderBufferIndex * 0x5C)), drawRect);
    if (g_ItemMenuWidgets[0].column != 2) {
        if (g_ItemMenuCurrentScreen == 5) {
            if ((g_ItemSwapState != 0) && (frameCount & 2)) {
                cursorYOffset = ((g_ItemSwapSourceSlot - g_ItemSwapListOffset) * 0x10) + (g_ItemSwapScrollAnimY * 4);
                if (((u32)(cursorYOffset + 0xB)) < 0x10FU) {
                    SysMenuDrawCursor(0xA5, cursorYOffset + 0x38);
                }
            }
            SysMenuDrawCursor(0xA9, (g_ItemSwapCursorRow[0] * 0x10) + 0x3C);
            widgetScreen = 5;
        } else {
            widgetScreen = 1;
        }
        do {
            g_ItemMenuScrollbar[0] = 0xA;
            g_ItemMenuScrollbarTotalRows = 0x140;
        } while (0);
        slotIdx = (s8*)(widgetScreen * 0x12);
        scrollPos = *((u16*)((((u8*)g_ItemMenuWidgets) + 2) + ((s32)slotIdx)));
        g_ItemMenuScrollbarTrackX = 0x160;
        g_ItemMenuScrollbarTrackY = 0x35;
        g_ItemMenuScrollbarTrackW = 0xA;
        g_ItemMenuScrollbarTrackH = 0xA5;
        g_ItemMenuScrollbarTopRow = scrollPos;
        numVisibleRows = 0xA;
        SysMenuDrawScrollbar(g_ItemMenuScrollbar, scrollPos);
        hasScrollAnimation = *((s16*)((((u8*)g_ItemMenuWidgets) + 8) + ((s32)slotIdx)));
        if (hasScrollAnimation != 0) {
            numVisibleRows = 0xB;
        }
        SysMenuUnkNoop(9);
        rowIdx = 0;
        if ((s16)numVisibleRows != 0) {
            do {
                yBaseOffset = 0x3A;
                itemSlot = (*((s16*)((((u8*)g_ItemMenuWidgets) + 2) + ((s32)slotIdx)))) + rowIdx;
                invItem = *(Savemap.inventory - (-itemSlot));
                if ((invItem & 0xFFFF) != 0xFFFF) {
                    selectedItemId = invItem & 0x1FF;
                    textColor = (-((func_801D0DCC(selectedItemId) & 4) == 0)) & 7;
                    SysMenuDrawString(
                        0xD6, (rowIdx * 0x10) + (((&g_ItemMenuWidgets[0].scrollAnimY)[(s32)slotIdx] * 4) + yBaseOffset),
                        SysKernGetString(4, selectedItemId, 8), textColor);
                }
                rowIdx += 1;
            } while (rowIdx < numVisibleRows);
        }
        rowIdx = 0;
        if (numVisibleRows != 0) {
            widgetScreen *= 0x12;
            do {
                u16 item;
                s32 itemWord;
                s32 count;

                charSlot = (*((s16*)((((u8*)g_ItemMenuWidgets) + 2) + widgetScreen))) + rowIdx;
                item = *(Savemap.inventory + charSlot);
                itemWord = item & 0xFFFF;
                if (itemWord != 0xFFFF) {
                    selectedItemId = item & 0x1FF;
                    count = (u32)itemWord >> 9;
                    itemUsableFlag = func_801D0DCC(selectedItemId) & 4;
                    textColor = (-(itemUsableFlag == 0)) & 7;
                    ((void (*)())ITEMMENU_DrawItemTypeIcon)(
                        0xC4, ((s8*)(rowIdx * 0x10)) + (((&g_ItemMenuWidgets[0].scrollAnimY)[widgetScreen] * 4) + 0x38),
                        selectedItemId, 0);
                    SysMenuDrawSingleFontLetter(
                        0x13F,
                        (s32)(((s8*)(rowIdx * 0x10)) +
                              (((&g_ItemMenuWidgets[0].scrollAnimY)[widgetScreen] * 4) + 0x3C)),
                        0xD5, textColor);
                    SysMenuDrawDigitsWithoutLeadingZeroes(
                        0x140,
                        (s32)(((s8*)(rowIdx * 0x10)) +
                              (((&g_ItemMenuWidgets[0].scrollAnimY)[widgetScreen] * 4) + 0x3B)),
                        count, 3, textColor);
                }
                rowIdx += 1;
            } while (rowIdx < numVisibleRows);
        }
    } else {
        numVisibleRows = 0xA;
        g_ItemMenuScrollbar[0] = 0xA;
        g_ItemMenuScrollbarTotalRows = 0x20;
        g_ItemMenuScrollbarTrackW = 0xA;
        g_ItemMenuScrollbarTrackX = 0x160;
        g_ItemMenuScrollbarTrackY = 0x35;
        g_ItemMenuScrollbarTrackH = 0xA5;
        g_ItemMenuScrollbarTopRow = (u16)g_KeyItemListOffset;
        keyRowIdx = 0;
        selectedItemId = 0x38;
        do {
        } while (0);
        ((void (*)())SysMenuDrawScrollbar)(g_ItemMenuScrollbar);
        SysMenuUnkNoop(9);
        slotIdx = 0;
        do {
            widgetScreen = keyRowIdx * 0x10;
            textColor = 0x20;
            itemSlot = (g_KeyItemListOffset + keyRowIdx) * 2;
        loop_66:
            selectedItemId = itemSlot + ((s32)slotIdx);

            keyItemId = g_KeyItemList[selectedItemId];
            if (keyItemId != 0xFF) {
                SysMenuDrawString(textColor, widgetScreen + (scrollAnimOffset = (g_KeyItemScrollAnimY * 4) + 0x3A),
                                  SysKernGetString(0xE, keyItemId, 8), 7);
            }
            slotIdx += 1;
            textColor += 0xA6;
            if (((s32)slotIdx) < 2) {
                goto loop_66;
            }
            keyRowIdx += 1;
            slotIdx = 0;
        } while (keyRowIdx < 0xC);
    }
    canRestoreMp = 0x35;
    drawRect[1] = canRestoreMp;
    drawRect[2] = 0x16C;
    drawRect[3] = 0xA5;
    drawRect[0] = 0;
    SysMenuSetDrawenv((void*)(((u8*)D_800706A4) + (g_MenuRenderBufferIndex * 0x5C)), drawRect);
    slotIdx = 0;
    windowDefPtr = (s8*)(&g_ItemMenuWindows);
    do {
        SysMenuDrawWindow(windowDefPtr);
        slotIdx += 1;
        windowDefPtr = windowDefPtr + 8;
    } while (((s32)slotIdx) < 3);
    if (SysMenuGetMenuListState() == 0) {
        SysMenuHandleButtons((MenuTable*)(((u8*)g_ItemMenuWidgets) + (g_ItemMenuCurrentScreen * 0x12)));
        switch (g_ItemMenuCurrentScreen) {
        case ITEMMENU_SCREEN_SELECTOR:
            if (g_Pad0KeysPressed & PAD_CIRCLE) {
                PlayItemMenuSfx(SFX_MENU_CURSOR_MOVE);
                menuWidget = (MenuTable*)(&g_ItemMenuWidgets[0].column);
                switch (*((s8*)menuWidget)) {
                case USE_ITEM_MENU_SELECTED:
                    g_ItemMenuCurrentScreen = ITEMMENU_SCREEN_USE;
                    return;

                case ARRANGE_MENU_SELECTED:
                    SysMenuSetCursorMovement((MenuTable*)(((s8*)menuWidget) + 0x3E), 0, 0, 1, 8, 0, 0,
                                             (s32)(*((s8*)menuWidget)), 8, 0, 0, 0, (s32)(*((s8*)menuWidget)), 0);
                    g_ItemMenuCurrentScreen = ITEMMENU_SCREEN_ARRANGE_SELECT;
                    return;

                case KEY_ITEMS_MENU_SELECTED:
                    SysMenuSetCursorMovement((MenuTable*)(((s8*)menuWidget) + 0x2C), 0, 0, 2, 0xA, 0, 0,
                                             (s32)(*((s8*)menuWidget)), 0x20, 0, 0, (s32)(*((s8*)menuWidget)), 0, 0);
                    g_ItemMenuCurrentScreen = ITEMMENU_SCREEN_KEY_ITEMS;
                    return;
                }

            } else if (g_Pad0KeysRepeat & PAD_CROSS) {
                PlayItemMenuSfx(SFX_MENU_BACK);
                SysMenuSetMenuListAnimation(5, 0);
                SysMenuLoadMenuFileById(0);
                return;
            }
            break;

        case ITEMMENU_SCREEN_USE:
            if (g_ItemListScrolling == 0) {
                if (g_Pad0KeysPressed & PAD_CIRCLE) {
                    selectedItemId = g_ItemUseListCursorPos[0] + g_ItemListOffet;
                    selectedItemEntry = Savemap.inventory[selectedItemId];
                    if (((selectedItemEntry & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) &&
                        (!(GetItemUseFlagsContext(selectedItemId = selectedItemEntry & ITEM_ID_MASK) & 4))) {
                        if (selectedItemId != ITEM_SAVE_CRYSTAL) {
                            if (selectedItemId == ITEM_EARTH_HARP) {
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                Savemap.party[6].level = 1;
                                Savemap.party[6].limit_level = 1;
                                Savemap.party[6].char_id = CHAR_CAITSITH;
                                Savemap.party[6].limit_charge = 0xFF;
                                Savemap.party[6].exp = 0xFFFFFF;
                                Savemap.party[7].level = 1;
                                Savemap.party[7].char_id = CHAR_VINCENT;
                                Savemap.party[7].limit_level = 1;
                                Savemap.party[7].limit_charge = 0xFF;
                                Savemap.party[7].exp = 0xFFFFFF;
                                return;
                            }
                            goto block_e48;
                        }
                        PlayItemMenuSfx(SFX_MENU_APPLY);
                        Savemap.memory_bank_4[0x60] |= 1;
                        SysMenuSetMenuListAnimation(5, 0);
                        SysMenuLoadMenuFileById(0);
                        SysMenuClose();
                        return;
                    block_e48:
                        PlayItemMenuSfx(SFX_MENU_CURSOR_MOVE);

                        g_ItemMenuTargetSelectDelay = 0;
                        g_ItemMenuCurrentScreen = 2;
                        return;
                    }
                    PlayItemMenuSfx(SFX_MENU_BAD);
                    return;
                }
                cancelButtonPressed = g_Pad0KeysPressed & PAD_CROSS;
                goto block_217;
            }
            break;

        case ITEMMENU_SCREEN_ITEM_SELECTED:
            if (g_ItemMenuTargetSelectDelay == 0) {
                if (g_Pad0KeysPressed & PAD_CIRCLE) {
                    cursorPosX = Savemap.partyID[g_PartySlotSelected[0]];
                    selectedItemId = Savemap.inventory[g_ItemUseListCursorPos[0] + g_ItemListOffet] & ITEM_ID_MASK;
                    isRegularItem = selectedItemId < ITEM_TYPE_REGUALR_BASE;
                    characterId = cursorPosX;
                    isItemUsable = isRegularItem;
                    if (characterId == CHAR_EMPTY) {
                        if ((selectedItemId != 6) && (selectedItemId != ITEM_TENT)) {
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            return;
                        }
                    }
                    {
                        switch (selectedItemId) {
                        // Can be used on "Healthy"	Party memebers (Inflicts Fury / Heal Sadness)
                        case ITEM_HYPER:
                            cursorPosX = characterId * 0x84;
                            statusFlags = (&Savemap.party[0].status_flags)[cursorPosX];
                            if (!(statusFlags & STATUS_FURY)) {
                                if (!(statusFlags & STATUS_SADNESS)) {
                                    newStatusFlags = statusFlags | STATUS_FURY;
                                } else {
                                    newStatusFlags = statusFlags & CLEAR_SADNESS;
                                }
                                (&Savemap.party[0].status_flags)[cursorPosX] = newStatusFlags;
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        // Inflicts Sadness / Heal Fury
                        case ITEM_TRANQUILIZER:
                            charStructOffset = characterId;
                            charStructOffset *= 0x84;
                            statusFlagsTranquilizer = (&Savemap.party[0].status_flags)[charStructOffset];
                            hasFuryCondition = STATUS_FURY;
                            hasFuryCondition = (statusFlagsTranquilizer & hasFuryCondition) != 0;
                            if (hasFuryCondition || ((statusFlagsTranquilizer & STATUS_SADNESS) == 0)) {
                                newStatusFlagsTranquilizer =
                                    hasFuryCondition ? (statusFlagsTranquilizer & CLEAR_FURY)
                                                     : (statusFlagsTranquilizer | STATUS_SADNESS);
                                (&Savemap.party[0].status_flags)[charStructOffset] = newStatusFlagsTranquilizer;
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        // Apply final limit breaks
                        case ITEM_OMNISLASH:

                        case ITEM_CATASTROPHE:

                        case ITEM_FINAL_HEAVEN:

                        case ITEM_GREAT_GOSPEL:

                        case ITEM_COSMO_MEMORY:

                        case ITEM_ALL_CREATION:

                        case ITEM_CHAOS:

                        case ITEM_HIGHWIND:
                            if (characterId == g_LimitManualCharIds[selectedItemId]) {
                                if (HasLearnedAllLimits(characterId) != 0) {
                                    PlayItemMenuSfx(SFX_MENU_SET);
                                    Savemap.party[g_LimitManualCharIds[selectedItemId]].limit_learn |= 0x200;
                                    SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                    ;
                                    if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) == ITEM_EMPTY_SLOT) {
                                        g_ItemMenuCurrentScreen = 1;
                                    }
                                    SetNotificationText(((selectedItemId - 0x57) * 0x66) + g_LimitManualMsgs);
                                    SysMenuRequestAddWindow((s32*)g_ItemMenuNotificationText, 7);
                                    return;
                                }
                                msgPtr =
                                    (s32*)(((selectedItemId - 0x57) * 0x66) + ((s8*)(&g_LimitManualAlreadyLearnedMsg)));
                                SetNotificationText(msgPtr);
                                SysMenuRequestAddWindow((s32*)g_ItemMenuNotificationText, 7);
                                PlayItemMenuSfx(SFX_MENU_BAD);
                                return;
                            }
                            if (characterId == 6) {
                                msgPtr = &g_LimitManualNoUserMsg;
                            } else {
                                yBaseOffset = 6;
                                if (((s32)characterId) >= yBaseOffset) {
                                    charMsgIdx = (characterId - 1) * 3;
                                } else {
                                    charMsgIdx = characterId * 3;
                                }
                                msgPtr = (s32*)(((charMsgIdx + 2) * 0x22) + ((s8*)g_LimitManualMsgs));
                            }
                            SetNotificationText(msgPtr);
                            SysMenuRequestAddWindow((s32*)g_ItemMenuNotificationText, 7);
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            return;

                        case ITEM_POWER_SOURCE:

                        case ITEM_GUARD_SOURCE:

                        case ITEM_MAGIC_SOURCE:

                        case ITEM_MIND_SOURCE:

                        case ITEM_SPEED_SOURCE:

                        case ITEM_LUCK_SOURCE:
                            switch (selectedItemId) {
                            case ITEM_POWER_SOURCE:
                                strengthBonus = Savemap.party[characterId].strength_bonus;
                                if (strengthBonus < STAT_MAX) {
                                    Savemap.party[characterId].strength_bonus = strengthBonus - (-1);
                                default:
                                    goto src_tail;

                                } else {
                                    PlayItemMenuSfx(SFX_MENU_BAD);
                                    return;
                                }
                                break;

                            case ITEM_GUARD_SOURCE:
                                vitalityBonus = Savemap.party[characterId].vitality_bonus;
                                if (vitalityBonus < STAT_MAX) {
                                    Savemap.party[characterId].vitality_bonus = vitalityBonus + 1;
                                    goto src_tail;
                                    PlayItemMenuSfx(SFX_MENU_BAD);
                                }
                                PlayItemMenuSfx(SFX_MENU_BAD);
                                return;

                            case ITEM_MAGIC_SOURCE:
                                magicBonus = Savemap.party[characterId].magic_bonus;
                                if (magicBonus < STAT_MAX) {
                                    Savemap.party[characterId].magic_bonus = magicBonus + 1;
                                    goto src_tail;
                                }
                                PlayItemMenuSfx(SFX_MENU_BAD);
                                return;

                            case ITEM_MIND_SOURCE:
                                spiritBonus = Savemap.party[characterId].spirit_bonus;
                                if (spiritBonus < STAT_MAX) {
                                    Savemap.party[characterId].spirit_bonus = spiritBonus + 1;
                                    goto src_tail;
                                }
                                PlayItemMenuSfx(SFX_MENU_BAD);
                                return;

                            case ITEM_SPEED_SOURCE:
                                dexterityBonus = Savemap.party[characterId].dexterity_bonus;
                                if (dexterityBonus < STAT_MAX) {
                                    Savemap.party[characterId].dexterity_bonus = dexterityBonus + 1;
                                    goto src_tail;
                                }
                                PlayItemMenuSfx(SFX_MENU_BAD);
                                return;

                            case ITEM_LUCK_SOURCE:
                                luckBonus = Savemap.party[characterId].luck_bonus;
                                if (luckBonus < STAT_MAX) {
                                    Savemap.party[characterId].luck_bonus = luckBonus + 1;
                                src_tail:
                                    PlayItemMenuSfx(SFX_MENU_APPLY);

                                    SysInitPlayerStatFromEquip(g_PartySlotSelected[0]);
                                    SysInitPlayerStatFromMateria(*((u8*)g_PartySlotSelected));
                                    SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                    if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                        return;
                                    }
                                    g_ItemMenuCurrentScreen = 1;
                                    return;
                                }
                                PlayItemMenuSfx(SFX_MENU_BAD);
                                return;
                            }

                            break;

                        case ITEM_POTION:
                            if ((IsCharacterHpFull(g_PartySlotSelected[0]) == 0) &&
                                (g_ActiveCharacters[g_PartySlotSelected[0]].hp != 0)) {
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SystemMenuAddHpByPartyId(g_PartySlotSelected[0], RESTORE_POTION_HP);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        case ITEM_HI_POTION:
                            if ((IsCharacterHpFull(g_PartySlotSelected[0]) == 0) &&
                                (g_ActiveCharacters[g_PartySlotSelected[0]].hp != 0)) {
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SystemMenuAddHpByPartyId(g_PartySlotSelected[0], RESTORE_HI_POTION_HP);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        case ITEM_ETHER:
                            canRestoreMp = IsCharacterMpFull(g_PartySlotSelected[0]) == 0;
                            rowIdx = 0;
                            if (canRestoreMp && (g_ActiveCharacters[g_PartySlotSelected[rowIdx]].hp != 0)) {
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SystemMenuAddMpByPartyId(g_PartySlotSelected[0], RESTORE_ETHER_MP);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if (ITEM_EMPTY_SLOT != (SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT)) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        case ITEM_TURBO_ETHER:
                            if ((IsCharacterMpFull(g_PartySlotSelected[0]) == 0) &&
                                (g_ActiveCharacters[g_PartySlotSelected[0]].hp != 0)) {
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SystemMenuAddMpByPartyId(g_PartySlotSelected[0], MAX_MP);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        case ITEM_PHOENIXDOWN:
                            if (g_ActiveCharacters[g_PartySlotSelected[0]].hp == 0) {
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SystemMenuAddHpByPartyId(
                                    g_PartySlotSelected[0], g_ActiveCharacters[g_PartySlotSelected[0]].baseHp / 4);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        case ITEM_TENT:
                            slotIdx = 0;
                            canHealAnyParty = 0;
                            if (selectedItemId) {
                            }
                            do {
                                if ((Savemap.partyID[(s32)slotIdx] != CHAR_EMPTY) &&
                                    ((IsCharacterHpFull((s32)slotIdx) == 0) ||
                                     (IsCharacterMpFull((s32)slotIdx) == 0))) {
                                    canHealAnyParty = 1;
                                }
                                slotIdx += 1;
                            } while (((s32)slotIdx) < 3);
                            slotIdx = 0;
                            if (canHealAnyParty != 0) {
                                emptySlot = CHAR_EMPTY;
                                canHealAnyParty = 0;
                                do {
                                    if (((*((s16*)(((u8*)&g_ActiveCharacters[0].hp) + canHealAnyParty))) != 0) &&
                                        (Savemap.partyID[(s32)slotIdx] != emptySlot)) {
                                        SystemMenuAddHpByPartyId((s32)slotIdx, MAX_HP);
                                        SystemMenuAddMpByPartyId((s32)slotIdx, MAX_MP);
                                    }
                                    slotIdx += 1;
                                    canHealAnyParty += 0x440;
                                } while (((s32)slotIdx) < 3);
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                itemSearchRes = SysMenuSearchItem(selectedItemId);
                                if ((itemSearchRes & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        case ITEM_XPOTION:
                            if ((IsCharacterHpFull(g_PartySlotSelected[0]) == 0) &&
                                (g_ActiveCharacters[g_PartySlotSelected[0]].hp != 0)) {
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SystemMenuAddHpByPartyId(g_PartySlotSelected[0], MAX_HP);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        case ITEM_ELIXIR:
                            if (((IsCharacterHpFull(g_PartySlotSelected[0]) == 0) ||
                                 (IsCharacterMpFull(g_PartySlotSelected[0]) == 0)) &&
                                (g_ActiveCharacters[g_PartySlotSelected[0]].hp != 0)) {
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SystemMenuAddHpByPartyId(g_PartySlotSelected[0], MAX_HP);
                                SystemMenuAddMpByPartyId(g_PartySlotSelected[0], MAX_MP);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                statusFlagsTranquilizer = (&Savemap.party[0].status_flags)[charStructOffset];
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                if (!g_ItemMenuWidgets[0].column) {
                                }
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;

                        case ITEM_MEGALIXIR:
                            slotIdx = 0;
                            canHealPartyMember = 0;
                            do {
                                if ((Savemap.partyID[(s32)slotIdx] != CHAR_EMPTY) &&
                                    ((IsCharacterHpFull((s32)slotIdx) == 0) ||
                                     (IsCharacterMpFull((s32)slotIdx) == 0))) {
                                    canHealPartyMember = 1;
                                }
                                slotIdx += 1;
                            } while (((s32)slotIdx) < 3);
                            slotIdx = 0;
                            if (canHealPartyMember != 0) {
                                emptyPartySlot = CHAR_EMPTY;
                                charHpOffset = 0;
                                do {
                                    if (((*((s16*)(((u8*)&g_ActiveCharacters[0].hp) + charHpOffset))) != 0) &&
                                        (Savemap.partyID[(s32)slotIdx] != emptyPartySlot)) {
                                        SystemMenuAddHpByPartyId((s32)slotIdx, MAX_HP);
                                        SystemMenuAddMpByPartyId((s32)slotIdx, MAX_MP);
                                    }
                                    slotIdx += 1;
                                    charHpOffset += 0x440;
                                } while (((s32)slotIdx) < 3);
                                PlayItemMenuSfx(SFX_MENU_APPLY);
                                SysMenuRemoveItem(selectedItemId | ITEM_CONSUMED);
                                if ((SysMenuSearchItem(selectedItemId) & ITEM_EMPTY_SLOT) != ITEM_EMPTY_SLOT) {
                                    return;
                                }
                                g_ItemMenuCurrentScreen = 1;
                                return;
                            }
                            PlayItemMenuSfx(SFX_MENU_BAD);
                            break;
                        }
                    }
                } else if (g_Pad0KeysPressed & PAD_CROSS) {
                    PlayItemMenuSfx(SFX_MENU_BACK);
                    g_ItemMenuCurrentScreen = 1;
                    return;
                }
            }
            break;

        case ITEMMENU_SCREEN_KEY_ITEMS:
            cancelButtonPressed = g_Pad0KeysPressed & PAD_CROSS;
            goto block_217;

        case ITEMMENU_SCREEN_ARRANGE_SELECT:
            if (g_Pad0KeysPressed & PAD_CIRCLE) {
                PlayItemMenuSfx(SFX_MENU_CURSOR_MOVE);
                isCustomizeMode = g_ArrangeMenuCursorRow[0] == 0;
                if (isCustomizeMode) {
                    arrangeWidget = (MenuTable*)((&g_ArrangeMenuCursorRow[0]) + 7);
                    SysMenuSetCursorMovement(arrangeWidget, 0, 0, 1, 0xA, 0, 0, 1, 0x140, 0, 0, 0, 0, 0);
                    g_ItemSwapState = 0;
                    g_ItemSwapSourceColumn = 0;
                    g_ItemSwapSourceSlot = 0;
                    g_ItemMenuCurrentScreen = 5;
                    return;
                }
                ArrangeItems(g_ArrangeMenuCursorRow[0]);
                goto block_219;
            }
            cancelButtonPressed = g_Pad0KeysPressed & PAD_CROSS;
            goto block_217;

        case ITEMMENU_SCREEN_ARRANGE_CUSTOM:
            if (g_Pad0KeysPressed & PAD_CIRCLE) {
                switch (g_ItemSwapState) {
                case 0:
                    PlayItemMenuSfx(SFX_MENU_CURSOR_MOVE);
                    g_ItemSwapSourceColumn = (s32)g_ItemSwapCursorColumn;
                    g_ItemSwapSourceSlot = g_ItemSwapCursorRow[0] + g_ItemSwapListOffset;
                    g_ItemSwapState += 1;
                    return;

                case 1:
                    PlayItemMenuSfx(SFX_MENU_CURSOR_MOVE);
                    sourceSlotPtr = &Savemap.inventory[g_ItemSwapSourceSlot];
                    cursorRowPtr = g_ItemSwapCursorRow;
                    swapItemPtr = sourceSlotPtr;
                    swappedItem = *swapItemPtr;
                    *sourceSlotPtr = Savemap.inventory[cursorRowPtr[0] + g_ItemSwapListOffset];
                    g_ItemSwapState = 0;
                    Savemap.inventory[cursorRowPtr[0] + g_ItemSwapListOffset] = swappedItem;
                    return;
                }

            } else {
                cancelButtonPressed = g_Pad0KeysPressed & PAD_CROSS;
            block_217:
                if (cancelButtonPressed != 0) {
                    PlayItemMenuSfx(SFX_MENU_BACK);
                block_219:
                    g_ItemMenuCurrentScreen = 0;
                }
            }
            break;
        }
    }
}

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
