//! PSYQ=3.3

#include <game.h>

#include <libetc.h>
#include <psxsdk/inline_c.h>

void HighwayModelScalePartVrtxs(FieldModelPart* part, s16 scale, s32 force);
void HighwayModelScaleAnimTranslat(FieldModelAnimation* animation, s16 scale, s32 force);
void func_800B2F2C(FieldModelPart* part, s16 r, s16 g, s16 b);

// Same as FieldModelScaleModel.
void HighwayModelScaleModel(FieldModelEntry* model, s16 scale, s32 force) {
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
s32 HighwayApplyModelLightingToPacket(FieldModelEntry* model, u8* light) {
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

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_model2", func_800B2F2C);
