//! PSYQ=3.3

#include "highway_private.h"

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A7050);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A70E4);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A714C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A71E8);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A780C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A7A48);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A7A50);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A7AF0);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A7C34);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A7DA0);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A7F48);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A8010);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A80D4);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A8ADC);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A8B48);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A8C0C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A9658);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A971C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A9A9C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A9B60);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A9DE8);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A9E1C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800A9E88);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AA03C);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AA438);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AA7FC);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AA9E4);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AAACC);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AAB3C);

void HighwayPropsInit(void) {
    s32 offset;
    s32 i;

    if (!g_HighwayArcadeMode) {
        g_HighwayPropData = g_HighwayCourses[0].propData;
        g_HighwayPropScripts[0] = g_HighwayCourses[0].propScripts[0];
        g_HighwayPropScripts[1] = g_HighwayCourses[0].propScripts[1];
        g_HighwayPropScripts[2] = g_HighwayCourses[0].propScripts[2];
        g_HighwayPropScripts[3] = g_HighwayCourses[0].propScripts[3];
        g_HighwayPropScripts[4] = g_HighwayCourses[0].propScripts[4];
        g_HighwayPropScripts[5] = g_HighwayCourses[0].propScripts[5];
        g_HighwayPropScripts[6] = g_HighwayCourses[0].propScripts[6];
        g_HighwayPropScripts[7] = g_HighwayCourses[0].propScripts[7];
        g_HighwayPropScripts[8] = g_HighwayCourses[0].propScripts[8];
        g_HighwayPropScripts[9] = g_HighwayCourses[0].propScripts[9];
    } else {
        g_HighwayPropData = g_HighwayCourses[1].propData;
        g_HighwayPropScripts[0] = g_HighwayCourses[1].propScripts[0];
        g_HighwayPropScripts[1] = g_HighwayCourses[1].propScripts[1];
        g_HighwayPropScripts[2] = g_HighwayCourses[1].propScripts[2];
        g_HighwayPropScripts[3] = g_HighwayCourses[1].propScripts[3];
        g_HighwayPropScripts[4] = g_HighwayCourses[1].propScripts[4];
        g_HighwayPropScripts[5] = g_HighwayCourses[1].propScripts[5];
        g_HighwayPropScripts[6] = g_HighwayCourses[1].propScripts[6];
        g_HighwayPropScripts[7] = g_HighwayCourses[1].propScripts[7];
        g_HighwayPropScripts[8] = g_HighwayCourses[1].propScripts[8];
        g_HighwayPropScripts[9] = g_HighwayCourses[1].propScripts[9];
    }
    g_HighwayPropPatternCount = *g_HighwayPropData;
    offset = g_HighwayPropPatternCount + 1;
    for (i = 0; i < g_HighwayPropPatternCount; i++) {
        if (!g_HighwayArcadeMode) {
            g_HighwayPropPatterns[i] =
                g_HighwayCourses[0].propData + g_HighwayPropData[offset] + (g_HighwayPropData[offset + 1] << 8);
        }
        if (g_HighwayArcadeMode == 1) {
            g_HighwayPropPatterns[i] =
                g_HighwayCourses[1].propData + g_HighwayPropData[offset] + (g_HighwayPropData[offset + 1] << 8);
        }
        offset += 2;
    }
    for (i = 0; i < LEN(g_HighwayPropRecord); i++) {
        g_HighwayPropRecord[i] = 0;
        g_HighwayPropNeedNext[i] = 1;
    }
}

void HighwayPropScriptStep(u8 index, u8* modelId, s16* offset, s16* height, u16* flags, u16* yaw) {
    u8 a;
    u8 b;
    u8 c;
    HighwayPropScript* rec;

    if (g_HighwayPropNeedNext[index]) {
        rec = &g_HighwayPropScripts[index][g_HighwayPropRecord[index]];
        g_HighwayPropCurrent = rec;
        g_HighwayPropRecord[index]++;
        g_HighwayPropPattern[index] = rec->pattern;
        g_HighwayPropRepeat[index] = rec->repeat;
        g_HighwayPropOffset[index] = rec->offset;
        g_HighwayPropHeight[index] = rec->height;
        g_HighwayPropFlags[index] = rec->flags;
        g_HighwayPropYaw[index] = rec->yaw;
        g_HighwayPropPatternLen[index] = g_HighwayPropData[rec->pattern + 1];
        g_HighwayPropPatternPos[index] = 0;
        g_HighwayPropNeedNext[index] = 0;
    }
    HighwayPropPatternGet(g_HighwayPropPattern[index], g_HighwayPropPatternPos[index], &a, &b, &c);
    g_HighwayPropPatternPos[index]++;
    if (g_HighwayPropPatternPos[index] == g_HighwayPropPatternLen[index]) {
        g_HighwayPropPatternPos[index] = 0;
        if (!--g_HighwayPropRepeat[index]) {
            g_HighwayPropNeedNext[index] = 1;
        }
    }
    *modelId = a;
    *offset = g_HighwayPropOffset[index];
    *height = g_HighwayPropHeight[index];
    *flags = g_HighwayPropFlags[index];
    *yaw = g_HighwayPropYaw[index];
}

void HighwayPropPatternGet(u8 table, u8 index, u8* first, u8* second, u8* unused) {
    u8* entry;

    entry = &g_HighwayPropPatterns[table][index * 3];
    *first = entry[0];
    *second = entry[1];
}

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AB674);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AB738);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AC2FC);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AC304);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AC374);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AC3C4);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AC518);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", func_800AC540);
