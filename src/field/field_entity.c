//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern s16 g_FieldSinCosTable[256][2];
extern u16 D_80113F28;
extern s16 D_801144CC;
extern u8 D_80071C0C;
extern u32 g_FieldKeyState;

static s16 D_800DEF88[130] = {
    0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,
    8,  9,  9,  9,  10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17,
    17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26,
    27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
};

u8 FieldEntityDirByVec(VECTOR* start, VECTOR* target, s32* distance);
static void FieldEntityVectorSub(VECTOR* res, SVECTOR* v1, SVECTOR* v2);
s32 FieldEntityCalculateZ(VECTOR* edge1, VECTOR* edge2, VECTOR* point, SVECTOR* anchor);
s32 FieldEntityCollisionCheck(s16 modelId, VECTOR* pos);
s32 FieldEntityLineCheck(FieldEntity* entity, FieldLine* lines, VECTOR* pos);
static void FieldEntityGatewayCheck(FieldEntity* entity, FieldGateway* gateways, VECTOR* pos);
static void FieldEntityTriggerCheck(FieldEntity* entity, FieldBgTrigger* triggers, VECTOR* pos);
s32 FieldCalcLinearStep(s32 start, s32 target, s32 duration, s32 step);
s32 FieldCalcEaseInOut(s32 start, s32 target, s32 duration, s32 step);
s32 FieldEntityMove(s16 modelId);
s32 FieldEntityAutoMove(FieldEntity* data, s16 targetRadius);
void FieldBattleCheck(void);

void FieldEntityInitPos(void) {
    VECTOR edge1;
    VECTOR edge2;
    VECTOR point;
    s16 i;

    if (!g_FieldState.characterLock) {
        g_PlayerModelId = g_FieldState.pcModelId;
        g_FieldEntity[g_PlayerModelId].PosI = g_FieldState.pcPosI;
        if (g_FieldState.pcPosX == 0x7FFF) {
            g_FieldEntity[g_PlayerModelId].PosX =
                ((g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3].vx +
                  g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 1].vx +
                  g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 2].vx) /
                 3)
                << 12;
            g_FieldEntity[g_PlayerModelId].PosY =
                ((g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3].vy +
                  g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 1].vy +
                  g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 2].vy) /
                 3)
                << 12;
            g_FieldEntity[g_PlayerModelId].PosZ =
                ((g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3].vz +
                  g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 1].vz +
                  g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 2].vz) /
                 3)
                << 12;
        } else {
            g_FieldEntity[g_PlayerModelId].PosX = g_FieldState.pcPosX << 12;
            g_FieldEntity[g_PlayerModelId].PosY = g_FieldState.pcPosY << 12;
            FieldEntityVectorSub(&edge1, &g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 1],
                                 &g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3]);
            FieldEntityVectorSub(&edge2, &g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 2],
                                 &g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3 + 1]);
            point.vx = g_FieldState.pcPosX;
            point.vy = g_FieldState.pcPosY;
            g_FieldEntity[g_PlayerModelId].PosZ =
                FieldEntityCalculateZ(
                    &edge1, &edge2, &point, &g_WalkmeshTriangles[g_FieldEntity[g_PlayerModelId].PosI * 3])
                << 12;
        }
        g_FieldEntity[g_PlayerModelId].SolidRange = (34 * g_FieldState.currentFieldScale) >> 9;
        g_FieldEntity[g_PlayerModelId].MoveSpeed = g_FieldState.currentFieldScale << 10 >> 9;
        g_FieldEntity[g_PlayerModelId].animSpeed = 16;
    }
    for (i = 0; i < g_FieldState.modelCount; i++) {
        g_FieldEntity[i].MoveDirAdd = 0;
    }
}

void FieldEntityAddRotate(u32 activeKeys, s16 modelId) {
    if (!g_FieldState.characterLock) {
        if (g_FieldState.activeKeys & PAD_R1) {
            g_FieldEntity[modelId].MoveDirAdd = 224;
        } else if (g_FieldState.activeKeys & PAD_L1) {
            g_FieldEntity[modelId].MoveDirAdd = 32;
        } else {
            g_FieldEntity[modelId].MoveDirAdd = 0;
        }
    }
}

void FieldEntityAnimationUpdate(s32 modelId) {
    u8 modelEntryId;
    FieldModel* model;
    FieldModelAnimation* anims;

    modelEntryId = g_FieldModelLoaderData[modelId].modelEntryIndex;
    if (modelEntryId != 0xFF) {
        model = &g_FieldModelData->models[modelEntryId];
        anims = (FieldModelAnimation*)(model->modelData + model->animationOffset);
        if (!g_FieldState.suspendWalkAndAnim) {
            g_FieldEntity[modelId].animCurrentFrame += g_FieldEntity[modelId].animSpeed;
            if (modelId == g_PlayerModelId && !g_FieldState.characterLock) {
                g_FieldEntity[modelId].animLastFrame = anims[g_FieldEntity[modelId].activeAnimId].frameCount - 1;
                if (g_FieldEntity[modelId].animCurrentFrame > (g_FieldEntity[modelId].animLastFrame << 4)) {
                    g_FieldEntity[modelId].animCurrentFrame = 0;
                }
            } else {
                if (g_FieldEntity[modelId].animCurrentFrame > (g_FieldEntity[modelId].animLastFrame << 4)) {
                    g_FieldEntity[modelId].animCurrentFrame = g_FieldEntity[modelId].animLastFrame << 4;
                }
            }
        }
    }
}

void FieldEntityMovementUpdate(u32 activeKeys) {
    s16 i;
    VECTOR edge1, edge2, point;
    s32 ok;
    s32 distance;

    for (i = 0; i < g_FieldState.modelCount; i++) {
        u8 modelEntryId = g_FieldModelLoaderData[i].modelEntryIndex;
        if (modelEntryId != 0xFF) {
            FieldModel* model = &g_FieldModelData->models[modelEntryId];
            if (g_FieldEntity[i].visible == 1) {
                model->flags = 1;
            } else {
                model->flags = 0;
            }
        }
    }
    for (i = 0; i < g_FieldState.modelCount; i++) {
        switch (g_FieldEntity[i].TurnType) {
        case 0:
            break;
        case 1:
            g_FieldEntity[i].Dir = FieldCalcLinearStep(g_FieldEntity[i].TurnStart, g_FieldEntity[i].TurnEnd,
                                                       g_FieldEntity[i].TurnSteps, g_FieldEntity[i].TurnStep);
            if (g_FieldEntity[i].TurnStep == g_FieldEntity[i].TurnSteps) {
                g_FieldEntity[i].TurnType = 3;
            } else {
                g_FieldEntity[i].TurnStep++;
            }
            break;
        case 2:
            g_FieldEntity[i].Dir = FieldCalcEaseInOut(g_FieldEntity[i].TurnStart, g_FieldEntity[i].TurnEnd,
                                                      g_FieldEntity[i].TurnSteps, g_FieldEntity[i].TurnStep);
            if (g_FieldEntity[i].TurnStep == g_FieldEntity[i].TurnSteps) {
                g_FieldEntity[i].TurnType = 3;
            } else {
                g_FieldEntity[i].TurnStep++;
            }
            break;
        }
    }
    for (i = 0; i < g_FieldState.modelCount; i++) {
        switch (g_FieldEntity[i].OfsType) {
        case OMODE_INSTANT:
            break;
        case OMODE_LINEAR:
            g_FieldEntity[i].OffsetX = FieldCalcLinearStep(g_FieldEntity[i].OffsetStartX, g_FieldEntity[i].OffsetEndX,
                                                           g_FieldEntity[i].OffsetSteps, g_FieldEntity[i].OffsetStep);
            g_FieldEntity[i].OffsetY = FieldCalcLinearStep(g_FieldEntity[i].OffsetStartY, g_FieldEntity[i].OffsetEndY,
                                                           g_FieldEntity[i].OffsetSteps, g_FieldEntity[i].OffsetStep);
            g_FieldEntity[i].OffsetZ = FieldCalcLinearStep(g_FieldEntity[i].OffsetStartZ, g_FieldEntity[i].OffsetEndZ,
                                                           g_FieldEntity[i].OffsetSteps, g_FieldEntity[i].OffsetStep);
            if (g_FieldEntity[i].OffsetStep == g_FieldEntity[i].OffsetSteps) {
                g_FieldEntity[i].OfsType = OMODE_DONE;
            } else {
                g_FieldEntity[i].OffsetStep++;
            }
            if (i == g_PlayerModelId) {
                FieldEntityLineClear(g_FieldLines);
            }
            break;
        case OMODE_SMOOTH:
            g_FieldEntity[i].OffsetX = FieldCalcEaseInOut(g_FieldEntity[i].OffsetStartX, g_FieldEntity[i].OffsetEndX,
                                                          g_FieldEntity[i].OffsetSteps, g_FieldEntity[i].OffsetStep);
            g_FieldEntity[i].OffsetY = FieldCalcEaseInOut(g_FieldEntity[i].OffsetStartY, g_FieldEntity[i].OffsetEndY,
                                                          g_FieldEntity[i].OffsetSteps, g_FieldEntity[i].OffsetStep);
            g_FieldEntity[i].OffsetZ = FieldCalcEaseInOut(g_FieldEntity[i].OffsetStartZ, g_FieldEntity[i].OffsetEndZ,
                                                          g_FieldEntity[i].OffsetSteps, g_FieldEntity[i].OffsetStep);
            if (g_FieldEntity[i].OffsetStep == g_FieldEntity[i].OffsetSteps) {
                g_FieldEntity[i].OfsType = OMODE_DONE;
            } else {
                g_FieldEntity[i].OffsetStep++;
            }
            if (i == g_PlayerModelId) {
                FieldEntityLineClear(g_FieldLines);
            }
            break;
        }
    }
    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (g_FieldEntity[i].scriptedMoveMode == SMODE_NONE) {
            if (i == g_PlayerModelId) {
                if (g_FieldState.characterLock == 1) {
                    FieldEntityAnimationUpdate(i);
                    continue;
                }
                FieldEntityAddRotate(activeKeys, i);
                g_FieldEntity[g_PlayerModelId].activeAnimId = g_FieldState.idleAnimId;
                if (activeKeys & PAD_CROSS) {
                    if (!g_FieldState.backgroundMovieEnabled) {
                        g_FieldEntity[g_PlayerModelId].MoveSpeed = g_FieldState.currentFieldScale << 12 >> 9;
                    } else {
                        g_FieldEntity[g_PlayerModelId].MoveSpeed = (6144 * g_FieldState.currentFieldScale) >> 9;
                    }
                } else if (!g_FieldState.backgroundMovieEnabled) {
                    g_FieldEntity[g_PlayerModelId].MoveSpeed = g_FieldState.currentFieldScale << 10 >> 9;
                } else {
                    g_FieldEntity[g_PlayerModelId].MoveSpeed = (3072 * g_FieldState.currentFieldScale) >> 10;
                }
                if (activeKeys & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)) {
                    if (activeKeys & PAD_UP) {
                        g_FieldEntity[i].MoveDir = 0;
                        if (activeKeys & PAD_LEFT) {
                            g_FieldEntity[i].MoveDir = 32;
                        }
                        if (activeKeys & PAD_RIGHT) {
                            g_FieldEntity[i].MoveDir = 224;
                        }
                    } else if (activeKeys & PAD_DOWN) {
                        g_FieldEntity[i].MoveDir = 128;
                        if (activeKeys & PAD_LEFT) {
                            g_FieldEntity[i].MoveDir = 96;
                        }
                        if (activeKeys & PAD_RIGHT) {
                            g_FieldEntity[i].MoveDir = 160;
                        }
                    } else {
                        if (activeKeys & PAD_RIGHT) {
                            g_FieldEntity[i].MoveDir = 192;
                        }
                        if (activeKeys & PAD_LEFT) {
                            g_FieldEntity[i].MoveDir = 64;
                        }
                    }
                    g_FieldEntity[i].MoveDir += g_FieldTriggers->controlDirection + g_FieldEntity[i].MoveDirAdd;
                    ok = FieldEntityMove(i);
                    if (!g_FieldEntity[i].DirLock) {
                        g_FieldEntity[i].Dir = g_FieldEntity[i].MoveDir;
                    }
                    if (g_FieldState.eventCmd != EVTCMD_FIELD_MAP_CHANGE && ok == 1) {
                        FieldBattleCheck();
                    }
                }
            }
            FieldEntityAnimationUpdate(i);
        }
    }
    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (g_FieldEntity[i].scriptedMoveMode == SMODE_WALK && g_FieldState.suspendWalkAndAnim != 1) {
            g_FieldEntity[i].MoveDirAdd = 0;

            if (!FieldEntityAutoMove(&g_FieldEntity[i], g_FieldEntity[i].ActionArg)) {
                g_FieldEntity[i].ActionState = 2;
            } else {
                g_FieldEntity[i].ActionState = 1;
                FieldEntityMove(i);
                if (!g_FieldEntity[i].DirLock) {
                    g_FieldEntity[i].Dir = g_FieldEntity[i].MoveDir;
                }
            }
            FieldEntityAnimationUpdate(i);
            if (i == g_PlayerModelId) {
                FieldEntityLineClear(g_FieldLines);
            }
        }
    }
    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (g_FieldEntity[i].scriptedMoveMode == SMODE_JUMP) {
            if (!g_FieldEntity[i].ActionState) {
                g_FieldEntity[i].MoveDirAdd = 0;
                g_FieldEntity[i].MoveStartX = g_FieldEntity[i].PosX;
                g_FieldEntity[i].MoveStartY = g_FieldEntity[i].PosY;
                g_FieldEntity[i].MoveStartZ = g_FieldEntity[i].PosZ;
                FieldEntityVectorSub(&edge1, &g_WalkmeshTriangles[g_FieldEntity[i].MoveEndI * 3 + 1],
                                     &g_WalkmeshTriangles[g_FieldEntity[i].MoveEndI * 3]);
                FieldEntityVectorSub(&edge2, &g_WalkmeshTriangles[g_FieldEntity[i].MoveEndI * 3 + 2],
                                     &g_WalkmeshTriangles[g_FieldEntity[i].MoveEndI * 3 + 1]);
                point.vx = g_FieldEntity[i].MoveEndX / 4096;
                point.vy = g_FieldEntity[i].MoveEndY / 4096;
                g_FieldEntity[i].MoveEndZ =
                    FieldEntityCalculateZ(&edge1, &edge2, &point, &g_WalkmeshTriangles[g_FieldEntity[i].MoveEndI * 3])
                    << 12;
                g_FieldEntity[i].MoveB =
                    (g_FieldEntity[i].MoveEndZ - g_FieldEntity[i].MoveStartZ) / g_FieldEntity[i].MoveSteps -
                    -16000 * g_FieldEntity[i].MoveSteps / 2;
                g_FieldEntity[i].MoveStep = 0;
                g_FieldEntity[i].ActionState = 1;
            } else {
                if (g_FieldEntity[i].MoveSteps == g_FieldEntity[i].MoveStep) {
                    g_FieldEntity[i].PosI = g_FieldEntity[i].MoveEndI;
                    g_FieldEntity[i].ActionState = 2;
                } else {
                    g_FieldEntity[i].MoveStep++;
                    g_FieldEntity[i].PosX = FieldCalcLinearStep(g_FieldEntity[i].MoveStartX, g_FieldEntity[i].MoveEndX,
                                                                g_FieldEntity[i].MoveSteps, g_FieldEntity[i].MoveStep);
                    g_FieldEntity[i].PosY = FieldCalcLinearStep(g_FieldEntity[i].MoveStartY, g_FieldEntity[i].MoveEndY,
                                                                g_FieldEntity[i].MoveSteps, g_FieldEntity[i].MoveStep);
                    g_FieldEntity[i].PosZ = g_FieldEntity[i].MoveStep * g_FieldEntity[i].MoveB +
                                            g_FieldEntity[i].MoveStep * -16000 * g_FieldEntity[i].MoveStep / 2 +
                                            g_FieldEntity[i].MoveStartZ;
                }
            }
            FieldEntityAnimationUpdate(i);
            if (i == g_PlayerModelId) {
                FieldEntityLineClear(g_FieldLines);
            }
        }
    }
    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (g_FieldEntity[i].scriptedMoveMode == SMODE_LADDER_V) {
            if (g_FieldModelLoaderData[i].modelEntryIndex != 0xFF) {
                FieldModel* model = &g_FieldModelData->models[g_FieldModelLoaderData[i].modelEntryIndex];
                FieldModelAnimation* anims = (FieldModelAnimation*)(model->modelData + model->animationOffset);

                if (!g_FieldEntity[i].ActionState) {
                    g_FieldEntity[i].MoveDirAdd = 0;
                    g_FieldEntity[i].MoveStartX = g_FieldEntity[i].PosX;
                    g_FieldEntity[i].MoveStartY = g_FieldEntity[i].PosY;
                    g_FieldEntity[i].MoveStartZ = g_FieldEntity[i].PosZ;
                    edge1.vx = (g_FieldEntity[i].MoveEndX - g_FieldEntity[i].MoveStartX) / 4096;
                    edge1.vy = (g_FieldEntity[i].MoveEndY - g_FieldEntity[i].MoveStartY) / 4096;
                    edge1.vz = (g_FieldEntity[i].MoveEndZ - g_FieldEntity[i].MoveStartZ) / 4096;
                    distance = SquareRoot0(edge1.vx * edge1.vx + edge1.vy * edge1.vy + edge1.vz * edge1.vz);
                    distance /= 4;
                    g_FieldEntity[i].MoveSteps = distance;
                    g_FieldEntity[i].MoveStep = 0;
                    g_FieldEntity[i].ActionState = 1;
                    g_FieldEntity[i].animLastFrame = anims[g_FieldEntity[i].activeAnimId].frameCount - 1;

                    if (i == g_PlayerModelId) {
                        FieldEntityLineClear(g_FieldLines);
                    }
                } else {
                    if (i == g_PlayerModelId && !g_FieldState.characterLock) {
                        if (!g_FieldEntity[i].ActionArg) {
                            if (activeKeys & PAD_UP) {
                                if (!g_FieldEntity[i].MoveStep) {
                                    g_FieldEntity[i].ActionState = 2;
                                } else {
                                    g_FieldEntity[i].MoveStep--;
                                    g_FieldEntity[i].animCurrentFrame -= g_FieldEntity[i].animSpeed;
                                    if (g_FieldEntity[i].animCurrentFrame < 0) {
                                        g_FieldEntity[i].animCurrentFrame = g_FieldEntity[i].animLastFrame << 4;
                                    }
                                }
                            }
                            if (activeKeys & PAD_DOWN) {
                                if (g_FieldEntity[i].MoveStep == g_FieldEntity[i].MoveSteps) {
                                    g_FieldEntity[i].PosI = g_FieldEntity[i].MoveEndI;
                                    g_FieldEntity[i].ActionState = 2;
                                } else {
                                    g_FieldEntity[i].MoveStep++;
                                    g_FieldEntity[i].animCurrentFrame += g_FieldEntity[i].animSpeed;
                                    if (g_FieldEntity[i].animCurrentFrame > (g_FieldEntity[i].animLastFrame << 4)) {
                                        g_FieldEntity[i].animCurrentFrame = 0;
                                    }
                                }
                            }
                        } else {
                            if (activeKeys & PAD_DOWN) {
                                if (!g_FieldEntity[i].MoveStep) {
                                    g_FieldEntity[i].ActionState = 2;
                                } else {
                                    g_FieldEntity[i].MoveStep--;
                                    g_FieldEntity[i].animCurrentFrame -= g_FieldEntity[i].animSpeed;
                                    if (g_FieldEntity[i].animCurrentFrame < 0) {
                                        g_FieldEntity[i].animCurrentFrame = g_FieldEntity[i].animLastFrame << 4;
                                    }
                                }
                            }
                            if (activeKeys & PAD_UP) {
                                if (g_FieldEntity[i].MoveStep == g_FieldEntity[i].MoveSteps) {
                                    g_FieldEntity[i].PosI = g_FieldEntity[i].MoveEndI;
                                    g_FieldEntity[i].ActionState = 2;
                                } else {
                                    g_FieldEntity[i].MoveStep++;
                                    g_FieldEntity[i].animCurrentFrame += g_FieldEntity[i].animSpeed;
                                    if (g_FieldEntity[i].animCurrentFrame > (g_FieldEntity[i].animLastFrame << 4)) {
                                        g_FieldEntity[i].animCurrentFrame = 0;
                                    }
                                }
                            }
                        }
                    } else {
                        if (g_FieldEntity[i].MoveStep == g_FieldEntity[i].MoveSteps) {
                            g_FieldEntity[i].PosI = g_FieldEntity[i].MoveEndI;
                            g_FieldEntity[i].ActionState = 2;
                        } else {
                            g_FieldEntity[i].MoveStep++;
                            g_FieldEntity[i].animCurrentFrame += g_FieldEntity[i].animSpeed;
                            if (g_FieldEntity[i].animCurrentFrame > (g_FieldEntity[i].animLastFrame << 4)) {
                                g_FieldEntity[i].animCurrentFrame = 0;
                            }
                        }
                    }
                    g_FieldEntity[i].PosX = FieldCalcLinearStep(g_FieldEntity[i].MoveStartX, g_FieldEntity[i].MoveEndX,
                                                                g_FieldEntity[i].MoveSteps, g_FieldEntity[i].MoveStep);
                    g_FieldEntity[i].PosY = FieldCalcLinearStep(g_FieldEntity[i].MoveStartY, g_FieldEntity[i].MoveEndY,
                                                                g_FieldEntity[i].MoveSteps, g_FieldEntity[i].MoveStep);
                    g_FieldEntity[i].PosZ = FieldCalcLinearStep(g_FieldEntity[i].MoveStartZ, g_FieldEntity[i].MoveEndZ,
                                                                g_FieldEntity[i].MoveSteps, g_FieldEntity[i].MoveStep);
                }
            }
        }
    }
    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (g_FieldEntity[i].scriptedMoveMode == SMODE_LADDER_H) {
            if (g_FieldModelLoaderData[i].modelEntryIndex != 0xFF) {
                FieldModel* model = &g_FieldModelData->models[g_FieldModelLoaderData[i].modelEntryIndex];
                FieldModelAnimation* anims = (FieldModelAnimation*)(model->modelData + model->animationOffset);

                if (!g_FieldEntity[i].ActionState) {
                    g_FieldEntity[i].MoveDirAdd = 0;
                    g_FieldEntity[i].MoveStartX = g_FieldEntity[i].PosX;
                    g_FieldEntity[i].MoveStartY = g_FieldEntity[i].PosY;
                    g_FieldEntity[i].MoveStartZ = g_FieldEntity[i].PosZ;
                    edge1.vx = (g_FieldEntity[i].MoveEndX - g_FieldEntity[i].MoveStartX) / 4096;
                    edge1.vy = (g_FieldEntity[i].MoveEndY - g_FieldEntity[i].MoveStartY) / 4096;
                    edge1.vz = (g_FieldEntity[i].MoveEndZ - g_FieldEntity[i].MoveStartZ) / 4096;
                    distance = SquareRoot0(edge1.vx * edge1.vx + edge1.vy * edge1.vy + edge1.vz * edge1.vz);
                    g_FieldEntity[i].MoveSteps = distance / 4;
                    g_FieldEntity[i].MoveStep = 0;
                    g_FieldEntity[i].ActionState = 1;
                    g_FieldEntity[i].animLastFrame = anims[g_FieldEntity[i].activeAnimId].frameCount - 1;

                    if (i == g_PlayerModelId) {
                        FieldEntityLineClear(g_FieldLines);
                    }
                } else {
                    if (i == g_PlayerModelId && !g_FieldState.characterLock) {
                        if (!g_FieldEntity[i].ActionArg) {
                            if (activeKeys & PAD_LEFT) {
                                if (!g_FieldEntity[i].MoveStep) {
                                    g_FieldEntity[i].ActionState = 2;
                                } else {
                                    g_FieldEntity[i].MoveStep--;
                                    g_FieldEntity[i].animCurrentFrame -= g_FieldEntity[i].animSpeed;
                                    if (g_FieldEntity[i].animCurrentFrame < 0) {
                                        g_FieldEntity[i].animCurrentFrame = g_FieldEntity[i].animLastFrame << 4;
                                    }
                                }
                            }
                            if (activeKeys & PAD_RIGHT) {
                                if (g_FieldEntity[i].MoveStep == g_FieldEntity[i].MoveSteps) {
                                    g_FieldEntity[i].PosI = g_FieldEntity[i].MoveEndI;
                                    g_FieldEntity[i].ActionState = 2;
                                } else {
                                    g_FieldEntity[i].MoveStep++;
                                    g_FieldEntity[i].animCurrentFrame += g_FieldEntity[i].animSpeed;
                                    if (g_FieldEntity[i].animCurrentFrame > (g_FieldEntity[i].animLastFrame << 4)) {
                                        g_FieldEntity[i].animCurrentFrame = 0;
                                    }
                                }
                            }
                        } else {
                            if (activeKeys & PAD_RIGHT) {
                                if (!g_FieldEntity[i].MoveStep) {
                                    g_FieldEntity[i].ActionState = 2;
                                } else {
                                    g_FieldEntity[i].MoveStep--;
                                    g_FieldEntity[i].animCurrentFrame -= g_FieldEntity[i].animSpeed;
                                    if (g_FieldEntity[i].animCurrentFrame < 0) {
                                        g_FieldEntity[i].animCurrentFrame = g_FieldEntity[i].animLastFrame << 4;
                                    }
                                }
                            }
                            if (activeKeys & PAD_LEFT) {
                                if (g_FieldEntity[i].MoveStep == g_FieldEntity[i].MoveSteps) {
                                    g_FieldEntity[i].PosI = g_FieldEntity[i].MoveEndI;
                                    g_FieldEntity[i].ActionState = 2;
                                } else {
                                    g_FieldEntity[i].MoveStep++;
                                    g_FieldEntity[i].animCurrentFrame += g_FieldEntity[i].animSpeed;
                                    if (g_FieldEntity[i].animCurrentFrame > (g_FieldEntity[i].animLastFrame << 4)) {
                                        g_FieldEntity[i].animCurrentFrame = 0;
                                    }
                                }
                            }
                        }
                    } else {
                        if (g_FieldEntity[i].MoveStep == g_FieldEntity[i].MoveSteps) {
                            g_FieldEntity[i].PosI = g_FieldEntity[i].MoveEndI;
                            g_FieldEntity[i].ActionState = 2;
                        } else {
                            g_FieldEntity[i].MoveStep++;
                            g_FieldEntity[i].animCurrentFrame += g_FieldEntity[i].animSpeed;
                            if (g_FieldEntity[i].animCurrentFrame > (g_FieldEntity[i].animLastFrame << 4)) {
                                g_FieldEntity[i].animCurrentFrame = 0;
                            }
                        }
                    }
                    g_FieldEntity[i].PosX = FieldCalcLinearStep(g_FieldEntity[i].MoveStartX, g_FieldEntity[i].MoveEndX,
                                                                g_FieldEntity[i].MoveSteps, g_FieldEntity[i].MoveStep);
                    g_FieldEntity[i].PosY = FieldCalcLinearStep(g_FieldEntity[i].MoveStartY, g_FieldEntity[i].MoveEndY,
                                                                g_FieldEntity[i].MoveSteps, g_FieldEntity[i].MoveStep);
                    g_FieldEntity[i].PosZ = FieldCalcLinearStep(g_FieldEntity[i].MoveStartZ, g_FieldEntity[i].MoveEndZ,
                                                                g_FieldEntity[i].MoveSteps, g_FieldEntity[i].MoveStep);
                }
            }
        }
    }
}

void FieldEntityGatewayMapLoad(FieldGateway* gateway) {
    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
    g_FieldState.eventCmdParam = gateway->fieldMapId;
    g_FieldState.pcPosX = gateway->destFieldPos.vx;
    g_FieldState.pcPosY = gateway->destFieldPos.vy;
    g_FieldState.pcPosI = gateway->pcPosI;
    g_FieldState.pcDirection = gateway->pcDirection;
}

void FieldEntityCheckTalk(void) {
    VECTOR start;
    VECTOR target;
    s16 diffs[16];
    s32 distance;
    s32 i;
    s16 modelId;
    s16 bestDiff;

    if ((g_FieldState.activeKeys & PAD_CIRCLE) && !(g_FieldState.activeKeysPrev & PAD_CIRCLE)) {
        start.vx = g_FieldEntity[g_PlayerModelId].PosX >> 12;
        start.vy = g_FieldEntity[g_PlayerModelId].PosY >> 12;
        start.vz = g_FieldEntity[g_PlayerModelId].PosZ >> 12;
        for (i = 0; i < g_FieldState.modelCount; i++) {
            diffs[i] = 256;
            if (i != g_PlayerModelId && !g_FieldEntity[i].TalkOff) {
                target.vx = g_FieldEntity[i].PosX >> 12;
                target.vy = g_FieldEntity[i].PosY >> 12;
                target.vz = g_FieldEntity[i].PosZ >> 12;
                if ((start.vx != target.vx || start.vy != target.vy) && start.vz - target.vz > -256 &&
                    start.vz - target.vz < 256) {
                    diffs[i] =
                        (g_FieldEntity[g_PlayerModelId].Dir - FieldEntityDirByVec(&start, &target, &distance)) & 0xFF;
                    if (diffs[i] > 128) {
                        diffs[i] = 256 - diffs[i];
                    }
                    if (distance >= g_FieldEntity[i].TalkRange + g_FieldEntity[g_PlayerModelId].SolidRange) {
                        diffs[i] = 256;
                    }
                }
            }
        }
        modelId = g_PlayerModelId;
        bestDiff = 64;
        for (i = 0; i < g_FieldState.modelCount; i++) {
            if (diffs[i] < bestDiff) {
                bestDiff = diffs[i];
                modelId = i;
            }
        }
        if (modelId != g_PlayerModelId) {
            s32 limit = 64;
            if (bestDiff != limit) {
                u8 request = 1;
                g_FieldEntity[modelId].requestTalkScript = request;
            }
        }
    }
}

static s16 FieldEntityGetDirVectorX(u8 arg0) { return g_FieldSinCosTable[arg0][0]; }

s16 FieldEntityGetDirVectorY(u8 arg0) { return g_FieldSinCosTable[arg0][1]; }

u8 FieldEntityDirByVec(VECTOR* start, VECTOR* target, s32* distance) {
    u8 angle;
    s32 dy;
    s32 dx;

    dx = target->vx - start->vx;
    dy = target->vy - start->vy;
    *distance = dx * dx + dy * dy;
    *distance = SquareRoot0(*distance);
    dx = (dx << 12) / *distance / 32;
    dy = (dy << 12) / *distance / 32;
    if (dx * dx > dy * dy) {
        if (dx > 0) {
            if (dy > 0) {
                angle = D_800DEF88[dy];
            } else {
                angle = 256 - D_800DEF88[-dy];
            }
        } else {
            if (dy > 0) {
                angle = 128 - D_800DEF88[dy];
            } else {
                angle = D_800DEF88[-dy] + 128;
            }
        }
    } else {
        if (dy > 0) {
            if (dx > 0) {
                angle = 64 - D_800DEF88[dx];
            } else {
                angle = 128 - (64 - D_800DEF88[-dx]);
            }
        } else {
            if (dx > 0) {
                angle = 256 - (64 - D_800DEF88[dx]);
            } else {
                angle = 64 - D_800DEF88[-dx] + 128;
            }
        }
    }
    return angle - 192;
}

s32 FieldEntityAutoMove(FieldEntity* data, s16 targetRadius) {
    VECTOR start;
    VECTOR target;
    s32 distanceSq;
    s32 stepDistanceSq;
    s32 dx;
    s32 dy;
    s32 thresholdSq;

    start.vx = data->PosX >> 12;
    start.vy = data->PosY >> 12;
    target.vx = data->MoveEndX >> 12;
    target.vy = data->MoveEndY >> 12;
    dx = target.vx - start.vx;
    dy = target.vy - start.vy;
    distanceSq = dx * dx + dy * dy;
    thresholdSq = (data->SolidRange + targetRadius) * (data->SolidRange + targetRadius) + 4096;
    if (targetRadius && distanceSq <= thresholdSq) {
        return 0;
    }
    stepDistanceSq = (data->MoveSpeed * data->MoveSpeed) >> 16;
    if (distanceSq < stepDistanceSq || distanceSq < 4) {
        data->PosX = data->MoveEndX;
        data->PosY = data->MoveEndY;
        return 0;
    }
    data->MoveDir = FieldEntityDirByVec(&start, &target, &distanceSq) - data->MoveDirAdd;
    return 1;
}

s32 FieldEntityWalkmeshCross(u16* triangleId, VECTOR* testPoint, VECTOR* testProbe, VECTOR* blockedEdge) {
    VECTOR* vectors = (VECTOR*)getScratchAddr(0);
    s16 access;
    s32 cross1, cross2, cross3;
    s32 result = 0;

    vectors[3].vx = testPoint->vx / 4096;
    vectors[3].vy = testPoint->vy / 4096;
    vectors[3].vz = 0;
    D_80113F28 = 0xFFFF;

    while (1) {
        FieldEntityVectorSub(
            &vectors[0], &g_WalkmeshTriangles[*triangleId * 3 + 1], &g_WalkmeshTriangles[*triangleId * 3]);
        FieldEntityVectorSub(
            &vectors[1], &g_WalkmeshTriangles[*triangleId * 3 + 2], &g_WalkmeshTriangles[*triangleId * 3 + 1]);
        FieldEntityVectorSub(
            &vectors[2], &g_WalkmeshTriangles[*triangleId * 3], &g_WalkmeshTriangles[*triangleId * 3 + 2]);

        cross1 = (vectors[3].vx - g_WalkmeshTriangles[*triangleId * 3].vx) * vectors[0].vy -
                 (vectors[3].vy - g_WalkmeshTriangles[*triangleId * 3].vy) * vectors[0].vx;
        cross2 = (vectors[3].vx - g_WalkmeshTriangles[*triangleId * 3 + 1].vx) * vectors[1].vy -
                 (vectors[3].vy - g_WalkmeshTriangles[*triangleId * 3 + 1].vy) * vectors[1].vx;
        cross3 = (vectors[3].vx - g_WalkmeshTriangles[*triangleId * 3 + 2].vx) * vectors[2].vy -
                 (vectors[3].vy - g_WalkmeshTriangles[*triangleId * 3 + 2].vy) * vectors[2].vx;

        if (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) {
            break;
        }
        if (cross1 < 0) {
            access = g_WalkmeshAccesses[*triangleId].access1;
            if (access >= 0 && !((g_FieldState.blockedAccesses[access >> 3] >> (access % 8)) & 1)) {
                *triangleId = access;
            } else {
                blockedEdge->vx = vectors[0].vx;
                blockedEdge->vy = vectors[0].vy;
                blockedEdge->vz = vectors[0].vz;
                if (vectors[0].vx * testProbe->vx + vectors[0].vy * testProbe->vy >= 0) {
                    result = 8;
                } else {
                    result = -8;
                }
                D_80113F28 = *triangleId;
                D_801144CC = 0;
                break;
            }
        } else if (cross2 < 0) {
            access = g_WalkmeshAccesses[*triangleId].access2;
            if (access >= 0 && !((g_FieldState.blockedAccesses[access >> 3] >> (access % 8)) & 1)) {
                *triangleId = access;
            } else {
                blockedEdge->vx = vectors[1].vx;
                blockedEdge->vy = vectors[1].vy;
                blockedEdge->vz = vectors[1].vz;
                if (vectors[1].vx * testProbe->vx + vectors[1].vy * testProbe->vy >= 0) {
                    result = 8;
                } else {
                    result = -8;
                }
                D_80113F28 = *triangleId;
                D_801144CC = 1;
                break;
            }
        } else if (cross3 < 0) {
            access = g_WalkmeshAccesses[*triangleId].access3;
            if (access >= 0 && !((g_FieldState.blockedAccesses[access >> 3] >> (access % 8)) & 1)) {
                *triangleId = access;
            } else {
                blockedEdge->vx = vectors[2].vx;
                blockedEdge->vy = vectors[2].vy;
                blockedEdge->vz = vectors[2].vz;
                if (vectors[2].vx * testProbe->vx + vectors[2].vy * testProbe->vy >= 0) {
                    result = 8;
                } else {
                    result = -8;
                }
                D_80113F28 = *triangleId;
                D_801144CC = 2;
                break;
            }
        }
    }
    testPoint->vz = FieldEntityCalculateZ(&vectors[0], &vectors[1], &vectors[3], &g_WalkmeshTriangles[*triangleId * 3]);
    return result;
}

static void FieldEntityVectorSub(VECTOR* res, SVECTOR* v1, SVECTOR* v2) {
    res->vx = v1->vx - v2->vx;
    res->vy = v1->vy - v2->vy;
    res->vz = v1->vz - v2->vz;
}

s32 FieldEntityCalculateZ(VECTOR* edge1, VECTOR* edge2, VECTOR* point, SVECTOR* anchor) {
    VECTOR normal;

    normal.vx = -edge1->vy * edge2->vz + edge2->vy * edge1->vz;
    normal.vy = -edge1->vz * edge2->vx + edge1->vx * edge2->vz;
    normal.vz = -edge1->vx * edge2->vy + edge2->vx * edge1->vy;
    edge1->vx = anchor->vx;
    edge1->vy = anchor->vy;
    edge1->vz = anchor->vz;

    return (normal.vx * edge1->vx + normal.vy * edge1->vy + normal.vz * edge1->vz - normal.vx * point->vx -
            normal.vy * point->vy) /
           normal.vz;
}

s32 FieldEntityMove(s16 modelId) {
    VECTOR* vectors = (VECTOR*)getScratchAddr(sizeof(VECTOR) / 4 * 4);
    u16 triangleId;
    s32 leftCorrection, rightCorrection, headCorrection;
    s32 headContact, leftContact, rightContact;
    s32 finalCorrection;
    s32 absNx, absNy;
    u32 i;

    triangleId = g_FieldEntity[modelId].PosI;
    vectors[0].vx = g_WalkmeshTriangles[triangleId * 3 + 1].vx - g_WalkmeshTriangles[triangleId * 3].vx;
    vectors[0].vy = g_WalkmeshTriangles[triangleId * 3 + 1].vy - g_WalkmeshTriangles[triangleId * 3].vy;
    vectors[0].vz = g_WalkmeshTriangles[triangleId * 3 + 1].vz - g_WalkmeshTriangles[triangleId * 3].vz;
    vectors[1].vx = g_WalkmeshTriangles[triangleId * 3 + 2].vx - g_WalkmeshTriangles[triangleId * 3 + 1].vx;
    vectors[1].vy = g_WalkmeshTriangles[triangleId * 3 + 2].vy - g_WalkmeshTriangles[triangleId * 3 + 1].vy;
    vectors[1].vz = g_WalkmeshTriangles[triangleId * 3 + 2].vz - g_WalkmeshTriangles[triangleId * 3 + 1].vz;
    OuterProduct0(&vectors[0], &vectors[1], &vectors[2]);
    vectors[2].vx /= 256;
    vectors[2].vy /= 256;
    vectors[2].vz /= 256;
    VectorNormal(&vectors[2], &vectors[2]);
    vectors[2].vx = (vectors[2].vz << 12) /
                    SquareRoot12(vectors[2].vx * vectors[2].vx / 4096 + vectors[2].vz * vectors[2].vz / 4096);
    vectors[2].vy = (vectors[2].vz << 12) /
                    SquareRoot12(vectors[2].vy * vectors[2].vy / 4096 + vectors[2].vz * vectors[2].vz / 4096);
    if (vectors[2].vx > 4096) {
        vectors[2].vx = 4096;
    }
    if (vectors[2].vx < -4096) {
        vectors[2].vx = -4096;
    }
    if (vectors[2].vy > 4096) {
        vectors[2].vy = 4096;
    }
    if (vectors[2].vy < -4096) {
        vectors[2].vy = -4096;
    }
    if (vectors[2].vz > 4096) {
        vectors[2].vz = 4096;
    }
    if (vectors[2].vz < -4096) {
        vectors[2].vz = -4096;
    }
    absNx = vectors[2].vx;
    if (vectors[2].vx < 0) {
        absNx = -vectors[2].vx;
    }
    absNy = vectors[2].vy;
    if (vectors[2].vy < 0) {
        absNy = -vectors[2].vy;
    }
    i = 0;
    while (1) {
        i++;
        if (modelId == g_PlayerModelId && D_80071C0C == 1) {
            if (i > 2) {
                D_80071C0C = 0;
                break;
            }
        } else if (i > 16) {
            break;
        }

        vectors[3].vx = FieldEntityGetDirVectorX(g_FieldEntity[modelId].MoveDir) * absNx / 4096;
        vectors[3].vy = -(FieldEntityGetDirVectorY(g_FieldEntity[modelId].MoveDir) * absNy) / 4096;
        vectors[3].vx = g_FieldEntity[modelId].MoveSpeed * vectors[3].vx / 256;
        vectors[3].vy = g_FieldEntity[modelId].MoveSpeed * vectors[3].vy / 256;
        vectors[3].vx = g_FieldEntity[modelId].PosX + vectors[3].vx;
        vectors[3].vy = g_FieldEntity[modelId].PosY + vectors[3].vy;
        vectors[3].vz = g_FieldEntity[modelId].PosZ;

        vectors[5].vx =
            FieldEntityGetDirVectorX(g_FieldEntity[modelId].MoveDir + 32) * g_FieldEntity[modelId].SolidRange;
        vectors[5].vy =
            -FieldEntityGetDirVectorY(g_FieldEntity[modelId].MoveDir + 32) * g_FieldEntity[modelId].SolidRange;
        vectors[4].vx = vectors[3].vx + vectors[5].vx;
        vectors[4].vy = vectors[3].vy + vectors[5].vy;
        vectors[4].vz = vectors[3].vz;
        leftCorrection = FieldEntityWalkmeshCross(&triangleId, &vectors[4], &vectors[5], &vectors[1]);
        leftContact = FieldEntityCollisionCheck(modelId, &vectors[4]) != 0;
        triangleId = g_FieldEntity[modelId].PosI;

        vectors[5].vx =
            FieldEntityGetDirVectorX(g_FieldEntity[modelId].MoveDir - 32) * g_FieldEntity[modelId].SolidRange;
        vectors[5].vy =
            -FieldEntityGetDirVectorY(g_FieldEntity[modelId].MoveDir - 32) * g_FieldEntity[modelId].SolidRange;
        vectors[4].vx = vectors[3].vx + vectors[5].vx;
        vectors[4].vy = vectors[3].vy + vectors[5].vy;
        vectors[4].vz = vectors[3].vz;
        rightCorrection = FieldEntityWalkmeshCross(&triangleId, &vectors[4], &vectors[5], &vectors[2]);
        rightContact = FieldEntityCollisionCheck(modelId, &vectors[4]) != 0;
        triangleId = g_FieldEntity[modelId].PosI;

        vectors[5].vx = FieldEntityGetDirVectorX(g_FieldEntity[modelId].MoveDir) * g_FieldEntity[modelId].SolidRange;
        vectors[5].vy = -FieldEntityGetDirVectorY(g_FieldEntity[modelId].MoveDir) * g_FieldEntity[modelId].SolidRange;
        vectors[4].vx = vectors[3].vx + vectors[5].vx;
        vectors[4].vy = vectors[3].vy + vectors[5].vy;
        vectors[4].vz = vectors[3].vz;
        headCorrection = FieldEntityWalkmeshCross(&triangleId, &vectors[4], &vectors[5], &vectors[0]);
        headContact = 0;
        if (FieldEntityCollisionCheck(modelId, &vectors[4]) && !headCorrection) {
            headContact = 8;
        }
        if (!headCorrection && !leftCorrection && !rightCorrection && !headContact && !leftContact && !rightContact) {
            break;
        }
        if (modelId == g_PlayerModelId && !g_FieldState.characterLock) {
            if (headContact || leftContact || rightContact) {
                break;
            }
        } else {
            if (headCorrection && !leftCorrection && !rightCorrection) {
                g_FieldEntity[modelId].MoveDir -= headCorrection;
            } else if (headContact && !leftContact && !rightContact) {
                g_FieldEntity[modelId].MoveDir -= headContact;
            }
        }
        if (leftCorrection && rightCorrection) {
            break;
        }
        if (leftCorrection || leftContact) {
            g_FieldEntity[modelId].MoveDir += 248;
        } else if (rightCorrection || rightContact) {
            g_FieldEntity[modelId].MoveDir += 8;
        }
    }

    finalCorrection = FieldEntityWalkmeshCross(&g_FieldEntity[modelId].PosI, &vectors[3], &vectors[5], &vectors[0]);
    if (modelId == g_PlayerModelId && !g_FieldState.characterLock) {
        D_80071C0C = FieldEntityLineCheck(&g_FieldEntity[modelId], g_FieldLines, &vectors[3]);
        if (!g_FieldState.mapJumpDisabled) {
            FieldEntityGatewayCheck(&g_FieldEntity[modelId], g_FieldTriggers->gateways, &vectors[3]);
        }
        FieldEntityTriggerCheck(&g_FieldEntity[modelId], g_FieldTriggers->triggers, &vectors[3]);
    }
    if (!headCorrection && !leftCorrection && !rightCorrection && !headContact && !leftContact && !rightContact &&
        !finalCorrection) {
        g_FieldEntity[modelId].PosX = vectors[3].vx;
        g_FieldEntity[modelId].PosY = vectors[3].vy;
        g_FieldEntity[modelId].PosZ = vectors[3].vz << 12;
        if (g_FieldEntity[modelId].scriptedMoveMode == SMODE_NONE && modelId == g_PlayerModelId) {
            u8 activeAnimation;

            g_FieldEntity[modelId].animSpeed = 16;
            if (g_FieldKeyState & PAD_CROSS) {
                activeAnimation = 0;
                if (g_FieldModelData->models[g_FieldModelLoaderData[modelId].modelEntryIndex].animationCount >
                    g_FieldState.runAnimId) {
                    activeAnimation = g_FieldState.runAnimId;
                }
                g_FieldEntity[modelId].activeAnimId = activeAnimation;
            } else {
                activeAnimation = 0;
                if (g_FieldModelData->models[g_FieldModelLoaderData[modelId].modelEntryIndex].animationCount >
                    g_FieldState.walkAnimId) {
                    activeAnimation = g_FieldState.walkAnimId;
                }
                g_FieldEntity[modelId].activeAnimId = activeAnimation;
            }
        }
        return 1;
    }
    return 0;
}

s32 FieldEntityCollisionCheck(s16 modelId, VECTOR* pos) {
    s32 collisionRadius;
    s32 dz;
    s16 i;
    s32 dy;
    s32 dx;
    s32 distanceSq;
    s32 thresholdSq;
    s32 result;

    result = 0;
    collisionRadius = g_FieldEntity[modelId].SolidRange;
    for (i = 0; i < g_FieldState.modelCount; i++) {
        if (i != modelId && !g_FieldEntity[i].SolidOff) {
            dz = (g_FieldEntity[i].PosZ >> 12) - pos->vz;
            if (dz > -127 && dz < 128) {
                dx = (g_FieldEntity[i].PosX - pos->vx) >> 12;
                dy = (g_FieldEntity[i].PosY - pos->vy) >> 12;
                thresholdSq = (collisionRadius + g_FieldEntity[i].SolidRange) / 2;
                thresholdSq *= thresholdSq;
                distanceSq = dx * dx + dy * dy;
                if (thresholdSq > distanceSq) {
                    if (modelId == g_PlayerModelId) {
                        g_FieldEntity[i].requestPushScript = 1;
                    }
                    result = 1;
                }
            }
        }
    }
    return result;
}

s32 FieldEntitySqrDistToLine(LinePos* pos, VECTOR* from, VECTOR* nearest) {
    s32 dist;

    dist = -256 *
           ((pos->x1 - from->vx) * (pos->x2 - pos->x1) + (pos->y1 - from->vy) * (pos->y2 - pos->y1) +
            (pos->z1 - from->vz) * (pos->z2 - pos->z1)) /
           ((pos->x2 - pos->x1) * (pos->x2 - pos->x1) + (pos->y2 - pos->y1) * (pos->y2 - pos->y1) +
            (pos->z2 - pos->z1) * (pos->z2 - pos->z1));
    nearest->vx = ((dist * (pos->x2 - pos->x1)) >> 8) + pos->x1;
    nearest->vy = ((dist * (pos->y2 - pos->y1)) >> 8) + pos->y1;
    nearest->vz = ((dist * (pos->z2 - pos->z1)) >> 8) + pos->z1;
    if (!(((pos->x1 - nearest->vx < 0 || pos->x2 - nearest->vx > 0) &&
           (pos->x1 - nearest->vx > 0 || pos->x2 - nearest->vx < 0)) ||
          ((pos->y1 - nearest->vy < 0 || pos->y2 - nearest->vy > 0) &&
           (pos->y1 - nearest->vy > 0 || pos->y2 - nearest->vy < 0)))) {
        dist =
            (nearest->vx - from->vx) * (nearest->vx - from->vx) + (nearest->vy - from->vy) * (nearest->vy - from->vy) +
            (nearest->vz - from->vz) * (nearest->vz - from->vz);
    } else {
        dist = -1;
    }
    return dist;
}

s32 FieldEntityLineCheck(FieldEntity* entity, FieldLine* lines, VECTOR* pos) {
    VECTOR* from = (VECTOR*)getScratchAddr(0);
    VECTOR* to = (VECTOR*)getScratchAddr(sizeof(VECTOR) / 4);
    VECTOR* nearest = (VECTOR*)getScratchAddr(sizeof(VECTOR) / 4 * 2);
    s32 distanceSq;
    s32 sideFrom;
    s32 sideTo;
    s32 result;
    s32 i;

    from->vx = entity->PosX >> 12;
    from->vy = entity->PosY >> 12;
    result = 0;
    from->vz = entity->PosZ >> 12;
    to->vx = pos->vx >> 12;
    to->vy = pos->vy >> 12;
    to->vz = entity->PosZ >> 12;
    for (i = 0; i < 32; i++, lines++) {
        if (lines->isActive == 1) {
            lines->isOnLine = 0;
            distanceSq = FieldEntitySqrDistToLine(&lines->pos, from, nearest);
            if (distanceSq != -1 && distanceSq < entity->SolidRange * entity->SolidRange) {
                if (lines->slipDisabled == 1) {
                    result = 1;
                }
                if (!lines->touch) {
                    lines->touchOn = 1;
                }
                lines->touch = 1;
                sideFrom = (lines->pos.x2 - lines->pos.x1) * (from->vy - lines->pos.y1) -
                           (from->vx - lines->pos.x1) * (lines->pos.y2 - lines->pos.y1);
                sideTo = (lines->pos.x2 - lines->pos.x1) * (to->vy - lines->pos.y1) -
                         (to->vx - lines->pos.x1) * (lines->pos.y2 - lines->pos.y1);
                if ((sideFrom >= 0 && sideTo < 0) || (sideTo >= 0 && sideFrom < 0) || (sideFrom > 0 && sideTo <= 0) ||
                    (sideTo > 0 && sideFrom <= 0)) {
                    lines->across = 1;
                }
                if (from->vx == nearest->vx && from->vy == nearest->vy) {
                    lines->requestPushScript = 1;
                    lines->isOnLine = 1;
                } else {
                    lines->proximityAngle = FieldEntityDirByVec(from, nearest, &distanceSq);
                    if (((lines->proximityAngle - entity->MoveDir + 64) & 0xFF) < 128) {
                        lines->requestPushScript = 1;
                        lines->isOnLine = 1;
                    }
                }
            } else {
                if (lines->touch == 1) {
                    lines->touchOff = 1;
                }
                lines->touch = 0;
            }
        }
    }
    return result;
}

void FieldEntityLineInteract(FieldEntity* entity, FieldLine* lines) {
    VECTOR* from = (VECTOR*)getScratchAddr(0);
    VECTOR* nearest = (VECTOR*)getScratchAddr(sizeof(VECTOR) / 4);
    s32 distanceSq;
    s32 i;

    from->vx = entity->PosX >> 12;
    from->vy = entity->PosY >> 12;
    from->vz = entity->PosZ >> 12;

    for (i = 0; i < LEN(g_FieldLines); i++, lines++) {
        if (lines->isActive == 1 && entity->scriptedMoveMode == SMODE_NONE) {
            distanceSq = FieldEntitySqrDistToLine(&lines->pos, from, nearest);
            if (distanceSq != -1 && distanceSq < entity->SolidRange * entity->SolidRange) {
                if (!lines->touch) {
                    lines->touchOn = 1;
                }
                lines->touch = 1;
            } else {
                if (lines->touch == 1) {
                    lines->touchOff = 1;
                }
                lines->touch = 0;
            }
            if (lines->isOnLine == 1 && ((lines->proximityAngle - entity->MoveDir + 32) & 0xFF) < 64 &&
                (g_FieldState.activeKeys & PAD_CIRCLE) && !(g_FieldState.activeKeysPrev & PAD_CIRCLE)) {
                lines->requestTalkScript = 1;
            }
        }
    }
}

void FieldEntityLineClear(FieldLine* lines) {
    s32 i;

    for (i = 0; i < LEN(g_FieldLines); i++) {
        lines->isOnLine = 0;
        lines++;
    }
}

static void FieldEntityGatewayCheck(FieldEntity* entity, FieldGateway* gateways, VECTOR* pos) {
    VECTOR* from = (VECTOR*)getScratchAddr(0);
    VECTOR* to = (VECTOR*)getScratchAddr(sizeof(VECTOR) / 4);
    VECTOR* nearest = (VECTOR*)getScratchAddr(sizeof(VECTOR) / 4 * 2);
    s32 distanceSq;
    s32 sideFrom;
    s32 sideTo;
    s32 i;

    from->vx = entity->PosX >> 12;
    from->vy = entity->PosY >> 12;
    from->vz = entity->PosZ >> 12;
    to->vx = pos->vx >> 12;
    to->vy = pos->vy >> 12;
    to->vz = entity->PosZ >> 12;

    for (i = 0; i < LEN(g_FieldTriggers->gateways); i++, gateways++) {
        if (gateways->fieldMapId != 0x7FFF) {
            distanceSq = FieldEntitySqrDistToLine(&gateways->pos, from, nearest);
            if (distanceSq != -1 && distanceSq < entity->SolidRange * entity->SolidRange) {
                sideFrom = (gateways->pos.x2 - gateways->pos.x1) * (from->vy - gateways->pos.y1) -
                           (from->vx - gateways->pos.x1) * (gateways->pos.y2 - gateways->pos.y1);
                sideTo = (gateways->pos.x2 - gateways->pos.x1) * (to->vy - gateways->pos.y1) -
                         (to->vx - gateways->pos.x1) * (gateways->pos.y2 - gateways->pos.y1);
                if ((sideFrom >= 0 && sideTo < 0) || (sideTo >= 0 && sideFrom < 0) || (sideFrom > 0 && sideTo <= 0) ||
                    (sideTo > 0 && sideFrom <= 0)) {
                    FieldEntityGatewayMapLoad(gateways);
                }
            }
        }
    }
}

s16 FieldEntityBgTriggerActivate(FieldBgTrigger* trigger, u8 behaviour) {
    u8 frameBit;
    s16 changed;

    changed = 0;
    switch (behaviour) {
    case 0:
    case 2:
    case 4:
        frameBit = 1 << trigger->backgroundFrameId;
        if (!(g_FieldState.backgroundLayerVisibility[trigger->backgroundGroupId] & frameBit)) {
            changed = 1;
        }
        g_FieldState.backgroundLayerVisibility[trigger->backgroundGroupId] |= frameBit;
        break;
    case 1:
    case 3:
    case 5:
        frameBit = ~(1 << trigger->backgroundFrameId);
        if ((g_FieldState.backgroundLayerVisibility[trigger->backgroundGroupId] | frameBit) == 0xFF) {
            changed = 1;
        }
        g_FieldState.backgroundLayerVisibility[trigger->backgroundGroupId] &= frameBit;
        break;
    }
    return changed;
}

static void FieldEntityTriggerCheck(FieldEntity* entity, FieldBgTrigger* triggers, VECTOR* pos) {
    VECTOR* from = (VECTOR*)getScratchAddr(0);
    VECTOR* nearest = (VECTOR*)getScratchAddr(sizeof(VECTOR) / 4 * 2);
    s32 distanceSq;
    u16 soundIds[4] = {SFX_NULL, SFX_METAL_DOOR, SFX_WOOD_DOOR, SFX_QUICK_SWIPE};
    s32 i;

    from->vx = entity->PosX >> 12;
    from->vy = entity->PosY >> 12;
    from->vz = entity->PosZ >> 12;

    for (i = 0; i < LEN(g_FieldTriggers->triggers); i++, triggers++) {
        if (triggers->backgroundGroupId != 0xFF) {
            distanceSq = FieldEntitySqrDistToLine(&triggers->pos, from, nearest);
            if (distanceSq != -1 && distanceSq < entity->SolidRange * entity->SolidRange) {
                if (from->vx == nearest->vx && from->vy == nearest->vy) {
                    if (FieldEntityBgTriggerActivate(triggers, triggers->behaviour) == 1) {
                        AkaoPlaySoundEffect(soundIds[triggers->soundId]);
                    }
                } else if (((FieldEntityDirByVec(from, nearest, &distanceSq) - entity->MoveDir + 64) & 0xFF) < 128) {
                    if (FieldEntityBgTriggerActivate(triggers, triggers->behaviour) == 1) {
                        AkaoPlaySoundEffect(soundIds[triggers->soundId]);
                    }
                }
            } else if (triggers->behaviour < 4 || // 0/1 persistent, no revert on leaving
                       (triggers->pos.x2 - triggers->pos.x1) * (from->vy - triggers->pos.y1) -
                               (triggers->pos.y2 - triggers->pos.y1) * (from->vx - triggers->pos.x1) <=
                           0) { // 4/5: only revert leaving from the trigger's facing side
                if ((triggers->behaviour == 2 || triggers->behaviour == 4) && // momentary show, revert to hide
                    FieldEntityBgTriggerActivate(triggers, 1) == 1) {
                    AkaoPlaySoundEffect(soundIds[triggers->soundId]);
                }
                if (triggers->behaviour == 3 || triggers->behaviour == 5) { // momentary hide, revert to show
                    if (FieldEntityBgTriggerActivate(triggers, 0) == 1) {
                        AkaoPlaySoundEffect(soundIds[triggers->soundId]);
                    }
                }
            }
        }
    }
}

void FieldEntityBgTriggerInit(FieldBgTrigger* triggers) {
    s32 i;

    for (i = 0; i < 12; i++, triggers++) {
        if (triggers->backgroundGroupId != 0xFF) {
            switch (triggers->behaviour) {
            case 0:
            case 2:
            case 4:
                FieldEntityBgTriggerActivate(triggers, 1);
                break;
            case 1:
            case 3:
            case 5:
                FieldEntityBgTriggerActivate(triggers, 0);
                break;
            }
        }
    }
}
