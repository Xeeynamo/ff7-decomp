//! G=0
#include "main_private.h"

void func_80014A00(s32* dst, s32* src, s32 len);
u8* func_80014C80(s32 arg0);
void func_80015CA0(GzHeader* src, s32* dst);
void func_80015B44(u8* arg0);
u16 func_80015B50(void);
u16 func_80015B88(void);
u32 func_80015BC0(u8* dst);

// obtain file sector from a YamadaFile
s32 func_800144D8(s32 file_no) { return D_80048D84[file_no].loc; }

void func_800144F0(s32 file_no) { func_80033DAC(file_no, 0); }

void func_80014510(s32 file_no) { func_800144F0(D_80048D84[file_no].loc); }

// used to load WORLD/WORLD.BIN or FIELD/FIELD.BIN
void func_80014540(void) {
    SystemLoadFileBySector(D_80071744, D_80095DD8, D_800722C8, NULL);
}

void func_80014578(s32 file_no, void* dst, void (*cb)(void)) {
    SystemLoadFileBySector(
        D_80048D84[file_no].loc, D_80048D84[file_no].len, dst, cb);
}

void func_800145BC(void (*cb)(void)) {
    while (SystemCdromReadChain()) {
        if (cb) {
            cb();
        }
    }
}

void func_80014608(void) {}

// initialize LBA system
void func_80014610(void) {
    u8 buf[2048];
    SystemLoadFileBySector(LBA_INIT_YAMADA, sizeof(buf), (u_long*)&buf, NULL);
    func_800145BC(0);
    func_80014A00(
        (s32*)D_80048D84, (s32*)&buf, sizeof(Yamada) * YAMADA_FILE_NUM);
}

void func_80014658(s32 file_no, void (*cb)(void)) {
    func_80014578(file_no, (void*)0x801B0000, 0);
    func_800145BC(0);
    func_80015CA0((GzHeader*)0x801B0000, (s32*)0x800A0000);
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
            func_800140F4();
            func_80014658(BATTLE_BATTLE, D_800A1158);
            break;
        default:
            var_s0 = 0;
            break;
        }
    }
}

void func_80014750(void) {
    s32 temp_a0;
    s32 temp_s0;

    func_80015B44(0x801B0000);
    while (1) {
        temp_s0 = func_80015B50() & 0xFFFF;
        if (temp_s0 == 0xFFFF) {
            break;
        }
        temp_a0 = func_80015B88() & 0xFFFF;
        if (temp_s0 == 9) {
            func_80015BC0(func_80014C80(temp_a0));
        } else if (D_80048DD4[temp_s0]) {
            func_80015BC0(D_80048DD4[temp_s0]);
        }
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/144D8", func_80014804);
