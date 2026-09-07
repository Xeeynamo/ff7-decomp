//! PSYQ=3.6 CC1=2.7.2 G=8
#include "main_private.h"

// Functions used by the battle module for formulas requiring random nummbers.
// g_KernelRandomTable is the random number table from KERNEL.BIN section 3 and belongs
// to a larger struct.

static u32 g_KernelRandomCallCount = 0;
static u8 g_KernelRandomTablePos[8];
static s32 g_KernelRandomTableIndex;

extern u8 g_KernelRandomTable[256];

void SysIncSeedForRandom(void) {
    g_KernelRandomTableIndex++;
    g_KernelRandomTableIndex &= 7;
}

u8 SysGetRandomByteFromTable(void) { return g_KernelRandomTable[g_KernelRandomTablePos[g_KernelRandomTableIndex]++]; }

u8 SysGetRandomByteRange(s32 upperBound) { return (SysGetRandomByteFromTable() * upperBound) >> 8; }

u16 SysRandomTwoBytes(void) {
    u8 lo;

    lo = SysGetRandomByteFromTable();
    if (g_KernelRandomCallCount++ & 7) {
        SysIncSeedForRandom();
    }
    return (SysGetRandomByteFromTable() << 8) | lo;
}

void SysInitRndTablePos(s32 seed) {
    s32 i;

    for (i = 0; i < 8; i++) {
        g_KernelRandomTablePos[i] = seed;
        seed >>= 1;
    }
    g_KernelRandomTableIndex = 0;
}
