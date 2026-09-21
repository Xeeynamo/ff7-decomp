//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

#define PREEMPTIVE_CHANCE(x) ((x) & 0x7F)
#define PREEMPTIVE_MASTERED(x) (((x) & 0x80) != 0)
#define ENCOUNTER_PROB(x) ((x) >> 10)
#define ENCOUNTER_MASTERED_PROB(x) ((x) >> 11) // Half the chance if player has equipped mastered Pre-emptive materia.
#define ENCOUNTER_ID(x) ((x) & 0x3FF)

typedef struct {
    s16 enabled;
    s16 standard[6];
    s16 special[4];
    s16 pad;
} FieldEncounterTable;

extern u8 D_800716D0;
extern u16 D_8007173C;
extern FieldEncounterTable* g_FieldEncounters;
extern u8 D_80071C20;
extern s16 D_8007E774;
extern u8 D_8007EBC8;
extern u8 D_8009C6D8;
extern u8 g_RandomTable[256];

u8 FieldGetRandomU8FromList(void) {
    D_8009C540++;
    if (!D_8009C540) {
        D_8009AD2C += 13;
    }
    return g_RandomTable[D_8009C540] - D_8009AD2C;
}

u8 FieldGetNextRandomU8(void) {
    D_80071C20++;
    return g_RandomTable[D_80071C20];
}

void FieldBattleCheck(void) {
    u8 rand;
    s32 i;
    u8 prob;
    FieldEncounterTable* encTable;

    if (!g_FieldState.encounterTableId) {
        encTable = &g_FieldEncounters[0];
    } else {
        encTable = &g_FieldEncounters[1];
    }

    // Check once every eight calls.
    D_8009C6D8 += 32;
    if (D_8009C6D8) {
        return;
    }

    // Increase HP for party members that have the Cat's Bell accessory.
    func_800262D8();

    // The step counter is stored as a u16 in script bank 4.
    Savemap.memory_bank_4[6]++;
    if (!Savemap.memory_bank_4[6] && Savemap.memory_bank_4[7] != 255) {
        Savemap.memory_bank_4[7]++;
    }
    if (!(encTable->enabled & 1) || g_FieldMovieJustStarted || g_FieldState.battlesDisabled) {
        return;
    }

    D_8007173C += g_FieldEntity[g_PlayerModelId].MoveSpeed / ((encTable->enabled & 0xFF00) >> 8);

    // Do we get a pre-emptive encounter?
    rand = FieldGetRandomU8FromList();
    if (rand < PREEMPTIVE_CHANCE(D_80062F1B)) {
        D_800716D0 = 4;
    } else {
        D_800716D0 = 0;
    }

    // Do we get an encounter?
    rand = FieldGetRandomU8FromList();
    if (rand >= ((u32)(D_8007173C * D_80062F19) >> 12)) {
        return;
    }

    StopFieldMapPreload();
    g_FieldState.eventCmd = EVTCMD_ENTERING_BATTLE;
    D_8007EBC8 = 1;

    // Checking special encounters.
    prob = 0;
    rand = FieldGetNextRandomU8() >> 2;
    // Back attack 1.
    if (!PREEMPTIVE_MASTERED(D_80062F1B)) {
        prob += ENCOUNTER_PROB(encTable->special[0]);
    } else {
        prob += ENCOUNTER_MASTERED_PROB(encTable->special[0]);
    }
    if (rand < prob) {
        g_FieldState.eventCmdParam = ENCOUNTER_ID(encTable->special[0]);
        D_800716D0 = 0;
        return;
    }
    // Back attack 2.
    if (!PREEMPTIVE_MASTERED(D_80062F1B)) {
        prob += ENCOUNTER_PROB(encTable->special[1]);
    } else {
        prob += ENCOUNTER_MASTERED_PROB(encTable->special[1]);
    }
    if (rand < prob) {
        g_FieldState.eventCmdParam = ENCOUNTER_ID(encTable->special[1]);
        D_800716D0 = 0;
        return;
    }
    // Attack enemy from both sides.
    prob += ENCOUNTER_PROB(encTable->special[2]);
    if (rand < prob) {
        g_FieldState.eventCmdParam = ENCOUNTER_ID(encTable->special[2]);
        return;
    }
    // Attacked from both sides.
    if (!PREEMPTIVE_MASTERED(D_80062F1B)) {
        prob += ENCOUNTER_PROB(encTable->special[3]);
    } else {
        prob += ENCOUNTER_MASTERED_PROB(encTable->special[3]);
    }
    if (rand < prob) {
        g_FieldState.eventCmdParam = ENCOUNTER_ID(encTable->special[3]);
        return;
    }

    // Did not get special encounter, checking regular encounters.
    prob = 0;
    rand = FieldGetNextRandomU8() >> 2;
    g_FieldState.eventCmdParam = ENCOUNTER_ID(encTable->standard[5]);
    for (i = 0; i < 5; i++) {
        prob += ENCOUNTER_PROB(encTable->standard[i]);
        if (rand < prob) {
            g_FieldState.eventCmdParam = ENCOUNTER_ID(encTable->standard[i]);
            break;
        }
    }

    // Try again with new random number if we get the same encounter twice in a row.
    if (g_FieldState.eventCmdParam != D_8007E774) {
        D_8007E774 = g_FieldState.eventCmdParam;
        return;
    }

    prob = 0;
    rand = FieldGetNextRandomU8() >> 2;
    g_FieldState.eventCmdParam = ENCOUNTER_ID(encTable->standard[5]);
    for (i = 0; i < 5; i++) {
        prob += ENCOUNTER_PROB(encTable->standard[i]);
        if (rand < prob) {
            g_FieldState.eventCmdParam = ENCOUNTER_ID(encTable->standard[i]);
            // Bug: If we get encounter 6, this doesn't get updated.
            D_8007E774 = g_FieldState.eventCmdParam;
            return;
        }
    }
}
