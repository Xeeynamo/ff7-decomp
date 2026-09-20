//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", LoadLocalFieldModelAndInitAll);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelCreatePktsAndScale);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelCreatePktsForPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelLoadBsxTexToVram);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelBsxTdbModify);

u8* FieldModelStructInit(FieldModelLoaderHeader* header, FieldModelData* modelData) {
    FieldModelLoaderData* loaders;
    FieldModelEntry* entry;
    u8* buffer;
    s16 partsOffset;
    u32 i;

    buffer = (u8*)modelData;
    modelData->modelCount = 0;
    loaders = (FieldModelLoaderData*)(header + 1);
    for (i = 0; i < header->modelCount; i++) {
        if (loaders[i].npcFlag) {
            loaders[i].modelEntryIndex = modelData->modelCount;
            modelData->modelCount++;
        } else {
            loaders[i].modelEntryIndex = 0xFF;
        }
    }

    modelData->unk2 = 0;
    modelData->unk1 = 0;
    modelData->modelEntries = (FieldModelEntry*)(modelData + 1);
    modelData->unk8 = NULL;
    buffer += sizeof(FieldModelData) + modelData->modelCount * sizeof(FieldModelEntry);
    for (i = 0; i < header->modelCount; i++) {
        if (loaders[i].npcFlag) {
            if (loaders[i].globalModelId > 0 && loaders[i].globalModelId < 10 && loaders[i].animationCount < 3) {
                loaders[i].animationCount = 3;
            }
            entry = &modelData->modelEntries[loaders[i].modelEntryIndex];
            entry->flags = 1;
            entry->kawaiType = -1;
            entry->boneCount = loaders[i].boneCount;
            entry->partCount = loaders[i].partCount;
            entry->animationCount = loaders[i].animationCount;
            entry->rotationZ = 0;
            entry->rotationY = 0;
            entry->rotationX = 0;
            entry->translationZ = 0;
            entry->translationY = 0;
            entry->translationX = 0;
            entry->globalModelId = loaders[i].globalModelId;
            entry->textureFaceId = loaders[i].faceId;
            entry->scale = 0x1000;
            partsOffset = loaders[i].boneCount * 4;
            entry->partsOffset = partsOffset;
            entry->animationOffset = partsOffset + (loaders[i].partCount << 5);
            entry->modelData = buffer;
            entry->partMatrices = NULL;
            buffer += loaders[i].boneCount * 4 + (loaders[i].partCount << 5) + loaders[i].animationCount * 0x10;
        }
    }
    D_800E0204 = 0;
    return buffer;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelLoadGlobalModels);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelLoadBcx);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelPrepareRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelAddToRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelAnimCalcMtrxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelScaleModel);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelScalePartVrtxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelScaleAnimTranslat);
