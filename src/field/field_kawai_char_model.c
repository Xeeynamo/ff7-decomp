//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include <psxsdk/inline_c.h>
#include <psxsdk/gtemac.h>
#include <gte.h>
#include "field_private.h"

typedef struct {
    /* 0x00 */ u8 modelCount;
    /* 0x01 */ u8 unused;
    /* 0x02 */ u8 modelParams[15][2];
} FieldKawaiState; // size:0x20

typedef struct {
    /* 0x00 */ FieldKawaiState state;
    /* 0x20 */ u8 params[32];
    /* 0x40 */ u32 data[16][15];
} FieldKawaiWorkspace; // size:0x400

typedef struct {
    /* 0x00 */ s16 r;
    /* 0x02 */ s16 g;
    /* 0x04 */ s16 b;
    /* 0x06 */ s16 targetR;
    /* 0x08 */ s16 targetG;
    /* 0x0A */ s16 targetB;
    /* 0x0C */ s16 stepR;
    /* 0x0E */ s16 stepG;
    /* 0x10 */ s16 stepB;
    /* 0x12 */ u8 flags;
    /* 0x13 */ u8 finished;
} FieldKawaiColorFade; // size:0x14

typedef struct {
    /* 0x00 */ s16 r;
    /* 0x02 */ s16 g;
    /* 0x04 */ s16 b;
    /* 0x06 */ s16 level;
    /* 0x08 */ s16 targetR;
    /* 0x0A */ s16 targetG;
    /* 0x0C */ s16 targetB;
    /* 0x0E */ s16 targetLevel;
    /* 0x10 */ s16 stepR;
    /* 0x12 */ s16 stepG;
    /* 0x14 */ s16 stepB;
    /* 0x16 */ s16 stepLevel;
    /* 0x18 */ u8 flags;
    /* 0x19 */ u8 finished;
} FieldKawaiColorFadeBelowLvl; // size:0x1A

typedef struct {
    /* 0x00 */ u8 ambient[3];
    /* 0x03 */ u8 lightColors[3][3];
    /* 0x0C */ s16 lightMatrix[3][3];
} FieldKawaiLighting; // size:0x1E

typedef struct {
    /* 0x00 */ s16 r;
    /* 0x02 */ s16 g;
    /* 0x04 */ s16 b;
    /* 0x06 */ s16 level;
    /* 0x08 */ s16 targetR;
    /* 0x0A */ s16 targetG;
    /* 0x0C */ s16 targetB;
    /* 0x0E */ s16 targetLevel;
    /* 0x10 */ u8 flags;
} FieldKawaiGradient; // size:0x12

typedef struct {
    /* 0x00 */ s16 level;
    /* 0x02 */ s16 r;
    /* 0x04 */ s16 g;
    /* 0x06 */ s16 b;
    /* 0x08 */ s16 frame;
    /* 0x0A */ s16 timer;
    /* 0x0C */ u8 flags;
} FieldKawaiSplash; // size:0x0E

typedef struct {
    /* 0x00 */ s16 phase;
    /* 0x02 */ s16 timer;
    /* 0x04 */ s16 state;
    /* 0x06 */ s16 phaseStep;
    /* 0x08 */ s16 r;
    /* 0x0A */ s16 g;
    /* 0x0C */ s16 b;
    /* 0x0E */ s16 waitFrames;
    /* 0x10 */ s16 radius;
    /* 0x12 */ s16 xShift;
    /* 0x14 */ s16 yShift;
    /* 0x16 */ s16 zShift;
    /* 0x18 */ s16 zPhaseShift;
    /* 0x1A */ s16 zOffset;
} FieldKawaiPointLight; // size:0x1C

// One per-bone slot for the waterline effect: a pair of blending textured quads plus the
// per-bone vector and scale used to displace them.
typedef struct {
    /* 0x00 */ POLY_FT4 poly[2];
    /* 0x50 */ SVECTOR position;
    /* 0x58 */ s16 effectScale; // abs() divisor, negated bone length
    /* 0x5A */ s16 flags;       // bitmask
} FieldSplashEntry;             // size:0x5C

// Thirty slots of scratch per model, pointed at by g_FieldKawaiSplashEntries.
#define FIELD_SPLASH_SLOTS 30

#define KAWAI_PART_FLAGS (*(s32*)getScratchAddr(128))
#define KAWAI_PART_LEVEL (*(s16*)getScratchAddr(129))
#define KAWAI_GRADIENT_FLAGS (*getScratchAddr(251))

typedef s32 (*FieldKawaiHandler)(FieldModel* model, u8* params);

s32 KawaiSetModelTransparency(FieldModel* model, u8* params);
void KawaiSetColorToPartPkts(FieldModelPart* part, s16 r, s16 g, s16 b);
void KawaiSetColorToPartPktsBelowLvl(FieldModelPart* part, s32 r, s32 g, s32 b);
void KawaiSetLightingToPartPkts(FieldModelPart* part, s16* light);
s32 KawaiSetCustomLightToModelPkts(FieldModel* model, u8* params);
void KawaiSetVertexColorFromLighting(FieldModelPart* part);
void KawaiLightingApplyToPolyColor(FieldModelPart* part, s32 force);
s32 KawaiSetColorToPktsBelowLvl(FieldModel* model, u8* params);
s32 KawaiSetLightingToModelPkts(FieldModel* model, u8* params);
s32 KawaiFadeModelColor(FieldModel* model, u8* params);
s32 KawaiSetCustomLighting(FieldModel* model, u8* params);
s32 KawaiColorFadeBelowLvl(FieldModel* model, u8* params);
s32 KawaiSetSplashToPktsBelowLvl(FieldModel* model, u8* params);
s32 KawaiSetPartAttribute(FieldModel* model, u8* params);
s32 KawaiDirectionalColorGradient(FieldModel* model, u8* params);
s32 KawaiApplyBoneTransform(FieldModel* model, u8* params);
void KawaiRenderClippedPart(FieldModelPart* part, SVECTOR* normal, MATRIX* toPlane, MATRIX* fromPlane);
s32 KawaiAnimatedPointLight(FieldModel* model, u8* params);

static FieldKawaiHandler kawai_ops_table[14] = {
    KawaiLoadEyesMouthTexToVram, KawaiSetModelTransparency,    KawaiSetColorToModelPkts, KawaiSetCustomLightToModelPkts,
    KawaiSetColorToPktsBelowLvl, KawaiSetLightingToModelPkts,  KawaiFadeModelColor,      KawaiSetCustomLighting,
    KawaiColorFadeBelowLvl,      KawaiSetSplashToPktsBelowLvl, KawaiSetPartAttribute,    KawaiDirectionalColorGradient,
    KawaiApplyBoneTransform,     KawaiAnimatedPointLight,
};

u8 g_FieldPrimitiveBufferIndex = 0;
// Unused.
static u8 D_800DF115 = 12;
static u8 D_800DF116 = 14;
static u8 D_800DF117 = 0;

FieldRenderData* g_FieldCurrentRenderData = NULL;
static FieldKawaiHandler* kawai_ops = kawai_ops_table;

s16 g_FieldSinCosTable[256][2] = {
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

static SVECTOR normal_table[240] = {
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

// clang-format off
static u8 eye_tex_ids[34][7] = {
    {126, 0, 1, 2, 3, 4, 7},             {126, 8, 9, 10, 11, 12, 15},         {126, 16, 17, 18, 19, 20, 23},
    {126, 24, 25, 26, 27, 28, 31},       {126, 32, 33, 34, 35, 36, 39},       {126, 40, 41, 42, 43, 44, 47},
    {126, 48, 49, 50, 51, 52, 55},       {126, 56, 57, 58, 59, 60, 63},       {126, 64, 65, 66, 67, 68, 71},
    {126, 72, 73, 74, 75, 76, 79},       {126, 80, 126, 126, 126, 126, 126},  {126, 82, 126, 126, 126, 126, 126},
    {126, 84, 126, 126, 126, 126, 126},  {126, 86, 126, 126, 126, 126, 126},  {126, 88, 126, 126, 126, 126, 126},
    {126, 90, 126, 126, 126, 126, 126},  {126, 92, 126, 126, 126, 126, 126},  {126, 94, 126, 126, 126, 126, 126},
    {126, 96, 126, 126, 126, 126, 126},  {126, 98, 126, 126, 126, 126, 126},  {126, 100, 126, 126, 126, 126, 126},
    {126, 102, 126, 126, 126, 126, 126}, {126, 104, 126, 126, 126, 126, 126}, {126, 106, 126, 126, 126, 126, 126},
    {126, 108, 126, 126, 126, 126, 126}, {126, 110, 126, 126, 126, 126, 126}, {126, 112, 126, 126, 126, 126, 126},
    {126, 114, 126, 126, 126, 126, 126}, {126, 116, 126, 126, 126, 126, 126}, {126, 118, 126, 126, 126, 126, 126},
    {126, 120, 126, 126, 126, 126, 126}, {126, 122, 126, 126, 126, 126, 126}, {126, 124, 126, 126, 126, 126, 126},
    {126, 126, 126, 126, 126, 126, 126},
};
// clang-format on

static u8 mouth_tex_ids[34][3] = {
    {126, 5, 6},     {126, 13, 14},   {126, 21, 22},   {126, 29, 30},   {126, 38, 39},   {126, 45, 46},
    {126, 53, 54},   {126, 61, 62},   {126, 69, 70},   {126, 77, 78},   {126, 81, 126},  {126, 83, 126},
    {126, 85, 126},  {126, 87, 126},  {126, 89, 126},  {126, 91, 126},  {126, 93, 126},  {126, 95, 126},
    {126, 97, 126},  {126, 99, 126},  {126, 101, 126}, {126, 103, 126}, {126, 105, 126}, {126, 107, 126},
    {126, 109, 126}, {126, 111, 126}, {126, 113, 126}, {126, 115, 126}, {126, 117, 126}, {126, 119, 126},
    {126, 121, 126}, {126, 123, 126}, {126, 125, 126}, {126, 126, 126},
};

static FieldKawaiWorkspace kawai_work = {0};
static KawaiClippedVertex* clipped_verts = (KawaiClippedVertex*)0x801AF800;
u8* g_FieldKawaiSplashEntries = NULL;
u8* g_FieldModelLoadCursor = NULL;

void KawaiClearData(void) {
    s32 count;
    s32 i;
    FieldKawaiState* state = &kawai_work.state;

    count = 16;
    state->modelCount = count;
    // The original loop also clears the first two bytes of the following buffer.
    for (i = 0; i < count; i++) {
        state->modelParams[i][0] = 0;
        state->modelParams[i][1] = 0;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiExecute);

s32 KawaiSetCustomLightToModelPkts(FieldModel* model, u8* params) {
    MATRIX direction;
    MATRIX* partMatrices;
    FieldModelPart* parts;
    u32 i;
    MATRIX* color;

    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    partMatrices = model->partMatrices;
    gte_SetBackColor(params[0], params[1], params[2]);
    color = (MATRIX*)getScratchAddr(128);
    color->m[0][0] = params[3] << 4;
    color->m[1][0] = params[4] << 4;
    color->m[2][0] = params[5] << 4;
    color->m[0][1] = params[6] << 4;
    color->m[1][1] = params[7] << 4;
    color->m[2][1] = params[8] << 4;
    color->m[0][2] = params[9] << 4;
    color->m[1][2] = params[10] << 4;
    color->m[2][2] = params[11] << 4;
    color->t[0] = color->t[1] = color->t[2] = 0;
    gte_SetColorMatrix(color);
    direction.m[0][0] = KAWAI_READ_S16(params, 12);
    direction.m[0][1] = KAWAI_READ_S16(params, 14);
    direction.m[0][2] = KAWAI_READ_S16(params, 16);
    direction.m[1][0] = KAWAI_READ_S16(params, 18);
    direction.m[1][1] = KAWAI_READ_S16(params, 20);
    direction.m[1][2] = KAWAI_READ_S16(params, 22);
    direction.m[2][0] = KAWAI_READ_S16(params, 24);
    direction.m[2][1] = KAWAI_READ_S16(params, 26);
    direction.m[2][2] = KAWAI_READ_S16(params, 28);
    direction.t[0] = direction.t[1] = direction.t[2] = 0;
    for (i = 0; i < model->partCount; i++) {
        if (parts[i].flags) {
            MATRIX* light = (MATRIX*)getScratchAddr(136);
            gte_CompMatrix(&direction, &partMatrices[parts[i].boneIndex], light);
            gte_SetLightMatrix(light);
        }
        KawaiSetVertexColorFromLighting(&parts[i]);
    }
    return 1;
}

void KawaiSetVertexColorFromLighting(FieldModelPart* part) {
    u8* packets;
    SVECTOR* normalTable;
    u8* polygons;
    u32 counts, count, i, j;

    normalTable = normal_table;
    packets = part->packets;
    polygons = part->data + part->polygonsOffset;
    if (g_FieldPrimitiveBufferIndex) {
        packets += part->packetBufferSize;
    }
    counts = *(u32*)&part->polyGT4Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += sizeof(FieldModelPolyGT4)) {
        POLY_GT4* poly = (POLY_GT4*)packets;
        if (poly->tag) {
            FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
            u8* output;
            u8 code = getcode(poly);
            for (j = 0, output = &poly->r0; j < 4; output += 12, j++) {
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += sizeof(FieldModelPolyGT3)) {
        POLY_GT3* poly = (POLY_GT3*)packets;
        if (poly->tag) {
            FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
            u8* output;
            u8 code = getcode(poly);
            for (j = 0, output = &poly->r0; j < 3; output += 12, j++) {
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += sizeof(FieldModelPolyFT4)) {
        POLY_FT4* poly = (POLY_FT4*)packets;
        if (poly->tag) {
            FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += sizeof(FieldModelPolyFT3)) {
        POLY_FT3* poly = (POLY_FT3*)packets;
        if (poly->tag) {
            FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    counts = *(u32*)&part->polyF3Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += sizeof(FieldModelPolyF3)) {
        POLY_F3* poly = (POLY_F3*)packets;
        if (poly->tag) {
            FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += sizeof(FieldModelPolyF4)) {
        POLY_F4* poly = (POLY_F4*)packets;
        if (poly->tag) {
            FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += sizeof(FieldModelPolyG3)) {
        POLY_G3* poly = (POLY_G3*)packets;
        if (poly->tag) {
            FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
            u8* output;
            u8 code = getcode(poly);
            for (j = 0, output = &poly->r0; j < 3; output += 8, j++) {
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += sizeof(FieldModelPolyG4)) {
        POLY_G4* poly = (POLY_G4*)packets;
        if (poly->tag) {
            FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
            u8* output;
            u8 code = getcode(poly);
            for (j = 0, output = &poly->r0; j < 4; output += 8, j++) {
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
}

s32 KawaiSetColorToModelPkts(FieldModel* model, u8* params) {
    FieldModelPart* parts;
    u32 i, count;
    s16 r, g, b;

    count = model->partCount;
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    r = KAWAI_READ_S16(params, 0);
    g = KAWAI_READ_S16(params, 2);
    b = KAWAI_READ_S16(params, 4);
    KAWAI_PART_FLAGS = params[6];
    for (i = 0; i < count; i++) {
        KawaiSetColorToPartPkts(&parts[i], r, g, b);
    }
    return 1;
}

void KawaiSetColorToPartPkts(FieldModelPart* part, s16 r, s16 g, s16 b) {
    u8* packets;
    u8* polygons;
    u32 i, counts, count;
    u8 code;
    s32 rOffset, gOffset, bOffset;

    packets = part->packets;
    polygons = part->data + part->polygonsOffset;
    if (g_FieldPrimitiveBufferIndex) {
        packets += part->packetBufferSize;
    }
    gte_lddp(16);
    rOffset = r << 4;
    gOffset = g << 4;
    bOffset = b << 4;

    if (KAWAI_PART_FLAGS & 1) {
        counts = *(u32*)&part->polyGT4Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += sizeof(FieldModelPolyGT4)) {
            POLY_GT4* polyGT4 = (POLY_GT4*)packets;
            FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
            code = getcode(polyGT4);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[0]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[1]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[2]);
            gte_gpl0();
            gte_stRGB0(&polyGT4->r0);
            gte_stRGB1(&polyGT4->r1);
            gte_strgb(&polyGT4->r2);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[3]);
            gte_gpl0();
            gte_strgb(&polyGT4->r3);
            setcode(polyGT4, code);
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += sizeof(FieldModelPolyGT3)) {
            POLY_GT3* polyGT3 = (POLY_GT3*)packets;
            FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
            code = getcode(polyGT3);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[0]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[1]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[2]);
            gte_gpl0();
            gte_stRGB0(&polyGT3->r0);
            gte_stRGB1(&polyGT3->r1);
            gte_strgb(&polyGT3->r2);
            setcode(polyGT3, code);
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += sizeof(FieldModelPolyFT4)) {
            POLY_FT4* polyFT4 = (POLY_FT4*)packets;
            FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
            code = getcode(polyFT4);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->color);
            gte_gpl0();
            gte_strgb(&polyFT4->r0);
            setcode(polyFT4, code);
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += sizeof(FieldModelPolyFT3)) {
            POLY_FT3* polyFT3 = (POLY_FT3*)packets;
            FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
            code = getcode(polyFT3);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->color);
            gte_gpl0();
            gte_strgb(&polyFT3->r0);
            setcode(polyFT3, code);
        }

        counts = *(u32*)&part->polyF3Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += sizeof(FieldModelPolyF3)) {
            POLY_F3* polyF3 = (POLY_F3*)packets;
            FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
            code = getcode(polyF3);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->color);
            gte_gpl0();
            gte_strgb(&polyF3->r0);
            setcode(polyF3, code);
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += sizeof(FieldModelPolyF4)) {
            POLY_F4* polyF4 = (POLY_F4*)packets;
            FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
            code = getcode(polyF4);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->color);
            gte_gpl0();
            gte_strgb(&polyF4->r0);
            setcode(polyF4, code);
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += sizeof(FieldModelPolyG3)) {
            POLY_G3* polyG3 = (POLY_G3*)packets;
            FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
            code = getcode(polyG3);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[0]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[1]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[2]);
            gte_gpl0();
            gte_stRGB0(&polyG3->r0);
            gte_stRGB1(&polyG3->r1);
            gte_strgb(&polyG3->r2);
            setcode(polyG3, code);
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += sizeof(FieldModelPolyG4)) {
            POLY_G4* polyG4 = (POLY_G4*)packets;
            FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
            code = getcode(polyG4);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[0]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[1]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[2]);
            gte_gpl0();
            gte_stRGB0(&polyG4->r0);
            gte_stRGB1(&polyG4->r1);
            gte_strgb(&polyG4->r2);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[3]);
            gte_gpl0();
            gte_strgb(&polyG4->r3);
            setcode(polyG4, code);
        }
    } else {
        counts = *(u32*)&part->polyGT4Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += sizeof(FieldModelPolyGT4)) {
            POLY_GT4* polyGT4 = (POLY_GT4*)packets;
            FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
            if (polyGT4->tag << 8) {
                code = getcode(polyGT4);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[0]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[1]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[2]);
                gte_gpl0();
                gte_stRGB0(&polyGT4->r0);
                gte_stRGB1(&polyGT4->r1);
                gte_strgb(&polyGT4->r2);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[3]);
                gte_gpl0();
                gte_strgb(&polyGT4->r3);
                setcode(polyGT4, code);
            }
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += sizeof(FieldModelPolyGT3)) {
            POLY_GT3* polyGT3 = (POLY_GT3*)packets;
            FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
            if (polyGT3->tag << 8) {
                code = getcode(polyGT3);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[0]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[1]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[2]);
                gte_gpl0();
                gte_stRGB0(&polyGT3->r0);
                gte_stRGB1(&polyGT3->r1);
                gte_strgb(&polyGT3->r2);
                setcode(polyGT3, code);
            }
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += sizeof(FieldModelPolyFT4)) {
            POLY_FT4* polyFT4 = (POLY_FT4*)packets;
            FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
            if (polyFT4->tag << 8) {
                code = getcode(polyFT4);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->color);
                gte_gpl0();
                gte_strgb(&polyFT4->r0);
                setcode(polyFT4, code);
            }
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += sizeof(FieldModelPolyFT3)) {
            POLY_FT3* polyFT3 = (POLY_FT3*)packets;
            FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
            if (polyFT3->tag << 8) {
                code = getcode(polyFT3);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->color);
                gte_gpl0();
                gte_strgb(&polyFT3->r0);
                setcode(polyFT3, code);
            }
        }

        counts = *(u32*)&part->polyF3Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += sizeof(FieldModelPolyF3)) {
            POLY_F3* polyF3 = (POLY_F3*)packets;
            FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
            if (polyF3->tag << 8) {
                code = getcode(polyF3);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->color);
                gte_gpl0();
                gte_strgb(&polyF3->r0);
                setcode(polyF3, code);
            }
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += sizeof(FieldModelPolyF4)) {
            POLY_F4* polyF4 = (POLY_F4*)packets;
            FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
            if (polyF4->tag << 8) {
                code = getcode(polyF4);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->color);
                gte_gpl0();
                gte_strgb(&polyF4->r0);
                setcode(polyF4, code);
            }
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += sizeof(FieldModelPolyG3)) {
            POLY_G3* polyG3 = (POLY_G3*)packets;
            FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
            if (polyG3->tag << 8) {
                code = getcode(polyG3);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[0]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[1]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[2]);
                gte_gpl0();
                gte_stRGB0(&polyG3->r0);
                gte_stRGB1(&polyG3->r1);
                gte_strgb(&polyG3->r2);
                setcode(polyG3, code);
            }
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += sizeof(FieldModelPolyG4)) {
            POLY_G4* polyG4 = (POLY_G4*)packets;
            FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
            if (polyG4->tag << 8) {
                code = getcode(polyG4);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[0]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[1]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[2]);
                gte_gpl0();
                gte_stRGB0(&polyG4->r0);
                gte_stRGB1(&polyG4->r1);
                gte_strgb(&polyG4->r2);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[3]);
                gte_gpl0();
                gte_strgb(&polyG4->r3);
                setcode(polyG4, code);
            }
        }
    }
}

s32 KawaiLoadEyesMouthTexToVram(FieldModel* model, u8* params) {
    RECT rect;
    s32 slot;
    s32 x, y;
    u_long* image;
    s32 textureIndex;

    slot = params[3];
    if (slot < 33) {
        textureIndex = params[0];
        image = (u_long*)((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->texturesOffset +
                          eye_tex_ids[model->textureFaceId][textureIndex] * 512);
        x = (slot % 4) * 16;
        y = (slot / 4) * 32 + 256;
        setRECT(&rect, x + 768, y, 8, 32);
        LoadImage(&rect, image);

        textureIndex = params[1];
        image = (u_long*)((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->texturesOffset +
                          eye_tex_ids[model->textureFaceId][textureIndex] * 512);
        setRECT(&rect, x + 776, y, 8, 32);
        LoadImage(&rect, image);

        textureIndex = params[2];
        image = (u_long*)((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->texturesOffset +
                          mouth_tex_ids[model->textureFaceId][textureIndex] * 512);
        setRECT(&rect, (slot % 8) * 8 + 768, (slot / 8) * 32 + 416, 8, 32);
        LoadImage(&rect, image);
    }
    return 1;
}

s32 KawaiLightingApplyToModel(FieldModel* model, u8* params) {
    MATRIX direction;
    MATRIX* partMatrices;
    FieldModelPart* parts;
    u32 i;
    MATRIX* color;
    s32 force;

    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    partMatrices = model->partMatrices;
    gte_SetBackColor((s32)params[0], (s32)params[1], (s32)params[2]);
    color = (MATRIX*)getScratchAddr(128);
    color->m[0][0] = params[3] << 4;
    color->m[1][0] = params[4] << 4;
    color->m[2][0] = params[5] << 4;
    color->m[0][1] = params[6] << 4;
    color->m[1][1] = params[7] << 4;
    color->m[2][1] = params[8] << 4;
    color->m[0][2] = params[9] << 4;
    color->m[1][2] = params[10] << 4;
    color->m[2][2] = params[11] << 4;
    color->t[0] = color->t[1] = color->t[2] = 0;
    gte_SetColorMatrix(color);
    direction.m[0][0] = KAWAI_READ_S16(params, 12);
    direction.m[0][1] = KAWAI_READ_S16(params, 14);
    direction.m[0][2] = KAWAI_READ_S16(params, 16);
    direction.m[1][0] = KAWAI_READ_S16(params, 18);
    direction.m[1][1] = KAWAI_READ_S16(params, 20);
    direction.m[1][2] = KAWAI_READ_S16(params, 22);
    direction.m[2][0] = KAWAI_READ_S16(params, 24);
    direction.m[2][1] = KAWAI_READ_S16(params, 26);
    direction.m[2][2] = KAWAI_READ_S16(params, 28);
    direction.t[0] = direction.t[1] = direction.t[2] = 0;
    force = params[30];
    for (i = 0; i < model->partCount; i++) {
        if (parts[i].flags) {
            MATRIX* light = (MATRIX*)getScratchAddr(136);
            if (model->kawaiType != -1) {
                MATRIX* transformedLight = (MATRIX*)getScratchAddr(144);
                gte_CompMatrix(partMatrices, &partMatrices[parts[i].boneIndex], light);
                gte_CompMatrix(&direction, light, transformedLight);
                gte_SetLightMatrix(transformedLight);
            } else {
                gte_CompMatrix(&direction, &partMatrices[parts[i].boneIndex], light);
                gte_SetLightMatrix(light);
            }
        }
        KawaiLightingApplyToPolyColor(&parts[i], force);
    }
    return 1;
}

void KawaiLightingApplyToPolyColor(FieldModelPart* part, s32 force) {
    u8* polygons;
    SVECTOR* normalTable;
    CVECTOR* result;
    u32 counts, count, i, j;

    normalTable = normal_table;
    result = (CVECTOR*)getScratchAddr(0);
    if ((*(u32*)part->data & 0x2) && !force) {
        return;
    }
    polygons = part->data + part->polygonsOffset;
    counts = *(u32*)&part->polyGT4Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyGT4)) {
        FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
        for (j = 0; j < 4; j++) {
            gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j].r, result);
            source->colors[j].r = result->r;
            source->colors[j].g = result->g;
            source->colors[j].b = result->b;
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyGT3)) {
        FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
        for (j = 0; j < 3; j++) {
            gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j].r, result);
            source->colors[j].r = result->r;
            source->colors[j].g = result->g;
            source->colors[j].b = result->b;
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyFT4)) {
        FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
        gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color.r, result);
        source->color.r = result->r;
        source->color.g = result->g;
        source->color.b = result->b;
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyFT3)) {
        FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
        gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color.r, result);
        source->color.r = result->r;
        source->color.g = result->g;
        source->color.b = result->b;
    }
    counts = *(u32*)&part->polyF3Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyF3)) {
        FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
        gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color.r, result);
        source->color.r = result->r;
        source->color.g = result->g;
        source->color.b = result->b;
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyF4)) {
        FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
        gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color.r, result);
        source->color.r = result->r;
        source->color.g = result->g;
        source->color.b = result->b;
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyG3)) {
        FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
        for (j = 0; j < 3; j++) {
            gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j].r, result);
            source->colors[j].r = result->r;
            source->colors[j].g = result->g;
            source->colors[j].b = result->b;
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyG4)) {
        FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
        for (j = 0; j < 4; j++) {
            gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j].r, result);
            source->colors[j].r = result->r;
            source->colors[j].g = result->g;
            source->colors[j].b = result->b;
        }
    }
    *(u32*)part->data |= 0x2;
}

s32 KawaiSetModelTransparency(FieldModel* model, u8* params) {
    FieldModelPart* parts;
    FieldModelPart* part;
    u8* packets;
    u32 i, j, k;
    u32 count, enabled;

    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    enabled = params[0];
    for (i = 0; i < model->partCount; i++) {
        part = &parts[i];
        for (j = 0; j < 2; j++) {
            packets = part->packets;
            if (j) {
                packets += part->packetBufferSize;
            }

            count = part->polyGT4Count;
            for (k = 0; k < count; packets += sizeof(POLY_GT4), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyGT3Count;
            for (k = 0; k < count; packets += sizeof(POLY_GT3), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyFT4Count;
            for (k = 0; k < count; packets += sizeof(POLY_FT4), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyFT3Count;
            for (k = 0; k < count; packets += sizeof(POLY_FT3), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyF3Count;
            for (k = 0; k < count; packets += sizeof(POLY_F3), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyF4Count;
            for (k = 0; k < count; packets += sizeof(POLY_F4), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyG3Count;
            for (k = 0; k < count; packets += sizeof(POLY_G3), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyG4Count;
            for (k = 0; k < count; packets += sizeof(POLY_G4), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }
        }
    }
    return 1;
}

s32 KawaiSetColorToPktsBelowLvl(FieldModel* model, u8* params) {
    FieldModelPart* parts;
    u32 i;
    u8 count;
    s16 r, g, b;
    s16 level;
    s32 flags;

    count = model->partCount;
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    r = KAWAI_READ_S16(params, 0);
    g = KAWAI_READ_S16(params, 2);
    b = KAWAI_READ_S16(params, 4);
    level = KAWAI_READ_S16(params, 6);
    flags = KAWAI_READ_S16(params, 8);
    for (i = 0; i < count; i++) {
        KAWAI_PART_LEVEL = level;
        KAWAI_PART_FLAGS = flags;
        gte_SetRotMatrix(&model->partMatrices[parts[i].boneIndex]);
        gte_SetTransMatrix(&model->partMatrices[parts[i].boneIndex]);
        KawaiSetColorToPartPktsBelowLvl(&parts[i], r, g, b);
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetColorToPartPktsBelowLvl);

s32 KawaiFadeModelColor(FieldModel* model, u8* params) {
    u8* colorParams;
    FieldKawaiColorFade* fade;
    s32 finished;

    fade = (FieldKawaiColorFade*)kawai_work.data[params[1]];
    switch (params[0]) {
    case 0:
        fade->r = KAWAI_READ_S16(params, 2);
        fade->g = KAWAI_READ_S16(params, 4);
        fade->b = KAWAI_READ_S16(params, 6);
        fade->targetR = KAWAI_READ_S16(params, 8);
        fade->targetG = KAWAI_READ_S16(params, 10);
        fade->targetB = KAWAI_READ_S16(params, 12);
        fade->stepR = KAWAI_READ_S16(params, 14);
        fade->stepG = KAWAI_READ_S16(params, 16);
        fade->stepB = KAWAI_READ_S16(params, 18);
        fade->flags = params[20];
        fade->finished = 0;
        return 1;
    case 1:
        finished = 0;
        colorParams = kawai_work.params;
        KAWAI_WRITE_S16(colorParams, 0, fade->r);
        KAWAI_WRITE_S16(colorParams, 2, fade->g);
        KAWAI_WRITE_S16(colorParams, 4, fade->b);
        colorParams[6] = fade->flags;
        KawaiSetColorToModelPkts(model, colorParams);
        if (!fade->finished) {
            fade->r += fade->stepR;
            if ((fade->stepR >= 0 && fade->r >= fade->targetR) || (fade->stepR < 0 && fade->r <= fade->targetR)) {
                finished |= 1;
                fade->r = fade->targetR;
            }
            fade->g += fade->stepG;
            if ((fade->stepG >= 0 && fade->g >= fade->targetG) || (fade->stepG < 0 && fade->g <= fade->targetG)) {
                finished |= 2;
                fade->g = fade->targetG;
            }
            fade->b += fade->stepB;
            if ((fade->stepB >= 0 && fade->b >= fade->targetB) || (fade->stepB < 0 && fade->b <= fade->targetB)) {
                finished |= 4;
                fade->b = fade->targetB;
            }
            if (finished == 7) {
                fade->finished++;
            }
            return 0;
        }
        break;
    }
    return 1;
}

s32 KawaiSetCustomLighting(FieldModel* model, u8* params) {
    u8* lightParams;
    FieldKawaiLighting* lighting = (FieldKawaiLighting*)kawai_work.data[params[1]];

    switch (params[0]) {
    case 0:
        lighting->ambient[0] = params[2];
        lighting->ambient[1] = params[3];
        lighting->ambient[2] = params[4];
        lighting->lightColors[0][0] = params[5];
        lighting->lightColors[0][1] = params[6];
        lighting->lightColors[0][2] = params[7];
        lighting->lightColors[1][0] = params[8];
        lighting->lightColors[1][1] = params[9];
        lighting->lightColors[1][2] = params[10];
        lighting->lightColors[2][0] = params[11];
        lighting->lightColors[2][1] = params[12];
        lighting->lightColors[2][2] = params[13];
        lighting->lightMatrix[0][0] = KAWAI_READ_S16(params, 14);
        lighting->lightMatrix[0][1] = KAWAI_READ_S16(params, 16);
        lighting->lightMatrix[0][2] = KAWAI_READ_S16(params, 18);
        lighting->lightMatrix[1][0] = KAWAI_READ_S16(params, 20);
        lighting->lightMatrix[1][1] = KAWAI_READ_S16(params, 22);
        lighting->lightMatrix[1][2] = KAWAI_READ_S16(params, 24);
        lighting->lightMatrix[2][0] = KAWAI_READ_S16(params, 26);
        lighting->lightMatrix[2][1] = KAWAI_READ_S16(params, 28);
        lighting->lightMatrix[2][2] = KAWAI_READ_S16(params, 30);
        return 1;
    case 1:
        lightParams = kawai_work.params;
        lightParams[0] = lighting->ambient[0];
        lightParams[1] = lighting->ambient[1];
        lightParams[2] = lighting->ambient[2];
        lightParams[3] = lighting->lightColors[0][0];
        lightParams[4] = lighting->lightColors[0][1];
        lightParams[5] = lighting->lightColors[0][2];
        lightParams[6] = lighting->lightColors[1][0];
        lightParams[7] = lighting->lightColors[1][1];
        lightParams[8] = lighting->lightColors[1][2];
        lightParams[9] = lighting->lightColors[2][0];
        lightParams[10] = lighting->lightColors[2][1];
        lightParams[11] = lighting->lightColors[2][2];
        KAWAI_WRITE_S16(lightParams, 12, lighting->lightMatrix[0][0]);
        KAWAI_WRITE_S16(lightParams, 14, lighting->lightMatrix[0][1]);
        KAWAI_WRITE_S16(lightParams, 16, lighting->lightMatrix[0][2]);
        KAWAI_WRITE_S16(lightParams, 18, lighting->lightMatrix[1][0]);
        KAWAI_WRITE_S16(lightParams, 20, lighting->lightMatrix[1][1]);
        KAWAI_WRITE_S16(lightParams, 22, lighting->lightMatrix[1][2]);
        KAWAI_WRITE_S16(lightParams, 24, lighting->lightMatrix[2][0]);
        KAWAI_WRITE_S16(lightParams, 26, lighting->lightMatrix[2][1]);
        KAWAI_WRITE_S16(lightParams, 28, lighting->lightMatrix[2][2]);
        KawaiSetCustomLightToModelPkts(model, lightParams);
        break;
    }
    return 0;
}

s32 KawaiColorFadeBelowLvl(FieldModel* model, u8* params) {
    u8* colorParams;
    s32 finished;
    FieldKawaiColorFadeBelowLvl* fade = (FieldKawaiColorFadeBelowLvl*)kawai_work.data[params[1]];

    switch (params[0]) {
    case 0:
        fade->r = KAWAI_READ_S16(params, 2);
        fade->g = KAWAI_READ_S16(params, 4);
        fade->b = KAWAI_READ_S16(params, 6);
        fade->level = KAWAI_READ_S16(params, 8);
        fade->targetR = KAWAI_READ_S16(params, 10);
        fade->targetG = KAWAI_READ_S16(params, 12);
        fade->targetB = KAWAI_READ_S16(params, 14);
        fade->targetLevel = KAWAI_READ_S16(params, 16);
        fade->stepR = KAWAI_READ_S16(params, 18);
        fade->stepG = KAWAI_READ_S16(params, 20);
        fade->stepB = KAWAI_READ_S16(params, 22);
        fade->stepLevel = KAWAI_READ_S16(params, 24);
        fade->flags = params[26];
        fade->finished = 0;
        return 1;
    case 1:
        finished = 0;
        colorParams = kawai_work.params;
        KAWAI_WRITE_S16(colorParams, 0, fade->r);
        KAWAI_WRITE_S16(colorParams, 2, fade->g);
        KAWAI_WRITE_S16(colorParams, 4, fade->b);
        KAWAI_WRITE_S16(colorParams, 6, fade->level);
        colorParams[8] = fade->flags;
        KawaiSetColorToPktsBelowLvl(model, colorParams);
        if (!fade->finished) {
            fade->r += fade->stepR;
            if ((fade->stepR >= 0 && fade->r >= fade->targetR) || (fade->stepR < 0 && fade->r <= fade->targetR)) {
                finished |= 1;
                fade->r = fade->targetR;
            }
            fade->g += fade->stepG;
            if ((fade->stepG >= 0 && fade->g >= fade->targetG) || (fade->stepG < 0 && fade->g <= fade->targetG)) {
                finished |= 2;
                fade->g = fade->targetG;
            }
            fade->b += fade->stepB;
            if ((fade->stepB >= 0 && fade->b >= fade->targetB) || (fade->stepB < 0 && fade->b <= fade->targetB)) {
                finished |= 4;
                fade->b = fade->targetB;
            }
            fade->level += fade->stepLevel;
            if ((fade->stepLevel >= 0 && fade->level >= fade->targetLevel) ||
                (fade->stepLevel < 0 && fade->level <= fade->targetLevel)) {
                finished |= 8;
                fade->level = fade->targetLevel;
            }
            if (finished == 0xF) {
                fade->finished++;
            }
            return 0;
        }
        break;
    }
    return 1;
}

s32 KawaiSetLightingToModelPkts(FieldModel* model, u8* params) {
    u32 i;
    s16 light[4];
    MATRIX* color;
    MATRIX* partMatrices = model->partMatrices;
    FieldModelPart* parts = (FieldModelPart*)(model->modelData + model->partsOffset);

    gte_SetBackColor(KAWAI_READ_S16(params, 14), KAWAI_READ_S16(params, 16), KAWAI_READ_S16(params, 18));
    color = (MATRIX*)getScratchAddr(128);
    color->m[0][0] = KAWAI_READ_S16(params, 8) << 4;
    color->m[1][0] = KAWAI_READ_S16(params, 10) << 4;
    color->m[2][0] = KAWAI_READ_S16(params, 12) << 4;
    color->m[0][1] = 0;
    color->m[1][1] = 0;
    color->m[2][1] = 0;
    color->m[0][2] = 0;
    color->m[1][2] = 0;
    color->m[2][2] = 0;
    gte_SetColorMatrix(color);
    color->m[1][0] = 0;
    color->m[1][1] = 0;
    color->m[1][2] = 0;
    color->m[2][0] = 0;
    color->m[2][1] = 0;
    color->m[2][2] = 0;
    gte_SetLightMatrix(color);
    light[0] = KAWAI_READ_S16(params, 0);
    light[1] = KAWAI_READ_S16(params, 2);
    light[2] = KAWAI_READ_S16(params, 4);
    light[3] = KAWAI_READ_S16(params, 6);
    for (i = 0; i < model->partCount; i++) {
        if (parts[i].flags) {
            gte_SetRotMatrix(&partMatrices[parts[i].boneIndex]);
            gte_SetTransMatrix(&partMatrices[parts[i].boneIndex]);
            KawaiSetLightingToPartPkts(&parts[i], light);
        }
    }
    return 1;
}

void KawaiSetLightingToPartPkts(FieldModelPart* part, s16* light) {
    MATRIX* matrix;
    VECTOR* transformed;
    SVECTOR* lightVectors;
    SVECTOR* vertices;
    SVECTOR* vector;
    SVECTOR* normalTable;
    u8* packets;
    u8* polygons;
    u32 counts, count, i, j;
    s16 lightX, lightY, lightZ;
    s32 dx, dy, dz, distance, attenuation;

    lightVectors = (SVECTOR*)getScratchAddr(16);
    matrix = (MATRIX*)getScratchAddr(8);
    transformed = (VECTOR*)getScratchAddr(0);
    vector = (SVECTOR*)getScratchAddr(0);
    normalTable = normal_table;
    vertices = (SVECTOR*)(part->data + 4);
    count = part->vertexCount;
    light[3]++;
    lightX = light[0];
    lightY = light[1];
    lightZ = light[2];
    for (i = 0; i < count; i++) {
        gte_ldv0(&vertices[i]);
        gte_rt();
        gte_stlvnl(transformed);
        dx = lightX - transformed->vx;
        dy = lightY - transformed->vy;
        dz = lightZ - transformed->vz;
        distance = SquareRoot0(dx * dx + dy * dy + dz * dz) + 2;
        setVector(&lightVectors[i], (dx << 12) / distance, (dy << 12) / distance, (dz << 12) / distance);
        attenuation = light[3] - distance;
        if (attenuation < 0) {
            attenuation = 0;
        }
        lightVectors[i].pad = (attenuation << 12) / light[3]; // Pad stores attenuation.
    }

    gte_ReadRotMatrix(matrix);
    TransposeMatrix(matrix, (MATRIX*)getScratchAddr(0));
    gte_SetRotMatrix((MATRIX*)getScratchAddr(0));
    gte_SetTransMatrix((MATRIX*)getScratchAddr(0));
    packets = part->packets;
    polygons = part->data + part->polygonsOffset;
    if (g_FieldPrimitiveBufferIndex) {
        packets += part->packetBufferSize;
    }
    counts = *(u32*)&part->polyGT4Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += sizeof(FieldModelPolyGT4)) {
        POLY_GT4* poly = (POLY_GT4*)packets;
        if (poly->tag) {
            FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
            u8 code = getcode(poly);
            u8* output;
            for (j = 0, output = &poly->r0; j < 4; output += 12, j++) {
                SVECTOR* lightVector = &lightVectors[source->vertexIndices[j]];
                setVector(vector, (lightVector->vx * lightVector->pad) >> 12,
                          (lightVector->vy * lightVector->pad) >> 12, (lightVector->vz * lightVector->pad) >> 12);
                gte_ldv0(vector);
                gte_rtv0();
                gte_stsv(vector);
                gte_ldsvllrow0(vector);
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += sizeof(FieldModelPolyGT3)) {
        POLY_GT3* poly = (POLY_GT3*)packets;
        if (poly->tag) {
            FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
            u8 code = getcode(poly);
            u8* output;
            for (j = 0, output = &poly->r0; j < 3; output += 12, j++) {
                SVECTOR* lightVector = &lightVectors[source->vertexIndices[j]];
                setVector(vector, (lightVector->vx * lightVector->pad) >> 12,
                          (lightVector->vy * lightVector->pad) >> 12, (lightVector->vz * lightVector->pad) >> 12);
                gte_ldv0(vector);
                gte_rtv0();
                gte_stsv(vector);
                gte_ldsvllrow0(vector);
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += sizeof(FieldModelPolyFT4)) {
        POLY_FT4* poly = (POLY_FT4*)packets;
        if (poly->tag) {
            FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            SVECTOR* lightVector = &lightVectors[source->vertexIndices[0]];
            setVector(vector, (lightVector->vx * lightVector->pad) >> 12, (lightVector->vy * lightVector->pad) >> 12,
                      (lightVector->vz * lightVector->pad) >> 12);
            gte_ldv0(vector);
            gte_rtv0();
            gte_stsv(vector);
            gte_ldsvllrow0(vector);
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += sizeof(FieldModelPolyFT3)) {
        POLY_FT3* poly = (POLY_FT3*)packets;
        if (poly->tag) {
            FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            SVECTOR* lightVector = &lightVectors[source->vertexIndices[0]];
            setVector(vector, (lightVector->vx * lightVector->pad) >> 12, (lightVector->vy * lightVector->pad) >> 12,
                      (lightVector->vz * lightVector->pad) >> 12);
            gte_ldv0(vector);
            gte_rtv0();
            gte_stsv(vector);
            gte_ldsvllrow0(vector);
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    counts = *(u32*)&part->polyF3Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += sizeof(FieldModelPolyF3)) {
        POLY_F3* poly = (POLY_F3*)packets;
        if (poly->tag) {
            FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            SVECTOR* lightVector = &lightVectors[source->vertexIndices[0]];
            setVector(vector, (lightVector->vx * lightVector->pad) >> 12, (lightVector->vy * lightVector->pad) >> 12,
                      (lightVector->vz * lightVector->pad) >> 12);
            gte_ldv0(vector);
            gte_rtv0();
            gte_stsv(vector);
            gte_ldsvllrow0(vector);
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += sizeof(FieldModelPolyF4)) {
        POLY_F4* poly = (POLY_F4*)packets;
        if (poly->tag) {
            FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            SVECTOR* lightVector = &lightVectors[source->vertexIndices[0]];
            setVector(vector, (lightVector->vx * lightVector->pad) >> 12, (lightVector->vy * lightVector->pad) >> 12,
                      (lightVector->vz * lightVector->pad) >> 12);
            gte_ldv0(vector);
            gte_rtv0();
            gte_stsv(vector);
            gte_ldsvllrow0(vector);
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += sizeof(FieldModelPolyG3)) {
        POLY_G3* poly = (POLY_G3*)packets;
        if (poly->tag) {
            FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
            u8 code = getcode(poly);
            u8* output;
            for (j = 0, output = &poly->r0; j < 3; output += 8, j++) {
                SVECTOR* lightVector = &lightVectors[source->vertexIndices[j]];
                setVector(vector, (lightVector->vx * lightVector->pad) >> 12,
                          (lightVector->vy * lightVector->pad) >> 12, (lightVector->vz * lightVector->pad) >> 12);
                gte_ldv0(vector);
                gte_rtv0();
                gte_stsv(vector);
                gte_ldsvllrow0(vector);
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += sizeof(FieldModelPolyG4)) {
        POLY_G4* poly = (POLY_G4*)packets;
        if (poly->tag) {
            FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
            u8 code = getcode(poly);
            u8* output;
            for (j = 0, output = &poly->r0; j < 4; output += 8, j++) {
                SVECTOR* lightVector = &lightVectors[source->vertexIndices[j]];
                setVector(vector, (lightVector->vx * lightVector->pad) >> 12,
                          (lightVector->vy * lightVector->pad) >> 12, (lightVector->vz * lightVector->pad) >> 12);
                gte_ldv0(vector);
                gte_rtv0();
                gte_stsv(vector);
                gte_ldsvllrow0(vector);
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetSplashToPktsBelowLvl);

void KawaiInitSplashPkts(FieldModel* model, s32 slot) {
    FieldSplashEntry* entries;
    FieldModelBone* bones;
    u16 clut, tpage;
    s32 i, last;

    entries = (FieldSplashEntry*)g_FieldKawaiSplashEntries + slot * FIELD_SPLASH_SLOTS;
    clut = getClut(704, 432);
    tpage = getTPage(1, 0, 704, 256);
    bones = (FieldModelBone*)model->modelData;
    last = FIELD_SPLASH_SLOTS + 1;
    for (i = 1; i < last; i++) {
        FieldSplashEntry* entry = &entries[i];
        setPolyFT4(&entry->poly[0]);
        setPolyFT4(&entry->poly[1]);
        entry->poly[1].b0 = 128;
        entry->poly[0].b0 = 128;
        entry->poly[1].g0 = 128;
        entry->poly[0].g0 = 128;
        entry->poly[1].r0 = 128;
        entry->poly[0].r0 = 128;
        entry->poly[1].clut = clut;
        entry->poly[0].clut = clut;
        entry->poly[1].tpage = tpage;
        entry->poly[0].tpage = tpage;
        setVector(&entry->position, 0, 0, 0);
        setSemiTrans(&entry->poly[0], 1);
        setSemiTrans(&entry->poly[1], 1);
        entry->effectScale = -bones[i].length;
        entry->flags = 0;
    }
}

s32 KawaiSetPartAttribute(FieldModel* model, u8* params) {
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

s32 KawaiApplyBoneTransform(FieldModel* model, u8* params) {
    KawaiClipPlane* plane;
    MATRIX* partMatrices;

    plane = (KawaiClipPlane*)kawai_work.data[params[1]];
    switch (params[0]) {
    case 0: {
        u32 i;
        FieldModelPart* parts;
        SVECTOR* normal = (SVECTOR*)getScratchAddr(128);
        SVECTOR* unit = (SVECTOR*)getScratchAddr(130);
        SVECTOR* anchor = (SVECTOR*)getScratchAddr(132);
        SVECTOR* reference = (SVECTOR*)getScratchAddr(134);
        SVECTOR* axis = (SVECTOR*)getScratchAddr(136);
        SVECTOR* unitAxis = (SVECTOR*)getScratchAddr(138);
        s16 cosine;
        s16 sine;
        s32 xx, yy, zz, xy, xz, yz, xs, ys, zs;
        s32 complement;

        plane->normal[0] = KAWAI_READ_S16(params, 2);
        plane->normal[1] = KAWAI_READ_S16(params, 4);
        plane->normal[2] = KAWAI_READ_S16(params, 6);
        plane->distance = KAWAI_READ_S16(params, 8);
        setVector(normal, plane->normal[0], plane->normal[1], plane->normal[2]);
        normal->pad = 0;
        VectorNormalSS(normal, unit);
        plane->unitNormal[0] = unit->vx;
        plane->unitNormal[1] = unit->vy;
        plane->unitNormal[2] = unit->vz;
        if (unit->vz) {
            anchor->vy = 0;
            anchor->vx = 0;
            anchor->vz = (plane->distance << 12) / unit->vz;
        } else if (unit->vy) {
            anchor->vz = 0;
            anchor->vx = 0;
            anchor->vy = (plane->distance << 12) / unit->vy;
        } else if (unit->vx) {
            anchor->vz = 0;
            anchor->vy = 0;
            anchor->vx = (plane->distance << 12) / unit->vx;
        } else {
            return 1;
        }
        plane->anchor[0] = anchor->vx;
        plane->anchor[1] = anchor->vy;
        plane->anchor[2] = anchor->vz;
        setVector(reference, 0, 0, 4096);
        cosine = (unit->vx * reference->vx + unit->vy * reference->vy + unit->vz * reference->vz) / 4096;
        setVector(axis, (unit->vy * reference->vz - reference->vy * unit->vz) / 4096,
                  (reference->vx * unit->vz - unit->vx * reference->vz) / 4096,
                  (unit->vx * reference->vy - reference->vx * unit->vy) / 4096);
        sine = SquareRoot0(axis->vx * axis->vx + axis->vy * axis->vy + axis->vz * axis->vz);
        VectorNormalSS(axis, unitAxis);
        if (!unitAxis->vx && !unitAxis->vy && !unitAxis->vz) {
            plane->rotation[0][0] = 4096;
            plane->rotation[0][1] = 0;
            plane->rotation[0][2] = 0;
            plane->rotation[1][0] = 0;
            plane->rotation[1][1] = 4096;
            plane->rotation[1][2] = 0;
            plane->rotation[2][0] = 0;
            plane->rotation[2][1] = 0;
            plane->rotation[2][2] = 4096;
        } else {
            xx = unitAxis->vx * unitAxis->vx;
            yy = unitAxis->vy * unitAxis->vy;
            zz = unitAxis->vz * unitAxis->vz;
            xy = unitAxis->vx * unitAxis->vy;
            xz = unitAxis->vx * unitAxis->vz;
            yz = unitAxis->vy * unitAxis->vz;
            xs = unitAxis->vx * sine;
            ys = unitAxis->vy * sine;
            zs = unitAxis->vz * sine;
            complement = 4096 - cosine;
            plane->rotation[0][0] = (xx + cosine * ((4096 * 4096 - xx) / 4096)) / 4096;
            plane->rotation[0][1] = ((xy / 4096) * complement - zs) / 4096;
            plane->rotation[0][2] = ((xz / 4096) * complement + ys) / 4096;
            plane->rotation[1][0] = ((xy / 4096) * complement + zs) / 4096;
            plane->rotation[1][1] = (yy + cosine * ((4096 * 4096 - yy) / 4096)) / 4096;
            plane->rotation[1][2] = ((yz / 4096) * complement - xs) / 4096;
            plane->rotation[2][0] = ((xz / 4096) * complement - ys) / 4096;
            plane->rotation[2][1] = ((yz / 4096) * complement + xs) / 4096;
            plane->rotation[2][2] = (zz + cosine * ((4096 * 4096 - zz) / 4096)) / 4096;
        }
        parts = (FieldModelPart*)(model->modelData + model->partsOffset);
        for (i = 0; i < model->partCount; i++) {
            parts[i].flags = 0;
        }
        break;
    }
    case 1: {
        u32 i;
        FieldModelPart* parts;
        SVECTOR* normal = (SVECTOR*)getScratchAddr(128);
        MATRIX* translation = (MATRIX*)getScratchAddr(130);
        MATRIX* rotation = (MATRIX*)getScratchAddr(138);
        MATRIX* transpose = (MATRIX*)getScratchAddr(146);
        MATRIX* toPlane = (MATRIX*)getScratchAddr(154);
        MATRIX* fromPlane = (MATRIX*)getScratchAddr(162);

        parts = (FieldModelPart*)(model->modelData + model->partsOffset);
        partMatrices = model->partMatrices;
        translation->m[0][0] = translation->m[1][1] = translation->m[2][2] = 4096;
        translation->t[0] = translation->t[1] = translation->t[2] = 0;
        translation->m[0][1] = translation->m[0][2] = translation->m[1][0] = translation->m[1][2] =
            translation->m[2][0] = translation->m[2][1] = 0;
        rotation->t[0] = rotation->t[1] = rotation->t[2] = 0;
        transpose->t[0] = transpose->t[1] = transpose->t[2] = 0;
        setVector(normal, plane->unitNormal[0], plane->unitNormal[1], plane->unitNormal[2]);
        normal->pad = 0;
        for (i = 0; i < model->partCount; i++) {
            translation->t[0] = -plane->anchor[0];
            translation->t[1] = -plane->anchor[1];
            translation->t[2] = -plane->anchor[2];
            rotation->m[0][0] = plane->rotation[0][0];
            rotation->m[0][1] = plane->rotation[0][1];
            rotation->m[0][2] = plane->rotation[0][2];
            rotation->m[1][0] = plane->rotation[1][0];
            rotation->m[1][1] = plane->rotation[1][1];
            rotation->m[1][2] = plane->rotation[1][2];
            rotation->m[2][0] = plane->rotation[2][0];
            rotation->m[2][1] = plane->rotation[2][1];
            rotation->m[2][2] = plane->rotation[2][2];
            gte_CompMatrix(translation, &partMatrices[parts[i].boneIndex], toPlane);
            gte_CompMatrix(rotation, toPlane, toPlane);
            translation->t[0] = plane->anchor[0];
            translation->t[1] = plane->anchor[1];
            translation->t[2] = plane->anchor[2];
            TransposeMatrix(rotation, transpose);
            gte_CompMatrix(partMatrices, translation, fromPlane);
            gte_CompMatrix(fromPlane, transpose, fromPlane);
            KawaiRenderClippedPart(&parts[i], normal, toPlane, fromPlane);
        }
        return 0;
    }
    }
    return 1;
}

void KawaiRenderClippedPart(FieldModelPart* part, SVECTOR* normal, MATRIX* toPlane, MATRIX* fromPlane) {
    FieldRenderData* renderData;
    KawaiClippedVertex* vertices;
    u8* data;
    u8* packets;
    u32 counts;
    u32 count;
    u32 i;
    s32 reverse;

    renderData = g_FieldCurrentRenderData;
    gte_SetRotMatrix(toPlane);
    gte_SetTransMatrix(toPlane);
    data = part->data + 4;
    count = part->vertexCount;
    vertices = clipped_verts;
    reverse = !normal->vx && !normal->vy && normal->vz == -4096;
    for (i = 0; i < count; i++) {
        gte_ldv0(data);
        gte_rtv0tr();
        data += sizeof(SVECTOR);
        gte_stsv(&vertices[i].position);
        if (reverse) {
            if (vertices[i].position.vz > 0) {
                vertices[i].position.vz = 0;
            }
        } else if (vertices[i].position.vz < 0) {
            vertices[i].position.vz = 0;
        }
    }
    gte_SetRotMatrix(fromPlane);
    gte_SetTransMatrix(fromPlane);
    for (i = 0; i < count; i++) {
        gte_ldv0(&vertices[i].position);
        gte_rtps();
        gte_stsxy(&vertices[i].screen);
        gte_stsz(&vertices[i].depth);
    }
    packets = part->packets;
    if (g_FieldPrimitiveBufferIndex) {
        packets += part->packetBufferSize;
    }
    counts = *(u32*)&part->polyGT4Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), data += sizeof(FieldModelPolyGT4)) {
        POLY_GT4* polygon = (POLY_GT4*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        KawaiClippedVertex* v3 = &vertices[indices >> 24];
        s32 facing;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;
        s32 screen3 = v3->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        *(s32*)&polygon->x3 = screen3;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz && !v3->position.vz)) {
            setaddr(polygon, 0);
        } else {
            addPrim(&renderData->ot[(v0->depth + v1->depth + v2->depth + v3->depth) >> 4], polygon);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), data += sizeof(FieldModelPolyGT3)) {
        POLY_GT3* polygon = (POLY_GT3*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        s32 facing;
        u32 depth;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            gte_ldsz3(v0->depth, v1->depth, v2->depth);
            gte_avsz3();
            gte_rdOTZ(depth);
            addPrim(&renderData->ot[depth], polygon);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), data += sizeof(FieldModelPolyFT4)) {
        POLY_FT4* polygon = (POLY_FT4*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        KawaiClippedVertex* v3 = &vertices[indices >> 24];
        s32 facing;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;
        s32 screen3 = v3->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        *(s32*)&polygon->x3 = screen3;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz && !v3->position.vz)) {
            setaddr(polygon, 0);
        } else {
            addPrim(&renderData->ot[(v0->depth + v1->depth + v2->depth + v3->depth) >> 4], polygon);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), data += sizeof(FieldModelPolyFT3)) {
        POLY_FT3* polygon = (POLY_FT3*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        s32 facing;
        u32 depth;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            gte_ldsz3(v0->depth, v1->depth, v2->depth);
            gte_avsz3();
            gte_rdOTZ(depth);
            addPrim(&renderData->ot[depth], polygon);
        }
    }
    counts = *(u32*)&part->polyF3Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F3), data += sizeof(FieldModelPolyF3)) {
        POLY_F3* polygon = (POLY_F3*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        s32 facing;
        u32 depth;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            gte_ldsz3(v0->depth, v1->depth, v2->depth);
            gte_avsz3();
            gte_rdOTZ(depth);
            addPrim(&renderData->ot[depth], polygon);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F4), data += sizeof(FieldModelPolyF4)) {
        POLY_F4* polygon = (POLY_F4*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        KawaiClippedVertex* v3 = &vertices[indices >> 24];
        s32 facing;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;
        s32 screen3 = v3->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        *(s32*)&polygon->x3 = screen3;
        gte_rdMAC0(facing);
        // Unlike GT4 and FT4, the original leaves v3 out of this test.
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            addPrim(&renderData->ot[(v0->depth + v1->depth + v2->depth + v3->depth) >> 4], polygon);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G3), data += sizeof(FieldModelPolyG3)) {
        POLY_G3* polygon = (POLY_G3*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        s32 facing;
        u32 depth;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            gte_ldsz3(v0->depth, v1->depth, v2->depth);
            gte_avsz3();
            gte_rdOTZ(depth);
            addPrim(&renderData->ot[depth], polygon);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G4), data += sizeof(FieldModelPolyG4)) {
        POLY_G4* polygon = (POLY_G4*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        KawaiClippedVertex* v3 = &vertices[indices >> 24];
        s32 facing;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;
        s32 screen3 = v3->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        *(s32*)&polygon->x3 = screen3;
        gte_rdMAC0(facing);
        // Unlike GT4 and FT4, the original leaves v3 out of this test.
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            addPrim(&renderData->ot[(v0->depth + v1->depth + v2->depth + v3->depth) >> 4], polygon);
        }
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiDirectionalColorGradient);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiGradientColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiAnimatedPointLight);
