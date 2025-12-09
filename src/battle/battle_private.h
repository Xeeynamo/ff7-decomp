// should be imported only by the BATTLE overlay, not BATINI or similar
#include "battle.h"

enum QueueMethod {
    QUEUE_LOAD_IMAGE,
    QUEUE_STORE_IMAGE,
    QUEUE_MOVE_IMAGE,
    QUEUE_CLEAR_IMAGE,
};

typedef struct {
    u8 unk[0x30];
    s32 unk30;
} Unk800BB67C;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    u8 raw[0x3C];
} Unk800AF470;

typedef struct {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
    s8 unk4;
    s8 unk5;
    s16 unk6;
    s16 unk8;
    s16 unkA;
} Unk800A2F4C; // size: 0x12

typedef struct {
    s16 D_801621AC;
    s16 D_801621AE;
    s16 D_801621B0;
    s16 D_801621B2;
    s16 D_801621B4;
    s16 D_801621B6;
    s16 unk8;
    s16 unkA;
    s32 unkC;
    s32 unk10;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    s16 unk18;
    s16 unk1A;
} Unk801620AC; // size:0x20

typedef struct {
    /* 0x00 */ s16 D_80162978;
    /* 0x02 */ s16 D_8016297A;
    /* 0x04 */ s16 D_8016297C;
    /* 0x06 */ s16 D_8016297E;
    /* 0x08 */ s16 D_80162980;
    /* 0x0A */ s16 D_80162982;
    /* 0x0C */ s16 unk8;
    /* 0x0E */ s16 unkA;
    /* 0x10 */ s16 unkC;
    /* 0x12 */ s16 unkE;
    /* 0x14 */ s16 unk10;
    /* 0x16 */ s16 unk12;
    /* 0x18 */ u8 unk14;
    /* 0x19 */ u8 unk15;
    /* 0x1A */ s16 unk16;
    /* 0x1C */ s16 unk18;
    /* 0x1E */ s16 unk1A;
} Unk80162978; // size:0x20

typedef struct {
    u16 unk0;
    s16 unk2;
} Unk80162200;
typedef union {
    u8* ptr;
    Unk80162200 unk;
} Union80162200;

typedef struct {
    s16 D_801621F0;
    s16 D_801621F2;
    s16 D_801621F4;
    s16 D_801621F6; // player idx? 0, 1 or 2. See func_800D0C80
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    Union80162200 unk10;
    s32 unk14;
    u8 unk18;
    s8 unk19;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
} Unk801621F0; // size:0x20

typedef struct {
    /* 0x00 */ s8 D_801636B8;
    /* 0x01 */ s8 D_801636B9;
    /* 0x02 */ s8 D_801636BA;
    /* 0x03 */ s8 D_801636BB;
    /* 0x04 */ u8 D_801636BC;
    /* 0x05 */ s8 D_801636BD;
    /* 0x06 */ s16 D_801636BE;
    /* 0x08 */ s32 D_801636C0;
    /* 0x0C */ s32 D_801636C4;
} Unk801636B8; // size:0x10

typedef struct {
    s32 method; // enum QueueMethod
    RECT* rect;
    u_long* ptr;
    s32 x;
    s32 y;
} Unk800F01DC; // size:0x14

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ BattleModelSub unk8;
    /* 0x3C */ s32 unkC;
} Unk800FA6D8;

typedef struct {
    short vx, vy, vz;
} ShortVectorXYZ;

typedef struct {
    s32 D_80151200;
    s32 D_80151204;
    s32 D_80151208;
    s16 D_8015120C;
    s16 D_8015120E;
    s32 D_80151210;
    s32 D_80151214;
    s32 D_80151218;
    s32 D_8015121C;
    s32 D_80151220;
    s32 D_80151224;
    s32 D_80151228;
    s16 D_8015122C;
    s16 D_8015122E;
    s32 D_80151230;
    u8 D_80151234;
    s8 D_80151235;
    s16 D_80151236;
    s16 D_80151238;
    s16 D_8015123A;
    s16 D_8015123C;
    s16 D_8015123E;
    s32 D_80151240;
    s32 D_80151244;
    s32 D_80151248;
    s32 D_8015124C;
    s32 D_80151250;
    s32 D_80151254;
    s32 D_80151258;
    s32 D_8015125C;
    s32 D_80151260;
    s32 D_80151264;
    s32 D_80151268;
    s32 D_8015126C;
    s32 D_80151270;
} Unk80151200;

typedef struct {
    /* 0x00 */ MATRIX m;
    /* 0x20 */ SVECTOR sv;
    /* 0x28 */ VECTOR v;
} Unk800BB75C; // size:0x38

extern s32 D_800E7A38;
extern u8 D_800E7A48[0x10];
extern s8 D_800E7A58[];
extern VECTOR D_800E7D10;
extern VECTOR D_800E7D20;
extern Yamada D_800E8068[];
extern s32 D_800EA50C[];
extern short D_800EEB28[9][8];
extern void (*D_800EF9D8[])(s16, u8);
extern void (*D_800EFFE0[])(s16, u8);
extern void (*D_800EFAF0[])(s16, u8);
extern void (*D_800EFBC8[])(s16, u8);
extern void (*D_800EFC28[])(s16, u8);
extern s32 (*D_800EFEA0[])(s16, u8);
extern Unk800F01DC* D_800F01DC;
extern s32 D_800F01E0;
extern s32 D_800F01E4;
extern s32 D_800F199C;
extern s8 D_800F19A4;
extern s32 D_800F311C;
extern s16 D_800F3122; // part of a struct?
extern s8 D_800F3468;
extern unsigned char D_800F384A[];
extern s16 D_800F3896;
extern s32 D_800F3944;
extern s32 D_800F3948;
extern s32 D_800F3950;
extern s32 D_800F3954;
extern s32 D_800F39E0;
extern s32 D_800F39E4;
extern s32 D_800F39EC;
extern u8 D_800F39F0[][6];
extern s8 D_800F3A80[];
extern u16 D_800F4280[];
extern s32 D_800F4AF4;
extern s32 D_800F4AF8;
extern RECT D_800F4B2C[];
extern RECT D_800F4B6C[];
extern Unk800F01DC D_800F4BAC[];
extern u8 D_800F514C[];
extern s8 D_800F5760;
extern Unk800AF470 D_800F5BB8[];
extern s32 D_800F8368;
extern s16 D_800F8370;
extern s32* D_800F839C; // CD offset?
extern u8 D_800F83A6;
extern u16 D_800F83C6; // part of struct?
extern u16 D_800F83D0;
extern u8 D_800F83E4[];
extern s8 D_800F8CF0;
extern u8 D_800F99E8;
extern u16 D_800F9DA4;
extern Unk800BB75C D_800FA63C;
extern s16 D_800FA69C;
extern s16 D_800FA6B8;
extern u8 D_800FA6D4;
extern Unk800FA6D8 D_800FA6D8[];
extern MATRIX D_800FA958;
extern s16 D_800FA9C4;
extern s8 D_800FA9E8;
extern u8 D_800FAFDC;
extern s32 D_800FAFEC;
extern s32 D_800FAFF0;
extern s16 D_801031E8;
extern u8 D_801031F0;
extern Unk80151200 D_80151200[3];
extern u16 D_80151694;
extern s16 D_8015169C;
extern s16 D_801516FC[][4]; // most likely a struct
extern s16 D_80151774;
extern u8 D_801517BC;
extern s16 D_80158D00;
extern u8 D_801518DC;
extern u8 D_801590CC;
extern s16 D_801590D0;
extern s16 D_801590D4;
extern s8 D_801590DC;
extern s8 D_80162094;
extern Unk801620AC D_801620AC[10];
extern Unk801621F0 D_801621F0[60];
extern Unk80162978 D_80162978[100];
extern u8 D_801635FC;
extern s16 D_80163608;
extern Unk801636B8 D_801636B8[5];
extern u16 D_80163758[]; // part of a struct
extern Unk800A2F4C D_80163798[0x40];
extern s8 D_80163A98;
extern s16 D_80163B80;
extern s32 D_80163C74;
extern u8 D_80163C7C;
extern ShortVectorXYZ D_80163C80[];
extern s8 D_80166F58;
extern s8 D_80166F64;
extern u8 D_80166F68;

void func_800A8E84(s32);
s32 func_800B3030(s32);
void func_800B4794(void);
void func_800B5CD4(s32);
void func_800B5D38(s32);
void func_800B6B98(s32, s32);
void func_800B7FB4(void);
void func_800BA40C();
void func_800BA4C8();
void func_800BBA84(u16 arg0, s32 arg1, s32 arg2);
void func_800C5E94(void);
void func_800D088C(s32 loc, s32 len);
void func_800D0C80(u8 arg0);
void func_800D2980(u_long* addr, s16 imgXY, s16 clutX, s16 clutY);
void func_800D3BF0();
void func_800D8A78(s8);
void func_800D9E0C(s32, s32, s32);
void func_800E15D8(void);
