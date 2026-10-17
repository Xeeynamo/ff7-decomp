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
    // dataOffsets: AttackData base index used in func_80014CBC
    {
        /* 0x00 */ MAGIC_START,       // Magic
        /* 0x01 */ SUMMON_START,      // Summons
        /* 0x02 */ ENEMY_SKILL_START, // Enemy skill
    },
    {0, 128, 256, 288, 384, 65535},
    {4, 10, 11, 12, 13},
    {0, 0, 0},
    // magicTypeOffsets: Attack-name base index for types 0–3
    {
        /* 0x00 */ MAGIC_START,       // Magic
        /* 0x01 */ SUMMON_START,      // Summons
        /* 0x02 */ ENEMY_SKILL_START, // Enemy skill
        /* 0x03 */ 128                // Not sure, limit breaks maybe?
    },
    // typeToSection: Corresponds to the type used in SysKernGetString
    {/* 0x00 */ KERNEL_TEXT_DESC_MAGIC,
     /* 0x01 */ KERNEL_TEXT_DESC_MAGIC,
     /* 0x02 */ KERNEL_TEXT_DESC_MAGIC,
     /* 0x03 */ KERNEL_TEXT_DESC_MAGIC,
     /* 0x04 */ KERNEL_TEXT_DESC_ITEM,
     /* 0x05 */ KERNEL_TEXT_DESC_COMMAND,
     /* 0x06 */ KERNEL_TEXT_INVALID,
     /* 0x07 */ KERNEL_TEXT_INVALID,
     /* 0x08 */ KERNEL_TEXT_INVALID,
     /* 0x09 */ KERNEL_TEXT_INVALID,
     /* 0x0A */ KERNEL_TEXT_DESC_WEAPON,
     /* 0x0B */ KERNEL_TEXT_DESC_ARMOR,
     /* 0x0C */ KERNEL_TEXT_DESC_ACCESSORY,
     /* 0x0D */ KERNEL_TEXT_DESC_MATERIA,
     /* 0x0E */ KERNEL_TEXT_DESC_KEY_ITEM,
     /* 0x0F */ KERNEL_TEXT_DESC_COMMAND}, // unused/padding?
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

            case BATTLE_MSG_ARG_ITEM_NAME:
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
                    cursor = SysAppendString(cursor, g_BattleWork.enemy[enemyId].name, LEN(g_BattleWork.enemy[0].name));
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

extern u8 D_80063560[0x100];

u8* SysDecompKernStringWithF9(u8* src, u8* dst) {
    u8 value;
    s32 pos;
    s32 len;
    s32 count;
    u32 i;
    s32 off;

    i = 0;
    pos = 0;
    do {
        value = *src++;
        pos++;
        if (value == 0xF9) {
            value = *src++;
            pos++;
            off = pos - 3;
            off -= value & 0x3F;
            len = (value >> 6) * 2 + 4;
            for (count = 0; count < len; count++) {
                value = dst[off++];
                D_80063560[i++] = value;
            }
        } else if (value >= BATTLE_MSG_ARG_START && value <= BATTLE_MSG_ARG_END) {
            D_80063560[i++] = value;
            pos++;
            D_80063560[i++] = *src++;
            pos++;
            D_80063560[i++] = *src++;
        } else {
            D_80063560[i++] = value;
        }
    } while (value != 0xFF && i < 0x100);
    return D_80063560;
}

u8* SysGetKernBattleTextPtr(s32 textId) { return SysGetKernTextPtr(KERNEL_TEXT_BATTLE_MESSAGES, textId, 0); }

s32 SysGetKernBattleTextById(s32 textId) {
    u8* tmpBuf = SysGetKernBattleTextPtr(textId);
    return SysDecompKernStringWithF9(tmpBuf, tmpBuf);
}

extern u8 D_80063660;

// Returns a pointer to an 0xFF-terminated string
// type: Which string is returned depends on the type
// index: Meaning changes depending on the type
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
        for (i = 0; i < sizeof(kernel_maps.itemToType); i++) {
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

        if (kernel_maps.typeToSection[type] != KERNEL_TEXT_INVALID) {
            result = SysGetKernTextPtr(kernel_maps.typeToSection[type] + blockOffset, index, 0);
            if (blockOffset == 0) {
                result = SysDecompKernStringWithF9(result, result);
            }
        } else {
            switch (type) {
            case 6: // index is used as an entryId here
                blockId = KERNEL_TEXT_NAME_MAGIC;
                if (index < NUM_SUMMONS) {
                    blockId = KERNEL_TEXT_NAME_SUMMON;
                }
                result = SysGetKernTextPtr(blockId, index, 0);
                break;

            case 7: // Sense command formatting, index maps to unit slot here
                if (index >= NUM_ENEMY) {
                    break;
                }

                slot = index + START_ENEMY;
                str = SysAppendString(
                    &D_80063660, g_BattleWork.enemy[g_BattleData.activeEncounter.formation[index].enemyID].name, 0x20);

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
                // 0x100 seems to be the string buffer base
                if (index >= 0x100) {
                    str = BattleGetStringPtrFromStringBuffer(index - 0x100);
                } else {
                    str = SysGetKernBattleTextPtr(index);
                }
                result = SysDecompKernStringWithF9(SysExpandBattleString(buffer, str), str);
                break;

            case 9:
                result = g_BattleWork.attackNames[index];
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

void SysAddBattleEarnedItems(void) {
    s32 i;
    s32 j;
    s32 freeSlot;
    s16 id;
    s32 quantity;
    u16 entry;

    for (i = 0; i < LEN(g_BattleItemsEarned); i++) {
        if ((u16)g_BattleItemsEarned[i].enabled == 0) {
            continue;
        }
        id = g_BattleItemsEarned[i].id;
        if (id == -1) {
            continue;
        }
        quantity = g_BattleItemsEarned[i].quantity;
        freeSlot = -1;
        if (id < 0x180) {
            for (j = 0; j < MAX_INVENTORY_COUNT; j++) {
                entry = Savemap.inventory[j];
                if ((entry & 0x1FF) == id) {
                    quantity += entry >> 9;
                    if (quantity > 99) {
                        quantity = 99;
                    }
                    Savemap.inventory[j] = id | (quantity << 9);
                    break;
                }
                if (entry == 0xFFFF && freeSlot == -1) {
                    freeSlot = j;
                }
            }
            if (j == MAX_INVENTORY_COUNT && freeSlot != -1) {
                Savemap.inventory[freeSlot] = id | (quantity << 9);
            }
        } else {
            // scans 0x100 slots, past MAX_MATERIA_COUNT (200)
            for (j = 0; j < 0x100; j++) {
                if (Savemap.materia[j] != -1) {
                    continue;
                }
                quantity--;
                Savemap.materia[j] = id - 0x180;
                if (!quantity) {
                    break;
                }
            }
        }
    }
}

s32 SysGetLimitCmdId(s32 charId, s32 limitIndex) {
    s32 unused[12]; // unknown locals
    s32 cmdId;

    cmdId = 0x7F;
    if ((u32)charId < NUM_CHARACTERS) {
        s32 addr;

        addr = (s32)D_80082268[charId].limitCmdId;
        cmdId = *(u8*)(limitIndex + addr) - 0x80;
    }
    return cmdId;
}
