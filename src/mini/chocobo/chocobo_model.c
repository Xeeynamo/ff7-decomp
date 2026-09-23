//! PSYQ=3.6
#include "chocobo_private.h"
#include "libetc.h"
#include <libgte.h>
#include <psxsdk/inline_c.h>
#include "sincos.h"

u8* func_800AD91C(ChocoboModel* model, u8* buf, s32 arg2) {
    ChocoboModelPart* parts;
    u32 i;

    model->unk20 = buf;
    buf += model->unk2 * sizeof(ChocoboModelPart);
    parts = (ChocoboModelPart*)(model->partsOffset + (u_long)model->data);
    for (i = 0; i < model->nParts; i++) {
        buf = func_800AD9D8(&parts[i], buf, 0, arg2);
    }
    func_800AF9E4(model, model->unk16, 0);
    return buf;
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AD9D8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AE534);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AE7D4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AF11C);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AF9E4);

void func_800AFC64(ChocoboModelPart* part, s16 scale, s32 force) {
    MATRIX* m;
    s16* out;
    SVECTOR* verts;
    u32 i;
    u32 n;

    m = (MATRIX*)getScratchAddr(0);
    out = (s16*)(m + 1);
    if ((part->verts->flags & 1) && !force) {
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
    verts = part->verts->verts;
    n = part->nVerts;
    for (i = 0; i < n; i++) {
        gte_ldv0(&verts[i]);
        gte_rt();
        gte_stlvnl(out);
        verts[i].vx = out[0];
        verts[i].vy = out[2];
        verts[i].vz = out[4];
    }
    part->verts->flags |= 1;
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800AFDBC);

s32 func_800B00DC(ChocoboModel* model, u8* data) {
    ChocoboModelPart* parts;
    u32 i;
    u32 n;
    s16 x;
    s16 y;
    s16 z;
    SVECTOR unused;

    n = model->nParts;
    parts = (ChocoboModelPart*)(model->partsOffset + (u_long)model->data);
    x = (data[1] << 8) | data[0];
    y = (data[3] << 8) | data[2];
    z = (data[5] << 8) | data[4];
    *(u_long*)getScratchAddr(0x80) = data[6];
    for (i = 0; i < n; i++) {
        func_800B01B0(&parts[i], x, y, z);
    }
    return 1;
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800B01B0);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo_model", func_800B0E7C);
