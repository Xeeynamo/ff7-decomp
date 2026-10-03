//! PSYQ=3.5

#include "common.h"
#include "magic.h"
#include "../battle/battle.h"
#include <libc.h>

// Choco/Mog, the chocobo-and-moogle summon.

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ union {
        SVECTOR vec;
        struct {
            /* 0x0C */ s16* Script;
            /* 0x10 */ MATRIX* unk10;
        } ptr;
    } u;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
} Choco0Data; // size:0x20

typedef struct {
    /* 0x00 */ s16 Opcode;
    /* 0x02 */ s16 FramesLeft;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s16 ActorIndex;
    /* 0x0E */ s16 PartIndex;
    /* 0x10 */ SVECTOR Pos;
    /* 0x18 */ SVECTOR unk18;
    /* 0x20 */ SVECTOR unk20;
} Choco0CameraPath; // size:0x28

extern s32* D_801D2574[];
extern Unk800F57D0 D_801D267C;
extern u_long g_Choco0Texture[];
extern SVECTOR D_801E5754;
extern SVECTOR D_801E575C;
extern s16 D_801E5764[];
extern s16 D_801E5804[];
extern MATRIX D_801E5898;
extern SpriteRenderDesc D_801E58B8;
extern BattleSpriteDesc D_801E58C4;
extern MATRIX D_801E58D4;
extern VECTOR* D_801E58F4;
extern SpriteRenderDesc D_801E58F8;
extern SVECTOR* D_801E5904;
extern SpriteRenderDesc D_801E5908;
extern SVECTOR D_801E5914;
extern SVECTOR D_801E591C;
extern MATRIX D_801E5924;
extern MATRIX D_801E5944;
extern SpriteRenderDesc D_801E5964;
extern RECT D_801E5970;
extern Choco0CameraPath g_Choco0CameraEyePath;
extern Choco0CameraPath g_Choco0CameraTargetPath;
extern Choco0CameraPath* g_Choco0CameraPathCur;
extern Choco0CameraPath* g_Choco0CameraPathOther;
extern MATRIX D_801E59D0;
extern MATRIX D_801E59F0;
extern s32 g_Choco0TargetMask;
extern s16 D_801E5A12[];

extern Choco0Data g_BattleEffectSlots[];
extern void* D_80163C74;
extern SVECTOR g_BattleCameraTarget;
extern SVECTOR g_BattleCameraPos;

void Choco0MainSetup(s32 targetMask, s32 callbackArg);

Unk800F57D0* MAGIC_Choco0(s32 targetMask, s32 callbackArg) {
    Choco0MainSetup(targetMask, callbackArg);
    return &D_801D267C;
}

s32 func_801B0060(s16** arg0) {
    s32 value;
    s16* ptr;

    ptr = *arg0;
    *arg0 = ptr + 1;
    value = *ptr;
    if (value < 0) {
        value = D_801E5A12[-value];
    }
    return value;
}

void Choco0UpdateCamera(void) {
    Choco0Data* effect;
    SVECTOR* sv0;
    SVECTOR* sv8;
    VECTOR* vec;
    s32 i;
    u16 op;
    s32 t;
    u8 unused[0x100]; // unreferenced, but part of the stack frame
    s32 flag;

    sv8 = (SVECTOR*)0x1F800008;
    vec = (VECTOR*)0x1F800010;
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    sv0 = (SVECTOR*)0x1F800000;
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 0) {
            g_Choco0CameraEyePath.ActorIndex = -1;
            g_Choco0CameraTargetPath.ActorIndex = -1;
            g_Choco0CameraEyePath.FramesLeft = 0;
            g_Choco0CameraTargetPath.FramesLeft = 0;
            effect->AnimationFrame = 1;
        }
        while (g_Choco0CameraEyePath.FramesLeft == 0 || g_Choco0CameraTargetPath.FramesLeft == 0) {
            op = *effect->u.ptr.Script++;
            if (op & 0x40) {
                g_Choco0CameraPathCur = &g_Choco0CameraEyePath;
                g_Choco0CameraPathOther = &g_Choco0CameraTargetPath;
            } else {
                g_Choco0CameraPathCur = &g_Choco0CameraTargetPath;
                g_Choco0CameraPathOther = &g_Choco0CameraEyePath;
            }
            g_Choco0CameraPathCur->Opcode = op & 0xFF3F;
            switch (g_Choco0CameraPathCur->Opcode) {
            case 0:
                g_Choco0CameraPathCur->FramesLeft = func_801B0060(&effect->u.ptr.Script);
                break;
            case 1:
                g_Choco0CameraPathCur->Pos.vx = *effect->u.ptr.Script++;
                g_Choco0CameraPathCur->Pos.vy = *effect->u.ptr.Script++;
                g_Choco0CameraPathCur->Pos.vz = *effect->u.ptr.Script++;
                break;
            case 2:
                g_Choco0CameraPathCur->FramesLeft = func_801B0060(&effect->u.ptr.Script);
                g_Choco0CameraPathCur->unk20.vx =
                    (*effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vx) / g_Choco0CameraPathCur->FramesLeft;
                g_Choco0CameraPathCur->unk20.vy =
                    (*effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vy) / g_Choco0CameraPathCur->FramesLeft;
                g_Choco0CameraPathCur->unk20.vz =
                    (*effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vz) / g_Choco0CameraPathCur->FramesLeft;
                break;
            case 3:
                sv0->vx = g_Choco0CameraPathOther->Pos.vx - g_Choco0CameraPathCur->Pos.vx;
                sv0->vy = g_Choco0CameraPathOther->Pos.vy - g_Choco0CameraPathCur->Pos.vy;
                sv0->vz = g_Choco0CameraPathOther->Pos.vz - g_Choco0CameraPathCur->Pos.vz;
                g_Choco0CameraPathCur->unk4 = SquareRoot0(sv0->vx * sv0->vx + sv0->vy * sv0->vy + sv0->vz * sv0->vz);
                g_Choco0CameraPathCur->unk18 = g_Choco0CameraPathCur->Pos;
                g_Choco0CameraPathCur->FramesLeft = func_801B0060(&effect->u.ptr.Script);
                sv0->vx = *effect->u.ptr.Script++;
                sv0->vy = *effect->u.ptr.Script++;
                sv0->vz = *effect->u.ptr.Script++;
                g_Choco0CameraPathCur->unk20.vx =
                    (sv0->vx - g_Choco0CameraPathCur->Pos.vx) / g_Choco0CameraPathCur->FramesLeft;
                g_Choco0CameraPathCur->unk20.vy =
                    (sv0->vy - g_Choco0CameraPathCur->Pos.vy) / g_Choco0CameraPathCur->FramesLeft;
                g_Choco0CameraPathCur->unk20.vz =
                    (sv0->vz - g_Choco0CameraPathCur->Pos.vz) / g_Choco0CameraPathCur->FramesLeft;
                if (g_Choco0CameraPathOther->Opcode == 2) {
                    sv8->vx = g_Choco0CameraPathOther->Pos.vx +
                              g_Choco0CameraPathOther->unk20.vx * g_Choco0CameraPathOther->FramesLeft;
                    sv8->vy = g_Choco0CameraPathOther->Pos.vy +
                              g_Choco0CameraPathOther->unk20.vy * g_Choco0CameraPathOther->FramesLeft;
                    sv8->vz = g_Choco0CameraPathOther->Pos.vz +
                              g_Choco0CameraPathOther->unk20.vz * g_Choco0CameraPathOther->FramesLeft;
                } else {
                    sv8->vx = g_Choco0CameraPathOther->Pos.vx;
                    sv8->vy = g_Choco0CameraPathOther->Pos.vy;
                    sv8->vz = g_Choco0CameraPathOther->Pos.vz;
                }
                sv0->vx -= sv8->vx;
                sv0->vy -= sv8->vy;
                sv0->vz -= sv8->vz;
                g_Choco0CameraPathCur->unk8 =
                    (SquareRoot0(sv0->vx * sv0->vx + sv0->vy * sv0->vy + sv0->vz * sv0->vz) -
                     g_Choco0CameraPathCur->unk4) /
                    g_Choco0CameraPathCur->FramesLeft;
                break;
            case 4:
                g_Choco0CameraPathCur->FramesLeft = func_801B0060(&effect->u.ptr.Script);
                sv0->vx = *effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vx;
                sv0->vy = *effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vy;
                sv0->vz = *effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vz;
                g_Choco0CameraPathCur->unk18.vx =
                    ((sv0->vx - g_Choco0CameraPathCur->unk20.vx * g_Choco0CameraPathCur->FramesLeft) * 2) /
                    (g_Choco0CameraPathCur->FramesLeft * g_Choco0CameraPathCur->FramesLeft);
                g_Choco0CameraPathCur->unk18.vy =
                    ((sv0->vy - g_Choco0CameraPathCur->unk20.vy * g_Choco0CameraPathCur->FramesLeft) * 2) /
                    (g_Choco0CameraPathCur->FramesLeft * g_Choco0CameraPathCur->FramesLeft);
                g_Choco0CameraPathCur->unk18.vz =
                    ((sv0->vz - g_Choco0CameraPathCur->unk20.vz * g_Choco0CameraPathCur->FramesLeft) * 2) /
                    (g_Choco0CameraPathCur->FramesLeft * g_Choco0CameraPathCur->FramesLeft);
                break;
            case 5:
                g_Choco0CameraPathCur->FramesLeft = func_801B0060(&effect->u.ptr.Script);
                g_Choco0CameraPathCur->unk4 = 0;
                g_Choco0CameraPathCur->unk18.vx = g_Choco0CameraPathCur->Pos.vx;
                g_Choco0CameraPathCur->unk18.vy = g_Choco0CameraPathCur->Pos.vy;
                g_Choco0CameraPathCur->unk18.vz = g_Choco0CameraPathCur->Pos.vz;
                g_Choco0CameraPathCur->unk8 = 0x1000 / g_Choco0CameraPathCur->FramesLeft;
                g_Choco0CameraPathCur->unk20.vx = *effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vx;
                g_Choco0CameraPathCur->unk20.vy = *effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vy;
                g_Choco0CameraPathCur->unk20.vz = *effect->u.ptr.Script++ - g_Choco0CameraPathCur->Pos.vz;
                break;
            case 6:
                g_Choco0CameraPathCur->ActorIndex = *effect->u.ptr.Script++;
                if (g_Choco0CameraPathCur->ActorIndex == 0) {
                    g_Choco0CameraPathCur->ActorIndex = effect->unk14;
                }
                g_Choco0CameraPathCur->PartIndex = *effect->u.ptr.Script++;
                break;
            case 7:
                g_Choco0CameraPathCur->ActorIndex = -1;
                break;
            default:
                effect->StartFrame = -1;
                return;
            }
        }
        g_Choco0CameraPathCur = &g_Choco0CameraEyePath;
        g_Choco0CameraPathOther = &g_Choco0CameraTargetPath;
        for (i = 0; i < 2; i++) {
            switch (g_Choco0CameraPathCur->Opcode) {
            case 0:
            case 1:
                break;
            case 4:
                g_Choco0CameraPathCur->unk20.vx += g_Choco0CameraPathCur->unk18.vx;
                g_Choco0CameraPathCur->unk20.vy += g_Choco0CameraPathCur->unk18.vy;
                g_Choco0CameraPathCur->unk20.vz += g_Choco0CameraPathCur->unk18.vz;
            case 2:
                g_Choco0CameraPathCur->Pos.vx += g_Choco0CameraPathCur->unk20.vx;
                g_Choco0CameraPathCur->Pos.vy += g_Choco0CameraPathCur->unk20.vy;
                g_Choco0CameraPathCur->Pos.vz += g_Choco0CameraPathCur->unk20.vz;
                break;
            case 3:
                g_Choco0CameraPathCur->unk18.vx += g_Choco0CameraPathCur->unk20.vx;
                g_Choco0CameraPathCur->unk18.vy += g_Choco0CameraPathCur->unk20.vy;
                g_Choco0CameraPathCur->unk18.vz += g_Choco0CameraPathCur->unk20.vz;
                g_Choco0CameraPathCur->unk4 += g_Choco0CameraPathCur->unk8;
                vec->vx = g_Choco0CameraPathOther->Pos.vx - g_Choco0CameraPathCur->unk18.vx;
                vec->vy = g_Choco0CameraPathOther->Pos.vy - g_Choco0CameraPathCur->unk18.vy;
                vec->vz = g_Choco0CameraPathOther->Pos.vz - g_Choco0CameraPathCur->unk18.vz;
                VectorNormalS(vec, sv0);
                g_Choco0CameraPathCur->Pos.vx =
                    g_Choco0CameraPathOther->Pos.vx - ((sv0->vx * g_Choco0CameraPathCur->unk4) >> 12);
                g_Choco0CameraPathCur->Pos.vy =
                    g_Choco0CameraPathOther->Pos.vy - ((sv0->vy * g_Choco0CameraPathCur->unk4) >> 12);
                g_Choco0CameraPathCur->Pos.vz =
                    g_Choco0CameraPathOther->Pos.vz - ((sv0->vz * g_Choco0CameraPathCur->unk4) >> 12);
                break;
            case 5:
                t = g_Choco0CameraPathCur->unk4 += g_Choco0CameraPathCur->unk8;
                t = rsin(rsin(t / 4) / 4);
                g_Choco0CameraPathCur->Pos.vx =
                    g_Choco0CameraPathCur->unk18.vx + ((g_Choco0CameraPathCur->unk20.vx * t) >> 12);
                g_Choco0CameraPathCur->Pos.vy =
                    g_Choco0CameraPathCur->unk18.vy + ((g_Choco0CameraPathCur->unk20.vy * t) >> 12);
                g_Choco0CameraPathCur->Pos.vz =
                    g_Choco0CameraPathCur->unk18.vz + ((g_Choco0CameraPathCur->unk20.vz * t) >> 12);
                break;
            }
            g_Choco0CameraPathCur->FramesLeft--;
            g_Choco0CameraPathCur = &g_Choco0CameraTargetPath;
            g_Choco0CameraPathOther = &g_Choco0CameraEyePath;
        }
    }
    if (effect->u.ptr.unk10) {
        SetRotMatrix(effect->u.ptr.unk10);
        SetTransMatrix(effect->u.ptr.unk10);
        if (g_Choco0CameraEyePath.ActorIndex == -1) {
            RotTrans(&g_Choco0CameraEyePath.Pos, vec, &flag);
            g_BattleCameraPos.vx = vec->vx;
            g_BattleCameraPos.vy = vec->vy;
            g_BattleCameraPos.vz = vec->vz;
        } else {
            ApplyRotMatrix(&g_Choco0CameraEyePath.Pos, vec);
            if (g_Choco0CameraEyePath.PartIndex != -1) {
                BattleGetPartPosition(g_Choco0CameraEyePath.ActorIndex, g_Choco0CameraEyePath.PartIndex, sv0);
            } else {
                sv0->vx = g_BattleModels[g_Choco0CameraEyePath.ActorIndex].rootTrans.vx;
                sv0->vy = g_BattleModels[g_Choco0CameraEyePath.ActorIndex].rootTrans.vy;
                sv0->vz = g_BattleModels[g_Choco0CameraEyePath.ActorIndex].rootTrans.vz;
            }
            g_BattleCameraPos.vx = vec->vx + sv0->vx;
            g_BattleCameraPos.vy = vec->vy + sv0->vy;
            g_BattleCameraPos.vz = vec->vz + sv0->vz;
            SetRotMatrix(effect->u.ptr.unk10);
            SetTransMatrix(effect->u.ptr.unk10);
        }
        if (g_Choco0CameraTargetPath.ActorIndex == -1) {
            RotTrans(&g_Choco0CameraTargetPath.Pos, vec, &flag);
            g_BattleCameraTarget.vx = vec->vx;
            g_BattleCameraTarget.vy = vec->vy;
            g_BattleCameraTarget.vz = vec->vz;
        } else {
            ApplyRotMatrix(&g_Choco0CameraTargetPath.Pos, vec);
            if (g_Choco0CameraTargetPath.PartIndex != -1) {
                BattleGetPartPosition(g_Choco0CameraTargetPath.ActorIndex, g_Choco0CameraTargetPath.PartIndex, sv0);
            } else {
                sv0->vx = g_BattleModels[g_Choco0CameraTargetPath.ActorIndex].rootTrans.vx;
                sv0->vy = g_BattleModels[g_Choco0CameraTargetPath.ActorIndex].rootTrans.vy;
                sv0->vz = g_BattleModels[g_Choco0CameraTargetPath.ActorIndex].rootTrans.vz;
            }
            g_BattleCameraTarget.vx = vec->vx + sv0->vx;
            g_BattleCameraTarget.vy = vec->vy + sv0->vy;
            g_BattleCameraTarget.vz = vec->vz + sv0->vz;
        }
    } else {
        g_BattleCameraPos = g_Choco0CameraEyePath.Pos;
        g_BattleCameraTarget = g_Choco0CameraTargetPath.Pos;
    }
}

void func_801B103C(s16* script, MATRIX* arg1, s32 callbackArg) {
    Choco0Data* effect;

    effect = &g_BattleEffectSlots[BattleEffectRegister(Choco0UpdateCamera)];
    effect->u.ptr.Script = script;
    effect->u.ptr.unk10 = arg1;
    effect->unk14 = callbackArg;
}

MATRIX* func_801B10A0(SVECTOR* pos, s32 scale, s32 depthBias) {
    VECTOR dir;
    s32 flag;

    D_801E5898.m[0][0] = D_801E5898.m[1][1] = D_801E5898.m[2][2] = scale;
    SetRotMatrix(&D_801E59F0);
    SetTransMatrix(&D_801E59F0);
    RotTrans(pos, (VECTOR*)D_801E5898.t, &flag);
    if (depthBias) {
        VectorNormal((VECTOR*)D_801E5898.t, &dir);
        D_801E5898.t[0] += (depthBias * dir.vx) >> 12;
        D_801E5898.t[1] += (depthBias * dir.vy) >> 12;
        D_801E5898.t[2] += (depthBias * dir.vz) >> 12;
    }
    SetRotMatrix(&D_801E5898);
    SetTransMatrix(&D_801E5898);
    return &D_801E5898;
}

void func_801B11BC(void) {
    Choco0Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    func_801B10A0(&effect->Pos, effect->unk14, 0);
    D_801E58B8.frames = D_801D2574[effect->AnimationFrame];
    D_80163C74 = func_800D4D90(&D_801E58B8, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 16) {
            effect->StartFrame = -1;
            return;
        }
        effect->Pos.vx += effect->u.vec.vx;
        effect->Pos.vy += effect->u.vec.vy;
        effect->Pos.vz += effect->u.vec.vz;
        effect->u.vec.vx = (effect->u.vec.vx * 7) >> 3;
        effect->u.vec.vz = (effect->u.vec.vz * 7) >> 3;
    }
}

void func_801B12DC(void) {
    Choco0Data* effect;
    Choco0Data* child;
    s32 i;
    s32 speed;
    s32 angle;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        for (i = 0; i < 3; i++) {
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B11BC)];
            child->Pos.vx = D_801E575C.vx;
            child->Pos.vy = D_801E575C.vy;
            child->Pos.vz = D_801E575C.vz;
            speed = rand() % 100 + 100;
            angle = rand() & 0x7FF;
            child->u.vec.vx = (rcos(angle) * speed) >> 12;
            child->u.vec.vy = -(rand() % 30 + 20);
            child->u.vec.vz = (-rsin(angle) * speed) >> 12;
            child->unk14 = rand() % 0x800 + 0x1000;
        }
        if (++effect->AnimationFrame >= 60) {
            effect->StartFrame = -1;
        }
    }
}

void Choco0RenderBoom(void) {
    Choco0Data* effect;
    s32 phase;
    s16 scale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    phase = effect->AnimationFrame % 7;
    if (phase < 3) {
        scale = (phase << 12) / 3 + 0x1000;
        D_801E58D4.m[1][1] = scale;
        D_801E58D4.m[0][0] = scale;
    } else {
        phase -= 3;
        scale = -(phase << 12) / 4 + 0x2000;
        D_801E58D4.m[1][1] = scale;
        D_801E58D4.m[0][0] = scale;
    }
    D_801E58D4.t[2] = ReadGeomScreen() * 8;
    SetRotMatrix(&D_801E58D4);
    SetTransMatrix(&D_801E58D4);
    D_80163C74 = BattleEffectSpriteAdd(&D_801E58C4, &g_cDb->unk4080[1], 0, D_80163C74);
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 20) {
            effect->StartFrame = -1;
        }
    }
}

void Choco0MoveModel(void) {
    Choco0Data* effect;
    s32 frame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        frame = effect->AnimationFrame;
        if (frame < 60) {
            D_801E575C.vz = frame * 250 - 15000;
        } else if ((frame -= 60) < 20) {
            if (frame == 19) {
                D_801E575C.vz = -5000;
                D_801E575C.vx = -750;
                g_BattleModels[3].rootRot.vy += 0x400;
            }
        } else if ((frame -= 20) >= 50) {
            effect->StartFrame = -1;
            return;
        }
        SetRotMatrix(&D_801E59D0);
        SetTransMatrix(&D_801E59D0);
        RotTrans(&D_801E575C, D_801E58F4, (s32*)(D_801E58F4 + 1));
        g_BattleModels[3].rootTrans.vx = D_801E58F4->vx;
        g_BattleModels[3].rootTrans.vy = D_801E58F4->vy;
        g_BattleModels[3].rootTrans.vz = D_801E58F4->vz;
        effect->AnimationFrame++;
    }
}

void Choco0RenderStars(void) {
    Choco0Data* effect;
    s32 i;
    s32 angle;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    angle = effect->u.vec.vy;
    for (i = 0; i < 4; i++) {
        D_801E5904->vx = effect->Pos.vx + ((rsin(angle) * 150) >> 12);
        D_801E5904->vy = effect->Pos.vy;
        D_801E5904->vz = effect->Pos.vz + ((rcos(angle) * 150) >> 12);
        angle += 0x400;
        func_801B10A0(D_801E5904, 0x500, 0);
        D_80163C74 = func_800D4D90(&D_801E58F8, g_cDb->unk70, 12, D_80163C74);
    }
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 35) {
            effect->StartFrame = -1;
            return;
        }
        effect->u.vec.vy += 0x40;
    }
}

void func_801B18BC(void) {
    Choco0Data* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    func_801B10A0(&effect->Pos, effect->unk14, 0);
    D_801E5908.frames = D_801D2574[effect->AnimationFrame];
    D_80163C74 = func_800D4D90(&D_801E5908, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 16) {
            effect->StartFrame = -1;
            return;
        }
        effect->Pos.vy += effect->u.vec.vy;
    }
}

void func_801B1998(void) {
    Choco0Data* effect;
    Choco0Data* child;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        for (i = 0; i < 3; i++) {
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B18BC)];
            child->Pos.vx = rand() % 4000 - 2000;
            child->Pos.vy = 0;
            child->Pos.vz = rand() % 3000 - 3000;
            child->u.vec.vx = 0;
            child->u.vec.vy = -(rand() % 30 + 20);
            child->u.vec.vz = 0;
            child->unk14 = rand() % 0x2000 + 0x1000;
        }
        if (++effect->AnimationFrame >= 50) {
            effect->StartFrame = -1;
        }
    }
}

void Choco0RenderSwirlEyes(void) {
    Choco0Data* effect;
    s32 flag;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    SetRotMatrix(&g_BattleModels[3].boneTransforms[13].m);
    SetTransMatrix(&g_BattleModels[3].boneTransforms[13].m);
    RotTrans(&D_801E5914, (VECTOR*)D_801E5924.t, &flag);
    RotTrans(&D_801E591C, (VECTOR*)D_801E5944.t, &flag);
    D_801E5964.frameIndex = effect->AnimationFrame & 7;
    SetRotMatrix(&D_801E5924);
    SetTransMatrix(&D_801E5924);
    D_80163C74 = func_800D4D90(&D_801E5964, &g_cDb->unk4080[1], 0, D_80163C74);
    SetRotMatrix(&D_801E5944);
    SetTransMatrix(&D_801E5944);
    D_80163C74 = func_800D4D90(&D_801E5964, &g_cDb->unk4080[1], 0, D_80163C74);
    if (D_80062D98 == 0) {
        if (++effect->AnimationFrame >= 50) {
            effect->StartFrame = -1;
        }
    }
}

void Choco0AnimationUpdate(void) {
    Choco0Data* effect;
    Choco0Data* child;
    s16* event;
    s32 frame;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    CompMatrix(&D_800FA63C.m, &D_801E59D0, &D_801E59F0);
    if (D_80062D98 == 0) {
        frame = effect->AnimationFrame;
        if (frame < 5) {
            if (frame == 4) {
                event = BattleEventQueuePush(1);
                event[2] = 0;
                event[3] = 0;
                event[4] = D_801E59D0.t[2] - 15000;
                event[8] = D_801E5754.vy + 0x800;
            }
        } else if ((frame -= 5) < 20) {
            if (frame == 0) {
                BattleEffectRegister(func_801B12DC);
                BattleEffectRegister(Choco0MoveModel);
                BattleAkaoCommand(AKAO_PLAY_THREE_SOUNDS, AKAO_PAN_CENTER, SFX_24D, SFX_24E, SFX_24F);
            }
        } else if ((frame -= 20) < 20) {
        } else if ((frame -= 20) < 20) {
        } else if ((frame -= 20) < 20) {
            if (frame == 0) {
                BattleEffectRegister(Choco0RenderBoom);
                BattleEffectRegister(func_801B1998);
            }
            if (frame == 19) {
                BattleEnqueueClearImage(&D_801E5970, 0, 0, 0);
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
                BattleEventQueuePush(2);
            }
        } else if ((frame -= 20) < 15) {
            if (frame == 0) {
                for (i = 0; i < 10; i++) {
                    if ((g_Choco0TargetMask >> i) & 1) {
                        func_800D5774(i);
                    }
                }
            }
        } else {
            effect->StartFrame = -1;
        }
        effect->AnimationFrame++;
    }
}

void Choco0MainSetup(s32 targetMask, s32 callbackArg) {
    SVECTOR center;

    BattleSetLoadTimToVram(g_Choco0Texture, 0, 0, 0);
    g_Choco0TargetMask = targetMask;
    BattleEntityGetCenter(targetMask, &center);
    D_801E59D0.t[0] = D_801E59D0.t[1] = 0;
    D_801E59D0.t[2] = center.vz;
    if (center.vz < g_BattleModels[callbackArg].rootTrans.vz) {
        D_801E5754.vy = 0x800;
    }
    RotMatrixYXZ(&D_801E5754, &D_801E59D0);
    BattleEffectRegister(Choco0AnimationUpdate);
    if (rand() & 0x100) {
        func_801B103C(D_801E5764, &D_801E59D0, callbackArg);
    } else {
        func_801B103C(D_801E5804, &D_801E59D0, callbackArg);
    }
}
