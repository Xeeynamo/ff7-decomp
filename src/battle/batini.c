#include "battle.h"
#include "unzip.h"

extern Unk801B2308 D_80163624;
extern u16 D_8016376C;
void func_800A3354(void); // battle callback for batini, move to battle.h
void func_801B2308(void);

// entrypoint
INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B0050);

static void func_801B23E0(s32 sceneID, void (*cb)(void));
void func_801B0490(s32 sceneID) {
    Unk800F83E0* c;
    s32 i;
    s32 var_s1;

    var_s1 = 4;
    if (D_8016376C) {
        var_s1 = 0;
        func_800A7254(0, 0, 15, 0);
        func_800A7254(0, 0, 14, 0);
    }
    for (i = 0; i < 0x40; i++) {
        if (D_800F6936[i][0] >= var_s1) {
            D_800F6936[i][0] = -1;
        }
    }
    for (i = 0; i < 3; i++) {
        D_800F5E60[i].unk6 = 0;
    }
    if (D_8016376C) {
        func_801B0F08();
    }
    func_801B23E0(sceneID, func_800A3354);
    func_801B1E0C();
    g_BattleState.presentMask = 0;
    for (i = 0; i < 10; i++) {
        func_800AE954(i);
        c = &g_BattleState.combatant[i];
        if (c->unk8 != -1) {
            g_BattleState.presentMask |= 1 << i;
        }
    }
    g_BattleState.sceneID = sceneID;
    D_800F83A8 = D_80163624.unk2;
    func_801B19AC();
    func_800A4540();
    func_801B2308();
    func_800A4540();
    for (i = 4; i < 10; i++) {
        D_800F5BBC[i][0] = ((u8)SysGetRandomByteRange(0x40) + 0x80) << 8;
        func_800B108C(i);
    }
}

extern u16 D_8016375A;
extern u16 D_8009D864[][0x220]; // stride 0x440, one per Unk8009D84C record
u16 func_800B2F50(void);        // random, 16-bit

// Rolls the initial ATB timer of every present combatant and writes it into
// D_800F5BBC. The battle type (D_800F7DC8) then biases those timers: a
// preemptive-style opening zeroes the party's, an ambush pushes it towards the
// enemies, and a Battle Square opening (setup flag 8) overrides both.
void func_801B0668(void) {
    s32 timer[10];
    s32 presentMask;
    s32 max;
    s32 val;
    s32 t;
    s32 i;

    presentMask = D_8016375A;
    max = 0;
    for (i = 0; i < 10; i++) {
        D_800F5BBC[i][0] = 0;
        val = 0;
        if ((presentMask >> i) & 1) {
            val = func_800B2F50() >> 1;
            if (max < val) {
                max = val;
            }
        }
        timer[i] = val;
    }
    for (i = 0; i < 10; i++) {
        if ((presentMask >> i) & 1) {
            switch (D_800F5F44.D_800F7DC8) {
            case 0:
            case 5:
                t = timer[i] + 0xE000;
                timer[i] = t - max;
                break;
            case 2:
            case 4:
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

void func_801B085C(s32 arg0) { D_800F5F44.D_800F7DA6 = 0x10000 / ((arg0 * 480 / 256 + 0x78) * 2); }

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B08C0);

// The per-party work area at 0x800F5BB8: the turn state, the three party
// records (D_800F5E60) and their setup config (D_800F5EFC) are one object, so
// BATINI reaches all three off a single base.
typedef struct {
    /* 0x00 */ u8 unk0;
    /* 0x01 */ u8 unk1;
    /* 0x02 */ u8 unk2;
    /* 0x03 */ u8 unk3;
    /* 0x04 */ u8 unk4[0xA];
    /* 0x0E */ u16 unkE;
    /* 0x10 */ u8 unk10[4];
    /* 0x14 */ s32 unk14;
} Unk800F5EFC; // size:0x18

typedef struct {
    /* 0x000 */ Unk800AF470 turn[10];
    /* 0x2A8 */ Unk800F5E60 party[3];
    /* 0x344 */ Unk800F5EFC setup[3];
} BattleWork; // size:0x38C

extern BattleWork g_CombatantTurnState;
extern SavePartyMember D_8009C738[];
void func_801B1598(s32 slot, s32 accessory);
void func_801B11BC(s32 slot);
void func_800A4BA4(s32 slot);
s32 func_801B1734(s32 slot);

// Seeds the three live party slots from the save data: finds each slot's
// party member record, copies HP/MP and the derived battle stats across, then
// applies the equipped accessory, the command list and the row/limit setup.
void func_801B0F08(void) {
    Unk800F5E60* party;
    ActiveCharacterData* rec;
    Unk800F83E0* c;
    Unk800AF470* t;
    Unk800F5EFC* setup;
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
                    func_801B18F8(rec, party, c);
                    t->unk34 = rec->immuneStatuses;
                    setup->unkE = rec->weapon.attackElement | rec->physicalAttackElements;
                    setup->unk14 = rec->physicalAttackStatuses;
                    setup->unk3 = rec->weapon.attackPercent;
                    setup->unk0 = rec->weapon.targetFlags;
                    t->unk29 &= 0xFD;
                    if (rec->characterFlags & 4) {
                        setup->unk0 &= 0xDF;
                    }
                    if (!(setup->unk0 & 0x20)) {
                        t->unk29 |= 2;
                    }
                    func_801B1598(i, m->accessory);
                    func_801B11BC(i);
                    func_800A4BA4(i);
                    if (func_801B1734(i) == 0) {
                        func_800B108C(i);
                    }
                    break;
                }
            }
        }
    }
}

extern void func_800A6000(s32, s32, s32);

void func_801B1120(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (((s8)D_80163624.unk94[i][0] != -1) && !(g_BattleState.combatant[i].status & 1)) {
            func_800A6000(i, 0, 0);
        }
    }
}

extern u8 D_800707C5[][8];    // command table, 8-byte stride
extern u8 D_800708D0[][0x1C]; // attack table, 0x1C stride
extern u8 D_800F5EFC[][0x18]; // per-slot formation-setup config
extern u8 D_800F5BE1[][0x44]; // same records as D_800F5BBC

// Fixes up party member arg0's battle command list: each of the 16 command
// slots gets its target flags from the command table (falling back to the
// formation setup), with extra flags for the Enemy Skill / W- commands, and
// unk21 ends up as the number of command rows in use. The second pass clears
// the "usable" byte of every equipped materia whose attack is not flagged
// battle-usable.
void func_801B11BC(s32 arg0) {
    ActiveCharacterData* e;
    s32 cmd;
    s32 flags;
    s32 id;
    s32 i;

    e = &g_ActiveCharacters[arg0];
    e->unk21 = 1;
    for (i = 0; i < 16; i++) {
        flags = 0xFF;
        cmd = e->commandMenu[i].id;
        if (cmd != 0xFF) {
            flags = D_800707C5[cmd][0];
            if (flags == 0xFF) {
                flags = D_800F5EFC[arg0][0];
            }
            if (cmd < 0x1C) {
                if (cmd >= 0x18) {
                    e->commandMenu[i].allCount = 0xFF;
                }
            }
            if (e->commandMenu[i].initialCursorAction == 7) {
                if (D_800F5BE1[arg0][0] & 2) {
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

void func_801B137C(s32 arg0) {
    s32 i;
    ActiveCharacterData* data;

    data = &g_ActiveCharacters[arg0];
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
} Unk801B13DC; // size:0x68

// Resolves up to three materia slots of character arg0 against the equipment
// mask arg1: a slot whose bit is set copies its paired value into unk3 and
// counts towards unk6.
void func_801B13DC(s32 arg0, s32 arg1, Unk801B13DC* arg2) {
    s32 count;
    s32 i;
    s32 j;

    count = 0;
    for (i = 0; i < 3; i++) {
        if (arg2->materiaID[i] != 0xFF) {
            for (j = 0; j < 12; j++) {
                if (SysGetLimitCmdId(arg0, j) == arg2->materiaID[i]) {
                    break;
                }
            }
            if (j == 12) {
                func_800155A4(0x26);
            } else if ((arg1 >> j) & 1) {
                count++;
                arg2->unk3[i] = arg2->unk14[i].unk0;
            }
        }
    }
    arg2->unk7 = 0;
    arg2->count = count;
}

s32 func_801B14E8(u32 arg0) {
    u8 temp_v1;
    s32 ret;

    temp_v1 = arg0;
    ret = 0;
    if (temp_v1 != 0xFF && (D_800730CC[temp_v1].unk11 & 0xF) == 7) {
        ret = (arg0 >> 8) | 0x80000000;
    }
    return ret;
}

s32 func_801B1530(u32* arg0) {
    s32 ret;
    s32 i;

    ret = 0;
    for (i = 0; i < 8; i++) {
        ret |= func_801B14E8(arg0[0x10 + i]);
        ret |= func_801B14E8(arg0[0x18 + i]);
    }
    return ret;
}

extern u8 D_80071C29[][0x10]; // accessory table, 0x10 stride

// Applies party member `slot`'s equipped accessory: the status the previously
// equipped one granted is cleared first, then the new accessory's permanent
// status is ORed into the combatant, its turn state and the party record.
void func_801B1598(s32 slot, s32 accessory) {
    Unk800AF470* t;
    Unk800F5E60* party;
    Unk800F83E0* c;
    u8 effect;

    t = &g_CombatantTurnState.turn[slot];
    party = &g_CombatantTurnState.party[slot];
    c = &g_BattleState.combatant[slot];
    c->status &= ~party->unk20;
    t->unk34 &= ~party->unk20;
    party->unk20 = 0;
    t->unkD = 0xFF;
    if (accessory != 0xFF) {
        effect = D_80071C29[accessory][0];
        t->unkD = effect;
        switch (effect) {
        case 0:
            c->status |= STATUS_HASTE;
            t->unk34 |= STATUS_HASTE;
            party->unk20 |= STATUS_HASTE;
            break;
        case 1:
            c->status |= STATUS_BERSERK;
            t->unk34 |= STATUS_BERSERK;
            party->unk20 |= STATUS_BERSERK;
            break;
        case 2:
            c->status |= STATUS_D_SENTENCE;
            t->unk34 |= STATUS_D_SENTENCE;
            party->unk20 |= STATUS_D_SENTENCE;
            t->unk12 = 0xFF;
            break;
        case 3:
            c->status |= STATUS_REFLECT;
            t->unk34 |= STATUS_REFLECT;
            party->unk20 |= STATUS_REFLECT;
            break;
        case 6:
            c->status |= STATUS_BARRIER | STATUS_M_BARRIER;
            party->unk20 |= STATUS_BARRIER | STATUS_M_BARRIER;
            break;
        }
    }
}

const s32 D_801B001C[] = {0x0000, 0x1000, 0x0008, 0x0800};
const s32 D_801B002C[] = {0x0000, 0x000A, 0x0027, 0x000A};
extern u8 D_800F9DA0; // pending battle-start status flags, one bit per entry
                      // of D_801B001C / D_801B002C (bit 4 = full-heal)
void func_800A7254(s32, s32, s32, s32);

// Applies the pending battle-start effects in D_800F9DA0 to party member
// `slot`: bit 4 restores half its max HP, bits 0-3 inflict the matching status
// from D_801B001C unless the member's turn state already carries it. Returns
// nonzero if any status was inflicted.
s32 func_801B1734(s32 slot) {
    s32 mask;
    s32 ret;
    s32 i;

    mask = g_CombatantTurnState.turn[slot].unk34;
    g_BattleState.combatant[slot].status &= ~STATUS_D_SENTENCE;
    ret = 0;
    if (g_CombatantTurnState.turn[slot].unk29 & 8) {
        mask |= STATUS_FROG;
    }
    if (D_800F9DA0 & 0x10) {
        g_BattleState.combatant[slot].curHP += g_BattleState.combatant[slot].maxHP >> 1;
        if (g_BattleState.combatant[slot].curHP > g_BattleState.combatant[slot].maxHP) {
            g_BattleState.combatant[slot].curHP = g_BattleState.combatant[slot].maxHP;
        }
        func_800A7254(2, slot, 0x17, 0);
    }
    for (i = 0; i < 4; i++) {
        if ((D_800F9DA0 >> i) & 1) {
            g_BattleState.combatant[slot].status |= D_801B001C[i] & ~mask;
            func_800A7254(2, slot, 0x17, D_801B002C[i]);
            ret = 1;
        }
    }
    return ret;
}

void func_801B18F8(ActiveCharacterData* arg0, Unk800F5E60* arg1, Unk800F83E0* arg2) {
    arg2->unk14 = arg0->dexterity;
    arg2->unk15 = arg0->luck;
    arg2->maxHP = arg0->baseHp;
    arg2->unk2A = arg0->baseMp;
    arg2->unkD = arg0->physAttack;
    arg2->unkE = arg0->magAttack;
    arg2->unk20 = arg0->physDefence;
    arg2->unk22 = arg0->magDefence;
    if (arg2->unkD == 0) {
        arg2->unkD = 1;
    }
    arg1->maxHP = arg2->maxHP;
    arg1->maxMP = arg2->unk2A;
    // 8 = HP_MP_SWAP
    if (arg0->characterFlags & 8) {
        arg1->capHP = 999;
        arg1->capMP = 9999;
    } else {
        arg1->capHP = 9999;
        arg1->capMP = 999;
    }
}

const u8 D_801B003C[] = {0xFF, 0x32, 0x33, 0x34, 0x35, 0xFF, 0x48, 0x07};
extern u16 D_8016376E[3];
extern s16 D_801636BE[][8]; // stride 0x10
void func_800B1060(s32);

// Lays out the two sides for the opening of the battle. D_801B003C picks the
// intro animation for the battle type, then the type decides which rows the
// party and the enemies occupy (row[0]/row[1]/row[2]) and which combatants
// start "turned around" (bit 0x80 of unk4) -- back attacks, side attacks and
// pincers each split the party differently. Finally the front/back row bit is
// re-derived for the three party slots.
void func_801B19AC(void) {
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
    intro = D_801B003C[D_800F5F44.D_800F7DC8];
    if (intro != 0xFF && g_BattleState.sceneID != 0x3D6) {
        func_800B1060(intro);
    }
    mask = 0;
    row[0] = 0;
    row[1] = 0;
    row[2] = 0;
    switch (D_800F5F44.D_800F7DC8) {
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
        switch (D_800F5F44.D_800F7DC8) {
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
    D_8016376E[0] = row[0];
    D_8016376E[1] = row[1];
    D_8016376E[2] = row[2];
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
void func_801B1CB0(void) {
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

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B1E0C);

void func_801B2308(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (D_80163624.unk34[i].unk0 != -1) {
            func_800A6000(i + 4, 0, 0);
        }
    };
    for (i = 0; i < 6; i++) {
        D_80163624.unk34[i].unkC = g_BattleState.combatant[4 + i].unk4;
        D_80163624.unk94[4 + i][1] = g_BattleState.combatant[4 + i].unk10;
        g_BattleState.combatant[4 + i].unk44[0] = g_BattleState.combatant[4 + i].status;
    }
}

static const s8 D_801B0044[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x03, 0x03, 0x03, 0x05, 0x6E, 0x64, 0x62};
static void func_801B23E0(s32 sceneID, void (*cb)(void)) {
    u8 dummy[0x100];
    SceneContainer scene;
    s32 chunkID;
    s32 temp_s1;
    s32 formationIndex;
    s32 i;
    u_long* var_s2;
    s32* var_s5;
    s32* var_s3_2;

    var_s5 = (s32*)0x801C0000;
    chunkID = sceneID / 4;
    temp_s1 = func_801B2738(chunkID);              // sector modified based on the Chunk ID
    SystemLoadFileBySector(                        // load file from disk
        func_800144D8(BATTLE_SCENE) + temp_s1 * 4, // Disk sector where to load the file from
        0x800 * 4,                                 // Size in bytes to copy
        (u_long*)var_s5,                           // Destination
        NULL);
    formationIndex = chunkID - D_80083184[temp_s1];
    func_800145BC(cb); // wait until all data is read, keep executing the vsync
                       // callback until then
    i = var_s5[formationIndex];
    var_s3_2 = &var_s5[i];
    var_s2 = (u_long*)&scene;
    Unzip(        // gzip decompress
        var_s3_2, // src
        var_s2);  // dst
    formationIndex = sceneID - chunkID * 4;
    func_80014A00(D_8016360C.enemyModelIDs, scene.enemyModelIDs, sizeof(scene.enemyModelIDs));
    func_80014A00((s32*)&D_8016360C.setup, &scene.setup[formationIndex], sizeof(BattleSetup));
    func_80014A00((s32*)&D_8016360C.camera, &scene.camera[formationIndex], sizeof(CameraPlacement) * 4);
    func_80014A00((s32*)&D_8016360C.formation, &scene.formation[formationIndex], sizeof(FormationEntry) * 6);
    func_80014A00((s32*)&D_800F5F44.enemy, &scene.enemy, sizeof(scene.enemy));
    func_80014A00((s32*)&D_800F5F44.attacks, &scene.attacks, sizeof(scene.attacks));
    func_80014A00((s32*)&D_800F5F44.attackIDs, scene.attackIDs, sizeof(scene.attackIDs));
    func_80014A00((s32*)&D_800F5F44.attackNames, &scene.attackNames, sizeof(scene.attackNames));
    func_80014A00((s32*)&D_800F5F44._5, &scene.unkC80, sizeof(Unk800F5F44_5));
    func_80014A00((s32*)&D_800F5F44.script, &scene.script, sizeof(scene.script));
    if (D_8016376A & 4 && D_8016360C.setup.flags & SETUP_NO_PREEMPTIVE_STRIKE) {
        if (D_8016360C.setup.type == SETUP_DEFAULT) {
            D_8016360C.setup.type = SETUP_PREEMPTIVE;
        }
    }
    D_800F5F44.D_800F7DC8 = (u8)D_801B0044[D_8016360C.setup.type];
    if (D_8016376A & EVENT_BATTLE_SQUARE) {
        D_8016360C.setup.stageID = 37;
        D_8016360C.setup.flags |= SETUP_CANNOT_ESCAPE;
        D_8016360C.setup.cameraID = (SysGetRandomByteFromTable() & 3) + 0x60;
        D_8016360C.setup.escapeCounter = 1;
        // enemy strength and magic is 25% higher at battle square
        for (i = 0; i < 3; i++) {
            D_800F5F44.enemy[i].unk90[5] *= 2;
            D_800F5F44.enemy[i].strength = func_801B2770(D_800F5F44.enemy[i].strength);
            D_800F5F44.enemy[i].magic = func_801B2770(D_800F5F44.enemy[i].magic);
        }
    } else if (D_8016376A & 8) {
        D_8016360C.setup.flags &= ~SETUP_CANNOT_ESCAPE;
    }
    if (!(D_8016360C.setup.flags & SETUP_CANNOT_ESCAPE)) {
        D_8016376A |= 8;
    }
    D_800F5F44.D_800F7DB2 = D_8016360C.setup.escapeCounter;
    if (D_800F5F44.D_800F7DC8 == 1 || D_800F5F44.D_800F7DC8 == 3) {
        D_800F5F44.D_800F7DB2 = 1;
    }
    D_800F5F44.D_800F7DB6 = D_800F5F44.D_800F7DB2;
}

s32 func_801B2738(s32 arg0) {
    u32 i;

    for (i = 1; i < LEN(D_80083184); i++) {
        if (arg0 < D_80083184[i]) {
            break;
        }
    }
    return i - 1;
}

// increase param by 25%
s32 func_801B2770(s32 arg0) {
    arg0 = (arg0 * 125) / 100;
    if (arg0 > 255) {
        arg0 = 255;
    }
    return arg0;
}
