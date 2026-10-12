//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "otsunami.h"
#include "../battle/battle.h"
#include <libgte.h>
#include <psxsdk/inline_c.h>

// Effect shared by Leviathan Scales (海龍のウロコ), Big Wave (ビッグウェーブ)
// and Huge Tidal Wave (大津波).

void OtsunamiStart(s32 targetMask, s32 callbackArg);

// Primitive buffer pages, one 0xC000 page per double-buffered frame.
extern u8 D_801C0F8C[2][0xC000];
extern void* D_801D8F8C;
extern SpriteRenderDesc D_801C0F74;
extern SpriteAnim* D_801C0F80[];
extern s16 D_801D8F90[];
extern s16 D_801D9190[][0x60];
extern u_long D_801B0D20[];
extern s16 D_801DAF90;
extern s16 D_801C0F70;
extern s16 D_801C0F88;
extern u16 D_801C0F8A;
extern u8 D_800F5B70;
extern u8 D_800F5B71;
extern u8 D_800F5B72;
void OtsunamiWaveMeshTick(void);
void OtsunamiMainTick(void);

// Only StartFrame/AnimationFrame are common to every magic overlay.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ SVECTOR unkC;
    /* 0x14 */ char pad14[6];
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ char pad1C[4];
} OtsunamiData; // size:0x20

extern OtsunamiData g_BattleEffectSlots[];

void MAGIC_Otsunami(s32 targetMask, s32 callbackArg) { OtsunamiStart(targetMask, callbackArg); }

// Same as BarrierDoubleBufferFlip.
void OtsunamiDoubleBufferFlip(void) {
    OtsunamiData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801D8F8C = D_801C0F8C[effect->AnimationFrame];
    effect->AnimationFrame ^= 1;
    if (g_BattleEffectCount < 2) {
        effect->StartFrame = -1;
    }
}

void OtsunamiWaveSpriteTick(void) {
    OtsunamiData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, (effect->unk1A == 0) ? 0x1800 : 0x2800, 0);
    D_801C0F74.frameIndex = effect->AnimationFrame;
    D_801C0F74.frames = D_801C0F80[effect->unk1A];
    D_801D8F8C = func_800D4D90(&D_801C0F74, g_cDb->unk70, 12, D_801D8F8C);
    if (!D_80062D98) {
        effect->AnimationFrame++;
        if (effect->AnimationFrame == D_801C0F80[effect->unk1A]->frameCount) {
            effect->StartFrame = -1;
        }
    }
}

void OtsunamiUvPairMirror(u8* a, u8* b) {
    if (!*a) {
        *a = 0xFF;
    } else {
        *a = -*a;
    }
    if (*b == 0xFF) {
        *b = 0;
    } else {
        *b = -*b;
    }
}

s16 OtsunamiWaveHeightCalc(s16 angle, s16 index) {
    s32 pad[2]; // stack pad needed to match the 8-byte frame
    s32 w;
    s32 t;

    if ((u16)angle > 0x800) {
        angle = 0;
    }
    w = -D_801D8F90[angle] * 625;
    t = D_801D9190[index][0];
    return ((t * ((w << 6) >> 16)) << 4) >> 16;
}

void OtsunamiWaveMeshTick(void) {
    OtsunamiData* effect;
    POLY_FT4* prim;
    SVECTOR v[4];
    s32 otz;
    u16 clut;
    u16 tpage;
    s16 base;
    s16 i;
    s16 j;
    s16 row;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    base = (u16)effect->AnimationFrame * D_801C0F8A - 1000;
    tpage = GetTPage(0, 1, 0x300, 0);
    clut = GetClut(0x80, 0x1E0);
    prim = D_801D8F8C;
    SetRotMatrix(&g_BattleWorldView);
    SetTransMatrix(&g_BattleWorldView);
    for (j = -4; j < 16; j++) {
        for (i = 0; i < 16; i++) {
            row = j - base / 1000;
            v[0].vx = (i - 8) * 1000;
            v[0].vz = (base + (row - 8) * 1000) * D_801C0F70;
            v[1].vx = (i - 8) * 1000 + 1000;
            v[1].vz = v[0].vz;
            v[2].vx = v[0].vx;
            v[2].vz = (base + (row - 8) * 1000 + 1000) * D_801C0F70;
            v[3].vx = v[1].vx;
            v[3].vz = v[2].vz;
            v[0].vy = OtsunamiWaveHeightCalc(effect->AnimationFrame * 160 - row * 400, i) - 200;
            v[2].vy = OtsunamiWaveHeightCalc(effect->AnimationFrame * 160 - (row + 1) * 400, i) - 200;
            v[1].vy = OtsunamiWaveHeightCalc(effect->AnimationFrame * 160 - row * 400, i + 1) - 200;
            v[3].vy = OtsunamiWaveHeightCalc(effect->AnimationFrame * 160 - (row + 1) * 400, i + 1) - 200;
            gte_ldv3(&v[0], &v[1], &v[2]);
            gte_rtpt();
            gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
            gte_ldv0(&v[3]);
            gte_rtps();
            gte_stsxy(&prim->x3);
            gte_avsz4();
            gte_stotz(&otz);
            if (otz > 0) {
                prim->tag = 0x09000000;
                *(u32*)&prim->r0 = D_801C0F88 * 0x10101 + (D_801C0F88 == 0x80 ? 0x2C000000 : 0x2E000000);
                prim->u0 = i << 6;
                prim->v0 = row << 6;
                prim->u1 = (i + 1) << 6;
                prim->v1 = row << 6;
                prim->u2 = i << 6;
                prim->v2 = (row + 1) << 6;
                prim->u3 = (i + 1) << 6;
                prim->v3 = (row + 1) << 6;
                if (!prim->u1) {
                    prim->u1 = prim->u3 = 0xFF;
                }
                if (!prim->v2) {
                    prim->v2 = prim->v3 = 0xFF;
                }
                if ((i & 7) >= 4) {
                    OtsunamiUvPairMirror(&prim->u0, &prim->u1);
                    OtsunamiUvPairMirror(&prim->u2, &prim->u3);
                }
                if ((row & 7) >= 4) {
                    OtsunamiUvPairMirror(&prim->v0, &prim->v2);
                    OtsunamiUvPairMirror(&prim->v1, &prim->v3);
                }
                prim->tpage = tpage;
                prim->clut = clut;
                addPrim(&g_cDb->unk70[otz >> 2], prim);
                prim++;
            }
        }
    }
    D_801D8F8C = prim;
    if (!D_80062D98) {
        D_801C0F8A -= 3;
        if (effect->AnimationFrame < 4) {
            D_801C0F88 += 0x20;
        }
        if (effect->AnimationFrame >= 0x2B) {
            D_801C0F88 -= 0x10;
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 0x32) {
            effect->StartFrame = -1;
        }
    }
}

void OtsunamiMainTick(void) {
    OtsunamiData* effect;
    OtsunamiData* child;
    s16 i;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (!D_80062D98) {
        if (!effect->AnimationFrame) {
            D_800F5B70 = D_800F5B71 = D_800F5B72 = 0;
            BattleFixedPointRampUpdateInit(0x1000, 4);
        }
        if (effect->AnimationFrame == 20) {
            BattleFixedPointRampUpdateInit(0, 8);
        }
        if (!effect->AnimationFrame) {
            BattleEffectRegister(OtsunamiWaveMeshTick);
        }
        if (effect->AnimationFrame >= 37 && effect->AnimationFrame < 44) {
            for (i = 0; i < 10; i++) {
                if ((D_801DAF90 >> i) & 1) {
                    child = &g_BattleEffectSlots[BattleEffectRegister(OtsunamiWaveSpriteTick)];
                    BattleGetPartPosition(i, g_BattleModels[i].boneIndices[0], &child->Pos);
                    child->Pos.vx += rand() % 600 - 300;
                    child->Pos.vy += rand() % 600 - 300;
                    child->Pos.vz += rand() % 600 - 300;
                    child->unk1A = 1;
                    child = &g_BattleEffectSlots[BattleEffectRegister(OtsunamiWaveSpriteTick)];
                    BattleGetPartPosition(i, g_BattleModels[i].boneIndices[0], &child->Pos);
                    child->Pos.vx += rand() % 1400 - 700;
                    child->Pos.vy += rand() % 1400 - 700;
                    child->Pos.vz += rand() % 1400 - 700;
                    child->unk1A = 0;
                }
            }
        }
        if (effect->AnimationFrame == 40) {
            for (i = 0; i < 10; i++) {
                if ((D_801DAF90 >> i) & 1) {
                    func_800D5774(i);
                }
            }
        }
        effect->AnimationFrame++;
        if (effect->AnimationFrame == 60) {
            effect->StartFrame = -1;
        }
    }
}

void OtsunamiStart(s32 targetMask, s32 callbackArg) {
    OtsunamiData* effect;
    s16 i;

    BattleEffectRegister(OtsunamiDoubleBufferFlip);
    BattleSetLoadTimToVram(D_801B0D20, 0, 0, 0);
    D_801DAF90 = targetMask;
    for (i = 0; i < 0x1000; i++) {
        D_801D8F90[i] = rsin(i);
    }
    effect = &g_BattleEffectSlots[BattleEffectRegister(OtsunamiMainTick)];
    BattleGetPartPosition(callbackArg, g_BattleModels[callbackArg].boneIndices[9], &effect->Pos);
    BattleEntityGetCenter(targetMask, &effect->unkC);
    if (effect->Pos.vz > effect->unkC.vz) {
        D_801C0F70 *= -1;
    }
    func_800BBA84(0x132, callbackArg, 0x40);
}
