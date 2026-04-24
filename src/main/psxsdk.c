//! PSYQ=3.3
#include <game.h>
#include <psxsdk/types.h>
#include <psxsdk/libcd.h>

typedef enum {
    CDOP_0,
    CDOP_1,
    CDOP_3 = 3,
    CDOP_11 = 11,
    CDOP_19 = 0x13,
    CDOP_20 = 0x14,
} CdOp;

extern void (*D_8004A634[21])(void);
extern int D_800698E8; // sector_no
extern int D_800698F0;
extern int D_8006E0F0;
extern int D_8006E0F4;
extern CdOp D_80071A60;      // some kind of operation?
extern int D_80071A64;       //
extern CdlLOC D_80071A68;    // cd sector
extern size_t D_80071A6C;    // amount of sectors to read
extern u_long* D_80071A80;   // read content destination
extern void (*D_80071A84)(); // callback
void func_80033B70(void) {
    while (!CdInit()) {
    }
    D_80071A60 = CDOP_0;
    func_8003DDA4(0);
    func_80034F3C();
    CdControlB(CdlSetmode, (u8*)0x80, NULL);
    VSync(3);
    D_80071A64 = func_80034350();
    func_80034F5C();
}

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80033BE0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80033C20);

void func_80033CB8(int op, int sector, size_t len, u_long* dst, void (*cb)());
INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80033CB8);

int func_80033DAC(int sector_no, void (*cb)()) {
    func_80033CB8(CDOP_1, sector_no, 0, NULL, cb);
    return 0;
}

int func_80033DE4(int sector_no) {
    func_80033CB8(CDOP_0, sector_no, 0, NULL, NULL);
    do {

    } while (CdControl(CdlSetloc, (u_char*)&D_80071A68, NULL) == 0);
    return 0;
}

int func_80033E34(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    func_80033CB8(CDOP_3, sector_no, size, dst, cb);
    return 0;
}

int DS_read(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    func_80033CB8(CDOP_11, sector_no, size, dst, cb);
    D_800698E8 = sector_no;
    func_80034D2C(&D_800698F0, dst);
    return 0;
}

int func_80033EDC(int sector_no, void (*cb)()) {
    while (func_80033DAC(sector_no, cb)) {
    }
    while (func_80034B44()) {
        VSync(0);
    }
    return 0;
}

int func_80033F40(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    while (func_80033E34(sector_no, size, dst, cb)) {
    }
    while (func_80034B44()) {
        VSync(0);
    }
    return 0;
}

int func_80033FC4(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    while (DS_read(sector_no, size, dst, cb)) {
    }
    while (func_80034B44()) {
        VSync(0);
    }
    return 0;
}

void func_80034048(void) {
    D_80071A6C = 0;
    D_80071A80 = NULL;
    D_80071A84 = NULL;
    D_80071A60 = CDOP_19;
    func_80034B44();
}

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003408C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034104);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034150);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034350);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800343F0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034410);

void func_80034420(void) {}

void func_80034428(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034430);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034444);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003447C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800344C0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800345BC);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034600);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800346F8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034754);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800347B4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800347F8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800348F4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034974);

void func_80034A58(void) {
    CdControlF(CdlPause, NULL);
    D_80071A60 = CDOP_20;
    D_8006E0F4 = 0;
}

void func_80034A90(void) {
    s32 temp_v0;
    s32* var_a1;

    switch (func_8003DE2C(1, 0)) {
    case 2:
        func_80034444();
        return;
    case 5:
        D_80071A60 = CDOP_19;
        return;
    default:
        temp_v0 = VSync(-1);
        var_a1 = &D_8006E0F0;
        if (*var_a1 != temp_v0) {
            *var_a1 = temp_v0;
            D_8006E0F4++;
            if (D_8006E0F4 == 3600) {
                D_80071A60 = CDOP_19;
                func_80034CAC(3);
            }
        }
        return;
    }
}

u32 func_80034B44(void) {
    u32* op;
    if (D_80071A60 >= LEN(D_8004A634)) {
        while (1) {
        }
    }
    op = &D_80071A60;
    D_8004A634[*op]();
    return *op;
}

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034BB0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034CAC);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ChangeClearSIO);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034D18);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034D2C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034D5C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034DB0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034E00);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034F3C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034F5C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80034FC8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80035430);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800354CC);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80035658);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80035744);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80035CF0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80035D64);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80035DC8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80035F14);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80036038);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80036100);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80036190);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80036244);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80036298);

// NOTE: please do not decompile any of these functions.
// Please refer to psyz/decomp for decompiled PSX SDK functions:
// https://github.com/Xeeynamo/psyz/tree/main/decomp

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _SpuInit);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuStart);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_init);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_writeByIO);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_FiDMA);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_r_);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_t);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_write);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_read);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_FsetRXX);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_FsetRXXa);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_FgetRXXa);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_FsetPCR);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_FsetDelayW);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_FsetDelayR);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_FwaitFs);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _SpuDataCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuInitMalloc);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _SpuMallocSeparateTo3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuMallocWithStartAddr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_gcSPU);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetNoiseVoice);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _SpuSetAnyVoice);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetNoiseClock);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuRead);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetReverb);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _SpuIsInAllocateArea);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _SpuIsInAllocateArea_);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetReverbModeParam);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _spu_setReverbAttr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuGetReverbModeParam);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetReverbDepth);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetReverbVoice);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuClearReverbWorkArea);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetIRQ);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetIRQAddr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetIRQCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _SpuCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetKey);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuGetKeyStatus);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80038F04);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetTransferStartAddr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetTransferMode);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetTransferCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetPitchLFOVoice);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetCommonAttr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceVolume);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceVolumeAttr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoicePitch);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceStartAddr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceLoopStartAddr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceDR);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceSL);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceARAttr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceSRAttr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SpuSetVoiceRRAttr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", rsin);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", sin_1);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", rcos);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetFogNearFar);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", csqrt_1);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", csqrt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", InitGeom);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SquareRoot0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", InvSquareRoot);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", VectorNormalS);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", VectorNormal);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", VectorNormalSS);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003A0E8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MatrixNormal);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", gteMIMefunc);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadAverage12);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadAverage0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadAverageShort12);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadAverageShort0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadAverageByte);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadAverageCol);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SquareRoot12);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CompMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MulMatrix0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MulRotMatrix0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MulRotMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetMulMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ApplyMatrixLV);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ApplyRotMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ScaleMatrixL);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PushMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PopMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadRotMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadLightMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadColorMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MulMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MulMatrix2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ApplyMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ApplyMatrixSV);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", TransMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ScaleMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetRotMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetLightMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetColorMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetTransMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetVertex0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetVertex1);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetVertex2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetVertexTri);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetRGBfifo);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetIR123);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetIR0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetSZfifo3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetSZfifo4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetSXSYfifo);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetRii);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetMAC123);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetData32);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDQA);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDQB);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadGeomOffset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadGeomScreen);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetBackColor);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetFarColor);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetGeomOffset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetGeomScreen);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LocalLight);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DpqColor);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", NormalColor);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", NormalColor3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", NormalColorDpq);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", NormalColorDpq3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", NormalColorCol);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", NormalColorCol3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ColorDpq);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ColorCol);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", AverageSZ3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", AverageSZ4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadOTZ);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LightColor);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DpqColorLight);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DpqColor3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", Intpl);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", Square12);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", Square0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", AverageZ3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", AverageZ4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", OuterProduct12);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", OuterProduct0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", Lzc);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadLZC);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotTransSV);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SquareSS12);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SquareSS0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SquareSL12);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SquareSL0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotTransPers);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotTransPers3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotTrans);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotTransPers4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotAverage3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotAverage4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotAverageNclip3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotAverageNclip4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", TransposeMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotMatrix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotMatrixYXZ);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotMatrixZYX);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotMatrixX);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotMatrixY);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RotMatrixZ);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ratan2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _patch_gte);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003CE0C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PadInit);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PadRead);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PadStop);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", VSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", v_wait);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ResetCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", InterruptCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DMACallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", VSyncCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", VSyncCallbacks);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StopCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", RestartCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CheckCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetIntrMask);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetIntrMask);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", startIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", trapIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", setIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", stopIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", restartIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", memclr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", startIntrVSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", trapIntrVSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", setIntrVSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DMA_memclr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", startIntrDMA);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", trapIntrDMA);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", setIntrDMA);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", VSync_memclr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StSetRing);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdInit);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", def_cbsync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", def_cbready);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", def_cbread);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DCD8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DCE8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DCF8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdLastPos);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdReset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DD84);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DDA4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DDBC);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DDF4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DE2C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DE4C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DE6C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003DE84);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdControl);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdControlF);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdControlB);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdMix);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdGetSector);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003E28C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003E2B0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdIntToPos);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdPosToInt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", getintr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_sync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_ready);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003EF30);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_vol);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_flush);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_initvol);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_initintr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_init);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_datasync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_getsector);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_8003FA9C);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", callback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdSearchFile);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _cmp);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_newmedia);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_searchdir);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_cachefile);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", cd_read);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CD_memcpy);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdRead2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StCdInterrupt2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdDiskReady);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CdGetDiskType);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StClearRing);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StUnSetRing);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", data_ready_callback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StGetBackloc);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StSetStream);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StFreeRing);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", init_ring_status);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StGetNext);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StSetMask);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80040CA8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80041620);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80041654);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StRingStatus);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800418D8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80041AFC);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80041CD4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80041D28);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80041E30);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80041EFC);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTReset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTGetEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTPutEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTBufSize);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTin);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTout);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTinSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCToutSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTinCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCToutCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MDEC_reset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MDEC_in);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MDEC_out);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MDEC_in_sync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MDEC_out_sync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", timeout);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800425F8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTvlcSize);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DecDCTvlc);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", InitHeap);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", FlushCache);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _bu_init);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _96_remove);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetMem);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SystemError);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DeliverEvent);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", OpenEvent);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CloseEvent);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", WaitEvent);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", TestEvent);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", EnableEvent);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DisableEvent);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", InitPAD);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StartPAD);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StopPAD);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PAD_init);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PAD_dr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReturnFromException);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ResetEntryInt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", HookEntryInt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", UnDeliverEvent);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", EnterCriticalSection);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ExitCriticalSection);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetSp);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", open);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", read);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", write);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", close);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", format);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", firstfile);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", nextfile);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", delete);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ChangeClearPAD);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ChangeClearRCnt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetRCnt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetRCnt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StartRCnt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StopRCnt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ResetRCnt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", exit);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", puts);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", setjmp);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", strcmp);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", strncmp);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", bcopy);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", memcpy);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", rand);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", srand);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", printf);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", sprintf);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", strlen);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", memchr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", memmove);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadTPage);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadClut);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDefDrawEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDefDispEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetVideoMode);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetVideoMode);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ResetGraph);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetGraphReverse);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetGraphDebug);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetGraphQueue);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetGraphType);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetGraphDebug);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DrawSyncCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDispMask);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DrawSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", checkRECT);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ClearImage);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", LoadImage);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StoreImage);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MoveImage);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ClearOTag);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ClearOTagR);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DrawPrim);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DrawOTag);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PutDrawEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetDrawEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", PutDispEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetDispEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetODE);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetTexWindow);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDrawArea);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDrawOffset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPriority);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDrawMode);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDrawEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_mode);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_cs);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_ce);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_ofs);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_tw);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_dx);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _status);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _otc);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _clr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _dws);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _drs);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _ctl);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _getctl);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _cwb);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _cwc);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _param);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _addque);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _addque2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _exeque);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _reset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _sync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", set_alarm);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_alarm);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _version);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GPU_memset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GPU_cw);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetTPage);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", GetClut);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DumpTPage);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DumpClut);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", NextPrim);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", IsEndPrim);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", AddPrim);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", AddPrims);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", CatPrim);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", TermPrim);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetSemiTrans);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetShadeTex);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPolyF3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPolyFT3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPolyG3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPolyGT3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPolyF4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPolyFT4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPolyG4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetPolyGT4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetSprt8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetSprt16);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetSprt);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetTile1);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetTile8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetTile16);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetTile);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetLineF2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetLineG2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetLineF3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetLineG3);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetLineF4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetLineG4);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetBlockFill);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SetDrawMove);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", MargePrim);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DumpDrawEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", DumpDispEnv);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", OpenTIM);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadTIM);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", OpenTMD);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", ReadTMD);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_tim_addr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", get_tmd_addr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", unpack_packet);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", memset);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800484A8);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80048540);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800485A0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_800487F0);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _card_info);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _card_load);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _card_auto);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _card_clear);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _card_write);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _new_card);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", InitCARD);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StartCARD);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", SsInitHot);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", InitCARD2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StartCARD2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", StopCARD2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _patch_card);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80048BBC);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _patch_card2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", func_80048C58);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk", _ExitCard);
