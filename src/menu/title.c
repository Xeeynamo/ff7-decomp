//! PSYQ=3.3 CC1=2.7.2
#include "savemenu.h"

extern Unk80026448 D_801E3DEC[2];

void func_801D39C4(void) {
    D_801E3698 = 0;
    D_801E2CF8 = 7;
    func_8001DEF0(D_801E368C);
    func_80025D14(D_801D4EDC, 0x380, 0, 0, 0x1E0);
    DrawSync(0);
    func_80026448(D_801E3DFE, 0, 1, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
    func_80025CD4(D_801E3F2C);
    func_80025B8C(D_801E8F44);
    func_80025C14(D_801E4538);
    func_80025DF8();
    func_801D19C4();
}

s32 func_801D3AB0(s32 arg0) {
    RECT sp38;
    RECT rect;
    s32 temp_v1_2;
    s32 var_a0_3;
    s32 var_s0;
    s32 var_s0_2;
    s32 temp_s1;
    s32 var_s1;
    s32 var_s2;
    s32 temp_s2;
    s32 var_s3;

    if ((D_801E2CF8 < 2 || D_801E2CF8 == 7) && D_801E3D54 != 2 &&
        D_801E3D54 != 0) {
        func_801D3668(arg0);
    }
    func_80026B5C(0x80);
    if (D_801E3D54 == 0) {
        if (func_801D2B98(-15) == 0) {
            D_801E3D54 = 1;
        }
    } else if (D_801E3D54 == 2) {
        if (func_801D2B98(15) == 0xFF) {
            D_801E3D54 = -1;
        }
    }
    func_8001F6B4();
    switch (D_801E2CF8) {
    case 0:
        func_8001EB2C(
            D_801E3668.x - 18, D_801E3668.y + 6 + D_801E3D80[0].unkB * 12);
        func_80026F44(10, 11, D_801E2CFC[1], 7);
        func_80026F44(D_801E3668.x + 12, D_801E3668.y + 5, D_801E2CFC[3],
                      -(D_801E8F38[0][0] != 0) & 7);
        func_80026F44(D_801E3668.x + 12, D_801E3668.y + 0x11, D_801E2CFC[4],
                      -(D_801E8F3B != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        func_80026A34(0, 1, 0x7F, &rect);
        func_8001E040(&D_801E3668);
        break;
    case 1:
        if (!D_801E8F38[D_801E3D80[0].unkB][0]) {
            D_801E2CF8 = 0;
        } else {
            func_800269D0();
            func_800269C0(D_801E3D58 * 0x5000 + D_801D4EDC);
            func_8001EB2C(8, (D_801E3D80[1].unkB * 64) | 0x38);
            var_s3 = !D_801E3D80[1].unk8 ? 3 : 4;
            for (var_s0 = 0; var_s0 < var_s3; var_s0++) {
                if ((D_80062F3C >> (var_s0 + D_801E3D80[1].unk2)) & 1) {
                    func_8001DE70();
                    func_801D370C(
                        0, var_s0 * 64 + 0x1D + D_801E3D80[1].unkF * 8,
                        var_s0 + D_801E3D80[1].unk2);
                    func_8001DEB0();
                } else {
                    func_80026F44(
                        0x32, var_s0 * 64 + 55 + D_801E3D80[1].unkF * 8,
                        D_801E2CFC[8], 6);
                    func_8001DE40(&sp38, &D_801E3660);
                    func_8001DE24(
                        &sp38, 0, var_s0 * 64 + 0x1D + D_801E3D80[1].unkF * 8);
                    func_8001E040(&sp38);
                }
            }
            func_80026B5C(0x80);
            rect.y = 0x1D;
            rect.w = 0x16C;
            rect.h = 0xC3;
            rect.x = 0;
            func_80026A94(&D_801E3E34[D_801E3D58], &rect);
            func_80026F44(10, 11, D_801E2CFC[2], 7);
            func_80026F44(0xCE, 11, D_801E2CFC[9], 6);
            func_80026F44(
                func_80026B70(D_801E2CFC[9]) + 0xD0, 11,
                ((13 + D_801E3D80[1].unkB + D_801E3D80[1].unk2) * 36) +
                    (D_801E2CFC[0]),
                7);
            func_8001DE0C(&sp38, 200, 5, 0x4E, 0x18);
            func_8001E040(&sp38);
            func_800269E8();
        }
        break;
    case 2:
    case 3:
        if (D_801E2CF8 == 2) {
            var_s2 = 0x40;
            var_s1 = 0x20;
            var_s0_2 = 0xA0;
        } else {
            var_s2 = 0xE0;
            var_s1 = 0x80;
            var_s0_2 = 0;
        }
        func_80026F44(10, 11, D_801E2CFC[12], 7);
        if (D_801E3F1C == 0) {
            func_800285AC(
                122, 117, (D_801E3F20 + 1) * 8, 8, var_s2, var_s1, var_s0_2);
            rect.x = 0;
            rect.y = 0;
            rect.w = 0xFF;
            rect.h = 0xFF;
            func_80026A34(0, 1, 0x3F, &rect);
        }
        func_8001DE0C(&sp38, 112, 0x6D, 0x8C, 0x18);
        func_8001E040(&sp38);
        break;
    case 4:
        if (D_801E3D54 != 2) {
            temp_s1 = func_80026B70(D_801E2CFC[6]) + 0x10;
            func_80026F44(190 - temp_s1 / 2, 0x73, D_801E2CFC[6], 7);
            func_8001DE0C(&sp38, 0xB6 - temp_s1 / 2, 0x6D, temp_s1, 24);
            func_8001E040(&sp38);
        }
        break;
    case 6:
        if (arg0 & 2) {
            func_8001EB2C(D_801E3668.x - 0x12,
                          D_801E3668.y + 6 + D_801E3D80[0].unkB * 12);
        }
        func_80026F44(D_801E3668.x + 12, D_801E3668.y + 5, D_801E2CFC[3],
                      -(D_801E8F38[0][0] != 0) & 7);
        func_80026F44(D_801E3668.x + 12, D_801E3668.y + 0x11, D_801E2CFC[4],
                      -(D_801E8F3B != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        func_80026A34(0, 1, 0x7F, &rect);
        func_8001E040(&D_801E3668.x);
        func_80026F44(10, 11, D_801E3260[4], 7);
        temp_s2 = func_80026B70(D_801E3260[5]) + 0x10;
        func_80026F44(190 - temp_s2 / 2, D_801E3668.h + 99, D_801E3260[5], 7);
        func_80026F44(228 - temp_s2 / 2, D_801E3668.h + 112, D_801E2CFC[34], 7);
        func_80026F44(228 - temp_s2 / 2, D_801E3668.h + 124, D_801E2CFC[35], 7);
        func_8001EB2C(
            200 - temp_s2 / 2, 0x73 + D_801E3DEC[0].unkB * 12 + D_801E3668.h);
        func_8001DE0C(
            &sp38, 0xB6 - temp_s2 / 2, D_801E3668.h + 0x5D, temp_s2, 0x30);
        func_8001E040(&sp38);
        break;
    case 7:
        func_8001EB2C(
            D_801E3668.x - 0x12, D_801E3668.y + 6 + D_801E3DEC[1].unkB * 12);
        func_80026F44(
            D_801E3668.x + 8, D_801E3668.y + 6, D_801E2CFC[32], 7); // new game
        func_80026F44(
            D_801E3668.x + 8, D_801E3668.y + 18, D_801E2CFC[10],
            D_801E8F38[0][0] || D_801E8F38[1][0] ? 7 : 0); // continue?
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        func_80026A34(0, 1, 0x7F, &rect);
        SetPolyFT4(D_80062F24.ft4);
        D_80062F24.ft4->r0 = 0x60;
        D_80062F24.ft4->g0 = 0x60;
        D_80062F24.ft4->b0 = 0x60;
        D_80062F24.ft4->x0 = 0x6F;
        D_80062F24.ft4->y0 = 0x54;
        D_80062F24.ft4->x1 = 0x119;
        D_80062F24.ft4->y1 = 0x54;
        D_80062F24.ft4->x2 = 0x6F;
        D_80062F24.ft4->y2 = 0xEA;
        D_80062F24.ft4->x3 = 0x119;
        D_80062F24.ft4->y3 = 0xEA;
        D_80062F24.ft4->u0 = 0;
        D_80062F24.ft4->v0 = 0;
        D_80062F24.ft4->u1 = 0x96;
        D_80062F24.ft4->v1 = 0;
        D_80062F24.ft4->u2 = 0;
        D_80062F24.ft4->v2 = 0x96;
        D_80062F24.ft4->u3 = 0x96;
        D_80062F24.ft4->v3 = 0x96;
        D_80062F24.ft4->clut = GetClut(0, 0x1E0);
        D_80062F24.ft4->tpage = GetTPage(1, 0, 0x380, 0);
        AddPrim(D_80062FC4, D_80062F24.ft4);
        D_80062F24.ft4++;
        break;
    }
    if (D_801E2CF8 != 7) {
        func_80026F44(0x126, 11, D_801E2CFC[0], 7);
        func_8001DE0C(&sp38, 0x116, 5, 0x56, 0x18);
        func_8001E040(&sp38);
        func_8001DE0C(&sp38, 0, 5, 0x16C, 0x18);
        func_8001E040(&sp38);
    }
    if (!(func_8001F6B4() & 0xFF) && D_801E3D54 == 1) {
        switch (D_801E2CF8) {
        case 0:
            if (D_80062D7C & 0x20) {
                temp_v1_2 = D_801E3D80[0].unkB;
                if (temp_v1_2 >= 2) {
                    break;
                }
                if (temp_v1_2 < 0) {
                    break;
                }
                if (D_801E8F38[temp_v1_2][0]) {
                    func_801D2B58(1);
                    if (D_801E8F38[D_801E3D80[0].unkB][2]) {
                        D_801E2CF8 = 6;
                        func_80026448(&D_801E3D80[6], 0, 1, 1, 2, 0, 0, 1, 2, 0,
                                      0, 0, 1, 0);
                    } else {
                        D_801E3F18 = 10;
                        D_801E2CF8 = 2;
                        D_801E3F20 = 0;
                        D_801E3F14 = 0;
                        D_80062F3C = 0;
                        D_801E3F1C = 1;
                        func_80026448(&D_801E3D80[1], 0, 0, 1, 3, 0, 0, 1, 15,
                                      0, 0, 0, 0, 0);
                    }
                } else {
                    func_801D2B58(3);
                    func_8001F6C0(D_801E33B0, 7);
                }
            } else if (D_80062D7C & 0x40) {
                func_801D2B58(4);
                D_801E2CF8 = 7;
            } else {
                func_800264A8(&D_801E3D80[0]);
            }
            break;
        case 1:
            var_s1 = D_801E3D80[1].unkF;
            func_801D2DA8(&D_801E3D80[1]);
            if (!D_801E3D80[1].unkF && !var_s1) {
                if ((u16)D_80062D7C & 0x20) {
                    if (((s32)D_80062F3C >>
                         (D_801E3D80[1].unkB + D_801E3D80[1].unk2)) &
                        1) {
                        func_801D2B58(1);
                        D_801E2CF8 = 4;
                        D_801E3F18 = 10;
                    } else {
                        func_801D2B58(3);
                    }
                } else if ((u16)D_80062D7C & 0x40) {
                    func_801D2B58(4);
                    D_801E2CF8 = 0;
                }
            }
            break;
        case 2:
            if (D_801E3F18 == 0) {
                if (D_801E3F1C) {
                    D_801E3F18 = 0;
                    D_801E3F1C = 0;
                    D_80062F3C = func_801D1C2C(D_801E3D80[0].unkB);
                } else {
                    var_s1 = 0;
                    if ((D_80062F3C >> D_801E3F20) & 1) {
                        var_s1 = func_801D3698(D_801E3D80[0].unkB, D_801E3F20);
                    }
                    D_801E3F20++;
                    if (var_s1) {
                        D_801E2CF8 = 0;
                        func_8001F6C0(D_801E33B0[8], 2);
                        func_801D2B58(3);
                    }
                    if (D_801E3F20 == 0xF) {
                        D_801E3F20 = 0xE;
                        D_801E2CF8 = 3;
                        D_801E3F18 = 10;
                        func_801D2B58(2);
                    }
                }
            } else {
                D_801E3F18--;
            }
            break;
        case 3:
            if (D_801E3F18 == 0) {
                D_801E2CF8 = 1;
            }
            D_801E3F18--;
            break;
        case 4:
            if (D_801E3F18 != 0) {
                D_801E3F18--;
                break;
            }
            D_80062D99 = 1;
            var_a0_3 = D_801E3D80[1].unkB + D_801E3D80[1].unk2;
            if (D_801E3D80[0].unkB) {
                var_a0_3 |= 0x10;
            }
            var_s1 = (s16)func_801D1F40(var_a0_3);
            if (var_s1 == 0) {
                if (_work.header.checksum !=
                    (u16)func_801D1950(
                        sizeof(SaveWork) - 4, &_work.header.leader_level)) {
                    D_801E2CF8 = 1;
                    func_801D2B58(3);
                    func_8001F6C0(D_801E2CFC[31], 0);
                } else {
                    func_801D2B58(0xD0);
                    D_801E3D54 = 2;
                    func_801D2D10(D_8009D7BE & 3);
                }
            } else {
                D_801E2CF8 = 1;
                func_801D2B58(3);
                func_8001F6C0(D_801E2CFC[11], var_s1);
            }
            D_80062D99 = 0;
            break;
        case 6:
            func_800264A8(&D_801E3DEC[0]);
            if (D_80062D7C & 0x20) {
                if (D_801E3DEC[0].unkB == 0) {
                    if (D_801E3D80[0].unkB) {
                        temp_v1_2 = format("bu10:");
                    } else {
                        temp_v1_2 = format("bu00:");
                    }
                    D_801E2CF8 = 0;
                    if (temp_v1_2 == 1) {
                        D_801E8F38[D_801E3D80[0].unkB][2] = 0;
                        func_8001F6C0(D_801E2CFC[41], 7);
                        func_801D2B58(0xD0);
                    } else {
                        func_8001F6C0(D_801E3260[3], 7);
                        func_801D2B58(3);
                    }
                } else {
                    D_801E2CF8 = 0;
                    func_801D2B58(4);
                }
            } else if (D_80062D7C & 0x40) {
                D_801E2CF8 = 0;
                func_801D2B58(4);
            }
            break;
        case 7:
            if (D_80062D7C & 0x20) {
                switch (D_801E3D80[7].unkB) {
                case 0:
                    func_801D2B58(0xD0);
                    D_801E3698 = 1;
                    D_801E3D54 = 2;
                    break;
                case 1:
                    if (D_801E8F38[0][0] || D_801E8F3B) {
                        func_801D2B58(1);
                        func_80026448(&D_801E3D80[0], 0, 0, 1, 2, 0, 0, 1, 2, 0,
                                      0, 0, 1, 0);
                        D_801E2CF8 = 0;
                    } else {
                        func_801D2B58(3);
                    }
                    break;
                }
            } else {
                func_800264A8(D_801E3DFE);
            }
            break;
        }
    }
    return D_801E3698;
}

void func_801D4C38(void) {
    func_80025ED4();
    func_80025CD4(D_800756F8);
    func_80025C94(D_801E3F2C);
    func_80025BD0(D_801E8F44);
    func_80025C54(D_801E4538);
    func_801D1BA4();
    PutDispEnv(D_801E3EEC);
    PutDrawEnv(D_801E3E34);
}

// title screen loop
s32 func_801D4CC0(void) {
    s32 i;
    s32 ret;

    func_80021044(D_801E3E34, D_801E3EEC);
    D_801E3D54 = 0;
    func_801D39C4();
    D_801E3D58 = 0;
    for (i = 0;; i++) {
        func_8001CB48();
        func_800269C0(D_80077F64[D_801E3D58]);
        D_801E3D5C = (u_long*)D_801E3D60[D_801E3D58];
        ClearOTag(D_801E3D5C, 1);
        func_80026A00(D_801E3D5C);
        func_8001F710();
        ret = func_801D3AB0(i);
        if (D_801E3D54 == -1) {
            break;
        }
        DrawSync(0);
        VSync(0);
        PutDispEnv(&D_801E3EEC[D_801E3D58]);
        PutDrawEnv(&D_801E3E34[D_801E3D58]);
        DrawOTag(D_801E3D5C);
        D_801E3D58 ^= 1; // flip back buffer ID?
    }
    func_801D4C38();
    VSync(0);
    PutDispEnv(&D_801E3EEC[1]);
    PutDrawEnv(&D_801E3E34[1]);
    for (i = 0; i < 3; i++) {
        if (D_8009CBDC[i] != 0xFF) {
            func_80020058(i);
            func_8001786C((u8)i);
        }
    }
    func_80017678();
    return ret;
}
