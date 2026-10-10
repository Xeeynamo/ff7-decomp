//! PSYQ=3.3 CC1=2.6.3 G=8 COMM=true
#include "game.h"
#include "main_private.h"

static u8 D_80062E54[8];
static u8 D_80062E5C;                   // Pre-emptive materia is at maximum level.
static ActiveCharacterData* D_80062E60; // Current active character.
static u32 D_80062E64;
static u32 D_80062E68;
static s16 D_80062E6C[4];
static s16 D_80062E74;
static u32 D_80062E78;
static s32 D_80062E7C;
static s32 D_80062E80;
static s32 D_80062E84;
static u32 D_80062E88;
static u32 D_80062E8C;
static u32 D_80062E90;

// not sure what this is, used by SysAddMateriaEquipStatBonus
static s16 D_80049060[168] = {
    0, 0,  0, 0, 0,  0,  0,   0,  -2, -1, 2,  1, 0, 0,  -5,  5,  -4, -2, 4,  2, 0,  0, -10, 10, 0, 0, 0,  0,
    2, -2, 0, 0, -1, -1, 1,   1,  0,  0,  0,  0, 1, 1,  -1,  -1, 0,  0,  0,  0, 0,  1, 0,   0,  0, 0, 0,  0,
    0, 0,  0, 0, 0,  1,  0,   0,  0,  0,  0,  0, 0, -1, 0,   0,  0,  0,  0,  0, -2, 0, 0,   0,  0, 0, 0,  0,
    2, 0,  0, 0, -1, 0,  1,   0,  0,  0,  -2, 2, 0, 0,  1,   0,  0,  0,  -2, 2, 0,  0, 1,   1,  0, 0, -5, 5,
    0, 0,  2, 2, 0,  0,  -10, 10, 0,  0,  4,  4, 0, 0,  -10, 15, 0,  0,  8,  8, 0,  0, -10, 20, 0, 0, 0,  0,
    0, 0,  0, 0, 0,  0,  0,   0,  0,  0,  0,  0, 0, 0,  0,   0,  0,  0,  0,  0, 0,  0, 0,   0,  0, 0, 0,  0,
};

static u16 g_ElementIdToBitmask[16] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
};

s32 g_BattleCharIdToCharId[14] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 6, 7, 0, 0, 0};

s32 SysGetMateriaActivatedStars(u8 arg0, s32 arg1);
void SysAddMagicSummonSkillToUnitStructure(u8, u8, u8);
void SysRemoveStealIfMug(void);
void SysAddMateriaEquipStatBonus(u8 materiaId);
static void SysAddMateriaX0(u8 materiaSubType, u8 materiaId, s32 materiaAp);
void SysAddMateriaX1(u8 materiaSubType, u8 materiaId, s32 materiaAp);
void SysAddMateriaX2(u8 materiaSubType, u8 materiaId, s32 materiaAp);
void SysAddMateriaX3(u8 materiaSubType, u8 materiaId, s32 materiaAp);
void SysAddMateriaX4(u8 arg0, s32 arg1);
void SysAddMateriaX5(u8 materiaSubType, u8 materiaId, s32 materiaAp);
void SysAddMateriaX6(u8 materiaId, s32 materiaAp);
void SysAddMateriaX7(s32 arg0, s32 arg1, s32 arg2);
void SysAddMateriaX8(void);
void SysAddMateriaX9(u8 materiaId, s32 materiaAp);
void SysAddMateriaXa(void);
void SysAddMateriaXb(u8 materiaId, s32 materiaAp);
void SysAddMateriaXc(void);
u8 SysGetCommandOrder(u8 commandId);
void SysCopyCommandToUnitStructure(u8 commandId, u8 order);
void SysAddPairMateriaUnordered(u32 materia1, u32 materia2, u8 arg2, u8 arg3, u8 arg4);
u8* GetPartySlotArmorMateriaSlots(s32 arg0);
ActiveCharacterData* SysGetPartyPlayerStructureAddressByPartyId(s32 partyId);
u8 D_80063020; // %gp_rel
extern s32 D_80062FBC;
s32 SysSearchExistedMagic(u8);
void SysAddPairMagicWithQuadraMagic(u8, u8, s32);
void SysAddPairMasterMagicWithQuadraMagic(u8);
void SysAddPairSummonWithQuadraMagic(u8, u8);
void SysAddPairMasterSummonWithQuadraMagic(u8);

static s32 func_80017238(u32 arg0, u32* arg1, u8* arg2) {
    *arg2 = arg0;
    *arg1 = arg0 >> 8;
    return SysGetMateriaActivatedStars(*arg2, *arg1);
}

void SysGiveApToEquippedMateria(s16 partyId, u16 ap) {
    ArmorRecord* armor;
    u32* materia_weapon;
    u32* materia_armor;
    u32 materiaAp;
    u8 materiaId;
    s32 stars;
    s32 grownStars;
    s32 i;

    D_80063020 = 0;
    D_80062F34[partyId] = 0;
    if (Savemap.partyID[partyId] == 0xFF) {
        return;
    }
    D_80062E60 = SysGetPartyPlayerStructureAddressByPartyId(partyId);
    materia_weapon = Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].materia_weapon;
    for (i = 0; i < NUM_MATERIA_ROW; i++) {
        stars = func_80017238(materia_weapon[i], &materiaAp, &materiaId);
        if (materiaId == 0xFF || materiaId == 0x2C || materiaAp == 0xFFFFFF) {
            continue;
        }
        switch (D_80062E60->weapon.materiaGrowth) {
        case 0:
            break;
        case 1:
        default:
            materiaAp += ap;
            break;
        case 2:
            materiaAp += ap * 2;
            break;
        case 3:
            materiaAp += ap * 3;
            break;
        }
        materia_weapon[i] = ((materiaAp & 0xFFFFFF) << 8) | materiaId;
        grownStars = func_80017238(materia_weapon[i], &materiaAp, &materiaId);
        if (stars < grownStars) {
            D_80062F34[partyId] |= 1 << i;
            if (grownStars == D_80062FBC) {
                SysAddMateriaReplacingLowest(materiaId);
                materia_weapon[i] = materiaId | 0xFFFFFF00;
            }
        }
    }
    armor = SysGetArmorAddressById(Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].armor);
    materia_armor = Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].materia_armor;
    for (i = 0; i < NUM_MATERIA_ROW; i++) {
        stars = func_80017238(materia_armor[i], &materiaAp, &materiaId);
        if (materiaId == 0xFF || materiaId == 0x2C || materiaAp == 0xFFFFFF) {
            continue;
        }
        switch (armor->materiaGrowth) {
        case 0:
            break;
        case 1:
        default:
            materiaAp += ap;
            break;
        case 2:
            materiaAp += ap * 2;
            break;
        case 3:
            materiaAp += ap * 3;
            break;
        }
        materia_armor[i] = ((materiaAp & 0xFFFFFF) << 8) | materiaId;
        grownStars = func_80017238(materia_armor[i], &materiaAp, &materiaId);
        if (stars < grownStars) {
            D_80062F34[partyId] |= 1 << (i + 8);
            if (grownStars == D_80062FBC) {
                SysAddMateriaReplacingLowest(materiaId);
                materia_armor[i] = materiaId | 0xFFFFFF00;
            }
        }
    }
}

void SysCalcTotalLureGilPreempVal(void) {
    s32 i;
    s32 encounterDown;
    s32 encounterUp;

    encounterUp = 0;
    encounterDown = 0;
    D_80062F18 = 16;
    D_80062F19 = 16;
    D_80062F1A = 0;
    D_80062F1B = 16;
    Savemap.memory_bank_1[0x7A] &= ~0x80;
    for (i = 0; i < NUM_PARTY; i++) {
        if (Savemap.partyID[i] != 0xFF) {
            D_80062E60 = SysGetPartyPlayerStructureAddressByPartyId(i);
            D_80062F18 += D_80062E60->gilBonus;
            encounterUp += D_80062E60->encounterRate;
            encounterDown += D_80062E60->encounterDownRate;
            D_80062F1A += D_80062E60->chocoboChance;
            D_80062F1B += D_80062E60->preemptiveChance;
            if (D_80062E60->characterFlags & 1) {
                Savemap.memory_bank_1[0x7A] |= 0x80;
            }
        }
    }
    if (encounterUp + 16 < encounterDown) {
        D_80062F19 = 2;
    } else {
        D_80062F19 = encounterUp + 16 - encounterDown;
    }
    if (D_80062F18 > 32) {
        D_80062F18 = 32;
    }
    if (D_80062F19 > 32) {
        D_80062F19 = 32;
    }
    if (D_80062F1A > 32) {
        D_80062F1A = 32;
    }
    if (D_80062F1B > 85) {
        D_80062F1B = 85;
    }
    if (D_80062E5C) {
        D_80062F1B |= 0x80;
    }
}

void SysInitPlayerStatFromMateria(s32 arg0) {
    u32 materia1;
    u32 materia2;
    s32 mpCost;
    s32 i;
    u8 slot2;
    u32* materia;
    u8 slot1;
    u8* armorSlots;
    u8* slots;
    u8 partyId;

    partyId = arg0;
    if (Savemap.partyID[partyId] == 0xFF) {
        return;
    }
    slots = GetPartySlotArmorMateriaSlots(partyId);
    D_80062E60 = SysGetPartyPlayerStructureAddressByPartyId(partyId);
    SysInitPlayerTempStat(partyId, D_80062E60);
    D_80063020 = 0;
    materia = Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].materia_weapon;
    for (i = 0; i < 8; i++) {
        D_8006966C[i] = *materia++;
    }
    materia = Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].materia_armor;
    for (i = 8; i < 16; i++) {
        D_8006966C[i] = *materia++;
    }
    for (i = 0; i < 16; i++) {
        SysParseMateriaEquip(D_8006966C[i]);
    }
    armorSlots = slots;
    materia = D_8006966C;
    for (i = 0; i < 4; i++) {
        materia1 = *materia++;
        materia2 = *materia++;
        SysAddPairMateriaWithSlotCheck(D_80062E60->weapon.materiaSlot[i * 2], D_80062E60->weapon.materiaSlot[i * 2 + 1],
                                       materia1, materia2, 0, 0, 0);
    }
    materia = &D_8006966C[8];
    for (i = 0; i < 4; i++) {
        slot1 = *armorSlots++;
        slot2 = *armorSlots++;
        materia1 = *materia++;
        materia2 = *materia++;
        SysAddPairMateriaWithSlotCheck(slot1, slot2, materia1, materia2, 0, 0, 1);
    }
    SysCopyTempMagicToUnitStructure();
    SysCopyAndSortCommand();
    SysCopySummonToUnitStructure();
    SysCopyBoostedStatToUnitStructure();
    D_80069538.physAttack += D_80062E60->strength;
    D_80069538.physDefence += D_80062E60->vitality;
    D_80069538.magAttack += D_80062E60->magic;
    D_80069538.magDefence += D_80062E60->spirit;
    if (D_80069538.physAttack > 255) {
        D_80069538.physAttack = 255;
    }
    if (D_80069538.physDefence > 255) {
        D_80069538.physDefence = 255;
    }
    if (D_80069538.magAttack > 255) {
        D_80069538.magAttack = 255;
    }
    if (D_80069538.magDefence > 255) {
        D_80069538.magDefence = 255;
    }
    if (D_80069538.physAttack < 0) {
        D_80069538.physAttack = 0;
    }
    if (D_80069538.physDefence < 0) {
        D_80069538.physDefence = 0;
    }
    if (D_80069538.magAttack < 0) {
        D_80069538.magAttack = 0;
    }
    if (D_80069538.magDefence < 0) {
        D_80069538.magDefence = 0;
    }
    D_80062E60->physAttack = D_80069538.physAttack;
    D_80062E60->physDefence = D_80069538.physDefence;
    D_80062E60->magAttack = D_80069538.magAttack;
    D_80062E60->magDefence = D_80069538.magDefence;
    func_8001AE08();
    for (i = 0; i < 16; i++) {
        SysParseMegaallMateria(D_8006966C[i]);
    }
    SysSortMagicInUnitStructure(partyId);
    for (i = 0; i < 72; i++) {
        if (D_80062E60->enabledMagic[i].costModifier & 0xE0) {
            mpCost =
                D_80062E60->enabledMagic[i].mpCost +
                (D_80062E60->enabledMagic[i].mpCost * ((D_80062E60->enabledMagic[i].costModifier & 0xE0) >> 5) / 10 +
                 1);
            if (mpCost > 255) {
                mpCost = 255;
            }
            D_80062E60->enabledMagic[i].mpCost = mpCost;
        }
    }
    if (D_80062E60->characterFlags & 8) {
        i = D_80062E60->baseHp;
        D_80062E60->baseHp = D_80062E60->baseMp;
        D_80062E60->baseMp = i;
    }
    if (D_80062E60->baseHp < 10) {
        D_80062E60->baseHp = 10;
    }
    if (D_80062E60->baseMp < 10) {
        D_80062E60->baseMp = 10;
    }
    if (D_80062E60->baseHp < D_80062E60->hp) {
        D_80062E60->hp = D_80062E60->baseHp;
        Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].curHP = D_80062E60->hp;
    }
    if (D_80062E60->baseMp < D_80062E60->mp) {
        D_80062E60->mp = D_80062E60->baseMp;
        Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].curMP = D_80062E60->mp;
    }
    Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].hp_max = D_80062E60->baseHp;
    Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[partyId]]].mp_max = D_80062E60->baseMp;
}

void SysAddPairMateriaWithSlotCheck(u8 slot1, u8 slot2, u32 materia1, u32 materia2, u8 arg4, u8 arg5, u8 arg6) {
    if (slot1 == 2 && slot2 == 3) {
        SysAddPairMateriaUnordered(materia1, materia2, arg4, arg5, arg6);
    }
    if (slot1 == 6 && slot2 == 7) {
        SysAddPairMateriaUnordered(materia1, materia2, arg4, arg5, arg6);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMateriaUnordered);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMateriaOrdered);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithElemental);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithAddedEffect);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithMagicCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithSneakFinalAttack);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterMateriaWithCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairCommandWithCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMagicWithMagicCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairSummonWithMagicCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairSummonWithMpTurbo);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterSummonWithMpTurbo);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterMagicWithMpTurbo);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMagicWithMpTurbo);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithMpTurbo);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairFlagToCommandMagicSummon);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairFlagToSummon);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairFlagToAllSummons);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairFlagToActiveCommand);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairFlagToAllActiveCommands);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairFlagToAllMagics);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairFlagToMagic);

void SysAddPairWithQuadraMagic(s32 arg0, s32 allCount, Materia materia) {
    s32 materiaId;
    s32 ap;
    s32 type;

    materiaId = materia.materiaId;
    ap = materia.ap;
    type = g_MateriaData[materiaId].materiaType & 0xF;
    switch (type) {
    case 9:
        SysAddPairMagicWithQuadraMagic(allCount & 0xFF, materiaId, ap);
        break;
    case 0xA:
        SysAddPairMasterMagicWithQuadraMagic(allCount & 0xFF);
        break;
    case 0xB:
        SysAddPairSummonWithQuadraMagic(allCount & 0xFF, materiaId);
        break;
    case 0xC:
        SysAddPairMasterSummonWithQuadraMagic(allCount & 0xFF);
        break;
    }
}

void SysAddPairMagicWithQuadraMagic(u8 allCount, u8 materiaId, s32 ap) {
    s32 i;
    s32 stars;
    s32 foundIdx;

    stars = SysGetMateriaActivatedStars(materiaId, ap);
    for (i = stars; i > 0; i--) {
        foundIdx = SysSearchExistedMagic(g_MateriaData[materiaId].materiaAttributes[i - 1]);
        if (foundIdx != -1) {
            D_80069554[foundIdx].quadEnabled++;
            D_80069554[foundIdx].allCount += allCount;
        }
    }
}

void SysAddPairMasterMagicWithQuadraMagic(u8 allCount) {
    s32 i;

    for (i = 0; i < LEN(D_80069554); i++) {
        D_80069554[i].quadEnabled = D_80069554[i].quadEnabled + 1;
        D_80069554[i].allCount = allCount + D_80069554[i].allCount;
    }
}

void SysAddPairSummonWithQuadraMagic(u8 allCount, u8 materiaId) {
    s32 idx;

    idx = g_MateriaData[materiaId].materiaAttributes[0] - 0x38;
    D_800694C4[idx] = D_800694C4[idx] + 1;
    D_800694D4[idx] = D_800694D4[idx] + allCount;
}

void SysAddPairMasterSummonWithQuadraMagic(u8 allCount) {
    s32 i;

    for (i = 0; i < LEN(D_800694C4); i++) {
        D_800694C4[i]++;
        D_800694D4[i] += allCount;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithAll);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterMagicWithAll);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMagicWithAll);

s32 SysSearchExistedCommand(u8 commandId) {
    s32 i;

    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        if (D_80069508[i].id == commandId) {
            return i;
        }
    }
    return -1;
}

s32 SysSearchExistedMagic(u8 magicId) {
    s32 i;

    for (i = 0; i < NUM_MAGICS; i++) {
        if (D_80069554[i].id == magicId) {
            return i;
        }
    }
    return -1;
}

void SysParseMegaallMateria(u32 materia) {
    s32 i;
    s32 stars;
    u8 id;
    s32 ap;

    id = materia;
    ap = materia >> 8;
    if ((g_MateriaData[id].materiaType & 0xF) != 4) {
        return;
    }
    stars = SysGetMateriaActivatedStars(id, ap);
    for (i = 0; i < NUM_MAGICS; i++) {
        if (D_80062E60->enabledMagic[i].id != 0xFF) {
            D_80062E60->enabledMagic[i].quadraAttacksLeft += stars;
        }
    }
    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        if (D_80062E60->commandMenu[i].id != 0xFF) {
            switch (D_80062E60->commandMenu[i].id) {
            case 5:
            case 6:
            case 9:
            case 10:
            case 11:
            case 17:
                D_80062E60->commandMenu[i].allCount += stars;
                break;
            }
        }
    }
    if (D_80062E60->commandMenu[0].id != 26) {
        SysCopyCommandToUnitStructure(24, 0);
    }
}

void SysParseMateriaEquip(u32 materia) {
    Unk80062F7C* attr;
    u8 i;
    u32 m;
    u8 materiaId;
    u8 subType;
    s32 materiaAp;

    if (D_80063020) {
        attr = D_80062F7C;
        for (i = 0; i < 8; i++) {
            attr->unkA[i] = 0;
        }
    }
    materiaId = materia & 0xFF;
    materiaAp = materia >> 8;
    if (materiaId == 0xFF) {
        return;
    }
    SysAddMateriaEquipStatBonus(materiaId);
    subType = g_MateriaData[materiaId].materiaType >> 4;
    switch (g_MateriaData[materiaId].materiaType & 0xF) {
    case 0:
        SysAddMateriaX0(subType, materiaId, materiaAp);
        break;
    case 1:
        SysAddMateriaX1(subType, materiaId, materiaAp);
        break;
    case 2:
        SysAddMateriaX2(subType, materiaId, materiaAp);
        break;
    case 3:
        SysAddMateriaX3(subType, materiaId, materiaAp);
        break;
    case 4:
        SysAddMateriaX4(materiaId, materiaAp);
        break;
    case 5:
        SysAddMateriaX5(subType, materiaId, materiaAp);
        break;
    case 6:
        SysAddMateriaX6(materiaId, materiaAp);
        break;
    case 7:
        SysAddMateriaX7(subType, materiaId, materiaAp);
        break;
    case 8:
        SysAddMateriaX8();
        break;
    case 9:
        SysAddMateriaX9(materiaId, materiaAp);
        break;
    case 10:
        SysAddMateriaXa();
        break;
    case 11:
        SysAddMateriaXb(materiaId, materiaAp);
        break;
    case 12:
        SysAddMateriaXc();
        break;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaEquipStatBonus);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX2);

static void SysAddMateria30(u8 arg0, u8 arg1) {
    if (arg1 == 0xB) {
        SysAddMateriaLongRange(arg0);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaCounterAttack);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaLongRange);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria12);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX3);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX5);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria35);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria25);

s8 D_80062FFC; // %gp_rel
void SysAddMateriaX4(u8 arg0, s32 arg1) {
    SysGetMateriaActivatedStars(arg0, arg1);
    if (D_80063020) {
        D_80062FFC = 11;
    }
}

void SysAddMateriaX7(s32 arg0, s32 arg1, s32 arg2) {
    u8 param;
    s32 i;
    s32 enabled;
    s32 bits;

    if (D_80063020 == 0) {
        bits = arg2 & 0xFFFFFF;
        for (i = 0; i < 0x18; i++) {
            enabled = bits & 1;
            bits >>= 1;
            if (enabled) {
                param = i + 0x48;
                SysAddMagicSummonSkillToUnitStructure(i, param, param);
            }
        }

        SysAddCommandToTemp(13);
        return;
    }
    D_80062FFC = 8;
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaXa);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaXc);

void SysAddMateria00(u8, s32);
void SysAddMateria20(u8, s32);
void SysAddMateria40(u8, s32);
static void SysAddMateriaX0(u8 materiaSubType, u8 materiaId, s32 materiaAp) {
    u8 id;
    u8 materiaLevel;

    id = materiaId;
    materiaLevel = SysGetMateriaActivatedStars(id, materiaAp);
    switch (materiaSubType) {
    case 0:
        SysAddMateria00(id, materiaAp);
        break;
    case 2:
        SysAddMateria20(id, materiaAp);
        break;
    case 4:
        SysAddMateria40(id, materiaAp);
        break;
    case 3:
        SysAddMateria30(materiaLevel, id);
        break;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria00);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria20);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria40);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX1);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria21);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria41);

s32 D_80062F10;          // %gp_rel
Unk80062F7C* D_80062F7C; // %gp_rel
s32 D_80062FBC;          // %gp_rel
s32 SysGetMateriaActivatedStars(u8 arg0, s32 arg1) {
    s32 i;
    s32 found;
    u16 temp_a2;
    Unk80062F7C* new_var;

    found = 1;
    for (i = 3; i >= 0; i--) {
        temp_a2 = g_MateriaData[arg0].levelUpApLimits[i];
        if (temp_a2 == 0xFFFF || arg1 < temp_a2 * 100) {
            continue;
        }
        found = i + 2;
        break;
    }
    D_80062FBC = 1;
    for (i = 0; i < 4; i++) {
        temp_a2 = g_MateriaData[arg0].levelUpApLimits[i];
        if (temp_a2 != 0xFFFF) {
            D_80062FBC++;
        }
    }
    if (D_80063020) {
        temp_a2 = g_MateriaData[arg0].levelUpApLimits[found - 1];
        if (temp_a2 == 0xFFFF || found == D_80062FBC) {
            D_80062F10 = 0;
        } else {
            D_80062F10 = temp_a2 * 100 - arg1;
        }
        new_var = D_80062F7C;
        new_var->unk0 = found;
        new_var->unk1 = *(u8*)&D_80062FBC;
        new_var->unk4 = D_80062F10;
    }
    return found;
}

void func_8001AE08(void) {
    if (D_80062E6C[0] > 32) {
        D_80062E6C[0] = 32;
    }
    if (D_80062E6C[2] > 32) {
        D_80062E6C[2] = 32;
    }
    if (D_80062E6C[1] > 255) {
        D_80062E6C[1] = 255;
    }
    if (D_80062E74 > 255) {
        D_80062E74 = 255;
    }
    D_80062E60->gilBonus += D_80062E6C[0];
    D_80062E60->encounterRate += D_80062E6C[1];
    D_80062E60->encounterDownRate += D_80062E74;
    D_80062E60->chocoboChance += D_80062E6C[2];
    D_80062E60->preemptiveChance += D_80062E6C[3];
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysCopyBoostedStatToUnitStructure);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaXb);

void SysCopySummonToUnitStructure(void) {
    s32 i;

    for (i = 0; i < 16; i++) {
        D_80062E60->enabledMagic[56 + i].quadraAttacksLeft = D_800694B4[i];
        D_80062E60->enabledMagic[56 + i].quadEnabled = D_800694C4[i];
        D_80062E60->enabledMagic[56 + i].allAttacksLeft = D_800694D4[i];
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX9);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX6);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddCommandToTemp);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysRemoveStealIfMug);

void SysCopyAndSortCommand(void) {
    s32 i;

    D_80062E7C = 0;
    D_80062E80 = 0;
    SysRemoveStealIfMug();
    for (i = 0; i < 16; i++) {
        if (D_80069508[i].id != 0xFF) {
            if (D_80069508[i].id == 2 && (u8)D_80062E8C == 2) {
                D_80069508[i].id = 21;
            }
            if (D_80069508[i].id == 3 && (u8)D_80062E90 == 2) {
                D_80069508[i].id = 22;
            }
            SysCopyCommandToUnitStructure(D_80069508[i].id, SysGetCommandOrder(D_80069508[i].id));
            D_80062E7C++;
        }
    }
    D_80062E60->unk21 = (D_80062E7C - 1) / 4 + 1;
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysGetCommandOrder);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysCopyCommandToUnitStructure);

void SysCopyTempMagicToUnitStructure(void) {
    s32 i;

    for (i = 0; i < NUM_MAGICS; i++) {
        SysAddMagicSummonSkillToUnitStructure(D_80069554[i].id, i, D_80069554[i].id);
        D_80062E60->enabledMagic[i].quadraAttacksLeft = D_80069554[i].quadCount;
        D_80062E60->enabledMagic[i].quadEnabled = D_80069554[i].quadEnabled;
        D_80062E60->enabledMagic[i].allAttacksLeft = D_80069554[i].allCount;
        D_80062E60->enabledMagic[i].costModifier = D_80069554[i].costModifier;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMagicToTemp);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMagicSummonSkillToUnitStructure);

void SysInitPlayerTempStat(u8 partyId, ActiveCharacterData* chr) {
    s32 i;
    ActiveCharacterData* unit;
    CurrentCharMagicCommand* magic;

    *(u8*)&D_80062E64 = 0; // HACK: part of array?
    *(u8*)&D_80062E68 = 0; // HACK: part of array?
    *(u8*)&D_80062E90 = 0; // HACK: part of array?
    *(u8*)&D_80062E8C = 0; // HACK: part of array?
    for (i = 0; i < NUM_MAGICS; i++) {
        D_80069554[i].id = 0xFF;
        magic = &D_80069554[i];
        magic->allCount = 0;
        magic->quadEnabled = 0;
        D_80069554[i].quadCount = 0;
        D_80069554[i].costModifier = 0;
    }
    for (i = 0; i < NUM_MAGICS_ALL; i++) {
        chr->enabledMagic[i].id = 0xFF;
        chr->enabledMagic[i].allAttacksLeft = 0;
        chr->enabledMagic[i].quadEnabled = 0;
        chr->enabledMagic[i].quadraAttacksLeft = 0;
        chr->enabledMagic[i].costModifier = 0;
    }
    for (i = 0; i < 16; i++) {
        D_800694D4[i] = 0;
        D_800694C4[i] = 0;
        D_800694B4[i] = 0;
    }
    for (i = 0; i < 12; i++) {
        D_800694E4[i] = 0;
    }
    for (i = 0; i < 6; i++) {
        D_800694FC[i] = 0;
    }
    for (i = 0; i < 16; i++) {
        D_80069508[i].id = 0xFF;
        D_80069508[i].allCount = 0;
        D_80069508[i].materiaEffectFlags = 0;
    }
    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        chr->commandMenu[i].id = 0xFF;
        chr->commandMenu[i].unk4 = 1;
        chr->commandMenu[i].materiaEffectFlags = 0;
    }
    for (i = 0; i < 8; i++) {
        unit = D_80062E60;
        unit->enabledCounters[i].materiaAttribute = 0;
        unit->enabledCounters[i].battleCommand = 0;
        unit->enabledCounters[i].counterType = 0;
    }
    D_80069538.strength = D_80062E60->strength;
    D_80069538.vitality = D_80062E60->vitality;
    D_80069538.magic = D_80062E60->magic;
    D_80069538.spirit = D_80062E60->spirit;
    D_80069538.dexterity = D_80062E60->dexterity;
    D_80069538.luck = D_80062E60->luck;
    D_80069538.baseHp = D_80062E60->baseHp;
    D_80069538.baseMp = D_80062E60->baseMp;
    D_80069538.id = D_80062E60->id;
    D_80069538.coverChance = D_80062E60->coverChance;
    D_80062E60->characterFlags = 0;
    D_80069538.physAttack = SysGetPlayerBaseAttackDefense(partyId, 0);
    D_80069538.physDefence = SysGetPlayerBaseAttackDefense(partyId, 1);
    D_80069538.magAttack = SysGetPlayerBaseAttackDefense(partyId, 2);
    D_80069538.magDefence = SysGetPlayerBaseAttackDefense(partyId, 3);
    SysAddCommandToTemp(1);
    SysAddCommandToTemp(4);
    for (i = 0; i < 4; i++) {
        D_80062E6C[i] = 0;
    }
    D_80062E74 = 0;
    D_80062E60->gilBonus = 0;
    D_80062E60->encounterRate = 0;
    D_80062E60->encounterDownRate = 0;
    D_80062E60->chocoboChance = 0;
    D_80062E60->preemptiveChance = 0;
    D_80062E5C = 0;
}
