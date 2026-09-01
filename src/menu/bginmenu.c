//! PSYQ=3.3 CC1=2.7.2
#include <game.h>
#include <psxsdk/libetc.h>

typedef struct {
    u8 unk0;
    u8 unk1;
} Unk801D026C;
typedef struct {
    u8 unk0[8];
} Unk8001E040;

extern s32 D_801D07F0;
extern Unk8001E040 D_801D07F4[2];
extern s16 D_801D07FE;
extern u8 D_801D0804[];
extern u8 D_801D082C[21];
extern u8 D_801D0844[16];
extern u8 D_801D0854[7];
extern u8 D_801D085C[2];
extern MenuTable D_801D0860[];
extern s8 D_801D086B;
extern u8 D_8009D78A[];
extern s32 D_8009CE60[];

// Initializes window parameters and UI elements for party selection screen
// (bginmenu).
void func_801D0000(void) {
    volatile s32 padding;
    func_80026448(&D_801D0860[0], 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 0, 1, 0);
    func_80026448(&D_801D0860[1], 0, 0, 1, 3, 0, 0, 1, 9, 0, 0, 0, 0, 0);
    D_801D07F0 = 0;
}

// Updates screen state, renders menu elements, and handles
// scrolling/interaction.
void func_801D00C4(void) {
    volatile s32 padding[4];
    s32 i;

    func_800230C4(g_MenuRenderBufferIndex);
    if (D_801D07F0 == 0) {
        func_8001EB2C(0, D_801D07FE + (D_801D086B << 6) + 0x20);
    }
    func_80026F44(0x10, 0xB, D_801D0804, 7);
    for (i = 0; i < 2; i++) {
        func_8001E040(&D_801D07F4[i]);
    }
    func_800264A8(&D_801D0860[D_801D07F0]);
    if (g_Pad1KeysRepeat & PADRdown) {
        func_8002305C(5, 0);
        func_8002120C(0);
    }
}

// Empty stub/hook function.
static void func_801D01BC(void) {}

extern u8 D_8009C778[]; // Savemap.party
extern u8 D_8009C798[]; // Savemap.party

// Never called by this overlay -- present in the original and kept so the
// layout matches, like BrizadAttachToTargetUnused in brizad.c.
// Counts the materia equipped by party member arg0: the 8 weapon slots at
// party[arg0].materia_weapon plus the 8 armor slots, an empty slot being -1.
static s32 CountEquippedMateria(s32 arg0) {
    s32 i;
    s32 count;
    s32 minus_one;
    s32* ptr;

    i = 0;
    count = 0;
    minus_one = -1;
    ptr = (s32*)&D_8009C778[arg0 * 0x84];

    for (; i < 8; i++) {
        if (ptr[i] != minus_one) {
            count++;
        }
    }

    i = 0;
    minus_one = -1;
    ptr = (s32*)&D_8009C798[arg0 * 0x84];
    for (; i < 8; i++) {
        if (ptr[i] != minus_one) {
            count++;
        }
    }

    return count;
}

// Reads a 16-bit little-endian value from Unk801D026C structure.
static s32 BankRead16(Unk801D026C* arg0) {
    return arg0->unk0 | (arg0->unk1 << 8);
}

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
        scaled =
            Savemap.party[i].hp_cur *
            BankRead16((Unk801D026C*)&Savemap.memory_bank_2[116] + i) / 65535;
        if (scaled <= 0) {
            scaled = 1;
        }
        Savemap.party[i].hp_cur = scaled;
    }
}

// Removes materia `materiaId` from the party's slots and inventory -- the
// removal counterpart of PartyHasMasteredMateria/PartyHasMateria. Callers pass
// the same three materia lists those two are checked against.
static void RemoveMasteredMateria(s32 materiaId) {
    s32 i, j;

    materiaId |= 0xFFFFFF00;

    for (i = 0; i < MAX_PARTY_COUNT; i++) {
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

    for (i = 0; i < MAX_PARTY_COUNT; i++) {
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
    s32 flags;
    u8* base;
    u8* party0;
    u8* party1;
    u8* inventory;

    i = 0;
    base = D_8009D78A;
    flags = *(u16*)base;
    party1 = base - 0xFF2;
    party0 = base - 0x1012;

    for (; i < 9; i++) {
        if ((flags >> i) & 1) {
            for (j = 0; j < 8; j++) {
                if (party0[j * 4] == materiaId) {
                    return 1;
                }
            }
            for (j = 0; j < 8; j++) {
                if (party1[j * 4] == materiaId) {
                    return 1;
                }
            }
        }
        party1 += 0x84;
        party0 += 0x84;
    }

    inventory = (u8*)D_8009CE60;
    for (j = 0; j < 200; j++) {
        if (inventory[j * 4] == materiaId) {
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
        func_8002542C(0x49);
        break;
    case 1:
        for (i = 0; i < 16; i++) {
            RemoveMasteredMateria(D_801D0844[i]);
        }
        func_8002542C(0x5A);
        break;
    case 2:
        for (i = 0; i < 7; i++) {
            RemoveMasteredMateria(D_801D0854[i]);
        }
        func_8002542C(0x30);
        break;
    case 3:
        func_8002542C(0x58);
        break;
    }
}
