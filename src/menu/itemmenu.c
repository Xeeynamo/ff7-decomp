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

#define ITEM_TYPE_WEAPON_BASE 0x80
#define ITEM_TYPE_ARMOR_BASE 0x100
#define ITEM_TYPE_ACCESSORY_BASE 0x120
#define ITEM_ICON_BASE_U 0x60
#define ITEM_ICON_BASE_V 0x70
#define ITEM_ICON_SIZE 0x10
#define ITEM_ICON_CLUT 1


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

    for (keyItemId = 0, count = 0, keyItemPtr = g_KeyItemList; keyItemId < 0x40; keyItemId++) {
        if ((Savemap.memory_bank_1[0x40 + keyItemId / 8] >> (keyItemId & 7)) & 1) {
            *keyItemPtr = keyItemId;
            keyItemPtr += 1;
            count += 1;
        }
    }
    while (count < 0x40) {
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
    return Sign((itemA & 0x1FF) - (itemB & 0x1FF));
}

// Sort comparator for the "Most" arrange option: orders inventory slots by
// quantity (high 7 bits) descending, sending empty slots (0xFFFF) first.
static s32 CompareItemsByMost(s16 slotA, s16 slotB, s32* inventoryBase) {
    s32 qtyA;
    s32 qtyB;
    u16 itemB;
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    if (itemA == 0xFFFF) {
        qtyA = 0;
    } else {
        qtyA = itemA >> 9;
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    qtyB = itemB >> 9;
    if (itemB == 0xFFFF) {
        qtyB = 0;
    }
    return Sign(qtyB - qtyA);
}

// Sort comparator for the "Least" arrange option: orders inventory slots by
// quantity (high 7 bits) ascending, sending empty slots (0xFFFF) to the end.
static s32 CompareItemsByLeast(s16 slotA, s16 slotB, s32* inventoryBase) {
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    s32 qtyA = (itemA == 0xFFFF) ? 0x4E20 : (itemA >> 9);
    u16 itemB = *(u16*)(*inventoryBase + slotB * 2);
    s32 qtyB = (itemB == 0xFFFF) ? 0x4E20 : (itemB >> 9);
    return Sign(qtyA - qtyB);
}

// Sort comparator for the "Name" arrange option: orders inventory slots by a
// per-item sort-order table, sending empty slots (0xFFFF) to the end.
static s32 CompareItemsByName(s16 slotA, s16 slotB, s32* inventoryBase) {
    u16 itemA = *(u16*)(*inventoryBase + slotA * 2);
    s16 sortKeyA;
    u16 itemB;
    s16 sortKeyB;
    if (itemA == 0xFFFF) {
        sortKeyA = 0x4E20;
    } else {
        sortKeyA = g_ItemNameSortKeys[itemA & 0x1FF];
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    if (itemB == 0xFFFF) {
        sortKeyB = 0x4E20;
    } else {
        sortKeyB = g_ItemNameSortKeys[itemB & 0x1FF];
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
    if (itemA == 0xFFFF) {
        priorityA = 0;
    } else {
        priorityA = (SysMenuGetInventoryRestrictionMask(itemA & 0x1FF) & 4) ? 1 : 2;
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    if (itemB == 0xFFFF) {
        priorityB = 0;
    } else {
        priorityB = (SysMenuGetInventoryRestrictionMask(itemB & 0x1FF) & 4) ? 1 : 2;
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
    if (itemA == 0xFFFF) {
        priorityA = 0;
    } else {
        priorityA = (SysMenuGetInventoryRestrictionMask(itemA & 0x1FF) & 2) ? 1 : 2;
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    if (itemB == 0xFFFF) {
        priorityB = 0;
    } else {
        priorityB = (SysMenuGetInventoryRestrictionMask(itemB & 0x1FF) & 2) ? 1 : 2;
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
    if (itemA == 0xFFFF) {
        priorityA = 0;
    } else {
        priorityA = (SysMenuGetInventoryRestrictionMask(itemA & 0x1FF) & 8) ? 1 : 2;
    }
    itemB = *(u16*)(*inventoryBase + slotB * 2);
    if (itemB == 0xFFFF) {
        priorityB = 0;
    } else {
        priorityB = (SysMenuGetInventoryRestrictionMask(itemB & 0x1FF) & 8) ? 1 : 2;
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
    case 0:
        break;
    case 1:
        Quicksort((s32)Savemap.inventory, 0x140, (SortCmp)CompareItemsByField, (SortSwap)SwapItemSlots);
        break;
    case 2:
        Quicksort((s32)Savemap.inventory, 0x140, (SortCmp)CompareItemsByBattle, (SortSwap)SwapItemSlots);
        break;
    case 3:
        Quicksort((s32)Savemap.inventory, 0x140, (SortCmp)CompareItemsByThrow, (SortSwap)SwapItemSlots);
        break;
    case 4:
        Quicksort((s32)Savemap.inventory, 0x140, (SortCmp)CompareItemsByType, (SortSwap)SwapItemSlots);
        break;
    case 5:
        Quicksort((s32)Savemap.inventory, 0x140, (SortCmp)CompareItemsByName, (SortSwap)SwapItemSlots);
        break;
    case 6:
        Quicksort((s32)Savemap.inventory, 0x140, (SortCmp)CompareItemsByMost, (SortSwap)SwapItemSlots);
        break;
    case 7:
        Quicksort((s32)Savemap.inventory, 0x140, (SortCmp)CompareItemsByLeast, (SortSwap)SwapItemSlots);
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
    SysMenuSetCursorMovement(&g_ItemMenuWidgets[1], 0, 0, 1, 0xA, 0, 0, 1, 0x140, 0, 0, 0, 0, 0);
    SysMenuSetCursorMovement(&g_ItemMenuWidgets[2], 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 0, 1, 0);
    BuildKeyItemList();
}

// True if the two adjacent record fields for entry charIdx are equal.
static s32 IsCharacterHpFull(s32 charIdx) { return g_ActiveCharacters[charIdx].baseHp == g_ActiveCharacters[charIdx].hp; }

// True if the two adjacent record fields for entry charIdx are equal.
static s32 IsCharacterMpFull(s32 charIdx) { return g_ActiveCharacters[charIdx].baseMp == g_ActiveCharacters[charIdx].mp; }

// Builds a 10-bit mask of which of character charIdx's slots are occupied (slot
// value != 0x7F), clears bit 9, and returns whether it matches the stored
// value.
static s32 HasLearnedAllLimits(s32 charIdx) {
    s32 mask;
    s32 limitIdx;
    for (limitIdx = 0, mask = 0; limitIdx < 10; limitIdx++) {
        if (SysGetLimitCmdId(charIdx, limitIdx) != 0x7F) {
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
    if (itemId != 0x46) {
        if (itemId == 0x62) {
            if (!(Savemap.memory_bank_4[0x60] & 2)) {
                flags |= 4;
            }
        }
    } else {
        if (g_MenuLocationFlags & 0x200) {
            flags |= 4;
        }
    }
    return flags;
}

// Copies 0x50 bytes from text into the g_ItemMenuNotificationText buffer.
static void SetNotificationText(u8* text) {
    s32 byteIdx;
    for (byteIdx = 0; byteIdx < 0x50; byteIdx++) {
        g_ItemMenuNotificationText[byteIdx] = *text;
        text++;
    }
}

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", ITEMMENU_Main);

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
    } while (slotIdx < 0x30);
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
    } while (slotIdx < 0x30);
    return lowestPriority;
}

static void OfferMateriaToSteal(s32* materiaPtr) {
    s32 slotIdx;
    s32 lowestPriority;

    if (*materiaPtr == -1) {
        return;
    }
    slotIdx = 0;
    do {
        if (g_MateriaStealLoot[slotIdx] == -1) {
            g_MateriaStealLoot[slotIdx] = *materiaPtr;
            return;
        }
        slotIdx += 1;
    } while (slotIdx < 0x30);

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

    for (charIdx = 8; charIdx != -1; charIdx--) {
        if ((Savemap.phs_visibility_mask >> charIdx) & 1) {
            {
                s32 slotIdx;
                for (slotIdx = 0; slotIdx < NUM_MATERIA_ROW; slotIdx++) {
                    if (Savemap.party[charIdx].materia_weapon[slotIdx] == -1 &&
                        g_WeaponTable[Savemap.party[charIdx].weapon].materiaSlot[slotIdx]) {
                        Savemap.party[charIdx].materia_weapon[slotIdx] = materia;
                        return 0;
                    }
                }
            }
            {
                s32 slotIdx;
                for (slotIdx = 0; slotIdx < NUM_MATERIA_ROW; slotIdx++) {
                    if (Savemap.party[charIdx].materia_armor[slotIdx] == -1 &&
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

    for (charIdx = 0; charIdx < 9; charIdx++) {
        if ((Savemap.phs_visibility_mask >> charIdx) & 1) {
            for (slotIdx = 0; slotIdx < 8; slotIdx++) {
                if (Savemap.party[charIdx].materia_weapon[slotIdx] == materia) {
                    Savemap.party[charIdx].materia_weapon[slotIdx] = -1;
                    return;
                }
            }
            for (slotIdx = 0; slotIdx < 8; slotIdx++) {
                if (Savemap.party[charIdx].materia_armor[slotIdx] == materia) {
                    Savemap.party[charIdx].materia_armor[slotIdx] = -1;
                    return;
                }
            }
        }
    }
    for (slotIdx = 0; slotIdx < MAX_MATERIA_COUNT; slotIdx++) {
        if (Savemap.materia[slotIdx] == materia) {
            Savemap.materia[slotIdx] = -1;
            return;
        }
    }
}

static void FinalizeMateriaSteal(void) {
    s32 slotIdx;
    s32 materia;

    for (slotIdx = 0; slotIdx < 0x30; slotIdx++) {
        materia = g_MateriaStealLoot[slotIdx];
        if (materia != -1) {
            RemoveMateriaFromPlayer(materia);
        }
    }
    for (slotIdx = 0; slotIdx < 0x30; slotIdx++) {
        Savemap.yuffie_stolen_materia[slotIdx] = g_MateriaStealLoot[slotIdx];
    }
}

void ITEMMENU_StealAllMateria(void) {
    s32 lootIdx;
    s32 charIdx;
    s32 slotIdx;

    for (lootIdx = 0; lootIdx < 0x30; lootIdx++) {
        g_MateriaStealLoot[lootIdx] = -1;
    }
    for (charIdx = 0; charIdx < 9; charIdx++) {
        if ((Savemap.phs_visibility_mask >> charIdx) & 1) {
            for (slotIdx = 0; slotIdx < 8; slotIdx++) {
                do {
                    OfferMateriaToSteal(&Savemap.party[charIdx].materia_weapon[slotIdx]);
                } while (0);
            }
            for (slotIdx = 0; slotIdx < 8; slotIdx++) {
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

    for (slotIdx = 0; slotIdx < 0x30; slotIdx++) {
        if (Savemap.yuffie_stolen_materia[slotIdx] != -1) {
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
        s32 emptySlot = -1;
        s32* materiaSlotPtr = Savemap.party[charIdx].materia_weapon;
        do {
            if (*materiaSlotPtr != emptySlot) {
                SysMenuAddMateria(*materiaSlotPtr);
                *materiaSlotPtr = emptySlot;
            }
            slotIdx += 1;
            materiaSlotPtr += 1;
        } while (slotIdx < 8);
    }
    {
        s32 slotIdx = 0;
        s32 emptySlot = -1;
        s32* materiaSlotPtr = Savemap.party[charIdx].materia_armor;
        do {
            if (*materiaSlotPtr != emptySlot) {
                SysMenuAddMateria(*materiaSlotPtr);
                *materiaSlotPtr = emptySlot;
            }
            slotIdx += 1;
            materiaSlotPtr += 1;
        } while (slotIdx < 8);
    }
    accessory = Savemap.party[charIdx].accessory;
    if (accessory != 0xFF) {
        SysMenuAddItem((accessory + 0x120) | 0x200);
        Savemap.party[charIdx].accessory = 0xFF;
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
        } while (i < 3);
    }
    {
        s32 emptySlot;
        s32* materiaInvPtr;
        u8* destPtr;
        i = 0;
        emptySlot = -1;
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
        } while (i < 3);
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
        emptySlot = -1;
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
        } while (i < 8);
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
        } while (i < 3);
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
        } while (i < 3);
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
        } while (i < 8);
    }
}

// Uploads the coin-pattern texture at g_CoinTextureTim (64x32, 4bpp, seamlessly
// tileable) into VRAM: pixel data to (0x3F0, 0x120), CLUT to (0x110, 0x1E0).
// Runs once at boot/menu init (main -> func_80026258 -> HandleLoadCoinTexture); the
// texture stays resident so the battle UI can scroll it as the animated
// backdrop behind the coin-throw amount prompt.
void ITEMMENU_LoadCoinTexture(void) { MENU_LoadTim((u_long*)g_CoinTextureTim, 0x3F0, 0x120, 0x110, 0x1E0); }
