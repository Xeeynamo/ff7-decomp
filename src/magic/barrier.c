//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"

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

typedef struct {
    s32* unk0;
    s32 unk4;
    s16 unk8;
    s16 unkA;
    s32 unkC;
} Unk801B0C98;

extern MATRIX D_800FA63C;

extern short D_8015169C;
extern int D_801517C0;
extern short D_80162080;

extern BarrierData D_80162978[];

static s32 D_801B0B68[] = { // this seems to be a header+3D model
    0x00000030, 0xFE0C0000, 0x000001CA, 0x00000000, 0x0000FFD7, 0x0000FE0C,
    0x000002E5, 0xFFEDFE2F, 0x000002C2, 0xFE2AFFF2, 0x000001C2, 0xFFEDFFF2,
    0x00000000, 0x00200000, 0x00000000, 0x00000000, 0x00000006, 0x00280008,
    0x00180010, 0x38FFFFFF, 0x006A6A6A, 0x00C0C0C0, 0x006A6A6A, 0x00200000,
    0x00280008, 0x38C0C0C0, 0x006A6A6A, 0x00FFFFFF, 0x006A6A6A, 0x00180010,
    0x00200000, 0x38C0C0C0, 0x006A6A6A, 0x00C0C0C0, 0x006A6A6A, 0x00100008,
    0x00180028, 0x38FFFFFF, 0x00C0C0C0, 0x006A6A6A, 0x006A6A6A, 0x00080000,
    0x00280020, 0x38C0C0C0, 0x00FFFFFF, 0x006A6A6A, 0x006A6A6A, 0x00000010,
    0x00200018, 0x38C0C0C0, 0x00C0C0C0, 0x006A6A6A, 0x006A6A6A};
static s32 D_801B0C3C[] = { // this seems to be a header+3D model
    0x00000018, 0xFFEDFE2F, 0x000002C2, 0xFE2AFFF2, 0x000001C2, 0xFFEDFFF2,
    0x00000000, 0x00200000, 0x00000000, 0x00000002, 0x00000008, 0x00000010,
    0x303F3F3F, 0x003F3F3F, 0x00D4D4D4, 0x00100008, 0x00000000, 0x303F3F3F,
    0x00D4D4D4, 0x003F3F3F, 0x00000000};
static SVECTOR D_801B0C90 = {0, 0, -500};
static Unk801B0C98 D_801B0C98 = {D_801B0B68, 0, 0, 0, 0x20};
static SVECTOR D_801B0CA8 = {0, 0, -500};
static Unk801B0C98 D_801B0CB0 = {D_801B0C3C, 0, 0, 0, 0x20};
static int D_801B0CC0;
static char D_801B0CC4[0x20000];
static void* D_801D0CC4;

// barrier.c forward declarations
static void func_801B0AF0(int arg0, int arg1);

void func_801B0000(int arg0, int arg1) { func_801B0AF0(arg0, arg1); }

static void func_801B0020(void) {
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
    } else if (temp_a0 > 7) {
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

    D_801B0C98.unk4 = var_s3 | 0x80;
    D_801B0C98.unkA = var_s4;
    D_801D0CC4 =
        func_800D29D4((int*)&D_801B0C98, D_801517C0 + 0x70, 12, D_801D0CC4);

    if (D_80062D98 == 0) {
        data->unk2++;
    }
}

static void func_801B0220(void) {
    MATRIX* matrix1 = (MATRIX*)0x1F800000;
    MATRIX* matrix2 = (MATRIX*)0x1F800020;
    VECTOR* scale1 = (VECTOR*)0x1F800040;
    VECTOR* scale2 = (VECTOR*)0x1F800050;
    BarrierData* data = &D_80162978[D_8015169C];
    int temp_a0 = data->unk2 + data->unk0 - 17;
    int var_s5;
    int var_s6;

    if (temp_a0 < 0) {
        if (data->unk2 < 6) {
            scale1->vx = scale1->vy = scale1->vz =
                (data->unk2 * (D_801B0CC0 << 9)) >> 12;
        } else {
            scale1->vx = scale1->vy = scale1->vz = (D_801B0CC0 * 0xC00) >> 12;
        }

        scale2->vx = scale2->vy = scale2->vz = (D_801B0CC0 * 0xC00) >> 12;

        var_s5 = data->unk18;
        var_s6 = 0;
    } else if (temp_a0 > 7) {
        data->unk0 = -1;
        return;
    } else {
        var_s5 = data->unk18 | 8;
        var_s6 = temp_a0 << 9;
        scale1->vx = scale1->vy = scale1->vz = scale2->vx = scale2->vy =
            scale2->vz = (((temp_a0 * 0x180) + 0xC00) * D_801B0CC0) >> 12;
    }

    SetFarColor(0, 0, 0);
    RotMatrixYXZ(&data->rot, matrix1);
    *matrix2 = *matrix1;
    ScaleMatrix(matrix1, scale1);
    ScaleMatrix(matrix2, scale2);
    ApplyMatrix(matrix2, &D_801B0CA8, matrix1->t);
    matrix1->t[0] += data->pos.vx;
    matrix1->t[1] += data->pos.vy;
    matrix1->t[2] += data->pos.vz;
    CompMatrix(&D_800FA63C, matrix1, matrix1);
    SetRotMatrix(matrix1);
    SetTransMatrix(matrix1);

    D_801B0CB0.unk4 = var_s5 | 0x80;
    D_801B0CB0.unkA = var_s6;
    D_801D0CC4 =
        func_800D29D4((int*)&D_801B0CB0, D_801517C0 + 0x70, 12, D_801D0CC4);

    if (D_80062D98 == 0) {
        data->unk2++;
    }
}

static void func_801B04F4(void) {
    BarrierData* data = &D_80162978[D_8015169C];
    BarrierData* next;

    if (D_80062D98 != 0) {
        return;
    }

    if (data->unk2 == 0) {
        next = &D_80162978[func_800BBEAC(func_801B0020)];
        next->unk0 = data->unk2;
        next->unk18 = 0;
        next->rot = data->rot;
        next->pos = data->pos;
    }

    if (data->unk2 == 2) {
        next = &D_80162978[func_800BBEAC(func_801B0020)];
        next->unk0 = data->unk2;
        next->unk18 = 1;
        next->rot = data->rot;
        next->pos = data->pos;
    }

    if (data->unk2 == 4) {
        next = &D_80162978[func_800BBEAC(func_801B0020)];
        next->unk0 = data->unk2;
        next->unk18 = 3;
        next->rot = data->rot;
        next->pos = data->pos;
    }

    if (data->unk2 == 6) {
        next = &D_80162978[func_800BBEAC(func_801B0020)];
        next->unk0 = data->unk2;
        next->unk18 = 2;
        next->rot = data->rot;
        next->pos = data->pos;
    }

    if (data->unk2 == 1) {
        next = &D_80162978[func_800BBEAC(func_801B0220)];
        next->unk0 = data->unk2;
        next->unk18 = 0;
        next->rot = data->rot;
        next->pos = data->pos;
    }

    if (data->unk2 == 3) {
        next = &D_80162978[func_800BBEAC(func_801B0220)];
        next->unk0 = data->unk2;
        next->unk18 = 1;
        next->rot = data->rot;
        next->pos = data->pos;
    }

    if (data->unk2 == 5) {
        next = &D_80162978[func_800BBEAC(func_801B0220)];
        next->unk0 = data->unk2;
        next->unk18 = 3;
        next->rot = data->rot;
        next->pos = data->pos;
    }

    if (data->unk2 == 7) {
        next = &D_80162978[func_800BBEAC(func_801B0220)];
        next->unk0 = data->unk2;
        next->unk18 = 2;
        next->rot = data->rot;
        next->pos = data->pos;
    }

    if (data->unk2 == 17) {
        func_800D5774(data->unk4);
        data->unk0 = -1;
    }

    data->unk2++;
}

static void func_801B092C(int arg0) {
    BarrierData* data = &D_80162978[func_800BBEAC(func_801B04F4)];

    func_800D3994(arg0, D_801518E4[arg0].D_8015190F, &data->pos);
    data->pos.vx -=
        (rsin(D_801518E4[arg0].unk160.vy) * D_801518E4[arg0].unk12) >> 12;
    data->pos.vz -=
        (rcos(D_801518E4[arg0].unk160.vy) * D_801518E4[arg0].unk12) >> 12;
    data->rot = D_801518E4[arg0].unk160;
    data->unk4 = arg0;
}

static void func_801B0A90(void) {
    BarrierData* data = &D_80162978[D_8015169C];

    D_801D0CC4 = &D_801B0CC4[data->unk2 * 65536];
    data->unk2 ^= 1;

    if (D_80162080 < 2) {
        data->unk0 = -1;
    }
}

static void func_801B0AF0(int arg0, int arg1) {
    D_801B0CC0 = 0x3000;
    func_800BBEAC(func_801B0A90);
    func_800D5444(arg0, arg1, 4, func_801B092C);
    func_800D55F4(32, func_800D574C(arg0), 94);
}
