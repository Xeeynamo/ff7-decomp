//! PSYQ=3.3

#include "highway_private.h"

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800AD0A4);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800AD374);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800AD47C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800AD534);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800AD7AC);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800AD854);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800AD8C8);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", HighwayPlaySfx);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", HighwaySetSlotPitch);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", HighwaySetSlotVolume);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800ADC24);

void HighwayTrackAdvance(void) {
    s32 i;

    g_HighwayDistance += g_HighwaySpeed;
    g_HighwaySegmentFrac += g_HighwaySpeed;
    g_HighwaySegmentsCrossed = g_HighwaySegmentFrac >> 8;
    g_HighwaySegmentFrac &= 0xFF;
    g_HighwayTrackSegment += g_HighwaySegmentsCrossed;
    g_HighwayTrackPos = g_HighwayDistance + 0x2300;
    for (i = 0; i < g_HighwaySegmentsCrossed; i++) {
        HighwayTrackFreeSegment();
        HighwayTrackGenerateSegment();
    }
}

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_race", func_800ADD6C);
