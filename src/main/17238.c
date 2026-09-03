//! PSYQ=3.3 CC1=2.6.3 G=8
#include "main_private.h"

u8 D_80062E54[8];
u8 D_80062E5C;
ActiveCharacterData* D_80062E60;
u32 D_80062E64;
u32 D_80062E68;
s16 D_80062E6C[4];
u32 D_80062E74;
u32 D_80062E78;
s32 D_80062E7C;
s32 D_80062E80;
s32 D_80062E84;
u32 D_80062E88;
u32 D_80062E8C;
u32 D_80062E90;

s32 SysGetMateriaActivatedStars(u8, s32);
s32 SysAddCommandToTemp(s32);
void SysAddMagicSummonSkillToUnitStructure(u8, u8, u8);

s32 func_80017238(u32 arg0, u32* arg1, u8* arg2) {
    *arg2 = arg0;
    *arg1 = arg0 >> 8;
    return SysGetMateriaActivatedStars(*arg2, *arg1);
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001726C);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysCalculateTotalLureGilPreemptiveValue);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysInitPlayerStatFromMateria);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMateriaWithSlotCheck);

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

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithQuadraMagic);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMagicWithQuadraMagic);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterMagicWithQuadraMagic);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairSummonWithQuadraMagic);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterSummonWithQuadraMagic);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairWithAll);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMasterMagicWithAll);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddPairMagicWithAll);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysSearchExistedCommand);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysSearchExistedMagic);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysParseMegaallMateria);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysParseMateriaEquip);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaEquipStatBonus);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX2);

void SysAddMateria30(u8 arg0, u8 arg1) {
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

#ifndef NON_MATCHINGS
// needs BSS import for %gp regs
void SysAddMateriaX4(u8 arg0, s32 arg1);
INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX4);
#else
extern s8 D_80062FFC = 0;
extern u8 D_80063020 = 0;
void SysAddMateriaX4(u8 arg0, s32 arg1) {
    SysGetMateriaActivatedStars(arg0, arg1);
    if (D_80063020) {
        D_80062FFC = 11;
    }
}
#endif

#ifndef NON_MATCHINGS
// needs BSS import for %gp regs
void SysAddMateriaX7(s32 arg0, s32 arg1, s32 arg2);
INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX7);
#else
extern s8 D_80062FFC = 0;
extern u8 D_80063020 = 0;
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
#endif

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaXa);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaXc);

void SysAddMateria00(u8, s32);
void SysAddMateria20(u8, s32);
void SysAddMateria40(u8, s32);
void SysAddMateriaX0(u8 materiaSubType, u8 materiaId, s32 materiaAp) {
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

#ifndef NON_MATCHINGS
// matching with GCC 2.6.3
s32 SysGetMateriaActivatedStars(u8, s32);
INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysGetMateriaActivatedStars);
#else
extern Unk80062F7C* D_80062F7C = NULL; // %gp
extern s32 D_80062F10 = 0;             // %gp
extern s32 D_80062FBC = 0;             // %gp
extern u8 D_80063020 = 0;              // %gp
s32 SysGetMateriaActivatedStars(u8 arg0, s32 arg1) {
    s32 i;
    s32 found;
    u16 temp_a2;
    Unk80062F7C* new_var;

    found = 1;
    for (i = 3; i >= 0; i--) {
        temp_a2 = D_800730CC[arg0].unk4[i];
        if (temp_a2 == 0xFFFF || arg1 < temp_a2 * 100) {
            continue;
        }
        found = i + 2;
        break;
    }
    D_80062FBC = 1;
    for (i = 0; i < 4; i++) {
        temp_a2 = D_800730CC[arg0].unk4[i];
        if (temp_a2 != 0xFFFF) {
            D_80062FBC++;
        }
    }
    if (D_80063020) {
        temp_a2 = D_800730CC[arg0].unk4[found - 1];
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
#endif

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001AE08);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysCopyBoostedStatToUnitStructure);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaXb);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysCopySummonToUnitStructure);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX9);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMateriaX6);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddCommandToTemp);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysRemoveStealIfMug);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysCopyAndSortCommand);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysGetCommandOrder);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysCopyCommandToUnitStructure);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysCopyTempMagicToUnitStructure);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMagicToTemp);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysAddMagicSummonSkillToUnitStructure);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", SysInitPlayerTempStat);
