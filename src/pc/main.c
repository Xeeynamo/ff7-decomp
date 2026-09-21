#include <game.h>
#include <psyz/dbgserver.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

static void ReserveAddrRam(uintptr_t addr, size_t size) {
    void* p = mmap((void*)addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (p == MAP_FAILED) {
        ERRORF("failed to reserve 0x%08X with len 0x%x", addr, size);
        exit(1);
    }
}

static void ReservePsxRam(void) {
    ReserveAddrRam(0x80010000, 0x1F0000);
    ReserveAddrRam(0x1F800000, 0x400);
}

unsigned char g_FileBufCmp[0x80000];
unsigned char g_FileBufDec[0x80000];

static void LoadFile(const char* filename, void* dst, size_t len) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        ERRORF("failed to load '%s'", filename);
        exit(1);
    }
    fread(dst, 1, len, f);
    fclose(f);
}

void PC_LoadStartup(void) { LoadFile("disks/us/STARTUP/SCEAP.LZS", (void*)0x80180000, 0x80000); }

void PC_FetchDiskNo(unsigned char* dst) {
    const char DiskStr[] = "DISK0000";
    memcpy(dst, DiskStr, sizeof(DiskStr));
    dst[7] = '1';
}

extern u8* D_800707C0;
extern s8 D_80062DFC;

typedef struct {
    s32 tableBase;
    s32 rowBase;
    s32 vAdd;
    s32 clutRow;
} GlyphMode;

extern u8 D_80063048[];
void func_80014804(void) {
    SysGzipSetDataBlock((u8*)0x801B0000);
    for (;;) {
        u16 kind = SysGzipGetType();
        if (kind == 0xFFFF) {
            break;
        }
        if (kind == 0) {
            SysGzipPackDecompressNextBlock((u8*)0x801C0000);
            SysLoadDrawSync();
            SysLoadUncompressImg((u8*)0x801C0000);
        } else if (kind == 1) {
            SysGzipPackDecompressNextBlock(D_80063048);
        }
    }
}

extern s32 D_80062F88;
extern u8 D_80063048[];

void func_800148A0(void) {
    D_80062F88 = 0;
    g_BattleMode = 0;
}

extern s32 D_80062F90;
void InputInit(void);
void func_80026258(void) {
    s32 i;

    D_80062F90 = 0;
    InputInit();
    D_8009D7BC.unk0 = 0x80;
    D_8009D7BC.unk1 = 0x80;
    D_8009D7BC.unk14 = 0x80;
    D_8009D7BC.unk2 = 0x41;
    for (i = 0; i < 16; i++) {
        D_8009D7BC.unk4[i] = i;
    }
}

void SysLoadUncompressImg(u8* tim) {
    if (*(u32*)(tim + 4) & 8) {
        LoadImage((RECT*)(tim + 0xC), (u_long*)(tim + 0x14));
        tim += (*(u32*)(tim + 8) >> 2) << 2;
    }
    LoadImage((RECT*)(tim + 0xC), (u_long*)(tim + 0x14));
}

extern void GameMain(void);

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    Psyz_DebugServer(8081);
    ReservePsxRam();
    GameMain();
    return 0;
}
