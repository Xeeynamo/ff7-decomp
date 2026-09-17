//! G=8 COMM=true
#include "main_private.h"

void func_80014578(s32 file_no, void* dst, void (*cb)(void));
s32 SysGzipPackDecompressById(u8* src, void* dst, s32 type);
void func_80014610(void);
void func_80014750(void);
void func_80014804(void);
void func_80014C70(void);

void func_800148A0(void);
INCLUDE_ASM("asm/us/main/nonmatchings/btlinit", func_800148A0);

extern u8 D_80063048;
Unk800A8D04* g_CurrentAction;

int func_800148B4(void) {
    func_800148A0();
    g_CurrentAction = (Unk800A8D04*)0x1F800000;
    D_800707C0 = &D_80063048;
    func_80014610();
    func_80014C70();
    func_80014578(1, (void*)0x801B0000, func_80014804);
    SystemCdWaitCallback(NULL);
    func_80014578(2, (void*)0x801B0000, func_80014750);
    SystemCdWaitCallback(NULL);
    return 1;
}

void func_80014934(void) {
    func_800148A0();
    func_80014578(INIT_KERNEL, (void*)0x801B0000, 0);
    SystemCdWaitCallback(0);
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
