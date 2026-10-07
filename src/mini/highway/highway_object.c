//! PSYQ=3.3

#include "highway_private.h"

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

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_object", func_800B01AC);

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
