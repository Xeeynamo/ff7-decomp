//! PSYQ=3.3

#include "highway_private.h"

void HighwayDrawNode(HighwayBuffer* db, JetNode* node, s16 otIndex, s32 unused);

// Mark every object free and rebuild the object index free list.
void HighwayObjectsInit(void) {
    s32 i;

    for (i = 0; i < LEN(g_HighwayObjects); i++) {
        g_HighwayObjects[i].index = -1;
        g_HighwayObjects[i].active = 0;
    }
    g_HighwayNextFreeObject = 0;
    for (i = 0; i < LEN(g_HighwayObjectFreeList); i++) {
        g_HighwayObjectFreeList[i] = i + 1;
    }
}

// Allocate an object from a spawn template, like JetObjectAlloc.
s16 HighwayObjectAlloc(HighwayObject* spawn, s16 parentIndex) {
    s16 index;
    s16 modelId;

    if (g_HighwayObjectCount < LEN(g_HighwayObjects) - 1) {
        g_HighwayObjectCount++;
        index = HighwayObjectIndexAlloc();
        g_HighwayObjects[index] = *spawn;
        modelId = spawn->unk2C;
        g_HighwayObjects[index].active = 1;
        g_HighwayObjects[index].index = index;
        if (!parentIndex) {
            g_HighwayObjects[index].node =
                HighwayNodeAlloc(modelId, 0, 0, 1, &g_HighwayRootNode, spawn->position.vx, spawn->position.vy,
                                 spawn->position.vz, spawn->rotation.vx, spawn->rotation.vy, spawn->rotation.vz);
        } else {
            g_HighwayObjects[index].node = HighwayNodeAlloc(
                modelId, 0, 0, 1, g_HighwayObjects[parentIndex].node, spawn->position.vx, spawn->position.vy,
                spawn->position.vz, spawn->rotation.vx, spawn->rotation.vy, spawn->rotation.vz);
        }
    }
    return index;
}

// Free an object and its node, like JetObjectFree.
inline void HighwayObjectFree(HighwayObject* object) {
    if (object->index != -1) {
        g_HighwayObjectCount--;
        HighwayNodeFree(object->node);
        HighwayObjectIndexFree(object->index);
        object->index = -1;
        object->active = 0;
    }
}

s16 HighwayObjectIndexAlloc(void) {
    s16 index;

    index = g_HighwayNextFreeObject;
    g_HighwayNextFreeObject = g_HighwayObjectFreeList[index];

    return index;
}

void HighwayObjectIndexFree(s16 index) {
    s16 next;

    next = g_HighwayNextFreeObject;
    g_HighwayNextFreeObject = index;
    g_HighwayObjectFreeList[index] = next;
}

void HighwayObjectsUpdate(HighwayBuffer* db) {
    // FAKE: preserves stack space; the original local declarations are unknown.
    VECTOR unused;
    HighwayObject* obj;
    HighwayRiderState* state;
    s32 i;
    u8 order;
    u8 drawMode;
    u16 otIndex;

    for (i = 0; i < LEN(g_HighwayObjects); i++) {
        otIndex = 0;
        obj = &g_HighwayObjects[i];
        if (!obj->active) {
            continue;
        }
        order = 2;
        drawMode = 0;
        switch (obj->unk28) {
        case 1:
            state = &g_HighwayRiders[obj->unk78].state;
            if (!obj->unk34) {
                obj->unk34 = 1;
                obj->unk38 = 0;
                obj->unk50 = 0;
                obj->unk54 = 0;
            }
            HighwayTrackSample(g_HighwayTrackPos + obj->unk50 + (state->common.z >> 8), state->common.x >> 8,
                               &obj->position, &obj->rotation);
            obj->unk38++;
            obj->unk50 -= 40;
            obj->unk54 += rand() % 20;
            obj->position.vy -= obj->unk54;
            if (obj->unk38 > 10) {
                HighwayObjectFree(obj);
            }
            break;
        case 2:
            if (!obj->unk34) {
                obj->unk34 = 1;
                obj->unk38 = 0;
                obj->unk50 = 0;
                obj->unk54 = 0;
                obj->unk58 = 0;
            }
            obj->unk58 = (obj->unk58 + 1) % 3;
            obj->unk5C = obj->unk58 + 170;
            obj->node->model = g_HighwayModelTable[obj->unk5C];
            obj->unk38++;
            if (obj->unk38 >= 3) {
                HighwayObjectFree(obj);
            }
            break;
        case 3:
            if (!obj->unk34) {
                obj->unk34 = 1;
                obj->unk38 = 0;
                obj->unk50 = 0;
                obj->unk54 = 0;
                obj->unk58 = 0;
            }
            obj->unk58 = (obj->unk58 + 1) % 3;
            obj->unk5C = obj->unk58 + 178;
            obj->node->model = g_HighwayModelTable[obj->unk5C];
            obj->unk38++;
            if (obj->unk38 >= 2) {
                HighwayObjectFree(obj);
            }
            break;
        case 4:
            if (!obj->unk34) {
                obj->unk34 = 1;
                obj->unk38 = 0;
                obj->unk50 = 0;
                obj->unk54 = -30;
                obj->unk58 = 0;
            }
            obj->unk58++;
            obj->rotation.vy += 0x100;
            obj->position.vy += obj->unk54;
            obj->unk54++;
            obj->unk38++;
            if (obj->unk38 > 60) {
                HighwayObjectFree(obj);
            }
            break;
        }
        obj->node->m.t[0] = obj->position.vx;
        obj->node->m.t[1] = obj->position.vy;
        obj->node->m.t[2] = obj->position.vz;
        if (!order) {
            RotMatrixYXZ(&obj->rotation, &obj->node->m);
        }
        if (order == 1) {
            RotMatrixZYX(&obj->rotation, &obj->node->m);
        }
        if (order == 2) {
            RotMatrix(&obj->rotation, &obj->node->m);
        }
        if (!drawMode) {
            HighwayDrawNode(db, obj->node, otIndex, 0);
        }
    }
}

HighwayObject* HighwayObjectSpawn(s16 x, s16 y, s16 z, s16 type, s16 modelId) {
    g_HighwayObjectTemplate.position.vx = x;
    g_HighwayObjectTemplate.position.vy = y;
    g_HighwayObjectTemplate.position.vz = z;
    g_HighwayObjectTemplate.unk28 = type;
    g_HighwayObjectTemplate.unk34 = 1;
    g_HighwayObjectTemplate.unkA8 = 0;
    g_HighwayObjectTemplate.unk2C = modelId;
    return &g_HighwayObjects[HighwayObjectAlloc(&g_HighwayObjectTemplate, 0)];
}
