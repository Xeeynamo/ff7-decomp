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
static u16 D_80062E88;
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
void SysAddMateria25(u8 arg0, s32 arg1);
void SysAddMateria35(u8 arg0, s32 arg1);
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
void SysAddPairMateriaOrdered(u32 materia1, u32 materia2, u8 arg2, u8 arg3);
u8* GetPartySlotArmorMateriaSlots(s32 arg0);
ActiveCharacterData* SysGetPartyPlayerStructureAddressByPartyId(s32 partyId);
u8 D_80063020; // %gp_rel
extern s32 D_80062FBC;
s32 SysSearchExistedMagic(u8 magicId);
void SysAddPairMagicWithQuadraMagic(u8 allCount, u8 materiaId, s32 ap);
void SysAddPairMasterMagicWithQuadraMagic(u8 allCount);
void SysAddPairSummonWithQuadraMagic(u8 allCount, u8 materiaId);
void SysAddPairMasterSummonWithQuadraMagic(u8 allCount);
s32 SysSearchExistedCommand(u8 commandId);
void SysAddPairMagicWithMagicCounter(u8 allCount, u8 materiaId, s32 ap);
void SysAddPairSummonWithMagicCounter(u8 allCount, u8 materiaId, s32 ap);
void SysAddPairMasterMateriaWithCounter(s32 arg0, u8 arg1);
void SysAddPairMagicWithMpTurbo(s32, s32, s32);
void SysAddPairSummonWithMpTurbo(s32 arg0, s32 arg1, s32 arg2);
void SysAddPairFlagToActiveCommand(s32 arg0);
void SysAddPairFlagToAllActiveCommands(void);
void SysAddPairFlagToMagic(s32, s32);
void SysAddPairFlagToAllMagics(void);
void SysAddPairFlagToSummon(s32 arg0, s32 arg1);
void SysAddPairFlagToAllSummons(void);
void SysAddPairMagicWithAll(u8 allCount, u8 materiaId, s32 ap);
void SysAddPairMasterMagicWithAll(u8 allCount, u8 materiaId, s32 ap);
void SysAddMateria12(u8 arg0, u8 arg1);
void SysAddMateria00(u8, s32);
void SysAddMateria20(u8 arg0, s32 arg1);
void SysAddMateria40(u8, s32);
void SysAddMateria21(u8, s32);
void SysAddMateria41(u8, s32);
void SysAddPairCommandWithCounter(s32 arg0, s32 arg1, s32 arg2);
void SysAddPairMasterMagicWithMpTurbo(s32 arg0);
void SysAddMateriaLongRange(u8 arg0);

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
    for (i = MAGIC_START; i < SUMMON_END; i++) {
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

void SysAddPairWithAddedEffect(s32 arg0, u8 arg1, u8 arg2) {
    u32 statusEffects;

    statusEffects = g_MateriaData[arg1].statusEffects[0] + (g_MateriaData[arg1].statusEffects[1] << 8) +
                    (g_MateriaData[arg1].statusEffects[2] << 16);
    if (!arg2) {
        D_80062E60->physicalAttackStatuses |= statusEffects;
    } else {
        D_80062E60->immuneStatuses |= statusEffects;
    }
}

void SysAddPairWithCounter(s32 arg0, s32 allCount, u32 materia) {
    s32 materiaId;
    s32 ap;
    s32 type;

    materiaId = materia & 0xFF;
    ap = materia >> 8;
    type = g_MateriaData[materiaId].materiaType & 0xF;
    D_80062E88 = 0;
    switch (type) {
    case MATERIA_TYPE_COMMAND:
        SysAddPairCommandWithCounter(allCount & 0xFF, materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_COMMAND:
        SysAddPairMasterMateriaWithCounter(allCount & 0xFF, 0xFD);
        break;
    }
}

void SysAddPairWithMagicCounter(s32 arg0, s32 allCount, u32 materia) {
    s32 materiaId;
    s32 ap;
    s32 type;

    materiaId = materia & 0xFF;
    ap = materia >> 8;
    type = g_MateriaData[materiaId].materiaType & 0xF;
    D_80062E88 = 0;
    switch (type) {
    case MATERIA_TYPE_MAGIC:
        SysAddPairMagicWithMagicCounter(allCount, materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_MAGIC:
        SysAddPairMasterMateriaWithCounter(allCount & 0xFF, 0xFE);
        break;
    case MATERIA_TYPE_SUMMON:
        SysAddPairSummonWithMagicCounter(allCount, materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_SUMMON:
        SysAddPairMasterMateriaWithCounter(allCount & 0xFF, 0xFF);
        break;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithSneakFinalAttack);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterMateriaWithCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairCommandWithCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMagicWithMagicCounter);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairSummonWithMagicCounter);

void SysAddPairSummonWithMpTurbo(s32 arg0, s32 arg1, s32 arg2) {
    s32 count;
    u8 costMod;
    s32 newVal;
    u8 maskedArg0;
    u8 id;

    id = arg1;
    maskedArg0 = arg0;
    count = SysGetMateriaActivatedStars(id, arg2);
    while (count > 0) {
        costMod = D_80062E60->enabledMagic[g_MateriaData[id].materiaAttributes[count - 1]].costModifier;
        newVal = (costMod >> 5) + maskedArg0;
        if (newVal > 5) {
            newVal = 5;
        }
        D_80062E60->enabledMagic[g_MateriaData[id].materiaAttributes[count - 1]].costModifier =
            (costMod & 0x1F) | ((newVal & 7) << 5);
        count--;
    }
}

void SysAddPairMasterSummonWithMpTurbo(u8 arg0) {
    s32 i;
    u8 costMod;
    s32 newVal;

    for (i = 0; i < NUM_SUMMONS; i++) {
        costMod = D_80062E60->enabledMagic[NUM_MAGICS + i].costModifier;
        newVal = (costMod >> 5) + arg0;
        if (newVal > 5) {
            newVal = 5;
        }
        D_80062E60->enabledMagic[NUM_MAGICS + i].costModifier = (costMod & 0x1F) | ((newVal & 7) << 5);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterMagicWithMpTurbo);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMagicWithMpTurbo);

void SysAddPairWithMpTurbo(s32 arg0, u32 arg1) {
    s32 materiaId;
    s32 ap;
    s32 type;

    materiaId = arg1 & 0xFF;
    ap = arg1 >> 8;
    type = g_MateriaData[materiaId].materiaType & 0xF;
    switch (type) {
    case MATERIA_TYPE_MAGIC:
        SysAddPairMagicWithMpTurbo(arg0 & 0xFF, materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_MAGIC:
        SysAddPairMasterMagicWithMpTurbo(arg0 & 0xFF);
        break;
    case MATERIA_TYPE_SUMMON:
        SysAddPairSummonWithMpTurbo(arg0 & 0xFF, materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_SUMMON:
        SysAddPairMasterSummonWithMpTurbo(arg0 & 0xFF);
        break;
    }
}

void SysAddPairFlagToCommandMagicSummon(u32 materia) {
    u8 materiaId;
    s32 ap;
    s32 type;

    materiaId = materia;
    type = g_MateriaData[materiaId].materiaType & 0xF;
    ap = materia >> 8;
    switch (type) {
    case MATERIA_TYPE_COMMAND:
        SysAddPairFlagToActiveCommand(materiaId);
        break;
    case MATERIA_TYPE_MASTER_COMMAND:
        SysAddPairFlagToAllActiveCommands();
        break;
    case MATERIA_TYPE_MAGIC:
        SysAddPairFlagToMagic(materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_MAGIC:
        SysAddPairFlagToAllMagics();
        break;
    case MATERIA_TYPE_SUMMON:
        SysAddPairFlagToSummon(materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_SUMMON:
        SysAddPairFlagToAllSummons();
        break;
    }
}

void SysAddPairFlagToSummon(s32 arg0, s32 arg1) {
    s32 id;
    s32 stars;
    u8 attr;
    id = arg0 & 0xFF;
    stars = SysGetMateriaActivatedStars(id, arg1);
    while (stars > 0) {
        attr = g_MateriaData[id].materiaAttributes[stars - 1];
        D_80062E60->enabledMagic[attr].costModifier |= D_80062E78;
        stars--;
    }
}

void SysAddPairFlagToAllSummons(void) {
    s32 i;

    for (i = 0; i < NUM_SUMMONS; i++) {
        D_80062E60->enabledMagic[NUM_MAGICS + i].costModifier |= D_80062E78;
    }
}

void SysAddPairFlagToActiveCommand(s32 arg0) {
    s32 cmdIdx;
    u8 id;

    cmdIdx = SysSearchExistedCommand(g_MateriaData[arg0 & 0xFF].materiaAttributes[0]);
    if (cmdIdx != -1) {
        D_80069508[cmdIdx].materiaEffectFlags |= D_80062E78;
    }
    id = g_MateriaData[arg0 & 0xFF].materiaAttributes[1];
    if (id != 7 && id != 0x11) {
        return;
    }
    cmdIdx = SysSearchExistedCommand(id);
    if (cmdIdx != -1) {
        D_80069508[cmdIdx].materiaEffectFlags |= D_80062E78;
    }
}

void SysAddPairFlagToAllActiveCommands(void) {
    s32 cmdIdx;
    s32 i;

    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        cmdIdx = SysSearchExistedCommand(D_80069508[i].id);
        if (cmdIdx != -1) {
            D_80069508[cmdIdx].materiaEffectFlags |= D_80062E78;
        }
    }
}

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
    case MATERIA_TYPE_MAGIC:
        SysAddPairMagicWithQuadraMagic(allCount, materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_MAGIC:
        SysAddPairMasterMagicWithQuadraMagic(allCount);
        break;
    case MATERIA_TYPE_SUMMON:
        SysAddPairSummonWithQuadraMagic(allCount, materiaId);
        break;
    case MATERIA_TYPE_MASTER_SUMMON:
        SysAddPairMasterSummonWithQuadraMagic(allCount);
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

    idx = g_MateriaData[materiaId].materiaAttributes[0] - SUMMON_START;
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

void SysAddPairWithAll(s32 arg0, s32 allCount, Materia materia) {
    u8 type;
    s32 ap;

    type = g_MateriaData[materia.materiaId].materiaType & 0xF;
    ap = materia.ap;

    switch (type) {
    case MATERIA_TYPE_MAGIC:
        SysAddPairMagicWithAll(allCount, materia.materiaId, ap);
        break;
    case MATERIA_TYPE_MASTER_MAGIC:
        SysAddPairMasterMagicWithAll(allCount, materia.materiaId, ap);
        break;
    }
}

void SysAddPairMasterMagicWithAll(u8 allCount, u8 materiaId, s32 ap) {
    s32 i;
    s32 foundIdx;

    for (i = 0; i < NUM_MAGICS; i++) {
        foundIdx = SysSearchExistedMagic(i);
        if (foundIdx != -1) {
            D_80069554[foundIdx].quadCount += allCount;
        }
    }
}

void SysAddPairMagicWithAll(u8 allCount, u8 materiaId, s32 ap) {
    s32 i;
    s32 stars;
    s32 foundIdx;

    stars = SysGetMateriaActivatedStars(materiaId, ap);
    for (i = stars; i > 0; i--) {
        foundIdx = SysSearchExistedMagic(g_MateriaData[materiaId].materiaAttributes[i - 1]);
        if (foundIdx != -1) {
            D_80069554[foundIdx].quadCount += allCount;
        }
    }
}

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
    case MATERIA_TYPE_UNK_0:
        SysAddMateriaX0(subType, materiaId, materiaAp);
        break;
    case MATERIA_TYPE_UNK_1:
        SysAddMateriaX1(subType, materiaId, materiaAp);
        break;
    case MATERIA_TYPE_COMMAND_REPLACE:
        SysAddMateriaX2(subType, materiaId, materiaAp);
        break;
    case MATERIA_TYPE_W_COMMAND:
        SysAddMateriaX3(subType, materiaId, materiaAp);
        break;
    case MATERIA_TYPE_MEGA_ALL:
        SysAddMateriaX4(materiaId, materiaAp);
        break;
    case MATERIA_TYPE_SUPPORT:
        SysAddMateriaX5(subType, materiaId, materiaAp);
        break;
    case MATERIA_TYPE_COMMAND:
        SysAddMateriaX6(materiaId, materiaAp);
        break;
    case MATERIA_TYPE_ENEMY_SKILL:
        SysAddMateriaX7(subType, materiaId, materiaAp);
        break;
    case MATERIA_TYPE_MASTER_COMMAND:
        SysAddMateriaX8();
        break;
    case MATERIA_TYPE_MAGIC:
        SysAddMateriaX9(materiaId, materiaAp);
        break;
    case MATERIA_TYPE_MASTER_MAGIC:
        SysAddMateriaXa();
        break;
    case MATERIA_TYPE_SUMMON:
        SysAddMateriaXb(materiaId, materiaAp);
        break;
    case MATERIA_TYPE_MASTER_SUMMON:
        SysAddMateriaXc();
        break;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaEquipStatBonus);

void SysAddMateriaX2(u8 materiaSubType, u8 materiaId, s32 materiaAp) {
    u8 materiaLevel;
    u8 id;

    id = materiaId;
    materiaLevel = SysGetMateriaActivatedStars(id, materiaAp);
    if (materiaSubType == 1) {
        SysAddMateria12(materiaLevel, id);
    }
}

static void SysAddMateria30(u8 arg0, u8 arg1) {
    if (arg1 == 0xB) {
        SysAddMateriaLongRange(arg0);
    }
}

void SysAddMateriaCounterAttack(s32 arg0, s32 arg1) {
    s32 counterIdx;
    if (!D_80063020) {
        if (D_80062E80 != 8) {
            D_80062E60->enabledCounters[D_80062E80].counterType = 9;
            D_80062E60->enabledCounters[D_80062E80].battleCommand = 9;
            counterIdx = D_80062E80;
            D_80062E80++;
            D_80062E60->enabledCounters[counterIdx].materiaAttribute =
                g_MateriaData[arg1 & 0xFF].materiaAttributes[arg0 & 0xFF];
        }
    } else {
        D_80062FFC = 0x12;
    }
}

void SysAddMateriaLongRange(u8 arg0) {
    if (!D_80063020) {
        D_80062E60->characterFlags |= 4;
        return;
    }
    D_80062FFC = 0x12;
}
void SysAddMateria12(u8 arg0, u8 arg1) {
    s32 i;
    u8* ptr;
    Unk80062F7C* unk;

    i = 0;
    if (!D_80063020) {
        for (i = arg0 - 1; i > 0; i--) {
            if (g_MateriaData[arg1].materiaAttributes[i] != 0xFF) {
                break;
            }
        }
        D_80069508[0].id = g_MateriaData[arg1].materiaAttributes[i];
    } else {
        unk = D_80062F7C;
        ptr = g_MateriaData[arg1].materiaAttributes;
        for (; i < 5; i++) {
            unk->materiaAttributes[i].value = *ptr++;
            unk->materiaAttributes[i].selected = 0;
        }
        if (unk->unk1 == arg0) {
            unk->materiaAttributes[arg0 - 2].selected = 1;
        } else {
            unk->materiaAttributes[arg0 - 1].selected = 1;
        }
        D_80062FFC = 0x13;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX3);

void SysAddMateriaX5(u8 materiaSubType, u8 materiaId, s32 materiaAp) {
    switch (materiaSubType) {
    case 2:
        SysAddMateria25(materiaId, materiaAp);
        break;
    case 3:
        SysAddMateria35(materiaId, materiaAp);
        break;
    }
}

void SysAddMateria35(u8 arg0, s32 arg1) {
    Unk80062F7C* attr;
    u8 star;

    star = SysGetMateriaActivatedStars(arg0, arg1);
    if (!D_80063020) {
        return;
    }
    attr = D_80062F7C;
    attr->materiaAttributes[0].value = g_MateriaData[arg0].materiaAttributes[0];
    attr->materiaAttributes[0].selected = star;
    switch (g_MateriaData[arg0].materiaAttributes[0]) {
    case 0x57:
        D_80062FFC = 0x10;
        break;
    case 0x51:
    case 0x63:
        D_80062FFC = 0xF;
        break;
    }
}

void SysAddMateria25(u8 arg0, s32 arg1) {
    Unk80062F7C* attr;
    s32 star;

    star = SysGetMateriaActivatedStars(arg0, arg1);
    if (!D_80063020) {
        return;
    }
    attr = D_80062F7C;
    attr->materiaAttributes[0].value = g_MateriaData[arg0].materiaAttributes[0];
    attr->materiaAttributes[0].selected = star * 10;
    switch (g_MateriaData[arg0].materiaAttributes[0]) {
    case 0x64:
        D_80062FFC = 0x10;
        break;
    case 0x58:
        D_80062FFC = 0x10;
        break;
    case 0x59:
        D_80062FFC = 0x10;
        break;
    case 0x5A:
        D_80062FFC = 0x10;
        break;
    case 0x56:
        D_80062FFC = 0x10;
        break;
    case 0x5C:
        D_80062FFC = 0x10;
        break;
    case 0x5D:
        D_80062FFC = 0x10;
        break;
    case 0x5E:
        D_80062FFC = 0x10;
        break;
    case 0x5F:
        D_80062FFC = 0x10;
        break;
    case 0x54:
    case 0x55:
        D_80062FFC = 0xF;
        break;
    }
}

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

    if (!D_80063020) {
        bits = arg2 & 0xFFFFFF;
        for (i = 0; i < NUM_ENEMY_SKILLS; i++) {
            enabled = bits & 1;
            bits >>= 1;
            if (enabled) {
                param = ENEMY_SKILL_START + i;
                SysAddMagicSummonSkillToUnitStructure(i, param, param);
            }
        }

        SysAddCommandToTemp(13);
        return;
    }
    D_80062FFC = 8;
}

void SysAddMateriaX8(void) {
    if (!D_80063020) {
        SysAddCommandToTemp(5);
        SysAddCommandToTemp(6);
        SysAddCommandToTemp(7);
        SysAddCommandToTemp(9);
        SysAddCommandToTemp(0xA);
        SysAddCommandToTemp(0xB);
        SysAddCommandToTemp(0xC);
        return;
    }
    D_80062FFC = 5;
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaXa);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaXc);

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

void SysAddMateria20(u8 arg0, s32 arg1) {
    u8 materiaId;
    s32 stars;
    Unk80062F7C* unk;

    materiaId = arg0;
    stars = SysGetMateriaActivatedStars(materiaId, arg1);
    if (materiaId == 0xD) {
        SysAddMateriaCounterAttack((u8)stars, 0xD);
        return;
    }
    if (!D_80063020) {
        D_800694E4[g_MateriaData[materiaId].materiaAttributes[0]] += g_MateriaData[materiaId].materiaAttributes[stars];
    } else {
        unk = D_80062F7C;
        unk->materiaAttributes[0].value = g_MateriaData[materiaId].materiaAttributes[0];
        unk->materiaAttributes[0].selected = g_MateriaData[materiaId].materiaAttributes[stars];
        D_80062FFC = 4;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateria40);

void SysAddMateriaX1(u8 materiaSubType, u8 materiaId, s32 materiaAp) {
    s32 subType;

    subType = materiaSubType & 0xFF;
    switch (subType) {
    case 4:
        SysAddMateria41(materiaId, materiaAp);
        return;
    case 2:
        SysAddMateria21(materiaId, materiaAp);
        return;
    }
}

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

    for (i = 0; i < NUM_SUMMONS; i++) {
        D_80062E60->enabledMagic[NUM_MAGICS + i].quadraAttacksLeft = D_800694B4[i];
        D_80062E60->enabledMagic[NUM_MAGICS + i].quadEnabled = D_800694C4[i];
        D_80062E60->enabledMagic[NUM_MAGICS + i].allAttacksLeft = D_800694D4[i];
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX9);

void SysAddMateriaX6(u8 materiaId, s32 materiaAp) {
    s32 star;
    Unk80062F7C* unk;
    u8* ptr;
    s32 i;

    star = SysGetMateriaActivatedStars(materiaId, materiaAp);
    if (!D_80063020) {
        for (i = star - 1; i >= 0; i--) {
            if (g_MateriaData[materiaId].materiaAttributes[i] != 0xFF) {
                break;
            }
        }
        SysAddCommandToTemp(g_MateriaData[materiaId].materiaAttributes[i]);
    } else {
        i = 0;
        unk = D_80062F7C;
        ptr = g_MateriaData[materiaId].materiaAttributes;
        do {
            unk->materiaAttributes[i].value = *ptr++;
            unk->materiaAttributes[i].selected = 0;
            i++;
        } while (i < 5);
        if (unk->unk1 == star) {
            unk->materiaAttributes[star - 2].selected = 1;
        } else {
            unk->materiaAttributes[star - 1].selected = 1;
        }
        D_80062FFC = 3;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddCommandToTemp);

void SysRemoveStealIfMug(void) {
    s32 i;
    s32 hasMug = 0;

    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        if (D_80069508[i].id == 0x11) {
            hasMug = 1;
            break;
        }
    }

    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        if (hasMug != 1) {
            continue;
        }
        if (D_80069508[i].id == 5) {
            D_80069508[i].id = 0xFF;
        }
    }
}

void SysCopyAndSortCommand(void) {
    s32 i;

    D_80062E7C = 0;
    D_80062E80 = 0;
    SysRemoveStealIfMug();
    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        if (D_80069508[i].id == 0xFF) {
            continue;
        }
        if (D_80069508[i].id == 2 && (u8)D_80062E8C == 2) {
            D_80069508[i].id = 21;
        }
        if (D_80069508[i].id == 3 && (u8)D_80062E90 == 2) {
            D_80069508[i].id = 22;
        }
        SysCopyCommandToUnitStructure(D_80069508[i].id, SysGetCommandOrder(D_80069508[i].id));
        D_80062E7C++;
    }
    D_80062E60->unk21 = (D_80062E7C - 1) / 4 + 1;
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysGetCommandOrder);

void SysCopyCommandToUnitStructure(u8 commandId, u8 order) {
    s32 i;

    D_80062E60->commandMenu[order].id = commandId;
    D_80062E60->commandMenu[order].menuOptionFlags = MENU_OPTION_NONE;
    D_80062E60->commandMenu[order].initialCursorAction = D_800707C4[commandId].initialCursorAction;
    D_80062E60->commandMenu[order].targetFlags = D_800707C4[commandId].targetFlags;

    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        if (commandId != D_80069508[i].id) {
            continue;
        }
        D_80062E60->commandMenu[order].allCount = D_80069508[i].allCount;
        D_80062E60->commandMenu[order].materiaEffectFlags = D_80069508[i].materiaEffectFlags;
        break;
    }
}

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
    for (i = 0; i < LEN(D_800694D4); i++) {
        D_800694D4[i] = 0;
        D_800694C4[i] = 0;
        D_800694B4[i] = 0;
    }
    for (i = 0; i < LEN(D_800694E4); i++) {
        D_800694E4[i] = 0;
    }
    for (i = 0; i < LEN(D_800694FC); i++) {
        D_800694FC[i] = 0;
    }
    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        D_80069508[i].id = 0xFF;
        D_80069508[i].allCount = 0;
        D_80069508[i].materiaEffectFlags = 0;
    }
    for (i = 0; i < NUM_BATTLE_COMMANDS; i++) {
        chr->commandMenu[i].id = 0xFF;
        chr->commandMenu[i].menuOptionFlags = MENU_OPTION_UNK1;
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
    for (i = 0; i < LEN(D_80062E6C); i++) {
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
