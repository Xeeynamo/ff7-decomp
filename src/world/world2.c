//! PSYQ=3.3 CC1=2.6.3
#include "world.h"
#include <libetc.h>
#include <psxsdk/inline_c.h>

s32 WmCreatePacketForModelPart(FieldModelPart*, s32, s32, s32);
void WmScaleModelVertexes(FieldModelPart*, s16, s32);
void WmScaleModelAnimations(FieldModelAnimation*, s16, s32);
void WmScaleModelAll(FieldModelEntry*, s16, s32);
void WmApplyPolyLightingToPacket(FieldModelPart*, s32, s32, s32);

s32 WmLoadModelPacketAndScale(FieldModelEntry* model, s32 packet, s32 arg2) {
    FieldModelPart* parts;
    u32 i;

    model->partMatrices = (u8*)packet;
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

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmScaleModelAnimations);

s32 WmApplyModelLightingToPacket(FieldModelEntry* model, u8* light) {
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
    *(s32*)0x1F800200 = light[6];
    for (i = 0; i < count; i++) {
        WmApplyPolyLightingToPacket(&parts[i], r, g, b);
    }
    return 1;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmApplyPolyLightingToPacket);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmCalculateModelLighting);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmCalculatePartLighting);

void WmUpdatePartTransparency(FieldModelPart* part, s32 enable) {
    u8* p;
    u32 i;
    u32 buf;
    u32 count;

    for (buf = 0; buf < 2; buf++) {
        p = part->packets;
        if (buf != 0) {
            p += part->packetBufferSize;
        }
        count = part->polyGT4Count;
        for (i = 0; i < count; i++, p += sizeof(POLY_GT4)) {
            setSemiTrans((POLY_GT4*)p, enable);
            setShadeTex((POLY_GT4*)p, enable);
        }
        count = part->polyGT3Count;
        for (i = 0; i < count; i++, p += sizeof(POLY_GT3)) {
            setSemiTrans((POLY_GT3*)p, enable);
            setShadeTex((POLY_GT3*)p, enable);
        }
        count = part->polyFT4Count;
        for (i = 0; i < count; i++, p += sizeof(POLY_FT4)) {
            setSemiTrans((POLY_FT4*)p, enable);
            setShadeTex((POLY_FT4*)p, enable);
        }
        count = part->polyFT3Count;
        for (i = 0; i < count; i++, p += sizeof(POLY_FT3)) {
            setSemiTrans((POLY_FT3*)p, enable);
            setShadeTex((POLY_FT3*)p, enable);
        }
        count = part->polyF3Count;
        for (i = 0; i < count; i++, p += sizeof(POLY_F3)) {
            setSemiTrans((POLY_F3*)p, enable);
            setShadeTex((POLY_F3*)p, enable);
        }
        count = part->polyF4Count;
        for (i = 0; i < count; i++, p += sizeof(POLY_F4)) {
            setSemiTrans((POLY_F4*)p, enable);
            setShadeTex((POLY_F4*)p, enable);
        }
        count = part->polyG3Count;
        for (i = 0; i < count; i++, p += sizeof(POLY_G3)) {
            setSemiTrans((POLY_G3*)p, enable);
            setShadeTex((POLY_G3*)p, enable);
        }
        count = part->polyG4Count;
        for (i = 0; i < count; i++, p += sizeof(POLY_G4)) {
            setSemiTrans((POLY_G4*)p, enable);
            setShadeTex((POLY_G4*)p, enable);
        }
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdateModelPacket);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdatePacketForModelPartWithMatrixes);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdateModelLighting);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdatePartLighting);

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
