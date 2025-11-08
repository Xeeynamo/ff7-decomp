// should be imported only by the BATTLE overlay, not BATINI or similar
#include "battle.h"

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
    /* 0x000 */ s32 D_801518FC;
    /* 0x004 */ s32 D_80151900;
    /* 0x008 */ s16 D_80151904;
    /* 0x00A */ u8 D_80151906;
    /* 0x00B */ u8 D_80151907;
    /* 0x00C */ s32 D_80151908;
    /* 0x010 */ s16 D_8015190C;
    /* 0x012 */ s8 D_8015190E;
    /* 0x013 */ s8 D_8015190F;
    /* 0x014 */ s32 D_80151910;
    /* 0x018 */ s32 D_80151914;
    /* 0x01C */ s32 D_80151918;
    /* 0x020 */ s32 D_8015191C;
    /* 0x024 */ s32 D_80151920;
    /* 0x028 */ s32 D_80151924;
    /* 0x02C */ u8 unk2C[0xB70];
} Unk801518FC; // size:0xB9C

extern s32 D_800E7A38;
extern u8 D_800E7A48[0x10];
extern s8 D_800E7A58[];
extern void (*D_800EF9D8[])(s16, u8);
extern void (*D_800EFFE0[])(s16, u8);
extern s32* D_800F01DC;
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
extern s32 D_800F4BAC[];
extern u8 D_800F514C[];
extern s8 D_800F5760;
extern Unk800AF470 D_800F5BB8[];
extern u16 D_800F7DC2[1]; // part of a struct
extern u16 D_800F7DC4[1]; // 100% part of a struct
extern u16 D_800F83C6;    // part of struct?
extern u16 D_800F83D0;
extern u8 D_800F83E4[];
extern s8 D_800F8CF0;
extern u16 D_800F9DA4;
extern u8 D_800FAFDC;
extern s32 D_800FAFEC;
extern s32 D_800FAFF0;
extern s8 D_801518DC;

extern s16 D_80151774;
extern Unk801518FC D_801518FC[];
extern u8 D_801590CC;    // part of D_801518FC?
extern u16 D_80163758[]; // part of a struct
extern Unk800A2F4C D_80163798[0x40];
extern u8 D_80163C7C;

void func_800D088C(s32 loc, s32 len);
