//! PSYQ=3.3 CC1=2.7.2 G=8
#include "game.h"
#include "main_private.h"

s32 D_80062DCC = 0x00000000;
s32 D_80062DD0 = 0x00000000;
s32 D_80062DD4 = 0x00000000;
s16 g_RewardMenuState = 0x0000;
u8 D_80062DDA = 0x00;
u8 D_80062DDB = 0x00;
u8 D_80062DDC = 0x02;
static s8 _D_80062DDD = 0x00;
static s8 _D_80062DDE = 0x00;
static s8 _D_80062DDF = 0x00;
s32 D_80062DE0 = 0x00000000;
u8 D_80062DE4 = 0x00;
u8 D_80062DE5 = 0x00;
s16 D_80062DE6 = 0x00B4;
s16 D_80062DE8 = 0x0068;
s16 D_80062DEA = 0x0000;
s32 D_80062DEC = 0x801D0000;
s32 D_80062DF0 = 0x00000084;
s32 D_80062DF4 = 0xFFFFFFFF;
s32 g_PartyMenuListState = 0x00000001;

// clang-format off
MainMenuColorLabels g_Labels = {
    {0x69, 0x99, 0x53, 0xFF}, // not sure if these are actually colors or is it a JP unused string
    {
        _SL(12, "Item"),
        _SL(12, "Magic"),
        _SL(12, "Materia"),
        _SL(12, "Equip"),
        _SL(12, "Status"),
        _SL(12, "Order"),
        _SL(12, "Limit"),
        _SL(12, "Config"),
        _SL(12, "PHS"),
        _SL(12, "Save"),
        _SL(12, "Uso"),
        _SL(12, "Beginner"),
        _SL(12, "Uso"),
        _SL(12, "Uso"),
        _SL(12, "Time"),
        _SL(12, "Gil"),
        _SL(12, "next level"),
        _SL(12, "Limit level"),
        _SL(12, "Tutorial"),
        _SL(12, "Under"),
        _SL(12, "LEVEL UP"),
        _SL(12, "Fury"),
        _SL(12, "Sadness"),
    },
};
// clang-format on

// kernel tables that hold equipment and materia records
static void* D_8004935C[5] = {
    D_800722CC, g_WeaponTable, g_ArmorTable, g_AccessoryTable, g_MateriaData,
};

static s32 D_80049370[5] = {0xE00, 0x1600, 0x480, 0x200, 0x7D0};
static u8 D_80049384[12] = {
    0x70, 0x00, 0xB0, // RGB for something
    0x70, 0x00, 0x80, // RGB for something
    0x70, 0x00, 0x50, // RGB for something
    0x70, 0x00, 0x20, // RGB for something
};
static u8 D_80049390[3][8] = {
    {0xBB, 0xC3, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xC0, 0xC3, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xBF, 0xC9, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00},
};

s32 g_RewardMenuHasEarnedItems;
u_long* g_CurrentMenuOrderingTable;
s32 D_80062EA8;
s32 D_80062EAC;
s32 D_80062EB0;
u32 D_80062EB4;
u8* D_80062EB8;
u8 D_80062EBC;
u32 D_80062EC0;
u32 D_80062EC4;
u32 D_80062EC8;
s32 D_80062ECC;

u8 SysMenuIsWindowActive(void) { return D_80062DDB; }

void SysMenuRequestAddWindow(u8* arg0, s8 arg1) {
    D_80062DDB = 1;
    D_80062DDC = arg1;
    D_80062DE0 = 0x28;
    D_80062EB8 = arg0;
    D_80062DE5 = 1;
}

void SysMenuSetPosAddWindow(s16 arg0, s16 arg1, s16 arg2) {
    D_80062DE4 = arg0;
    if (arg0) {
        D_80062DE6 = arg1;
        D_80062DE8 = arg2;
    } else {
        D_80062DDB = 0;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuDrawAddWindow);

static void SysMenuSound(u16 arg0) {
    g_AkaoCmd.opcode = AKAO_PLAY_MENU_SOUND;
    g_AkaoCmd.params[0] = arg0;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
}

static void func_8001FA68(u16 arg0) {
    g_AkaoCmd.opcode = 0x28;
    g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
}

static void func_8001FAAC(u16 arg0) {
    g_AkaoCmd.opcode = AKAO_PLAY_SLOT1;
    g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
}

static void func_8001FAF0(void) {}

u16 SysMenuGetInventoryRestrictionMask(s32 itemId) {
    if (itemId < 0x80) {
        return D_800722CC[itemId].cameraMultiID;
    }
    if (itemId < 0x100) {
        return g_WeaponTable[itemId - 0x80].restrictionMask;
    }
    if (itemId < 0x120) {
        return g_ArmorTable[itemId - 0x100].restrictionMask;
    }
    return g_AccessoryTable[itemId - 0x120].restrictionMask;
}

s32 SysGetPlayerBaseAttackDefense(s32 partyIdx, s32 kind, s32 fallback) {
    s32 charId;

    charId = g_BattleCharIdToCharId[Savemap.partyID[partyIdx]];
    switch (kind) {
    case 0:
        fallback = g_WeaponTable[Savemap.party[g_BattleCharIdToCharId[charId]].weapon].attack;
        break;
    case 1:
        fallback = g_ArmorTable[Savemap.party[g_BattleCharIdToCharId[charId]].armor].defense;
        break;
    case 2:
        fallback = 0;
        break;
    case 3:
        fallback = 0;
        break;
    }
    return fallback;
}

void SysAddStats(s32 charId, s32 statId, u8 amount) {
    switch (statId) {
    case 0:
        g_ActiveCharacters[charId].strength += amount;
        break;
    case 1:
        g_ActiveCharacters[charId].vitality += amount;
        break;
    case 2:
        g_ActiveCharacters[charId].magic += amount;
        break;
    case 3:
        g_ActiveCharacters[charId].spirit += amount;
        break;
    case 4:
        g_ActiveCharacters[charId].dexterity += amount;
        break;
    case 5:
        g_ActiveCharacters[charId].luck += amount;
        break;
    }
}

void SysAddElementalDefense(s32 charId, s32 effect, u16 mask) {
    switch (effect) {
    case 0:
        g_ActiveCharacters[charId].absorbedElements |= mask;
        break;
    case 1:
        g_ActiveCharacters[charId].nullifiedElements |= mask;
        break;
    case 2:
        g_ActiveCharacters[charId].halvedElements |= mask;
        break;
    }
}

void SysAddAttackType(s32 charId, u16 element) { g_ActiveCharacters[charId].physicalAttackElements |= element; }

void SysAddStatusAttackBit(s32 charId, s32 statusId) {
    if (statusId < 0x41) {
        g_ActiveCharacters[charId].physicalAttackStatuses |= 1 << statusId;
    }
}

void SysAddStatusProtectBit(s32 charId, s32 arg1) {
    if (arg1 < 0x41) {
        g_ActiveCharacters[charId].immuneStatuses |= 1 << arg1;
    }
}

void SysAddStatusProtect(s32 charId, s32 arg1) { g_ActiveCharacters[charId].immuneStatuses |= arg1; }

void SysInitPlayerStatFromEquip(s32 charId) {
    ArmorRecord* armor;
    AccessoryRecord* accessory;
    s32 battleCharId;
    s32 i;
    s32 strength;
    s32 dexterity;
    s32 vitality;
    s32 spirit;
    s32 magic;
    s32 luck;

    if (Savemap.partyID[charId] == 0xFF) {
        return;
    }

    battleCharId = g_BattleCharIdToCharId[Savemap.partyID[charId]];

    g_ActiveCharacters[charId].physicalAttackElements = 0;
    g_ActiveCharacters[charId].halvedElements = 0;
    g_ActiveCharacters[charId].nullifiedElements = 0;
    g_ActiveCharacters[charId].absorbedElements = 0;
    g_ActiveCharacters[charId].physicalAttackStatuses = 0;
    g_ActiveCharacters[charId].immuneStatuses = 0;
    g_ActiveCharacters[charId].weapon = g_WeaponTable[Savemap.party[battleCharId].weapon];
    g_ActiveCharacters[charId].hp = Savemap.party[battleCharId].curHP;
    g_ActiveCharacters[charId].mp = Savemap.party[battleCharId].curMP;
    g_ActiveCharacters[charId].baseHp = Savemap.party[battleCharId].hp_base;
    g_ActiveCharacters[charId].baseMp = Savemap.party[battleCharId].mp_base;
    g_ActiveCharacters[charId].strength = 0;
    g_ActiveCharacters[charId].dexterity = 0;
    g_ActiveCharacters[charId].vitality = 0;
    g_ActiveCharacters[charId].spirit = 0;
    g_ActiveCharacters[charId].magic = 0;
    g_ActiveCharacters[charId].luck = 0;

    for (i = 0; i < 4; i++) {
        SysAddStats(charId, g_ActiveCharacters[charId].weapon.statBonusId[i],
                    g_ActiveCharacters[charId].weapon.statBonusValue[i]);
    }
    SysAddAttackType(charId, g_ActiveCharacters[charId].weapon.attackElement);
    SysAddStatusAttackBit(charId, g_ActiveCharacters[charId].weapon.statusAttack);

    armor = SysGetArmorAddressById(Savemap.party[battleCharId].armor);
    for (i = 0; i < 4; i++) {
        SysAddStats(charId, armor->statBonusId[i], armor->statBonusValue[i]);
    }
    SysAddElementalDefense(charId, armor->elementalEffect, armor->elementalMask);
    SysAddStatusProtectBit(charId, armor->statusDefense);

    if (Savemap.party[battleCharId].accessory != 0xFF) {
        accessory = SysGetAccessoryAddressById(Savemap.party[battleCharId].accessory);
        for (i = 0; i < 2; i++) {
            SysAddStats(charId, accessory->statBonusId[i], accessory->statBonusValue[i]);
        }
        SysAddElementalDefense(charId, accessory->elementalStrength, *(u16*)accessory->elementMask);
        SysAddStatusProtect(charId, *(u32*)accessory->statusProtect);
    }

    strength = (Savemap.party[battleCharId].strength + Savemap.party[battleCharId].strength_bonus) +
               g_ActiveCharacters[charId].strength;
    dexterity = (Savemap.party[battleCharId].dexterity + Savemap.party[battleCharId].dexterity_bonus) +
                g_ActiveCharacters[charId].dexterity;
    vitality = (Savemap.party[battleCharId].vitality + Savemap.party[battleCharId].vitality_bonus) +
               g_ActiveCharacters[charId].vitality;
    spirit = (Savemap.party[battleCharId].spirit + Savemap.party[battleCharId].spirit_bonus) +
             g_ActiveCharacters[charId].spirit;
    magic = (Savemap.party[battleCharId].magic + Savemap.party[battleCharId].magic_bonus) +
            g_ActiveCharacters[charId].magic;
    luck =
        (Savemap.party[battleCharId].luck + Savemap.party[battleCharId].luck_bonus) + g_ActiveCharacters[charId].luck;

    if ((Savemap.party[battleCharId].accessory + 0x120) == 0x139) {
        // Curse Ring stat boost
        strength += 15;
        dexterity += 15;
        vitality += 15;
        spirit += 15;
        magic += 15;
        luck += 10;
    }

    if (strength > 255) {
        strength = 255;
    }
    if (dexterity > 255) {
        dexterity = 255;
    }
    if (vitality > 255) {
        vitality = 255;
    }
    if (spirit > 255) {
        spirit = 255;
    }
    if (magic > 255) {
        magic = 255;
    }
    if (luck > 255) {
        luck = 255;
    }

    g_ActiveCharacters[charId].strength = strength;
    g_ActiveCharacters[charId].dexterity = dexterity;
    g_ActiveCharacters[charId].vitality = vitality;
    g_ActiveCharacters[charId].spirit = spirit;
    g_ActiveCharacters[charId].magic = magic;
    g_ActiveCharacters[charId].luck = luck;
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuDrawCharNameLvHpMpBySaveCharId);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuDrawCharNameLvHpMpByPartyId);

void SysMenuCreateDrawenvDispenv(DRAWENV* draw_env, DISPENV* disp_env) {
    VSync(0);
    SetDefDrawEnv(draw_env, 0, 0, 0x180, 0x1D8);
    draw_env[0].dfe = 1;
    draw_env[0].isbg = 1;
    PutDrawEnv(draw_env);
    VSync(0);
    SetDefDrawEnv(draw_env, 0, 8, 0x180, 0xE0);
    SetDefDrawEnv(&draw_env[1], 0, 0xF0, 0x180, 0xE0);
    SetDefDispEnv(&disp_env[0], 0, 0xE8, 0x16C, 0xF0);
    SetDefDispEnv(&disp_env[1], 0, 0, 0x16C, 0xF0);
    draw_env[1].isbg = 1;
    draw_env[0].isbg = 1;
    draw_env[1].dfe = 1;
    draw_env[0].dfe = 1;
    draw_env[1].dtd = 1;
    draw_env[0].dtd = 1;
    draw_env[0].r0 = 0;
    draw_env[0].g0 = 0;
    draw_env[0].b0 = 0;
    draw_env[1].r0 = 0;
    draw_env[1].g0 = 0;
    draw_env[1].b0 = 0;
    draw_env[0].tpage = draw_env[1].tpage = GetGraphType() != 1 && GetGraphType() != 2 ? 0x3F : 0xAF;
    VSync(0);
    PutDispEnv(disp_env);
    PutDrawEnv(draw_env);
    SetDispMask(1);
}

static void func_800211B8(s32 arg0) { D_80062DEC = arg0; }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800211C4);

void SysMenuLoadMenuFileById(s32 yamaOvlId) {
    s32 prev;

    prev = D_80062DD4;
    D_80062DD4 = yamaOvlId;
    D_80062DD0 = prev;
    if (yamaOvlId != 0 && (prev < 3 || prev > 4 || yamaOvlId < 3 || yamaOvlId > 4)) {
        func_800211C4(yamaOvlId);
    }
}

static const char* func_80021258(s32 arg0) { return SysKernGetString(13, arg0, 8); }

static void func_80021280(s32 arg0) { SysKernGetString(4, arg0, 8); }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800212A8);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021BAC);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021C4C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021D5C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021E70);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021F58);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuDrawBattleResult);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80022DE4);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80022FE0);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuGetMenuListState);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuSetMenuListAnimation);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuDrawMenuList);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuInitInput);

s32 SysGetHoursFromSeconds(s32 arg0) {
    if (arg0 > 359999) { // clamp to 99:59:59, in seconds
        arg0 = 359999;
    }
    return (arg0 / D_80049474[0]) * 10 + (arg0 % D_80049474[0]) / D_80049474[1];
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysGetMinutesFromSeconds);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysGetSecondsFromSeconds);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuClose);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuDrawMainMenu);

// Push the current display and draw environments to the GPU: the per-frame
// double-buffer flip (activate the finished buffer for scanout, point drawing
// at the other one).
static void func_80024A04(void) {
    PutDispEnv(&D_8007075C[0]);
    PutDrawEnv(&D_800706A4[1]);
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuShow);

typedef enum {
    YAMA_1 = 1,
    YAMA_10 = 10,
    YAMA_12 = 12,
    YAMA_13,
    YAMA_14,
    YAMA_15,
    YAMA_SAVEMENU,
} YamadaOvl;

void func_80024D88(s32 arg0) {
    func_800211C4(YAMA_13);
    do {
    } while (SystemCdromReadChain());
    VSync(30);
    func_801D131C(arg0);
}

void func_80024DD4(s32 arg0) {
    func_800211C4(YAMA_14);
    do {
    } while (SystemCdromReadChain());
    func_801D1A6C(arg0);
}

void func_80024E18(s32 arg0) {
    func_800211C4(YAMA_15);
    do {
    } while (SystemCdromReadChain());
    func_801D4118(arg0);
}

int HandleTitle(void) {
    func_800211C4(YAMA_SAVEMENU);
    do {
    } while (SystemCdromReadChain());
    TitleMain();
}

void HandleSaveMenu(void) {
    func_800211C4(YAMA_10);
    do {
    } while (SystemCdromReadChain());
    SaveMain();
}

void func_80024ECC(void) {
    func_800211C4(YAMA_1);
    do {
    } while (SystemCdromReadChain());
    func_801D2D74();
}

void func_80024F04(void) {
    func_800211C4(YAMA_1);
    do {
    } while (SystemCdromReadChain());
    func_801D2E84();
}

void func_80024F3C(s32 arg0) {
    func_800211C4(YAMA_1);
    do {
    } while (SystemCdromReadChain());
    func_801D2F00(arg0);
}

void func_80024F80(s32 arg0) {
    func_800211C4(YAMA_1);
    do {
    } while (SystemCdromReadChain());
    func_801D3138(arg0);
}

void func_80024FC4(s32 arg0) {
    func_800211C4(YAMA_1);
    do {
    } while (SystemCdromReadChain());
    func_801D3018(arg0);
}

static void func_80025008(void) {
    func_800211C4(YAMA_1);
    do {
    } while (SystemCdromReadChain());
    func_801D3228();
}

// MENU event 0x18: snapshot each present party member's level into
// Savemap.memory_bank_2+0xC4. The endgame battle AI (Jenova-SYNTHESIS) counts how many of
// these are 99 to scale Safer-Sephiroth's HP.

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SnapshotPartyLevels);
#else
void SnapshotPartyLevels(void) {
    s32 i;
    u16* present;
    for (i = 0, present = &Savemap.phs_visibility_mask; i < 8; i++) {
        if ((*present >> D_80049500[i]) & 1) {
            Savemap.memory_bank_2[0xC4 + i] = Savemap.party[D_80049500[i]].level;
        }
    }
}
#endif

void func_800250B4(void) {
    func_800211C4(YAMA_12);
    do {
    } while (SystemCdromReadChain());
    ScalePartyHp();
}

void func_800250EC(s32 arg0) {
    func_800211C4(YAMA_12);
    do {
    } while (SystemCdromReadChain());
    func_801D05C4(arg0);
}

void func_80025130(s32 arg0) {
    func_800211C4(YAMA_12);
    do {
    } while (SystemCdromReadChain());
    func_801D0704(arg0);
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025174);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuRemoveItem);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuSearchItem);

static void func_80025360() { SysMenuSound(SFX_19F); }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuAddItem);

s32 SysMenuAddMateria(s32 arg0) {
    s32 i;
    for (i = 0; i < MAX_MATERIA_COUNT; i++) {
        if (Savemap.materia[i] == -1) {
            Savemap.materia[i] = arg0;
            if (SysMenuGetMateriaColorByType(arg0 & 0xFF) == 10) {
                Savemap.memory_bank_1[75] |= 1;
            }
            if ((arg0 & 0xFF) == 44) {
                Savemap.memory_bank_1[75] |= 2;
            }
            return -1;
        }
    }
    return arg0;
}

void func_800254D8(void) { D_80062EBC = 0; }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800254E4);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025514);

void SysMenuRemoveMateria(void) {}

void func_80025650(void) {}

// get party leader (Cloud) level
s32 func_80025658() { return Savemap.party[0].level; }

// Party slot -> equipped character -> that character's equipped armor's
// materia-slot configuration (slot count / linked-pair layout / growth rate;
// see ArmorRecord in main_private.h). Returns sentinel (void*)0xFF for an
// empty party slot.
u8* GetPartySlotArmorMateriaSlots(s32 arg0) {
    u8 temp_v1;
    u8* var_v0;

    temp_v1 = Savemap.partyID[arg0];
    var_v0 = (u8*)0xFF;
    if (temp_v1 != 0xFF) {
        u32 idx = g_BattleCharIdToCharId[temp_v1];
        var_v0 = g_ArmorTable[Savemap.party[idx].armor].materiaSlot;
    }
    return var_v0;
}

// Weapon counterpart of GetPartySlotArmorMateriaSlots: party slot -> equipped
// character -> that character's equipped weapon's materia-slot configuration
// (see WeaponRecord in main_private.h). Returns sentinel (void*)0xFF for an
// empty party slot.
static void* GetPartySlotWeaponMateriaSlots(s32 arg0) {
    u8 temp_v1;
    void* var_v0;

    temp_v1 = Savemap.partyID[arg0];
    var_v0 = (void*)0xFF;
    if (temp_v1 != 0xFF) {
        u32 idx = g_BattleCharIdToCharId[temp_v1];
        var_v0 = g_WeaponTable[Savemap.party[idx].weapon].materiaSlot;
    }
    return var_v0;
}

ArmorRecord* SysGetArmorAddressById(s32 armorId) { return &g_ArmorTable[armorId]; }

AccessoryRecord* SysGetAccessoryAddressById(s32 accessoryId) { return &g_AccessoryTable[accessoryId]; }

ActiveCharacterData* SysGetPartyPlayerStructureAddressByPartyId(s32 partyId) {
    if (Savemap.partyID[partyId] != 0xFF) {
        return &g_ActiveCharacters[partyId];
    }
    return (ActiveCharacterData*)0xFF;
}

static void func_800257C4(void) {}

u8* GetCharacterName(s32 battleCharId) { return Savemap.party[g_BattleCharIdToCharId[battleCharId]].name; }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuRemoveHpByPartyId);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SystemMenuAddHpByPartyId);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SysMenuRemoveMpByPartyId);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SystemMenuAddMpByPartyId);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SystemMenuRemovePartyGold);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SystemMenuAddPartyGold);

s32 SystemMenuGetPartyGold(void) { return Savemap.gil; }

void SysMenuStoreAvatarVram(u_long* image) {
    RECT rect;
    rect.x = 0x340;
    rect.y = 0x184;
    rect.w = 0x30;
    rect.h = 0x78;
    StoreImage(&rect, image);
}

void SysMenuRestoreAvatarVram(u_long* image) {
    RECT rect;
    rect.x = 0x340;
    rect.y = 0x184;
    rect.w = 0x30;
    rect.h = 0x78;
    LoadImage(&rect, image);
}

void SysMenuStoreFontVram(u_long* image) {
    RECT rect;
    rect.x = 0x180;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 9;
    StoreImage(&rect, image);
}

void SysMenuRestoreFontVram(u_long* image) {
    RECT rect;
    rect.x = 0x180;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 9;
    LoadImage(&rect, image);
}

void SysMenuLoadCharacterClutFromRam(u_long* image) {
    RECT rect;
    rect.x = 0x100;
    rect.y = 0x1ED;
    rect.w = 0x100;
    rect.h = 3;
    LoadImage(&rect, image);
}

void SysMenuStoreCharacterClutToRam(u_long* image) {
    RECT rect;
    rect.x = 0x100;
    rect.y = 0x1ED;
    rect.w = 0x100;
    rect.h = 3;
    StoreImage(&rect, image);
}

void MENU_LoadTim(u_long* addr, s32 px, s32 py, s32 cx, s32 cy) {
    TIM_IMAGE tim;
    OpenTIM(addr);
    while (ReadTIM(&tim)) {
        if (tim.caddr) {
            tim.crect->x = cx;
            tim.crect->y = cy;
            LoadImage(tim.crect, tim.caddr);
            DrawSync(0);
        }
        if (tim.paddr) {
            tim.prect->x = px;
            tim.prect->y = py;
            LoadImage(tim.prect, tim.paddr);
            DrawSync(0);
        }
    }
}

// this function seems to be responsible of loading the characters' portrait
void SysMenuLoadAvatars(void) {
    u8 dummy[8];
    u8 buf[0x1000];
    u_long* dst;
    s32 i;
    s32* sector_off;
    s32* length;
    s32 cx, cy;

    i = 0;
    dst = (u_long*)buf;
    sector_off = &D_80048FE8->loc;
    length = &D_80048FE8->len;
    for (; i < NUM_CHARACTERS; i++) {
        SysCdromLoadFile(sector_off[i * 2], length[i * 2], dst, 0);
        cx = 0x340 + (i / 5) * 24;
        cy = 0x100 + (i % 5) * 48;
        MENU_LoadTim(dst, cx, cy, 0x180, i);
        DrawSync(0);
    }
}

void SysMenuLoadPartyPortraits(void) {
    RECT rect;
    s32 i;
    s32 portraitId;

    DrawSync(0);
    for (i = 0; i < NUM_PARTY; i++) {
        portraitId = Savemap.partyID[i];
        if (portraitId == -1) {
            continue;
        }
        rect.w = 24;
        rect.h = 48;
        rect.x = 0x340;
        rect.y = 0x100;
        if (portraitId > 4) {
            rect.x = 0x358;
        } else {
            rect.x = 0x340;
        }
        rect.y = rect.y + (portraitId % 5) * 48;
        MoveImage(&rect, 0x3C0, 0x138 + i * 48);
        DrawSync(0);
        rect.x = 0x180;
        rect.y = portraitId;
        rect.w = 0x100;
        rect.h = 1;
        MoveImage(&rect, 0x100, 0x1ED + i);
        DrawSync(0);
    }
    SysMenuStoreCharacterClutToRam((u_long*)g_PartyPortraitClut);
    DrawSync(0);
}

static void func_80026034(void) {}

s32 SysMenuGetMateriaColorByType(s32 materiaId) {
    return D_80049520[D_80049528[g_MateriaData[materiaId & 0xFF].materiaType & 0xF]];
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80026090);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800260DC);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80026258);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800262D8);
