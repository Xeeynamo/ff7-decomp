//! PSYQ=3.3

#include <game.h>

#include <libetc.h>
#include <psxsdk/inline_c.h>

void HighwayModelScalePartVrtxs(FieldModelPart* part, s16 scale, s32 force);
void HighwayModelScaleAnimTranslat(FieldModelAnimation* animation, s16 scale, s32 force);
void func_800B2F2C(FieldModelPart* part, s16 r, s16 g, s16 b);

// Same as FieldModelScaleModel.
void HighwayModelScaleModel(FieldModel* model, s16 scale, s32 force) {
    MATRIX* scaleMatrix = (MATRIX*)getScratchAddr(0);
    SVECTOR* input = (SVECTOR*)getScratchAddr(8);
    VECTOR* output = (VECTOR*)getScratchAddr(10);
    FieldModelPart* parts;
    FieldModelAnimation* animations;
    FieldModelBone* bones;
    u32 count;
    u32 i;

    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    count = model->partCount;
    for (i = 0; i < count; i++) {
        HighwayModelScalePartVrtxs(&parts[i], scale, force);
    }
    scaleMatrix->m[0][0] = scale;
    scaleMatrix->m[1][1] = scale;
    scaleMatrix->m[2][2] = scale;
    scaleMatrix->m[0][1] = scaleMatrix->m[0][2] = scaleMatrix->m[1][0] = scaleMatrix->m[1][2] = scaleMatrix->m[2][0] =
        scaleMatrix->m[2][1] = scaleMatrix->t[0] = scaleMatrix->t[1] = scaleMatrix->t[2] = 0;
    gte_SetRotMatrix(scaleMatrix);
    gte_SetTransMatrix(scaleMatrix);
    bones = (FieldModelBone*)model->modelData;
    count = (u8)(model->boneCount / 3);
    for (i = 0; i < count; i++) {
        input->vx = bones[i * 3].length;
        input->vy = bones[i * 3 + 1].length;
        input->vz = bones[i * 3 + 2].length;
        gte_ldv0(input);
        gte_rt();
        gte_stlvnl(output);
        bones[i * 3].length = output->vx;
        bones[i * 3 + 1].length = output->vy;
        bones[i * 3 + 2].length = output->vz;
    }
    for (i = count * 3; i < model->boneCount; i++) {
        input->vx = bones[i].length;
        gte_ldv0(input);
        gte_rt();
        gte_stlvnl(output);
        bones[i].length = output->vx;
    }
    animations = (FieldModelAnimation*)(model->modelData + model->animationOffset);
    count = model->animationCount;
    for (i = 0; i < count; i++) {
        HighwayModelScaleAnimTranslat(&animations[i], scale, force);
    }
}

// Same as FieldModelScalePartVrtxs.
void HighwayModelScalePartVrtxs(FieldModelPart* part, s16 scale, s32 force) {
    MATRIX* scaleMatrix = (MATRIX*)getScratchAddr(0);
    VECTOR* output = (VECTOR*)getScratchAddr(8);
    SVECTOR* vertices;
    u8 count;
    u32 i;

    if (!(*(u32*)part->data & 0x1) || force) {
        scaleMatrix->m[0][0] = scale;
        scaleMatrix->m[1][1] = scale;
        scaleMatrix->m[2][2] = scale;
        scaleMatrix->m[0][1] = scaleMatrix->m[0][2] = scaleMatrix->m[1][0] = scaleMatrix->m[1][2] =
            scaleMatrix->m[2][0] = scaleMatrix->m[2][1] = scaleMatrix->t[0] = scaleMatrix->t[1] = scaleMatrix->t[2] = 0;
        gte_SetRotMatrix(scaleMatrix);
        gte_SetTransMatrix(scaleMatrix);
        vertices = (SVECTOR*)(part->data + sizeof(u32));
        count = part->vertexCount;
        for (i = 0; i < count; i++) {
            gte_ldv0(&vertices[i]);
            gte_rt();
            gte_stlvnl(output);
            vertices[i].vx = output->vx;
            vertices[i].vy = output->vy;
            vertices[i].vz = output->vz;
        }
        *(u32*)part->data |= 0x1;
    }
}

// Same as FieldModelScaleAnimTranslat.
void HighwayModelScaleAnimTranslat(FieldModelAnimation* animation, s16 scale, s32 force) {
    MATRIX* scaleMatrix = (MATRIX*)getScratchAddr(0);
    // The matrix can be overwritten after it has been loaded into the GTE.
    SVECTOR* input = (SVECTOR*)getScratchAddr(0);
    VECTOR* output = (VECTOR*)getScratchAddr(2);
    u32 frameCount;
    u32 count;
    u32 groups;
    u32 i, j;

    if (!*(u32*)animation->data || force) {
        scaleMatrix->m[0][0] = scale;
        scaleMatrix->m[1][1] = scale;
        scaleMatrix->m[2][2] = scale;
        scaleMatrix->m[0][1] = scaleMatrix->m[0][2] = scaleMatrix->m[1][0] = scaleMatrix->m[1][2] =
            scaleMatrix->m[2][0] = scaleMatrix->m[2][1] = scaleMatrix->t[0] = scaleMatrix->t[1] = scaleMatrix->t[2] = 0;
        gte_SetRotMatrix(scaleMatrix);
        gte_SetTransMatrix(scaleMatrix);
        count = animation->translationCount;
        frameCount = animation->frameCount;
        for (i = 0; i < count; i++) {
            s16* trans = (s16*)(animation->data + animation->translationOffset) + i * frameCount;
            groups = frameCount / 3;
            for (j = 0; j < groups; j++) {
                input->vx = trans[j * 3];
                input->vy = trans[j * 3 + 1];
                input->vz = trans[j * 3 + 2];
                gte_ldv0(input);
                gte_rt();
                gte_stlvnl(output);
                trans[j * 3] = output->vx;
                trans[j * 3 + 1] = output->vy;
                trans[j * 3 + 2] = output->vz;
            }
            for (j = groups * 3; j < frameCount; j++) {
                input->vx = trans[j];
                gte_ldv0(input);
                gte_rt();
                gte_stlvnl(output);
                trans[j] = output->vx;
            }
        }
        count = animation->staticTranslationCount;
        groups = count / 3;
        for (i = 0; i < groups; i++) {
            s16* statics = (s16*)(animation->data + animation->staticTranslationsOffset) + i * 3;
            input->vx = statics[0];
            input->vy = statics[1];
            input->vz = statics[2];
            gte_ldv0(input);
            gte_rt();
            gte_stlvnl(output);
            statics[0] = output->vx;
            statics[1] = output->vy;
            statics[2] = output->vz;
        }
        for (i = groups * 3; i < count; i++) {
            s16* statics = (s16*)(animation->data + animation->staticTranslationsOffset) + i;
            input->vx = *statics;
            gte_ldv0(input);
            gte_rt();
            gte_stlvnl(output);
            *statics = output->vx;
        }
        *(u32*)animation->data = 1;
    }
}

// Same as WmApplyModelLightingToPacket, except the scratchpad word is written through getScratchAddr(0x80).
s32 HighwayApplyModelLightingToPacket(FieldModel* model, u8* light) {
    FieldModelPart* parts;
    u32 i;
    u32 count;
    s16 r;
    s16 g;
    s16 b;
    u8 unused[8];

    count = model->partCount;
    parts = (FieldModelPart*)(model->partsOffset + (u_long)model->modelData);
    r = (light[1] << 8) | light[0];
    g = (light[3] << 8) | light[2];
    b = (light[5] << 8) | light[4];
    *(s32*)getScratchAddr(0x80) = light[6];
    for (i = 0; i < count; i++) {
        func_800B2F2C(&parts[i], r, g, b);
    }
    return 1;
}

extern u8 g_HighwayKawaiBufferIndex[4];

#ifdef PLATFORM_PSYZ
#define gte_ldmac(r0, r1, r2)
#define gte_strgb0(r0)
#define gte_strgb1(r0)
#define gte_strgb2(r0)
#else
#define gte_ldmac(r0, r1, r2)                                                                                          \
    __asm__ volatile("mtc2 %0, $25;"                                                                                   \
                     "mtc2 %1, $26;"                                                                                   \
                     "mtc2 %2, $27"                                                                                    \
                     :                                                                                                 \
                     : "r"(r0), "r"(r1), "r"(r2))
#define gte_strgb0(r0) __asm__ volatile("swc2 $20, 0(%0)" : : "r"(r0) : "memory")
#define gte_strgb1(r0) __asm__ volatile("swc2 $21, 0(%0)" : : "r"(r0) : "memory")
#define gte_strgb2(r0) __asm__ volatile("swc2 $22, 0(%0)" : : "r"(r0) : "memory")

#endif

// Lights one vertex: MAC = base color, IR = vertex color, then interpolates.
#define LIGHT_VERTEX(r, g, b, col)                                                                                     \
    gte_ldmac(r, g, b);                                                                                                \
    gte_ldcv(col);                                                                                                     \
    gte_gpl0()

void func_800B2F2C(FieldModelPart* part, s16 r, s16 g, s16 b) {
    u8* pkt;
    u8* poly;
    u32 counts;
    u32 n;
    u32 i;
    u8 code;
    s32 cr;
    s32 cg;
    s32 cb;

    pkt = part->packets;
    poly = part->data + part->polygonsOffset;
    if (g_HighwayKawaiBufferIndex[0]) {
        pkt += part->packetBufferSize;
    }
    gte_lddp(0x10);
    cr = r << 4;
    cg = g << 4;
    cb = b << 4;
    if (*(s32*)0x1F800200 & 1) {
        counts = *(u32*)&part->polyGT4Count;
        n = counts & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_GT4), poly += 0x18) {
            code = ((POLY_GT4*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            LIGHT_VERTEX(cr, cg, cb, poly + 8);
            LIGHT_VERTEX(cr, cg, cb, poly + 12);
            gte_strgb0(&((POLY_GT4*)pkt)->r0);
            gte_strgb1(&((POLY_GT4*)pkt)->r1);
            gte_strgb2(&((POLY_GT4*)pkt)->r2);
            LIGHT_VERTEX(cr, cg, cb, poly + 16);
            gte_strgb2(&((POLY_GT4*)pkt)->r3);
            ((POLY_GT4*)pkt)->code = code;
        }
        n = (counts & 0xFF00) >> 8;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_GT3), poly += 0x14) {
            code = ((POLY_GT3*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            LIGHT_VERTEX(cr, cg, cb, poly + 8);
            LIGHT_VERTEX(cr, cg, cb, poly + 12);
            gte_strgb0(&((POLY_GT3*)pkt)->r0);
            gte_strgb1(&((POLY_GT3*)pkt)->r1);
            gte_strgb2(&((POLY_GT3*)pkt)->r2);
            ((POLY_GT3*)pkt)->code = code;
        }
        n = (counts >> 16) & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_FT4), poly += 0xC) {
            code = ((POLY_FT4*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            gte_strgb2(&((POLY_FT4*)pkt)->r0);
            ((POLY_FT4*)pkt)->code = code;
        }
        n = counts >> 24;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_FT3), poly += 0xC) {
            code = ((POLY_FT3*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            gte_strgb2(&((POLY_FT3*)pkt)->r0);
            ((POLY_FT3*)pkt)->code = code;
        }
        counts = *(u32*)&part->polyF3Count;
        n = counts & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_F3), poly += 0x8) {
            code = ((POLY_F3*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            gte_strgb2(&((POLY_F3*)pkt)->r0);
            ((POLY_F3*)pkt)->code = code;
        }
        n = (counts & 0xFF00) >> 8;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_F4), poly += 0x8) {
            code = ((POLY_F4*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            gte_strgb2(&((POLY_F4*)pkt)->r0);
            ((POLY_F4*)pkt)->code = code;
        }
        n = (counts >> 16) & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_G3), poly += 0x10) {
            code = ((POLY_G3*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            LIGHT_VERTEX(cr, cg, cb, poly + 8);
            LIGHT_VERTEX(cr, cg, cb, poly + 12);
            gte_strgb0(&((POLY_G3*)pkt)->r0);
            gte_strgb1(&((POLY_G3*)pkt)->r1);
            gte_strgb2(&((POLY_G3*)pkt)->r2);
            ((POLY_G3*)pkt)->code = code;
        }
        n = counts >> 24;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_G4), poly += 0x14) {
            code = ((POLY_G4*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            LIGHT_VERTEX(cr, cg, cb, poly + 8);
            LIGHT_VERTEX(cr, cg, cb, poly + 12);
            gte_strgb0(&((POLY_G4*)pkt)->r0);
            gte_strgb1(&((POLY_G4*)pkt)->r1);
            gte_strgb2(&((POLY_G4*)pkt)->r2);
            LIGHT_VERTEX(cr, cg, cb, poly + 16);
            gte_strgb2(&((POLY_G4*)pkt)->r3);
            ((POLY_G4*)pkt)->code = code;
        }
    } else {
        // Only relight packets that are linked into an ordering table.
        counts = *(u32*)&part->polyGT4Count;
        n = counts & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_GT4), poly += 0x18) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_GT4*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                LIGHT_VERTEX(cr, cg, cb, poly + 8);
                LIGHT_VERTEX(cr, cg, cb, poly + 12);
                gte_strgb0(&((POLY_GT4*)pkt)->r0);
                gte_strgb1(&((POLY_GT4*)pkt)->r1);
                gte_strgb2(&((POLY_GT4*)pkt)->r2);
                LIGHT_VERTEX(cr, cg, cb, poly + 16);
                gte_strgb2(&((POLY_GT4*)pkt)->r3);
                ((POLY_GT4*)pkt)->code = code;
            }
        }
        n = (counts & 0xFF00) >> 8;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_GT3), poly += 0x14) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_GT3*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                LIGHT_VERTEX(cr, cg, cb, poly + 8);
                LIGHT_VERTEX(cr, cg, cb, poly + 12);
                gte_strgb0(&((POLY_GT3*)pkt)->r0);
                gte_strgb1(&((POLY_GT3*)pkt)->r1);
                gte_strgb2(&((POLY_GT3*)pkt)->r2);
                ((POLY_GT3*)pkt)->code = code;
            }
        }
        n = (counts >> 16) & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_FT4), poly += 0xC) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_FT4*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                gte_strgb2(&((POLY_FT4*)pkt)->r0);
                ((POLY_FT4*)pkt)->code = code;
            }
        }
        n = counts >> 24;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_FT3), poly += 0xC) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_FT3*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                gte_strgb2(&((POLY_FT3*)pkt)->r0);
                ((POLY_FT3*)pkt)->code = code;
            }
        }
        counts = *(u32*)&part->polyF3Count;
        n = counts & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_F3), poly += 0x8) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_F3*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                gte_strgb2(&((POLY_F3*)pkt)->r0);
                ((POLY_F3*)pkt)->code = code;
            }
        }
        n = (counts & 0xFF00) >> 8;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_F4), poly += 0x8) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_F4*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                gte_strgb2(&((POLY_F4*)pkt)->r0);
                ((POLY_F4*)pkt)->code = code;
            }
        }
        n = (counts >> 16) & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_G3), poly += 0x10) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_G3*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                LIGHT_VERTEX(cr, cg, cb, poly + 8);
                LIGHT_VERTEX(cr, cg, cb, poly + 12);
                gte_strgb0(&((POLY_G3*)pkt)->r0);
                gte_strgb1(&((POLY_G3*)pkt)->r1);
                gte_strgb2(&((POLY_G3*)pkt)->r2);
                ((POLY_G3*)pkt)->code = code;
            }
        }
        n = counts >> 24;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_G4), poly += 0x14) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_G4*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                LIGHT_VERTEX(cr, cg, cb, poly + 8);
                LIGHT_VERTEX(cr, cg, cb, poly + 12);
                gte_strgb0(&((POLY_G4*)pkt)->r0);
                gte_strgb1(&((POLY_G4*)pkt)->r1);
                gte_strgb2(&((POLY_G4*)pkt)->r2);
                LIGHT_VERTEX(cr, cg, cb, poly + 16);
                gte_strgb2(&((POLY_G4*)pkt)->r3);
                ((POLY_G4*)pkt)->code = code;
            }
        }
    }
}
