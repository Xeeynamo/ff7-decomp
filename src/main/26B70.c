//! PSYQ=3.3 CC1=2.7.2 G=8
#include "main_private.h"

u8 D_80062DFC = 0x40;
s8 _D_80062DFD = 0x00;
static s8 _D_80062DFE = 0x00;
static s8 _D_80062DFF = 0x00;
s32 D_80062E00 = 0x00000000;
s32 D_80062E04 = 0x00000000;
s16 D_80062E08 = 0x0000;
s16 D_80062E0A = 0x0000;
s32 D_80062E0C = 0;

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

INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawSingleLetter);

// print FF7 string
INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysMenuDrawString);

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
