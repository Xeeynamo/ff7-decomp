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
    s16 D_801621F0;
    s16 D_801621F2;
    s16 D_801621F4;
    s16 D_801621F6;
    s16 unk8[12];
} Unk801621F0; // size:0x20

typedef struct {
    s32 method; // enum QueueMethod
    RECT* rect;
    u_long* ptr;
    s32 x;
    s32 y;
} Unk800F01DC; // size:0x14

extern s32 D_800E7A38;
extern u8 D_800E7A48[0x10];
extern s8 D_800E7A58[];
extern Yamada D_800E8068[];
extern void (*D_800EF9D8[])(s16, u8);
extern void (*D_800EFFE0[])(s16, u8);
extern void (*D_800EFAF0[])(s16, u8);
extern void (*D_800EFBC8[])(s16, u8);
extern void (*D_800EFC28[])(s16, u8);
extern s32 (*D_800EFEA0[])(s16, u8);
extern Unk800F01DC* D_800F01DC;
extern s32 D_800F01E0;
extern s32 D_800F01E4;
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
extern RECT D_800F4B2C[];
extern RECT D_800F4B6C[];
extern Unk800F01DC D_800F4BAC[];
extern u8 D_800F514C[];
extern s8 D_800F5760;
extern Unk800AF470 D_800F5BB8[];
extern u16 D_800F83C6; // part of struct?
extern u16 D_800F83D0;
extern u8 D_800F83E4[];
extern s8 D_800F8CF0;
extern u16 D_800F9DA4;
extern s16 D_800FA9C4;
extern s8 D_800FA9E8;
extern u8 D_800FAFDC;
extern s32 D_800FAFEC;
extern s32 D_800FAFF0;
extern u8 D_801031F0;
extern u8 D_801590CC;
extern s16 D_801590D4;
extern s16 D_80151774;
extern s8 D_801518DC;
extern Unk801621F0 D_801621F0[];
extern s16 D_801621F4[][0x10];
extern u16 D_80163758[]; // part of a struct
extern Unk800A2F4C D_80163798[0x40];
extern s8 D_80163A98;
extern u8 D_80163C7C;

void func_800D088C(s32 loc, s32 len);
void func_800D2980(u_long* addr, s16 imgXY, s16 clutX, s16 clutY);
