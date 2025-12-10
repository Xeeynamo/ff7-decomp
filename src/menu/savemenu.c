//! PSYQ=3.3 CC1=2.7.2
#include <psxsdk/libapi.h>
#include <psxsdk/libgpu.h>
#include <psxsdk/kernel.h>

#include "savemenu.h"

static void func_801D0408(u16 arg0) {
    D_8009A000 = 0x30;
    D_8009A004 = arg0;
    D_8009A008 = arg0;
    func_8002DA7C(arg0);
}

static s32 func_801D0448(s32 arg0) {
    RECT rect;

    setTile(D_80062F24.tile);
    SetSemiTrans(D_80062F24.tile, 1);
    D_80062F24.tile->x0 = 0;
    D_80062F24.tile->y0 = 0;
    D_80062F24.tile->w = 0x180;
    D_80062F24.tile->h = 0xE8;
    D_80062F24.tile->r0 = D_801D4EC4;
    D_80062F24.tile->g0 = D_801D4EC4;
    D_80062F24.tile->b0 = D_801D4EC4;
    AddPrim(D_80062FC4, D_80062F24.tile++);
    rect.x = 0;
    rect.y = 0;
    rect.w = 255;
    rect.h = 255;
    func_80026A34(0, 1, 0x5F, &rect);
    D_801D4EC4 += arg0;
    if (D_801D4EC4 < 0) {
        D_801D4EC4 = 0;
    }
    if (D_801D4EC4 >= 0x100) {
        D_801D4EC4 = 0xFF;
    }
    return D_801D4EC4;
}

void func_801D05C0(u8 arg0) {
    D_801E3860 = 0xF0;
    D_801E36B8 = arg0;
    D_801E3850 = 0;
    func_80026448(menus.D_801E379C, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
    func_80025B8C(&D_801E8F44);
    func_80025C14(&D_801E4538);
    func_80025DF8();
    func_801D19C4();
}

static void func_801D0670(void) {
    func_80025BD0(D_801E8F44);
    func_80025C54(D_801E4538);
    func_801D1BA4();
}

int func_801D06B0(s32 arg0) {
    RECT sp38;
    RECT rect;
    s32 temp_s1;
    s32 temp_s2;
    s32 temp_v1;
    s32 var_s0;
    s32 var_s1;
    s32 var_s2;
    s32 var_s3;
    s32 var_v0_6;
    s8 temp_s0_2;

    if (D_801E36B8 == 0) {
        func_800230C4(D_80062F58);
    } else if (D_801E36B0 == 0) {
        if (func_801D0448(-15) == 0) {
            D_801E36B0 = 1;
        }
    } else if (D_801E36B0 == 2) {
        if (func_801D0448(15) == 0xFF) {
            D_801E36B0 = -1;
        }
    }
    if (!func_80023050() || (D_801E36B8 && D_801E36B0 == 1)) {
        if (!(u8)func_8001F6B4()) {
            if (D_801E3850 >= 0 && D_801E3850 < 2) {
                func_801D3668(arg0);
            }
            if (D_801E3860) {
                D_801E3860--;
            }
        }
    }
    func_80026B5C(0x80);
    switch (D_801E3850) {
    case 0:
        func_8001EB2C(D_801D4EC8.x - 18,
                      D_801D4EC8.y + 6 + (menus.D_801E379C[0].unkB * 12));
        func_80026F44(0xA, 0xB, D_801E2CFC[1], 7);
        func_80026F44(D_801D4EC8.x + 12, D_801D4EC8.y + 5, D_801E2CFC[3],
                      -(D_801E8F38[0][0] != 0) & 7);
        func_80026F44(D_801D4EC8.x + 12, D_801D4EC8.y + 17, D_801E2CFC[4],
                      -(D_801E8F3B != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        func_80026A34(0, 1, 0x7F, &rect);
        func_8001E040(&D_801D4EC8);
        break;
    case 7:
        func_8001EB2C(D_801D4ED0.x + 0x16,
                      0x15 + D_801D4ED0.y + menus.D_801E3808[1].unkB * 0xC);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        func_80026A34(0, 1, 0x7F, &rect);
        func_80026F44(D_801D4ED0.x + 0xA, D_801D4ED0.y + 6, D_801E2CFC[33], 7);
        func_80026F44(D_801D4ED0.x + 48, D_801D4ED0.y + 19, D_801E2CFC[34], 7);
        func_80026F44(D_801D4ED0.x + 48, D_801D4ED0.y + 31, D_801E2CFC[35], 7);
        func_8001E040(&D_801D4ED0);
        /* fallthrough */
    case 1:
        if (!D_801E8F38[menus.D_801E379C[0].unkB][0]) {
            D_801E3850 = 0;
        } else {
            func_800269D0();
            if (D_801E36B8 == 0) {
                func_800269C0(D_80062F58 * 0x5000 + D_801D4EDC);
            } else {
                func_800269C0(D_801E36B4 * 0x5000 + D_801D4EDC);
            }
            if (D_801E3850 != 7 || (arg0 & 2)) {
                func_8001EB2C(8, (menus.D_801E379C[1].unkB << 6) | 0x38);
            }
            var_s3 = !menus.D_801E379C[1].unk8 ? 3 : 4;
            for (var_s0 = 0; var_s0 < var_s3; var_s0++) {
                if ((D_80062F3C >> (var_s0 + menus.D_801E379C[1].unk2)) & 1) {
                    func_8001DE70();
                    func_801D370C(
                        0, var_s0 * 64 + 29 + menus.D_801E379C[1].unkF * 8,
                        var_s0 + menus.D_801E379C[1].unk2);
                    func_8001DEB0();
                } else {
                    func_80026F44(
                        0x32, var_s0 * 64 + 55 + menus.D_801E379C[1].unkF * 8,
                        D_801E2CFC[8], 6);
                    func_8001DE40(&sp38, &D_801DEEF4);
                    func_8001DE24(
                        &sp38, 0,
                        var_s0 * 64 + 29 + menus.D_801E379C[1].unkF * 8);
                    func_8001E040(&sp38);
                }
            }
            func_80026B5C(0x80);
            rect.y = 29;
            rect.w = 364;
            rect.x = 0;
            rect.h = 195;
            if (D_801E36B8 == 0) {
                func_80026A94(&D_800706A4[D_80062F58], &rect);
            } else {
                func_80026A94(&D_801E36BC[D_801E36B4], &rect);
            }
            func_80026F44(10, 11, D_801E2CFC[2], 7);
            func_80026F44(206, 11, D_801E2CFC[9], 6);
            func_80026F44(
                func_80026B70(D_801E2CFC[9]) + 208, 11,
                ((13 + menus.D_801E379C[1].unkB + menus.D_801E379C[1].unk2) *
                 36) +
                    D_801E2CFC[0],
                7);
            func_8001DE0C(&sp38, 200, 5, 78, 24);
            func_8001E040(&sp38);
            func_800269E8();
        }
        break;
    case 2:
    case 3:
        if (D_801E3850 == 2) {
            var_s2 = 64;
            var_s1 = 32;
            var_s0 = 160;
        } else {
            var_s2 = 224;
            var_s1 = 128;
            var_s0 = 0;
        }
        func_80026F44(10, 11, D_801E2CFC[12], 7);
        if (D_801E36A8 == 0) {
            func_800285AC(
                122, 117, (D_801E36AC + 1) * 8, 8, var_s2, var_s1, var_s0);
            rect.x = 0;
            rect.y = 0;
            rect.w = 0xFF;
            rect.h = 0xFF;
            func_80026A34(0, 1, 0x3F, &rect);
        }
        func_8001DE0C(&sp38, 0x70, 0x6D, 0x8C, 0x18);
        func_8001E040(&sp38);
        break;
    case 4:
        temp_s1 = func_80026B70(D_801E2CFC[7]) + 0x10;
        func_80026F44(190 - temp_s1 / 2, 115, D_801E2CFC[7], 7);
        func_8001DE0C(&sp38, 182 - temp_s1 / 2, 109, temp_s1, 24);
        func_8001E040(&sp38);
        break;
    case 6:
        if (arg0 & 2) {
            func_8001EB2C(D_801D4EC8.x - 18,
                          D_801D4EC8.y + 6 + menus.D_801E379C[0].unkB * 0xC);
        }
        func_80026F44(D_801D4EC8.x + 12, D_801D4EC8.y + 5, D_801E2CFC[3],
                      -(D_801E8F38[0][0] != 0) & 7);
        func_80026F44(D_801D4EC8.x + 12, D_801D4EC8.y + 0x11, D_801E2CFC[4],
                      -(D_801E8F38[1][0] != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        func_80026A34(0, 1, 0x7F, &rect);
        func_8001E040(&D_801D4EC8);
        func_80026F44(10, 11, D_801E3260[4], 7);
        temp_s2 = func_80026B70(D_801E3260[5]) + 0x10;
        func_80026F44(190 - temp_s2 / 2, D_801D4EC8.h + 99, D_801E3260[5], 7);
        func_80026F44(228 - temp_s2 / 2, D_801D4EC8.h + 112, D_801E2CFC[34], 7);
        func_80026F44(228 - temp_s2 / 2, D_801D4EC8.h + 124, D_801E2CFC[35], 7);
        func_8001EB2C(200 - temp_s2 / 2,
                      115 + (menus.D_801E3808[0].unkB * 12) + D_801D4EC8.h);
        func_8001DE0C(
            &sp38, 182 - temp_s2 / 2, D_801D4EC8.h + 93, temp_s2, 0x30);
        func_8001E040(&sp38);
        break;
    }
    if (D_801E36B8 != 0) {
        func_80026B5C(0x80);
        func_80026F44(294, 11, &D_801DEEDC, 7);
        func_8001E040(&D_801DEEFC);
    }
    func_8001DE0C(&sp38, 0, 5, 364, 24);
    func_8001E040(&sp38);
    if (!(D_801E36B8 == 0 && !func_80023050()) &&
        (D_801E36B8 == 0 || D_801E36B0 != 1)) {
        return;
    }
    if (func_8001F6B4() & 0xFF) {
        return;
    }
    switch (D_801E3850) {
    case 0:
        if (D_80062D7C & 0x20) {
            if (D_801E8F38[menus.D_801E379C[0].unkB][0]) {
                func_801D0408(1);
                if (D_801E8F38[menus.D_801E379C[0].unkB][2]) {
                    D_801E3850 = 6;
                    func_80026448(&menus.D_801E3808[0], 0, 1, 1, 2, 0, 0, 1, 2,
                                  0, 0, 0, 1, 0);
                } else {
                    D_801E3850 = 2;
                    D_801E36AC = 0;
                    D_801E36A0 = 0;
                    D_80062F3C = 0;
                    D_801E36A8 = 1;
                    D_801E36A4 = 0x3C;
                    func_80026448(&menus.D_801E379C[1], 0, 0, 1, 3, 0, 0, 1, 15,
                                  0, 0, 0, 0, 0);
                }
            } else {
                func_801D0408(3);
                func_8001F6C0(!D_801E3860 ? D_801E33B0[0] : D_801E3260[6], 7);
            }
        } else {
            func_800264A8(&menus.D_801E379C[0]);
            if (D_801E36B8 != 0) {
                if (D_80062D7C & 0x40) {
                    func_801D0408(4);
                    D_801E36B0 = 2;
                }
            } else if (D_80062D7C & 0x40) {
                func_801D0408(4);
                func_801D0670();
                func_8002305C(5, 0);
                func_8002120C(0);
            }
        }
        break;
    case 1:
        var_s0 = menus.D_801E379C[1].unkF;
        func_801D2DA8(&menus.D_801E379C[1]);
        if ((menus.D_801E379C[1].unkF == 0) && (var_s0 == 0)) {
            if (D_80062D7C & 0x20) {
                D_801E3850 = 7;
                func_80026448(&menus.D_801E3808[1], 0, 0, 1, 2, 0, 0, 1, 2, 0,
                              0, 0, 1, 0);
                func_801D0408(1);
            } else if (D_80062D7C & 0x40) {
                func_801D0408(4);
                D_801E3850 = 0;
            }
        }
        break;
    case 2:
        if (D_801E36A4 == 0) {
            if (D_801E36A8) {
                D_801E36A4 = 0;
                D_801E36A8 = 0;
                D_80062F3C = func_801D1C2C(menus.D_801E379C[0].unkB);
            } else {
                var_s0 = 0;
                if ((D_80062F3C >> D_801E36AC) & 1) {
                    var_s0 =
                        func_801D3698(menus.D_801E379C[0].unkB, D_801E36AC);
                }
                D_801E36AC++;
                if (var_s0) {
                    D_801E3850 = 0;
                    func_8001F6C0(D_801E33B0[8], 2);
                }
                if (D_801E36AC == 0xF) {
                    D_801E36AC = 0xE;
                    D_801E3850 = 3;
                    D_801E36A4 = 0xA;
                    func_801D0408(2);
                }
            }
        } else {
            D_801E36A4--;
        }
        break;
    case 3:
        if (D_801E36A4 == 0) {
            D_801E3850 = 1;
        }
        D_801E36A4--;
        break;
    case 4:
        if (D_801E36A4 != 0) {
            D_801E36A4--;
            return;
        }
        D_801E3850 = 1;
        var_v0_6 = menus.D_801E379C[1].unkB + menus.D_801E379C[1].unk2;
        if (menus.D_801E379C[0].unkB != 0) {
            var_v0_6 |= 0x10;
        }
        if (!func_801D2A34(var_v0_6)) {
            func_801D0408(0xD0);
            func_8001F6C0(D_801E2CFC[28], 7);
            D_80062F3C |=
                1 << (menus.D_801E379C[1].unkB + menus.D_801E379C[1].unk2);
        } else {
            func_801D0408(3);
            func_8001F6C0(D_801E33B0[3], 7);
        }
        break;
    case 6:
        func_800264A8(&menus.D_801E3808[0]);
        if (D_80062D7C & 0x20) {
            if (menus.D_801E3808[0].unkB) {
                D_801E3850 = 0;
                func_801D0408(4);
            } else {
                if (menus.D_801E379C[0].unkB) {
                    temp_v1 = format("bu10:");
                } else {
                    temp_v1 = format("bu00:");
                }
                D_801E3850 = 0;
                if (temp_v1 == 1) {
                    D_801E8F38[menus.D_801E379C[0].unkB][2] = 0;
                    func_8001F6C0(D_801E2CFC[41], 7);
                    func_801D0408(0xD0);
                } else {
                    func_8001F6C0(D_801E3260[3], 7);
                    func_801D0408(3);
                }
            }
        } else if (D_80062D7C & 0x40) {
            D_801E3850 = 0;
            func_801D0408(4);
        }
        break;
    case 7:
        if (D_80062D7C & 0x20) {
            temp_s0_2 = menus.D_801E3808[1].unkB;
            switch (menus.D_801E3808[1].unkB) {
            case 0:
                func_801D0408(1);
                D_801E3850 = 4;
                D_801E36A4 = 0xA;
                break;
            case 1:
                func_801D0408(4);
                D_801E3850 = temp_s0_2;
                break;
            }
        } else if (D_80062D7C & 0x40) {
            D_801E3850 = 1;
            func_801D0408(4);
        } else {
            func_800264A8(&menus.D_801E3808[1]);
        }
        break;
    }
}

static const char* D_801E2C78[] = {
    "BASCUS-94163FF7-S01", "BASCUS-94163FF7-S02", "BASCUS-94163FF7-S03",
    "BASCUS-94163FF7-S04", "BASCUS-94163FF7-S05", "BASCUS-94163FF7-S06",
    "BASCUS-94163FF7-S07", "BASCUS-94163FF7-S08", "BASCUS-94163FF7-S09",
    "BASCUS-94163FF7-S10", "BASCUS-94163FF7-S11", "BASCUS-94163FF7-S12",
    "BASCUS-94163FF7-S13", "BASCUS-94163FF7-S14", "BASCUS-94163FF7-S15",
};
static s32 D_801E2CB4 = 0;

s32 func_801D1774(void) {
    s32 ret;
    s32 i;

    func_80021044(D_801E36BC, D_801E3774);
    i = 0;
    D_801E36B0 = 0;
    func_801D05C0(1);
    D_801E36B4 = 0;
    while (1) {
        func_8001CB48();
        func_800269C0(D_80077F64[D_801E36B4]);
        D_801E3854 = (u_long*)D_801E3858[D_801E36B4];
        ClearOTag(D_801E3854, 1);
        func_80026A00(D_801E3854);
        func_8001F710();
        ret = func_801D06B0(i);
        if (D_801E36B0 == -1) {
            break;
        }
        DrawSync(0);
        VSync(0);
        PutDispEnv(&D_801E3774[D_801E36B4]);
        PutDrawEnv(&D_801E36BC[D_801E36B4]);
        DrawOTag(D_801E3854);
        D_801E36B4 ^= 1;
        i++;
    }
    func_801D0670();
    VSync(0);
    PutDispEnv(&D_801E3774[0]);
    PutDrawEnv(&D_801E36BC[0]);
    VSync(0);
    PutDispEnv(&D_801E3774[1]);
    PutDrawEnv(&D_801E36BC[1]);
    return ret;
}

u16 func_801D1950(u16 len, u8* data) {
    u16 i, j;
    s32 sum = 0xFFFF;
    for (i = 0; i < len; i++) {
        sum ^= *(data + i) << 8;
        for (j = 0; j < 8; j++) {
            if (sum & 0x8000) {
                sum = (sum * 2) ^ 0x1021;
            } else {
                sum *= 2;
            }
        }
    }
    return ~sum;
}

void func_801D19C4(void) {
    s32 i;

    if (D_80062DCC == 0) {
        EnterCriticalSection();
        D_8009A024[0] = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL);
        D_8009A024[1] = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL);
        D_8009A024[2] = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
        D_8009A024[3] = OpenEvent(SwCARD, EvSpNEW, EvMdNOINTR, NULL);
        D_8009A024[4] = OpenEvent(HwCARD, EvSpIOE, EvMdNOINTR, NULL);
        D_8009A024[5] = OpenEvent(HwCARD, EvSpERROR, EvMdNOINTR, NULL);
        D_8009A024[6] = OpenEvent(HwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
        D_8009A024[7] = OpenEvent(HwCARD, EvSpNEW, EvMdNOINTR, NULL);
        InitCARD(1);
        func_80048A44();
        ChangeClearPAD(0);
        _bu_init();
        _card_auto(0);
        for (i = 0; i < 8; i++) {
            EnableEvent(D_8009A024[i]);
        }
        ExitCriticalSection();
        D_80062DCC = 1;
    }
    for (i = 0; i < 2; i++) {
        D_801E8F38[i][0] = 0;
        D_801E8F38[i][1] = 0;
        D_801E8F38[i][2] = 0;
    }
}

void func_801D1BA4(void) {}

static void func_801D1BAC(s32 arg0, s32 arg1) { TestEvent(D_8009A024[arg1]); }

// strcmp?
static s32 func_801D1BE0(u8* arg0, u8* arg1) {
    while (1) {
        if (*arg0++ != *arg1++) {
            return 0;
        }
        if (!*arg0 && !*arg1) {
            return 1;
        }
    }
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/menu/nonmatchings/savemenu", func_801D1C2C);
#else
u16 func_801D1C2C(s32 arg0) {
    DIRENTRY sp10;
    const char* memcard;
    s32 i;
    DIRENTRY* entry;
    u16 var_s3;

    var_s3 = 0;
    i = 0;
    while (1) {
        memcard = &D_801D0184;
        if (arg0) {
            memcard = &D_801D017C;
        }
        entry = firstfile2(memcard, &sp10);
        if (entry) {
            break;
        }
        i++;
        if (i >= 100) {
            var_s3 = 0;
            goto end;
        }
    }
    while (entry) {
        for (i = 0; i < 15; i++) {
            if (func_801D1BE0(entry->name, D_801E2C78[i]) != 0) {
                var_s3 |= 1 << i;
            }
        }
        entry = nextfile(entry);
    }
end:
    return var_s3;
}
#endif

const char D_801D018C[] = "bu10:%s";
const char D_801D0194[] = "bu00:%s";

SaveHeader* func_801D1D1C(s32 arg0) { return &D_801E3864[arg0]; }

INCLUDE_ASM("asm/us/menu/nonmatchings/savemenu", func_801D1D40);

INCLUDE_ASM("asm/us/menu/nonmatchings/savemenu", func_801D1F40);

static s32 func_801D2150(s8 arg0) {
    if (arg0 > -0x68 && arg0 < -0x60 || arg0 > -32 && arg0 < -3) {
        return 1;
    }
    return 0;
}

s32 func_801D2184(s8 arg0) {
    if (arg0 > -0x80 && arg0 < -0x60 || arg0 > -33 && arg0 < -3) {
        return 1;
    }
    return 0;
}

static void func_801D21B8(u8* arg0, u8* arg1) {
    s32 i;
    for (i = 0; i < 0x40; i++) {
        *arg0++ = *arg1++;
    }
}

static void func_801D21E0(s32 arg0) { D_801E2CB4 = arg0; }

void func_801D21F0(u8* arg0, u8* arg1) {
    s32 i;
    for (i = 0; i < D_801E2CB4; i++) {
        *arg0++ = *arg1;
        if (*arg1 == 0xFF) {
            break;
        }
        arg1++;
    }
}

INCLUDE_ASM("asm/us/menu/nonmatchings/savemenu", func_801D224C);

INCLUDE_ASM("asm/us/menu/nonmatchings/savemenu", func_801D2408);

static const char* D_801E2CB8[] = {
    "ＦＦ７／ＳＡＶＥ０１／００：００", "ＦＦ７／ＳＡＶＥ０２／００：００",
    "ＦＦ７／ＳＡＶＥ０３／００：００", "ＦＦ７／ＳＡＶＥ０４／００：００",
    "ＦＦ７／ＳＡＶＥ０５／００：００", "ＦＦ７／ＳＡＶＥ０６／００：００",
    "ＦＦ７／ＳＡＶＥ０７／００：００", "ＦＦ７／ＳＡＶＥ０８／００：００",
    "ＦＦ７／ＳＡＶＥ０９／００：００", "ＦＦ７／ＳＡＶＥ１０／１１：１１",
    "ＦＦ７／ＳＡＶＥ１１／１１：１１", "ＦＦ７／ＳＡＶＥ１２／１１：１１",
    "ＦＦ７／ＳＡＶＥ１３／１１：１１", "ＦＦ７／ＳＡＶＥ１４／１１：１１",
    "ＦＦ７／ＳＡＶＥ１５／１１：１１",
};

static s16 func_801D2A34(s32 save_id) {
    char sp10[0x40];
    s32 ret;
    s32 slot;

    if (save_id & 0x10) {
        func_80042DD8(&sp10, D_801D018C, D_801E2C78[save_id & 15]);
    } else {
        func_80042DD8(&sp10, D_801D0194, D_801E2C78[save_id & 15]);
    }
    slot = save_id & 15;
    D_801E3D50 = slot;
    ret = func_801D2408(&sp10, D_801E2CB8[slot]);
    if (!(s16)ret) {
        memcpy(&D_801E3864[slot], &_work.header, sizeof(SaveHeader));
    }
    return ret;
}

s32 D_801E2CF4 = 0xFF;                                       // used by title.c
StartMenuMode g_MenuStartMode = START_MENU_MODE_SELECT_SLOT; // used by title.c

unsigned char D_801E2CFC[][0x24] = {
    _S("Load"),
    _S("Select a slot."),
    _S("Select a file."),
    _S("SLOT 1"),
    _S("SLOT 2"),
    _S("Are you sure?"),
    _S("Loading. Do not remove Memory card."),
    _S("Saving. Do not remove Memory card."),
    _S("EMPTY"),
    _S("FILE"),
    _S("Continue?"),
    _S("/15"),
    _S("Checking Memory card."),
    _S("01"),
    _S("02"),
    _S("03"),
    _S("04"),
    _S("05"),
    _S("06"),
    _S("07"),
    _S("08"),
    _S("09"),
    _S("10"),
    _S("11"),
    _S("12"),
    _S("13"),
    _S("14"),
    _S("15"),
    _S("Saved."),
    _S("Could not save."),
    _S("Could not load."),
    _S("File is ruined."),
    _S("NEW GAME"),
    _S("Are you sure you want to save?"),
    _S("Yes"),
    _S("No"),
    _S(""),
    _S("Completed."),
};
static u32 _padding[] = {0, 0, 0};
unsigned char D_801E3260[][0x30] = {
    _S(""),
    _S(""),
    _S("Formatted."),
    _S("Could not format."),
    _S("Not formatted."),
    _S("Want to format it now?"),
    _S("No Memory card."),
};
unsigned char D_801E33B0[][0x30] = {
    _S("No Memory card."),
    _S("This Memory card is damaged and cannot be used."),
    _S("Please insert another Memory card."),
    _S("No enough memory left on Memory card."),
    _S("Use another Memory card,"),
    _S("or erase 1 block of saved data."),
    _S(""),
    _S(""),
    _S("Couldn't read it."),
    _S("Still want to begin the game?"),
    _S("‘’"),
    _S("‘’"),
    _S("‘’"),
    _S(""),
};
