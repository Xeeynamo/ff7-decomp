#include "battle.h"

// Growth curves are stored as a multiplier and an offset per (stat, tier).
typedef struct {
    /* 0x0 */ u8 mul;
    /* 0x1 */ s8 add;
} StatGrowth; // size:0x2

// A party member's row on the results screen. Gauges 0..5 are the primary
// stats, 6 is HP and 7 is MP, in SavePartyMember order.
typedef struct {
    /* 0x00 */ s32 tier; // level bracket, 0..7, indexes the growth curve
    /* 0x04 */ u8 charId;
    /* 0x05 */ u8 level;
    /* 0x06 */ u8 unk6;
    /* 0x07 */ u8 unk7;
    /* 0x08 */ u16 baseStat[8];
    /* 0x18 */ u16 stat[8];
    /* 0x28 */ u8* curve;
    /* 0x2C */ u32 expStart;
    /* 0x30 */ u32 exp;
    /* 0x34 */ s32 unk34;
    /* 0x38 */ s32 unk38;
    /* 0x3C */ s32 unk3C[16];
    /* 0x7C */ u32 expLevelStart;
    /* 0x80 */ u32 expLevelEnd;
} BatresRow; // size:0x84

// One party slot's live battle record, three of them starting at 0x800F5E60.
typedef struct {
    /* 0x00 */ u8 unk0[8];
    /* 0x08 */ u16 limitCharge;
    /* 0x0A */ u8 unkA[0x2A];
} Unk800F5E60; // size:0x34

extern u8 D_80082268[];
extern u8 D_80082460[3][12]; // one 12-entry roll table per gauge kind
extern StatGrowth D_80082484[][8];
extern s32 D_8009D7D8;
extern s32 D_8009D7DC;
extern s32 D_8009D7E0;
extern u8 D_8009D7ED[][12];
extern s16 D_8009D7EE[][6]; // same 12-byte record as D_8009D7ED
extern SavePartyMember D_8009C738[];
extern u8 D_8009D58A[]; // gil, stored unaligned, so it is copied a byte at a time
extern Unk800F5E60 D_800F5E60[3];
extern u16 D_800F7DD2;
extern u8 D_80163790[]; // the char_id occupying each of the three party slots
extern SavePartyMember D_80167938;

s32 SysGetLimitCmdId(s32 charId, s32 limitIndex);
void func_801B0EF8(SavePartyMember* c, s32 exp, s32 slot);

INCLUDE_ASM("asm/us/battle/nonmatchings/batres", func_801B0000);

void CommitBattleResults(s32 hpOverride, s32 mpOverride) {
    SavePartyMember* c;
    SavePartyMember* src;
    SavePartyMember* dst;
    u8 gil[4];
    u32 i;
    s32 slot;
    s32 j;
    s32 k;
    s32 hp;
    s32 mp;
    s32 id;
    s32 leader;

    for (i = 0; i < 4; i++) { // the save's gil field is not aligned
        gil[i] = D_8009D58A[i];
    }
    *(s32*)gil += D_8009D7DC;
    for (i = 0; i < 4; i++) {
        D_8009D58A[i] = gil[i];
    }
    for (slot = 0; slot < 3; slot++) {
        hp = g_BattleState.combatant[slot].curHP;
        mp = (u16)g_BattleState.combatant[slot].unk28;
        id = D_80163790[slot];
        if ((D_800F7DD2 >> slot) & 1) {
            hp = hpOverride;
            mp = mpOverride;
        }
        if (hp == 7777) { // all sevens does not survive the battle
            hp = 1;
        }
        for (j = 0; j < 9; j++) {
            c = &D_8009C738[j];
            if (id == c->char_id) {
                c->hp_cur = hp;
                c->mp_cur = mp;
                c->limit_charge = D_800F5E60[slot].limitCharge;
                c->status_flags = g_BattleState.combatant[slot].status & 0x30;
                if (g_BattleState.setupFlags & 0x10) {
                    if (c->char_id == 0) {
                        for (k = 0; k < 12; k++) {
                            if (SysGetLimitCmdId(id, k) != 0x7F) {
                                c->limit_learn |= 1 << k;
                            }
                        }
                        c->limit_level = 4;
                        c->limit_charge = 0;
                    }
                }
                break;
            }
        }
    }
    if (D_8016376A & 0x40) {
        leader = D_80167938.char_id;
        for (j = 0; j < 9; j++) {
            if (leader == D_8009C738[j].char_id) {
                dst = &D_8009C738[j];
                src = &D_80167938;
                *dst = *src;
                return;
            }
        }
    }
}

void GiveMateriaAp(SavePartyMember* c, s32 ap) {
    s32 bits;
    s32 i;
    s32 m;
    u8 id;

    bits = ap << 8;
    for (i = 0; i < 8; i++) {
        m = c->materia_weapon[i];
        id = m;
        if (id != 0xFF && (D_800730CC[id].unk11 & 0xF) == 7) {
            c->materia_weapon[i] = m | bits;
        }
        m = c->materia_armor[i];
        id = m;
        if (id != 0xFF && (D_800730CC[id].unk11 & 0xF) == 7) {
            c->materia_armor[i] = m | bits;
        }
    }
}

void GiveSharedExp(s32 mask) {
    SavePartyMember* c;
    s32 hp;
    s32 mp;
    s32 i;
    u8 id;

    for (i = 0; i < 9; i++) {
        id = D_8009C738[i].char_id;
        if (id < 9 && ((mask >> id) & 1)) {
            c = &D_8009C738[i];
            hp = c->hp_base;
            mp = c->mp_base;
            func_801B0EF8(c, D_8009D7D8 / 2, -1);
            if (c->hp_cur != 0) {
                if (hp < c->hp_base) {
                    c->hp_cur += c->hp_base - hp;
                }
                if (mp < c->mp_base) {
                    c->mp_cur += c->mp_base - mp;
                }
            }
        }
    }
}

void ResetBattleResults(void) {
    s32 mask;
    s32 i;

    mask = 0;
    D_8009D7D8 = 0;
    D_8009D7DC = 0;
    D_8009D7E0 = 0;
    for (i = 0; i < 4; i++) {
        g_BattleItemsEarned[i].id = -1;
        g_BattleItemsEarned[i].enabled = 0;
    }
    for (i = 0; i < 3; i++) {
        D_8009D7EE[i][0] = 0;
        D_8009D7ED[i][0] = 0;
    }
    for (i = 0; i < 3; i++) {
        if (D_8009CBDC[i] == 0xFF) {
            mask |= 1 << i;
        }
    }
    func_80015654(mask);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/batres", func_801B0EF8);

// Load a party member's saved stats into a results row.
void LoadResultsRow(BatresRow* p, SavePartyMember* c) {
    p->charId = c->char_id;
    p->level = c->level;
    p->baseStat[0] = c->strength;
    p->baseStat[1] = c->vitality;
    p->baseStat[2] = c->magic;
    p->baseStat[3] = c->spirit;
    p->baseStat[4] = c->dexterity;
    p->baseStat[5] = c->luck;
    p->baseStat[6] = c->hp_base;
    p->baseStat[7] = c->mp_base;
    p->expStart = c->exp;
}

void StoreResultsRow(BatresRow* p, SavePartyMember* c) {
    c->level = p->level;
    c->strength = p->stat[0];
    c->vitality = p->stat[1];
    c->magic = p->stat[2];
    c->spirit = p->stat[3];
    c->dexterity = p->stat[4];
    c->luck = p->stat[5];
    c->hp_base = p->stat[6];
    c->mp_base = p->stat[7];
    c->exp = p->exp;
    c->exp_to_next_level = p->expLevelEnd - p->exp;
    c->level_progress_bar = 0;
    if (p->expLevelEnd != p->expLevelStart) {
        c->level_progress_bar = (p->exp - p->expLevelStart) * 61 / (p->expLevelEnd - p->expLevelStart);
    }
    if (p->level >= 99) {
        c->exp_to_next_level = 0;
        c->level_progress_bar = 61;
    }
}

void InitResultsRow(BatresRow* p) {
    s32 i;

    p->curve = &D_80082268[p->charId * 56];
    for (i = 0; i < 8; i++) {
        if (p->baseStat[i] == 0) {
            p->baseStat[i] = 1;
        }
        p->stat[i] = p->baseStat[i];
    }
    for (i = 15; i >= 0; i--) {
        p->unk3C[i] = 0;
    }
}

void GiveExp(BatresRow* p) {
    s32 i;
    s32 g;
    s32 lv;
    u32 next;

    p->exp = p->expStart + p->unk34;
    i = 0;
    if (p->exp > 999999999) {
        p->exp = 999999999;
    }
    p->unk38 = CalcTotalExp(p, p->level - 1);
    p->expLevelStart = p->unk38;
    lv = p->level;
    while (lv < 99) {
        next = CalcTotalExp(p, lv);
        p->expLevelEnd = next;
        if (i < 16) {
            p->unk3C[i] = next;
            i++;
        }
        if (p->exp < next) {
            break;
        }
        for (g = 0; g < 6; g++) {
            GrowStat(p, g);
        }
        GrowMaxHp(p);
        GrowMaxMp(p);
        p->level++;
        p->expLevelStart = next;
        lv = p->level;
    }
}

s32 CalcTotalExp(BatresRow* r, s32 level) {
    s32 total;
    s32 i;
    s32 m;
    s32 step;

    total = 0;
    r->tier = 0;
    if (level != 0) {
        if (level < 61) {
            r->tier = (level - 1) / 10;
        } else {
            r->tier = (level - 61) / 20 + 6;
        }
    }
    i = 1;
    if (i <= level) {
        step = D_80082484[r->curve[8]][r->tier].mul;
        m = step;
        do {
            total += (m * i) / 10;
            i++;
            m += step;
        } while (i <= level);
    }
    return total;
}

s32 RollGrowthRank(s32 arg0) {
    s32 v;

    v = arg0 + (SysGetRandomByteFromTable() & 7) + 1;
    if (v < 0) {
        v = 0;
    } else if ((u32)v >= 12) {
        v = 11;
    }
    return v;
}

s32 CalcStatGrowth(BatresRow* p, s32 level, s32 gauge) {
    return D_80082484[p->curve[gauge]][p->tier].mul * level / 100 + D_80082484[p->curve[gauge]][p->tier].add;
}

s32 CalcHpGrowth(BatresRow* p, s32 level) {
    return D_80082484[p->curve[6]][p->tier].mul * level + D_80082484[p->curve[6]][p->tier].add * 40;
}

s32 CalcMpGrowth(BatresRow* p, s32 level) {
    return D_80082484[p->curve[7]][p->tier].mul * level / 10 + D_80082484[p->curve[7]][p->tier].add * 2;
}

void GrowStat(BatresRow* p, s32 gauge) {
    s32 v;

    v = p->stat[gauge] + D_80082460[0][RollGrowthRank(CalcStatGrowth(p, p->level + 1, gauge) - p->stat[gauge])];
    if (v >= 101) {
        v = 100;
    }
    p->stat[gauge] = v;
}

void GrowMaxHp(BatresRow* p) {
    s32 cur;
    s32 next;
    s32 v;

    cur = CalcHpGrowth(p, p->level);
    next = CalcHpGrowth(p, p->level + 1);
    v = p->stat[6] + (next - cur) * D_80082460[1][RollGrowthRank(cur * 100 / p->stat[6] - 100)] / 100;
    if (v >= 10000) {
        v = 9999;
    }
    p->stat[6] = v;
}

void GrowMaxMp(BatresRow* p) {
    s32 cur;
    s32 next;
    s32 v;

    cur = CalcMpGrowth(p, p->level);
    next = CalcMpGrowth(p, p->level + 1);
    v = p->stat[7] + (next - cur) * D_80082460[2][RollGrowthRank(cur * 100 / p->stat[7] - 100)] / 100;
    if (v >= 1000) {
        v = 999;
    }
    p->stat[7] = v;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/batres", func_801B17CC);
