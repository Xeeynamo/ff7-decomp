//! PSYQ=3.3 CC1=2.6.3
#include <game.h>

const u32 D_800A0000[] = {0, 0x01D801E0};
extern s8 D_800E0628;
extern s8 D_800E0630;
extern u16 D_800E1024;
extern s16 D_800E41B8;
extern s16 D_800E41C0;
extern s16 D_800E41BC;
extern s16 D_800E41C4;
extern u16 D_800E4210;
extern char D_800E4254[];
extern char D_800E4288[];

static void func_800D4848(const char* errmsg);
static void func_800DA368(char* arg0, char* arg1);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A1368);

void func_800A1498(void) {
    if (D_800965E8 == 1) {
        func_8003408C();
    }
    D_80071A5C = 0;
    D_800965E8 = 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A14D8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A16CC);

const u32 D_800A0024[] = {0x00000000, 0x000801E0};
const u32 D_800A002C[] = {0x00E80000, 0x000801E0};
const u32 D_800A0034[] = {0x01D00000, 0x000801E0};
const u32 D_800A003C[] = {0x00000000, 0x00080140};
const u32 D_800A0044[] = {0x00E80000, 0x00080140};
const u32 D_800A004C[] = {0x01D00000, 0x00080140};
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A2314);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A2D5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A2F78);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A3020);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A364C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A4094);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A4134);

static s32 func_800A41CC(SVECTOR* arg0, long* arg1) {
    long sp10;
    long sp14;
    s32 ret;

    PushMatrix();
    SetRotMatrix(D_80071E40);
    SetTransMatrix(D_80071E40);
    SetGeomOffset(0, 0);
    ret = RotTransPers(arg0, arg1, &sp10, &sp14);
    PopMatrix();
    return ret;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A424C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A4430);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A45D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A47F8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A48B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A496C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A4BEC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A5FB4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A635C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A6418);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A65A4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A82A0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8304);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8600);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8620);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8640);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8858);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8968);

static void func_800A8DF4(s32* arg0, s16* arg1, s16* arg2) {
    arg0[0] = arg1[0] - arg2[0];
    arg0[1] = arg1[1] - arg2[1];
    arg0[2] = arg1[2] - arg2[2];
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8E34);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8F88);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A9B64);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A9CE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A9EEC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA180);

static void func_800AA32C(Unk8007E7AC* arg0) {
    s32 i;

    for (i = 0; i < 32; i++) {
        arg0->unk15 = 0;
        arg0++;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA348);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA514);

const u32 D_800A00BC[] = {0x00360000, 0x012A007A};
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA5E4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA870);

const u32 D_800A00DC[] = {0x00000000};
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA930);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AAB24);

void func_800AB2AC(void) {}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB2B4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB310);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB4AC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB5E8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB728);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB9C8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ABA34);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ABA70);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ABF0C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ABFE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AC35C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ACBA0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ACC5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AD7B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AD858);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ADAA4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ADC90);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ADD70);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AE23C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AE4DC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AEE24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AF6EC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AF96C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AFAC4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AFDE4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AFE1C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B0618);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B0A48);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B0EDC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B0FB0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B1C7C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B1E40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B2598);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B2A00);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B2DD4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B2F40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B480C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B4B04);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B4EAC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B5260);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B5504);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B62C4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B69C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B6AE4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B6B4C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B79B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B86D8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B8CF0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B9B0C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BA534);

static void func_800D7F9C(void);
void func_800BA65C(s32 arg0) {
    if (D_8007EBE0) {
        func_800D4BFC();
        func_800BC338();
        func_800D7D6C();
        func_800D7F9C();
        D_80095DCC = 0;
        D_8009FE8C = 0;
        D_8007EBE0 = 0;
        if (D_8009C6DC[1] < 5) {
            SystemError('K', 11);
        }
        if (D_8009C6DC[0] < 2) {
            SystemError('K', 10);
        }
        if (D_8009C6DC[0] > 2 || D_8009C6DC[1] > 5) {
            SystemError('K', 12);
        }
    }
    if (D_80099FFC != 4) {
        if (D_80099FFC != 5 || D_80070788 != 0) {
            func_800BB3A8();
        }
    }
    if (D_80071E2C) {
        func_8001F1BC(&D_80083274, 4, arg0, D_8009C6E0->unk0[0] ^ 1);
    }
    func_800BC438(arg0);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BA7C4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BAF54);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BB1B4);

const u32 D_800A013C[] = {0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BB3A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BBBCC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BBF74);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC338);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC438);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC4D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC9FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BEAD4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BECA4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BEE10);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BF3AC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BF908);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0248);

s32 func_800C0B54(void) {
    if (D_8009D820 & 3) {
        func_800DA444(D_8009A058, D_800E4288);
        func_800DA368(D_800E4288, "???");
        func_800BEAD4(D_800E4288, 8);
        func_800DA214(3, 0x7F, 0, 0);
    } else {
        func_800D4848("Bad Event code!");
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0BE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0C18);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0DE0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0E5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0EDC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0F58);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0FD8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C107C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C13B0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1674);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1714);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C17B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1858);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1AB4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1BF4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1D24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1DE4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1EEC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2000);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C228C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2394);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C24A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2754);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C285C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2970);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2BFC);

void func_800C2CA8(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("keyon", 3);
    }
    if (!((&D_8009C6DC[D_800831FC[D_800722C4]])[2] & 2)) {
        func_800C2E00(D_8009C6E0->unk70);
    } else {
        func_800C2E00(D_8009C6E0->unk80);
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2D54);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2E00);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2F7C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2FFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C307C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C30FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C31E4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C32CC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C33B4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3A20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3C34);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3EA0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3F1C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3FA0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C401C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C40A4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C42B0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4350);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C43C4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C45AC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C46A4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C46D0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4804);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C493C);

void func_800C49EC(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("music", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x10;
    func_800C4BCC();
}

void func_800C4A40(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("musvt", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x14;
    func_800C4BCC();
}

void func_800C4A94(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("musvm", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x15;
    func_800C4BCC();
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4AE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4BCC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4C9C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4CE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4DE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4EE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C506C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C50EC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5194);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C523C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C532C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5414);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C54BC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5564);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C560C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5668);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5740);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5898);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5A2C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5B38);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5CE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5E80);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5FF4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C63CC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C6748);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C684C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C6924);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C6D64);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C6FD8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C728C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7354);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C75F0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7C3C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7CE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7D5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C814C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C81C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C826C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8514);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8588);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8634);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8B98);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8F64);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9080);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C91D8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C955C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9A20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9B88);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9C84);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9D80);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9E7C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9F78);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA074);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA158);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA254);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA394);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA490);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA5D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA77C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA95C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CAA24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CAAEC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CAC98);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CADFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CAF60);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB01C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB0B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB1CC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB28C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB354);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB450);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB4F8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB5C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB660);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB718);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB7C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB858);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB8F4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB98C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBA28);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBAC0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBB5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBBF4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBCA4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBD40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBDFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBE94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBF40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBFDC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC098);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC134);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC1D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC284);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC358);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC404);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC4D8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC558);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC5EC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC670);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC70C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC78C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC824);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC8A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC944);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC9EC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCA68);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCB10);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCBBC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCC3C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCCC8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCD54);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCE94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCFE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD0C4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD16C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD2E8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD3F0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD554);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD5F0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD6B0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD770);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD834);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD91C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDA24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDB0C);

static void func_800CDC14(s16* arg0) {
    arg0[0] = 0;
    arg0[1] = 0;
    arg0[2] = 0;
    arg0[3] = 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDC28);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDD40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDE8C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE054);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE480);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE6F4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE904);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CEB20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CEB94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CEE44);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF028);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF140);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF200);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF2BC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF368);

void func_800CF4CC(void*, void*, void*, void*);
void func_800CF66C(void*, void*);
void func_800CF6C0(void*, void*);
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF4CC);

static void func_800CF5A0(void) {
    s32 sp10[2];
    s32 sp18[2];

    func_800CF4CC(D_8009CBDC, D_8009CBDC + 0x7B5, sp10, sp18);
    func_800CF66C(D_8009CBDC, sp18);
    func_800CF6C0(D_8009CBDC, sp10);
    D_80071E34 = 1;
}

static void func_800CF60C(void) {
    s32 sp10[2];
    s32 sp18[2];

    func_800CF4CC(D_8009D391, D_8009D391 - 0x7B5, sp10, sp18);
    func_800CF66C(D_8009D391, sp18);
    func_800CF6C0(D_8009D391, sp10);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF66C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF6C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF718);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF874);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF9B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFAF0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFB84);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFC1C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFCE4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFE78);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0180);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0518);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0938);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0B4C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1200);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1350);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D152C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1654);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D184C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D195C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1A80);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1B94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1C68);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1D3C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1DB8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1F20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1FDC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2098);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2164);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2794);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D28A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D298C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2A70);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2B60);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2C60);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2E94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2F3C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3004);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3124);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3264);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3330);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D33FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D348C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3548);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D368C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3728);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3840);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3958);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3A70);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3B88);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3C18);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3CA8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3D40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3DCC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3E64);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3F30);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4038);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4160);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4300);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4378);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4420);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D44E8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4780);

static void func_800D4838(void) {}

static void func_800D4840(void) {}

static void func_800D4848(const char* errmsg) {
    func_800D828C(0, 100, 100, 150, 12);
    func_800DA214(0, 0x7F, 0, 0);
    func_800D9F00(0, errmsg);
    D_80095DCC = 1;
    D_80099FFC = 4;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D48C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D493C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4B28);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4BC0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4BFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4C68);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4E24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4E88);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4EB4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D5228);

static void func_800D5750(void) {
    func_800C46A4();
    D_8009A000[0] = 0x30;
    D_8009A004[0] = 1;
    D_8009A008[0] = 0x40;
    func_8002DA7C();
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D579C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D5A60);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D5C9C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D6D44);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D6E0C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D6F6C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D707C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D726C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D775C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D785C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D7970);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D7A58);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D7C98);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D7D6C);

static void func_800DA334(char* dst, char* src);
static void func_800D7F9C(void) {
    func_800D828C(5, 0x6C, 0, 0x6C, 0x52);
    func_800DA334(D_800E4254, "Authr:");
    func_800DA368(D_800E4254, (s8*)(D_8009C6DC + 0x10));
    func_800D9F00(5, &D_800E4254);
    func_800DA334(D_800E4254, "Event:");
    func_800DA368(D_800E4254, (s8*)(D_8009C6DC + 0x18));
    func_800D9F00(5, D_800E4254);
    func_800D9F00(5, "  Go");
    func_800D9F00(5, "  Stop");
    func_800D9F00(5, "  Step");
    func_800DA124(5, 5, "  Actor OFF");
    func_800DA124(5, 6, "  Info  OFF");
    func_800DA2CC(5);
    func_800D828C(4, 0x6C, 0x52, 0x6C, 0x52);
    func_800D9F00(4, &D_800E0628);
    func_800DA2CC(4);
    func_800D828C(3, 0x6C, 0xA4, 0x6C, 0x5C);
    func_800D9F00(3, &D_800E0630);
    func_800DA2CC(3);
    func_800D828C(1, 0, 0, 0x6C, 0xCA);
    func_800D9F00(1, &D_800E0628);
    func_800DA2CC(1);
    D_80099FFC = 3;
    D_8007EBCC = 4;
    D_8007EBDC = 8;
    D_80071E24 = 0;
    D_8009D820 = 0;
    D_80070788 = 0;
    D_80071C08 = 5;
    func_800DA1D4(5, 4);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8194);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D828C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8334);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D83A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8420);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8498);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D84CC);

static void func_800D85C0(void) {
    D_800E41B8 = 0;
    D_800E41C0 = 0;
    D_800E41BC = 0;
    D_800E41C4 = 0;
    D_800E1024 ^= 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D85FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8710);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D9C04);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D9F00);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D9FFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA124);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA194);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA1D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA28C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA2CC);

static void func_800DA310(void) { D_800E4210 = (D_800E4210 + 1) & 3; }

static void func_800DA334(char* dst, char* src) {
    if (*src) {
        do {
            *dst++ = *src++;
        } while (*src != '\0');
    }
    *dst = '\0';
}

static void func_800DA368(char* arg0, char* arg1) {
    if (*arg0 != '\0') {
        while (*++arg0 != '\0') {
        }
    }
    if (*arg1 != '\0') {
        do {
            *arg0++ = *arg1++;
        } while (*arg1 != '\0');
    }
    *arg0 = '\0';
}

static s32 func_800DA3C4(char* arg0) {
    s32 len = 0;

    while (*arg0 != '\0') {
        arg0++;
        len++;
    }
    return len;
}

static void func_800DA3F0(char* dst, char* src, s32 len) {
    s32 i;
    for (i = len - 1; i != -1; i--) {
        *dst = *src;
        src++;
        dst++;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA424);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA444);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA480);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA4FC);
