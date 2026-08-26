//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"

extern s32 g_dbIndex;
// Battle far colour; func_800B9568 feeds r/g/b straight to SetFarColor.
extern CVECTOR D_800F5B70;
extern s16 D_800F5B74;
extern u8 D_80062D98; // set while the battle is paused

// One slot of the shared battle effect array, as Lv5 Death uses it.
typedef struct Lv5DeathEffect {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
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
    /* 0x4 */ CVECTOR color;
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

void MAGIC_Lv5Death(s32 targetMask, s32 arg1) {
    func_801B0508(targetMask, arg1);
}

void func_801B0074(void) {
    // Unused, but required for the match; it gives the function its 0x58 frame.
    char pad[0x34];
    Lv5DeathEffect* effect;
    s32 rot;
    s32 val;
    Unk801B0C98* desc;

    effect = &D_80162978[D_8015169C];
    rot = effect->unkC << 16;
    func_800D4368(&effect->pos, rot >> 16, -(rot >> 19));

    // Render descriptor built in scratchpad RAM.
    desc = (Unk801B0C98*)0x1F800000;
    desc->unk0 = D_801C0868;
    desc->unk4 = 0x88;
    desc->unk8 = 0;
    desc->unkA = 0x800;
    desc->unkC = 0;
    desc->unkE = 0;

    val = effect->AnimationFrame;
    if (val < 8) {
        val <<= 8;
        desc->unkA = 0x1000 - val;
    } else if (val >= 37) {
        desc->unkA = (val << 8) - 0x1D00;
    }

    SetFarColor(0, 0, 0);
    D_801D8E50 = func_800D29D4(desc, g_cDb->unk70, 12, D_801D8E50);

    if (effect->AnimationFrame >= 45) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

void func_801B01BC(void) {
    Lv5DeathEffect* effect;
    s32 frame;
    u8 color;

    effect = &D_80162978[D_8015169C];
    D_801C0E44.unk8 = effect->AnimationFrame & 7;

    frame = effect->AnimationFrame;
    if (frame < 8) {
        color = frame * 16;
    } else if (frame >= 37) {
        color = -128 - ((frame - 37) * 16);
    } else {
        color = 128;
    }
    D_801C0E44.color.r = D_801C0E44.color.g = D_801C0E44.color.b = color;

    func_800D4368(&effect->pos, (s16)effect->unkC, -((s16)effect->unkC >> 2));
    D_801D8E50 = func_800D4D90(&D_801C0E44, g_cDb->unk70, 12, D_801D8E50);

    if (effect->AnimationFrame >= 45) {
        effect->StartFrame = -1;
        D_801D8E58--;
    }
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 35) {
            func_800D5774(effect->unkE);
        }
        effect->AnimationFrame++;
    }
}

void func_801B0310(void) {
    Lv5DeathEffect* effect;
    s32 val;

    effect = &D_80162978[D_8015169C];
    if (effect->AnimationFrame < 8) {
        D_800F5B70.r = D_800F5B70.g = D_800F5B70.b = 0;
        val = effect->AnimationFrame * 320;
    } else if (D_801D8E58 <= 0) {
        if (effect->unkE == 0) {
            effect->unkE = effect->AnimationFrame;
        }
        val = 2560 - (effect->AnimationFrame - effect->unkE) * 320;
    } else {
        val = 2560;
    }

    if (effect->AnimationFrame >= 53) {
        val = 0;
        effect->StartFrame = -1;
    }

    D_800F5B74 = val;
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
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
