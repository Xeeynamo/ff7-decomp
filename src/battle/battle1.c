//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"
#include <libetc.h>
#include <libgpu.h>

static void func_800B37EC(void);
static void func_800B3D38(void);
static void func_800B3D88(void);
static void func_800B3DBC(void);
static void func_800B7FDC(void);
static void func_800B85E0();
static void func_800BA4C8(void);
void func_800BA598(s16);
void func_800BB030(s16);
static void func_800BB75C(Unk800BB75C* arg0, MATRIX* m, s16* arg2, s16* arg3);
static void func_800BB804(void);
static void func_800BB864(void);
int func_800C4D10();

void func_800B30E4(void) {
    s32 i;

    D_801517C0 = &D_800FAFF4;
    D_801031E4 = 0;
    D_800F8368 = 0;
    D_80162084 = 0x200;
    func_800B383C();
    func_800B430C();
    VSync(0);
    SetDispMask(0);
    D_800F9F34 = 0;
    *(s8*)&D_800FA63C.u.sub.unk34 = 0;
    D_800FA6A0 = 0;
    func_800B37A0();
    func_800B3E2C();
    func_800BB684();
    func_800BC04C(func_800C4D10);
    func_800B7FDC();
    func_800B7FDC();
    do {
    } while (D_80095DD4);
    func_800B37EC();
    SetDispMask(1);
    while (1) {
        switch (D_80163C7C) {
        case 0:
            D_801635FC = 0x3D;
            func_800B38E0();
            func_800B7FDC();
            D_80163C7C = 1;
            break;
        case 1:
            func_800B7FDC();
            if (D_800F7DF4 == (u8)D_80166F64 && D_801518DC == 0) {
                func_800B3D38();
                func_800B5138();
                D_80163C7C = 6;
            }
            break;
        case 6:
            func_800B7FDC();
            func_800B3D88();
            for (i = 4; i < D_800F7E04[0] + 4; i++) {
                D_801518E4[i].D_80151922 |= 4;
            }
            D_80163C7C = 2;
            break;
        case 2:
            func_800B7FDC();
            if ((u8)D_80166F64 == 3 && D_801518DC == 0) {
                func_800B3DBC();
                D_80163C7C = 3;
                D_801518E4[0].D_80151922 |= 4;
                D_801518E4[1].D_80151922 |= 4;
                D_801518E4[2].D_80151922 |= 4;
            }
            break;
        case 3:
            func_800B7FDC();
            if (D_801635FC == 0) {
                D_80163C7C = 4;
                func_800C61C0();
            }
            break;
        default:
            return;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B33A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B36B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B37A0);

static void func_800B37EC(void) {
    D_80162094 = 4;
    func_800D8A78(4);
    func_800E15D8();
    func_800D9E0C(-1, -1, 0);
    D_80095DD4 = 2;
}

// Load stage files
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B383C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B38E0);

static void func_800B3934(void) {
    func_800B5D38(2);
    func_800B5CD4(2);
    D_80166F64 = 3;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3968);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3A04);

void func_800B3AB8(void);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3AB8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3B84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3C50);

static void func_800B3CD0(void) {
    Yamada* y;
    u_long* dst;

    dst = (u_long*)0x801B0000;
    func_800D2980(dst, 0, 0, 0);
    y = &D_800E8068[D_800FA9C4];
    DS_read(y->loc, *&D_800E8068[D_800FA9C4].len, dst, func_800B3AB8);
    func_800B7FB4();
}

static void func_800B3D38(void) {
    func_800C5E94();
    D_800F839C = D_800EA50C;
    DS_read(LBA_ENEMY6_SEFFECT, 0xA800, (u_long*)0x801B0000, func_800B3CD0);
    func_800B7FB4();
}

static void func_800B3D88(void) {
    func_800B588C();
    func_800B6B98(4, 10);
    func_800B36B4();
}

static void func_800B3DBC(void) {
    s32 i;

    func_800B4794();
    func_800B6B98(0, 3);
    func_800B6B98(3, 3);
    if (D_8016360C.setup.stageID == 57) {
        for (i = 0; i < 10; i++) {
            D_801518E4[i].D_80151909 |= 0x10;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3E2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3FAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3FFC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B430C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B45F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B46B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B4794);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B4E30);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5138);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B54B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B588C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5AAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5CD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5D38);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5E64);

void func_800B60E0(s16);
void func_800B5FC4(s16 arg0) { func_800B60E0(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5FE8);

void func_800B60E0(s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B60E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B64CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B677C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B6B98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B6D6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B7764);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B798C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B79F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B7DB4);

static void func_800B7F6C(void) {
    volatile s32 padding;

    while (D_80062D99) {
        func_800B7FB4();
    }
    D_80062D98 = 0;
}

void func_800B7FB4(void) { D_801518DC = func_80034B44(); }

static void func_800B7FDC(void) {
    s32 i;

    func_800B7FB4();
    ClearOTagR((u_long*)D_801517C0->unk40A4, LEN(D_801517C0->unk40A4));
    ClearOTag((u_long*)D_801517C0->unk4070, LEN(D_801517C0->unk4070));
    ClearOTag((u_long*)D_801517C0->unk4078, LEN(D_801517C0->unk4078));
    ClearOTagR((u_long*)D_801517C0->unk70, LEN(D_801517C0->unk70));
    ClearOTagR((u_long*)D_801517C0->unk4080, LEN(D_801517C0->unk4080));
    ClearOTag((u_long*)D_801517C0->unk40E4, LEN(D_801517C0->unk40E4));
    ClearOTag((u_long*)D_801517C0->unk40EC, LEN(D_801517C0->unk40EC));
    D_80163C74 = D_800F8368 == 0 ? (DR_MODE*)0x80168000 : (DR_MODE*)0x80184000;
    func_800B8360(1);
    func_800C5CC0();
    func_800B8438();
    for (i = 0; i < 10; i++) {
        if (D_801518E4[i].D_8015190A == 0) {
            D_800F7DE4 = 0;
            break;
        }
        if (D_80162080 == 0) {
            D_800F7DE4 = 1;
        } else {
            D_800F7DE4 = 0;
        }
    }
    func_800A3ED0();
    func_800B8360(2);
    func_800DCFD4((u_long*)D_801517C0->unk40E4);
    if (D_800F9D94 == 0) {
        ResetGraph(1);
        D_800F9D94 = 1;
    }
    if (D_8016376A & 2) {
        func_800E16B8(D_801517C0->unk40E4, 0x10, 0x10, D_8009D268[0]);
    }
    D_800FA9B8 = VSync(1);
    BATTLE_FlushImageQueue();
    func_800B7FB4();
    D_80158D08 = func_800D8A88();
    SetGeomScreen(D_80162084);
    D_801516F4++;
    func_800B7F6C();
    func_800B950C();
    D_801516A0 = D_800F198C;
}

void func_800B8234(s32 arg0) {
    if (arg0) {
        func_800D0C80(D_801590CC);
        D_801517BC = 0;
    }
}

static void func_800B8268(void) {
    s32 i;
    u8* var_a1;
    s32 var_t1;

    i = 0;
    var_t1 = 1;
    var_a1 = D_80163784;
    while (i < 10) {
        *var_a1 = D_801636B8[i].D_801636B9;
        if (!(D_80151200[i].D_8015120C & 8) &&
            D_801518E4[i].D_801518E6 != *var_a1 &&
            D_801518E4[i].D_8015190A == var_t1) {
            D_801518E4[i].D_80151922 |= 1;
            D_801518E4[i].D_801518E6 = *var_a1;
        }
        var_a1++;
        i += 1;
    }
    D_80163787 = 0;
}

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

static void func_800B85E0() {
    s32 i;

    if (D_800F7ED4 != 100 && D_800FA6B8) {
        func_800BB804();
        D_80163C7C = 5;
        func_800D8B2C();
        D_800F7ED4 = 100;
        D_80163798[D_801590E0].unk8 = -3;
        func_800BB684();
        for (i = 0; i < 3; i++) {
            D_801518E4[i].D_80151922 |= 0x20;
            D_80151200[i].D_80151200 = D_801636B8[i].D_801636C0;
        }
    }
    if (D_800F9D98 != 100 && (D_800707BE & 1)) {
        D_80163C7C = 5;
        func_800D8B2C();
        D_800F9D98 = 100;
        D_80163798[D_801590E0].unk8 = -1;
        func_800BB684();
    }
    if (!D_801590D8 && D_80163B80) {
        func_800BB864();
        D_801590D8 = 1;
    }
    if (D_800F9D9C != 100) {
        i = 0;
        if (D_800707BE & 8) {
            for (; i < 3; i++) {
                D_801518E4[i].D_80151922 |= 1;
                D_801518E4[i].D_801518E6 = D_801636B8[i].D_801636B9;
                D_801518E4[i].D_80151922 |= 0x20;
                D_80151200[i].D_80151200 = D_801636B8[i].D_801636C0;
            }
            D_800F9D9C = 100;
            D_80163C7C = 5;
            func_800D8B2C();
            D_80163798[D_801590E0].unk8 = -1;
            func_800BB684();
        }
    }
}

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

static void func_800BA40C(void) {
    s32 i;
    u8 param;

    for (i = 0; i < 3; i++) {
        if (!(D_801518E4[i].D_80151909 & 2)) {
            param = i;
            func_800C1908(param);
            func_800BA598(i);
            if (D_801518E4[i].D_8015190B & 0x80) {
                func_800BB2A8(param);
                func_800BB030(i);
            }
        }
    }
}

static void func_800BA4C8(void) {
    s32 i;

    for (i = 4; i < D_800F7E04[0] + 4; i++) {
        if (!(D_801518E4[i].D_80151909 & 0x80)) {
            continue;
        }
        if (D_801518E4[i].D_80151909 & 2) {
            continue;
        }
        func_800C1908(i);
        func_800BA598(i);
        if (D_801518E4[i].D_8015190B & 0x80) {
            func_800BB030(i);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA598);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BACEC);

static void func_800BAF34(BattleModelSub* modelSub) {
    s32 flag;

    *(MATRIX**)0x1F800020 = modelSub->pm;
    *(MATRIX*)0x1F800024 = **(MATRIX**)0x1F800020;
    MulMatrix2((MATRIX*)0x1F800024, &modelSub->m);
    SetRotMatrix((MATRIX*)0x1F800024);
    SetTransMatrix((MATRIX*)0x1F800024);
    RotTrans(&modelSub->sv2, (VECTOR*)modelSub->m.t, &flag);
    SetRotMatrix(&modelSub->m);
    SetTransMatrix(&modelSub->m);
}

static void func_800BAFF8(MATRIX* m, VECTOR* v) {
    ScaleMatrix(m, v);
    SetRotMatrix(m);
    SetTransMatrix(m);
}

static void func_800BB030(s16 arg0) {
    s32 i;
    Unk801B0C98* unk;

    unk = (Unk801B0C98*)0x1F800020;
    SetFarColor(D_801518E4[arg0].D_8015190C, D_801518E4[arg0].D_8015190D,
                D_801518E4[arg0].D_8015190E);
    SetRotMatrix(&D_801518E4[arg0].m);
    SetTransMatrix(&D_801518E4[arg0].m);
    for (i = 0; i < D_800FA6D8[arg0].unk3C; i++) {
        RotMatrixYXZ(
            &D_800FA6D8[arg0].unk8[i].sv1, &D_800FA6D8[arg0].unk8[i].m);
    }

    for (i = 0; i < D_800FA6D8[arg0].unk3C; i++) {
        func_800BAF34(&D_800FA6D8[arg0].unk8[i]);
        if (!D_800FA6D8[arg0].unk4[i])
            continue;
        unk->unk0 = D_800FA6D8[arg0].unk4[i];
        unk->unk4 = D_800FA6D8[arg0].unk3E[i] | 0x180;
        unk->unk8 = 0;
        unk->unkA = D_801518E4[arg0].unk14[0];
        unk->unkC = 0x20;
        unk->unkE = D_801518E4[arg0].unk14[1];
        if (D_801518E4[arg0].D_80151909 & 4) {
            continue;
        }
        D_80163C74 = func_800D29D4(unk, D_801517C0->unk70, 12, D_80163C74);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB2A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB430);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB4F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB538);

void func_800BB67C(s32 arg0, Unk800BB67C* arg1) { arg1->unk30 = arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB684);

static void func_800BB75C(Unk800BB75C* arg0, MATRIX* m, s16* arg2, s16* arg3) {
    int flag;

    func_800D85B0(m, arg2, arg3, &D_800E7D10);
    RotMatrixYXZ(&arg0->sv, &arg0->m);
    TransMatrix(&arg0->m, &arg0->u.v);
    MulMatrix2(m, &arg0->m);
    SetRotMatrix(m);
    SetTransMatrix(m);
    RotTrans(&arg0->u.sub.sv2, (VECTOR*)&arg0->m.t, &flag);
    func_800BAFF8(&arg0->m, &D_800E7D20);
}

static void func_800BB89C(void);
static void func_800BB804(void) {
    if (!(D_8016376A & 0x20)) {
        func_80033E34(
            LBA_ENEMY6_FAN2, 0x1000, (u_long*)0x801D0000, func_800BB89C);
        func_800B7FB4();
        return;
    }
    D_80163B80 = 0;
    D_800FA6B8 = 0;
}

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
            PadStop();
            ResetGraph(1);
            StopCallback();
            SystemError('b', 0);
        }
    } else {
        D_800F4AF8 = D_80163C74;
        if ((u32)D_80163C74 > (u32)0x801A0000) {
            PadStop();
            ResetGraph(1);
            StopCallback();
            SystemError('b', 1);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBEAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBF7C);

s32 func_800BC04C(void (*cb)(void));
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

static s32 func_800C169C(u8 arg0) {
    D_801518E4[arg0].D_80151909 |= 8;
    if (D_80151200[arg0].D_80151200 & 0x2000) {
        return 10;
    }
    if (D_80151200[arg0].D_80151200 & 0x4000) {
        return 5;
    }
    if (D_80151200[arg0].D_80151200 & 0x0008) {
        return 1;
    }
    if (D_80151200[arg0].D_80151200 & 0x800000) {
        return 3;
    }
    if (D_80151200[arg0].D_80151200 & 0x01000000) {
        return 6;
    }
    if (D_80151200[arg0].D_80151200 & 0x04000000) {
        return 8;
    }
    if (D_80151200[arg0].D_80151200 & 0x8000) {
        return 9;
    }
    if (D_80151200[arg0].D_80151200 & 0x400000) {
        return 7;
    }
    D_801518E4[arg0].D_80151909 &= ~8;
    return 0;
}

static void func_800C17A0(s32 arg0, s32 arg1) {
    switch (D_800EA19C[arg1][0]) {
    case 0:
        D_801518E4[arg0].unk14[0] = 0;
        break;
    case 1:
        D_801518E4[arg0].unk14[0] = 0x800;
        break;
    case 2:
        D_801518E4[arg0].unk14[0] = 0xC00;
        break;
    }
    D_801518E4[arg0].D_8015190C = D_800EA19C[arg1][1];
    D_801518E4[arg0].D_8015190D = D_800EA19C[arg1][2];
    D_801518E4[arg0].D_8015190E = D_800EA19C[arg1][3];
    D_801518E4[arg0].D_80151908 = 0;
}

static void func_800C5468(u8 arg0);
void func_800C5170(u8);

static void func_800C1908(u8 arg0) {
    s32 temp_a1;
    s16 var_a0;
    u8 temp_s0;

    temp_s0 = arg0;
    if (D_801518E4[temp_s0].D_80151922 & 0x20) {
        if (temp_s0 < 4) {
            D_800F9F28[temp_s0] = D_801636B8[temp_s0].D_801636C0;
        }
        func_800C5170(temp_s0);
        func_800C5468(temp_s0);
        func_800C17A0(temp_s0, func_800C169C(temp_s0));
        D_801518E4[temp_s0].D_80151922 &= 0xDF;
    }
    temp_a1 = arg0;
    if (D_80151200[temp_a1].D_80151235 == 0) {
        if (D_80151200[temp_a1].D_80151200 & 0x4000) {
            D_80151200[temp_a1].D_80151233 = 3;
            return;
        }
        D_80151200[temp_a1].D_80151233 = 0;
        if (D_80151200[temp_a1].D_80151200 & 0x100) {
            D_80151200[temp_a1].D_80151233 = 1;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x200) {
            D_80151200[temp_a1].D_80151233 = 2;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x400) {
            D_80151200[temp_a1].D_80151233 = 3;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x02000000) {
            D_80151200[temp_a1].D_80151233 = 3;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x40) {
            if (D_801518E4[temp_a1].D_801518E6 == D_80163784[temp_a1]) {
                D_801518E4[temp_a1].unk160.vy += 0x100;
            }
        }
        var_a0 = arg0;
        if (D_80151200[var_a0].D_80151200 & 0x400000 &&
            D_801518E4[var_a0].D_801518E6 == D_80163784[var_a0]) {
            if (D_801518E4[var_a0].D_801518FC == 0) {
                D_801518E4[var_a0].unk160.vy = 0x800;
            } else {
                D_801518E4[var_a0].unk160.vy = 0;
            }
        }
        var_a0 = arg0;
        if (D_801518E4[var_a0].D_80151909 & 8) {
            if (D_801518E4[var_a0].D_80151908 < 0x10) {
                D_801518E4[var_a0].unk14[0] += 0x80;
            } else {
                D_801518E4[var_a0].unk14[0] -= 0x80;
            }
            D_801518E4[arg0].D_80151908--;
            D_801518E4[arg0].D_80151908 &= 0x1F;
        }
    }
}

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

static u_long* func_800C5040(u8 r, u8 g, u8 b, s32 tpage, u_long* ot);

u_long* func_800C4FC8(u8 r, u8 g, u8 b) {
    return func_800C5040(r, g, b, 1, (u_long*)&D_801517C0->unk4080[1]);
}

u_long* func_800C5004(u8 r, u8 g, u8 b) {
    return func_800C5040(r, g, b, 2, (u_long*)&D_801517C0->unk40EC);
}

static u_long* func_800C5040(u8 r, u8 g, u8 b, s32 tpage, u_long* ot) {
    DR_MODE* drMode;
    POLY_F4* poly;

    drMode = D_80163C74;
    SetDrawMode(drMode, 1, 0, (tpage & 3) << 5, NULL);
    poly = (POLY_F4*)(drMode + 24);
    SetPolyF4(poly);
    SetSemiTrans(poly, 1);
    poly->r0 = r;
    poly->g0 = g;
    poly->b0 = b;
    poly->x0 = 0;
    poly->y0 = 8;
    poly->x1 = 320;
    poly->y1 = 8;
    poly->x2 = 0;
    poly->y2 = 166;
    poly->x3 = 320;
    poly->y3 = 166;
    addPrim(ot, poly);
    addPrim(ot, drMode);
    return (u_long*)(poly + 1);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5170);

static void func_800C55B8(void);
static void func_800C5468(u8 arg0) {
    s32 var_v0_2;
    s32 var_v0;
    u16 temp_a1;

    var_v0 = arg0;
    if (D_80151200[var_v0].D_80151200 & 0x1000) {
        temp_a1 = D_80151200[var_v0].D_8015120C;
        if (!(temp_a1 & 0x80)) {
            D_80151200[var_v0].D_8015120C |= 0x80;
            var_v0_2 = func_800BC04C(func_800C55B8);
            D_801621F0[var_v0_2].D_801621F6 = arg0;
            D_801621F0[var_v0_2].D_801621F4 = 0x10;
            D_801621F0[var_v0_2].D_801621F2 = -0x80;
        }
    } else {
        temp_a1 = D_80151200[var_v0].D_8015120C;
        if (temp_a1 & 0x80) {
            D_80151200[var_v0].D_8015120C = temp_a1 & (~0x80);
            var_v0_2 = func_800BC04C(func_800C55B8);
            var_v0_2 = var_v0_2;
            D_801621F0[var_v0_2].D_801621F6 = arg0;
            D_801621F0[var_v0_2].D_801621F4 = 0x10;
            D_801621F0[var_v0_2].D_801621F2 = 0x80;
        }
    }
}

static void func_800C55B8(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801518E4[D_801621F0[D_801590D4].D_801621F6].D_801518EA +=
        D_801621F0[D_801590D4].D_801621F2;
    D_801621F0[D_801590D4].D_801621F4--;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5694);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C57B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C59B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5ADC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5BEC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5C18);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5CC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5E94);

s32 func_800C60F4(void) { return Savemap.battle_msg_speed / 4 + 4; }

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
    if (!(D_801518E4[3].D_80151909 & 2)) {
        func_800C7C4C(3, D_800F57D0->unk8, D_800F57D0 + 1, D_800F57D0);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C74E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C76C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C7924);
