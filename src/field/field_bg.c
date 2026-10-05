//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include <libgte.h>
#include "field_private.h"

extern s16 g_CameraScrollCurrentStep;
extern s16 g_CameraScrollStartX;
extern s16 g_CameraScrollTargetX;
extern s16 g_CameraScrollStartY;
extern s16 g_CameraScrollTargetY;
extern s16 g_CameraScrollNumSteps;

INCLUDE_ASM("asm/us/field/nonmatchings/field_bg", FieldBackgroundInitPackets);

INCLUDE_ASM("asm/us/field/nonmatchings/field_bg", AddBackgroundToRender);

s32 FieldCalcLinearStep(s32 start, s32 target, s32 duration, s32 step) {
    s32 delta = target - start;

    if (delta > -524288 && delta < 524288) {
        start += (delta * step) / duration;
    } else {
        start += (delta / duration) * step;
    }
    return start;
}

s32 FieldCalcEaseInOut(s32 start, s32 target, s32 duration, s32 step) {
    u8 angle;
    s32 delta;

    angle = (step << 12) / duration / 32 + 128;
    delta = target - start;
    start += (FieldEntityGetDirVectorY(angle) + 4096) * delta / 8192;
    return start;
}

s32 FieldCalcWorldToScreenPos(SVECTOR* worldPos, SVECTOR* screenPos) {
    long flag;
    long depth;
    s32 ret;

    PushMatrix();
    SetRotMatrix(D_80071E40);
    SetTransMatrix(D_80071E40);
    SetGeomOffset(0, 0);
    ret = RotTransPers(worldPos, (long*)screenPos, &flag, &depth);
    PopMatrix();
    return ret;
}

void FieldBGShakeUpdate(FieldShakeData* data) {
    if (data->enabled == 1) {
        if (!data->segmentActive) {
            data->currentStep = 0;
            data->start = 0;
            data->target = (s16)(g_RandomTable[data->rngId] * data->amplitude) / 256;
            data->rngId++;
            data->segmentActive = 1;
        } else if (data->numStepsPerSegment < data->currentStep) {
            data->currentStep = 0;
            data->start = data->target;
            if (data->start < 0) {
                data->target = (s16)(g_RandomTable[data->rngId] * data->amplitude) / 256;
            } else {
                data->target = -(s16)(g_RandomTable[data->rngId] * data->amplitude) / 256;
            }
            data->rngId++;
        } else {
            data->currentStep++;
            data->currentOffset =
                FieldCalcEaseInOut(data->start, data->target, data->numStepsPerSegment, data->currentStep);
        }
    } else if (data->segmentActive == 1) {
        if (data->numStepsPerSegment < data->currentStep) {
            data->currentStep = 0;
            data->start = data->target;
            data->target = 0;
            data->rngId++;
            data->segmentActive = 0;
        } else {
            data->currentStep++;
            data->currentOffset =
                FieldCalcEaseInOut(data->start, data->target, data->numStepsPerSegment, data->currentStep);
        }
    } else if (data->numStepsPerSegment == data->currentStep) {
        data->currentOffset = 0;
    } else {
        data->currentStep++;
        data->currentOffset =
            FieldCalcEaseInOut(data->start, data->target, data->numStepsPerSegment, data->currentStep);
    }
}

void FieldBGScrollInit(void) {
    if (g_FieldState.cameraScrollState == SCRLST_INIT) {
        switch (g_FieldState.cameraScrollMode) {
        case SCRL_OFF:
            g_CameraScrollEnabled = 0;
            g_CameraScrollX = 0;
            g_CameraScrollY = 0;
            g_FieldState.cameraScrollState = SCRLST_DONE;
            break;
        case SCRL_TO_ENTITY_INSTANT:
            g_CameraScrollEnabled = 1;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case SCRL_TO_ENTITY_LINEAR:
            g_CameraScrollEnabled = 1;
            g_CameraScrollNumSteps = g_FieldState.cameraScrollNumSteps;
            g_CameraScrollCurrentStep = 0;
            g_CameraScrollStartX = g_CameraScrollX;
            g_CameraScrollStartY = g_CameraScrollY;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case SCRL_TO_ENTITY_SMOOTH:
            g_CameraScrollEnabled = 1;
            g_CameraScrollNumSteps = g_FieldState.cameraScrollNumSteps;
            g_CameraScrollCurrentStep = 0;
            g_CameraScrollStartX = g_CameraScrollX;
            g_CameraScrollStartY = g_CameraScrollY;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case SCRL_TO_COORDS_INSTANT:
            g_CameraScrollEnabled = 1;
            g_CameraScrollX = g_FieldState.cameraScrollTargetX;
            g_CameraScrollY = g_FieldState.cameraScrollTargetY;
            g_FieldState.cameraScrollState = SCRLST_DONE;
            break;
        case SCRL_TO_COORDS_LINEAR:
            g_CameraScrollEnabled = 1;
            g_CameraScrollNumSteps = g_FieldState.cameraScrollNumSteps;
            g_CameraScrollCurrentStep = 0;
            g_CameraScrollStartX = g_CameraScrollX;
            g_CameraScrollStartY = g_CameraScrollY;
            g_CameraScrollTargetX = g_FieldState.cameraScrollTargetX;
            g_CameraScrollTargetY = g_FieldState.cameraScrollTargetY;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case SCRL_TO_COORDS_SMOOTH:
            g_CameraScrollEnabled = 1;
            g_CameraScrollNumSteps = g_FieldState.cameraScrollNumSteps;
            g_CameraScrollCurrentStep = 0;
            g_CameraScrollStartX = g_CameraScrollX;
            g_CameraScrollStartY = g_CameraScrollY;
            g_CameraScrollTargetX = g_FieldState.cameraScrollTargetX;
            g_CameraScrollTargetY = g_FieldState.cameraScrollTargetY;
            g_FieldState.cameraScrollState = SCRLST_ACTIVE;
            break;
        case 7:
        case 8:
        case 9:
            break;
        }
    }
}

void FieldCalcPointOnLine(FieldTriggers* trigs, SVECTOR* pos) {
    s32 projection;
    s32 denominator;
    s32 dx;
    s32 dy;

    if (trigs->cameraRailMode == CAMRAIL_TL_BR) {
        projection = -((trigs->cameraRange.left + 160 - pos->vx) *
                           (trigs->cameraRange.right - 160 - (trigs->cameraRange.left + 160)) +
                       (trigs->cameraRange.top + 120 - pos->vy) *
                           (trigs->cameraRange.bottom - 120 - (trigs->cameraRange.top + 120)));
        denominator = ((trigs->cameraRange.right - 160 - (trigs->cameraRange.left + 160)) *
                           (trigs->cameraRange.right - 160 - (trigs->cameraRange.left + 160)) +
                       (trigs->cameraRange.bottom - 120 - (trigs->cameraRange.top + 120)) *
                           (trigs->cameraRange.bottom - 120 - (trigs->cameraRange.top + 120))) >>
                      8;
        dx = trigs->cameraRange.right - 160 - (trigs->cameraRange.left + 160);
        dy = trigs->cameraRange.bottom - 120 - (trigs->cameraRange.top + 120);
        pos->vx = ((projection * dx / denominator) >> 8) + (trigs->cameraRange.left + 160);
        pos->vy = ((projection * dy / denominator) >> 8) + (trigs->cameraRange.top + 120);
    }
    if (trigs->cameraRailMode == CAMRAIL_BL_TR) {
        projection = -((trigs->cameraRange.left + 160 - pos->vx) *
                           (trigs->cameraRange.right - 160 - (trigs->cameraRange.left + 160)) +
                       (trigs->cameraRange.bottom - 120 - pos->vy) *
                           (trigs->cameraRange.top + 120 - (trigs->cameraRange.bottom - 120)));
        denominator = ((trigs->cameraRange.right - 160 - (trigs->cameraRange.left + 160)) *
                           (trigs->cameraRange.right - 160 - (trigs->cameraRange.left + 160)) +
                       (trigs->cameraRange.top - 120 - (trigs->cameraRange.bottom - 120)) *
                           (trigs->cameraRange.top + 120 - (trigs->cameraRange.bottom - 120))) >>
                      8;
        dx = trigs->cameraRange.right - 160 - (trigs->cameraRange.left + 160);
        dy = trigs->cameraRange.top + 120 - (trigs->cameraRange.bottom - 120);
        pos->vx = ((projection * dx / denominator) >> 8) + (trigs->cameraRange.left + 160);
        pos->vy = ((projection * dy / denominator) >> 8) + (trigs->cameraRange.bottom - 120);
    }
}

void FieldBGClampPos(SVECTOR* pos) {
    if (pos->vx > g_FieldTriggers->cameraRange.right - 160) {
        pos->vx = g_FieldTriggers->cameraRange.right - 160;
    }
    if (pos->vx < g_FieldTriggers->cameraRange.left + 160) {
        pos->vx = g_FieldTriggers->cameraRange.left + 160;
    }
    if (pos->vy > g_FieldTriggers->cameraRange.bottom - 120) {
        pos->vy = g_FieldTriggers->cameraRange.bottom - 120;
    }
    if (pos->vy < g_FieldTriggers->cameraRange.top + 120) {
        pos->vy = g_FieldTriggers->cameraRange.top + 120;
    }
}

void FieldBGGetEntityScreenPos(SVECTOR* screenPos) {
    SVECTOR pos;

    pos.vx = g_FieldEntity[g_FieldState.cameraScrollTargetId].PosX >> 12;
    pos.vy = g_FieldEntity[g_FieldState.cameraScrollTargetId].PosY >> 12;
    pos.vz = (g_FieldEntity[g_FieldState.cameraScrollTargetId].PosZ >> 12) + g_FieldState.viewOffset;
    FieldCalcWorldToScreenPos(&pos, screenPos);
}

void FieldBGScrollUpdate(void) {
    SVECTOR pos;

    if (g_FieldState.cameraScrollState != SCRLST_ACTIVE) {
        return;
    }

    switch (g_FieldState.cameraScrollMode) {
    case SCRL_OFF:
        break;
    case SCRL_TO_ENTITY_INSTANT:
        FieldBGGetEntityScreenPos(&pos);
        FieldBGClampPos(&pos);
        g_CameraScrollX = -pos.vx;
        g_CameraScrollY = -pos.vy;
        break;
    case SCRL_TO_ENTITY_LINEAR:
        FieldBGGetEntityScreenPos(&pos);
        FieldBGClampPos(&pos);
        g_CameraScrollX =
            FieldCalcLinearStep(g_CameraScrollStartX, -pos.vx, g_CameraScrollNumSteps, g_CameraScrollCurrentStep);
        g_CameraScrollY =
            FieldCalcLinearStep(g_CameraScrollStartY, -pos.vy, g_CameraScrollNumSteps, g_CameraScrollCurrentStep);
        if (g_CameraScrollNumSteps == g_CameraScrollCurrentStep) {
            g_FieldState.cameraScrollState = SCRLST_DONE;
        } else {
            g_CameraScrollCurrentStep++;
        }
        break;
    case SCRL_TO_ENTITY_SMOOTH:
        FieldBGGetEntityScreenPos(&pos);
        FieldBGClampPos(&pos);
        g_CameraScrollX =
            FieldCalcEaseInOut(g_CameraScrollStartX, -pos.vx, g_CameraScrollNumSteps, g_CameraScrollCurrentStep);
        g_CameraScrollY =
            FieldCalcEaseInOut(g_CameraScrollStartY, -pos.vy, g_CameraScrollNumSteps, g_CameraScrollCurrentStep);
        if (g_CameraScrollNumSteps == g_CameraScrollCurrentStep) {
            g_FieldState.cameraScrollState = SCRLST_DONE;
        } else {
            g_CameraScrollCurrentStep++;
        }
        break;
    case SCRL_TO_COORDS_LINEAR:
        g_CameraScrollX = FieldCalcLinearStep(
            g_CameraScrollStartX, g_CameraScrollTargetX, g_CameraScrollNumSteps, g_CameraScrollCurrentStep);
        g_CameraScrollY = FieldCalcLinearStep(
            g_CameraScrollStartY, g_CameraScrollTargetY, g_CameraScrollNumSteps, g_CameraScrollCurrentStep);
        if (g_CameraScrollNumSteps == g_CameraScrollCurrentStep) {
            g_FieldState.cameraScrollState = SCRLST_DONE;
        } else {
            g_CameraScrollCurrentStep++;
        }
        break;
    case SCRL_TO_COORDS_SMOOTH:
        g_CameraScrollX = FieldCalcEaseInOut(
            g_CameraScrollStartX, g_CameraScrollTargetX, g_CameraScrollNumSteps, g_CameraScrollCurrentStep);
        g_CameraScrollY = FieldCalcEaseInOut(
            g_CameraScrollStartY, g_CameraScrollTargetY, g_CameraScrollNumSteps, g_CameraScrollCurrentStep);
        if (g_CameraScrollNumSteps == g_CameraScrollCurrentStep) {
            g_FieldState.cameraScrollState = SCRLST_DONE;
        } else {
            g_CameraScrollCurrentStep++;
        }
        break;
    case 9:
        break;
    }
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field_bg", FieldBGUpdateDrawenv);
#else

extern s16 D_80071A48;
extern s16 D_80071A4A;
extern s16 D_80071A4C;
extern s16 D_80071A4E;
extern s16 D_80071A50;
extern s16 D_80071A52;
extern s32 D_8007EB90;
extern s32 D_8007EB94;
extern s16 D_800E48E4;
extern s16 D_800E48E6;
extern s32 D_800E48EC;
extern DRAWENV D_80113FE4[2];
extern DRAWENV D_8011409C[2];
extern DRAWENV D_80114154[2];
extern DRAWENV D_8011420C[2];
extern s16 g_PosCursorX;
extern s16 g_PosCursorY;

void FieldBGUpdateDrawenv(struct FieldRenderData* renderData) {
    SVECTOR pos;
    SVECTOR screenPos;
    s16 layer2ParallaxScrollX;
    s16 layer2ParallaxScrollY;
    s16 layer3ParallaxScrollX;
    s16 layer3ParallaxScrollY;
    DR_ENV* parallaxPacket;
    DRAWENV* parallaxEnv;
    DR_ENV* scrollPacket;
    DRAWENV* scrollEnv;
    DR_ENV* moviePacket;
    DRAWENV* movieEnv;

    g_FieldTriggers->layer2ScrollPhaseX += g_FieldState.layer2_bgScrollXSpeed;
    g_FieldTriggers->layer2ScrollPhaseY += g_FieldState.layer2_bgScrollYSpeed;
    g_FieldTriggers->layer2ScrollPhaseX %= g_FieldTriggers->layer2AnimWidth * 16;
    g_FieldTriggers->layer2ScrollPhaseY %= g_FieldTriggers->layer2AnimHeight * 16;
    g_FieldTriggers->layer3ScrollPhaseX += g_FieldState.layer3_bgScrollXSpeed;
    g_FieldTriggers->layer3ScrollPhaseY += g_FieldState.layer3_bgScrollYSpeed;
    g_FieldTriggers->layer3ScrollPhaseX %= g_FieldTriggers->layer3AnimWidth * 16;
    g_FieldTriggers->layer3ScrollPhaseY %= g_FieldTriggers->layer3AnimHeight * 16;
    SetGeomScreen(((s16*)D_80071E40)[18]);
    if (!g_FieldMovieJustStarted || g_FieldState.backgroundMovieEnabled) {
        if (g_CameraScrollEnabled == 0) {
            switch (g_FieldState.viewOffsetMode) {
            case 0:
                break;
            case 1:
                g_FieldState.viewOffset = FieldCalcLinearStep(g_FieldState.viewOffsetStart, g_FieldState.viewOffsetTarget,
                                                               g_FieldState.viewOffsetNumSteps,
                                                               g_FieldState.viewOffsetCurrentStep) & 0xFF;
                if (g_FieldState.viewOffsetCurrentStep == g_FieldState.viewOffsetNumSteps) {
                    g_FieldState.viewOffsetMode = 3;
                } else {
                    g_FieldState.viewOffsetCurrentStep++;
                }
                break;
            case 2:
                g_FieldState.viewOffset = FieldCalcEaseInOut(g_FieldState.viewOffsetStart, g_FieldState.viewOffsetTarget,
                                                              g_FieldState.viewOffsetNumSteps,
                                                              g_FieldState.viewOffsetCurrentStep) & 0xFF;
                if (g_FieldState.viewOffsetCurrentStep == g_FieldState.viewOffsetNumSteps) {
                    g_FieldState.viewOffsetMode = 3;
                } else {
                    g_FieldState.viewOffsetCurrentStep++;
                }
                break;
            default:
                break;
            }
            pos.vx = (g_FieldEntity[g_PlayerModelId].PosX >> 12) + g_FieldEntity[g_PlayerModelId].OffsetX;
            pos.vy = (g_FieldEntity[g_PlayerModelId].PosY >> 12) + g_FieldEntity[g_PlayerModelId].OffsetY;
            pos.vz = (g_FieldEntity[g_PlayerModelId].PosZ >> 12) + g_FieldEntity[g_PlayerModelId].OffsetZ +
                     ((g_FieldState.currentFieldScale * 128) >> 9);
            FieldCalcWorldToScreenPos(&pos, &screenPos);
            g_PosCursorX = (u16)screenPos.vx + D_8007EB90;
            pos.vx = g_FieldEntity[g_PlayerModelId].PosX >> 12;
            g_PosCursorY = (u16)screenPos.vy + D_8007EB94;
            pos.vy = g_FieldEntity[g_PlayerModelId].PosY >> 12;
            pos.vz = (g_FieldEntity[g_PlayerModelId].PosZ >> 12) + g_FieldState.viewOffset;
            D_800E48EC = FieldCalcWorldToScreenPos(&pos, &screenPos);
            D_800E48E4 = screenPos.vx;
            D_800E48E6 = screenPos.vy;
            FieldBGClampPos(&screenPos);
            FieldCalcPointOnLine(g_FieldTriggers, &screenPos);
            layer2ParallaxScrollX = (s16)(((screenPos.vx * g_FieldTriggers->layer2ParallaxFactorX) >> 8) +
                                            (g_FieldTriggers->layer2ScrollPhaseX >> 4)) % g_FieldTriggers->layer2AnimWidth;
            layer2ParallaxScrollY = (s16)(((screenPos.vy * g_FieldTriggers->layer2ParallaxFactorY) >> 8) +
                                            (g_FieldTriggers->layer2ScrollPhaseY >> 4)) % g_FieldTriggers->layer2AnimHeight;
            layer3ParallaxScrollX = (s16)(((screenPos.vx * g_FieldTriggers->layer3ParallaxFactorX) >> 8) +
                                            (g_FieldTriggers->layer3ScrollPhaseX >> 4)) % g_FieldTriggers->layer3AnimWidth;
            layer3ParallaxScrollY = (s16)(((screenPos.vy * g_FieldTriggers->layer3ParallaxFactorY) >> 8) +
                                            (g_FieldTriggers->layer3ScrollPhaseY >> 4)) % g_FieldTriggers->layer3AnimHeight;
            g_PosCursorY -= (u16)screenPos.vy;
            g_PosCursorX -= (u16)screenPos.vx;
            if (renderData == g_FieldRenderData) {
                D_80113F2C[0].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - (u16)screenPos.vx);
                D_80113F2C[0].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - (u16)screenPos.vy);
                SetDrawEnv(&renderData->sceneDrenv, &D_80113F2C[0]);
                D_80114154[0].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - (u16)screenPos.vx);
                D_80114154[0].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - (u16)screenPos.vy);
                SetDrawEnv(&renderData->bgDrenv3E, &D_80114154[0]);
                D_8011420C[0].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - (u16)screenPos.vx);
                D_8011420C[0].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - (u16)screenPos.vy);
                SetDrawEnv(&renderData->bgDrenv4E, &D_8011420C[0]);
                D_80113FE4[0].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - layer2ParallaxScrollX);
                D_80113FE4[0].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - layer2ParallaxScrollY);
                SetDrawEnv(&renderData->bgDrenv3S, &D_80113FE4[0]);
                parallaxPacket = &renderData->bgDrenv4S;
                D_8011409C[0].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - layer3ParallaxScrollX);
                D_8011409C[0].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - layer3ParallaxScrollY);
                parallaxEnv = &D_8011409C[0];
            } else {
                D_80113F2C[1].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - (u16)screenPos.vx);
                D_80113F2C[1].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - (u16)screenPos.vy) + 232;
                SetDrawEnv(&g_FieldRenderData[1].sceneDrenv, &D_80113F2C[1]);
                D_80114154[1].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - (u16)screenPos.vx);
                D_80114154[1].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - (u16)screenPos.vy) + 232;
                SetDrawEnv(&g_FieldRenderData[1].bgDrenv3E, &D_80114154[1]);
                D_8011420C[1].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - (u16)screenPos.vx);
                D_8011420C[1].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - (u16)screenPos.vy) + 232;
                SetDrawEnv(&g_FieldRenderData[1].bgDrenv4E, &D_8011420C[1]);
                D_80113FE4[1].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - layer2ParallaxScrollX);
                D_80113FE4[1].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - layer2ParallaxScrollY) + 232;
                SetDrawEnv(&g_FieldRenderData[1].bgDrenv3S, &D_80113FE4[1]);
                parallaxPacket = &g_FieldRenderData[1].bgDrenv4S;
                parallaxEnv = &D_8011409C[1];
                D_8011409C[1].ofs[0] = g_FieldState.shakeX.currentOffset + (D_8007EB90 - layer3ParallaxScrollX);
                D_8011409C[1].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 - layer3ParallaxScrollY) + 232;
            }
            SetDrawEnv(parallaxPacket, parallaxEnv);
            g_CameraScrollX = -screenPos.vx;
            D_80071A48 = (((u16)screenPos.vx + 320) - D_8007EB90) - g_FieldState.shakeX.currentOffset;
            D_80071A4C = ((layer2ParallaxScrollX + 320) - D_8007EB90) - g_FieldState.shakeX.currentOffset;
            g_CameraScrollY = -screenPos.vy;
            D_80071A4E = ((layer2ParallaxScrollY + 232) - D_8007EB94) - g_FieldState.shakeY.currentOffset;
            D_80071A4A = (((u16)screenPos.vy + 232) - D_8007EB94) - g_FieldState.shakeY.currentOffset;
            D_80071A50 = ((layer3ParallaxScrollX + 320) - D_8007EB90) - g_FieldState.shakeX.currentOffset;
            D_80071A52 = ((layer3ParallaxScrollY + 232) - D_8007EB94) - g_FieldState.shakeY.currentOffset;
            return;
        }
        layer2ParallaxScrollX = (s16)(((-g_CameraScrollX * g_FieldTriggers->layer2ParallaxFactorX) >> 8) +
                                        (g_FieldTriggers->layer2ScrollPhaseX >> 4)) % g_FieldTriggers->layer2AnimWidth;
        layer2ParallaxScrollY = (s16)(((-g_CameraScrollY * g_FieldTriggers->layer2ParallaxFactorY) >> 8) +
                                        (g_FieldTriggers->layer2ScrollPhaseY >> 4)) % g_FieldTriggers->layer2AnimHeight;
        layer3ParallaxScrollX = (s16)(((-g_CameraScrollX * g_FieldTriggers->layer3ParallaxFactorX) >> 8) +
                                        (g_FieldTriggers->layer3ScrollPhaseX >> 4)) % g_FieldTriggers->layer3AnimWidth;
        layer3ParallaxScrollY = (s16)(((-g_CameraScrollY * g_FieldTriggers->layer3ParallaxFactorY) >> 8) +
                                        (g_FieldTriggers->layer3ScrollPhaseY >> 4)) % g_FieldTriggers->layer3AnimHeight;
        if (renderData == g_FieldRenderData) {
            D_80113F2C[0].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) + g_CameraScrollX);
            D_80113F2C[0].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 + ((s16*)D_80071E40)[17] + g_CameraScrollY);
            SetDrawEnv(&renderData->sceneDrenv, &D_80113F2C[0]);
            D_80114154[0].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) + (u16)g_CameraScrollX);
            D_80114154[0].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 + ((s16*)D_80071E40)[17] + (u16)g_CameraScrollY);
            SetDrawEnv(&renderData->bgDrenv3E, &D_80114154[0]);
            D_8011420C[0].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) + (u16)g_CameraScrollX);
            D_8011420C[0].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 + ((s16*)D_80071E40)[17] + (u16)g_CameraScrollY);
            SetDrawEnv(&renderData->bgDrenv4E, &D_8011420C[0]);
            D_80113FE4[0].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) - layer2ParallaxScrollX);
            D_80113FE4[0].ofs[1] = g_FieldState.shakeY.currentOffset + ((D_8007EB94 + ((s16*)D_80071E40)[17]) - layer2ParallaxScrollY);
            SetDrawEnv(&renderData->bgDrenv3S, &D_80113FE4[0]);
            scrollPacket = &renderData->bgDrenv4S;
            D_8011409C[0].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) - layer3ParallaxScrollX);
            D_8011409C[0].ofs[1] = g_FieldState.shakeY.currentOffset + ((D_8007EB94 + ((s16*)D_80071E40)[17]) - layer3ParallaxScrollY);
            scrollEnv = &D_8011409C[0];
        } else {
            D_80113F2C[1].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) + g_CameraScrollX);
            D_80113F2C[1].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 + ((s16*)D_80071E40)[17] + g_CameraScrollY) + 232;
            SetDrawEnv(&g_FieldRenderData[1].sceneDrenv, &D_80113F2C[1]);
            D_80114154[1].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) + (u16)g_CameraScrollX);
            D_80114154[1].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 + ((s16*)D_80071E40)[17] + (u16)g_CameraScrollY) + 232;
            SetDrawEnv(&g_FieldRenderData[1].bgDrenv3E, &D_80114154[1]);
            D_8011420C[1].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) + (u16)g_CameraScrollX);
            D_8011420C[1].ofs[1] = g_FieldState.shakeY.currentOffset + (D_8007EB94 + ((s16*)D_80071E40)[17] + (u16)g_CameraScrollY) + 232;
            SetDrawEnv(&g_FieldRenderData[1].bgDrenv4E, &D_8011420C[1]);
            D_80113FE4[1].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) - layer2ParallaxScrollX);
            D_80113FE4[1].ofs[1] = g_FieldState.shakeY.currentOffset + ((D_8007EB94 + ((s16*)D_80071E40)[17]) - layer2ParallaxScrollY) + 232;
            SetDrawEnv(&g_FieldRenderData[1].bgDrenv3S, &D_80113FE4[1]);
            scrollPacket = &g_FieldRenderData[1].bgDrenv4S;
            scrollEnv = &D_8011409C[1];
            D_8011409C[1].ofs[0] = g_FieldState.shakeX.currentOffset + ((D_8007EB90 - ((s16*)D_80071E40)[16]) - layer3ParallaxScrollX);
            D_8011409C[1].ofs[1] = g_FieldState.shakeY.currentOffset + ((D_8007EB94 + ((s16*)D_80071E40)[17]) - layer3ParallaxScrollY) + 232;
        }
        SetDrawEnv(scrollPacket, scrollEnv);
        D_80071A48 = ((320 - (u16)g_CameraScrollX) - D_8007EB90) - g_FieldState.shakeX.currentOffset;
        D_80071A4A = ((232 - (u16)g_CameraScrollY) - D_8007EB94) - g_FieldState.shakeY.currentOffset;
        D_80071A4C = ((layer2ParallaxScrollX + 320) - D_8007EB90) - g_FieldState.shakeX.currentOffset;
        D_80071A4E = ((layer2ParallaxScrollY + 232) - D_8007EB94) - g_FieldState.shakeY.currentOffset;
        D_80071A50 = ((layer3ParallaxScrollX + 320) - D_8007EB90) - g_FieldState.shakeX.currentOffset;
        D_80071A52 = ((layer3ParallaxScrollY + 232) - D_8007EB94) - g_FieldState.shakeY.currentOffset;
        return;
    }
    if (g_FieldState.movieCamDisabled == 1) {
        moviePacket = &renderData->sceneDrenv;
        if (renderData == g_FieldRenderData) {
            D_80113F2C[0].ofs[0] = D_8007EB90 + (u16)g_CameraScrollX;
            D_80113F2C[0].ofs[1] = D_8007EB94 + (u16)g_CameraScrollY;
            movieEnv = &D_80113F2C[0];
        } else {
            moviePacket = &g_FieldRenderData[1].sceneDrenv;
            movieEnv = &D_80113F2C[1];
            D_80113F2C[1].ofs[0] = D_8007EB90 + (u16)g_CameraScrollX;
            D_80113F2C[1].ofs[1] = D_8007EB94 + (u16)g_CameraScrollY + 232;
        }
    } else {
        moviePacket = &renderData->sceneDrenv;
        if (renderData == g_FieldRenderData) {
            D_80113F2C[0].ofs[0] = D_8007EB90 - ((s16*)D_80071E40)[16];
            D_80113F2C[0].ofs[1] = D_8007EB94 + ((s16*)D_80071E40)[17];
            movieEnv = &D_80113F2C[0];
        } else {
            moviePacket = &g_FieldRenderData[1].sceneDrenv;
            D_80113F2C[1].ofs[0] = D_8007EB90 - ((s16*)D_80071E40)[16];
            movieEnv = &D_80113F2C[1];
            D_80113F2C[1].ofs[1] = D_8007EB94 + ((s16*)D_80071E40)[17] + 232;
        }
    }
    SetDrawEnv(moviePacket, movieEnv);
}

#endif
