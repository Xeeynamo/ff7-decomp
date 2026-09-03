//! G=8
#include "main_private.h"
#include "unzip.h"

#define BIN_HDR_SIZE 6
/*
typedef struct {
    u16 zippedSize;
    u16 unzippedSize;
    u16 type;
} BinSectionHeader;
 */

static u8* s_Bin;

void SysGzipSetDataBlock(u8* arg0) { s_Bin = arg0; }

u16 SysGzipGetType(void) {
    u16 zippedSize;
    u16 type;

    zippedSize = s_Bin[0] | (s_Bin[1] << 8);
    type = 0xFFFF;
    if (zippedSize != 0) {
        type = s_Bin[4] | (s_Bin[5] << 8);
    }
    return type;
}

u16 SysGzipGetSize(void) {
    u16 zippedSize;
    u16 size;

    zippedSize = s_Bin[0] | (s_Bin[1] << 8);
    size = 0;
    if (zippedSize != 0) {
        size = s_Bin[2] | (s_Bin[3] << 8);
    }
    return size;
}

s32 SysGzipPackDecompressNextBlock(u8* dst) {
    u16 zippedSize;
    s32 size;

    size = -1;
    if (zippedSize = s_Bin[0] | (s_Bin[1] << 8)) {
        size = Unzip(s_Bin + BIN_HDR_SIZE, dst);
        if (size == (s_Bin[2] | (s_Bin[3] << 8))) {
            s_Bin += zippedSize + BIN_HDR_SIZE;
        }
    }
    return size;
}

// Load kernel section by type.
// https://wiki.ffrtt.ru/index.php/FF7/Kernel/Low_level_libraries#BIN-GZIP_Type_Archives
s32 SysGzipPackDecompressById(u8* src, void* dst, s32 type) {
    u16 zippedSize;
    s32 size;

    size = -1;
    while (zippedSize = (src[0] | (src[1] << 8))) {
        if ((src[4] | (src[5] << 8)) == type) {
            size = Unzip(src + BIN_HDR_SIZE, dst);
            break;
        }
        src += zippedSize + BIN_HDR_SIZE;
    }
    return size;
}

void SysGzipBinDecompress(GzHeader* src, s32* dst) {
    s32 i;
    s32* var_s1;
    u32 len;
    s32 unk4;

    unk4 = src->unk4;
    len = src->len;
    Unzip(src + 1, dst);
    var_s1 = &dst[len >> 2];
    unk4 = (u32)unk4 >> 2;
    for (i = 0; i < unk4; i++) {
        var_s1[i] = 0;
    }
}
