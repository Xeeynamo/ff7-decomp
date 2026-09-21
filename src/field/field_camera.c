//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

const u32 D_800A00DC[] = {0x00000000};

extern u8 D_800DF114[4];
extern struct FieldRenderData* D_800DF118;
extern u8 D_801144D8;
extern s16 D_801142C8;
extern volatile s16 g_FieldMoviePlayed;
extern u8 g_RandomTable[256];

void FieldModelPrepareRender(FieldModelEntry* model);
s32 KawaiExecute(FieldModelEntry* model, u8* params, s32 modelId, MATRIX* matrix);
void FieldModelAnimCalcMtrxs(FieldModelEntry* model, MATRIX* matrix, s32 animationId, s32 frame);

void FieldModelLoadAndInit(void) {
    FieldModelEntry** entries;
    FieldModelEntry* entry;
    u8* result;
    s32 offset;
    u32 i;

    g_FieldFaceTextureData = (FieldFaceTextureHeader*)0x80128000;
    result = FieldModelStructInit(D_8007E770, g_FieldModelData);
    D_80075E10 = result;
    D_800E0204 = result;
    SysCdromStartLoadLzs(g_FieldFileInfo[g_CurrentFieldIndex * 6 + 4], g_FieldFileInfo[g_CurrentFieldIndex * 6 + 5],
                         (u_long*)0x801B0000, NULL);
    while (SystemCdromReadChain()) {
    }

    *(Yamada**)getScratchAddr(0) = g_FieldGlobalModelFiles;
    *(Yamada**)getScratchAddr(1) = &g_FieldFaceTextureFile;
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

void HandleKawaiDataInModel(struct FieldRenderData* buf) {
    SVECTOR pos;
    SVECTOR screenPos;
    MATRIX identity;
    FieldModelLoaderData* models;
    s32 i;
    s32 blinkRandomIndex;
    s8 matrixKawaiType;
    s8 kawaiType;
    u8* faceParams = (u8*)getScratchAddr(0);
    u32* matrixSrc;
    u32* matrixDst;

    D_800DF114[0] = D_80075DEC;
    D_800DF118 = buf;
    models = (FieldModelLoaderData*)(D_8007E770 + 1);

    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (models[i].modelEntryIndex != 0xFF) {
            pos.vx = (g_FieldEntity[i].PosX >> 12) + g_FieldEntity[i].OffsetX;
            pos.vy = (g_FieldEntity[i].PosY >> 12) + g_FieldEntity[i].OffsetY;
            pos.vz = (g_FieldEntity[i].PosZ >> 12) + g_FieldEntity[i].OffsetZ - 10;
            g_FieldModelData->modelEntries[models[i].modelEntryIndex].translationX = pos.vx;
            g_FieldModelData->modelEntries[models[i].modelEntryIndex].translationY = pos.vy;
            g_FieldModelData->modelEntries[models[i].modelEntryIndex].translationZ = pos.vz;
            if (FieldCalcWorldToScreenPos(&pos, &screenPos) < 3840) {
                g_FieldModelData->modelEntries[models[i].modelEntryIndex].rotationZ = g_FieldEntity[i].Dir;
                matrixKawaiType = g_FieldModelData->modelEntries[models[i].modelEntryIndex].kawaiType;
                if (matrixKawaiType == 4 || matrixKawaiType == 8 || matrixKawaiType == 9 || matrixKawaiType == 11 ||
                    matrixKawaiType == 12) {
                    identity.m[0][0] = identity.m[1][1] = identity.m[2][2] = 4096;
                    identity.m[0][1] = identity.m[0][2] = identity.m[1][0] = identity.m[1][2] = identity.m[2][0] =
                        identity.m[2][1] = identity.t[0] = identity.t[1] = identity.t[2] = 0;
                    *getScratchAddr(0) = 3;
                    FieldModelAnimCalcMtrxs(&g_FieldModelData->modelEntries[models[i].modelEntryIndex], &identity,
                                            g_FieldEntity[i].activeAnimId, g_FieldEntity[i].animCurrentFrame >> 4);
                    matrixDst = (u32*)g_FieldModelData->modelEntries[models[i].modelEntryIndex].partMatrices;
                    matrixSrc = (u32*)D_80071E40;
                    matrixDst[0] = matrixSrc[0];
                    matrixDst[1] = matrixSrc[1];
                    matrixDst[2] = matrixSrc[2];
                    matrixDst[3] = matrixSrc[3];
                    matrixDst[4] = matrixSrc[4];
                    matrixDst[5] = matrixSrc[5];
                    matrixDst[6] = matrixSrc[6];
                    matrixDst[7] = matrixSrc[7];
                } else {
                    *getScratchAddr(0) = 3;
                    FieldModelAnimCalcMtrxs(&g_FieldModelData->modelEntries[models[i].modelEntryIndex], D_80071E40,
                                            g_FieldEntity[i].activeAnimId, g_FieldEntity[i].animCurrentFrame >> 4);
                }
            }
        }
    }

    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (models[i].modelEntryIndex != 0xFF) {
            pos.vx = g_FieldEntity[i].PosX >> 12;
            pos.vy = g_FieldEntity[i].PosY >> 12;
            pos.vz = (g_FieldEntity[i].PosZ >> 12) - 10;
            if (FieldCalcWorldToScreenPos(&pos, &screenPos) < 3840) {
                kawaiType = g_FieldModelData->modelEntries[models[i].modelEntryIndex].kawaiType;
                if (kawaiType == 4 || kawaiType == 8 || kawaiType == 9 || kawaiType == 11 || kawaiType == 12) {
                    FieldModelPrepareRender(&g_FieldModelData->modelEntries[models[i].modelEntryIndex]);
                } else {
                    g_FieldModelData->modelEntries[models[i].modelEntryIndex].kawaiType = -1;
                    FieldModelPrepareRender(&g_FieldModelData->modelEntries[models[i].modelEntryIndex]);
                    g_FieldModelData->modelEntries[models[i].modelEntryIndex].kawaiType = kawaiType;
                }
            }
        }
    }

    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (models[i].modelEntryIndex != 0xFF && g_FieldEntity[i].KawaiOp1 == 1) {
            pos.vx = g_FieldEntity[i].PosX >> 12;
            pos.vy = g_FieldEntity[i].PosY >> 12;
            pos.vz = (g_FieldEntity[i].PosZ >> 12) - 10;
            if (FieldCalcWorldToScreenPos(&pos, &screenPos) < 3840) {
                if (KawaiExecute(&g_FieldModelData->modelEntries[models[i].modelEntryIndex],
                                 g_FieldEntity[i].KawaiDataOffset, models[i].modelEntryIndex, D_80071E40) == 1) {
                    g_FieldEntity[i].KawaiOp1 = 2;
                }
            }
        }
    }

    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (models[i].modelEntryIndex != 0xFF && g_FieldEntity[i].BlinkOn != 1) {
            if (!g_FieldEntity[i].KawaiA) {
                faceParams[0] = 2;
                blinkRandomIndex = D_801144D8++;
                faceParams[1] = 2;
                faceParams[2] = 0;
                faceParams[3] = i;
                g_FieldEntity[i].KawaiA = (g_RandomTable[blinkRandomIndex] & 0x1F) + 64;
            } else {
                faceParams[0] = 1;
                faceParams[1] = 1;
                faceParams[2] = 0;
                faceParams[3] = i;
                g_FieldEntity[i].KawaiA--;
            }
            KawaiLoadEyesMouthTexToVram(&g_FieldModelData->modelEntries[i], faceParams);
        }
    }
}

// Possible Debug routine. Ran at beginning of every main field loop. (FPS?)
void DebugRunEveryLoop(void) {}

void FieldCameraAssign(void) {
    if (g_FieldMovieJustStarted == 0 || g_FieldState.movieCamDisabled == 1) {
        D_80071E40 = *D_80083578;
    } else {
        D_80071E40 = D_80083270;
    }
}

void FieldUpdateMovieStream(void) {
    if (g_IsFieldLoading == 1) {
        if (!SystemCdromReadChain()) {
            g_IsFieldLoading = 2;
        }
        return;
    }
    if (g_FieldState.eventCmd == EVTCMD_UNK14) {
        SysMovieAbortPlay();
        g_FieldMovieJustStarted = 0;
        g_FieldMoviePlayed = 0;
        g_FieldState.movieCommandState = MOVCMD_DONE;
        return;
    }
    switch (SystemCdromReadChain()) {
    case 0:
        if (g_FieldState.eventCmd == EVTCMD_LOAD_MOVIE && g_FieldState.movieCommandState == MOVCMD_IDLE) {
            if (D_80075E10 <= (u8*)0x801AFFFF) {
                SysMoviePlay(D_80075E10, g_FieldState.eventCmdParam);
            } else {
                SysMoviePlay((void*)0x801B0000, g_FieldState.eventCmdParam);
            }
            g_FieldState.movieCommandState = MOVCMD_ACTIVE;
            g_FieldMoviePlayed = 1;
        }
        if (g_FieldMovieJustStarted == 1) {
            D_801142C8 = 1;
            g_FieldMovieJustStarted = 0;
            g_FieldMoviePlayed = 0;
            g_FieldState.movieCommandState = MOVCMD_DONE;
        }
        break;
    case 10:
        if (g_FieldState.eventCmd == EVTCMD_LOAD_MOVIE) {
            g_FieldState.movieCommandState = MOVCMD_DONE;
        }
        if (g_FieldState.eventCmd == EVTCMD_PLAY_MOVIE) {
            g_FieldState.movieCommandState = MOVCMD_ACTIVE;
            func_800354CC();
            g_FieldMovieJustStarted = 1;
        }
        break;
    }
}
