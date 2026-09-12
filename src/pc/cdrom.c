#include <game.h>
#include <libcd.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_DISK_BIN "disks/Final Fantasy VII (USA) (Disc 1).bin"

enum {
    CDOP_NONE = 0,
    CDOP_SEEK = 1,
    CDOP_READ = 3,
    CDOP_READ_LZS = 11,
};

#define RAW_SECTOR_BYTES 2352 // MODE2/2352 sector on disc
#define USER_DATA_OFFSET 24   // MODE2/Form1 user data starts here
#define USER_DATA_BYTES 2048  // usable bytes per sector

int D_800698E8;           // LZS source sector
u8 D_800698F0[0x4800];    // disc buffer for LZS reads
s32 D_80071A60;           // current chain operation
int D_80071A64;           // disk number
CdlLOC D_80071A68;        // read position
size_t D_80071A6C;        // sectors left to read
u_long* D_80071A80;       // read destination
void (*D_80071A84)(void); // completion callback

static u_long* s_lzs_dst;
static FILE* s_disk;

static FILE* DiskFile(void) {
    if (!s_disk) {
        s_disk = fopen(DEFAULT_DISK_BIN, "rb");
        if (!s_disk) {
            ERRORF("failed to open disk image '%s'", DEFAULT_DISK_BIN);
            exit(1);
        }
    }
    return s_disk;
}

void SysCdromSetLzsExtract(void* src, void* dst) {
    (void)src;
    s_lzs_dst = dst;
}

void SysCdromSetChainParam(int op, int sector, size_t len, u_long* dst, void (*cb)(void)) {
    while (SystemCdromReadChain()) {
    }
    CdIntToPos(sector, &D_80071A68);
    D_80071A6C = (len + 0x7FF) >> 11; // bytes -> sectors, rounded up
    D_80071A80 = dst;
    D_80071A84 = cb;
    D_80071A60 = op;
}

static void ReadSectors(u_long* dst) {
    FILE* f = DiskFile();
    int start = CdPosToInt(&D_80071A68);
    u8* out = (u8*)dst;
    size_t i;
    u8 raw[RAW_SECTOR_BYTES];

    for (i = 0; i < D_80071A6C; i++) {
        long off = (long)(start + i) * RAW_SECTOR_BYTES;
        if (fseek(f, off, SEEK_SET) != 0 || fread(raw, 1, RAW_SECTOR_BYTES, f) != RAW_SECTOR_BYTES) {
            ERRORF("disk read failed at sector %d", start + (int)i);
            return;
        }
        memcpy(out + i * USER_DATA_BYTES, raw + USER_DATA_OFFSET, USER_DATA_BYTES);
    }
}

u32 SystemCdromReadChain(void) {
    switch (D_80071A60) {
    case CDOP_READ:
        ReadSectors(D_80071A80);
        break;
    case CDOP_READ_LZS:
        ReadSectors((u_long*)D_800698F0);
        SystemLzsDecompress((u8*)(s_lzs_dst ? s_lzs_dst : D_80071A80), D_800698F0);
        break;
    case CDOP_SEEK:
    default:
        break;
    }
    if (D_80071A84) {
        void (*cb)(void) = D_80071A84;
        D_80071A84 = NULL;
        cb();
    }
    D_80071A60 = CDOP_NONE;
    return 0;
}
