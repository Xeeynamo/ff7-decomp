//! G=8 COMM=true
#include "main_private.h"

void func_80014578(s32 fileNo, void* dst, void (*cb)(void));
s32 SysGzipPackDecompressById(u8* src, void* dst, s32 type);
void func_80014610(void);
void func_80014750(void);
void func_80014804(void);
void func_80014C70(void);

s32 D_80062F88;

void SysBattleModeReset(void) {
    D_80062F88 = 0;
    g_BattleMode = 0;
}

extern u8 D_80063048;
struct Unk800A8D04* g_CurrentAction;

int func_800148B4(void) {
    SysBattleModeReset();
    g_CurrentAction = (struct Unk800A8D04*)0x1F800000;
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
    SysBattleModeReset();
    func_80014578(INIT_KERNEL, (void*)0x801B0000, 0);
    SystemCdWaitCallback(0);
    SysGzipPackDecompressById((u8*)0x801B0000, &Savemap.party, KERNEL_INIT);
}

void SysLoadUncompressImg(u8* tim) {
    if (((u32*)tim)[1] & 8) {
        LoadImage((RECT*)(tim + 0xC), (u_long*)(tim + 0x14));
        tim += (((u32*)tim)[2] >> 2) << 2;
    }
    LoadImage((RECT*)(tim + 0xC), (u_long*)(tim + 0x14));
}

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

s32 SysSelectRandomBit(u32 mask) {
    s32 bit = 0;
    s32 count = SysCountActiveBits(mask);

    if (count) {
        count = (SysGetRandomByteFromTable() * count) >> 8;
        for (bit = 1; bit != 0; bit <<= 1) {
            if ((mask & bit) && --count < 0) {
                break;
            }
        }
    }
    return bit;
}

u32 SysAddSubWithMaxMinBound(u32 val, u32 addend, u32 max, u32 isAdd) {
    u32 orig_val;

    orig_val = val;
    if (isAdd) {
        val += addend;
        if (val < orig_val) {
            val = max;
        }
        if (max < val) {
            val = max;
        }
    } else {
        val -= addend;
        if (orig_val < val) {
            val = 0;
        }
    }
    return val;
}
