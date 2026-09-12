//! PSYQ=3.3
#include <game.h>
#include <libcd.h>
#include <libetc.h>

typedef enum {
    CDOP_0,
    CDOP_1,
    CDOP_3 = 3,
    CDOP_11 = 11,
    CDOP_19 = 0x13,
    CDOP_20 = 0x14,
} CdOp;

extern void (*D_8004A634[21])(void);
extern int D_800698E8;        // sector_no
extern u8 D_800698F0[0x4800]; // disc buffer
extern int D_8006E0F0;
extern int D_8006E0F4;
extern CdOp D_80071A60;      // some kind of operation?
extern int D_80071A64;       //
extern CdlLOC D_80071A68;    // cd sector
extern size_t D_80071A6C;    // amount of sectors to read
extern u_long* D_80071A80;   // read content destination
extern void (*D_80071A84)(); // callback
void func_80034CAC(u32 arg0);
static s32 ReadDiskNo(void);

void SysCdromInit(void) {
    while (!CdInit()) {
    }
    D_80071A60 = CDOP_0;
    func_8003DDA4(0);
    func_80034F3C();
    CdControlB(CdlSetmode, (u8*)CdlModeSpeed, NULL);
    VSync(3);
    D_80071A64 = ReadDiskNo();
    SysMovieLoadMovieSettings();
}

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80033BE0);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80033C20);

void SysCdromSetChainParam(int op, int sector, size_t len, u_long* dst, void (*cb)());
INCLUDE_ASM("asm/us/main/nonmatchings/33B70", SysCdromSetChainParam);

int func_80033DAC(int sector_no, void (*cb)()) {
    SysCdromSetChainParam(CDOP_1, sector_no, 0, NULL, cb);
    return 0;
}

int func_80033DE4(int sector_no) {
    SysCdromSetChainParam(CDOP_0, sector_no, 0, NULL, NULL);
    do {

    } while (CdControl(CdlSetloc, (u_char*)&D_80071A68, NULL) == 0);
    return 0;
}

int SystemLoadFileBySector(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    SysCdromSetChainParam(CDOP_3, sector_no, size, dst, cb);
    return 0;
}

int SysCdromStartLoadLzs(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    SysCdromSetChainParam(CDOP_11, sector_no, size, dst, cb);
    D_800698E8 = sector_no;
    SysCdromSetLzsExtract(D_800698F0, dst);
    return 0;
}

int func_80033EDC(int sector_no, void (*cb)()) {
    while (func_80033DAC(sector_no, cb)) {
    }
    while (SystemCdromReadChain()) {
        VSync(0);
    }
    return 0;
}

int SysCdromLoadFile(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    while (SystemLoadFileBySector(sector_no, size, dst, cb)) {
    }
    while (SystemCdromReadChain()) {
        VSync(0);
    }
    return 0;
}

int SysCdromLoadLzs(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    while (SysCdromStartLoadLzs(sector_no, size, dst, cb)) {
    }
    while (SystemCdromReadChain()) {
        VSync(0);
    }
    return 0;
}

static void func_80034048(void) {
    D_80071A6C = 0;
    D_80071A80 = NULL;
    D_80071A84 = NULL;
    D_80071A60 = CDOP_19;
    SystemCdromReadChain();
}

void SystemCdromAbortLoading(void) {
    switch (D_80071A60) {
    case 0:
    case 7:
        return;
    case 5:
    case 6:
    case 13:
    case 14:
        func_8003DE6C(0);
        func_8003DE84(0);
        break;
    case 8:
    case 9:
    case 10:
        SysMovieAbortPlay();
        return;
    case 1:
    case 2:
    case 3:
    case 4:
    case 11:
    case 12:
    case 15:
    case 16:
    case 17:
    case 18:
        break;
    }
    func_80034048();
}

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80034104);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80034150);

static s32 ReadDiskNo(void) {
    CdlFILE file;
    s32 fd;
    s32 res;

    do {
    } while (SystemCdromReadChain());
    do {
        fd = (s32)CdSearchFile(&file, "\\MINT\\DISKINFO.CNF;1");
        if (fd <= 0) {
            if (fd >= -1) {
                return -1;
            }
        }
        CdControlB(CdlSetloc, &file.pos.minute, NULL);
        func_80041D28(1, D_800698F0, 0x80);
        do {
            res = func_80041E30(1, 0);
        } while (res > 0);
    } while (res != 0);

    // DISK0001, where [7] is '1'
    return D_800698F0[7] - '0';
}

s32 SYS_GetDiskNo(void) { return ReadDiskNo(); }

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80034410);

void func_80034420(void) {}

void func_80034428(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80034430);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80034444);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_8003447C);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_800344C0);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_800345BC);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80034600);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_800346F8);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80034754);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_800347B4);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_800347F8);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_800348F4);

INCLUDE_ASM("asm/us/main/nonmatchings/33B70", func_80034974);

static void func_80034A58(void) {
    CdControlF(CdlPause, NULL);
    D_80071A60 = CDOP_20;
    D_8006E0F4 = 0;
}

static void func_80034A90(void) {
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

u32 SystemCdromReadChain(void) {
    u32* op;
    if (D_80071A60 >= LEN(D_8004A634)) {
        while (1) {
        }
    }
    op = &D_80071A60;
    D_8004A634[*op]();
    return *op;
}

// Haruhiko Okumura's PD implementation modified to work on byte streams.
// Original macros:
#define N 4096      // Size of ring buffer
#define F 18        // Upper limit for match_length
#define THRESHOLD 2 // Encode string into position and length if match_length is greater than this

void SystemLzsDecompress(u8* src, u8* dst) {
    s32 flags, flagCount, i, j;
    u8 *copy, *copyEnd, *dstStart, *srcEnd;

    flagCount = 0;
    flags = 0;
    dstStart = dst;
    srcEnd = src + *(u32*)src + 4;
    src += 4;
    for (;;) {
        if (!flagCount) {
            flagCount = 8;
            if (src >= srcEnd) {
                return;
            }
            flags = *src++;
        }
        if (flags & 1) {
            if (src >= srcEnd) {
                return;
            }
            *dst++ = *src++;
        } else {
            if (src >= srcEnd) {
                return;
            }
            i = *src++;
            j = *src++;
            i |= (j & 0xF0) << 4;
            copyEnd = dst + (j & 0x0F) + THRESHOLD + 1;
            copy = &dst[-((dst - dstStart - (i - (N - F))) & (N - 1))];
            for (; copy < dstStart; copy++) {
                *dst++ = 0;
            }
            for (; dst < copyEnd; copy++) {
                *dst++ = *copy;
            }
        }
        flags >>= 1;
        flagCount--;
    }
}

#undef N
#undef F
#undef THRESHOLD

void func_80034CAC(u32 arg0) {
    *D_8009A000 = 48;
    *D_8009A004 = arg0;
    *D_8009A008 = arg0;
    SystemAkaoExecute();
    VSync(60);
}
