//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include <libetc.h>
#include "magic.h"
#include "tornado.h"
#include "../battle/battle.h"

// Tornado (トルネド / Torunedo).

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ s16 unk4;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ s16 unk8;
    /* 0x0A */ s16 unkA;
    /* 0x0C */ s16 unkC;
    /* 0x0E */ s16 unkE;
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
} TornadoData;

typedef struct {
    /* 0x00 */ s16 state;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ s16 unk4;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ s16 unk8;
    /* 0x0A */ s16 unkA;
    /* 0x0C */ SVECTOR trail[8];
} TornadoParticle; // size:0x4C

extern TornadoData g_BattleEffectSlots[];
extern TornadoParticle D_801C2F34[100];
extern TornadoParticle D_801C4CE4[100];
extern u8 D_801C6A94[2][0x10000];
extern void* D_801E6A94;
extern s32 D_801C2EEC;
extern u_long D_801B1164[];
extern SVECTOR D_801C2F28;
extern TornadoData* D_801C2F18;
extern TornadoData* D_801C2F1C;
extern TornadoData* D_801C2F20;
extern CVECTOR D_800F5B70;
extern ModelRenderDesc D_801C2F00;
extern SVECTOR D_801C2F10;
extern s32 D_801C11C4[];
extern s32 D_801C1E98[];
extern s32 D_801C2F30;
extern BattleSpriteDesc D_801C2EF0;

void TornadoStart(s32 targetMask, s32 callbackArg);
void TornadoSpriteParticleTick(TornadoParticle* particle);
void TornadoRibbonParticleTick(TornadoParticle* particle);
void TornadoTargetTick(void);
void TornadoTargetSetup(s32 target, s32 callbackArg);
TornadoParticle* TornadoRibbonParticleAlloc(void);
TornadoParticle* TornadoSpriteParticleAlloc(void);
void TornadoDoubleBufferFlip(void);

void MAGIC_Tornado(s32 targetMask, s32 callbackArg) { TornadoStart(targetMask, callbackArg); }

TornadoParticle* TornadoSpriteParticleAlloc(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 100; i++) {
        if (D_801C2F34[i].state == -1) {
            D_801C2F34[i].state = 0;
            D_801C2F34[i].unk2 = 0;
            D_801C2F34[i].unkA = 0;
            return &D_801C2F34[i];
        }
    }
    return NULL;
}

void TornadoSpriteParticleTick(TornadoParticle* particle) {
    SVECTOR* vec = (SVECTOR*)getScratchAddr(0);
    SVECTOR* rot = (SVECTOR*)getScratchAddr(2);
    MATRIX* matrix = (MATRIX*)getScratchAddr(4);
    MATRIX* view;
    s32 i;
    s32 count;
    s32 slot;
    s32 level;
    s32 decay;
    s32 prevUnk4;

    if (D_80062D98 == 0) {
        for (i = 0; i < 3; i++) {
            particle->unk8 += 0x25000 / particle->unk4;
            particle->unk6 -= 0x3A98 / (particle->unk4 - 300);
            prevUnk4 = particle->unk4;
            decay = 100; // match: the 100 must sit in a local for the unk4 step to compile
            particle->unk4 = particle->unk6 >> 3 >= -0x6D ? prevUnk4 - decay - (particle->unk6 >> 4) : prevUnk4 + 10;
            if (particle->unk4 < 0x190) {
                particle->unk4 = 0x190;
            }
            vec->vx = particle->unk4;
            vec->vy = particle->unk6;
            vec->vz = 0;
            rot->vx = 0;
            rot->vz = 0;
            rot->vy = particle->unk8;
            RotMatrixYXZ(rot, matrix);
            ApplyMatrixSV(matrix, vec, vec);
            particle->trail[particle->unkA].vx = vec->vx + D_801C2F28.vx;
            particle->trail[particle->unkA].vy = vec->vy;
            particle->trail[particle->unkA].vz = vec->vz + D_801C2F28.vz;
            particle->unkA = (particle->unkA + 1) & 7;
        }
    }
    count = particle->unk2;
    if (count > 5) {
        count = 5;
    }
    slot = (particle->unkA - count) & 7;
    for (i = 1; i < count; i++) {
        view = BattleSetBillboardMatrix(&particle->trail[slot], 0x1000, 0);
        if ((u32)(view->t[2] - 1) < 0x752F) {
            level = ((i << 7) / 5) * D_801C2F30 >> 12;
            D_801C2EF0.r = D_801C2EF0.g = D_801C2EF0.b = level;
            D_801E6A94 = BattleEffectSpriteAdd(&D_801C2EF0, g_cDb->unk70, 12, D_801E6A94);
        }
        slot = (slot + 1) & 7;
    }
    if (D_80062D98 == 0) {
        particle->unk2++;
        if (particle->unk2 >= 0x33) {
            particle->state = -1;
        }
    }
}

void TornadoRibbonParticleTick(TornadoParticle* particle) {
    SVECTOR* vec = (SVECTOR*)getScratchAddr(0);
    SVECTOR* rot = (SVECTOR*)getScratchAddr(2);
    SVECTOR* a = (SVECTOR*)getScratchAddr(4);
    SVECTOR* b = (SVECTOR*)getScratchAddr(6);
    SVECTOR* c = (SVECTOR*)getScratchAddr(8);
    SVECTOR* d = (SVECTOR*)getScratchAddr(10);
    MATRIX* matrix = (MATRIX*)getScratchAddr(12);
    POLY_G4* poly;
    u32 col;
    s32 i;
    s32 count;
    s32 slot;
    s32 next;
    s32 level;
    s32 decay;
    s32 prevUnk4;
    s32 p;
    s32 flag;
    s32 otz;

    col = 0x3A000000;
    if (D_80062D98 == 0) {
        for (i = 0; i < 3; i++) {
            particle->unk8 += 0x25000 / particle->unk4;
            particle->unk6 -= 0x3A98 / (particle->unk4 - 300);
            prevUnk4 = particle->unk4;
            decay = 100; // match: the 100 must sit in a local for the unk4 step to compile
            particle->unk4 = particle->unk6 >> 3 >= -0x6D ? prevUnk4 - decay - (particle->unk6 >> 4) : prevUnk4 + 10;
            if (particle->unk4 < 0x190) {
                particle->unk4 = 0x190;
            }
            vec->vx = particle->unk4;
            vec->vy = particle->unk6;
            vec->vz = 0;
            rot->vx = 0;
            rot->vz = 0;
            rot->vy = particle->unk8;
            RotMatrixYXZ(rot, matrix);
            ApplyMatrixSV(matrix, vec, vec);
            particle->trail[particle->unkA].vx = vec->vx + D_801C2F28.vx;
            particle->trail[particle->unkA].vy = vec->vy;
            particle->trail[particle->unkA].vz = vec->vz + D_801C2F28.vz;
            particle->unkA = (particle->unkA + 1) & 7;
        }
    }
    SetRotMatrix(&g_BattleWorldView);
    SetTransMatrix(&g_BattleWorldView);
    count = particle->unk2;
    if (count > 8) {
        count = 8;
    }
    slot = (particle->unkA - count) & 7;
    for (i = 1; i < count; i++) {
        a->vx = b->vx = particle->trail[slot].vx;
        a->vy = particle->trail[slot].vy - 25;
        b->vy = particle->trail[slot].vy + 25;
        a->vz = b->vz = particle->trail[slot].vz;
        next = (slot + 1) & 7;
        c->vx = d->vx = particle->trail[next].vx;
        c->vy = particle->trail[next].vy - 25;
        d->vy = particle->trail[next].vy + 25;
        c->vz = d->vz = particle->trail[next].vz;
        poly = D_801E6A94;
        otz =
            RotAverage4(a, b, c, d, (long*)&poly->x0, (long*)&poly->x1, (long*)&poly->x2, (long*)&poly->x3, &p, &flag);
        if ((flag & 0x60000) == 0) {
            poly->tag = 0x08000000;
            *(u32*)&poly->r1 = col;
            *(u32*)&poly->r0 = col;
            if (i == count - 1) {
                col = 0x3A000000;
            } else {
                level = (D_801C2F30 << 4) >> 12;
                col += (level << 16) | (level << 8) | level;
            }
            *(u32*)&poly->r3 = col;
            *(u32*)&poly->r2 = col;
            AddPrim(&g_cDb->unk70[otz >> 2], poly);
            D_801E6A94 = poly + 1;
        }
        slot = next;
    }
    if (D_80062D98 == 0) {
        particle->unk2++;
        if (particle->unk2 >= 0x33) {
            particle->state = -1;
        }
    }
}

TornadoParticle* TornadoRibbonParticleAlloc(void) {
    s32 i;

    for (i = 0; i < 100; i++) {
        if (D_801C4CE4[i].state == -1) {
            D_801C4CE4[i].state = 0;
            D_801C4CE4[i].unk2 = 0;
            D_801C4CE4[i].unkA = 0;
            return &D_801C4CE4[i];
        }
    }
    return NULL;
}

void TornadoSpriteDriverTick(void) {
    TornadoData* effect;
    s32 active;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    active = 0;
    for (i = 0; i < 100; i++) {
        if (D_801C2F34[i].state != -1) {
            TornadoSpriteParticleTick(&D_801C2F34[i]);
            active++;
        }
    }
    if (effect->StartFrame == 1 && active == 0) {
        effect->StartFrame = -1;
    }
}

void TornadoRibbonDriverTick(void) {
    TornadoData* effect;
    s32 active;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    active = 0;
    for (i = 0; i < 100; i++) {
        if (D_801C4CE4[i].state != -1) {
            TornadoRibbonParticleTick(&D_801C4CE4[i]);
            active++;
        }
    }
    if (effect->StartFrame == 1 && active == 0) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        D_801C2EEC++;
    }
}

void TornadoModelTick(void) {
    TornadoData* effect;
    VECTOR* scale = (VECTOR*)getScratchAddr(8);
    MATRIX* matrix = (MATRIX*)getScratchAddr(0);
    s32 frame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    frame = effect->AnimationFrame;
    if (frame < 15) {
        scale->vy = 0x1500;
        D_801C2F00.color = frame * 128 / 15;
        scale->vx = scale->vz = frame * 0x1500 / 15;
    } else {
        frame -= 15;
        if (frame < 0) {
            D_801C2F00.color = 0x80;
            scale->vy = scale->vx = scale->vz = 0x1500;
        } else if (frame < 30) {
            D_801C2F00.color = -(frame * 128) / 30 + 0x80;
            scale->vy = scale->vx = scale->vz = frame * 0x1000 / 30 + 0x1500;
        } else {
            effect->StartFrame = -1;
            return;
        }
    }
    RotMatrixYXZ(&D_801C2F10, matrix);
    ScaleMatrix(matrix, scale);
    matrix->t[0] = effect->unk8;
    matrix->t[1] = 0;
    matrix->t[2] = effect->unkC;
    CompMatrix(&g_BattleWorldView.m, matrix, matrix);
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    D_801C2F00.model = D_801C11C4;
    D_801E6A94 = func_800D29D4(&D_801C2F00, g_cDb->unk70, 12, D_801E6A94);
    D_801C2F00.model = D_801C1E98;
    D_801E6A94 = func_800D29D4(&D_801C2F00, g_cDb->unk70, 12, D_801E6A94);
    if (D_80062D98 == 0) {
        D_801C2F10.vy += 0x80;
        effect->AnimationFrame++;
    }
}

void TornadoTargetTick(void) {
    TornadoData* effect;
    TornadoParticle* particle;
    s32 step;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 4) {
            D_801C2F18 = &g_BattleEffectSlots[BattleEffectRegister(TornadoSpriteDriverTick)];
            D_801C2F18->StartFrame = 0;
            D_801C2F1C = &g_BattleEffectSlots[BattleEffectRegister(TornadoRibbonDriverTick)];
            D_801C2F1C->StartFrame = 0;
            D_801C2F20 = &g_BattleEffectSlots[BattleEffectRegister(TornadoModelTick)];
            *(SVECTOR*)&D_801C2F20->unk8 = D_801C2F28;
        }
        if (effect->AnimationFrame == 0x22) {
            func_800D5774(effect->unk4);
        }
        if (effect->AnimationFrame == 0) {
            BattleFixedPointRampUpdateInit(0x400, 4);
            D_800F5B70.b = D_800F5B70.g = D_800F5B70.r = 0;
        }
        step = effect->AnimationFrame - 4;
        if (step >= 0) {
            if (step < 30) {
                if (step % 5 == 0) {
                    particle = TornadoRibbonParticleAlloc();
                    if (particle != NULL) {
                        particle->unk4 = rand() % 1000 + 5000;
                        particle->unk6 = 0;
                        particle->unk8 = rand() & 0xFFF;
                    }
                }
                particle = TornadoSpriteParticleAlloc();
                if (particle != NULL) {
                    particle->unk4 = rand() % 1000 + 5000;
                    particle->unk6 = 0;
                    particle->unk8 = rand() & 0xFFF;
                    particle->state = 0;
                }
            } else {
                step = effect->AnimationFrame - 0x22;
                if (step < 15) {
                    D_801C2F30 = -(step << 12) / 15 + 0x1000;
                } else {
                    D_801C2F18->StartFrame = -1;
                    D_801C2F1C->StartFrame = -1;
                }
            }
        }
        if (effect->AnimationFrame == 0x31) {
            effect->StartFrame = -1;
        }
        effect->AnimationFrame++;
    }
}

void TornadoTargetSetup(s32 target, s32 callbackArg) {
    TornadoData* effect;
    s32 i;
    s32 j;

    for (i = 0; i < 100; i++) {
        D_801C2F34[i].state = -1;
    }
    for (j = 0; j < 100; j++) {
        D_801C4CE4[j].state = -1;
    }
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &D_801C2F28);
    D_801C2F28.vy = 0;
    D_801C2F30 = 0x1000;
    effect = &g_BattleEffectSlots[BattleEffectRegister(TornadoTargetTick)];
    effect->unk4 = target;
    effect->unk6 = callbackArg;
    BattleAkaoCommand(0x20, BattlePositionToStereoPan(&D_801C2F28), 0x4E);
}

void TornadoDoubleBufferFlip(void) {
    TornadoData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801E6A94 = D_801C6A94[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        BattleFixedPointRampUpdateInit(0, 4);
        effect->StartFrame = -1;
    }
}

void TornadoStart(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(D_801B1164, 0, 0, 0);
    BattleEffectRegister(TornadoDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 0x5A, TornadoTargetSetup);
}
