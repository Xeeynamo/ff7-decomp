//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern u8 g_RandomTable[256];
extern s16 g_CameraScrollX;
extern s16 g_CameraScrollY;
extern s16 g_CameraScrollCurrentStep;
extern s16 g_CameraScrollStartX;
extern s16 g_CameraScrollTargetX;
extern s16 g_CameraScrollStartY;
extern s16 g_CameraScrollTargetY;
extern s16 g_CameraScrollEnabled;
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_bg", FieldCalcPointOnLine);

INCLUDE_ASM("asm/us/field/nonmatchings/field_bg", FieldBGClampPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field_bg", FieldBGGetEntityScreenPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field_bg", FieldBGScrollUpdate);

INCLUDE_ASM("asm/us/field/nonmatchings/field_bg", FieldBGUpdateDrawenv);
