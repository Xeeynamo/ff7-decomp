//! PSYQ=3.3
#include <game.h>

const u32 D_800A0000[] = {
    0x220002, 0x10015, 0x15000C, 0x0, 0x10000, 0x15000C, 0x0, 0x20000, 0x180022, 0x0, 0x20000, 0x150022, 0xC0001, 0x18};
const u32 D_800A0038[] = {0x60A, 0x4000200, 0x50600, 0x4050205, 0x60A0605, 0x60A060A, 0x20A000A, 0x0};

// The condor minigame's unit record. Only the fields reached so far are named;
// the gaps stay as filler so every later offset keeps its place.
typedef struct {
    /* 0x00 */ u16 flags;
    /* 0x02 */ u8 unk02;
    /* 0x03 */ u8 unk03[2];
    /* 0x05 */ u8 unk05;
    /* 0x06 */ u16 unk06;
    /* 0x08 */ u8 unk08[2];
    /* 0x0A */ u16 unk0A;
    /* 0x0C */ u8 unk0C[6];
    /* 0x12 */ u8 unk12;
    /* 0x13 */ u8 unk13;
    /* 0x14 */ u8 unk14[8];
    /* 0x1C */ u16 timer;
    /* 0x1E */ u8 unk1E[4];
    /* 0x22 */ u8 width;
    /* 0x23 */ u8 height;
    /* 0x24 */ u16 unk24;
    /* 0x26 */ u16 unk26;
    /* 0x28 */ u16 unk28;
    /* 0x2A */ u8 state;
    /* 0x2B */ u8 unk2B;
    /* 0x2C */ u8 unk2C;
    /* 0x2D */ u8 unk2D;
    /* 0x2E */ u8 unk2E;
    /* 0x2F */ u8 unk2F;
    /* 0x30 */ u16 unk30;
    /* 0x32 */ u16 unk32;
    /* 0x34 */ u16 unk34;
    /* 0x36 */ u8 unk36[4];
    /* 0x3A */ u8 unk3A;
    /* 0x3B */ u8 unk3B;
    /* 0x3C */ u8 unk3C[5];
    /* 0x41 */ u8 unk41;
    /* 0x42 */ u8 unk42;
    /* 0x43 */ u8 unk43;
    /* 0x44 */ u8 unk44[4];
    /* 0x48 */ s16 unk48;
    /* 0x4A */ s16 unk4A;
    /* 0x4C */ u16 unk4C;
    /* 0x4E */ u8 unk4E[2];
    /* 0x50 */ s16 unk50;
    /* 0x52 */ s16 unk52;
    /* 0x54 */ u16 unk54;
    /* 0x56 */ u8 unk56[2];
    /* 0x58 */ u16 unk58;
    /* 0x5A */ u16 unk5A;
    /* 0x5C */ u8 unk5C[4];
    /* 0x60 */ u16 unk60;
    /* 0x62 */ u8 unk62[2];
    /* 0x64 */ u16 unk64;
    /* 0x66 */ u8 unk66[0x12];
} CondorUnit;

// The x/y pair the spawn helpers take.
typedef struct {
    /* 0x0 */ u16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
} CondorPoint;

// What the pending-spawn pointer at D_800BE8BC refers to.
typedef struct {
    /* 0x0 */ u8 kind;
    /* 0x1 */ u8 unk1;
    /* 0x2 */ u8 unk2;
    /* 0x3 */ u8 unk3;
    /* 0x4 */ u16 unk4;
} CondorSpawn;

// The file table condor loads its pieces from, a flat pair per entry: sector
// then length. Flat rather than a struct because retail computes the two
// element addresses separately off one base, which is what two indices into one
// array gives and what a struct does not: a struct shares `&files[index]`.
extern int D_800BE7D8[];
extern CondorSpawn* D_800BE8BC;
extern CondorUnit D_800BE8C0[];
extern s16 D_8012C41C;
extern s32 D_8012C524;
extern s32 D_8012C528;
extern s32 D_8012C3E8;
extern s32 D_8012C3FC;
extern s32 D_8012C508;
extern u8 D_80121EF0[];
extern s32 D_8012C3F0;
extern s16 D_8012C47C;
extern s16 D_8012C480;
extern s16 D_8012C484;
extern s16 D_8012C488;
extern s16 D_8012C430;
extern u32 D_8012C53C;
extern s16 D_8012C5CC;
extern s32 D_8012C3EC;
extern CondorPoint D_8012C3A4;
extern CondorPoint D_8012C3AC;
extern s16 D_8012C40C;
extern u16 D_8012AF54;
extern u8 D_8012AF5C[];
extern s16 D_8012AF3C;
extern s16 D_8012AF3E;
extern u16 D_8012AF40;
extern u16 D_8012AF42;
extern s16 D_8012AF58;
extern s32 D_80188008;
extern s32 D_80188010;
extern s32 D_8012C404;
extern s16 D_8012C4E4;
extern s16 D_8012C4F0;
extern s16 D_8012C4F4;
extern s16 D_8012C4F8;
extern s16 D_8012C4FC;
extern u16 D_8012C388;
extern s32 D_8012C530;
extern s16 D_8012C5C0;
extern s16 D_8012B85C;
extern u16 D_80180000[];
extern s32 D_8012C544;
extern s32 D_8012C548;

// Still INCLUDE_ASM. Declared because GCC 2.x compiles an undeclared call as
// implicit int without saying anything, and the object comes out wrong.
u32 func_800AC498(CondorUnit*);
void func_800ABE54(CondorUnit*);
void func_800AC72C(CondorUnit*, CondorSpawn*, s32, s16*);
s16 func_800AC5C0(s16 index, CondorPoint* pos);
s16 func_800B0604(CondorUnit*, s32);
s16 func_800A8004(CondorUnit* unit);
void func_800B332C(void);
void func_800B3A48(CondorUnit*, s32);
void func_800B42AC(CondorUnit*);
void func_800B4720();
void func_800B3644(CondorUnit*);
void func_800B7630(void);
void func_800A2D68(void);
void func_800B5BDC(CondorUnit*);
void func_800B6758(void);
s16 func_800B64EC(void);
void func_800B47C8(CondorUnit*, s32);
void func_800AB71C(u32 buttons, CondorPoint* position);
void func_800B7638(void);
void func_800A427C(void);
void func_800AB5BC(s32, s32);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A037C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A0548);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A115C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A1A80);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A1B94);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A1EEC);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A2068);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A224C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A2414);

s16 CondorUnitOverlapAt(CondorUnit* self, s16 index) {
    CondorUnit* other;
    s32 halfWidth;
    s16 otherPos;
    s16 selfPos;
    s16 hit;
    s16 found;

    other = &D_800BE8C0[index];
    found = -1;
    if (other->flags != 0) {
        halfWidth = (other->width + self->width) >> 1;
        otherPos = other->unk48;
        selfPos = self->unk48;
        hit = 0;
        if (otherPos < selfPos) {
            hit = otherPos >= selfPos - halfWidth;
        } else if (selfPos >= otherPos - halfWidth) {
            hit = 1;
        }
        if (hit != 0) {
            otherPos = other->unk4A;
            selfPos = self->unk4A;
            hit = 0;
            if (otherPos < selfPos) {
                hit = otherPos >= selfPos - self->height;
            } else if (selfPos >= otherPos - other->height) {
                hit = 1;
            }
            if ((hit != 0) && (other != self)) {
                found = index;
            }
        }
    }
    return found;
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A2640);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A2798);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A28E4);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A2B40);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A2CB4);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A2D68);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A2FD4);

u8 func_800A3260(u32 arg0) {
    u32 total = D_8012C53C;
    u32 count;

    if (total == 0) {
        if (D_8012C430 == 0) {
            if (arg0 != 1) {
                return -1;
            }
            return 0;
        }
        return 0;
    }

    count = total / arg0;
    if (count == 0) {
        if (D_8012C430 == 0) {
            return -1;
        }
        return 0;
    }

    D_8012C430 = (u16)D_8012C430 + 1;
    D_8012C53C = total - (count * arg0);
    return count;
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A3304);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A3A98);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A427C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A46E4);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A4B2C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A4BB0);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A4DFC);

void CondorPlaySound(s16 arg0, s16 arg1) {
    g_AkaoCmd.opcode = arg1 + 39;
    g_AkaoCmd.params[0] = 64;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A5258);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A5B64);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A5E04);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A6048);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A6298);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A6430);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A65BC);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A6808);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A698C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A709C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A71D8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A73AC);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A7584);

void CondorAngleToVector(s32 angle) {
    u16 frac;

    frac = angle & 0x1FF;
    frac >>= 1;
    switch (angle & 0xE00) {
    case 0x200:
        D_8012C544 = -0x100;
        D_8012C548 = 0xFF - frac;
        break;
    case 0x400:
        D_8012C544 = -0x100;
        D_8012C548 = -frac;
        break;
    case 0x600:
        D_8012C544 = frac - 0xFF;
        D_8012C548 = -0x100;
        break;
    case 0x800:
        D_8012C544 = frac;
        D_8012C548 = -0x100;
        break;
    case 0xA00:
        D_8012C544 = 0x100;
        D_8012C548 = frac - 0xFF;
        break;
    case 0xC00:
        D_8012C544 = 0x100;
        D_8012C548 = frac;
        break;
    case 0xE00:
        D_8012C544 = 0xFF - frac;
        D_8012C548 = 0x100;
        break;
    case 0:
        D_8012C544 = -frac;
        D_8012C548 = 0x100;
        break;
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A77EC);

u16 func_800A7584(u16* vector);
extern s32 D_8012C554;
extern s32 D_8012C558;
extern s32 D_8012C55C;
extern s32 D_8012C560;
extern s32 D_8012C564;
extern s32 D_8012C568;

s16 func_800A7BF4(SVECTOR* first, SVECTOR* second) {
    SVECTOR vector;
    s32 dx, y, destY;
    s32 x, destX;
    x = first->vx;
    destX = second->vx;
    dx = destX - x;
    y = first->vy + 0x4000;
    destY = second->vy + 0x4000;
    D_8012C560 = 0;
    D_8012C55C = 0;
    D_8012C568 = 0;
    D_8012C564 = 0;
    D_8012C554 = dx;
    if (D_8012C554 < 0) {
        D_8012C564 = 1;
        D_8012C554 = -D_8012C554;
    }
    D_8012C558 = destY - y;
    if (D_8012C558 < 0) {
        D_8012C568 = 1;
        D_8012C558 = -D_8012C558;
    }
    if (D_8012C554 >= D_8012C558) {
        D_8012C544 = 256;
        if ((D_8012C558 << 8) == 0) {
            D_8012C548 = 1;
        } else {
            D_8012C548 = (D_8012C558 << 8) / D_8012C554;
        }
        if (D_8012C548 == 0) {
            D_8012C548 = 1;
        }
    } else {
        D_8012C548 = 256;
        if ((D_8012C554 << 8) == 0) {
            D_8012C544 = 1;
        } else {
            D_8012C544 = (D_8012C554 << 8) / D_8012C558;
        }
        if (D_8012C544 == 0) {
            D_8012C544 = 1;
        }
    }
    if (D_8012C564 != 0) {
        D_8012C544 = -D_8012C544;
    }
    if (D_8012C568 != 0) {
        D_8012C548 = -D_8012C548;
    }
    vector.vz = 0;
    vector.vx = D_8012C544;
    vector.vy = D_8012C548;
    return (s16)func_800A7584((u16*)&vector);
}

typedef struct {
    /* 0x00 */ SVECTOR vectors[4];
    /* 0x20 */ s16 neighbors[3];
    /* 0x26 */ u8 unk26[2];
    /* 0x28 */ SVECTOR vertices[3];
    /* 0x40 */ s16 minX;
    /* 0x42 */ s16 maxX;
    /* 0x44 */ s16 minY;
    /* 0x46 */ s16 maxY;
    /* 0x48 */ u16 unk48;
    /* 0x4A */ u16 unk4A;
} CondorMapTriangle; // size:0x4C
extern CondorMapTriangle D_8010C33C[];
extern s32 D_8012C518;
extern s16 D_8012C5A4;
extern s16 D_8012C5A8;
extern s16 D_8012C5AC;

static inline s32 CondorAngleOutside(s16 point, s16 first, s16 last) {
    s16 high, low;
    if (first >= last) {
        high = first;
        low = last;
    } else {
        high = last;
        low = first;
    }
    if (high - low < 2048) {
        if (high < point || point < low) {
            return 1;
        }
    } else {
        if (point < high && low < point) {
            return 1;
        }
    }
    return 0;
}

s32 func_800A7E20(CondorMapTriangle* triangle, s16 x, s16 y) {
    SVECTOR point;
    SVECTOR* vertex;
    point.vx = x;
    point.vy = y;
    point.vz = 0;
    vertex = &triangle->vertices[0];
    D_8012C5A4 = func_800A7BF4(vertex, &point);
    D_8012C5A8 = func_800A7BF4(vertex, &triangle->vertices[1]);
    D_8012C5AC = func_800A7BF4(vertex, &triangle->vertices[2]) + 8;
    if (CondorAngleOutside(D_8012C5A4, D_8012C5A8, D_8012C5AC)) {
        return 1;
    }
    vertex = &triangle->vertices[1];
    D_8012C5A4 = func_800A7BF4(vertex, &point);
    D_8012C5A8 = func_800A7BF4(vertex, &triangle->vertices[2]);
    D_8012C5AC = func_800A7BF4(vertex, &triangle->vertices[0]);
    return CondorAngleOutside(D_8012C5A4, D_8012C5A8, D_8012C5AC);
}

s16 func_800A8004(CondorUnit* unit) {
    CondorMapTriangle* triangle;
    s16 index;
    u16 x;
    u16 y;
    s16 shiftedX;
    s16 shiftedY;
    triangle = D_8010C33C;
    x = unit->unk50;
    y = unit->unk52;
    shiftedX = x + 0x4000;
    shiftedY = y + 0x4000;
    for (index = 0; index < D_8012C518; index++, triangle++) {
        if (shiftedX >= triangle->minX && shiftedX <= triangle->maxX && shiftedY >= triangle->minY &&
            shiftedY <= triangle->maxY && (s16)func_800A7E20(triangle, x, y) == 0) {
            unit->unk60 = triangle->unk48;
            unit->unk64 = triangle->unk4A;
            return index;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A8158);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A8454);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A85DC);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A8AA8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A8D08);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A93A0);

s32 CondorTrySpawn(void) {
    CondorPoint at;

    if (D_8012C3EC == 0) {
        if ((D_8012C5CC == 0) && (D_8012C41C == -1)) {
            if (D_8012C508 < 0x14) {
                if (D_8012C404 == 1) {
                    if (D_8012C3A4.y >= 0x2A0) {
                        return 1;
                    }
                } else if (D_8012C3A4.y >= D_8012C530) {
                    return 1;
                }
                {
                    at.z = -0x28;
                    at.x = D_8012C3A4.x;
                    at.y = 0x400 - (u16)D_8012C3A4.y;
                    if (func_800AC5C0(0, &at) != 1) {
                        D_8012C5C0 = 1;
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

void CondorClearSelection(void) {
    D_8012C41C = -1;
    D_8012C524 = 256;
    D_8012C528 = 0;
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A95E8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A9B20);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AB5BC);

void func_800AB6B0(s32 arg0) {
    CondorPoint* table = &D_8012C3A4;

    if (D_8012C5CC == 0) {
        table = &D_8012C3AC;
    }
    if ((D_8012C3E8 != 3) && (D_8012C3EC == 0)) {
        table = &D_8012C3A4;
    }
    func_800AB71C(arg0, table);
}

extern u16 D_8012C434;
extern u16 D_8012C438;
extern VECTOR D_8012C6EC;

void func_800AB71C(u32 buttons, CondorPoint* position) {
    DVECTOR relative;
    s32 moveRelative;
    relative.vx = position->x - D_8012C434;
    relative.vy = position->y - D_8012C438;
    if (buttons & 0x1000) {
        moveRelative = 0;
        if (relative.vy >= 105) {
            moveRelative = 1;
        } else {
            D_8012C438--;
            if (D_8012C438 > 0x8000) {
                D_8012C438 = 0;
                moveRelative = 1;
            } else {
                D_8012C6EC.vy++;
            }
        }
        if (moveRelative) {
            relative.vy--;
            if (relative.vy < 0) {
                relative.vy = 0;
            }
        }
    }
    if (buttons & 0x4000) {
        moveRelative = 0;
        if (relative.vy < 104) {
            moveRelative = 1;
        } else {
            D_8012C438++;
            if (D_8012C438 >= 805) {
                D_8012C438 = 804;
                moveRelative = 1;
            } else {
                D_8012C6EC.vy--;
            }
        }
        if (moveRelative) {
            relative.vy++;
            if (relative.vy >= 204) {
                relative.vy = 204;
            }
        }
    }
    if (buttons & 0x8000) {
        moveRelative = 0;
        if (relative.vx >= 161) {
            moveRelative = 1;
        } else {
            D_8012C434--;
            if (D_8012C434 > 0x8000) {
                moveRelative = 1;
                D_8012C434 = 0;
                D_8012C6EC.vx &= ~7;
            } else {
                D_8012C6EC.vx++;
            }
        }
        if (moveRelative) {
            relative.vx--;
            if (relative.vx < 0) {
                relative.vx = 0;
            }
        }
    }
    if (buttons & 0x2000) {
        moveRelative = 0;
        if (relative.vx < 160) {
            moveRelative = 1;
        } else {
            D_8012C434++;
            if (D_8012C434 >= 193) {
                moveRelative = 1;
                D_8012C434 = 192;
                D_8012C6EC.vx &= ~7;
            } else {
                D_8012C6EC.vx--;
            }
        }
        if (moveRelative) {
            relative.vx++;
            if (relative.vx >= 320) {
                relative.vx = 320;
            }
        }
    }
    position->x = relative.vx + D_8012C434;
    position->y = relative.vy + D_8012C438;
}

void func_800ABA58(u32 buttons) {
    if (buttons & 0x1000) {
        D_8012C438--;
        if (D_8012C438 > 0x8000) {
            D_8012C438 = 0;
        } else {
            D_8012C6EC.vy++;
        }
    }
    if (buttons & 0x4000) {
        D_8012C438++;
        if (D_8012C438 > 0x324) {
            D_8012C438 = 0x324;
        } else {
            D_8012C6EC.vy--;
        }
    }
    if (buttons & 0x8000) {
        D_8012C434--;
        if (D_8012C434 > 0x8000) {
            D_8012C434 = 0;
            D_8012C6EC.vx &= ~7;
        } else {
            D_8012C6EC.vx++;
        }
    }
    if (buttons & 0x2000) {
        D_8012C434++;
        if (D_8012C434 > 0xC0) {
            D_8012C434 = 0xC0;
            D_8012C6EC.vx &= ~7;
        } else {
            D_8012C6EC.vx--;
        }
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800ABC18);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800ABE54);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AC1A8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AC498);

void func_800A2414(CondorUnit* self, s32 first, s16 end);

s16 func_800AC5C0(s16 index, CondorPoint* pos) {
    CondorUnit unit;
    CondorUnit* unitPtr = &unit;
    u8* record;
    u16 depth;
    s32 result;
    unitPtr->unk48 = pos->x;
    depth = D_8012C388;
    unitPtr->unk4A = 0x400 - pos->y;
    unitPtr->unk4C = depth;
    record = *(u16*)0x80180000 + (u8*)0x80180000;
    record += index * 32;
    unitPtr->unk50 = pos->x - 0x100;
    unitPtr->unk52 = 0x200 - pos->y;
    unitPtr->unk54 = depth;
    unitPtr->width = record[13];
    unitPtr->height = record[14];
    func_800A2414(unitPtr, 0, 0x27);
    result = 1;
    if (D_8012B85C == 0) {
        result = func_800A8004(unitPtr);
        return result == -1;
    }
    return result;
}

s32 func_800AC68C(s16 index, CondorPoint* pos) {
    CondorUnit unit;
    u8* record;
    u16 depth;
    unit.unk48 = pos->x;
    depth = D_8012C388;
    unit.unk4A = 0x400 - pos->y;
    unit.unk4C = depth;
    record = *(u16*)0x80180000 + (u8*)0x80180000;
    record += index * 32;
    unit.unk50 = pos->x - 0x100;
    unit.unk52 = 0x200 - pos->y;
    unit.unk54 = depth;
    unit.width = record[13];
    unit.height = record[14];
    return func_800A8004(&unit) == -1;
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AC72C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800ACAA4);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800ACDB0);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AD02C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AD288);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AD32C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AD578);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AD73C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AE0D0);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AE384);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AEB90);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AEDE0);

void CondorRemoveSelectedUnit(void) {
    D_800BE8C0[D_8012C41C].unk05 = 0xFF;
    D_8012C508 = D_8012C508 - 1;
    func_800A2D68();
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AF08C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AF278);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AF470);

typedef struct {
    DRAWENV draw;
    DISPENV display;
    OT_TYPE ot[4096];
} CondorDrawBuffer;
extern CondorDrawBuffer* D_80129F34;

typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ u16 z;
    /* 0x6 */ u8 unk6[2];
    /* 0x8 */ u8 state;
    /* 0x9 */ u8 timer;
    /* 0xA */ u8 image;
    /* 0xB */ u8 unkB;
    /* 0xC */ u8 unkC;
    /* 0xD */ u8 unkD[3];
} CondorEffect;
const u32 D_800A01BC[] = {0x4030201, 0x7050D0C, 0x0};
const u32 D_800A01C8[] = {0x4030201, 0x7050D0C, 0x8};
const u32 D_800A01D4[] = {0x4030201, 0x6050D0C, 0x7};
const u32 D_800A01E0[] = {0x4030201, 0x6050D0C, 0x807};

s32 func_800AF278(CondorUnit* unit);

void func_800AF5A8(POLY_FT4* poly, CondorEffect* effect) {
    CondorUnit impact;
    // Each frame stores the blend mode, duration, and image index.
    s16 frames[12][3] = {{4, 1, 0}, {1, 1, 0}, {0, 1, 0}, {0, 2, 1}, {0, 2, 2}, {0, 2, 3},
                         {0, 4, 4}, {0, 6, 5}, {0, 6, 6}, {1, 6, 7}, {4, 6, 7}, {4, 1, 7}};
    s16 blendMode;
    u32 state;
    s16 timer;
    u16 depth;
    state = effect->state;
    if (state >= 0x40) {
        effect->state = 0xFF;
        effect->timer = 0;
    } else if (state < 0xB) {
        if (effect->timer == 0) {
            effect->state++;
            // The animation reads the low byte of the duration and image words.
            timer = *((u8*)frames + 2 + effect->state * 6);
            timer--;
            effect->timer = timer;
            effect->image = *((u8*)frames + 4 + effect->state * 6);
            poly->clut = GetClut(0x340, 0xF1);
        } else {
            timer = effect->timer;
            timer--;
            effect->timer = timer;
        }
        blendMode = frames[effect->state][0];
        if (blendMode == 0) {
            SetSemiTrans(poly, 0);
        }
        if (blendMode == 1) {
            SetSemiTrans(poly, 1);
            poly->tpage = GetTPage(0, 1, 0x340, 0);
        }
        if (blendMode == 4) {
            SetSemiTrans(poly, 1);
            poly->tpage = GetTPage(0, 3, 0x340, 0);
        }
        poly->u0 = (effect->image & 3) << 6;
        poly->v0 = (effect->image & 4) * 8;
        poly->u1 = ((effect->image & 3) << 6) + 0x3F;
        poly->v1 = (effect->image & 4) * 8;
        poly->u2 = (effect->image & 3) << 6;
        poly->v2 = (effect->image & 4) * 8 + 0x1F;
        poly->u3 = ((effect->image & 3) << 6) + 0x3F;
        poly->v3 = (effect->image & 4) * 8 + 0x1F;
        poly->x0 = effect->x + 0x80;
        poly->y0 = effect->y + 0x68;
        poly->x1 = effect->x + 0xC0;
        poly->y1 = effect->y + 0x68;
        poly->x2 = effect->x + 0x80;
        poly->y2 = effect->y + 0x88;
        poly->x3 = effect->x + 0xC0;
        poly->y3 = effect->y + 0x88;
        AddPrim(&D_80129F34->ot[0], poly);
    } else {
        effect->state++;
    }
    if (effect->state == 0xC) {
        impact.unk48 = effect->x + 0x100;
        impact.unk4A = effect->y + 0x210;
        depth = effect->z;
        impact.width = 0x40;
        impact.height = 0x20;
        impact.unk4C = depth;
        impact.unk12 = effect->unkB;
        impact.unk43 = effect->unkC;
        func_800AF278(&impact);
    }
}

void func_800AF98C(POLY_FT4* poly, CondorEffect* effect) {
    CondorUnit impact;
    // Each frame stores the blend mode, duration, and image index.
    s16 frames[12][3] = {{4, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 2, 1}, {4, 2, 2}, {4, 2, 3},
                         {4, 4, 4}, {4, 6, 5}, {4, 6, 6}, {4, 3, 7}, {4, 2, 7}, {4, 1, 7}};
    s16 blendMode;
    u32 state;
    s16 timer;
    u16 depth;
    state = effect->state;
    if (state >= 0x40) {
        effect->state = 0xFF;
        effect->timer = 0;
    } else if (state < 0xB) {
        if (effect->timer == 0) {
            effect->state++;
            // The animation reads the low byte of the duration and image words.
            timer = *((u8*)frames + 2 + effect->state * 6);
            timer--;
            effect->timer = timer;
            effect->image = *((u8*)frames + 4 + effect->state * 6);
            poly->clut = GetClut(0x340, 0xF3);
        } else {
            timer = effect->timer;
            timer--;
            effect->timer = timer;
        }
        blendMode = frames[effect->state][0];
        if (blendMode == 0) {
            SetSemiTrans(poly, 0);
        }
        if (blendMode == 1) {
            SetSemiTrans(poly, 1);
            poly->tpage = GetTPage(0, 1, 0x340, 0);
        }
        if (blendMode == 4) {
            SetSemiTrans(poly, 1);
            poly->tpage = GetTPage(0, 3, 0x340, 0);
        }
        poly->u0 = (effect->image & 3) << 6;
        poly->v0 = (effect->image & 4) * 8 + 0x40;
        poly->u1 = ((effect->image & 3) << 6) + 0x3F;
        poly->v1 = (effect->image & 4) * 8 + 0x40;
        poly->u2 = (effect->image & 3) << 6;
        poly->v2 = (effect->image & 4) * 8 + 0x5F;
        poly->u3 = ((effect->image & 3) << 6) + 0x3F;
        poly->v3 = (effect->image & 4) * 8 + 0x5F;
        poly->x0 = effect->x + 0x80;
        poly->y0 = effect->y + 0x68;
        poly->x1 = effect->x + 0xC0;
        poly->y1 = effect->y + 0x68;
        poly->x2 = effect->x + 0x80;
        poly->y2 = effect->y + 0x88;
        poly->x3 = effect->x + 0xC0;
        poly->y3 = effect->y + 0x88;
        AddPrim(&D_80129F34->ot[0], poly);
    } else {
        effect->state++;
    }
    if (effect->state == 0xA) {
        impact.unk48 = effect->x + 0x100;
        impact.unk4A = effect->y + 0x210;
        depth = effect->z;
        impact.width = 0x20;
        impact.height = 0x20;
        impact.unk4C = depth;
        impact.unk12 = effect->unkB;
        impact.unk43 = effect->unkC;
        func_800AF278(&impact);
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AFD74);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AFED4);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B00F4);

void CondorAngleToVectorScaled(s32 angle, s16 len) {
    s32 step;
    s32 sn;
    s32 oct;
    s32 cs;

    step = angle & 0x3FF;
    sn = (rsin(step) * len) >> 12;
    oct = angle & 0xC00;
    cs = (rcos(step) * len) >> 12;
    switch (oct) {
    case 0x0:
        D_8012C544 = -sn;
        D_8012C548 = cs;
        break;
    case 0x400:
        D_8012C544 = -cs;
        D_8012C548 = -sn;
        break;
    case 0x800:
        D_8012C544 = sn;
        D_8012C548 = -cs;
        break;
    case 0xC00:
        D_8012C544 = cs;
        D_8012C548 = sn;
        break;
    default:
        D_8012C544 = sn;
        D_8012C548 = cs;
        break;
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B0404);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B0604);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B0D50);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B1620);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B17A0);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B18E0);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B1FD0);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B21A8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B2520);

const u8 D_800A02EC[] = {0, 40, 40, 44, 40, 20, 28, 44, 44, 0, 0, 0, 40, 40, 20, 20, 48, 38, 30, 40, 20, 20, 20, 20};

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B2804);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B2940);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B2A70);

void CondorLoadFile(s32 index, s32 arg1, u_long* dst) {
    SystemLoadFileBySector(D_800BE7D8[index * 2], D_800BE7D8[(index * 2) + 1], dst, func_800B7630);
    while (SystemCdromReadChain() != 0) {
    }
}

void CondorLoadFileLzs(s32 index, s32 arg1, u_long* dst) {
    SysCdromStartLoadLzs(D_800BE7D8[index * 2], D_800BE7D8[(index * 2) + 1], dst, func_800B7630);
    while (SystemCdromReadChain() != 0) {
    }
}

s32 CondorUnitIsActive(CondorUnit* unit) {
    s32 kind = unit->unk13;
    s32 active = 0;

    switch (kind) {
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
        active = 1;
        break;
    }
    return active;
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B2D18);

void CondorUpdateUnits(void) {
    CondorUnit* unit;
    s32 i;

    for (unit = D_800BE8C0, i = 0; i < 40; i++, unit++) {
        if (unit->flags != 0) {
            func_800B3644(unit);
        }
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B2ED0);

void CondorUnitTickTimer(CondorUnit* unit) {
    if (unit->timer != 0) {
        unit->timer = unit->timer - 1;
        if (unit->timer == 0) {
            unit->flags &= 0xEF;
            if (unit->state != 2) {
                unit->state = 1;
                func_800AC498(unit);
            }
        }
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B332C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B3494);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B34F8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B3644);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B37B8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B3A48);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B3B84);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B4060);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B42AC);

void func_800B46D4(CondorUnit* unit) {
    if (unit->unk58 | unit->unk5A) {
        func_800B4720();
        func_800B42AC(unit);
    }
}

void func_800B4720(CondorUnit* unit) {
    u16 step;

    step = unit->unk58 + unit->unk3A;
    unit->unk3A = step;
    step = step >> 8;
    if (step != 0) {
        if (step >= 0x80) {
            step |= 0xFF00;
        }
        unit->unk48 += step;
        unit->unk50 += step;
    }

    step = unit->unk5A + unit->unk3B;
    unit->unk3B = step;
    step = step >> 8;
    if (step != 0) {
        if (step >= 0x80) {
            step |= 0xFF00;
        }
        unit->unk4A += step;
        unit->unk52 += step;
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B47C8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B4EFC);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B5678);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B5A50);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B5BDC);

void func_800B5E38(CondorUnit* unit) {
    s32 index;
    s32 kind;
    s16 active;

    func_800B5BDC(unit);
    index = unit - D_800BE8C0;

    if (unit->unk26 | unit->unk28) {
        func_800B46D4(unit);
    }

    kind = unit->unk13;
    active = 0;
    switch (kind) {
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
        active = 1;
        break;
    }
    if (active) {
        func_800B47C8(unit, index);
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B5EF4);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B61F4);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B636C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B64EC);

void func_800B6688(CondorUnit* unit) {
    s32 index;
    s32 kind;
    s16 active;

    if (func_800B64EC()) {
        return;
    }

    func_800B5BDC(unit);
    index = unit - D_800BE8C0;

    if (unit->unk26 | unit->unk28) {
        func_800B46D4(unit);
    }

    kind = unit->unk13;
    active = 0;
    switch (kind) {
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
        active = 1;
        break;
    }
    if (active) {
        func_800B47C8(unit, index);
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B6758);

void func_800B6900(CondorUnit* unit) {
    s32 index;
    s32 kind;
    s16 active;

    func_800B6758();
    func_800B5BDC(unit);
    index = unit - D_800BE8C0;

    if (unit->unk26 | unit->unk28) {
        func_800B46D4(unit);
    }

    kind = unit->unk13;
    active = 0;
    switch (kind) {
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
        active = 1;
        break;
    }
    if (active) {
        func_800B47C8(unit, index);
    }
}

void func_800B69C4(CondorUnit* unit) {
    s32 index;
    s32 kind;
    s16 active;
    s16 other;

    if (unit->unk02 == 0xA) {
        if (unit->state != 6) {
            unit->state = 6;
            func_800AC498(unit);
            return;
        }
        if (unit->unk2C == 0xFF) {
            unit->state = 0;
            func_800AC498(unit);
            unit->unk02 = 0;
            other = func_800B0604(unit, 0xB);
            if (other != -1) {
                unit->unk24 = 0x80;
                D_800BE8C0[other].unk60 = 0x180;
                D_800BE8C0[other].unk41 = unit - D_800BE8C0;
            }
        }
    } else {
        func_800B5BDC(unit);
        index = unit - D_800BE8C0;

        if (unit->unk26 | unit->unk28) {
            func_800B46D4(unit);
        }

        kind = unit->unk13;
        active = 0;
        switch (kind) {
        case 1:
        case 2:
        case 5:
        case 6:
        case 7:
            active = 1;
            break;
        }
        if (active) {
            func_800B47C8(unit, index);
        }
    }
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B6B58);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B6CF8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B70C8);

void func_800B7630(void) {}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B7638);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B779C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B7874);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B7AEC);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B7EC0);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B8954);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800B9668);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800BA804);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800BAC4C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800BADD8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800BAF80);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800BB4EC);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800BDA24);

void func_800BE77C(void) { D_8012C3FC = ((D_8012C3E8 > 0) && (D_8012C41C != -1)) ? D_800BE8C0[D_8012C41C].unk0A : -1; }
