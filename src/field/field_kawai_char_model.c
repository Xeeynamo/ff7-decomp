//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern u8 g_FieldKawaiState[];
extern FieldFaceTextureHeader* g_FieldFaceTextureData;
extern u8 g_FieldEyeTextureIndices[34][7];
extern u8 g_FieldMouthTextureIndices[34][3];
void KawaiClearData(void) {
    s32 i;
    u8* data;
    s32 count;

    data = g_FieldKawaiState;
    count = 0x10;
    *data = count;
    i = 0;
    for (; i < count; i++) {
        data[i * 2 + 2] = 0;
        data[i * 2 + 3] = 0;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiExecute);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetCustomLightToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetVertexColorFromLighting);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetColorToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetColorToPartPkts);

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

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetModelTransparency);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetColorToPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetColorToPartPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiFadeModelColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetCustomLighting);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiColorFadeBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetLightingToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetLightingToPartPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetSplashToPktsBelowLvl);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiInitSplashPkts);

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
