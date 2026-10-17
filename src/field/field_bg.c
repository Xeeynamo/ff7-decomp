//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include <libgte.h>
#include "field_private.h"

typedef struct {
    s16 x;
    s16 y;
    u8 u;
    u8 v;
    u16 clut;
} FieldBgTile16;

typedef struct {
    FieldBgTile16 tile;
    u8 parameter;
    u8 state;
} FieldBgTile;

typedef struct {
    FieldBgTile16 tile;
    u16 tpage;
    u16 depth;
    u8 parameter;
    u8 state;
} FieldBgAnimatedTile16;

typedef struct FieldBgSourceOffsets {
    DVECTOR layer01;
    DVECTOR layer2;
    DVECTOR layer3;
} FieldBgSourceOffsets;

extern DVECTOR g_FieldPlayerScreenPos;
extern s32 D_800E48EC;
extern DRAWENV D_80113FE4[2];
extern DRAWENV D_8011409C[2];
extern DRAWENV D_80114154[2];
extern DRAWENV D_8011420C[2];
extern s16 g_PosCursorX;
extern s16 g_PosCursorY;
extern FieldBgSourceOffsets g_FieldBgSourceOffsets;
extern s16 g_CameraScrollCurrentStep;
extern s16 g_CameraScrollStartX;
extern s16 g_CameraScrollTargetX;
extern s16 g_CameraScrollStartY;
extern s16 g_CameraScrollTargetY;
extern s16 g_CameraScrollNumSteps;
extern FieldBgTile* g_FieldBgLayer3;
extern s16 g_FieldBgSpriteToDrawModeOffset;
extern s16 g_FieldBg16SpriteCount;
extern s16 g_FieldBgAnimatedDrawModeCount;

s32 FieldEntityGetDirVectorY(u8 angle);

void FieldBackgroundInitPackets(SPRT_16* sprite16, SPRT* sprite, FieldBgAnimEntry* anim, DR_MODE* mode) {
    s16* record;
    FieldBgTile16* layer1;
    FieldBgAnimatedTile16* layer2;
    u16* texturePages;
    u16 spriteCount = 0;
    u16 largeSpriteCount = 0;
    s16 count;

    g_FieldBgSpriteToDrawModeOffset = 0;
    g_FieldBgAnimatedDrawModeCount = 0;
    record = (*g_FieldTilemap)->data;
    layer1 = (FieldBgTile16*)((u8*)*g_FieldTilemap + (*g_FieldTilemap)->layer1Offset);
    texturePages = (u16*)((u8*)*g_FieldTilemap + (*g_FieldTilemap)->texturePageOffset);
    for (;; record += 3) {
        s32 type = record[0];
        if (type == 0x7FFF) {
            record++;
            break;
        }
        if (type == 0x7FFE) {
            SetDrawMode(mode, 0, 1, *texturePages++, NULL);
            g_FieldBgSpriteToDrawModeOffset++;
            mode++;
        } else {
            for (count = record[2]; count; layer1++, sprite16++, anim++, --count, spriteCount++) {
                SetSprt16(sprite16);
                SetShadeTex(sprite16, 1);
                SetSemiTrans(sprite16, 0);
                setRGB0(sprite16, 128, 128, 128);
                setXY0(sprite16, layer1->x, layer1->y);
                setUV0(sprite16, layer1->u, layer1->v);
                sprite16->clut = layer1->clut;
            }
        }
    }
    g_FieldBgSpriteToDrawModeOffset = spriteCount - g_FieldBgSpriteToDrawModeOffset;

    layer2 = (FieldBgAnimatedTile16*)((u8*)*g_FieldTilemap + (*g_FieldTilemap)->layer2Offset);
    for (;; record += 3) {
        s32 type = record[0];
        if (type == 0x7FFF) {
            record++;
            break;
        }
        for (count = record[2]; count; --count, layer2++, sprite16++, anim++, spriteCount++) {
            SetDrawMode(mode, 0, 1, layer2->tpage, NULL);
            g_FieldBgAnimatedDrawModeCount++;
            SetSprt16(sprite16);
            SetShadeTex(sprite16, 1);
            mode++;
            if (!(layer2->parameter & 0x80)) {
                SetSemiTrans(sprite16, 0);
            } else {
                SetSemiTrans(sprite16, 1);
            }
            setRGB0(sprite16, layer2->depth, layer2->depth >> 8, 128);
            setXY0(sprite16, layer2->tile.x, layer2->tile.y);
            setUV0(sprite16, layer2->tile.u, layer2->tile.v);
            sprite16->clut = layer2->tile.clut;
            anim->parameter = layer2->parameter;
            anim->state = layer2->state;
        }
    }
    g_FieldBg16SpriteCount = spriteCount;
    g_FieldBgLayer3 = (FieldBgTile*)((u8*)*g_FieldTilemap + (*g_FieldTilemap)->layer3Offset);

    for (;; record += 3) {
        s32 type = record[0];
        if (type == 0x7FFF) {
            record++;
            break;
        }
        if (type == 0x7FFE) {
            SetDrawMode(mode, 0, 1, *texturePages, NULL);
            texturePages++;
            mode++;
        } else {
            record[1] = largeSpriteCount;
            for (count = record[2]; count; sprite++, --count, g_FieldBgLayer3++, anim++, largeSpriteCount++) {
                SetSprt(sprite);
                SetShadeTex(sprite, 1);
                if (!(g_FieldBgLayer3->parameter & 0x80)) {
                    SetSemiTrans(sprite, 0);
                } else {
                    SetSemiTrans(sprite, 1);
                }
                setRGB0(sprite, 128, 128, 128);
                setXY0(sprite, g_FieldBgLayer3->tile.x, g_FieldBgLayer3->tile.y);
                setUV0(sprite, g_FieldBgLayer3->tile.u, g_FieldBgLayer3->tile.v);
                setWH(sprite, 32, 32);
                sprite->clut = g_FieldBgLayer3->tile.clut;
                anim->parameter = g_FieldBgLayer3->parameter;
                anim->state = g_FieldBgLayer3->state;
            }
        }
    }
    for (;; record += 3) {
        s32 type = record[0];
        if (type == 0x7FFF) {
            record++;
            break;
        }
        if (type == 0x7FFE) {
            SetDrawMode(mode, 0, 1, *texturePages, NULL);
            texturePages++;
            mode++;
        } else {
            record[1] = largeSpriteCount;
            for (count = record[2]; count; sprite++, --count, g_FieldBgLayer3++, anim++, largeSpriteCount++) {
                SetSprt(sprite);
                SetShadeTex(sprite, 1);
                if (!(g_FieldBgLayer3->parameter & 0x80)) {
                    SetSemiTrans(sprite, 0);
                } else {
                    SetSemiTrans(sprite, 1);
                }
                setRGB0(sprite, 128, 128, 128);
                setXY0(sprite, g_FieldBgLayer3->tile.x, g_FieldBgLayer3->tile.y);
                setUV0(sprite, g_FieldBgLayer3->tile.u, g_FieldBgLayer3->tile.v);
                setWH(sprite, 32, 32);
                sprite->clut = g_FieldBgLayer3->tile.clut;
                anim->parameter = g_FieldBgLayer3->parameter;
                anim->state = g_FieldBgLayer3->state;
            }
        }
    }
}

void AddBackgroundToRender(FieldRenderData* renderData) {
    s16 index;
    s16 count;
    s32 group;
    s32 depth;
    s16* data;

    data = (*g_FieldTilemap)->data;

    while (1) {
        s32 type = data[0];
        if (type == 0x7FFF) {
            data++;
            break;
        }
        if (type == 0x7FFE) {
            addPrim(&renderData->ot[4095], &renderData->bgDm[data[1]]);
        } else if (g_FieldBgSourceOffsets.layer01.vy - 256 < type && type < g_FieldBgSourceOffsets.layer01.vy) {
            for (index = data[1], count = data[2]; count != 0; index++, count--) {
                if (g_FieldBgSourceOffsets.layer01.vx - 336 < renderData->bg1[index].x0 &&
                    renderData->bg1[index].x0 < g_FieldBgSourceOffsets.layer01.vx) {
                    addPrim(&renderData->ot[4095], &renderData->bg1[index]);
                }
            }
        }
        data += 3;
    }

    while (1) {
        s32 type = data[0];
        if (type == 0x7FFF) {
            data++;
            break;
        }
        if (g_FieldBgSourceOffsets.layer01.vy - 256 < type && type < g_FieldBgSourceOffsets.layer01.vy) {
            for (index = data[1], count = data[2]; count != 0; index++, count--) {
                if (g_FieldBgSourceOffsets.layer01.vx - 336 < renderData->bg1[index].x0 &&
                    renderData->bg1[index].x0 < g_FieldBgSourceOffsets.layer01.vx) {
                    group = renderData->bgAnim[index].parameter & 0x3F;
                    if (!group) {
                        depth = renderData->bg1[index].r0 + (renderData->bg1[index].g0 << 8);
                        addPrim(&renderData->ot[depth], &renderData->bg1[index]);
                        addPrim(&renderData->ot[depth], &renderData->bgDm[index - g_FieldBgSpriteToDrawModeOffset]);
                    } else {
                        s32 state = renderData->bgAnim[index].state;
                        if (g_FieldState.backgroundLayerVisibility[group] & state) {
                            depth = renderData->bg1[index].r0 + (renderData->bg1[index].g0 << 8);
                            addPrim(&renderData->ot[depth], &renderData->bg1[index]);
                            addPrim(&renderData->ot[depth], &renderData->bgDm[index - g_FieldBgSpriteToDrawModeOffset]);
                        }
                    }
                }
            }
        }
        data += 3;
    }

    addPrim(&renderData->ot[g_FieldState.layer2Depth], &renderData->bgDrenv3E);
    while (1) {
        s32 type = data[0];
        if (type == 0x7FFF) {
            addPrim(&renderData->ot[g_FieldState.layer2Depth], &renderData->bgDrenv3S);
            data++;
            break;
        }
        if (type == 0x7FFE) {
            addPrim(
                &renderData->ot[g_FieldState.layer2Depth], &renderData->bgDm[data[1] + g_FieldBgAnimatedDrawModeCount]);
        } else {
            for (index = data[1], count = data[2]; count != 0; index++, count--) {
                if (renderData->bg2[index].x0 <= g_FieldBgSourceOffsets.layer2.vx - 352 ||
                    renderData->bg2[index].x0 >= g_FieldBgSourceOffsets.layer2.vx) {
                    if (renderData->bg2[index].x0 < g_FieldBgSourceOffsets.layer2.vx - 160) {
                        renderData->bg2[index].x0 += g_FieldTriggers->layer2AnimWidth;
                    } else {
                        renderData->bg2[index].x0 -= g_FieldTriggers->layer2AnimWidth;
                    }
                }
                if (renderData->bg2[index].y0 <= g_FieldBgSourceOffsets.layer2.vy - 256 ||
                    renderData->bg2[index].y0 >= g_FieldBgSourceOffsets.layer2.vy) {
                    if (renderData->bg2[index].y0 < g_FieldBgSourceOffsets.layer2.vy - 112) {
                        renderData->bg2[index].y0 += g_FieldTriggers->layer2AnimHeight;
                    } else {
                        renderData->bg2[index].y0 -= g_FieldTriggers->layer2AnimHeight;
                    }
                }
                group = renderData->bgAnim[index + g_FieldBg16SpriteCount].parameter & 0x3F;
                if (!group) {
                    addPrim(&renderData->ot[g_FieldState.layer2Depth], &renderData->bg2[index]);
                } else {
                    s32 state = renderData->bgAnim[index + g_FieldBg16SpriteCount].state;
                    if (g_FieldState.backgroundLayerVisibility[group] & state) {
                        addPrim(&renderData->ot[g_FieldState.layer2Depth], &renderData->bg2[index]);
                    }
                }
            }
        }
        data += 3;
    }

    addPrim(&renderData->ot[g_FieldState.layer3Depth], &renderData->bgDrenv4E);
    while (1) {
        s32 type = data[0];
        if (type == 0x7FFF) {
            addPrim(&renderData->ot[g_FieldState.layer3Depth], &renderData->bgDrenv4S);
            break;
        }
        if (type == 0x7FFE) {
            addPrim(
                &renderData->ot[g_FieldState.layer3Depth], &renderData->bgDm[data[1] + g_FieldBgAnimatedDrawModeCount]);
        } else {
            for (index = data[1], count = data[2]; count != 0; index++, count--) {
                if (renderData->bg2[index].x0 <= g_FieldBgSourceOffsets.layer3.vx - 352 ||
                    renderData->bg2[index].x0 >= g_FieldBgSourceOffsets.layer3.vx) {
                    if (renderData->bg2[index].x0 < g_FieldBgSourceOffsets.layer3.vx - 160) {
                        renderData->bg2[index].x0 += g_FieldTriggers->layer3AnimWidth;
                    } else {
                        renderData->bg2[index].x0 -= g_FieldTriggers->layer3AnimWidth;
                    }
                }
                if (renderData->bg2[index].y0 <= g_FieldBgSourceOffsets.layer3.vy - 256 ||
                    renderData->bg2[index].y0 >= g_FieldBgSourceOffsets.layer3.vy) {
                    if (renderData->bg2[index].y0 < g_FieldBgSourceOffsets.layer3.vy - 112) {
                        renderData->bg2[index].y0 += g_FieldTriggers->layer3AnimHeight;
                    } else {
                        renderData->bg2[index].y0 -= g_FieldTriggers->layer3AnimHeight;
                    }
                }
                if (g_FieldBgSourceOffsets.layer3.vx - 352 < renderData->bg2[index].x0 &&
                    renderData->bg2[index].x0 < g_FieldBgSourceOffsets.layer3.vx) {
                    group = renderData->bgAnim[index + g_FieldBg16SpriteCount].parameter & 0x3F;
                    if (!group) {
                        addPrim(&renderData->ot[g_FieldState.layer3Depth], &renderData->bg2[index]);
                    } else {
                        s32 state = renderData->bgAnim[index + g_FieldBg16SpriteCount].state;
                        if (g_FieldState.backgroundLayerVisibility[group] & state) {
                            addPrim(&renderData->ot[g_FieldState.layer3Depth], &renderData->bg2[index]);
                        }
                    }
                }
            }
        }
        data += 3;
    }
}

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
    int flag;
    int depth;
    s32 ret;

    PushMatrix();
    SetRotMatrix(&g_FieldCamera->lookAt);
    SetTransMatrix(&g_FieldCamera->lookAt);
    SetGeomOffset(0, 0);
    ret = RotTransPers(worldPos, (int*)screenPos, &flag, &depth);
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

void FieldBGUpdateDrawenv(FieldRenderData* renderData) {
    SVECTOR pos;
    SVECTOR screenPos;
    s16 layer2ParallaxScrollX;
    s16 layer2ParallaxScrollY;
    s16 layer3ParallaxScrollX;
    s16 layer3ParallaxScrollY;

    g_FieldTriggers->layer2ScrollPhaseX += g_FieldState.layer2ScrollXSpeed;
    g_FieldTriggers->layer2ScrollPhaseY += g_FieldState.layer2ScrollYSpeed;
    g_FieldTriggers->layer2ScrollPhaseX %= g_FieldTriggers->layer2AnimWidth * 16;
    g_FieldTriggers->layer2ScrollPhaseY %= g_FieldTriggers->layer2AnimHeight * 16;
    g_FieldTriggers->layer3ScrollPhaseX += g_FieldState.layer3ScrollXSpeed;
    g_FieldTriggers->layer3ScrollPhaseY += g_FieldState.layer3ScrollYSpeed;
    g_FieldTriggers->layer3ScrollPhaseX %= g_FieldTriggers->layer3AnimWidth * 16;
    g_FieldTriggers->layer3ScrollPhaseY %= g_FieldTriggers->layer3AnimHeight * 16;
    SetGeomScreen(g_FieldCamera->zoom);
    if (!g_FieldMovieJustStarted || g_FieldState.backgroundMovieEnabled) {
        if (!g_CameraScrollEnabled) {
            switch (g_FieldState.viewOffsetMode) {
            case OMODE_INSTANT:
                break;
            case OMODE_LINEAR:
                g_FieldState.viewOffset = (u8)FieldCalcLinearStep(
                    g_FieldState.viewOffsetStart, g_FieldState.viewOffsetTarget, g_FieldState.viewOffsetNumSteps,
                    g_FieldState.viewOffsetCurrentStep);
                if (g_FieldState.viewOffsetCurrentStep == g_FieldState.viewOffsetNumSteps) {
                    g_FieldState.viewOffsetMode = OMODE_DONE;
                } else {
                    g_FieldState.viewOffsetCurrentStep++;
                }
                break;
            case OMODE_SMOOTH:
                g_FieldState.viewOffset = (u8)FieldCalcEaseInOut(
                    g_FieldState.viewOffsetStart, g_FieldState.viewOffsetTarget, g_FieldState.viewOffsetNumSteps,
                    g_FieldState.viewOffsetCurrentStep);
                if (g_FieldState.viewOffsetCurrentStep == g_FieldState.viewOffsetNumSteps) {
                    g_FieldState.viewOffsetMode = OMODE_DONE;
                } else {
                    g_FieldState.viewOffsetCurrentStep++;
                }
                break;
            }
            pos.vx = (g_FieldEntity[g_PlayerModelId].PosX >> 12) + g_FieldEntity[g_PlayerModelId].OffsetX;
            pos.vy = (g_FieldEntity[g_PlayerModelId].PosY >> 12) + g_FieldEntity[g_PlayerModelId].OffsetY;
            pos.vz = (g_FieldEntity[g_PlayerModelId].PosZ >> 12) + g_FieldEntity[g_PlayerModelId].OffsetZ +
                     ((g_FieldState.currentFieldScale * 128) >> 9);
            FieldCalcWorldToScreenPos(&pos, &screenPos);
            g_PosCursorX = screenPos.vx + g_CameraCenterX;
            g_PosCursorY = screenPos.vy + g_CameraCenterY;
            pos.vx = g_FieldEntity[g_PlayerModelId].PosX >> 12;
            pos.vy = g_FieldEntity[g_PlayerModelId].PosY >> 12;
            pos.vz = (g_FieldEntity[g_PlayerModelId].PosZ >> 12) + g_FieldState.viewOffset;
            D_800E48EC = FieldCalcWorldToScreenPos(&pos, &screenPos);
            g_FieldPlayerScreenPos.vx = screenPos.vx;
            g_FieldPlayerScreenPos.vy = screenPos.vy;
            FieldBGClampPos(&screenPos);
            FieldCalcPointOnLine(g_FieldTriggers, &screenPos);
            g_PosCursorX -= screenPos.vx;
            g_PosCursorY -= screenPos.vy;
            layer2ParallaxScrollX = ((screenPos.vx * g_FieldTriggers->layer2ParallaxFactorX) >> 8) +
                                    (g_FieldTriggers->layer2ScrollPhaseX >> 4);
            layer2ParallaxScrollY = ((screenPos.vy * g_FieldTriggers->layer2ParallaxFactorY) >> 8) +
                                    (g_FieldTriggers->layer2ScrollPhaseY >> 4);
            layer2ParallaxScrollX %= g_FieldTriggers->layer2AnimWidth;
            layer2ParallaxScrollY %= g_FieldTriggers->layer2AnimHeight;
            layer3ParallaxScrollX = ((screenPos.vx * g_FieldTriggers->layer3ParallaxFactorX) >> 8) +
                                    (g_FieldTriggers->layer3ScrollPhaseX >> 4);
            layer3ParallaxScrollY = ((screenPos.vy * g_FieldTriggers->layer3ParallaxFactorY) >> 8) +
                                    (g_FieldTriggers->layer3ScrollPhaseY >> 4);
            layer3ParallaxScrollX %= g_FieldTriggers->layer3AnimWidth;
            layer3ParallaxScrollY %= g_FieldTriggers->layer3AnimHeight;
            if (renderData == g_FieldRenderData) {
                D_80113F2C[0].ofs[0] = g_CameraCenterX - screenPos.vx + g_FieldState.shakeX.currentOffset;
                D_80113F2C[0].ofs[1] = g_CameraCenterY - screenPos.vy + g_FieldState.shakeY.currentOffset;
                SetDrawEnv(&renderData->sceneDrenv, &D_80113F2C[0]);
                D_80114154[0].ofs[0] = g_CameraCenterX - screenPos.vx + g_FieldState.shakeX.currentOffset;
                D_80114154[0].ofs[1] = g_CameraCenterY - screenPos.vy + g_FieldState.shakeY.currentOffset;
                SetDrawEnv(&renderData->bgDrenv3E, &D_80114154[0]);
                D_8011420C[0].ofs[0] = g_CameraCenterX - screenPos.vx + g_FieldState.shakeX.currentOffset;
                D_8011420C[0].ofs[1] = g_CameraCenterY - screenPos.vy + g_FieldState.shakeY.currentOffset;
                SetDrawEnv(&renderData->bgDrenv4E, &D_8011420C[0]);
                D_80113FE4[0].ofs[0] = g_CameraCenterX - layer2ParallaxScrollX + g_FieldState.shakeX.currentOffset;
                D_80113FE4[0].ofs[1] = g_CameraCenterY - layer2ParallaxScrollY + g_FieldState.shakeY.currentOffset;
                SetDrawEnv(&renderData->bgDrenv3S, &D_80113FE4[0]);
                D_8011409C[0].ofs[0] = g_CameraCenterX - layer3ParallaxScrollX + g_FieldState.shakeX.currentOffset;
                D_8011409C[0].ofs[1] = g_CameraCenterY - layer3ParallaxScrollY + g_FieldState.shakeY.currentOffset;
                SetDrawEnv(&renderData->bgDrenv4S, &D_8011409C[0]);
            } else {
                D_80113F2C[1].ofs[0] = g_CameraCenterX - screenPos.vx + g_FieldState.shakeX.currentOffset;
                D_80113F2C[1].ofs[1] = g_CameraCenterY - screenPos.vy + g_FieldState.shakeY.currentOffset + 232;
                SetDrawEnv(&g_FieldRenderData[1].sceneDrenv, &D_80113F2C[1]);
                D_80114154[1].ofs[0] = g_CameraCenterX - screenPos.vx + g_FieldState.shakeX.currentOffset;
                D_80114154[1].ofs[1] = g_CameraCenterY - screenPos.vy + g_FieldState.shakeY.currentOffset + 232;
                SetDrawEnv(&g_FieldRenderData[1].bgDrenv3E, &D_80114154[1]);
                D_8011420C[1].ofs[0] = g_CameraCenterX - screenPos.vx + g_FieldState.shakeX.currentOffset;
                D_8011420C[1].ofs[1] = g_CameraCenterY - screenPos.vy + g_FieldState.shakeY.currentOffset + 232;
                SetDrawEnv(&g_FieldRenderData[1].bgDrenv4E, &D_8011420C[1]);
                D_80113FE4[1].ofs[0] = g_CameraCenterX - layer2ParallaxScrollX + g_FieldState.shakeX.currentOffset;
                D_80113FE4[1].ofs[1] =
                    g_CameraCenterY - layer2ParallaxScrollY + g_FieldState.shakeY.currentOffset + 232;
                SetDrawEnv(&g_FieldRenderData[1].bgDrenv3S, &D_80113FE4[1]);
                D_8011409C[1].ofs[0] = g_CameraCenterX - layer3ParallaxScrollX + g_FieldState.shakeX.currentOffset;
                D_8011409C[1].ofs[1] =
                    g_CameraCenterY - layer3ParallaxScrollY + g_FieldState.shakeY.currentOffset + 232;
                SetDrawEnv(&g_FieldRenderData[1].bgDrenv4S, &D_8011409C[1]);
            }
            g_CameraScrollX = -screenPos.vx;
            g_CameraScrollY = -screenPos.vy;
            g_FieldBgSourceOffsets.layer01.vx =
                screenPos.vx + 320 - g_CameraCenterX - g_FieldState.shakeX.currentOffset;
            g_FieldBgSourceOffsets.layer01.vy =
                screenPos.vy + 232 - g_CameraCenterY - g_FieldState.shakeY.currentOffset;
            g_FieldBgSourceOffsets.layer2.vx =
                layer2ParallaxScrollX + 320 - g_CameraCenterX - g_FieldState.shakeX.currentOffset;
            g_FieldBgSourceOffsets.layer2.vy =
                layer2ParallaxScrollY + 232 - g_CameraCenterY - g_FieldState.shakeY.currentOffset;
            g_FieldBgSourceOffsets.layer3.vx =
                layer3ParallaxScrollX + 320 - g_CameraCenterX - g_FieldState.shakeX.currentOffset;
            g_FieldBgSourceOffsets.layer3.vy =
                layer3ParallaxScrollY + 232 - g_CameraCenterY - g_FieldState.shakeY.currentOffset;
            return;
        }
        layer2ParallaxScrollX = ((-g_CameraScrollX * g_FieldTriggers->layer2ParallaxFactorX) >> 8) +
                                (g_FieldTriggers->layer2ScrollPhaseX >> 4);
        layer2ParallaxScrollY = ((-g_CameraScrollY * g_FieldTriggers->layer2ParallaxFactorY) >> 8) +
                                (g_FieldTriggers->layer2ScrollPhaseY >> 4);
        layer2ParallaxScrollX %= g_FieldTriggers->layer2AnimWidth;
        layer2ParallaxScrollY %= g_FieldTriggers->layer2AnimHeight;
        layer3ParallaxScrollX = ((-g_CameraScrollX * g_FieldTriggers->layer3ParallaxFactorX) >> 8) +
                                (g_FieldTriggers->layer3ScrollPhaseX >> 4);
        layer3ParallaxScrollY = ((-g_CameraScrollY * g_FieldTriggers->layer3ParallaxFactorY) >> 8) +
                                (g_FieldTriggers->layer3ScrollPhaseY >> 4);
        layer3ParallaxScrollX %= g_FieldTriggers->layer3AnimWidth;
        layer3ParallaxScrollY %= g_FieldTriggers->layer3AnimHeight;
        if (renderData == g_FieldRenderData) {
            D_80113F2C[0].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx + g_CameraScrollX + g_FieldState.shakeX.currentOffset;
            D_80113F2C[0].ofs[1] =
                g_CameraCenterY + g_FieldCamera->offset.vy + g_CameraScrollY + g_FieldState.shakeY.currentOffset;
            SetDrawEnv(&renderData->sceneDrenv, &D_80113F2C[0]);
            D_80114154[0].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx + g_CameraScrollX + g_FieldState.shakeX.currentOffset;
            D_80114154[0].ofs[1] =
                g_CameraCenterY + g_FieldCamera->offset.vy + g_CameraScrollY + g_FieldState.shakeY.currentOffset;
            SetDrawEnv(&renderData->bgDrenv3E, &D_80114154[0]);
            D_8011420C[0].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx + g_CameraScrollX + g_FieldState.shakeX.currentOffset;
            D_8011420C[0].ofs[1] =
                g_CameraCenterY + g_FieldCamera->offset.vy + g_CameraScrollY + g_FieldState.shakeY.currentOffset;
            SetDrawEnv(&renderData->bgDrenv4E, &D_8011420C[0]);
            D_80113FE4[0].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx - layer2ParallaxScrollX + g_FieldState.shakeX.currentOffset;
            D_80113FE4[0].ofs[1] =
                g_CameraCenterY + g_FieldCamera->offset.vy - layer2ParallaxScrollY + g_FieldState.shakeY.currentOffset;
            SetDrawEnv(&renderData->bgDrenv3S, &D_80113FE4[0]);
            D_8011409C[0].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx - layer3ParallaxScrollX + g_FieldState.shakeX.currentOffset;
            D_8011409C[0].ofs[1] =
                g_CameraCenterY + g_FieldCamera->offset.vy - layer3ParallaxScrollY + g_FieldState.shakeY.currentOffset;
            SetDrawEnv(&renderData->bgDrenv4S, &D_8011409C[0]);
        } else {
            D_80113F2C[1].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx + g_CameraScrollX + g_FieldState.shakeX.currentOffset;
            D_80113F2C[1].ofs[1] =
                g_CameraCenterY + g_FieldCamera->offset.vy + g_CameraScrollY + g_FieldState.shakeY.currentOffset + 232;
            SetDrawEnv(&g_FieldRenderData[1].sceneDrenv, &D_80113F2C[1]);
            D_80114154[1].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx + g_CameraScrollX + g_FieldState.shakeX.currentOffset;
            D_80114154[1].ofs[1] =
                g_CameraCenterY + g_FieldCamera->offset.vy + g_CameraScrollY + g_FieldState.shakeY.currentOffset + 232;
            SetDrawEnv(&g_FieldRenderData[1].bgDrenv3E, &D_80114154[1]);
            D_8011420C[1].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx + g_CameraScrollX + g_FieldState.shakeX.currentOffset;
            D_8011420C[1].ofs[1] =
                g_CameraCenterY + g_FieldCamera->offset.vy + g_CameraScrollY + g_FieldState.shakeY.currentOffset + 232;
            SetDrawEnv(&g_FieldRenderData[1].bgDrenv4E, &D_8011420C[1]);
            D_80113FE4[1].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx - layer2ParallaxScrollX + g_FieldState.shakeX.currentOffset;
            D_80113FE4[1].ofs[1] = g_CameraCenterY + g_FieldCamera->offset.vy - layer2ParallaxScrollY +
                                   g_FieldState.shakeY.currentOffset + 232;
            SetDrawEnv(&g_FieldRenderData[1].bgDrenv3S, &D_80113FE4[1]);
            D_8011409C[1].ofs[0] =
                g_CameraCenterX - g_FieldCamera->offset.vx - layer3ParallaxScrollX + g_FieldState.shakeX.currentOffset;
            D_8011409C[1].ofs[1] = g_CameraCenterY + g_FieldCamera->offset.vy - layer3ParallaxScrollY +
                                   g_FieldState.shakeY.currentOffset + 232;
            SetDrawEnv(&g_FieldRenderData[1].bgDrenv4S, &D_8011409C[1]);
        }
        g_FieldBgSourceOffsets.layer01.vx = 320 - g_CameraScrollX - g_CameraCenterX - g_FieldState.shakeX.currentOffset;
        g_FieldBgSourceOffsets.layer01.vy = 232 - g_CameraScrollY - g_CameraCenterY - g_FieldState.shakeY.currentOffset;
        g_FieldBgSourceOffsets.layer2.vx =
            layer2ParallaxScrollX + 320 - g_CameraCenterX - g_FieldState.shakeX.currentOffset;
        g_FieldBgSourceOffsets.layer2.vy =
            layer2ParallaxScrollY + 232 - g_CameraCenterY - g_FieldState.shakeY.currentOffset;
        g_FieldBgSourceOffsets.layer3.vx =
            layer3ParallaxScrollX + 320 - g_CameraCenterX - g_FieldState.shakeX.currentOffset;
        g_FieldBgSourceOffsets.layer3.vy =
            layer3ParallaxScrollY + 232 - g_CameraCenterY - g_FieldState.shakeY.currentOffset;
        return;
    }
    if (g_FieldState.movieCamDisabled == 1) {
        if (renderData == g_FieldRenderData) {
            D_80113F2C[0].ofs[0] = g_CameraCenterX + g_CameraScrollX;
            D_80113F2C[0].ofs[1] = g_CameraCenterY + g_CameraScrollY;
            SetDrawEnv(&renderData->sceneDrenv, &D_80113F2C[0]);
        } else {
            D_80113F2C[1].ofs[0] = g_CameraCenterX + g_CameraScrollX;
            D_80113F2C[1].ofs[1] = g_CameraCenterY + g_CameraScrollY + 232;
            SetDrawEnv(&g_FieldRenderData[1].sceneDrenv, &D_80113F2C[1]);
        }
    } else {
        if (renderData == g_FieldRenderData) {
            D_80113F2C[0].ofs[0] = g_CameraCenterX - g_FieldCamera->offset.vx;
            D_80113F2C[0].ofs[1] = g_CameraCenterY + g_FieldCamera->offset.vy;
            SetDrawEnv(&renderData->sceneDrenv, &D_80113F2C[0]);
        } else {
            D_80113F2C[1].ofs[0] = g_CameraCenterX - g_FieldCamera->offset.vx;
            D_80113F2C[1].ofs[1] = g_CameraCenterY + g_FieldCamera->offset.vy + 232;
            SetDrawEnv(&g_FieldRenderData[1].sceneDrenv, &D_80113F2C[1]);
        }
    }
}
