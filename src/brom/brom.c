#include "common.h"

INCLUDE_ASM("asm/us/brom/nonmatchings/brom", func_800A0000);

INCLUDE_ASM("asm/us/brom/nonmatchings/brom", func_800A00CC);

INCLUDE_ASM("asm/us/brom/nonmatchings/brom", func_800A015C);

INCLUDE_ASM("asm/us/brom/nonmatchings/brom", func_800A01A0);

INCLUDE_ASM("asm/us/brom/nonmatchings/brom", func_800A0514);

s32 func_800A0514(s32 arg0);

typedef struct {
    u16 magic[4];
    u16 unk8;
    u16 unkA;
    u16 colors[1];
} BromStruct;

void func_800A0534(BromStruct* arg0)
{
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
            arg0->colors[i] = func_800A0514(((color >> 8) | (color << 8)) & 0xFFFF);
            i++;
        } while (i < limit);
    }
}

extern void LoadImage(void* rect, void* p);

u16 func_800A05D4(BromStruct* img, s32 arg1, s32 arg2)
{
    s16 rect[4];
    s32 w, h;
    s32 y;
    u16 temp_v1;

    if ((temp_v1 = img->magic[0]) != 0x4152) return temp_v1;
    if ((temp_v1 = img->magic[1]) != 0x2057) return temp_v1;
    if ((temp_v1 = img->magic[2]) != 0x4752) return temp_v1;
    if ((temp_v1 = img->magic[3]) != 0x2042) return temp_v1;

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
    LoadImage(rect, img->colors);
    return 0;
}
