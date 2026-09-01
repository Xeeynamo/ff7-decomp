//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern FieldFileInfo g_FieldFileInfo[];
extern void SystemLzsDecompress(void* dst, void* src);
extern s32* g_FieldModelsP;
extern s32 g_FieldTriggers;
extern s32 g_FieldEncounters;
extern s32 D_8007E770;
extern s16 g_CurrentFieldIndex;
extern s32* g_FieldTriggersP;
extern s32* g_FieldEncountersP;
extern u32 g_FieldLzsInfo[];

/* .rodata 0x800A00E0-0x800A013C, a base of 0 mod 8, holding jtbl_800A00E0 and
 * KawaiExecute's jtbl_800A0104. Split out of field.c for the same reason as
 * field2.c: the table at 0xE0 is 0 mod 8 and the unit before it is not. */

/* Copy one global (BCX) model into place. `pkts` is the streaming buffer the
 * file lands in; the record it holds is linked against 0x80000000, so every
 * pointer inside it is rebased by (pkts - 0x80000000) on the way through.
 * When the model is already resident (globalLoaded set) the copy is made from
 * the earlier entry that carries the same globalModelId instead of from disk.
 */
u8* FieldModelLoadBcx(
    FieldModelFileDesc* desc, FieldModelData* data, u8* pkts, s32 index) {
    FieldModelLoaderData* models;
    u32* fileInfo;
    FieldModelEntry* src;
    FieldModelEntry* dst;
    s32* s;
    s32* d;
    s32* sp;
    s32* dp;
    s32* sa;
    s32* da;
    s32 fixup;
    s32 id;
    u32 count;
    u32 i;
    u32 count2;
    u32 i2;
    u32 j;

    models = desc->models;
    fileInfo = (u32*)((s32*)0x1F800000)[0];
    if (models[index].npcFlag == 0) {
        return pkts;
    }
    id = models[index].globalModelId;
    if (id >= 1 && id <= 9) {
        if (models[index].globalLoaded == 0) {
            switch (id) {
            case 1:
                DS_read(fileInfo[0], fileInfo[1], (u_long*)pkts, NULL);
                break;
            case 2:
                DS_read(fileInfo[2], fileInfo[3], (u_long*)pkts, NULL);
                break;
            case 3:
                DS_read(fileInfo[4], fileInfo[5], (u_long*)pkts, NULL);
                break;
            case 4:
                DS_read(fileInfo[6], fileInfo[7], (u_long*)pkts, NULL);
                break;
            case 5:
                DS_read(fileInfo[8], fileInfo[9], (u_long*)pkts, NULL);
                break;
            case 6:
                DS_read(fileInfo[10], fileInfo[11], (u_long*)pkts, NULL);
                break;
            case 7:
                DS_read(fileInfo[12], fileInfo[13], (u_long*)pkts, NULL);
                break;
            case 8:
                DS_read(fileInfo[14], fileInfo[15], (u_long*)pkts, NULL);
                break;
            case 9:
                DS_read(fileInfo[16], fileInfo[17], (u_long*)pkts, NULL);
                break;
            }
            while (SystemCdromReadChain() != 0) {
            }
            for (i = 0; i < desc->count; i++) {
                if (models[i].globalModelId == id) {
                    models[i].globalLoaded = 1;
                }
            }
            dst = &data->modelEntries[models[index].modelEntryIndex];
            src = (FieldModelEntry*)(pkts + ((u32*)pkts)[1]);
            fixup = (s32)pkts - 0x80000000;
            count = src->boneCount;
            src->modelData = (u8*)((s32)src->modelData + fixup);
            d = (s32*)dst->modelData;
            s = (s32*)src->modelData;
            for (i = 0; i < count; i++) {
                d[i] = s[i];
            }
            count = src->partCount;
            dp = (s32*)(dst->modelData + dst->partsOffset);
            sp = (s32*)(src->modelData + src->partsOffset);
            for (i = 0; i < count; i++) {
                dp[i * 8 + 0] = sp[i * 8 + 0];
                dp[i * 8 + 1] = sp[i * 8 + 1];
                dp[i * 8 + 2] = sp[i * 8 + 2];
                dp[i * 8 + 3] = sp[i * 8 + 3];
                dp[i * 8 + 4] = sp[i * 8 + 4];
                dp[i * 8 + 5] = sp[i * 8 + 5];
                dp[i * 8 + 6] = sp[i * 8 + 6];
                dp[i * 8 + 7] = sp[i * 8 + 7];
                dp[i * 8 + 6] = sp[i * 8 + 6] + fixup;
            }
            count = src->animationCount;
            da = (s32*)(dst->modelData + dst->animationOffset);
            sa = (s32*)(src->modelData + src->animationOffset);
            for (i = 0; i < count; i++) {
                da[i * 4 + 0] = sa[i * 4 + 0];
                da[i * 4 + 1] = sa[i * 4 + 1];
                da[i * 4 + 2] = sa[i * 4 + 2];
                da[i * 4 + 3] = sa[i * 4 + 3];
                da[i * 4 + 3] = sa[i * 4 + 3] + fixup;
            }
            D_800E0204 = (u8*)src;
            return (u8*)src;
        }
        for (j = 0; j < index; j++) {
            if (models[j].globalModelId == id) {
                dst = &data->modelEntries[models[index].modelEntryIndex];
                src = &data->modelEntries[j];

                d = (s32*)dst->modelData;
                count2 = src->boneCount;
                s = (s32*)src->modelData;
                for (i2 = 0; i2 < count2; i2++) {
                    d[i2] = s[i2];
                }
                count2 = src->partCount;
                dp = (s32*)(dst->modelData + dst->partsOffset);
                sp = (s32*)(src->modelData + src->partsOffset);
                for (i2 = 0; i2 < count2; i2++) {
                    dp[i2 * 8 + 0] = sp[i2 * 8 + 0];
                    dp[i2 * 8 + 1] = sp[i2 * 8 + 1];
                    dp[i2 * 8 + 2] = sp[i2 * 8 + 2];
                    dp[i2 * 8 + 3] = sp[i2 * 8 + 3];
                    dp[i2 * 8 + 4] = sp[i2 * 8 + 4];
                    dp[i2 * 8 + 5] = sp[i2 * 8 + 5];
                    dp[i2 * 8 + 6] = sp[i2 * 8 + 6];
                    dp[i2 * 8 + 7] = sp[i2 * 8 + 7];
                }
                count2 = src->animationCount;
                da = (s32*)(dst->modelData + dst->animationOffset);
                sa = (s32*)(src->modelData + src->animationOffset);
                for (i2 = 0; i2 < count2; i2++) {
                    da[i2 * 4 + 0] = sa[i2 * 4 + 0];
                    da[i2 * 4 + 1] = sa[i2 * 4 + 1];
                    da[i2 * 4 + 2] = sa[i2 * 4 + 2];
                    da[i2 * 4 + 3] = sa[i2 * 4 + 3];
                }
                break;
            }
        }
        D_800E0204 = pkts;
    }
    return pkts;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelPrepareRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelAddToRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelAnimCalcMtrxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelScaleModel);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelScalePartVrtxs);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", FieldModelScaleAnimTranslat);

/////////////////////////////////////////////////
// Begin of field_kawai_char_model.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_kawai_char_model.c
/////////////////////////////////////////////////

void KawaiClearData(void) {
    u8* p = D_800DFDFC;
    s32 count = 16;
    s32 i;

    p[0] = count;
    for (i = 0; i < count; i++) {
        p[i * 2 + 2] = 0;
        p[i * 2 + 3] = 0;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiExecute);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetCustomLightToModelPkts);

/* Apply the GTE lighting to each vertex colour of a model's *packets*: for
 * every polygon of every kind, run NormalColorColSingle on the vertex normal
 * the polygon table names and write the result straight into the packet's RGB.
 * The sibling KawaiLightingApplyToPolyColor below does the same GTE work in
 * place in the polygon table; this one is the pass that pushes the result out
 * to the GPU packets, so it walks two cursors at once. */
void KawaiSetVertexColorFromLighting(FieldModelPart* part) {
    u8* normals;
    u8* poly;
    u8* pkt;
    u8* c1;
    u8* c2;
    u8* c3;
    u8* c4;
    u8* rgb1;
    u8* rgb2;
    u8* rgb3;
    u8* rgb4;
    u8* rgbs1;
    u8* rgbs2;
    u8* rgbs3;
    u8* rgbs4;
    u8 code1;
    u8 code2;
    u8 code3;
    u8 code4;
    u8 codes1;
    u8 codes2;
    u8 codes3;
    u8 codes4;
    s32 off1;
    s32 off2;
    s32 off3;
    s32 off4;
    u32 counts;
    u32 count;
    u32 i;
    u32 k;

    poly = (u8*)(part->polyOffset + (u32)part->data);
    normals = (u8*)D_800DF520;
    pkt = part->pkts;
    if (D_800DF114 != 0) {
        pkt += part->pktSize;
    }
    counts = (*(u32*)&part->gt4Count);

    count = counts & 0xFF;
    for (i = 0; i < count; i++, pkt += 0x34, poly += 0x18) {
        if (*(u32*)pkt != 0) {
            c1 = poly;
            code1 = pkt[7];
            for (k = 0, rgb1 = pkt + 4, off1 = 4; k < 4; k++) {
                gte_ldv0(normals + c1[k * 4 + 7] * 8);
                gte_ldrgb(&c1[off1]);
                gte_nccs();
                gte_strgb(rgb1);
                rgb1 += 0xC;
                off1 += 4;
            }
            pkt[7] = code1;
        }
    }

    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, pkt += 0x28, poly += 0x14) {
        if (*(u32*)pkt != 0) {
            c2 = poly;
            code2 = pkt[7];
            for (k = 0, rgb2 = pkt + 4, off2 = 4; k < 3; k++) {
                gte_ldv0(normals + c2[k * 4 + 7] * 8);
                gte_ldrgb(&c2[off2]);
                gte_nccs();
                gte_strgb(rgb2);
                rgb2 += 0xC;
                off2 += 4;
            }
            pkt[7] = code2;
        }
    }

    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, pkt += 0x28, poly += 0xC) {
        if (*(u32*)pkt != 0) {
            codes1 = pkt[7];
            rgbs1 = pkt + 4;
            gte_ldv0(normals + poly[7] * 8);
            gte_ldrgb(&poly[4]);
            gte_nccs();
            gte_strgb(rgbs1);
            pkt[7] = codes1;
        }
    }

    count = counts >> 24;
    for (i = 0; i < count; i++, pkt += 0x20, poly += 0xC) {
        if (*(u32*)pkt != 0) {
            codes2 = pkt[7];
            rgbs2 = pkt + 4;
            gte_ldv0(normals + poly[7] * 8);
            gte_ldrgb(&poly[4]);
            gte_nccs();
            gte_strgb(rgbs2);
            pkt[7] = codes2;
        }
    }

    counts = (*(u32*)&part->f3Count);

    count = counts & 0xFF;
    for (i = 0; i < count; i++, pkt += 0x14, poly += 8) {
        if (*(u32*)pkt != 0) {
            codes3 = pkt[7];
            rgbs3 = pkt + 4;
            gte_ldv0(normals + poly[7] * 8);
            gte_ldrgb(&poly[4]);
            gte_nccs();
            gte_strgb(rgbs3);
            pkt[7] = codes3;
        }
    }

    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, pkt += 0x18, poly += 8) {
        if (*(u32*)pkt != 0) {
            codes4 = pkt[7];
            rgbs4 = pkt + 4;
            gte_ldv0(normals + poly[7] * 8);
            gte_ldrgb(&poly[4]);
            gte_nccs();
            gte_strgb(rgbs4);
            pkt[7] = codes4;
        }
    }

    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, pkt += 0x1C, poly += 0x10) {
        if (*(u32*)pkt != 0) {
            c3 = poly;
            code3 = pkt[7];
            for (k = 0, rgb3 = pkt + 4, off3 = 4; k < 3; k++) {
                gte_ldv0(normals + c3[k * 4 + 7] * 8);
                gte_ldrgb(&c3[off3]);
                gte_nccs();
                gte_strgb(rgb3);
                rgb3 += 8;
                off3 += 4;
            }
            pkt[7] = code3;
        }
    }

    count = counts >> 24;
    for (i = 0; i < count; i++, pkt += 0x24, poly += 0x14) {
        if (*(u32*)pkt != 0) {
            c4 = poly;
            code4 = pkt[7];
            for (k = 0, rgb4 = pkt + 4, off4 = 4; k < 4; k++) {
                gte_ldv0(normals + c4[k * 4 + 7] * 8);
                gte_ldrgb(&c4[off4]);
                gte_nccs();
                gte_strgb(rgb4);
                rgb4 += 8;
                off4 += 4;
            }
            pkt[7] = code4;
        }
    }
}

/* Applies one unaligned little-endian u16 per colour channel to every part of
 * the model, and puts the seventh byte in the scratchpad for the handwritten
 * KawaiSetColorToPartPkts to pick up. */
s32 KawaiSetColorToModelPkts(FieldModelEntry* model, u8* data) {
    u8 unused[8];
    u8* parts;
    u32 count;
    u32 i;
    s32 r;
    s32 g;
    s32 b;
    u32 redLo;
    u32 greenLo;
    u32 blueLo;
    u32 redHi;
    u32 greenHi;
    u32 blueHi;

    count = model->partCount;
    parts = model->modelData + model->partsOffset;
    redHi = data[1];
    redLo = data[0];
    greenHi = data[3];
    greenLo = data[2];
    blueHi = data[5];
    blueLo = data[4];
    r = redLo | (redHi << 8);
    g = greenLo | (greenHi << 8);
    b = blueLo | (blueHi << 8);
    *(u32*)0x1F800200 = data[6];
    for (i = 0; i < count; i++) {
        KawaiSetColorToPartPkts(&parts[i * 32], r, g, b);
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetColorToPartPkts);

/* Load this model's animated eye/mouth textures into VRAM. The face selector
 * is four bytes: two mouth frames, one eye frame, and the model's own slot
 * index, which both places the tiles in VRAM and gates the whole function --
 * slots 0x21 and up have no tiles reserved for them and the call is a no-op.
 * Each variant is looked up in a per-textureFaceId index table (mouth: stride
 * 7, eye: stride 3) and the matching 0x200-byte page of the shared model
 * texture block is uploaded to that slot's tile. */
extern u8 g_KawaiMouthTexPages[][7]; /* mouth texture page index, per face, per
                                        frame */
extern u8
    g_KawaiEyeTexPages[][3]; /* eye texture page index, per face, per frame */

/* Reload one model's eye and mouth textures into VRAM: three LoadImage calls
 * into a per-model 16x32 slot, the two mouth halves side by side at x+0x300
 * and x+0x308 and the eyes on the row below at y+0x1A0. `faceSel` is the
 * per-frame selection -- [0] and [1] index the mouth table, [2] the eye
 * table, [3] the VRAM slot. */
s32 KawaiLoadEyesMouthTexToVram(FieldModelEntry* model, u8* faceSel) {
    RECT rect;
    u_long* src;
    s32 slot;
    s32 sel;

    slot = faceSel[3];
    if (slot < 0x21) {
        sel = faceSel[0];
        src = (u_long*)((u8*)D_800DFCA0 + D_800DFCA0->pageOffset +
                        (g_KawaiMouthTexPages[model->textureFaceId][sel] << 9));
        rect.x = ((slot - (slot / 4) * 4) << 4) + 0x300;
        rect.y = ((slot / 4) << 5) + 0x100;
        rect.w = 8;
        rect.h = 0x20;
        LoadImage(&rect, src);
        sel = faceSel[1];
        src = (u_long*)((u8*)D_800DFCA0 + D_800DFCA0->pageOffset +
                        (g_KawaiMouthTexPages[model->textureFaceId][sel] << 9));
        rect.x = ((slot - (slot / 4) * 4) << 4) + 0x308;
        rect.y = ((slot / 4) << 5) + 0x100;
        rect.w = 8;
        rect.h = 0x20;
        LoadImage(&rect, src);
        sel = faceSel[2];
        src = (u_long*)((u8*)D_800DFCA0 + D_800DFCA0->pageOffset +
                        (g_KawaiEyeTexPages[model->textureFaceId][sel] << 9));
        rect.x = ((slot - (slot / 8) * 8) << 3) + 0x300;
        rect.y = ((slot / 8) << 5) + 0x1A0;
        rect.w = 8;
        rect.h = 0x20;
        LoadImage(&rect, src);
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiLightingApplyToModel);

/* Light one model part in place. Every polygon colour word carries the index
 * of its vertex normal in the byte the GPU would read as `code`, so each is
 * fed to NormalColorColSingle and the result written back over the same three
 * bytes. The eight groups differ only in the primitive's stride and how many
 * colour words it carries. Bit 1 of the part's data header marks it lit, so a
 * second call with redo == 0 returns immediately. */
void KawaiLightingApplyToPolyColor(FieldModelPart* part, s32 redo) {
    u8* scratch;
    u8* normals;
    u8* data;
    u8* poly;
    u8* c;
    u8* c1;
    u32 counts;
    u32 count;
    u32 i;
    u32 k;

    data = part->data;
    normals = (u8*)D_800DF520;
    scratch = (u8*)0x1F800000;
    if ((*(u32*)data & 2) && redo == 0) {
        return;
    }

    poly = (u8*)(part->polyOffset + (u32)data);
    counts = (*(u32*)&part->gt4Count);

    count = counts & 0xFF;
    for (i = 0; i < count; i++, poly += 0x18) {
        c1 = poly;
        for (k = 0; k < 4; k++) {
            gte_ldv0(normals + c1[k * 4 + 7] * 8);
            gte_ldrgb(&c1[k * 4 + 4]);
            gte_nccs();
            gte_strgb(scratch);
            c1[k * 4 + 4] = scratch[0];
            c1[k * 4 + 5] = scratch[1];
            c1[k * 4 + 6] = scratch[2];
        }
    }

    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, poly += 0x14) {
        c = poly;
        for (k = 0; k < 3; k++) {
            gte_ldv0(normals + c[k * 4 + 7] * 8);
            gte_ldrgb(&c[k * 4 + 4]);
            gte_nccs();
            gte_strgb(scratch);
            c[k * 4 + 4] = scratch[0];
            c[k * 4 + 5] = scratch[1];
            c[k * 4 + 6] = scratch[2];
        }
    }

    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, poly += 0xC) {
        gte_ldv0(normals + poly[7] * 8);
        gte_ldrgb(&poly[4]);
        gte_nccs();
        gte_strgb(scratch);
        poly[4] = scratch[0];
        poly[5] = scratch[1];
        poly[6] = scratch[2];
    }

    count = counts >> 24;
    for (i = 0; i < count; i++, poly += 0xC) {
        gte_ldv0(normals + poly[7] * 8);
        gte_ldrgb(&poly[4]);
        gte_nccs();
        gte_strgb(scratch);
        poly[4] = scratch[0];
        poly[5] = scratch[1];
        poly[6] = scratch[2];
    }

    counts = (*(u32*)&part->f3Count);

    count = counts & 0xFF;
    for (i = 0; i < count; i++, poly += 8) {
        gte_ldv0(normals + poly[7] * 8);
        gte_ldrgb(&poly[4]);
        gte_nccs();
        gte_strgb(scratch);
        poly[4] = scratch[0];
        poly[5] = scratch[1];
        poly[6] = scratch[2];
    }

    count = (counts & 0xFF00) >> 8;
    for (i = 0; i < count; i++, poly += 8) {
        gte_ldv0(normals + poly[7] * 8);
        gte_ldrgb(&poly[4]);
        gte_nccs();
        gte_strgb(scratch);
        poly[4] = scratch[0];
        poly[5] = scratch[1];
        poly[6] = scratch[2];
    }

    count = (counts >> 16) & 0xFF;
    for (i = 0; i < count; i++, poly += 0x10) {
        c = poly;
        for (k = 0; k < 3; k++) {
            gte_ldv0(normals + c[k * 4 + 7] * 8);
            gte_ldrgb(&c[k * 4 + 4]);
            gte_nccs();
            gte_strgb(scratch);
            c[k * 4 + 4] = scratch[0];
            c[k * 4 + 5] = scratch[1];
            c[k * 4 + 6] = scratch[2];
        }
    }

    count = counts >> 24;
    for (i = 0; i < count; i++, poly += 0x14) {
        c = poly;
        for (k = 0; k < 4; k++) {
            gte_ldv0(normals + c[k * 4 + 7] * 8);
            gte_ldrgb(&c[k * 4 + 4]);
            gte_nccs();
            gte_strgb(scratch);
            c[k * 4 + 4] = scratch[0];
            c[k * 4 + 5] = scratch[1];
            c[k * 4 + 6] = scratch[2];
        }
    }

    *(u32*)part->data |= 2;
}

/* Set the semi-transparency/shade bits of every packet of every part of one
 * model. Walks each part's double-buffered packet area (the two ordering-table
 * copies) and toggles the ABE and shade bits of each primitive's tag byte, in
 * eight unrolled blocks, one per primitive type (strides 34/28/28/20/14/18/
 * 1C/24). */
s32 KawaiSetModelTransparency(FieldModelEntry* model, u8* data) {
    u8* parts;
    u8* part;
    u8* base;
    u32 enable;
    u32 i;
    u32 ot;
    u32 j;
    u32 n;

    parts = (u8*)(model->partsOffset + (s32)model->modelData);
    enable = data[0];
    for (i = 0; i < model->partCount; i++) {
        part = &parts[i * 0x20];
        for (ot = 0; ot < 2; ot++) {
            base = *(u8**)(part + 0x1C);
            if (ot != 0) {
                base += *(u16*)(part + 0x16);
            }
            n = part[4];
            for (j = 0; j < n; base += 0x34, j++) {
                if (enable) {
                    base[7] |= 2;
                } else {
                    base[7] &= ~2;
                }
                if (enable) {
                    base[7] |= 1;
                } else {
                    base[7] &= ~1;
                }
            }
            n = part[5];
            for (j = 0; j < n; base += 0x28, j++) {
                if (enable) {
                    base[7] |= 2;
                } else {
                    base[7] &= ~2;
                }
                if (enable) {
                    base[7] |= 1;
                } else {
                    base[7] &= ~1;
                }
            }
            n = part[6];
            for (j = 0; j < n; base += 0x28, j++) {
                if (enable) {
                    base[7] |= 2;
                } else {
                    base[7] &= ~2;
                }
                if (enable) {
                    base[7] |= 1;
                } else {
                    base[7] &= ~1;
                }
            }
            n = part[7];
            for (j = 0; j < n; base += 0x20, j++) {
                if (enable) {
                    base[7] |= 2;
                } else {
                    base[7] &= ~2;
                }
                if (enable) {
                    base[7] |= 1;
                } else {
                    base[7] &= ~1;
                }
            }
            n = part[8];
            for (j = 0; j < n; base += 0x14, j++) {
                if (enable) {
                    base[7] |= 2;
                } else {
                    base[7] &= ~2;
                }
                if (enable) {
                    base[7] |= 1;
                } else {
                    base[7] &= ~1;
                }
            }
            n = part[9];
            for (j = 0; j < n; base += 0x18, j++) {
                if (enable) {
                    base[7] |= 2;
                } else {
                    base[7] &= ~2;
                }
                if (enable) {
                    base[7] |= 1;
                } else {
                    base[7] &= ~1;
                }
            }
            n = part[10];
            for (j = 0; j < n; base += 0x1C, j++) {
                if (enable) {
                    base[7] |= 2;
                } else {
                    base[7] &= ~2;
                }
                if (enable) {
                    base[7] |= 1;
                } else {
                    base[7] &= ~1;
                }
            }
            n = part[11];
            for (j = 0; j < n; base += 0x24, j++) {
                if (enable) {
                    base[7] |= 2;
                } else {
                    base[7] &= ~2;
                }
                if (enable) {
                    base[7] |= 1;
                } else {
                    base[7] &= ~1;
                }
            }
        }
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetColorToPktsBelowLvl);

INCLUDE_ASM(
    "asm/us/field/nonmatchings/field3", KawaiSetColorToPartPktsBelowLvl);

/* The slot table starts 0x20 past the scratch quad. Reaching it that way,
 * rather than through its own g_KawaiColorFadeSlots symbol, is what lets cse
 * hand the scratch's own address back as `-0x20($a2)` off the table base
 * register. */
#define KawaiFadeSlots ((KawaiColorFadeSlot*)(g_KawaiFadeScratch + 0x20))

/* Fade a model's vertex colour over time (KAWAI sub-command). data[0]==0 inits
 * the slot from the descriptor and returns 1; data[0]==1 exports the current
 * colour to the scratch quad, pushes it to the packets, advances each channel
 * toward its target with clamping, and returns 0 (1 if the slot was already
 * finished); any other sub-command returns 1. */
s32 KawaiFadeModelColor(FieldModelEntry* model, u8* data) {
    KawaiColorFadeSlot* slot;
    s32 done;
    u8 unusedLocals[0x38];

    slot = &KawaiFadeSlots[data[1]];
    switch (data[0]) {
    case 0:
        slot->curR = data[0x02] | (data[0x03] << 8);
        slot->curG = data[0x04] | (data[0x05] << 8);
        slot->curB = data[0x06] | (data[0x07] << 8);
        slot->targetR = data[0x08] | (data[0x09] << 8);
        slot->targetG = data[0x0A] | (data[0x0B] << 8);
        slot->targetB = data[0x0C] | (data[0x0D] << 8);
        slot->deltaR = data[0x0E] | (data[0x0F] << 8);
        slot->deltaG = data[0x10] | (data[0x11] << 8);
        slot->deltaB = data[0x12] | (data[0x13] << 8);
        slot->unk12 = data[0x14];
        slot->done = 0;
        return 1;
    case 1:
        done = 0;
        g_KawaiFadeScratch[0] = slot->curR;
        g_KawaiFadeScratch[1] = slot->curR >> 8;
        g_KawaiFadeScratch[2] = slot->curG;
        g_KawaiFadeScratch[3] = slot->curG >> 8;
        g_KawaiFadeScratch[4] = slot->curB;
        g_KawaiFadeScratch[5] = slot->curB >> 8;
        g_KawaiFadeScratch[6] = slot->unk12;
        KawaiSetColorToModelPkts(model, g_KawaiFadeScratch);
        if (slot->done != 0) {
            return 1;
        }
        slot->curR += slot->deltaR;
        if (slot->deltaR >= 0) {
            if (slot->curR < slot->targetR) {
                goto skipR;
            }
        } else if (slot->curR > slot->targetR) {
            goto skipR;
        }
        slot->curR = slot->targetR;
        done |= 1;
    skipR:
        slot->curG += slot->deltaG;
        if (slot->deltaG >= 0) {
            if (slot->curG < slot->targetG) {
                goto skipG;
            }
        } else if (slot->curG > slot->targetG) {
            goto skipG;
        }
        slot->curG = slot->targetG;
        done |= 2;
    skipG:
        slot->curB += slot->deltaB;
        if (slot->deltaB >= 0) {
            if (slot->curB >= slot->targetB) {
                goto clampB;
            }
            goto skipB;
        } else if (slot->curB > slot->targetB) {
            goto skipB;
        }
    clampB:
        slot->curB = slot->targetB;
        done |= 4;
    skipB:
        if (done == 7) {
            slot->done++;
        }
        return 0;
    }
    return 1;
}

/* Store/apply a custom GTE lighting setup (KAWAI sub-command). data[0]==0
 * copies the 0x1E-byte descriptor into the slot -- twelve loose bytes, then
 * nine LE u16 words -- and returns 1; data[0]==1 expands the slot into the
 * g_KawaiFadeScratch scratch buffer and calls the handwritten GTE driver,
 * returning 0; any other sub-command returns 0. The slot reuses the
 * KawaiFadeModelColor table's 0x3C stride with a flat lighting-blob layout. */
s32 KawaiSetCustomLighting(FieldModelEntry* model, u8* data) {
    u8* slot;
    u8 unusedLocals[0x38];

    slot = (u8*)&KawaiFadeSlots[data[1]];
    switch (data[0]) {
    case 0:
        slot[0x00] = data[0x02];
        slot[0x01] = data[0x03];
        slot[0x02] = data[0x04];
        slot[0x03] = data[0x05];
        slot[0x04] = data[0x06];
        slot[0x05] = data[0x07];
        slot[0x06] = data[0x08];
        slot[0x07] = data[0x09];
        slot[0x08] = data[0x0A];
        slot[0x09] = data[0x0B];
        slot[0x0A] = data[0x0C];
        slot[0x0B] = data[0x0D];
        *(u16*)(slot + 0x0C) = data[0x0E] | (data[0x0F] << 8);
        *(u16*)(slot + 0x0E) = data[0x10] | (data[0x11] << 8);
        *(u16*)(slot + 0x10) = data[0x12] | (data[0x13] << 8);
        *(u16*)(slot + 0x12) = data[0x14] | (data[0x15] << 8);
        *(u16*)(slot + 0x14) = data[0x16] | (data[0x17] << 8);
        *(u16*)(slot + 0x16) = data[0x18] | (data[0x19] << 8);
        *(u16*)(slot + 0x18) = data[0x1A] | (data[0x1B] << 8);
        *(u16*)(slot + 0x1A) = data[0x1C] | (data[0x1D] << 8);
        *(u16*)(slot + 0x1C) = data[0x1E] | (data[0x1F] << 8);
        return 1;
    case 1:
        g_KawaiFadeScratch[0x00] = slot[0x00];
        g_KawaiFadeScratch[0x01] = slot[0x01];
        g_KawaiFadeScratch[0x02] = slot[0x02];
        g_KawaiFadeScratch[0x03] = slot[0x03];
        g_KawaiFadeScratch[0x04] = slot[0x04];
        g_KawaiFadeScratch[0x05] = slot[0x05];
        g_KawaiFadeScratch[0x06] = slot[0x06];
        g_KawaiFadeScratch[0x07] = slot[0x07];
        g_KawaiFadeScratch[0x08] = slot[0x08];
        g_KawaiFadeScratch[0x09] = slot[0x09];
        g_KawaiFadeScratch[0x0A] = slot[0x0A];
        g_KawaiFadeScratch[0x0B] = slot[0x0B];
        g_KawaiFadeScratch[0x0C] = *(u16*)(slot + 0x0C);
        g_KawaiFadeScratch[0x0D] = *(u16*)(slot + 0x0C) >> 8;
        g_KawaiFadeScratch[0x0E] = *(u16*)(slot + 0x0E);
        g_KawaiFadeScratch[0x0F] = *(u16*)(slot + 0x0E) >> 8;
        g_KawaiFadeScratch[0x10] = *(u16*)(slot + 0x10);
        g_KawaiFadeScratch[0x11] = *(u16*)(slot + 0x10) >> 8;
        g_KawaiFadeScratch[0x12] = *(u16*)(slot + 0x12);
        g_KawaiFadeScratch[0x13] = *(u16*)(slot + 0x12) >> 8;
        g_KawaiFadeScratch[0x14] = *(u16*)(slot + 0x14);
        g_KawaiFadeScratch[0x15] = *(u16*)(slot + 0x14) >> 8;
        g_KawaiFadeScratch[0x16] = *(u16*)(slot + 0x16);
        g_KawaiFadeScratch[0x17] = *(u16*)(slot + 0x16) >> 8;
        g_KawaiFadeScratch[0x18] = *(u16*)(slot + 0x18);
        g_KawaiFadeScratch[0x19] = *(u16*)(slot + 0x18) >> 8;
        g_KawaiFadeScratch[0x1A] = *(u16*)(slot + 0x1A);
        g_KawaiFadeScratch[0x1B] = *(u16*)(slot + 0x1A) >> 8;
        g_KawaiFadeScratch[0x1C] = *(u16*)(slot + 0x1C);
        g_KawaiFadeScratch[0x1D] = *(u16*)(slot + 0x1C) >> 8;
        KawaiSetCustomLightToModelPkts(model, g_KawaiFadeScratch);
        return 0;
    }
    return 0;
}

s32 KawaiColorFadeBelowLvl(FieldModelEntry* model, u8* data) {
    KawaiFadeBelowLvlSlot* slot;
    s32 done;
    u8 unusedLocals[0x50];

    slot = (KawaiFadeBelowLvlSlot*)&KawaiFadeSlots[data[1]];
    switch (data[0]) {
    case 0:
        slot->cur0 = data[0x02] | (data[0x03] << 8);
        slot->cur1 = data[0x04] | (data[0x05] << 8);
        slot->cur2 = data[0x06] | (data[0x07] << 8);
        slot->cur3 = data[0x08] | (data[0x09] << 8);
        slot->target0 = data[0x0A] | (data[0x0B] << 8);
        slot->target1 = data[0x0C] | (data[0x0D] << 8);
        slot->target2 = data[0x0E] | (data[0x0F] << 8);
        slot->target3 = data[0x10] | (data[0x11] << 8);
        slot->delta0 = data[0x12] | (data[0x13] << 8);
        slot->delta1 = data[0x14] | (data[0x15] << 8);
        slot->delta2 = data[0x16] | (data[0x17] << 8);
        slot->delta3 = data[0x18] | (data[0x19] << 8);
        slot->unk18 = data[0x1A];
        slot->done = 0;
        return 1;
    case 1:
        done = 0;
        g_KawaiFadeScratch[0] = slot->cur0;
        g_KawaiFadeScratch[1] = slot->cur0 >> 8;
        g_KawaiFadeScratch[2] = slot->cur1;
        g_KawaiFadeScratch[3] = slot->cur1 >> 8;
        g_KawaiFadeScratch[4] = slot->cur2;
        g_KawaiFadeScratch[5] = slot->cur2 >> 8;
        g_KawaiFadeScratch[6] = slot->cur3;
        g_KawaiFadeScratch[7] = slot->cur3 >> 8;
        g_KawaiFadeScratch[8] = slot->unk18;
        KawaiSetColorToPktsBelowLvl(model, g_KawaiFadeScratch);
        if (slot->done != 0) {
            return 1;
        }
        slot->cur0 += slot->delta0;
        if (slot->delta0 >= 0) {
            if ((s16)slot->cur0 < slot->target0) {
                goto cur0done;
            }
        } else if ((s16)slot->cur0 > slot->target0) {
            goto cur0done;
        }
        slot->cur0 = slot->target0;
        done |= 1;
    cur0done:
        slot->cur1 += slot->delta1;
        if (slot->delta1 >= 0) {
            if ((s16)slot->cur1 < slot->target1) {
                goto cur1done;
            }
        } else if ((s16)slot->cur1 > slot->target1) {
            goto cur1done;
        }
        slot->cur1 = slot->target1;
        done |= 2;
    cur1done:
        slot->cur2 += slot->delta2;
        if (slot->delta2 >= 0) {
            if ((s16)slot->cur2 < slot->target2) {
                goto cur2done;
            }
        } else if ((s16)slot->cur2 > slot->target2) {
            goto cur2done;
        }
        slot->cur2 = slot->target2;
        done |= 4;
    cur2done:
        slot->cur3 += slot->delta3;
        if (slot->delta3 >= 0) {
            if ((s16)slot->cur3 >= slot->target3) {
                goto cur3clamp;
            }
            goto cur3done;
        } else if ((s16)slot->cur3 > slot->target3) {
            goto cur3done;
        }
    cur3clamp:
        slot->cur3 = slot->target3;
        done |= 8;
    cur3done:
        if (done == 0xF) {
            slot->done++;
        }
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetLightingToModelPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetLightingToPartPkts);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiSetSplashToPktsBelowLvl);

/* Build the 30 splash-sprite packet pairs for one field model's render slot:
 * two sprites per part, both 0x2C-coded semi-transparent, sharing the texture
 * page and CLUT, with the part's y offset negated into the second sprite. */
void KawaiInitSplashPkts(void* arg0, s32 arg1) {
    s16 clut;
    s16 tex;
    s32 i;
    u8* pkt;
    u16* parts;
    u8* base;
    s32 count;

    base = (u8*)D_800E0200 + arg1 * 0xAC8;
    tex = 0x6C2C;
    if (GetGraphType() == 1 || GetGraphType() == 2) {
        clut = 0x22B;
    } else {
        clut = 0x9B;
    }
    count = 0x1F;
    parts = (u16*)(*(u32*)((u8*)arg0 + 0x1C) + 4);
    for (i = 1; i < count; i++) {
        pkt = &base[i * 0x5C];
        pkt[0x3] = 9;
        pkt[0x2B] = 9;
        pkt[0x7] = 0x2C;
        pkt[0x2F] = 0x2C;
        pkt[0x2E] = 0x80;
        pkt[0x6] = 0x80;
        pkt[0x2D] = 0x80;
        pkt[0x5] = 0x80;
        pkt[0x2C] = 0x80;
        pkt[0x4] = 0x80;
        *(s16*)(pkt + 0x36) = tex;
        *(s16*)(pkt + 0xE) = tex;
        *(s16*)(pkt + 0x3E) = clut;
        *(s16*)(pkt + 0x16) = clut;
        *(s16*)(pkt + 0x50) = 0;
        *(s16*)(pkt + 0x52) = 0;
        *(s16*)(pkt + 0x54) = 0;
        pkt[0x7] |= 2;
        pkt[0x2F] |= 2;
        *(s16*)(pkt + 0x58) = -*(s16*)parts;
        *(s16*)(pkt + 0x5A) = 0;
        parts += 2;
    }
}

s32 KawaiSetPartAttribute(FieldModelEntry* model, u8* data) {
    u8* parts;
    s32 count;
    s32 i;
    s32 partIdx;

    count = data[0];
    if (count > 0) {
        parts = model->modelData + model->partsOffset;
        for (i = 0; i < count; i++) {
            partIdx = data[i * 2 + 1];
            if (partIdx < model->partCount) {
                parts[partIdx * 32] = data[i * 2 + 2];
            }
        }
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiApplyBoneTransform);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiRenderClippedPart);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiDirectionalColorGradient);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiGradientColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field3", KawaiAnimatedPointLight);
