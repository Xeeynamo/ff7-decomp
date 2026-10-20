//! PSYQ=3.3
#include "chocobo_private.h"
#include <gte.h>
#include <libetc.h>
#include <libgte.h>
#include <psxsdk/inline_c.h>
#include "sincos.h"

u8* ChocoboModelSetupParts(FieldModel* model, u8* nextFree, s32 arg2) {
    FieldModelPart* parts;
    u32 i;

    model->partMatrices = (MATRIX*)nextFree;
    nextFree += model->boneCount * sizeof(MATRIX);
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    for (i = 0; i < model->partCount; i++) {
        nextFree = func_800AD9D8(&parts[i], nextFree, 0, arg2);
    }
    func_800AF9E4(model, model->scale, 0);
    return nextFree;
}

u8* func_800AD9D8(FieldModelPart* part, u8* nextFree, s32 relocate, s32 modelId) {
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

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AE534);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AE7D4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AF11C);

void func_800AF9E4(FieldModel* model, s16 scale, s32 force) {
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
        ChocoboScalePartVerts(&parts[i], scale, force);
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
        func_800AFDBC(&animations[i], scale, force);
    }
}

void ChocoboScalePartVerts(FieldModelPart* part, s16 scale, s32 force) {
    MATRIX* m;
    s16* out;
    SVECTOR* verts;
    u32 i;
    u32 n;

    m = (MATRIX*)getScratchAddr(0);
    out = (s16*)(m + 1);
    if ((*(u32*)part->data & 1) && !force) {
        return;
    }
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
    verts = (SVECTOR*)(part->data + sizeof(u32));
    n = part->vertexCount;
    for (i = 0; i < n; i++) {
        gte_ldv0(&verts[i]);
        gte_rt();
        gte_stlvnl(out);
        verts[i].vx = out[0];
        verts[i].vy = out[2];
        verts[i].vz = out[4];
    }
    *(u32*)part->data |= 1;
}

void func_800AFDBC(FieldModelAnimation* animation, s16 scale, s32 force) {
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

s32 ChocoboModelApplyPartRotation(FieldModel* model, u8* data) {
    FieldModelPart* parts;
    u32 i;
    u32 n;
    s16 x;
    s16 y;
    s16 z;

    n = model->partCount;
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    x = KAWAI_READ_S16(data, 0);
    y = KAWAI_READ_S16(data, 2);
    z = KAWAI_READ_S16(data, 4);
    *(u_long*)getScratchAddr(128) = data[6];
    for (i = 0; i < n; i++) {
        func_800B01B0(&parts[i], x, y, z);
    }
    return 1;
}

void func_800B01B0(FieldModelPart* part, s16 r, s16 g, s16 b) {
    u8* packets;
    u8* polygons;
    u32 counts;
    u32 count;
    u32 i;
    u8 code;
    s32 rOffset, gOffset, bOffset;

    packets = part->packets;
    polygons = part->data + part->polygonsOffset;
    if (D_800B66CC.index) {
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

s32 func_800B0E7C(FieldModel* model, u8* params) {
    FieldModelPart* parts;
    FieldModelPart* part;
    u8* packets;
    u32 i, j, k;
    u32 count, enabled;

    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    enabled = params[0];
    for (i = 0; i < model->partCount; i++) {
        part = &parts[i];
        for (j = 0; j < 2; j++) {
            packets = part->packets;
            if (j) {
                packets += part->packetBufferSize;
            }

            count = part->polyGT4Count;
            for (k = 0; k < count; packets += sizeof(POLY_GT4), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyGT3Count;
            for (k = 0; k < count; packets += sizeof(POLY_GT3), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyFT4Count;
            for (k = 0; k < count; packets += sizeof(POLY_FT4), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyFT3Count;
            for (k = 0; k < count; packets += sizeof(POLY_FT3), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyF3Count;
            for (k = 0; k < count; packets += sizeof(POLY_F3), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyF4Count;
            for (k = 0; k < count; packets += sizeof(POLY_F4), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyG3Count;
            for (k = 0; k < count; packets += sizeof(POLY_G3), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }

            count = part->polyG4Count;
            for (k = 0; k < count; packets += sizeof(POLY_G4), k++) {
                setSemiTrans(packets, enabled);
                setShadeTex(packets, enabled);
            }
        }
    }
    return 1;
}
