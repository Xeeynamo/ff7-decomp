//! PSYQ=3.3

#include "highway_private.h"

void HighwayDrawGauge(HighwayBuffer* db, s16 index, s32 flash, u8 state) {
    POLY_FT4* poly;
    s32 len;
    s32 barR;
    s32 barG;
    u8 r;
    u8 g;
    u8 b;

    if (!g_HighwayGauges[index].maxHp ||
        (len = (g_HighwayGauges[index].hp * 64) / g_HighwayGauges[index].maxHp - 1) < 0) {
        len = 0;
    }
    // the loop notes of this do-while keep the color setup ahead of the
    // g_HighwayGauges load below when scheduling (probably a macro originally)
    do {
        barR = flash * 2;
        barG = 0x80 - flash;
    } while (0);
    if (!g_HighwayGauges[index].hp) {
        r = 0x30;
        g = 0x30;
        b = 0x30;
    } else {
        r = 0x80;
        g = 0x80;
        b = 0x80;
    }
    poly = db->prims.ft4Cursor;
    setXY4(poly, g_HighwayGaugeLayout[index].unk4, g_HighwayGaugeLayout[index].unk6,
           g_HighwayGaugeLayout[index].unk4 + len, g_HighwayGaugeLayout[index].unk6, g_HighwayGaugeLayout[index].unk4,
           g_HighwayGaugeLayout[index].unk6 + 4, g_HighwayGaugeLayout[index].unk4 + len,
           g_HighwayGaugeLayout[index].unk6 + 4);
    setRGB0(poly, barR, barG, 0);
    setUV4(poly, 0, 0x38, 0, 0x38, 0, 0x3D, 0, 0x3D);
    poly->tpage = g_HighwayGaugeTPage;
    poly->clut = g_HighwayGaugeClut;
    addPrim(&db->hudOt[3], poly);
    poly++;
    setXY4(poly, g_HighwayGaugeLayout[index].x + g_HighwayGaugeLayout[index].unk13, g_HighwayGaugeLayout[index].y + 1,
           g_HighwayGaugeLayout[index].x + g_HighwayGaugeLayout[index].unk13 + g_HighwayGaugeLayout[index].unk12,
           g_HighwayGaugeLayout[index].y + 1, g_HighwayGaugeLayout[index].x + g_HighwayGaugeLayout[index].unk13,
           g_HighwayGaugeLayout[index].y + (g_HighwayGaugeLayout[index].unk12 + 1),
           g_HighwayGaugeLayout[index].x + g_HighwayGaugeLayout[index].unk13 + g_HighwayGaugeLayout[index].unk12,
           g_HighwayGaugeLayout[index].y + (g_HighwayGaugeLayout[index].unk12 + 1));
    setRGB0(poly, r, g, b);
    setUV4(poly, g_HighwayGaugeLayout[index].unk8[state], g_HighwayGaugeLayout[index].unkC[state],
           g_HighwayGaugeLayout[index].unk8[state] + g_HighwayGaugeLayout[index].unk12,
           g_HighwayGaugeLayout[index].unkC[state], g_HighwayGaugeLayout[index].unk8[state],
           g_HighwayGaugeLayout[index].unkC[state] + g_HighwayGaugeLayout[index].unk12,
           g_HighwayGaugeLayout[index].unk8[state] + g_HighwayGaugeLayout[index].unk12,
           g_HighwayGaugeLayout[index].unkC[state] + g_HighwayGaugeLayout[index].unk12);
    poly->tpage = g_HighwayGaugeTPage;
    poly->clut = g_HighwayGaugeClut;
    addPrim(&db->hudOt[3], poly);
    poly++;
    setXYWH(poly, g_HighwayGaugeLayout[index].x, g_HighwayGaugeLayout[index].y, g_HighwayGaugeLayout[index].w,
            g_HighwayGaugeLayout[index].h);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUVWH(poly, g_HighwayGaugeLayout[index].unk8[0], g_HighwayGaugeLayout[index].unkC[0],
            g_HighwayGaugeLayout[index].w, g_HighwayGaugeLayout[index].h);
    poly->tpage = g_HighwayGaugeTPage;
    poly->clut = g_HighwayGaugeClut;
    addPrim(&db->hudOt[3], poly);
    poly++;
    db->prims.ft4Cursor = poly;
}

void HighwayDrawNumber(HighwayBuffer* db, s32 value, s32 x, s32 y, s16 zeroPad) {
    POLY_FT4* poly;
    s32 digit;
    s32 power;
    s32 remain;
    s32 i;
    u16 startX;
    s32 w;
    s32 left;
    u8 leading;

    startX = x;
    power = 10000;
    leading = 1;
    remain = value + 1;
    poly = db->prims.ft4Cursor;
    for (i = 0; i < 5; i++) {
        digit = 0;
        while (remain > power) {
            remain -= power;
            digit++;
        }
        if (digit) {
            leading = 0;
        }
        if (!value && power == 1) {
            leading = 0;
        }
        if (zeroPad == 1 || digit || !leading) {
            left = x + i * 12;
            w = i * 12 + 16;
            setXY4(poly, left, y, startX + w, y, left, y + 16, startX + w, y + 16);
            setRGB0(poly, 0x80, 0x80, 0x80);
            setUV4(poly, digit * 16, 0xC8, digit * 16 + 15, 0xC8, digit * 16, 0xD7, digit * 16 + 15, 0xD7);
            poly->tpage = g_HighwaySpriteTPage[0];
            poly->clut = g_HighwaySpriteClut[0];
            SetSemiTrans(poly, 0);
            addPrim(&db->hudOt[3], poly);
            poly++;
        }
        power /= 10;
    }
    db->prims.ft4Cursor = poly;
}

void HighwayDrawNumberSmall(HighwayBuffer* db, s32 value, s32 x, s32 y, s16 zeroPad) {
    POLY_FT4* poly;
    s32 digit;
    s32 power;
    s32 remain;
    s32 i;
    u16 startX;
    s32 left;
    s32 w;
    u8 leading;

    startX = x;
    power = 10000;
    leading = 1;
    remain = value + 1;
    poly = db->prims.ft4Cursor;
    for (i = 0; i < 5; i++) {
        digit = 0;
        while (remain > power) {
            remain -= power;
            digit++;
        }
        if (digit) {
            leading = 0;
        }
        if (!value && power == 1) {
            leading = 0;
        }
        if (zeroPad == 1 || digit || !leading) {
            left = x + i * 8;
            poly->x0 = left;
            poly->y0 = y;
            w = i * 8 + 8;
            poly->x1 = startX + w;
            poly->y1 = y;
            poly->x2 = left;
            poly->y2 = y + 16;
            poly->x3 = startX + w;
            poly->y3 = y + 16;
            setRGB0(poly, 0x80, 0x80, 0x80);
            setUV4(poly, digit * 8, 0xE0, digit * 8 + 8, 0xE0, digit * 8, 0xEF, digit * 8 + 8, 0xEF);
            poly->tpage = g_HighwaySpriteTPage[0];
            poly->clut = g_HighwaySpriteClut[0];
            SetSemiTrans(poly, 0);
            addPrim(&db->hudOt[3], poly);
            poly++;
        }
        power /= 10;
    }
    db->prims.ft4Cursor = poly;
}

// Draw a textured sprite, like JetDrawSprite.
void HighwayDrawSprite(HighwayBuffer* db, s16 sprite, s16 x, s16 y, s16 w, s16 h, u8 u, u8 v, u8 textureWidth,
                       u8 textureHeight, u8 semiTrans) {
    POLY_FT4* poly;

    poly = db->prims.ft4Cursor;
    setXYWH(poly, x, y, w, h);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUVWH(poly, u, v, textureWidth, textureHeight);
    poly->tpage = g_HighwaySpriteTPage[sprite];
    poly->clut = g_HighwaySpriteClut[sprite];
    SetSemiTrans(poly, semiTrans);
    addPrim(&db->hudOt[3], poly);
    poly++;
    db->prims.ft4Cursor = poly;
}

void HighwayKawaiModelsInit(void) {
    s32 i;
    s32 j;
    s32 k;
    s32 count;

    D_800B6D4C = 0;
    D_800BE53C = 0;
    D_80110A94 = 0;
    g_HighwayKawaiPktCursor = (u8*)0x801C0000;
    g_HighwayKawai = (HighwayKawaiHeader*)0x801A0000;
    g_HighwayKawaiModels = g_HighwayKawai->models;
    for (i = 0; i < g_HighwayKawai->count; i++) {
        g_HighwayKawaiPktCursor = func_800B0698(&g_HighwayKawaiModels[i], g_HighwayKawaiPktCursor, i);
        g_HighwayKawaiModels[i].flags = 0;
        g_HighwayKawaiStates[i].unkB0 = g_HighwayKawaiModels[i].animationCount - 1;
        g_HighwayKawaiModels[i].rotationX = 64;
        g_HighwayKawaiModels[i].rotationY = 0;
        g_HighwayKawaiModels[i].rotationZ = 128;
    }
    for (i = 0; i < g_HighwayKawai->count; i++) {
        for (k = LEN(g_HighwayKawaiStates[i].lastFrame) - 1; k >= 0; k--) {
            g_HighwayKawaiStates[i].lastFrame[k] = 0;
        }
        count = g_HighwayKawaiModels[i].animationCount;
        for (j = 0; j < count; j++) {
            g_HighwayKawaiAnim =
                (FieldModelAnimation*)(g_HighwayKawaiModels[i].modelData + g_HighwayKawaiModels[i].animationOffset) + j;
            g_HighwayKawaiStates[i].lastFrame[j] = g_HighwayKawaiAnim->frameCount - 1;
        }
        g_HighwayKawaiStates[i].m.t[0] = 0;
        g_HighwayKawaiStates[i].m.t[1] = 0;
        g_HighwayKawaiStates[i].m.t[2] = 0;
        g_HighwayKawaiStates[i].unk98 = 0;
        g_HighwayKawaiStates[i].unk9C = 0;
        g_HighwayKawaiStates[i].animation = 0;
        g_HighwayKawaiStates[i].frame = 0;
        g_HighwayKawaiStates[i].unkA8 = 0;
        g_HighwayKawaiStates[i].flash = 0;
        g_HighwayKawaiStates[i].unkAE = 0;
    }
}

void HighwayKawaiModelsUpdate(void) {
    HighwayKawaiState* state;
    s32 i;

    state = g_HighwayKawaiStates;
    for (i = 0; i < g_HighwayKawai->count; i++, state++) {
        *getScratchAddr(0) = 3;
        func_800B1E98(&g_HighwayKawaiModels[i], &state->m, state->animation, state->frame);
    }
    state = g_HighwayKawaiStates;
    for (i = 0; i < g_HighwayKawai->count; i++, state++) {
        if (state->unkAE == 1) {
            KAWAI_WRITE_S16(g_HighwayKawaiLight, 0, state->flash);
            KAWAI_WRITE_S16(g_HighwayKawaiLight, 2, state->flash);
            KAWAI_WRITE_S16(g_HighwayKawaiLight, 4, state->flash);
            g_HighwayKawaiLight[6] = 1;
            func_800B2E58(&g_HighwayKawaiModels[i], g_HighwayKawaiLight);
            state->unkAE = 0;
        }
        if (state->flash > 0) {
            state->flash -= 0x80;
            KAWAI_WRITE_S16(g_HighwayKawaiLight, 0, state->flash);
            KAWAI_WRITE_S16(g_HighwayKawaiLight, 2, state->flash);
            KAWAI_WRITE_S16(g_HighwayKawaiLight, 4, state->flash);
            g_HighwayKawaiLight[6] = 1;
            func_800B2E58(&g_HighwayKawaiModels[i], g_HighwayKawaiLight);
            if (state->flash < 0) {
                state->flash = 0;
            }
            state->unkAE = 1;
        }
    }
    for (i = 0; i < g_HighwayKawai->count; i++) {
        func_800B12B0(&g_HighwayKawaiModels[i]);
    }
}
