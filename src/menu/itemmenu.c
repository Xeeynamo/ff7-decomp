//! PSYQ=3.3 CC1=2.7.2
#include <game.h>

extern u8 g_MateriaPriority[];
extern s32 g_MateriaStealLoot[];
extern u8 D_800738BC[][44];
extern u8 D_80071E4D[][36];
extern u16
    D_8009CBE0[]; // item inventory (320 slots; each u16 = (count << 9) | id)
extern u16 D_801D35B4[]; // per-item-id sort order for the "Name" arrange option

s32 func_8001FAF8(
    s32); // returns an item's usage flags (0x2 battle, 0x4 field, 0x8 throw)
static s32 Quicksort(
    s32, s32, s32 (*)(s32, s32, s32*), void (*)(s32, s32, s32*));

s32 func_80015AFC(s32, s32);
void func_80025D14(u_long*, s32, s32, s32, s32);
void func_80028CA0(s16, s16, s32, s32, s32, s32, s32, s32);
extern u16 D_80062F50;
extern u8 D_8009D5E8;
extern u16 D_8009C75A[]; // character record fields, stride 0x84
extern s16 D_8009D85C[]; // record fields, stride 0x440
extern s16 D_8009D85E[];
extern s16 D_8009D860[];
extern s16 D_8009D862[];
extern u8 D_801D3890[];
extern u8 D_801D3E60[];

// Likely plays a menu sound effect: loads a sound command (0x30) and the sound
// id (arg0) into the sound-request globals, then dispatches via func_8002DA7C.
void func_801D01E8(u16 arg0) {
    D_8009A000[0] = 0x30;
    D_8009A004[0] = arg0;
    D_8009A008[0] = arg0;
    func_8002DA7C();
}

// Draws the type icon for an item at (arg0, arg1): maps the item id (arg2) to
// one of several icon cells, then blits a 16x16 sprite via func_80028CA0.
void func_801D0228(s16 arg0, s16 arg1, s32 arg2) {
    s32 icon;
    if (arg2 < 0x80) {
        icon = 0;
    } else if (arg2 < 0x100) {
        arg2 -= 0x80;
        if (arg2 < 0x10) {
            icon = 1;
        } else if (arg2 < 0x20) {
            icon = 3;
        } else if (arg2 < 0x30) {
            icon = 2;
        } else if (arg2 < 0x3E) {
            icon = 5;
        } else if (arg2 < 0x49) {
            icon = 4;
        } else if (arg2 < 0x57) {
            icon = 9;
        } else if (arg2 < 0x65) {
            icon = 6;
        } else if (arg2 < 0x72) {
            icon = 7;
        } else {
            icon = 8;
        }
    } else if (arg2 < 0x120) {
        icon = 0xA;
    } else {
        icon = 0xB;
    }
    {
        s32 ix = ((icon & 1) << 4) | 0x60;
        s32 iy = (((u32)icon >> 1) << 4) + 0x70;
        func_80028CA0(arg0, arg1, ix, iy, 0x10, 0x10, 1, 0);
    }
}

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D031C);

// Swap the two 32-bit values pointed to by arg0 and arg1.
static void SwapS32(s32* arg0, s32* arg1) {
    s32 a = *arg1;
    s32 b = *arg0;
    *arg0 = a;
    *arg1 = b;
}

// Iterative Hoare quicksort over item-slot indices [0, count), driving the
// cmp/swap callbacks. Explicit 64-deep bounds stack (lo half / hi half of one
// 128-word array), recursing into the smaller partition first (SwapS32 swaps
// the bounds pairs). Returns 1 on completion, 0 on bounds-stack overflow.
// NOTE: the do{}while(0) wrapper, the va1 register copy of j, the duplicated
// cont computation and the tmp* temporaries are all required for the
// byte-perfect match (they reproduce the original register allocation).
static s32 Quicksort(s32 base, s32 count, s32 (*cmp)(s32, s32, s32*),
                     void (*swap)(s32, s32, s32*)) {
    s32 stack[128];
    s32 tmp4;
    s32 tmp5;
    s32 j;
    s32 lo;
    s32 i;
    int tmp;
    s32 tmp3;
    s32* p;
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
        p = stack;
        stack[0] = 0;
        stack[64] = count - 1;
    loop_4:
        lo = p[0];
        tmp = (i = lo + 1);
        j = p[64];
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
                if ((((u32)i) < (va1 = (u32)count)) &&
                    (((u32)(j - lo)) < ((u32)(count - i)))) {
                    SwapS32(&j, &count);
                    SwapS32(&lo, &i);
                }
                tmp5 = j;
                if (((u32)lo) < ((u32)tmp5)) {
                    p[0] = lo;
                    p[64] = tmp5;
                    p += 1;
                    depth += 1;
                }
            }
        }
        cont = ((u32)depth) < 0x40U;
        tmp4 = count;
        if (((u32)i) < tmp4) {
            p[0] = i;
            p[64] = tmp4;
            p += 1;
            depth += 1;
        }
        cont = ((u32)depth) < 0x40U;
        depth -= 1;
    } while (0);
    if (cont != 0) {
        p -= 1;
        if (depth == (-1)) {
            return 1;
        }
        goto loop_4;
    }
    goto ret0;
}

// Swap the two 16-bit values pointed to by arg0 and arg1.
static void SwapU16(u16* arg0, u16* arg1) {
    u16 a = *arg1;
    u16 b = *arg0;
    *arg0 = a;
    *arg1 = b;
}

// Returns the sign of arg0: -1, 0, or 1.
static s32 Sign(s32 arg0) {
    if (arg0 != 0) {
        if (arg0 < 0) {
            return -1;
        }
        return 1;
    }
    return 0;
}

// Sort comparator for the "Type" arrange option: orders inventory slots arg0
// and arg1 by item id (low 9 bits; the item id space is grouped by type).
static s32 CompareItemsByType(s16 arg0, s16 arg1, s32* arg2) {
    u16 a = *(u16*)(*arg2 + arg0 * 2);
    u16 b = *(u16*)(*arg2 + arg1 * 2);
    return Sign((a & 0x1FF) - (b & 0x1FF));
}

// Sort comparator for the "Most" arrange option: orders inventory slots by
// quantity (high 7 bits) descending, sending empty slots (0xFFFF) first.
static s32 CompareItemsByMost(s16 arg0, s16 arg1, s32* arg2) {
    s32 ka;
    s32 kb;
    u16 b;
    u16 a = *(u16*)(*arg2 + arg0 * 2);
    if (a == 0xFFFF) {
        ka = 0;
    } else {
        ka = a >> 9;
    }
    b = *(u16*)(*arg2 + arg1 * 2);
    kb = b >> 9;
    if (b == 0xFFFF) {
        kb = 0;
    }
    return Sign(kb - ka);
}

// Sort comparator for the "Least" arrange option: orders inventory slots by
// quantity (high 7 bits) ascending, sending empty slots (0xFFFF) to the end.
static s32 CompareItemsByLeast(s16 arg0, s16 arg1, s32* arg2) {
    u16 a = *(u16*)(*arg2 + arg0 * 2);
    s32 ka = (a == 0xFFFF) ? 0x4E20 : (a >> 9);
    u16 b = *(u16*)(*arg2 + arg1 * 2);
    s32 kb = (b == 0xFFFF) ? 0x4E20 : (b >> 9);
    return Sign(ka - kb);
}

// Sort comparator for the "Name" arrange option: orders inventory slots by a
// per-item sort-order table, sending empty slots (0xFFFF) to the end.
static s32 CompareItemsByName(s16 arg0, s16 arg1, s32* arg2) {
    u16 a = *(u16*)(*arg2 + arg0 * 2);
    s16 ka;
    u16 b;
    s16 kb;
    if (a == 0xFFFF) {
        ka = 0x4E20;
    } else {
        ka = D_801D35B4[a & 0x1FF];
    }
    b = *(u16*)(*arg2 + arg1 * 2);
    if (b == 0xFFFF) {
        kb = 0x4E20;
    } else {
        kb = D_801D35B4[b & 0x1FF];
    }
    return Sign(ka - kb);
}

// Sort comparator for the "Field" arrange option: groups items usable in the
// field (usage flag 0x4) ahead of others; empty slots (0xFFFF) sort first.
static s32 CompareItemsByField(s16 arg0, s16 arg1, s32* arg2) {
    u16 a = *(u16*)(*arg2 + arg0 * 2);
    s32 ka;
    u16 b;
    s32 kb;
    if (a == 0xFFFF) {
        ka = 0;
    } else {
        ka = (func_8001FAF8(a & 0x1FF) & 4) ? 1 : 2;
    }
    b = *(u16*)(*arg2 + arg1 * 2);
    if (b == 0xFFFF) {
        kb = 0;
    } else {
        kb = (func_8001FAF8(b & 0x1FF) & 4) ? 1 : 2;
    }
    return Sign(kb - ka);
}

// Sort comparator for the "Battle" arrange option: groups items usable in
// battle (usage flag 0x2) ahead of others; empty slots (0xFFFF) sort first.
static s32 CompareItemsByBattle(s16 arg0, s16 arg1, s32* arg2) {
    u16 a = *(u16*)(*arg2 + arg0 * 2);
    s32 ka;
    u16 b;
    s32 kb;
    if (a == 0xFFFF) {
        ka = 0;
    } else {
        ka = (func_8001FAF8(a & 0x1FF) & 2) ? 1 : 2;
    }
    b = *(u16*)(*arg2 + arg1 * 2);
    if (b == 0xFFFF) {
        kb = 0;
    } else {
        kb = (func_8001FAF8(b & 0x1FF) & 2) ? 1 : 2;
    }
    return Sign(kb - ka);
}

// Sort comparator for the "Throw" arrange option: groups throwable items
// (usage flag 0x8) ahead of others; empty slots (0xFFFF) sort first.
static s32 CompareItemsByThrow(s16 arg0, s16 arg1, s32* arg2) {
    u16 a = *(u16*)(*arg2 + arg0 * 2);
    s32 ka;
    u16 b;
    s32 kb;
    if (a == 0xFFFF) {
        ka = 0;
    } else {
        ka = (func_8001FAF8(a & 0x1FF) & 8) ? 1 : 2;
    }
    b = *(u16*)(*arg2 + arg1 * 2);
    if (b == 0xFFFF) {
        kb = 0;
    } else {
        kb = (func_8001FAF8(b & 0x1FF) & 8) ? 1 : 2;
    }
    return Sign(kb - ka);
}

// Swap two item inventory slots (indices arg0 and arg1 in the u16 array at
// *arg2). Used by the item menu's "Customize" manual swap and as the swap
// callback for the inventory sort.
static void SwapItemSlots(s16 arg0, s16 arg1, s32* arg2) {
    SwapU16((u16*)(*arg2 + arg0 * 2), (u16*)(*arg2 + arg1 * 2));
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
        Quicksort((s32)D_8009CBE0, 0x140, CompareItemsByField, SwapItemSlots);
        break;
    case 2:
        Quicksort((s32)D_8009CBE0, 0x140, CompareItemsByBattle, SwapItemSlots);
        break;
    case 3:
        Quicksort((s32)D_8009CBE0, 0x140, CompareItemsByThrow, SwapItemSlots);
        break;
    case 4:
        Quicksort((s32)D_8009CBE0, 0x140, CompareItemsByType, SwapItemSlots);
        break;
    case 5:
        Quicksort((s32)D_8009CBE0, 0x140, CompareItemsByName, SwapItemSlots);
        break;
    case 6:
        Quicksort((s32)D_8009CBE0, 0x140, CompareItemsByMost, SwapItemSlots);
        break;
    case 7:
        Quicksort((s32)D_8009CBE0, 0x140, CompareItemsByLeast, SwapItemSlots);
        break;
    }
}

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0BA0);

// True if the two adjacent record fields for entry arg0 are equal.
s32 func_801D0CAC(s32 arg0) {
    return D_8009D85E[arg0 * 0x220] == D_8009D85C[arg0 * 0x220];
}

// True if the two adjacent record fields for entry arg0 are equal.
s32 func_801D0CE8(s32 arg0) {
    return D_8009D862[arg0 * 0x220] == D_8009D860[arg0 * 0x220];
}

// Builds a 10-bit mask of which of character arg0's slots are occupied (slot
// value != 0x7F), clears bit 9, and returns whether it matches the stored
// value.
s32 func_801D0D24(s32 arg0) {
    s32 mask;
    s32 i;
    for (i = 0, mask = 0; i < 10; i++) {
        if (func_80015AFC(arg0, i) != 0x7F) {
            mask |= 1 << i;
        }
    }
    mask &= ~0x200;
    return (D_8009C75A[arg0 * 0x42] ^ mask) == 0;
}

// Returns an item's usage flags (func_8001FAF8), with two context-dependent
// overrides: item 0x46 (the Tent) becomes field-usable while a location flag
// permits resting, and item 0x62 (the Save Crystal) while its one-time-use
// save flag is still clear.
s32 func_801D0DCC(s32 arg0) {
    s32 flags = func_8001FAF8(arg0);
    if (arg0 != 0x46) {
        if (arg0 == 0x62) {
            if (!(D_8009D5E8 & 2)) {
                flags |= 4;
            }
        }
    } else {
        if (D_80062F50 & 0x200) {
            flags |= 4;
        }
    }
    return flags;
}

// Copies 0x50 bytes from arg0 into the D_801D3E60 buffer.
void func_801D0E4C(u8* arg0) {
    s32 i;
    for (i = 0; i < 0x50; i++) {
        D_801D3E60[i] = *arg0;
        arg0++;
    }
}

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0E80);

void func_801D296C(void) {}

static void EvictWeakestStolenMateria(s32 newMateria, s32 priority) {
    s32 i;
    s32* loot;

    i = 0;
    loot = g_MateriaStealLoot;
    do {
        if (g_MateriaPriority[*(u8*)loot] == priority) {
            *loot = newMateria;
            return;
        }
        i += 1;
        loot += 1;
    } while (i < 0x30);
}

static s32 GetLowestStealPriority(void) {
    s32 i;
    s32 lowest;
    u8* loot;

    lowest = 0xFF;
    i = 0;
    loot = (u8*)g_MateriaStealLoot;
    do {
        u8 id = *loot;
        s32 prio = g_MateriaPriority[id];
        if (prio < lowest) {
            lowest = prio;
        }
        i += 1;
        loot += 4;
    } while (i < 0x30);
    return lowest;
}

static void OfferMateriaToSteal(s32* materiaPtr) {
    s32 i;
    s32 lowest;

    if (*materiaPtr == -1) {
        return;
    }
    i = 0;
    do {
        if (g_MateriaStealLoot[i] == -1) {
            g_MateriaStealLoot[i] = *materiaPtr;
            return;
        }
        i += 1;
    } while (i < 0x30);

    lowest = GetLowestStealPriority();
    if (g_MateriaPriority[*materiaPtr & 0xFF] < lowest) {
        return;
    }
    EvictWeakestStolenMateria(*materiaPtr, lowest);
}

// Re-equip a returned materia into the first free, unlocked weapon then armor
// slot of any visible party member. Returns 0 if placed, 1 if no slot was free.
static s32 ReequipReturnedMateria(s32 materia) {
    s32 c;

    for (c = 8; c != -1; c--) {
        if ((Savemap.phs_visibility_mask >> c) & 1) {
            {
                s32 j;
                for (j = 0; j < 8; j++) {
                    if (Savemap.party[c].materia_weapon[j] == -1 &&
                        D_800738BC[Savemap.party[c].weapon][j]) {
                        Savemap.party[c].materia_weapon[j] = materia;
                        return 0;
                    }
                }
            }
            {
                s32 j;
                for (j = 0; j < 8; j++) {
                    if (Savemap.party[c].materia_armor[j] == -1 &&
                        D_80071E4D[Savemap.party[c].armor][j]) {
                        Savemap.party[c].materia_armor[j] = materia;
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

static void RemoveMateriaFromPlayer(s32 materia) {
    s32 c;
    s32 s;

    for (c = 0; c < 9; c++) {
        if ((Savemap.phs_visibility_mask >> c) & 1) {
            for (s = 0; s < 8; s++) {
                if (Savemap.party[c].materia_weapon[s] == materia) {
                    Savemap.party[c].materia_weapon[s] = -1;
                    return;
                }
            }
            for (s = 0; s < 8; s++) {
                if (Savemap.party[c].materia_armor[s] == materia) {
                    Savemap.party[c].materia_armor[s] = -1;
                    return;
                }
            }
        }
    }
    for (s = 0; s < MAX_MATERIA_COUNT; s++) {
        if (Savemap.materia[s] == materia) {
            Savemap.materia[s] = -1;
            return;
        }
    }
}

static void FinalizeMateriaSteal(void) {
    s32 i;
    s32 materia;

    for (i = 0; i < 0x30; i++) {
        materia = g_MateriaStealLoot[i];
        if (materia != -1) {
            RemoveMateriaFromPlayer(materia);
        }
    }
    for (i = 0; i < 0x30; i++) {
        Savemap.yuffie_stolen_materia[i] = g_MateriaStealLoot[i];
    }
}

void StealAllMateria(void) {
    s32 i;
    s32 c;
    s32 slot;

    for (i = 0; i < 0x30; i++) {
        g_MateriaStealLoot[i] = -1;
    }
    for (c = 0; c < 9; c++) {
        if ((Savemap.phs_visibility_mask >> c) & 1) {
            for (slot = 0; slot < 8; slot++) {
                do {
                    OfferMateriaToSteal(&Savemap.party[c].materia_weapon[slot]);
                } while (0);
            }
            for (slot = 0; slot < 8; slot++) {
                OfferMateriaToSteal(&Savemap.party[c].materia_armor[slot]);
            }
        }
    }
    for (slot = 0; slot < MAX_MATERIA_COUNT; slot++) {
        OfferMateriaToSteal(&Savemap.materia[slot]);
    }
    FinalizeMateriaSteal();
}

// Give back every materia that was stolen: try to re-equip each one, and if no
// equip slot is free, return it to the materia inventory instead.
void ReturnStolenMateria(void) {
    s32 i;

    for (i = 0; i < 0x30; i++) {
        if (Savemap.yuffie_stolen_materia[i] != -1) {
            if (ReequipReturnedMateria(Savemap.yuffie_stolen_materia[i]) != 0) {
                // no free equip slot - add it to the materia inventory
                func_8002542C(Savemap.yuffie_stolen_materia[i]);
            }
        }
    }
}

// Unequip a party member: move their 16 equipped materia into the materia
// inventory and their accessory into the item inventory.
void UnequipCharacterMateria(s32 charIdx) {
    u8 v;
    {
        s32 i = 0;
        s32 empty = -1;
        s32* p = Savemap.party[charIdx].materia_weapon;
        do {
            if (*p != empty) {
                func_8002542C(*p);
                *p = empty;
            }
            i += 1;
            p += 1;
        } while (i < 8);
    }
    {
        s32 i = 0;
        s32 empty = -1;
        s32* p = Savemap.party[charIdx].materia_armor;
        do {
            if (*p != empty) {
                func_8002542C(*p);
                *p = empty;
            }
            i += 1;
            p += 1;
        } while (i < 8);
    }
    v = Savemap.party[charIdx].accessory;
    if (v != 0xFF) {
        func_80025380((v + 0x120) | 0x200);
        Savemap.party[charIdx].accessory = 0xFF;
    }
}

// Save the current party lineup, a party member's weapon/armor ids, the first
// three materia inventory slots and the member's 16 equipped materia into the
// stolen-materia buffer (reused as scratch space), clearing each source slot.
void BackupCharacterMateria(s32 charIdx) {
    s32 i = 0;
    u8* base = (u8*)Savemap.yuffie_stolen_materia;
    {
        u8* b = base;
        do {
            *b = Savemap.partyID[i];
            i += 1;
            b += 1;
        } while (i < 3);
    }
    {
        s32 empty;
        s32* st;
        u8* b;
        i = 0;
        empty = -1;
        st = Savemap.materia;
        base[4] = Savemap.party[charIdx].weapon;
        b = base;
        base[5] = Savemap.party[charIdx].armor;
        do {
            s32 m = *st;
            i += 1;
            *(s32*)(b + 0x48) = m;
            *st = empty;
            st += 1;
            b += 4;
        } while (i < 3);
    }
    {
        s32 empty;
        s32 t;
        s32* ap;
        s32* wp;
        u8* b;
        u8* wbase;
        u8* abase;
        i = 0;
        empty = -1;
        t = charIdx * sizeof(SavePartyMember);
        wbase = (u8*)Savemap.party[0].materia_weapon;
        abase = wbase + 0x20;
        ap = (s32*)(abase + t);
        wp = (s32*)(wbase + t);
        b = base;
        do {
            s32 m;
            m = *wp;
            i += 1;
            *(s32*)(b + 8) = m;
            *wp = empty;
            wp += 1;
            m = *ap;
            *(s32*)(b + 0x28) = m;
            *ap = empty;
            ap += 1;
            b += 2;
            b += 2;
        } while (i < 8);
    }
    Savemap.party[charIdx].weapon = 0;
}

// Restore everything saved by BackupCharacterMateria: party lineup, the
// member's weapon/armor ids, the first three materia inventory slots and
// their 16 equipped materia.
void RestoreCharacterMateria(s32 charIdx) {
    s32 i = 0;
    u8* base = (u8*)Savemap.yuffie_stolen_materia;
    {
        u8* b = base;
        do {
            Savemap.partyID[i] = *b;
            i += 1;
            b += 1;
        } while (i < 3);
    }
    {
        s32* st;
        u8* b;
        i = 0;
        st = Savemap.materia;
        Savemap.party[charIdx].weapon = base[4];
        b = base;
        Savemap.party[charIdx].armor = base[5];
        do {
            s32 m = *(s32*)(b + 0x48);
            b += 4;
            i += 1;
            *st = m;
            st += 1;
        } while (i < 3);
    }
    {
        s32 t;
        s32* ap;
        s32* wp;
        u8* b;
        u8* wbase;
        u8* abase;
        i = 0;
        t = charIdx * sizeof(SavePartyMember);
        wbase = (u8*)Savemap.party[0].materia_weapon;
        abase = wbase + 0x20;
        ap = (s32*)(abase + t);
        wp = (s32*)(wbase + t);
        b = base;
        do {
            s32 m;
            m = *(s32*)(b + 8);
            i += 1;
            *wp = m;
            wp += 1;
            m = *(s32*)(b + 0x28);
            *ap = m;
            ap += 1;
            b += 2;
            b += 2;
        } while (i < 8);
    }
}

// Uploads the coin-pattern texture at D_801D3890 (64x32, 4bpp, seamlessly
// tileable) into VRAM: pixel data to (0x3F0, 0x120), CLUT to (0x110, 0x1E0).
// Runs once at boot/menu init (main -> func_80026258 -> func_80025008); the
// texture stays resident so the battle UI can scroll it as the animated
// backdrop behind the coin-throw amount prompt.
void func_801D3228(void) {
    func_80025D14(&D_801D3890, 0x3F0, 0x120, 0x110, 0x1E0);
}

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D3260);
