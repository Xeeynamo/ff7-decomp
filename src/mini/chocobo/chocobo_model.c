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

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AD9D8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AE534);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AE7D4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AF11C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AF9E4);

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

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AFDBC);

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
