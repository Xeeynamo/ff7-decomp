//! PSYQ=3.3 COMM=true

#include "highway_private.h"

const SVECTOR g_HighwaySegmentStep = {0, 0, 256, 0};

const SVECTOR g_HighwayZeroRot = {0, 0, 0, 0};

void HighwayTexturesInit(void) {
    TIM_IMAGE timimg;
    s32 i;
    u_long** tims;

    for (i = 0, tims = g_HighwayTimAddr; i < LEN(g_HighwayTimAddr); i++) {
        HighwayLoadTim(*tims);
        OpenTIM(*tims++);
        ReadTIM(&timimg);
    }
    g_HighwaySpriteTPage[0] = GetTPage(1, 1, 0x2C0, 0x100);
    g_HighwaySpriteClut[0] = GetClut(0, 0x1FD);
    g_HighwaySpriteTPage[1] = GetTPage(1, 1, 0x2C0, 0x100);
    g_HighwaySpriteClut[1] = GetClut(0, 0x1FD);
    g_HighwaySpriteTPage[2] = GetTPage(1, 1, 0x2C0, 0x100);
    g_HighwaySpriteClut[2] = GetClut(0, 0x1FD);
    g_HighwayRoadTPage[0] = GetTPage(0, 1, 0x280, 0);
    g_HighwayRoadClut[0] = GetClut(0, 0x1E0);
    g_HighwayRoadTPage[1] = GetTPage(0, 1, 0x280, 0x40);
    g_HighwayRoadClut[1] = GetClut(0, 0x1E1);
    g_HighwayRoadTPage[2] = GetTPage(0, 1, 0x280, 0x80);
    g_HighwayRoadClut[2] = GetClut(0x20, 0x1E0);
    g_HighwayRoadTPage[3] = GetTPage(0, 1, 0x280, 0xC0);
    g_HighwayRoadClut[3] = GetClut(0x10, 0x1E0);
    g_HighwayRoadTPage[4] = GetTPage(1, 1, 0x2C0, 0x40);
    g_HighwayRoadClut[4] = GetClut(0, 0x1FE);
    g_HighwayRoadTPage[5] = GetTPage(0, 1, 0x240, 0xC0);
    g_HighwayRoadClut[5] = GetClut(0xF0, 0x1E3);
    g_HighwayRoadTPage[6] = GetTPage(0, 1, 0x240, 0x40);
    g_HighwayRoadClut[6] = GetClut(0xF0, 0x1E2);
    g_HighwayRoadTPage[7] = GetTPage(0, 1, 0x240, 0x80);
    g_HighwayRoadClut[7] = GetClut(0xF0, 0x1E4);
    g_HighwayRoadTPage[8] = GetTPage(0, 1, 0x2C0, 0);
    g_HighwayRoadClut[8] = GetClut(0xF0, 0x1E1);
    g_HighwayRoadTPage[9] = GetTPage(0, 1, 0x2C0, 0xC0);
    g_HighwayRoadClut[9] = GetClut(0xF0, 0x1E5);
    g_HighwayRoadTPage[10] = GetTPage(0, 1, 0x300, 0xC0);
    g_HighwayRoadClut[10] = GetClut(0xF0, 0x1E0);
    g_HighwayWallClut[0] = GetClut(0x30, 0x1E0);
    g_HighwayWallTPage[0] = GetTPage(0, 1, 0x300, 0);
    g_HighwayWallClut[1] = GetClut(0x40, 0x1E0);
    g_HighwayWallTPage[1] = GetTPage(0, 1, 0x300, 0);
    g_HighwayWallClut[2] = GetClut(0x50, 0x1E0);
    g_HighwayWallTPage[2] = GetTPage(0, 1, 0x300, 0);
    g_HighwayWallClut[3] = GetClut(0x60, 0x1E0);
    g_HighwayWallTPage[3] = GetTPage(0, 1, 0x300, 0);
    g_HighwayWallClut[4] = GetClut(0x70, 0x1E0);
    g_HighwayWallTPage[4] = GetTPage(0, 1, 0x300, 0);
    g_HighwayWallClut[5] = GetClut(0x80, 0x1E0);
    g_HighwayWallTPage[5] = GetTPage(0, 1, 0x300, 0);
    g_HighwayWallClut[6] = GetClut(0x90, 0x1E0);
    g_HighwayWallTPage[6] = GetTPage(0, 1, 0x300, 0);
    g_HighwayWallClut[7] = GetClut(0xA0, 0x1E0);
    g_HighwayWallTPage[7] = GetTPage(0, 1, 0x300, 0);
    D_800C4A98 = GetClut(0, 0x1E5);
    D_8010FD7C = GetTPage(0, 1, 0x240, 0);
    D_80116674 = GetClut(0x30, 0x1E2);
    D_800BD590 = GetTPage(0, 1, 0x208, 0);

    g_HighwayGaugeLayout[4].x = 0xD9;
    g_HighwayGaugeLayout[4].y = 0x28;
    g_HighwayGaugeLayout[4].unk4 = 0xDC;
    g_HighwayGaugeLayout[4].unk6 = 0x31;
    g_HighwayGaugeLayout[4].unk8[0] = 0;
    g_HighwayGaugeLayout[4].unkC[0] = 0;
    g_HighwayGaugeLayout[4].unk8[1] = 0x5C;
    g_HighwayGaugeLayout[4].unkC[1] = 0x2A;
    g_HighwayGaugeLayout[4].unk8[2] = 0x6A;
    g_HighwayGaugeLayout[4].unkC[2] = 0x2A;
    g_HighwayGaugeLayout[4].unk8[3] = 0x78;
    g_HighwayGaugeLayout[4].unkC[3] = 0x2A;
    g_HighwayGaugeLayout[4].w = 0x54;
    g_HighwayGaugeLayout[4].h = 0x10;
    g_HighwayGaugeLayout[4].unk12 = 0xE;
    g_HighwayGaugeLayout[4].unk13 = 0x45;

    g_HighwayGaugeLayout[2].x = 0xD9;
    g_HighwayGaugeLayout[2].y = 0x14;
    g_HighwayGaugeLayout[2].unk4 = 0xDC;
    g_HighwayGaugeLayout[2].unk6 = 0x1D;
    g_HighwayGaugeLayout[2].unk8[0] = 0;
    g_HighwayGaugeLayout[2].unkC[0] = 0;
    g_HighwayGaugeLayout[2].unk8[1] = 0x5C;
    g_HighwayGaugeLayout[2].unkC[1] = 0xE;
    g_HighwayGaugeLayout[2].unk8[2] = 0x6A;
    g_HighwayGaugeLayout[2].unkC[2] = 0xE;
    g_HighwayGaugeLayout[2].unk8[3] = 0x78;
    g_HighwayGaugeLayout[2].unkC[3] = 0xE;
    g_HighwayGaugeLayout[2].w = 0x54;
    g_HighwayGaugeLayout[2].h = 0x10;
    g_HighwayGaugeLayout[2].unk12 = 0xE;
    g_HighwayGaugeLayout[2].unk13 = 0x45;

    g_HighwayGaugeLayout[3].x = 0x14;
    g_HighwayGaugeLayout[3].y = 0x14;
    g_HighwayGaugeLayout[3].unk4 = 0x25;
    g_HighwayGaugeLayout[3].unk6 = 0x1D;
    g_HighwayGaugeLayout[3].unk8[0] = 0;
    g_HighwayGaugeLayout[3].unkC[0] = 0x10;
    g_HighwayGaugeLayout[3].unk8[1] = 0x5C;
    g_HighwayGaugeLayout[3].unkC[1] = 0x1C;
    g_HighwayGaugeLayout[3].unk8[2] = 0x6A;
    g_HighwayGaugeLayout[3].unkC[2] = 0x1C;
    g_HighwayGaugeLayout[3].unk8[3] = 0x78;
    g_HighwayGaugeLayout[3].unkC[3] = 0x1C;
    g_HighwayGaugeLayout[3].w = 0x54;
    g_HighwayGaugeLayout[3].h = 0x10;
    g_HighwayGaugeLayout[3].unk12 = 0xE;
    g_HighwayGaugeLayout[3].unk13 = 1;

    g_HighwayGaugeLayout[1].x = 0x14;
    g_HighwayGaugeLayout[1].y = 0x28;
    g_HighwayGaugeLayout[1].unk4 = 0x25;
    g_HighwayGaugeLayout[1].unk6 = 0x31;
    g_HighwayGaugeLayout[1].unk8[0] = 0;
    g_HighwayGaugeLayout[1].unkC[0] = 0x10;
    g_HighwayGaugeLayout[1].unk8[1] = 0x5C;
    g_HighwayGaugeLayout[1].unkC[1] = 0;
    g_HighwayGaugeLayout[1].unk8[2] = 0x6A;
    g_HighwayGaugeLayout[1].unkC[2] = 0;
    g_HighwayGaugeLayout[1].unk8[3] = 0x78;
    g_HighwayGaugeLayout[1].unkC[3] = 0;
    g_HighwayGaugeLayout[1].w = 0x54;
    g_HighwayGaugeLayout[1].h = 0x10;
    g_HighwayGaugeLayout[1].unk12 = 0xE;
    g_HighwayGaugeLayout[1].unk13 = 1;

    // The first three fields of record 0 must be written as g_HighwayGaugeLayout->field
    // (not g_HighwayGaugeLayout[0].field) to reproduce the original addressing.
    g_HighwayGaugeLayout->x = 0x74;
    g_HighwayGaugeLayout->y = 0xC8;
    g_HighwayGaugeLayout->unk4 = 0x8D;
    g_HighwayGaugeLayout[0].unk6 = 0xD9;
    g_HighwayGaugeLayout[0].unk8[0] = 0;
    g_HighwayGaugeLayout[0].unkC[0] = 0x20;
    g_HighwayGaugeLayout[0].unk8[1] = 0x5C;
    g_HighwayGaugeLayout[0].unkC[1] = 0x38;
    g_HighwayGaugeLayout[0].unk8[2] = 0x72;
    g_HighwayGaugeLayout[0].unkC[2] = 0x38;
    g_HighwayGaugeLayout[0].unk8[3] = 0x88;
    g_HighwayGaugeLayout[0].unkC[3] = 0x38;
    g_HighwayGaugeLayout[0].w = 0x5C;
    g_HighwayGaugeLayout[0].h = 0x18;
    g_HighwayGaugeLayout[0].unk12 = 0x16;
    g_HighwayGaugeLayout[0].unk13 = 1;

    g_HighwayGaugeTPage = GetTPage(1, 1, 0x140, 0x100);
    g_HighwayGaugeClut = GetClut(0, 0x1F4);
}

// Same as JetLoadTim: upload a TIM's image and CLUT to VRAM.
void HighwayLoadTim(u_long* tim) {
    TIM_IMAGE timimg;

    OpenTIM(tim);

    while (ReadTIM(&timimg)) {
        if (timimg.caddr) {
            LoadImage(timimg.crect, timimg.caddr);
        }
        if (timimg.paddr) {
            LoadImage(timimg.prect, timimg.paddr);
        }
    }
}

void HighwayRoadUvInit(void) {
    s32 i;

    setUV4(&g_HighwayRoadUv[0], 0, 0, 0xFF, 0, 0, 0x3F, 0xFF, 0x3F);
    g_HighwayRoadUv[0].tpage = g_HighwayRoadTPage[0];
    g_HighwayRoadUv[0].clut = g_HighwayRoadClut[0];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[0], 0);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[0], 0);
    setUV4(&g_HighwayRoadUv[1], 0, 0x40, 0xFF, 0x40, 0, 0x7F, 0xFF, 0x7F);
    g_HighwayRoadUv[1].tpage = g_HighwayRoadTPage[1];
    g_HighwayRoadUv[1].clut = g_HighwayRoadClut[1];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[1], 1);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[1], 1);
    setUV4(&g_HighwayRoadUv[2], 0, 0x80, 0xFF, 0x80, 0, 0xBF, 0xFF, 0xBF);
    g_HighwayRoadUv[2].tpage = g_HighwayRoadTPage[2];
    g_HighwayRoadUv[2].clut = g_HighwayRoadClut[2];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[2], 2);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[2], 2);
    setUV4(&g_HighwayRoadUv[3], 0, 0xC0, 0xFF, 0xC0, 0, 0xFF, 0xFF, 0xFF);
    g_HighwayRoadUv[3].tpage = g_HighwayRoadTPage[3];
    g_HighwayRoadUv[3].clut = g_HighwayRoadClut[3];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[3], 3);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[3], 3);
    setUV4(&g_HighwayRoadUv[4], 0, 0x40, 0xFF, 0x40, 0, 0x7F, 0xFF, 0x7F);
    g_HighwayRoadUv[4].tpage = g_HighwayRoadTPage[4];
    g_HighwayRoadUv[4].clut = g_HighwayRoadClut[4];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[4], 4);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[4], 4);
    setUV4(&g_HighwayRoadUv[5], 0, 0xC0, 0xFF, 0xC0, 0, 0xFF, 0xFF, 0xFF);
    g_HighwayRoadUv[5].tpage = g_HighwayRoadTPage[5];
    g_HighwayRoadUv[5].clut = g_HighwayRoadClut[5];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[5], 5);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[5], 5);
    setUV4(&g_HighwayRoadUv[6], 0, 0x40, 0xFF, 0x40, 0, 0x7F, 0xFF, 0x7F);
    g_HighwayRoadUv[6].tpage = g_HighwayRoadTPage[6];
    g_HighwayRoadUv[6].clut = g_HighwayRoadClut[6];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[6], 6);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[6], 6);
    setUV4(&g_HighwayRoadUv[7], 0, 0x80, 0xFF, 0x80, 0, 0xBF, 0xFF, 0xBF);
    g_HighwayRoadUv[7].tpage = g_HighwayRoadTPage[7];
    g_HighwayRoadUv[7].clut = g_HighwayRoadClut[7];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[7], 7);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[7], 7);
    setUV4(&g_HighwayRoadUv[8], 0, 0, 0xFF, 0, 0, 0x3F, 0xFF, 0x3F);
    g_HighwayRoadUv[8].tpage = g_HighwayRoadTPage[8];
    g_HighwayRoadUv[8].clut = g_HighwayRoadClut[8];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[8], 8);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[8], 8);
    setUV4(&g_HighwayRoadUv[9], 0, 0xC0, 0xFF, 0xC0, 0, 0xFF, 0xFF, 0xFF);
    g_HighwayRoadUv[9].tpage = g_HighwayRoadTPage[9];
    g_HighwayRoadUv[9].clut = g_HighwayRoadClut[9];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[9], 9);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[9], 9);
    setUV4(&g_HighwayRoadUv[10], 0, 0xC0, 0xFF, 0xC0, 0, 0xFF, 0xFF, 0xFF);
    g_HighwayRoadUv[10].tpage = g_HighwayRoadTPage[10];
    g_HighwayRoadUv[10].clut = g_HighwayRoadClut[10];
    HighwayQuadUvSplit8(&g_HighwayRoadUv[10], 10);
    HighwayQuadUvSplitH(&g_HighwayRoadUv[10], 10);
    setUV4(&g_HighwayWallUv[0], 0, 0, 0x3F, 0, 0, 0x1F, 0x3F, 0x1F);
    g_HighwayWallUv[0].tpage = g_HighwayWallTPage[0];
    g_HighwayWallUv[0].clut = g_HighwayWallClut[0];
    setUV4(&g_HighwayWallUv[1], 0x40, 0, 0x7F, 0, 0x40, 0x1F, 0x7F, 0x1F);
    g_HighwayWallUv[1].tpage = g_HighwayWallTPage[1];
    g_HighwayWallUv[1].clut = g_HighwayWallClut[1];
    setUV4(&g_HighwayWallUv[2], 0x80, 0, 0xBF, 0, 0x80, 0x1F, 0xBF, 0x1F);
    g_HighwayWallUv[2].tpage = g_HighwayWallTPage[2];
    g_HighwayWallUv[2].clut = g_HighwayWallClut[2];
    setUV4(&g_HighwayWallUv[3], 0xC0, 0, 0xFF, 0, 0xC0, 0x1F, 0xFF, 0x1F);
    g_HighwayWallUv[3].tpage = g_HighwayWallTPage[3];
    g_HighwayWallUv[3].clut = g_HighwayWallClut[3];
    setUV4(&g_HighwayWallUv[4], 0, 0x20, 0x3F, 0x20, 0, 0x3F, 0x3F, 0x3F);
    g_HighwayWallUv[4].tpage = g_HighwayWallTPage[4];
    g_HighwayWallUv[4].clut = g_HighwayWallClut[4];
    setUV4(&g_HighwayWallUv[5], 0x40, 0x20, 0x7F, 0x20, 0x40, 0x3F, 0x7F, 0x3F);
    g_HighwayWallUv[5].tpage = g_HighwayWallTPage[5];
    g_HighwayWallUv[5].clut = g_HighwayWallClut[5];
    setUV4(&g_HighwayWallUv[6], 0x80, 0x20, 0xBF, 0x20, 0x80, 0x3F, 0xBF, 0x3F);
    g_HighwayWallUv[6].tpage = g_HighwayWallTPage[6];
    g_HighwayWallUv[6].clut = g_HighwayWallClut[6];
    setUV4(&g_HighwayWallUv[7], 0xC0, 0x20, 0xFF, 0x20, 0xC0, 0x3F, 0xFF, 0x3F);
    g_HighwayWallUv[7].tpage = g_HighwayWallTPage[7];
    g_HighwayWallUv[7].clut = g_HighwayWallClut[7];
    for (i = 0; i < LEN(g_HighwayNearUvLeft); i++) {
        HighwayQuadUvSplitV(&g_HighwayWallUv[i], &g_HighwayNearUvLeft[i], &g_HighwayNearUvRight[i]);
    }
}

void HighwayQuadUvSplitV(HighwayQuadUv* src, HighwayQuadUv* top, HighwayQuadUv* bottom) {
    u8 mid;

    mid = (src->v0 + src->v2 - 1) >> 1;
    setUV4(top, src->u0, src->v0, src->u1, src->v1, src->u2, mid, src->u3, mid);
    top->tpage = src->tpage;
    top->clut = src->clut;
    setUV4(bottom, src->u0, mid, src->u1, mid, src->u2, src->v2, src->u3, src->v3);
    bottom->tpage = src->tpage;
    bottom->clut = src->clut;
}

void HighwayQuadUvSplit8(HighwayQuadUv* src, s32 index) {
    u8 midU;
    u8 left;
    u8 right;
    u8 midV;
    s32 i;

    midU = (src->u0 + src->u1 - 1) >> 1;
    left = (src->u0 + midU - 1) >> 1;
    right = (src->u1 + midU - 1) >> 1;
    midV = (src->v0 + src->v2 - 1) >> 1;
    i = index * 8;
    g_HighwayRoadUvEighths[i].u0 = src->u0;
    g_HighwayRoadUvEighths[i].v0 = src->v0;
    g_HighwayRoadUvEighths[i].u1 = left;
    g_HighwayRoadUvEighths[i].v1 = src->v0;
    g_HighwayRoadUvEighths[i].u2 = src->u0;
    g_HighwayRoadUvEighths[i].v2 = midV;
    g_HighwayRoadUvEighths[i].u3 = left;
    g_HighwayRoadUvEighths[i].v3 = midV;
    g_HighwayRoadUvEighths[i].clut = src->clut;
    g_HighwayRoadUvEighths[i].tpage = src->tpage;
    i = index * 8 + 1;
    g_HighwayRoadUvEighths[i].u0 = left + 1;
    g_HighwayRoadUvEighths[i].v0 = src->v0;
    g_HighwayRoadUvEighths[i].u1 = midU;
    g_HighwayRoadUvEighths[i].v1 = src->v0;
    g_HighwayRoadUvEighths[i].u2 = left + 1;
    g_HighwayRoadUvEighths[i].v2 = midV;
    g_HighwayRoadUvEighths[i].u3 = midU;
    g_HighwayRoadUvEighths[i].v3 = midV;
    g_HighwayRoadUvEighths[i].clut = src->clut;
    g_HighwayRoadUvEighths[i].tpage = src->tpage;
    i = index * 8 + 2;
    g_HighwayRoadUvEighths[i].u0 = midU + 1;
    g_HighwayRoadUvEighths[i].v0 = src->v0;
    g_HighwayRoadUvEighths[i].u1 = right;
    g_HighwayRoadUvEighths[i].v1 = src->v0;
    g_HighwayRoadUvEighths[i].u2 = midU + 1;
    g_HighwayRoadUvEighths[i].v2 = midV;
    g_HighwayRoadUvEighths[i].u3 = right;
    g_HighwayRoadUvEighths[i].v3 = midV;
    g_HighwayRoadUvEighths[i].clut = src->clut;
    g_HighwayRoadUvEighths[i].tpage = src->tpage;
    i = index * 8 + 3;
    g_HighwayRoadUvEighths[i].u0 = right + 1;
    g_HighwayRoadUvEighths[i].v0 = src->v0;
    g_HighwayRoadUvEighths[i].u1 = src->u1;
    g_HighwayRoadUvEighths[i].v1 = src->v0;
    g_HighwayRoadUvEighths[i].u2 = right + 1;
    g_HighwayRoadUvEighths[i].v2 = midV;
    g_HighwayRoadUvEighths[i].u3 = src->u1;
    g_HighwayRoadUvEighths[i].v3 = midV;
    g_HighwayRoadUvEighths[i].clut = src->clut;
    g_HighwayRoadUvEighths[i].tpage = src->tpage;
    i = index * 8 + 4;
    g_HighwayRoadUvEighths[i].u0 = src->u0;
    g_HighwayRoadUvEighths[i].v0 = midV + 1;
    g_HighwayRoadUvEighths[i].u1 = left;
    g_HighwayRoadUvEighths[i].v1 = midV + 1;
    g_HighwayRoadUvEighths[i].u2 = src->u0;
    g_HighwayRoadUvEighths[i].v2 = src->v2;
    g_HighwayRoadUvEighths[i].u3 = left;
    g_HighwayRoadUvEighths[i].v3 = src->v2;
    g_HighwayRoadUvEighths[i].clut = src->clut;
    g_HighwayRoadUvEighths[i].tpage = src->tpage;
    i = index * 8 + 5;
    g_HighwayRoadUvEighths[i].u0 = left + 1;
    g_HighwayRoadUvEighths[i].v0 = midV + 1;
    g_HighwayRoadUvEighths[i].u1 = midU;
    g_HighwayRoadUvEighths[i].v1 = midV + 1;
    g_HighwayRoadUvEighths[i].u2 = left + 1;
    g_HighwayRoadUvEighths[i].v2 = src->v2;
    g_HighwayRoadUvEighths[i].u3 = midU;
    g_HighwayRoadUvEighths[i].v3 = src->v2;
    g_HighwayRoadUvEighths[i].clut = src->clut;
    g_HighwayRoadUvEighths[i].tpage = src->tpage;
    i = index * 8 + 6;
    g_HighwayRoadUvEighths[i].u0 = midU + 1;
    g_HighwayRoadUvEighths[i].v0 = midV + 1;
    g_HighwayRoadUvEighths[i].u1 = right;
    g_HighwayRoadUvEighths[i].v1 = midV + 1;
    g_HighwayRoadUvEighths[i].u2 = midU + 1;
    g_HighwayRoadUvEighths[i].v2 = src->v2;
    g_HighwayRoadUvEighths[i].u3 = right;
    g_HighwayRoadUvEighths[i].v3 = src->v2;
    g_HighwayRoadUvEighths[i].clut = src->clut;
    g_HighwayRoadUvEighths[i].tpage = src->tpage;
    i = index * 8 + 7;
    g_HighwayRoadUvEighths[i].u0 = right + 1;
    g_HighwayRoadUvEighths[i].v0 = midV + 1;
    g_HighwayRoadUvEighths[i].u1 = src->u1;
    g_HighwayRoadUvEighths[i].v1 = midV + 1;
    g_HighwayRoadUvEighths[i].u2 = right + 1;
    g_HighwayRoadUvEighths[i].v2 = src->v2;
    g_HighwayRoadUvEighths[i].u3 = src->u1;
    g_HighwayRoadUvEighths[i].v3 = src->v2;
    g_HighwayRoadUvEighths[i].clut = src->clut;
    g_HighwayRoadUvEighths[i].tpage = src->tpage;
}

void HighwayQuadUvSplitH(HighwayQuadUv* src, s32 index) {
    u8 mid;

    mid = (src->u0 + src->u1 - 1) >> 1;
    g_HighwayRoadUvHalves[index * 2].u0 = src->u0;
    g_HighwayRoadUvHalves[index * 2].v0 = src->v0;
    g_HighwayRoadUvHalves[index * 2].u1 = mid;
    g_HighwayRoadUvHalves[index * 2].v1 = src->v1;
    g_HighwayRoadUvHalves[index * 2].u2 = src->u2;
    g_HighwayRoadUvHalves[index * 2].v2 = src->v2;
    g_HighwayRoadUvHalves[index * 2].u3 = mid;
    g_HighwayRoadUvHalves[index * 2].v3 = src->v3;
    g_HighwayRoadUvHalves[index * 2].clut = src->clut;
    g_HighwayRoadUvHalves[index * 2].tpage = src->tpage;
    g_HighwayRoadUvHalves[index * 2 + 1].u0 = mid;
    g_HighwayRoadUvHalves[index * 2 + 1].v0 = src->v0;
    g_HighwayRoadUvHalves[index * 2 + 1].u1 = src->u1;
    g_HighwayRoadUvHalves[index * 2 + 1].v1 = src->v1;
    g_HighwayRoadUvHalves[index * 2 + 1].u2 = mid;
    g_HighwayRoadUvHalves[index * 2 + 1].v2 = src->v2;
    g_HighwayRoadUvHalves[index * 2 + 1].u3 = src->u3;
    g_HighwayRoadUvHalves[index * 2 + 1].v3 = src->v3;
    g_HighwayRoadUvHalves[index * 2 + 1].clut = src->clut;
    g_HighwayRoadUvHalves[index * 2 + 1].tpage = src->tpage;
}

void HighwayTrackReset(void) {
    s32 i;
    s32 j;

    HighwayPropsInit();
    if (!g_HighwayArcadeMode) {
        g_HighwayTrackCommand = g_HighwayCourses[0].trackCommands;
    }
    if (g_HighwayArcadeMode == 1) {
        g_HighwayTrackCommand = g_HighwayCourses[1].trackCommands;
    }
    g_HighwayTrackGenYaw = 0;
    D_801163EC = 0;
    g_HighwayTrackGenPos.vx = 0;
    g_HighwayTrackGenPos.vy = 0;
    g_HighwayTrackGenPos.vz = 0;
    g_HighwayTrackNextCommand = 1;
    g_HighwayTrackCommandIndex = 0;
    g_HighwayTrackTurnStep = 0;
    g_HighwayTrackGenYaw = 0;
    g_HighwayTrackPitchTo = 0;
    g_HighwayTrackPitchFrom = 0;
    g_HighwayTrackPitchStep = 0;
    g_HighwayTrackPitchAcc = 0;
    g_HighwayTrackRollTo = 0;
    g_HighwayTrackRollFrom = 0;
    g_HighwayTrackRollStep = 0;
    g_HighwayTrackRollAcc = 0;
    if (!g_HighwayArcadeMode) {
        g_HighwayRoadPatterns = g_HighwayCourses[0].unk30;
    }
    if (g_HighwayArcadeMode == 1) {
        g_HighwayRoadPatterns = g_HighwayCourses[1].unk30;
    }
    g_HighwayRoadPatternLen = 0;
    D_80110CCC = 0;
    g_HighwayRoadPatternPos = 0;
    D_80110CC8 = 0;
    for (i = 0; i < LEN(g_HighwayRoad); i++) {
        for (j = LEN(g_HighwayRoad[i].unk46) - 1; j >= 0; j--) {
            g_HighwayRoad[i].unk46[j] = 0;
        }
    }
    for (i = 0; i < LEN(g_HighwayPropAnchors); i++) {
        g_HighwayPropAnchors[i].vx = 0;
        g_HighwayPropAnchors[i].vy = 0;
        g_HighwayPropAnchors[i].vz = 0;
    }
    D_8010EBC8 = 0;
}

void HighwayTrackGenerateSegment(void) {
    SVECTOR forward = g_HighwaySegmentStep;
    SVECTOR step = g_HighwaySegmentStep;
    SVECTOR rot = g_HighwayZeroRot;
    MATRIX m;
    s32 pitch;
    s32 roll;
    s32 radius;
    s32 dist;
    HighwayTrackCommand* p;

    if (g_HighwayTrackNextCommand) {
        g_HighwayTrackRollFrom = g_HighwayTrackRollTo;
        g_HighwayTrackPitchFrom = g_HighwayTrackPitchTo;
        p = g_HighwayTrackCommand;
        g_HighwayTrackCmdLeft = p->length;
        g_HighwayTrackRollAcc = 0;
        g_HighwayTrackPitchAcc = 0;
        g_HighwayTrackTurn = p->turn;
        g_HighwayTrackWidth = p->width;
        g_HighwayTrackPitchTo = p->pitch;
        g_HighwayTrackRollTo = p->roll;
        g_HighwayTrackPattern = p->pattern;
        radius = p->radius;
        dist = csqrt(((radius * radius) << 12) - 0x400);
        g_HighwayTrackPitchStep = ((g_HighwayTrackPitchTo - g_HighwayTrackPitchFrom) << 16) / g_HighwayTrackCmdLeft;
        g_HighwayTrackRollStep = ((g_HighwayTrackRollTo - g_HighwayTrackRollFrom) << 16) / g_HighwayTrackCmdLeft;
        g_HighwayTrackTurnStep = ratan2(0x800, dist) * 2;
        g_HighwayTrackNextCommand = 0;
        D_801163EC = 0;
        g_HighwayTrackRadius = radius;
        g_HighwayRoadPatternPos = 0;
        D_80110CC8 = 0;
        g_HighwayRoadPatternLen = (g_HighwayRoadPatterns + g_HighwayTrackPattern)[1];
        D_80110CCC = (g_HighwayRoadPatterns + g_HighwayTrackPattern)[1];
        D_80110BC0 = g_HighwayRoadPatternPtr =
            g_HighwayRoadPatterns + (g_HighwayRoadPatterns + g_HighwayRoadPatterns[0])[g_HighwayTrackPattern * 2 + 1];
        g_HighwayTrackCommand++;
        g_HighwayTrackCommandIndex++;
    }
    g_HighwayRoadPatternPos++;
    D_80110CC8++;
    g_HighwayRoadPatternPos %= g_HighwayRoadPatternLen;
    D_80110CC8 %= g_HighwayRoadPatternLen;
    g_HighwayRoad[g_HighwayRoadHead].unk46[0] = g_HighwayRoadPatternPtr[g_HighwayRoadPatternPos];
    if (D_8010EBC8 == 1) {
        g_HighwayRoad[g_HighwayRoadHead].unk46[9] = 1;
    }
    if (g_HighwayRoad[g_HighwayRoadHead].unk46[0] == 4) {
        D_8010EBC8 = 1;
    }
    if (g_HighwayTrackTurn == 1) {
        g_HighwayTrackGenYaw += g_HighwayTrackTurnStep;
        D_801163EC += g_HighwayTrackTurnStep;
    }
    if (g_HighwayTrackTurn == 2) {
        g_HighwayTrackGenYaw -= g_HighwayTrackTurnStep;
        D_801163EC += g_HighwayTrackTurnStep;
    }
    if (!--g_HighwayTrackCmdLeft) {
        g_HighwayTrackNextCommand = 1;
    }
    if (g_HighwayTrackGenYaw < 0) {
        g_HighwayTrackGenYaw += 0x1000;
    }
    g_HighwayTrackGenYaw %= 0x1000;
    g_HighwayTrackRollAcc += g_HighwayTrackRollStep;
    g_HighwayTrackPitchAcc += g_HighwayTrackPitchStep;
    pitch = (g_HighwayTrackPitchAcc >> 16) + g_HighwayTrackPitchFrom;
    roll = (g_HighwayTrackRollAcc >> 16) + g_HighwayTrackRollFrom;
    rot.vx = pitch;
    rot.vy = g_HighwayTrackGenYaw;
    rot.vz = 0;
    RotMatrixYXZ(&rot, &m);
    ApplyMatrixSV(&m, &forward, &step);
    g_HighwayTrackGenPos.vx += step.vx;
    g_HighwayTrackGenPos.vy += step.vy;
    g_HighwayTrackGenPos.vz += step.vz;
    g_HighwayRoad[g_HighwayRoadHead].pos.vx = g_HighwayTrackGenPos.vx;
    g_HighwayRoad[g_HighwayRoadHead].pos.vy = g_HighwayTrackGenPos.vy;
    g_HighwayRoad[g_HighwayRoadHead].pos.vz = g_HighwayTrackGenPos.vz;
    g_HighwayRoad[g_HighwayRoadHead].radius = g_HighwayTrackRadius;
    g_HighwayRoad[g_HighwayRoadHead].turn = g_HighwayTrackTurn;
    g_HighwayRoad[g_HighwayRoadHead].width = g_HighwayTrackWidth;
    g_HighwayRoad[g_HighwayRoadHead].rot.vx = 0;
    g_HighwayRoad[g_HighwayRoadHead].rot.vy = g_HighwayTrackGenYaw;
    g_HighwayRoad[g_HighwayRoadHead].rot.vz = roll;
    RotMatrix(&g_HighwayRoad[g_HighwayRoadHead].rot, &g_HighwayRoad[g_HighwayRoadHead].m);
    g_HighwayRoad[g_HighwayRoadHead].rot.vx = pitch;
    HighwayTrackSpawnProps();
    g_HighwayRoadHead = (g_HighwayRoadHead + 1) % LEN(g_HighwayRoad);
}

// Free the nodes attached to a track segment.
void HighwayTrackFreeSegment(void) {
    s32 i;

    for (i = 0; i < g_HighwayRoad[g_HighwayRoadHead].nodeCount; i++) {
        HighwayNodeFree(g_HighwayRoad[g_HighwayRoadHead].nodes[i]);
    }
    g_HighwayRoad[g_HighwayRoadHead].nodeCount = 0;
}

void HighwayTrackSamplePos(s32 pos, s32 offset, VECTOR* out) {
    HighwayRoadSegment* cur;
    HighwayRoadSegment* next;
    s32 frac;

    g_HighwayScratchpad->unk30.vx = offset << 18;
    g_HighwayScratchpad->unk30.vy = 0;
    g_HighwayScratchpad->unk30.vz = 0;
    frac = pos & 0xFF;
    cur = &g_HighwayRoad[(pos >> 8) % LEN(g_HighwayRoad)];
    next = &g_HighwayRoad[((pos >> 8) + 1) % LEN(g_HighwayRoad)];
    ApplyMatrixLV(&cur->m, &g_HighwayScratchpad->unk30, &g_HighwayScratchpad->unk40);
    ApplyMatrixLV(&next->m, &g_HighwayScratchpad->unk30, &g_HighwayScratchpad->unk50);
    g_HighwayScratchpad->unk60.vx = (g_HighwayScratchpad->unk40.vx >> 18) + cur->pos.vx;
    g_HighwayScratchpad->unk60.vy = (g_HighwayScratchpad->unk40.vy >> 18) + cur->pos.vy;
    g_HighwayScratchpad->unk60.vz = (g_HighwayScratchpad->unk40.vz >> 18) + cur->pos.vz;
    g_HighwayScratchpad->unk70.vx = (g_HighwayScratchpad->unk50.vx >> 18) + next->pos.vx;
    g_HighwayScratchpad->unk70.vy = (g_HighwayScratchpad->unk50.vy >> 18) + next->pos.vy;
    g_HighwayScratchpad->unk70.vz = (g_HighwayScratchpad->unk50.vz >> 18) + next->pos.vz;
    out->vx =
        (((g_HighwayScratchpad->unk70.vx - g_HighwayScratchpad->unk60.vx) * frac) >> 8) + g_HighwayScratchpad->unk60.vx;
    out->vy =
        (((g_HighwayScratchpad->unk70.vy - g_HighwayScratchpad->unk60.vy) * frac) >> 8) + g_HighwayScratchpad->unk60.vy;
    out->vz =
        (((g_HighwayScratchpad->unk70.vz - g_HighwayScratchpad->unk60.vz) * frac) >> 8) + g_HighwayScratchpad->unk60.vz;
}

void HighwayTrackSample(s32 pos, s32 offset, VECTOR* out, SVECTOR* rot) {
    HighwayRoadSegment* cur;
    HighwayRoadSegment* next;
    s32 frac;
    s16 a;
    s16 b;

    g_HighwayScratchpad->unk30.vx = offset << 18;
    g_HighwayScratchpad->unk30.vy = 0;
    g_HighwayScratchpad->unk30.vz = 0;
    frac = pos & 0xFF;
    cur = &g_HighwayRoad[(pos >> 8) % LEN(g_HighwayRoad)];
    next = &g_HighwayRoad[((pos >> 8) + 1) % LEN(g_HighwayRoad)];
    ApplyMatrixLV(&cur->m, &g_HighwayScratchpad->unk30, &g_HighwayScratchpad->unk40);
    ApplyMatrixLV(&next->m, &g_HighwayScratchpad->unk30, &g_HighwayScratchpad->unk50);
    g_HighwayScratchpad->unk60.vx = (g_HighwayScratchpad->unk40.vx >> 18) + cur->pos.vx;
    g_HighwayScratchpad->unk60.vy = (g_HighwayScratchpad->unk40.vy >> 18) + cur->pos.vy;
    g_HighwayScratchpad->unk60.vz = (g_HighwayScratchpad->unk40.vz >> 18) + cur->pos.vz;
    g_HighwayScratchpad->unk70.vx = (g_HighwayScratchpad->unk50.vx >> 18) + next->pos.vx;
    g_HighwayScratchpad->unk70.vy = (g_HighwayScratchpad->unk50.vy >> 18) + next->pos.vy;
    g_HighwayScratchpad->unk70.vz = (g_HighwayScratchpad->unk50.vz >> 18) + next->pos.vz;
    out->vx =
        (((g_HighwayScratchpad->unk70.vx - g_HighwayScratchpad->unk60.vx) * frac) >> 8) + g_HighwayScratchpad->unk60.vx;
    out->vy =
        (((g_HighwayScratchpad->unk70.vy - g_HighwayScratchpad->unk60.vy) * frac) >> 8) + g_HighwayScratchpad->unk60.vy;
    out->vz =
        (((g_HighwayScratchpad->unk70.vz - g_HighwayScratchpad->unk60.vz) * frac) >> 8) + g_HighwayScratchpad->unk60.vz;
    rot->vx = cur->rot.vx;
    rot->vy = cur->rot.vy;
    rot->vz = cur->rot.vz;

    a = cur->rot.vx;
    b = next->rot.vx;
    if (b - cur->rot.vx > 0x800) {
        a += 0x1000;
    }
    if (a - b > 0x800) {
        b += 0x1000;
    }
    rot->vx += ((b - a) * frac) >> 8;

    a = cur->rot.vy;
    b = next->rot.vy;
    if (b - cur->rot.vy > 0x800) {
        a += 0x1000;
    }
    if (a - b > 0x800) {
        b += 0x1000;
    }
    rot->vy += ((b - a) * frac) >> 8;

    a = cur->rot.vz;
    b = next->rot.vz;
    if (b - cur->rot.vz > 0x800) {
        a += 0x1000;
    }
    if (a - b > 0x800) {
        b += 0x1000;
    }
    rot->vz += ((b - a) * frac) >> 8;
}

void HighwayTrackSampleNoRoll(s32 pos, s32 offset, VECTOR* out, SVECTOR* rot) {
    HighwayRoadSegment* cur;
    HighwayRoadSegment* next;
    s32 frac;
    s16 a;
    s16 b;

    g_HighwayScratchpad->unk30.vx = offset << 16;
    g_HighwayScratchpad->unk30.vy = 0;
    g_HighwayScratchpad->unk30.vz = 0;
    frac = pos & 0xFF;
    cur = &g_HighwayRoad[(pos >> 8) % LEN(g_HighwayRoad)];
    next = &g_HighwayRoad[((pos >> 8) + 1) % LEN(g_HighwayRoad)];
    ApplyMatrixLV(&cur->m, &g_HighwayScratchpad->unk30, &g_HighwayScratchpad->unk40);
    ApplyMatrixLV(&next->m, &g_HighwayScratchpad->unk30, &g_HighwayScratchpad->unk50);
    g_HighwayScratchpad->unk60.vx = (g_HighwayScratchpad->unk40.vx >> 16) + cur->pos.vx;
    g_HighwayScratchpad->unk60.vy = (g_HighwayScratchpad->unk40.vy >> 16) + cur->pos.vy;
    g_HighwayScratchpad->unk60.vz = (g_HighwayScratchpad->unk40.vz >> 16) + cur->pos.vz;
    g_HighwayScratchpad->unk70.vx = (g_HighwayScratchpad->unk50.vx >> 16) + next->pos.vx;
    g_HighwayScratchpad->unk70.vy = (g_HighwayScratchpad->unk50.vy >> 16) + next->pos.vy;
    g_HighwayScratchpad->unk70.vz = (g_HighwayScratchpad->unk50.vz >> 16) + next->pos.vz;
    out->vx =
        (((g_HighwayScratchpad->unk70.vx - g_HighwayScratchpad->unk60.vx) * frac) >> 8) + g_HighwayScratchpad->unk60.vx;
    out->vy =
        (((g_HighwayScratchpad->unk70.vy - g_HighwayScratchpad->unk60.vy) * frac) >> 8) + g_HighwayScratchpad->unk60.vy;
    out->vz =
        (((g_HighwayScratchpad->unk70.vz - g_HighwayScratchpad->unk60.vz) * frac) >> 8) + g_HighwayScratchpad->unk60.vz;
    rot->vx = cur->rot.vx;
    rot->vy = cur->rot.vy;
    rot->vz = 0;

    a = cur->rot.vx;
    b = next->rot.vx;
    if (b - cur->rot.vx > 0x800) {
        a += 0x1000;
    }
    if (a - b > 0x800) {
        b += 0x1000;
    }
    rot->vx += ((b - a) * frac) >> 8;

    a = cur->rot.vy;
    b = next->rot.vy;
    if (b - cur->rot.vy > 0x800) {
        a += 0x1000;
    }
    if (a - b > 0x800) {
        b += 0x1000;
    }
    rot->vy += ((b - a) * frac) >> 8;
}

void HighwayTrackSpawnProps(void) {
    VECTOR pos;
    SVECTOR rot;
    SVECTOR unused;
    u8 modelId;
    s16 offset;
    s16 height;
    u16 flags;
    u16 yaw;
    s32 count;
    s32 i;

    count = 0;
    unused = g_HighwayZeroRot;
    for (i = 0; i < LEN(g_HighwayPropAnchors); i++) {
        HighwayPropScriptStep(i, &modelId, &offset, &height, &flags, &yaw);
        if (!modelId) {
            if (flags & 2) {
                HighwayTrackSampleNoRoll(g_HighwayRoadHead << 8, offset, &pos, &rot);
                g_HighwayPropAnchors[i].vx = pos.vx;
                g_HighwayPropAnchors[i].vy = pos.vy;
                g_HighwayPropAnchors[i].vz = pos.vz;
            }
        }
        if (modelId) {
            if (i < 6) {
                HighwayTrackSampleNoRoll(g_HighwayRoadHead << 8, offset, &pos, &rot);
                rot.vx = 0;
                rot.vz = 0;
                rot.vy += yaw;
            } else {
                HighwayTrackSample(g_HighwayRoadHead << 8, offset, &pos, &rot);
                rot.vy += yaw;
            }
            if (flags & 1) {
                pos.vy = g_HighwayPropAnchors[i].vy - height;
            } else {
                pos.vy -= height;
            }
            g_HighwayRoad[g_HighwayRoadHead].nodes[count++] =
                HighwayNodeAllocYXZ(modelId, &g_HighwayRootNode, &pos, &rot);
            g_HighwayRoad[g_HighwayRoadHead].nodeCount++;
        }
    }
}
