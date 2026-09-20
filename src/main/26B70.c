//! PSYQ=3.3 CC1=2.7.2 G=8 COMM=true
#include "main_private.h"

u8 D_80062DFC = 0x40;
u8 _D_80062DFD = 0x00;
static s8 _D_80062DFE = 0x00;
static s8 _D_80062DFF = 0x00;
s32 D_80062E00 = 0x00000000;
s32 D_80062E04 = 0x00000000;
s16 g_AkaoTransfer = 0x0000;
s16 D_80062E0A = 0x0000;
s32 D_80062E0C = 0;
OT_TYPE* D_80062FC4;
void* D_80063008;
OT_TYPE* D_8006300C;

void SysMenuSetPoly(void* arg0) { D_80062F24.poly = arg0; }

void func_800269D0(void) { D_80063008 = D_80062F24.poly; }

void func_800269E8(void) { D_80062F24.poly = D_80063008; }

void SysMenuSetOtag(OT_TYPE* otag) { D_80062FC4 = otag; }

void func_80026A0C(void) { D_8006300C = D_80062FC4; }

void func_80026A20(void) { D_80062FC4 = D_8006300C; }

void SysMenuSetDrawMode(s32 dfe, s32 dtd, s32 tpage, RECT* tw) {
    SetDrawMode(D_80062F24.poly, dfe, dtd, tpage, tw);
    AddPrim(D_80062FC4, D_80062F24.dr_mode++);
}

void SysMenuSetDrawenv(DRAWENV* src, RECT* rect) {
    DRAWENV env;

    SetDefDrawEnv(&env, src->ofs[0], src->ofs[1], src->clip.w, src->clip.h);
    env.dfe = 1;
    env.isbg = 0;
    env.clip.x += rect->x;
    env.clip.y += rect->y;
    env.clip.w = rect->w;
    env.clip.h = rect->h;
    env.tpage = 0x1F | 0x20; // 0x1F:last tpage, 0x20:8bpp
    SetDrawEnv(D_80062F24.poly, &env);
    AddPrim(D_80062FC4, D_80062F24.poly);
    D_80062F24.poly = (u8*)D_80062F24.poly + 0x40;
}

void func_80026B5C(void) {}

void func_80026B64(u8 arg0) { D_80062DFC = arg0; }

// strlen but for FF7 strings
// FF7 string is 0x00: ' ', 0x10: '0', 0x21: 'A', 0xFF: terminator
s32 SysGetSingleStringWidth(u8* str) {
    s32 width;
    s32 i;
    s32 base;
    u8 temp_v1;

    width = 0;
    for (i = 0; i < D_80062DFC; i++) {
        if (!str) {
            break;
        }
        if (*str == 0xFF) {
            break;
        }
        switch (*str) {
        case 0xFA:
            str++;
            base = 0xE7;
            break;
        case 0xFB:
            str++;
            base = 0x1B9;
            break;
        case 0xFC:
            str++;
            base = 0x2A0;
            break;
        case 0xFD:
            str++;
            base = 0x372;
            break;
        case 0xFE:
            str++;
            base = 0x444;
            break;
        default:
            base = 0;
            break;
        }
        temp_v1 = D_800707C0[*str++ + base];
        width += temp_v1 >> 5;
        width += temp_v1 & 0x1F;
    }
    return width;
}

s32 SysMenuDrawSingleLetter(s32 x, s32 y, s32 color, s32 character) {
    RECT sp18;
    u16 sp20;
    s16 base;
    s32 tv;
    s16 palette;
    s16 var_s5;
    u8 tu;
    s32 clutY;
    s32 clutX;
    u8* var_v1;

    palette = 0;
    var_s5 = 0;
    sp20 = character;
    var_v1 = (u8*)&sp20;
    switch (*var_v1) {
    case 0xF8:
        return x;
    case 0xF9:
        base = 0;
        tv = 0;
        break;
    case 0xFA:
        var_v1++;
        tv = 0x84;
        base = 0xE7;
        break;
    case 0xFB:
        var_v1++;
        tv = 0;
        palette = 0x10;
        base = 0x1B9;
        break;
    case 0xFC:
        var_v1++;
        tv = 0x84;
        palette = 0x10;
        base = 0x2A0;
        break;
    case 0xFD:
        var_v1++;
        tv = 0x84;
        base = 0x372;
        var_s5 = -0x40;
        break;
    case 0xFE:
        var_v1++;
        tv = 0x84;
        palette = 0x10;
        base = 0x444;
        var_s5 = -0x40;
        break;
    default:
        base = 0;
        tv = 0;
        break;
    }

    sp20 = *var_v1;
    tu = (sp20 % 21) * 12;
    tv = sp20 / 21 * 12 + tv;
    clutY = color;
    clutX = 0x100;
    x += D_800707C0[sp20 + base] >> 5;
    setSprt(D_80062F24.sprt);
    SetShadeTex(D_80062F24.sprt, 1);
    D_80062F24.sprt->x0 = x;
    D_80062F24.sprt->y0 = y;
    D_80062F24.sprt->u0 = tu;
    D_80062F24.sprt->v0 = tv;
    D_80062F24.sprt->w = 12;
    D_80062F24.sprt->h = 12;
    D_80062F24.sprt->clut = GetClut(palette | clutX, clutY + 0x1F0);
    AddPrim(D_80062FC4, D_80062F24.sprt++);
    x += D_800707C0[sp20 + base] & 0x1F;
    if (_D_80062DFD == 0) {
        sp18.w = 0xFF;
        sp18.h = 0xFF;
        sp18.x = 0;
        sp18.y = 0;
        SetDrawMode(D_80062F24.dr_mode, 0, 1, (((var_s5 + 0x380) & 0x3FF) >> 6) | 0x30, &sp18);
        AddPrim(D_80062FC4, D_80062F24.dr_mode++);
    }
    return x;
}

void SysMenuDrawString(s32 x, s32 y, const char* str, s32 color) {
    RECT rect;
    s16 i;
    u8 ch;

    if (!str) {
        return;
    }
    for (i = 0; i < D_80062DFC; i++) {
        ch = (u8)*str;
        if (ch == 0xFF) {
            break;
        }
        if (ch > 0xF9 && ch < 0xFF || ch == 0xF8) {
            x = SysMenuDrawSingleLetter((s16)x, (s16)y, (u8)color, ch | (str[1] << 8));
            str += 2;
        } else {
            str++;
            x = SysMenuDrawSingleLetter((s16)x, (s16)y, (u8)color, ch);
        }
    }
    if (_D_80062DFD) {
        rect.w = 0xFF;
        rect.h = 0xFF;
        rect.x = 0;
        rect.y = 0;
        SetDrawMode(D_80062F24.dr_mode, 0, 1, 0x3E, &rect);
        AddPrim(D_80062FC4, D_80062F24.dr_mode++);
    }
}

void SysMenuDrawSingleFontLetter(s16 x, s16 y, s32 ch, u8 color) {
    s32 index;
    s32 u;
    s32 v;
    u32 c;

    c = ch & 0xFF;
    if (c < 0x29) {
        setSprt8(D_80062F24.sprt8);
        SetShadeTex(D_80062F24.sprt8, 1);
        D_80062F24.sprt8->x0 = x;
        D_80062F24.sprt8->y0 = y - 8;
        D_80062F24.sprt8->u0 = 0x88;
        D_80062F24.sprt8->v0 = 0x98;
        D_80062F24.sprt8->clut = GetClut(0x100, (color & 0xFF) + 0x1E0);
        AddPrim(D_80062FC4, D_80062F24.sprt8++);
        index = c + 0x40;
        u = ((index & 0xF) * 8) | 0x80;
        index >>= 4;
        index *= 8;
        v = index + 0x80;
    } else if ((u8)(ch - 0xB4) >= 0x1A && (u8)(ch - 0x29) < 0xA) {
        setSprt8(D_80062F24.sprt8);
        SetShadeTex(D_80062F24.sprt8, 1);
        D_80062F24.sprt8->x0 = x;
        D_80062F24.sprt8->y0 = y - 8;
        D_80062F24.sprt8->u0 = 0x90;
        D_80062F24.sprt8->v0 = 0x98;
        D_80062F24.sprt8->clut = GetClut(0x100, (color & 0xFF) + 0x1E0);
        AddPrim(D_80062FC4, D_80062F24.sprt8++);
        index = c + 0x17;
        u = ((index & 0xF) * 8) | 0x80;
        index >>= 4;
        index *= 8;
        v = index + 0x80;
    } else {
        u = ((ch & 0xF) * 8) | 0x80;
        v = ((c >> 4) * 8) | 0x80;
    }

    setSprt8(D_80062F24.sprt8);
    SetShadeTex(D_80062F24.sprt8, 1);
    D_80062F24.sprt8->x0 = x;
    D_80062F24.sprt8->y0 = y;
    D_80062F24.sprt8->u0 = u;
    D_80062F24.sprt8->v0 = v;
    D_80062F24.sprt8->clut = GetClut(0x100, (color & 0xFF) + 0x1E0);
    AddPrim(D_80062FC4, D_80062F24.sprt8++);
}

void SysMenuDraw8widthFont(s16 x, s16 y, u8* str, u8 color) {
    s16 i;

    if (!str) {
        return;
    }
    for (i = 0; i < D_80062DFC; i++) {
        if (*str == 0xFF) {
            break;
        }
        SysMenuDrawSingleFontLetter(x, y, *str++, color);
        x += 8;
    }
}

void SysMenuDrawHpMpBar(MenuHpMpBar* bar) {
    s16 w;
    s16 fillWidth;
    s16 barWidth;
    s16 x;
    s16 y;
    s16 h;
    u8 r;
    u8 g;
    u8 b;

    if (bar->max == 0) {
        return;
    }
    w = bar->w;
    fillWidth = w * bar->fillValue / bar->max;
    barWidth = w * bar->barValue / bar->max;
    x = bar->x;
    y = bar->y;
    h = bar->h;

    SetPolyG4(D_80062F24.polyg4);
    setXYWH(D_80062F24.polyg4, x, y, barWidth, h);
    r = bar->r;
    g = bar->g;
    b = bar->b;
    setRGB0(D_80062F24.polyg4, r, g, b);
    setRGB1(D_80062F24.polyg4, 0xC8, 0xC8, 0xC8);
    setRGB2(D_80062F24.polyg4, r, g, b);
    setRGB3(D_80062F24.polyg4, 0xC8, 0xC8, 0xC8);
    AddPrim(D_80062FC4, D_80062F24.polyg4++);

    if (bar->barMode != 0) {
        if (bar->barMode == 1) {
            r = 0;
            g = 0xC8;
            b = 0x50;
        } else {
            r = 0xC8;
            g = 0;
            b = 0;
        }
        SetPolyG4(D_80062F24.polyg4);
        setXYWH(D_80062F24.polyg4, x, y, fillWidth, h);
        setRGB0(D_80062F24.polyg4, r, g, b);
        setRGB1(D_80062F24.polyg4, r, g, b);
        setRGB2(D_80062F24.polyg4, r, g, b);
        setRGB3(D_80062F24.polyg4, r, g, b);
        AddPrim(D_80062FC4, D_80062F24.polyg4++);
    }

    SetPolyG4(D_80062F24.polyg4);
    setXY4(D_80062F24.polyg4, x, y, x + w, y, x, y + h + 1, x + w, y + h + 1);
    setRGB0(D_80062F24.polyg4, 0x50, 0, 0);
    setRGB1(D_80062F24.polyg4, 0x50, 0, 0);
    setRGB2(D_80062F24.polyg4, 0, 0, 0);
    setRGB3(D_80062F24.polyg4, 0, 0, 0);
    AddPrim(D_80062FC4, D_80062F24.polyg4++);
}

// unused?
void MENU_OutlineRect(s32 x, s32 y, s32 w, s32 h, s32 color) {
    setLineF4(D_80062F24.linef4);
    setXY4(D_80062F24.linef4, x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1);
    setRGB0(D_80062F24.linef4, color, color, color);
    AddPrim(D_80062FC4, D_80062F24.linef4++);

    setLineF2(D_80062F24.linef2);
    setXY2(D_80062F24.linef2, x, y, x, y + h - 1);
    setRGB0(D_80062F24.linef2, color, color, color);
    AddPrim(D_80062FC4, D_80062F24.linef2++);
}

void SysMenuDrawScrollbarTrack(RECT* rect) {
    setLineF2(D_80062F24.linef2);
    SetSemiTrans(D_80062F24.linef2, 1);
    setXY2(D_80062F24.linef2, rect->x, rect->y, rect->x + rect->w - 1, rect->y);
    setRGB0(D_80062F24.linef2, 0x10, 0x10, 0x10);
    AddPrim(D_80062FC4, D_80062F24.linef2++);

    setLineF2(D_80062F24.linef2);
    SetSemiTrans(D_80062F24.linef2, 1);
    setXY2(D_80062F24.linef2, rect->x, rect->y + rect->h - 1, rect->x + rect->w - 1, rect->y + rect->h - 1);
    setRGB0(D_80062F24.linef2, 0x70, 0x70, 0x70);
    AddPrim(D_80062FC4, D_80062F24.linef2++);

    setLineF2(D_80062F24.linef2);
    SetSemiTrans(D_80062F24.linef2, 1);
    setXY2(D_80062F24.linef2, rect->x, rect->y, rect->x, rect->y + rect->h - 1);
    setRGB0(D_80062F24.linef2, 0x20, 0x20, 0x20);
    AddPrim(D_80062FC4, D_80062F24.linef2++);

    setLineF2(D_80062F24.linef2);
    SetSemiTrans(D_80062F24.linef2, 1);
    setXY2(D_80062F24.linef2, rect->x + rect->w - 1, rect->y, rect->x + rect->w - 1, rect->y + rect->h - 1);
    setRGB0(D_80062F24.linef2, 0x70, 0x70, 0x70);
    AddPrim(D_80062FC4, D_80062F24.linef2++);

    setTile(D_80062F24.tile);
    SetSemiTrans(D_80062F24.tile, 1);
    setXY0(D_80062F24.tile, rect->x, rect->y);
    setWH(D_80062F24.tile, rect->w, rect->h);
    setRGB0(D_80062F24.tile, 0x50, 0x50, 0x50);
    AddPrim(D_80062FC4, D_80062F24.tile++);
}

void SysMenuDrawScrollbarSlider(RECT* rect) {
    setLineF2(D_80062F24.linef2);
    setXY2(D_80062F24.linef2, rect->x, rect->y, rect->x + rect->w - 1, rect->y);
    setRGB0(D_80062F24.linef2, 0xF0, 0xF0, 0xF0);
    AddPrim(D_80062FC4, D_80062F24.linef2++);

    setLineF2(D_80062F24.linef2);
    setXY2(D_80062F24.linef2, rect->x, rect->y + rect->h - 1, rect->x + rect->w - 1, rect->y + rect->h - 1);
    setRGB0(D_80062F24.linef2, 0x40, 0x40, 0x40);
    AddPrim(D_80062FC4, D_80062F24.linef2++);

    setLineF2(D_80062F24.linef2);
    setXY2(D_80062F24.linef2, rect->x, rect->y, rect->x, rect->y + rect->h - 1);
    setRGB0(D_80062F24.linef2, 0xC8, 0xC8, 0xC8);
    AddPrim(D_80062FC4, D_80062F24.linef2++);

    setLineF2(D_80062F24.linef2);
    setXY2(D_80062F24.linef2, rect->x + rect->w - 1, rect->y, rect->x + rect->w - 1, rect->y + rect->h - 1);
    setRGB0(D_80062F24.linef2, 0x70, 0x70, 0x70);
    AddPrim(D_80062FC4, D_80062F24.linef2++);

    setTile(D_80062F24.tile);
    setXY0(D_80062F24.tile, rect->x, rect->y);
    setWH(D_80062F24.tile, rect->w, rect->h);
    setRGB0(D_80062F24.tile, 0xA0, 0xA0, 0xA0);
    AddPrim(D_80062FC4, D_80062F24.tile++);
}

void SysMenuDrawScrollbar(MenuScrollbar* bar) {
    RECT rect;
    s32 offset;
    s32 length;

    offset = bar->track.h * bar->topRow / bar->totalRows;
    length = bar->track.h * bar->visibleRows / bar->totalRows;

    rect.x = bar->track.x;
    rect.y = bar->track.y + offset;
    rect.w = bar->track.w - 1;
    rect.h = length + 1;
    SysMenuDrawScrollbarSlider(&rect);

    rect.x = bar->track.x;
    rect.y = bar->track.y;
    rect.w = bar->track.w;
    rect.h = bar->track.h;
    SysMenuDrawScrollbarTrack(&rect);

    SysMenuSetDrawMode(0, 1, 0x1F, NULL);
}

void SysMenuDrawProgressBar(s16 x, s16 y, s16 w, s16 h, u8 r, u8 g, u8 b) {
    SetPolyG4(D_80062F24.polyg4);
    SetSemiTrans(D_80062F24.polyg4, 1);
    setXY4(D_80062F24.polyg4, x, y, x + w, y, x, y + h / 2, x + w, y + h / 2);
    setRGB0(D_80062F24.polyg4, r, g, b);
    setRGB1(D_80062F24.polyg4, r, g, b);
    setRGB2(D_80062F24.polyg4, 0x80, 0x80, 0x80);
    setRGB3(D_80062F24.polyg4, 0x80, 0x80, 0x80);
    AddPrim(D_80062FC4, D_80062F24.polyg4++);

    SetPolyG4(D_80062F24.polyg4);
    SetSemiTrans(D_80062F24.polyg4, 1);
    setXY4(D_80062F24.polyg4, x, y + h / 2, x + w, y + h / 2, x, y + h, x + w, y + h);
    setRGB0(D_80062F24.polyg4, r, g, b);
    setRGB1(D_80062F24.polyg4, r, g, b);
    setRGB2(D_80062F24.polyg4, 0, 0, 0);
    setRGB3(D_80062F24.polyg4, 0, 0, 0);
    AddPrim(D_80062FC4, D_80062F24.polyg4++);
}

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", func_80028930);

void SysMenuDrawTexturedRect(s16 x, s16 y, u8 u, u8 v, s16 w, s16 h, s16 clutY, s16 semiTrans) {
    setSprt(D_80062F24.sprt);
    SetShadeTex(D_80062F24.sprt, 1);
    if (semiTrans) {
        SetSemiTrans(D_80062F24.sprt, 1);
    }
    setXY0(D_80062F24.sprt, x, y);
    setUV0(D_80062F24.sprt, u, v);
    setWH(D_80062F24.sprt, w, h);
    D_80062F24.sprt->clut = GetClut(0x100, clutY + 0x1E0);
    AddPrim(D_80062FC4, D_80062F24.sprt);
    D_80062F24.sprt++;
}

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawDigitsWithoutLeadingZeroes);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawDigitsWithLeadingZeroes);
