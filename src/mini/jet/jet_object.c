//! PSYQ=3.3 FORCE_MEM=true

#include "jet_private.h"
#include <libc.h>

// JetObjectState.type selects behaviour; the model sets the look.
enum JetObjectType {
    JET_OBJ_FLYER = 1,            // follows its path, turned to face along it
    JET_OBJ_BALLOON = 2,          // rises along its path, leaning to a random tilt
    JET_OBJ_STALACTITE = 4,       // hangs still, then falls
    JET_OBJ_SPINNER = 5,          // follows its path, spinning at a fixed rate
    JET_OBJ_FIREWORK = 8,         // rises, slows, bursts into sparks
    JET_OBJ_FIREWORK_SPARK = 9,   // drifts and spins for 100 frames
    JET_OBJ_CART = 10,            // the player's cart, placed on the track each frame
    JET_OBJ_EXPLOSION = 11,       // spawns a burst of debris, then frees itself
    JET_OBJ_DEBRIS = 12,          // flies out and falls for 100 frames
    JET_OBJ_ERUPTION = 14,        // a flame jet out of the lava that throws up debris
    JET_OBJ_ERUPTION_DEBRIS = 15, // flung up, spins and falls for 200 frames
    JET_OBJ_ERUPTION_FLAME = 16,  // rises out of the lava after 21 frames, then sinks
    JET_OBJ_IMPACT = 202,         // spawned where a shot lands
    JET_OBJ_SCORE_CHECK = 250,    // ends the ride below a score threshold
    JET_OBJ_RIDE_START = 252,     // enables drawing and fades in
    JET_OBJ_RIDE_END = 253,       // fades the screen, then sets g_JetExit
    JET_OBJ_STOP = 254,           // holds g_JetSpeed at 0 for a while, then accelerates
    JET_OBJ_SPEED_CHANGE = 255,   // ends the ride if g_JetSpeed drops below 0
};

extern SVECTOR* D_800A8954;
extern s32 D_800A8984;
extern s32 g_JetNextSpawnSegment;
extern s32 g_JetSpawnIndex;
extern u16 g_JetNextFreeObject;
extern u16 g_JetObjectFreeList[100];
extern JetObject g_JetSpawnTemplate;
extern JetObject g_JetObjects[0x64];
extern s16 g_JetObjectCount;

static s16 JetObjectIndexAlloc(void);
static void JetObjectIndexFree(s16 index);
static void JetObjectCreate(s16 x, s16 y, s16 z, s16 type, s16 modelId);
static void JetObjectDamage(JetObject* object);
static void JetObjectAwardPoints(JetObject* arg0);

const u8 g_JetObjectRotOrder = 0;

void JetObjectsInit(void) {
    JetObject* obj;
    JetObject* pool;
    s32 i;

    obj = g_JetObjects;
    for (i = 0; i < LEN(g_JetObjects); i++) {
        obj[i].index = -1;
        obj[i].active = 0;
    }
    g_JetNextFreeObject = 0;
    for (i = 0; i < LEN(g_JetObjectFreeList); i++) {
        g_JetObjectFreeList[i] = i + 1;
    }
    g_JetShotPower = 128;
    g_JetFiring = 0;
    g_JetShotRepeatCounter = 0;
    g_JetCursorX = 160;
    g_JetCursorY = 120;
    g_JetNextSpawnSegment = 0;
    g_JetSpawnIndex = 0;
    g_JetObjectCount = 0;
}

static void JetObjectPathLoad(u8 pathIndex, u8 mode) {
    s32* lengths;
    s32* offsets;
    s32 offset;

    if (mode == 0) {
        lengths = g_JetXbinAdr.objectPathLengths;
        offsets = g_JetXbinAdr.objectPathOffsets;
        D_800A8984 = lengths[pathIndex];
        offset = offsets[pathIndex];
        D_800A8954 = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
    }
    if (mode == 1) {
        lengths = g_JetXbinAdr.trackPathLengths;
        offsets = g_JetXbinAdr.trackPathOffsets;
        D_800A8984 = lengths[pathIndex];
        offset = offsets[pathIndex];
        D_800A8954 = (SVECTOR*)(g_JetXbinAdr.trackPaths + offset);
    }
}

// Sample a path at a 16.16 position, mirroring y and z when flag is zero.
static void JetPathSample(u32 pathPosition, SVECTOR* path, VECTOR* position, u8 flag) {
    SVECTOR seg[2];
    VECTOR delta;
    s32 idx;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 frac;

    idx = pathPosition >> 16;
    frac = pathPosition & 0xFFFF;
    seg[0].vx = path[idx].vx;
    seg[0].vy = path[idx].vy;
    seg[0].vz = path[idx].vz;
    seg[1].vx = path[idx + 1].vx;
    seg[1].vy = path[idx + 1].vy;
    seg[1].vz = path[idx + 1].vz;
    dx = seg[1].vx - seg[0].vx;
    dy = seg[1].vy - seg[0].vy;
    dz = seg[1].vz - seg[0].vz;
    delta.vx = dx;
    delta.vy = dy;
    delta.vz = dz;
    delta.vx = dx * frac;
    delta.vy = dy * frac;
    delta.vz = dz * frac;
    delta.vx = delta.vx >> 16;
    delta.vy = delta.vy >> 16;
    delta.vz = delta.vz >> 16;
    if (flag == 0) {
        position->vx = path[idx].vx + delta.vx;
        position->vy = -path[idx].vy - delta.vy;
        position->vz = -path[idx].vz - delta.vz;
    } else {
        position->vx = path[idx].vx + delta.vx;
        position->vy = path[idx].vy + delta.vy;
        position->vz = path[idx].vz + delta.vz;
    }
}

// Draw the aiming cursor sprite.
static void JetDrawCursor(JetBuffer* db) {
    POLY_FT4* poly;

    poly = db->prims.ft4Cursor;
    setXY4(poly, g_JetCursorX - 16, g_JetCursorY - 16, g_JetCursorX + 16, g_JetCursorY - 16, g_JetCursorX - 16,
           g_JetCursorY + 16, g_JetCursorX + 16, g_JetCursorY + 16);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0, 0, 0x40, 0, 0, 0x40, 0x40, 0x40);
    poly->tpage = g_JetSpriteTPage[0];
    poly->clut = g_JetSpriteClut[0];
    SetSemiTrans(poly, 0);
    addPrim(&db->ot[1], poly);
    poly++;
    db->prims.ft4Cursor = poly;
}

// Draw the two laser beams, from each gun muzzle to the aiming cursor.
static void JetDrawBeams(void) {
    JetBuffer** db;
    POLY_FT4* poly;
    u8* scroll;
    s32 spread;
    s16* cursorX;
    s16* cursorY;

    if (g_JetFiring != 1) {
        return;
    }
    cursorX = &g_JetCursorX;
    cursorY = &g_JetCursorY;
    db = g_JetBufferPtr;
    scroll = &g_JetBeamScroll;
    spread = g_JetShotPower >> 3;
    poly = db[0]->prims.ft4Cursor;
    setXY4(poly, g_JetBeam0OriginX + spread, g_JetBeam0OriginY, *cursorX, *cursorY, g_JetBeam0OriginX - spread,
           g_JetBeam0OriginY, *cursorX, *cursorY);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0x20 - *scroll, 0, 0x20 - *scroll, 0x40, 0x10 - *scroll, 0, 0x10 - *scroll, 0x40);
    poly->tpage = g_JetSpriteTPage[1];
    poly->clut = g_JetSpriteClut[1];
    SetSemiTrans(poly, 1);
    addPrim(&db[0]->ot[1], poly);
    poly++;
    setXY4(poly, g_JetBeam1OriginX + spread, g_JetBeam1OriginY, *cursorX, *cursorY, g_JetBeam1OriginX - spread,
           g_JetBeam1OriginY, *cursorX, *cursorY);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0x20 - *scroll, 0, 0x20 - *scroll, 0x40, 0x10 - *scroll, 0, 0x10 - *scroll, 0x40);
    poly->tpage = g_JetSpriteTPage[1];
    poly->clut = g_JetSpriteClut[1];
    SetSemiTrans(poly, 1);
    addPrim(&db[0]->ot[1], poly);
    poly++;
    db[0]->prims.ft4Cursor = poly;
}

// Allocate an object and its scene node, then initialise its six bounding box face centres.
static s16 JetObjectAlloc(JetObject* spawn, s16 parentIndex) {
    s16* count;
    JetObject* obj;
    JetObject* pool;
    JetObject* parentObj;
    JetObject* box;
    JetObject* boxPool;
    s16 index;
    s32 rawId;
    s16 modelId;
    s16 maxX;
    s16 minX;
    s16 minY;
    s16 maxY;
    s16 minZ;
    s16 maxZ;

    count = &g_JetObjectCount;
    if (*count < 99) {
        *count = *count + 1;
        index = JetObjectIndexAlloc();
        g_JetObjects[index] = *spawn;
        pool = g_JetObjects;
        obj = &pool[index];
        rawId = spawn->state.modelId;
        modelId = rawId;
        obj->active = 1;
        obj->index = index;
        if (parentIndex == 0) {
            obj->node = JetNodeAlloc(rawId, 0, 0, 1, &g_JetRootNode, spawn->position.vx, spawn->position.vy,
                                     spawn->position.vz, spawn->rotation.vx, spawn->rotation.vy, spawn->rotation.vz);
        } else {
            parentObj = &pool[parentIndex];
            obj->node = JetNodeAlloc(rawId, 0, 0, 1, parentObj->node, spawn->position.vx, spawn->position.vy,
                                     spawn->position.vz, spawn->rotation.vx, spawn->rotation.vy, spawn->rotation.vz);
        }
        minX = g_JetModelInfo[modelId].boundsMin.vx;
        maxX = g_JetModelInfo[modelId].boundsMax.vx;
        minY = g_JetModelInfo[modelId].boundsMin.vy;
        maxY = g_JetModelInfo[modelId].boundsMax.vy;
        minZ = g_JetModelInfo[modelId].boundsMin.vz;
        maxZ = g_JetModelInfo[modelId].boundsMax.vz;
        boxPool = g_JetObjects;
        box = &boxPool[index];
        setVector(&box->unkDC[0], (maxX + minX) >> 1, (maxY + minY) >> 1, maxZ);
        setVector(&box->unkDC[1], (maxX + minX) >> 1, (maxY + minY) >> 1, minZ);
        setVector(&box->unkDC[2], (maxX + minX) >> 1, maxY, (maxZ + minZ) >> 1);
        setVector(&box->unkDC[3], (maxX + minX) >> 1, minY, (maxZ + minZ) >> 1);
        setVector(&box->unkDC[4], maxX, (maxY + minY) >> 1, (maxZ + minZ) >> 1);
        setVector(&box->unkDC[5], minX, (maxY + minY) >> 1, (maxZ + minZ) >> 1);
    }
    return index;
}

static void JetObjectRelease(JetObject* object) {
    s16* count;

    if (object->index != -1) {
        count = &g_JetObjectCount;
        *count -= 1;
        JetNodeFree(object->node);
        JetObjectIndexFree(object->index);
        object->index = -1;
        object->active = 0;
    }
}

static s16 JetObjectIndexAlloc(void) {
    u16* head;
    s16 index;

    head = &g_JetNextFreeObject;
    index = *head;
    *head = g_JetObjectFreeList[index];

    return index;
}

static void JetObjectIndexFree(s16 index) {
    u16* head;
    u16* slot;

    slot = &g_JetObjectFreeList[index];
    head = &g_JetNextFreeObject;
    *slot = *head;
    *head = index;
}

// Spawn the objects scheduled for every track segment reached this frame.
static void JetObjectsSpawnScheduled(void) {
    JetObjectSpawn* spawns;
    u8* counts;
    u8* count;
    s32 segment;
    s32 index;
    s32 i;
    s32 j;

    for (segment = g_JetNextSpawnSegment; segment < g_JetTrackSegment; segment++) {
        counts = g_JetXbinAdr.spawnCounts;
        count = counts + segment;
        for (i = 0; i < *count; i++) {
            spawns = g_JetXbinAdr.spawns;
            for (j = 0; j < LEN(g_JetSpawnTemplate.state.params.raw); j++) {
                g_JetSpawnTemplate.state.params.raw[j] = spawns[*(s32*)(u_long)&g_JetSpawnIndex].params[j];
            }
            index = *(s32*)(u_long)&g_JetSpawnIndex;
            g_JetSpawnTemplate.state.pathIndex = spawns[index].pathIndex;
            g_JetSpawnTemplate.state.speed = spawns[index].speed;
            JetObjectCreate(0, 0, 0, spawns[index].type, spawns[index].modelId);
            (*(s32*)(u_long)&g_JetSpawnIndex)++;
        }
    }
    g_JetNextSpawnSegment = g_JetTrackSegment;
}

static void JetObjectCreate(s16 x, s16 y, s16 z, s16 type, s16 modelId) {
    g_JetSpawnTemplate.position.vx = x;
    g_JetSpawnTemplate.position.vy = y;
    g_JetSpawnTemplate.position.vz = z;
    g_JetSpawnTemplate.state.type = type;
    g_JetSpawnTemplate.state.needsInit = 1;
    g_JetSpawnTemplate.state.modelId = modelId;
    g_JetSpawnTemplate.state.hit = 0;
    JetObjectAlloc(&g_JetSpawnTemplate, 0);
}

inline void JetObjectCreateUnscheduled(s16 x, s16 y, s16 z, s16 type, s16 modelId) {
    g_JetSpawnTemplate.position.vx = x;
    g_JetSpawnTemplate.position.vy = y;
    g_JetSpawnTemplate.position.vz = z;
    g_JetSpawnTemplate.state.type = type;
    g_JetSpawnTemplate.state.needsInit = 1;
    g_JetSpawnTemplate.state.modelId = modelId;
    g_JetSpawnTemplate.state.hit = 0;
    g_JetSpawnTemplate.state.params.raw[0xC] = 0;
    JetObjectAlloc(&g_JetSpawnTemplate, 0);
}

static inline void JetObjectFree(JetObject* object) {
    if (object->index != -1) {
        g_JetObjectCount--;
        JetNodeFree(object->node);
        JetObjectIndexFree(object->index);
        object->index = -1;
        object->active = 0;
    }
}

// Step every live object through its behaviour, then queue its model.
void JetObjectsUpdate(JetBuffer* db) {
    VECTOR next;
    VECTOR unused[2];
    VECTOR pos;
    SVECTOR rot;
    JetObject* obj;
    JetObject* pool;
    JetObjectState* st;
    POLY_G4* fade;
    POLY_FT4* tpagePrim;
    s16 pathIndex;
    s32 shade;
    s32 count;
    s32 i;
    s32 j;
    s32 dx;
    s32 dy;
    s32 dz;
    s32* score;
    s32* segment;
    s32 sound;
    u8 order;
    u8 drawMode;
    u16 otIndex;

    JetObjectsSpawnScheduled();
    JetDrawCursor(db);
    JetDrawBeams();
    for (i = 0; i < LEN(g_JetObjects); i++) {
        otIndex = 0;
        pool = g_JetObjects;
        obj = &pool[i];
        st = &obj->state;
        if (obj->active == 0) {
            continue;
        }
        drawMode = 0;
        do {
        } while (0);
        switch (obj->state.type) {
        case 100:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->age = 0;
                st->hit = 0;
                st->params.raw[10] = 1;
                st->unk28 = 0;
                st->unk2C = obj->position.vx;
                st->unk30 = obj->position.vy;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                st->unk34 = obj->position.vz;
                st->params.raw[0] = 0;
            } else {
                st->age++;
                st->unk28++;
            }
            if (st->life == 0) {
                JetObjectFree(obj);
                break;
            }
            {
                s32 step;
                s32* pathPos;
                s32 x;
                s32 y;
                s32 z;

                step = st->unk28;
                pathPos = &g_JetCameraPathPos;
                JetTrackSample(pathPos[0] + 0x2FFFD, -100, &pos, &rot);
                x = st->unk2C;
                x += (step * (pos.vx - x)) >> 7;
                y = st->unk30;
                y += (step * (pos.vy - y)) >> 7;
                z = st->unk34;
                z += (step * (pos.vz - z)) >> 7;
                obj->position.vx = x;
                obj->position.vy = y;
                obj->position.vz = z;
                JetTrackSample(pathPos[0] + 0x3FFFC, -100, &pos, &rot);
            }
            dx = obj->position.vx - pos.vx;
            dy = obj->position.vy - pos.vy;
            dz = obj->position.vz - pos.vz;
            SquareRoot0(dx * dx + dy * dy + dz * dz);
            if (st->age >= 0x81) {
                score = &g_JetScore;
                if (*score > 5) {
                    *score -= 5;
                } else {
                    *score = 0;
                }
                st->life = 0;
            }
            if (st->hit) {
                JetObjectDamage(obj);
            }
            break;
        case 17:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                sound = st->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->age = 0;
                st->hit = 0;
                st->unk28 = 0;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                st->unk2C = (obj->pathLen - 2) << 16;
            } else {
                st->age++;
            }
            segment = &g_JetTrackSegment;
            if (st->params.raw[3] < *segment) {
                st->unk28 += st->speed;
            }
            if (st->params.common.endSegment < *segment) {
                st->life = 0;
            }
            if (st->life == 0) {
                JetObjectFree(obj);
                break;
            }
            if (st->hit) {
                JetObjectDamage(obj);
            }
            if (st->unk28 < st->unk2C) {
                JetPathSample(st->unk28, obj->path, &obj->position, 0);
                obj->rotation.vx = 0;
                obj->rotation.vy = 0;
                obj->rotation.vz = 0;
            }
            break;
        case 0:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                sound = st->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->age = 0;
                st->hit = 0;
                st->unk28 = 0;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                st->unk2C = (obj->pathLen - 1) << 16;
            } else {
                st->age++;
            }
            st->unk28 += st->speed;
            if (st->params.common.loopPath == 1) {
                st->unk28 %= st->unk2C;
            }
            if (st->unk28 > st->unk2C) {
                st->life = 0;
            }
            if (st->params.common.endSegment < g_JetTrackSegment) {
                st->life = 0;
            }
            if (st->life == 0) {
                JetObjectFree(obj);
                break;
            }
            JetPathSample(st->unk28, obj->path, &obj->position, 0);
            obj->rotation.vx += st->params.raw[3];
            obj->rotation.vy += st->params.raw[4];
            obj->rotation.vz += st->params.raw[5];
            if (st->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_FLYER:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                sound = st->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->age = 0;
                st->hit = 0;
                st->unk28 = 0;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                st->unk2C = (obj->pathLen - 1) << 16;
            } else {
                st->age++;
            }
            st->unk28 += st->speed;
            if (st->params.common.loopPath == 1) {
                st->unk28 %= st->unk2C;
            }
            if (st->unk28 > st->unk2C) {
                st->life = 0;
            }
            if (st->params.common.endSegment < g_JetTrackSegment) {
                st->life = 0;
            }
            if (st->life == 0) {
                JetObjectFree(obj);
                break;
            }
            JetPathSample(st->unk28, obj->path, &obj->position, 0);
            JetPathSample((st->unk28 + 0x10000) % ((obj->pathLen - 1) << 16), obj->path, &next, 0);
            dx = next.vx - obj->position.vx;
            dy = next.vy - obj->position.vy;
            dz = next.vz - obj->position.vz;
            obj->rotation.vx = ratan2(dy, SquareRoot0(dx * dx + dz * dz));
            obj->rotation.vy = -ratan2(dz, dx) - 0x400;
            obj->rotation.vz = 0;
            if (st->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_CART:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                sound = st->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                pathLen = g_JetXbinAdr.trackPathLengths[0];
                offset = g_JetXbinAdr.trackPathOffsets[0];
                path = (SVECTOR*)(g_JetXbinAdr.trackPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->age = 0;
                st->hit = 0;
                st->unk28 = 0;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                st->unk2C = (obj->pathLen - 1) << 16;
            } else {
                st->age++;
            }
            st->unk28 += st->speed;
            if (st->params.common.loopPath == 1) {
                st->unk28 %= st->unk2C;
            }
            if (st->unk28 > st->unk2C) {
                st->life = 0;
            }
            if (st->params.common.endSegment < g_JetTrackSegment) {
                st->life = 0;
            }
            if (st->life == 0) {
                JetObjectFree(obj);
                break;
            }
            JetTrackSample(g_JetCameraPathPos + 0x3FFFC, 10, &obj->position, &obj->rotation);
            drawMode = 1;
            if (st->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_STALACTITE:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                sound = st->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->hit = 0;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                JetPathSample(0, obj->path, &obj->position, 0);
            }
            segment = &g_JetTrackSegment;
            if (st->params.common.endSegment < *segment) {
                st->life = 0;
            }
            if (st->params.stalactite.fallSegment < *segment) {
                st->unk2C += 4;
            }
            if (st->life == 0) {
                JetObjectFree(obj);
                break;
            }
            obj->position.vy += st->unk2C;
            if (obj->position.vy > 0) {
                st->life = 0;
            }
            if (st->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_SPINNER:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                sound = st->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->hit = 0;
                st->unk28 = 0;
                st->unk2C = (obj->pathLen - 1) << 16;
                obj->rotation.vx = st->params.spinner.startRot[0];
                obj->rotation.vy = st->params.spinner.startRot[1];
                D_800A8984 = pathLen;
                D_800A8954 = path;
                obj->rotation.vz = st->params.spinner.startRot[2];
            }
            st->unk28 += st->speed;
            if (st->params.common.loopPath == 1) {
                st->unk28 %= st->unk2C;
            }
            if (st->unk28 > st->unk2C) {
                st->life = 0;
            }
            if (st->params.common.endSegment < g_JetTrackSegment) {
                st->life = 0;
            }
            obj->rotation.vx += st->params.spinner.rotStep[0];
            obj->rotation.vy += st->params.spinner.rotStep[1];
            obj->rotation.vz += st->params.spinner.rotStep[2];
            if (st->params.raw[10] == 5) {
                obj->node->model = g_JetModelTable[91 + st->params.raw[14]];
            }
            if (st->params.raw[14] == 1) {
                st->params.raw[14] = 0;
            }
            if (st->life) {
                JetPathSample(st->unk28, obj->path, &obj->position, 0);
                if (st->hit) {
                    if (st->params.raw[10] != 5 || g_JetSpeed < 16405) {
                        JetObjectDamage(obj);
                    }
                    if (st->params.raw[10] == 5) {
                        st->params.raw[14] = 1;
                    }
                }
            } else {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_BALLOON:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                sound = st->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                D_800A8984 = pathLen;
                D_800A8954 = path;
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->hit = 0;
                st->unk28 = (rand() % st->params.balloon.tiltRange) * 2 - st->params.balloon.tiltRange - 1;
                st->unk2C = 0;
                st->unk30 = 0;
                st->unk34 = (obj->pathLen - 1) << 16;
                obj->rotation.vx = 0;
                obj->rotation.vy = 0;
                obj->rotation.vz = 0;
                JetPathSample(0, obj->path, &obj->position, 0);
            }
            st->unk30 += st->speed;
            if (st->unk34 < st->unk30) {
                st->life = 0;
            }
            if (st->params.common.endSegment < g_JetTrackSegment) {
                st->life = 0;
            }
            if (st->unk2C > st->unk28) {
                st->unk2C -= 5;
            }
            if (st->unk2C < st->unk28) {
                st->unk2C += 5;
            }
            obj->rotation.vx = st->unk2C;
            obj->rotation.vy += st->params.balloon.yawStep;
            if (st->life == 0) {
                JetObjectFree(obj);
                break;
            }
            JetPathSample(st->unk30, obj->path, &obj->position, 0);
            if (st->hit) {
                JetObjectDamage(obj);
            }
            break;
        case 230:
            if (st->needsInit == 1) {
                st->needsInit = 0;
                st->life = 1;
                st->hit = 0;
            }
            if (st->params.common.endSegment < g_JetTrackSegment) {
                st->life = 0;
            }
            if (st->life) {
                if (st->hit) {
                    JetObjectDamage(obj);
                }
            } else {
                JetObjectFree(obj);
            }
            break;
        case 7:
        case 13:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->hit = 0;
                obj->rotation.vx = st->params.raw[3];
                obj->rotation.vy = st->params.raw[4];
                obj->rotation.vz = 0;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                JetPathSample(0, obj->path, &obj->position, 0);
                st->unk28 = 0;
            }
            segment = &g_JetTrackSegment;
            if (st->params.common.endSegment < *segment) {
                st->life = 0;
            }
            if (st->params.raw[5] < *segment) {
                count = st->unk28;
                st->unk28 = count + 1;
                if (count < st->params.raw[7]) {
                    obj->rotation.vx += st->params.raw[6];
                }
            }
            if (st->life) {
                if (st->hit) {
                    JetObjectDamage(obj);
                }
            } else {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_EXPLOSION:
            JetPlaySfx(SFX_FIRAGA);
            for (j = 0; j < st->params.explosion.debrisCount; j++) {
                JetObjectCreateUnscheduled(13382, -10000, 8395, JET_OBJ_DEBRIS, 42);
            }
            JetObjectFree(obj);
            // falls through into the debris behaviour below
        case JET_OBJ_DEBRIS:
            if (st->needsInit == 1) {
                st->hit = 0;
                st->life = 100;
                st->needsInit = 0;
                st->age = 0;
                st->unk28 = rand() % 60 - 30;
                st->unk2C = -rand() % 200;
                st->unk30 = rand() % 60 - 30;
            } else {
                st->age++;
            }
            st->unk2C++;
            obj->position.vx += st->unk28;
            obj->position.vy += st->unk2C;
            obj->position.vz += st->unk30;
            obj->rotation.vx += 10;
            obj->rotation.vy += 400;
            obj->rotation.vz += 200;
            st->life--;
            if (st->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_FIREWORK:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->hit = 0;
                st->unk28 = 0;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                JetPathSample(0, obj->path, &obj->position, 0);
            }
            if (st->params.common.endSegment < g_JetTrackSegment) {
                st->life = 0;
            }
            if (st->life == 0) {
                JetObjectFree(obj);
                break;
            }
            obj->position.vy -= st->params.firework.riseSpeed;
            st->params.firework.riseSpeed -= st->params.firework.riseDecel;
            if (st->params.firework.riseSpeed < 0) {
                JetPlaySfx(SFX_DETONANTE_ECHO);
                for (j = 0; j < 20; j++) {
                    s32 x;
                    s32 y;
                    s32 z;

                    x = obj->position.vx;
                    y = obj->position.vy;
                    z = obj->position.vz;
                    JetObjectCreateUnscheduled(x, y, z, JET_OBJ_FIREWORK_SPARK, rand() % 3 + 68);
                }
                st->life = 0;
            }
            break;
        case JET_OBJ_FIREWORK_SPARK:
            if (st->needsInit == 1) {
                st->hit = 0;
                st->life = 100;
                st->needsInit = 0;
                st->age = 0;
                st->unk28 = rand() % 60 - 30;
                st->unk2C = rand() % 60 - 30;
                st->unk30 = rand() % 60 - 30;
            } else {
                st->age++;
            }
            obj->position.vx += st->unk28;
            obj->position.vy += st->unk2C;
            obj->position.vz += st->unk30;
            obj->rotation.vx += 10;
            obj->rotation.vy += 400;
            obj->rotation.vz += 200;
            st->life--;
            if (st->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_ERUPTION:
            if (st->needsInit == 1) {
                SVECTOR* path;
                s32 pathLen;
                s32 offset;

                JetPlaySfx(SFX_FIRA);
                pathIndex = st->pathIndex & 0xFF;
                pathLen = g_JetXbinAdr.objectPathLengths[pathIndex];
                offset = g_JetXbinAdr.objectPathOffsets[pathIndex];
                path = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
                obj->path = path;
                obj->pathLen = pathLen;
                st->needsInit = 0;
                st->life = 1;
                st->hit = 0;
                st->age = 0;
                st->unk28 = -0x46;
                D_800A8984 = pathLen;
                D_800A8954 = path;
                JetPathSample(0, obj->path, &obj->position, 0);
            }
            st->unk28++;
            st->age++;
            if (st->age == 5) {
                for (j = 0; j < st->params.eruption.debrisCount; j++) {
                    s32 x;
                    s32 y;
                    s32 z;

                    x = obj->position.vx + rand() % 100 - 0x32;
                    y = obj->position.vy + 0x1F4;
                    z = obj->position.vz + rand() % 100 - 0x32;
                    JetObjectCreateUnscheduled(x, y, z, JET_OBJ_ERUPTION_DEBRIS, 42);
                }
                {
                    s32 x;
                    s32 y;
                    s32 z;

                    x = obj->position.vx;
                    y = obj->position.vy;
                    z = obj->position.vz;
                    JetObjectCreateUnscheduled(x, y, z, JET_OBJ_ERUPTION_FLAME, 41);
                }
            }
            if (st->unk28 < 0) {
                obj->rotation.vy += 20;
            }
            if (st->unk28 == 0x50) {
                st->life = 0;
            }
            obj->position.vy += st->unk28;
            if (st->params.common.endSegment < g_JetTrackSegment) {
                st->life = 0;
            }
            if (st->life) {
                if (st->hit) {
                    JetObjectDamage(obj);
                }
            } else {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_ERUPTION_DEBRIS:
            if (st->needsInit == 1) {
                st->hit = 0;
                st->life = 200;
                st->needsInit = 0;
                st->age = 0;
                st->unk28 = rand() % 80 - 40;
                st->unk2C = rand() % 80 - 40;
                st->unk30 = -(rand() % 40 + 40);
            } else {
                st->age++;
            }
            st->unk30++;
            obj->rotation.vx += 480;
            obj->rotation.vy += 40;
            obj->rotation.vz += 610;
            obj->position.vx += st->unk28;
            obj->position.vy += st->unk30;
            obj->position.vz += st->unk2C;
            st->life--;
            if (st->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_ERUPTION_FLAME:
            if (st->needsInit == 1) {
                st->life = 200;
                st->hit = 0;
                st->needsInit = 0;
                st->age = 0;
                st->unk28 = -0x3C;
            } else {
                st->age++;
            }
            if (st->age >= 0x15) {
                obj->position.vy += st->unk28;
                st->unk28++;
            }
            st->life--;
            if (st->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case 3:
            obj->position.vx = g_JetCameraPosCopy.vx;
            obj->position.vy = g_JetCameraPosCopy.vy - 0x9C4;
            obj->position.vz = g_JetCameraPosCopy.vz;
            otIndex = 0x3E8;
            obj->rotation.vx = 0;
            obj->rotation.vy = 0;
            obj->rotation.vz = 0;
            break;
        case 201:
            if (st->needsInit == 1) {
                st->life = 0x14;
                st->needsInit = 0;
                st->age = 0;
                st->hit = 0;
            } else {
                st->age++;
            }
            {
                s32 x;
                s32 y;
                s32 z;

                x = obj->position.vx;
                y = obj->position.vy;
                z = obj->position.vz;
                JetObjectCreateUnscheduled(x, y, z, JET_OBJ_IMPACT, 42);
            }
            st->life--;
            if (st->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_IMPACT:
            if (st->needsInit == 1) {
                st->hit = 0;
                st->life = 0x32;
                st->needsInit = 0;
                st->age = 0;
                st->unk28 = rand() % 20 - 10;
                st->unk2C = rand() % 40 - 20;
                st->unk30 = rand() % 20 - 10;
            } else {
                st->age++;
            }
            obj->position.vx += st->unk28;
            obj->position.vy += st->unk2C;
            obj->position.vz += st->unk30;
            obj->rotation.vx += 10;
            obj->rotation.vy += 100;
            obj->rotation.vz += 20;
            st->life--;
            if (st->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case 203:
            if (st->needsInit == 1) {
                st->hit = 0;
                st->life = 0x32;
                st->needsInit = 0;
                st->age = 0;
                st->unk28 = rand() % 200 - 100;
                st->unk2C = rand() % 200 - 100;
                st->unk30 = rand() % 200 - 100;
            } else {
                st->age++;
            }
            obj->position.vx += st->unk28;
            obj->position.vy += st->unk2C;
            obj->position.vz += st->unk30;
            obj->rotation.vx += 0;
            obj->rotation.vy += 300;
            obj->rotation.vz += 0;
            st->life--;
            if (st->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_SPEED_CHANGE:
            if (st->needsInit == 1) {
                st->needsInit = 0;
                st->age = 0;
            } else {
                st->age++;
            }
            {
                s32* speed;

                speed = &g_JetSpeed;
                if (*speed > st->params.speedChange.minSpeed) {
                    *speed -= st->params.speedChange.step;
                }
                if (*speed < 0) {
                    *speed = 0;
                    JetObjectCreateUnscheduled(0, 0, 0, JET_OBJ_RIDE_END, 29);
                }
            }
            if (st->age > st->params.speedChange.frames) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_STOP:
            if (st->needsInit == 1) {
                st->needsInit = 0;
                st->age = 0;
                st->unk28 = 0;
                st->unk2C = VSync(-1);
                st->unk30 = 0;
            } else {
                st->age++;
            }
            if (st->unk28 == 0) {
                g_JetSpeed = 0;
            }
            if (st->unk28 == 1) {
                s32* speed;

                speed = &g_JetSpeed;
                *speed += st->params.stop.accel;
                st->unk30++;
            }
            if (st->params.stop.delay < VSync(-1) - st->unk2C) {
                st->unk28 = 1;
            }
            if (st->unk30 > st->params.stop.accelFrames) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_RIDE_START:
            if (st->needsInit == 1) {
                st->needsInit = 0;
                st->age = 0;
                g_JetSpeed = 0;
                g_JetDrawEnabled = 1;
                JetObjectCreateUnscheduled(0, 0, 0, 3, 59);
            } else {
                st->age++;
            }
            shade = ~(st->age * 2);
            fade = db->prims.g4Cursor;
            setXY4(fade, 0, 0, 320, 0, 0, 240, 320, 240);
            setRGB0(fade, shade, shade, shade);
            setRGB1(fade, shade, shade, shade);
            setRGB2(fade, shade, shade, shade);
            setRGB3(fade, shade, shade, shade);
            SetSemiTrans(fade, 1);
            addPrim(&db->ot2[0], fade);
            fade++;
            db->prims.g4Cursor = fade;
            tpagePrim = db->prims.ft4Cursor;
            setRGB0(tpagePrim, 0, 0, 0);
            setXY4(tpagePrim, 0, 0, 0, 0, 0, 0, 0, 0);
            tpagePrim->tpage = g_JetFadeTPage;
            tpagePrim->clut = g_JetFadeClut;
            SetSemiTrans(tpagePrim, 0);
            addPrim(&db->ot2[1], tpagePrim);
            tpagePrim++;
            db->prims.ft4Cursor = tpagePrim;
            if (st->age >= 0x7E) {
                JetObjectFree(obj);
                g_JetSpeed = 16384;
            }
            break;
        case JET_OBJ_RIDE_END:
            if (st->needsInit == 1) {
                st->needsInit = 0;
                st->age = 0;
                g_JetSpeed = 0;
                JetAudioFadeOut();
            } else {
                st->age++;
            }
            shade = st->age * 2;
            fade = db->prims.g4Cursor;
            setXY4(fade, 0, 0, 320, 0, 0, 240, 320, 240);
            setRGB0(fade, shade, shade, shade);
            setRGB1(fade, shade, shade, shade);
            setRGB2(fade, shade, shade, shade);
            setRGB3(fade, shade, shade, shade);
            SetSemiTrans(fade, 1);
            addPrim(&db->ot2[0], fade);
            fade++;
            db->prims.g4Cursor = fade;
            tpagePrim = db->prims.ft4Cursor;
            setRGB0(tpagePrim, 0, 0, 0);
            setXY4(tpagePrim, 0, 0, 0, 0, 0, 0, 0, 0);
            tpagePrim->tpage = g_JetFadeTPage;
            tpagePrim->clut = g_JetFadeClut;
            SetSemiTrans(tpagePrim, 0);
            addPrim(&db->ot2[1], tpagePrim);
            tpagePrim++;
            db->prims.ft4Cursor = tpagePrim;
            if (st->age >= 0x80) {
                JetObjectFree(obj);
                g_JetSpeed = 16384;
                g_JetDrawEnabled = 0;
                g_JetExit = 1;
            }
            break;
        case JET_OBJ_SCORE_CHECK:
            if (g_JetScore < st->params.scoreCheck.minScore) {
                JetObject* spawn;

                spawn = &g_JetSpawnTemplate;
                spawn->state.params.speedChange.step = 300;
                spawn->state.params.speedChange.frames = 400;
                spawn->state.params.speedChange.minSpeed = 0;
                {
                    s32 x;
                    s32 y;
                    s32 z;

                    x = obj->position.vx;
                    y = obj->position.vy;
                    z = obj->position.vz;
                    JetObjectCreateUnscheduled(x, y, z, JET_OBJ_SPEED_CHANGE, 30);
                }
            }
            JetObjectFree(obj);
            break;
        }
        obj->node->m.t[0] = obj->position.vx;
        obj->node->m.t[1] = obj->position.vy;
        obj->node->m.t[2] = obj->position.vz;
        order = g_JetObjectRotOrder;
        if (order == 0) {
            RotMatrixYXZ(&obj->rotation, &obj->node->m);
        }
        if (order == 1) {
            RotMatrixZYX(&obj->rotation, &obj->node->m);
        }
        if (order == 2) {
            RotMatrix(&obj->rotation, &obj->node->m);
        }
        if (drawMode == 0) {
            JetDrawObjectAndCheckHit(db, obj->node, otIndex, 0, obj);
        }
        if (drawMode == 1) {
            JetDrawCartAndProjectBeams(db, obj->node, otIndex, 0, obj);
        }
    }
}

static void JetObjectDamage(JetObject* object) {
    JetObjectState* state = &object->state;
    u8 amount;
    s32 x;
    s32 y;
    s32 z;

    amount = g_JetShotPower >> 5;
    if (amount == 0) {
        amount = 1;
    }
    state->params.common.health -= amount;
    if (state->params.common.health < 0) {
        JetObjectAwardPoints(object);
    } else {
        x = object->position.vx;
        y = object->position.vy;
        z = object->position.vz;
        JetObjectCreateUnscheduled(x, y, z, JET_OBJ_IMPACT, 63);
    }
}

// Award the score for a hit object and scatter its debris.
static void JetObjectAwardPoints(JetObject* obj) {
    JetObjectState* st = &obj->state;
    s32* score;
    s32* frame;
    SVECTOR* path;
    s32 pathLen;
    s32 sound;
    s32 i;
    s16 modelId;

    if (st->params.raw[10] == 1) {
        s32* score;
        s32 points;
        s32 x;
        s32 y;
        s32 z;

        score = &g_JetScore;
        *score += st->params.common.points;
        JetPlaySfx(st->params.common.deathSfx);
        st->life = 0;
        for (i = 0; i < 3; i++) {
            x = obj->position.vx;
            y = obj->position.vy;
            z = obj->position.vz;
            JetObjectCreateUnscheduled(x, y, z, JET_OBJ_IMPACT, rand() % 3 + 63);
        }
        g_JetPopupModelId = obj->node->modelId;
        points = st->params.common.points;
        g_JetPopupPoints = points;
        g_JetPopupTimer = 100;
        g_JetScorePopupAlternate = 1;
        setVector(&g_JetPopupRot, 0, 0, 0);
    }
    if (st->params.raw[10] == 2) {
        s32* score;
        s32 points;
        s32 x;
        s32 y;
        s32 z;

        score = &g_JetScore;
        *score += st->params.common.points;
        JetPlaySfx(st->params.common.deathSfx);
        st->life = 0;
        for (i = 0; i < 3; i++) {
            x = obj->position.vx;
            y = obj->position.vy;
            z = obj->position.vz;
            JetObjectCreateUnscheduled(x, y, z, 0xCB, rand() % 3 + 60);
        }
        g_JetPopupModelId = obj->node->modelId;
        points = st->params.common.points;
        g_JetPopupPoints = points;
        g_JetPopupTimer = 100;
        g_JetScorePopupAlternate = 1;
        setVector(&g_JetPopupRot, 0, 0, 0);
    }
    if (st->params.raw[10] == 3) {
        s32* score;
        s32 points;

        score = &g_JetScore;
        *score += st->params.common.points;
        points = st->params.raw[11];
        obj->rotation.vx += points;
    }
    if (st->params.raw[10] == 4) {
        s32* score;
        s32 points;
        score = &g_JetScore;
        *score += st->params.common.points;
        JetPlaySfx(st->params.common.deathSfx);
        st->life = 0;
        g_JetPopupModelId = obj->node->modelId;
        points = st->params.common.points;
        g_JetPopupPoints = points;
        g_JetPopupTimer = 100;
        g_JetScorePopupAlternate = 1;
        setVector(&g_JetPopupRot, 0, 0, 0);
    }
    if (st->params.raw[10] == 5) {
        s32* score;
        s32 points;
        s32 x;
        s32 y;
        s32 z;

        score = &g_JetScore;
        *score += st->params.common.points;
        JetPlaySfx(st->params.common.deathSfx);
        st->life = 0;
        for (i = 0; i < 100; i++) {
            x = obj->position.vx;
            y = obj->position.vy;
            z = obj->position.vz;
            JetObjectCreateUnscheduled(x, y, z, 0xCB, rand() % 3 + 63);
        }
        g_JetPopupModelId = obj->node->modelId;
        points = st->params.common.points;
        g_JetPopupPoints = points;
        g_JetPopupTimer = 100;
        g_JetScorePopupAlternate = 1;
        setVector(&g_JetPopupRot, 0, 0, 0);
    }
    score = &g_JetScore;
    if (*score > 9999) {
        *score = 9999;
    }
}
