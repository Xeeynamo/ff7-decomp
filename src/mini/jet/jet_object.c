//! PSYQ=3.3 FORCE_MEM=true FORCE_ADDR=true

#include "jet_private.h"
#include <libc.h>

#define JET_TRACK_SEGMENT 0xFFFF

// JetObjectState.type selects behaviour; the model sets the look.
enum JetObjectType {
    JET_OBJ_ROTATOR = 0,          // rotates at a fixed per-axis rate, optionally moving along its path
    JET_OBJ_FLYER = 1,            // follows its path, turned to face along it
    JET_OBJ_BALLOON = 2,          // rises along its path, leaning to a random tilt
    JET_OBJ_STARFIELD = 3,        // follows the camera, drawn behind everything
    JET_OBJ_STALACTITE = 4,       // hangs still, then falls
    JET_OBJ_SPINNER = 5,          // follows its path, spinning at a fixed rate
    JET_OBJ_FLIP_UNUSED = 7,      // same behaviour as JET_OBJ_FLIP; not used by the ride
    JET_OBJ_FIREWORK = 8,         // rises, slows, bursts into sparks
    JET_OBJ_FIREWORK_SPARK = 9,   // drifts and spins for 100 frames
    JET_OBJ_CART = 10,            // the player's cart, placed on the track each frame
    JET_OBJ_EXPLOSION = 11,       // spawns a burst of debris, then frees itself
    JET_OBJ_DEBRIS = 12,          // flies out and falls for 100 frames
    JET_OBJ_FLIP = 13,            // holds still, then flips about x once the ride passes a segment
    JET_OBJ_ERUPTION = 14,        // a flame jet out of the lava that throws up debris
    JET_OBJ_ERUPTION_DEBRIS = 15, // flung up, spins and falls for 200 frames
    JET_OBJ_ERUPTION_FLAME = 16,  // rises out of the lava after 21 frames, then sinks
    JET_OBJ_TRIGGERED = 17,       // waits at its path start until a track segment, then follows the path once
    JET_OBJ_INCOMING = 100,       // flies at the camera; costs 5 points unless shot in time
    JET_OBJ_IMPACT_BURST = 201,   // spawns an impact every frame for 20 frames
    JET_OBJ_IMPACT = 202,         // spawned where a shot lands
    JET_OBJ_FRAGMENT = 203,       // flung fast out of a destroyed target for 50 frames
    JET_OBJ_STATIC = 230,         // stays where it spawned; can be shot
    JET_OBJ_SCORE_CHECK = 250,    // ends the ride below a score threshold
    JET_OBJ_RIDE_START = 252,     // enables drawing and fades in
    JET_OBJ_RIDE_END = 253,       // fades the screen, then sets g_JetExit
    JET_OBJ_STOP = 254,           // holds g_JetSpeed at 0 for a while, then accelerates
    JET_OBJ_SPEED_CHANGE = 255,   // ends the ride if g_JetSpeed drops below 0
};

enum JetPathType {
    JET_PATH_OBJECT,
    JET_PATH_TRACK,
};

extern SVECTOR* g_JetLastPath;
extern s32 g_JetLastPathLen;
extern s32 g_JetNextSpawnSegment;
extern s32 g_JetSpawnIndex;
extern u16 g_JetNextFreeObject;
extern u16 g_JetObjectFreeList[100];
extern JetObject g_JetSpawnTemplate;
extern JetObject g_JetObjects[100];
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

inline void JetObjectPathLoad(u8 pathIndex, u8 mode) {
    s32* lengths;
    s32* offsets;
    s32 offset;

    if (mode == JET_PATH_OBJECT) {
        lengths = g_JetXbinAdr.objectPathLengths;
        offsets = g_JetXbinAdr.objectPathOffsets;
        g_JetLastPathLen = lengths[pathIndex];
        offset = offsets[pathIndex];
        g_JetLastPath = (SVECTOR*)(g_JetXbinAdr.objectPaths + offset);
    }
    if (mode == JET_PATH_TRACK) {
        lengths = g_JetXbinAdr.trackPathLengths;
        offsets = g_JetXbinAdr.trackPathOffsets;
        g_JetLastPathLen = lengths[pathIndex];
        offset = offsets[pathIndex];
        g_JetLastPath = (SVECTOR*)(g_JetXbinAdr.trackPaths + offset);
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
    s32 spread;

    if (g_JetFiring != 1) {
        return;
    }
    db = g_JetBufferPtr;
    spread = g_JetShotPower >> 3;
    poly = db[0]->prims.ft4Cursor;
    setXY4(poly, g_JetBeam0OriginX + spread, g_JetBeam0OriginY, g_JetCursorX, g_JetCursorY, g_JetBeam0OriginX - spread,
           g_JetBeam0OriginY, g_JetCursorX, g_JetCursorY);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0x20 - g_JetBeamScroll, 0, 0x20 - g_JetBeamScroll, 0x40, 0x10 - g_JetBeamScroll, 0,
           0x10 - g_JetBeamScroll, 0x40);
    poly->tpage = g_JetSpriteTPage[1];
    poly->clut = g_JetSpriteClut[1];
    SetSemiTrans(poly, 1);
    addPrim(&db[0]->ot[1], poly);
    poly++;
    setXY4(poly, g_JetBeam1OriginX + spread, g_JetBeam1OriginY, g_JetCursorX, g_JetCursorY, g_JetBeam1OriginX - spread,
           g_JetBeam1OriginY, g_JetCursorX, g_JetCursorY);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0x20 - g_JetBeamScroll, 0, 0x20 - g_JetBeamScroll, 0x40, 0x10 - g_JetBeamScroll, 0,
           0x10 - g_JetBeamScroll, 0x40);
    poly->tpage = g_JetSpriteTPage[1];
    poly->clut = g_JetSpriteClut[1];
    SetSemiTrans(poly, 1);
    addPrim(&db[0]->ot[1], poly);
    poly++;
    db[0]->prims.ft4Cursor = poly;
}

// Allocate an object and its scene node, then initialise its six bounding box face centres.
static s16 JetObjectAlloc(JetObject* spawn, s16 parentIndex) {
    JetObject* obj;
    JetObject* parentObj;
    JetObject* box;
    s16 index;
    s32 rawId;
    s16 modelId;
    s16 maxX;
    s16 minX;
    s16 minY;
    s16 maxY;
    s16 minZ;
    s16 maxZ;

    if (g_JetObjectCount < 99) {
        g_JetObjectCount = g_JetObjectCount + 1;
        index = JetObjectIndexAlloc();
        g_JetObjects[index] = *spawn;
        obj = &g_JetObjects[index];
        rawId = spawn->state.modelId;
        modelId = rawId;
        obj->active = 1;
        obj->index = index;
        if (parentIndex == 0) {
            obj->node = JetNodeAlloc(rawId, 0, 0, 1, &g_JetRootNode, spawn->position.vx, spawn->position.vy,
                                     spawn->position.vz, spawn->rotation.vx, spawn->rotation.vy, spawn->rotation.vz);
        } else {
            parentObj = &g_JetObjects[parentIndex];
            obj->node = JetNodeAlloc(rawId, 0, 0, 1, parentObj->node, spawn->position.vx, spawn->position.vy,
                                     spawn->position.vz, spawn->rotation.vx, spawn->rotation.vy, spawn->rotation.vz);
        }
        minX = g_JetModelInfo[modelId].boundsMin.vx;
        maxX = g_JetModelInfo[modelId].boundsMax.vx;
        minY = g_JetModelInfo[modelId].boundsMin.vy;
        maxY = g_JetModelInfo[modelId].boundsMax.vy;
        minZ = g_JetModelInfo[modelId].boundsMin.vz;
        maxZ = g_JetModelInfo[modelId].boundsMax.vz;
        box = &g_JetObjects[index];
        setVector(&box->boxFaceCentres[0], (maxX + minX) >> 1, (maxY + minY) >> 1, maxZ);
        setVector(&box->boxFaceCentres[1], (maxX + minX) >> 1, (maxY + minY) >> 1, minZ);
        setVector(&box->boxFaceCentres[2], (maxX + minX) >> 1, maxY, (maxZ + minZ) >> 1);
        setVector(&box->boxFaceCentres[3], (maxX + minX) >> 1, minY, (maxZ + minZ) >> 1);
        setVector(&box->boxFaceCentres[4], maxX, (maxY + minY) >> 1, (maxZ + minZ) >> 1);
        setVector(&box->boxFaceCentres[5], minX, (maxY + minY) >> 1, (maxZ + minZ) >> 1);
    }
    return index;
}

static void JetObjectRelease(JetObject* object) {

    if (object->index != -1) {
        g_JetObjectCount -= 1;
        JetNodeFree(object->node);
        JetObjectIndexFree(object->index);
        object->index = -1;
        object->active = 0;
    }
}

static s16 JetObjectIndexAlloc(void) {
    s16 index;

    index = g_JetNextFreeObject;
    g_JetNextFreeObject = g_JetObjectFreeList[index];

    return index;
}

static void JetObjectIndexFree(s16 index) {
    u16* slot;

    slot = &g_JetObjectFreeList[index];
    *slot = g_JetNextFreeObject;
    g_JetNextFreeObject = index;
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
                g_JetSpawnTemplate.state.params.raw[j] = spawns[g_JetSpawnIndex].params[j];
            }
            index = g_JetSpawnIndex;
            g_JetSpawnTemplate.state.pathIndex = spawns[index].pathIndex;
            g_JetSpawnTemplate.state.speed = spawns[index].speed;
            JetObjectCreate(0, 0, 0, spawns[index].type, spawns[index].modelId);
            g_JetSpawnIndex++;
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
    g_JetSpawnTemplate.state.params.common.unk30 = 0;
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
    JetObjectState* objState;
    POLY_G4* fade;
    POLY_FT4* tpagePrim;
    s32 shade;
    s32 count;
    s32 i;
    s32 j;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 step;
    s32 x;
    s32 y;
    s32 z;
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
        objState = &obj->state;
        if (obj->active == 0) {
            continue;
        }
        drawMode = 0;
        do {
        } while (0);
        switch (obj->state.type) {
        case JET_OBJ_INCOMING:
            if (objState->needsInit == 1) {
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->age = 0;
                objState->hit = 0;
                objState->params.common.awardMode = JET_AWARD_SPARKLE_BURST;
                objState->vars.incoming.step = 0;
                objState->vars.incoming.startX = obj->position.vx;
                objState->vars.incoming.startY = obj->position.vy;
                objState->vars.incoming.startZ = obj->position.vz;
                objState->params.common.points = 0;
            } else {
                objState->age++;
                objState->vars.incoming.step++;
            }
            if (objState->life == 0) {
                JetObjectFree(obj);
                break;
            }
            step = objState->vars.incoming.step;
            JetTrackSample(g_JetCameraPathPos + 3 * JET_TRACK_SEGMENT, -100, &pos, &rot);
            x = objState->vars.incoming.startX;
            x += (step * (pos.vx - x)) >> 7;
            y = objState->vars.incoming.startY;
            y += (step * (pos.vy - y)) >> 7;
            z = objState->vars.incoming.startZ;
            z += (step * (pos.vz - z)) >> 7;
            obj->position.vx = x;
            obj->position.vy = y;
            obj->position.vz = z;
            JetTrackSample(g_JetCameraPathPos + 4 * JET_TRACK_SEGMENT, -100, &pos, &rot);
            dx = obj->position.vx - pos.vx;
            dy = obj->position.vy - pos.vy;
            dz = obj->position.vz - pos.vz;
            SquareRoot0(dx * dx + dy * dy + dz * dz);
            if (objState->age >= 129) {
                if (g_JetScore > 5) {
                    g_JetScore -= 5;
                } else {
                    g_JetScore = 0;
                }
                objState->life = 0;
            }
            if (objState->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_TRIGGERED:
            if (objState->needsInit == 1) {
                sound = objState->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->age = 0;
                objState->hit = 0;
                objState->vars.path.pathPos = 0;
                objState->vars.path.pathEnd = (obj->pathLen - 2) << 16;
            } else {
                objState->age++;
            }
            if (objState->params.triggered.startSegment < g_JetTrackSegment) {
                objState->vars.path.pathPos += objState->speed;
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->life == 0) {
                JetObjectFree(obj);
                break;
            }
            if (objState->hit) {
                JetObjectDamage(obj);
            }
            if (objState->vars.path.pathPos < objState->vars.path.pathEnd) {
                JetPathSample(objState->vars.path.pathPos, obj->path, &obj->position, 0);
                obj->rotation.vx = 0;
                obj->rotation.vy = 0;
                obj->rotation.vz = 0;
            }
            break;
        case JET_OBJ_ROTATOR:
            if (objState->needsInit == 1) {
                sound = objState->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->age = 0;
                objState->hit = 0;
                objState->vars.path.pathPos = 0;
                objState->vars.path.pathEnd = (obj->pathLen - 1) << 16;
            } else {
                objState->age++;
            }
            objState->vars.path.pathPos += objState->speed;
            if (objState->params.common.loopPath == 1) {
                objState->vars.path.pathPos %= objState->vars.path.pathEnd;
            }
            if (objState->vars.path.pathPos > objState->vars.path.pathEnd) {
                objState->life = 0;
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->life == 0) {
                JetObjectFree(obj);
                break;
            }
            JetPathSample(objState->vars.path.pathPos, obj->path, &obj->position, 0);
            obj->rotation.vx += objState->params.rotator.rotStep[0];
            obj->rotation.vy += objState->params.rotator.rotStep[1];
            obj->rotation.vz += objState->params.rotator.rotStep[2];
            if (objState->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_FLYER:
            if (objState->needsInit == 1) {
                sound = objState->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->age = 0;
                objState->hit = 0;
                objState->vars.path.pathPos = 0;
                objState->vars.path.pathEnd = (obj->pathLen - 1) << 16;
            } else {
                objState->age++;
            }
            objState->vars.path.pathPos += objState->speed;
            if (objState->params.common.loopPath == 1) {
                objState->vars.path.pathPos %= objState->vars.path.pathEnd;
            }
            if (objState->vars.path.pathPos > objState->vars.path.pathEnd) {
                objState->life = 0;
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->life == 0) {
                JetObjectFree(obj);
                break;
            }
            JetPathSample(objState->vars.path.pathPos, obj->path, &obj->position, 0);
            JetPathSample((objState->vars.path.pathPos + (1 << 16)) % ((obj->pathLen - 1) << 16), obj->path, &next, 0);
            dx = next.vx - obj->position.vx;
            dy = next.vy - obj->position.vy;
            dz = next.vz - obj->position.vz;
            obj->rotation.vx = ratan2(dy, SquareRoot0(dx * dx + dz * dz));
            obj->rotation.vy = -ratan2(dz, dx) - 0x400;
            obj->rotation.vz = 0;
            if (objState->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_CART:
            if (objState->needsInit == 1) {
                sound = objState->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                JetObjectPathLoad(0, JET_PATH_TRACK);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->age = 0;
                objState->hit = 0;
                objState->vars.path.pathPos = 0;
                objState->vars.path.pathEnd = (obj->pathLen - 1) << 16;
            } else {
                objState->age++;
            }
            objState->vars.path.pathPos += objState->speed;
            if (objState->params.common.loopPath == 1) {
                objState->vars.path.pathPos %= objState->vars.path.pathEnd;
            }
            if (objState->vars.path.pathPos > objState->vars.path.pathEnd) {
                objState->life = 0;
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->life == 0) {
                JetObjectFree(obj);
                break;
            }
            JetTrackSample(g_JetCameraPathPos + 4 * JET_TRACK_SEGMENT, 10, &obj->position, &obj->rotation);
            drawMode = 1;
            if (objState->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_STALACTITE:
            if (objState->needsInit == 1) {
                sound = objState->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->hit = 0;
                JetPathSample(0, obj->path, &obj->position, 0);
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->params.stalactite.fallSegment < g_JetTrackSegment) {
                objState->vars.stalactite.fallSpeed += 4;
            }
            if (objState->life == 0) {
                JetObjectFree(obj);
                break;
            }
            obj->position.vy += objState->vars.stalactite.fallSpeed;
            if (obj->position.vy > 0) {
                objState->life = 0;
            }
            if (objState->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_SPINNER:
            if (objState->needsInit == 1) {
                sound = objState->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->hit = 0;
                objState->vars.path.pathPos = 0;
                objState->vars.path.pathEnd = (obj->pathLen - 1) << 16;
                obj->rotation.vx = objState->params.spinner.startRot[0];
                obj->rotation.vy = objState->params.spinner.startRot[1];
                obj->rotation.vz = objState->params.spinner.startRot[2];
            }
            objState->vars.path.pathPos += objState->speed;
            if (objState->params.common.loopPath == 1) {
                objState->vars.path.pathPos %= objState->vars.path.pathEnd;
            }
            if (objState->vars.path.pathPos > objState->vars.path.pathEnd) {
                objState->life = 0;
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            obj->rotation.vx += objState->params.spinner.rotStep[0];
            obj->rotation.vy += objState->params.spinner.rotStep[1];
            obj->rotation.vz += objState->params.spinner.rotStep[2];
            if (objState->params.common.awardMode == JET_AWARD_SPARKLE_SHOWER) {
                obj->node->model = g_JetModelTable[JET_MODEL_UFO + objState->params.spinner.wasHit];
            }
            if (objState->params.spinner.wasHit == 1) {
                objState->params.spinner.wasHit = 0;
            }
            if (objState->life) {
                JetPathSample(objState->vars.path.pathPos, obj->path, &obj->position, 0);
                if (objState->hit) {
                    if (objState->params.common.awardMode != JET_AWARD_SPARKLE_SHOWER || g_JetSpeed < 16405) {
                        JetObjectDamage(obj);
                    }
                    if (objState->params.common.awardMode == JET_AWARD_SPARKLE_SHOWER) {
                        objState->params.spinner.wasHit = 1;
                    }
                }
            } else {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_BALLOON:
            if (objState->needsInit == 1) {
                sound = objState->params.common.spawnSfx;
                if (sound) {
                    JetPlaySfx(sound);
                }
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->hit = 0;
                objState->vars.balloon.targetTilt =
                    (rand() % objState->params.balloon.tiltRange) * 2 - objState->params.balloon.tiltRange - 1;
                objState->vars.balloon.tilt = 0;
                objState->vars.balloon.pathPos = 0;
                objState->vars.balloon.pathEnd = (obj->pathLen - 1) << 16;
                obj->rotation.vx = 0;
                obj->rotation.vy = 0;
                obj->rotation.vz = 0;
                JetPathSample(0, obj->path, &obj->position, 0);
            }
            objState->vars.balloon.pathPos += objState->speed;
            if (objState->vars.balloon.pathEnd < objState->vars.balloon.pathPos) {
                objState->life = 0;
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->vars.balloon.tilt > objState->vars.balloon.targetTilt) {
                objState->vars.balloon.tilt -= 5;
            }
            if (objState->vars.balloon.tilt < objState->vars.balloon.targetTilt) {
                objState->vars.balloon.tilt += 5;
            }
            obj->rotation.vx = objState->vars.balloon.tilt;
            obj->rotation.vy += objState->params.balloon.yawStep;
            if (objState->life == 0) {
                JetObjectFree(obj);
                break;
            }
            JetPathSample(objState->vars.balloon.pathPos, obj->path, &obj->position, 0);
            if (objState->hit) {
                JetObjectDamage(obj);
            }
            break;
        case JET_OBJ_STATIC:
            if (objState->needsInit == 1) {
                objState->needsInit = 0;
                objState->life = 1;
                objState->hit = 0;
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->life) {
                if (objState->hit) {
                    JetObjectDamage(obj);
                }
            } else {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_FLIP_UNUSED:
        case JET_OBJ_FLIP:
            if (objState->needsInit == 1) {
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->hit = 0;
                obj->rotation.vx = objState->params.flip.startRot[0];
                obj->rotation.vy = objState->params.flip.startRot[1];
                obj->rotation.vz = 0;
                JetPathSample(0, obj->path, &obj->position, 0);
                objState->vars.flip.frame = 0;
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->params.flip.flipSegment < g_JetTrackSegment) {
                count = objState->vars.flip.frame;
                objState->vars.flip.frame = count + 1;
                if (count < objState->params.flip.flipFrames) {
                    obj->rotation.vx += objState->params.flip.flipStep;
                }
            }
            if (objState->life) {
                if (objState->hit) {
                    JetObjectDamage(obj);
                }
            } else {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_EXPLOSION:
            JetPlaySfx(SFX_FIRAGA);
            for (j = 0; j < objState->params.explosion.debrisCount; j++) {
                JetObjectCreateUnscheduled(13382, -10000, 8395, JET_OBJ_DEBRIS, JET_MODEL_DEBRIS);
            }
            JetObjectFree(obj);
            // falls through into the debris behaviour below
        case JET_OBJ_DEBRIS:
            if (objState->needsInit == 1) {
                objState->hit = 0;
                objState->life = 100;
                objState->needsInit = 0;
                objState->age = 0;
                objState->vars.velocity.x = rand() % 60 - 30;
                objState->vars.velocity.y = -rand() % 200;
                objState->vars.velocity.z = rand() % 60 - 30;
            } else {
                objState->age++;
            }
            objState->vars.velocity.y++;
            obj->position.vx += objState->vars.velocity.x;
            obj->position.vy += objState->vars.velocity.y;
            obj->position.vz += objState->vars.velocity.z;
            obj->rotation.vx += 10;
            obj->rotation.vy += 400;
            obj->rotation.vz += 200;
            objState->life--;
            if (objState->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_FIREWORK:
            if (objState->needsInit == 1) {
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->hit = 0;
                objState->vars.raw[0] = 0;
                JetPathSample(0, obj->path, &obj->position, 0);
            }
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->life == 0) {
                JetObjectFree(obj);
                break;
            }
            obj->position.vy -= objState->params.firework.riseSpeed;
            objState->params.firework.riseSpeed -= objState->params.firework.riseDecel;
            if (objState->params.firework.riseSpeed < 0) {
                JetPlaySfx(SFX_DETONANTE_ECHO);
                for (j = 0; j < 20; j++) {
                    JetObjectCreateUnscheduled(obj->position.vx, obj->position.vy, obj->position.vz,
                                               JET_OBJ_FIREWORK_SPARK, rand() % 3 + JET_MODEL_CONFETTI);
                }
                objState->life = 0;
            }
            break;
        case JET_OBJ_FIREWORK_SPARK:
            if (objState->needsInit == 1) {
                objState->hit = 0;
                objState->life = 100;
                objState->needsInit = 0;
                objState->age = 0;
                objState->vars.velocity.x = rand() % 60 - 30;
                objState->vars.velocity.y = rand() % 60 - 30;
                objState->vars.velocity.z = rand() % 60 - 30;
            } else {
                objState->age++;
            }
            obj->position.vx += objState->vars.velocity.x;
            obj->position.vy += objState->vars.velocity.y;
            obj->position.vz += objState->vars.velocity.z;
            obj->rotation.vx += 10;
            obj->rotation.vy += 400;
            obj->rotation.vz += 200;
            objState->life--;
            if (objState->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_ERUPTION:
            if (objState->needsInit == 1) {
                JetPlaySfx(SFX_FIRA);
                JetObjectPathLoad(objState->pathIndex, JET_PATH_OBJECT);
                obj->path = g_JetLastPath;
                obj->pathLen = g_JetLastPathLen;
                objState->needsInit = 0;
                objState->life = 1;
                objState->hit = 0;
                objState->age = 0;
                objState->vars.jump.velY = -70;
                JetPathSample(0, obj->path, &obj->position, 0);
            }
            objState->vars.jump.velY++;
            objState->age++;
            if (objState->age == 5) {
                for (j = 0; j < objState->params.eruption.debrisCount; j++) {
                    JetObjectCreateUnscheduled(
                        obj->position.vx + rand() % 100 - 50, obj->position.vy + 500,
                        obj->position.vz + rand() % 100 - 50, JET_OBJ_ERUPTION_DEBRIS, JET_MODEL_DEBRIS);
                }
                JetObjectCreateUnscheduled(
                    obj->position.vx, obj->position.vy, obj->position.vz, JET_OBJ_ERUPTION_FLAME, JET_MODEL_FLAME);
            }
            if (objState->vars.jump.velY < 0) {
                obj->rotation.vy += 20;
            }
            if (objState->vars.jump.velY == 80) {
                objState->life = 0;
            }
            obj->position.vy += objState->vars.jump.velY;
            if (objState->params.common.endSegment < g_JetTrackSegment) {
                objState->life = 0;
            }
            if (objState->life) {
                if (objState->hit) {
                    JetObjectDamage(obj);
                }
            } else {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_ERUPTION_DEBRIS:
            if (objState->needsInit == 1) {
                objState->hit = 0;
                objState->life = 200;
                objState->needsInit = 0;
                objState->age = 0;
                objState->vars.eruptionDebris.velX = rand() % 80 - 40;
                objState->vars.eruptionDebris.velZ = rand() % 80 - 40;
                objState->vars.eruptionDebris.velY = -(rand() % 40 + 40);
            } else {
                objState->age++;
            }
            objState->vars.eruptionDebris.velY++;
            obj->rotation.vx += 480;
            obj->rotation.vy += 40;
            obj->rotation.vz += 610;
            obj->position.vx += objState->vars.eruptionDebris.velX;
            obj->position.vy += objState->vars.eruptionDebris.velY;
            obj->position.vz += objState->vars.eruptionDebris.velZ;
            objState->life--;
            if (objState->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_ERUPTION_FLAME:
            if (objState->needsInit == 1) {
                objState->life = 200;
                objState->hit = 0;
                objState->needsInit = 0;
                objState->age = 0;
                objState->vars.jump.velY = -60;
            } else {
                objState->age++;
            }
            if (objState->age >= 21) {
                obj->position.vy += objState->vars.jump.velY;
                objState->vars.jump.velY++;
            }
            objState->life--;
            if (objState->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_STARFIELD:
            obj->position.vx = g_JetCameraPosCopy.vx;
            obj->position.vy = g_JetCameraPosCopy.vy - 2500;
            obj->position.vz = g_JetCameraPosCopy.vz;
            otIndex = 1000;
            obj->rotation.vx = 0;
            obj->rotation.vy = 0;
            obj->rotation.vz = 0;
            break;
        case JET_OBJ_IMPACT_BURST:
            if (objState->needsInit == 1) {
                objState->life = 20;
                objState->needsInit = 0;
                objState->age = 0;
                objState->hit = 0;
            } else {
                objState->age++;
            }
            JetObjectCreateUnscheduled(
                obj->position.vx, obj->position.vy, obj->position.vz, JET_OBJ_IMPACT, JET_MODEL_DEBRIS);
            objState->life--;
            if (objState->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_IMPACT:
            if (objState->needsInit == 1) {
                objState->hit = 0;
                objState->life = 50;
                objState->needsInit = 0;
                objState->age = 0;
                objState->vars.velocity.x = rand() % 20 - 10;
                objState->vars.velocity.y = rand() % 40 - 20;
                objState->vars.velocity.z = rand() % 20 - 10;
            } else {
                objState->age++;
            }
            obj->position.vx += objState->vars.velocity.x;
            obj->position.vy += objState->vars.velocity.y;
            obj->position.vz += objState->vars.velocity.z;
            obj->rotation.vx += 10;
            obj->rotation.vy += 100;
            obj->rotation.vz += 20;
            objState->life--;
            if (objState->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_FRAGMENT:
            if (objState->needsInit == 1) {
                objState->hit = 0;
                objState->life = 50;
                objState->needsInit = 0;
                objState->age = 0;
                objState->vars.velocity.x = rand() % 200 - 100;
                objState->vars.velocity.y = rand() % 200 - 100;
                objState->vars.velocity.z = rand() % 200 - 100;
            } else {
                objState->age++;
            }
            obj->position.vx += objState->vars.velocity.x;
            obj->position.vy += objState->vars.velocity.y;
            obj->position.vz += objState->vars.velocity.z;
            obj->rotation.vx += 0;
            obj->rotation.vy += 300;
            obj->rotation.vz += 0;
            objState->life--;
            if (objState->life == 0) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_SPEED_CHANGE:
            if (objState->needsInit == 1) {
                objState->needsInit = 0;
                objState->age = 0;
            } else {
                objState->age++;
            }
            if (g_JetSpeed > objState->params.speedChange.minSpeed) {
                g_JetSpeed -= objState->params.speedChange.step;
            }
            if (g_JetSpeed < 0) {
                g_JetSpeed = 0;
                JetObjectCreateUnscheduled(0, 0, 0, JET_OBJ_RIDE_END, JET_MODEL_PLACEHOLDER);
            }
            if (objState->age > objState->params.speedChange.frames) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_STOP:
            if (objState->needsInit == 1) {
                objState->needsInit = 0;
                objState->age = 0;
                objState->vars.stop.accelerating = 0;
                objState->vars.stop.startVsync = VSync(-1);
                objState->vars.stop.accelFrame = 0;
            } else {
                objState->age++;
            }
            if (objState->vars.stop.accelerating == 0) {
                g_JetSpeed = 0;
            }
            if (objState->vars.stop.accelerating == 1) {
                g_JetSpeed += objState->params.stop.accel;
                objState->vars.stop.accelFrame++;
            }
            if (objState->params.stop.delay < VSync(-1) - objState->vars.stop.startVsync) {
                objState->vars.stop.accelerating = 1;
            }
            if (objState->vars.stop.accelFrame > objState->params.stop.accelFrames) {
                JetObjectFree(obj);
            }
            break;
        case JET_OBJ_RIDE_START:
            if (objState->needsInit == 1) {
                objState->needsInit = 0;
                objState->age = 0;
                g_JetSpeed = 0;
                g_JetDrawEnabled = 1;
                JetObjectCreateUnscheduled(0, 0, 0, JET_OBJ_STARFIELD, JET_MODEL_STARFIELD);
            } else {
                objState->age++;
            }
            shade = ~(objState->age * 2);
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
            if (objState->age >= 126) {
                JetObjectFree(obj);
                g_JetSpeed = 16384;
            }
            break;
        case JET_OBJ_RIDE_END:
            if (objState->needsInit == 1) {
                objState->needsInit = 0;
                objState->age = 0;
                g_JetSpeed = 0;
                JetAudioFadeOut();
            } else {
                objState->age++;
            }
            shade = objState->age * 2;
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
            if (objState->age >= 128) {
                JetObjectFree(obj);
                g_JetSpeed = 16384;
                g_JetDrawEnabled = 0;
                g_JetExit = 1;
            }
            break;
        case JET_OBJ_SCORE_CHECK:
            if (g_JetScore < objState->params.scoreCheck.minScore) {

                g_JetSpawnTemplate.state.params.speedChange.step = 300;
                g_JetSpawnTemplate.state.params.speedChange.frames = 400;
                g_JetSpawnTemplate.state.params.speedChange.minSpeed = 0;
                JetObjectCreateUnscheduled(
                    obj->position.vx, obj->position.vy, obj->position.vz, JET_OBJ_SPEED_CHANGE, JET_MODEL_BLUE_PLANE);
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
    JetObjectState* objState = &object->state;
    u8 amount;

    amount = g_JetShotPower >> 5;
    if (amount == 0) {
        amount = 1;
    }
    objState->params.common.health -= amount;
    if (objState->params.common.health < 0) {
        JetObjectAwardPoints(object);
    } else {
        JetObjectCreateUnscheduled(
            object->position.vx, object->position.vy, object->position.vz, JET_OBJ_IMPACT, JET_MODEL_SPARKLE);
    }
}

// Award the score for a hit object and scatter its debris.
static void JetObjectAwardPoints(JetObject* obj) {
    JetObjectState* objState = &obj->state;
    s32 i;

    if (objState->params.common.awardMode == JET_AWARD_SPARKLE_BURST) {
        s32 points;

        g_JetScore += objState->params.common.points;
        JetPlaySfx(objState->params.common.deathSfx);
        objState->life = 0;
        for (i = 0; i < 3; i++) {
            JetObjectCreateUnscheduled(
                obj->position.vx, obj->position.vy, obj->position.vz, JET_OBJ_IMPACT, rand() % 3 + JET_MODEL_SPARKLE);
        }
        g_JetPopupModelId = obj->node->modelId;
        points = objState->params.common.points;
        g_JetPopupPoints = points;
        g_JetPopupTimer = 100;
        g_JetScorePopupAlternate = 1;
        setVector(&g_JetPopupRot, 0, 0, 0);
    }
    if (objState->params.common.awardMode == JET_AWARD_SHARD_BURST) {
        s32 points;

        g_JetScore += objState->params.common.points;
        JetPlaySfx(objState->params.common.deathSfx);
        objState->life = 0;
        for (i = 0; i < 3; i++) {
            JetObjectCreateUnscheduled(
                obj->position.vx, obj->position.vy, obj->position.vz, JET_OBJ_FRAGMENT, rand() % 3 + JET_MODEL_SHARD);
        }
        g_JetPopupModelId = obj->node->modelId;
        points = objState->params.common.points;
        g_JetPopupPoints = points;
        g_JetPopupTimer = 100;
        g_JetScorePopupAlternate = 1;
        setVector(&g_JetPopupRot, 0, 0, 0);
    }
    if (objState->params.common.awardMode == JET_AWARD_TILT) {
        s32 points;

        g_JetScore += objState->params.common.points;
        points = objState->params.common.awardTilt;
        obj->rotation.vx += points;
    }
    if (objState->params.common.awardMode == JET_AWARD_NO_DEBRIS) {
        s32 points;
        g_JetScore += objState->params.common.points;
        JetPlaySfx(objState->params.common.deathSfx);
        objState->life = 0;
        g_JetPopupModelId = obj->node->modelId;
        points = objState->params.common.points;
        g_JetPopupPoints = points;
        g_JetPopupTimer = 100;
        g_JetScorePopupAlternate = 1;
        setVector(&g_JetPopupRot, 0, 0, 0);
    }
    if (objState->params.common.awardMode == JET_AWARD_SPARKLE_SHOWER) {
        s32 points;

        g_JetScore += objState->params.common.points;
        JetPlaySfx(objState->params.common.deathSfx);
        objState->life = 0;
        for (i = 0; i < 100; i++) {
            JetObjectCreateUnscheduled(
                obj->position.vx, obj->position.vy, obj->position.vz, JET_OBJ_FRAGMENT, rand() % 3 + JET_MODEL_SPARKLE);
        }
        g_JetPopupModelId = obj->node->modelId;
        points = objState->params.common.points;
        g_JetPopupPoints = points;
        g_JetPopupTimer = 100;
        g_JetScorePopupAlternate = 1;
        setVector(&g_JetPopupRot, 0, 0, 0);
    }
    if (g_JetScore > 9999) {
        g_JetScore = 9999;
    }
}
