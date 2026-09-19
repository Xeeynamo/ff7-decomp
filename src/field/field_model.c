//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", LoadLocalFieldModelAndInitAll);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelCreatePktsAndScale);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelCreatePktsForPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelLoadBsxTexToVram);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelBsxTdbModify);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelStructInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelLoadGlobalModels);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelLoadBcx);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelPrepareRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelAddToRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelAnimCalcMtrxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelScaleModel);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelScalePartVrtxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelScaleAnimTranslat);
