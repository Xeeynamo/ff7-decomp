//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"

static void func_800BB75C(Unk800BB75C* arg0, MATRIX* m, s16* arg2, s16* arg3);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8360);

static void func_800B83C4() {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_80151200[i].D_8015120C & 1) {
            func_800BA4C8();
            func_800BA40C();
            return;
        }
    }
    func_800BA40C();
    func_800BA4C8();
}

void func_800B8438(void) {
    func_800B9568();
    if (D_801635FC) {
        D_801635FC--;
    }
    switch (D_80163C7C) {
    case 2:
        func_800B905C();
        func_800BC440();
        func_800BA4C8();
        break;
    case 0:
    case 1:
    case 6:
        break;
    case 3:
    case 4:
    case 5:
    default:
        func_800B8EE4();
        func_800B905C();
        func_800B8234(D_801517BC);
        func_800BC440();
        func_800B7FB4();
        func_800B83C4();
        func_800B8B48();
        break;
    }
    func_800B7FB4();
    func_800B91CC();
    D_80151694 = D_80163758[1];
    func_800B85E0();
    func_800BC81C(D_800F8370, D_801518E4[D_801590CC].D_80151906);
    func_800BC8B0(D_800F8370);
    func_800B8268();
    SetFarColor(0, 0, 0);
    func_800BC538();
    func_800BC348();
    func_800BB75C(&D_800FA63C, &D_800FA958, &D_80158D00, &D_801031E8);
    func_800C627C();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B85E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B888C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B88CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8944);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8A34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8B48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8E48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8EE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8FCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B905C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B91CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B950C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B9568);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA11C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA24C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA2BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA360);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA40C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA4C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA598);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BACEC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BAF34);

static void func_800BAFF8(MATRIX* m, VECTOR* v) {
    ScaleMatrix(m, v);
    SetRotMatrix(m);
    SetTransMatrix(m);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB030);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB2A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB430);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB4F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB538);

void func_800BB67C(s32 arg0, Unk800BB67C* arg1) { arg1->unk30 = arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB684);

static void func_800BB75C(Unk800BB75C* arg0, MATRIX* m, s16* arg2, s16* arg3) {
    SVECTOR sv;

    func_800D85B0(m, arg2, arg3, &D_800E7D10);
    RotMatrixYXZ(&arg0->sv, &arg0->m);
    TransMatrix(&arg0->m, &arg0->v);
    MulMatrix2(m, &arg0->m);
    SetRotMatrix(m);
    SetTransMatrix(m);
    RotTrans(&arg0->v, (SVECTOR*)&arg0->m.t, &sv);
    func_800BAFF8(&arg0->m, &D_800E7D20);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB804);

static void func_800BB89C(void);
static void func_800BB864(void) {
    func_80033E34(LBA_ENEMY6_OVER2, 0x800, (u_long*)0x801D0000, func_800BB89C);
    func_800B7FB4();
}

static void func_800BB89C(void) {
    D_80163B80 = 0;
    D_800FA6B8 = 0;
    D_8009A000 = !(!(D_8016376A & 0x10) && !D_80083338) ? 0x10 : 0x14;
    D_8009A004 = 0x801D0000;
    func_8002DA7C();
}

void func_800BB90C(void) {
    D_8009A000 = 0xA0;
    D_8009A004 = 0x7F;
    func_8002DA7C();
}

static void func_800BB944(void) {
    func_800BB90C();
    D_8009A000 = 0xF1;
    func_8002DA7C();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB978);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB9B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB9FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBA40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBA84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBB20);

static void func_800BBDF8(void) {
    if (D_800F8368 == 0) {
        D_800F4AF4 = D_80163C74;
        if ((u32)D_80163C74 > (u32)0x80184000) {
            func_8003CEBC();
            func_80043938(1);
            StopCallback();
            SystemError('b', 0);
        }
    } else {
        D_800F4AF8 = D_80163C74;
        if ((u32)D_80163C74 > (u32)0x801A0000) {
            func_8003CEBC();
            func_80043938(1);
            StopCallback();
            SystemError('b', 1);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBEAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBF7C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC04C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC11C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC1E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC2F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC348);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC440);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC538);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC630);

void func_800BCA58(s32);
void func_800C1104();
static void func_800BC72C(void) {
    func_800C1104();
    func_800BCA58(3);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC754);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC81C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC8B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BCA58);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BCB1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE49C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE69C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE86C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BEA38);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFA98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFB10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFB88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFDA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFF88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C018C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0254);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0314);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C03B8);

s32 func_800C03FC(s32 arg0, s32 arg1) { return arg0 < 0 ? -arg1 : arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0410);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0630);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0900);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0970);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0B20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0DD8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1104);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C14C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C169C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C17A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1908);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1D8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2000);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C20E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2150);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C223C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2704);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2928);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2F20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2FD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3068);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C328C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C33F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3578);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C36B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3950);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3AA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3CA8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3DE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3F44);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C40F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C428C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C44B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C45EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C4814);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C494C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C4B60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C4D10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C4DC8);

void* func_800C5040(u8, u8, u8, s32, u8*);
extern u8* D_801517C0;

void* func_800C4FC8(u8 arg0, u8 arg1, u8 arg2) {
    return func_800C5040(arg0, arg1, arg2, 1, D_801517C0 + 0x4084);
}

void* func_800C5004(u8 arg0, u8 arg1, u8 arg2) {
    return func_800C5040(arg0, arg1, arg2, 2, D_801517C0 + 0x40EC);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5040);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5170);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5468);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C55B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5694);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C57B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C59B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5ADC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5BEC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5C18);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5CC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5E94);

s32 func_800C60F4(void) { return _work.battle_msg_speed / 4 + 4; }

static void func_800C610C(void) {
    while (D_801518DC) {
        func_800B7FB4();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C614C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C61C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C627C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C62F4);

void func_800C679C(void);

void func_800C64AC(void) { func_800BBEAC(func_800C679C); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C64D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C6628);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C679C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C6CB8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C70AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C7220);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C7340);

static void func_800C74A4(void) {
    if (!(D_80153BDD & 2)) {
        func_800C7C4C(3, D_800F57D0->unk8, D_800F57D0 + 1, D_800F57D0);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C74E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C76C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C7924);
