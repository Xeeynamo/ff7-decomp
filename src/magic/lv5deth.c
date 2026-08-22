#include "common.h"
#include "../battle/battle.h"

typedef struct Lv5DeathData {
    s16 unk0;
    s16 unk2;
    SVECTOR unk4;
    u16 unkC;
    s16 unkE;
    char pad10[0x10];
} Lv5DeathData;

/* Battle effect instance array. */
extern Lv5DeathData D_80162978[];


/* State used by the LV5 Death effect. */
extern s16 D_80162080;
extern s32 D_801C0E50;
extern s32 D_801CCE50;
extern s32 *D_801D8E50;
extern s32 *D_801D8E54;
extern s32 g_dbIndex;

/* Battle / magic engine functions. */
extern s32 BattleEffectRegister(void (*func)(void));
extern void MagicAnimationRegister(s32, s32, s32, void (*func)(void));
extern void func_800D2980(void *, s32, s32, s32);
extern s32 *func_800D29D4(s32, s32, s32, s32 *);
extern void func_800D4368(void *, s16, s32);
extern s32 func_800D4D90(s8*, s32, s32, s32);
extern void func_800D5774(s16);

/* LV5 Death functions. */
extern void func_801B0000(void);
extern void func_801B0074(void);
extern void func_801B0310(void);
extern void func_801B0414(s32 arg0);

void func_801B01BC();



extern func_801B0508(s32 arg0, s32 arg1);

extern s32 D_801B05E0;
extern s32 D_801D8E58;
extern s32 *D_801D8E50;
extern u8 D_80062D98;
extern s16 D_8015169C;
extern s32 D_801C0868;
extern s32 g_cDb;



void func_801B0000(void)
{
    s32 *var_v1;

    var_v1 = &D_801CCE50;

    if (g_dbIndex == 0) {
        var_v1 = &D_801C0E50;
    }

    D_801D8E50 = var_v1;

    if (D_80162080 < 2) {
        *D_801D8E54 = -1;
    }
}


void func_801B0054(void)
{
    func_801B0508();
}



void func_801B0074(void)
{
    char pad[0x34];

    Lv5DeathData *temp_s0;
    s32 temp_a2;
    s32 temp_v1;
    s32 *var_s1;
    
    temp_s0 = &D_80162978[D_8015169C];

    temp_a2 = temp_s0->unkC << 16;

    func_800D4368(
        &temp_s0->unk4,
        temp_a2 >> 16,
        -(temp_a2 >> 19)
    );

    var_s1 = (s32 *)0x1F800000;

    var_s1[0] = (s32)&D_801C0868;
    var_s1[1] = 0x88;
    ((s16 *)var_s1)[4] = 0;
    ((s16 *)var_s1)[5] = 0x800;
    ((s16 *)var_s1)[6] = 0;
    ((s16 *)var_s1)[7] = 0;


    temp_v1 = temp_s0->unk2;
    if (temp_v1 < 8) {
        temp_v1 <<= 8;
        ((s16 *)var_s1)[5] = 0x1000 - temp_v1;
    } else if (temp_v1 >= 0x25) {
        ((s16 *)var_s1)[5] = (temp_v1 << 8) - 0x1D00;
    }
    SetFarColor(0, 0, 0);

    D_801D8E50 = func_800D29D4(
        (s32)var_s1,
        g_cDb + 0x70,
        0xC,
        D_801D8E50
    );

    if (temp_s0->unk2 >= 0x2D) {
        temp_s0->unk0 = -1;
    }

    if (D_80062D98 == 0) {
        temp_s0->unk2++;
    }
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B01BC);
#else

extern s16 D_8015169C;

extern u8 D_80062D98;

extern s8 D_801C0E48;
extern volatile s8 D_801C0E49;
extern volatile s8 D_801C0E4A;
extern s16 D_801C0E4C;

extern s32 D_801D8E50;
extern s32 D_801D8E58;
extern s32 g_cDb;

extern void func_800D4368(void*, s16, s32);
extern s32 func_800D4D90(s8*, s32, s32, s32);
extern void func_800D5774(s16);

void func_801B01BC(void) {
    s16 temp_v1;
    u8 var_v1;
    s32 temp_a2;
    s8* temp_s0;
    s16* temp_unk2;
    Lv5DeathData* temp_s1;

    temp_s1 = &D_80162978[D_8015169C];

    temp_unk2 = &temp_s1->unk2;

    D_801C0E4C = *temp_unk2 & 7;

    temp_v1 = *temp_unk2;

    if (temp_v1 < 8) {
        var_v1 = temp_v1 * 0x10;
    } else if (temp_v1 >= 0x25) {
        var_v1 = -0x80 - ((temp_v1 - 0x25) * 0x10);
    } else {
        var_v1 = 0x80;
    }

    temp_s0 = &D_801C0E48;

    D_801C0E4A = var_v1;
    D_801C0E49 = var_v1;
    *(volatile s8*)temp_s0 = var_v1;

    temp_a2 = temp_s1->unkC;

    func_800D4368(
        temp_s1->unk4,
        (s16)temp_a2,
        -((s32)(temp_a2 << 16) >> 18)
    );

    D_801D8E50 = func_800D4D90(
        temp_s0 - 4,
        g_cDb + 0x70,
        0xC,
        D_801D8E50
    );

    if (temp_s1->unk2 >= 0x2D) {
        temp_s1->unk0 = -1;
        D_801D8E58 -= 1;
    }

    if (D_80062D98 == 0) {
        if (temp_s1->unk2 == 0x23) {
            func_800D5774(temp_s1->unkE);
        }

        temp_s1->unk2++;
    }
}

#endif

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0310);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0414);
#else



extern u8 D_8015190F[];

void func_801B0414(s32 arg0)
{
    Lv5DeathData* temp_s0;
    Lv5DeathData* temp_v0;
    s32 temp_s1;
    s8* temp_s3;

    temp_s1 = arg0;

    temp_s0 = &D_80162978[BattleEffectRegister(func_801B01BC)];

    temp_s3 = (s8*)&temp_s0->unk4;

    BattleGetPartPosition(
        temp_s1,
        D_8015190F[temp_s1 * 0xB9C],
        temp_s3
    );

    temp_s0->unkC = 0x1CCC;
    temp_s0->unkE = temp_s1;

    temp_v0 = &D_80162978[BattleEffectRegister(func_801B0074)];

    temp_v0->unk4 = temp_s0->unk4;

    temp_v0->unkC = 0x13DC;

    BattleCommandSend(
        0x20,
        BattlePositionToStereoPan(temp_s3),
        0xAA
    );
}
#endif


void func_801B0508(s32 arg0, s32 arg1)
{
    s32 var_a0;
    s32 var_v1;
    Lv5DeathData *var_v0;

    D_801D8E54 = &D_80162978[BattleEffectRegister(func_801B0000)];

    func_800D2980(&D_801B05E0, 0, 0, 0);

    var_v0 = &D_80162978[BattleEffectRegister(func_801B0310)];

    var_v0->unkE = 0;

    MagicAnimationRegister(arg0, arg1, 2, func_801B0414);

    var_v1 = 0;
    var_a0 = 0;

    do {
        if ((arg0 >> var_v1++) & 1) {
            var_a0++;
        }
    } while (var_v1 < 0xA);

    D_801D8E58 = var_a0;
}
