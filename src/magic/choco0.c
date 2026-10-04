//! PSYQ=3.5

#include "common.h"
#include "magic.h"
#include "magic_private.h"
#include "../battle/battle.h"
#include "choco0.h"
#include <libc.h>

// Choco/Mog (チョコモグ), the chocobo-and-moogle summon.

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ union {
        SVECTOR velocity; // dust, smoke
        struct {
            /* 0x0C */ s16 unk0C;
            /* 0x0E */ s16 Angle;
        } stars;
        struct {
            /* 0x0C */ s16* Script;
            /* 0x10 */ MATRIX* SceneMatrix;
        } camera;
    } u;
    /* 0x14 */ s16 Scale; // the camera effect keeps the caster index here
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
} Choco0Data; // size:0x20

typedef struct {
    /* 0x00 */ s16 Opcode;
    /* 0x02 */ s16 FramesLeft;
    /* 0x04 */ union {
        struct {
            /* 0x04 */ s32 Distance; // to the other path
            /* 0x08 */ s32 DistanceStep;
        } dolly;
        struct {
            /* 0x04 */ s32 Progress; // 0 to 0x1000
            /* 0x08 */ s32 ProgressStep;
        } ease;
    } u;
    /* 0x0C */ s16 ActorIndex;
    /* 0x0E */ s16 PartIndex;
    /* 0x10 */ SVECTOR Pos;
    /* 0x18 */ SVECTOR StartPos; // the acceleration for CAM_OP_ACCEL_TO
    /* 0x20 */ SVECTOR Step;     // the whole displacement for CAM_OP_EASE_TO
} Choco0CameraPath;              // size:0x28

extern EffectModel D_801D267C;
extern u_long choco0_texture_tim[];
extern Choco0Data g_BattleEffectSlots[];
extern void* D_80163C74;
extern SVECTOR g_BattleCameraTarget;
extern SVECTOR g_BattleCameraPos;

static SVECTOR choco0_scene_rot = {0};
static SVECTOR choco0_unit_pos = {0, 0, -15000, 0};
static s16 choco0_camera_script_a[] = {
    CAM_SET_POS(CAM_TARGET, 0, 0, -15000),
    CAM_SET_POS(CAM_EYE, 0, -500, 5000),
    CAM_HOLD(CAM_TARGET, 24),
    CAM_HOLD(CAM_EYE, 24),
    CAM_ATTACH(CAM_TARGET, 3, 0),
    CAM_ATTACH(CAM_EYE, 3, 0),
    CAM_SET_POS(CAM_TARGET, 0, 0, 0),
    CAM_SET_POS(CAM_EYE, -500, -1000, 1000),
    CAM_MOVE_TO(CAM_EYE, 20, -800, -1200, 3000),
    CAM_MOVE_TO(CAM_EYE, 20, -2000, -1500, 5000),
    CAM_HOLD(CAM_TARGET, 20),
    CAM_HOLD(CAM_EYE, 20),
    CAM_DETACH(CAM_TARGET),
    CAM_DETACH(CAM_EYE),
    CAM_SET_POS(CAM_TARGET, 0, 0, 0),
    CAM_SET_POS(CAM_EYE, -5000, -5000, -5000),
    CAM_HOLD(CAM_TARGET, 40),
    CAM_HOLD(CAM_EYE, 40),
    CAM_SET_POS(CAM_TARGET, 0, -200, -5000),
    CAM_SET_POS(CAM_EYE, 0, -300, -7000),
    CAM_HOLD(CAM_TARGET, 50),
    CAM_HOLD(CAM_EYE, 50),
    CAM_SET_POS(CAM_TARGET, 0, 0, 0),
    CAM_SET_POS(CAM_EYE, -10000, -5000, -5000),
    CAM_HOLD(CAM_TARGET, 15),
    CAM_HOLD(CAM_EYE, 15),
    CAM_END,
    0, // pad
};
static s16 choco0_camera_script_b[] = {
    CAM_SET_POS(CAM_TARGET, 0, 0, -15000),
    CAM_SET_POS(CAM_EYE, 0, -1000, 5000),
    CAM_HOLD(CAM_TARGET, 24),
    CAM_HOLD(CAM_EYE, 24),
    CAM_ATTACH(CAM_TARGET, 3, 0),
    CAM_ATTACH(CAM_EYE, 3, 0),
    CAM_SET_POS(CAM_TARGET, 0, 0, 1000),
    CAM_SET_POS(CAM_EYE, -800, -500, 1000),
    CAM_MOVE_TO(CAM_EYE, 20, -6000, -500, 2000),
    CAM_HOLD(CAM_TARGET, 20),
    CAM_HOLD(CAM_EYE, 20),
    CAM_DETACH(CAM_TARGET),
    CAM_DETACH(CAM_EYE),
    CAM_SET_POS(CAM_TARGET, 0, 0, 0),
    CAM_SET_POS(CAM_EYE, -2000, -2000, -7000),
    CAM_HOLD(CAM_TARGET, 40),
    CAM_HOLD(CAM_EYE, 40),
    CAM_SET_POS(CAM_TARGET, 0, -200, -5000),
    CAM_SET_POS(CAM_EYE, 0, -300, -7000),
    CAM_HOLD(CAM_TARGET, 50),
    CAM_HOLD(CAM_EYE, 50),
    CAM_SET_POS(CAM_TARGET, 0, 0, 0),
    CAM_SET_POS(CAM_EYE, -10000, -5000, -5000),
    CAM_HOLD(CAM_TARGET, 15),
    CAM_HOLD(CAM_EYE, 15),
    CAM_END,
};
static MATRIX choco0_sprite_matrix = {0};
static SpriteRenderDesc choco0_render_desc0 = {NULL, {0x80, 0x80, 0x80, 0x2E}, 0, 0};
static BattleSpriteDesc choco0_screen_quad = {-128, -96, 0, 0, 255, 191, 0x80, 0x80, 0x80, 0x2C, 0x8D, 0x3FF0};
static MATRIX choco0_screen_matrix = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
static VECTOR* choco0_scratch_vec = (VECTOR*)0x1F800000;
static SpriteRenderDesc choco0_star_desc = {&g_Choco0StarFrames, {0x80, 0x80, 0x80, 0x2C}, 0, 0};
static SVECTOR* choco0_scratch_svec = (SVECTOR*)0x1F800000;
static SpriteRenderDesc choco0_render_desc1 = {NULL, {0x80, 0x80, 0x80, 0x2E}, 0, 0};
static SVECTOR choco0_left_eye_offset = {-50, 80, -90, 0};
static SVECTOR choco0_right_eye_offset = {50, 80, -90, 0};
static MATRIX choco0_left_eye_matrix = {{{0x200, 0, 0}, {0, 0x200, 0}, {0, 0, 0x200}}, {0, 0, 0}};
static MATRIX choco0_right_eye_matrix = {{{0x200, 0, 0}, {0, 0x200, 0}, {0, 0, 0x200}}, {0, 0, 0}};
static SpriteRenderDesc choco0_swirl_eye_desc = {&g_Choco0SwirlEyeFrames.anim, {0x80, 0x80, 0x80, 0x2C}, 0, 0};
static RECT choco0_clear_rect = {960, 0, 32, 64};

static Choco0CameraPath choco0_camera_eye_path;
static Choco0CameraPath choco0_camera_target_path;
static Choco0CameraPath* choco0_camera_path_cur;
static Choco0CameraPath* choco0_camera_path_other;
static MATRIX choco0_scene_matrix;
static MATRIX choco0_view_matrix;
static s32 choco0_target_mask;
static s16 choco0_camera_script_vars[2];

static void Choco0MainSetup(s32 targetMask, s32 callbackArg);

EffectModel* MAGIC_Choco0(s32 targetMask, s32 callbackArg) {
    Choco0MainSetup(targetMask, callbackArg);
    return &D_801D267C;
}

static s32 Choco0ReadScriptValue(s16** script) {
    s32 word;
    s16* cursor;

    cursor = *script;
    *script = cursor + 1;
    word = *cursor;
    if (word < 0) {
        word = choco0_camera_script_vars[-word - 1];
    }
    return word;
}

static void Choco0UpdateCamera(void) {
    Choco0Data* effect;
    SVECTOR* delta;
    SVECTOR* otherEnd;
    VECTOR* worldPos;
    s32 i;
    u16 command;
    s32 t;
    u8 unused[0x100];
    s32 flag;

    otherEnd = (SVECTOR*)0x1F800008;
    worldPos = (VECTOR*)0x1F800010;
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    delta = (SVECTOR*)0x1F800000;
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            choco0_camera_eye_path.ActorIndex = -1;
            choco0_camera_target_path.ActorIndex = -1;
            choco0_camera_eye_path.FramesLeft = 0;
            choco0_camera_target_path.FramesLeft = 0;
            effect->AnimationFrame = 1;
        }
        while (choco0_camera_eye_path.FramesLeft == 0 || choco0_camera_target_path.FramesLeft == 0) {
            command = *effect->u.camera.Script++;
            if (command & CAM_EYE) {
                choco0_camera_path_cur = &choco0_camera_eye_path;
                choco0_camera_path_other = &choco0_camera_target_path;
            } else {
                choco0_camera_path_cur = &choco0_camera_target_path;
                choco0_camera_path_other = &choco0_camera_eye_path;
            }
            choco0_camera_path_cur->Opcode = command & ~(CAM_EYE | CAM_TARGET);
            switch (choco0_camera_path_cur->Opcode) {
            case CAM_OP_HOLD:
                choco0_camera_path_cur->FramesLeft = Choco0ReadScriptValue(&effect->u.camera.Script);
                break;
            case CAM_OP_SET_POS:
                choco0_camera_path_cur->Pos.vx = *effect->u.camera.Script++;
                choco0_camera_path_cur->Pos.vy = *effect->u.camera.Script++;
                choco0_camera_path_cur->Pos.vz = *effect->u.camera.Script++;
                break;
            case CAM_OP_MOVE_TO:
                choco0_camera_path_cur->FramesLeft = Choco0ReadScriptValue(&effect->u.camera.Script);
                choco0_camera_path_cur->Step.vx =
                    (*effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vx) / choco0_camera_path_cur->FramesLeft;
                choco0_camera_path_cur->Step.vy =
                    (*effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vy) / choco0_camera_path_cur->FramesLeft;
                choco0_camera_path_cur->Step.vz =
                    (*effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vz) / choco0_camera_path_cur->FramesLeft;
                break;
            case CAM_OP_DOLLY_TO:
                delta->vx = choco0_camera_path_other->Pos.vx - choco0_camera_path_cur->Pos.vx;
                delta->vy = choco0_camera_path_other->Pos.vy - choco0_camera_path_cur->Pos.vy;
                delta->vz = choco0_camera_path_other->Pos.vz - choco0_camera_path_cur->Pos.vz;
                choco0_camera_path_cur->u.dolly.Distance =
                    SquareRoot0(delta->vx * delta->vx + delta->vy * delta->vy + delta->vz * delta->vz);
                choco0_camera_path_cur->StartPos = choco0_camera_path_cur->Pos;
                choco0_camera_path_cur->FramesLeft = Choco0ReadScriptValue(&effect->u.camera.Script);
                delta->vx = *effect->u.camera.Script++;
                delta->vy = *effect->u.camera.Script++;
                delta->vz = *effect->u.camera.Script++;
                choco0_camera_path_cur->Step.vx =
                    (delta->vx - choco0_camera_path_cur->Pos.vx) / choco0_camera_path_cur->FramesLeft;
                choco0_camera_path_cur->Step.vy =
                    (delta->vy - choco0_camera_path_cur->Pos.vy) / choco0_camera_path_cur->FramesLeft;
                choco0_camera_path_cur->Step.vz =
                    (delta->vz - choco0_camera_path_cur->Pos.vz) / choco0_camera_path_cur->FramesLeft;
                if (choco0_camera_path_other->Opcode == CAM_OP_MOVE_TO) {
                    otherEnd->vx = choco0_camera_path_other->Pos.vx +
                                   choco0_camera_path_other->Step.vx * choco0_camera_path_other->FramesLeft;
                    otherEnd->vy = choco0_camera_path_other->Pos.vy +
                                   choco0_camera_path_other->Step.vy * choco0_camera_path_other->FramesLeft;
                    otherEnd->vz = choco0_camera_path_other->Pos.vz +
                                   choco0_camera_path_other->Step.vz * choco0_camera_path_other->FramesLeft;
                } else {
                    otherEnd->vx = choco0_camera_path_other->Pos.vx;
                    otherEnd->vy = choco0_camera_path_other->Pos.vy;
                    otherEnd->vz = choco0_camera_path_other->Pos.vz;
                }
                delta->vx -= otherEnd->vx;
                delta->vy -= otherEnd->vy;
                delta->vz -= otherEnd->vz;
                choco0_camera_path_cur->u.dolly.DistanceStep =
                    (SquareRoot0(delta->vx * delta->vx + delta->vy * delta->vy + delta->vz * delta->vz) -
                     choco0_camera_path_cur->u.dolly.Distance) /
                    choco0_camera_path_cur->FramesLeft;
                break;
            case CAM_OP_ACCEL_TO:
                choco0_camera_path_cur->FramesLeft = Choco0ReadScriptValue(&effect->u.camera.Script);
                delta->vx = *effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vx;
                delta->vy = *effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vy;
                delta->vz = *effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vz;
                choco0_camera_path_cur->StartPos.vx =
                    ((delta->vx - choco0_camera_path_cur->Step.vx * choco0_camera_path_cur->FramesLeft) * 2) /
                    (choco0_camera_path_cur->FramesLeft * choco0_camera_path_cur->FramesLeft);
                choco0_camera_path_cur->StartPos.vy =
                    ((delta->vy - choco0_camera_path_cur->Step.vy * choco0_camera_path_cur->FramesLeft) * 2) /
                    (choco0_camera_path_cur->FramesLeft * choco0_camera_path_cur->FramesLeft);
                choco0_camera_path_cur->StartPos.vz =
                    ((delta->vz - choco0_camera_path_cur->Step.vz * choco0_camera_path_cur->FramesLeft) * 2) /
                    (choco0_camera_path_cur->FramesLeft * choco0_camera_path_cur->FramesLeft);
                break;
            case CAM_OP_EASE_TO:
                choco0_camera_path_cur->FramesLeft = Choco0ReadScriptValue(&effect->u.camera.Script);
                choco0_camera_path_cur->u.ease.Progress = 0;
                choco0_camera_path_cur->StartPos.vx = choco0_camera_path_cur->Pos.vx;
                choco0_camera_path_cur->StartPos.vy = choco0_camera_path_cur->Pos.vy;
                choco0_camera_path_cur->StartPos.vz = choco0_camera_path_cur->Pos.vz;
                choco0_camera_path_cur->u.ease.ProgressStep = 0x1000 / choco0_camera_path_cur->FramesLeft;
                choco0_camera_path_cur->Step.vx = *effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vx;
                choco0_camera_path_cur->Step.vy = *effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vy;
                choco0_camera_path_cur->Step.vz = *effect->u.camera.Script++ - choco0_camera_path_cur->Pos.vz;
                break;
            case CAM_OP_ATTACH:
                choco0_camera_path_cur->ActorIndex = *effect->u.camera.Script++;
                if (choco0_camera_path_cur->ActorIndex == 0) {
                    choco0_camera_path_cur->ActorIndex = effect->Scale;
                }
                choco0_camera_path_cur->PartIndex = *effect->u.camera.Script++;
                break;
            case CAM_OP_DETACH:
                choco0_camera_path_cur->ActorIndex = -1;
                break;
            default:
                effect->StartFrame = -1;
                return;
            }
        }
        choco0_camera_path_cur = &choco0_camera_eye_path;
        choco0_camera_path_other = &choco0_camera_target_path;
        for (i = 0; i < 2; i++) {
            switch (choco0_camera_path_cur->Opcode) {
            case CAM_OP_HOLD:
            case CAM_OP_SET_POS:
                break;
            case CAM_OP_ACCEL_TO:
                choco0_camera_path_cur->Step.vx += choco0_camera_path_cur->StartPos.vx;
                choco0_camera_path_cur->Step.vy += choco0_camera_path_cur->StartPos.vy;
                choco0_camera_path_cur->Step.vz += choco0_camera_path_cur->StartPos.vz;
            case CAM_OP_MOVE_TO:
                choco0_camera_path_cur->Pos.vx += choco0_camera_path_cur->Step.vx;
                choco0_camera_path_cur->Pos.vy += choco0_camera_path_cur->Step.vy;
                choco0_camera_path_cur->Pos.vz += choco0_camera_path_cur->Step.vz;
                break;
            case CAM_OP_DOLLY_TO:
                choco0_camera_path_cur->StartPos.vx += choco0_camera_path_cur->Step.vx;
                choco0_camera_path_cur->StartPos.vy += choco0_camera_path_cur->Step.vy;
                choco0_camera_path_cur->StartPos.vz += choco0_camera_path_cur->Step.vz;
                choco0_camera_path_cur->u.dolly.Distance += choco0_camera_path_cur->u.dolly.DistanceStep;
                worldPos->vx = choco0_camera_path_other->Pos.vx - choco0_camera_path_cur->StartPos.vx;
                worldPos->vy = choco0_camera_path_other->Pos.vy - choco0_camera_path_cur->StartPos.vy;
                worldPos->vz = choco0_camera_path_other->Pos.vz - choco0_camera_path_cur->StartPos.vz;
                VectorNormalS(worldPos, delta);
                choco0_camera_path_cur->Pos.vx =
                    choco0_camera_path_other->Pos.vx - ((delta->vx * choco0_camera_path_cur->u.dolly.Distance) >> 12);
                choco0_camera_path_cur->Pos.vy =
                    choco0_camera_path_other->Pos.vy - ((delta->vy * choco0_camera_path_cur->u.dolly.Distance) >> 12);
                choco0_camera_path_cur->Pos.vz =
                    choco0_camera_path_other->Pos.vz - ((delta->vz * choco0_camera_path_cur->u.dolly.Distance) >> 12);
                break;
            case CAM_OP_EASE_TO:
                t = choco0_camera_path_cur->u.ease.Progress += choco0_camera_path_cur->u.ease.ProgressStep;
                t = rsin(rsin(t / 4) / 4);
                choco0_camera_path_cur->Pos.vx =
                    choco0_camera_path_cur->StartPos.vx + ((choco0_camera_path_cur->Step.vx * t) >> 12);
                choco0_camera_path_cur->Pos.vy =
                    choco0_camera_path_cur->StartPos.vy + ((choco0_camera_path_cur->Step.vy * t) >> 12);
                choco0_camera_path_cur->Pos.vz =
                    choco0_camera_path_cur->StartPos.vz + ((choco0_camera_path_cur->Step.vz * t) >> 12);
                break;
            }
            choco0_camera_path_cur->FramesLeft--;
            choco0_camera_path_cur = &choco0_camera_target_path;
            choco0_camera_path_other = &choco0_camera_eye_path;
        }
    }
    if (effect->u.camera.SceneMatrix) {
        SetRotMatrix(effect->u.camera.SceneMatrix);
        SetTransMatrix(effect->u.camera.SceneMatrix);
        if (choco0_camera_eye_path.ActorIndex == -1) {
            RotTrans(&choco0_camera_eye_path.Pos, worldPos, &flag);
            g_BattleCameraPos.vx = worldPos->vx;
            g_BattleCameraPos.vy = worldPos->vy;
            g_BattleCameraPos.vz = worldPos->vz;
        } else {
            ApplyRotMatrix(&choco0_camera_eye_path.Pos, worldPos);
            if (choco0_camera_eye_path.PartIndex != -1) {
                BattleGetPartPosition(choco0_camera_eye_path.ActorIndex, choco0_camera_eye_path.PartIndex, delta);
            } else {
                delta->vx = g_BattleModels[choco0_camera_eye_path.ActorIndex].rootTrans.vx;
                delta->vy = g_BattleModels[choco0_camera_eye_path.ActorIndex].rootTrans.vy;
                delta->vz = g_BattleModels[choco0_camera_eye_path.ActorIndex].rootTrans.vz;
            }
            g_BattleCameraPos.vx = worldPos->vx + delta->vx;
            g_BattleCameraPos.vy = worldPos->vy + delta->vy;
            g_BattleCameraPos.vz = worldPos->vz + delta->vz;
            SetRotMatrix(effect->u.camera.SceneMatrix);
            SetTransMatrix(effect->u.camera.SceneMatrix);
        }
        if (choco0_camera_target_path.ActorIndex == -1) {
            RotTrans(&choco0_camera_target_path.Pos, worldPos, &flag);
            g_BattleCameraTarget.vx = worldPos->vx;
            g_BattleCameraTarget.vy = worldPos->vy;
            g_BattleCameraTarget.vz = worldPos->vz;
        } else {
            ApplyRotMatrix(&choco0_camera_target_path.Pos, worldPos);
            if (choco0_camera_target_path.PartIndex != -1) {
                BattleGetPartPosition(choco0_camera_target_path.ActorIndex, choco0_camera_target_path.PartIndex, delta);
            } else {
                delta->vx = g_BattleModels[choco0_camera_target_path.ActorIndex].rootTrans.vx;
                delta->vy = g_BattleModels[choco0_camera_target_path.ActorIndex].rootTrans.vy;
                delta->vz = g_BattleModels[choco0_camera_target_path.ActorIndex].rootTrans.vz;
            }
            g_BattleCameraTarget.vx = worldPos->vx + delta->vx;
            g_BattleCameraTarget.vy = worldPos->vy + delta->vy;
            g_BattleCameraTarget.vz = worldPos->vz + delta->vz;
        }
    } else {
        g_BattleCameraPos = choco0_camera_eye_path.Pos;
        g_BattleCameraTarget = choco0_camera_target_path.Pos;
    }
}

static void Choco0SpawnCamera(s16* script, MATRIX* sceneMatrix, s32 callbackArg) {
    Choco0Data* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(Choco0UpdateCamera)];
    effect->u.camera.Script = script;
    effect->u.camera.SceneMatrix = sceneMatrix;
    effect->Scale = callbackArg;
}

static MATRIX* Choco0SetSpriteMatrix(SVECTOR* pos, s32 scale, s32 depthBias) {
    VECTOR dir;
    s32 flag;

    choco0_sprite_matrix.m[0][0] = choco0_sprite_matrix.m[1][1] = choco0_sprite_matrix.m[2][2] = scale;
    SetRotMatrix(&choco0_view_matrix);
    SetTransMatrix(&choco0_view_matrix);
    RotTrans(pos, (VECTOR*)choco0_sprite_matrix.t, &flag);
    if (depthBias) {
        VectorNormal((VECTOR*)choco0_sprite_matrix.t, &dir);
        choco0_sprite_matrix.t[0] += (depthBias * dir.vx) >> 12;
        choco0_sprite_matrix.t[1] += (depthBias * dir.vy) >> 12;
        choco0_sprite_matrix.t[2] += (depthBias * dir.vz) >> 12;
    }
    SetRotMatrix(&choco0_sprite_matrix);
    SetTransMatrix(&choco0_sprite_matrix);
    return &choco0_sprite_matrix;
}

static void Choco0RenderDust(void) {
    Choco0Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    Choco0SetSpriteMatrix(&effect->Pos, effect->Scale, 0);
    choco0_render_desc0.frames = g_Choco0PuffFrames[effect->AnimationFrame];
    D_80163C74 = func_800D4D90(&choco0_render_desc0, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 16) {
            effect->StartFrame = -1;
        } else {
            effect->Pos.vx += effect->u.velocity.vx;
            effect->Pos.vy += effect->u.velocity.vy;
            effect->Pos.vz += effect->u.velocity.vz;
            effect->u.velocity.vx = (effect->u.velocity.vx * 7) >> 3;
            effect->u.velocity.vz = (effect->u.velocity.vz * 7) >> 3;
        }
    }
}

static void Choco0SpawnDust(void) {
    Choco0Data* effect;
    Choco0Data* child;
    s32 i;
    s32 speed;
    s32 angle;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98) {
        return;
    }
    for (i = 0; i < 3; i++) {
        child = &g_BattleEffectSlots[BattleEffectRegister(Choco0RenderDust)];
        child->Pos.vx = choco0_unit_pos.vx;
        child->Pos.vy = choco0_unit_pos.vy;
        child->Pos.vz = choco0_unit_pos.vz;
        speed = rand() % 100 + 100;
        angle = rand() & 0x7FF;
        child->u.velocity.vx = (rcos(angle) * speed) >> 12;
        child->u.velocity.vy = -(rand() % 30 + 20);
        child->u.velocity.vz = (-rsin(angle) * speed) >> 12;
        child->Scale = rand() % 0x800 + 0x1000;
    }
    if (++effect->AnimationFrame >= 60) {
        effect->StartFrame = -1;
    }
}

static void Choco0RenderBoom(void) {
    Choco0Data* effect;
    s32 phase;
    s16 scale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    phase = effect->AnimationFrame % 7;
    if (phase < 3) {
        scale = (phase << 12) / 3 + 0x1000;
        choco0_screen_matrix.m[1][1] = scale;
        choco0_screen_matrix.m[0][0] = scale;
    } else {
        phase -= 3;
        scale = -(phase << 12) / 4 + 0x2000;
        choco0_screen_matrix.m[1][1] = scale;
        choco0_screen_matrix.m[0][0] = scale;
    }
    choco0_screen_matrix.t[2] = ReadGeomScreen() * 8;
    SetRotMatrix(&choco0_screen_matrix);
    SetTransMatrix(&choco0_screen_matrix);
    D_80163C74 = BattleEffectSpriteAdd(&choco0_screen_quad, &g_cDb->unk4080[1], 0, D_80163C74);
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 20) {
            effect->StartFrame = -1;
        }
    }
}

static void Choco0MoveModel(void) {
    Choco0Data* effect;
    s32 frame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98) {
        return;
    }
    frame = effect->AnimationFrame;
    if (frame < 60) {
        choco0_unit_pos.vz = frame * 250 - 15000;
    } else if ((frame -= 60) < 20) {
        if (frame == 19) {
            choco0_unit_pos.vz = -5000;
            choco0_unit_pos.vx = -750;
            g_BattleModels[EFFECT_MODEL_SLOT].rootRot.vy += 0x400;
        }
    } else if ((frame -= 20) >= 50) {
        effect->StartFrame = -1;
        return;
    }
    SetRotMatrix(&choco0_scene_matrix);
    SetTransMatrix(&choco0_scene_matrix);
    RotTrans(&choco0_unit_pos, choco0_scratch_vec, (s32*)(choco0_scratch_vec + 1));
    g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vx = choco0_scratch_vec->vx;
    g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vy = choco0_scratch_vec->vy;
    g_BattleModels[EFFECT_MODEL_SLOT].rootTrans.vz = choco0_scratch_vec->vz;
    effect->AnimationFrame++;
}

static void Choco0RenderStars(void) {
    Choco0Data* effect;
    s32 i;
    s32 angle;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    angle = effect->u.stars.Angle;
    for (i = 0; i < 4; i++) {
        choco0_scratch_svec->vx = effect->Pos.vx + ((rsin(angle) * 150) >> 12);
        choco0_scratch_svec->vy = effect->Pos.vy;
        choco0_scratch_svec->vz = effect->Pos.vz + ((rcos(angle) * 150) >> 12);
        angle += 0x400;
        Choco0SetSpriteMatrix(choco0_scratch_svec, 0x500, 0);
        D_80163C74 = func_800D4D90(&choco0_star_desc, g_cDb->unk70, 12, D_80163C74);
    }
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 35) {
            effect->StartFrame = -1;
        } else {
            effect->u.stars.Angle += 0x40;
        }
    }
}

static void Choco0RenderSmoke(void) {
    Choco0Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    Choco0SetSpriteMatrix(&effect->Pos, effect->Scale, 0);
    choco0_render_desc1.frames = g_Choco0PuffFrames[effect->AnimationFrame];
    D_80163C74 = func_800D4D90(&choco0_render_desc1, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 16) {
            effect->StartFrame = -1;
        } else {
            effect->Pos.vy += effect->u.velocity.vy;
        }
    }
}

static void Choco0SpawnSmoke(void) {
    Choco0Data* effect;
    Choco0Data* child;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98) {
        return;
    }
    for (i = 0; i < 3; i++) {
        child = &g_BattleEffectSlots[BattleEffectRegister(Choco0RenderSmoke)];
        child->Pos.vx = rand() % 4000 - 2000;
        child->Pos.vy = 0;
        child->Pos.vz = rand() % 3000 - 3000;
        child->u.velocity.vx = 0;
        child->u.velocity.vy = -(rand() % 30 + 20);
        child->u.velocity.vz = 0;
        child->Scale = rand() % 0x2000 + 0x1000;
    }
    if (++effect->AnimationFrame >= 50) {
        effect->StartFrame = -1;
    }
}

static void Choco0RenderSwirlEyes(void) {
    Choco0Data* effect;
    s32 flag;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    SetRotMatrix(&g_BattleModels[EFFECT_MODEL_SLOT].boneTransforms[13].m);
    SetTransMatrix(&g_BattleModels[EFFECT_MODEL_SLOT].boneTransforms[13].m);
    RotTrans(&choco0_left_eye_offset, (VECTOR*)choco0_left_eye_matrix.t, &flag);
    RotTrans(&choco0_right_eye_offset, (VECTOR*)choco0_right_eye_matrix.t, &flag);
    choco0_swirl_eye_desc.frameIndex = effect->AnimationFrame & 7;
    SetRotMatrix(&choco0_left_eye_matrix);
    SetTransMatrix(&choco0_left_eye_matrix);
    D_80163C74 = func_800D4D90(&choco0_swirl_eye_desc, &g_cDb->unk4080[1], 0, D_80163C74);
    SetRotMatrix(&choco0_right_eye_matrix);
    SetTransMatrix(&choco0_right_eye_matrix);
    D_80163C74 = func_800D4D90(&choco0_swirl_eye_desc, &g_cDb->unk4080[1], 0, D_80163C74);
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 50) {
            effect->StartFrame = -1;
        }
    }
}

static void Choco0AnimationUpdate(void) {
    Choco0Data* effect;
    Choco0Data* child;
    s16* event;
    s32 frame;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    CompMatrix(&D_800FA63C.m, &choco0_scene_matrix, &choco0_view_matrix);
    if (D_80062D98) {
        return;
    }
    frame = effect->AnimationFrame;
    if (frame < 5) {
        if (frame == 4) {
            event = BattleEventQueuePush(BATTLE_EVENT_EFFECT_MODEL_START);
            event[2] = 0;
            event[3] = 0;
            event[4] = choco0_scene_matrix.t[2] - 15000;
            event[8] = choco0_scene_rot.vy + 0x800;
        }
    } else if ((frame -= 5) < 20) {
        if (frame == 0) {
            BattleEffectRegister(Choco0SpawnDust);
            BattleEffectRegister(Choco0MoveModel);
            BattleAkaoCommand(AKAO_PLAY_THREE_SOUNDS, AKAO_PAN_CENTER, SFX_24D, SFX_24E, SFX_24F);
        }
    } else if ((frame -= 20) < 20) {
    } else if ((frame -= 20) < 20) {
    } else if ((frame -= 20) < 20) {
        if (frame == 0) {
            BattleEffectRegister(Choco0RenderBoom);
            BattleEffectRegister(Choco0SpawnSmoke);
        }
        if (frame == 19) {
            BattleEnqueueClearImage(&choco0_clear_rect, 0, 0, 0);
        }
        if (frame == 0) {
            BattleAkaoCommand(AKAO_PLAY_THREE_SOUNDS, AKAO_PAN_CENTER, SFX_250, SFX_251, SFX_252);
        }
    } else if ((frame -= 20) < 25) {
        if (frame == 0) {
            child = &g_BattleEffectSlots[BattleEffectRegister(Choco0RenderStars)];
            child->Pos.vx = 50;
            child->Pos.vy = -500;
            child->Pos.vz = -5000;
            BattleEffectRegister(Choco0RenderSwirlEyes);
        }
    } else if ((frame -= 25) < 5) {
    } else if ((frame -= 5) < 20) {
        if (frame == 18) {
            BattleEventQueuePush(BATTLE_EVENT_EFFECT_MODEL_END);
        }
    } else if ((frame -= 20) < 15) {
        if (frame == 0) {
            for (i = 0; i < NUM_BATTLE_ACTOR; i++) {
                if ((choco0_target_mask >> i) & 1) {
                    func_800D5774(i);
                }
            }
        }
    } else {
        effect->StartFrame = -1;
    }
    effect->AnimationFrame++;
}

static void Choco0MainSetup(s32 targetMask, s32 callbackArg) {
    SVECTOR center;

    BattleSetLoadTimToVram(choco0_texture_tim, 0, 0, 0);
    choco0_target_mask = targetMask;
    BattleEntityGetCenter(targetMask, &center);
    choco0_scene_matrix.t[0] = choco0_scene_matrix.t[1] = 0;
    choco0_scene_matrix.t[2] = center.vz;
    if (center.vz < g_BattleModels[callbackArg].rootTrans.vz) {
        choco0_scene_rot.vy = 0x800;
    }
    RotMatrixYXZ(&choco0_scene_rot, &choco0_scene_matrix);
    BattleEffectRegister(Choco0AnimationUpdate);
    if (rand() & 0x100) {
        Choco0SpawnCamera(choco0_camera_script_a, &choco0_scene_matrix, callbackArg);
    } else {
        Choco0SpawnCamera(choco0_camera_script_b, &choco0_scene_matrix, callbackArg);
    }
}
