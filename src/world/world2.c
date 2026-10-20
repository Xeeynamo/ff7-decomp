//! PSYQ=3.3 CC1=2.6.3
#include "world.h"
#include <gte.h>
#include <libetc.h>
#include <psxsdk/gtemac.h>
#include <psxsdk/inline_c.h>

u8* WmCreatePacketForModelPart(FieldModelPart* part, u8* nextFree, s32 relocate, s32 modelId);
void WmScaleModelVertexes(FieldModelPart* part, s16 scale, s32 force);
void WmScaleModelAnimations(FieldModelAnimation* animation, s16 scale, s32 force);
void WmScaleModelAll(FieldModel* model, s16 scale, s32 force);
void WmApplyPolyLightingToPacket(FieldModelPart* part, s16 r, s16 g, s16 b);
void WmUpdatePartLighting(FieldModelPart* part);
void WmCalculatePartLighting(FieldModelPart* part, s32 force);
void WmUpdatePacketForModelPartWithMatrixes(FieldModelPart* part, SVECTOR* normal, MATRIX* toPlane, MATRIX* fromPlane);

u8* WmLoadModelPacketAndScale(FieldModel* model, u8* nextFree, s32 modelId) {
    FieldModelPart* parts;
    u32 i;

    model->partMatrices = (MATRIX*)nextFree;
    nextFree += model->boneCount * sizeof(MATRIX);
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    for (i = 0; i < model->partCount; i++) {
        nextFree = WmCreatePacketForModelPart(&parts[i], nextFree, 0, modelId);
    }
    WmScaleModelAll(model, model->scale, 0);
    return nextFree;
}

u8* WmCreatePacketForModelPart(FieldModelPart* part, u8* nextFree, s32 relocate, s32 modelId) {
    u8* textureFlags;
    u32* polygons;
    u8* packets;
    u32 buffer;
    u32 i;
    u32 count;
    s32 uOffset;
    s32 vOffset;
    s32 uOffset4;
    s32 vOffset4;
    s32 uOffset8;
    s32 vOffset8;
    u16* texCoords;
    u32* textures;

    textures = (u32*)(part->data + part->texturesOffset);
    texCoords = (u16*)(part->data + part->texCoordsOffset);
    if (relocate) {
        part->data = (u8*)(part + 1);
    }
    part->packets = nextFree;
    uOffset4 = (modelId % 4) * 64;
    vOffset4 = (modelId / 4) * 32;
    uOffset8 = (modelId % 8) * 32;
    vOffset8 = (modelId / 8) * 32;

    for (buffer = 0; buffer < 2; buffer++) {
        packets = nextFree;
        textureFlags = part->data + part->textureFlagsOffset;
        if (buffer) {
            packets += part->packetBufferSize;
        }
        polygons = (u32*)(part->data + part->polygonsOffset);
        count = part->polyGT4Count;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), polygons += sizeof(FieldModelPolyGT4) / sizeof(u32)) {
            POLY_GT4* poly = (POLY_GT4*)packets;
            u32 uvIndices;
            u32 texture;
            u32 mode;
            s32 paletteOffset;
            u8 flags;

            *(u32*)&poly->r0 = polygons[1];
            *(u32*)&poly->r1 = polygons[2];
            *(u32*)&poly->r2 = polygons[3];
            *(u32*)&poly->r3 = polygons[4];
            uvIndices = polygons[5];
            *(u16*)&poly->u0 = texCoords[uvIndices & 0xFF];
            *(u16*)&poly->u1 = texCoords[(uvIndices & 0xFF00) >> 8];
            *(u16*)&poly->u2 = texCoords[(uvIndices & 0xFF0000) >> 16];
            *(u16*)&poly->u3 = texCoords[uvIndices >> 24];
            flags = *textureFlags++;
            texture = textures[flags & 0xF];
            paletteOffset = (texture & 0x3F) == 2 ? 0 : modelId;
            poly->clut = getClut(((texture >> 16) & 0x3F) * 16, ((texture & 0x7FC00000) >> 22) + paletteOffset);
            poly->tpage =
                getTPage((texture & 0xC0) >> 6, flags >> 5, (texture & 0xF00) >> 2, ((texture >> 12) & 0x1) * 256);
            mode = texture & 0x3F;
            if (mode == 0) {
                uOffset = uOffset4;
                vOffset = vOffset4;
            } else if (mode == 1) {
                uOffset = uOffset8;
                vOffset = vOffset8;
            } else {
                uOffset = vOffset = 0;
            }
            setPolyGT4(poly);
            poly->u0 += uOffset;
            poly->v0 += vOffset;
            poly->u1 += uOffset;
            poly->v1 += vOffset;
            poly->u2 += uOffset;
            poly->v2 += vOffset;
            poly->u3 += uOffset;
            poly->v3 += vOffset;
            if (flags & 0x10) {
                setcode(poly, 0x3E);
            }
        }
        count = part->polyGT3Count;
        for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), polygons += sizeof(FieldModelPolyGT3) / sizeof(u32)) {
            POLY_GT3* poly = (POLY_GT3*)packets;
            u32 uvIndices;
            u32 texture;
            u32 mode;
            s32 paletteOffset;
            u8 flags;

            *(u32*)&poly->r0 = polygons[1];
            *(u32*)&poly->r1 = polygons[2];
            *(u32*)&poly->r2 = polygons[3];
            uvIndices = polygons[4];
            *(u16*)&poly->u0 = texCoords[uvIndices & 0xFF];
            *(u16*)&poly->u1 = texCoords[(uvIndices & 0xFF00) >> 8];
            *(u16*)&poly->u2 = texCoords[(uvIndices & 0xFF0000) >> 16];
            flags = *textureFlags++;
            texture = textures[flags & 0xF];
            paletteOffset = (texture & 0x3F) == 2 ? 0 : modelId;
            poly->clut = getClut(((texture >> 16) & 0x3F) * 16, ((texture & 0x7FC00000) >> 22) + paletteOffset);
            poly->tpage =
                getTPage((texture & 0xC0) >> 6, flags >> 5, (texture & 0xF00) >> 2, ((texture >> 12) & 0x1) * 256);
            mode = texture & 0x3F;
            if (mode == 0) {
                uOffset = uOffset4;
                vOffset = vOffset4;
            } else if (mode == 1) {
                uOffset = uOffset8;
                vOffset = vOffset8;
            } else {
                uOffset = vOffset = 0;
            }
            setPolyGT3(poly);
            poly->u0 += uOffset;
            poly->v0 += vOffset;
            poly->u1 += uOffset;
            poly->v1 += vOffset;
            poly->u2 += uOffset;
            poly->v2 += vOffset;
            if (flags & 0x10) {
                setcode(poly, 0x36);
            }
        }
        count = part->polyFT4Count;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), polygons += sizeof(FieldModelPolyFT4) / sizeof(u32)) {
            POLY_FT4* poly = (POLY_FT4*)packets;
            u32 uvIndices;
            u32 texture;
            u32 mode;
            s32 paletteOffset;
            u8 flags;

            *(u32*)&poly->r0 = polygons[1];
            uvIndices = polygons[2];
            *(u16*)&poly->u0 = texCoords[uvIndices & 0xFF];
            *(u16*)&poly->u1 = texCoords[(uvIndices & 0xFF00) >> 8];
            *(u16*)&poly->u2 = texCoords[(uvIndices & 0xFF0000) >> 16];
            *(u16*)&poly->u3 = texCoords[uvIndices >> 24];
            flags = *textureFlags++;
            texture = textures[flags & 0xF];
            paletteOffset = (texture & 0x3F) == 2 ? 0 : modelId;
            poly->clut = getClut(((texture >> 16) & 0x3F) * 16, ((texture & 0x7FC00000) >> 22) + paletteOffset);
            poly->tpage =
                getTPage((texture & 0xC0) >> 6, flags >> 5, (texture & 0xF00) >> 2, ((texture >> 12) & 0x1) * 256);
            mode = texture & 0x3F;
            if (mode == 0) {
                uOffset = uOffset4;
                vOffset = vOffset4;
            } else if (mode == 1) {
                uOffset = uOffset8;
                vOffset = vOffset8;
            } else {
                uOffset = vOffset = 0;
            }
            setPolyFT4(poly);
            poly->u0 += uOffset;
            poly->v0 += vOffset;
            poly->u1 += uOffset;
            poly->v1 += vOffset;
            poly->u2 += uOffset;
            poly->v2 += vOffset;
            poly->u3 += uOffset;
            poly->v3 += vOffset;
            if (flags & 0x10) {
                setcode(poly, 0x2E);
            }
        }
        count = part->polyFT3Count;
        for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), polygons += sizeof(FieldModelPolyFT3) / sizeof(u32)) {
            POLY_FT3* poly = (POLY_FT3*)packets;
            u32 uvIndices;
            u32 texture;
            u32 mode;
            s32 paletteOffset;
            u8 flags;

            *(u32*)&poly->r0 = polygons[1];
            uvIndices = polygons[2];
            *(u16*)&poly->u0 = texCoords[uvIndices & 0xFF];
            *(u16*)&poly->u1 = texCoords[(uvIndices & 0xFF00) >> 8];
            *(u16*)&poly->u2 = texCoords[(uvIndices & 0xFF0000) >> 16];
            flags = *textureFlags++;
            texture = textures[flags & 0xF];
            paletteOffset = (texture & 0x3F) == 2 ? 0 : modelId;
            poly->clut = getClut(((texture >> 16) & 0x3F) * 16, ((texture & 0x7FC00000) >> 22) + paletteOffset);
            poly->tpage =
                getTPage((texture & 0xC0) >> 6, flags >> 5, (texture & 0xF00) >> 2, ((texture >> 12) & 0x1) * 256);
            mode = texture & 0x3F;
            if (mode == 0) {
                uOffset = uOffset4;
                vOffset = vOffset4;
            } else if (mode == 1) {
                uOffset = uOffset8;
                vOffset = vOffset8;
            } else {
                uOffset = vOffset = 0;
            }
            setPolyFT3(poly);
            poly->u0 += uOffset;
            poly->v0 += vOffset;
            poly->u1 += uOffset;
            poly->v1 += vOffset;
            poly->u2 += uOffset;
            poly->v2 += vOffset;
            if (flags & 0x10) {
                setcode(poly, 0x26);
            }
        }
        count = part->polyF3Count;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F3), polygons += sizeof(FieldModelPolyF3) / sizeof(u32)) {
            POLY_F3* poly = (POLY_F3*)packets;
            *(u32*)&poly->r0 = polygons[1];
            setPolyF3(poly);
        }
        count = part->polyF4Count;
        for (i = 0; i < count; i++, packets += sizeof(POLY_F4), polygons += sizeof(FieldModelPolyF4) / sizeof(u32)) {
            POLY_F4* poly = (POLY_F4*)packets;
            *(u32*)&poly->r0 = polygons[1];
            setPolyF4(poly);
        }
        count = part->polyG3Count;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G3), polygons += sizeof(FieldModelPolyG3) / sizeof(u32)) {
            POLY_G3* poly = (POLY_G3*)packets;
            *(u32*)&poly->r0 = polygons[1];
            *(u32*)&poly->r1 = polygons[2];
            *(u32*)&poly->r2 = polygons[3];
            setPolyG3(poly);
        }
        count = part->polyG4Count;
        for (i = 0; i < count; i++, packets += sizeof(POLY_G4), polygons += sizeof(FieldModelPolyG4) / sizeof(u32)) {
            POLY_G4* poly = (POLY_G4*)packets;
            *(u32*)&poly->r0 = polygons[1];
            *(u32*)&poly->r1 = polygons[2];
            *(u32*)&poly->r2 = polygons[3];
            *(u32*)&poly->r3 = polygons[4];
            setPolyG4(poly);
        }
    }
    return nextFree + part->packetBufferSize * 2;
}

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

void WmScaleModelAll(FieldModel* model, s16 scale, s32 force) {
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

s32 WmApplyModelLightingToPacket(FieldModel* model, u8* params) {
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

s32 WmCalculateModelLighting(FieldModel* model, u8* params) {
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

s32 WmUpdateModelPacket(FieldModel* model, u8* params) {
    KawaiClipPlane* plane;
    MATRIX* partMatrices;

    plane = (KawaiClipPlane*)D_800C8100[params[1]];
    switch (params[0]) {
    case 0: {
        u32 i;
        FieldModelPart* parts;
        SVECTOR* normal = (SVECTOR*)getScratchAddr(128);
        SVECTOR* unit = (SVECTOR*)getScratchAddr(130);
        SVECTOR* anchor = (SVECTOR*)getScratchAddr(132);
        SVECTOR* reference = (SVECTOR*)getScratchAddr(134);
        SVECTOR* axis = (SVECTOR*)getScratchAddr(136);
        SVECTOR* unitAxis = (SVECTOR*)getScratchAddr(138);
        s16 cosine;
        s16 sine;
        s32 xx, yy, zz, xy, xz, yz, xs, ys, zs;
        s32 complement;

        plane->normal[0] = KAWAI_READ_S16(params, 2);
        plane->normal[1] = KAWAI_READ_S16(params, 4);
        plane->normal[2] = KAWAI_READ_S16(params, 6);
        plane->distance = KAWAI_READ_S16(params, 8);
        setVector(normal, plane->normal[0], plane->normal[1], plane->normal[2]);
        normal->pad = 0;
        VectorNormalSS(normal, unit);
        plane->unitNormal[0] = unit->vx;
        plane->unitNormal[1] = unit->vy;
        plane->unitNormal[2] = unit->vz;
        if (unit->vz) {
            anchor->vy = 0;
            anchor->vx = 0;
            anchor->vz = (plane->distance << 12) / unit->vz;
        } else if (unit->vy) {
            anchor->vz = 0;
            anchor->vx = 0;
            anchor->vy = (plane->distance << 12) / unit->vy;
        } else if (unit->vx) {
            anchor->vz = 0;
            anchor->vy = 0;
            anchor->vx = (plane->distance << 12) / unit->vx;
        } else {
            return 1;
        }
        plane->anchor[0] = anchor->vx;
        plane->anchor[1] = anchor->vy;
        plane->anchor[2] = anchor->vz;
        setVector(reference, 0, 0, 4096);
        cosine = (unit->vx * reference->vx + unit->vy * reference->vy + unit->vz * reference->vz) / 4096;
        setVector(axis, (unit->vy * reference->vz - reference->vy * unit->vz) / 4096,
                  (reference->vx * unit->vz - unit->vx * reference->vz) / 4096,
                  (unit->vx * reference->vy - reference->vx * unit->vy) / 4096);
        sine = SquareRoot0(axis->vx * axis->vx + axis->vy * axis->vy + axis->vz * axis->vz);
        VectorNormalSS(axis, unitAxis);
        if (!unitAxis->vx && !unitAxis->vy && !unitAxis->vz) {
            plane->rotation[0][0] = 4096;
            plane->rotation[0][1] = 0;
            plane->rotation[0][2] = 0;
            plane->rotation[1][0] = 0;
            plane->rotation[1][1] = 4096;
            plane->rotation[1][2] = 0;
            plane->rotation[2][0] = 0;
            plane->rotation[2][1] = 0;
            plane->rotation[2][2] = 4096;
        } else {
            xx = unitAxis->vx * unitAxis->vx;
            yy = unitAxis->vy * unitAxis->vy;
            zz = unitAxis->vz * unitAxis->vz;
            xy = unitAxis->vx * unitAxis->vy;
            xz = unitAxis->vx * unitAxis->vz;
            yz = unitAxis->vy * unitAxis->vz;
            xs = unitAxis->vx * sine;
            ys = unitAxis->vy * sine;
            zs = unitAxis->vz * sine;
            complement = 4096 - cosine;
            plane->rotation[0][0] = (xx + cosine * ((4096 * 4096 - xx) / 4096)) / 4096;
            plane->rotation[0][1] = ((xy / 4096) * complement - zs) / 4096;
            plane->rotation[0][2] = ((xz / 4096) * complement + ys) / 4096;
            plane->rotation[1][0] = ((xy / 4096) * complement + zs) / 4096;
            plane->rotation[1][1] = (yy + cosine * ((4096 * 4096 - yy) / 4096)) / 4096;
            plane->rotation[1][2] = ((yz / 4096) * complement - xs) / 4096;
            plane->rotation[2][0] = ((xz / 4096) * complement - ys) / 4096;
            plane->rotation[2][1] = ((yz / 4096) * complement + xs) / 4096;
            plane->rotation[2][2] = (zz + cosine * ((4096 * 4096 - zz) / 4096)) / 4096;
        }
        parts = (FieldModelPart*)(model->modelData + model->partsOffset);
        for (i = 0; i < model->partCount; i++) {
            parts[i].flags = 0;
        }
        break;
    }
    case 1: {
        u32 i;
        FieldModelPart* parts;
        SVECTOR* normal = (SVECTOR*)getScratchAddr(128);
        MATRIX* translation = (MATRIX*)getScratchAddr(130);
        MATRIX* rotation = (MATRIX*)getScratchAddr(138);
        MATRIX* transpose = (MATRIX*)getScratchAddr(146);
        MATRIX* toPlane = (MATRIX*)getScratchAddr(154);
        MATRIX* fromPlane = (MATRIX*)getScratchAddr(162);

        parts = (FieldModelPart*)(model->modelData + model->partsOffset);
        partMatrices = model->partMatrices;
        translation->m[0][0] = translation->m[1][1] = translation->m[2][2] = 4096;
        translation->t[0] = translation->t[1] = translation->t[2] = 0;
        translation->m[0][1] = translation->m[0][2] = translation->m[1][0] = translation->m[1][2] =
            translation->m[2][0] = translation->m[2][1] = 0;
        rotation->t[0] = rotation->t[1] = rotation->t[2] = 0;
        transpose->t[0] = transpose->t[1] = transpose->t[2] = 0;
        setVector(normal, plane->unitNormal[0], plane->unitNormal[1], plane->unitNormal[2]);
        normal->pad = 0;
        for (i = 0; i < model->partCount; i++) {
            translation->t[0] = -plane->anchor[0];
            translation->t[1] = -plane->anchor[1];
            translation->t[2] = -plane->anchor[2];
            rotation->m[0][0] = plane->rotation[0][0];
            rotation->m[0][1] = plane->rotation[0][1];
            rotation->m[0][2] = plane->rotation[0][2];
            rotation->m[1][0] = plane->rotation[1][0];
            rotation->m[1][1] = plane->rotation[1][1];
            rotation->m[1][2] = plane->rotation[1][2];
            rotation->m[2][0] = plane->rotation[2][0];
            rotation->m[2][1] = plane->rotation[2][1];
            rotation->m[2][2] = plane->rotation[2][2];
            gte_CompMatrix(translation, &partMatrices[parts[i].boneIndex], toPlane);
            gte_CompMatrix(rotation, toPlane, toPlane);
            translation->t[0] = plane->anchor[0];
            translation->t[1] = plane->anchor[1];
            translation->t[2] = plane->anchor[2];
            TransposeMatrix(rotation, transpose);
            gte_CompMatrix(partMatrices, translation, fromPlane);
            gte_CompMatrix(fromPlane, transpose, fromPlane);
            WmUpdatePacketForModelPartWithMatrixes(&parts[i], normal, toPlane, fromPlane);
        }
        return 0;
    }
    }
    return 1;
}

void WmUpdatePacketForModelPartWithMatrixes(FieldModelPart* part, SVECTOR* normal, MATRIX* toPlane, MATRIX* fromPlane) {
    OT_TYPE* ot;
    KawaiClippedVertex* vertices;
    u8* data;
    u8* packets;
    u32 counts;
    u32 count;
    u32 i;
    s32 reverse;

    ot = D_800C7530;
    gte_SetRotMatrix(toPlane);
    gte_SetTransMatrix(toPlane);
    data = part->data + 4;
    count = part->vertexCount;
    vertices = D_800C80BC;
    reverse = !normal->vx && !normal->vy && normal->vz == -4096;
    for (i = 0; i < count; i++) {
        gte_ldv0(data);
        gte_rtv0tr();
        data += sizeof(SVECTOR);
        gte_stsv(&vertices[i].position);
        if (reverse) {
            if (vertices[i].position.vz > 0) {
                vertices[i].position.vz = 0;
            }
        } else if (vertices[i].position.vz < 0) {
            vertices[i].position.vz = 0;
        }
    }
    gte_SetRotMatrix(fromPlane);
    gte_SetTransMatrix(fromPlane);
    for (i = 0; i < count; i++) {
        gte_ldv0(&vertices[i].position);
        gte_rtps();
        gte_stsxy(&vertices[i].screen);
        gte_stsz(&vertices[i].depth);
    }
    packets = part->packets;
    if (D_800C752C) {
        packets += part->packetBufferSize;
    }
    counts = *(u32*)&part->polyGT4Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT4), data += sizeof(FieldModelPolyGT4)) {
        POLY_GT4* polygon = (POLY_GT4*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        KawaiClippedVertex* v3 = &vertices[indices >> 24];
        s32 facing;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;
        s32 screen3 = v3->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        *(s32*)&polygon->x3 = screen3;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz && !v3->position.vz)) {
            setaddr(polygon, 0);
        } else {
            addPrim(&ot[(v0->depth + v1->depth + v2->depth + v3->depth) >> 4], polygon);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_GT3), data += sizeof(FieldModelPolyGT3)) {
        POLY_GT3* polygon = (POLY_GT3*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        s32 facing;
        u32 depth;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            gte_ldsz3(v0->depth, v1->depth, v2->depth);
            gte_avsz3();
            gte_rdOTZ(depth);
            addPrim(&ot[depth], polygon);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT4), data += sizeof(FieldModelPolyFT4)) {
        POLY_FT4* polygon = (POLY_FT4*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        KawaiClippedVertex* v3 = &vertices[indices >> 24];
        s32 facing;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;
        s32 screen3 = v3->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        *(s32*)&polygon->x3 = screen3;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz && !v3->position.vz)) {
            setaddr(polygon, 0);
        } else {
            addPrim(&ot[(v0->depth + v1->depth + v2->depth + v3->depth) >> 4], polygon);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_FT3), data += sizeof(FieldModelPolyFT3)) {
        POLY_FT3* polygon = (POLY_FT3*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        s32 facing;
        u32 depth;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            gte_ldsz3(v0->depth, v1->depth, v2->depth);
            gte_avsz3();
            gte_rdOTZ(depth);
            addPrim(&ot[depth], polygon);
        }
    }
    counts = *(u32*)&part->polyF3Count;
    count = counts & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F3), data += sizeof(FieldModelPolyF3)) {
        POLY_F3* polygon = (POLY_F3*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        s32 facing;
        u32 depth;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            gte_ldsz3(v0->depth, v1->depth, v2->depth);
            gte_avsz3();
            gte_rdOTZ(depth);
            addPrim(&ot[depth], polygon);
        }
    }
    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, packets += sizeof(POLY_F4), data += sizeof(FieldModelPolyF4)) {
        POLY_F4* polygon = (POLY_F4*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        KawaiClippedVertex* v3 = &vertices[indices >> 24];
        s32 facing;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;
        s32 screen3 = v3->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        *(s32*)&polygon->x3 = screen3;
        gte_rdMAC0(facing);
        // Unlike GT4 and FT4, the original leaves v3 out of this test.
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            addPrim(&ot[(v0->depth + v1->depth + v2->depth + v3->depth) >> 4], polygon);
        }
    }
    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G3), data += sizeof(FieldModelPolyG3)) {
        POLY_G3* polygon = (POLY_G3*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        s32 facing;
        u32 depth;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        gte_rdMAC0(facing);
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            gte_ldsz3(v0->depth, v1->depth, v2->depth);
            gte_avsz3();
            gte_rdOTZ(depth);
            addPrim(&ot[depth], polygon);
        }
    }
    count = counts >> 24;
    for (i = 0; i < count; i++, packets += sizeof(POLY_G4), data += sizeof(FieldModelPolyG4)) {
        POLY_G4* polygon = (POLY_G4*)packets;
        u32 indices = *(u32*)data;
        KawaiClippedVertex* v0 = &vertices[indices & 0xFF];
        KawaiClippedVertex* v1 = &vertices[(indices & 0xFF00) >> 8];
        KawaiClippedVertex* v2 = &vertices[(indices >> 16) & 0xFF];
        KawaiClippedVertex* v3 = &vertices[indices >> 24];
        s32 facing;
        s32 screen0 = v0->screen;
        s32 screen1 = v1->screen;
        s32 screen2 = v2->screen;
        s32 screen3 = v3->screen;

        gte_ldsxy3(screen0, screen1, screen2);
        gte_nclip();
        *(s32*)&polygon->x0 = screen0;
        *(s32*)&polygon->x1 = screen1;
        *(s32*)&polygon->x2 = screen2;
        *(s32*)&polygon->x3 = screen3;
        gte_rdMAC0(facing);
        // Unlike GT4 and FT4, the original leaves v3 out of this test.
        if (facing <= 0 || (!v0->position.vz && !v1->position.vz && !v2->position.vz)) {
            setaddr(polygon, 0);
        } else {
            addPrim(&ot[(v0->depth + v1->depth + v2->depth + v3->depth) >> 4], polygon);
        }
    }
}

s32 WmUpdateModelLighting(FieldModel* model, u8* params) {
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

s32 WmGetModelTotalRenderPacketSize(FieldModel* model) {
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
