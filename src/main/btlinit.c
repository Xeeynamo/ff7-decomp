//! G=8
#include "main_private.h"

void func_80014578(s32 file_no, void* dst, void (*cb)(void));
void func_800145BC(void (*cb)(void));
s32 func_80015C3C(u8* src, void* dst, s32 type);

INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", func_800148A0);

INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", func_800148B4);

void func_80014934(void) {
    func_800148A0();
    func_80014578(INIT_KERNEL, (void*)0x801B0000, 0);
    func_800145BC(0);
    func_80015C3C(0x801B0000, D_8009C738, KERNEL_INIT);
}

INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", func_80014980);

void func_800149E0(void) { DrawSync(0); }

void func_80014A00(s32* dst, s32* src, s32 len) {
    int i;
    for (i = 0; i < len >> 2; i++) {
        *dst++ = *src++;
    }
}

s32 func_80014A38(u32 arg0) {
    s32 i;
    for (i = 0;; i++) {
        arg0 >>= 1;
        if (!arg0) {
            return i;
        }
    }
}

s32 func_80014A58(u32 arg0) {
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

INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", func_80014A84);

INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", func_80014B08);
