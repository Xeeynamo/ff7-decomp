//! PSYQ=3.3 CC1=2.7.2 G=8
#include "main_private.h"

s8 D_80062EBC = 0;
static s8 _D_80062EBD = 0;
static s8 _D_80062EBE = 0;
static s8 _D_80062EBF = 0;
s8 D_80062EC0 = 0;
static s8 _D_80062EC1 = 0;
static s8 _D_80062EC2 = 0;
static s8 _D_80062EC3 = 0;

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80021D5C);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80021E70);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80021F58);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80022B5C);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80022DE4);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80022FE0);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80023050);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_8002305C);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800230C4);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_8002368C);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80023788);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_8002382C);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80023940);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80023AC4);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80023AD4);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80024A04);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80024A3C);

void func_80024D88(s32 arg0) {
    func_800211C4(0xD);
    do {
    } while (func_80034B44());
    VSync(30);
    func_801D131C(arg0);
}

void func_80024DD4(s32 arg0) {
    func_800211C4(0xE);
    do {
    } while (func_80034B44());
    func_801D1A6C(arg0);
}

void func_80024E18(s32 arg0) {
    func_800211C4(0xF);
    do {
    } while (func_80034B44());
    func_801D4118(arg0);
}

// This should be the title screen handler
void func_80024E5C(void) {
    func_800211C4(0x10); // load title screen?
    do {                 // wait until it's loaded?
    } while (func_80034B44());
    func_801D4CC0(); // jump into title screen loop?
}

void func_80024E94(void) {
    func_800211C4(0xA);
    do {
    } while (func_80034B44());
    func_801D1774();
}

void func_80024ECC(void) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D2D74();
}

void func_80024F04(void) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D2E84();
}

void func_80024F3C(s32 arg0) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D2F00(arg0);
}

void func_80024F80(s32 arg0) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D3138(arg0);
}

void func_80024FC4(s32 arg0) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D3018(arg0);
}

void func_80025008(void) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D3228();
}

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025040);

void func_800250B4(void) {
    func_800211C4(0xC);
    do {
    } while (func_80034B44());
    func_801D027C();
}

void func_800250EC(s32 arg0) {
    func_800211C4(0xC);
    do {
    } while (func_80034B44());
    func_801D05C4(arg0);
}

void func_80025130(s32 arg0) {
    func_800211C4(0xC);
    do {
    } while (func_80034B44());
    func_801D0704(arg0);
}

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025174);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025288);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025310);

void func_80025360() { func_8001FA28(0x19F); }

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025380);

s32 func_8002542C(s32 arg0) {
    s32 i;
    for (i = 0; i < MAX_MATERIA_COUNT; i++) {
        if (Savemap.materia[i] == -1) {
            Savemap.materia[i] = arg0;
            if (func_8002603C(arg0 & 0xFF) == 10) {
                Savemap.memory_bank_1[75] |= 1;
            }
            if ((arg0 & 0xFF) == 44) {
                Savemap.memory_bank_1[75] |= 2;
            }
            return -1;
        }
    }
    return arg0;
}

void func_800254D8() { D_80062EBC = 0; }

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800254E4);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025514);

void func_80025648(void) {}

void func_80025650(void) {}

// get party leader (Cloud) level
s32 func_80025658() { return D_8009C738[0].level; }

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025668);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800256DC);

s32* func_80025758(s32 arg0) { return &D_80071E44[arg0 * 9]; }

s32* func_80025774(s32 arg0) { return &D_80071C24[arg0 * 4]; }

Unk8009D84C* func_80025788(s32 arg0) {
    Unk8009D84C* partyMember;

    partyMember = (Unk8009D84C*)0xFF;
    if (Savemap.partyID[arg0] != 0xFF) {
        return &D_8009D84C[arg0];
    }
    return 0xFF;
}

void func_800257C4(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800257CC);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025800);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800258BC);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025988);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025A44);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025B10);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025B48);

s32 SYS_gil(void) { return Savemap.gil; }

void func_80025B8C(u_long* image) {
    RECT rect;
    rect.x = 0x340;
    rect.y = 0x184;
    rect.w = 0x30;
    rect.h = 0x78;
    StoreImage(&rect, image);
}

void func_80025BD0(u_long* image) {
    RECT rect;
    rect.x = 0x340;
    rect.y = 0x184;
    rect.w = 0x30;
    rect.h = 0x78;
    LoadImage(&rect, image);
}

void func_80025C14(u_long* image) {
    RECT rect;
    rect.x = 0x180;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 9;
    StoreImage(&rect, image);
}

void func_80025C54(u_long* image) {
    RECT rect;
    rect.x = 0x180;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 9;
    LoadImage(&rect, image);
}

void func_80025C94(u_long* image) {
    RECT rect;
    rect.x = 0x100;
    rect.y = 0x1ED;
    rect.w = 0x100;
    rect.h = 3;
    LoadImage(&rect, image);
}

void func_80025CD4(u_long* image) {
    RECT rect;
    rect.x = 0x100;
    rect.y = 0x1ED;
    rect.w = 0x100;
    rect.h = 3;
    StoreImage(&rect, image);
}

void func_80025D14(u_long* addr, s32 px, s32 py, s32 cx, s32 cy) {
    TIM_IMAGE tim;
    OpenTIM(addr);
    while (ReadTIM(&tim)) {
        if (tim.caddr) {
            tim.crect->x = cx;
            tim.crect->y = cy;
            LoadImage(tim.crect, tim.caddr);
            DrawSync(0);
        }
        if (tim.paddr) {
            tim.prect->x = px;
            tim.prect->y = py;
            LoadImage(tim.prect, tim.paddr);
            DrawSync(0);
        }
    }
}

// this function seems to be responsible of loading the characters' portrait
void func_80025DF8(void) {
    u8 dummy[8];
    u8 buf[0x1000];
    u_long* dst;
    s32 i;
    s32* sector_off;
    s32* length;
    s32 cx, cy;

    i = 0;
    dst = (u_long*)buf;
    sector_off = &D_80048FE8->sector_off;
    length = &D_80048FE8->length;
    for (; i < 9; i++) {
        func_80033F40(sector_off[i * 2], length[i * 2], dst, 0);
        cx = 0x340 + (i / 5) * 0x18;
        cy = 0x100 + (i % 5) * 0x30;
        func_80025D14(dst, cx, cy, 0x180, i);
        DrawSync(0);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80025ED4);

void func_80026034(void) {}

u8 func_8002603C(u8 arg0) {
    return D_80049520[D_80049528[D_800730DC[arg0][1] & 0xF]];
}

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026090);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800260DC);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026258);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800262D8);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026408);

void func_80026448(Unk80026448* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4,
                   s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9, s32 arg10,
                   s32 arg11, s32 arg12, u16 arg13) {
    arg0->unkA = arg1;
    arg0->unkB = arg2;
    arg0->unkC = arg3;
    arg0->unkD = arg4;
    arg0->unk0 = arg5;
    arg0->unk2 = arg6;
    arg0->unk4 = arg7;
    arg0->unk6 = arg8;
    arg0->unkE = arg9;
    arg0->unkF = arg10;
    arg0->unk10 = arg11;
    arg0->unk11 = arg12;
    arg0->unk8 = arg13;
}

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800264A8);

void func_800269C0(void* arg0) { D_80062F24.poly = arg0; }

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800269D0);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800269E8);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026A00);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026A0C);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026A20);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026A34);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026A94);

void func_80026B5C(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026B64);

// strlen but for FF7 strings
// FF7 string is 0x00: ' ', 0x10: '0', 0x21: 'A', 0xFF: terminator
INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026B70);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026C5C);

// print FF7 string
INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80026F44);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_8002708C);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80027354);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80027408);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80027990);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80027B84);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80028030);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80028484);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_800285AC);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80028930);

void func_80028CA0(s16, s16, s16, s16, s16, s16, s16, s16);
INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80028CA0);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80028E00);

INCLUDE_ASM("asm/us/main/nonmatchings/1255C", func_80029114);
