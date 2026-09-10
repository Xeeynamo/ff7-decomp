//! G=0
#include "main_private.h"

// placeholder sectors and sizes, replaced at boot with INIT/YAMADA.BIN
static Yamada D_80048D84[YAMADA_FILE_NUM] = {
    {501, 2048},     // INIT_YAMADA,
    {502, 40960},    // INIT_WINDOW,
    {522, 16384},    // INIT_KERNEL,
    {10001, 2048},   // BATTLE_BROM,
    {10002, 14336},  // BATTLE_TITLE,
    {10009, 90112},  // BATTLE_BATTLE,
    {10053, 4096},   // BATTLE_BATINI,
    {10055, 139264}, // BATTLE_SCENE,
    {10053, 4096},   // BATTLE_BATRES,
    {0, 0},          // BATTLE_CO,
};

// decompression destinations, indexed by SysGzipGetType
static void* D_80048DD4[] = {
    D_800707C4,    (void*)0x800708C4, (void*)0x80082268, (void*)0x8009C738, (void*)0x800722CC,
    g_WeaponTable, g_ArmorTable,      g_AccessoryTable,  g_MateriaData,
};

// obtain file sector from a YamadaFile
s32 func_800144D8(s32 file_no) { return D_80048D84[file_no].loc; }

static void func_800144F0(s32 file_no) { func_80033DAC(file_no, 0); }

static void func_80014510(s32 file_no) { func_800144F0(D_80048D84[file_no].loc); }

// used to load WORLD/WORLD.BIN or FIELD/FIELD.BIN
void func_80014540(void) { SystemLoadFileBySector(D_80071744, D_80095DD8, D_800722C8, NULL); }

void func_80014578(s32 file_no, void* dst, void (*cb)(void)) {
    SystemLoadFileBySector(D_80048D84[file_no].loc, D_80048D84[file_no].len, dst, cb);
}

void func_800145BC(void (*cb)(void)) {
    while (SystemCdromReadChain()) {
        if (cb) {
            cb();
        }
    }
}

static void func_80014608(void) {}

// initialize LBA system
void func_80014610(void) {
    u8 buf[2048];
    SystemLoadFileBySector(LBA_INIT_YAMADA, sizeof(buf), (u_long*)&buf, NULL);
    func_800145BC(0);
    SysMemCopy32(D_80048D84, &buf, sizeof(Yamada) * YAMADA_FILE_NUM);
}

static void func_80014658(s32 file_no, void (*cb)(void)) {
    func_80014578(file_no, (void*)0x801B0000, 0);
    func_800145BC(0);
    SysGzipBinDecompress((GzHeader*)0x801B0000, (u8*)0x800A0000);
    cb();
}

void func_800146A4(void) {
    s32 var_s0 = -1;
    while (var_s0) {
        switch (D_8009C560) {
        case 4:
            func_800145BC(0);
            func_80014658(BATTLE_BROM, D_800A00CC);
            break;
        case 2:
            SysBattleSwirlInit();
            func_80014658(BATTLE_BATTLE, D_800A1158);
            break;
        default:
            var_s0 = 0;
            break;
        }
    }
}

void func_80014750(void) {
    s32 size;
    s32 kind;

    SysGzipSetDataBlock((u8*)0x801B0000);
    while (1) {
        kind = SysGzipGetType();
        if (kind == 0xFFFF) {
            break;
        }
        size = SysGzipGetSize();
        if (kind == 9) {
            SysGzipPackDecompressNextBlock(func_80014C80(size));
        } else if (D_80048DD4[kind]) {
            SysGzipPackDecompressNextBlock(D_80048DD4[kind]);
        }
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/144D8", func_80014804);
