//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern s16 D_800DF120[][2];
u8 FieldEntityDirByVec(VECTOR* start, VECTOR* target, s32* distance);

static s16 D_800DEF88[130] = {
    0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,
    8,  9,  9,  9,  10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17,
    17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26,
    27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
};

INCLUDE_ASM("asm/us/field/nonmatchings/field_entity", FieldEntityInitPos);

void FieldEntityAddRotate(u32 activeInputs, s16 modelId) {
    if (!g_FieldState.characterLock) {
        if (g_FieldState.activeKeys & PADR1) {
            g_FieldEntity[modelId].MoveDirAdd = 224;
        } else if (g_FieldState.activeKeys & PADL1) {
            g_FieldEntity[modelId].MoveDirAdd = 32;
        } else {
            g_FieldEntity[modelId].MoveDirAdd = 0;
        }
    }
}

void FieldEntityAnimationUpdate(s32 modelId) {
    u8 modelEntryId;
    FieldModelEntry* model;
    u8* anims;

    modelEntryId = g_FieldModelLoaderData[modelId].modelEntryIndex;
    if (modelEntryId != 0xFF) {
        model = &g_FieldModelData->modelEntries[modelEntryId];
        anims = model->modelData + model->animationOffset;
        if (!g_FieldState.suspendWalkAndAnim) {
            g_FieldEntity[modelId].animCurrentFrame += g_FieldEntity[modelId].animSpeed;
            if (modelId == g_PlayerModelId && !g_FieldState.characterLock) {
                g_FieldEntity[modelId].animLastFrame = *(u16*)&anims[g_FieldEntity[modelId].activeAnimId * 16] - 1;
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_entity", FieldEntityMovementUpdate);

void FieldEntityGatewayMapLoad(FieldGateway* gateway) {
    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
    g_FieldState.eventCmdParam = gateway->fieldId;
    g_FieldState.pcPosX = gateway->destFieldPos.vx;
    g_FieldState.pcPosY = gateway->destFieldPos.vy;
    g_FieldState.pcWalkMeshId = gateway->pcWalkMeshTriangleId;
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

    if ((g_FieldState.activeKeys & PADRright) && !(g_FieldState.activeKeysPrev & PADRright)) {
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
            /* Locals preserve GCC 2.6.3's constant-load and delay-slot ordering. */
            s32 limit = 64;
            if (bestDiff != limit) {
                u8 request = 1;
                g_FieldEntity[modelId].requestTalkScript = request;
            }
        }
    }
}

static s16 FieldEntityGetDirVectorX(u8 arg0) { return D_800DF120[arg0][0]; }

s32 FieldEntityGetDirVectorY(u8 arg0) { return D_800DF120[arg0][1]; }

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

INCLUDE_ASM("asm/us/field/nonmatchings/field_entity", FieldEntityWalkmechCross);

static void FieldEntityVectorSub(s32* arg0, s16* arg1, s16* arg2) {
    arg0[0] = arg1[0] - arg2[0];
    arg0[1] = arg1[1] - arg2[1];
    arg0[2] = arg1[2] - arg2[2];
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_entity", FieldEntityMove);

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
                    lines->requestTouchOnScript = 1;
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
                    lines->requestTouchOffScript = 1;
                }
                lines->touch = 0;
            }
        }
    }
    return result;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_entity", FieldEntityLineInteract);

void FieldEntityLineClear(FieldLine* lines) {
    s32 i;

    for (i = 0; i < LEN(g_FieldLines); i++) {
        lines->isOnLine = 0;
        lines++;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_entity", FieldEntityGatewayCheck);

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

const u16 D_800A00BC[4] = {0, 54, 122, 298};
INCLUDE_ASM("asm/us/field/nonmatchings/field_entity", FieldEntityTriggerCheck);

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
