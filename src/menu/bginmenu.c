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

void func_801D0000(void) {
    volatile s32 padding;
    func_80026448(&D_801D0860[0], 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 0, 1, 0);
    func_80026448(&D_801D0860[1], 0, 0, 1, 3, 0, 0, 1, 9, 0, 0, 0, 0, 0);
    D_801D07F0 = 0;
}

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

static void func_801D01BC(void) {}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/menu/nonmatchings/bginmenu", func_801D01C4);
#else
// -O1
extern u8 D_8009C778[]; // Savemap.party
extern u8 D_8009C798[]; // Savemap.party
s32 func_801D01C4(s32 arg0) {
    s32 var_a1;
    s32 var_a2;
    s32 var_a3;
    s32* var_v1;

    var_a1 = 0;
    var_a2 = 0;
    var_a3 = -1;
    var_v1 = (s32*)&D_8009C778[arg0 * 0x84];
    for (; var_a1 < 8; var_a1++) {
        if (*var_v1 != var_a3) {
            var_a2++;
        }
        var_v1++;
    }
    var_a1 = 0;
    var_a3 = -1;
    var_v1 = (s32*)&D_8009C798[arg0 * 0x84];
    for (; var_a1 < 8; var_a1++) {
        if (*var_v1 != var_a3) {
            var_a2++;
        }
        var_v1++;
    }
    return var_a2;
}
#endif

static s32 func_801D0258(Unk801D026C* arg0) {
    return arg0->unk0 | (arg0->unk1 << 8);
}

static void func_801D026C(Unk801D026C* arg0, u16 arg1) {
    arg0->unk0 = arg1;
    arg0->unk1 = arg1 >> 8;
}

INCLUDE_ASM("asm/us/menu/nonmatchings/bginmenu", func_801D027C);

INCLUDE_ASM("asm/us/menu/nonmatchings/bginmenu", func_801D0324);

INCLUDE_ASM("asm/us/menu/nonmatchings/bginmenu", func_801D0408);

INCLUDE_ASM("asm/us/menu/nonmatchings/bginmenu", func_801D0500);

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
