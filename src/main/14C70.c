//! G=8
#include "main_private.h"

s32 D_80062D50 = 0x000000FF;
s32 D_80062E1C;
s32 D_80062E20;
s32 D_80062E24;
s32 D_80062E28;
s32 D_80062E2C;

void func_80014C70() {
    D_80062E1C = 0;
    D_80062E20 = 0;
}

u8* func_80014C80(s32 arg0) {
    s32 text_index;
    s32 text_offset;

    text_index = D_80062E1C++;
    text_offset = D_80062E20;
    g_KernelTextBlockOffsets[text_index] = text_offset;
    D_80062E20 = text_offset + arg0;
    return g_KernelTextBuffer + text_offset;
}

s32 func_80014CBC(s32 arg0, s32 arg1) {
    s32 var_a2;
    u8 var_v1;

    var_v1 = 0xFF;
    var_a2 = -1;
    switch (arg0) {
    case 0:
    case 1:
    case 2:
        var_v1 = D_800708C4[D_80010100[arg0] + arg1].conditionSubmenu;
        break;
    case 4:
        if (arg1 < 0x80) {
            var_v1 = D_800722CC[arg1].conditionSubmenu;
        }
    }
    if (var_v1 != 0xFF) {
        var_a2 = var_v1;
    }
    return var_a2;
}

static u8* func_80014D58(u8* arg0, u8* arg1, s32 arg2) {
    u8 var_a3 = *arg1;
    while (var_a3 != 0xFF) {
        *arg0 = var_a3;
        arg1++;
        arg2--;
        arg0++;
        if (arg2 == -1) {
            break;
        }
        var_a3 = *arg1;
    }
    return arg0;
}

u8* SysGetKernTextPtr(s32 blockId, s32 entryId, s32 blockOffset) {
    u8* sectionBase = g_KernelTextBuffer + g_KernelTextBlockOffsets[blockId + blockOffset];
    return (u8*)&sectionBase[*(u16*)&sectionBase[entryId * 2]];
}

static void func_80014DD0(s32 arg0, s32 arg1, u8* arg2) { func_80014D58(arg2, SysGetKernTextPtr(arg0, arg1, 0), -1); }

INCLUDE_ASM("asm/us/main/nonmatchings/14C70", func_80014E0C);

INCLUDE_ASM("asm/us/main/nonmatchings/14C70", func_80014E74);

s32 SysDecompKernStringWithF9(u16* arg0, u16* arg1);
INCLUDE_ASM("asm/us/main/nonmatchings/14C70", SysDecompKernStringWithF9);

u8* SysGetKernBattleTextPtr(s32 TextId) { return SysGetKernTextPtr(KERNEL_TEXT_BATTLE_MESSAGES, TextId, 0); }

s32 SysGetKernBattleTextById(s32 TextId) {
    u8* tmpBuf = SysGetKernBattleTextPtr(TextId);
    return SysDecompKernStringWithF9(tmpBuf, tmpBuf);
}

INCLUDE_ASM("asm/us/main/nonmatchings/14C70", SysKernGetString);

INCLUDE_ASM("asm/us/main/nonmatchings/14C70", SysSetEngineErrorCode);

INCLUDE_ASM("asm/us/main/nonmatchings/14C70", func_800155B0);

void func_80015654(s32 arg0) {
    D_80062E24 = 0;
    D_80062E28 = 0;
    D_80062E2C = arg0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/14C70", func_80015668);

INCLUDE_ASM("asm/us/main/nonmatchings/14C70", func_800159B0);

INCLUDE_ASM("asm/us/main/nonmatchings/14C70", SysGetLimitCmdId);
