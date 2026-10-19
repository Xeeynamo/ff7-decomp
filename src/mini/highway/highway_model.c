//! PSYQ=3.3

#include <game.h>

#include <psxsdk/inline_c.h>

u8* HighwayModelCreatePktsForPart(FieldModelPart* part, u8* nextFree, s32 relocate, s32 modelId);
void HighwayModelScaleModel(FieldModel* model, s16 scale, s32 force);

// Same as FieldModelCreatePktsAndScale.
u8* HighwayModelCreatePktsAndScale(FieldModel* model, u8* nextFree, s32 modelId) {
    FieldModelPart* parts;
    u32 i;

    model->partMatrices = (MATRIX*)nextFree;
    nextFree += model->boneCount * sizeof(MATRIX);
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    for (i = 0; i < model->partCount; i++) {
        nextFree = HighwayModelCreatePktsForPart(&parts[i], nextFree, 0, modelId);
    }
    HighwayModelScaleModel(model, model->scale, 0);
    return nextFree;
}

// Same as FieldModelCreatePktsForPart.
u8* HighwayModelCreatePktsForPart(FieldModelPart* part, u8* nextFree, s32 relocate, s32 modelId) {
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
