//! PSYQ=3.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

typedef struct {
    u32 size;
    u32 sectionOffset;
} FieldBsxFile;

typedef struct {
    u32 unk0;
    u32 modelCount;
    u32 texturesOffset;
    u32 workspaceEndOffset;
} FieldBsxSection;

typedef struct {
    u32 size;
    u8 textureCount;
    u8 tdbOffsetHigh;
    u16 tdbOffsetLow;
} FieldBsxTextures;

typedef struct {
    u16 width;
    u16 height;
    u16 x;
    u16 y;
    u32 dataOffset;
} FieldBsxTexture;

typedef struct {
    u16 modelId;
    u16 scale;
    u32 dataOffset;
    u8 ambientR;
    u8 ambientG;
    u8 ambientB;
    u8 unkB;
    u16 light1X;
    u16 light1Y;
    u16 light1Z;
    s8 boneStart;
    u8 boneEnd;
    u8 light1R;
    u8 light1G;
    u8 light1B;
    u8 boneCount;
    u16 light2X;
    u16 light2Y;
    u16 light2Z;
    s8 partStart;
    u8 partsEnd;
    u8 light2R;
    u8 light2G;
    u8 light2B;
    u8 partCount;
    u16 light3X;
    u16 light3Y;
    u16 light3Z;
    s8 animationStart;
    u8 animationEnd;
    u8 light3R;
    u8 light3G;
    u8 light3B;
    u8 animationCount;
} FieldBsxModel;

typedef struct {
    u32 type;
    u32 sourceOffset;
    u32 size;
    union {
        void* destination;
        u32 index;
        RECT rect;
    } target;
} FieldTdbEntry;

typedef struct {
    s32 count;
    u32 unk4;
} FieldTdbHeader;

extern u8* D_800E0200;

u8* FieldModelCreatePktsAndScale(FieldModelEntry* model, u8* nextFree, s32 modelId);
u8* FieldModelCreatePktsForPart(FieldModelPart* part, u8* nextFree, s32 relocate, s32 modelId);
void FieldModelLoadBsxTexToVram(FieldBsxTextures* textures);
void FieldModelBsxTdbModify(void* data);
void FieldModelScaleModel(FieldModelEntry* model, s16 scale, s32 force);
void FieldModelScalePartVrtxs(FieldModelPart* part, s16 scale, s32 force);
void FieldModelScaleAnimTranslat(FieldModelAnimation* animation, s16 scale, s32 force);
s32 KawaiLightingApplyToModel(FieldModelEntry* model, u8* params);
u8* FieldModelLoadBcx(FieldModelLoaderHeader*, FieldModelData*, u8*, u32);

u8* LoadLocalFieldModelAndInitAll(
    FieldModelLoaderHeader* header, FieldModelData* models, const u32* options, void* data) {
    MATRIX identity;
    RECT rect;
    FieldModelLoaderData* loader = (FieldModelLoaderData*)(header + 1);
    u8* workspaceEnd;
    Yamada* fileInfo;
    FieldBsxFile* file;
    FieldBsxSection* section;
    FieldBsxTextures* textures;
    FieldBsxModel* model;
    FieldBsxModel* copyModelSrc;
    FieldBsxModel* copyModelDst;
    FieldModelEntry* entry;
    u32* src;
    u32* copySrc;
    u32* copyDst;
    u32 byteSize;
    u32* bones;
    // Used for optimized struct copying.
    u32(*parts)[8];
    u32(*anims)[4];
    u8* nextFree;
    u8* params = (u8*)getScratchAddr(0);
    u8* bufferIndex;
    u32 modelCount;
    s32 i, j, k;
    u32 count;
    s32 modelId;
    s32 words;
    s32 wholeWords;
    s32 blocks;
    u32 relocation;
    s32 loaderAddress;

    fileInfo = *(Yamada**)params;
    file = data;
    if (*(u8*)options) {
        SysCdromStartLoadLzs(fileInfo->loc, fileInfo->len, (u_long*)file, NULL);
        while (SystemCdromReadChain()) {
        }
    } else {
        copySrc = (u32*)file;
        byteSize = file->size;
        copyDst = (u32*)D_800E0204;
        wholeWords = byteSize >> 2;
        byteSize &= 3;
        words = wholeWords + (byteSize != 0);
        blocks = words / 4;
        for (k = 0; k < blocks; k++) {
            copyDst[0] = copySrc[0];
            copyDst[1] = copySrc[1];
            copyDst[2] = copySrc[2];
            copyDst[3] = copySrc[3];
            copyDst += 4;
            copySrc += 4;
        }
        for (k = blocks * 4; k < words; k++) {
            *copyDst++ = *copySrc++;
        }
        file = (FieldBsxFile*)D_800E0204;
    }
    section = (FieldBsxSection*)((u8*)file + file->sectionOffset);
    textures = (FieldBsxTextures*)((u8*)section + section->texturesOffset);
    if (((u32*)textures)[1] & 0xFFFFFF00) {
        FieldModelBsxTdbModify((u8*)textures + ((textures->tdbOffsetHigh << 16) | textures->tdbOffsetLow));
    }
    FieldModelLoadBsxTexToVram(textures);
    DrawSync(0);

    relocation = (u32)file - 0x80000000;
    modelCount = section->modelCount;
    for (i = 0; i < modelCount; i++) {
        if (loader[i].npcFlag) {
            j = 0;
            model = &((FieldBsxModel*)(section + 1))[i];
            entry = &models->modelEntries[loader[i].modelEntryIndex];
            entry->scale = model->scale;
            src = (u32*)((u8*)model + model->dataOffset);
            count = model->boneCount;
            bones = (u32*)entry->modelData;
            for (; j < count; j++) {
                bones[model->boneStart + j] = *src++;
            }
            count = model->partCount;
            parts = (u32(*)[8])(entry->modelData + entry->partsOffset);
            for (j = 0; j < count; j++) {
                parts[model->partStart + j][0] = src[0];
                parts[model->partStart + j][1] = src[1];
                parts[model->partStart + j][2] = src[2];
                parts[model->partStart + j][3] = src[3];
                parts[model->partStart + j][4] = src[4];
                parts[model->partStart + j][5] = src[5];
                parts[model->partStart + j][6] = src[6];
                parts[model->partStart + j][7] = src[7];
                parts[model->partStart + j][6] = relocation + src[6];
                src += 8;
            }
            count = model->animationCount;
            anims = (u32(*)[4])(entry->modelData + entry->animationOffset);
            for (j = 0; j < count; j++) {
                anims[model->animationStart + j][0] = src[0];
                anims[model->animationStart + j][1] = src[1];
                anims[model->animationStart + j][2] = src[2];
                anims[model->animationStart + j][3] = src[3];
                anims[model->animationStart + j][3] = relocation + src[3];
                src += 4;
            }
        }
    }

    workspaceEnd = (u8*)section + section->workspaceEndOffset;
    copyModelSrc = (FieldBsxModel*)(section + 1);
    copyModelDst = (FieldBsxModel*)workspaceEnd;
    for (i = 0; i < modelCount; i++) {
        u32* modelSrc = (u32*)&copyModelSrc[i];
        u32* modelDst =(u32*)&copyModelDst[i];
        modelDst[0] = modelSrc[0];
        modelDst[1] = modelSrc[1];
        modelDst[2] = modelSrc[2];
        modelDst[3] = modelSrc[3];
        modelDst[4] = modelSrc[4];
        modelDst[5] = modelSrc[5];
        modelDst[6] = modelSrc[6];
        modelDst[7] = modelSrc[7];
        modelDst[8] = modelSrc[8];
        modelDst[9] = modelSrc[9];
        modelDst[10] = modelSrc[10];
        modelDst[11] = modelSrc[11];
    }

    nextFree = (u8*)section;
    for (i = 0; i < modelCount; i++) {
        FieldModelLoaderData* entry = loader;
        model = &((FieldBsxModel*)workspaceEnd)[i];

        loaderAddress = (s32)entry;
        entry = (FieldModelLoaderData*)(i * sizeof(FieldModelLoaderData) + loaderAddress);
        if (entry->npcFlag) {
            modelId = entry->modelEntryIndex;
            nextFree = FieldModelCreatePktsAndScale(&models->modelEntries[modelId], nextFree, modelId);
            if (models->modelEntries[modelId].textureFaceId < 33) {
                setRECT(&rect, 320, 480 + modelId, 16, 1);
                LoadImage(&rect, (u_long*)((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->palettesOffset +
                                           models->modelEntries[modelId].textureFaceId * 32));
                params[0] = 0;
                params[1] = 0;
                params[2] = 0;
                params[3] = modelId;
                KawaiLoadEyesMouthTexToVram(&models->modelEntries[modelId], params);
            }
            identity.m[0][0] = identity.m[1][1] = identity.m[2][2] = 4096;
            identity.m[0][1] = identity.m[0][2] = identity.m[1][0] = identity.m[1][2] = identity.m[2][0] =
                identity.m[2][1] = identity.t[0] = identity.t[1] = identity.t[2] = 0;
            *(u32*)getScratchAddr(0) = 1;
            FieldModelAnimCalcMtrxs(&models->modelEntries[modelId], &identity, 0, 0);
            params[0] = model->ambientR;
            params[1] = model->ambientG;
            params[2] = model->ambientB;
            params[3] = model->light1R;
            params[4] = model->light1G;
            params[5] = model->light1B;
            params[12] = model->light1X;
            params[13] = model->light1X >> 8;
            params[14] = model->light1Y;
            params[15] = model->light1Y >> 8;
            params[16] = model->light1Z;
            params[17] = model->light1Z >> 8;
            params[6] = model->light2R;
            params[7] = model->light2G;
            params[8] = model->light2B;
            params[18] = model->light2X;
            params[19] = model->light2X >> 8;
            params[20] = model->light2Y;
            params[21] = model->light2Y >> 8;
            params[22] = model->light2Z;
            params[23] = model->light2Z >> 8;
            params[9] = model->light3R;
            params[10] = model->light3G;
            params[11] = model->light3B;
            params[24] = model->light3X;
            params[25] = model->light3X >> 8;
            params[26] = model->light3Y;
            params[27] = model->light3Y >> 8;
            params[28] = model->light3Z;
            params[29] = model->light3Z >> 8;
            params[30] = 0;
            KawaiLightingApplyToModel(&models->modelEntries[modelId], params);
            params[0] = 0;
            params[1] = 0;
            params[2] = 0;
            params[3] = 0;
            params[4] = 0;
            params[5] = 0;
            params[6] = 1;
            KawaiSetColorToModelPkts(&models->modelEntries[modelId], params);
            bufferIndex = &g_FieldPrimitiveBufferIndex;
            *bufferIndex ^= 1;
            params[0] = 0;
            params[1] = 0;
            params[2] = 0;
            params[3] = 0;
            params[4] = 0;
            params[5] = 0;
            params[6] = 1;
            KawaiSetColorToModelPkts(&models->modelEntries[modelId], params);
            *bufferIndex ^= 1;
        }
    }

    D_800E0200 = workspaceEnd;
    return workspaceEnd;
}

u8* FieldModelCreatePktsAndScale(FieldModelEntry* model, u8* nextFree, s32 modelId) {
    FieldModelPart* parts;
    u32 i;

    model->partMatrices = nextFree;
    nextFree += model->boneCount * sizeof(MATRIX);
    parts = (FieldModelPart*)(model->modelData + model->partsOffset);
    for (i = 0; i < model->partCount; i++) {
        nextFree = FieldModelCreatePktsForPart(&parts[i], nextFree, 0, modelId);
    }
    FieldModelScaleModel(model, model->scale, 0);
    return nextFree;
}

u8* FieldModelCreatePktsForPart(FieldModelPart* part, u8* nextFree, s32 relocate, s32 modelId) {
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

void FieldModelLoadBsxTexToVram(FieldBsxTextures* textures) {
    RECT rect;
    u32 i;
    u32 count = textures->textureCount;
    FieldBsxTexture* texture = (FieldBsxTexture*)(textures + 1);

    for (i = 0; i < count; i++) {
        setRECT(&rect, texture[i].x, texture[i].y, texture[i].width, texture[i].height);
        LoadImage(&rect, (u_long*)((u8*)textures + texture[i].dataOffset));
    }
}

void FieldModelBsxTdbModify(void* data) {
    FieldTdbHeader* header = data;
    FieldTdbEntry* entries;
    s32 count;
    s32 i;

    if (header && (count = header->count)) {
        entries = (FieldTdbEntry*)(header + 1);
        for (i = 0; i < count; i++) {
            switch (entries[i].type) {
            case 0:
                memcpy(entries[i].target.destination, (u8*)data + entries[i].sourceOffset, entries[i].size);
                break;
            case 1:
                if (entries[i].target.index < g_FieldFaceTextureData->textureCount) {
                    memcpy((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->texturesOffset +
                               entries[i].target.index * 512,
                           (u8*)data + entries[i].sourceOffset, 512);
                }
                break;
            case 2:
                if (entries[i].target.index < g_FieldFaceTextureData->paletteCount) {
                    memcpy((u8*)g_FieldFaceTextureData + g_FieldFaceTextureData->palettesOffset +
                               entries[i].target.index * 32,
                           (u8*)data + entries[i].sourceOffset, 32);
                }
                break;
            case 3:
                LoadImage(&entries[i].target.rect, (u_long*)((u8*)data + entries[i].sourceOffset));
                break;
            }
        }
    }
}

u8* FieldModelStructInit(FieldModelLoaderHeader* header, FieldModelData* modelData) {
    FieldModelLoaderData* loaders;
    FieldModelEntry* entry;
    u8* buffer;
    s16 partsOffset;
    u32 i;

    buffer = (u8*)modelData;
    modelData->modelCount = 0;
    loaders = (FieldModelLoaderData*)(header + 1);
    for (i = 0; i < header->modelCount; i++) {
        if (loaders[i].npcFlag) {
            loaders[i].modelEntryIndex = modelData->modelCount;
            modelData->modelCount++;
        } else {
            loaders[i].modelEntryIndex = 0xFF;
        }
    }

    modelData->unk2 = 0;
    modelData->unk1 = 0;
    modelData->modelEntries = (FieldModelEntry*)(modelData + 1);
    modelData->unk8 = NULL;
    buffer += sizeof(FieldModelData) + modelData->modelCount * sizeof(FieldModelEntry);
    for (i = 0; i < header->modelCount; i++) {
        if (loaders[i].npcFlag) {
            if (loaders[i].globalModelId > 0 && loaders[i].globalModelId < 10 && loaders[i].animationCount < 3) {
                loaders[i].animationCount = 3;
            }
            entry = &modelData->modelEntries[loaders[i].modelEntryIndex];
            entry->flags = 1;
            entry->kawaiType = -1;
            entry->boneCount = loaders[i].boneCount;
            entry->partCount = loaders[i].partCount;
            entry->animationCount = loaders[i].animationCount;
            entry->rotationZ = 0;
            entry->rotationY = 0;
            entry->rotationX = 0;
            entry->translationZ = 0;
            entry->translationY = 0;
            entry->translationX = 0;
            entry->globalModelId = loaders[i].globalModelId;
            entry->textureFaceId = loaders[i].faceId;
            entry->scale = 4096;
            partsOffset = loaders[i].boneCount * 4;
            entry->partsOffset = partsOffset;
            entry->animationOffset = partsOffset + (loaders[i].partCount << 5);
            entry->modelData = buffer;
            entry->partMatrices = NULL;
            buffer += loaders[i].boneCount * 4 + (loaders[i].partCount << 5) + loaders[i].animationCount * 16;
        }
    }
    D_800E0204 = 0;
    return buffer;
}

u8* FieldModelLoadGlobalModels(
    FieldModelLoaderHeader* header, FieldModelData* modelData, u8* buffer, s32 loadTextures) {
    FieldModelLzsRequest* request;
    s32 savedScratch;
    u32 i;

    savedScratch = *(u_long*)getScratchAddr(0);
    request = *(FieldModelLzsRequest**)getScratchAddr(1);
    for (i = 0; i < header->modelCount; i++) {
        *(u_long*)0x1F800000 = savedScratch;
        buffer = FieldModelLoadBcx(header, modelData, buffer, i);
    }
    if (loadTextures) {
        SysCdromStartLoadLzs(request->sector, request->size, (u_long*)g_FieldFaceTextureData, NULL);
        while (SystemCdromReadChain()) {
        }
    }
    return buffer;
}

u8* FieldModelLoadBcx(FieldModelLoaderHeader* header, FieldModelData* models, u8* nextFree, u32 modelIndex) {
    FieldModelLoaderData* loader = (FieldModelLoaderData*)(header + 1);
    FieldModelLoaderData* entry = (FieldModelLoaderData*)(modelIndex * sizeof(FieldModelLoaderData) + (u32)loader);
    Yamada* fileInfo = *(Yamada**)getScratchAddr(0);
    s32 globalModelId;
    u32 relocation;
    u32 i, j;
    u32 count;
    u32 copyCount;
    u32 existingIndex;
    FieldModelEntry* source;
    FieldModelEntry* model;
    u32* sourceBones;
    u32* bones;
    // Used for optimized struct copying.
    u32(*partSrc)[8];
    u32(*partDst)[8];
    u32(*animationSrc)[4];
    u32(*animationDst)[4];

    if (entry->npcFlag) {
        globalModelId = (s8)entry->globalModelId;
        if ((u32)(globalModelId - 1) < 9) {
            if (!entry->globalModelLoaded) {
                switch (globalModelId) {
                case 1:
                    SysCdromStartLoadLzs(fileInfo[0].loc, fileInfo[0].len, (u_long*)nextFree, NULL);
                    break;
                case 2:
                    SysCdromStartLoadLzs(fileInfo[1].loc, fileInfo[1].len, (u_long*)nextFree, NULL);
                    break;
                case 3:
                    SysCdromStartLoadLzs(fileInfo[2].loc, fileInfo[2].len, (u_long*)nextFree, NULL);
                    break;
                case 4:
                    SysCdromStartLoadLzs(fileInfo[3].loc, fileInfo[3].len, (u_long*)nextFree, NULL);
                    break;
                case 5:
                    SysCdromStartLoadLzs(fileInfo[4].loc, fileInfo[4].len, (u_long*)nextFree, NULL);
                    break;
                case 6:
                    SysCdromStartLoadLzs(fileInfo[5].loc, fileInfo[5].len, (u_long*)nextFree, NULL);
                    break;
                case 7:
                    SysCdromStartLoadLzs(fileInfo[6].loc, fileInfo[6].len, (u_long*)nextFree, NULL);
                    break;
                case 8:
                    SysCdromStartLoadLzs(fileInfo[7].loc, fileInfo[7].len, (u_long*)nextFree, NULL);
                    break;
                case 9:
                    SysCdromStartLoadLzs(fileInfo[8].loc, fileInfo[8].len, (u_long*)nextFree, NULL);
                    break;
                }
                while (SystemCdromReadChain()) {
                }
                for (i = 0; i < header->modelCount; i++) {
                    if ((s8)loader[i].globalModelId == globalModelId) {
                        loader[i].globalModelLoaded = 1;
                    }
                }
                model = &models->modelEntries[loader[modelIndex].modelEntryIndex];
                source = (FieldModelEntry*)(nextFree + ((FieldBsxFile*)nextFree)->sectionOffset);
                relocation = (u32)nextFree - 0x80000000;
                count = source->boneCount;
                source->modelData = (u8*)(relocation + (u32)source->modelData);
                sourceBones = (u32*)source->modelData;
                bones = (u32*)model->modelData;
                for (i = 0; i < count; i++) {
                    bones[i] = sourceBones[i];
                }
                count = source->partCount;
                partDst = (u32(*)[8])(model->modelData + model->partsOffset);
                partSrc = (u32(*)[8])(source->modelData + source->partsOffset);
                for (i = 0; i < count; i++) {
                    partDst[i][0] = partSrc[i][0];
                    partDst[i][1] = partSrc[i][1];
                    partDst[i][2] = partSrc[i][2];
                    partDst[i][3] = partSrc[i][3];
                    partDst[i][4] = partSrc[i][4];
                    partDst[i][5] = partSrc[i][5];
                    partDst[i][6] = partSrc[i][6];
                    partDst[i][7] = partSrc[i][7];
                    
                    partDst[i][6] = relocation + partSrc[i][6];
                }
                count = source->animationCount;
                animationDst = (u32(*)[4])(model->modelData + model->animationOffset);
                animationSrc = (u32(*)[4])(source->modelData + source->animationOffset);
                for (i = 0; i < count; i++) {
                    animationDst[i][0] = animationSrc[i][0];
                    animationDst[i][1] = animationSrc[i][1];
                    animationDst[i][2] = animationSrc[i][2];
                    animationDst[i][3] = animationSrc[i][3];
                    
                    animationDst[i][3] = relocation + animationSrc[i][3];
                }
                D_800E0204 = (u8*)source;
                return (u8*)source;
            }
            for (existingIndex = 0; existingIndex < modelIndex; existingIndex++) {
                if ((s8)loader[existingIndex].globalModelId == globalModelId) {
                    model = &models->modelEntries[loader[modelIndex].modelEntryIndex];
                    source = &models->modelEntries[existingIndex];
                    bones = (u32*)model->modelData;
                    copyCount = source->boneCount;
                    sourceBones = (u32*)source->modelData;
                    for (j = 0; j < copyCount; j++) {
                        bones[j] = sourceBones[j];
                    }
                    copyCount = source->partCount;
                    partDst = (u32(*)[8])(model->modelData + model->partsOffset);
                    partSrc = (u32(*)[8])(source->modelData + source->partsOffset);
                    for (j = 0; j < copyCount; j++) {
                        partDst[j][0] = partSrc[j][0];
                        partDst[j][1] = partSrc[j][1];
                        partDst[j][2] = partSrc[j][2];
                        partDst[j][3] = partSrc[j][3];
                        partDst[j][4] = partSrc[j][4];
                        partDst[j][5] = partSrc[j][5];
                        partDst[j][6] = partSrc[j][6];
                        partDst[j][7] = partSrc[j][7];
                    }
                    copyCount = source->animationCount;
                    animationDst = (u32(*)[4])(model->modelData + model->animationOffset);
                    animationSrc = (u32(*)[4])(source->modelData + source->animationOffset);
                    for (j = 0; j < copyCount; j++) {
                        animationDst[j][0] = animationSrc[j][0];
                        animationDst[j][1] = animationSrc[j][1];
                        animationDst[j][2] = animationSrc[j][2];
                        animationDst[j][3] = animationSrc[j][3];
                    }
                    break;
                }
            }
            D_800E0204 = nextFree;
        }
    }
    return nextFree;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelPrepareRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelAddToRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field_model", FieldModelAnimCalcMtrxs);

void FieldModelScaleModel(FieldModelEntry* model, s16 scale, s32 force) {
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
        FieldModelScalePartVrtxs(&parts[i], scale, force);
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
        FieldModelScaleAnimTranslat(&animations[i], scale, force);
    }
}

void FieldModelScalePartVrtxs(FieldModelPart* part, s16 scale, s32 force) {
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

void FieldModelScaleAnimTranslat(FieldModelAnimation* animation, s16 scale, s32 force) {
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
