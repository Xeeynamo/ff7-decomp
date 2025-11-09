//! PSYQ=3.3 CC1=2.7.2
#include "battle_private.h"

extern Yamada D_800EEBB8[]; // MAGIC/*.BIN overlay
extern s16 D_800EF5B0[];
extern s16 D_800EF63C[];
extern s16 D_800EF6A8[];
extern s16 D_800EF6D8[];
extern s16 D_800EF6FC[];
extern s16 D_800EF838[];
extern s16 D_800EF8D8[];

void func_800D1530();

// MAGIC/ entrypoints
void func_801B037C(s16, u8);
void func_801B0000(s16, u8);
void func_801B0000_2(s16, u8);
void func_801B000C(s16, u8);
void func_801B0040(s16, u8);
void func_801B0054(s16, u8);
void func_801B0084(s16, u8);

s32 func_800BC04C(void (*f)());

static void func_800D0AD4(void);
void func_800D0A4C(void) {
    s32 ret;
    s32 i;

    for (i = 0; i < LEN(D_801518E4); i++) {
        D_801518E4[i].D_80151909 |= 1;
    }
    func_801B0040(D_80151774, D_801590CC);
    ret = func_800BC04C(func_800D0AD4);
    *(s32*)0x1F800000 = ret;
    D_801621F0[ret].D_801621F4 = 2;
}

static void func_800D0AD4(void) {
    if (!D_801621F0[D_801590D4].D_801621F4) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        func_800BB978();
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

void func_800D0B4C(u8 arg0) {
    D_800F8CF0 = 0;
    func_800D1530();
    switch (D_801518E4[arg0].D_80151907) {
    case 4:
        D_800EF9D8[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 7:
        func_801B037C(D_80151774, D_801590CC);
        break;
    case 8:
        D_800EFFE0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    }
}

void func_800D0C80(u8 arg0) {
    D_800F8CF0 = 0;
    func_800D1530();
    switch (D_801518E4[arg0].D_80151907) {
    case 2:
        if (D_801031F0 == 0) {
            if (D_801518E4[arg0].D_80151906 == 25) {
                D_801518E4[0].D_8015190A = 1;
                D_801518E4[1].D_8015190A = 1;
                D_801518E4[2].D_8015190A = 1;
            }
            D_800EFAF0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
            return;
        }
        switch (D_801518E4[arg0].D_80151906) {
        case 41:
            func_801B0000(D_80151774, D_801590CC);
            break;
        case 44:
            func_801B0000_2(D_80151774, D_801590CC);
            break;
        case 35:
            func_801B000C(D_80151774, D_801590CC);
            break;
        case 32:
            func_801B0054(D_80151774, D_801590CC);
            break;
        case 29:
            func_801B0084(D_80151774, D_801590CC);
            break;
        default:
            D_800EFAF0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
            break;
        }
        break;
    case 13:
        D_800EFBC8[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 20:
        if (D_801518E4[arg0].D_80151906 == 2) {
            if (D_801590CC == D_800FA9E8) {
                D_80163A98 = 0;
            } else {
                D_80163A98 = 1;
            }
        }
        *(s32*)0x1F800000 =
            D_800EFEA0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        switch (D_801518E4[arg0].D_80151906) {
        case 0x2D:
        case 0x2E:
        case 0x2F:
        case 0x30:
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4F:
            *(s32*)0x1F800000 = 0;
            break;
        }
        func_800D08B8(arg0, *(s32*)0x1F800000);
        break;
    case 32:
        D_800EFC28[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 3:
        func_800C64AC();
        break;
    }
}

void func_800D1110(u8 arg0) {
    s32 lba;
    s32 var_a1;
    s32 id;

    switch (D_801518E4[arg0].D_80151907) {
    case 2:
        if (D_801031F0 == 0) {
            id = D_800EF63C[D_801518E4[arg0].D_80151906];
            func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        } else {
            switch (D_801518E4[arg0].D_80151906) {
            case 29:
                func_800D088C(D_800EEBB8[77].loc, D_800EEBB8[77].len);
                break;
            case 41:
                func_800D088C(D_800EEBB8[32].loc, D_800EEBB8[32].len);
                break;
            case 44:
                func_800D088C(D_800EEBB8[26].loc, D_800EEBB8[26].len);
                break;
            case 32:
                func_800D088C(D_800EEBB8[255].loc, D_800EEBB8[255].len);
                break;
            case 35:
                func_800D088C(D_800EEBB8[6].loc, D_800EEBB8[6].len);
                break;
            default:
                id = D_800EF63C[D_801518E4[arg0].D_80151906];
                lba = D_800EEBB8[id].loc;
                var_a1 = D_800EEBB8[id].len;
                func_800D088C(lba, var_a1);
                break;
            }
        }
        break;
    case 7:
        func_800D088C(D_800EEBB8[221].loc, D_800EEBB8[221].len);
        break;
    case 8:
        id = D_800EF8D8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 13:
        id = D_800EF6A8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 20:
        id = D_800EF838[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 4:
        id = D_800EF5B0[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 32:
        id = D_800EF6FC[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 3:
        id = D_800EF6D8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D1530);

void BATTLE_EnqueueLoadImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_LOAD_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

void BATTLE_EnqueueStoreImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_STORE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

void BATTLE_EnqueueMoveImage(RECT* rect, s32 x, s32 y) {
    D_800F01DC->method = QUEUE_MOVE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->x = x;
    D_800F01DC->y = y;
    D_800F01DC++;
}

void BATTLE_EnqueueClearImage(RECT* rect) {
    D_800F01DC->method = QUEUE_CLEAR_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC++;
}

void BATTLE_FlushImageQueue(void) {
    Unk800F01DC* item;

    for (item = D_800F4BAC; item < D_800F01DC; item++) {
        switch (item->method) {
        case QUEUE_LOAD_IMAGE:
            LoadImage(item->rect, item->ptr);
            break;
        case QUEUE_STORE_IMAGE:
            StoreImage(item->rect, item->ptr);
            break;
        case QUEUE_MOVE_IMAGE:
            MoveImage(item->rect, item->x, item->y);
            break;
        case QUEUE_CLEAR_IMAGE:
            ClearImage(item->rect, 0, 0, 0);
            break;
        }
    }
    D_800F01DC = D_800F4BAC;
}

void BATTLE_ResetImageQueue(void) { D_800F01DC = D_800F4BAC; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D2710);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D2828);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D2980);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D29D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D32B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3354);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3418);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3474);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D34C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3520);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3548);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D35D8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3658);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D376C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3994);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3A6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3AF0);

const s32 D_800A0D98[] = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                          0x00001000, 0x00000000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3BF0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3D88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3E8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3F0C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3F8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D415C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D41FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4284);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4368);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4484);

const s32 D_800A0DB8[] = {0x00000000, 0xFFFFF000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D44E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D461C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4710);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D491C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4A64);

void func_800D4D6C(s32 arg0, s32 arg1, s32 arg2);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4C08);

void func_800D4CBC(s32, s32, s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4CBC);

void func_800D4D4C(s32 arg0, s32 arg1) { func_800D4CBC(arg0, arg1, 0x1000); }

void func_800D4D6C(s32 arg0, s32 arg1, s32 arg2) {
    func_800D4C08(arg0, arg1, 0x1000, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4D90);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4FA8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4FF0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D508C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5138);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D51D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5230);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D52A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5350);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5444);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D54BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D54EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D55A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D55F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D56A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D574C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5774);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D57C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D58D0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5938);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5A68);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5AC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5B6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5D28);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D61AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6260);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D650C);

extern u8 D_800F10EC[];
extern u8 D_800F11E8[];
extern u8 D_800F1304[];
u8* const D_800A0DC8[] = {D_800F10EC, D_800F11E8, D_800F1304};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6734);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D67BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D67E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6814);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6840);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6998);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6ACC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6C20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6D8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6F78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D70C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7178);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D72B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7340);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7368);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D751C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D76B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7724);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7888);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7A88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7B1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7BA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7C2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7D3C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D83A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8468);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D84F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D85B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D87EC);

void func_800D8A04(void) {}

s32 func_800D8A0C(s32 arg0) { return arg0 < 0 ? -arg0 : arg0; }

void func_800D8A24(void) {}

extern s32 D_800F199C;

void func_800D8A2C(void) { D_800F199C = 0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8A3C);

void func_800D8A70(void) {}

void func_800D8A78(s8 arg0) { D_800F19A4 = arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8A88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8AF0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8B2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8B60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8D78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D91DC);

void func_800D93DC(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D93E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D9BF4);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D9DEC);
#else
//! PSYQ=3.3 CC1=2.7.2
u8 func_800D9DEC(s16 arg0) { return D_800F514C[arg0]; }
#endif

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D9E0C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D9F5C);

void func_800D9F80(void) {
    D_800F311C = 0;
    D_800FAFEC = -0x100;
    D_800FAFF0 = -0x100;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D9FA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DA380);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DB818);

const s32 D_800A0E48[] = {0, 0, 0};
void func_800DBC18(s32, s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DBC18);

void func_800DBEA4(s32 arg0, s16 arg1) { func_800DBC18(arg0, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DBEC8);

void func_800DBF54(void) { func_800269C0(D_80077F64); }

void func_800DBF7C(void) { D_800F3122 = 0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DBF8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DC0CC);

void func_800DCF58(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DCF60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DCF94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DCFD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DD690);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DD85C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DDAD8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DDC34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DDCE8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DDE90);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DDFEC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DE2B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DE3CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DE46C);

void func_800DE5D8(void) {}

void func_800DE5E0(void) {}

void func_800DE5E8(void) {}

void func_800DE5F0(void) {}

void func_800DE5F8(void) {}

void func_800DE600(void) {}

void func_800DE608(void) {}

void func_800DE610(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DE618);

void func_800DE910(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DE918);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DE94C);

void func_800DEB10(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DEB18);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DEC10);

void func_800DF244(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DF24C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DF2CC);

void func_800DF528(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DF530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DF5C8);

void func_800DF7BC(void) {}

void func_800DF7C4(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DF7CC);

void func_800DF8F0(void) {}

void func_800DF8F8(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DF900);

void func_800DF9F0(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DF9F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DFA94);

void func_800DFC38(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DFC40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DFE34);

void func_800DFFDC(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800DFFE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E010C);

void func_800E026C(void) {}

void func_800E53C8();
void func_800E0274(void) { func_800E53C8(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E0294);

void func_800E03C8(void) {}

void func_800E4B88();
void func_800E03D0(void) { func_800E4B88(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E03F0);

void func_800E0528(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E0530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E05E4);

void func_800E078C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E0794);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E084C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E08C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E0BE0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E0DF4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E0E34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E15D8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E16B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E1938);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E1A2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E1AC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E1C40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E2054);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E2098);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E2C6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E3088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E33A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E368C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E3B64);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E3E10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E3FB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E4180);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E4394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E4A64);

void func_800E4B88();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E4B88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E4BCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E4C08);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E5358);

void func_800E53C8();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E53C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E54EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E5530);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E5814);

void func_800E58B0(void) {
    D_800F3468 = 0;
    D_800F5760 = 10;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E58CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E593C);

s32 func_800E5960(s32 arg0) { return arg0 < 0 ? -arg0 : arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E5978);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E5F30);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E5F70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E5FB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E6018);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E60F8);

s32 func_800E6820(void) { return func_80026B70(D_800F384A); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E6848);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E68B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E6904);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E6B40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E6B94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E6DCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800E7170);
