//! PSYQ=3.6 CC1=2.7.2 G=8
#include "main_private.h"

// Functions used by the battle module for formulas requiring random nummbers.
// D_80083084 is the random number table from KERNEL.BIN section 3 and belongs
// to a larger struct.

static u32 D_80062D4C = 0;
static u8 D_80062E10[8];
static s32 D_80062E18;

extern u8 D_80083084[256];

void SysIncSeedForRandom(void) {
    D_80062E18++;
    D_80062E18 &= 7;
}

u8 SysGetRandomByteFromTable(void) {
    return D_80083084[D_80062E10[D_80062E18]++];
}

u8 SysGetRandomByteRange(s32 arg0) {
    return (SysGetRandomByteFromTable() * arg0) >> 8;
}

u16 SysRandomTwoBytes(void) {
    u8 lo;

    lo = SysGetRandomByteFromTable();
    if (D_80062D4C++ & 7) {
        SysIncSeedForRandom();
    }
    return (SysGetRandomByteFromTable() << 8) | lo;
}

void func_80014C44(s32 arg0) {
    s32 i;

    for (i = 0; i < 8; i++) {
        D_80062E10[i] = arg0;
        arg0 >>= 1;
    }
    D_80062E18 = 0;
}
