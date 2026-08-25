//! PSYQ=3.3 CC1=2.6.3 G=8
#include "main_private.h"

const u8 D_8001029C[6][4] = {
    {0, 1, 2, 3}, {0, 2, 1, 3}, {2, 0, 1, 3},
    {1, 0, 2, 3}, {1, 2, 0, 3}, {2, 1, 0, 3},
};

INCLUDE_ASM("asm/us/main/nonmatchings/1C0EC", func_8001C0EC);

void func_8001C3C4(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/1C0EC", func_8001C3CC);
