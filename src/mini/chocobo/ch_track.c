//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <game.h>
#include <libgte.h>
#include <psxsdk/inline_c.h>
#include "chocobo_private.h"

const s32 D_800A0030 = 0x20000;

void ChocoboDrawTrackTris(void) {
    ChocoboTri* tri;
    POLY_G3* p;
    OT_TYPE* ot;
    s32 flag;
    VECTOR unused;
    s32 opz;
    GpuBuffer** gfx;
    s32 i;
    s32 n;

    n = 0;
    for (i = 0; i < (*&D_800F5078.track)->nTris; i++) {
        gfx = &D_800F5074;
        tri = &(*&D_800F5078.track)->tris[i];
        gte_ldv3c(tri->v);
        gte_rtpt();
        p = &(*gfx)->polys[n];
        *(u32*)&p->r0 = tri->rgb[0];
        setPolyG3(p);
        gte_readflg(flag);
        if (flag < 0) {
            continue;
        }
        gte_nclip();
        *(u32*)&p->r1 = tri->rgb[1];
        *(u32*)&p->r2 = tri->rgb[2];
        gte_stopz(&opz);
        if (opz < 0) {
            continue;
        }
        if (n > 0xF8) {
            return;
        }
        gte_stsxy3_g3(p);
        ot = &(*gfx)->ot2[1];
        addPrim(ot, p);
        n++;
    }
}

void ChocoboDrawTrackSegments(void) {
    s32 count;
    s32 start;
    s32 end;

    count = D_800F5078.track->count;
    start = (D_800B7598 + count) % count;
    end = (D_800F5078.unk4 + count) % count;
    D_800F5034 = 0;
    if (end < start) {
        func_800A2BD4(0, end);
        func_800A2BD4(start, count);
    } else {
        func_800A2BD4(start, end);
    }
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_track", func_800A2BD4);
