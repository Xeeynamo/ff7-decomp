//! PSYQ=3.3

#include <game.h>

#include <psxsdk/inline_c.h>

u8* HighwayModelCreatePktsForPart(FieldModelPart* part, u8* nextFree, s32 relocate, s32 modelId);
void HighwayModelScaleModel(FieldModelEntry* model, s16 scale, s32 force);

// Same as FieldModelCreatePktsAndScale.
u8* HighwayModelCreatePktsAndScale(FieldModelEntry* model, u8* nextFree, s32 modelId) {
    FieldModelPart* parts;
    u32 i;

    model->partMatrices = nextFree;
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
    u32* polygon;
    u8* cursor;
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
    POLY_GT4* polyGT4;
    POLY_GT3* polyGT3;
    POLY_FT4* polyFT4;
    POLY_FT3* polyFT3;
    POLY_F3* polyF3;
    POLY_F4* polyF4;
    POLY_G3* polyG3;
    POLY_G4* polyG4;

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
        cursor = nextFree;
        textureFlags = part->data + part->textureFlagsOffset;
        if (buffer) {
            cursor += part->packetBufferSize;
        }
        polygon = (u32*)(part->data + part->polygonsOffset);
        count = part->polyGT4Count;
        for (i = 0; i < count; i++, cursor += sizeof(POLY_GT4), polygon += 6) {
            u32 uvIndices;
            u32 texture;
            u32 mode;
            s32 paletteOffset;
            u8 flags;

            polyGT4 = (POLY_GT4*)cursor;
            *(u32*)&polyGT4->r0 = polygon[1];
            *(u32*)&polyGT4->r1 = polygon[2];
            *(u32*)&polyGT4->r2 = polygon[3];
            *(u32*)&polyGT4->r3 = polygon[4];
            uvIndices = polygon[5];
            *(u16*)&polyGT4->u0 = texCoords[uvIndices & 0xFF],
            *(u16*)&polyGT4->u1 = texCoords[(uvIndices & 0xFF00) >> 8],
            *(u16*)&polyGT4->u2 = texCoords[(uvIndices & 0xFF0000) >> 16],
            *(u16*)&polyGT4->u3 = texCoords[uvIndices >> 24];
            flags = *textureFlags++;
            texture = textures[flags & 0xF];
            paletteOffset = (texture & 0x3F) == 2 ? 0 : modelId;
            polyGT4->clut = getClut(((texture >> 16) & 0x3F) * 16, ((texture & 0x7FC00000) >> 22) + paletteOffset);
            polyGT4->tpage =
                getTPage((texture & 0xC0) >> 6, flags >> 5, (texture & 0xF00) >> 2, ((texture >> 12) & 0x1) * 256);
            mode = texture & 0x3F;
            if (mode == 0) {
                uOffset = uOffset4;
                vOffset = vOffset4;
            } else if (mode == 1) {
                uOffset = uOffset8;
                vOffset = vOffset8;
            } else {
                vOffset = 0;
                uOffset = 0;
            }
            setPolyGT4(polyGT4);
            polyGT4->u0 += uOffset;
            polyGT4->v0 += vOffset;
            polyGT4->u1 += uOffset;
            polyGT4->v1 += vOffset;
            polyGT4->u2 += uOffset;
            polyGT4->v2 += vOffset;
            polyGT4->u3 += uOffset;
            polyGT4->v3 += vOffset;
            if (flags & 0x10) {
                setcode(polyGT4, 62);
            }
        }
        count = part->polyGT3Count;
        for (i = 0; i < count; i++, cursor += sizeof(POLY_GT3), polygon += 5) {
            u32 uvIndices;
            u32 texture;
            u32 mode;
            s32 paletteOffset;
            u8 flags;

            polyGT3 = (POLY_GT3*)cursor;
            *(u32*)&polyGT3->r0 = polygon[1];
            *(u32*)&polyGT3->r1 = polygon[2];
            *(u32*)&polyGT3->r2 = polygon[3];
            uvIndices = polygon[4];
            *(u16*)&polyGT3->u0 = texCoords[uvIndices & 0xFF],
            *(u16*)&polyGT3->u1 = texCoords[(uvIndices & 0xFF00) >> 8],
            *(u16*)&polyGT3->u2 = texCoords[(uvIndices & 0xFF0000) >> 16];
            flags = *textureFlags++;
            texture = textures[flags & 0xF];
            paletteOffset = (texture & 0x3F) == 2 ? 0 : modelId;
            polyGT3->clut = getClut(((texture >> 16) & 0x3F) * 16, ((texture & 0x7FC00000) >> 22) + paletteOffset);
            polyGT3->tpage =
                getTPage((texture & 0xC0) >> 6, flags >> 5, (texture & 0xF00) >> 2, ((texture >> 12) & 0x1) * 256);
            mode = texture & 0x3F;
            if (mode == 0) {
                uOffset = uOffset4;
                vOffset = vOffset4;
            } else if (mode == 1) {
                uOffset = uOffset8;
                vOffset = vOffset8;
            } else {
                vOffset = 0;
                uOffset = 0;
            }
            setPolyGT3(polyGT3);
            polyGT3->u0 += uOffset;
            polyGT3->v0 += vOffset;
            polyGT3->u1 += uOffset;
            polyGT3->v1 += vOffset;
            polyGT3->u2 += uOffset;
            polyGT3->v2 += vOffset;
            if (flags & 0x10) {
                setcode(polyGT3, 54);
            }
        }
        count = part->polyFT4Count;
        for (i = 0; i < count; i++, cursor += sizeof(POLY_FT4), polygon += 3) {
            u32 uvIndices;
            u32 texture;
            u32 mode;
            s32 paletteOffset;
            u8 flags;

            polyFT4 = (POLY_FT4*)cursor;
            *(u32*)&polyFT4->r0 = polygon[1];
            uvIndices = polygon[2];
            *(u16*)&polyFT4->u0 = texCoords[uvIndices & 0xFF],
            *(u16*)&polyFT4->u1 = texCoords[(uvIndices & 0xFF00) >> 8],
            *(u16*)&polyFT4->u2 = texCoords[(uvIndices & 0xFF0000) >> 16],
            *(u16*)&polyFT4->u3 = texCoords[uvIndices >> 24];
            flags = *textureFlags++;
            texture = textures[flags & 0xF];
            paletteOffset = (texture & 0x3F) == 2 ? 0 : modelId;
            polyFT4->clut = getClut(((texture >> 16) & 0x3F) * 16, ((texture & 0x7FC00000) >> 22) + paletteOffset);
            polyFT4->tpage =
                getTPage((texture & 0xC0) >> 6, flags >> 5, (texture & 0xF00) >> 2, ((texture >> 12) & 0x1) * 256);
            mode = texture & 0x3F;
            if (mode == 0) {
                uOffset = uOffset4;
                vOffset = vOffset4;
            } else if (mode == 1) {
                uOffset = uOffset8;
                vOffset = vOffset8;
            } else {
                vOffset = 0;
                uOffset = 0;
            }
            setPolyFT4(polyFT4);
            polyFT4->u0 += uOffset;
            polyFT4->v0 += vOffset;
            polyFT4->u1 += uOffset;
            polyFT4->v1 += vOffset;
            polyFT4->u2 += uOffset;
            polyFT4->v2 += vOffset;
            polyFT4->u3 += uOffset;
            polyFT4->v3 += vOffset;
            if (flags & 0x10) {
                setcode(polyFT4, 46);
            }
        }
        count = part->polyFT3Count;
        for (i = 0; i < count; i++, cursor += sizeof(POLY_FT3), polygon += 3) {
            u32 uvIndices;
            u32 texture;
            u32 mode;
            s32 paletteOffset;
            u8 flags;

            polyFT3 = (POLY_FT3*)cursor;
            *(u32*)&polyFT3->r0 = polygon[1];
            uvIndices = polygon[2];
            *(u16*)&polyFT3->u0 = texCoords[uvIndices & 0xFF],
            *(u16*)&polyFT3->u1 = texCoords[(uvIndices & 0xFF00) >> 8],
            *(u16*)&polyFT3->u2 = texCoords[(uvIndices & 0xFF0000) >> 16];
            flags = *textureFlags++;
            texture = textures[flags & 0xF];
            paletteOffset = (texture & 0x3F) == 2 ? 0 : modelId;
            polyFT3->clut = getClut(((texture >> 16) & 0x3F) * 16, ((texture & 0x7FC00000) >> 22) + paletteOffset);
            polyFT3->tpage =
                getTPage((texture & 0xC0) >> 6, flags >> 5, (texture & 0xF00) >> 2, ((texture >> 12) & 0x1) * 256);
            mode = texture & 0x3F;
            if (mode == 0) {
                uOffset = uOffset4;
                vOffset = vOffset4;
            } else if (mode == 1) {
                uOffset = uOffset8;
                vOffset = vOffset8;
            } else {
                vOffset = 0;
                uOffset = 0;
            }
            setPolyFT3(polyFT3);
            polyFT3->u0 += uOffset;
            polyFT3->v0 += vOffset;
            polyFT3->u1 += uOffset;
            polyFT3->v1 += vOffset;
            polyFT3->u2 += uOffset;
            polyFT3->v2 += vOffset;
            if (flags & 0x10) {
                setcode(polyFT3, 38);
            }
        }
        count = part->polyF3Count;
        for (i = 0; i < count; i++, cursor += sizeof(POLY_F3), polygon += 2) {
            polyF3 = (POLY_F3*)cursor;
            *(u32*)&polyF3->r0 = polygon[1];
            setPolyF3(polyF3);
        }
        count = part->polyF4Count;
        for (i = 0; i < count; i++, cursor += sizeof(POLY_F4), polygon += 2) {
            polyF4 = (POLY_F4*)cursor;
            *(u32*)&polyF4->r0 = polygon[1];
            setPolyF4(polyF4);
        }
        count = part->polyG3Count;
        for (i = 0; i < count; i++, cursor += sizeof(POLY_G3), polygon += 4) {
            polyG3 = (POLY_G3*)cursor;
            *(u32*)&polyG3->r0 = polygon[1];
            *(u32*)&polyG3->r1 = polygon[2];
            *(u32*)&polyG3->r2 = polygon[3];
            setPolyG3(polyG3);
        }
        count = part->polyG4Count;
        for (i = 0; i < count; i++, cursor += sizeof(POLY_G4), polygon += 5) {
            polyG4 = (POLY_G4*)cursor;
            *(u32*)&polyG4->r0 = polygon[1];
            *(u32*)&polyG4->r1 = polygon[2];
            *(u32*)&polyG4->r2 = polygon[3];
            *(u32*)&polyG4->r3 = polygon[4];
            setPolyG4(polyG4);
        }
    }
    return nextFree + part->packetBufferSize * 2;
}
