//! PSYQ=3.3 CC1=2.7.2 G=8

#include "main_private.h"

void SysMenuSetPoly(void* arg0) { D_80062F24.poly = arg0; }

INCLUDE_ASM("asm/us/main/nonmatchings/269C0", func_800269D0);

INCLUDE_ASM("asm/us/main/nonmatchings/269C0", func_800269E8);

INCLUDE_ASM("asm/us/main/nonmatchings/269C0", SysMenuSetOtag);

INCLUDE_ASM("asm/us/main/nonmatchings/269C0", func_80026A0C);

INCLUDE_ASM("asm/us/main/nonmatchings/269C0", func_80026A20);

INCLUDE_ASM("asm/us/main/nonmatchings/269C0", SysMenuSetDrawMode);

INCLUDE_ASM("asm/us/main/nonmatchings/269C0", SysMenuSetDrawenv);
