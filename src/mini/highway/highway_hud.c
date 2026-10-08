//! PSYQ=3.3

#include "highway_private.h"

// Declared with int parameters: the gauge loop below passes its arguments unpromoted.
void HighwayDrawGauge(HighwayBuffer* db, s32 index, s32 flash, s32 state);

u8 HighwayDrawFade(HighwayBuffer* db, u8 mode) {
    POLY_G4* fade;
    POLY_FT4* poly;

    switch (mode) {
    case 1:
        g_HighwayFadeLevel = 0xFC;
        mode = 3;
        break;
    case 2:
        g_HighwayFadeLevel = 0;
        mode = 4;
        break;
    case 3:
        g_HighwayFadeLevel -= 4;
        if (!g_HighwayFadeLevel) {
            mode = 0;
        }
        break;
    case 4:
        if (g_HighwayFadeLevel < 0xF0) {
            g_HighwayFadeLevel += 4;
        } else {
            g_HighwayFadeLevel = 0xFF;
        }
        break;
    }
    fade = db->prims.g4Cursor;
    poly = db->prims.ft4Cursor;
    setRGB0(fade, g_HighwayFadeLevel, g_HighwayFadeLevel, g_HighwayFadeLevel);
    setRGB1(fade, g_HighwayFadeLevel, g_HighwayFadeLevel, g_HighwayFadeLevel);
    setRGB2(fade, g_HighwayFadeLevel, g_HighwayFadeLevel, g_HighwayFadeLevel);
    setXY4(fade, 0, 0, 0x140, 0, 0, 0xF0, 0x140, 0xF0);
    setRGB3(fade, g_HighwayFadeLevel, g_HighwayFadeLevel, g_HighwayFadeLevel);
    SetSemiTrans(fade, 1);
    addPrim(&db->hudOt[1], fade);
    fade++;
    setXY4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    setRGB0(poly, 0, 0, 0);
    setUV4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    poly->tpage = GetTPage(1, 2, 0x140, 0xE0);
    poly->clut = GetClut(0x10, 0x1E4);
    addPrim(&db->hudOt[1], poly);
    poly++;
    db->prims.ft4Cursor = poly;
    db->prims.g4Cursor = fade;
    return mode;
}

void HighwayGaugeDamage(u8 index, s16 damage) {
    u8 i;
    u8 next;
    u8 found;

    if (g_HighwayGauges[2].hp < 30 && index == 2) {
        damage = 0;
    }
    g_HighwayGauges[index].unk8 = 0x70;
    g_HighwayGauges[index].unk4 += damage;
    if (damage < g_HighwayGauges[index].hp) {
        g_HighwayGauges[index].hp -= damage;
    } else {
        g_HighwayGauges[index].hp = 0;
    }
    if (!g_HighwayGauges[index].hp) {
        found = 0;
        for (i = 0; i < 4; i++) {
            next = (index + i) % 4 + 1;
            if (g_HighwayGauges[next].hp) {
                index = next;
                found = 1;
            }
        }
        if (found == 1) {
            g_HighwayGauges[index].unk8 = 0x70;
            g_HighwayGauges[index].unk4 += damage;
            if (damage < g_HighwayGauges[index].hp) {
                g_HighwayGauges[index].hp -= damage;
            } else {
                g_HighwayGauges[index].hp = 0;
            }
        }
    }
}

void HighwayDrawGauges(HighwayBuffer* db) {
    s32 i;

    for (i = 0; i < LEN(g_HighwayGauges); i++) {
        if (rand() % 30 == 0 && g_HighwayGauges[i].unk9 == 1 && g_HighwayGauges[i].hp > 10) {
            g_HighwayGauges[i].unk9 = 3;
        }
        if (g_HighwayGauges[i].unk9 >= 2) {
            g_HighwayGauges[i].unk9--;
        }
        HighwayDrawGauge(db, i, g_HighwayGauges[i].unk8, g_HighwayGauges[i].unk9);
        if (g_HighwayGauges[i].unk8) {
            g_HighwayGauges[i].unk8 -= 8;
        }
    }
}
