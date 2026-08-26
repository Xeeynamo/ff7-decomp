//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"

extern s32 g_dbIndex;
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;
extern s16 D_800F5B74;
extern u8 D_80062D98; // set while the battle is paused

// One slot of the shared battle effect array, as Lv5 Death uses it.
typedef struct Lv5DeathEffect {
    /* 0x00 */ s16 unk0;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ SVECTOR pos;
    /* 0x0C */ u16 unkC;
    /* 0x0E */ s16 unkE;
    /* 0x10 */ char pad10[0x10];
} Lv5DeathEffect; // size:0x20

extern Lv5DeathEffect D_80162978[];

// Primitive buffer, one 0xC000 page per double-buffered frame.
extern char D_801C0E50[];
extern char D_801CCE50[];
extern void* D_801D8E50;
extern Lv5DeathEffect* D_801D8E54; // this effect's slot
extern s32 D_801D8E58;             // number of targets
extern u_long D_801B05E0[];        // TIM uploaded on setup

void func_801B0074(void);
void func_801B01BC(void);
void func_801B0310(void);
void func_801B0414(s32);
void BattleGetPartPosition(s32 arg0, s32 arg1, void* arg2);
s32 BattlePositionToStereoPan(SVECTOR* sv);
void MagicAnimationRegister(s32 arg0, s32 arg1, s32 arg2, void (*func)(int));
void func_800D2980(u_long* addr, s16 imgXY, s16 clutX, s16 clutY);
void func_800D4368(SVECTOR* pos, s32 arg1, s32 arg2);
extern s32 D_801C0868[]; // model data

// Static render descriptor for the func_800D4D90 pass.
typedef struct {
    /* 0x0 */ s32* unk0;
    /* 0x4 */ u8 unk4[3];
    /* 0x7 */ u8 unk7;
    /* 0x8 */ s16 unk8;
    /* 0xA */ s16 unkA;
} Lv5DeathDesc; // size:0xC

extern Lv5DeathDesc D_801C0E44;
void* func_800D4D90(Lv5DeathDesc*, u_long**, int, void*);
void func_800D5774(u32 arg0);

void func_801B0000(void) {
    D_801D8E50 = g_dbIndex == 0 ? D_801C0E50 : D_801CCE50;
    if (D_80162080 < 2) {
        *(s32*)D_801D8E54 = -1;
    }
}

void func_801B0508(s32 targetMask, s32 arg1);

void func_801B0054(s32 targetMask, s32 arg1) {
    func_801B0508(targetMask, arg1);
}

void func_801B0074(void) {
    Lv5DeathEffect* effect;
    Unk801B0C98* desc;
    s32 val;
    // The unused array is required for the match; it gives the 0x58 frame.
    u8 unused[0x38];

    effect = &D_80162978[D_8015169C];
    func_800D4368(&effect->pos, (s16)effect->unkC, -((s16)effect->unkC >> 3));

    // Render descriptor built in scratchpad RAM.
    desc = (Unk801B0C98*)0x1F800000;
    desc->unk0 = D_801C0868;
    *(s32*)0x1F800004 = 0x88;
    *(s16*)0x1F800008 = 0;
    *(s16*)0x1F80000A = 0x800;
    *(s16*)0x1F80000C = 0;
    // The do{}while(0) stops the effect->unk2 load below being scheduled
    // above these stores and is required for the match.
    do {
        *(s16*)0x1F80000E = 0;
    } while (0);

    if (effect->unk2 < 8) {
        val = 0x1000 - (effect->unk2 << 8);
    } else if (effect->unk2 >= 0x25) {
        val = (effect->unk2 << 8) - 0x1D00;
    } else {
        goto skip;
    }
    *(s16*)0x1F80000A = val;
skip:;

    SetFarColor(0, 0, 0);
    D_801D8E50 = func_800D29D4(desc, g_cDb->unk70, 12, D_801D8E50);

    if (effect->unk2 >= 0x2D) {
        effect->unk0 = -1;
    }
    if (D_80062D98 == 0) {
        effect->unk2++;
    }
}

void func_801B01BC(void) {
    Lv5DeathEffect* effect;
    s32 v;
    long long idx;

    effect = &D_80162978[D_8015169C];
    D_801C0E44.unk8 = effect->unk2 & 7;

    v = effect->unk2;
    if (v < 8) {
        v = v << 4;
    } else if (v >= 0x25) {
        v = -0x80 - ((v - 0x25) << 4);
    } else {
        v = 0x80;
    }
    // NOTE: the 64-bit idx is required for the match. It defeats constant
    // folding, so the array base ends up in a register and unk4[0] is reached
    // through it; an int idx or a literal 2LL both fold and do not match.
    idx = 2;
    D_801C0E44.unk4[idx] = v;
    D_801C0E44.unk4[1] = v;
    D_801C0E44.unk4[0] = v;

    func_800D4368(&effect->pos, (s16)effect->unkC, -((s16)effect->unkC >> 2));
    D_801D8E50 = func_800D4D90(&D_801C0E44, g_cDb->unk70, 12, D_801D8E50);

    if (effect->unk2 >= 0x2D) {
        effect->unk0 = -1;
        D_801D8E58--;
    }
    if (D_80062D98 == 0) {
        if (effect->unk2 == 0x23) {
            func_800D5774(effect->unkE);
        }
        effect->unk2++;
    }
}

void func_801B0310(void) {
    Lv5DeathEffect* effect;
    s32 val;

    effect = &D_80162978[D_8015169C];
    if (effect->unk2 < 8) {
        D_800F5B72 = 0;
        D_800F5B71 = 0;
        // The do{}while(0) stops the effect->unk2 reload below being
        // scheduled above these stores and is required for the match.
        do {
            D_800F5B70 = 0;
        } while (0);
        val = effect->unk2 * 320;
    } else if (D_801D8E58 <= 0) {
        if (effect->unkE == 0) {
            effect->unkE = effect->unk2;
        }
        val = 0xA00 - (effect->unk2 - effect->unkE) * 320;
    } else {
        val = 0xA00;
    }

    if (effect->unk2 >= 0x35) {
        val = 0;
        effect->unk0 = -1;
    }

    D_800F5B74 = val;
    if (D_80062D98 == 0) {
        effect->unk2++;
    }
}

void func_801B0414(s32 target) {
    Lv5DeathEffect* effect;
    Lv5DeathEffect* next;

    effect = &D_80162978[BattleEffectRegister(func_801B01BC)];
    BattleGetPartPosition(target, D_801518E4[target].D_8015190F, &effect->pos);
    effect->unkC = 0x1CCC;
    effect->unkE = target;

    next = &D_80162978[BattleEffectRegister(func_801B0074)];
    next->pos = effect->pos;
    next->unkC = 0x13DC;

    BattleCommandSend(0x20, BattlePositionToStereoPan(&effect->pos), 0xAA);
}

void func_801B0508(s32 targetMask, s32 arg1) {
    Lv5DeathEffect* effect;
    s32 count;
    s32 i;

    D_801D8E54 = &D_80162978[BattleEffectRegister(func_801B0000)];
    func_800D2980(D_801B05E0, 0, 0, 0);
    effect = &D_80162978[BattleEffectRegister(func_801B0310)];
    effect->unkE = 0;
    MagicAnimationRegister(targetMask, arg1, 2, func_801B0414);

    i = 0;
    count = 0;
    while (i < 10) {
        if ((targetMask >> i++) & 1) {
            count++;
        }
    }
    D_801D8E58 = count;
}
