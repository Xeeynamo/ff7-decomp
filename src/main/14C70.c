//! G=8
#include "main_private.h"
#include "../battle/battle.h"

typedef struct {
    s32 dataOffsets[3];
    u16 itemOffsets[6];
    u8 itemToType[5];
    u8 pad_1[3];
    u8 magicTypeOffsets[4];
    u8 typeToSection[16];
    u8 pad_2[4];
} KernelTextMaps;

static const KernelTextMaps kernel_maps = {
    {0, 56, 72},         {0, 128, 256, 288, 384, 65535},
    {4, 10, 11, 12, 13}, {0, 0, 0},
    {0, 56, 72, 128},    {1, 1, 1, 1, 2, 0, 255, 255, 255, 255, 3, 4, 5, 6, 7, 0},
    {0, 0, 0, 0},
};

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
        var_v1 = D_800708C4[kernel_maps.dataOffsets[arg0] + arg1].conditionSubmenu;
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

static u8* func_80014D58(u8* arg0, const u8* arg1, s32 arg2) {
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

static u8* func_80014DD0(s32 arg0, s32 arg1, u8* arg2) {
    return func_80014D58(arg2, SysGetKernTextPtr(arg0, arg1, 0), -1);
}

static u8* func_80014E0C(s32 charId, u8* dst) {
    s32 i;

    for (i = 0; i < NUM_CHARACTERS; i++) {
        if (Savemap.party[i].char_id == charId) {
            dst = func_80014D58(dst, Savemap.party[i].name, LEN(Savemap.party[i].name));
            break;
        }
    }
    return dst;
}

#define MAX_DIGITS 16U
u8* func_80014E74(u8* dst, const u8* src) {
    s32 digits[MAX_DIGITS];
    u8* buffer = dst;
    u8 value = 0;
    s32 j = 0;
    s32 i;

    while (value != 0xFF) {
        value = src[j++];

        if (value >= 0xEA && value <= 0xF1) {
            u16 arg = src[j++] << 8;
            arg |= src[j++];

            switch (value) {
            case 0xEA:
                buffer = func_80014E0C(arg, buffer);
                break;

            case 0xEB:
                buffer = func_80014D58(buffer, SysKernGetString(4, arg, 8), -1);
                break;

            case 0xEC:
                // Needs to produce at least one digit, so a do-while fits here
                i = 0;
                do {
                    digits[i++] = arg % 10;
                    arg /= 10;
                } while (arg > 0 && i < MAX_DIGITS);

                if (i > 0) {
                    do {
                        *buffer++ = digits[i - 1] + g_FFTextNumberOffset;
                    } while (--i > 0);
                }
                break;

            case 0xED:
                if (arg < NUM_PARTY) {
                    buffer = func_80014E0C(g_BattleData.actors[arg].charId, buffer);
                } else if (arg >= START_ENEMY) {
                    buffer = func_80014D58(
                        buffer,
                        g_BattleSceneContext.enemy[g_BattleData.activeEncounter.formation[arg - START_ENEMY].enemyID]
                            .name,
                        0x20);
                }

                break;

            case 0xEE:
                buffer = func_80014DD0(9, arg, buffer);
                break;

            case 0xEF:
                if (arg < 26) {
                    *buffer++ = arg + g_FFTextLetterOffset;
                }
                break;

            case 0xF0:
                buffer = func_80014DD0(16, arg, buffer);
                break;

            case 0xF1:
                buffer = func_80014DD0(arg >> 8, arg & 0xFF, buffer);
                break;
            }
        } else {
            *buffer++ = value;
            if (value == 0xF9) {
                *buffer++ = src[j++];
            }
        }
    }
    return dst;
}

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
