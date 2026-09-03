//! PSYQ=3.3 CC1=2.7.2 G=8
#include "main_private.h"

s8 D_80062DFC = 0x40;
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
INCLUDE_ASM("asm/us/main/nonmatchings/26B70", SysGetSingleStringWidth);

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
