//! PSYQ=3.3 CC1=2.7.2 G=8
#include "main_private.h"

u8 D_80062DFC = 0x40;
u8 _D_80062DFD = 0x00;
static s8 _D_80062DFE = 0x00;
static s8 _D_80062DFF = 0x00;
s32 D_80062E00 = 0x00000000;
s32 D_80062E04 = 0x00000000;
s16 D_80062E08 = 0x0000;
s16 D_80062E0A = 0x0000;
s32 D_80062E0C = 0;
extern OT_TYPE* D_80062FC4; // TODO: import sbss

void func_80026B5C(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", func_80026B64);

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

s32 SysMenuDrawSingleLetter(s16, s16, u8, s32);
INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawSingleLetter);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawString);
#else
void SysMenuDrawString(s32 x, s32 y, const char* str, s32 color) {
    RECT rect;
    s16 i;
    u8 ch;
    DR_MODE* temp_a1;

    if (!str) {
        return;
    }
    for (i = 0; i < D_80062DFC; i++) {
        ch = (u8)*str;
        if (ch == 0xFF) {
            break;
        }
        if (ch > 0xF9 && ch < 0xFF || ch == 0xF8) {
            x = SysMenuDrawSingleLetter(x, y, (u8)color, (ch | (str[1] << 8)));
            str += 2;
        } else {
            str++;
            x = SysMenuDrawSingleLetter(x, y, (u8)color, ch);
        }
    }
    if (_D_80062DFD) {
        rect.w = 0xFF;
        rect.h = 0xFF;
        rect.x = 0;
        rect.y = 0;
        SetDrawMode((DR_MODE*)D_80062F24.poly, 0, 1, 0x3E, &rect);
        temp_a1 = D_80062F24.poly;
        D_80062F24.poly = temp_a1 + 1;
        AddPrim(D_80062FC4, temp_a1);
    }
}
#endif

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawSingleFontLetter);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDraw8widthFont);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawHpMpBar);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", func_80027990);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawScrollbarTrack);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawScrollbarSlider);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawScrollbar);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawProgressBar);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", func_80028930);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawTexturedRect);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawDigitsWithoutLeadingZeroes);

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawDigitsWithLeadingZeroes);
