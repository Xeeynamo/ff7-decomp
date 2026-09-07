//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"
#include "magic_private.h"

// Bolt (サンダー / Thunder), tier 1. Three passes share the effect slot array:
// a model drawn through func_800D29D4, and two textured-quad passes drawn
// through func_800D4D90, all double-buffered into a 0x20000 primitive page.

// Model pass: full grey for the first half, then dimmed by GREY_PER_FRAME.
#define MODEL_LIFETIME 16
#define DIM_START_FRAME 8
#define GREY_FULL 0x80
#define GREY_PER_FRAME 0x10

// Bolt pass: registers the renderers on frame 0, scatters a spark from
// SPARK_START_FRAME on, retires at BOLT_LIFETIME.
#define BOLT_LIFETIME 16
#define SPARK_START_FRAME 2

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR unk0C;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ u16 Scale;     // 0x1000 == 1.0, grows by ScaleStep
    /* 0x18 */ u16 ScaleStep; // per-frame growth, seeded 0x200
    /* 0x1A */ u16 unk1A;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ char pad1E[2];
} ThunderData; // size:0x20

typedef struct {
    /* 0x00 */ char pad[MAGIC_PAGE_SIZE];
} ThunderPrimPage; // size:0x10000

// The renderers write several primitive kinds at varying sizes and hand
// back the next write position. Every magic overlay declares it this way.
extern void* ThunderBufferPtr;
extern ThunderData D_80162978[];
extern ThunderPrimPage ThunderPrimBuffer[];
extern u_long ThunderTexture[]; // 8bpp TIM + CLUT, uploaded on setup
extern SpriteRenderDesc ThunderRenderDesc0;
extern SpriteRenderDesc ThunderRenderDesc1;
extern MATRIX ThunderModelMatrix;
extern ModelRenderDesc ThunderModelDesc;

static void ThunderMainSetup(s32 arg0, s32 arg1);

void MAGIC_Thunder(s32 arg0, s32 arg1) { ThunderMainSetup(arg0, arg1); }

// Draws the embedded model through the model path, growing it by 0x200 a
// frame from 1.0x to 2.875x across the 16 frames and dimming it over the
// last 8.
//
// Flags 0x08 leave MODEL_DEPTH_CUE clear, so color is a grey level: it
// steps from 0x80 down to 0x10, dimming the model from grey 128 to 16.
//
// The matrix holds a fixed orientation scaled by Scale -- m[0][0] and
// m[2][1] take Scale, m[1][2] takes -Scale. Confirmed live: the other six
// entries hold zero on all 16 frames.
static void ThunderRenderModel(void) {
    MATRIX matrix;
    ThunderData* effect = &D_80162978[D_8015169C];
    s16 frame = effect->AnimationFrame;
    u16* scale; // read through a pointer; a plain field read does not match

    if (frame < DIM_START_FRAME) {
        ThunderModelDesc.color = GREY_FULL;
    } else if (frame < MODEL_LIFETIME) {
        ThunderModelDesc.color = GREY_FULL - ((frame - DIM_START_FRAME) * GREY_PER_FRAME);
    } else {
        effect->StartFrame = -1;
        return;
    }

    scale = &effect->Scale;
    ThunderModelMatrix.m[0][0] = ThunderModelMatrix.m[2][1] = *scale;
    ThunderModelMatrix.m[1][2] = -(s16)*scale;
    ThunderModelMatrix.t[0] = (s32)effect->Pos.vx;
    ThunderModelMatrix.t[1] = (s32)effect->Pos.vy;
    ThunderModelMatrix.t[2] = (s32)effect->Pos.vz;
    CompMatrix(&D_800FA63C.m, &ThunderModelMatrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    ThunderBufferPtr = func_800D29D4(&ThunderModelDesc, g_cDb->unk70, 0xC, ThunderBufferPtr);
    if (D_80062D98 == 0) {
        effect->AnimationFrame = (u16)effect->AnimationFrame + 1;
        effect->Scale += effect->ScaleStep;
    }
}

// Quad pass over ThunderRenderDesc0, 9 frames. Each block in the data draws
// one quad at a stepped position offset, two on the last frame. Not renamed:
// what it draws is not established.
static void func_801B0180(void) {
    ThunderData* effect;
    s16 nextFrame;

    effect = &D_80162978[D_8015169C];
    func_800D4368(&effect->Pos, 0x2000, effect->unk1C);
    ThunderRenderDesc0.frameIndex = effect->AnimationFrame >> 1;
    ThunderBufferPtr = func_800D4D90(&ThunderRenderDesc0, g_cDb->unk70, 0xC, ThunderBufferPtr);
    if (D_80062D98 == 0) {
        nextFrame = (u16)effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame == 9) {
            effect->StartFrame = -1;
        }
    }
}

// Quad pass over ThunderRenderDesc1, 8 frames, mirrored per instance by the
// random bits in unk1A. Not renamed: what it draws is not established.
static void func_801B023C(void) {
    MATRIX* matrix;
    ThunderData* effect = &D_80162978[D_8015169C];
    s16 nextFrame;

    matrix = func_800D4368(&effect->Pos, 0x2000, effect->unk1C);
    if (effect->unk1A & 1) {
        matrix->m[0][0] = -matrix->m[0][0];
    }
    if (effect->unk1A & 2) {
        matrix->m[1][1] = -matrix->m[1][1];
    }
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    ThunderRenderDesc1.frameIndex = effect->AnimationFrame;
    ThunderBufferPtr = func_800D4D90(&ThunderRenderDesc1, g_cDb->unk70, 0xC, ThunderBufferPtr);
    if (D_80062D98 == 0) {
        nextFrame = (u16)effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame == 8) {
            effect->StartFrame = -1;
        }
    }
}

// Spawn callback for the bolt. On frame 0 it registers the two one-shot
// renderers; from frame 2 on it scatters a spark each frame, retiring at 16.
static void ThunderSpawnBolt(void) {
    ThunderData* next;
    ThunderData* effect = &D_80162978[D_8015169C];
    s16 nextFrame;

    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            next = &D_80162978[BattleEffectRegister(func_801B0180)];
            next->Pos = effect->Pos;
            next->Pos.vy = 0;
            next->unk1C = (u16)effect->unk1C;
            func_800D5774(effect->unk14);
            // Re-tested after the call; the original re-loads the frame here
            // and the match needs it.
            if (effect->AnimationFrame == 0) {
                next = &D_80162978[BattleEffectRegister(ThunderRenderModel)];
                next->Pos = effect->Pos;
                next->Scale = FIXED_ONE;
                next->Pos.vy = 0;
                next->ScaleStep = 0x200;
                next->unk1C = (u16)effect->unk1C;
            }
        }
        if (effect->AnimationFrame >= SPARK_START_FRAME) {
            next = &D_80162978[BattleEffectRegister(func_801B023C)];
            next->Pos.vx = ((u16)effect->Pos.vx + rand() % 1000) - 500;
            next->Pos.vy = ((u16)effect->Pos.vy + rand() % 1000) - 500;
            next->Pos.vz = ((u16)effect->Pos.vz + rand() % 1000) - 500;
            next->unk1A = rand() & 3;
            next->unk1C = (u16)effect->unk1C;
        }
        nextFrame = (u16)effect->AnimationFrame + 1;
        effect->AnimationFrame = nextFrame;
        if (nextFrame == BOLT_LIFETIME) {
            effect->StartFrame = -1;
        }
    }
}

static void ThunderAttachToTarget(s32 target) {
    ThunderData* effect = &D_80162978[BattleEffectRegister(ThunderSpawnBolt)];

    BattleGetPartPosition(target, D_801518E4[target].D_8015190F, &effect->Pos);
    effect->unk14 = target;
    effect->unk1C = -D_801518E4[target].unk12;
    BattleCommandSend(0x20, BattlePositionToStereoPan(&effect->Pos), 0xB);
}

static void ThunderDoubleBufferFlip(void) {
    ThunderData* data = &D_80162978[D_8015169C];

    ThunderBufferPtr = &ThunderPrimBuffer[data->AnimationFrame];
    data->AnimationFrame = data->AnimationFrame ^ 1;
    if (D_80162080 < 2) {
        data->StartFrame = -1;
    }
}

static void ThunderMainSetup(s32 arg0, s32 arg1) {
    func_800D2980(ThunderTexture, 0, 0, 0);
    BattleEffectRegister(ThunderDoubleBufferFlip);
    MagicAnimationRegister(arg0, arg1, 2, ThunderAttachToTarget);
}
