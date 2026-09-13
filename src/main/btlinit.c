//! G=8
#include "main_private.h"

void func_80014578(s32 file_no, void* dst, void (*cb)(void));
void func_800145BC(void (*cb)(void));
s32 SysGzipPackDecompressById(u8* src, void* dst, s32 type);

void func_800148A0(void);
INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", func_800148A0);

void func_800148B4(void);
INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", func_800148B4);

void func_80014934(void) {
    func_800148A0();
    func_80014578(INIT_KERNEL, (void*)0x801B0000, 0);
    func_800145BC(0);
    SysGzipPackDecompressById((u8*)0x801B0000, &Savemap.party, KERNEL_INIT);
}

INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", SysLoadUncompressImg);

void SysLoadDrawSync(void) { DrawSync(0); }

void SysMemCopy32(void* dst, const void* src, const s32 len) {
    s32 i;
    for (i = 0; i < len >> 2; i++) {
        ((s32*)dst)[i] = ((s32*)src)[i];
    }
}

s32 SysGetLsbNumber(u32 arg0) {
    s32 i;
    for (i = 0;; i++) {
        arg0 >>= 1;
        if (!arg0) {
            return i;
        }
    }
}

s32 SysCountActiveBits(u32 arg0) {
    s32 i;
    i = 0;
    while (arg0) {
        if (arg0 & 1) {
            i++;
        }
        arg0 >>= 1;
    }
    return i;
}

INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", SysSelectRandomBit);

INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", SysAddSubWithMaxMinBound);
