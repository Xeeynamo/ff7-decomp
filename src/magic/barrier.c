//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"
#include "magic_private.h"

// Fade phase: the depth cue ramps by FADE_PER_FRAME to 0xE00 on the last
// frame, and the scale grows from SCALE_BASE by SCALE_PER_FADE_FRAME.
#define FADE_LAST_FRAME 7
#define FADE_PER_FRAME 0x200
#define SCALE_BASE 0xC00
#define SCALE_PER_FADE_FRAME 0x180

// This is placeholder for now so I can access the SVECTORs correctly
typedef struct BarrierData {
    s16 StartFrame;
    s16 AnimationFrame;
    s16 TargetIndex;
    s16 unk6;
    SVECTOR Pos;
    SVECTOR Rot;
    u16 FaceIndex; // 0..3, straight into the descriptor's mirror bits
                   // 0x1 and 0x2; one instance per quadrant
    char pad1[0x6];
} BarrierData;

// Battle effect instances.
extern BarrierData D_80162978[];
static s32 bari_a1[] = {    // Embedded Model
                            // 6 verts, 12 polys,1 group
    0x00000030,             // Vertex data size: 6 verts × 8 bytes
    0xFE0C0000, 0x000001CA, // (-500, 0, 458, 0)
    0x00000000, 0x0000FFD7, // (0, 0, -41, 0)
    0x0000FE0C, 0x000002E5, // (-500, 0, 741, 0)
    0xFFEDFE2F, 0x000002C2, // (-19, -465, 706, 0)
    0xFE2AFFF2, 0x000001C2, // (-470, -14, 450, 0)
    0xFFEDFFF2, 0x00000000, // (-19, -14, 0, 0)
    0x00200000, 0x00000000, 0x00000000,
    0x00000006, // Number of primitives

    // Vertex Index pairs offset into the vertex table
    //(0x00, 0x08, 0x10, 0x18, 0x20, 0x28).
    0x00280008, // vertex 0, vertex 5
    0x00180010, // vertex 2, vertex 3
    // GPU primitive command 0x38, (POLY_G4 - Gouraud Quad)
    // Then vertex color data
    0x38FFFFFF, 0x006A6A6A, 0x00C0C0C0, 0x006A6A6A,

    // repeat for other vertexes
    0x00200000, 0x00280008, 0x38C0C0C0, 0x006A6A6A, 0x00FFFFFF, 0x006A6A6A, 0x00180010, 0x00200000, 0x38C0C0C0,
    0x006A6A6A, 0x00C0C0C0, 0x006A6A6A, 0x00100008, 0x00180028, 0x38FFFFFF, 0x00C0C0C0, 0x006A6A6A, 0x006A6A6A,
    0x00080000, 0x00280020, 0x38C0C0C0, 0x00FFFFFF, 0x006A6A6A, 0x006A6A6A, 0x00000010, 0x00200018, 0x38C0C0C0,
    0x00C0C0C0, 0x006A6A6A, 0x006A6A6A};

static s32 bari_a2[] = {    // Embedded Model
                            // 3 verts 2 ploys 1 group
    0x00000018,             // Vertex data size: 3 × 8 bytes
    0xFFEDFE2F, 0x000002C2, // vertex 0 (-19, -465, 706)
    0xFE2AFFF2, 0x000001C2, // vertex 1 (-470, -14, 450)
    0xFFEDFFF2, 0x00000000, // vertex 2 (-19, -14, 0)
    0x00200000, 0x00000000,
    0x00000002, // Number of primitives

    // Vertex Index pairs offset into the vertex table
    0x00000008, 0x00000010,
    // GPU Primitive Command 0x30 (POLY_G3 - Gouraud Triangle):
    0x303F3F3F, // Primitive/color: command 0x30, RGB 3F3F3F
    0x003F3F3F, // Triangle 0: vertex 1 color, RGB 3F3F3F
    0x00D4D4D4, // Triangle 0: vertex 2 color, RGB D4D4D4

    // repeat
    0x00100008, 0x00000000,
    // GPU primitive command 0x30, Gouraud-shaded, opaque, 3-vertex poly
    0x303F3F3F,  // Primitive/color: command 0x30, RGB 3F3F3F
    0x00D4D4D4,  // Triangle 1: vertex 1 color, RGB D4D4D4
    0x003F3F3F}; // Triangle 1: vertex 2 color, RGB 3F3F3F
static int emptyPoly = 0x00000000;
static SVECTOR BorderPivotOffset = {0, 0, -500};
static ModelRenderDesc BorderRenderDesc = {bari_a1, 0, 0, 0, 0x20};
static SVECTOR ShieldPivotOffset = {0, 0, -500};
static ModelRenderDesc ShieldRenderDesc = {bari_a2, 0, 0, 0, 0x20};
static int BarrierBaseScale;
static char BarrierPrimBuffer[2 * MAGIC_PAGE_SIZE];
static void* BarrierBufferPtr;

// barrier.c forward declarations
static void BarrierMainSetup(int arg0, int arg1);

void MAGIC_Barrier(int arg0, int arg1) { BarrierMainSetup(arg0, arg1); }

// Four instances per cast, two frames apart, FaceIndex 0, 1, 3, 2 -- Gray
// code, so the shell grows through adjacent quadrants. Each draws the same
// quarter model and the mirror bits assemble the other three.
static void BarrierRenderBorder(void) {
    MATRIX* matrix = (MATRIX*)0x1F800000;
    VECTOR* scale = (VECTOR*)0x1F800020;
    BarrierData* barrier = &D_80162978[D_8015169C];
    int temp_a0 = (barrier->AnimationFrame + barrier->StartFrame) - 17;
    int fade;
    int faceFlags;

    // temp_a0 counts from the fade: below 0 opaque, 0..7 fading, past 7
    // retires the instance.
    if (temp_a0 < 0) {
        scale->vx = scale->vy = scale->vz = (BarrierBaseScale * SCALE_BASE) >> FIXED_SHIFT;
        faceFlags = barrier->FaceIndex;
        fade = 0;
    } else if (temp_a0 > FADE_LAST_FRAME) {
        barrier->StartFrame = -1;
        return;
    } else {
        // semi-transparency for the fade; the cue ramps 0x200 a frame
        // to 0xE00. FaceIndex is 0, 1, 3, 2 across the four instances,
        // which are the mirror bits that place each quadrant.
        faceFlags = barrier->FaceIndex | MODEL_SEMI_TRANS;
        fade = temp_a0 * FADE_PER_FRAME;
        scale->vx = scale->vy = scale->vz =
            (((temp_a0 * SCALE_PER_FADE_FRAME) + SCALE_BASE) * BarrierBaseScale) >> FIXED_SHIFT;
    }

    SetFarColor(0, 0, 0);
    RotMatrixYXZ(&barrier->Rot, matrix);
    ScaleMatrix(matrix, scale);
    ApplyMatrix(matrix, &BorderPivotOffset, matrix->t);

    matrix->t[0] += barrier->Pos.vx;
    matrix->t[1] += barrier->Pos.vy;
    matrix->t[2] += barrier->Pos.vz;

    CompMatrix(&D_800FA63C.m, matrix, matrix);
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);

    BorderRenderDesc.flags = faceFlags | MODEL_DEPTH_CUE;
    BorderRenderDesc.color = fade;
    BarrierBufferPtr = func_800D29D4(&BorderRenderDesc, g_cDb->unk70, 12, BarrierBufferPtr);

    if (D_80062D98 == 0) {
        barrier->AnimationFrame++;
    }
}

static void BarrierRenderShield(void) {
    MATRIX* matrix1 = (MATRIX*)0x1F800000;
    MATRIX* matrix2 = (MATRIX*)0x1F800020;
    VECTOR* scale1 = (VECTOR*)0x1F800040;
    VECTOR* scale2 = (VECTOR*)0x1F800050;
    BarrierData* barrier = &D_80162978[D_8015169C];
    int temp_a0 = barrier->AnimationFrame + barrier->StartFrame - 17;
    int faceFlags;
    int fade;

    if (temp_a0 < 0) {
        if (barrier->AnimationFrame < 6) {
            scale1->vx = scale1->vy = scale1->vz = (barrier->AnimationFrame * (BarrierBaseScale << 9)) >> FIXED_SHIFT;
        } else {
            scale1->vx = scale1->vy = scale1->vz = (BarrierBaseScale * SCALE_BASE) >> FIXED_SHIFT;
        }

        scale2->vx = scale2->vy = scale2->vz = (BarrierBaseScale * SCALE_BASE) >> FIXED_SHIFT;

        faceFlags = barrier->FaceIndex;
        fade = 0;
    } else if (temp_a0 > FADE_LAST_FRAME) {
        barrier->StartFrame = -1;
        return;
    } else {
        faceFlags = barrier->FaceIndex | MODEL_SEMI_TRANS;
        fade = temp_a0 * FADE_PER_FRAME;
        scale1->vx = scale1->vy = scale1->vz = scale2->vx = scale2->vy = scale2->vz =
            (((temp_a0 * SCALE_PER_FADE_FRAME) + SCALE_BASE) * BarrierBaseScale) >> FIXED_SHIFT;
    }

    SetFarColor(0, 0, 0);
    RotMatrixYXZ(&barrier->Rot, matrix1);
    *matrix2 = *matrix1;
    ScaleMatrix(matrix1, scale1);
    ScaleMatrix(matrix2, scale2);
    ApplyMatrix(matrix2, &ShieldPivotOffset, matrix1->t);
    matrix1->t[0] += barrier->Pos.vx;
    matrix1->t[1] += barrier->Pos.vy;
    matrix1->t[2] += barrier->Pos.vz;
    CompMatrix(&D_800FA63C.m, matrix1, matrix1);
    SetRotMatrix(matrix1);
    SetTransMatrix(matrix1);

    ShieldRenderDesc.flags = faceFlags | MODEL_DEPTH_CUE;
    ShieldRenderDesc.color = fade;
    BarrierBufferPtr = func_800D29D4(&ShieldRenderDesc, g_cDb->unk70, 12, BarrierBufferPtr);

    if (D_80062D98 == 0) {
        barrier->AnimationFrame++;
    }
}

static void BarrierAnimationUpdate(void) {
    BarrierData* barrier = &D_80162978[D_8015169C]; // model instance
    BarrierData* next;

    if (D_80062D98 != 0) {
        return;
    }

    if (barrier->AnimationFrame == 0) {
        next = &D_80162978[BattleEffectRegister(BarrierRenderBorder)];
        next->StartFrame = barrier->AnimationFrame;
        next->FaceIndex = 0;
        next->Rot = barrier->Rot;
        next->Pos = barrier->Pos;
    }

    if (barrier->AnimationFrame == 2) {
        next = &D_80162978[BattleEffectRegister(BarrierRenderBorder)];
        next->StartFrame = barrier->AnimationFrame;
        next->FaceIndex = 1;
        next->Rot = barrier->Rot;
        next->Pos = barrier->Pos;
    }

    if (barrier->AnimationFrame == 4) {
        next = &D_80162978[BattleEffectRegister(BarrierRenderBorder)];
        next->StartFrame = barrier->AnimationFrame;
        next->FaceIndex = 3;
        next->Rot = barrier->Rot;
        next->Pos = barrier->Pos;
    }

    if (barrier->AnimationFrame == 6) {
        next = &D_80162978[BattleEffectRegister(BarrierRenderBorder)];
        next->StartFrame = barrier->AnimationFrame;
        next->FaceIndex = 2;
        next->Rot = barrier->Rot;
        next->Pos = barrier->Pos;
    }

    if (barrier->AnimationFrame == 1) {
        next = &D_80162978[BattleEffectRegister(BarrierRenderShield)];
        next->StartFrame = barrier->AnimationFrame;
        next->FaceIndex = 0;
        next->Rot = barrier->Rot;
        next->Pos = barrier->Pos;
    }

    if (barrier->AnimationFrame == 3) {
        next = &D_80162978[BattleEffectRegister(BarrierRenderShield)];
        next->StartFrame = barrier->AnimationFrame;
        next->FaceIndex = 1;
        next->Rot = barrier->Rot;
        next->Pos = barrier->Pos;
    }

    if (barrier->AnimationFrame == 5) {
        next = &D_80162978[BattleEffectRegister(BarrierRenderShield)];
        next->StartFrame = barrier->AnimationFrame;
        next->FaceIndex = 3;
        next->Rot = barrier->Rot;
        next->Pos = barrier->Pos;
    }

    if (barrier->AnimationFrame == 7) {
        next = &D_80162978[BattleEffectRegister(BarrierRenderShield)];
        next->StartFrame = barrier->AnimationFrame;
        next->FaceIndex = 2;
        next->Rot = barrier->Rot;
        next->Pos = barrier->Pos;
    }

    if (barrier->AnimationFrame == 17) {
        func_800D5774(barrier->TargetIndex);
        barrier->StartFrame = -1;
    }

    barrier->AnimationFrame++;
}

static void BarrierAttachToTarget(int target) {
    BarrierData* barrier = &D_80162978[BattleEffectRegister(BarrierAnimationUpdate)];

    BattleGetPartPosition(target, D_801518E4[target].D_8015190F, &barrier->Pos);
    barrier->Pos.vx -= (rsin(D_801518E4[target].unk160.vy) * D_801518E4[target].unk12) >> FIXED_SHIFT;
    barrier->Pos.vz -= (rcos(D_801518E4[target].unk160.vy) * D_801518E4[target].unk12) >> FIXED_SHIFT;
    barrier->Rot = D_801518E4[target].unk160;
    barrier->TargetIndex = target;
}

static void BarrierDoubleBufferFlip(void) {
    BarrierData* barrier = &D_80162978[D_8015169C];

    BarrierBufferPtr = &BarrierPrimBuffer[barrier->AnimationFrame * MAGIC_PAGE_SIZE];
    barrier->AnimationFrame ^= 1;

    if (D_80162080 < 2) {
        barrier->StartFrame = -1;
    }
}

static void BarrierMainSetup(int arg0, int arg1) {
    BarrierBaseScale = 0x3000;
    BattleEffectRegister(BarrierDoubleBufferFlip);
    MagicAnimationRegister(arg0, arg1, 4, BarrierAttachToTarget);
    BattleCommandSend(32, BattleEntityGetStereoPan(arg0), 94);
}
