//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "psxsdk/libgte.h"

typedef struct BarrierData {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    SVECTOR pos;
    SVECTOR rot;
    u16 unk18;
    char pad1[0x6];
} BarrierData;

extern char D_80062D98;

extern MATRIX D_800FA63C;

extern short D_8015169C;
extern int D_801517C0;
extern short D_80162080;

extern BarrierData D_80162978[];

extern SVECTOR D_801B0C90;
extern int D_801B0C98[];
extern short D_801B0CA2;
extern int D_801B0CC0;
extern char D_801B0CC4[];

extern void* D_801D0CC4;

// battle.c
int func_800BBEAC(void (*func)(void));

// battle2.c
void* func_800D29D4(int*, int, int, void*);
void func_800D3994(int, char, void*);
void func_800D5444(int, int, int, void (*func)(int));
void func_800D55F4(int, int, int);
int func_800D574C(int);

// barrier.c forward declarations
void func_801B0AF0(int arg0, int arg1);

void func_801B0000(int arg0, int arg1) { func_801B0AF0(arg0, arg1); }

void func_801B0020(void) {
    MATRIX* matrix = (MATRIX*)0x1F800000;
    VECTOR* scale = (VECTOR*)0x1F800020;
    BarrierData* data = &D_80162978[D_8015169C];
    int temp_a0 = (data->unk2 + data->unk0) - 17;
    int var_s4;
    int var_s3;

    if (temp_a0 < 0) {
        scale->vx = scale->vy = scale->vz = (D_801B0CC0 * 0xC00) >> 12;
        var_s3 = data->unk18;
        var_s4 = 0;
    } else if (temp_a0 >= 8) {
        data->unk0 = -1;
        return;
    } else {
        var_s3 = data->unk18 | 8;
        var_s4 = temp_a0 << 9;
        scale->vx = scale->vy = scale->vz =
            (((temp_a0 * 0x180) + 0xC00) * D_801B0CC0) >> 12;
    }

    SetFarColor(0, 0, 0);
    RotMatrixYXZ(&data->rot, matrix);
    ScaleMatrix(matrix, scale);
    ApplyMatrix(matrix, &D_801B0C90, matrix->t);

    matrix->t[0] += data->pos.vx;
    matrix->t[1] += data->pos.vy;
    matrix->t[2] += data->pos.vz;

    CompMatrix(&D_800FA63C, matrix, matrix);
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);

    D_801B0C98[1] = var_s3 | 0x80;
    D_801B0CA2 = var_s4;
    D_801D0CC4 = func_800D29D4(D_801B0C98, D_801517C0 + 0x70, 12, D_801D0CC4);

    if (D_80062D98 == 0) {
        data->unk2++;
    }
}

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
