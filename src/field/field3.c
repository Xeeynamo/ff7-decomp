//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern u8 g_RandomTable[256];
extern u8 g_RainForce;

/////////////////////////////////////////////////
// Begin of field_camera.c
/////////////////////////////////////////////////

const u32 D_800A00DC[] = {0x00000000};
INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelLoadAndInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", HandleKawaiDataInModel);

// Possable Debug routine. Ran at beginning of every main field loop. (FPS?)
void DebugRunEveryLoop(void) {}

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldCameraAssign);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldUpdateMovieStream);

/////////////////////////////////////////////////
// Begin of field_rain.c
/////////////////////////////////////////////////

struct FieldRain {
    /* 0x00 */ SVECTOR p1;
    /* 0x08 */ SVECTOR p2;
    /* 0x10 */ s16 wait;
    /* 0x12 */ s16 rndSeed;
    /* 0x14 */ s16 z;
    /* 0x16 */ s16 render;
};

extern struct FieldRain g_FieldRain[64];
extern s16 D_800E42EE[0x40][12];

void FieldRainInit(struct FieldRenderData* renderData) {
    LINE_F2* line;
    s32 i;
    s32 adjustedIndex;

    for (i = 0; i < LEN(g_FieldRain); i++) {
        g_FieldRain[i].render = 0;
        g_FieldRain[i].rndSeed = i * 4;
        g_FieldRain[i].wait = i % 8;

        line = &renderData->Rain[i];

        SetLineF2(line);
        SetSemiTrans(line, 1);

        renderData->Rain[i].r0 = 0x10;
        renderData->Rain[i].g0 = 0x10;
        renderData->Rain[i].b0 = 0x10;
    }

    SetDrawMode(&renderData->RainDm, 0, 0, GetTPage(0, 1, 0, 0) & 0xffff, NULL);
}

void FieldRainAddToRender(u32* ot, LINE_F2* rain, MATRIX* matrix, DR_MODE* rainDm) {
    long p;
    long flag;
    s32 i;
    s32 j;

    PushMatrix();
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);

    for (i = 0, j = 0; i < LEN(g_FieldRain); i++) {
        // 12 * sizeof(s16) = 24 bytes (0x18), the exact size of FieldRain
        if (D_800E42EE[i][0] == 1) {
            RotTransPers(&g_FieldRain[i].p1, (long*)&rain->x0, &p, &flag);
            RotTransPers(&g_FieldRain[i].p2, (long*)&rain->x1, &p, &flag);
            AddPrim(ot, rain);
        }
        rain++;
    }

    PopMatrix();

    *(u32*)rainDm = (*(u32*)rainDm & 0xFF000000) | (*ot & 0xFFFFFF);

    *ot = (*ot & 0xFF000000) | ((u32)rainDm & 0xFFFFFF);
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldRainUpdate);
#else

extern u8 g_RainControl;
extern s16 g_PlayerModelId;

extern FieldEntity g_FieldEntities[];
extern struct FieldRain g_FieldRain[];

void FieldRainUpdate(void) {
    s32 i;
    s32 limit;
    s32 player;
    s32 max = 255;
    s32 vz;

    if ((g_RainControl & 0x80) == 0) {
        if (g_RainForce != 0) {
            g_RainForce--;
        }
    } else {
        if (g_RainForce != max) {
            g_RainForce++;
        }
    }

    limit = g_RainForce / 4;
    player = g_PlayerModelId;

    for (i = 0; i < 0x40; i++) {
        if (g_FieldRain[i].wait == 0) {
            if (i < limit) {

                u8 seed3;

                g_FieldRain[i].render = 1;
                g_FieldRain[i].rndSeed++;
                g_FieldRain[i].wait = 7;

                g_FieldRain[i].p2.vx =
                    (g_FieldEntities[player].PosX >> 12) + g_RandomTable[g_FieldRain[i].rndSeed & 0xFF] * 12 - 0x600;

                seed3 = g_FieldRain[i].rndSeed * 3;
                g_FieldRain[i].p2.vy = (g_FieldEntities[player].PosY >> 12) + g_RandomTable[seed3] * 12 - 0x600;

                g_FieldRain[i].p1.vx = g_FieldRain[i].p2.vx;
                g_FieldRain[i].p1.vy = g_FieldRain[i].p2.vy;

                g_FieldRain[i].z = (g_FieldEntities[player].PosZ >> 12) - 0x300;
            } else {
                g_FieldRain[i].wait = 1;
                g_FieldRain[i].render = 0;
            }
        }

        g_FieldRain[i].p2.vz = g_FieldRain[i].z + (g_FieldRain[i].wait & 0x7) * 0x80;

        vz = (g_FieldRain[i].wait & 0x7) * 0x80;
        vz += 0x100;

        g_FieldRain[i].p1.vz = g_FieldRain[i].z + vz;

        g_FieldRain[i].wait--;
    }
}
#endif

/////////////////////////////////////////////////
// Begin of field_battle.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldGetRandomU8FromList);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldGetNextRandomU8);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldBattleCheck);

/////////////////////////////////////////////////
// Begin of field_arrow.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldArrowsInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldArrowsAddToRender);

/////////////////////////////////////////////////
// Begin of field_model.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field3", LoadLocalFieldModelAndInitAll);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelCreatePktsAndScale);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelCreatePktsForPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelLoadBsxTexToVram);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelBsxTdbModify);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelStructInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelLoadGlobalModels);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelLoadBcx);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelPrepareRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelAddToRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelAnimCalcMtrxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelScaleModel);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelScalePartVrtxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelScaleAnimTranslat);

/////////////////////////////////////////////////
// Begin of field_kawai_char_model.c
/////////////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiClearData);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiExecute);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetCustomLightToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetVertexColorFromLighting);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetColorToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetColorToPartPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiLoadEyesMouthTexToVram);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiLightingApplyToModel);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiLightingApplyToPolyColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetModelTransparency);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetColorToPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetColorToPartPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiFadeModelColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetCustomLighting);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiColorFadeBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetLightingToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetLightingToPartPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetSplashToPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiInitSplashPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetPartAttribute);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiApplyBoneTransform);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiRenderClippedPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiDirectionalColorGradient);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiGradientColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiAnimatedPointLight);
