#include "common.h"

typedef struct Lv5DeathData {
    char pad0[0x0E];
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

/* LV5 Death functions. */
extern void func_801B0000(void);
extern void func_801B0310(void);
extern void func_801B0414(void);
void func_801B0508(s32 arg0, s32 arg1);

extern s32 D_801B05E0;
extern s32 D_801D8E58;


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

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0074);

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B01BC);

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0310);

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0414);


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