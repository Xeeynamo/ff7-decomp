//! PSYQ=3.3 CC1=2.6.3
#include "world.h"
#include <gte.h>
#include <libetc.h>
#include <psxsdk/gtemac.h>
#include <psxsdk/inline_c.h>

s32 WmCreatePacketForModelPart(FieldModelPart*, s32, s32, s32);
void WmScaleModelVertexes(FieldModelPart* part, s16 scale, s32 force);
void WmScaleModelAnimations(FieldModelAnimation* animation, s16 scale, s32 force);
void WmScaleModelAll(FieldModelEntry* model, s16 scale, s32 force);
void WmApplyPolyLightingToPacket(FieldModelPart* part, s16 r, s16 g, s16 b);
void WmUpdatePartLighting(FieldModelPart* part);
void WmCalculatePartLighting(FieldModelPart* part, s32 force);

s32 WmLoadModelPacketAndScale(FieldModelEntry* model, s32 packet, s32 arg2) {
    FieldModelPart* parts;
    u32 i;

    model->partMatrices = (MATRIX*)packet;
    packet += model->boneCount * 32;
    parts = (FieldModelPart*)(model->partsOffset + (u_long)model->modelData);
    for (i = 0; i < model->partCount; i++) {
        packet = WmCreatePacketForModelPart(&parts[i], packet, 0, arg2);
    }
    WmScaleModelAll(model, (s16)model->scale, 0);
    return packet;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmCreatePacketForModelPart);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", func_800C02F4);

void WmLoadTexturesToVram(WorldTextureBlock* block) {
    RECT rect;
    WorldTexture* textures;
    u32 i;
    u32 count;

    count = block->textureCount;
    textures = block->textures;
    for (i = 0; i < count; i++) {
        rect.x = textures[i].x;
        rect.y = textures[i].y;
        rect.w = textures[i].w;
        rect.h = textures[i].h;
        LoadImage(&rect, (u_long*)((u8*)block + textures[i].dataOffset));
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdateModelPacket2);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdatePacketForModelPartWithoutMatrixes);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmCalculateBoneMatrixes);

void WmScaleModelAll(FieldModelEntry* model, s16 scale, s32 force) {
    MATRIX* m;
    SVECTOR* tmp;
    VECTOR* out;
    FieldModelPart* parts;
    FieldModelAnimation* anims;
    u16* raw;
    u8* data;
    u32 i;
    s32 count;

    tmp = (SVECTOR*)getScratchAddr(sizeof(MATRIX) / 4);
    out = (VECTOR*)getScratchAddr((sizeof(MATRIX) + sizeof(SVECTOR)) / 4);
    m = (MATRIX*)getScratchAddr(0);
    parts = (FieldModelPart*)(model->partsOffset + (u_long)model->modelData);
    count = model->partCount;
    for (i = 0; i < count; i++) {
        WmScaleModelVertexes(&parts[i], scale, force);
    }
    m->m[2][2] = m->m[1][1] = m->m[0][0] = scale;
    m->t[0] = m->t[1] = m->t[2] = 0;
    m->m[0][1] = m->m[0][2] = m->m[1][0] = m->m[1][2] = m->m[2][0] = m->m[2][1] = 0;
    gte_SetRotMatrix(m);
    gte_SetTransMatrix(m);
    data = model->modelData;
    count = (u8)(model->boneCount / 3);
    raw = (u16*)data;
    for (i = 0; i < count; i++) {
        tmp->vx = raw[i * 6];
        tmp->vy = raw[i * 6 + 2];
        tmp->vz = raw[i * 6 + 4];
        gte_ldv0(tmp);
        gte_rt();
        gte_stlvnl(out);
        raw[i * 6] = out->vx;
        raw[i * 6 + 2] = out->vy;
        raw[i * 6 + 4] = out->vz;
    }
    for (i = count * 3; i < model->boneCount; i++) {
        tmp->vx = raw[i * 2];
        gte_ldv0(tmp);
        gte_rt();
        gte_stlvnl(out);
        raw[i * 2] = out->vx;
    }
    anims = (FieldModelAnimation*)(model->animationOffset + (u_long)model->modelData);
    count = model->animationCount;
    for (i = 0; i < count; i++) {
        WmScaleModelAnimations(&anims[i], scale, force);
    }
}

void WmScaleModelVertexes(FieldModelPart* part, s16 scale, s32 force) {
    MATRIX* m;
    VECTOR* out;
    SVECTOR* verts;
    u32 i;
    u8 count;

    out = (VECTOR*)getScratchAddr(sizeof(MATRIX) / 4);
    m = (MATRIX*)getScratchAddr(0);
    if (!(((WorldPartData*)part->data)->flags & 1) || force) {
        m->m[0][0] = scale;
        m->m[1][1] = scale;
        m->m[2][2] = scale;
        m->t[2] = 0;
        m->t[1] = 0;
        m->t[0] = 0;
        m->m[2][1] = 0;
        m->m[2][0] = 0;
        m->m[1][2] = 0;
        m->m[1][0] = 0;
        m->m[0][2] = 0;
        m->m[0][1] = 0;
        gte_SetRotMatrix(m);
        gte_SetTransMatrix(m);
        verts = ((WorldPartData*)part->data)->verts;
        count = part->vertexCount;
        for (i = 0; i < count; i++) {
            gte_ldv0(&verts[i]);
            gte_rt();
            gte_stlvnl(out);
            verts[i].vx = *(u16*)&out->vx;
            verts[i].vy = *(u16*)&out->vy;
            verts[i].vz = *(u16*)&out->vz;
        }
        ((WorldPartData*)part->data)->flags |= 1;
    }
}

// Same as FieldModelScaleAnimTranslat.
void WmScaleModelAnimations(FieldModelAnimation* animation, s16 scale, s32 force) {
    MATRIX* scaleMatrix;
    SVECTOR* input;
    VECTOR* output;
    u32 frameCount;
    u32 count;
    u32 groups;
    u32 i, j;

    scaleMatrix = (MATRIX*)getScratchAddr(0);
    // The matrix can be overwritten after it has been loaded into the GTE.
    input = (SVECTOR*)getScratchAddr(0);
    output = (VECTOR*)getScratchAddr(2);
    if (*(u32*)animation->data && !force) {
        return;
    }
    scaleMatrix->m[0][0] = scale;
    scaleMatrix->m[1][1] = scale;
    scaleMatrix->m[2][2] = scale;
    scaleMatrix->m[0][1] = scaleMatrix->m[0][2] = scaleMatrix->m[1][0] = scaleMatrix->m[1][2] = scaleMatrix->m[2][0] =
        scaleMatrix->m[2][1] = scaleMatrix->t[0] = scaleMatrix->t[1] = scaleMatrix->t[2] = 0;
    gte_SetRotMatrix(scaleMatrix);
    gte_SetTransMatrix(scaleMatrix);
    count = animation->translationCount;
    frameCount = animation->frameCount;
    for (i = 0; i < count; i++) {
        s16* trans;

        trans = (s16*)(animation->data + animation->translationOffset) + i * frameCount;
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
        s16* statics;

        statics = (s16*)(animation->data + animation->staticTranslationsOffset) + i * 3;
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
        s16* statics;

        statics = (s16*)(animation->data + animation->staticTranslationsOffset) + i;
        input->vx = *statics;
        gte_ldv0(input);
        gte_rt();
        gte_stlvnl(output);
        *statics = output->vx;
    }
    *(u32*)animation->data = 1;
}

s32 WmApplyModelLightingToPacket(FieldModelEntry* model, u8* params) {
    FieldModelPart* parts;
    u32 i, count;
    s16 r, g, b;

    count = model->partCount;
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    r = KAWAI_READ_S16(params, 0);
    g = KAWAI_READ_S16(params, 2);
    b = KAWAI_READ_S16(params, 4);
    *(s32*)getScratchAddr(128) = params[6];
    for (i = 0; i < count; i++) {
        WmApplyPolyLightingToPacket(&parts[i], r, g, b);
    }
    return 1;
}

void WmApplyPolyLightingToPacket(FieldModelPart* part, s16 r, s16 g, s16 b) {
    u8* packets;
    u8* polygons;
    u32 counts;
    u32 count;
    u32 i;
    u8 code;
    s32 rOffset, gOffset, bOffset;

    packets = part->packets;
    polygons = part->data + part->polygonsOffset;
    if (D_800C752C) {
        packets += part->packetBufferSize;
    }
    gte_lddp(16);
    rOffset = r << 4;
    gOffset = g << 4;
    bOffset = b << 4;

    if (*(s32*)getScratchAddr(128) & 1) {
        counts = *(u32*)&part->polyGT4Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += sizeof(FieldModelPolyGT4)) {
            POLY_GT4* polyGT4 = (POLY_GT4*)packets;
            FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
            code = getcode(polyGT4);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[0]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[1]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[2]);
            gte_gpl0();
            gte_stRGB0(&polyGT4->r0);
            gte_stRGB1(&polyGT4->r1);
            gte_strgb(&polyGT4->r2);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[3]);
            gte_gpl0();
            gte_strgb(&polyGT4->r3);
            setcode(polyGT4, code);
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += sizeof(FieldModelPolyGT3)) {
            POLY_GT3* polyGT3 = (POLY_GT3*)packets;
            FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
            code = getcode(polyGT3);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[0]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[1]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[2]);
            gte_gpl0();
            gte_stRGB0(&polyGT3->r0);
            gte_stRGB1(&polyGT3->r1);
            gte_strgb(&polyGT3->r2);
            setcode(polyGT3, code);
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += sizeof(FieldModelPolyFT4)) {
            POLY_FT4* polyFT4 = (POLY_FT4*)packets;
            FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
            code = getcode(polyFT4);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->color);
            gte_gpl0();
            gte_strgb(&polyFT4->r0);
            setcode(polyFT4, code);
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += sizeof(FieldModelPolyFT3)) {
            POLY_FT3* polyFT3 = (POLY_FT3*)packets;
            FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
            code = getcode(polyFT3);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->color);
            gte_gpl0();
            gte_strgb(&polyFT3->r0);
            setcode(polyFT3, code);
        }

        counts = *(u32*)&part->polyF3Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += sizeof(FieldModelPolyF3)) {
            POLY_F3* polyF3 = (POLY_F3*)packets;
            FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
            code = getcode(polyF3);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->color);
            gte_gpl0();
            gte_strgb(&polyF3->r0);
            setcode(polyF3, code);
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += sizeof(FieldModelPolyF4)) {
            POLY_F4* polyF4 = (POLY_F4*)packets;
            FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
            code = getcode(polyF4);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->color);
            gte_gpl0();
            gte_strgb(&polyF4->r0);
            setcode(polyF4, code);
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += sizeof(FieldModelPolyG3)) {
            POLY_G3* polyG3 = (POLY_G3*)packets;
            FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
            code = getcode(polyG3);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[0]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[1]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[2]);
            gte_gpl0();
            gte_stRGB0(&polyG3->r0);
            gte_stRGB1(&polyG3->r1);
            gte_strgb(&polyG3->r2);
            setcode(polyG3, code);
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += sizeof(FieldModelPolyG4)) {
            POLY_G4* polyG4 = (POLY_G4*)packets;
            FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
            code = getcode(polyG4);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[0]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[1]);
            gte_gpl0();
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[2]);
            gte_gpl0();
            gte_stRGB0(&polyG4->r0);
            gte_stRGB1(&polyG4->r1);
            gte_strgb(&polyG4->r2);
            gte_LoadMAC(rOffset, gOffset, bOffset);
            gte_ldcv(&source->colors[3]);
            gte_gpl0();
            gte_strgb(&polyG4->r3);
            setcode(polyG4, code);
        }
    } else {
        counts = *(u32*)&part->polyGT4Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += sizeof(FieldModelPolyGT4)) {
            POLY_GT4* polyGT4 = (POLY_GT4*)packets;
            FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
            if (polyGT4->tag << 8) {
                code = getcode(polyGT4);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[0]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[1]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[2]);
                gte_gpl0();
                gte_stRGB0(&polyGT4->r0);
                gte_stRGB1(&polyGT4->r1);
                gte_strgb(&polyGT4->r2);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[3]);
                gte_gpl0();
                gte_strgb(&polyGT4->r3);
                setcode(polyGT4, code);
            }
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += sizeof(FieldModelPolyGT3)) {
            POLY_GT3* polyGT3 = (POLY_GT3*)packets;
            FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
            if (polyGT3->tag << 8) {
                code = getcode(polyGT3);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[0]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[1]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[2]);
                gte_gpl0();
                gte_stRGB0(&polyGT3->r0);
                gte_stRGB1(&polyGT3->r1);
                gte_strgb(&polyGT3->r2);
                setcode(polyGT3, code);
            }
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += sizeof(FieldModelPolyFT4)) {
            POLY_FT4* polyFT4 = (POLY_FT4*)packets;
            FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
            if (polyFT4->tag << 8) {
                code = getcode(polyFT4);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->color);
                gte_gpl0();
                gte_strgb(&polyFT4->r0);
                setcode(polyFT4, code);
            }
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += sizeof(FieldModelPolyFT3)) {
            POLY_FT3* polyFT3 = (POLY_FT3*)packets;
            FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
            if (polyFT3->tag << 8) {
                code = getcode(polyFT3);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->color);
                gte_gpl0();
                gte_strgb(&polyFT3->r0);
                setcode(polyFT3, code);
            }
        }

        counts = *(u32*)&part->polyF3Count;
        count = counts & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += sizeof(FieldModelPolyF3)) {
            POLY_F3* polyF3 = (POLY_F3*)packets;
            FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
            if (polyF3->tag << 8) {
                code = getcode(polyF3);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->color);
                gte_gpl0();
                gte_strgb(&polyF3->r0);
                setcode(polyF3, code);
            }
        }

        count = (counts & 0xFF00) >> 8;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += sizeof(FieldModelPolyF4)) {
            POLY_F4* polyF4 = (POLY_F4*)packets;
            FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
            if (polyF4->tag << 8) {
                code = getcode(polyF4);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->color);
                gte_gpl0();
                gte_strgb(&polyF4->r0);
                setcode(polyF4, code);
            }
        }

        count = (counts >> 16) & 0xFF;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += sizeof(FieldModelPolyG3)) {
            POLY_G3* polyG3 = (POLY_G3*)packets;
            FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
            if (polyG3->tag << 8) {
                code = getcode(polyG3);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[0]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[1]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[2]);
                gte_gpl0();
                gte_stRGB0(&polyG3->r0);
                gte_stRGB1(&polyG3->r1);
                gte_strgb(&polyG3->r2);
                setcode(polyG3, code);
            }
        }

        count = counts >> 24;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += sizeof(FieldModelPolyG4)) {
            POLY_G4* polyG4 = (POLY_G4*)packets;
            FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
            if (polyG4->tag << 8) {
                code = getcode(polyG4);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[0]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[1]);
                gte_gpl0();
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[2]);
                gte_gpl0();
                gte_stRGB0(&polyG4->r0);
                gte_stRGB1(&polyG4->r1);
                gte_strgb(&polyG4->r2);
                gte_LoadMAC(rOffset, gOffset, bOffset);
                gte_ldcv(&source->colors[3]);
                gte_gpl0();
                gte_strgb(&polyG4->r3);
                setcode(polyG4, code);
            }
        }
    }
}

s32 WmCalculateModelLighting(FieldModelEntry* model, u8* params) {
    MATRIX direction;
    MATRIX* partMatrices;
    FieldModelPart* parts;
    u32 i;
    MATRIX* color;
    s32 force;

    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    partMatrices = model->partMatrices;
    gte_SetBackColor((s32)params[0], (s32)params[1], (s32)params[2]);
    color = (MATRIX*)getScratchAddr(128);
    color->m[0][0] = params[3] << 4;
    color->m[1][0] = params[4] << 4;
    color->m[2][0] = params[5] << 4;
    color->m[0][1] = params[6] << 4;
    color->m[1][1] = params[7] << 4;
    color->m[2][1] = params[8] << 4;
    color->m[0][2] = params[9] << 4;
    color->m[1][2] = params[10] << 4;
    color->m[2][2] = params[11] << 4;
    color->t[0] = color->t[1] = color->t[2] = 0;
    gte_SetColorMatrix(color);
    direction.m[0][0] = KAWAI_READ_S16(params, 12);
    direction.m[0][1] = KAWAI_READ_S16(params, 14);
    direction.m[0][2] = KAWAI_READ_S16(params, 16);
    direction.m[1][0] = KAWAI_READ_S16(params, 18);
    direction.m[1][1] = KAWAI_READ_S16(params, 20);
    direction.m[1][2] = KAWAI_READ_S16(params, 22);
    direction.m[2][0] = KAWAI_READ_S16(params, 24);
    direction.m[2][1] = KAWAI_READ_S16(params, 26);
    direction.m[2][2] = KAWAI_READ_S16(params, 28);
    direction.t[0] = direction.t[1] = direction.t[2] = 0;
    force = params[30];
    for (i = 0; i < model->partCount; i++) {
        if (parts[i].flags) {
            MATRIX* light = (MATRIX*)getScratchAddr(136);
            if (model->kawaiType != -1) {
                MATRIX* transformedLight = (MATRIX*)getScratchAddr(144);
                gte_CompMatrix(partMatrices, &partMatrices[parts[i].boneIndex], light);
                gte_CompMatrix(&direction, light, transformedLight);
                gte_SetLightMatrix(transformedLight);
            } else {
                gte_CompMatrix(&direction, &partMatrices[parts[i].boneIndex], light);
                gte_SetLightMatrix(light);
            }
        }
        WmCalculatePartLighting(&parts[i], force);
    }
    return 1;
}

void WmCalculatePartLighting(FieldModelPart* part, s32 force) {
    u8* polygons;
    SVECTOR* normalTable;
    CVECTOR* result;
    u32 counts, count, i, j;

    normalTable = D_800C7938;
    result = (CVECTOR*)getScratchAddr(0);
    if ((*(u32*)part->data & 0x2) && !force) {
        return;
    }
    polygons = part->data + part->polygonsOffset;
    counts = *(u32*)&part->polyGT4Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyGT4)) {
        FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
        for (j = 0; j < 4; j++) {
            gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j].r, result);
            source->colors[j].r = result->r;
            source->colors[j].g = result->g;
            source->colors[j].b = result->b;
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyGT3)) {
        FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
        for (j = 0; j < 3; j++) {
            gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j].r, result);
            source->colors[j].r = result->r;
            source->colors[j].g = result->g;
            source->colors[j].b = result->b;
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyFT4)) {
        FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
        gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color.r, result);
        source->color.r = result->r;
        source->color.g = result->g;
        source->color.b = result->b;
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyFT3)) {
        FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
        gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color.r, result);
        source->color.r = result->r;
        source->color.g = result->g;
        source->color.b = result->b;
    }
    counts = *(u32*)&part->polyF3Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyF3)) {
        FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
        gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color.r, result);
        source->color.r = result->r;
        source->color.g = result->g;
        source->color.b = result->b;
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyF4)) {
        FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
        gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color.r, result);
        source->color.r = result->r;
        source->color.g = result->g;
        source->color.b = result->b;
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyG3)) {
        FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
        for (j = 0; j < 3; j++) {
            gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j].r, result);
            source->colors[j].r = result->r;
            source->colors[j].g = result->g;
            source->colors[j].b = result->b;
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, polygons += sizeof(FieldModelPolyG4)) {
        FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
        for (j = 0; j < 4; j++) {
            gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j].r, result);
            source->colors[j].r = result->r;
            source->colors[j].g = result->g;
            source->colors[j].b = result->b;
        }
    }
    *(u32*)part->data |= 0x2;
}

void WmUpdatePartTransparency(FieldModelPart* part, s32 enable) {
    u8* packet;
    u32 j, i;
    u32 count;

    for (i = 0; i < 2; i++) {
        packet = part->packets;
        if (i) {
            packet += part->packetBufferSize;
        }
        count = part->polyGT4Count;
        for (j = 0; j < count; j++, packet += sizeof(POLY_GT4)) {
            setSemiTrans(packet, enable);
            setShadeTex(packet, enable);
        }
        count = part->polyGT3Count;
        for (j = 0; j < count; j++, packet += sizeof(POLY_GT3)) {
            setSemiTrans(packet, enable);
            setShadeTex(packet, enable);
        }
        count = part->polyFT4Count;
        for (j = 0; j < count; j++, packet += sizeof(POLY_FT4)) {
            setSemiTrans(packet, enable);
            setShadeTex(packet, enable);
        }
        count = part->polyFT3Count;
        for (j = 0; j < count; j++, packet += sizeof(POLY_FT3)) {
            setSemiTrans(packet, enable);
            setShadeTex(packet, enable);
        }
        count = part->polyF3Count;
        for (j = 0; j < count; j++, packet += sizeof(POLY_F3)) {
            setSemiTrans(packet, enable);
            setShadeTex(packet, enable);
        }
        count = part->polyF4Count;
        for (j = 0; j < count; j++, packet += sizeof(POLY_F4)) {
            setSemiTrans(packet, enable);
            setShadeTex(packet, enable);
        }
        count = part->polyG3Count;
        for (j = 0; j < count; j++, packet += sizeof(POLY_G3)) {
            setSemiTrans(packet, enable);
            setShadeTex(packet, enable);
        }
        count = part->polyG4Count;
        for (j = 0; j < count; j++, packet += sizeof(POLY_G4)) {
            setSemiTrans(packet, enable);
            setShadeTex(packet, enable);
        }
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdateModelPacket);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdatePacketForModelPartWithMatrixes);

s32 WmUpdateModelLighting(FieldModelEntry* model, u8* params) {
    MATRIX direction;
    MATRIX* partMatrices;
    FieldModelPart* parts;
    u32 i;
    MATRIX* color;

    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    partMatrices = model->partMatrices;
    gte_SetBackColor(params[0], params[1], params[2]);
    color = (MATRIX*)getScratchAddr(128);
    color->m[0][0] = params[3] << 4;
    color->m[1][0] = params[4] << 4;
    color->m[2][0] = params[5] << 4;
    color->m[0][1] = params[6] << 4;
    color->m[1][1] = params[7] << 4;
    color->m[2][1] = params[8] << 4;
    color->m[0][2] = params[9] << 4;
    color->m[1][2] = params[10] << 4;
    color->m[2][2] = params[11] << 4;
    color->t[0] = color->t[1] = color->t[2] = 0;
    gte_SetColorMatrix(color);
    direction.m[0][0] = KAWAI_READ_S16(params, 12);
    direction.m[0][1] = KAWAI_READ_S16(params, 14);
    direction.m[0][2] = KAWAI_READ_S16(params, 16);
    direction.m[1][0] = KAWAI_READ_S16(params, 18);
    direction.m[1][1] = KAWAI_READ_S16(params, 20);
    direction.m[1][2] = KAWAI_READ_S16(params, 22);
    direction.m[2][0] = KAWAI_READ_S16(params, 24);
    direction.m[2][1] = KAWAI_READ_S16(params, 26);
    direction.m[2][2] = KAWAI_READ_S16(params, 28);
    direction.t[0] = direction.t[1] = direction.t[2] = 0;
    for (i = 0; i < model->partCount; i++) {
        if (parts[i].flags) {
            MATRIX* light = (MATRIX*)getScratchAddr(136);
            gte_CompMatrix(&direction, &partMatrices[parts[i].boneIndex], light);
            gte_SetLightMatrix(light);
        }
        WmUpdatePartLighting(&parts[i]);
    }
    return 1;
}

void WmUpdatePartLighting(FieldModelPart* part) {
    u8* packets;
    SVECTOR* normalTable;
    u8* polygons;
    u32 counts, count, i, j;

    normalTable = D_800C7938;
    packets = part->packets;
    polygons = part->data + part->polygonsOffset;
    if (D_800C752C) {
        packets += part->packetBufferSize;
    }
    counts = *(u32*)&part->polyGT4Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += sizeof(FieldModelPolyGT4)) {
        POLY_GT4* poly = (POLY_GT4*)packets;
        if (poly->tag) {
            FieldModelPolyGT4* source = (FieldModelPolyGT4*)polygons;
            u8* output;
            u8 code = getcode(poly);
            for (j = 0, output = &poly->r0; j < 4; output += 12, j++) {
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += sizeof(FieldModelPolyGT3)) {
        POLY_GT3* poly = (POLY_GT3*)packets;
        if (poly->tag) {
            FieldModelPolyGT3* source = (FieldModelPolyGT3*)polygons;
            u8* output;
            u8 code = getcode(poly);
            for (j = 0, output = &poly->r0; j < 3; output += 12, j++) {
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += sizeof(FieldModelPolyFT4)) {
        POLY_FT4* poly = (POLY_FT4*)packets;
        if (poly->tag) {
            FieldModelPolyFT4* source = (FieldModelPolyFT4*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += sizeof(FieldModelPolyFT3)) {
        POLY_FT3* poly = (POLY_FT3*)packets;
        if (poly->tag) {
            FieldModelPolyFT3* source = (FieldModelPolyFT3*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    counts = *(u32*)&part->polyF3Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += sizeof(FieldModelPolyF3)) {
        POLY_F3* poly = (POLY_F3*)packets;
        if (poly->tag) {
            FieldModelPolyF3* source = (FieldModelPolyF3*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += sizeof(FieldModelPolyF4)) {
        POLY_F4* poly = (POLY_F4*)packets;
        if (poly->tag) {
            FieldModelPolyF4* source = (FieldModelPolyF4*)polygons;
            u8 code = getcode(poly);
            u8* output = &poly->r0;
            gte_NormalColorCol(&normalTable[source->color.normalIndex], &source->color, output);
            setcode(poly, code);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += sizeof(FieldModelPolyG3)) {
        POLY_G3* poly = (POLY_G3*)packets;
        if (poly->tag) {
            FieldModelPolyG3* source = (FieldModelPolyG3*)polygons;
            u8* output;
            u8 code = getcode(poly);
            for (j = 0, output = &poly->r0; j < 3; output += 8, j++) {
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += sizeof(FieldModelPolyG4)) {
        POLY_G4* poly = (POLY_G4*)packets;
        if (poly->tag) {
            FieldModelPolyG4* source = (FieldModelPolyG4*)polygons;
            u8* output;
            u8 code = getcode(poly);
            for (j = 0, output = &poly->r0; j < 4; output += 8, j++) {
                gte_NormalColorCol(&normalTable[source->colors[j].normalIndex], &source->colors[j], output);
            }
            setcode(poly, code);
        }
    }
}

s32 WmGetModelTotalRenderPacketSize(FieldModelEntry* model) {
    FieldModelPart* part;
    u32 i;
    s32 size;

    size = model->boneCount * 32;
    part = (FieldModelPart*)(model->partsOffset + (u_long)model->modelData);
    for (i = 0; i < model->partCount; i++) {
        size += part->packetBufferSize * 2;
        part++;
    }
    return size;
}
