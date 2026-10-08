//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"
#include "../magic/magic.h"
#include <libc.h>
#include <libetc.h>

void func_800D751C();
void func_800D7888();
void func_800D7368();
void BattleSubModelFlashTick();
void func_800D6D8C();
void func_800D6F78();
void func_800D5D28();
void BattleHitFlashGrowTick();
void BattleHitFlashShrinkTick();
void BattleTriggerActorFlashMode0(s32 arg0);
void BattleTriggerActorFlashMode1(s32 arg0);
void BattleTriggerActorFlashMode2(s32 arg0);
void BattleSpawnActorRampEffect(s32 arg0, s16 arg1, s16 arg2);
void BattleSpawnPartEffect(s32 actor, s32 hitFlashType);
s32 BattleDetachedRegister(void (*f)());
s32 BattleModelReadAnimStream(BattleModelSub* arg0, s32 arg1, s16 nItems, u8* arg3);
void func_800D3AF0();
void func_800D4710();
MATRIX* BattleSetMatrixPosition(SVECTOR* pos, s32 depthBias, MATRIX* m);
void BattleSpawnFloatingIcon(s32 actor, s32 arg1);

s32 BattleModelAnimReadDynamicFrameOffsBits(u8* arg0, s32* arg1) {
    s32 pos;
    s32 tmp;
    u8* p;
    s32 bit;
    s32 window;
    s32 mask;

    pos = *arg1;
    tmp = pos;
    if (pos < 0) {
        tmp = pos + 7;
    }
    p = arg0 + (tmp >> 3);
    bit = pos & 7;
    window = (p[0] << 8) | p[1];
    mask = 1 << (0xF - bit);
    if ((window & mask) == 0) {
        *arg1 = pos + 8;
        return (s32)(window << (bit + 1) << 16) >> 0x19;
    } else {
        window = (window << 8) | p[2];
        *arg1 = pos + 0x11;
        return (s32)(window << (bit + 1) << 8) >> 0x10;
    }
}

static s32 BattleModelAnimReadBitStream(u8* arg0, s32* arg1, s32 arg2) {
    s32 bits;
    s32 i;

    bits = 0;
    for (i = 0; i < arg2; i++) {
        bits <<= 1;
        if ((arg0[*arg1 / 8] >> (7 - (*arg1 & 7))) & 1) {
            bits++;
        }
        *arg1 = *arg1 + 1;
    }
    bits <<= 32 - arg2;
    bits >>= 32 - arg2;
    return bits;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", BattleModelAnimReadEncryptedRotBits);

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", BattleModelReadAnimStream);

void BattleGetPartPosition(s32 actor, s32 bone, SVECTOR* pos) {
    MATRIX inverse;

    pos->vx = g_BattleModels[actor].boneTransforms[bone].m.t[0] - g_BattleWorldView.m.t[0];
    pos->vy = g_BattleModels[actor].boneTransforms[bone].m.t[1] - g_BattleWorldView.m.t[1];
    pos->vz = g_BattleModels[actor].boneTransforms[bone].m.t[2] - g_BattleWorldView.m.t[2];
    TransposeMatrix(&g_BattleWorldView.m, &inverse);
    ApplyMatrixSV(&inverse, pos, pos);
}

// Take the low 16 bits of each of m's translation components relative to the
// camera g_BattleWorldView, then rotate that offset by the camera's transposed
// orientation into pos.
static void BattleGetMatrixPosition(MATRIX* m, SVECTOR* pos) {
    MATRIX inverse;

    pos->vx = m->t[0] - g_BattleWorldView.m.t[0];
    pos->vy = m->t[1] - g_BattleWorldView.m.t[1];
    pos->vz = m->t[2] - g_BattleWorldView.m.t[2];
    TransposeMatrix(&g_BattleWorldView.m, &inverse);
    ApplyMatrixSV(&inverse, pos, pos);
}

void func_800D3AF0(void) {
    BattleSparkleSlot* slot;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].sparkle;
    D_800F01E8.u = slot->frame * 32;
    D_800F01F8.m[0][0] = slot->scaleX;
    D_800F01F8.m[1][1] = slot->scaleY;
    BattleSetMatrixPosition(&slot->pos, -slot->scaleX >> 4, &D_800F01F8);
    SetRotMatrix(&D_800F01F8);
    SetTransMatrix(&D_800F01F8);
    D_80163C74 = BattleEffectSpriteAdd(&D_800F01E8, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (slot->frame++ >= 7) {
            slot->unk0 = -1;
        }
    }
}

const MATRIX D_800A0D98 = {{{0, 0, 0}, {0, 0, 0}, {0, 0, 4096}}, {0, 0, 0}};
extern BattleSpriteDesc D_800F0218;

void BattleEffectSingleDustCloud(void) {
    MATRIX m = D_800A0D98;
    s32 flag;
    Unk801621F0* slot;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].raw;
    D_800F0218.u = slot->D_801621F2 * 32;
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans((SVECTOR*)&slot->D_801621F4, (VECTOR*)m.t, &flag);
    m.t[2] -= (s16)slot->unk10.unk.unk0 >> 4;
    m.m[0][0] = slot->unkE + ((slot->unkE * slot->D_801621F2) >> 3);
    m.m[1][1] = (s16)slot->unk10.unk.unk0 + (((s16)slot->unk10.unk.unk0 * slot->D_801621F2) >> 3);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    D_80163C74 = BattleEffectSpriteAdd(&D_800F0218, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (slot->D_801621F2++ >= 7) {
            slot->D_801621F0 = -1;
        }
    }
}

static void BattleEffectDustClouds(void) {
    Unk801621F0* temp_s0_2;
    Unk801621F0* temp_s1;
    s32 temp_s0;
    u16 temp_s2;

    temp_s1 = &g_BattleDetachedSlots[g_BattleDetachedCursor].raw;
    temp_s0 = temp_s1->D_801621F0;
    temp_s2 = (&g_BattleModels[temp_s0].boneIndices[11])[temp_s1->D_801621F2 & 1];
    temp_s0++; // !FAKE
    temp_s0--; // !FAKE
    if (temp_s2 != 0xFF) {
        temp_s0_2 = &g_BattleDetachedSlots[BattleDetachedRegister(BattleEffectSingleDustCloud)].raw;
        BattleGetPartPosition(temp_s0, temp_s2, (SVECTOR*)&temp_s0_2->D_801621F4);
        temp_s0_2->D_801621F6 = 0;
        temp_s0_2->unkE = temp_s1->unkE;
        temp_s0_2->unk10.unk.unk0 = temp_s1->unk10.unk.unk0;
    }
    temp_s1->D_801621F2++;
    if (temp_s1->D_801621F2 == 4) {
        temp_s1->D_801621F0 = -1;
    }
}

void BattleSpawnPartFlickerEffect(s32 arg0) {
    Unk801621F0* temp_v0;

    temp_v0 = &g_BattleDetachedSlots[BattleDetachedRegister(BattleEffectDustClouds)].raw;
    temp_v0->D_801621F0 = arg0;
    temp_v0->unkE = *(s16*)& temp_v0->unk10 = g_BattleModels[arg0].scale;
}

void BattleSpawnSparkleEffect(SVECTOR* pos, s16 scaleX, s16 scaleY) {
    BattleSparkleSlot* dst;

    dst = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D3AF0)].sparkle;
    dst->pos = *pos;
    dst->scaleX = scaleX;
    dst->scaleY = scaleY;
}

static void BattleDelayedRotatedSpawnTick(void) {
    Unk801621F0* temp_s0;
    Unk801621F0* temp_s1;

    temp_s1 = &g_BattleDetachedSlots[g_BattleDetachedCursor].raw;
    if (D_80062D98 == 0) {
        temp_s1->unkC--;
        if (temp_s1->unkC == -1) {
            temp_s0 = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D3AF0)].raw;
            RotMatrixYXZ(&g_BattleModels[temp_s1->unk10.unk.unk2].rootRot, (MATRIX*)0x1F800008);
            ApplyMatrixSV((MATRIX*)0x1F800008, (SVECTOR*)&temp_s1->D_801621F4, (SVECTOR*)0x1F800000);
            temp_s0->D_801621F4 = g_BattleModels[temp_s1->unk10.unk.unk2].rootTrans.vx + ((SVECTOR*)0x1F800000)->vx;
            temp_s0->D_801621F6 = g_BattleModels[temp_s1->unk10.unk.unk2].rootTrans.vy + ((SVECTOR*)0x1F800000)->vy;
            temp_s0->unk8 = g_BattleModels[temp_s1->unk10.unk.unk2].rootTrans.vz + ((SVECTOR*)0x1F800000)->vz;
            temp_s0->unkE = temp_s1->unkE;
            temp_s0->unk10.unk.unk0 = temp_s1->unk10.unk.unk0;
            temp_s1->D_801621F0 = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D415C);

static void BattleComputeRelativeMatrix(MATRIX* arg0, MATRIX* arg1, MATRIX* arg2) {
    arg2->t[0] = arg1->t[0] - arg0->t[0];
    arg2->t[1] = arg1->t[1] - arg0->t[1];
    arg2->t[2] = arg1->t[2] - arg0->t[2];
    TransposeMatrix(arg0, arg2);
    ApplyMatrixLV(arg2, (VECTOR*)arg2->t, (VECTOR*)arg2->t);
    MulMatrix(arg2, arg1);
}

MATRIX* BattleSetMatrixPosition(SVECTOR* pos, s32 depthBias, MATRIX* m) {
    VECTOR normal;
    s32 flag;

    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(pos, (VECTOR*)m->t, &flag);
    if (depthBias != 0) {
        VectorNormal((VECTOR*)m->t, &normal);
        m->t[0] = ((depthBias * normal.vx) >> 12) + m->t[0];
        m->t[1] = ((depthBias * normal.vy) >> 12) + m->t[1];
        m->t[2] = ((depthBias * normal.vz) >> 12) + m->t[2];
    }
    return m;
}

MATRIX* BattleSetBillboardMatrix(SVECTOR* pos, s32 scale, s32 depthBias) {
    VECTOR normal;
    s32 flag;

    g_BattleBillboardMatrix.m[0][0] = g_BattleBillboardMatrix.m[1][1] = g_BattleBillboardMatrix.m[2][2] = scale;
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(pos, (VECTOR*)g_BattleBillboardMatrix.t, &flag);
    if (depthBias != 0) {
        VectorNormal((VECTOR*)g_BattleBillboardMatrix.t, &normal);
        g_BattleBillboardMatrix.t[0] = ((depthBias * normal.vx) >> 12) + g_BattleBillboardMatrix.t[0];
        g_BattleBillboardMatrix.t[1] = ((depthBias * normal.vy) >> 12) + g_BattleBillboardMatrix.t[1];
        g_BattleBillboardMatrix.t[2] = ((depthBias * normal.vz) >> 12) + g_BattleBillboardMatrix.t[2];
    }
    SetRotMatrix(&g_BattleBillboardMatrix);
    SetTransMatrix(&g_BattleBillboardMatrix);
    return &g_BattleBillboardMatrix;
}

static void BattleAddDrawModePrim(u_long* ot, u16 tpage) {
    DR_MODE* dr_mode;

    dr_mode = D_80163C74;
    SetDrawMode(dr_mode, 0, 1, tpage, NULL);
    AddPrim(ot, (void*)dr_mode);
    D_80163C74 = dr_mode + 1;
}

const VECTOR D_800A0DB8 = {0, -4096, 0, 0};
void BattleMatrixFromDirection(SVECTOR* dir, MATRIX* m) {
    VECTOR side;
    VECTOR up = D_800A0DB8;
    VECTOR fwd;

    fwd.vx = dir->vx;
    fwd.vy = dir->vy;
    fwd.vz = dir->vz;
    VectorNormal(&fwd, &fwd);
    side.vx = fwd.vz;
    side.vy = 0;
    side.vz = -fwd.vx;
    VectorNormal(&side, &side);
    OuterProduct12(&fwd, &side, &up);
    VectorNormal(&up, &up);
    m->m[0][0] = side.vx;
    m->m[1][0] = side.vy;
    m->m[2][0] = side.vz;
    m->m[0][1] = up.vx;
    m->m[1][1] = up.vy;
    m->m[2][1] = up.vz;
    m->m[0][2] = fwd.vx;
    m->m[1][2] = fwd.vy;
    m->m[2][2] = fwd.vz;
}

void BattleMatrixOrthonormalize(MATRIX* m) {
    VECTOR side;
    VECTOR up;
    VECTOR fwd;

    up.vx = m->m[0][1];
    up.vy = m->m[1][1];
    up.vz = m->m[2][1];
    VectorNormal(&up, &up);
    m->m[0][1] = up.vx;
    m->m[1][1] = up.vy;
    m->m[2][1] = up.vz;
    side.vx = up.vy;
    side.vy = -up.vx;
    side.vz = 0;
    VectorNormal(&side, &side);
    m->m[0][0] = side.vx;
    m->m[1][0] = side.vy;
    m->m[2][0] = side.vz;
    OuterProduct12(&side, &up, &fwd);
    VectorNormal(&fwd, &fwd);
    m->m[0][2] = fwd.vx;
    m->m[1][2] = fwd.vy;
    m->m[2][2] = fwd.vz;
}

void func_800D4710(void) {
    BattleKeyframeParticleSlot* p;
    SpriteRenderDesc* desc;
    MATRIX* m;
    u8 anim;

    p = &g_BattleDetachedSlots[g_BattleDetachedCursor].keyframeParticle;
    desc = (SpriteRenderDesc*)getScratchAddr(0);
    desc->frameIndex = p->frame | 0x8000;
    desc->clutBias = p->clutBias;
    *(u32*)&desc->color = 0x2C808080;
    D_800F10B8.m[0][0] = D_800F10B8.m[1][1] = D_800F10B8.m[2][2] = p->scale;
    anim = p->flags;
    if (anim != 8) {
        m = &D_800F10B8;
        desc->frames = D_800F0B14[anim];
    } else {
        m = (MATRIX*)getScratchAddr(3);
        desc->frames = D_800F0B14[5];
        *m = D_800F10B8;
        RotMatrixZ(0x200, m);
    }
    if (p->flags & 0x100) {
        m->m[0][0] = -m->m[0][0];
        m->m[0][1] = -m->m[0][1];
        m->m[0][2] = -m->m[0][2];
    }
    BattleSetMatrixPosition(&p->pos, p->depthBias, m);
    m->t[0] += p->offsetX;
    m->t[1] += p->offsetY;
    SetRotMatrix(m);
    SetTransMatrix(m);
    D_80163C74 = func_800D4D90(desc, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (++p->frame >= desc->frames->frameCount) {
            p->flags = -1;
        }
    }
}

void BattleSpawnKeyframeParticle(s8* key, SVECTOR* pos, BattleKeyframeEffectSlot* parent) {
    BattleKeyframeParticleSlot* p;
    s32 scale;

    p = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D4710)].keyframeParticle;
    p->pos = *pos;
    p->flags = (*key++ - 1) | parent->flags;
    if (parent->flags & 0x100) {
        p->offsetX = (-(*key++ << 3) * parent->scale) >> 12;
    } else {
        p->offsetX = ((*key++ << 3) * parent->scale) >> 12;
    }
    p->offsetY = ((*key++ << 3) * parent->scale) >> 12;
    p->depthBias = parent->depthBias;
    scale = ((*key++ << 8) * parent->scale) >> 12;
    if (scale > 0x7FFF) {
        scale = 0x7FFF;
    }
    p->scale = scale;
    p->clutBias = *key << 6;
}

void BattleKeyframeEffectTick(void) {
    BattleKeyframeEffectSlot* slot;
    s8* key;
    s8* sub;
    s32 alive;
    s8 frame;
    s8 subFrame;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].keyframeEffect;
    if (D_80062D98 == 0) {
        alive = 0;
        key = slot->script;
        while ((frame = *key++) != -1) {
            if (*key != -2) {
                if (frame == slot->frame) {
                    BattleSpawnKeyframeParticle(key, &slot->pos, slot);
                } else if (slot->frame < frame) {
                    alive = 1;
                }
                key += 5;
            } else {
                sub = D_800F0C44[key[1]];
                key += 2;
                while ((subFrame = *sub++) != -1) {
                    if (frame + subFrame == slot->frame) {
                        BattleSpawnKeyframeParticle(sub, &slot->pos, slot);
                    } else if (slot->frame < frame + subFrame) {
                        alive = 1;
                    }
                    sub += 5;
                }
            }
        }
        if (alive == 0) {
            slot->flags = -1;
        }
        slot->frame++;
    }
}

static void BattleSpawnFloatingIconAt(void* arg0, s32 arg1, s32 arg2);
void func_800D4C08(SVECTOR* pos, s32 scriptAndFlags, s32 scale, s32 depthBias) {
    BattleKeyframeEffectSlot* slot;

    slot = &g_BattleDetachedSlots[BattleDetachedRegister(BattleKeyframeEffectTick)].keyframeEffect;
    slot->flags = scriptAndFlags & 0xFF00;
    slot->script = D_800F0F98[scriptAndFlags & 0xFF];
    slot->pos = *pos;
    slot->scale = scale;
    slot->depthBias = depthBias;
}

static void BattleSpawnFloatingIconAtPart(s32 actor, s32 arg1, s32 arg2) {
    SVECTOR pos;

    BattleGetPartPosition(actor, g_BattleModels[actor].boneIndices[0], &pos);
    func_800D4C08(&pos, arg1, arg2, -g_BattleModels[actor].collisionRadius);
}

void BattleSpawnFloatingIcon(s32 actor, s32 arg1) { BattleSpawnFloatingIconAtPart(actor, arg1, 0x1000); }

static void BattleSpawnFloatingIconAt(void* arg0, s32 arg1, s32 arg2) { func_800D4C08(arg0, arg1, 0x1000, arg2); }

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D4D90);

extern s32 D_800F10D8;
extern s32 D_800F4CEC[16];
extern s16 D_800F4D2C[16][10];

// Enqueue a value into the 16-entry circular battle queue and return
// the associated data slot for the newly queued entry.
s16* BattleEventQueuePush(s32 arg0) {
    s32 idx = D_800F10D8;
    s32 next = (idx + 1) & 0xF;

    D_800F4CEC[idx] = arg0;
    D_800F10D8 = next;
    return D_800F4D2C[idx];
}

s32 BattleEventQueuePop(s16** arg0) {
    s32 ret;

    if (D_800F10D8 == D_800F10DC) {
        *arg0 = D_800F4D2C[(D_800F10D8 - 1) & 0xF];
        return 0;
    } else {
        ret = D_800F4CEC[D_800F10DC];
        *arg0 = D_800F4D2C[D_800F10DC];
        D_800F10DC = (D_800F10DC + 1) & 0xF;
        return ret;
    }
}
extern Unk80162978* D_800F10E0;

void BattleFixedPointRampTick();
void BattleFixedPointRampTick(void) {
    Unk80162978* slot = &g_BattleEffectSlots[g_BattleEffectCursor].raw;
    s32 v;
    u8 c;

    if (D_80062D98 == 0) {
        if (*(s32*)&slot->unkC != 0) {
            v = *(s32*)&slot->D_8016297C + *(s32*)&slot->unkC;
            *(s32*)&slot->D_8016297C = v;
            if (v <= 0) {
                slot->D_80162978 = -1;
                D_800F10E0 = NULL;
                return;
            }
            if (v > 0xFFFF) {
                *(s32*)&slot->D_8016297C = 0xFFFF;
                *(s32*)&slot->unkC = 0;
            }
        }
    }
    c = ((u8*)&slot->D_8016297C)[1];
    D_80163C74 = (DR_MODE*)func_800C4FC8(c, c, c);
}

// Reset the fixed-point ramp: zero the accumulator (0x04) and seed the
// countdown (0x0C) so it lasts arg0 ticks.
static void BattleFixedPointRampInit(s32 arg0) {
    if (D_800F10E0 == NULL) {
        D_800F10E0 = &g_BattleEffectSlots[BattleEffectRegister(BattleFixedPointRampTick)].raw;
    }
    *(s32*)&D_800F10E0->D_8016297C = 0;
    *(s32*)&D_800F10E0->unkC = 0x10000 / arg0;
}

void BattleFixedPointRampReconfigure(s32 arg0);
void BattleFixedPointRampReconfigure(s32 arg0) {
    if (D_800F10E0 != NULL) {
        *(s32*)&D_800F10E0->unkC = -*(s32*)&D_800F10E0->D_8016297C / arg0;
    }
}

extern s32 D_800F10E4;

// Step the ramp once: accumulate (0x04 += 0x08), publish the high word, and
// free the slot when the countdown (0x0C) reaches 0.
static void BattleFixedPointRampUpdate(void) {
    Unk80162978* slot = &g_BattleEffectSlots[g_BattleEffectCursor].raw;
    s32 v0;
    s32 v1;

    if (D_80062D98 == 0) {
        v0 = *(s32*)&slot->D_8016297C + *(s32*)&slot->D_80162980;
        *(s32*)&slot->D_8016297C = v0;
        D_800F5B74 = v0 >> 0x10;
        v1 = *(s32*)&slot->unkC - 1;
        *(s32*)&slot->unkC = v1;
        if (v1 == 0) {
            D_800F10E4 = 0;
            slot->D_80162978 = -1;
        }
    }
}

void BattleFixedPointRampUpdateInit(s32 arg0, s32 arg1) {
    Unk80162978* slot;
    s32 accum;

    if (D_800F10E4 == 0) {
        slot = &g_BattleEffectSlots[BattleEffectRegister(BattleFixedPointRampUpdate)].raw;
        accum = D_800F5B74 << 0x10;
        D_800F10E4 = (s32)slot;
        *(s32*)&slot->unkC = arg1;
        *(s32*)&slot->D_8016297C = accum;
        *(s32*)&slot->D_80162980 = ((arg0 << 0x10) - accum) / arg1;
    }
}

// Fan one magic animation out over its target mask. Each activation scans to
// the next target in the mask, fires the callback, and retires the slot once
// the mask is exhausted; FrameStep decides how often that happens.
void BattleAnimationUpdate(void) {
    MagicAnimationData* slot = &g_BattleEffectSlots[g_BattleEffectCursor].magicAnimation;
    s16 target;

    if (D_80062D98 != 0) { // global pause
        return;
    }
    if (slot->FrameCounter == 0) {
        do {
            target = slot->TargetCursor;
            while (((slot->TargetMask >> target) & 1) == 0) {
                target = target + 1;
                slot->TargetCursor = target;
            }
            slot->Callback(slot->TargetCursor, slot->CallbackArg);
            // These two fields are read back with lhu here and lh everywhere
            // else, so the u16 casts have to stay.
            slot->TargetCursor = (u16)slot->TargetCursor + 1;
            // No bit left at or above the cursor, so every target is done.
            if (slot->TargetMask < (1 << slot->TargetCursor)) {
                slot->TargetCursor = -1;
                return;
            }
        } while (slot->FrameStep == 0); // 0 fans out to every target at once
    }
    slot->FrameCounter = (u16)slot->FrameCounter + 1;
    if (slot->FrameCounter >= slot->FrameStep) {
        slot->FrameCounter = 0;
    }
}

// TODO: signature is a best guess. Certain: two args are passed, the target
// index and arg1 (offset 0x06). Guessed: the types -- s16 and s32
// compile identically, no overlay yet reads arg1, and editing this
// leaves every object byte-identical, so the build cannot check it.
void MagicAnimationRegister(s32 targetMask, s32 callbackArg, s32 frameStep, void (*func)(s32, s32)) {
    MagicAnimationData* temp_v0 = &g_BattleEffectSlots[BattleEffectRegister(BattleAnimationUpdate)].magicAnimation;
    temp_v0->TargetCursor = 0;
    temp_v0->TargetMask = targetMask;
    temp_v0->CallbackArg = callbackArg;
    temp_v0->FrameStep = frameStep;
    temp_v0->Callback = func;
}

static s32 BattleCountSetBits(s32 arg0) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 10; i++) {
        if ((arg0 >> i) & 1) {
            count++;
        }
    }
    return count;
}

SVECTOR* BattleEntityGetCenter(s32 targetMask, SVECTOR* center) {
    s32 minX = 32767;
    s32 minZ = 32767;
    s32 maxX = -32768;
    s32 maxZ = -32768;
    s32 i;
    BattleModelSub* root;

    for (i = 0; i < NUM_BATTLE_ACTOR; i++) {
        root = (BattleModelSub*)&g_BattleModels[i].stageMatrix;
        if ((targetMask >> i) & 1) {
            if (root->trans.vx < minX) {
                minX = root->trans.vx;
            }
            if (root->trans.vx > maxX) {
                maxX = root->trans.vx;
            }
            if (root->trans.vz < minZ) {
                minZ = root->trans.vz;
            }
            if (root->trans.vz > maxZ) {
                maxZ = root->trans.vz;
            }
        }
    }
    center->vx = (minX + maxX) / 2;
    center->vz = (minZ + maxZ) / 2;
    center->vy = 0;
    return center;
}

s32 func_800D55A4(s32 arg0) {
    return (g_BattleModels[arg0].collisionRadius * 0x10) * g_BattleModels[arg0].scale >> 0xC;
}

// Generic AKAO sound-command dispatcher: the first vararg's low 16 bits are
// the command id, which selects how many trailing u32 params get copied into
// the g_AkaoCmd parameter array before calling AkaoExec.
void BattleAkaoCommand(s32 cmdId, ...) {
    void** args = (void**)&cmdId;
    u32* dst = (u32*)cmdId;
    u32* src;
    s32 cmd = *(u16*)args;
    s32 count;
    s32 nExtra;

    g_AkaoCmd.opcode = cmd;
    switch (cmd & 0xFFFF) {
    case AKAO_PLAY_TWO_SOUNDS:
        nExtra = 3;
        break;
    case AKAO_PLAY_THREE_SOUNDS:
        nExtra = 4;
        break;
    case AKAO_PLAY_FOUR_SOUNDS:
        nExtra = 5;
        break;
    default:
        nExtra = 2;
        break;
    }
    count = 1;
    if (count <= nExtra) {
        dst = (u32*)g_AkaoCmd.params;
        src = (u32*)args + 1;
        for (; count <= nExtra; count++) {
            *dst++ = *src++;
        }
    }
    AkaoExec();
}

// Project a point through the current view matrix and convert its clamped
// on-screen X (0..319) into a 0..127 stereo pan value.
s32 BattlePositionToStereoPan(SVECTOR* sv) {
    s16 sxy[2];
    s32 p;
    s32 flag;

    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTransPers(sv, (long*)sxy, (long*)&p, (long*)&flag);
    if (sxy[0] < 0) {
        sxy[0] = 0;
    } else if (sxy[0] >= 0x140) {
        sxy[0] = 0x13F;
    }
    return (sxy[0] * 128) / 320;
}

s32 BattleEntityGetStereoPan(s32 arg0) {
    SVECTOR sv;

    BattleEntityGetCenter(arg0, &sv);
    return BattlePositionToStereoPan(&sv);
}

// Queue a popup carrying bit index arg0, using push type 6 if that bit is
// set in the D_800F836C flag word, else type 4.
void func_800D5774(u32 arg0) {
    s32 cond;
    s16* ptr;

    cond = (D_800F836C >> arg0) & 1;
    if (cond) {
        ptr = BattleEventQueuePush(6);
    } else {
        ptr = BattleEventQueuePush(4);
    }
    *ptr = arg0;
}

void func_800D57C0();
INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D57C0);

void BattleSpawnActorRampEffect(s32 arg0, s16 arg1, s16 arg2) {
    Unk80162978* temp_v0 = &g_BattleEffectSlots[BattleEffectRegister(func_800D57C0)].raw;
    temp_v0->D_80162978 = 0;
    temp_v0->D_80162980 = arg0;
    temp_v0->D_8016297E = arg2;
    temp_v0->D_8016297C = arg1;
}

void func_800D5938();
INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D5938);

void BattleSpawnGlobalRampEffect(s16 arg0, s16 arg1) {
    Unk80162978* temp_v0;

    temp_v0 = &g_BattleEffectSlots[BattleEffectRegister(func_800D5938)].raw;
    temp_v0->D_80162978 = 0;
    temp_v0->D_8016297E = arg1;
    temp_v0->D_8016297C = arg0;
}

// Divide each byte lane of a packed color independently by a divisor,
// yielding a per-channel step (e.g. a color-fade increment).
static s32 BattleDivideColorChannels(s32 arg0, s32 arg1) {
    return (((arg0 & 0xFF0000) / arg1) & 0xFF0000) | (((arg0 & 0xFF00) / arg1) & 0xFF00) |
           (((arg0 & 0xFF) / arg1) & 0xFF);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D5B6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D5D28);

void BattleSpawnBlinkEffect(s32 arg0, s16 arg1, u32 arg2, s32 arg3, s32 arg4) {
    Unk801621F0* dst = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D5D28)].raw;

    dst->D_801621F0 = (s16)(arg2 >> 24);
    *(s32*)&dst->D_801621F4 = arg0;
    ((s16*)&dst->unk14)[0] = -arg1;
    *(s32*)&dst->unk8 = 0;
    *(s32*)&dst->unkC = (arg2 & 0xFFFFFF) | 0x3A000000;
    dst->unk10.ptr = (u8*)BattleDivideColorChannels(arg2, arg3);
    ((s16*)&dst->unk14)[1] = (s16)arg4;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", BattleEffectSpriteAdd);

extern ModelRenderDesc D_800F14D0;

// Draw a model 4 times through BattleDrawModel (same request-struct pattern as
// barrier.c's D_801B0C98/D_801B0CB0), toggling the 0x1/0x2 flag bits between
// passes. Those bits mirror on X and Y, so the four passes are the four
// quadrants of a symmetric model built from one quarter.
static void BattleDrawSelectionMarker(s32* arg0, s16 arg1) {
    D_800F14D0.model = arg0;
    D_800F14D0.color = arg1;
    SetFarColor(0, 0, 0);
    PushMatrix();
    D_80163C74 = BattleDrawModel(&D_800F14D0, g_cDb->unk70, 12, D_80163C74);
    PopMatrix();
    PushMatrix();
    D_800F14D0.flags |= MODEL_MIRROR_X;
    D_80163C74 = BattleDrawModel(&D_800F14D0, g_cDb->unk70, 12, D_80163C74);
    PopMatrix();
    PushMatrix();
    D_800F14D0.flags |= MODEL_MIRROR_Y;
    D_80163C74 = BattleDrawModel(&D_800F14D0, g_cDb->unk70, 12, D_80163C74);
    PopMatrix();
    D_800F14D0.flags &= ~MODEL_MIRROR_X;
    D_80163C74 = BattleDrawModel(&D_800F14D0, g_cDb->unk70, 12, D_80163C74);
    D_800F14D0.flags &= ~MODEL_MIRROR_Y;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D650C);

extern u8 D_800F10EC[];
extern u8 D_800F11E8[];
extern u8 D_800F1304[];
u8* const D_800A0DC8[] = {D_800F10EC, D_800F11E8, D_800F1304};
INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D6734);

void func_800D6734(s32, s32);

void BattleTriggerActorFlashMode0(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS; // Barrier
    func_800D6734(arg0, 0);
}

void BattleTriggerActorFlashMode1(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_NO_CULL | MODEL_SEMI_TRANS; // MBarrier
    func_800D6734(arg0, 1);
}

void BattleTriggerActorFlashMode2(s32 arg0) {
    D_800F14D0.flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
    func_800D6734(arg0, 2);
}

void BattleDrawHitFlashModel(MATRIX* m) {
    SetFarColor(0, 0, 0);
    SetRotMatrix(m);
    SetTransMatrix(m);
    D_800F1698.flags &= ~(MODEL_MIRROR_X | MODEL_MIRROR_Z);
    D_80163C74 = BattleDrawModel(&D_800F1698, g_cDb->unk70, 12, D_80163C74);
    SetRotMatrix(m);
    D_800F1698.flags |= MODEL_MIRROR_X;
    D_80163C74 = BattleDrawModel(&D_800F1698, g_cDb->unk70, 12, D_80163C74);
    SetRotMatrix(m);
    D_800F1698.flags |= MODEL_MIRROR_Z;
    D_80163C74 = BattleDrawModel(&D_800F1698, g_cDb->unk70, 12, D_80163C74);
    SetRotMatrix(m);
    D_800F1698.flags &= ~MODEL_MIRROR_X;
    D_80163C74 = BattleDrawModel(&D_800F1698, g_cDb->unk70, 12, D_80163C74);
    D_800F1698.uvOffset = 0;
    D_800F1698.clut = 0;
}

void BattleHitFlashGrowTick(void) {
    BattleHitFlashSlot* slot;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].hitFlash;
    D_800F16A8.m[1][1] = rsin(slot->frame << 7) + 0x1000;
    D_800F16A8.m[0][0] = D_800F16A8.m[2][2] = slot->frame * 1024;
    if (slot->frame < 8) {
        D_800F1698.color = 0;
    } else {
        D_800F1698.color = (slot->frame - 8) * 512;
    }
    D_800F16A8.t[0] = slot->pos.vx;
    D_800F16A8.t[1] = 0;
    D_800F16A8.t[2] = slot->pos.vz;
    CompMatrix(&g_BattleWorldView.m, &D_800F16A8, D_800F16C8);
    D_800F1698.model = D_800F15AC;
    BattleDrawHitFlashModel(D_800F16C8);
    if (D_80062D98 == 0) {
        if (++slot->frame == 16) {
            slot->unk0 = -1;
        }
    }
}

void BattleHitFlashBurstTick(void) {
    BattleHitFlashSlot* slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].hitFlash;
    u16 v;

    D_800F16CC.m[0][0] = D_800F16CC.m[2][2] = (slot->frame * 3) << 9;
    if (slot->frame < 8) {
        D_800F16CC.m[1][1] = (slot->frame * 3) << 10;
        D_800F1698.color = 0;
    } else if (slot->frame < 16) {
        D_800F16CC.m[1][1] = 0x6000;
        D_800F1698.color = (slot->frame - 8) << 9;
    }
    D_800F16CC.t[0] = slot->pos.vx;
    D_800F16CC.t[1] = 0;
    D_800F16CC.t[2] = slot->pos.vz;
    CompMatrix(&g_BattleWorldView.m, &D_800F16CC, D_800F16EC);
    D_800F1698.model = D_800F14E0;
    BattleDrawHitFlashModel(D_800F16EC);
    if (D_80062D98 == 0) {
        v = slot->frame + 1;
        slot->frame = v;
        if ((s16)v == 16) {
            slot->unk0 = -1;
        }
    }
}

void BattleHitFlashShrinkTick(void) {
    BattleHitFlashSlot* slot;
    u16 frame;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].hitFlash;
    D_800F16F0.m[1][1] = rsin((14 - slot->frame) << 7) + 0x1000;
    D_800F16F0.m[0][0] = D_800F16F0.m[2][2] = (14 - slot->frame) * 1024;
    if (slot->frame < 8) {
        D_800F1698.color = -(slot->frame << 12) / 8 + 0x1000;
    } else {
        D_800F1698.color = (slot->frame - 8) * 512;
    }
    D_800F16F0.t[0] = slot->pos.vx;
    D_800F16F0.t[1] = 0;
    D_800F16F0.t[2] = slot->pos.vz;
    CompMatrix(&g_BattleWorldView.m, &D_800F16F0, D_800F1710);
    D_800F1698.model = D_800F15AC;
    D_800F1698.uvOffset = 0x80;
    D_800F1698.clut = 0x80;
    BattleDrawHitFlashModel(D_800F1710);
    if (D_80062D98 == 0) {
        frame = slot->frame + 1;
        slot->frame = frame;
        if ((s16)frame == 16) {
            slot->unk0 = -1;
        }
    }
}

void func_800D6D8C(void) {
    VECTOR view;
    s32 flag;
    MATRIX* m;
    Unk800D6D8CSlot* spark;
    s32 progress;
    s32 arc;

    spark = &g_BattleDetachedSlots[g_BattleDetachedCursor].unk800D6D8C;
    progress = 0x1000 - (spark->frame << 12) / 8;
    arc = rsin(progress / 2);
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTrans(&spark->pos, &view, &flag);
    m = BattleSetBillboardMatrix(
        &spark->pos, (s16)((-(progress * 0x500) >> 12) + 0xA00), ((0x200 - view.vz) * progress) >> 12);
    m->t[0] += ((spark->dirX * progress) >> 12) + ((spark->perpX * arc) >> 11);
    m->t[1] += ((spark->dirY * progress) >> 12) + ((spark->perpY * arc) >> 11);
    SetTransMatrix(m);
    D_800F1714.clutBias = D_800F1720[spark->palette];
    D_80163C74 = func_800D4D90(&D_800F1714, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        if (++spark->frame >= 8) {
            spark->palette = -1;
        }
    }
}

void func_800D6F78(void) {
    u8 unused[0x50];
    Unk800D6F78Slot* slot;
    Unk800D6D8CSlot* spark;
    s32 palette;
    s32 angle;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].unk800D6F78;
    if (D_80062D98 == 0) {
        spark = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D6D8C)].unk800D6D8C;
        spark->palette = (slot->frame + palette) % 5;
        spark->pos = slot->pos;
        angle = rand() & 0xFFF;
        spark->dirX = (u32)(rsin(angle) * 25) >> 9;
        spark->dirY = (rcos(angle) * 200) >> 12;
        spark->perpX = spark->dirY;
        spark->perpY = -spark->dirX;
        if (++slot->frame >= 23) {
            slot->unk0 = -1;
        }
    }
}

void BattleSpawnTrailEffect(void) {
    BattleTrailSlot* src = &g_BattleDetachedSlots[g_BattleDetachedCursor].trail;
    BattleHitFlashSlot* dst;
    u16 v;

    if (D_80062D98 == 0) {
        if (!(src->frame & 3)) {
            dst = &g_BattleDetachedSlots[BattleDetachedRegister(src->spawnCallback)].hitFlash;
            dst->pos = src->pos;
        }
        v = src->frame + 1;
        src->frame = v;
        if ((s16)v == 0xD) {
            src->unk0 = -1;
        }
    }
}

void BattleSpawnPartEffect(s32 actor, s32 hitFlashType) {
    BattleTrailSlot* dst = &g_BattleDetachedSlots[BattleDetachedRegister(BattleSpawnTrailEffect)].trail;
    Unk800D6F78Slot* dst2;

    BattleGetPartPosition(actor, g_BattleModels[actor].boneIndices[0], &dst->pos);
    switch (hitFlashType) {
    case 0:
        dst->spawnCallback = BattleHitFlashGrowTick;
        return;
    case 1:
        dst->spawnCallback = BattleHitFlashBurstTick;
        return;
    case 2:
        dst->spawnCallback = BattleHitFlashShrinkTick;
        return;
    case 3:
        dst->spawnCallback = BattleHitFlashGrowTick;
        dst2 = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D6F78)].unk800D6F78;
        dst2->pos = dst->pos;
        return;
    }
}

static void BattleFixedPointRampEffectTick(void) {
    Unk801621F0* elem = &g_BattleDetachedSlots[g_BattleDetachedCursor].raw;

    if (D_80062D98 == 0) {
        // Advance this slot's per-tick state machine (field 0x2).
        if (elem->D_801621F2 == 0) {
            BattleFixedPointRampInit(1);
        }
        if (elem->D_801621F2 == 2) {
            BattleFixedPointRampReconfigure(1);
            elem->D_801621F0 = -1;
        }
        elem->D_801621F2++;
    }
}

static void BattleSpawnFixedPointRampEffect(void) { BattleEffectRegister(BattleFixedPointRampEffectTick); }

void func_800D7368(void) {
    MATRIX m;
    SVECTOR rot;
    BattleBounceParticle* p;

    rot.vz = 0;
    rot.vx = 0;
    p = &g_BattleDetachedSlots[g_BattleDetachedCursor].bounceParticle;
    rot.vy = p->facing;
    RotMatrixYXZ(&rot, &m);
    m.t[0] = p->pos.vx;
    m.t[1] = p->pos.vy;
    m.t[2] = p->pos.vz;
    CompMatrix(&g_BattleWorldView.m, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    D_80163C74 = BattleDrawModel(&D_800F1904, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        p->pos.vx += p->velocity.vx;
        p->pos.vy += p->velocity.vy;
        p->pos.vz += p->velocity.vz;
        p->velocity.vy += 30;
        if (p->pos.vy >= 0) {
            p->velocity.vy = (-p->velocity.vy >> 2) - (rand() & 0x1F);
            p->pos.vy = -p->pos.vy >> 2;
            p->velocity.vx = (p->velocity.vx >> 2) + (rand() & 0x1F) - 16;
            p->velocity.vz = (p->velocity.vz >> 2) + (rand() & 0x1F) - 16;
            p->bounces++;
            if (p->bounces == 2) {
                p->bounces = -1;
            }
            BattleSpawnSparkleEffect(&p->pos, 0x200, 0x400);
        }
    }
}

void func_800D751C(void) {
    MATRIX m;
    long p;
    long flag;
    POLY_FT4* quad;
    BattleStreakSlot* slot;
    s32 otz;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].streak;
    D_800F1954.vy = rand() & 0x3FF;
    RotMatrixYXZ(&D_800F1954, &D_800F1934);
    D_800F1934.t[0] = slot->pos.vx;
    D_800F1934.t[2] = slot->pos.vz;
    CompMatrix(&g_BattleWorldView.m, &D_800F1934, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    quad = D_80163C74;
    otz = RotAverage4(&D_800F1914, &D_800F191C, &D_800F1924, &D_800F192C, (long*)&quad->x0, (long*)&quad->x1,
                      (long*)&quad->x2, (long*)&quad->x3, &p, &flag);
    if (otz > 0) {
#ifdef PLATFORM_PSYZ
        setlen(quad, 9);
#else
        quad->tag = 0x09000000;
#endif
        if (slot->unk0 == 0) {
            *(u32*)&quad->r0 = 0x2E808080;
        } else {
            *(u32*)&quad->r0 = 0x2E202020;
        }
        quad->clut = 0x78C7;
        quad->tpage = 0x3A;
        *(s16*)&quad->u0 = 0xC000;
        *(s16*)&quad->u1 = 0xC03F;
        *(s16*)&quad->u2 = 0xFF00;
        *(s16*)&quad->u3 = 0xFF3F;
        AddPrim(&g_cDb->unk70[otz >> 2], quad);
        D_80163C74 = quad + 1;
    }
    slot->unk0 = -1;
}

void BattleSpawnStreakEffect(SVECTOR* pos) {
    BattleStreakSlot* dst;

    dst = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D751C)].streak;
    dst->pos = *pos;
    dst->unk0 = 1;
}

void BattleSubModelFlashTick(void) {
    MATRIX m;
    Unk801621F0* slot;
    BattleStreakSlot* streak;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].raw;
    if (D_80062D98 == 0 && slot->D_801621F2 != 0) {
        slot->D_801621F0 = -1;
        return;
    }
    D_800F195C.t[2] = -slot->unk1A;
    CompMatrix(slot->unk1C, &D_800F195C, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    D_80163C74 = BattleDrawModel(&D_800F197C, g_cDb->unk70, 12, D_80163C74);
    if (D_80062D98 == 0) {
        streak = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D751C)].streak;
        streak->pos.vx = m.t[0] - g_BattleWorldView.m.t[0];
        streak->pos.vy = m.t[1] - g_BattleWorldView.m.t[1];
        streak->pos.vz = m.t[2] - g_BattleWorldView.m.t[2];
        TransposeMatrix(&g_BattleWorldView.m, &m);
        ApplyMatrixSV(&m, &streak->pos, &streak->pos);
        streak->unk0 = 0;
        slot->D_801621F2++;
    }
}

void func_800D7888(void) {
    MATRIX facing;
    SVECTOR velocity;
    Unk801621F0* slot;
    Unk801621F0* child;
    BattleBounceParticle* particle;
    SVECTOR* pos;
    s32 elapsed;

    slot = &g_BattleDetachedSlots[g_BattleDetachedCursor].raw;
    if (D_80062D98 == 0) {
        elapsed = slot->D_801621F2;
        if (elapsed >= slot->unk8) {
            elapsed -= slot->unk8;
            if (elapsed < (s16)(slot->unkA & ~0x80)) {
                if (!(elapsed & 1)) {
                    child = &g_BattleDetachedSlots[BattleDetachedRegister(BattleSubModelFlashTick)].raw;
                    child->D_801621F6 = slot->D_801621F6;
                    child->D_801621F4 = slot->D_801621F4;
                    child->unk1C = slot->unk1C;
                    child->unk1A = slot->unk1A;
                    if (!(slot->unkA & 0x80)) {
                        child = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D7368)].raw;
                        particle = (BattleBounceParticle*)child;
                        pos = &particle->pos;
                        BattleGetMatrixPosition(slot->unk1C, pos);
                        velocity.vx = -60 - (rand() & 0xF);
                        velocity.vy = (rand() & 0x1F) - 150;
                        velocity.vz = (rand() & 0xF) + 20;
                        RotMatrixYXZ(&g_BattleModels[slot->D_801621F6].rootRot, &facing);
                        ApplyMatrixSV(&facing, &velocity, &particle->velocity);
                        particle->actor = slot->D_801621F6;
                        particle->bounces = 0;
                        particle->facing = g_BattleModels[slot->D_801621F6].rootRot.vy;
                        BattleSpawnSparkleEffect(pos, 0x400, 0x800);
                    }
                }
            } else {
                slot->D_801621F0 = -1;
            }
        }
        slot->D_801621F2++;
    }
}

void BattleSpawnSpriteEffect(s32 arg0, s32 actor, BattleModelSub* bone, s16 arg3, s32 arg4, s32 arg5) {
    Unk801621F0* slot = &g_BattleDetachedSlots[BattleDetachedRegister(func_800D7888)].raw;

    slot->D_801621F4 = arg0;
    slot->D_801621F6 = actor;
    slot->unk1C = bone;
    slot->unk1A = arg3;
    slot->unk8 = (s16)arg4;
    slot->unkA = (s16)arg5;
}

void BattleSpawnSpriteEffectAtSubModel(s32 arg0, s32 idx, s32 arg2, s32 arg3) {
    BattleSpawnSpriteEffect(arg0, idx, &g_BattleModels[idx].boneTransforms[g_BattleModels[idx].boneIndices[6]],
                            g_BattleModels[idx].defaultRotY, arg2, arg3);
}

void BattleSpawnSpriteEffectAtSubModel2(s32 arg0, s32 idx, s32 arg2, s32 arg3) {
    BattleSpawnSpriteEffect(arg0, idx, &g_BattleModels[idx].boneTransforms[g_BattleModels[idx].boneIndices[7]],
                            g_BattleModels[idx].defaultRotZ, arg2, arg3);
}

void BattleSpawnSpriteEffectAtBothSubModels(s32 arg0, s32 idx, s32 arg2, s32 arg3) {
    BattleSpawnSpriteEffect(arg0, idx, &g_BattleModels[idx].boneTransforms[g_BattleModels[idx].boneIndices[6]],
                            g_BattleModels[idx].defaultRotY, arg2, arg3);
    BattleSpawnSpriteEffect(arg0, idx, &g_BattleModels[idx].boneTransforms[g_BattleModels[idx].boneIndices[7]],
                            g_BattleModels[idx].defaultRotZ, arg2, arg3);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D7D3C);

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D8304);

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D83A4);

typedef struct {
    /* 0x00 */ u16 unk0;
    /* 0x02 */ u16 unk2;
    /* 0x04 */ u16 unk4;
    /* 0x06 */ u16 unk6;
    /* 0x08 */ u16 unk8;
    /* 0x0A */ u16 unkA;
    /* 0x0C */ u16 unkC;
    /* 0x0E */ u16 unkE;
    /* 0x10 */ u16 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
} UnkStruct800D8468; // size:0x20

static void BattleCopyEffectState(UnkStruct800D8468* dst, UnkStruct800D8468* src) {
    dst->unk0 = src->unk0;
    dst->unk6 = src->unk6;
    dst->unkC = src->unkC;
    dst->unk2 = src->unk2;
    dst->unk8 = src->unk8;
    dst->unkE = src->unkE;
    dst->unk4 = src->unk4;
    dst->unkA = src->unkA;
    dst->unk10 = src->unk10;
    dst->unk14 = src->unk14;
    dst->unk18 = src->unk18;
    dst->unk1C = src->unk1C;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D84F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D85B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/D3548", func_800D87EC);
