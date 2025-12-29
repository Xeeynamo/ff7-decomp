//! PSYQ=3.3 CC1=2.7.2
#include "battle_private.h"

static void func_800E5358(void);
static void func_800E4B88(void);
static s32 func_800E4BCC(void);

void func_800D8A04(void) {}

s32 func_800D8A0C(s32 arg0) { return arg0 < 0 ? -arg0 : arg0; }

void func_800D8A24(void) {}

void func_800D8A2C(void) { D_800F199C = 0; }

static void func_800D8A3C(s32 arg0) { D_800F9780[D_800F199C++] = arg0; }

void func_800D8A70(void) {}

void func_800D8A78(s8 arg0) { D_800F19A4 = arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D8A88);

static void func_800D8AF0(u16 arg0) {
    D_8009A000 = arg0;
    D_8009A004 = arg0;
    D_8009A008 = arg0;
    func_8002DA7C(arg0);
}

void func_800D8B2C(void) {
    if (D_80062D99) {
        func_800D8AF0(0x98);
    }
    D_80062D99 = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D8B60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D8D78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D91DC);

void func_800D93DC(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D93E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D9BF4);

u8 func_800D9DEC(s16 arg0) { return D_800F514C[arg0]; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D9E0C);

static void func_800D9F5C(s16 arg0) { D_800F514C[arg0] = 3; }

static void func_800D9F80(void) {
    D_800F311C = 0;
    D_800FAFEC = -0x100;
    D_800FAFF0 = -0x100;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D9FA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DA380);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DB818);

const s32 D_800A0E48[] = {0, 0, 0};
void func_800DBC18(s32, s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DBC18);

static void func_800DBEA4(s32 arg0, s16 arg1) { func_800DBC18(arg0, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DBEC8);

static void func_800DBF54(void) { SetPolyBuffer(D_80077F64); }

static void func_800DBF7C(void) { D_800F3122 = 0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DBF8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DC0CC);

void func_800DCF58(void) {}

void func_800DCF60(s16 arg0, s16 arg1) {
    D_800F3138 = 1;
    D_800F313C = arg0;
    D_800F3140 = arg1;
}

void func_800DCF94(s16 arg0) {
    if (arg0 == -1) {
        D_800F1E4F = 0;
        return;
    }
    D_800F1E4F = 1;
    D_800F1E50 = arg0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DCFD4);

static void func_800E6848(void);
static void func_800DD690(s32 arg0, s16 arg1) {
    s32 setupType;
    s32 temp_v1;

    MenuSetOTag();
    switch (arg1) {
    case 0:
        func_800E2098();
        break;
    case 1:
        func_800E2C6C(&D_800F90B4[D_800F38A0]);
        break;
    case 5:
        func_800E3088();
        break;
    case 6:
        func_800E3B64();
        break;
    case 4:
        func_800E3FB4();
        break;
    case 7:
        func_800E3E10();
        break;
    case 2:
        setupType = SETUP_SIDE_ATTACK_3;
        temp_v1 = D_8016360C.setup.type;
        if (temp_v1 < NUM_SETUP) {
            setupType = -(temp_v1 <= SETUP_BACK_ATTACK) & 7;
        }
        MenuDrawText(8, 7, func_80015248(5, 0x12, 8), setupType);
        break;
    case 3:
        MenuDrawText(9, 7, func_80015248(5, 0x13, 8), 7);
        break;
    case 24:
        func_800E33A0();
        break;
    case 20:
        func_800E4180();
        break;
    case 21:
        func_800E4394();
        break;
    case 19:
        func_800E4A64();
        break;
    case 26:
        func_800E4C08();
        break;
    case 27:
        func_800E5530();
        break;
    case 28:
        func_800E5978();
        break;
    case 9:
        func_800E6848();
        break;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DD85C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DDAD8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DDC34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DDCE8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DDE90);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DDFEC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DE2B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DE3CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DE46C);

void func_800DE5D8(void) {}

void func_800DE5E0(void) {}

void func_800DE5E8(void) {}

void func_800DE5F0(void) {}

void func_800DE5F8(void) {}

void func_800DE600(void) {}

void func_800DE608(void) {}

void func_800DE610(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DE618);

void func_800DE910(void) {}

void func_800DE918(void) {
    D_80151840 = D_8009D260;
    if (D_8009D260 > 600000) {
        D_80151840 = 600000;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DE94C);

void func_800DEB10(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DEB18);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DEC10);

void func_800DF244(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF24C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF2CC);

void func_800DF528(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF5C8);

void func_800DF7BC(void) {}

void func_800DF7C4(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF7CC);

void func_800DF8F0(void) {}

void func_800DF8F8(void) {}

void func_800DF900(void) {
    u16* tapped;

    if (D_800F3896 == 3 && D_800F99E4 == 0) {
        tapped = &ButtonsRepeating;
        if (*tapped & 0x20) {
            func_800BB9B8(1);
            D_800F99E4 = 1;
            D_800F3896 = -1;
            func_800A4350(D_800F38A0, D_800F389C, D_800F389E, D_801516F8);
            func_800D9F5C(3);
            func_800D9F5C(1);
        } else if (ButtonsRepeating & 0x8040) {
            func_800BB9B8(4);
            D_800F99E4 = 1;
            D_800F3896 = 1;
            func_800D9F5C(3);
        }
    }
}

void func_800DF9F0(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF9F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DFA94);

void func_800DFC38(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DFC40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DFE34);

void func_800DFFDC(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DFFE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E010C);

void func_800E026C(void) {}

void func_800E53C8();
void func_800E0274(void) { func_800E53C8(); }

void func_800E0294(void) {
    u8* temp_s0;

    temp_s0 = &D_8009D84C[D_800F38A1].unAC[0];
    if (D_800F3896 == 0x1B && D_800F99E4 == 0) {
        if (ButtonsRepeating & 0x20) {
            if (func_800E54EC() == 2) {
                D_800F99E4 = 1;
                func_800BB9B8(1);
                D_800F38A2 = temp_s0[3];
                D_800F389E = temp_s0[0];
                D_800FAFD4 = 0;
                func_800E6B94();
                func_800DDFEC();
                func_800E5814();
                func_800D9F5C(0x1B);
                func_800D9F5C(1);
            }
        } else if ((ButtonsRepeating & 0x40) && (func_800E54EC() == 0)) {
            func_800BB9B8(4);
            D_800F99E4 = 1;
            D_800F3896 = 1;
            func_800D9F5C(0x1B);
        }
    }
}

void func_800E03C8(void) {}

void func_800E03D0(void) { func_800E4B88(); }

void func_800E03F0(void) {
    u8* temp_s0;

    temp_s0 = &D_8009D8F8[D_800F38A1 * 0x440];
    if ((D_800F3896 == 0x1A) && (D_800F99E4 == 0)) {
        if (ButtonsRepeating & 0x20) {
            D_800F99E4 = 1;
            if (func_800E4BCC() == 2) {
                func_800BB9B8(1);
                D_800F38A2 = temp_s0[3];
                D_800F389E = temp_s0[0];
                D_800FAFD4 = 0;
                func_800E6B94();
                func_800DDFEC();
                func_800E5358();
                func_800D9F5C(0x1A);
                func_800D9F5C(1);
            }
        } else if ((ButtonsRepeating & 0x40) && (func_800E4BCC() == 0)) {
            func_800BB9B8(4);
            D_800F99E4 = 1;
            D_800F3896 = 1;
            func_800D9F5C(0x1A);
        }
    }
}

void func_800E0528(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E0530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E05E4);

void func_800E078C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E0794);

void func_800E084C(void) {
    if (D_800F3896 == 9) {
        MenuCursorPickerHandler(&D_800F9144);
        if (ButtonsTapped & 0x20) {
            if (D_800F914E == 0) {
                func_800A4844(1);
            } else {
                func_800A4844(0);
            }
            func_800D9F5C(9);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E08C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E0BE0);

static void func_800E0DF4(void) {
    s16 i;

    for (i = 0; i < 0x20; i++) {
        D_800F514C[i] = 0;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E0E34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E15D8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E16B8);

static void func_800E1938(s16 arg0, s16 arg1, s32 arg2) {
    s32 temp_a2;
    u32 var_a3;
    s32 x;
    s32 y;

    if (arg2 < 0x80) {
        var_a3 = 0;
    } else if (arg2 < 0x100) {
        temp_a2 = arg2 - 0x80;
        if (temp_a2 < 0x10) {
            var_a3 = 1;
        } else if (temp_a2 < 0x20) {
            var_a3 = 3;
        } else if (temp_a2 < 0x30) {
            var_a3 = 2;
        } else if (temp_a2 < 0x3E) {
            var_a3 = 5;
        } else if (temp_a2 < 0x49) {
            var_a3 = 4;
        } else if (temp_a2 < 0x57) {
            var_a3 = 9;
        } else if (temp_a2 < 0x65) {
            var_a3 = 6;
        } else if (temp_a2 < 0x72) {
            var_a3 = 7;
        } else {
            var_a3 = 8;
        }
    } else if (arg2 < 0x120) {
        var_a3 = 10;
    } else {
        var_a3 = 11;
    }
    x = ((var_a3 & 1) << 4) | 0x60;
    y = ((var_a3 >> 1) << 4) + 0x70;
    MenuDrawBtlImage(arg0, arg1, x, y, 0x10, 0x10, 1, 0);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E1A2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E1AC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E1C40);

void func_800E2054(s32 arg0, s32 arg1) {
    func_80027354(0xB0, arg0, &D_800F3184[arg1 * 10], 3);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E2098);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E2C6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E3088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E33A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E368C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E3B64);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E3E10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E3FB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E4180);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E4394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E4A64);

static void func_800E4B88(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        D_800F33A0[i] = 0;
    }

    D_800F5760 = 10;
    D_800F33AA = 0;
    D_800F5764 = 2;
}

static s32 func_800E4BCC(void) {
    if (D_800F33A0[0] == 0) {
        return 0;
    }
    if (D_800F33AA == 3) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E4C08);

static void func_800E5358(void) {
    s32 i;
    s32 index;

    for (i = 0; i < 3; i++) {
        index = (1 - ((D_800F338C[i] >> (D_800F5764 + 1)) & 0xF)) & 0xF;
        D_80163774[i] = D_800F332C[i][index];
    }
}

void func_800E53C8();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E53C8);

s32 func_800E54EC(void) {
    if (D_800F33A0[0] == 0) {
        return 0;
    }
    if (D_800F33AA == D_800F5774) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E5530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E5814);

void func_800E58B0(void) {
    D_800F3468 = 0;
    D_800F5760 = 10;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E58CC);

static u8 func_800E593C(void) { return D_800F381C[D_80163604]; }

s32 func_800E5960(s32 arg0) { return arg0 < 0 ? -arg0 : arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E5978);

// BUG: ret is not initialized
static s32 func_800E5F30() {
    s32 i;
    s32 ret;

    for (i = 0; i < 3; i++) {
        if (D_8009CBDC[i] != 0xFF) {
            ret = D_8009CBDC[i];
        }
    }
    return ret;
}

// BUG: ret is not initialized
static s32 func_800E5F70(void) {
    s32 i;
    s32 ret;

    for (i = 0; i < 3; i++) {
        if (D_8009CBDC[i] != 0xFF) {
            ret = i;
        }
    }

    return ret;
}

#ifndef NON_MATCHING
// might require a file split to match due to mis-aligned rodata
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E5FB4);
#else
s32 func_800E5FB4(u32 arg0, s32 arg1) {
    s32 ret;

    ret = 0;
    switch (arg0) {
    case 0:
        if (arg1 == 4) {
            ret = 1;
        }
        break;
    case 1:
        if (arg1 == 10) {
            ret = 1;
        }
        break;
    case 2:
        if (arg1 == 5) {
            ret = 1;
        }
        break;
    case 3:
        if (arg1 == 9) {
            ret = 1;
        }
        break;
    case 4:
        if (arg1 == 8) {
            ret = 1;
        }
        break;
    case 5:
        ret = 1;
        break;
    }
    return ret;
}
#endif

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E6018);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E60F8);

s32 func_800E6820(void) { return MenuGetTextWidth(D_800F384A); }

static void func_800E6848(void) {
    MenuDrawText(0x10, 8, &D_800F3828[0], 7);
    MenuDrawText(0x2C, 0x1C, &D_800F3828[0x22], 7);
    MenuDrawText(func_800E6820() + 0x4C, 0x1C, &D_800F3828[0x44], 7);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E68B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E6904);

// BUG function can return undefined value
static s32 func_800E6B40(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if ((1 << D_800F38A7) & D_801516F8 & D_8016376E[i]) {
            return i;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E6B94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E6DCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E7170);
