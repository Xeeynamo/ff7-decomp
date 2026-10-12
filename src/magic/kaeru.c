//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "kaeru.h"
#include "../battle/battle.h"

// Frog Song (カエルのうた / Kaeru no Uta).

// Only StartFrame/AnimationFrame are common to every magic overlay; the
// rest is payload this effect lays out for itself.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ s16 unkC;
    /* 0x0E */ s16 unkE;
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ char pad1A[6];
} KaeruData; // size:0x20
typedef struct {
    /* 0x0 */ SVECTOR Pos;
    /* 0x8 */ s16 unk8;
    /* 0xA */ char padA[6];
} KaeruBillboard; // size:0x10
extern KaeruData g_BattleEffectSlots[];
extern SVECTOR D_801D52A0[];
extern SpriteRenderDesc D_801BD268;
extern SpriteRenderDesc D_801BD274;

void func_801B0D08(s32 targetMask, s32 callbackArg);

// Scratchpad block read by func_801B0EB4 and func_801B2F08.
typedef struct {
    /* 0x00 */ s32* model;
    /* 0x04 */ u_long* ot;
    /* 0x08 */ s32 count;
    /* 0x0C */ void* buffer;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ char pad1C[4];
    /* 0x20 */ s32 unk20;
    /* 0x24 */ char pad24[4];
    /* 0x28 */ s32 unk28;
    /* 0x2C */ s32* unk2C;
    /* 0x30 */ char pad30[4];
    /* 0x34 */ MATRIX view;
    /* 0x54 */ char pad54[0x68];
    /* 0xBC */ SVECTOR v0;
    /* 0xC4 */ SVECTOR v1;
    /* 0xCC */ SVECTOR v2;
} KaeruScratch;
extern char D_801BD290[];
extern char D_801C9290[];
extern void* D_801D5290;
extern s32* D_801D5294; // slot running func_801B0000
extern s32 D_801D5298;
extern s32 D_801D529C;
extern u_long D_801B0DDC[];
void func_801B0B68(void);
void func_801B09E8(void);
void func_801B0838(void);
extern SVECTOR g_BattleCameraPos;
extern VECTOR D_801BD280;
extern MATRIX D_801D5990;
void func_801B05A8(void);

void func_801B0000(void) {
    D_801D5290 = g_dbIndex == 0 ? D_801BD290 : D_801C9290;
    if (g_BattleEffectCount < 2) {
        *D_801D5294 = -1;
    }
}

void func_801B0054(s32 targetMask, s32 callbackArg) { func_801B0D08(targetMask, callbackArg); }

s32 func_801B0074(VECTOR* a, VECTOR* b, VECTOR* out) {
    char pad[0x10]; // match: gives the function its 0x30 byte stack frame
    s32 dot;
    s32 len;
    s32 angle;

    dot = (a->vx * b->vx + a->vy * b->vy + a->vz * b->vz) >> 12;
    OuterProduct12(a, b, out);
    len = SquareRoot12((out->vx * out->vx + out->vy * out->vy + out->vz * out->vz) >> 12);
    angle = ratan2(len, dot);
    VectorNormal(out, out);
    return angle;
}

void func_801B0150(s32 angle, MATRIX* m, VECTOR* axis) {
    s32 sn;
    s32 cs;
    s32 omc;
    s32 x;
    s32 y;
    s32 z;
    s32 xx;
    s32 yy;
    s32 zz;

    sn = rsin(angle);
    cs = rcos(angle);
    omc = 0x1000 - cs;
    x = axis->vx;
    y = axis->vy;
    z = axis->vz;
    xx = (x * x) >> 12;
    yy = (y * y) >> 12;
    zz = (z * z) >> 12;
    m->m[0][0] = xx + (((0x1000 - xx) * cs) >> 12);
    m->m[0][1] = ((((x * y) >> 12) * omc) >> 12) - ((z * sn) >> 12);
    m->m[0][2] = ((((x * z) >> 12) * omc) >> 12) + ((y * sn) >> 12);
    m->m[1][0] = ((((x * y) >> 12) * omc) >> 12) + ((z * sn) >> 12);
    m->m[1][1] = yy + (((0x1000 - yy) * cs) >> 12);
    m->m[1][2] = ((((y * z) >> 12) * omc) >> 12) - ((x * sn) >> 12);
    m->m[2][0] = ((((x * z) >> 12) * omc) >> 12) - ((y * sn) >> 12);
    m->m[2][1] = ((((y * z) >> 12) * omc) >> 12) + ((x * sn) >> 12);
    m->m[2][2] = zz + (((0x1000 - zz) * cs) >> 12);
}

void func_801B02CC(SVECTOR* a, SVECTOR* b, VECTOR* n) {
    VECTOR d;
    s32 dot;

    d.vx = a->vx - b->vx;
    d.vy = a->vy - b->vy;
    d.vz = a->vz - b->vz;
    dot = (n->vx * d.vx + n->vy * d.vy + n->vz * d.vz) >> 12;
    a->vx -= (dot * n->vx) >> 12;
    a->vy -= (dot * n->vy) >> 12;
    a->vz -= (dot * n->vz) >> 12;
}

s16 func_801B03A8(SVECTOR* a, SVECTOR* b, VECTOR* n) {
    VECTOR dir;
    s32 pa;
    s32 pb;
    s32 pd;

    dir.vx = 0;
    dir.vy = -0x1000;
    dir.vz = 0;
    pa = a->vx * n->vx + a->vy * n->vy + a->vz * n->vz;
    pb = b->vx * n->vx + b->vy * n->vy + b->vz * n->vz;
    pd = (dir.vx * n->vx + dir.vy * n->vy + dir.vz * n->vz) >> 12;
    return (pb - pa) / pd;
}

// Same as func_801B0074.
void func_801B048C(void) {
    KaeruData* effect;
    s32 i;
    s32 n;
    s16 scale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801BD268.frameIndex = effect->AnimationFrame;
    n = effect->unk18;
    for (i = 0; i < 4; i++) {
        scale = (u16)D_801D52A0[n].pad;
        BattleSetBillboardMatrix(&D_801D52A0[n], scale, -(scale >> 2));
        D_801D5290 = func_800D4D90(&D_801BD268, (u_long**)&g_cDb->unk70, 12, D_801D5290);
        n++;
        if (n >= 222) {
            n = 0;
        }
    }
    if (effect->AnimationFrame > 10) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

void func_801B05A8(void) {
    KaeruData* effect;
    KaeruData* next;
    s32 i;
    s32 n;
    s32 scale;

    if (D_80062D98 == 0) {
        effect = &g_BattleEffectSlots[g_BattleEffectCursor];
        if ((u16)(effect->AnimationFrame - 0x12) < 0x24) {
            n = D_801D529C;
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B048C)];
            next->unk18 = n;
            for (i = 0; i < 4; i++) {
                scale = rand() % 0x1000 + 0x800;
                D_801D52A0[n].vx = effect->Pos.vx + rand() % effect->unkE - effect->unkC;
                D_801D52A0[n].vy = -(rand() % effect->unk10);
                D_801D52A0[n].vz = effect->Pos.vz + rand() % effect->unkE - effect->unkC;
                D_801D52A0[n].pad = effect->unk14 * scale >> 12;
                n++;
                if (n >= 222) {
                    n = 0;
                }
            }
            D_801D529C = n;
        }
        if (effect->AnimationFrame == 0x11) {
            BattleAkaoCommand(AKAO_PLAY_SOUND, BattlePositionToStereoPan(&effect->Pos), 0x134);
        }
        if (effect->AnimationFrame == 0x1E) {
            func_800D5774(D_801D5298);
        }
        if (effect->AnimationFrame >= 0x3C) {
            effect->StartFrame = -1;
        }
        effect->AnimationFrame++;
    }
}

void func_801B0838(void) {
    KaeruData* effect;
    SVECTOR rot;
    SVECTOR pos;
    MATRIX mat;
    s16 scale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = effect->unk10;
    pos.vx = 0;
    pos.vy = -effect->unk14;
    pos.vz = 0;
    RotMatrix(&rot, &mat);
    ApplyMatrixSV(&mat, &pos, &pos);
    ApplyMatrixSV(&D_801D5990, &pos, &pos);
    pos.vx += effect->Pos.vx;
    pos.vy += effect->Pos.vy;
    pos.vz += effect->Pos.vz;
    if (effect->AnimationFrame >= 8) {
        D_801BD274.color.r = D_801BD274.color.g = D_801BD274.color.b = -0x80 - (effect->AnimationFrame - 8) * 32;
    }
    scale = effect->unkC;
    BattleSetBillboardMatrix(&pos, scale, -(scale >> 2));
    D_801D5290 = func_800D4D90(&D_801BD274, (u_long**)&g_cDb->unk70, 12, D_801D5290);
    if (effect->AnimationFrame >= 12) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->unk10 += effect->unk12;
        effect->unk14 += effect->unk16;
        effect->AnimationFrame++;
    }
}

void func_801B09E8(void) {
    KaeruData* effect;
    KaeruData* next;
    VECTOR dir;
    VECTOR unit;
    VECTOR normal;
    s32 angle;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    dir.vx = g_BattleCameraPos.vx - effect->Pos.vx;
    dir.vy = 0;
    dir.vz = g_BattleCameraPos.vz - effect->Pos.vz;
    VectorNormal(&dir, &unit);
    angle = func_801B0074(&D_801BD280, &unit, &normal);
    func_801B0150(angle, &D_801D5990, &normal);
    if (D_80062D98 == 0) {
        if ((u16)(effect->AnimationFrame - 1) < 0x14) {
            next = &g_BattleEffectSlots[BattleEffectRegister(func_801B0838)];
            next->Pos = effect->Pos;
            next->unkC = effect->unkC;
            next->unk10 = (7 - (effect->AnimationFrame & 7)) << 9;
            next->unk12 = 0x60;
            next->unk14 = next->unk16 = (s16)effect->unkE >> 2;
        }
        if (effect->AnimationFrame > 0x14) {
            effect->StartFrame = -1;
        }
        effect->AnimationFrame++;
    }
}

void func_801B0B68(void) {
    KaeruData* effect;
    KaeruData* next;
    s32 radius;
    KaeruData* slots;

    g_BattleEffectSlots[g_BattleEffectCursor].StartFrame = -1;
    slots = g_BattleEffectSlots;
    effect = &slots[BattleEffectRegister(func_801B05A8)];
    BattleGetPartPosition(D_801D5298, g_BattleModels[D_801D5298].boneIndices[0], &effect->Pos);
    radius = g_BattleModels[D_801D5298].collisionRadius + 0xE0;
    if (radius > 0x4F2) {
        radius = 0x4F2;
    }
    effect->unkC = radius;
    effect->unkE = radius << 1;
    effect->unk10 = -((u16)effect->Pos.vy << 1);
    effect->unk14 = (radius << 12) / 450;
    next = &slots[BattleEffectRegister(func_801B09E8)];
    next->Pos = effect->Pos;
    next->unkE = radius;
    next->unkC = (radius << 12) / 1500;
}

void func_801B0D08(s32 targetMask, s32 callbackArg) {
    s32 i;
    s32 count;

    D_801D5294 = (s32*)&g_BattleEffectSlots[BattleEffectRegister(func_801B0000)];
    BattleSetLoadTimToVram(D_801B0DDC, 0, 0, 0);
    for (i = 0, count = 0; i < 10; i++) {
        if ((targetMask >> i) & 1) {
            count++;
            if (count == 1) {
                D_801D5298 = i;
            }
        }
    }
    BattleEffectRegister(func_801B0B68);
    D_801D529C = 0;
    BattleAkaoCommand(0x20, BattleEntityGetStereoPan(targetMask), 0x9B);
}
