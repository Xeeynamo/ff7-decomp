//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <game.h>
#include <libgte.h>
#include "chocobo_private.h"
#include <libetc.h>

extern s32 D_800B22D0[];
extern s32 D_800B232C[];
extern s32 D_800B2380[];
extern s32 D_800B23C0[];
extern ChocoboPrizePtr D_800B7458[1];
extern Unk800B7480 D_800B7480[5][3];
extern u8* D_800F502C;

#ifdef __psyz
#define SPRT_TAG(len) 0, len
#else
#define SPRT_TAG(len) (len) << 24
#endif

const SPRT D_800A00DC[11] = {
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 14, 14, 0, 0, 0x79A4, 168, 24},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 191, 10, 0, 32, 0x79A4, 80, 24},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 194, 40, 136, 32, 0x79A4, 64, 9},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 194, 63, 136, 48, 0x79A4, 64, 9},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 194, 86, 152, 64, 0x79A4, 32, 9},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 11, 155, 216, 64, 0x79A4, 32, 12},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 295, 48, 216, 48, 0x79A4, 24, 12},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 290, 218, 216, 32, 0x79A4, 32, 12},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 256, 192, 184, 240, 0x79E4, 26, 16},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 276, 10, 80, 32, 0x7AE4, 28, 32},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 286, 181, 0, 208, 0x79E4, 20, 24},
};

const SPRT D_800A01B8[11] = {
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 11, 155, 216, 64, 0x79A4, 32, 12},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 187, 36, 176, 112, 0x7AA4, 32, 26},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 187, 68, 176, 143, 0x7AA4, 32, 24},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 195, 108, 208, 112, 0x79E4, 28, 16},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 195, 139, 208, 128, 0x79E4, 28, 16},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 195, 168, 176, 176, 0x79E4, 28, 16},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 195, 198, 176, 192, 0x79E4, 28, 16},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 20, 10, 0, 64, 0x79A4, 144, 24},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 220, 37, 0, 88, 0x7A24, 24, 16},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 220, 70, 32, 88, 0x7A24, 16, 16},
    {SPRT_TAG(4), 0x80, 0x80, 0x80, 0x66, 232, 15, 192, 216, 0x7D24, 12, 10},
};

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_bet", func_800A9D94);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_bet", func_800AAC00);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_bet", func_800AAF1C);

void ChocoboSetPrizeTable(void) {
    s32 ids[16];
    s32 groups[16];
    ChocoboPrizePtr* table;
    s32* out;
    s32 i;
    s32 j;
    s32 nPrize;
    s32 prizeIndex;
    s32 x;
    s32 k;
    s32 locked;
    s32 strIndex;
    s32 id;
    s32 swap;
    ChocoboPrize* e;

    switch (D_800F5078.unk20) {
    case 0:
        D_800B7458->entries = (ChocoboPrize*)&D_800B23C0;
        break;
    case 1:
        D_800B7458->entries = (ChocoboPrize*)&D_800B2380;
        break;
    case 2:
        D_800B7458->entries = (ChocoboPrize*)&D_800B232C;
        break;
    case 3:
        D_800B7458->entries = (ChocoboPrize*)&D_800B22D0;
        break;
    }
    i = 0;
    locked = 0;
    nPrize = *(s32*)D_800B7458->entries;
    D_800B7458->entries++; // skip the header with the amount of entries
    D_800B745C[0] = D_800B745C[1] = D_800B745C[2] = -1;
    strIndex = -1;
    table = D_800B7458;
    out = D_800B745C;
    while (i != 3) {
        prizeIndex = rand() % nPrize;
        id = table->entries[prizeIndex].strIndex;
        if (id == D_800B745C[0] || id == D_800B745C[1] || id == D_800B745C[2]) {
            continue;
        }
        if (table->entries[prizeIndex].unk1) {
            if (locked && table->entries[prizeIndex].unk3) {
                continue;
            }
            if (D_800B747C) {
                *out++ = id;
                i++;
            }
            if (table->entries[prizeIndex].unk3) {
                strIndex = table->entries[prizeIndex].strIndex;
                locked = -1;
            }
        } else {
            e = &table->entries[prizeIndex];
            if (locked && e->unk3) {
                continue;
            }
            *out++ = id;
            i++;
            if (e->unk3) {
                strIndex = e->strIndex;
                locked = -1;
            }
        }
        if (i >= 3) {
            break;
        }
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            if (D_800B745C[j] > D_800B745C[j + 1]) {
                swap = D_800B745C[j];
                D_800B745C[j] = D_800B745C[j + 1];
                D_800B745C[j + 1] = swap;
            }
        }
    }

    if (strIndex != -1) {
        if (D_800B745C[0] == strIndex) {
            D_800B745C[0] = D_800B745C[2];
            D_800B745C[2] = strIndex;
        } else if (D_800B745C[1] == strIndex) {
            D_800B745C[1] = D_800B745C[2];
            D_800B745C[2] = strIndex;
        }
    }

    for (i = 0; i < 7; i++) {
        ids[i] = D_800B745C[0];
        groups[i] = 0;
    }
    for (i = 7; i < 12; i++) {
        ids[i] = D_800B745C[1];
        groups[i] = 1;
    }
    for (i = 12; i < 15; i++) {
        ids[i] = D_800B745C[2];
        groups[i] = 2;
    }

    for (i = 0; i < 100; i++) {
        x = rand() % 15;
        j = rand() % 15;
        swap = ids[x];
        ids[x] = ids[j];
        ids[j] = swap;
        swap = groups[x];
        groups[x] = groups[j];
        groups[j] = swap;
    }

    for (k = 0; k < 3; k++) {
        for (i = 0; i < 5; i++) {
            (&D_800B7480[i][k])->unk0 = 0;
            (&D_800B7480[i][k])->unk2 = 0;
            (&D_800B7480[i][k])->unk3 = 0;
            (&D_800B7480[i][k])->unk6 = ids[k * 5 + i];
            (&D_800B7480[i][k])->unk7 = groups[k * 5 + i];
            (&D_800B7480[i][k])->unk5 = 2;
            setlen(&D_800B7A68[0].unk1E368[i][k], 9);
            setlen(&D_800B7A68[1].unk1E368[i][k], 9);
        }
    }
    D_800F502C = (u8*)&Savemap.gil;
    D_800B7530.unkC = 0;
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_bet", func_800ABABC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_bet", func_800AC554);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_bet", func_800AD52C);

void ChocoboDrawText(const char* str, s32 len, s32 x, s32 y) { SysMenuDrawString(x, y, str, 7); }

void ChocoboDrawFade(void) {
    DRAWENV env;
    DR_ENV dr;
    DRAWENV* src;
    POLY_F4* prim;
    s32* fade;
    s32* speed;

    if (D_800B7514) {
        src = &D_800F5074->draw;
        env = *src;
        env.tpage = GetTPage(0, 2, 0, 0);
        env.isbg = 0;
        SetDrawEnv(&dr, &env);
        DrawPrim(&dr);
        prim = &D_800B14B4;
        prim->r0 = prim->g0 = prim->b0 = D_800B7514;
        DrawPrim(prim);
    }
    fade = &D_800B7514;
    speed = &D_800F5040.fadeSpeed;
    *fade += *speed;
    if (*fade < 0) {
        *fade = 0;
        *speed = 0;
    } else if (*fade > 0x100) {
        *fade = 0x100;
    }
}
