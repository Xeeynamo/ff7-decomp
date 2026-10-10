//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <game.h>
#include <libgte.h>
#include <psxsdk/inline_c.h>
#include "chocobo_private.h"

void ChocoboApplyItemEffect(s32 id, s32 effect) {
    Chocobo* base = D_800B75CC;
    Chocobo* c = &base[id];

    switch (effect) {
    case 1:
        c->unk98 = D_800F5078.track->unk4 + 20;
        break;
    case 2:
        c->unk6C += c->unk6C * 4 / 3;
        c->unk68 = c->unk6C;
        break;
    case 3:
        c->unk98 = D_800F5078.track->unk4 - 20;
        break;
    case 4:
        c->unk4C = 2;
        break;
    case 5:
        c->unk9A = 0;
        break;
    case 6:
        c->unk64 = 100;
        c->unk52 = 0;
        c->unk6 = 3;
        break;
    case 7:
        c->unk9C = -1;
        break;
    case 8:
        c->unk6C /= 2;
        c->unk68 = c->unk6C;
        break;
    case 9:
        c->unk86 &= 2;
        break;
    case 10:
        c->unk86 &= 1;
        break;
    case 11:
        c->unkA0 = -1;
        if (rand() & 1) {
            c->unk6C /= 2;
        }
        break;
    case 12:
        c->unkA2 = -1;
        break;
    case 13:
        c->unk52 = 3;
        break;
    }
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_racer", func_800A34A8);

void ChocoboUpdateRanking(void) {
    s32 keys[NUM_CHOCOBO];
    s32 ids[16]; // only the first NUM_CHOCOBO are used
    s32 i;
    s32 j;
    s32 tmp;
    Chocobo* c;

    for (i = 0; i < NUM_CHOCOBO; i++) {
        c = &D_800B75CC[i];
        keys[i] = c->unk0;
        ids[i] = i;
    }
    for (i = 0; i < NUM_CHOCOBO; i++) {
        for (j = i; j < NUM_CHOCOBO; j++) {
            if (keys[i] < keys[j]) {
                tmp = keys[i];
                keys[i] = keys[j];
                keys[j] = tmp;
                tmp = ids[i];
                ids[i] = ids[j];
                ids[j] = tmp;
            }
        }
    }
    for (i = 0; i < NUM_CHOCOBO; i++) {
        D_800B75CC[ids[i]].rank = i;
    }
    D_800B733C = ids[NUM_CHOCOBO - 1];
    for (i = 0; i < NUM_CHOCOBO; i++) {
        ChocoboUpdateRacer(i);
    }
    if (D_800B7A48.unk0) {
        func_800A6E50(1);
    } else {
        func_800A6E50(0);
    }
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_racer", ChocoboUpdateRacer);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_racer", func_800A500C);

void ChocoboSelectRacer(s32 id) {
    VECTOR dir;
    Chocobo* c;
    s32 w;

    if (D_800F5078.unk10 != id) {
        D_800F5078.unk10 = id;
        c = &D_800B75CC[6];
        *c = D_800B75CC[id];
        c->unk7C = 0x7F;
        w = 0x81;
        c->unk10 = (D_800B7500[c->unk0].p0.vx * c->unk7C + D_800B7500[c->unk0].p1.vx * w) / 256;
        c->unk12 = (D_800B7500[c->unk0].p0.vy * c->unk7C + D_800B7500[c->unk0].p1.vy * w) / 256;
        c->unk14 = (D_800B7500[c->unk0].p0.vz * c->unk7C + D_800B7500[c->unk0].p1.vz * w) / 256;
        c->unk28 = (D_800B7500[c->unk2].p0.vx * c->unk7C + D_800B7500[c->unk2].p1.vx * w) / 256;
        c->unk2A = (D_800B7500[c->unk2].p0.vy * c->unk7C + D_800B7500[c->unk2].p1.vy * w) / 256;
        c->unk2C = (D_800B7500[c->unk2].p0.vz * c->unk7C + D_800B7500[c->unk2].p1.vz * w) / 256;
        dir.vx = c->unk28 - c->unk10;
        dir.vy = 0;
        dir.vz = c->unk2C - c->unk14;
        VectorNormal(&dir, &dir);
        c->unk3A = ChocoboCalcAngle(dir.vx, dir.vz);
    }
}

void ChocoboSelectRacerAtSegment(s32 chocoboId, s32 speed, s32 seg) {
    VECTOR dir;
    Chocobo* c;
    s32 next;
    s32 w;

    D_800F5078.unk10 = chocoboId;
    c = &D_800B75CC[NUM_CHOCOBO];
    *c = D_800B75CC[chocoboId];
    next = (seg + 1 + D_800B74FC) % D_800B74FC;
    c->unk7C = 0x7F;
    w = 0x81;
    c->unk0 = seg;
    c->speed = speed;
    c->unk2 = next;
    c->unk10 = (D_800B7500[seg].p0.vx * c->unk7C + D_800B7500[seg].p1.vx * w) / 256;
    c->unk12 = (D_800B7500[seg].p0.vy * c->unk7C + D_800B7500[seg].p1.vy * w) / 256;
    c->unk14 = (D_800B7500[seg].p0.vz * c->unk7C + D_800B7500[seg].p1.vz * w) / 256;
    c->unk28 = (D_800B7500[next].p0.vx * c->unk7C + D_800B7500[next].p1.vx * w) / 256;
    c->unk2A = (D_800B7500[next].p0.vy * c->unk7C + D_800B7500[next].p1.vy * w) / 256;
    c->unk2C = (D_800B7500[next].p0.vz * c->unk7C + D_800B7500[next].p1.vz * w) / 256;
    dir.vx = c->unk28 - c->unk10;
    dir.vy = 0;
    dir.vz = c->unk2C - c->unk14;
    VectorNormal(&dir, &dir);
    c->unk3A = ChocoboCalcAngle(dir.vx, dir.vz);
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_racer", func_800A6E50);
