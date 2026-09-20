//! PSYQ=3.3 CC1=2.7.2
#include <game.h>
#include <libetc.h>

typedef struct {
    u8 unk0;
    u8 unk1;
} Unk801D026C;

extern s32 D_801D07F0;
extern MenuRect D_801D07F4[2];
extern u8 D_801D0804[];
extern u8 D_801D082C[21];
extern u8 D_801D0844[16];
extern u8 D_801D0854[7];
extern u8 D_801D085C[2];
extern MenuTable D_801D0860[];

// Initializes window parameters and UI elements for party selection screen
// (bginmenu).
void func_801D0000(void) {
    volatile s32 padding;
    SysMenuSetCursorMovement(&D_801D0860[0], 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 0, 1, 0);
    SysMenuSetCursorMovement(&D_801D0860[1], 0, 0, 1, 3, 0, 0, 1, 9, 0, 0, 0, 0, 0);
    D_801D07F0 = 0;
}

// Updates screen state, renders menu elements, and handles
// scrolling/interaction.
void func_801D00C4(void) {
    volatile s32 padding[4];
    s32 i;

    SysMenuDrawMenuList(g_MenuRenderBufferIndex);
    if (D_801D07F0 == 0) {
        SysMenuDrawCursor(0, D_801D07F4[1].y + (D_801D0860[0].row << 6) + 0x20);
    }
    SysMenuDrawString(0x10, 0xB, D_801D0804, 7);
    for (i = 0; i < 2; i++) {
        SysMenuDrawWindow(&D_801D07F4[i]);
    }
    SysMenuHandleButtons(&D_801D0860[D_801D07F0]);
    if (g_Pad1KeysRepeat & PADRdown) {
        SysMenuSetMenuListAnimation(5, 0);
        SysMenuLoadMenuFileById(0);
    }
}

// Empty stub/hook function.
static void func_801D01BC(void) {}

static s32 CountEquippedMateria(s32 partyIndex) {
    s32 i;
    s32 count;

    i = 0;
    count = 0;
    for (; i < NUM_MATERIA_ROW; i++) {
        if (Savemap.party[partyIndex].materia_weapon[i] != -1) {
            count++;
        }
    }

    for (i = 0; i < NUM_MATERIA_ROW; i++) {
        if (Savemap.party[partyIndex].materia_armor[i] != -1) {
            count++;
        }
    }

    return count;
}

// Reads a 16-bit little-endian value from Unk801D026C structure.
static s32 BankRead16(Unk801D026C* arg0) { return arg0->unk0 | (arg0->unk1 << 8); }

// Writes a 16-bit little-endian value into Unk801D026C structure.
static void BankWrite16(Unk801D026C* arg0, u16 arg1) {
    arg0->unk0 = arg1;
    arg0->unk1 = arg1 >> 8;
}

// Scales each party member's current HP by a per-member ratio out of 65535
// held in memory_bank_2[116..125], never dropping below 1. The bank values are
// byte-packed, hence the BankRead16 unaligned 16-bit read.
void ScalePartyHp(void) {
    s32 i;
    s32 scaled;

    for (i = 0; i < 5; i++) {
        scaled = Savemap.party[i].curHP * BankRead16((Unk801D026C*)&Savemap.memory_bank_2[116] + i) / 65535;
        if (scaled <= 0) {
            scaled = 1;
        }
        Savemap.party[i].curHP = scaled;
    }
}

// Removes materia `materiaId` from the party's slots and inventory -- the
// removal counterpart of PartyHasMasteredMateria/PartyHasMateria. Callers pass
// the same three materia lists those two are checked against.
static void RemoveMasteredMateria(s32 materiaId) {
    s32 i, j;

    materiaId |= 0xFFFFFF00;

    for (i = 0; i < NUM_CHARACTERS; i++) {
        if ((Savemap.phs_visibility_mask >> i) & 1) {
            for (j = 0; j < 8; j++) {
                if (Savemap.party[i].materia_weapon[j] == materiaId) {
                    Savemap.party[i].materia_weapon[j] = -1;
                    return;
                }
            }
            for (j = 0; j < 8; j++) {
                if (Savemap.party[i].materia_armor[j] == materiaId) {
                    Savemap.party[i].materia_armor[j] = -1;
                    return;
                }
            }
        }
    }

    for (j = 0; j < MAX_MATERIA_COUNT; j++) {
        if (Savemap.materia[j] == materiaId) {
            Savemap.materia[j] = -1;
            return;
        }
    }
}

// Returns 1 if the party owns a mastered copy of materia `materiaId`, searching
// every visible character's weapon and armor slots plus the whole materia
// inventory. Materia is stored as `id | (ap << 8)`, so an AP of 0xFFFFFF is
// the mastered marker.
static s32 PartyHasMasteredMateria(s32 materiaId) {
    s32 i, j;
    u32 materia;
    u32 ap;

    for (i = 0; i < NUM_CHARACTERS; i++) {
        if ((Savemap.phs_visibility_mask >> i) & 1) {
            for (j = 0; j < 8; j++) {
                materia = Savemap.party[i].materia_weapon[j];
                ap = materia >> 8;
                if (ap == 0xFFFFFF && (materia & 0xFF) == materiaId) {
                    return 1;
                }
            }
            for (j = 0; j < 8; j++) {
                materia = Savemap.party[i].materia_armor[j];
                ap = materia >> 8;
                if (ap == 0xFFFFFF && (materia & 0xFF) == materiaId) {
                    return 1;
                }
            }
        }
    }

    for (j = 0; j < MAX_MATERIA_COUNT; j++) {
        materia = Savemap.materia[j];
        ap = materia >> 8;
        if (ap == 0xFFFFFF && (materia & 0xFF) == materiaId) {
            return 1;
        }
    }

    return 0;
}

// Returns 1 if the party owns materia `materiaId` at all, mastered or not --
// the same search as PartyHasMasteredMateria above, minus the mastered-AP test.
// Walks every visible character's weapon and armor slots, then the materia
// inventory. (Despite the raw u8* pointers, base - 0x1012 and base - 0xFF2
// are party[0].materia_weapon and party[0].materia_armor.)
static s32 PartyHasMateria(s32 materiaId) {
    s32 i, j;

    i = 0;
    for (; i < NUM_CHARACTERS; i++) {
        if ((Savemap.phs_visibility_mask >> i) & 1) {
            for (j = 0; j < NUM_MATERIA_ROW; j++) {
                if ((u8)Savemap.party[i].materia_weapon[j] == materiaId) {
                    return 1;
                }
            }
            for (j = 0; j < NUM_MATERIA_ROW; j++) {
                if ((u8)Savemap.party[i].materia_armor[j] == materiaId) {
                    return 1;
                }
            }
        }
    }

    for (j = 0; j < MAX_MATERIA_COUNT; j++) {
        if ((u8)Savemap.materia[j] == materiaId) {
            return 1;
        }
    }

    return 0;
}

// Validates party conditions by category (0..3) and sets authorization flag in
// Savemap.memory_bank_5[111].
void func_801D05C4(s32 arg0) {
    s32 i;

    Savemap.memory_bank_5[111] = 0;
    switch (arg0) {
    case 0:
        for (i = 0; i < 21; i++) {
            if (!PartyHasMasteredMateria(D_801D082C[i])) {
                return;
            }
        }
        break;
    case 1:
        for (i = 0; i < 16; i++) {
            if (!PartyHasMasteredMateria(D_801D0844[i])) {
                return;
            }
        }
        break;
    case 2:
        for (i = 0; i < 7; i++) {
            if (!PartyHasMasteredMateria(D_801D0854[i])) {
                return;
            }
        }
        break;
    case 3:
        for (i = 0; i < 2; i++) {
            if (!PartyHasMateria(D_801D085C[i])) {
                return;
            }
        }
        break;
    }
    Savemap.memory_bank_5[111] = 1;
}

// Applies member removal/confirmation and plays corresponding sound effects.
void func_801D0704(s32 arg0) {
    s32 i;
    switch (arg0) {
    case 0:
        for (i = 0; i < 21; i++) {
            RemoveMasteredMateria(D_801D082C[i]);
        }
        SysMenuAddMateria(0x49);
        break;
    case 1:
        for (i = 0; i < 16; i++) {
            RemoveMasteredMateria(D_801D0844[i]);
        }
        SysMenuAddMateria(0x5A);
        break;
    case 2:
        for (i = 0; i < 7; i++) {
            RemoveMasteredMateria(D_801D0854[i]);
        }
        SysMenuAddMateria(0x30);
        break;
    case 3:
        SysMenuAddMateria(0x58);
        break;
    }
}
