//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "whirl.h"
#include "../battle/battle.h"

// Effect shared by Tornade Wave (トルネードウェイブ) and Dragon Dance (龍宮の舞).

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
} WhirlData;

typedef struct {
    /* 0x00 */ s16 state;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ s16 unk4;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ s16 unk8;
    /* 0x0A */ s16 unkA;
    /* 0x0C */ SVECTOR trail[8];
} WhirlParticle; // size:0x4C

extern WhirlData g_BattleEffectSlots[];
extern WhirlParticle D_801C2F20[100];
extern WhirlParticle D_801C4CD0[100];
extern u8 D_801C6A80[2][0x10000];
extern void* D_801E6A80;
extern s32 D_801C2ED8;
extern u_long D_801B1150[];
extern SVECTOR D_801C2F14;
extern WhirlData* D_801C2F04;
extern WhirlData* D_801C2F08;
extern WhirlData* D_801C2F0C;
extern CVECTOR D_800F5B70;
extern ModelRenderDesc D_801C2EEC;
extern SVECTOR D_801C2EFC;
extern s32 D_801C11B0[];
extern s32 D_801C1E84[];
extern s32 D_801C2F1C;
extern BattleSpriteDesc D_801C2EDC;

void WhirlStart(s32 targetMask, s32 callbackArg);
void WhirlSpriteParticleTick(WhirlParticle* particle);
void WhirlRibbonParticleTick(WhirlParticle* particle);
void WhirlTargetTick(void);
void WhirlTargetSetup(s32 target, s32 callbackArg);
WhirlParticle* WhirlRibbonParticleAlloc(void);
WhirlParticle* WhirlSpriteParticleAlloc(void);
void WhirlDoubleBufferFlip(void);

void MAGIC_Whirl(s32 targetMask, s32 callbackArg) { WhirlStart(targetMask, callbackArg); }

WhirlParticle* WhirlSpriteParticleAlloc(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 100; i++) {
        if (D_801C2F20[i].state == -1) {
            D_801C2F20[i].state = 0;
            D_801C2F20[i].unk2 = 0;
            D_801C2F20[i].unkA = 0;
            return &D_801C2F20[i];
        }
    }
    return NULL;
}

void WhirlSpriteParticleTick(WhirlParticle* particle) {
    SVECTOR* vec = (SVECTOR*)0x1F800000;
    SVECTOR* rot = (SVECTOR*)0x1F800008;
    MATRIX* matrix = (MATRIX*)0x1F800010;
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
            particle->trail[particle->unkA].vx = vec->vx + D_801C2F14.vx;
            particle->trail[particle->unkA].vy = vec->vy;
            particle->trail[particle->unkA].vz = vec->vz + D_801C2F14.vz;
            particle->unkA = (particle->unkA + 1) & 7;
        }
    }
    count = particle->unk2;
    if (count > 5) {
        count = 5;
    }
    slot = (particle->unkA - count) & 7;
    for (i = 1; i < count; i++) {
        BattleSetBillboardMatrix(&particle->trail[slot], 0x1000, 0);
        level = ((i << 7) / 5) * D_801C2F1C >> 12;
        D_801C2EDC.r = D_801C2EDC.g = D_801C2EDC.b = level;
        D_801E6A80 = BattleEffectSpriteAdd(&D_801C2EDC, g_cDb->unk70, 12, D_801E6A80);
        slot = (slot + 1) & 7;
    }
    if (D_80062D98 == 0) {
        particle->unk2++;
        if (particle->unk2 >= 0x33) {
            particle->state = -1;
        }
    }
}

void WhirlRibbonParticleTick(WhirlParticle* particle) {
    u32 col = 0x3A000000;
    SVECTOR* vec = (SVECTOR*)0x1F800000;
    SVECTOR* rot = (SVECTOR*)0x1F800008;
    SVECTOR* a = (SVECTOR*)0x1F800010;
    SVECTOR* b = (SVECTOR*)0x1F800018;
    SVECTOR* c = (SVECTOR*)0x1F800020;
    SVECTOR* d = (SVECTOR*)0x1F800028;
    MATRIX* matrix = (MATRIX*)0x1F800030;
    POLY_G4* poly;
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
            particle->trail[particle->unkA].vx = vec->vx + D_801C2F14.vx;
            particle->trail[particle->unkA].vy = vec->vy;
            particle->trail[particle->unkA].vz = vec->vz + D_801C2F14.vz;
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
        poly = D_801E6A80;
        otz =
            RotAverage4(a, b, c, d, (long*)&poly->x0, (long*)&poly->x1, (long*)&poly->x2, (long*)&poly->x3, &p, &flag);
        if ((flag & 0x60000) == 0) {
            poly->tag = 0x08000000;
            *(u32*)&poly->r1 = col;
            *(u32*)&poly->r0 = col;
            if (i == count - 1) {
                col = 0x3A000000;
            } else {
                level = (D_801C2F1C << 4) >> 12;
                col += (level << 16) | (level << 8) | level;
            }
            *(u32*)&poly->r3 = col;
            *(u32*)&poly->r2 = col;
            AddPrim(&g_cDb->unk70[otz >> 2], poly);
            D_801E6A80 = poly + 1;
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

WhirlParticle* WhirlRibbonParticleAlloc(void) {
    s32 i;

    for (i = 0; i < 100; i++) {
        if (D_801C4CD0[i].state == -1) {
            D_801C4CD0[i].state = 0;
            D_801C4CD0[i].unk2 = 0;
            D_801C4CD0[i].unkA = 0;
            return &D_801C4CD0[i];
        }
    }
    return NULL;
}

void WhirlSpriteDriverTick(void) {
    WhirlData* effect;
    s32 active;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    active = 0;
    for (i = 0; i < 100; i++) {
        if (D_801C2F20[i].state != -1) {
            WhirlSpriteParticleTick(&D_801C2F20[i]);
            active++;
        }
    }
    if (effect->StartFrame == 1 && active == 0) {
        effect->StartFrame = -1;
    }
}

void WhirlRibbonDriverTick(void) {
    WhirlData* effect;
    s32 active;
    s32 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    active = 0;
    for (i = 0; i < 100; i++) {
        if (D_801C4CD0[i].state != -1) {
            WhirlRibbonParticleTick(&D_801C4CD0[i]);
            active++;
        }
    }
    if (effect->StartFrame == 1 && active == 0) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        D_801C2ED8++;
    }
}

void WhirlModelTick(void) {
    WhirlData* effect;
    VECTOR* scale = (VECTOR*)0x1F800020;
    MATRIX* matrix = (MATRIX*)0x1F800000;
    s32 frame;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    frame = effect->AnimationFrame;
    if (frame < 15) {
        scale->vy = 0x1500;
        D_801C2EEC.color = frame * 128 / 15;
        scale->vx = scale->vz = frame * 0x1500 / 15;
    } else {
        frame -= 15;
        if (frame < 0) {
            D_801C2EEC.color = 0x80;
            scale->vy = scale->vx = scale->vz = 0x1500;
        } else if (frame < 30) {
            D_801C2EEC.color = -(frame * 128) / 30 + 0x80;
            scale->vy = scale->vx = scale->vz = frame * 0x1000 / 30 + 0x1500;
        } else {
            effect->StartFrame = -1;
            return;
        }
    }
    RotMatrixYXZ(&D_801C2EFC, matrix);
    ScaleMatrix(matrix, scale);
    matrix->t[0] = effect->unk8;
    matrix->t[1] = 0;
    matrix->t[2] = effect->unkC;
    CompMatrix(&g_BattleWorldView.m, matrix, matrix);
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    D_801C2EEC.model = D_801C11B0;
    D_801E6A80 = func_800D29D4(&D_801C2EEC, g_cDb->unk70, 12, D_801E6A80);
    D_801C2EEC.model = D_801C1E84;
    D_801E6A80 = func_800D29D4(&D_801C2EEC, g_cDb->unk70, 12, D_801E6A80);
    if (D_80062D98 == 0) {
        D_801C2EFC.vy += 0x80;
        effect->AnimationFrame++;
    }
}

void WhirlTargetTick(void) {
    WhirlData* effect;
    WhirlParticle* particle;
    s32 step;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 4) {
            D_801C2F04 = &g_BattleEffectSlots[BattleEffectRegister(WhirlSpriteDriverTick)];
            D_801C2F04->StartFrame = 0;
            D_801C2F08 = &g_BattleEffectSlots[BattleEffectRegister(WhirlRibbonDriverTick)];
            D_801C2F08->StartFrame = 0;
            D_801C2F0C = &g_BattleEffectSlots[BattleEffectRegister(WhirlModelTick)];
            *(SVECTOR*)&D_801C2F0C->unk8 = D_801C2F14;
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
                    particle = WhirlRibbonParticleAlloc();
                    if (particle != NULL) {
                        particle->unk4 = rand() % 1000 + 5000;
                        particle->unk6 = 0;
                        particle->unk8 = rand() & 0xFFF;
                    }
                }
                particle = WhirlSpriteParticleAlloc();
                if (particle != NULL) {
                    particle->unk4 = rand() % 1000 + 5000;
                    particle->unk6 = 0;
                    particle->unk8 = rand() & 0xFFF;
                    particle->state = 0;
                }
            } else {
                step = effect->AnimationFrame - 0x22;
                if (step < 15) {
                    D_801C2F1C = -(step << 12) / 15 + 0x1000;
                } else {
                    D_801C2F04->StartFrame = -1;
                    D_801C2F08->StartFrame = -1;
                }
            }
        }
        if (effect->AnimationFrame == 0x31) {
            effect->StartFrame = -1;
        }
        effect->AnimationFrame++;
    }
}

void WhirlTargetSetup(s32 target, s32 callbackArg) {
    WhirlData* effect;
    s32 i;
    s32 j;

    for (i = 0; i < 100; i++) {
        D_801C2F20[i].state = -1;
    }
    for (j = 0; j < 100; j++) {
        D_801C4CD0[j].state = -1;
    }
    BattleGetPartPosition(target, g_BattleModels[target].boneIndices[0], &D_801C2F14);
    D_801C2F14.vy = 0;
    D_801C2F1C = 0x1000;
    effect = &g_BattleEffectSlots[BattleEffectRegister(WhirlTargetTick)];
    effect->unk4 = target;
    effect->unk6 = callbackArg;
    BattleAkaoCommand(0x20, BattlePositionToStereoPan(&D_801C2F14), 0x4E);
}

void WhirlDoubleBufferFlip(void) {
    WhirlData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801E6A80 = D_801C6A80[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        BattleFixedPointRampUpdateInit(0, 4);
        effect->StartFrame = -1;
    }
}

void WhirlStart(s32 targetMask, s32 callbackArg) {
    BattleSetLoadTimToVram(D_801B1150, 0, 0, 0);
    BattleEffectRegister(WhirlDoubleBufferFlip);
    MagicAnimationRegister(targetMask, callbackArg, 0x5A, WhirlTargetSetup);
}
