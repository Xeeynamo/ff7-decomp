#include "common.h"
#include "psxsdk/libgpu.h"

extern DISPENV D_800A0728;
extern DRAWENV D_800A06CC;
extern u8 D_800A0739;
extern u8 D_800A06E3;
extern u8 D_800A06E2;
extern u8 D_800A06E4;
extern u16 D_800A06E0;

void func_800A0000(void) {
    register DISPENV* disp_env asm("s1");
    register DRAWENV* draw_env asm("s0");

    ResetGraph(0);
    SetGraphDebug(0);
    SetDispMask(1);

    disp_env = &D_800A0728;
    SetDefDispEnv(disp_env, 0, 0xE8, 0x140, 0xF0);

    draw_env = &D_800A06CC;
    SetDefDrawEnv(draw_env, 0, 0xF0, 0x140, 0xE0);

    D_800A0739 = 0;
    D_800A06E3 = 1;
    D_800A06E2 = 0;
    D_800A06E4 = 0;
    D_800A06E0 = 0;

    VSync(0);
    PutDispEnv(disp_env);
    PutDrawEnv(draw_env);
}

extern volatile u16 D_80095DD4;
extern u16 D_80075DEC;
extern u16 D_800707BE;
extern u16 D_8009C560;

extern void func_800A0000(void);
extern void func_800A01A0(void);
extern void func_80014540(void);

void func_800A00CC(void) {
    s32 one;

    while (D_80095DD4 != 0) {
    }

    one = 1;
    D_80075DEC = one;
    func_800A0000();
    func_800A01A0();

    if (D_800707BE != 0) {
        D_8009C560 = 2;
    } else {
        D_8009C560 = one;
        func_80014540();
    }
}

typedef struct {
    u16 magic[4];
    u16 unk8;
    u16 unkA;
    u16 colors[1];
} BromStruct;

void func_800A0534(BromStruct* arg0);
u16 func_800A05D4(BromStruct* img, s32 arg1, s32 arg2);
extern s32 func_80017108(void* arg0, void* arg1);

void func_800A015C(void) {
    if (func_80017108((void*)0x801B0000, (void*)0x801C0000) > 0) {
        func_800A0534((BromStruct*)0x801C0000);
        func_800A05D4((BromStruct*)0x801C0000, -1, -1);
    }
}

extern u16 D_800707BC;
extern u16 D_800707BE;
extern u32 D_800A473C[2];
extern u8 D_800A06BC[8];
extern u8 D_800A073C[];
extern RECT D_800A06C4;
extern u8 D_800A06B4[8];

extern void func_8001C980(void);
extern u32 func_8001C808(void);
extern void func_80020058(s32 arg0);
extern void func_8001786C(s32 arg0);
extern void func_80017678(void);
extern void func_80014578(s32 arg0, void* arg1, void* arg2);
extern void func_800145BC(s32 arg0);
extern void func_80026A00(u32* arg0);
extern void func_800269C0(u8* arg0);
extern void func_80025174(void);
extern void func_80026090(void);
extern void func_80027354(s32 arg0, s32 arg1, void* arg2, s32 arg3);

INCLUDE_ASM("asm/us/brom/nonmatchings/brom", func_800A01A0);

// Converts 15-bit color between BGR555 and RGB555 by swapping R and B channels.
s32 func_800A0514(s32 arg0) {
    s32 g = arg0 & 0x3E0;
    s32 r = arg0 & 0x1F;
    s32 b = (u32)(arg0 & 0x7C00) >> 10;
    s32 r10 = r << 10;
    s32 bg = b | g;

    return r10 | bg;
}

void func_800A0534(BromStruct* arg0) {
    s32 total_pixels;
    s32 i;
    s32 limit;
    s32 padding[2];
    s32 w, h;

    w = (arg0->unk8 >> 8) | (arg0->unk8 << 8);
    h = (arg0->unkA >> 8) | (arg0->unkA << 8);
    total_pixels = (w & 0xFFFF) * (h & 0xFFFF);
    if (total_pixels > 0) {
        i = 0;
        limit = total_pixels;
        do {
            u16 color = arg0->colors[i];
            arg0->colors[i] =
                func_800A0514(((color >> 8) | (color << 8)) & 0xFFFF);
            i++;
        } while (i < limit);
    }
}

u16 func_800A05D4(BromStruct* img, s32 arg1, s32 arg2) {
    s16 rect[4];
    s32 w, h;
    s32 y;
    u16 temp_v1;

    if ((temp_v1 = img->magic[0]) != 0x4152)
        return temp_v1;
    if ((temp_v1 = img->magic[1]) != 0x2057)
        return temp_v1;
    if ((temp_v1 = img->magic[2]) != 0x4752)
        return temp_v1;
    if ((temp_v1 = img->magic[3]) != 0x2042)
        return temp_v1;

    w = (img->unk8 >> 8) | (img->unk8 << 8);
    h = (img->unkA >> 8) | (img->unkA << 8);
    if (arg1 == -1) {
        arg1 = (0x140 - (w & 0xFFFF)) / 2;
    }
    y = arg2 + 0xE8;
    if (arg2 == -1) {
        arg2 = (0xF0 - (h & 0xFFFF)) / 2;
        y = arg2 + 0xE8;
    }
    rect[2] = w;
    rect[0] = arg1;
    rect[1] = y;
    rect[3] = h;
    LoadImage((RECT*)rect, (u_long*)img->colors);
    return 0;
}
