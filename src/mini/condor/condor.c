//! PSYQ=3.3
#include <game.h>

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
    /* 0x0C */ u8 unk0C[7];
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
    /* 0x42 */ u8 unk42[6];
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
    /* 0x62 */ u8 unk62[0x16];
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
extern s32 D_8012C3A4[];
extern s32 D_8012C3AC[];
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
extern u16 D_8012B85C;
extern u16 D_80180000[];
extern s32 D_8012C544;
extern s32 D_8012C548;

// Still INCLUDE_ASM. Declared because GCC 2.x compiles an undeclared call as
// implicit int without saying anything, and the object comes out wrong.
u32 func_800AC498(CondorUnit*);
void func_800ABE54(CondorUnit*);
void func_800AC72C(CondorUnit*, CondorSpawn*, s32, s16*);
s16 func_800AC5C0(s16, CondorPoint*);
s16 func_800B0604(CondorUnit*, s32);
s16 func_800A8004(CondorUnit*, CondorPoint*, u8*, u16);
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
void func_800AB71C(s32, s32*);
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

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A7BF4);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A7E20);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800A8004);

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
                    if (((s16*)D_8012C3A4)[1] >= 0x2A0) {
                        return 1;
                    }
                } else if (((s16*)D_8012C3A4)[1] >= D_8012C530) {
                    return 1;
                }
                {
                    at.z = -0x28;
                    at.x = *(u16*)D_8012C3A4;
                    at.y = 0x400 - ((u16*)D_8012C3A4)[1];
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
    s32* table = D_8012C3A4;

    if (D_8012C5CC == 0) {
        table = D_8012C3AC;
    }
    if ((D_8012C3E8 != 3) && (D_8012C3EC == 0)) {
        table = D_8012C3A4;
    }
    func_800AB71C(arg0, table);
}

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AB71C);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800ABA58);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800ABC18);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800ABE54);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AC1A8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AC498);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AC5C0);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AC68C);

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

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AF5A8);

INCLUDE_ASM("asm/us/mini/condor/nonmatchings/condor", func_800AF98C);

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
