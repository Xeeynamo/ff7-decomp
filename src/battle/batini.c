#include "battle.h"
#include "unzip.h"

extern Unk801B2308 D_80163624;
extern u16 g_IsMutiBattle;

// entrypoint
INCLUDE_ASM("asm/us/battle/nonmatchings/batini", BattleInitMain);

static void BattleInitLoadSceneData(s32 sceneID, void (*cb)(void));
static void BattleInitEnemyAI(void);
static void BattleInitPartyFromSavemap(void);
static void BattleInitCharStats(ActiveCharacterData* character, BattlePartyWork* partyWork, BattleUnit* battleUnit);
static void BattleInitFormation(void);
void BattleInitSetup(s32 sceneID) {
    BattleUnit* unit;
    s32 i;
    s32 var_s1;

    var_s1 = 4;
    if (g_IsMutiBattle) {
        var_s1 = 0;
        BattleQueueEvent(0, 0, 15, 0);
        BattleQueueEvent(0, 0, 14, 0);
    }
    for (i = 0; i < 0x40; i++) {
        if (D_800F6936[i][0] >= var_s1) {
            D_800F6936[i][0] = -1;
        }
    }
    for (i = 0; i < 3; i++) {
        g_BattlePartyWork[i].unk6 = 0;
    }
    if (g_IsMutiBattle) {
        BattleInitPartyFromSavemap();
    }
    BattleInitLoadSceneData(sceneID, BATTLE_RunFrame);
    BattleInitEnemyUnits();
    g_BattleState.presentMask = 0;
    for (i = 0; i < 10; i++) {
        BattleRecalcUnitSpeed(i);
        unit = &g_BattleState.combatant[i];
        if (unit->unk8 != -1) {
            g_BattleState.presentMask |= 1 << i;
        }
    }
    g_BattleState.sceneID = sceneID;
    D_800F83A8 = D_80163624.unk2;
    BattleInitFormation();
    BattleUpdateUnitMasks();
    BattleInitEnemyAI();
    BattleUpdateUnitMasks();
    for (i = 4; i < 10; i++) {
        D_800F5BBC[i][0] = ((u8)SysGetRandomByteRange(0x40) + 0x80) << 8;
        BattleInitUnitAction(i);
    }
}

extern u16 g_BattleUnitPresentMask;
extern u16 D_8009D864[][0x220]; // stride 0x440, one per Unk8009D84C record
u16 BattleGetRndU16(void);      // random, 16-bit

// Rolls the initial ATB timer of every present combatant and writes it into
// D_800F5BBC. The battle type (battleType) then biases those timers: a
// preemptive-style opening zeroes the party's, an ambush pushes it towards the
// enemies, and a Battle Square opening (setup flag 8) overrides both.
void BattleInitATBTimers(void) {
    s32 timer[10];
    s32 presentMask;
    s32 max;
    s32 val;
    s32 t;
    s32 i;

    presentMask = g_BattleUnitPresentMask;
    max = 0;
    for (i = 0; i < 10; i++) {
        D_800F5BBC[i][0] = 0;
        val = 0;
        if ((presentMask >> i) & 1) {
            val = BattleGetRndU16() >> 1;
            if (max < val) {
                max = val;
            }
        }
        timer[i] = val;
    }
    for (i = 0; i < 10; i++) {
        if ((presentMask >> i) & 1) {
            switch (D_800F5F44.battleType) {
            case SETUP_DEFAULT:
            case SETUP_PINCER_2:
                t = timer[i] + 0xE000;
                timer[i] = t - max;
                break;
            case SETUP_BACK_ATTACK:
            case SETUP_PINCER:
                if (i < 4) {
                    timer[i] = 0;
                } else {
                    t = timer[i] + 0xF000;
                    timer[i] = t - max;
                }
                break;
            default:
                if (i < 4) {
                    timer[i] = 0xFFFE;
                } else {
                    timer[i] = timer[i] >> 3;
                }
                break;
            }
            if (g_BattleState.setupFlags & 8) {
                if (i < 3) {
                    timer[i] = 0xFFFE;
                } else {
                    timer[i] = 0;
                }
            }
            D_800F5BBC[i][0] = timer[i];
        }
    }
    for (i = 0; i < 3; i++) {
        D_8009D864[i][0] = D_800F5BBC[i][0];
    }
}

void BattleInitSetSpeed(s32 speed) { D_800F5F44.battleSpeed = 0x10000 / ((speed * 480 / 256 + 0x78) * 2); }

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", BattleInitPlayer);

// The per-party work area at 0x800F5BB8: the turn state, the three party
// records (g_BattlePartyWork) and their setup config (D_800F5EFC) are one object, so
// BATINI reaches all three off a single base.
typedef struct {
    /* 0x00 */ u8 targetFlags;
    /* 0x01 */ u8 attackEffectId;
    /* 0x02 */ u8 damageFormulaId;
    /* 0x03 */ u8 hitChance;
    /* 0x04 */ u8 impactEffectId;
    /* 0x05 */ u8 criticalHitChance;
    /* 0x06 */ u8 unk06;
    /* 0x07 */ u8 unk07;
    /* 0x08 */ u16 normalAttackSound;
    /* 0x0A */ u16 criticalAttackSound;
    /* 0x0C */ u16 missAttackSound;
    /* 0x0E */ u16 attackElement;
    /* 0x10 */ u16 cameraMovementId;
    /* 0x12 */ u16 specialAttackFlags;
    /* 0x14 */ s32 attackStatusMask;
} BattleUnitAttackSetup; // size:0x18

typedef struct {
    /* 0x000 */ Unk800AF470 turn[10];
    /* 0x2A8 */ BattlePartyWork party[3];
    /* 0x344 */ BattleUnitAttackSetup setup[3];
} BattleWork; // size:0x38C

extern BattleWork g_CombatantTurnState;
extern SavePartyMember D_8009C738[];
void BattleInitApplyAccStatus(s32 slot, s32 accessory);
void BattleInitCharCmdMenu(s32 slot);
void BattleInitCharCmdState(s32 slot);
s32 BattleInitApplyStartFX(s32 slot);

// Seeds the three live party slots from the save data: finds each slot's
// party member record, copies HP/MP and the derived battle stats across, then
// applies the equipped accessory, the command list and the row/limit setup.
static void BattleInitPartyFromSavemap(void) {
    BattlePartyWork* party;
    ActiveCharacterData* rec;
    BattleUnit* c;
    Unk800AF470* t;
    BattleUnitAttackSetup* setup;
    SavePartyMember* m;
    s32 id;
    s32 i;
    s32 j;

    for (i = 0; i < 3; i++) {
        t = &g_CombatantTurnState.turn[i];
        party = &g_CombatantTurnState.party[i];
        rec = &g_ActiveCharacters[i];
        c = &g_BattleState.combatant[i];
        setup = &g_CombatantTurnState.setup[i];
        id = D_8009CBDC[i];
        if (id != 0xFF) {
            for (j = 0; j < 9; j++) {
                m = &D_8009C738[j];
                if (m->char_id == id) {
                    c->unk9 = m->level;
                    c->curHP = m->hp_cur;
                    c->unk28 = m->mp_cur;
                    t->unk3C = c->curHP;
                    t->unk3E = c->unk28;
                    BattleInitCharStats(rec, party, c);
                    t->unk34 = rec->immuneStatuses;
                    setup->attackElement = rec->weapon.attackElement | rec->physicalAttackElements;
                    setup->attackStatusMask = rec->physicalAttackStatuses;
                    setup->hitChance = rec->weapon.attackPercent;
                    setup->targetFlags = rec->weapon.targetFlags;
                    t->unk29 &= 0xFD;
                    if (rec->characterFlags & 4) {
                        setup->targetFlags &= 0xDF;
                    }
                    if (!(setup->targetFlags & 0x20)) {
                        t->unk29 |= 2;
                    }
                    BattleInitApplyAccStatus(i, m->accessory);
                    BattleInitCharCmdMenu(i);
                    BattleInitCharCmdState(i);
                    if (BattleInitApplyStartFX(i) == 0) {
                        BattleInitUnitAction(i);
                    }
                    break;
                }
            }
        }
    }
}

extern void BattleRunUnitScript(s32, s32, s32);

void BattleInitPartyScripts(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (((s8)D_80163624.unk94[i][0] != -1) && !(g_BattleState.combatant[i].status & 1)) {
            BattleRunUnitScript(i, 0, 0);
        }
    }
}

extern u8 D_800707C5[][8];    // command table, 8-byte stride
extern u8 D_800708D0[][0x1C]; // attack table, 0x1C stride
extern u8 D_800F5EFC[][0x18]; // per-slot formation-setup config
extern u8 D_800F5BE1[][0x44]; // same records as D_800F5BBC

// Fixes up party member sceneID's battle command list: each of the 16 command
// slots gets its target flags from the command table (falling back to the
// formation setup), with extra flags for the Enemy Skill / W- commands, and
// unk21 ends up as the number of command rows in use. The second pass clears
// the "usable" byte of every equipped materia whose attack is not flagged
// battle-usable.
void BattleInitCharCmdMenu(s32 sceneID) {
    ActiveCharacterData* e;
    s32 cmd;
    s32 flags;
    s32 id;
    s32 i;

    e = &g_ActiveCharacters[sceneID];
    e->unk21 = 1;
    for (i = 0; i < 16; i++) {
        flags = 0xFF;
        cmd = e->commandMenu[i].id;
        if (cmd != 0xFF) {
            flags = D_800707C5[cmd][0];
            if (flags == 0xFF) {
                flags = D_800F5EFC[sceneID][0];
            }
            if (cmd < 0x1C) {
                if (cmd >= 0x18) {
                    e->commandMenu[i].allCount = 0xFF;
                }
            }
            if (e->commandMenu[i].initialCursorAction == 7) {
                if (D_800F5BE1[sceneID][0] & 2) {
                    e->commandMenu[i].initialCursorAction = 0;
                }
                if (e->commandMenu[i].allCount != 0) {
                    if (e->commandMenu[i].id != 0x19) {
                        flags |= 0xC;
                    }
                }
                cmd = e->commandMenu[i].id;
                if (cmd == 5 || cmd == 0x11) {
                    flags |= 0x10;
                    if (e->commandMenu[i].allCount != 0) {
                        e->commandMenu[i].initialCursorAction = 0;
                    }
                }
            }
            e->unk21 = i / 4 + 1;
        }
        e->commandMenu[i].targetFlags = flags;
    }
    for (i = 0; i < 0x60; i++) {
        id = e->enabledMagic[i].id;
        if (id != 0xFF) {
            if (i >= 0x48) {
                id += 0x48;
            } else if (i >= 0x38) {
                id += 0x38;
            }
            if (i < 0x38) {
                if (!(D_800708D0[id][0] & 8)) {
                    e->enabledMagic[i].quadraAttacksLeft = 0;
                }
            }
        }
    }
}

void BattleInitResetExtraCmds(s32 sceneID) {
    s32 i;
    ActiveCharacterData* data;

    data = &g_ActiveCharacters[sceneID];
    data->unk21 = 1;
    for (i = 1; i < 4; i++) {
        data->commandMenu[i].id = 0xFF;
        data->commandMenu[i].initialCursorAction = 0;
        data->commandMenu[i].targetFlags = 0;
        data->commandMenu[i].unk4 = 3;
        data->commandMenu[i].allCount = 0;
        data->commandMenu[i].materiaEffectFlags = 0;
    }
}

s32 SysGetLimitCmdId(s32, s32); // extern

typedef struct {
    /* 00 */ u8 materiaID[3];
    /* 03 */ u8 unk3[3];
    /* 06 */ u8 count;
    /* 07 */ u8 unk7;
    /* 08 */ u8 unk8[0xC];
    /* 14 */ struct {
        u8 unk0;
        u8 unk1[0x1B];
    } unk14[3];
} BattleMateriaSlotData; // size:0x68

// Resolves up to three materia slots of character sceneID against the equipment
// mask arg1: a slot whose bit is set copies its paired value into unk3 and
// counts towards unk6.
void BattleResolveMateriaSlots(s32 slot, s32 materiaMask, BattleMateriaSlotData* data) {
    s32 count;
    s32 i;
    s32 j;

    count = 0;
    for (i = 0; i < 3; i++) {
        if (data->materiaID[i] != 0xFF) {
            for (j = 0; j < 12; j++) {
                if (SysGetLimitCmdId(slot, j) == data->materiaID[i]) {
                    break;
                }
            }
            if (j == 12) {
                func_800155A4(0x26);
            } else if ((materiaMask >> j) & 1) {
                count++;
                data->unk3[i] = data->unk14[i].unk0;
            }
        }
    }
    data->unk7 = 0;
    data->count = count;
}

s32 BattleGetMateriaValue(u32 sceneID) {
    u8 temp_v1;
    s32 ret;

    temp_v1 = sceneID;
    ret = 0;
    if (temp_v1 != 0xFF && (D_800730CC[temp_v1].unk11 & 0xF) == 7) {
        ret = (sceneID >> 8) | 0x80000000;
    }
    return ret;
}

s32 BattleGetEquipMateriaVal(u32* equipment) {
    s32 ret;
    s32 i;

    ret = 0;
    for (i = 0; i < 8; i++) {
        ret |= BattleGetMateriaValue(equipment[0x10 + i]);
        ret |= BattleGetMateriaValue(equipment[0x18 + i]);
    }
    return ret;
}

extern u8 D_80071C29[][0x10]; // accessory table, 0x10 stride

// Applies party member `slot`'s equipped accessory: the status the previously
// equipped one granted is cleared first, then the new accessory's permanent
// status is ORed into the combatant, its turn state and the party record.
void BattleInitApplyAccStatus(s32 slot, s32 accessory) {
    Unk800AF470* t;
    BattlePartyWork* party;
    BattleUnit* c;
    u8 effect;

    t = &g_CombatantTurnState.turn[slot];
    party = &g_CombatantTurnState.party[slot];
    c = &g_BattleState.combatant[slot];
    c->status &= ~party->accessoryStatusMask;
    t->unk34 &= ~party->accessoryStatusMask;
    party->accessoryStatusMask = 0;
    t->unkD = 0xFF;
    if (accessory != 0xFF) {
        effect = D_80071C29[accessory][0];
        t->unkD = effect;
        switch (effect) {
        case 0:
            c->status |= STATUS_HASTE;
            t->unk34 |= STATUS_HASTE;
            party->accessoryStatusMask |= STATUS_HASTE;
            break;
        case 1:
            c->status |= STATUS_BERSERK;
            t->unk34 |= STATUS_BERSERK;
            party->accessoryStatusMask |= STATUS_BERSERK;
            break;
        case 2:
            c->status |= STATUS_D_SENTENCE;
            t->unk34 |= STATUS_D_SENTENCE;
            party->accessoryStatusMask |= STATUS_D_SENTENCE;
            t->unk12 = 0xFF;
            break;
        case 3:
            c->status |= STATUS_REFLECT;
            t->unk34 |= STATUS_REFLECT;
            party->accessoryStatusMask |= STATUS_REFLECT;
            break;
        case 6:
            c->status |= STATUS_BARRIER | STATUS_M_BARRIER;
            party->accessoryStatusMask |= STATUS_BARRIER | STATUS_M_BARRIER;
            break;
        }
    }
}

const s32 D_801B001C[] = {0x0000, 0x1000, 0x0008, 0x0800};
const s32 D_801B002C[] = {0x0000, 0x000A, 0x0027, 0x000A};
extern u8 g_BattleStartFXFlags; // pending battle-start status flags, one bit per entry
                                // of D_801B001C / D_801B002C (bit 4 = full-heal)
void BattleQueueEvent(s32, s32, s32, s32);

// Applies the pending battle-start effects in g_BattleStartFXFlags to party member
// `slot`: bit 4 restores half its max HP, bits 0-3 inflict the matching status
// from D_801B001C unless the member's turn state already carries it. Returns
// nonzero if any status was inflicted.
s32 BattleInitApplyStartFX(s32 slot) {
    s32 mask;
    s32 ret;
    s32 i;

    mask = g_CombatantTurnState.turn[slot].unk34;
    g_BattleState.combatant[slot].status &= ~STATUS_D_SENTENCE;
    ret = 0;
    if (g_CombatantTurnState.turn[slot].unk29 & 8) {
        mask |= STATUS_FROG;
    }
    if (g_BattleStartFXFlags & 0x10) {
        g_BattleState.combatant[slot].curHP += g_BattleState.combatant[slot].maxHP >> 1;
        if (g_BattleState.combatant[slot].curHP > g_BattleState.combatant[slot].maxHP) {
            g_BattleState.combatant[slot].curHP = g_BattleState.combatant[slot].maxHP;
        }
        BattleQueueEvent(2, slot, 0x17, 0);
    }
    for (i = 0; i < 4; i++) {
        if ((g_BattleStartFXFlags >> i) & 1) {
            g_BattleState.combatant[slot].status |= D_801B001C[i] & ~mask;
            BattleQueueEvent(2, slot, 0x17, D_801B002C[i]);
            ret = 1;
        }
    }
    return ret;
}

static void BattleInitCharStats(ActiveCharacterData* character, BattlePartyWork* partyWork, BattleUnit* battleUnit) {
    battleUnit->dexterity = character->dexterity;
    battleUnit->luck = character->luck;
    battleUnit->maxHP = character->baseHp;
    battleUnit->maxMP = character->baseMp;
    battleUnit->physAttack = character->physAttack;
    battleUnit->magAttack = character->magAttack;
    battleUnit->physDefence = character->physDefence;
    battleUnit->magDefence = character->magDefence;
    if (battleUnit->physAttack == 0) {
        battleUnit->physAttack = 1;
    }
    partyWork->maxHP = battleUnit->maxHP;
    partyWork->maxMP = battleUnit->maxMP;
    // 8 = HP_MP_SWAP
    if (character->characterFlags & 8) {
        partyWork->capHP = 999;
        partyWork->capMP = 9999;
    } else {
        partyWork->capHP = 9999;
        partyWork->capMP = 999;
    }
}

const u8 D_801B003C[] = {0xFF, 0x32, 0x33, 0x34, 0x35, 0xFF, 0x48, 0x07};
extern u16 g_CharacterMask[3];
extern s16 D_801636BE[][8]; // stride 0x10
void func_800B1060(s32);

// Lays out the two sides for the opening of the battle. D_801B003C picks the
// intro animation for the battle type, then the type decides which rows the
// party and the enemies occupy (row[0]/row[1]/row[2]) and which combatants
// start "turned around" (bit 0x80 of unk4) -- back attacks, side attacks and
// pincers each split the party differently. Finally the front/back row bit is
// re-derived for the three party slots.
static void BattleInitFormation(void) {
    u16 row[3];
    s32 enemyMask;
    s32 partyMask;
    s32 sideMask;
    u16 mask;
    s32 back;
    s32 intro;
    s32 i;

    enemyMask = g_BattleState.unk12;
    partyMask = g_BattleState.unk10;
    sideMask = 5;
    if (D_8016360C.setup.type == SETUP_SIDE_ATTACK_3) {
        sideMask = ~5;
    }
    intro = D_801B003C[D_800F5F44.battleType];
    if (intro != 0xFF && g_BattleState.sceneID != 0x3D6) {
        func_800B1060(intro);
    }
    mask = 0;
    row[0] = 0;
    row[1] = 0;
    row[2] = 0;
    switch (D_800F5F44.battleType) {
    case 0:
        mask = enemyMask;
        /* fallthrough */
    case 1:
        row[0] = partyMask;
        row[1] = enemyMask;
        break;
    case 2:
        mask = partyMask;
        row[0] = enemyMask;
        row[1] = mask;
        break;
    case 4:
        row[1] = partyMask;
        for (i = 0; i < 6; i++) {
            if ((enemyMask >> (i + 4)) & 1) {
                row[g_BattleState.combatant[i + 4].unk4 & 2] |= 1 << (i + 4);
            }
        }
        mask = row[2] | (partyMask & 2);
        if (g_BattleState.sceneID == 0x3D6) {
            mask &= ~partyMask;
        }
        break;
    default:
        row[0] = partyMask & sideMask;
        row[1] = enemyMask;
        row[2] = partyMask & ~sideMask;
        mask = row[2];
        for (i = 0; i < 6; i++) {
            if (((enemyMask >> (i + 4)) & 1) && D_80163624.unk34[i].unk6 >= 0) {
                mask |= 1 << (i + 4);
            }
        }
        break;
    }
    for (i = 0; i < 10; i++) {
        g_BattleState.combatant[i].unk4 &= ~0x82;
        if ((row[2] >> i) & 1) {
            g_BattleState.combatant[i].unk4 |= 2;
        }
        if ((mask >> i) & 1) {
            g_BattleState.combatant[i].unk4 |= 0x80;
        }
    }
    for (i = 0; i < 3; i++) {
        back = g_BattleState.combatant[i].unk4 >> 6;
        back &= 1;
        switch (D_800F5F44.battleType) {
        case 0:
        case 1:
            break;
        case 2:
            back = !back;
            g_BattleState.combatant[i].unk4 ^= 0x40;
            break;
        default:
            back = 0;
            g_BattleState.combatant[i].unk4 &= ~0x40;
            break;
        }
        D_801636BE[i][0] = back;
    }
    g_CharacterMask[0] = row[0];
    g_CharacterMask[1] = row[1];
    g_CharacterMask[2] = row[2];
}

extern u16 D_8009CBE0[];     // item inventory (320 slots; (count << 9) | id)
extern u16 D_800722D6[][14]; // item table, stride 0x1C
extern u8 D_800722D8[][28];
extern u16 D_800738CA[][22]; // weapon table, stride 0x2C
extern u8 D_800738A0[][44];
extern u16 D_80071E64[][18]; // armor table, stride 0x24
extern u16 D_80071C32[][8];  // accessory table, stride 0x10
extern u8 D_80166F74;
extern BattleItemEntry D_801671B8[];

// Builds the in-battle item list from the inventory: every one of the 320
// inventory slots becomes one BattleItemEntry, with the target and restriction
// flags pulled from the item / weapon / armor / accessory table the id falls
// in. D_80166F74 ends up as half the number of slots up to the last used one
// (at least 3) -- the row count the item widget scrolls over.
void BattleInitItemList(void) {
    BattleItemEntry* entry;
    s32 i;
    s32 last;
    s32 rows;
    s32 id;
    s32 count;
    s32 targetFlags;
    s32 flags;

    last = 0;
    for (i = 0; i < 0x140; i++) {
        entry = &D_801671B8[i];
        id = D_8009CBE0[i];
        count = 0;
        targetFlags = 0;
        flags = 0xB;
        if (id != 0xFFFF) {
            count = (u32)id >> 9;
            id &= 0x1FF;
            if (id < 0x80) {
                flags = D_800722D6[id][0] & 0xB;
                targetFlags = D_800722D8[id][0];
            } else if (id < 0x100) {
                flags = D_800738CA[id - 0x80][0] & 0xB;
                targetFlags = D_800738A0[id - 0x80][0];
            } else if (id < 0x120) {
                flags = D_80071E64[id - 0x100][0] & 0xB;
                targetFlags = 3;
            } else if (id < 0x140) {
                flags = D_80071C32[id - 0x120][0] & 0xB;
                targetFlags = 3;
            }
            last = i + 1;
        }
        entry->id = id;
        entry->count = count;
        entry->targetFlags = targetFlags;
        entry->unk4 = flags;
    }
    rows = (last + 1) / 2;
    if (rows < 3) {
        rows = 3;
    }
    D_80166F74 = rows;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", BattleInitEnemyUnits);

static void BattleInitEnemyAI(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (D_80163624.unk34[i].unk0 != -1) {
            BattleRunUnitScript(i + 4, 0, 0);
        }
    };
    for (i = 0; i < 6; i++) {
        D_80163624.unk34[i].unkC = g_BattleState.combatant[4 + i].unk4;
        D_80163624.unk94[4 + i][1] = g_BattleState.combatant[4 + i].unk10;
        g_BattleState.combatant[4 + i].unk44[0] = g_BattleState.combatant[4 + i].status;
    }
}

static const s8 g_BattleTypeMap[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x03, 0x03, 0x03, 0x05, 0x6E, 0x64, 0x62};
static void BattleInitLoadSceneData(s32 sceneID, void (*cb)(void)) {
    u8 dummy[0x100];
    SceneContainer scene;
    s32 sceneChunkID;
    s32 scenePackID;
    s32 formationIndex;
    s32 i;
    u_long* dst;
    s32* scenePackBuffer;
    s32* src;

    scenePackBuffer = (s32*)0x801C0000;
    sceneChunkID = sceneID / 4;
    scenePackID = BattleGetScenePackId(sceneChunkID);  // sector modified based on the Chunk ID
    SystemLoadFileBySector(                            // load file from disk
        func_800144D8(BATTLE_SCENE) + scenePackID * 4, // Disk sector where to load the file from
        0x800 * 4,                                     // Size in bytes to copy
        (u_long*)scenePackBuffer,                      // Destination
        NULL);
    formationIndex = sceneChunkID - D_80083184[scenePackID];
    func_800145BC(cb); // wait until all data is read, keep executing the vsync
                       // callback until then
    i = scenePackBuffer[formationIndex];
    src = &scenePackBuffer[i];
    dst = (u_long*)&scene;
    Unzip(          
        (u8*)src,   
        (u8*)dst);  
    formationIndex = sceneID - sceneChunkID * 4;
    SysMemCopy32(D_8016360C.enemyModelIDs, scene.enemyModelIDs, sizeof(scene.enemyModelIDs));
    SysMemCopy32((s32*)&D_8016360C.setup, &scene.setup[formationIndex], sizeof(BattleSetup));
    SysMemCopy32((s32*)&D_8016360C.camera, &scene.camera[formationIndex], sizeof(CameraPlacement) * 4);
    SysMemCopy32((s32*)&D_8016360C.formation, &scene.formation[formationIndex], sizeof(FormationEntry) * 6);
    SysMemCopy32((s32*)&D_800F5F44.enemy, &scene.enemy, sizeof(scene.enemy));
    SysMemCopy32((s32*)&D_800F5F44.attacks, &scene.attacks, sizeof(scene.attacks));
    SysMemCopy32((s32*)&D_800F5F44.attackIDs, scene.attackIDs, sizeof(scene.attackIDs));
    SysMemCopy32((s32*)&D_800F5F44.attackNames, &scene.attackNames, sizeof(scene.attackNames));
    SysMemCopy32((s32*)&D_800F5F44._5, &scene.unkC80, sizeof(Unk800F5F44_5));
    SysMemCopy32((s32*)&D_800F5F44.script, &scene.script, sizeof(scene.script));
    if (D_8016376A & 4 && D_8016360C.setup.flags & SETUP_NO_PREEMPTIVE_STRIKE) {
        if (D_8016360C.setup.type == SETUP_DEFAULT) {
            D_8016360C.setup.type = SETUP_PREEMPTIVE;
        }
    }
    D_800F5F44.battleType = (u8)g_BattleTypeMap[D_8016360C.setup.type];
    if (D_8016376A & EVENT_BATTLE_SQUARE) {
        D_8016360C.setup.stageID = 37;
        D_8016360C.setup.flags |= SETUP_CANNOT_ESCAPE;
        D_8016360C.setup.cameraID = (SysGetRandomByteFromTable() & 3) + 0x60;
        D_8016360C.setup.escapeCounter = 1;
        // enemy strength and magic is 25% higher at battle square
        for (i = 0; i < 3; i++) {
            D_800F5F44.enemy[i].unk90[5] *= 2;
            D_800F5F44.enemy[i].strength = BattleBoostVal25Percent(D_800F5F44.enemy[i].strength);
            D_800F5F44.enemy[i].magic = BattleBoostVal25Percent(D_800F5F44.enemy[i].magic);
        }
    } else if (D_8016376A & 8) {
        D_8016360C.setup.flags &= ~SETUP_CANNOT_ESCAPE;
    }
    if (!(D_8016360C.setup.flags & SETUP_CANNOT_ESCAPE)) {
        D_8016376A |= 8;
    }
    D_800F5F44.D_800F7DB2 = D_8016360C.setup.escapeCounter;
    if (D_800F5F44.battleType == 1 || D_800F5F44.battleType == 3) {
        D_800F5F44.D_800F7DB2 = 1;
    }
    D_800F5F44.D_800F7DB6 = D_800F5F44.D_800F7DB2;
}

s32 BattleGetScenePackId(s32 sceneID) {
    u32 i;

    for (i = 1; i < LEN(D_80083184); i++) {
        if (sceneID < D_80083184[i]) {
            break;
        }
    }
    return i - 1;
}

// increase param by 25%
s32 BattleBoostVal25Percent(s32 value) {
    value = (value * 125) / 100;
    if (value > 255) {
        value = 255;
    }
    return value;
}
