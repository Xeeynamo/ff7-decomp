#include "common.h"
#include "psxsdk/libgte.h"

typedef struct BarrierData {
    short unk0;
    short unk2;
    short unk4;
    short unk6;
    SVECTOR unk8;
    SVECTOR unk10;
    char pad1[0x8];
} BarrierData;

extern short D_8015169C;
extern short D_80162080;

extern BarrierData D_80162978[];

extern int D_801B0CC0;
extern char D_801B0CC4[];

extern void* D_801D0CC4;

// battle.c
int func_800BBEAC(void (*func)(void));

// battle2.c
void func_800D3994(int, char, void*);
void func_800D5444(int, int, int, void (*func)(int));
void func_800D55F4(int, int, int);
int func_800D574C(int);

// barrier.c forward declarations
void func_801B0AF0(int arg0, int arg1);

void func_801B0000(int arg0, int arg1) { func_801B0AF0(arg0, arg1); }

INCLUDE_ASM("asm/us/magic/nonmatchings/barrier", func_801B0020);

INCLUDE_ASM("asm/us/magic/nonmatchings/barrier", func_801B0220);

INCLUDE_ASM("asm/us/magic/nonmatchings/barrier", func_801B04F4);
void func_801B04F4(void);

INCLUDE_ASM("asm/us/magic/nonmatchings/barrier", func_801B092C);
void func_801B092C(int arg0);

void func_801B0A90(void) {
    BarrierData* data = &D_80162978[D_8015169C];

    D_801D0CC4 = &D_801B0CC4[data->unk2 * 65536];
    data->unk2 ^= 1;

    if (D_80162080 < 2) {
        data->unk0 = -1;
    }
}

void func_801B0AF0(int arg0, int arg1) {
    D_801B0CC0 = 0x3000;
    func_800BBEAC(func_801B0A90);
    func_800D5444(arg0, arg1, 4, func_801B092C);
    func_800D55F4(32, func_800D574C(arg0), 94);
}
