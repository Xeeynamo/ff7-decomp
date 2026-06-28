//! PSYQ=3.3 CC1=2.7.2
#include <game.h>

extern u8 g_MateriaPriority[];
extern s32 g_MateriaStealLoot[];

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D01E8);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0228);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D031C);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D03B4);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D03C8);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0618);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D062C);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0650);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D069C);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0704);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D076C);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D081C);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D08E4);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D09AC);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0A74);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0AAC);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0BA0);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0CAC);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0CE8);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0D24);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0DCC);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0E4C);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D0E80);

void func_801D296C(void) {}

void EvictWeakestStolenMateria(s32 newMateria, s32 priority) {
    s32 i;
    s32* loot;

    i = 0;
    loot = g_MateriaStealLoot;
    do {
        if (g_MateriaPriority[*(u8*)loot] == priority) {
            *loot = newMateria;
            return;
        }
        i += 1;
        loot += 1;
    } while (i < 0x30);
}

s32 GetLowestStealPriority(void) {
    s32 i;
    s32 lowest;
    u8* loot;

    lowest = 0xFF;
    i = 0;
    loot = (u8*)g_MateriaStealLoot;
    do {
        u8 id = *loot;
        s32 prio = g_MateriaPriority[id];
        if (prio < lowest) {
            lowest = prio;
        }
        i += 1;
        loot += 4;
    } while (i < 0x30);
    return lowest;
}

void OfferMateriaToSteal(s32* materiaPtr) {
    s32 i;
    s32 lowest;

    if (*materiaPtr == -1) {
        return;
    }
    i = 0;
    do {
        if (g_MateriaStealLoot[i] == -1) {
            g_MateriaStealLoot[i] = *materiaPtr;
            return;
        }
        i += 1;
    } while (i < 0x30);

    lowest = GetLowestStealPriority();
    if (g_MateriaPriority[*materiaPtr & 0xFF] < lowest) {
        return;
    }
    EvictWeakestStolenMateria(*materiaPtr, lowest);
}

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D2ABC);

void RemoveMateriaFromPlayer(s32 materia) {
    s32 c;
    s32 s;

    for (c = 0; c < 9; c++) {
        if ((Savemap.phs_visibility_mask >> c) & 1) {
            for (s = 0; s < 8; s++) {
                if (Savemap.party[c].materia_weapon[s] == materia) {
                    Savemap.party[c].materia_weapon[s] = -1;
                    return;
                }
            }
            for (s = 0; s < 8; s++) {
                if (Savemap.party[c].materia_armor[s] == materia) {
                    Savemap.party[c].materia_armor[s] = -1;
                    return;
                }
            }
        }
    }
    for (s = 0; s < MAX_MATERIA_COUNT; s++) {
        if (Savemap.materia[s] == materia) {
            Savemap.materia[s] = -1;
            return;
        }
    }
}

void FinalizeMateriaSteal(void) {
    s32 i;
    s32 materia;

    for (i = 0; i < 0x30; i++) {
        materia = g_MateriaStealLoot[i];
        if (materia != -1) {
            RemoveMateriaFromPlayer(materia);
        }
    }
    for (i = 0; i < 0x30; i++) {
        Savemap.yuffie_stolen_materia[i] = g_MateriaStealLoot[i];
    }
}

void StealAllMateria(void) {
    s32 i;
    s32 c;
    s32 slot;

    for (i = 0; i < 0x30; i++) {
        g_MateriaStealLoot[i] = -1;
    }
    for (c = 0; c < 9; c++) {
        if ((Savemap.phs_visibility_mask >> c) & 1) {
            for (slot = 0; slot < 8; slot++) {
                do {
                    OfferMateriaToSteal(&Savemap.party[c].materia_weapon[slot]);
                } while (0);
            }
            for (slot = 0; slot < 8; slot++) {
                OfferMateriaToSteal(&Savemap.party[c].materia_armor[slot]);
            }
        }
    }
    for (slot = 0; slot < MAX_MATERIA_COUNT; slot++) {
        OfferMateriaToSteal(&Savemap.materia[slot]);
    }
    FinalizeMateriaSteal();
}

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D2E84);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D2F00);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D3018);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D3138);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D3228);

INCLUDE_ASM("asm/us/menu/nonmatchings/itemmenu", func_801D3260);
