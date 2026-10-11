//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include <inline_c.h>
#include "field_private.h"

#ifdef PLATFORM_PSYZ
#define KAWAI_LOAD_COLOR_OFFSETS(r, g, b)
#define KAWAI_STORE_RGB0(dst)
#define KAWAI_STORE_RGB1(dst)
#else
#define KAWAI_LOAD_COLOR_OFFSETS(r, g, b)                                                                              \
    __asm__ volatile("mtc2 %0, $25;"                                                                                   \
                     "mtc2 %1, $26;"                                                                                   \
                     "mtc2 %2, $27"                                                                                    \
                     :                                                                                                 \
                     : "r"(r), "r"(g), "r"(b))
#define KAWAI_STORE_RGB0(dst) __asm__ volatile("swc2 $20, 0(%0)" : : "r"(dst) : "memory")
#define KAWAI_STORE_RGB1(dst) __asm__ volatile("swc2 $21, 0(%0)" : : "r"(dst) : "memory")
#endif

typedef struct {
    u8 modelCount;
    u8 unused;
    u8 modelParams[15][2];
} FieldKawaiState; // size:0x20

typedef s32 (*FieldKawaiHandler)(FieldModelEntry* model, u8* params);

s32 KawaiSetModelTransparency(FieldModelEntry* model, u8* params);
s32 KawaiSetColorToModelPkts(FieldModelEntry* model, u8* params);
void KawaiSetColorToPartPkts(FieldModelPart* part, s16 r, s16 g, s16 b);
s32 KawaiSetCustomLightToModelPkts(FieldModelEntry* model, u8* params);
s32 KawaiSetColorToPktsBelowLvl(FieldModelEntry* model, u8* params);
s32 KawaiSetLightingToModelPkts(FieldModelEntry* model, u8* params);
s32 KawaiFadeModelColor(FieldModelEntry* model, u8* params);
s32 KawaiSetCustomLighting(FieldModelEntry* model, u8* params);
s32 KawaiColorFadeBelowLvl(FieldModelEntry* model, u8* params);
s32 KawaiSetSplashToPktsBelowLvl(FieldModelEntry* model, u8* params);
s32 KawaiSetPartAttribute(FieldModelEntry* model, u8* params);
s32 KawaiDirectionalColorGradient(FieldModelEntry* model, u8* params);
s32 KawaiApplyBoneTransform(FieldModelEntry* model, u8* params);
s32 KawaiAnimatedPointLight(FieldModelEntry* model, u8* params);

FieldKawaiHandler g_FieldKawaiHandlerTable[14] = {
    KawaiLoadEyesMouthTexToVram, KawaiSetModelTransparency,    KawaiSetColorToModelPkts, KawaiSetCustomLightToModelPkts,
    KawaiSetColorToPktsBelowLvl, KawaiSetLightingToModelPkts,  KawaiFadeModelColor,      KawaiSetCustomLighting,
    KawaiColorFadeBelowLvl,      KawaiSetSplashToPktsBelowLvl, KawaiSetPartAttribute,    KawaiDirectionalColorGradient,
    KawaiApplyBoneTransform,     KawaiAnimatedPointLight,
};

u8 g_FieldPrimitiveBufferIndex = 0;
u8 D_800DF115 = 12;
u8 D_800DF116 = 14;
u8 D_800DF117 = 0;

FieldRenderData* D_800DF118 = NULL;
FieldKawaiHandler* g_FieldKawaiHandlers = g_FieldKawaiHandlerTable;

s16 D_800DF120[256][2] = {
    {0, 4096},      {100, 4094},    {200, 4091},    {301, 4084},    {401, 4076},    {501, 4065},    {601, 4051},
    {700, 4035},    {799, 4017},    {897, 3996},    {995, 3973},    {1092, 3947},   {1189, 3919},   {1284, 3889},
    {1379, 3856},   {1474, 3821},   {1567, 3784},   {1659, 3744},   {1751, 3702},   {1841, 3658},   {1930, 3612},
    {2018, 3563},   {2105, 3513},   {2191, 3460},   {2275, 3405},   {2358, 3348},   {2439, 3289},   {2519, 3229},
    {2598, 3166},   {2675, 3101},   {2750, 3034},   {2824, 2966},   {2896, 2896},   {2966, 2824},   {3034, 2750},
    {3101, 2675},   {3166, 2598},   {3229, 2519},   {3289, 2439},   {3348, 2358},   {3405, 2275},   {3460, 2191},
    {3513, 2105},   {3563, 2018},   {3612, 1930},   {3658, 1841},   {3702, 1751},   {3744, 1659},   {3784, 1567},
    {3821, 1474},   {3856, 1379},   {3889, 1284},   {3919, 1189},   {3947, 1092},   {3973, 995},    {3996, 897},
    {4017, 799},    {4035, 700},    {4051, 601},    {4065, 501},    {4076, 401},    {4084, 301},    {4091, 200},
    {4094, 100},    {4096, 0},      {4094, -100},   {4091, -200},   {4084, -301},   {4076, -401},   {4065, -501},
    {4051, -601},   {4035, -700},   {4017, -799},   {3996, -897},   {3973, -995},   {3947, -1092},  {3919, -1189},
    {3889, -1284},  {3856, -1379},  {3821, -1474},  {3784, -1567},  {3744, -1659},  {3702, -1751},  {3658, -1841},
    {3612, -1930},  {3563, -2018},  {3513, -2105},  {3460, -2191},  {3405, -2275},  {3348, -2358},  {3289, -2439},
    {3229, -2519},  {3166, -2598},  {3101, -2675},  {3034, -2750},  {2966, -2824},  {2896, -2896},  {2824, -2966},
    {2750, -3034},  {2675, -3101},  {2598, -3166},  {2519, -3229},  {2439, -3289},  {2358, -3348},  {2275, -3405},
    {2191, -3460},  {2105, -3513},  {2018, -3563},  {1930, -3612},  {1841, -3658},  {1751, -3702},  {1659, -3744},
    {1567, -3784},  {1474, -3821},  {1379, -3856},  {1284, -3889},  {1189, -3919},  {1092, -3947},  {995, -3973},
    {897, -3996},   {799, -4017},   {700, -4035},   {601, -4051},   {501, -4065},   {401, -4076},   {301, -4084},
    {200, -4091},   {100, -4094},   {0, -4096},     {-100, -4094},  {-200, -4091},  {-301, -4084},  {-401, -4076},
    {-501, -4065},  {-601, -4051},  {-700, -4035},  {-799, -4017},  {-897, -3996},  {-995, -3973},  {-1092, -3947},
    {-1189, -3919}, {-1284, -3889}, {-1379, -3856}, {-1474, -3821}, {-1567, -3784}, {-1659, -3744}, {-1751, -3702},
    {-1841, -3658}, {-1930, -3612}, {-2018, -3563}, {-2105, -3513}, {-2191, -3460}, {-2275, -3405}, {-2358, -3348},
    {-2439, -3289}, {-2519, -3229}, {-2598, -3166}, {-2675, -3101}, {-2750, -3034}, {-2824, -2966}, {-2896, -2896},
    {-2966, -2824}, {-3034, -2750}, {-3101, -2675}, {-3166, -2598}, {-3229, -2519}, {-3289, -2439}, {-3348, -2358},
    {-3405, -2275}, {-3460, -2191}, {-3513, -2105}, {-3563, -2018}, {-3612, -1930}, {-3658, -1841}, {-3702, -1751},
    {-3744, -1659}, {-3784, -1567}, {-3821, -1474}, {-3856, -1379}, {-3889, -1284}, {-3919, -1189}, {-3947, -1092},
    {-3973, -995},  {-3996, -897},  {-4017, -799},  {-4035, -700},  {-4051, -601},  {-4065, -501},  {-4076, -401},
    {-4084, -301},  {-4091, -200},  {-4094, -100},  {-4096, 0},     {-4094, 100},   {-4091, 200},   {-4084, 301},
    {-4076, 401},   {-4065, 501},   {-4051, 601},   {-4035, 700},   {-4017, 799},   {-3996, 897},   {-3973, 995},
    {-3947, 1092},  {-3919, 1189},  {-3889, 1284},  {-3856, 1379},  {-3821, 1474},  {-3784, 1567},  {-3744, 1659},
    {-3702, 1751},  {-3658, 1841},  {-3612, 1930},  {-3563, 2018},  {-3513, 2105},  {-3460, 2191},  {-3405, 2275},
    {-3348, 2358},  {-3289, 2439},  {-3229, 2519},  {-3166, 2598},  {-3101, 2675},  {-3034, 2750},  {-2966, 2824},
    {-2896, 2896},  {-2824, 2966},  {-2750, 3034},  {-2675, 3101},  {-2598, 3166},  {-2519, 3229},  {-2439, 3289},
    {-2358, 3348},  {-2275, 3405},  {-2191, 3460},  {-2105, 3513},  {-2018, 3563},  {-1930, 3612},  {-1841, 3658},
    {-1751, 3702},  {-1659, 3744},  {-1567, 3784},  {-1474, 3821},  {-1379, 3856},  {-1284, 3889},  {-1189, 3919},
    {-1092, 3947},  {-995, 3973},   {-897, 3996},   {-799, 4017},   {-700, 4035},   {-601, 4051},   {-501, 4065},
    {-401, 4076},   {-301, 4084},   {-200, 4091},   {-100, 4094},
};

SVECTOR D_800DF520[240] = {
    {4012, -69, 818, 0},      {4012, -700, 428, 0},     {4012, 700, -428, 0},     {4012, 69, -818, 0},
    {4011, 705, 435, 0},      {4011, -705, -435, 0},    {3853, -1181, -730, 0},   {3853, 1181, 730, 0},
    {3727, 69, -1696, 0},     {3727, -1486, 821, 0},    {3727, -69, 1696, 0},     {3727, 1486, -821, 0},
    {3546, 1181, 1673, 0},    {3546, -1181, -1673, 0},  {3546, -2025, -308, 0},   {3546, 2025, 308, 0},
    {3501, 705, 2005, 0},     {3501, -705, -2005, 0},   {3501, -2108, 265, 0},    {3501, 2108, -265, 0},
    {3498, 1486, -1526, 0},   {3498, 700, -2011, 0},    {3498, -700, 2011, 0},    {3498, -1486, 1526, 0},
    {3050, 2547, 991, 0},     {3050, 2025, 1835, 0},    {3050, -2547, -991, 0},   {3050, -2025, -1835, 0},
    {2994, -2733, 583, 0},    {2994, 700, 2705, 0},     {2994, 2733, -583, 0},    {2994, -700, -2705, 0},
    {2988, -705, 2710, 0},    {2988, 705, -2710, 0},    {2988, -2108, 1843, 0},   {2988, 2108, -1843, 0},
    {2765, 69, 3020, 0},      {2765, -69, -3020, 0},    {2765, -2733, 1288, 0},   {2765, 2733, -1288, 0},
    {2687, -2025, 2334, 0},   {2687, -1181, 2855, 0},   {2687, 1181, -2855, 0},   {2687, 2025, -2334, 0},
    {2676, 2108, 2273, 0},    {2676, 2976, 869, 0},     {2676, -2976, -869, 0},   {2676, -2108, -2273, 0},
    {2532, -3218, -52, 0},    {2532, 3218, 52, 0},      {2532, -1486, -2855, 0},  {2532, 1486, 2855, 0},
    {2079, 2733, 2231, 0},    {2079, 3218, 1446, 0},    {2079, -3218, -1446, 0},  {2079, -2733, -2231, 0},
    {2018, -3218, 1531, 0},   {2018, 69, 3563, 0},      {2018, -69, -3563, 0},    {2018, 3218, -1531, 0},
    {1932, 1486, 3291, 0},    {1932, -1486, -3291, 0},  {1932, 3608, -142, 0},    {1932, -3608, 142, 0},
    {1885, -1181, 3438, 0},   {1885, 1181, -3438, 0},   {1885, -2547, 2594, 0},   {1885, 2547, -2594, 0},
    {1654, 705, -3680, 0},    {1654, -2976, 2276, 0},   {1654, 2976, -2276, 0},   {1654, -705, 3680, 0},
    {1647, -700, -3683, 0},   {1647, -3608, 1020, 0},   {1647, 700, 3683, 0},     {1647, 3608, -1020, 0},
    {1480, -2733, -2667, 0},  {1480, 2733, 2667, 0},    {1480, 3608, 1251, 0},    {1480, -3608, -1251, 0},
    {1389, 2025, -3277, 0},   {1389, -2025, 3277, 0},   {1334, 2108, 3247, 0},    {1334, -2108, -3247, 0},
    {1334, -3848, -433, 0},   {1334, 3848, 433, 0},     {832, 3218, -2392, 0},    {832, -3218, 2392, 0},
    {832, 700, -3948, 0},     {832, -700, 3948, 0},     {829, 2108, -3412, 0},    {829, -2108, 3412, 0},
    {824, 3848, -1135, 0},    {824, -3848, 1135, 0},    {824, 705, 3949, 0},      {824, -705, -3949, 0},
    {802, -4008, -260, 0},    {802, 4008, 260, 0},      {802, 2025, 3468, 0},     {802, -2025, -3468, 0},
    {732, -3608, -1794, 0},   {732, -3218, -2425, 0},   {732, 3608, 1794, 0},     {732, 3218, 2425, 0},
    {496, -4008, 682, 0},     {496, 4008, -682, 0},     {496, 1181, 3890, 0},     {496, -1181, -3890, 0},
    {461, 69, -4069, 0},      {461, -69, 4069, 0},      {461, 3608, -1882, 0},    {461, -3608, 1882, 0},
    {370, 2733, -3028, 0},    {370, 1486, -3798, 0},    {370, -2733, 3028, 0},    {370, -1486, 3798, 0},
    {0, -2976, -2814, 0},     {0, -2547, -3207, 0},     {0, 2547, 3207, 0},       {0, 2976, 2814, 0},
    {0, 3848, 1403, 0},       {0, 4008, 843, 0},        {0, -4008, -843, 0},      {0, -3848, -1403, 0},
    {-370, -1486, 3798, 0},   {-370, 1486, -3798, 0},   {-370, -2733, 3028, 0},   {-370, 2733, -3028, 0},
    {-461, 3608, -1882, 0},   {-461, -3608, 1882, 0},   {-461, -69, 4069, 0},     {-461, 69, -4069, 0},
    {-496, 1181, 3890, 0},    {-496, 4008, -682, 0},    {-496, -4008, 682, 0},    {-496, -1181, -3890, 0},
    {-732, 3218, 2425, 0},    {-732, 3608, 1794, 0},    {-732, -3608, -1794, 0},  {-732, -3218, -2425, 0},
    {-802, -2025, -3468, 0},  {-802, 2025, 3468, 0},    {-802, 4008, 260, 0},     {-802, -4008, -260, 0},
    {-824, 705, 3949, 0},     {-824, -705, -3949, 0},   {-824, 3848, -1135, 0},   {-824, -3848, 1135, 0},
    {-829, -2108, 3412, 0},   {-829, 2108, -3412, 0},   {-832, -700, 3948, 0},    {-832, 700, -3948, 0},
    {-832, -3218, 2392, 0},   {-832, 3218, -2392, 0},   {-1334, 3848, 433, 0},    {-1334, -3848, -433, 0},
    {-1334, 2108, 3247, 0},   {-1334, -2108, -3247, 0}, {-1389, -2025, 3277, 0},  {-1389, 2025, -3277, 0},
    {-1479, 3608, 1251, 0},   {-1480, -3608, -1251, 0}, {-1480, 2733, 2667, 0},   {-1480, -2733, -2667, 0},
    {-1647, -3608, 1020, 0},  {-1647, 3608, -1020, 0},  {-1647, 700, 3683, 0},    {-1647, -700, -3683, 0},
    {-1654, -2976, 2276, 0},  {-1654, 2976, -2276, 0},  {-1654, -705, 3680, 0},   {-1654, 705, -3680, 0},
    {-1885, -2547, 2594, 0},  {-1885, 2547, -2594, 0},  {-1885, -1181, 3438, 0},  {-1885, 1181, -3438, 0},
    {-1932, 3608, -142, 0},   {-1932, -3608, 142, 0},   {-1932, 1486, 3291, 0},   {-1932, -1486, -3291, 0},
    {-2018, -3218, 1531, 0},  {-2018, 3218, -1531, 0},  {-2018, 69, 3563, 0},     {-2018, -69, -3563, 0},
    {-2079, 2733, 2231, 0},   {-2079, -2733, -2231, 0}, {-2079, -3218, -1446, 0}, {-2079, 3218, 1446, 0},
    {-2532, 1486, 2855, 0},   {-2532, -3218, -52, 0},   {-2532, -1486, -2855, 0}, {-2532, 3218, 52, 0},
    {-2676, 2108, 2273, 0},   {-2676, -2108, -2273, 0}, {-2676, -2976, -869, 0},  {-2676, 2976, 869, 0},
    {-2687, -1181, 2855, 0},  {-2687, 2025, -2334, 0},  {-2687, -2025, 2334, 0},  {-2687, 1181, -2855, 0},
    {-2765, 69, 3020, 0},     {-2765, -2733, 1288, 0},  {-2765, 2733, -1288, 0},  {-2765, -69, -3020, 0},
    {-2988, -705, 2710, 0},   {-2988, -2108, 1843, 0},  {-2988, 2108, -1843, 0},  {-2988, 705, -2710, 0},
    {-2994, -2733, 583, 0},   {-2994, 2733, -583, 0},   {-2994, -700, -2705, 0},  {-2994, 700, 2705, 0},
    {-3050, -2025, -1835, 0}, {-3050, 2025, 1835, 0},   {-3050, 2547, 991, 0},    {-3050, -2547, -991, 0},
    {-3498, 700, -2011, 0},   {-3498, 1486, -1526, 0},  {-3498, -700, 2011, 0},   {-3498, -1486, 1526, 0},
    {-3501, 705, 2005, 0},    {-3501, -705, -2005, 0},  {-3501, -2108, 265, 0},   {-3501, 2108, -265, 0},
    {-3546, 2025, 308, 0},    {-3546, -2025, -308, 0},  {-3546, 1181, 1673, 0},   {-3546, -1181, -1673, 0},
    {-3727, -1486, 821, 0},   {-3727, 1486, -821, 0},   {-3727, 69, -1696, 0},    {-3727, -69, 1696, 0},
    {-3853, 1181, 730, 0},    {-3853, -1181, -730, 0},  {-4011, 705, 435, 0},     {-4011, -705, -435, 0},
    {-4012, -69, 818, 0},     {-4012, -700, 428, 0},    {-4012, 700, -428, 0},    {-4012, 69, -818, 0},
};

FieldFaceTextureHeader* g_FieldFaceTextureData = (FieldFaceTextureHeader*)0x801E01F0;

u8 g_FieldEyeTextureIndices[34][7] = {
    {126, 0, 1, 2, 3, 4, 7},
    {126, 8, 9, 10, 11, 12, 15},
    {126, 16, 17, 18, 19, 20, 23},
    {126, 24, 25, 26, 27, 28, 31},
    {126, 32, 33, 34, 35, 36, 39},
    {126, 40, 41, 42, 43, 44, 47},
    {126, 48, 49, 50, 51, 52, 55},
    {126, 56, 57, 58, 59, 60, 63},
    {126, 64, 65, 66, 67, 68, 71},
    {126, 72, 73, 74, 75, 76, 79},
    {126, 80, 126, 126, 126, 126, 126},
    {126, 82, 126, 126, 126, 126, 126},
    {126, 84, 126, 126, 126, 126, 126},
    {126, 86, 126, 126, 126, 126, 126},
    {126, 88, 126, 126, 126, 126, 126},
    {126, 90, 126, 126, 126, 126, 126},
    {126, 92, 126, 126, 126, 126, 126},
    {126, 94, 126, 126, 126, 126, 126},
    {126, 96, 126, 126, 126, 126, 126},
    {126, 98, 126, 126, 126, 126, 126},
    {126, 100, 126, 126, 126, 126, 126},
    {126, 102, 126, 126, 126, 126, 126},
    {126, 104, 126, 126, 126, 126, 126},
    {126, 106, 126, 126, 126, 126, 126},
    {126, 108, 126, 126, 126, 126, 126},
    {126, 110, 126, 126, 126, 126, 126},
    {126, 112, 126, 126, 126, 126, 126},
    {126, 114, 126, 126, 126, 126, 126},
    {126, 116, 126, 126, 126, 126, 126},
    {126, 118, 126, 126, 126, 126, 126},
    {126, 120, 126, 126, 126, 126, 126},
    {126, 122, 126, 126, 126, 126, 126},
    {126, 124, 126, 126, 126, 126, 126},
    {126, 126, 126, 126, 126, 126, 126},
};

u8 g_FieldMouthTextureIndices[34][3] = {
    {126, 5, 6},     {126, 13, 14},   {126, 21, 22},   {126, 29, 30},   {126, 38, 39},   {126, 45, 46},
    {126, 53, 54},   {126, 61, 62},   {126, 69, 70},   {126, 77, 78},   {126, 81, 126},  {126, 83, 126},
    {126, 85, 126},  {126, 87, 126},  {126, 89, 126},  {126, 91, 126},  {126, 93, 126},  {126, 95, 126},
    {126, 97, 126},  {126, 99, 126},  {126, 101, 126}, {126, 103, 126}, {126, 105, 126}, {126, 107, 126},
    {126, 109, 126}, {126, 111, 126}, {126, 113, 126}, {126, 115, 126}, {126, 117, 126}, {126, 119, 126},
    {126, 121, 126}, {126, 123, 126}, {126, 125, 126}, {126, 126, 126},
};

FieldKawaiState g_FieldKawaiState = {0};
u8 g_FieldKawaiParams[32] = {0};
// Sixteen shared effect slots; each handler interprets each 60-byte slot differently.
u32 D_800DFE3C[16][15] = {0};
void* D_800E01FC = (void*)0x801AF800;
u8* D_800E0200 = NULL;
u8* D_800E0204 = NULL;

void KawaiClearData(void) {
    s32 count;
    s32 i;
    FieldKawaiState* state = &g_FieldKawaiState;

    count = 16;
    state->modelCount = count;
    // The original loop also clears the first two bytes of the following buffer.
    for (i = 0; i < count; i++) {
        state->modelParams[i][0] = 0;
        state->modelParams[i][1] = 0;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiExecute);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetCustomLightToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetVertexColorFromLighting);

s32 KawaiSetColorToModelPkts(FieldModelEntry* model, u8* params) {
    FieldModelPart* parts;
    u32 i;
    u32 count;
    s16 r;
    s16 g;
    s16 b;
    u8 unused[8]; // Never read; only here to pad the stack frame out to 0x40.

    count = model->partCount;
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    r = (params[1] << 8) | params[0];
    g = (params[3] << 8) | params[2];
    b = (params[5] << 8) | params[4];
    *(s32*)getScratchAddr(128) = params[6];
    for (i = 0; i < count; i++) {
        KawaiSetColorToPartPkts(&parts[i], r, g, b);
    }
    return 1;
}

void KawaiSetColorToPartPkts(FieldModelPart* part, s16 r, s16 g, s16 b) {
    u8* packets;
    u32* polygons;
    u32 counts;
    u32 count;
    u32 i;
    u32 code;
    s32 rOffset, gOffset, bOffset;

    packets = part->packets;
    polygons = (u32*)(part->data + part->polygonsOffset);
    if (g_FieldPrimitiveBufferIndex) {
        packets += part->packetBufferSize;
    }
    gte_lddp(16);
    rOffset = r << 4;
    gOffset = g << 4;
    bOffset = b << 4;

    if (*(s32*)getScratchAddr(128) & 1) {
        counts = *(u32*)&part->polyGT4Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += 6) {
            POLY_GT4 *polyGT4 = (POLY_GT4*)packets;
            code = getcode(polyGT4);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[1]);
            gte_gpl0();
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[2]);
            gte_gpl0();
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[3]);
            gte_gpl0();
            KAWAI_STORE_RGB0(&polyGT4->r0);
            KAWAI_STORE_RGB1(&polyGT4->r1);
            gte_strgb(&polyGT4->r2);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[4]);
            gte_gpl0();
            gte_strgb(&polyGT4->r3);
            setcode(polyGT4, code);
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += 5) {
            POLY_GT3 *polyGT3 = (POLY_GT3*)packets;
            code = getcode(polyGT3);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[1]);
            gte_gpl0();
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[2]);
            gte_gpl0();
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[3]);
            gte_gpl0();
            KAWAI_STORE_RGB0(&polyGT3->r0);
            KAWAI_STORE_RGB1(&polyGT3->r1);
            gte_strgb(&polyGT3->r2);
            setcode(polyGT3, code);
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += 3) {
            POLY_FT4* polyFT4 = (POLY_FT4*)packets;
            code = getcode(polyFT4);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[1]);
            gte_gpl0();
            gte_strgb(&polyFT4->r0);
            setcode(polyFT4, code);
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += 3) {
            POLY_FT3* polyFT3 = (POLY_FT3*)packets;
            code = getcode(polyFT3);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[1]);
            gte_gpl0();
            gte_strgb(&polyFT3->r0);
            setcode(polyFT3, code);
        }

        counts = *(u32*)&part->polyF3Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += 2) {
            POLY_F3* polyF3 = (POLY_F3*)packets;
            code = getcode(polyF3);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[1]);
            gte_gpl0();
            gte_strgb(&polyF3->r0);
            setcode(polyF3, code);
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += 2) {
            POLY_F4* polyF4 = (POLY_F4*)packets;
            code = getcode(polyF4);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[1]);
            gte_gpl0();
            gte_strgb(&polyF4->r0);
            setcode(polyF4, code);
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += 4) {
            POLY_G3* polyG3 = (POLY_G3*)packets;
            code = getcode(polyG3);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[1]);
            gte_gpl0();
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[2]);
            gte_gpl0();
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[3]);
            gte_gpl0();
            KAWAI_STORE_RGB0(&polyG3->r0);
            KAWAI_STORE_RGB1(&polyG3->r1);
            gte_strgb(&polyG3->r2);
            setcode(polyG3, code);
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += 5) {
            POLY_G4* polyG4 = (POLY_G4*)packets;
            code = getcode(polyG4);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[1]);
            gte_gpl0();
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[2]);
            gte_gpl0();
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[3]);
            gte_gpl0();
            KAWAI_STORE_RGB0(&polyG4->r0);
            KAWAI_STORE_RGB1(&polyG4->r1);
            gte_strgb(&polyG4->r2);
            KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
            gte_ldcv(&polygons[4]);
            gte_gpl0();
            gte_strgb(&polyG4->r3);
            setcode(polyG4, code);
        }

    } else {
        counts = *(u32*)&part->polyGT4Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += 6) {
            POLY_GT4* polyGT4 = (POLY_GT4*)packets;
            if (polyGT4->tag << 8) {
                code = getcode(polyGT4);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[1]);
                gte_gpl0();
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[2]);
                gte_gpl0();
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[3]);
                gte_gpl0();
                KAWAI_STORE_RGB0(&polyGT4->r0);
                KAWAI_STORE_RGB1(&polyGT4->r1);
                gte_strgb(&polyGT4->r2);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[4]);
                gte_gpl0();
                gte_strgb(&polyGT4->r3);
                setcode(polyGT4, code);
            }
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += 5) {
            POLY_GT3* polyGT3 = (POLY_GT3*)packets;
            if (polyGT3->tag << 8) {
                code = getcode(polyGT3);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[1]);
                gte_gpl0();
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[2]);
                gte_gpl0();
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[3]);
                gte_gpl0();
                KAWAI_STORE_RGB0(&polyGT3->r0);
                KAWAI_STORE_RGB1(&polyGT3->r1);
                gte_strgb(&polyGT3->r2);
                setcode(polyGT3, code);
            }
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += 3) {
            POLY_FT4* polyFT4 = (POLY_FT4*)packets;
            if (polyFT4->tag << 8) {
                code = getcode(polyFT4);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[1]);
                gte_gpl0();
                gte_strgb(&polyFT4->r0);
                setcode(polyFT4, code);
            }
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += 3) {
            POLY_FT3* polyFT3 = (POLY_FT3*)packets;
            if (polyFT3->tag << 8) {
                code = getcode(polyFT3);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[1]);
                gte_gpl0();
                gte_strgb(&polyFT3->r0);
                setcode(polyFT3, code);
            }
        }

        counts = *(u32*)&part->polyF3Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += 2) {
            POLY_F3* polyF3 = (POLY_F3*)packets;
            if (polyF3->tag << 8) {
                code = getcode(polyF3);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[1]);
                gte_gpl0();
                gte_strgb(&polyF3->r0);
                setcode(polyF3, code);
            }
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += 2) {
            POLY_F4* polyF4 = (POLY_F4*)packets;
            if (polyF4->tag << 8) {
                code = getcode(polyF4);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[1]);
                gte_gpl0();
                gte_strgb(&polyF4->r0);
                setcode(polyF4, code);
            }
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += 4) {
            POLY_G3* polyG3 = (POLY_G3*)packets;
            if (polyG3->tag << 8) {
                code = getcode(polyG3);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[1]);
                gte_gpl0();
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[2]);
                gte_gpl0();
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[3]);
                gte_gpl0();
                KAWAI_STORE_RGB0(&polyG3->r0);
                KAWAI_STORE_RGB1(&polyG3->r1);
                gte_strgb(&polyG3->r2);
                setcode(polyG3, code);
            }
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += 5) {
            POLY_G4* polyG4 = (POLY_G4*)packets;
            if (polyG4->tag << 8) {
                code = getcode(polyG4);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[1]);
                gte_gpl0();
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[2]);
                gte_gpl0();
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[3]);
                gte_gpl0();
                KAWAI_STORE_RGB0(&polyG4->r0);
                KAWAI_STORE_RGB1(&polyG4->r1);
                gte_strgb(&polyG4->r2);
                KAWAI_LOAD_COLOR_OFFSETS(rOffset, gOffset, bOffset);
                gte_ldcv(&polygons[4]);
                gte_gpl0();
                gte_strgb(&polyG4->r3);
                setcode(polyG4, code);
            }
        }
    }
}

s32 KawaiLoadEyesMouthTexToVram(FieldModelEntry* model, u8* params) {
    RECT rect;
    s32 slot;
    s32 x;
    s32 y;
    u_long* image;
    s32 textureIndex;

    slot = params[3];
    if (slot < 33) {
        textureIndex = params[0];
        image = (u_long*)((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->texturesOffset +
                          g_FieldEyeTextureIndices[model->textureFaceId][textureIndex] * 512);
        x = (slot % 4) * 16;
        y = (slot / 4) * 32 + 256;
        setRECT(&rect, x + 768, y, 8, 32);
        LoadImage(&rect, image);

        textureIndex = params[1];
        image = (u_long*)((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->texturesOffset +
                          g_FieldEyeTextureIndices[model->textureFaceId][textureIndex] * 512);
        setRECT(&rect, x + 776, y, 8, 32);
        LoadImage(&rect, image);

        textureIndex = params[2];
        image = (u_long*)((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->texturesOffset +
                          g_FieldMouthTextureIndices[model->textureFaceId][textureIndex] * 512);
        setRECT(&rect, (slot % 8) * 8 + 768, (slot / 8) * 32 + 416, 8, 32);
        LoadImage(&rect, image);
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiLightingApplyToModel);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiLightingApplyToPolyColor);

s32 KawaiSetModelTransparency(FieldModelEntry* model, u8* params) {
    FieldModelPart* parts;
    FieldModelPart* part;
    u8* packets;
    POLY_GT4* polyGT4;
    POLY_GT3* polyGT3;
    POLY_FT4* polyFT4;
    POLY_FT3* polyFT3;
    POLY_F3* polyF3;
    POLY_F4* polyF4;
    POLY_G3* polyG3;
    POLY_G4* polyG4;
    u32 i, j, k;
    u32 polygonCount;
    u32 enabled;

    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    enabled = params[0];
    for (i = 0; i < model->partCount; i++) {
        part = &parts[i];
        for (j = 0; j < 2; j++) {
            packets = part->packets;
            if (j) {
                packets += part->packetBufferSize;
            }

            polyGT4 = (POLY_GT4*)packets;
            polygonCount = part->polyGT4Count;
            for (k = 0; k < polygonCount; k++) {
                setSemiTrans(polyGT4, enabled);
                setShadeTex(polyGT4, enabled);
                polyGT4++;
                packets += sizeof(POLY_GT4);
            }

            polyGT3 = (POLY_GT3*)packets;
            polygonCount = part->polyGT3Count;
            for (k = 0; k < polygonCount; k++) {
                setSemiTrans(polyGT3, enabled);
                setShadeTex(polyGT3, enabled);
                polyGT3++;
                packets += sizeof(POLY_GT3);
            }

            polyFT4 = (POLY_FT4*)packets;
            polygonCount = part->polyFT4Count;
            for (k = 0; k < polygonCount; k++) {
                setSemiTrans(polyFT4, enabled);
                setShadeTex(polyFT4, enabled);
                polyFT4++;
                packets += sizeof(POLY_FT4);
            }

            polyFT3 = (POLY_FT3*)packets;
            polygonCount = part->polyFT3Count;
            for (k = 0; k < polygonCount; k++) {
                setSemiTrans(polyFT3, enabled);
                setShadeTex(polyFT3, enabled);
                polyFT3++;
                packets += sizeof(POLY_FT3);
            }

            polyF3 = (POLY_F3*)packets;
            polygonCount = part->polyF3Count;
            for (k = 0; k < polygonCount; k++) {
                setSemiTrans(polyF3, enabled);
                setShadeTex(polyF3, enabled);
                polyF3++;
                packets += sizeof(POLY_F3);
            }

            polyF4 = (POLY_F4*)packets;
            polygonCount = part->polyF4Count;
            for (k = 0; k < polygonCount; k++) {
                setSemiTrans(polyF4, enabled);
                setShadeTex(polyF4, enabled);
                polyF4++;
                packets += sizeof(POLY_F4);
            }

            polyG3 = (POLY_G3*)packets;
            polygonCount = part->polyG3Count;
            for (k = 0; k < polygonCount; k++) {
                setSemiTrans(polyG3, enabled);
                setShadeTex(polyG3, enabled);
                polyG3++;
                packets += sizeof(POLY_G3);
            }

            polyG4 = (POLY_G4*)packets;
            polygonCount = part->polyG4Count;
            for (k = 0; k < polygonCount; k++) {
                setSemiTrans(polyG4, enabled);
                setShadeTex(polyG4, enabled);
                polyG4++;
                packets += sizeof(POLY_G4);
            }
        }
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetColorToPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetColorToPartPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiFadeModelColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetCustomLighting);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiColorFadeBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetLightingToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetLightingToPartPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetSplashToPktsBelowLvl);

// One per-bone slot for the waterline effect: a pair of blending textured quads plus the
// per-bone vector and scale used to displace them.
typedef struct {
    /* 0x00 */ POLY_FT4 poly[2];
    /* 0x50 */ s16 unk50;       // x, loaded into the GTE as the per-bone vertex
    /* 0x52 */ s16 unk52;       // y
    /* 0x54 */ s16 unk54;       // z
    /* 0x56 */ u16 unk56;       // never read or written by this module
    /* 0x58 */ s16 effectScale; // abs() divisor, negated bone length
    /* 0x5A */ u16 flags;       // bitmask
} FieldSplashEntry;             // size:0x5C

// Thirty slots of scratch per model, pointed at by D_800E0200.
#define FIELD_SPLASH_SLOTS 30

void KawaiInitSplashPkts(FieldModelEntry* model, s32 slot) {
    FieldSplashEntry* entries;
    FieldModelBone* bones;
    u16 clut;
    u16 tpage;
    s32 last; // Named so gcc keeps the bound in a register (slt) rather than folding it to a slti.
    s32 i;

    entries = (FieldSplashEntry*)((u8*)D_800E0200 + slot * FIELD_SPLASH_SLOTS * sizeof(FieldSplashEntry));
    clut = 0x6C2C;
    // GetGraphType is deliberately called twice cache drops jal
    tpage = getTPage(1, 0, 0x2C0, 0x100);
    bones = (FieldModelBone*)model->modelData;
    last = FIELD_SPLASH_SLOTS + 1;
    for (i = 1; i < last; i++) {
        FieldSplashEntry* entry = &entries[i];

        setPolyFT4(&entry->poly[0]);
        setPolyFT4(&entry->poly[1]);
        entry->poly[1].b0 = 0x80;
        entry->poly[0].b0 = 0x80;
        entry->poly[1].g0 = 0x80;
        entry->poly[0].g0 = 0x80;
        entry->poly[1].r0 = 0x80;
        entry->poly[0].r0 = 0x80;
        entry->poly[1].clut = clut;
        entry->poly[0].clut = clut;
        entry->poly[1].tpage = tpage;
        entry->poly[0].tpage = tpage;
        entry->unk50 = 0;
        entry->unk52 = 0;
        entry->unk54 = 0;
        setSemiTrans(&entry->poly[0], 1);
        setSemiTrans(&entry->poly[1], 1);
        entry->effectScale = -bones[i].length;
        entry->flags = 0;
    }
}

s32 KawaiSetPartAttribute(FieldModelEntry* model, u8* params) {
    s32 count = params[0];
    s32 i;
    s32 partId;
    FieldModelPart* parts;

    if (count > 0) {
        parts = (FieldModelPart*)(model->modelData + model->partsOffset);
        for (i = 0; i < count; i++) {
            partId = params[i * 2 + 1];
            if (partId < model->partCount) {
                parts[partId].flags = params[i * 2 + 2];
            }
        }
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiApplyBoneTransform);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiRenderClippedPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiDirectionalColorGradient);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiGradientColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiAnimatedPointLight);
