//! PSYQ=3.3

#include "highway_private.h"

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A00D0);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A0554);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A057C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A2ADC);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A2F4C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A30A0);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A31C8);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A34A4);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A3668);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A37C4);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A397C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A39E0);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway", func_800A3AA0);

// Same as JetNodesInit.
void HighwayNodesInit(void) {
    s32 i;

    HighwayNodeInit(&g_HighwayRootNode, 0);
    g_HighwayRootNode.depth = 0;
    g_HighwayNextFreeNode = 0;
    for (i = 0; i < LEN(g_HighwayNodeFreeList); i++) {
        g_HighwayNodeFreeList[i] = i + 1;
    }
    for (i = 0; i < LEN(g_HighwayNodeListHeads); i++) {
        g_HighwayNodeListHeads[i].next = &g_HighwayNodeListTails[i];
        g_HighwayNodeListTails[i].prev = &g_HighwayNodeListHeads[i];
        g_HighwayNodeListHeads[i].prev = NULL;
        g_HighwayNodeListTails[i].next = NULL;
    }
}
