//! PSYQ=3.3 CC1=2.7.2 G=8 COMM=true
#include "main_private.h"

u16 g_Pad1Buttons = 0;
u16 g_Pad1ButtonsPrev = 0;
u16 g_Pad1ButtonsPressed = 0;
u16 g_Pad1ButtonsRepeat = 0;
u16 g_Pad2Buttons = 0;
u16 g_Pad2ButtonsPrev = 0;
u16 g_Pad2ButtonsPressed = 0;
u16 g_Pad2ButtonsRepeat = 0;
u16 g_Pad1FastButtons = 0;
u16 g_Pad1FastButtonsPrev = 0;
u16 g_Pad1FastButtonsPressed = 0;
u16 g_Pad1FastButtonsRepeat = 0;
u16 g_Pad2FastButtons = 0;
u16 g_Pad2FastButtonsPrev = 0;
u16 g_Pad2FastButtonsPressed = 0;
u16 g_Pad2FastButtonsRepeat = 0;
u8 D_80062D98 = 0x00;
u8 D_80062D99 = 0x00;
static s16 D_80062D9A = 0x0000;
s32 D_80062D9C = 0x00000000;
s32 D_80062DA0 = 0x00140000;
s32 D_80062DA4 = 0x007800A0;
s32 D_80062DA8 = 0x00010002;
s32 D_80062DAC = 0x00060006;
s32 D_80062DB0 = 0x00010001;
s16 D_80062DB4 = 0x0007;
s16 D_80062DB6 = 0x0000;
s16 D_80062DB8 = 0x0000;
s16 D_80062DBA = 0x0000;
s16 D_80062DBC = 0x0000;
s16 D_80062DBE = 0x0000;
s32 D_80062DC0 = 0x00000000;
s32 D_80062DC4 = 0x00000000;
s32 D_80062DC8 = 0x00000000;

s32 D_80062F9C;
s32 D_80062FF0;

void func_8001155C(void);
void func_80014A00(s32* dst, s32* src, s32 len);
u16* func_80014D9C(s32, s32, s32);
s32 func_800150E4(u16*, u16*);
u16* func_800151F4(s32);
void func_80015CA0(GzHeader* src, s32* dst);
s32 func_8001AC9C(u8, s32);
s32 func_8001B834(s32);
void func_8001BD50(u8, u8, u8);
u8 func_8001F6B4();
void func_8001F6E4(
    s16 enabled, s16 x, s16 y); // PC: menu_setNotificationWindowPosition
void func_80026A34(s32 dfe, s32 dtd, u16 tpage, RECT* tw);

void func_8001CDA4(void) {
    SetPolyFT4(D_80062F24.ft4);
    SetShadeTex(D_80062F24.ft4, 1);
    D_80062F24.ft4->x0 = 0;
    D_80062F24.ft4->y0 = 5;
    D_80062F24.ft4->x1 = 64;
    D_80062F24.ft4->y1 = 5;
    D_80062F24.ft4->x2 = 0;
    D_80062F24.ft4->y2 = 69;
    D_80062F24.ft4->x3 = 64;
    D_80062F24.ft4->y3 = 69;
    D_80062F24.ft4->u0 = 0;
    D_80062F24.ft4->v0 = 0;
    D_80062F24.ft4->u1 = 128;
    D_80062F24.ft4->v1 = 0;
    D_80062F24.ft4->u2 = 0;
    D_80062F24.ft4->v2 = 128;
    D_80062F24.ft4->u3 = 128;
    D_80062F24.ft4->v3 = 128;
    D_80062F24.ft4->clut = GetClut(0, 0x1FE);
    D_80062F24.ft4->tpage = GetTPage(1, 0, 0x340, 0);
    AddPrim(D_80062FC4, D_80062F24.ft4);
    D_80062F24.ft4++;
}

void func_8001CF3C(s16 x, s16 y, s16 w, s16 h, u16 tx, u16 ty, u16 tw, u16 th,
                   s16 clut, s32 tex) {
    SetPolyFT4(D_80062F24.ft4);
    SetShadeTex(D_80062F24.ft4, 1);
    if (tex << 0x10) {
        SetSemiTrans(D_80062F24.ft4, 1);
    }
    D_80062F24.ft4->x0 = x;
    D_80062F24.ft4->y0 = y;
    D_80062F24.ft4->x1 = x + w;
    D_80062F24.ft4->y1 = y;
    D_80062F24.ft4->x2 = x;
    D_80062F24.ft4->y2 = y + h;
    D_80062F24.ft4->x3 = x + w;
    D_80062F24.ft4->y3 = y + h;
    D_80062F24.ft4->u0 = tx;
    D_80062F24.ft4->v0 = ty;
    D_80062F24.ft4->u1 = tx + tw;
    D_80062F24.ft4->v1 = ty;
    D_80062F24.ft4->u2 = tx;
    D_80062F24.ft4->v2 = ty + th;
    D_80062F24.ft4->u3 = tx + tw;
    D_80062F24.ft4->v3 = ty + th;
    D_80062F24.ft4->clut = GetClut(0x100, (s16)clut + 0x1E0);
    D_80062F24.ft4->tpage = GetTPage(1, 0, 0x3C0, 0x100);
    AddPrim(D_80062FC4, D_80062F24.ft4);
    D_80062F24.ft4++;
}

void func_8001D180(s16 x, s16 y, s16 w, s16 h, u16 tx, u16 ty, u16 tw, u16 th,
                   s16 clut, s32 tex) {
    SetPolyFT4(D_80062F24.ft4);
    SetShadeTex(D_80062F24.ft4, 1);
    if (tex << 0x10) {
        SetSemiTrans(D_80062F24.ft4, 1);
    }
    D_80062F24.ft4->x0 = x;
    D_80062F24.ft4->y0 = y;
    D_80062F24.ft4->x1 = x + w;
    D_80062F24.ft4->y1 = y;
    D_80062F24.ft4->x2 = x;
    D_80062F24.ft4->y2 = y + h;
    D_80062F24.ft4->x3 = x + w;
    D_80062F24.ft4->y3 = y + h;
    D_80062F24.ft4->u0 = tx;
    D_80062F24.ft4->v0 = ty;
    D_80062F24.ft4->u1 = tx + tw;
    D_80062F24.ft4->v1 = ty;
    D_80062F24.ft4->u2 = tx;
    D_80062F24.ft4->v2 = ty + th;
    D_80062F24.ft4->u3 = tx + tw;
    D_80062F24.ft4->v3 = ty + th;
    D_80062F24.ft4->clut = GetClut(0x180, (s16)clut);
    D_80062F24.ft4->tpage = GetTPage(1, 0, 0x340, 0x100);
    AddPrim(D_80062FC4, D_80062F24.ft4);
    D_80062F24.ft4++;
}

void func_8001D3C0(s16 x, s16 y) {
    SetTile1(D_80062F24.tile1);
    D_80062F24.tile1->x0 = x;
    D_80062F24.tile1->y0 = y;
    D_80062F24.tile1->r0 = 0xFF;
    D_80062F24.tile1->g0 = 0xFF;
    D_80062F24.tile1->b0 = 0;
    AddPrim(D_80062FC4, D_80062F24.tile1);
    D_80062F24.tile1++;
}

void func_8001D47C(s16 x0, s16 x1, s16 y, s32 color) {
    SetLineF2(D_80062F24.linef2);
    D_80062F24.linef2->r0 = color >> 16;
    D_80062F24.linef2->g0 = color >> 8;
    D_80062F24.linef2->b0 = color;
    D_80062F24.linef2->x0 = x0;
    D_80062F24.linef2->y0 = y;
    D_80062F24.linef2->x1 = x1;
    D_80062F24.linef2->y1 = y;
    AddPrim(D_80062FC4, D_80062F24.linef2);
    D_80062F24.linef2++;
}

void func_8001D56C(s16 x0, s16 y0, s16 x1, s16 y1, s16 is_yellow) {
    if (is_yellow) {
        SetLineF2(D_80062F24.linef2);
        D_80062F24.linef2->r0 = 0xFF;
        D_80062F24.linef2->g0 = 0xFF;
        D_80062F24.linef2->b0 = 0;
    } else {
        SetLineF2(D_80062F24.linef2);
        D_80062F24.linef2->r0 = 0x80;
        D_80062F24.linef2->g0 = 0x80;
        D_80062F24.linef2->b0 = 0x80;
    }
    D_80062F24.linef2->x0 = x0;
    D_80062F24.linef2->y0 = y0;
    D_80062F24.linef2->x1 = x1;
    D_80062F24.linef2->y1 = y1;
    AddPrim(D_80062FC4, D_80062F24.linef2);
    D_80062F24.linef2++;
}

s16 func_8001D6A8(s16 x, s16 y, s16 w, u8* txt) {
    RECT rect;
    u8 c;
    s32 width;
    s16 next;
    s16 textColor;
    s16 i;
    s16 windowBinOffset;
    s16 tv;
    u16 colorCycleCount;
    s16 tu;

    colorCycleCount = 0;
    for (i = 0; i < 1024; i++, colorCycleCount++) {
        c = *txt;
        if (D_80062DBC == 0 || c == 0xFF || c == 0xE8 || c == 0xE9)
            break;

        if (c == 0xE7) {
            x = 8;
            y += 16;
            txt++;
            D_80062F9C++;
            D_80062FF0++;
        } else {
            width = 0;
            switch (c) {
            case 0xFA:
                D_80062FF0++;
                txt++;
                tv = 132;
                windowBinOffset = 231;
                break;
            case 0xFB:
                D_80062FF0++;
                txt++;
                tv = 0;
                width = 16;
                windowBinOffset = 441;
                break;
            case 0xFC:
                D_80062FF0++;
                txt++;
                tv = 132;
                width = 16;
                windowBinOffset = 672;
                break;
            case 0xFD:
                D_80062FF0++;
                txt++;
                tv = 132;
                windowBinOffset = 882;
                break;
            case 0xFE:
                D_80062FF0++;
                txt++;
                if (*txt < 0xD2) {
                    tv = 132;
                    width = 16;
                    windowBinOffset = 1092;
                    break;
                }
                D_80062FF0++;
                if (*txt < 0xDA) {
                    D_80062DB4 = *txt++ - 0xD2;
                    continue;
                }
                if (*txt == 0xDA) {
                    D_80062DB6 ^= 1;
                    txt++;
                    continue;
                }
                if (*txt == 0xDB) {
                    D_80062DB8 ^= 1;
                    txt++;
                    continue;
                }
                if (*txt == 0xE9) {
                    D_80062DC4 ^= 1;
                    txt++;
                    continue;
                }
                break;
            default:
                if (c >= 0xF6 && c <= 0xF9) {
                    rect.x = 0;
                    rect.y = 0;
                    rect.w = 256;
                    rect.h = 256;
                    func_80026A34(0, 1, GetTPage(0, 1, 0x380, 0x100), &rect);

                    setSprt(D_80062F24.sprt);
                    SetShadeTex(D_80062F24.sprt, 1);
                    if (D_80062DC0 != 0)
                        SetSemiTrans(D_80062F24.sprt, 1);
                    D_80062F24.sprt->x0 = x;
                    D_80062F24.sprt->y0 = y - 2;
                    switch (*txt - 0xF6) {
                    case 0:
                        D_80062F24.sprt->u0 = 96;
                        D_80062F24.sprt->v0 = 64;
                        break;
                    case 1:
                        D_80062F24.sprt->u0 = 144;
                        D_80062F24.sprt->v0 = 64;
                        break;
                    case 2:
                        D_80062F24.sprt->u0 = 128;
                        D_80062F24.sprt->v0 = 64;
                        break;
                    case 3:
                        D_80062F24.sprt->u0 = 112;
                        D_80062F24.sprt->v0 = 64;
                        break;
                    }
                    D_80062F24.sprt->w = 16;
                    D_80062F24.sprt->h = 16;
                    D_80062F24.sprt->clut = GetClut(0x100, 0x1EA);
                    AddPrim(D_80062FC4, D_80062F24.sprt++);
                    txt++;

                    rect.x = 0;
                    rect.y = 0;
                    rect.w = 256;
                    rect.h = 256;
                    D_80062DBC--;
                    func_80026A34(0, 1, GetTPage(0, 1, 0x3C0, 0x100), &rect);
                    D_80062FF0++;
                    x += 16;
                    continue;
                }
                windowBinOffset = 0;
                tv = 0;
                break;
            }

            if (D_80062DB6 == 0) {
                if (D_80062DB8 == 0) {
                    textColor = D_80062DB4 + 0x1F0;
                } else {
                    textColor =
                        (((D_80062DBA >> 2) - colorCycleCount) & 7) | 0x1F0;
                }
            } else {
                if (D_80062DB8) {
                    textColor =
                        (((D_80062DBA >> 2) - colorCycleCount) & 7) | 0x1F0;
                } else if (((D_80062DBA >> 2) & 1) != 0) {
                    textColor = D_80062DB4 + 0x1F0;
                } else {
                    if (D_80062DB4 == 0) {
                        x += width;
                        continue;
                    }
                    textColor = 0x1F0;
                }
            }

            next = *txt;
            tu = 12 * (next % 21);
            tv += 12 * (next / 21);
            if (x + ((D_800707C0[next + windowBinOffset] >> 5) +
                     (D_800707C0[next + windowBinOffset] & 0x1F)) >
                w) {
                x = 8;
                y += 16;
                D_80062F9C++;
            }
            if (D_80062DC4 == 0) {
                x += D_800707C0[next + windowBinOffset] >> 5;
            }

            setSprt(D_80062F24.sprt);
            SetShadeTex(D_80062F24.sprt, 1);
            if (D_80062DC0 != 0)
                SetSemiTrans(D_80062F24.sprt, 1);
            D_80062F24.sprt->x0 = x;
            D_80062F24.sprt->y0 = y;
            D_80062F24.sprt->u0 = tu;
            D_80062F24.sprt->v0 = tv;
            D_80062F24.sprt->w = 12;
            D_80062F24.sprt->h = 12;
            D_80062F24.sprt->clut =
                GetClut((width << 16 >> 16) | 0x100, textColor);
            AddPrim(D_80062FC4, D_80062F24.sprt++);

            if (D_80062DC4 == 0)
                x += D_800707C0[next + windowBinOffset] & 0x1F;
            else
                x += 13;
            txt++;
            D_80062DBC--;
            D_80062FF0++;
        }
    }

    rect.x = 0;
    rect.y = 0;
    rect.w = 256;
    rect.h = 256;
    func_80026A34(0, 1, GetTPage(0, 1, 0x380, 0x100), &rect);
    return y;
}

void func_8001DE0C(Unk8001DE0C* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    arg0->unk0 = arg1;
    arg0->unk2 = arg2;
    arg0->unk4 = arg3;
    arg0->unk6 = arg4;
}

// translate window dialog
void func_8001DE24(Unk8001DE0C* arg0, s32 arg1, s32 arg2) {
    arg0->unk0 = arg0->unk0 + arg1;
    arg0->unk2 = arg0->unk2 + arg2;
}

// set window dialog rect
void func_8001DE40(Unk8001DE0C* arg0, Unk8001DE0C* arg1) {
    arg0->unk0 = arg1->unk0;
    arg0->unk2 = arg1->unk2;
    arg0->unk4 = arg1->unk4;
    arg0->unk6 = arg1->unk6;
}

INCLUDE_ASM("asm/us/main/nonmatchings/1CDA4", func_8001DE70);

INCLUDE_ASM("asm/us/main/nonmatchings/1CDA4", func_8001DEB0);

// sets the menu color with a quadruplet of RGB values
void func_8001DEF0(u8* menu_colors) {
    s32 i;
    for (i = 0; i < 12; i++) {
        g_MenuColors[i] = *menu_colors++;
    }
}

void func_8001DF24(RECT* rect, u8 arg1, u8 arg2, u8 arg3) {
    setTile(D_80062F24.tile);
    SetShadeTex(D_80062F24.tile, 1);
    D_80062F24.tile->x0 = rect->x;
    D_80062F24.tile->y0 = rect->y;
    D_80062F24.tile->w = rect->w;
    D_80062F24.tile->h = rect->h;
    D_80062F24.tile->r0 = arg1;
    D_80062F24.tile->g0 = arg2;
    D_80062F24.tile->b0 = arg3;
    AddPrim(D_80062FC4, D_80062F24.tile++);
}

// prints menu window
INCLUDE_ASM("asm/us/main/nonmatchings/1CDA4", func_8001E040);

// print menu cursor
void func_8001EB2C(s16 x, s16 y) {
    RECT rect;

    setSprt(D_80062F24.sprt);
    SetSemiTrans(D_80062F24.sprt, 1);
    SetShadeTex(D_80062F24.sprt, 1);
    D_80062F24.sprt->x0 = x;
    D_80062F24.sprt->y0 = y;
    D_80062F24.sprt->u0 = 224;
    D_80062F24.sprt->v0 = 8;
    D_80062F24.sprt->w = 24;
    D_80062F24.sprt->h = 16;
    D_80062F24.sprt->clut = GetClut(0x100, 0x1E1);
    AddPrim(D_80062FC4, D_80062F24.sprt++);
    rect.x = 0;
    rect.y = 0;
    rect.w = 0xFF;
    rect.h = 0xFF;
    func_80026A34(0, 1, (u16)GetTPage(0, 2, 0x3C0, 0x100), &rect);
}

INCLUDE_ASM("asm/us/main/nonmatchings/1CDA4", func_8001EC70);

void func_8001EF84(s32 x, s32 y, s32 n, s32 len) {
    RECT rect;
    s32 i;
    s32 uv;

    for (i = 0; i < 8; i++) {
        uv = n / D_80049224[i];
        setSprt(D_80062F24.sprt);
        SetShadeTex(D_80062F24.sprt, 1);
        D_80062F24.sprt->x0 = x;
        D_80062F24.sprt->y0 = y;
        D_80062F24.sprt->u0 = (uv % 5) * 16 - 80;
        D_80062F24.sprt->v0 = uv >= 5 ? 104 : 80;
        D_80062F24.sprt->w = 16;
        D_80062F24.sprt->h = 21;
        D_80062F24.sprt->clut = GetClut(0x100, 0x1EC);
        if (len >= 8 - i) {
            x += 16;
            AddPrim(D_80062FC4, D_80062F24.sprt++);
        }
        n %= D_80049224[i];
    }
    rect.x = 0;
    rect.y = 0;
    rect.w = 255;
    rect.h = 255;
    func_80026A34(0, 1, (u16)GetTPage(0, 1, 0x3C0, 0x100), &rect);
}

INCLUDE_ASM("asm/us/main/nonmatchings/1CDA4", SystemMenuDrawDialog);

void func_8001F6AC(void) {}
