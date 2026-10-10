//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <game.h>
#include "chocobo_private.h"

extern u16 D_800B26CC[];

inline s32 ChocoboCalcAngle(s16 x, s16 y) {
    s32 i;

    if (x >= 0x1000) {
        return 0x400;
    }
    if (x <= -0x1000) {
        return 0xC00;
    }
    i = (u16)(x + 0x1000) & 0x1FFF;
    if (y > 0) {
        return (D_800B26CC[i] + 0x400) & 0xFFF;
    }
    return (D_800B26CC[0x2000 - i] + 0xC00) & 0xFFF;
}

s32 ChocoboCalcAngleToPoint(SVECTOR* from, SVECTOR* to) {
    VECTOR dir;

    dir.vx = to->vx - from->vx;
    dir.vy = to->vy - from->vy;
    dir.vz = to->vz - from->vz;
    VectorNormal(&dir, &dir);
    return ChocoboCalcAngle(dir.vx, dir.vz);
}
