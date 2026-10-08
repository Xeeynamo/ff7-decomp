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

s32 D_80062D50 = 0x000000FF; // String terminator
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

// Copies src to dst up to (but not including) the 0xFF terminator, stopping
// after limit + 1 bytes (-1 = no limit). Returns the new end of dst
static u8* SysAppendString(u8* dst, const u8* src, s32 limit) {
    while (*src != 0xFF) {
        *dst++ = *src++;
        if (--limit == -1) {
            break;
        }
    }
    return dst;
}

u8* SysGetKernTextPtr(s32 blockId, s32 entryId, s32 blockOffset) {
    u8* sectionBase = g_KernelTextBuffer + g_KernelTextBlockOffsets[blockId + blockOffset];
    return (u8*)&sectionBase[*(u16*)&sectionBase[entryId * 2]];
}

static u8* SysKernAppendText(s32 blockId, s32 entryId, u8* dst) {
    return SysAppendString(dst, SysGetKernTextPtr(blockId, entryId, 0), -1);
}

static u8* SysAppendCharName(s32 charId, u8* dst) {
    s32 i;

    for (i = 0; i < NUM_CHARACTERS; i++) {
        if (Savemap.party[i].char_id == charId) {
            dst = SysAppendString(dst, Savemap.party[i].name, LEN(Savemap.party[i].name));
            break;
        }
    }
    return dst;
}

#define MAX_DIGITS 16U // Needs to be unsigned for loop condition
u8* SysExpandBattleString(u8* dst, const u8* src) {
    s32 digits[MAX_DIGITS];
    u8* cursor = dst;
    u8 value = 0;
    s32 pos = 0;
    s32 i;

    while (value != 0xFF) {
        value = src[pos++];

        if (value >= BATTLE_MSG_ARG_START && value <= BATTLE_MSG_ARG_END) {
            u16 arg = src[pos++] << 8;
            arg |= src[pos++];

            switch (value) {
            case BATTLE_MSG_ARG_CHAR_NAME:
                cursor = SysAppendCharName(arg, cursor);
                break;

            case BATTLE_MSG_ARG_UNK_EB:
                cursor = SysAppendString(cursor, SysKernGetString(4, arg, 8), -1);
                break;

            case BATTLE_MSG_ARG_NUMBER:
                // Needs to produce at least one digit, so a do-while fits here
                i = 0;
                do {
                    digits[i++] = arg % 10;
                    arg /= 10;
                } while (arg > 0 && i < MAX_DIGITS);

                if (i > 0) {
                    do {
                        *cursor++ = digits[i - 1] + g_FFTextNumberOffset;
                    } while (--i > 0);
                }
                break;

            case BATTLE_MSG_ARG_UNIT_NAME:
                if (arg < NUM_PARTY) {
                    cursor = SysAppendCharName(g_BattleData.actors[arg].charId, cursor);
                } else if (arg >= START_ENEMY) {
                    s16 enemyId = g_BattleData.activeEncounter.formation[arg - START_ENEMY].enemyID;
                    cursor = SysAppendString(
                        cursor, g_BattleSceneContext.enemy[enemyId].name, LEN(g_BattleSceneContext.enemy[0].name));
                }

                break;

            case BATTLE_MSG_ARG_MAGIC_NAME:
                cursor = SysKernAppendText(KERNEL_TEXT_NAME_MAGIC, arg, cursor);
                break;

            case BATTLE_MSG_ARG_ENEMY_LETTER:
                if (arg < 26) { // A-Z
                    *cursor++ = arg + g_FFTextLetterOffset;
                }
                break;

            case BATTLE_MSG_ARG_BATTLE_TEXT:
                cursor = SysKernAppendText(KERNEL_TEXT_BATTLE_MESSAGES, arg, cursor);
                break;

            case BATTLE_MSG_ARG_KERNEL_TEXT:
                cursor = SysKernAppendText(arg >> 8, arg & 0xFF, cursor);
                break;
            }
        } else {
            *cursor++ = value;
            if (value == 0xF9) {
                *cursor++ = src[pos++];
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

extern u8 D_80063660;

// blockOffset values when type maps to a valid section:
//  0: Resolves the desc of the respective KERNEL_TEXT_DESC_* blockId entry
//  8: Resolves the name of the respective KERNEL_TEXT_NAME_* blockId entry
const char* SysKernGetString(s32 type, s32 index, s32 blockOffset) {
    u8 buffer[0x100];
    u8* result;
    u8* str;
    s32 blockId;
    s32 slot;
    s32 i;

    result = (u8*)&D_80062D50;

    if (type == 4) {
        for (i = 0; i < 5U; i++) {
            if (index < kernel_maps.itemOffsets[i + 1]) {
                type = kernel_maps.itemToType[i];
                index -= kernel_maps.itemOffsets[i];
                break;
            }
        }
    }

    if ((type == 3) && (index == 0x7F)) {
        index = 0xFF;
    }

    if (index != 0xFF) {
        if (type < 4U && index + kernel_maps.magicTypeOffsets[type] < 0xE0) {
            index += kernel_maps.magicTypeOffsets[type];
        }

        if (kernel_maps.typeToSection[type] != 0xFF) {
            result = SysGetKernTextPtr(kernel_maps.typeToSection[type] + blockOffset, index, 0);
            if (blockOffset == 0) {
                result = SysDecompKernStringWithF9(result, result);
            }
        } else {
            switch (type) {
            case 6:
                blockId = KERNEL_TEXT_NAME_MAGIC;
                if (index < NUM_SUMMONS) {
                    blockId = KERNEL_TEXT_NAME_SUMMON;
                }
                result = SysGetKernTextPtr(blockId, index, 0);
                break;

            case 7: // Sense command formatting
                if (index >= NUM_ENEMY) {
                    break;
                }

                slot = index + START_ENEMY;
                str = SysAppendString(
                    &D_80063660, g_BattleSceneContext.enemy[g_BattleData.activeEncounter.formation[index].enemyID].name,
                    0x20);

                if (g_BattleWork.turn[slot].formationIndex != 0xFF) {
                    *str++ = g_BattleWork.turn[slot].formationIndex + g_FFTextLetterOffset;
                }

                if (g_BattleState.combatant[slot].stateFlags & COMBATANT_BACK_ROW) {
                    str = SysAppendString(str, SysGetKernBattleTextById(0x71), -1);
                }

                if (g_BattleWork.turn[slot].turnFlags & 0x40) {
                    u16 strArgs[2];
                    strArgs[0] = g_BattleWork.turn[slot].prevHP;
                    strArgs[1] = g_BattleState.combatant[slot].maxHP;
                    str = SysAppendString(str, SysGetKernBattleTextById(0x7F), -1);
                    BattleCopyMessageWithArgs(str, SysGetKernBattleTextById(0x72), strArgs);
                    SysExpandBattleString(buffer, str);
                    str = SysAppendString(str, buffer, -1);
                }
                *str = 0xFF;
                result = &D_80063660;
                break;

            case 8:
                if (index >= 0x100) {
                    str = BattleGetStringPtrFromStringBuffer(index - 0x100);
                } else {
                    str = SysGetKernBattleTextPtr(index);
                }
                result = SysDecompKernStringWithF9(SysExpandBattleString(buffer, str), str);
                break;

            case 9:
                result = g_BattleSceneContext.attackNames[index];
                break;
            }
        }
    }
    return result;
}

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
