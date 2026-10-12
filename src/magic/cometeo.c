//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include <libetc.h>
#include "magic.h"
#include "magic_private.h"
#include "cometeo.h"
#include "../battle/battle.h"

// Comet2 (コメテオ / Cometeo), tier 2.

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR Vel;
    /* 0x14 */ u8 Rot[3];
    /* 0x17 */ u8 pad17;
    /* 0x18 */ u8 RotVel[3];
    /* 0x1B */ u8 pad1B;
    /* 0x1C */ s16 Scale;
    /* 0x1E */ s16 ModelIndex;
} CometeoData; // size:0x20

extern CometeoData g_BattleEffectSlots[];
extern u_long D_801B154C[];
extern u_long D_801D4260[];
extern void* D_801EC260;
extern s32 D_801D4250;
extern s32 D_801D4254;
extern SVECTOR D_801D4258;
extern u8 D_801517BC;
extern u8 D_801517F0[];
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;
extern s32 D_801D162C[];
extern s32 D_801D1C90[];
extern s32 D_800EA294[];
extern s32 D_801D23EC[];
extern s32 D_801D3D3C[];
extern s32 D_801D40A4[];
extern s32 D_801D3A88[];
extern s32 D_801D40A8;
extern s32 D_801D3EF0[];

static ModelRenderDesc cometeo_model_desc0 = {D_801D162C, MODEL_DEPTH_CUE, 0, 0, 0, 0};
static ModelRenderDesc cometeo_model_desc1 = {D_801D1C90, MODEL_DEPTH_CUE | MODEL_SEMI_TRANS, 0, 0, 0x20, 0};
static SVECTOR cometeo_rot = {0, 0, 0, 0};
static VECTOR cometeo_scale = {0xA00, 0xA00, 0xA00, 0};
static MATRIX cometeo_matrix0 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
static ModelRenderDesc cometeo_model_desc2 = {D_800EA294, MODEL_DEPTH_CUE | MODEL_SEMI_TRANS, 0, 0, 0x40, 0};
static ModelRenderDesc cometeo_model_desc3 = {D_801D23EC, MODEL_DEPTH_CUE | MODEL_SEMI_TRANS, 0, 0, 0x20, 0};
static MATRIX cometeo_matrix1 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
static ModelRenderDesc cometeo_model_desc4 = {D_801D3D3C, MODEL_DEPTH_CUE, 0, 0, 0x20, 0};
static MATRIX cometeo_matrix2 = {{{0x500, 0, 0}, {0, 0x500, 0}, {0, 0, 0x500}}, {0, 0, 0}};
static ModelRenderDesc cometeo_model_desc5 = {D_800EA294, MODEL_DEPTH_CUE | MODEL_SEMI_TRANS, 0, 0, 0x40, 0};
static SpriteRenderDesc cometeo_render_desc = {(SpriteAnim*)D_801D40A4, {0x80, 0x80, 0x80, 0x2E}, 0, 0};

void CometeoStart(s32 targetMask, s32 callbackArg);
void func_801B0020(void);
void CometeoRockTick(void);
void func_801B0608(void);
void func_801B0844(void);
void func_801B0B88(void);
void CometeoDriverTick(void);
void CometeoMainTick(void);
void CometeoBufferFlip(void);

// Same as MAGIC_Biora.
void MAGIC_Cometeo(s32 targetMask, s32 callbackArg) { CometeoStart(targetMask, callbackArg); }

void func_801B0020(void) {
    CometeoData* effect;
    TILE_1* prim;
    long depth;
    long xy;
    long flag;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    prim = D_801EC260;
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    depth = RotTransPers(&effect->Pos, (long*)&prim->x0, &xy, &flag);
    if ((flag & 0x60000) == 0) {
        SetTile1(prim);
        prim->r0 = 0xFF;
        prim->g0 = rand();
        prim->b0 = 0;
        AddPrim(&g_cDb->unk70[depth >> 2], prim);
        D_801EC260 = prim + 1;
    }
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame < 6) {
            effect->Pos.vx += effect->Vel.vx;
            effect->Pos.vy += effect->Vel.vy;
            effect->Pos.vz += effect->Vel.vz;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 8) {
            effect->StartFrame = -1;
        }
    }
}

void CometeoRockTick(void) {
    CometeoData* effect;
    CometeoData* child;
    SVECTOR dir;
    s32 flag;
    MATRIX* m0;
    MATRIX* m1;
    MATRIX* m2;
    SVECTOR* tmp;
    VECTOR* out;
    s32 i;
    s32 angle;

    m0 = (MATRIX*)getScratchAddr(0);
    m1 = (MATRIX*)getScratchAddr(8);
    m2 = (MATRIX*)getScratchAddr(16);
    tmp = (SVECTOR*)getScratchAddr(24);
    out = (VECTOR*)getScratchAddr(26);
    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    dir.vx = -effect->Vel.vx;
    dir.vy = -effect->Vel.vy;
    dir.vz = -effect->Vel.vz;
    BattleMatrixFromDirection(&dir, m0);
    m0->t[0] = effect->Pos.vx;
    m0->t[1] = effect->Pos.vy;
    m0->t[2] = effect->Pos.vz;
    ScaleMatrix(m0, &cometeo_scale);
    CompMatrix(&g_BattleWorldView.m, m0, m1);
    SetRotMatrix(m1);
    SetTransMatrix(m1);
    D_801EC260 = func_800D29D4(&cometeo_model_desc1, g_cDb->unk70, 12, D_801EC260);
    RotMatrixYXZ(&cometeo_rot, m2);
    CompMatrix(m1, m2, m2);
    SetRotMatrix(m2);
    D_801EC260 = func_800D29D4(&cometeo_model_desc0, g_cDb->unk70, 12, D_801EC260);
    cometeo_matrix0.t[0] = effect->Pos.vx;
    cometeo_matrix0.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &cometeo_matrix0, m1);
    SetRotMatrix(m1);
    SetTransMatrix(m1);
    D_801EC260 = func_800D29D4(&cometeo_model_desc2, g_cDb->unk4078, 1, D_801EC260);
    if (D_80062D98 == 0) {
        SetRotMatrix(m0);
        SetTransMatrix(m0);
        for (i = 0; i < 3; i++) {
            angle = (i << 12) / 3 + rand() % 1365;
            tmp->vx = (u32)(rsin(angle) * 125) >> 9;
            tmp->vy = rcos(angle) * 1000 >> 12;
            tmp->vz = rand() % 1000;
            RotTrans(tmp, out, &flag);
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0020)];
            child->Pos.vx = out->vx;
            child->Pos.vy = out->vy;
            child->Pos.vz = out->vz;
            tmp->vz = 0;
            ApplyRotMatrix(tmp, out);
            child->Vel.vx = -out->vx / 6;
            child->Vel.vy = -out->vy / 6;
            child->Vel.vz = -out->vz / 6;
        }
        cometeo_model_desc1.uvOffset ^= 0x80;
        cometeo_rot.vx += 0x80;
        cometeo_rot.vy += 0x100;
        cometeo_rot.vz += 0x180;
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= 10) {
            effect->StartFrame = -1;
        }
        effect->Pos.vx += effect->Vel.vx;
        effect->Pos.vy += effect->Vel.vy;
        effect->Pos.vz += effect->Vel.vz;
    }
}

void func_801B0608(void) {
    CometeoData* effect;
    s32 frame;
    s32 delta;
    s32 scale;
    MATRIX* scratch = (MATRIX*)getScratchAddr(0);

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    frame = effect->AnimationFrame;
    if (frame < 10) {
        cometeo_model_desc3.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
        cometeo_model_desc3.color = -(frame << 12) / 10 + 0x1000;
        SetFarColor(0xFF, 0xFF, 0xFF);
    } else {
        delta = frame - 10;
        if (delta < 0) { // match: unreachable, the original keeps this arm
            cometeo_model_desc3.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
            cometeo_model_desc3.color = 0;
        } else if (delta < 10) {
            cometeo_model_desc3.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
            cometeo_model_desc3.color = (delta << 12) / 10;
            SetFarColor(0, 0, 0);
        } else {
            effect->StartFrame = -1;
            return;
        }
    }
    scale = effect->AnimationFrame * 9 * 512 / 10 + 0x800;
    cometeo_matrix1.m[0][0] = cometeo_matrix1.m[1][1] = cometeo_matrix1.m[2][2] = scale;
    cometeo_matrix1.t[0] = effect->Pos.vx;
    cometeo_matrix1.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &cometeo_matrix1, scratch);
    SetRotMatrix(scratch);
    SetTransMatrix(scratch);
    cometeo_model_desc3.model = D_801D23EC;
    D_801EC260 = func_800D29D4(&cometeo_model_desc3, g_cDb->unk70, 12, D_801EC260);
    cometeo_model_desc3.model = D_801D3A88;
    cometeo_model_desc3.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
    D_801EC260 = func_800D29D4(&cometeo_model_desc3, g_cDb->unk70, 12, D_801EC260);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

void func_801B0844(void) {
    CometeoData* effect;
    MATRIX m;
    VECTOR scale;
    SVECTOR rot;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (effect->ModelIndex == 0) {
        cometeo_model_desc4.model = D_801D3D3C;
    } else {
        cometeo_model_desc4.model = D_801D3EF0;
    }
    if (effect->StartFrame < 2) {
        cometeo_model_desc4.flags = MODEL_DEPTH_CUE;
        cometeo_model_desc4.color = 0;
    } else {
        cometeo_model_desc4.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
        cometeo_model_desc4.color = effect->AnimationFrame * 0x200;
    }
    rot.vx = effect->Rot[0] << 4;
    rot.vy = effect->Rot[1] << 4;
    rot.vz = effect->Rot[2] << 4;
    RotMatrixYXZ(&rot, &m);
    m.t[0] = effect->Pos.vx;
    m.t[1] = effect->Pos.vy;
    m.t[2] = effect->Pos.vz;
    scale.vx = scale.vy = scale.vz = effect->Scale;
    ScaleMatrix(&m, &scale);
    CompMatrix(&g_BattleWorldView.m, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    SetFarColor(0, 0, 0);
    D_801EC260 = func_800D29D4(&cometeo_model_desc4, g_cDb->unk70, 12, D_801EC260);
    cometeo_matrix2.t[0] = effect->Pos.vx;
    cometeo_matrix2.t[1] = 0;
    cometeo_matrix2.t[2] = effect->Pos.vz;
    CompMatrix(&g_BattleWorldView.m, &cometeo_matrix2, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    D_801EC260 = func_800D29D4(&cometeo_model_desc5, g_cDb->unk4078, 1, D_801EC260);
    if (D_80062D98 == 0) {
        if (effect->StartFrame >= 2) {
            effect->AnimationFrame++;
            if (effect->AnimationFrame >= 8) {
                effect->StartFrame = -1;
                return;
            }
        }
        effect->Pos.vx += effect->Vel.vx;
        effect->Pos.vy += effect->Vel.vy;
        effect->Pos.vz += effect->Vel.vz;
        effect->Vel.vy += 0x32;
        if (effect->Pos.vy > 0) {
            effect->Pos.vy = -effect->Pos.vy;
            effect->Vel.vy = -effect->Vel.vy / 4;
            effect->Vel.vx >>= 1;
            effect->Vel.vz >>= 1;
            effect->StartFrame++;
            effect->RotVel[0] >>= 1;
            effect->RotVel[1] >>= 1;
            effect->RotVel[2] >>= 1;
        }
        effect->Rot[0] += effect->RotVel[0];
        effect->Rot[1] += effect->RotVel[1];
        effect->Rot[2] += effect->RotVel[2];
    }
}

void func_801B0B88(void) {
    CometeoData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    cometeo_render_desc.frameIndex = effect->AnimationFrame;
    BattleSetBillboardMatrix(&effect->Pos, 0x2500, 0);
    D_801EC260 = func_800D4D90(&cometeo_render_desc, g_cDb->unk70, 12, D_801EC260);
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame >= D_801D40A8) {
            effect->StartFrame = -1;
        }
    }
}

void CometeoDriverTick(void) {
    CometeoData* effect;
    CometeoData* child;
    VECTOR dir;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 != 0) {
        return;
    }
    if (effect->AnimationFrame == 0) {
        child = &g_BattleEffectSlots[BattleEffectRegister(CometeoRockTick)];
        child->Pos.vx =
            g_BattleModels[D_801D4250].rootTrans.vx + (rsin(g_BattleModels[D_801D4250].rootRot.vy) * 3000 >> 12);
        child->Pos.vy = -6000;
        child->Pos.vz =
            g_BattleModels[D_801D4250].rootTrans.vz + (rcos(g_BattleModels[D_801D4250].rootRot.vy) * 3000 >> 12);
        child->Vel.vx = (effect->Pos.vx - child->Pos.vx) / 10;
        dir.vx = child->Vel.vx;
        child->Vel.vy = (effect->Pos.vy - child->Pos.vy) / 10;
        dir.vy = child->Vel.vy;
        child->Vel.vz = (effect->Pos.vz - child->Pos.vz) / 10;
        dir.vz = child->Vel.vz;
        VectorNormal(&dir, &dir);
        child->Pos.vx -= dir.vx * 500 >> 12;
        child->Pos.vy -= dir.vy * 500 >> 12;
        child->Pos.vz -= dir.vz * 500 >> 12;
    }
    if (effect->AnimationFrame == 9) {
        child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0608)];
        child->Pos = effect->Pos;
        for (i = 0; i < 5; i++) {
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0844)];
            child->Pos = effect->Pos;
            child->Pos.vy = 0;
            child->Vel.vx = rand() % 500 - 250;
            child->Vel.vy = -(rand() % 200 + 300);
            child->Vel.vz = rand() % 500 - 250;
            child->RotVel[0] = (rand() & 0x3F) - 0x20;
            child->RotVel[1] = (rand() & 0x3F) - 0x20;
            child->RotVel[2] = (rand() & 0x3F) - 0x20;
            child->Scale = rand() % 0xE00 + 0x200;
            child->StartFrame = 0;
            child->ModelIndex = rand() & 1;
        }
    }
    if ((u32)((u16)effect->AnimationFrame - 10) < 15) {
        for (i = 0; i < 1; i++) {
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0B88)];
            child->Pos.vx = effect->Pos.vx + (rand() % 0xA00 - 0x500);
            child->Pos.vy = effect->Pos.vy - rand() % 0x500;
            child->Pos.vz = effect->Pos.vz + (rand() % 0xA00 - 0x500);
        }
    }
    if (effect->AnimationFrame == 25) {
        effect->StartFrame = -1;
    }
    effect->AnimationFrame++;
}

void CometeoMainTick(void) {
    CometeoData* effect;
    CometeoData* child;
    s16* event;
    s32 frame;
    s32 id;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        frame = effect->AnimationFrame;
        if (frame == 0) {
            BattleFixedPointRampUpdateInit(0x800, 8);
            D_800F5B70 = D_800F5B71 = D_800F5B72 = 0;
        } else if ((frame -= 8) >= 0) {
            if (frame - 0x14 >= 0 && ((frame - 0x14) & 7) == 0) {
                id = D_801517F0[effect->StartFrame];
                if (id != 0xFF) {
                    event = BattleEventQueuePush(10);
                    event[0] = id;
                    event[8] = effect->StartFrame;
                    effect->StartFrame++;
                }
            }
            if (frame < 0x3C) {
                if (frame % 10 == 0) {
                    child = &g_BattleEffectSlots[BattleEffectRegister(CometeoDriverTick)];
                    child->Pos.vx = D_801D4258.vx + rand() % 6000 - 3000;
                    child->Pos.vy = 0;
                    child->Pos.vz = D_801D4258.vz + rand() % 6000 - 3000;
                }
            } else {
                effect->StartFrame = -1;
            }
        }
        effect->AnimationFrame++;
    }
}

// Same as CometBufferFlip.
void CometeoBufferFlip(void) {
    CometeoData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801EC260 = effect->AnimationFrame == 0 ? D_801D4260 : D_801B154C;
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        BattleFixedPointRampUpdateInit(0, 4);
        effect->StartFrame = -1;
    }
}

void CometeoStart(s32 targetMask, s32 callbackArg) {
    BattleEffectRegister(CometeoBufferFlip);
    D_801D4254 = targetMask;
    D_801D4250 = callbackArg;
    BattleEntityGetCenter(targetMask, &D_801D4258);
    if (D_801517BC == 0) {
        BattleSetLoadTimToVram(D_801B154C, 0, 0, 0);
    }
    g_BattleEffectSlots[BattleEffectRegister(CometeoMainTick)].StartFrame = 0;
    BattleAkaoCommand(AKAO_PLAY_TWO_SOUNDS, BattleEntityGetStereoPan(targetMask), 0x94, 0x19C);
}
