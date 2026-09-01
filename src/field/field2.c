//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern FieldFileInfo g_FieldFileInfo[];
extern void SystemLzsDecompress(void* dst, void* src);
extern s32* g_FieldModelsP;
extern s32 g_FieldTriggers;
extern s32 g_FieldEncounters;
extern s32 D_8007E770;
extern s16 g_CurrentFieldIndex;
extern s32* g_FieldTriggersP;
extern s32* g_FieldEncountersP;
extern u32 g_FieldLzsInfo[];

/* .rodata 0x800A0054-0x800A00E0, a base of 4 mod 8. Split out of field.c so
 * that the four jump tables in that range -- all at addresses 4 mod 8 -- keep
 * the offsets `.align 3` gives them. Merging this unit into either neighbour
 * moves every one of them four bytes. */

/* Seed the background-scroll state machine from the requested scroll mode.
 * Only runs while idle (g_FieldStateData.cameraScrollState == 0). Modes: 0
 * stops and recentres; 1 arms scrolling in place; 2/3 begin a single-target
 * scroll; 4 teleports the current position to the alt source; 5-9 begin a
 * dual-target (eased) scroll. The target positions/step/fraction are what
 * FieldBGScrollUpdate consumes each frame. */
extern s16 D_8009A100;           // scroll enable
extern s16 D_80071E38;           // current scroll X
extern s16 D_80071E3C;           // current scroll Y
extern s16 g_CameraScrollSteps;  // total steps of the active scroll
extern s16 g_CameraScrollStep;   // current step, 0..g_CameraScrollSteps
extern s16 g_CameraScrollStartX; // where the scroll began
extern s16 g_CameraScrollStartY; // where the scroll began
extern s16 g_CameraScrollEndX;   // copied from cameraScrollTargetX
extern s16 g_CameraScrollEndY;   // copied from cameraScrollTargetY

void FieldBGScrollInit(void) {
    if (g_FieldStateData.cameraScrollState != 0) {
        return;
    }
    switch (g_FieldStateData.cameraScrollMode) {
    case 0:
        D_8009A100 = 0;
        D_80071E38 = 0;
        D_80071E3C = 0;
        g_FieldStateData.cameraScrollState = 2;
        break;
    case 1:
        D_8009A100 = 1;
        g_FieldStateData.cameraScrollState = 1;
        break;
    case 2:
    case 3:
        D_8009A100 = 1;
        g_CameraScrollStep = 0;
        g_FieldStateData.cameraScrollState = 1;
        g_CameraScrollSteps = g_FieldStateData.cameraScrollNumSteps;
        g_CameraScrollStartX = D_80071E38;
        g_CameraScrollStartY = D_80071E3C;
        break;
    case 4:
        D_8009A100 = 1;
        g_FieldStateData.cameraScrollState = 2;
        D_80071E38 = g_FieldStateData.cameraScrollTargetX;
        D_80071E3C = g_FieldStateData.cameraScrollTargetY;
        break;
    case 5:
    case 6:
        D_8009A100 = 1;
        g_CameraScrollStep = 0;
        g_FieldStateData.cameraScrollState = 1;
        g_CameraScrollSteps = g_FieldStateData.cameraScrollNumSteps;
        g_CameraScrollStartX = D_80071E38;
        g_CameraScrollStartY = D_80071E3C;
        g_CameraScrollEndX = g_FieldStateData.cameraScrollTargetX;
        g_CameraScrollEndY = g_FieldStateData.cameraScrollTargetY;
        break;
    case 7:
    case 8:
    case 9:
        break;
    }
}

/* Rail the background scroll position onto the diagonal of the map's
 * scroll rectangle. */
INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldCalcPointOnLine);

/* Keeps a background scroll position half a screen (0xA0 x 0x78) inside the
 * map's scroll limits. */
void FieldBGClampPos(s16* pos) {
    if (FIELD_SCROLL_LIMITS->maxX - 0xA0 < pos[0]) {
        pos[0] = FIELD_SCROLL_LIMITS->maxX - 0xA0;
    }
    if (pos[0] < FIELD_SCROLL_LIMITS->minX + 0xA0) {
        pos[0] = FIELD_SCROLL_LIMITS->minX + 0xA0;
    }
    if (FIELD_SCROLL_LIMITS->maxY - 0x78 < pos[1]) {
        pos[1] = FIELD_SCROLL_LIMITS->maxY - 0x78;
    }
    if (pos[1] < FIELD_SCROLL_LIMITS->minY + 0x78) {
        pos[1] = FIELD_SCROLL_LIMITS->minY + 0x78;
    }
}

/* Project the tracked entity's world position onto the screen, lifting it by
 * the camera's height bias. */
s32 FieldBGGetEntityScreenPos(long* screenPos) {
    SVECTOR pos;
    volatile u8* tracked;

    tracked = &g_FieldBGTrackedEntity;
    pos.vx = g_FieldEntity[*tracked].PosX >> 12;
    pos.vy = g_FieldEntity[*tracked].PosY >> 12;
    pos.vz = (g_FieldEntity[*tracked].PosZ >> 12) + g_FieldBGCameraHeightBias;
    return FieldCalcWorldToScreenPos(&pos, screenPos);
}

/* Per-frame background scroll: on the field's scroll state machine, drive the
 * background X/Y toward the entity's clamped screen position (linear or
 * ease-in-out depending on the mode). */
void FieldBGScrollUpdate(void) {
    long screenPos;

#define SCREEN_X (((s16*)&screenPos)[0])
#define SCREEN_Y (((s16*)&screenPos)[1])
    if (g_FieldStateData.cameraScrollState == 1) {
        switch (g_FieldStateData.cameraScrollMode) {
        case 1:
            FieldBGGetEntityScreenPos(&screenPos);
            FieldBGClampPos((s16*)&screenPos);
            D_80071E38 = -SCREEN_X;
            D_80071E3C = -SCREEN_Y;
            break;
        case 2:
            FieldBGGetEntityScreenPos(&screenPos);
            FieldBGClampPos((s16*)&screenPos);
            D_80071E38 = FieldCalcLinearStep(
                g_CameraScrollStartX, -SCREEN_X, g_CameraScrollSteps,
                g_CameraScrollStep);
            D_80071E3C = FieldCalcLinearStep(
                g_CameraScrollStartY, -SCREEN_Y, g_CameraScrollSteps,
                g_CameraScrollStep);
            if (g_CameraScrollSteps != g_CameraScrollStep) {
                g_CameraScrollStep = g_CameraScrollStep + 1;
            } else {
                goto scrollDone;
            }
            break;
        case 3:
            FieldBGGetEntityScreenPos(&screenPos);
            FieldBGClampPos((s16*)&screenPos);
            D_80071E38 = FieldCalcEaseInOut(
                g_CameraScrollStartX, -SCREEN_X, g_CameraScrollSteps,
                g_CameraScrollStep);
            D_80071E3C = FieldCalcEaseInOut(
                g_CameraScrollStartY, -SCREEN_Y, g_CameraScrollSteps,
                g_CameraScrollStep);
            if (g_CameraScrollSteps != g_CameraScrollStep) {
                g_CameraScrollStep = g_CameraScrollStep + 1;
            } else {
                goto scrollDone;
            }
            break;
        case 5:
            D_80071E38 = FieldCalcLinearStep(
                g_CameraScrollStartX, g_CameraScrollEndX, g_CameraScrollSteps,
                g_CameraScrollStep);
            D_80071E3C = FieldCalcLinearStep(
                g_CameraScrollStartY, g_CameraScrollEndY, g_CameraScrollSteps,
                g_CameraScrollStep);
            if (g_CameraScrollSteps != g_CameraScrollStep) {
                g_CameraScrollStep = g_CameraScrollStep + 1;
            } else {
                goto scrollDone;
            }
            break;
        case 6:
            D_80071E38 = FieldCalcEaseInOut(
                g_CameraScrollStartX, g_CameraScrollEndX, g_CameraScrollSteps,
                g_CameraScrollStep);
            D_80071E3C = FieldCalcEaseInOut(
                g_CameraScrollStartY, g_CameraScrollEndY, g_CameraScrollSteps,
                g_CameraScrollStep);
            if (g_CameraScrollSteps == g_CameraScrollStep) {
            scrollDone:
                g_FieldStateData.cameraScrollState = 2;
            } else {
                g_CameraScrollStep = g_CameraScrollStep + 1;
            }
            break;
        case 0:
        case 4:
        case 7:
        case 8:
        case 9:
            break;
        }
    }
#undef SCREEN_X
#undef SCREEN_Y
}

/* Rebuild the four background layers' draw environments and scroll offsets
 * for this frame. */
INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldBGUpdateDrawenv);

/////////////////////////////////////////////////
// Begin of field_entity.c
/////////////////////////////////////////////////

/* Place the player's model when a field map starts. The walk mesh triangle the
 * player stands on comes from FieldState; when no exit position was stored
 * (pcPosX is the 0x7FFF sentinel) the model is dropped on that triangle's
 * centroid, otherwise it keeps the stored X/Y and its height is solved from
 * the triangle's plane. Interaction radius and walk speed both scale with the
 * map, and every entity's queued turn is cleared.
 *
 * D_800E4274 is the walk mesh: three vertices per triangle, each vertex three
 * s16 plus a pad word, so 12 shorts per triangle and 4 per vertex. */
void FieldEntityInitPos(void) {
    s32 edgeA[3];
    s32 edgeB[3];
    s32 point[3];
    s16 moveSpeed;
    s16 i;

    if (g_FieldAnimLock == 0) {
        g_PlayerModelId = g_FieldStateData.pcModelId;
        g_FieldEntity[g_PlayerModelId].PosI = g_FieldStateData.pcWalkMeshId;
        if (g_FieldStateData.pcPosX == 0x7FFF) {
            g_FieldEntity[g_PlayerModelId].PosX =
                ((((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     0] +
                  ((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     4] +
                  ((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     8]) /
                 3)
                << 12;
            g_FieldEntity[g_PlayerModelId].PosY =
                ((((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     1] +
                  ((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     5] +
                  ((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     9]) /
                 3)
                << 12;
            g_FieldEntity[g_PlayerModelId].PosZ =
                ((((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     2] +
                  ((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     6] +
                  ((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                     10]) /
                 3)
                << 12;
        } else {
            g_FieldEntity[g_PlayerModelId].PosX = g_FieldStateData.pcPosX << 12;
            g_FieldEntity[g_PlayerModelId].PosY = g_FieldStateData.pcPosY << 12;
            FieldEntityVectorSub(
                edgeA,
                &((s16*)
                      D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 + 4],
                &((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12]);
            FieldEntityVectorSub(
                edgeB,
                &((s16*)
                      D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 + 8],
                &((s16*)D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12 +
                                    4]);
            point[0] = g_FieldStateData.pcPosX;
            point[1] = g_FieldStateData.pcPosY;
            g_FieldEntity[g_PlayerModelId].PosZ =
                FieldEntityCalculateZ(
                    edgeA, edgeB, point,
                    &((s16*)
                          D_800E4274)[g_FieldEntity[g_PlayerModelId].PosI * 12])
                << 12;
        }
        g_FieldEntity[g_PlayerModelId].SolidRange =
            (g_FieldStateData.currentFieldScale * 0x11) >> 8;
        moveSpeed = g_FieldStateData.currentFieldScale * 2;
        g_FieldEntity[g_PlayerModelId].animSpeed = 0x10;
        g_FieldEntity[g_PlayerModelId].MoveSpeed = moveSpeed;
    }
    for (i = 0; i < g_FieldStateData.modelCount; i++) {
        g_FieldEntity[i].MoveDirAdd = 0;
    }
}

void FieldEntityAddRotate(s32 arg0, s16 entityIdx) {
    if (g_FieldAnimLock == 0) {
        if (g_FieldStateData.activeKeys & PADR1) {
            g_FieldEntity[entityIdx].MoveDirAdd = 0xE0;
        } else if (g_FieldStateData.activeKeys & PADL1) {
            g_FieldEntity[entityIdx].MoveDirAdd = 0x20;
        } else {
            g_FieldEntity[entityIdx].MoveDirAdd = 0;
        }
    }
}

/* Advance one entity's animation clock. animCurrentFrame counts in 1/16ths of
 * a frame, so the comparisons scale animLastFrame by 16. The player's own
 * model loops back to the start; every other entity holds on its last frame.
 * g_FieldAnimFreeze freezes all field animation at once. */
void FieldEntityAnimationUpdate(s32 entityId) {
    FieldModelEntry* model;
    u8* anims;
    u8 entryIndex;
    s32 unusedLocal;

    entryIndex = g_FieldModelLoaderData[entityId].modelEntryIndex;
    if (entryIndex == 0xFF) {
        return;
    }
    model = &g_FieldModelData->modelEntries[entryIndex];
    anims = model->modelData + model->animationOffset;
    if (g_FieldAnimFreeze != 0) {
        return;
    }
    g_FieldEntity[entityId].animCurrentFrame +=
        g_FieldEntity[entityId].animSpeed;
    if (entityId == g_PlayerModelId && g_FieldAnimLock == 0) {
        g_FieldEntity[entityId].animLastFrame =
            *(u16*)&anims[g_FieldEntity[entityId].activeAnimId * 16] - 1;
        if (g_FieldEntity[entityId].animLastFrame * 16 <
            g_FieldEntity[entityId].animCurrentFrame) {
            g_FieldEntity[entityId].animCurrentFrame = 0;
        }
    } else if (g_FieldEntity[entityId].animLastFrame * 16 <
               g_FieldEntity[entityId].animCurrentFrame) {
        g_FieldEntity[entityId].animCurrentFrame =
            g_FieldEntity[entityId].animLastFrame << 4;
    }
}

/* Advance every field entity one frame: turn, offset, walk, jump and the
 * player's own movement. */
INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldEntityMovementUpdate);

void FieldEntityGatewayMapLoad(FieldGateway* gateway) {
    g_FieldStateData.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
    g_FieldStateData.eventCmdParam = gateway->destFieldId;
    g_FieldStateData.pcPosX = gateway->destPosX;
    g_FieldStateData.pcPosY = gateway->destPosY;
    g_FieldStateData.pcWalkMeshId = gateway->destWalkMeshId;
    *(u16*)&g_FieldStateData.pcDirection = gateway->destDirection;
}

/* Per-frame talk scan: on the rising edge of the OK button, score every entity
 * by how directly the player faces it (and how near), then request the talk
 * script of the best candidate. */
s32 FieldEntityCheckTalk(void) {
    VECTOR from;
    VECTOR to;
    s16 quality[16];
    s32 sqrDist;
    s16 bestId;
    s16 best;
    u8 dirTo;
    s32 i;
    s16 talk;

    if (!(g_FieldPad2State & 0x20) || (g_FieldPad2PrevState & 0x20)) {
        return;
    }
    from.vx = g_FieldEntity[g_PlayerModelId].PosX >> 12;
    from.vy = g_FieldEntity[g_PlayerModelId].PosY >> 12;
    from.vz = g_FieldEntity[g_PlayerModelId].PosZ >> 12;
    for (i = 0; i < D_8009AC1C; i++) {
        quality[i] = 0x100;
        if (i == g_PlayerModelId) {
            continue;
        }
        if (g_FieldEntity[i].TalkOff != 0) {
            continue;
        }
        to.vx = g_FieldEntity[i].PosX >> 12;
        to.vy = g_FieldEntity[i].PosY >> 12;
        to.vz = g_FieldEntity[i].PosZ >> 12;
        if (from.vx == to.vx && from.vy == to.vy) {
            continue;
        }
        if ((u32)(from.vz - to.vz + 0xFF) >= 0x1FF) {
            continue;
        }
        dirTo = FieldEntityDirByVec(&from, &to, &sqrDist);
        quality[i] = (g_FieldEntity[g_PlayerModelId].Dir - dirTo) & 0xFF;
        if (quality[i] >= 0x81) {
            quality[i] = 0x100 - quality[i];
        }
        if (sqrDist >= g_FieldEntity[i].TalkRange +
                           g_FieldEntity[g_PlayerModelId].SolidRange) {
            quality[i] = 0x100;
        }
    }
    best = 0x40;
    bestId = g_PlayerModelId;
    for (i = 0; i < D_8009AC1C; i++) {
        if (quality[i] < best) {
            best = quality[i];
            bestId = i;
        }
    }
    if (bestId != g_PlayerModelId && best != 0x40) {
        talk = 1;
        g_FieldEntity[bestId].requestTalkScript = talk;
    }
}

s16 FieldEntityGetDirVectorX(u8 arg0) { return D_800DF120[arg0][0]; }

s16 FieldEntityGetDirVectorY(u8 arg0) { return D_800DF120[arg0][1]; }

/* Direction (0-255) from one point to another. The third parameter is
 * in/out: it is written with the squared distance, then *overwritten with the
 * distance itself* -- callers compare it against a plain range, not a squared
 * one. The slope of each axis is taken in 12-bit fixed point, divided down by
 * 32, and the arctan table g_FieldAtanTable is indexed by whichever axis is the
 * minor one; the eight-arm ladder is the quadrant correction and every arm
 * shares one final `+ 0x40` and one `& 0xFF`. */
u8 FieldEntityDirByVec(VECTOR* from, VECTOR* to, s32* sqrDist) {
    s32 dx;
    s32 dy;
    s32 dist;
    s32 angle;

    dx = to->vx - from->vx;
    dy = to->vy - from->vy;
    *sqrDist = dx * dx + dy * dy;
    dist = SquareRoot0(*sqrDist);
    *sqrDist = dist;
    dx = (dx << 12) / dist / 32;
    dy = (dy << 12) / dist / 32;
    if (dx * dx > dy * dy) {
        if (dx > 0) {
            if (dy > 0) {
                angle = g_FieldAtanTable[dy * 2];
            } else {
                angle = -g_FieldAtanTable[-dy * 2];
            }
        } else {
            if (dy > 0) {
                angle = -0x80 - g_FieldAtanTable[dy * 2];
            } else {
                angle = g_FieldAtanTable[-dy * 2] - 0x80;
            }
        }
    } else {
        if (dy > 0) {
            if (dx > 0) {
                angle = 0x40 - g_FieldAtanTable[dx * 2];
            } else {
                angle = g_FieldAtanTable[-dx * 2] + 0x40;
            }
        } else {
            if (dx > 0) {
                angle = g_FieldAtanTable[dx * 2] - 0x40;
            } else {
                angle = -0x40 - g_FieldAtanTable[-dx * 2];
            }
        }
    }
    return (angle + 0x40) & 0xFF;
}

/* One step of "walk towards MoveEnd". Returns 1 while still moving, 0 once the
 * entity is close enough -- either because it came within `range` of the goal
 * or because the remaining distance is below one frame of MoveSpeed, in which
 * case the position is snapped onto the goal exactly. */
s32 FieldEntityAutoMove(FieldEntity* entity, s16 range) {
    VECTOR from;
    VECTOR to;
    s32 sqrDist;
    s32 reach;

    from.vx = entity->PosX >> 12;
    from.vy = entity->PosY >> 12;
    to.vx = entity->MoveEndX >> 12;
    to.vy = entity->MoveEndY >> 12;
    reach = entity->SolidRange + range;
    sqrDist = (to.vx - from.vx) * (to.vx - from.vx) +
              (to.vy - from.vy) * (to.vy - from.vy);
    reach = reach * reach + 0x1000;
    if (range != 0 && reach >= sqrDist) {
        return 0;
    }
    if (sqrDist < (entity->MoveSpeed * entity->MoveSpeed) >> 16 ||
        sqrDist < 4) {
        entity->PosX = entity->MoveEndX;
        entity->PosY = entity->MoveEndY;
        return 0;
    }
    entity->MoveDir =
        FieldEntityDirByVec(&from, &to, &sqrDist) - entity->MoveDirAdd;
    return 1;
}

/* Detect when a moving entity crosses a walkmesh triangle edge, and hop to
 * the triangle across it. */
INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldEntityWalkmechCross);

static void FieldEntityVectorSub(s32* arg0, s16* arg1, s16* arg2) {
    arg0[0] = arg1[0] - arg2[0];
    arg0[1] = arg1[1] - arg2[1];
    arg0[2] = arg1[2] - arg2[2];
}

/* Height of `point` on the triangle plane spanned by edgeA/edgeB through
 * `vertex`. edgeA doubles as scratch: once the normal is known it is reloaded
 * with the vertex, so the caller must treat it as clobbered. */
s32 FieldEntityCalculateZ(s32* edgeA, s32* edgeB, s32* point, s16* vertex) {
    s32 normal[3];

    normal[0] = -edgeA[1] * edgeB[2] + edgeB[1] * edgeA[2];
    normal[1] = -edgeA[2] * edgeB[0] + edgeA[0] * edgeB[2];
    normal[2] = -edgeA[0] * edgeB[1] + edgeB[0] * edgeA[1];
    edgeA[0] = vertex[0];
    edgeA[1] = vertex[1];
    edgeA[2] = vertex[2];
    return (normal[0] * edgeA[0] + normal[1] * edgeA[1] + normal[2] * edgeA[2] -
            normal[0] * point[0] - normal[1] * point[1]) /
           normal[2];
}

/* Step one entity along its direction, sliding along walkmesh edges when
 * the straight path is blocked. */
INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldEntityMove);

/* Would `pos` put entity `entityId` inside another solid entity? Two entities
 * collide when their horizontal distance falls under the mean of their two
 * solid radii and they are within ~127 units of each other vertically, so
 * characters on a different floor of the same map never block one another.
 * Only the player's own collisions arm the other entity's push script. */
s32 FieldEntityCollisionCheck(s16 entityId, VECTOR* pos) {
    s16 i;
    s32 hit;
    s32 sqrRadius;
    s32 range;
    s32 dz;
    s32 radius;
    s32 dx;
    s32 dy;

    hit = 0;
    range = g_FieldEntity[entityId].SolidRange;
    for (i = 0; i < g_FieldStateData.modelCount; i++) {
        if (i == entityId) {
            continue;
        }
        if (g_FieldEntity[i].SolidOff != 0) {
            continue;
        }
        dz = (g_FieldEntity[i].PosZ >> 12) - pos->vz;
        if (dz < -126 || dz > 127) {
            continue;
        }
        sqrRadius = (range + g_FieldEntity[i].SolidRange) >> 1;
        radius = sqrRadius;
        dx = (g_FieldEntity[i].PosX - pos->vx) >> 12;
        dy = (g_FieldEntity[i].PosY - pos->vy) >> 12;
        sqrRadius = radius * radius;
        if (sqrRadius > dx * dx + dy * dy) {
            hit = 1;
            if (entityId == g_PlayerModelId) {
                g_FieldEntity[i].requestPushScript = 1;
            }
        }
    }
    return hit;
}

/* Squared distance from `point` to the segment `line`, with the foot of the
 * perpendicular written to `nearest`. Returns -1 when that foot lands outside
 * the segment on either the x or the y axis, which is how callers tell "past
 * the end of the line" apart from "near it". The line parameter runs in 8-bit
 * fixed point, so the projection stays in integer arithmetic throughout. */
s32 FieldEntitySqrDistToLine(FieldLine* line, s32* point, s32* nearest) {
    s32 t;

    t = -(((line->pos.x1 - point[0]) * (line->pos.x2 - line->pos.x1) +
           (line->pos.y1 - point[1]) * (line->pos.y2 - line->pos.y1) +
           (line->pos.z1 - point[2]) * (line->pos.z2 - line->pos.z1))
          << 8) /
        ((line->pos.x2 - line->pos.x1) * (line->pos.x2 - line->pos.x1) +
         (line->pos.y2 - line->pos.y1) * (line->pos.y2 - line->pos.y1) +
         (line->pos.z2 - line->pos.z1) * (line->pos.z2 - line->pos.z1));
    nearest[0] = ((t * (line->pos.x2 - line->pos.x1)) >> 8) + line->pos.x1;
    nearest[1] = ((t * (line->pos.y2 - line->pos.y1)) >> 8) + line->pos.y1;
    nearest[2] = ((t * (line->pos.z2 - line->pos.z1)) >> 8) + line->pos.z1;
    if ((line->pos.x1 - nearest[0] >= 0 && line->pos.x2 - nearest[0] <= 0) ||
        (line->pos.x1 - nearest[0] <= 0 && line->pos.x2 - nearest[0] >= 0)) {
        if ((line->pos.y1 - nearest[1] >= 0 &&
             line->pos.y2 - nearest[1] <= 0) ||
            (line->pos.y1 - nearest[1] <= 0 &&
             line->pos.y2 - nearest[1] >= 0)) {
            t = (nearest[0] - point[0]) * (nearest[0] - point[0]) +
                (nearest[1] - point[1]) * (nearest[1] - point[1]) +
                (nearest[2] - point[2]) * (nearest[2] - point[2]);
            goto out;
        }
    }
    t = -1;
out:
    return t;
}

/* Walk the map's 32 trigger lines against one entity and raise the script
 * requests each is due. Entering a line's radius arms touch-on (and, if the
 * entity crossed the line this frame and faces it within +/-64, push and
 * isOnLine), leaving arms touch-off. */
u8 FieldEntityLineCheck(FieldEntity* entity, FieldLine* lines, VECTOR* dest) {
    s32* from;
    s32* to;
    s32* nearest;
    s32 sqrDist;
    s32 crossFrom;
    s32 crossTo;
    u8 hit;
    s32 angle;
    s32 delta;
    s32 i;

    from = (s32*)0x1F800000;
    to = (s32*)0x1F800010;
    nearest = (s32*)0x1F800020;
    from[0] = entity->PosX >> 12;
    from[1] = entity->PosY >> 12;
    from[2] = entity->PosZ >> 12;
    to[0] = dest->vx >> 12;
    to[1] = dest->vy >> 12;
    to[2] = entity->PosZ >> 12;
    hit = 0;
    for (i = 0; i < 32; i++, lines++) {
        if (lines->isActive != 1) {
            continue;
        }
        lines->isOnLine = 0;
        sqrDist = FieldEntitySqrDistToLine(lines, from, nearest);
        if (sqrDist != -1 &&
            sqrDist < (s32)(entity->SolidRange * entity->SolidRange)) {
            if (lines->slipDisabled == 1) {
                hit = 1;
            }
            if (lines->touch == 0) {
                lines->requestTouchOnScript = 1;
            }
            lines->touch = 1;
            crossFrom =
                (lines->pos.x2 - lines->pos.x1) * (from[1] - lines->pos.y1) -
                (from[0] - lines->pos.x1) * (lines->pos.y2 - lines->pos.y1);
            crossTo =
                (lines->pos.x2 - lines->pos.x1) * (to[1] - lines->pos.y1) -
                (to[0] - lines->pos.x1) * (lines->pos.y2 - lines->pos.y1);
            if ((crossFrom >= 0 && crossTo < 0) ||
                (crossTo >= 0 && crossFrom < 0) ||
                (crossFrom > 0 && crossTo <= 0) ||
                (crossTo > 0 && crossFrom <= 0)) {
                lines->across = 1;
            }
            if (from[0] != nearest[0] || from[1] != nearest[1]) {
                lines->proximityAngle = FieldEntityDirByVec(
                    (VECTOR*)from, (VECTOR*)nearest, &sqrDist);
                angle = lines->proximityAngle;
                delta = (u8)(angle - entity->MoveDir + 0x40);
                if (delta >= 0x80) {
                    continue;
                }
            }
            lines->requestPushScript = 1;
            lines->isOnLine = 1;
        } else {
            if (lines->touch == 1) {
                lines->requestTouchOffScript = 1;
            }
            lines->touch = 0;
        }
    }
    return hit;
}

/* Walk the 32 field lines against one entity: enter/leave each line's trigger
 * volume, and fire its on/off scripts. The `active = 1;` at the top of the loop
 * body is load-bearing and is not a style choice. */
void FieldEntityLineInteract(FieldEntity* entity, FieldLine* line) {
    s32* from;
    s32* nearest;
    s32 i;
    s32 sqrDist;
    s32 diff;
    u32* pad2;
    s32 active;

    from = (s32*)0x1F800000;
    nearest = (s32*)0x1F800010;
    from[0] = entity->PosX >> 12;
    from[1] = entity->PosY >> 12;
    from[2] = entity->PosZ >> 12;
    for (i = 0; i < 32; i++, line++) {
        active = 1;
        pad2 = &g_FieldPad2State;
        if (line->isActive != active) {
            continue;
        }
        if (entity->scriptedMoveMode != 0) {
            continue;
        }
        sqrDist = FieldEntitySqrDistToLine(line, from, nearest);
        if (sqrDist != -1 &&
            sqrDist < entity->SolidRange * entity->SolidRange) {
            if (line->touch == 0) {
                line->requestTouchOnScript = active;
            }
            line->touch = active;
        } else {
            if (line->touch == 1) {
                line->requestTouchOffScript = active;
            }
            line->touch = 0;
        }
        if (line->isOnLine != 1) {
            continue;
        }
        diff = (u8)(line->proximityAngle - entity->MoveDir + 0x20);
        if (diff >= 0x40) {
            continue;
        }
        if (!(pad2[0] & 0x20)) {
            continue;
        }
        if (pad2[1] & 0x20) {
            continue;
        }
        line->requestTalkScript = active;
    }
}

void FieldEntityLineClear(FieldLine* lines) {
    s32 i;

    for (i = 0; i < LEN(g_FieldLines); i++) {
        lines->isOnLine = 0;
        lines++;
    }
}

/* Did this step take the entity across one of the map's twelve gateway lines?
 * The move is staged in the PS1 scratchpad as two points -- where the entity is
 * now and where it wants to go -- and each gateway near enough to matter gets a
 * pair of 2D cross products, one per point. Opposite signs mean the segment
 * crossed the line, which loads the destination map. A gateway whose
 * destFieldId is 0x7FFF is an unused slot. */
void FieldEntityGatewayCheck(
    FieldEntity* entity, FieldGateway* gateway, VECTOR* dest) {
    s32* from;
    s32* to;
    s32* nearest;
    s32 i;
    s32 sqrDist;
    s16 x1;
    s16 y1;
    s32 dx;
    s32 dy;
    s32 crossFrom;
    s32 crossTo;

    from = (s32*)0x1F800000;
    to = (s32*)0x1F800010;
    nearest = (s32*)0x1F800020;
    from[0] = entity->PosX >> 12;
    from[1] = entity->PosY >> 12;
    from[2] = entity->PosZ >> 12;
    to[0] = dest->vx >> 12;
    to[1] = dest->vy >> 12;
    to[2] = entity->PosZ >> 12;
    for (i = 0; i < 12; i++, gateway++) {
        if (gateway->destFieldId == 0x7FFF) {
            continue;
        }
        sqrDist = FieldEntitySqrDistToLine((FieldLine*)gateway, from, nearest);
        if (sqrDist == -1) {
            continue;
        }
        if (sqrDist >= entity->SolidRange * entity->SolidRange) {
            continue;
        }
        x1 = gateway->pos.x1;
        y1 = gateway->pos.y1;
        dx = gateway->pos.x2 - x1;
        dy = gateway->pos.y2 - y1;
        crossFrom = dx * (from[1] - y1) - (from[0] - x1) * dy;
        crossTo = dx * (to[1] - y1) - (to[0] - x1) * dy;
        if ((crossFrom >= 0 && crossTo < 0) ||
            (crossTo >= 0 && crossFrom < 0) ||
            (crossFrom > 0 && crossTo <= 0) ||
            (crossTo > 0 && crossFrom <= 0)) {
            FieldEntityGatewayMapLoad(gateway);
        }
    }
}

/* Arms (even type) or disarms (odd type) one background trigger, and reports
 * whether that actually changed the bit -- the caller only redraws when it did.
 *
 * The array element is read inline at each use rather than through an `old`
 * local: with the local, both arms allocate the same register for the index
 * and gcc's post-reload cross-jump merges their two identical store tails into
 * one, which is four instructions short of the original. */
s16 FieldEntityBgTriggerActivate(FieldBgTrigger* trigger, u8 type) {
    s32 changed;
    s32 bit;
    s32 mask;

    changed = 0;
    switch (type) {
    case 0:
    case 2:
    case 4:
        bit = 1 << trigger->bgState;
        if ((g_FieldEntityBgTrigger[trigger->entityId] & bit) == 0) {
            changed = 1;
        }
        g_FieldEntityBgTrigger[trigger->entityId] =
            bit | g_FieldEntityBgTrigger[trigger->entityId];
        break;
    case 1:
    case 3:
    case 5:
        mask = ~(1 << trigger->bgState);
        if ((u8)(g_FieldEntityBgTrigger[trigger->entityId] | mask) == 0xFF) {
            changed = 1;
        }
        g_FieldEntityBgTrigger[trigger->entityId] =
            mask & g_FieldEntityBgTrigger[trigger->entityId];
        break;
    }
    return changed;
}

/* Walk the 12 background triggers against one entity and arm/disarm each it
 * crosses or comes near. In-proximity arms directly when the entity stands on
 * the line, else needs the entity facing it within +/-64; crossing types 4/5
 * arm/disarm on the back-side sign test. Each state change plays the trigger's
 * sound effect. */
void FieldEntityTriggerCheck(
    FieldEntity* entity, FieldBgTrigger* trigger, VECTOR* dest) {
    s32* from = (s32*)0x1F800000;
    s32* nearest = (s32*)0x1F800020;
    u16 seIds[4] = {0x0, 0x36, 0x7A, 0x12A};
    s32 sqrDist;
    s32 cross;
    s32* posZ;
    s32 dir;
    s32 rel;
    s32 i;

    *(s32*)0x1F800000 = entity->PosX >> 12;
    posZ = &entity->PosZ;
    *(s32*)0x1F800004 = entity->PosY >> 12;
    *(s32*)0x1F800008 = *posZ >> 12;
    for (i = 0; i < 12; i++, trigger++) {
        if (trigger->entityId == 0xFF) {
            continue;
        }
        sqrDist = FieldEntitySqrDistToLine((FieldLine*)trigger, from, nearest);
        if (sqrDist != -1 &&
            sqrDist < (s32)(entity->SolidRange * entity->SolidRange)) {
            if (from[0] == nearest[0] && from[1] == nearest[1]) {
                if (FieldEntityBgTriggerActivate(trigger, trigger->type) == 1) {
                    func_8001117C(seIds[trigger->soundId]);
                }
                continue;
            }
            dir =
                FieldEntityDirByVec((VECTOR*)from, (VECTOR*)nearest, &sqrDist);
            rel = (u8)(dir - entity->MoveDir + 0x40);
            if (rel >= 0x80) {
                continue;
            }
            if (FieldEntityBgTriggerActivate(trigger, trigger->type) == 1) {
                func_8001117C(seIds[trigger->soundId]);
            }
            continue;
        }
        if (trigger->type >= 4) {
            cross = (trigger->pos.x2 - trigger->pos.x1) *
                        (from[1] - trigger->pos.y1) -
                    (trigger->pos.y2 - trigger->pos.y1) *
                        (from[0] - trigger->pos.x1);
            if (cross > 0) {
                continue;
            }
        }
        if (trigger->type == 2 || trigger->type == 4) {
            if (FieldEntityBgTriggerActivate(trigger, 1) == 1) {
                func_8001117C(seIds[trigger->soundId]);
            }
        }
        if (trigger->type == 3 || trigger->type == 5) {
            if (FieldEntityBgTriggerActivate(trigger, 0) == 1) {
                func_8001117C(seIds[trigger->soundId]);
            }
        }
    }
}

void FieldEntityBgTriggerInit(FieldBgTrigger* triggers) {
    s32 i;

    /* The pointer walk belongs in the header, after `i++`: the original
     * increments the counter first and the walking pointer second, and a
     * `triggers++` at the end of the body emits them the other way round. */
    for (i = 0; i < 12; i++, triggers++) {
        if (triggers->entityId != 0xFF) {
            switch (triggers->type) {
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

/////////////////////////////////////////////////
// Begin of field_camera.c
/////////////////////////////////////////////////

const u32 D_800A00DC[] = {0x00000000};
/* Top-level field model loader: build the FieldModelData from the loaded model
 * header, stream the field's model set off the CD into the overlay staging
 * buffer at 0x801B0000, load the global and then the local models, clear every
 * model's flags byte except the player's, and finally push each model's
 * eye/mouth texture to VRAM before resetting the KAWAI state.
 *
 * The three scratchpad slots are the loaders' out-of-band parameter block:
 * words 0 and 1 hand FieldModelLoadGlobalModels its part and animation staging
 * buffers, and bytes 0..3 hand KawaiLoadEyesMouthTexToVram the eye/mouth
 * selection for the model it is about to upload. */
void FieldModelLoadAndInit(void) {
    u8* buf;
    u32 i;

    D_800DFCA0 = (FieldTexBlockHeader*)0x80128000;
    buf =
        FieldModelStructInit((FieldModelFileDesc*)D_8007E770, g_FieldModelData);
    g_FieldModelBufferTop = (u32)buf;
    D_800E0204 = buf;
    DS_read(g_FieldFileSectors[g_CurrentFieldIndex * 6],
            g_FieldFileSectors[g_CurrentFieldIndex * 6 + 1],
            (u_long*)0x801B0000, NULL);
    while (SystemCdromReadChain() != 0) {
    }
    ((u8**)0x1F800000)[0] = g_FieldGlobalModelFiles;
    ((u8**)0x1F800000)[1] = g_FieldGlobalTexFile;
    g_FieldModelBufferTop = (u32)FieldModelLoadGlobalModels(
        (FieldModelFileDesc*)D_8007E770, g_FieldModelData,
        (u8*)g_FieldModelBufferTop, 1);
    g_FieldModelBufferTop = (u32)LoadLocalFieldModelAndInitAll(
        (FieldModelFileDesc*)D_8007E770, g_FieldModelData, (u8*)D_800A00DC,
        (u32*)0x801B0000);
    for (i = 1; i < g_FieldModelData->modelCount; i++) {
        g_FieldModelData->modelEntries[i].flags = 0;
    }
    for (i = 0; i < g_FieldModelData->modelCount; i++) {
        ((KawaiFaceSel*)0x1F800000)->mouth2 = 1;
        ((KawaiFaceSel*)0x1F800000)->eye = 0;
        ((KawaiFaceSel*)0x1F800000)->slot = i;
        *(u8*)0x1F800000 = 1; /* deliberately not through the record */
        KawaiLoadEyesMouthTexToVram(
            &g_FieldModelData->modelEntries[i], (u8*)0x1F800000);
    }
    KawaiClearData();
}

extern u8 D_801144D8; // blink RNG cursor

/* Per-frame KAWAI pass over every field entity, in four sweeps. First place
 * each model at its entity's position plus offset and, for the model types the
 * KAWAI script drives (4, 8, 9, 11, 12), build the animation matrices into a
 * scratch matrix and copy the current view matrix into the model's part
 * matrices -- everything else animates straight into the view matrix. Then
 * queue every visible model for rendering, run its KAWAI script, and finally
 * push the eye/mouth texture for the frame, blinking on a random countdown. */
void HandleKawaiDataInModel(struct FieldRenderData* buf) {
    SVECTOR pos;
    long screenPos;
    FieldModelLoaderData* models;
    FieldModelEntry* model;
    u8* faceSel;
    s32* dst;
    s32* src;
    s16 kawaiOp;
    s32 blink;
    s32 blinkOpen;
    u8 blinkClosed;
    s32 i;

    faceSel = (u8*)0x1F800000;
    D_800DF114 = D_80075DEC;
    D_800DF118 = buf;
    models = ((FieldModelFileDesc*)D_8007E770)->models;

    for (i = 0; i < D_8009AC1C; i++) {
        s8 kawaiType;

        if (models[i].modelEntryIndex == 0xFF) {
            continue;
        }
        pos.vx = (g_FieldEntity[i].PosX >> 12) + g_FieldEntity[i].OffsetX;
        pos.vy = (g_FieldEntity[i].PosY >> 12) + g_FieldEntity[i].OffsetY;
        pos.vz =
            ((g_FieldEntity[i].PosZ >> 12) + g_FieldEntity[i].OffsetZ) - 10;
        g_FieldModelData->modelEntries[models[i].modelEntryIndex].translationX =
            pos.vx;
        g_FieldModelData->modelEntries[models[i].modelEntryIndex].translationY =
            pos.vy;
        g_FieldModelData->modelEntries[models[i].modelEntryIndex].translationZ =
            pos.vz;
        if (FieldCalcWorldToScreenPos(&pos, &screenPos) < 0xF00) {
            g_FieldModelData->modelEntries[models[i].modelEntryIndex]
                .rotationZ = g_FieldEntity[i].Dir;
            kawaiType =
                g_FieldModelData->modelEntries[models[i].modelEntryIndex]
                    .kawaiType;
            if (kawaiType == 4 || kawaiType == 8 || kawaiType == 9 ||
                kawaiType == 11 || kawaiType == 12) {
                MATRIX mtx;

                mtx.m[0][0] = mtx.m[1][1] = mtx.m[2][2] = 0x1000;
                mtx.t[0] = mtx.t[1] = mtx.t[2] = 0;
                mtx.m[0][1] = mtx.m[0][2] = mtx.m[1][0] = mtx.m[1][2] =
                    mtx.m[2][0] = mtx.m[2][1] = 0;
                *(s32*)0x1F800000 = 3;
                FieldModelAnimCalcMtrxs(
                    &g_FieldModelData->modelEntries[models[i].modelEntryIndex],
                    &mtx, g_FieldEntity[i].activeAnimId,
                    g_FieldEntity[i].animCurrentFrame >> 4);
                dst = (s32*)g_FieldModelData
                          ->modelEntries[models[i].modelEntryIndex]
                          .partMatrices;
                src = (s32*)D_80071E40;
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
                dst[4] = src[4];
                dst[5] = src[5];
                dst[6] = src[6];
                dst[7] = src[7];
            } else {
                *(s32*)0x1F800000 = 3;
                FieldModelAnimCalcMtrxs(
                    &g_FieldModelData->modelEntries[models[i].modelEntryIndex],
                    D_80071E40, g_FieldEntity[i].activeAnimId,
                    g_FieldEntity[i].animCurrentFrame >> 4);
            }
        }
    }

    for (i = 0; i < D_8009AC1C; i++) {
        s8 kawaiType;

        if (models[i].modelEntryIndex == 0xFF) {
            continue;
        }
        pos.vx = g_FieldEntity[i].PosX >> 12;
        pos.vy = g_FieldEntity[i].PosY >> 12;
        pos.vz = (g_FieldEntity[i].PosZ >> 12) - 10;
        if (FieldCalcWorldToScreenPos(&pos, &screenPos) < 0xF00) {
            model = &g_FieldModelData->modelEntries[models[i].modelEntryIndex];
            kawaiType = model->kawaiType;
            if (kawaiType == 4 || kawaiType == 8 || kawaiType == 9 ||
                kawaiType == 11 || kawaiType == 12) {
                FieldModelPrepareRender(
                    &g_FieldModelData->modelEntries[models[i].modelEntryIndex]);
            } else {
                model->kawaiType = -1;
                FieldModelPrepareRender(
                    &g_FieldModelData->modelEntries[models[i].modelEntryIndex]);
                g_FieldModelData->modelEntries[models[i].modelEntryIndex]
                    .kawaiType = kawaiType;
            }
        }
    }

    for (i = 0; i < D_8009AC1C; i++) {
        if (models[i].modelEntryIndex == 0xFF) {
            continue;
        }
        kawaiOp = g_FieldEntity[i].KawaiOp1;
        if (kawaiOp != 1) {
            continue;
        }
        pos.vx = g_FieldEntity[i].PosX >> 12;
        pos.vy = g_FieldEntity[i].PosY >> 12;
        pos.vz = (g_FieldEntity[i].PosZ >> 12) - 10;
        if (FieldCalcWorldToScreenPos(&pos, &screenPos) < 0xF00) {
            if (KawaiExecute(
                    &g_FieldModelData->modelEntries[models[i].modelEntryIndex],
                    g_FieldEntity[i].KawaiDataOffset, models[i].modelEntryIndex,
                    D_80071E40) == kawaiOp) {
                g_FieldEntity[i].KawaiOp1 = 2;
            }
        }
    }

    for (i = 0; i < D_8009AC1C; i++) {
        blinkOpen = 1;
        blinkClosed = 2;
        if (models[i].modelEntryIndex == 0xFF) {
            continue;
        }
        if (g_FieldEntity[i].BlinkOn == 1) {
            continue;
        }
        if (g_FieldEntity[i].KawaiA == 0) {
            faceSel[0] = blinkClosed;
            faceSel[1] = blinkClosed;
            faceSel[2] = 0;
            faceSel[3] = i;
            blink = (g_RandomTable[D_801144D8++] & 0x1F) + 0x40;
            g_FieldEntity[i].KawaiA = blink;
        } else {
            faceSel[0] = blinkOpen;
            faceSel[1] = blinkOpen;
            faceSel[2] = 0;
            faceSel[3] = i;
            blink = g_FieldEntity[i].KawaiA - 1;
            g_FieldEntity[i].KawaiA = blink;
        }
        KawaiLoadEyesMouthTexToVram(
            &g_FieldModelData->modelEntries[i], faceSel);
    }
}

// Possable Debug routine. Ran at beginning of every main field loop. (FPS?)
void DebugRunEveryLoop(void) {}

void FieldCameraAssign(void) {
    if (g_FieldMovieStreamActive == 0 || g_FieldCameraMatrixSel == 1) {
        D_80071E40 = *D_80083578;
    } else {
        D_80071E40 = g_DebugMatrixP;
    }
}

/* Drive the CD stream that feeds the MDEC. While a field map is still loading
 * the stream is not touched at all; otherwise the chain reader's status decides
 * whether to arm the ring buffer, start playback, or tear it down. */
void FieldUpdateMovieStream(void) {
    u32 status;

    if (g_isFieldLoading == 1) {
        if (SystemCdromReadChain() == 0) {
            g_isFieldLoading = 2;
        }
        return;
    }
    if (g_FieldStateData.eventCmd == EVTCMD_UNK14) {
        func_80035658();
        g_FieldMovieStreamActive = 0;
        g_FieldMoviePlayed = 0;
        g_FieldStateData.movieCommandState = MOVCMD_DONE;
        return;
    }
    status = SystemCdromReadChain();
    switch (status) {
    case 0:
        if (g_FieldStateData.eventCmd == EVTCMD_LOAD_MOVIE &&
            g_FieldStateData.movieCommandState == MOVCMD_IDLE) {
            if (g_FieldModelBufferTop <= 0x801AFFFF) {
                func_80034FC8(
                    g_FieldModelBufferTop, g_FieldStateData.eventCmdParam);
            } else {
                func_80034FC8(0x801B0000, g_FieldStateData.eventCmdParam);
            }
            g_FieldStateData.movieCommandState = MOVCMD_ACTIVE;
            g_FieldMoviePlayed = 1;
        }
        if ((s16)g_FieldMovieStreamActive == 1) {
            g_FieldMovieStreamDone = 1;
            g_FieldMovieStreamActive = 0;
            g_FieldMoviePlayed = 0;
            g_FieldStateData.movieCommandState = MOVCMD_DONE;
        }
        break;
    case 0xA:
        if (g_FieldStateData.eventCmd == EVTCMD_LOAD_MOVIE) {
            g_FieldStateData.movieCommandState = MOVCMD_DONE;
        }
        if (g_FieldStateData.eventCmd == EVTCMD_PLAY_MOVIE) {
            g_FieldStateData.movieCommandState = MOVCMD_ACTIVE;
            func_800354CC();
            g_FieldMovieStreamActive = 1;
        }
        break;
    }
}

/////////////////////////////////////////////////
// Begin of field_rain.c
/////////////////////////////////////////////////

extern struct FieldRain g_FieldRain[64];
extern u8 g_RainForce;
extern s16 D_800E42EE[0x40][12];

void FieldRainInit(struct FieldRenderData* renderData) {
    LINE_F2* line;
    s32 i;
    s32 adjustedIndex;

    for (i = 0; i < LEN(g_FieldRain); i++) {
        g_FieldRain[i].render = 0;
        g_FieldRain[i].rndSeed = i * 4;
        g_FieldRain[i].wait = i % 8;

        line = &renderData->Rain[i];

        SetLineF2(line);
        SetSemiTrans(line, 1);

        renderData->Rain[i].r0 = 0x10;
        renderData->Rain[i].g0 = 0x10;
        renderData->Rain[i].b0 = 0x10;
    }

    SetDrawMode(&renderData->RainDm, 0, 0, GetTPage(0, 1, 0, 0) & 0xffff, NULL);
}

void FieldRainAddToRender(
    u32* ot, LINE_F2* rain, MATRIX* matrix, DR_MODE* rainDm) {
    long p;
    long flag;
    s32 i;
    s32 j;

    PushMatrix();
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);

    for (i = 0, j = 0; i < LEN(g_FieldRain); i++) {
        // 12 * sizeof(s16) = 24 bytes (0x18), the exact size of FieldRain
        if (D_800E42EE[i][0] == 1) {
            RotTransPers(&g_FieldRain[i].p1, &rain->x0, &p, &flag);
            RotTransPers(&g_FieldRain[i].p2, &rain->x1, &p, &flag);
            AddPrim(ot, rain);
        }
        rain++;
    }

    PopMatrix();

    *(u32*)rainDm = (*(u32*)rainDm & 0xFF000000) | (*ot & 0xFFFFFF);

    *ot = (*ot & 0xFF000000) | ((u32)rainDm & 0xFFFFFF);
}

/* Ramp the rain force towards 0 or 255 with the weather bit, then respawn any
 * drop whose wait has run out at a random offset around the player.
 *
 * The ceiling has to be a `u8` local (`u8 max = 255;`): as `s32` the constant
 * and the loaded `g_RainForce` swap $v0 and $v1 in the compare, which is four
 * rows. The parked body also carried a `g_FieldEntities[]` extern of its own,
 * which is not a symbol -- the array is `g_FieldEntity` -- so three rows were
 * checkfn refusing to alias `g_FieldEntities+0xc` onto `D_80074EB0`, and the
 * body would not have linked. Watch for that whenever a park's residue is a
 * handful of `%lo(sym+N)` rows against `D_` symbols: check the extern is the
 * real one before touching codegen. */
void FieldRainUpdate(void) {
    s32 i;
    s32 limit;
    s32 player;
    u8 max = 255;
    s32 vz;

    if ((g_RainControl & 0x80) == 0) {
        if (g_RainForce != 0) {
            g_RainForce--;
        }
    } else {
        if (g_RainForce != max) {
            g_RainForce++;
        }
    }

    limit = g_RainForce / 4;
    player = g_PlayerModelId;

    for (i = 0; i < 0x40; i++) {
        if (g_FieldRain[i].wait == 0) {
            if (i < limit) {

                u8 seed3;

                g_FieldRain[i].render = 1;
                g_FieldRain[i].rndSeed++;
                g_FieldRain[i].wait = 7;

                g_FieldRain[i].p2.vx =
                    (g_FieldEntity[player].PosX >> 12) +
                    g_RandomTable[g_FieldRain[i].rndSeed & 0xFF] * 12 - 0x600;

                seed3 = g_FieldRain[i].rndSeed * 3;
                g_FieldRain[i].p2.vy = (g_FieldEntity[player].PosY >> 12) +
                                       g_RandomTable[seed3] * 12 - 0x600;

                g_FieldRain[i].p1.vx = g_FieldRain[i].p2.vx;
                g_FieldRain[i].p1.vy = g_FieldRain[i].p2.vy;

                g_FieldRain[i].z = (g_FieldEntity[player].PosZ >> 12) - 0x300;
            } else {
                g_FieldRain[i].wait = 1;
                g_FieldRain[i].render = 0;
            }
        }

        g_FieldRain[i].p2.vz =
            g_FieldRain[i].z + (g_FieldRain[i].wait & 0x7) * 0x80;

        vz = (g_FieldRain[i].wait & 0x7) * 0x80;
        vz += 0x100;

        g_FieldRain[i].p1.vz = g_FieldRain[i].z + vz;

        g_FieldRain[i].wait--;
    }
}

/////////////////////////////////////////////////
// Begin of field_battle.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_battle.c
/////////////////////////////////////////////////

u8 FieldGetRandomU8FromList(void) {
    g_FieldRandListIndex++;
    if (g_FieldRandListIndex == 0) {
        g_FieldRandListOffset += 13;
    }
    return g_RandomTable[g_FieldRandListIndex] - g_FieldRandListOffset;
}

u8 FieldGetNextRandomU8(void) {
    g_FieldRandomIndex++;
    return g_RandomTable[g_FieldRandomIndex];
}

/* Check for a random or scripted battle this frame. The field's encounter
 * table is one of two 0x18-byte blocks (D_8009AC30 picks the second), holding
 * four "special" formations with their own 6- or 5-bit rates, a fallback, and
 * five ordinary formations. The step counter advances by the player's move
 * speed over the table's divisor; when a roll against it succeeds, the four
 * specials are tried in order against a cumulative rate, and failing those the
 * five ordinary slots are walked twice -- once, and again if the first walk
 * picked the same formation as last time. */
void FieldBattleCheck(void) {
    FieldEncounterTable* enc;
    s16* cur;
    s32 i;
    s32 sum;
    s32 rate;
    s32 total;
    s32 formation;
    u8 roll;
    u16 control;
    u16 slot;

    if (D_8009AC30 == 0) {
        enc = (FieldEncounterTable*)g_FieldEncounters;
    } else {
        enc = (FieldEncounterTable*)(g_FieldEncounters + 0x18);
    }
    D_8009C6D8 += 0x20;
    if (D_8009C6D8 == 0) {
        func_800262D8();
        Savemap.memory_bank_4[6]++;
        if (Savemap.memory_bank_4[6] == 0 && Savemap.memory_bank_4[7] != 0xFF) {
            Savemap.memory_bank_4[7]++;
        }
        control = enc->control;
        if ((control & 1) && g_FieldMovieStreamActive == 0 && D_8009AC2F == 0) {
            D_8007173C += (s32)g_FieldEntity[g_PlayerModelId].MoveSpeed /
                          (s32)(control >> 8);
            if (FieldGetRandomU8FromList() < (g_PreemptiveRate & 0x7F)) {
                D_800716D0 = 4;
            } else {
                D_800716D0 = 0;
            }
            if (FieldGetRandomU8FromList() <
                ((u32)D_8007173C * g_EncounterRateModifier) >> 12) {
                StopFieldMapPreload();
                D_8009ABF5 = 2;
                D_8007EBC8 = 1;
                roll = FieldGetNextRandomU8();
                roll >>= 2;
                if (!(g_PreemptiveRate & 0x80)) {
                    sum = (s32)(enc->special[0] << 16) >> 26;
                } else {
                    sum = (s32)(enc->special[0] << 16) >> 27;
                }
                if ((u8)roll < (u8)sum) {
                    D_800716D0 = 0;
                    formation = enc->special[0] & 0x3FF;
                    goto found;
                }
                if (!(g_PreemptiveRate & 0x80)) {
                    rate = (s32)(enc->special[1] << 16) >> 26;
                } else {
                    rate = (s32)(enc->special[1] << 16) >> 27;
                }
                sum += rate;
                if ((u8)roll < (u8)sum) {
                    D_800716D0 = 0;
                    formation = enc->special[1] & 0x3FF;
                    goto found;
                }
                slot = enc->special[2];
                total = sum + ((s32)(slot << 16) >> 26);
                if ((u8)roll < (u8)total) {
                    D_8009ABF6 = (total = slot) & 0x3FF;
                    return;
                }
                if (!(g_PreemptiveRate & 0x80)) {
                    rate = (s32)(enc->special[3] << 16) >> 26;
                } else {
                    rate = (s32)(enc->special[3] << 16) >> 27;
                }
                sum = total + rate;
                total = roll;
                if ((u8)total < (u8)sum) {
                    formation = enc->special[3] & 0x3FF;
                found:
                    D_8009ABF6 = formation;
                    return;
                }
                sum = 0;
                roll = FieldGetNextRandomU8();
                roll >>= 2;
                D_8009ABF6 = enc->fallback & 0x3FF;
                for (i = 0; i < 5; i++) {
                    slot = ((u16*)enc)[i + 1];
                    sum += (s32)(slot << 16) >> 26;
                    if ((u8)roll < (u8)sum) {
                        D_8009ABF6 = slot & 0x3FF;
                        break;
                    }
                }
                cur = &D_8009ABF6;
                if (*cur != g_FieldLastBattleFormationId) {
                    g_FieldLastBattleFormationId = *cur;
                    return;
                }
                sum = 0;
                roll = FieldGetNextRandomU8();
                roll >>= 2;
                *cur = enc->fallback & 0x3FF;
                for (i = 0; i < 5; i++) {
                    slot = ((u16*)enc)[i + 1];
                    sum += (s32)(slot << 16) >> 26;
                    if ((u8)roll < (u8)sum) {
                        D_8009ABF6 = slot & 0x3FF;
                        g_FieldLastBattleFormationId = slot & 0x3FF;
                        break;
                    }
                }
            }
        }
    }
}

/////////////////////////////////////////////////
// Begin of field_arrow.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_arrow.c
/////////////////////////////////////////////////

void FieldArrowsInit(SPRT_16* sprt, DR_MODE* dm) {
    s16 i;

    for (i = 0; i < 24; i++, sprt++) {
        SetSprt16(sprt);
        SetShadeTex(sprt, 1);
        SetSemiTrans(sprt, 0);
        sprt->r0 = 0x80;
        sprt->g0 = 0x80;
        sprt->b0 = 0x80;
        sprt->clut = GetClut(0x100, 0x1E9);
    }
    SetDrawMode(dm, 0, 1, GetTPage(0, 0, 0x3C0, 0x100), NULL);
}

/* Queue the field exit arrows: up to 12 gateway markers, then up to 12
 * "point" arrows the script can place, each projected through the current
 * camera matrix and linked into the render OT. */
void FieldArrowsAddToRender(
    struct FieldRenderData* buf, MATRIX* mtx, s32 markers) {
    SVECTOR pos;
    s32 off;
    s32 sz;
    s32 flag;
    s16 i;

    if ((g_FieldExitArrowState[0] == 1 && g_FieldAnimLock == 0) ||
        g_FieldExitArrowState[0] == 2) {
        i = 0;
        PushMatrix();
        SetRotMatrix(mtx);
        SetTransMatrix(mtx);
        do {
            if (*(u8*)(g_FieldTriggers + i + 0x218) == 1) {
                pos.vx = (*(s16*)(markers + i * 0x18) +
                          *(s16*)(markers + i * 0x18 + 6)) /
                         2;
                pos.vy = (*(s16*)(markers + i * 0x18 + 2) +
                          *(s16*)(markers + i * 0x18 + 8)) /
                         2;
                pos.vz = (*(s16*)(markers + i * 0x18 + 4) +
                          *(s16*)(markers + i * 0x18 + 0xA)) /
                         2;
                if (pos.vx != 0 || pos.vy != 0) {
                    RotTransPers(&pos, (s32*)&pos, &sz, &flag);
                    ((struct FieldRenderData*)(i * 0x10 + (s32)buf))
                        ->Arrows[0]
                        .u0 = (g_FieldArrowAnimTick * 4 & 0x30) + 0x30;
                    ((struct FieldRenderData*)(i * 0x10 + (s32)buf))
                        ->Arrows[0]
                        .v0 = 0xD0;
                    buf->Arrows[i].x0 = pos.vx - 7;
                    buf->Arrows[i].y0 = pos.vy - 8;
                    addPrim(buf->ot, &buf->Arrows[i]);
                }
            }
            i++;
        } while (i < 0xC);
        i = 0;
        do {
            off = i * 0x10;
            if (*(s32*)((u8*)g_FieldTriggers + off + 0x230) != 0) {
                pos.vx = *(u16*)((u8*)g_FieldTriggers + off + 0x224);
                pos.vy = *(u16*)((u8*)g_FieldTriggers + i * 0x10 + 0x228);
                pos.vz = *(u16*)((u8*)g_FieldTriggers + off + 0x22C);
                RotTransPers(&pos, (s32*)&pos, &sz, &flag);
                ((struct FieldRenderData*)(off + (s32)buf))->Arrows[0xC].u0 =
                    (g_FieldArrowAnimTick * 4 & 0x30) + 0x30;
                ((struct FieldRenderData*)(i * 0x10 + (s32)buf))
                    ->Arrows[0xC]
                    .v0 = 0xD0;
                buf->Arrows[i + 0xC].x0 = pos.vx - 7;
                buf->Arrows[i + 0xC].y0 = pos.vy - 8;
                if (*(s32*)((u8*)g_FieldTriggers + off + 0x230) == 2) {
                    buf->Arrows[i + 0xC].clut = GetClut(0x100, 0x1E8);
                }
                addPrim(buf->ot, &((SPRT_16*)((s32)buf + 0x40C0))[i]);
                do {
                } while (0);
            }
            i++;
        } while (i < 0xC);
        PopMatrix();
        addPrim(buf->ot, &buf->ArrowsDm);
        g_FieldArrowAnimTick++;
    }
}

/////////////////////////////////////////////////
// Begin of field_model.c
/////////////////////////////////////////////////

/* Load the map's own model file and bring every model in it up. */
INCLUDE_ASM("asm/us/field/nonmatchings/field2", LoadLocalFieldModelAndInitAll);

/* Reserves one 32-byte matrix slot per bone at the head of the packet buffer,
 * then emits the drawing packets for every part behind them. */
u8* FieldModelCreatePktsAndScale(FieldModelEntry* model, u8* pkts, s32 arg2) {
    u8* parts;
    u32 i;

    model->partMatrices = pkts;
    pkts += model->boneCount * 32;
    parts = model->modelData + model->partsOffset;
    for (i = 0; i < model->partCount; i++) {
        pkts = FieldModelCreatePktsForPart(
            (FieldModelPart*)&parts[i * 32], pkts, 0, arg2);
    }
    FieldModelScaleModel(model, model->scale, 0);
    return pkts;
}

/* Build one model part's render packets: eight primitive kinds in a fixed
 * order, twice over for the double buffer. */
INCLUDE_ASM("asm/us/field/nonmatchings/field2", FieldModelCreatePktsForPart);

void FieldModelLoadBsxTexToVram(BsxTexHeader* bsx) {
    RECT rect;
    u32 i;
    u32 count;
    BsxTexEntry* entries;

    count = bsx->texCount;
    entries = bsx->entries;
    for (i = 0; i < count; i++) {
        rect.x = entries[i].x;
        rect.y = entries[i].y;
        rect.w = entries[i].w;
        rect.h = entries[i].h;
        LoadImage(&rect, (u_long*)((u8*)bsx + entries[i].dataOffset));
    }
}

/* Apply a TDB ("texture delta") chunk from a BSX model file. Each record
 * relocates a raw blob (op 0), splices one 0x200-byte page (op 1) or one
 * 0x20-byte CLUT (op 2) into the shared model texture block at *D_800DFCA0, or
 * uploads an embedded image straight to VRAM (op 3). */
void FieldModelBsxTdbModify(u8* tdb) {
    TdbRecord* rec;
    s32 count;
    s32 i;

    if (tdb == NULL) {
        return;
    }
    count = *(s32*)tdb;
    if (count == 0) {
        return;
    }
    rec = (TdbRecord*)(tdb + 8);
    for (i = 0; i < count; i++) {
        switch (rec[i].opcode) {
        case 0:
            memcpy((u8*)rec[i].dst, tdb + rec[i].srcOff, rec[i].size);
            break;
        case 1:
            if (rec[i].dst < ((FieldTexBlockHeader*)D_800DFCA0)->numPages) {
                memcpy((u8*)D_800DFCA0 +
                           ((FieldTexBlockHeader*)D_800DFCA0)->pageOffset +
                           (rec[i].dst << 9),
                       tdb + rec[i].srcOff, 0x200);
            }
            break;
        case 2:
            if (rec[i].dst < ((FieldTexBlockHeader*)D_800DFCA0)->numCluts) {
                memcpy((u8*)D_800DFCA0 +
                           ((FieldTexBlockHeader*)D_800DFCA0)->clutOffset +
                           (rec[i].dst << 5),
                       tdb + rec[i].srcOff, 0x20);
            }
            break;
        case 3:
            LoadImage((RECT*)&rec[i].dst, (u_long*)(tdb + rec[i].srcOff));
            break;
        }
    }
}

/* Build the per-model FieldModelEntry table from the loaded model-file
 * descriptor. First pass numbers the NPC-flagged records; second pass fills
 * one entry each and hands out a running offset into the model data block. */
void* FieldModelStructInit(FieldModelFileDesc* desc, FieldModelData* data) {
    FieldModelData* d;
    FieldModelLoaderData* models;
    FieldModelEntry* entry;
    u32 i;
    s16 partsOff;
    u8 unusedLocals[0x10];

    /* two levels, and both are load-bearing -- see the note above */
    do {
        do {
            i = 0;
        } while (0);
    } while (0);
    d = data;
    data->modelCount = 0;
    models = desc->models;
    if (desc->count != 0) {
        do {
            if (models[i].npcFlag != 0) {
                models[i].modelEntryIndex = d->modelCount;
                d->modelCount = d->modelCount + 1;
            } else {
                models[i].modelEntryIndex = 0xFF;
            }
            i += 1;
        } while (i < desc->count);
    }
    i = 0;
    d->unk2 = 0;
    d->unk1 = 0;
    d->modelEntries = (FieldModelEntry*)((u8*)d + 0xC);
    d->unk8 = 0;
    /* `data` is the model-data cursor from here on */
    data = (FieldModelData*)((u8*)data + ((d->modelCount * 0x24) + 0xC));
    if (desc->count != 0) {
        do {
            if (models[i].npcFlag != 0) {
                if (((u32)(*(u8*)&models[i].globalModelId - 1) < 9) &&
                    (models[i].animationCount < 3)) {
                    models[i].animationCount = 3;
                }
                entry = &d->modelEntries[models[i].modelEntryIndex];
                entry->flags = 1;
                entry->kawaiType = -1;
                entry->boneCount = models[i].boneCount;
                entry->partCount = models[i].partCount;
                entry->animationCount = models[i].animationCount;
                entry->rotationZ = 0;
                entry->rotationY = 0;
                entry->rotationX = 0;
                entry->translationZ = 0;
                entry->translationY = 0;
                entry->translationX = 0;
                entry->globalModelId = *(u8*)&models[i].globalModelId;
                entry->textureFaceId = models[i].faceId;
                entry->scale = 0x1000;
                partsOff = models[i].boneCount * 4;
                entry->partsOffset = partsOff;
                entry->animationOffset = partsOff + (models[i].partCount << 5);
                entry->modelData = (u8*)data;
                entry->partMatrices = NULL;
                data = (FieldModelData*)((u8*)data +
                                         ((models[i].boneCount * 4) +
                                          (models[i].partCount << 5) +
                                          (models[i].animationCount * 0x10)));
            }
            i += 1;
        } while (i < desc->count);
    }
    D_800E0204 = 0;
    return data;
}

/* Loads every global (BCX) model in the header, then optionally kicks off the
 * next streamed read. Scratchpad word 0 is clobbered by each load and restored
 * before the next one; word 1 holds the sector/size pair for that read. */
u8* FieldModelLoadGlobalModels(
    FieldModelFileDesc* desc, FieldModelData* data, u8* pkts, s32 readFile) {
    u32* fileInfo;
    s32 saved;
    u32 i;

    saved = ((s32*)0x1F800000)[0];
    fileInfo = (u32*)((s32*)0x1F800000)[1];
    for (i = 0; i < desc->count; i++) {
        ((s32*)0x1F800000)[0] = saved;
        pkts = FieldModelLoadBcx(desc, data, pkts, i);
    }
    if (readFile) {
        DS_read(fileInfo[0], fileInfo[1], D_800DFCA0, NULL);
        while (SystemCdromReadChain() != 0) {
        }
    }
    return pkts;
}
