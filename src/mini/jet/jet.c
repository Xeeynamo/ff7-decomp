//! PSYQ=3.3

// "PC:" comments source https://github.com/ergonomy-joe/ff7-coaster

#include "types.h"
#include <game.h>

// Nine write cursors, each reset to the start of its own buffer below.
typedef struct {
    /* 0x0000 */ void* unk0;
    /* 0x0004 */ void* unk4;
    /* 0x0008 */ void* unk8;
    /* 0x000C */ void* unkC;
    /* 0x0010 */ void* unk10;
    /* 0x0014 */ void* unk14;
    /* 0x0018 */ void* unk18;
    /* 0x001C */ void* unk1C;
    /* 0x0020 */ void* unk20;
    /* 0x0024 */ POLY_F3 unk24[1];
    /* 0x0038 */ POLY_F4 unk38[1];
    /* 0x0050 */ POLY_G3 unk50[0x640];
    /* 0xAF50 */ POLY_G4 unkAF50[0x1E];
    /* 0xB388 */ POLY_FT3 unkB388[1];
    /* 0xB3A8 */ POLY_FT4 unkB3A8[0x12C];
    /* 0xE288 */ POLY_GT3 unkE288[1];
    /* 0xE2B0 */ POLY_GT4 unkE2B0[1];
    /* 0xE2E4 */ LINE_F2 unkE2E4[1];
} Unk800A7FAC; // size: 0xE2F4

// Offsets 0x00 and 0x5C are fixed by SetDefDrawEnv/SetDefDispEnv; both
// tables are sized by their ClearOTagR calls. PC: Class_coaster_D8
typedef struct {
    /* 0x0000 */ DRAWENV draw;
    /* 0x005C */ DISPENV disp;
    /* 0x0070 */ u_long unk70[0x1000];
    /* 0x4070 */ u_long unk4070[10];
    /* 0x4098 */ u_long unk4098[0xB4];
    /* 0x4368 */ Unk800A7FAC unk4368;
} Unk800D1964; // size: 0x1265C

// PC: t_coaster_ModelInfo
typedef struct {
    /* 0x00 */ s16 unk0;     // PC: wNumTri
    /* 0x02 */ s16 unk2;     // PC: wNumQua
    /* 0x04 */ SVECTOR unk4; // PC: f_04
    /* 0x0C */ SVECTOR unkC; // PC: f_0c
} Unk800A89D8;               // size: 0x14

// PC: t_coaster_Model
typedef struct {
    /* 0x00 */ s16 unk0; // PC: wNumPoly
    /* 0x02 */ s16 unk2;
    /* 0x04 */ s16 unk4; // PC: wNumTri
    /* 0x06 */ s16 unk6; // PC: wNumQua
    /* 0x08 */ s16 unk8;
    /* 0x0A */ char padA[2];
    /* 0x0C */ s32* unkC;  // PC: pTriangles
    /* 0x10 */ s32* unk10; // PC: pQuads
    /* 0x14 */ s16 unk14;  // PC: f_14
    /* 0x16 */ s16 unk16;  // PC: f_16
    /* 0x18 */ s16 unk18;  // PC: f_18
    /* 0x1A */ s16 unk1A;  // PC: f_1a
    /* 0x1C */ char pad1C[4];
} Unk800D0554; // size: 0x20

// Doubly linked list node, chained by func_800A8010 with a 0x38 stride.
// PC: t_coaster_Node
typedef struct Unk800EE1D4 {
    /* 0x00 */ Unk800D0554* unk0;         // PC: pModel
    /* 0x04 */ MATRIX m;                  // PC: sMatrixWorld
    /* 0x24 */ struct Unk800EE1D4* unk24; // PC: pParentNode
    /* 0x28 */ s16 unk28;                 // PC: wModelId
    /* 0x2A */ s16 unk2A;                 // PC: wIndex
    /* 0x2C */ u16 unk2C;                 // PC: wDepth
    /* 0x2E */ s16 unk2E;
    /* 0x30 */ struct Unk800EE1D4* unk30; // PC: pPrev
    /* 0x34 */ struct Unk800EE1D4* unk34; // PC: pNext
} Unk800EE1D4;                            // size: 0x38

// PC: t_coaster_Quad
typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ char pad4[0x24];
} Unk800D1968; // size: 0x28

// PC: t_coaster_Triangle
typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ char pad4[0x20];
} Unk800A8CCC; // size: 0x24

extern s32 D_800A892C;               // PC: sLNormal.vx
extern s32 D_800A8930;               // PC: sLNormal.vy
extern s32 D_800A8934;               // PC: sLNormal.vz
extern s32 D_800A893C;               // PC: sRNormal.vx
extern s32 D_800A8940;               // PC: sRNormal.vy
extern s32 D_800A8944;               // PC: sRNormal.vz
extern s32 D_800A8950;               // PC: dwLHelper
extern s32 D_800A8968;               // PC: dwRHelper
extern s32 D_800A8A5C;               // PC: dwLDistance
extern s32 D_800A8A64;               // PC: dwRDistance
extern s32 D_800AB890;               // PC: dwLNormalLength
extern s32 D_800D0550;               // PC: dwRNormalLength
extern Unk800A89D8* D_800A89D8;      // PC: D_00C5D0E4 (model info stream)
extern s32 D_800A8A70;               // PC: D_00C5D0E0 (read triangles index)
extern u32 D_800A8A8C;               // PC: D_00C5D0EC (allocated models)
extern Unk800EE1D4 D_800A8A90[10];   // PC: D_00C60320 (list heads per depth)
extern Unk800A8CCC* D_800A8CCC;      // PC: D_00C5D0E8 (triangles stream)
extern Unk800EE1D4 D_800A8CD0[0xC8]; // PC: D_00C5D590 (node pool)
extern Unk800D1964 D_800AB898[2];    // PC: Class_coaster_D8
extern Unk800D0554 D_800D0554[];     // PC: D_00C5BF60 (model pool)
extern Unk800EE1D4 D_800D16E4;       // PC: D_00C60150 (top node)
extern s32 D_800D171C;               // PC: D_00C5D320 (read quads index)
extern Unk800D0554* D_800D1730[];    // PC: D_00C5D0F0 (model pointer table)
extern Unk800D1964* D_800D1964[1];   // PC: D_00C3F888 (renderer)
extern Unk800D1968* D_800D1968;      // PC: D_00C5BF58 (quads stream)
extern s16 D_800D1A40[0xC8];         // PC: D_00C60190 (node index pool)
extern Unk800A89D8* D_800D1BD8;      // PC: xbin stream 1 (model info)
extern Unk800A8CCC* D_800D1BFC;      // PC: xbin stream 0xA (triangles)
extern Unk800D1968* D_800D1C14;      // PC: xbin stream 0x10 (quads)
extern s16 D_800D9944;               // PC: D_00C60188 (next node index)
extern Unk800EE1D4 D_800EE1D4[10];   // PC: D_00C5D360 (list tails per depth)

Unk800D0554* func_800A7BF4(void);
s32* func_800A7C20(s32 count);
s32* func_800A7C54(s32 count);
void func_800A7E70(Unk800A7FAC* arg0);
void func_800A7FAC(Unk800A7FAC* arg0);
void func_800A80A8(Unk800EE1D4* arg0, s16 arg1);
s16 func_800A8238(void);
void func_800A8264(s16 arg0);
void func_800A8290(Unk800EE1D4* arg0, Unk800EE1D4* arg1);
void func_800A82F0(Unk800EE1D4* arg0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", MINI_Jet);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A0874);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A0D78);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1198);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A12EC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A13AC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1450);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A16A4);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1A64);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1B64);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1CD8);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1F18);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2058);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2214);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2420);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2518);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A27F0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2860);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2938);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A29AC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2AA0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2B78);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2BE0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2C50);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2DE4);

void func_800A2E30(void) {}

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2E38);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A334C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3414);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A35DC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A372C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A385C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A38D4);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3980);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3A20);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3AAC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3B58);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3C04);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3D50);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3E58);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A40F4);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A4390);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A4400);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A442C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A4458);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A45C0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A4650);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A46E8);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A6B08);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A6BD8);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A70D4);

// PC: C_005EECB5, is a point inside both frustum planes
s32 func_800A7414(VECTOR* arg0) {
    s32 hsLeft;
    s32 rightOk;
    s32 leftOk;
    s32 hsRight;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;

    leftOk = 0;
    rightOk = 0;
    lx = D_800A892C;
    ly = D_800A8930;
    lz = D_800A8934;
    hsLeft = (lx * (arg0->vx >> 2)) + (ly * (arg0->vy >> 2)) + (lz * (arg0->vz >> 2)) + D_800A8A5C;
    rx = D_800A893C;
    ry = D_800A8940;
    rz = D_800A8944;
    hsRight = (rx * (arg0->vx >> 2)) + (ry * (arg0->vy >> 2)) + (rz * (arg0->vz >> 2)) + D_800A8A64;
    if (hsLeft > 0 && D_800A8950 > 0) {
        leftOk = 1;
    }
    if (hsLeft < 0 && D_800A8950 < 0) {
        leftOk = 1;
    }
    if (hsRight > 0 && D_800A8968 > 0) {
        rightOk = 1;
    }
    if (hsRight < 0 && D_800A8968 < 0) {
        rightOk = 1;
    }
    return leftOk & rightOk;
}

// PC: __005EEDAE, is a point inside both frustum planes
s32 func_800A7544(SVECTOR* arg0) {
    s32 hsLeft;
    s32 rightOk;
    s32 leftOk;
    s32 hsRight;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;

    leftOk = 0;
    rightOk = 0;
    lx = D_800A892C;
    ly = D_800A8930;
    lz = D_800A8934;
    hsLeft = (lx * (arg0->vx >> 2)) + (ly * (arg0->vy >> 2)) + (lz * (arg0->vz >> 2)) + D_800A8A5C;
    rx = D_800A893C;
    ry = D_800A8940;
    rz = D_800A8944;
    hsRight = (rx * (arg0->vx >> 2)) + (ry * (arg0->vy >> 2)) + (rz * (arg0->vz >> 2)) + D_800A8A64;
    if (hsLeft > 0 && D_800A8950 > 0) {
        leftOk = 1;
    }
    if (hsLeft < 0 && D_800A8950 < 0) {
        leftOk = 1;
    }
    if (hsRight > 0 && D_800A8968 > 0) {
        rightOk = 1;
    }
    if (hsRight < 0 && D_800A8968 < 0) {
        rightOk = 1;
    }
    return leftOk & rightOk;
}

// PC: __005EEEAD, half-space test against the left frustum plane
s32 func_800A7688(s32 arg0, s32 arg1, s32 arg2) {
    s32 a;
    s32 b;
    s32 c;

    a = D_800A892C;
    b = D_800A8930;
    c = D_800A8934;

    return (a * (arg0 >> 2)) + (b * (arg1 >> 2)) + (c * (arg2 >> 2)) + D_800A8A5C;
}

// PC: __005EEEEA, half-space test against the right frustum plane
s32 func_800A76DC(s32 arg0, s32 arg1, s32 arg2) {
    s32 a;
    s32 b;
    s32 c;
    a = D_800A893C;
    b = D_800A8940;
    c = D_800A8944;

    return (a * (arg0 >> 2)) + (b * (arg1 >> 2)) + (c * (arg2 >> 2)) + D_800A8A64;
}

// PC: __005EEF27, is a sphere inside both frustum planes
s32 func_800A7730(VECTOR* arg0, s16 arg1) {
    s32 leftOk;
    s32 hsLeft;
    s32 rightOk;
    s32 hsRight;
    s32 planeDistance;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;
    s32 len;

    leftOk = 0;
    rightOk = 0;
    lx = D_800A892C;
    ly = D_800A8930;
    lz = D_800A8934;
    hsLeft = (lx * (arg0->vx >> 2)) + (ly * (arg0->vy >> 2)) + (lz * (arg0->vz >> 2)) + D_800A8A5C;
    if (D_800A8950 > 0 && hsLeft >= 0) {
        leftOk = 1;
    }
    if (D_800A8950 < 0 && hsLeft <= 0) {
        leftOk = 1;
    }
    if (leftOk == 0) {
        len = D_800AB890;
        planeDistance = ((hsLeft < 0) ? -hsLeft : hsLeft) / len;
        if (planeDistance < arg1) {
            leftOk = 1;
        }
    }
    rx = D_800A893C;
    ry = D_800A8940;
    rz = D_800A8944;
    hsRight = (rx * (arg0->vx >> 2)) + (ry * (arg0->vy >> 2)) + (rz * (arg0->vz >> 2)) + D_800A8A64;
    if (D_800A8968 > 0 && hsRight >= 0) {
        rightOk = 1;
    }
    if (D_800A8968 < 0 && hsRight <= 0) {
        rightOk = 1;
    }
    if (rightOk == 0) {
        len = D_800D0550;
        planeDistance = ((hsRight < 0) ? -hsRight : hsRight) / len;
        if (planeDistance < arg1) {
            rightOk = 1;
        }
    }
    return leftOk & rightOk;
}

// PC: __005EF071, sphere test against the left frustum plane
s32 func_800A7928(s32 arg0, s32 arg1, s32 arg2, s16 arg3) {
    s32 a;
    s32 b;
    s32 c;
    s32 hs;
    s32 ok;
    s32 len;

    a = D_800A892C;
    b = D_800A8930;
    c = D_800A8934;
    ok = 0;
    hs = (a * (arg0 >> 2)) + (b * (arg1 >> 2)) + (c * (arg2 >> 2)) + D_800A8A5C;
    if (D_800A8950 > 0 && hs >= 0) {
        ok = 1;
    }
    if (D_800A8950 < 0 && hs <= 0) {
        ok = 1;
    }
    if (ok == 0) {
        len = D_800AB890;
        if (hs < 0) {
            hs = -hs;
        }
        if (hs / len < arg3) {
            ok = 1;
        }
    }
    return ok;
}

// PC: __005EF114, sphere test against the right frustum plane
s32 func_800A7A10(s32 arg0, s32 arg1, s32 arg2, s16 arg3) {
    s32 a;
    s32 b;
    s32 c;
    s32 hs;
    s32 ok;
    s32 len;

    a = D_800A893C;
    b = D_800A8940;
    c = D_800A8944;
    ok = 0;
    hs = (a * (arg0 >> 2)) + (b * (arg1 >> 2)) + (c * (arg2 >> 2)) + D_800A8A64;
    if (D_800A8968 > 0 && hs >= 0) {
        ok = 1;
    }
    if (D_800A8968 < 0 && hs <= 0) {
        ok = 1;
    }
    if (ok == 0) {
        len = D_800D0550;
        if (hs < 0) {
            hs = -hs;
        }
        if (hs / len < arg3) {
            ok = 1;
        }
    }
    return ok;
}

// PC: C_005EE7F0, model module init
void func_800A7AF8(void) {
    D_800A8A70 = 0;
    D_800D171C = 0;
    D_800A8A8C = 0;
    D_800A8CCC = D_800D1BFC;
    D_800D1968 = D_800D1C14;
    D_800A89D8 = D_800D1BD8;
}

// PC: C_005EE8CF, build a model from its info entry
Unk800D0554* func_800A7B48(s32 arg0) {
    Unk800D0554* model;
    s32 numTri;
    s32 numQua;

    model = func_800A7BF4();
    numTri = D_800A89D8[arg0].unk0;
    numQua = D_800A89D8[arg0].unk2;
    model->unk16 = D_800A89D8[arg0].unk4.vx;
    model->unk14 = D_800A89D8[arg0].unkC.vx;
    model->unk1A = D_800A89D8[arg0].unk4.vz;
    model->unk18 = D_800A89D8[arg0].unkC.vz;
    model->unk2 = 0;
    model->unk4 = numTri;
    model->unk6 = numQua;
    model->unk8 = 0;
    model->unk0 = numTri + numQua;
    model->unkC = func_800A7C20(numTri);
    model->unk10 = func_800A7C54(numQua);
    return model;
}

// PC: C_005EE9C2, allocate model
Unk800D0554* func_800A7BF4(void) {
    u32* counter;
    Unk800D0554* base;
    s32 index;

    counter = &D_800A8A8C;
    index = *counter;
    base = D_800D0554;
    *counter = index + 1;
    return &base[index];
}

// PC: C_005EE9EC, read triangles
s32* func_800A7C20(s32 count) {
    s32* cursor;
    Unk800A8CCC* base;
    s32 index;

    cursor = &D_800A8A70;
    index = *cursor;
    *cursor = index + count;
    base = D_800A8CCC;
    return &base[index].unk0;
}

// PC: C_005EEA19, read quads
s32* func_800A7C54(s32 count) {
    s32* cursor;
    Unk800D1968* base;
    s32 index;

    cursor = &D_800D171C;
    index = *cursor;
    *cursor = index + count;
    base = D_800D1968;
    return &base[index].unk0;
}

// No PC counterpart; the port replaced the PSX double buffer with the DirectX driver
void func_800A7C88(void) {
    Unk800A7FAC* temp_s1;
    Unk800D1964* db;
    u_char* isbg;

    SetDefDrawEnv(&D_800AB898[0].draw, 0, 0, 0x140, 0xF0);
    SetDefDispEnv(&D_800AB898[0].disp, 0, 0xF0, 0x140, 0xF0);
    SetDefDrawEnv(&D_800AB898[1].draw, 0, 0xF0, 0x140, 0xF0);
    SetDefDispEnv(&D_800AB898[1].disp, 0, 0, 0x140, 0xF0);
    db = D_800AB898;
    D_800AB898[0].draw.isbg = 0;
    // Stored off the buffer base register; a direct field store folds to an absolute address.
    isbg = &db[1].draw.isbg;
    *isbg = 0;
    setRGB0(&D_800AB898[0].draw, 0, 0, 8);
    setRGB0(&D_800AB898[1].draw, 0, 0, 8);
    SetGeomOffset(0xA0, 0xA0);
    SetGeomScreen(0x100);
    SetDispMask(1);
    SetBackColor(0x80, 0x80, 0x80);
    SetFarColor(0, 0, 8);
    temp_s1 = &D_800AB898[0].unk4368;
    func_800A7E70(temp_s1);
    func_800A7E70(&D_800AB898[1].unk4368);
    func_800A7FAC(temp_s1);
    func_800A7FAC(&D_800AB898[1].unk4368);
    ClearOTagR(D_800AB898[0].unk70, LEN(D_800AB898[0].unk70));
    ClearOTagR(D_800AB898[1].unk70, LEN(D_800AB898[1].unk70));
    ClearOTagR(D_800AB898[0].unk4098, LEN(D_800AB898[0].unk4098));
    ClearOTagR(D_800AB898[1].unk4098, LEN(D_800AB898[1].unk4098));
    *D_800D1964 = &D_800AB898[0];
}

void func_800A7E1C(void) {
    ClearOTagR(D_800D1964[0]->unk70, LEN(D_800D1964[0]->unk70));
    ClearOTagR(D_800D1964[0]->unk4098, LEN(D_800D1964[0]->unk4098));
    func_800A7FAC(&D_800D1964[0]->unk4368);
}

// Tag every primitive in the nine pools with its type and length.
void func_800A7E70(Unk800A7FAC* arg0) {
    s32 i;

    for (i = 0; i < LEN(arg0->unk24); i++) {
        SetPolyF3(&arg0->unk24[i]);
    }
    for (i = 0; i < LEN(arg0->unk38); i++) {
        SetPolyF4(&arg0->unk38[i]);
    }
    for (i = 0; i < LEN(arg0->unk50); i++) {
        SetPolyG3(&arg0->unk50[i]);
    }
    for (i = 0; i < LEN(arg0->unkAF50); i++) {
        SetPolyG4(&arg0->unkAF50[i]);
    }
    for (i = 0; i < LEN(arg0->unkB388); i++) {
        SetPolyFT3(&arg0->unkB388[i]);
    }
    for (i = 0; i < LEN(arg0->unkB3A8); i++) {
        SetPolyFT4(&arg0->unkB3A8[i]);
    }
    for (i = 0; i < LEN(arg0->unkE288); i++) {
        SetPolyGT3(&arg0->unkE288[i]);
    }
    for (i = 0; i < LEN(arg0->unkE2B0); i++) {
        SetPolyGT4(&arg0->unkE2B0[i]);
    }
    for (i = 0; i < LEN(arg0->unkE2E4); i++) {
        SetLineF2(&arg0->unkE2E4[i]);
    }
}

void func_800A7FAC(Unk800A7FAC* arg0) {
    arg0->unk0 = arg0->unk24;
    arg0->unk4 = arg0->unk38;
    arg0->unk8 = arg0->unk50;
    arg0->unkC = arg0->unkAF50;
    arg0->unk10 = arg0->unkB388;
    arg0->unk14 = arg0->unkB3A8;
    arg0->unk18 = arg0->unkE288;
    arg0->unk1C = arg0->unkE2B0;
    arg0->unk20 = arg0->unkE2E4;
}

// PC: C_005EF1C0, node module init
void func_800A8010(void) {
    Unk800EE1D4* a;
    Unk800EE1D4* b;
    s32 i;

    func_800A80A8(&D_800D16E4, 0);
    D_800D16E4.unk2C = 0;
    D_800D9944 = 0;
    for (i = 0; i < LEN(D_800D1A40); i++) {
        D_800D1A40[i] = i + 1;
    }
    for (i = 0; i < LEN(D_800A8A90); i++) {
        a = &D_800A8A90[i];
        b = &D_800EE1D4[i];
        a->unk30 = NULL;
        a->unk34 = b;
        b->unk30 = a;
        b->unk34 = NULL;
    }
}

// PC: C_005EF281, init node
void func_800A80A8(Unk800EE1D4* arg0, s16 arg1) {
    arg0->m.m[0][0] = 0x1000;
    arg0->m.m[1][1] = 0x1000;
    arg0->m.m[2][2] = 0x1000;
    arg0->m.t[0] = 0;
    arg0->m.t[1] = 0;
    arg0->m.t[2] = 0;
    arg0->m.m[0][1] = 0;
    arg0->m.m[0][2] = 0;
    arg0->m.m[1][0] = 0;
    arg0->m.m[1][2] = 0;
    arg0->m.m[2][0] = 0;
    arg0->m.m[2][1] = 0;
    arg0->unk24 = &D_800D16E4;
    arg0->unk2A = arg1;
    arg0->unk30 = 0;
    arg0->unk34 = 0;
}

// PC: C_005EF31E, allocate node (modelId, parent, x, y, z, rotation)
Unk800EE1D4* func_800A80F8(s16 arg0, s32 arg1, s32 arg2, s32 arg3, Unk800EE1D4* arg4, s32 arg5, s32 arg6, s32 arg7,
                           s16 arg8, s16 arg9, s16 arg10) {
    SVECTOR sp10;
    Unk800EE1D4* temp_s0;
    Unk800EE1D4* temp_v1;
    s16 temp_v0;

    temp_v0 = func_800A8238();
    temp_v1 = D_800A8CD0;
    temp_s0 = &temp_v1[temp_v0];
    func_800A8290(temp_s0, arg4);
    temp_s0->unk0 = D_800D1730[arg0];
    temp_s0->unk28 = arg0;
    temp_s0->unk2A = temp_v0;
    setVector(&sp10, arg8, arg9, arg10);
    RotMatrix(&sp10, &temp_s0->m);
    temp_s0->m.t[0] = arg5;
    temp_s0->m.t[1] = arg6;
    temp_s0->m.t[2] = arg7;
    return temp_s0;
}

// PC: C_005EF3BF, release node
void func_800A8204(Unk800EE1D4* arg0) {
    func_800A82F0(arg0);
    func_800A8264(arg0->unk2A);
}

// PC: C_005EF3E0, allocate node index
s16 func_800A8238(void) {
    s16* head;
    s16 result;

    head = &D_800D9944;
    result = *head;
    *head = D_800D1A40[result];

    return result;
}

// PC: C_005EF40C, release node index
void func_800A8264(s16 arg0) {
    s16* temp;
    s16* temp2;

    temp2 = &D_800D1A40[arg0];
    temp = &D_800D9944;
    *temp2 = *temp;
    *temp = arg0;
}

// PC: C_005EF42F, insert node at its parent's depth + 1
void func_800A8290(Unk800EE1D4* arg0, Unk800EE1D4* arg1) {
    Unk800EE1D4* temp_v0_2;
    Unk800EE1D4* temp_v1;
    s16 temp_v0;

    arg0->unk24 = arg1;
    temp_v0 = arg1->unk2C + 1;
    arg0->unk2C = temp_v0;
    temp_v1 = &D_800EE1D4[temp_v0];
    temp_v0_2 = temp_v1->unk30;
    arg0->unk30 = temp_v0_2;
    arg0->unk34 = temp_v0_2->unk34;
    temp_v1->unk30->unk34 = arg0;
    temp_v1->unk30 = arg0;
}

// PC: C_005EF49E, unlink node
void func_800A82F0(Unk800EE1D4* arg0) {
    arg0->unk30->unk34 = arg0->unk34;
    arg0->unk34->unk30 = arg0->unk30;
}
