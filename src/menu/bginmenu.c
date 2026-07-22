//! PSYQ=3.3 CC1=2.7.2
#include <game.h>

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
extern Unk80026448 D_801D0860[];
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

    func_800230C4(D_80062F58);
    if (D_801D07F0 == 0) {
        func_8001EB2C(0, D_801D07FE + (D_801D086B << 6) + 0x20);
    }
    func_80026F44(0x10, 0xB, D_801D0804, 7);
    for (i = 0; i < 2; i++) {
        func_8001E040(&D_801D07F4[i]);
    }
    func_800264A8(&D_801D0860[D_801D07F0]);
    if (D_80062D7E & 0x40) {
        func_8002305C(5, 0);
        func_8002120C(0);
    }
}

// Empty stub/hook function.
static void func_801D01BC(void) {}

extern u8 D_8009C778[]; // Savemap.party
extern u8 D_8009C798[]; // Savemap.party

// Counts active party members for section arg0 in Savemap.
s32 func_801D01C4(s32 arg0) {
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
static s32 func_801D0258(Unk801D026C* arg0) {
    return arg0->unk0 | (arg0->unk1 << 8);
}

// Writes a 16-bit little-endian value into Unk801D026C structure.
static void func_801D026C(Unk801D026C* arg0, u16 arg1) {
    arg0->unk0 = arg1;
    arg0->unk1 = arg1 >> 8;
}

// Recalculates display stats and ratios for party members.
INCLUDE_ASM("asm/us/menu/nonmatchings/bginmenu", func_801D027C);

// Removes specified item, materia, or character ID from party/inventory.
INCLUDE_ASM("asm/us/menu/nonmatchings/bginmenu", func_801D0324);

// Checks if a 32-bit ID (item, materia, or character) exists in party or
// inventory.
INCLUDE_ASM("asm/us/menu/nonmatchings/bginmenu", func_801D0408);

// Checks if a character ID (byte) exists in active party list or inventory.
s32 func_801D0500(s32 arg0) {
    s32 i, j;
    s32 flags;
    u8* base;
    u8* party0;
    u8* party1;
    u8* inventory;

    i = 0;
    base = D_8009D78A;
    flags = *(u16*)base;
    asm("" : "=r"(flags) : "0"(flags));
    party1 = base - 0xFF2;
    party0 = base - 0x1012;

    for (; i < 9; i++) {
        if ((flags >> i) & 1) {
            for (j = 0; j < 8; j++) {
                if (party0[j * 4] == arg0) {
                    return 1;
                }
            }
            for (j = 0; j < 8; j++) {
                if (party1[j * 4] == arg0) {
                    return 1;
                }
            }
        }
        party1 += 0x84;
        party0 += 0x84;
    }

    inventory = (u8*)D_8009CE60;
    for (j = 0; j < 200; j++) {
        if (inventory[j * 4] == arg0) {
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
            if (!func_801D0408(D_801D082C[i])) {
                return;
            }
        }
        break;
    case 1:
        for (i = 0; i < 16; i++) {
            if (!func_801D0408(D_801D0844[i])) {
                return;
            }
        }
        break;
    case 2:
        for (i = 0; i < 7; i++) {
            if (!func_801D0408(D_801D0854[i])) {
                return;
            }
        }
        break;
    case 3:
        for (i = 0; i < 2; i++) {
            if (!func_801D0500(D_801D085C[i])) {
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
            func_801D0324(D_801D082C[i]);
        }
        func_8002542C(0x49);
        break;
    case 1:
        for (i = 0; i < 16; i++) {
            func_801D0324(D_801D0844[i]);
        }
        func_8002542C(0x5A);
        break;
    case 2:
        for (i = 0; i < 7; i++) {
            func_801D0324(D_801D0854[i]);
        }
        func_8002542C(0x30);
        break;
    case 3:
        func_8002542C(0x58);
        break;
    }
}
