//! PSYQ=3.3 CC1=2.7.2
#include <psxsdk/libapi.h>
#include <psxsdk/libgpu.h>
#include <psxsdk/kernel.h>

#include "savemenu.h"
#include <psxsdk/libetc.h>

static void func_801D0408(u16 arg0) {
    D_8009A000[0] = 0x30;
    D_8009A004[0] = arg0;
    D_8009A008[0] = arg0;
    SystemAkaoExecute();
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
    SysMenuSetDrawMode(0, 1, 0x5F, &rect);
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
    SysMenuSetCursorMovement(menus.D_801E379C, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 0, 1, 0);
    func_80025B8C(&D_801E8F44);
    func_80025C14(&D_801E4538);
    SysMenuLoadAvatars();
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
        SysMenuDrawMenuList(g_MenuRenderBufferIndex);
    } else if (D_801E36B0 == 0) {
        if (func_801D0448(-15) == 0) {
            D_801E36B0 = 1;
        }
    } else if (D_801E36B0 == 2) {
        if (func_801D0448(15) == 0xFF) {
            D_801E36B0 = -1;
        }
    }
    if (!SysMenuGetMenuListState() || (D_801E36B8 && D_801E36B0 == 1)) {
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
        SysMenuDrawCursor(D_801D4EC8.x - 18,
                      D_801D4EC8.y + 6 + (menus.D_801E379C[0].row * 12));
        SysMenuDrawString(0xA, 0xB, D_801E2CFC[1], 7);
        SysMenuDrawString(D_801D4EC8.x + 12, D_801D4EC8.y + 5, D_801E2CFC[3],
                      -(D_801E8F38[0][0] != 0) & 7);
        SysMenuDrawString(D_801D4EC8.x + 12, D_801D4EC8.y + 17, D_801E2CFC[4],
                      -(D_801E8F3B != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SysMenuDrawWindow(&D_801D4EC8);
        break;
    case 7:
        SysMenuDrawCursor(D_801D4ED0.x + 0x16,
                      0x15 + D_801D4ED0.y + menus.D_801E3808[1].row * 0xC);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SysMenuDrawString(D_801D4ED0.x + 0xA, D_801D4ED0.y + 6, D_801E2CFC[33], 7);
        SysMenuDrawString(D_801D4ED0.x + 48, D_801D4ED0.y + 19, D_801E2CFC[34], 7);
        SysMenuDrawString(D_801D4ED0.x + 48, D_801D4ED0.y + 31, D_801E2CFC[35], 7);
        SysMenuDrawWindow(&D_801D4ED0);
        /* fallthrough */
    case 1:
        if (!D_801E8F38[menus.D_801E379C[0].row][0]) {
            D_801E3850 = 0;
        } else {
            func_800269D0();
            if (D_801E36B8 == 0) {
                SysMenuSetPoly(g_MenuRenderBufferIndex * 0x5000 + D_801D4EDC);
            } else {
                SysMenuSetPoly(D_801E36B4 * 0x5000 + D_801D4EDC);
            }
            if (D_801E3850 != 7 || (arg0 & 2)) {
                SysMenuDrawCursor(8, (menus.D_801E379C[1].row << 6) | 0x38);
            }
            var_s3 = !menus.D_801E379C[1].scrolling ? 3 : 4;
            for (var_s0 = 0; var_s0 < var_s3; var_s0++) {
                if ((D_80062F3C >> (var_s0 + menus.D_801E379C[1].rowOffset)) &
                    1) {
                    SysMenuStoreWindowColor();
                    func_801D370C(
                        0, var_s0 * 64 + 29 + menus.D_801E379C[1].unkF * 8,
                        var_s0 + menus.D_801E379C[1].rowOffset);
                    SysMenuRestoreWindowColor();
                } else {
                    SysMenuDrawString(
                        0x32, var_s0 * 64 + 55 + menus.D_801E379C[1].unkF * 8,
                        D_801E2CFC[8], 6);
                    SysMenuCopyWindowRect(&sp38, &D_801DEEF4);
                    SysMenuMoveWindowRect(
                        &sp38, 0,
                        var_s0 * 64 + 29 + menus.D_801E379C[1].unkF * 8);
                    SysMenuDrawWindow(&sp38);
                }
            }
            func_80026B5C(0x80);
            rect.y = 29;
            rect.w = 364;
            rect.x = 0;
            rect.h = 195;
            if (D_801E36B8 == 0) {
                SysMenuSetDrawenv(&D_800706A4[g_MenuRenderBufferIndex], &rect);
            } else {
                SysMenuSetDrawenv(&D_801E36BC[D_801E36B4], &rect);
            }
            SysMenuDrawString(10, 11, D_801E2CFC[2], 7);
            SysMenuDrawString(206, 11, D_801E2CFC[9], 6);
            SysMenuDrawString(SysGetSingleStringWidth(D_801E2CFC[9]) + 208, 11,
                          ((13 + menus.D_801E379C[1].row +
                            menus.D_801E379C[1].rowOffset) *
                           36) +
                              D_801E2CFC[0],
                          7);
            SysMenuSetWindowRect(&sp38, 200, 5, 78, 24);
            SysMenuDrawWindow(&sp38);
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
        SysMenuDrawString(10, 11, D_801E2CFC[12], 7);
        if (D_801E36A8 == 0) {
            SysMenuDrawProgressBar(
                122, 117, (D_801E36AC + 1) * 8, 8, var_s2, var_s1, var_s0);
            rect.x = 0;
            rect.y = 0;
            rect.w = 0xFF;
            rect.h = 0xFF;
            SysMenuSetDrawMode(0, 1, 0x3F, &rect);
        }
        SysMenuSetWindowRect(&sp38, 0x70, 0x6D, 0x8C, 0x18);
        SysMenuDrawWindow(&sp38);
        break;
    case 4:
        temp_s1 = SysGetSingleStringWidth(D_801E2CFC[7]) + 0x10;
        SysMenuDrawString(190 - temp_s1 / 2, 115, D_801E2CFC[7], 7);
        SysMenuSetWindowRect(&sp38, 182 - temp_s1 / 2, 109, temp_s1, 24);
        SysMenuDrawWindow(&sp38);
        break;
    case 6:
        if (arg0 & 2) {
            SysMenuDrawCursor(D_801D4EC8.x - 18,
                          D_801D4EC8.y + 6 + menus.D_801E379C[0].row * 0xC);
        }
        SysMenuDrawString(D_801D4EC8.x + 12, D_801D4EC8.y + 5, D_801E2CFC[3],
                      -(D_801E8F38[0][0] != 0) & 7);
        SysMenuDrawString(D_801D4EC8.x + 12, D_801D4EC8.y + 0x11, D_801E2CFC[4],
                      -(D_801E8F38[1][0] != 0) & 7);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x100;
        rect.h = 0x100;
        SysMenuSetDrawMode(0, 1, 0x7F, &rect);
        SysMenuDrawWindow(&D_801D4EC8);
        SysMenuDrawString(10, 11, D_801E3260[4], 7);
        temp_s2 = SysGetSingleStringWidth(D_801E3260[5]) + 0x10;
        SysMenuDrawString(190 - temp_s2 / 2, D_801D4EC8.h + 99, D_801E3260[5], 7);
        SysMenuDrawString(228 - temp_s2 / 2, D_801D4EC8.h + 112, D_801E2CFC[34], 7);
        SysMenuDrawString(228 - temp_s2 / 2, D_801D4EC8.h + 124, D_801E2CFC[35], 7);
        SysMenuDrawCursor(200 - temp_s2 / 2,
                      115 + (menus.D_801E3808[0].row * 12) + D_801D4EC8.h);
        SysMenuSetWindowRect(
            &sp38, 182 - temp_s2 / 2, D_801D4EC8.h + 93, temp_s2, 0x30);
        SysMenuDrawWindow(&sp38);
        break;
    }
    if (D_801E36B8 != 0) {
        func_80026B5C(0x80);
        SysMenuDrawString(294, 11, &D_801DEEDC, 7);
        SysMenuDrawWindow(&D_801DEEFC);
    }
    SysMenuSetWindowRect(&sp38, 0, 5, 364, 24);
    SysMenuDrawWindow(&sp38);
    if (!(D_801E36B8 == 0 && !SysMenuGetMenuListState()) &&
        (D_801E36B8 == 0 || D_801E36B0 != 1)) {
        return;
    }
    if (func_8001F6B4() & 0xFF) {
        return;
    }
    switch (D_801E3850) {
    case 0:
        if (g_Pad1KeysPressed & PADRright) {
            if (D_801E8F38[menus.D_801E379C[0].row][0]) {
                func_801D0408(1);
                if (D_801E8F38[menus.D_801E379C[0].row][2]) {
                    D_801E3850 = 6;
                    SysMenuSetCursorMovement(&menus.D_801E3808[0], 0, 1, 1, 2, 0, 0, 1, 2,
                                  0, 0, 0, 1, 0);
                } else {
                    D_801E3850 = 2;
                    D_801E36AC = 0;
                    D_801E36A0 = 0;
                    D_80062F3C = 0;
                    D_801E36A8 = 1;
                    D_801E36A4 = 0x3C;
                    SysMenuSetCursorMovement(&menus.D_801E379C[1], 0, 0, 1, 3, 0, 0, 1, 15,
                                  0, 0, 0, 0, 0);
                }
            } else {
                func_801D0408(3);
                SysMenuRequestAddWindow(!D_801E3860 ? D_801E33B0[0] : D_801E3260[6], 7);
            }
        } else {
            SysMenuHandleButtons(&menus.D_801E379C[0]);
            if (D_801E36B8 != 0) {
                if (g_Pad1KeysPressed & PADRdown) {
                    func_801D0408(4);
                    D_801E36B0 = 2;
                }
            } else if (g_Pad1KeysPressed & PADRdown) {
                func_801D0408(4);
                func_801D0670();
                SysMenuSetMenuListAnimation(5, 0);
                SysMenuLoadMenuFileById(0);
            }
        }
        break;
    case 1:
        var_s0 = menus.D_801E379C[1].unkF;
        func_801D2DA8(&menus.D_801E379C[1]);
        if ((menus.D_801E379C[1].unkF == 0) && (var_s0 == 0)) {
            if (g_Pad1KeysPressed & PADRright) {
                D_801E3850 = 7;
                SysMenuSetCursorMovement(&menus.D_801E3808[1], 0, 0, 1, 2, 0, 0, 1, 2, 0,
                              0, 0, 1, 0);
                func_801D0408(1);
            } else if (g_Pad1KeysPressed & PADRdown) {
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
                D_80062F3C = GetSaveSlotMask(menus.D_801E379C[0].row);
            } else {
                var_s0 = 0;
                if ((D_80062F3C >> D_801E36AC) & 1) {
                    var_s0 = func_801D3698(menus.D_801E379C[0].row, D_801E36AC);
                }
                D_801E36AC++;
                if (var_s0) {
                    D_801E3850 = 0;
                    SysMenuRequestAddWindow(D_801E33B0[8], 2);
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
        var_v0_6 = menus.D_801E379C[1].row + menus.D_801E379C[1].rowOffset;
        if (menus.D_801E379C[0].row != 0) {
            var_v0_6 |= 0x10;
        }
        if (!func_801D2A34(var_v0_6)) {
            func_801D0408(0xD0);
            SysMenuRequestAddWindow(D_801E2CFC[28], 7);
            D_80062F3C |=
                1 << (menus.D_801E379C[1].row + menus.D_801E379C[1].rowOffset);
        } else {
            func_801D0408(3);
            SysMenuRequestAddWindow(D_801E33B0[3], 7);
        }
        break;
    case 6:
        SysMenuHandleButtons(&menus.D_801E3808[0]);
        if (g_Pad1KeysPressed & PADRright) {
            if (menus.D_801E3808[0].row) {
                D_801E3850 = 0;
                func_801D0408(4);
            } else {
                if (menus.D_801E379C[0].row) {
                    temp_v1 = format("bu10:");
                } else {
                    temp_v1 = format("bu00:");
                }
                D_801E3850 = 0;
                if (temp_v1 == 1) {
                    D_801E8F38[menus.D_801E379C[0].row][2] = 0;
                    SysMenuRequestAddWindow(D_801E2CFC[41], 7);
                    func_801D0408(0xD0);
                } else {
                    SysMenuRequestAddWindow(D_801E3260[3], 7);
                    func_801D0408(3);
                }
            }
        } else if (g_Pad1KeysPressed & PADRdown) {
            D_801E3850 = 0;
            func_801D0408(4);
        }
        break;
    case 7:
        if (g_Pad1KeysPressed & PADRright) {
            temp_s0_2 = menus.D_801E3808[1].row;
            switch (menus.D_801E3808[1].row) {
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
        } else if (g_Pad1KeysPressed & PADRdown) {
            D_801E3850 = 1;
            func_801D0408(4);
        } else {
            SysMenuHandleButtons(&menus.D_801E3808[1]);
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

    SysMenuCreateDrawenvDispenv(D_801E36BC, D_801E3774);
    i = 0;
    D_801E36B0 = 0;
    func_801D05C0(1);
    D_801E36B4 = 0;
    while (1) {
        InputUpdateKeyStates();
        SysMenuSetPoly(D_80077F64[D_801E36B4]);
        D_801E3854 = (u_long*)D_801E3858[D_801E36B4];
        ClearOTag(D_801E3854, 1);
        SysMenuSetOtag(D_801E3854);
        SysMenuDrawAddWindow();
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
        StartCARD();
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

// "bu10:" is memory card slot 2, "bu00:" slot 1; the wildcard lists every
// file on the card.
static const char SaveSearchCard2[] = "bu10:*"; // D_801D017C
static const char SaveSearchCard1[] = "bu00:*"; // D_801D0184

// Returns a bitmask of which of the 15 FF7 save slots exist on the memory card
// in `cardSlot`, bit N set when BASCUS-94163FF7-S(N+1) is present. Retries the
// directory open up to 100 times, and gives up with an empty mask.
u16 GetSaveSlotMask(s32 cardSlot) {
    struct DIRENTRY dirEntry;
    s32 i;
    struct DIRENTRY* entry;
    u16 slotMask;

    slotMask = 0;
    for (i = 0; i < 100; i++) {
        if (cardSlot) {
            entry = firstfile((char*)SaveSearchCard2, &dirEntry);
        } else {
            entry = firstfile((char*)SaveSearchCard1, &dirEntry);
        }
        if (entry) {
            break;
        }
    }

    if (entry) {
        while (entry) {
            for (i = 0; i < 15; i++) {
                if (func_801D1BE0(entry->name, (u8*)D_801E2C78[i]) != 0) {
                    slotMask |= 1 << i;
                }
            }
            entry = nextfile(entry);
        }
    } else {
        slotMask = 0;
    }
    return slotMask;
}

static const char D_801D018C[] = "bu10:%s";
static const char D_801D0194[] = "bu00:%s";

SaveHeader* func_801D1D1C(s32 arg0) { return &D_801E3864[arg0]; }

s32 LoadSaveHeader(s32 save_id) {
    // Declared but never used, like the one GetSaveSlotMask passes to
    // firstfile(); it still occupies its stack slot.
    struct DIRENTRY dirEntry;
    char path[0x20];
    s32 fd;
    s32 readCount;
    s32 readRetry;
    u8* headerSrc;
    u8* headerDst;
    SaveHeader* headers;
    s32 slot;

    // Only the header page is needed to build the slot preview.
    g_SaveWriteRemaining = 0x280;
    if (save_id & 0x10) {
        sprintf(path, D_801D018C, D_801E2C78[save_id & 15]);
    } else {
        sprintf(path, D_801D0194, D_801E2C78[save_id & 15]);
    }
    fd = open(path, 1);
    if (fd == -1) {
        return 1;
    }
    readRetry = 0x1E;
    do {
        readCount = read(fd, &g_SaveFile, g_SaveWriteRemaining);
        if (readCount == g_SaveWriteRemaining) {
            goto read_ok;
        }
        readRetry--;
        if (readCount != -1) {
            g_SaveWriteRemaining -= readCount;
        }
    } while (readRetry != 0);
    close(fd);
    return 2;
read_ok:
    close(fd);
    slot = save_id & 15;
    headers = D_801E3864;
    headerDst = (u8*)&headers[slot];
    headerSrc = g_SaveFileData;
    memcpy(headerDst, headerSrc, sizeof(SaveHeader));
    return 0;
}

s32 LoadSaveFile(s32 save_id) {
    // Declared but never used, like the one GetSaveSlotMask passes to
    // firstfile(); it still occupies its stack slot.
    struct DIRENTRY dirEntry;
    char path[0x20];
    s32 fd;
    s32 readCount;
    s32 readRetry;
    s32 i;
    u8* saveSrc;
    u8* saveDst;

    // readCount doubles as a scratch copy of save_id here; the overlay
    // stops matching if either reuse below is given its own variable.
    readCount = save_id;
    g_SaveWriteRemaining = sizeof(MemcardSaveFile);
    if (readCount & 0x10) {
        fd = readCount;
        sprintf(path, D_801D018C, D_801E2C78[fd & 15]);
    } else {
        sprintf(path, D_801D0194, D_801E2C78[readCount & 15]);
    }
    fd = open(path, 1);
    if (fd == -1) {
        return 1;
    }
    readRetry = 0x1E;
    do {
        readCount = read(fd, &g_SaveFile, g_SaveWriteRemaining);
        if (readCount == g_SaveWriteRemaining) {
            goto read_ok;
        }
        // Counts up from 0x1E, so a persistent error spins until it
        // wraps rather than giving up after 30 tries. LoadSaveHeader
        // decrements here; this looks like a bug in the original.
        readRetry++;
        if (readCount != -1) {
            g_SaveWriteRemaining -= readCount;
        }
    } while (readRetry != 0);
    close(fd);
    return 2;
read_ok:
    close(fd);
    saveDst = (u8*)&Savemap;
    saveSrc = g_SaveFileData;
    memcpy(saveDst, saveSrc, sizeof(SaveWork));
    for (i = 0; i < 12; i++) {
        g_MenuColors[i] = Savemap.header.menu_color[i];
    }
    return 0;
}

static s32 func_801D2150(s8 arg0) {
    if (arg0 > -0x68 && arg0 < -0x60 || arg0 > -32 && arg0 < -3) {
        return 1;
    }
    return 0;
}

static s32 func_801D2184(s8 arg0) {
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

static void func_801D21F0(u8* arg0, u8* arg1) {
    s32 i;
    for (i = 0; i < D_801E2CB4; i++) {
        *arg0++ = *arg1;
        if (*arg1 == 0xFF) {
            break;
        }
        arg1++;
    }
}

static void UpdateSaveHeader(void) {
    s32 i;
    u8 id;

    for (i = 0; i < 3; i++) {
        Savemap.header.party_portraits[i] = Savemap.partyID[i];
    }
    func_801D21E0(0x10);
    for (i = 0; i < 3; i++) {
        id = Savemap.partyID[i];
        if (id != 0xFF) {
            func_801D21F0(Savemap.header.leader_name, Savemap.party[id].name);
            Savemap.header.leader_level = Savemap.party[id].level;
            Savemap.header.leader_hp = g_ActiveCharacters[i].hp;
            Savemap.header.leader_hp_max = g_ActiveCharacters[i].baseHp;
            Savemap.header.leader_mp = g_ActiveCharacters[i].mp;
            Savemap.header.leader_mp_max = g_ActiveCharacters[i].baseMp;
            break;
        }
    }
    for (i = 0; i < 12; i++) {
        Savemap.header.menu_color[i] = g_MenuColors[i];
    }
    Savemap.header.gil = Savemap.gil;
    Savemap.header.time = Savemap.time;
    func_801D21E0(0x18);
    func_801D21F0(Savemap.header.place_name, &Savemap.memory_bank_4[104]);
}

static s32 WriteSaveFile(s8* path, u8* title) {
    u8 hour;
    u8 minute;
    u8 digit;
    s32 existingFd;
    s32 createdFd;
    s32 fd;
    s32 written;
    s32 writeComplete;
    s32 deleteRetry;
    s32 createRetry;
    s32 openRetry;
    s32 writeRetry;
    s32 i;
    u8* reserved;
    u8* savemapSrc;
    u8* headerDst;
    u8* iconClut;
    // Never read, but the target reserves its 0x200 bytes on the stack.
    MemcardFileHeader unused;

    UpdateSaveHeader();
    g_SaveWriteRemaining = sizeof(MemcardSaveFile);
    g_SaveFileHeader.magic[0] = 'S';
    g_SaveFileHeader.magic[1] = 'C';
    g_SaveFileHeader.iconFlag = 0x11;
    g_SaveFileHeader.blockCount = 1;
    func_801D21B8(g_SaveFileHeader.title, title);
    i = sizeof(g_SaveFileHeader.reserved) - 1;
    reserved = &g_SaveFileHeader.reserved[i];
    for (; i >= 0; i--) {
        *reserved-- = 0;
    }
    hour = SysGetHoursFromSeconds(Savemap.header.time);
    digit = ((hour / 10) * 2) + 0x20;
    g_SaveFileHeader.title[0x16] = g_ShiftJisTable[digit];
    g_SaveFileHeader.title[0x17] = g_ShiftJisTable[digit + 1];
    digit = ((hour % 10) * 2) + 0x20;
    g_SaveFileHeader.title[0x18] = g_ShiftJisTable[digit];
    g_SaveFileHeader.title[0x19] = g_ShiftJisTable[digit + 1];
    minute = SysGetMinutesFromSeconds(Savemap.header.time);
    digit = ((minute / 10) * 2) + 0x20;
    g_SaveFileHeader.title[0x1C] = g_ShiftJisTable[digit];
    g_SaveFileHeader.title[0x1D] = g_ShiftJisTable[digit + 1];
    digit = ((minute % 10) * 2) + 0x20;
    g_SaveFileHeader.title[0x1E] = g_ShiftJisTable[digit];
    g_SaveFileHeader.title[0x1F] = g_ShiftJisTable[digit + 1];
    iconClut = &g_SaveIcons[g_SaveSlot * SAVE_ICON_SIZE];
    memcpy(g_SaveFileHeader.iconPalette, iconClut,
           sizeof(g_SaveFileHeader.iconPalette));
    memcpy(g_SaveFileHeader.iconFrame[0],
           &g_SaveIcons[(g_SaveSlot * SAVE_ICON_SIZE) + 0x2C],
           sizeof(g_SaveFileHeader.iconFrame[0]));
    headerDst = (u8*)&g_SaveFile.header;
    memcpy(headerDst, (u8*)&g_SaveFileHeader, sizeof(MemcardFileHeader));
    g_SavemapBusy = 1;
    Savemap.header.checksum =
        func_801D1950(0x10F0, (u8*)&Savemap.header.leader_level);
    savemapSrc = (u8*)&Savemap;
    memcpy((u8*)&g_SaveFile.save, savemapSrc, sizeof(SaveWork));
    g_SavemapBusy = 0;

    for (deleteRetry = 0; deleteRetry < 0xA; deleteRetry++) {
        // The store into fd is dead; the overlay stops matching without it.
        existingFd = fd = open(path, 1);
        if (existingFd != -1) {
            delete (path);
            close(existingFd);
            break;
        }
    }
    for (createRetry = 0x1E; createRetry != 0; createRetry--) {
        createdFd = open(path, (g_SaveFileHeader.blockCount << 0x10) | 0x200);
        if (createdFd != -1) {
            goto created;
        }
    }
    return 1;
created:
    close(createdFd);
    for (openRetry = 0x1E; openRetry != 0; openRetry--) {
        fd = open(path, 2);
        if (fd != -1) {
            goto opened;
        }
    }
    return 2;
opened:
    writeRetry = 0x1E;
    do {
        // Same: this copy is redundant but the overlay needs it.
        createdFd = fd;
        written = write(createdFd, &g_SaveFile, g_SaveWriteRemaining);
        writeComplete = written == g_SaveWriteRemaining;
        if (writeComplete) {
            goto written_ok;
        }
        writeRetry--;
        if (written != -1) {
            g_SaveWriteRemaining -= written;
        }
    } while (writeRetry != 0);
    close(createdFd);
    return 3;
written_ok:
    close(createdFd);
    return 0;
}

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
    char path[0x40];
    s32 ret;
    s32 slot;

    if (save_id & 0x10) {
        sprintf(path, D_801D018C, D_801E2C78[save_id & 15]);
    } else {
        sprintf(path, D_801D0194, D_801E2C78[save_id & 15]);
    }
    slot = save_id & 15;
    g_SaveSlot = slot;
    ret = WriteSaveFile(path, (u8*)D_801E2CB8[slot]);
    if (!(s16)ret) {
        memcpy(&D_801E3864[slot], &Savemap.header, sizeof(SaveHeader));
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
