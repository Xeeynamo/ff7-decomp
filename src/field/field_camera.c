//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern u32 D_800DA5C8[];
extern u8 D_800DF08C[];
extern u8 D_800DF0D4[];
extern s32 D_80075E10;
extern s32 D_800DFCA0;
extern s32 D_800E0204;

s32 FieldModelStructInit(FieldModelLoaderHeader* header, FieldModelData* modelData);
s32 FieldModelLoadGlobalModels(FieldModelLoaderHeader* header, FieldModelData* modelData, s32 arg2, s32 arg3);
s32 LoadLocalFieldModelAndInitAll(
    FieldModelLoaderHeader* header, FieldModelData* modelData, const u32* arg2, u32* arg3);
void KawaiLoadEyesMouthTexToVram(FieldModelEntry* entry, u8* scratch);
void KawaiClearData(void);

const u32 D_800A00DC[] = {0x00000000};

void FieldModelLoadAndInit(void) {
    FieldModelEntry** entries;
    FieldModelEntry* entry;
    s32 result;
    s32 offset;
    u32 i;

    D_800DFCA0 = 0x80128000;
    result = FieldModelStructInit(D_8007E770, g_FieldModelData);
    D_80075E10 = result;
    D_800E0204 = result;
    SysCdromStartLoadLzs(
        D_800DA5C8[g_CurrentFieldIndex * 6 + 0], D_800DA5C8[g_CurrentFieldIndex * 6 + 1], (u_long*)0x801B0000, NULL);
    while (SystemCdromReadChain()) {
    }

    *getScratchAddr(0) = (u_long)D_800DF08C;
    *getScratchAddr(1) = (u_long)D_800DF0D4;
    D_80075E10 = FieldModelLoadGlobalModels(D_8007E770, g_FieldModelData, D_80075E10, 1);
    D_80075E10 = LoadLocalFieldModelAndInitAll(D_8007E770, g_FieldModelData, D_800A00DC, (u32*)0x801B0000);

    for (i = 1; i < g_FieldModelData->modelCount; i++) {
        g_FieldModelData->modelEntries[i].flags = 0;
    }

    for (i = 0; i < g_FieldModelData->modelCount; i++) {
        *(u8*)0x1F800001 = 1;
        *(u8*)0x1F800002 = 0;
        *(u8*)0x1F800003 = i;
        entries = &g_FieldModelData->modelEntries;
        entry = *entries + i;
        *(u8*)0x1F800000 = 1;
        KawaiLoadEyesMouthTexToVram(entry, (u8*)0x1F800000);
    }
    KawaiClearData();
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_camera", HandleKawaiDataInModel);

// Possable Debug routine. Ran at beginning of every main field loop. (FPS?)
void DebugRunEveryLoop(void) {}

void FieldCameraAssign(void) {
    if (D_80114488 == 0 || D_8009AC2D == 1) {
        D_80071E40 = *D_80083578;
    } else {
        D_80071E40 = D_80083270;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_camera", FieldUpdateMovieStream);
