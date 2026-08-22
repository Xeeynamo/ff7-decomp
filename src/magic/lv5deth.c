#include "common.h"



extern s16 D_80162080;
extern s32 D_801C0E50;
extern s32 D_801CCE50;
extern s32* D_801D8E50;
extern s32* D_801D8E54;
extern s32 g_dbIndex;

void func_801B0000(void)
{
    s32* var_v1;

    var_v1 = &D_801CCE50;

    if (g_dbIndex == 0) {
        var_v1 = &D_801C0E50;
    }

    D_801D8E50 = var_v1;

    if (D_80162080 < 2) {
        *D_801D8E54 = -1;
    }
}



void func_801B0508();                             

void func_801B0054(void)
{
    func_801B0508();
}

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0074);

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B01BC);

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0310);

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0414);

INCLUDE_ASM("asm/us/magic/nonmatchings/lv5deth", func_801B0508);
