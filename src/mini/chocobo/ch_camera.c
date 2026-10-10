//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <game.h>
#include <libgte.h>
#include <psxsdk/inline_c.h>
#include "chocobo_private.h"

void ChocoboQueueEvent(s16 id, s16 arg1) {
    if (D_800B7A48.unk0 == -1) {
        D_800F5040.event.type = -1;
        D_800F5028 = arg1;
        D_800F5078.unk24 = id;
        return;
    }
    D_800B1358 = -1;
    if (id != -1) {
        D_800B7530.unkC = arg1;
        D_800F5078.unk14 = arg1;
        D_800F5040.event = D_800F5078.track->events[id - 1];
    }
}
const VECTOR D_800A0068 = {0, 0x1000, 0, 0};

void ChocoboLookAt(MATRIX* m, SVECTOR* eye, SVECTOR* at) {
    VECTOR dir;
    VECTOR side;
    VECTOR fwd;
    VECTOR up = D_800A0068;

    dir.vx = at->vx - eye->vx;
    dir.vy = at->vy - eye->vy;
    dir.vz = at->vz - eye->vz;
    VectorNormal(&dir, &fwd);
    OuterProduct12(&fwd, &up, &dir);
    VectorNormal(&dir, &side);
    OuterProduct12(&fwd, &side, &dir);
    VectorNormal(&dir, &up);
    m->m[0][0] = side.vx;
    m->m[0][1] = side.vy;
    m->m[0][2] = side.vz;
    m->m[1][0] = up.vx;
    m->m[1][1] = up.vy;
    m->m[1][2] = up.vz;
    m->m[2][0] = fwd.vx;
    m->m[2][1] = fwd.vy;
    m->m[2][2] = fwd.vz;
    ApplyMatrix(m, eye, &dir);
    m->t[0] = -dir.vx;
    m->t[1] = -dir.vy;
    m->t[2] = -dir.vz;
}

void ChocoboUpdateEventSpeed(void) {
    s32 speed;
    s32* pending;

    if (D_800F5040.event.type == 2) {
        switch (D_800F5040.event.unk1) {
        case 0:
            speed = 0;
            break;
        case 1:
            speed = D_800B75CC[D_800B7530.unkC].speed / 2;
            break;
        case 2:
            speed = D_800B75CC[D_800B7530.unkC].speed;
            break;
        case 3:
            speed = D_800B75CC[D_800B7530.unkC].speed * 2;
            break;
        }
        pending = &D_800B1358;
        if (*pending) {
            ChocoboSelectRacerAtSegment(D_800B7530.unkC, speed, D_800F5040.event.unkA);
            *pending = 0;
        }
    } else {
        ChocoboSelectRacer(D_800B7530.unkC);
    }
    if ((D_800F5040.unk8 && !D_800B7A48.unk0) || (D_800B7A48.unk0 && D_800B75CC[0].unk7E)) {
        D_800F5040.event.type = 10;
        D_800B759C = -1;
        D_800B7530.unkC = D_800B7594;
    }
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_camera", func_800A7CA4);
const VECTOR D_800A00A8 = {0, 0, 0, 0};
const SVECTOR D_800A00B8 = {0, 0, 0, 0};

void ChocoboUpdateCamera(void) {
    VECTOR pos = D_800A00A8;
    MATRIX m;
    SVECTOR rot = D_800A00B8;
    int flag;
    s32* snap;

    snap = &D_800B759C;
    if (*snap) {
        D_800B7340 = D_800B1348;
        D_800B7348 = D_800B1350;
        *snap = 0;
    }
    D_800B7348.vx = (D_800B7348.vx * 3 + D_800B1350.vx) / 4;
    D_800B7348.vy = (D_800B7348.vy * 7 + D_800B1350.vy) / 8;
    D_800B7348.vz = (D_800B7348.vz * 3 + D_800B1350.vz) / 4;
    D_800B7340.vx = (D_800B7340.vx * 3 + D_800B1348.vx) / 4;
    D_800B7340.vy = (D_800B7340.vy * 7 + D_800B1348.vy) / 8;
    D_800B7340.vz = (D_800B7340.vz * 3 + D_800B1348.vz) / 4;
    ChocoboLookAt(&m, &D_800B7340, &D_800B7348);
    RotMatrixYXZ(&rot, &D_800B7544);
    TransMatrix(&D_800B7544, &pos);
    MulMatrix2(&m, &D_800B7544);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    RotTrans((SVECTOR*)&pos, (VECTOR*)D_800B7544.t, &flag);
    SetRotMatrix(&D_800B7544);
    SetTransMatrix(&D_800B7544);
}

void ChocoboSetNodeStep(s16 node) {
    if (node > 0x80) {
        D_800F5078.track->nodes[node - 0x80].step = -1;
    } else {
        D_800F5078.track->nodes[node].step = 1;
    }
}
