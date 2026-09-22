//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern u8 D_800DFDFC[];
void KawaiClearData(void) {
    s32 i;
    u8* data;
    s32 count;

    data = D_800DFDFC;
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiLoadEyesMouthTexToVram);

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

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiSetPartAttribute);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiApplyBoneTransform);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiRenderClippedPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiDirectionalColorGradient);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiGradientColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field_kawai_char_model", KawaiAnimatedPointLight);
