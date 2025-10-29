//! G=8
#include <game.h>

typedef enum {
    LBA_INIT_YAMADA = 614,
} Lba;

typedef enum {
    INIT_YAMADA,
    INIT_WINDOW,
    INIT_KERNEL,
    BATTLE_BROM,
    BATTLE_TITLE,
    BATTLE_BATTLE,
    BATTLE_BATINI,
    BATTLE_SCENE,
    BATTLE_BATRES,
    BATTLE_CO,
    YAMADA_FILE_NUM,
} YamadaFile;

typedef enum {
    KERNEL_COMMAND,
    KERNEL_ATTACK,
    KERNEL_GROWTH,
    KERNEL_INIT,
    KERNEL_ITEM,
    KERNEL_WEAPON,
    KERNEL_ARMOR,
    KERNEL_ACCESSORY,
    KERNEL_MATERIA,
    KERNEL_DESC_COMMAND,
    KERNEL_DESC_MAGIC,
    KERNEL_DESC_ITEM,
    KERNEL_DESC_WEAPON,
    KERNEL_DESC_ARMOR,
    KERNEL_DESC_ACCESSORY,
    KERNEL_DESC_MATERIA,
    KERNEL_DESC_KEY_ITEM,
    KERNEL_NAME_COMMAND,
    KERNEL_NAME_MAGIC,
    KERNEL_NAME_ITEM,
    KERNEL_NAME_WEAPON,
    KERNEL_NAME_ARMOR,
    KERNEL_NAME_ACCESSORY,
    KERNEL_NAME_MATERIA,
    KERNEL_NAME_KEY_ITEM,
    KERNEL_TEXT_BATTLE,
    KERNEL_NAME_SUMMON,
} KernelID;

typedef struct {
    s32 loc; // disk sector where the file can be found
    s32 len; // file size in bytes
} Yamada;

typedef struct {
    s32 len; // decompressed length
    s32 unk4;
} GzHeader;

typedef struct {
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
} Unk8001DE0C;

typedef struct {
    u8 padABuffer;
    u8 unk1;
    u8 unk2;
    u8 unk3[31];
    u8 padBBuffer;
    u8 unk23;
    u8 unk24;
    u8 unk25[31];
} Unk800696AC;

typedef struct {
    s32 sector_off;
    s32 length;
} PortraitEntry;

extern s32 D_80010100[];
extern s32 D_80048CFC;
extern s32 D_80048D00;
extern s32 D_80048D04;
extern s32 D_80048D08;
extern s32 D_80048D0C;
extern s32 D_80048D10;
extern s32 D_80048D14;
extern s32 D_80048D18;
extern s32 D_80048D1C;
extern s32 D_80048D20;
extern Yamada D_80048D84[];
extern s32 D_80048DD4[];
extern PortraitEntry D_80048FE8[15];
extern u8 D_80049208[12]; // menu color RGB-quadruplet
extern s32 D_80049224[8];
extern u8 D_80049520[];
extern u8 D_80049528[];
extern u8 D_80063690[];
extern Unk800696AC D_800696AC;
extern u32 D_80062FA0; // Some sort of pad state
extern u16 D_80069490[];
extern u16 D_800707BC;
extern u16 D_800707BE;
extern u8 D_800708D4[];
extern u8 D_800716D0;
extern s32 D_80071744; // LBA loc for func_80014540
extern s16 D_80071A5C;
extern s32 D_80071C24[]; // accessory data?
extern s32 D_80071E44[]; // armor data?
extern s32 D_800722C8;   // LBA dst for func_80014540
extern u8 D_800722DC[];
extern u8 D_800730DC[][0x14];
extern s16 D_80095DD4;
extern s32 D_80095DD8; // LBA len for func_80014540
extern s16 D_800965E8;
extern u16 D_8009C560;
extern u8 D_8009C738[];
extern s16 D_8009A000;
extern u16 D_8009ABF6;
extern u16 D_8009AC32;
extern u8 D_8009D5E9;
extern void (*D_800A00CC)(void); // battle/brom entrypoint
extern void (*D_800A1158)(void); // battle/battle entrypoint

s32 D_80062D4C = 0x00000000;
s32 D_80062D50 = 0x000000FF;
s32 D_80062D54 = 0x00000000;
s32 D_80062D58 = 0x00000009;
s32 D_80062D5C = 0x00000006;
s16 D_80062D60 = 0x0000;
s16 D_80062D62 = 0x0000;
s16 D_80062D64 = 0x0000;
s16 D_80062D66 = 0x0000;
s16 D_80062D68 = 0x0000;
s16 D_80062D6A = 0x0000;
s16 D_80062D6C = 0x0000;
s16 D_80062D6E = 0x0000;
u8 g_bPadsInitialized = 0x00;
s8 D_80062D71 = 0x00;
s16 D_80062D72 = 0x0000;
s16 D_80062D74 = 0x0000;
s16 D_80062D76 = 0x0000;
u16 D_80062D78 = 0x0000;
s16 D_80062D7A = 0x0000;
u16 D_80062D7C = 0x0000;
u16 D_80062D7E = 0x0000;
u16 D_80062D80 = 0x0000;
u16 D_80062D82 = 0x0000;
s32 D_80062D84 = 0x00000000;
s16 D_80062D88 = 0x0000;
s16 D_80062D8A = 0x0000;
s32 D_80062D8C = 0x00000000;
s32 D_80062D90 = 0x00000000;
s32 D_80062D94 = 0x00000000;
s32 D_80062D98 = 0x00000000;
s32 D_80062D9C = 0x00000000;
s32 D_80062DA0 = 0x00140000;
s32 D_80062DA4 = 0x007800A0;
s32 D_80062DA8 = 0x00010002;
s32 D_80062DAC = 0x00060006;
s32 D_80062DB0 = 0x00010001;
s16 D_80062DB4 = 0x0007;
s16 D_80062DB6 = 0x0000;
s16 D_80062DB8 = 0x0000;
s16 D_80062DBA = 0x0000;
s16 D_80062DBC = 0x0000;
s16 D_80062DBE = 0x0000;
s32 D_80062DC0 = 0x00000000;
s32 D_80062DC4 = 0x00000000;
s32 D_80062DC8 = 0x00000000;
s32 D_80062DCC = 0x00000000;
s32 D_80062DD0 = 0x00000000;
s32 D_80062DD4 = 0x00000000;
s16 D_80062DD8 = 0x0000;
s8 D_80062DDA = 0x00;
u8 D_80062DDB = 0x00;
u8 D_80062DDC = 0x02;
static s8 _D_80062DDD = 0x00;
static s8 _D_80062DDE = 0x00;
static s8 _D_80062DDF = 0x00;
s32 D_80062DE0 = 0x00000000;
u8 D_80062DE4 = 0x00;
u8 D_80062DE5 = 0x00;
s16 D_80062DE6 = 0x00B4;
s16 D_80062DE8 = 0x0068;
s16 D_80062DEA = 0x0000;
s32 D_80062DEC = 0x801D0000;
s32 D_80062DF0 = 0x00000084;
s32 D_80062DF4 = 0xFFFFFFFF;
s32 D_80062DF8 = 0x00000001;
s8 D_80062DFC = 0x40;
static s8 _D_80062DFD = 0x00;
static s8 _D_80062DFE = 0x00;
static s8 _D_80062DFF = 0x00;
s32 D_80062E00 = 0x00000000;
s32 D_80062E04 = 0x00000000;
s16 D_80062E08 = 0x0000;
s16 D_80062E0A = 0x0000;
s32 D_80062E0C = 0;
s32 D_80062E10 = 0; // most likely a struct with D_80062E14 (see func_80014C44)
s32 D_80062E14 = 0;
s32 D_80062E18 = 0;
s32 D_80062E1C = 0;
s32 D_80062E20 = 0;
s32 D_80062E24 = 0x00000000;
s32 D_80062E28 = 0x00000000;
s32 D_80062E2C = 0x00000000;
s32 D_80062E30 = 0x00000000;
s32 D_80062E34 = 0x00000000;
s32 D_80062E38 = 0x00000000;
s32 D_80062E3C = 0x00000000;
s32 D_80062E40 = 0x00000000;
s32 D_80062E44 = 0x00000000;
s32 D_80062E48 = 0x00000000;
s32 D_80062E4C = 0x00000000;
s8 D_80062E50 = 0x00;
static s8 _D_80062E51 = 0x00;
static s8 _D_80062E52 = 0x00;
static s8 _D_80062E53 = 0x00;
s8 D_80062E54 = 0x00;
static s8 _D_80062E55 = 0x00;
static s8 _D_80062E56 = 0x00;
static s8 _D_80062E57 = 0x00;
s8 D_80062E58 = 0x00;
static s8 _D_80062E59 = 0x00;
static s8 _D_80062E5A = 0x00;
static s8 _D_80062E5B = 0x00;
s8 D_80062E5C = 0x00;
static s8 _D_80062E5D = 0x00;
static s8 _D_80062E5E = 0x00;
static s8 _D_80062E5F = 0x00;
s32 D_80062E60 = 0;
s8 D_80062E64 = 0x00;
static s8 _D_80062E65 = 0x00;
static s8 _D_80062E66 = 0x00;
static s8 _D_80062E67 = 0x00;
s8 D_80062E68 = 0x00;
static s8 _D_80062E69 = 0x00;
static s8 _D_80062E6A = 0x00;
static s8 _D_80062E6B = 0x00;
s32 D_80062E6C = 0x00000000;
s16 D_80062E70 = 0x0000;
s16 D_80062E72 = 0x0000;
s16 D_80062E74 = 0x0000;
s16 D_80062E76 = 0x0000;
s8 D_80062E78 = 0x00;
static s8 _D_80062E79 = 0x00;
static s8 _D_80062E7A = 0x00;
static s8 _D_80062E7B = 0x00;
s32 D_80062E7C = 0x00000000;
s32 D_80062E80 = 0x00000000;
s32 D_80062E84 = 0x00000000;
s16 D_80062E88 = 0x0000;
s16 D_80062E8A = 0x0000;
s8 D_80062E8C = 0x00;
static s8 _D_80062E8D = 0x00;
static s8 _D_80062E8E = 0x00;
static s8 _D_80062E8F = 0x00;
s8 D_80062E90 = 0x00;
static s8 _D_80062E91 = 0x00;
static s8 _D_80062E92 = 0x00;
static s8 _D_80062E93 = 0x00;
s32 D_80062E94 = 0x00000000;
s32 D_80062E98 = 0x00000000;
s32 D_80062E9C = 0x00000000;
s32 D_80062EA0 = 0x00000000;
s32 D_80062EA4 = 0x00000000;
s16 D_80062EA8 = 0x0000;
s16 D_80062EAA = 0x0000;
s16 D_80062EAC = 0x0000;
s16 D_80062EAE = 0x0000;
s16 D_80062EB0 = 0x0000;
s16 D_80062EB2 = 0x0000;
s8 D_80062EB4 = 0x00;
static s8 _D_80062EB5 = 0x00;
static s8 _D_80062EB6 = 0x00;
static s8 _D_80062EB7 = 0x00;
s32 D_80062EB8 = 0;
s8 D_80062EBC = 0;
static s8 _D_80062EBD = 0;
static s8 _D_80062EBE = 0;
static s8 _D_80062EBF = 0;
s8 D_80062EC0 = 0;
static s8 _D_80062EC1 = 0;
static s8 _D_80062EC2 = 0;
static s8 _D_80062EC3 = 0;

void func_8001155C(void);
void func_80014A00(s32* dst, s32* src, s32 len);
u16* func_80014D9C(s32, s32, s32);
s32 func_800150E4(u16*, u16*);
u16* func_800151F4(s32);
void func_80015CA0(GzHeader* src, s32* dst);
u8 func_8001F6B4();
s32 func_80034B44();

void func_800110B8(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800110C0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001117C);

void func_800111E4(void) {
    D_8009A000 = 0xF4;
    func_8002DA7C();
    if (!(D_8009D5E9 & 0x30)) {
        func_8001117C(0x2B);
    }
    D_800707BC = D_8009ABF6;
    D_800707BE = D_8009AC32;
    D_800707BE = D_800716D0 | D_800707BE;
    func_800146A4();
    D_800716D0 = 0;
}

void func_80011274(void) {
    func_80033E34(D_80048D1C, D_80048D20, (u32*)0x800E0000, 0);

    while (1) {
        if (func_80034B44() == 0) {
            break;
        }
    }

    func_80033E34(D_80048D14, D_80048D18, (u32*)0x800A0000, 0);

    while (1) {
        if (func_80034B44() == 0) {
            break;
        }
    }

    func_80029818((u32*)0x800A0000, (u32*)0x800E0000);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800112E8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001146C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001155C);

void func_8001171C(void) {
    StopCallback();
    ResetCallback();
    func_80043938(0);
    func_80036298();
    D_80095DD4 = 0;
    VSyncCallback(&func_8001155C);
    func_80043BA8(0);
    SetDispMask(0);
    func_80039EDC();
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80011784);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80011860);

void func_80011920(void) {
    D_800965E8 = 0;
    D_80071A5C = 0;
}

void func_80011938(void) {
    func_80033E34(D_80048CFC, D_80048D00, 0x800F0000, 0);
    do {
    } while (func_80034B44());
    func_80033E34(D_80048D04, D_80048D08, 0x801B0000, 0);
    do {
    } while (func_80034B44());
    func_80033E34(D_80048D0C, D_80048D10, 0x801BC800, 0);
    do {
    } while (func_80034B44());
    func_8002988C(0x800F0000, 0x801BC800);
    func_80029998(0x801B0000);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800119E4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80011AEC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80011BB4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", main);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80012840);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800128B8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800129D0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80012A8C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80012DB0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800131B8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800134F4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80013564);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800135C0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80013624);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80013800);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800138EC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80013C9C);

void func_800140A4(void) {
    D_8019DAA0++;
    if (!(D_8019DAA0 & 1)) {
        DrawOTag(D_8019D5E8);
        func_80013C9C();
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800140F4);

// obtain file sector from a YamadaFile
s32 func_800144D8(s32 file_no) { return D_80048D84[file_no].loc; }

void func_800144F0(s32 file_no) { func_80033DAC(file_no, 0); }

void func_80014510(s32 file_no) { func_800144F0(D_80048D84[file_no].loc); }

// used to load WORLD/WORLD.BIN or FIELD/FIELD.BIN
void func_80014540(void) {
    func_80033E34(D_80071744, D_80095DD8, D_800722C8, 0);
}

void func_80014578(s32 file_no, void* dst, s32 mode) {
    func_80033E34(D_80048D84[file_no].loc, D_80048D84[file_no].len, dst, mode);
}

void func_800145BC(void (*cb)(void)) {
    while (func_80034B44()) {
        if (cb) {
            cb();
        }
    }
}

void func_80014608(void) {}

// initialize LBA system
void func_80014610(void) {
    u8 buf[2048];
    func_80033E34(LBA_INIT_YAMADA, sizeof(buf), &buf, 0);
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

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800146A4);
#else
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
#endif

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

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014804);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800148A0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800148B4);

void func_80014934(void) {
    func_800148A0();
    func_80014578(INIT_KERNEL, (void*)0x801B0000, 0);
    func_800145BC(0);
    func_80015C3C(0x801B0000, &D_8009C738, KERNEL_INIT);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014980);

void func_800149E0(void) { DrawSync(0); }

void func_80014A00(s32* dst, s32* src, s32 len) {
    int i;
    for (i = 0; i < len >> 2; i++) {
        *dst++ = *src++;
    }
}

s32 func_80014A38(u32 arg0) {
    s32 i;
    for (i = 0;; i++) {
        arg0 >>= 1;
        if (!arg0) {
            return i;
        }
    }
}

s32 func_80014A58(u32 arg0) {
    s32 i;
    i = 0;
    while (arg0) {
        if (arg0 & 1) {
            i++;
        }
        arg0 >>= 1;
    }
    return i;
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014A84);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014B08);

void func_80014B54(void) { D_80062E18 = (D_80062E18 + 1) & 7; }

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014B70);

s32 func_80014BA8(s32 arg0) {
    return (u8)(((func_80014B70() & 0xFF) * arg0) >> 8);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014BE4);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014C44);
#else
// func_80014C44 needs to be in a different file than D_80062E10
void func_80014C44(s32 arg0) {
    s32 i;
    for (i = 0; i < 8; i++) {
        D_80062E10[i] = arg0;
        arg0 >>= 1;
    }
    D_80062E18 = 0;
}
#endif

void func_80014C70() {
    D_80062E1C = 0;
    D_80062E20 = 0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014C80);

s32 func_80014CBC(s32 arg0, s32 arg1) {
    s32 var_a2;
    u8 var_v1;

    var_v1 = 0xFF;
    var_a2 = -1;
    switch (arg0) {
    case 0:
    case 1:
    case 2:
        var_v1 = D_800708D4[(D_80010100[arg0] + arg1) * 0x1C];
        break;
    case 4:
        if (arg1 < 0x80) {
            var_v1 = D_800722DC[arg1 * 0x1C];
        }
    }
    if (var_v1 != 0xFF) {
        var_a2 = var_v1;
    }
    return var_a2;
}

u8* func_80014D58(u8* arg0, u8* arg1, s32 arg2) {
    u8 var_a3 = *arg1;
    while (var_a3 != 0xFF) {
        *arg0 = var_a3;
        arg1++;
        arg2--;
        arg0++;
        if (arg2 == -1) {
            break;
        }
        var_a3 = *arg1;
    }
    return arg0;
}

u16* func_80014D9C(s32 arg0, s32 arg1, s32 arg2) {
    u8* temp_v1 = D_80063690 + D_80069490[arg0 + arg2];
    return (u16*)&temp_v1[*(u16*)&temp_v1[arg1 * 2]];
}

void func_80014DD0(s32 arg0, s32 arg1, u8* arg2) {
    func_80014D58(arg2, (u8*)func_80014D9C(arg0, arg1, 0), -1);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014E0C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80014E74);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800150E4);

u16* func_800151F4(s32 arg0) { return func_80014D9C(0x10, arg0, 0); }

s32 func_8001521C(s32 arg0) {
    u16* temp_v0 = func_800151F4(arg0);
    return func_800150E4(temp_v0, temp_v0);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015248);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800155A4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800155B0);

void func_80015654(s32 arg0) {
    D_80062E24 = 0;
    D_80062E28 = 0;
    D_80062E2C = arg0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015668);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800159B0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015AFC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015B44);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015B50);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015B88);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015BC0);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015C3C);
#else
// load kernel module by its ID
// https://wiki.ffrtt.ru/index.php/FF7/Kernel/Low_level_libraries#BIN-GZIP_Type_Archives
s32 func_80015C3C(u8* src, void* dst, s32 id) {
    s32 len;
    s32 cur_id;
    s32 ret;

    ret = -1;
    while (len = (src[0] | (src[1] << 8))) {
        cur_id = src[4] | (src[5] << 8);
        if (cur_id == id) {
            ret = func_80017108(src + 6, dst);
            break;
        }
        src += len + 6;
    }
    return ret;
}
#endif

void func_80015CA0(GzHeader* src, s32* dst) {
    s32 i;
    s32* var_s1;
    u32 len;
    s32 unk4;

    unk4 = src->unk4;
    len = src->len;
    func_80017108(src + 1, dst);
    var_s1 = &dst[len >> 2];
    unk4 = (u32)unk4 >> 2;
    for (i = 0; i < unk4; i++) {
        var_s1[i] = 0;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015D14);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80015D64);

s32 func_80016320(s32* arg0) {
    while (arg0) {
        arg0 = (s32*)arg0[-1];
    }
    return 0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80016340);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800166C0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80016808);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800169B8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80016F90);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001708C);

// gzip inflate
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80017108);

void func_80017238(u32 arg0, u32* arg1, u8* arg2) {
    *arg2 = arg0;
    *arg1 = arg0 >> 8;
    func_8001AC9C(*arg2, *arg1);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001726C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80017678);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001786C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80017E68);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80017F38);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018028);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018220);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800182FC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018390);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001840C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800184C0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800185A8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018630);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018834);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018934);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018A04);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018AB0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018B14);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018BB8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018C94);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018D4C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018E18);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018E90);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018ECC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80018FC0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019064);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800190E8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800191A0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019254);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019338);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001937C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800193F4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019440);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800194BC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019544);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019608);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001964C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019690);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800197B8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019978);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019D1C);

void func_80019D74(u8 arg0, u8 arg1) {
    if (arg1 == 0xB) {
        func_80019E4C(arg0);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019DA0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019E4C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019E84);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80019F90);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A174);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A1C8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A280);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A384);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A3B8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A440);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A4A8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A518);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A5B4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A684);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A780);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A874);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A980);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001A9CC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001AB1C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001AC9C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001AE08);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001AEE4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001B4A0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001B570);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001B5E4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001B704);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001B834);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001B8A8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001B944);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001BA54);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001BB30);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001BC18);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001BCE8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001BD50);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001BDB0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C0EC);

void func_8001C3C4(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C3CC);

void SetupGamepad(void) {
    if (g_bPadsInitialized == 0) {
        g_bPadsInitialized = 1;
        StartPAD2();
        InitPAD2(&D_800696AC.padABuffer, 4, &D_800696AC.padBBuffer, 4);
    }
    D_80062FA0 = 0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C484);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C498);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C4E8);

void func_8001C58C(void) {
    if (!func_8001F6B4()) {
        D_80062D71 = 0;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C5BC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C788);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C808);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C8D4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001C980);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001CB48);

void func_8001CDA4(void) {
    SetPolyFT4(D_80062F24.ft4);
    SetShadeTex(D_80062F24.ft4, 1);
    D_80062F24.ft4->x0 = 0;
    D_80062F24.ft4->y0 = 5;
    D_80062F24.ft4->x1 = 0x40;
    D_80062F24.ft4->y1 = 5;
    D_80062F24.ft4->x2 = 0;
    D_80062F24.ft4->y2 = 0x45;
    D_80062F24.ft4->x3 = 0x40;
    D_80062F24.ft4->y3 = 0x45;
    D_80062F24.ft4->u0 = 0;
    D_80062F24.ft4->v0 = 0;
    D_80062F24.ft4->u1 = 128;
    D_80062F24.ft4->v1 = 0;
    D_80062F24.ft4->u2 = 0;
    D_80062F24.ft4->v2 = 128;
    D_80062F24.ft4->u3 = 128;
    D_80062F24.ft4->v3 = 128;
    D_80062F24.ft4->clut = GetClut(0, 0x1FE);
    D_80062F24.ft4->tpage = GetTPage(1, 0, 0x340, 0);
    AddPrim(D_80062FC4, D_80062F24.ft4);
    D_80062F24.ft4++;
}

void func_8001CF3C(s16 x, s16 y, s16 w, s16 h, u16 tx, u16 ty, u16 tw, u16 th,
                   s16 clut, s32 tex) {
    SetPolyFT4(D_80062F24.ft4);
    SetShadeTex(D_80062F24.ft4, 1);
    if (tex << 0x10) {
        SetSemiTrans(D_80062F24.ft4, 1);
    }
    D_80062F24.ft4->x0 = x;
    D_80062F24.ft4->y0 = y;
    D_80062F24.ft4->x1 = x + w;
    D_80062F24.ft4->y1 = y;
    D_80062F24.ft4->x2 = x;
    D_80062F24.ft4->y2 = y + h;
    D_80062F24.ft4->x3 = x + w;
    D_80062F24.ft4->y3 = y + h;
    D_80062F24.ft4->u0 = tx;
    D_80062F24.ft4->v0 = ty;
    D_80062F24.ft4->u1 = tx + tw;
    D_80062F24.ft4->v1 = ty;
    D_80062F24.ft4->u2 = tx;
    D_80062F24.ft4->v2 = ty + th;
    D_80062F24.ft4->u3 = tx + tw;
    D_80062F24.ft4->v3 = ty + th;
    D_80062F24.ft4->clut = GetClut(0x100, (s16)clut + 0x1E0);
    D_80062F24.ft4->tpage = GetTPage(1, 0, 0x3C0, 0x100);
    AddPrim(D_80062FC4, D_80062F24.ft4);
    D_80062F24.ft4++;
}

void func_8001D180(s16 x, s16 y, s16 w, s16 h, u16 tx, u16 ty, u16 tw, u16 th,
                   s16 clut, s32 tex) {
    SetPolyFT4(D_80062F24.ft4);
    SetShadeTex(D_80062F24.ft4, 1);
    if (tex << 0x10) {
        SetSemiTrans(D_80062F24.ft4, 1);
    }
    D_80062F24.ft4->x0 = x;
    D_80062F24.ft4->y0 = y;
    D_80062F24.ft4->x1 = x + w;
    D_80062F24.ft4->y1 = y;
    D_80062F24.ft4->x2 = x;
    D_80062F24.ft4->y2 = y + h;
    D_80062F24.ft4->x3 = x + w;
    D_80062F24.ft4->y3 = y + h;
    D_80062F24.ft4->u0 = tx;
    D_80062F24.ft4->v0 = ty;
    D_80062F24.ft4->u1 = tx + tw;
    D_80062F24.ft4->v1 = ty;
    D_80062F24.ft4->u2 = tx;
    D_80062F24.ft4->v2 = ty + th;
    D_80062F24.ft4->u3 = tx + tw;
    D_80062F24.ft4->v3 = ty + th;
    D_80062F24.ft4->clut = GetClut(0x180, (s16)clut);
    D_80062F24.ft4->tpage = GetTPage(1, 0, 0x340, 0x100);
    AddPrim(D_80062FC4, D_80062F24.ft4);
    D_80062F24.ft4++;
}

void func_8001D3C0(s16 x, s16 y) {
    SetTile1(D_80062F24.tile1);
    D_80062F24.tile1->x0 = x;
    D_80062F24.tile1->y0 = y;
    D_80062F24.tile1->r0 = 0xFF;
    D_80062F24.tile1->g0 = 0xFF;
    D_80062F24.tile1->b0 = 0;
    AddPrim(D_80062FC4, D_80062F24.tile1);
    D_80062F24.tile1++;
}

void func_8001D47C(s16 x, s16 w, s16 y, s32 color) {
    SetBlockFill(D_80062F24.blk_fill);
    D_80062F24.blk_fill->r0 = color >> 16;
    D_80062F24.blk_fill->g0 = color >> 8;
    D_80062F24.blk_fill->b0 = color;
    D_80062F24.blk_fill->x0 = x;
    D_80062F24.blk_fill->y0 = y;
    D_80062F24.blk_fill->w = w;
    D_80062F24.blk_fill->h = y;
    AddPrim(D_80062FC4, D_80062F24.blk_fill);
    D_80062F24.blk_fill++;
}

void func_8001D56C(s16 x, s16 y, s16 w, s16 h, s16 is_yellow) {
    if (is_yellow) {
        SetBlockFill(D_80062F24.blk_fill);
        D_80062F24.blk_fill->r0 = 0xFF;
        D_80062F24.blk_fill->g0 = 0xFF;
        D_80062F24.blk_fill->b0 = 0;
    } else {
        SetBlockFill(D_80062F24.blk_fill);
        D_80062F24.blk_fill->r0 = 0x80;
        D_80062F24.blk_fill->g0 = 0x80;
        D_80062F24.blk_fill->b0 = 0x80;
    }
    D_80062F24.blk_fill->x0 = x;
    D_80062F24.blk_fill->y0 = y;
    D_80062F24.blk_fill->w = w;
    D_80062F24.blk_fill->h = h;
    AddPrim(D_80062FC4, D_80062F24.blk_fill);
    D_80062F24.blk_fill++;
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001D6A8);

void func_8001DE0C(Unk8001DE0C* arg0, s16 arg1, s16 arg2, s16 arg3, s32 arg4) {
    arg0->unk0 = arg1;
    arg0->unk2 = arg2;
    arg0->unk4 = arg3;
    arg0->unk6 = arg4;
}

// translate window dialog
void func_8001DE24(Unk8001DE0C* arg0, s32 arg1, s32 arg2) {
    arg0->unk0 = arg0->unk0 + arg1;
    arg0->unk2 = arg0->unk2 + arg2;
}

// set window dialog rect
void func_8001DE40(Unk8001DE0C* arg0, Unk8001DE0C* arg1) {
    arg0->unk0 = arg1->unk0;
    arg0->unk2 = arg1->unk2;
    arg0->unk4 = arg1->unk4;
    arg0->unk6 = arg1->unk6;
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001DE70);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001DEB0);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001DEF0);
#else
// only matches with --aspsx-version=2.21
// sets the menu color with a quadruplet of RGB values
void func_8001DEF0(u8* menu_colors) {
    s32 i;
    for (i = 0; i < 12; i++) {
        D_80049208[i] = *menu_colors++;
    }
}
#endif

void func_8001DF24(RECT* rect, u8 arg1, u8 arg2, u8 arg3) {
    setTile(D_80062F24.tile);
    SetShadeTex(D_80062F24.tile, 1);
    D_80062F24.tile->x0 = rect->x;
    D_80062F24.tile->y0 = rect->y;
    D_80062F24.tile->w = rect->w;
    D_80062F24.tile->h = rect->h;
    D_80062F24.tile->r0 = arg1;
    D_80062F24.tile->g0 = arg2;
    D_80062F24.tile->b0 = arg3;
    AddPrim(D_80062FC4, D_80062F24.tile++);
}

// prints menu window
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001E040);

// print menu cursor
void func_8001EB2C(s16 x, s16 y) {
    RECT rect;

    setSprt(D_80062F24.sprt);
    SetSemiTrans(D_80062F24.sprt, 1);
    SetShadeTex(D_80062F24.sprt, 1);
    D_80062F24.sprt->x0 = x;
    D_80062F24.sprt->y0 = y;
    D_80062F24.sprt->u0 = 224;
    D_80062F24.sprt->v0 = 8;
    D_80062F24.sprt->w = 24;
    D_80062F24.sprt->h = 16;
    D_80062F24.sprt->clut = GetClut(0x100, 0x1E1);
    AddPrim(D_80062FC4, D_80062F24.sprt++);
    rect.x = 0;
    rect.y = 0;
    rect.w = 0xFF;
    rect.h = 0xFF;
    func_80026A34(0, 1, (u16)GetTPage(0, 2, 0x3C0, 0x100), &rect);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001EC70);

void func_8001EF84(s32 x, s32 y, s32 n, s32 len) {
    RECT rect;
    s32 i;
    s32 uv;

    for (i = 0; i < 8; i++) {
        uv = n / D_80049224[i];
        setSprt(D_80062F24.sprt);
        SetShadeTex(D_80062F24.sprt, 1);
        D_80062F24.sprt->x0 = x;
        D_80062F24.sprt->y0 = y;
        D_80062F24.sprt->u0 = (uv % 5) * 16 - 80;
        D_80062F24.sprt->v0 = uv >= 5 ? 104 : 80;
        D_80062F24.sprt->w = 16;
        D_80062F24.sprt->h = 21;
        D_80062F24.sprt->clut = GetClut(0x100, 0x1EC);
        if (len >= 8 - i) {
            x += 16;
            AddPrim(D_80062FC4, D_80062F24.sprt++);
        }
        n %= D_80049224[i];
    }
    rect.x = 0;
    rect.y = 0;
    rect.w = 255;
    rect.h = 255;
    func_80026A34(0, 1, (u16)GetTPage(0, 1, 0x3C0, 0x100), &rect);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001F1BC);

void func_8001F6AC(void) {}

u8 func_8001F6B4(void) { return D_80062DDB; }

void func_8001F6C0(s32 arg0, s8 arg1) {
    D_80062DDB = 1;
    D_80062DDC = arg1;
    D_80062DE0 = 0x28;
    D_80062EB8 = arg0;
    D_80062DE5 = 1;
}

void func_8001F6E4(s16 arg0, s16 arg1, s16 arg2) {
    D_80062DE4 = arg0;
    if (arg0) {
        D_80062DE6 = arg1;
        D_80062DE8 = arg2;
    } else {
        D_80062DDB = 0;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001F710);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FA28);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FA68);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FAAC);

void func_8001FAF0(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FAF8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FBAC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FCDC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FE6C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FF50);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FF8C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8001FFD4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8002001C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80020058);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800206E4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80020B68);

void func_80021044(DRAWENV* draw_env, DISPENV* disp_env) {
    VSync(0);
    SetDefDrawEnv(draw_env, 0, 0, 0x180, 0x1D8);
    draw_env[0].dfe = 1;
    draw_env[0].isbg = 1;
    PutDrawEnv(draw_env);
    VSync(0);
    SetDefDrawEnv(draw_env, 0, 8, 0x180, 0xE0);
    SetDefDrawEnv(&draw_env[1], 0, 0xF0, 0x180, 0xE0);
    SetDefDispEnv(&disp_env[0], 0, 0xE8, 0x16C, 0xF0);
    SetDefDispEnv(&disp_env[1], 0, 0, 0x16C, 0xF0);
    draw_env[1].isbg = 1;
    draw_env[0].isbg = 1;
    draw_env[1].dfe = 1;
    draw_env[0].dfe = 1;
    draw_env[1].dtd = 1;
    draw_env[0].dtd = 1;
    draw_env[0].r0 = 0;
    draw_env[0].g0 = 0;
    draw_env[0].b0 = 0;
    draw_env[1].r0 = 0;
    draw_env[1].g0 = 0;
    draw_env[1].b0 = 0;
    draw_env[0].tpage = draw_env[1].tpage =
        func_80043CC0() != 1 && func_80043CC0() != 2 ? 0x3F : 0xAF;
    VSync(0);
    PutDispEnv(disp_env);
    PutDrawEnv(draw_env);
    SetDispMask(1);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800211B8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800211C4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8002120C);

void func_80021258(s32 arg0) { func_80015248(13, arg0, 8); }

void func_80021280(s32 arg0) { func_80015248(4, arg0, 8); }

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800212A8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80021BAC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80021C4C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80021D5C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80021E70);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80021F58);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80022B5C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80022DE4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80022FE0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80023050);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8002305C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800230C4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8002368C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80023788);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8002382C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80023940);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80023AC4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80023AD4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80024A04);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80024A3C);

void func_80024D88(s32 arg0) {
    func_800211C4(0xD);
    do {
    } while (func_80034B44());
    VSync(30);
    func_801D131C(arg0);
}

void func_80024DD4(s32 arg0) {
    func_800211C4(0xE);
    do {
    } while (func_80034B44());
    func_801D1A6C(arg0);
}

void func_80024E18(s32 arg0) {
    func_800211C4(0xF);
    do {
    } while (func_80034B44());
    func_801D4118(arg0);
}

// This should be the title screen handler
void func_80024E5C(void) {
    func_800211C4(0x10); // load title screen?
    do {                 // wait until it's loaded?
    } while (func_80034B44());
    func_801D4CC0(); // jump into title screen loop?
}

void func_80024E94(void) {
    func_800211C4(0xA);
    do {
    } while (func_80034B44());
    func_801D1774();
}

void func_80024ECC(void) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D2D74();
}

void func_80024F04(void) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D2E84();
}

void func_80024F3C(s32 arg0) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D2F00(arg0);
}

void func_80024F80(s32 arg0) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D3138(arg0);
}

void func_80024FC4(s32 arg0) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D3018(arg0);
}

void func_80025008(void) {
    func_800211C4(1);
    do {
    } while (func_80034B44());
    func_801D3228();
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025040);

void func_800250B4(void) {
    func_800211C4(0xC);
    do {
    } while (func_80034B44());
    func_801D027C();
}

void func_800250EC(s32 arg0) {
    func_800211C4(0xC);
    do {
    } while (func_80034B44());
    func_801D05C4(arg0);
}

void func_80025130(s32 arg0) {
    func_800211C4(0xC);
    do {
    } while (func_80034B44());
    func_801D0704(arg0);
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025174);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025288);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025310);

void func_80025360() { func_8001FA28(0x19F); }

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025380);

s32 func_8002542C(s32 arg0) {
    s32 i;
    for (i = 0; i < MAX_MATERIA_COUNT; i++) {
        if (_work.materia[i] == -1) {
            _work.materia[i] = arg0;
            if (func_8002603C(arg0 & 0xFF) == 10) {
                _work.unkBEF |= 1;
            }
            if ((arg0 & 0xFF) == 44) {
                _work.unkBEF |= 2;
            }
            return -1;
        }
    }
    return arg0;
}

void func_800254D8() { D_80062EBC = 0; }

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800254E4);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025514);

void func_80025648(void) {}

void func_80025650(void) {}

s32 func_80025658() { return D_8009C738[1]; }

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025668);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800256DC);

s32* func_80025758(s32 arg0) { return &D_80071E44[arg0 * 9]; }

s32* func_80025774(s32 arg0) { return &D_80071C24[arg0 * 4]; }

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025788);
#else
// --aspsx-version=2.21
Unk8009D84C* func_80025788(s32 arg0) {
    Unk8009D84C* partyMember;

    partyMember = (Unk8009D84C*)0xFF;
    if (_work.partyID[arg0] != 0xFF) {
        return &D_8009D84C[arg0];
    }
    return 0xFF;
}
#endif

void func_800257C4(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800257CC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025800);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800258BC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025988);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025A44);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025B10);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025B48);

s32 SYS_gil(void) { return _work.gil; }

void func_80025B8C(u_long* image) {
    RECT rect;
    rect.x = 0x340;
    rect.y = 0x184;
    rect.w = 0x30;
    rect.h = 0x78;
    StoreImage(&rect, image);
}

void func_80025BD0(u_long* image) {
    RECT rect;
    rect.x = 0x340;
    rect.y = 0x184;
    rect.w = 0x30;
    rect.h = 0x78;
    LoadImage(&rect, image);
}

void func_80025C14(u_long* image) {
    RECT rect;
    rect.x = 0x180;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 9;
    StoreImage(&rect, image);
}

void func_80025C54(u_long* image) {
    RECT rect;
    rect.x = 0x180;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 9;
    LoadImage(&rect, image);
}

void func_80025C94(u_long* image) {
    RECT rect;
    rect.x = 0x100;
    rect.y = 0x1ED;
    rect.w = 0x100;
    rect.h = 3;
    LoadImage(&rect, image);
}

void func_80025CD4(u_long* image) {
    RECT rect;
    rect.x = 0x100;
    rect.y = 0x1ED;
    rect.w = 0x100;
    rect.h = 3;
    StoreImage(&rect, image);
}

void func_80025D14(u_long* addr, s32 px, s32 py, s32 cx, s32 cy) {
    TIM_IMAGE tim;
    OpenTIM(addr);
    while (ReadTIM(&tim)) {
        if (tim.caddr) {
            tim.crect->x = cx;
            tim.crect->y = cy;
            LoadImage(tim.crect, tim.caddr);
            DrawSync(0);
        }
        if (tim.paddr) {
            tim.prect->x = px;
            tim.prect->y = py;
            LoadImage(tim.prect, tim.paddr);
            DrawSync(0);
        }
    }
}

// this function seems to be responsible of loading the characters' portrait
void func_80025DF8(void) {
    u8 dummy[8];
    u8 buf[0x1000];
    u_long* dst;
    s32 i;
    s32* sector_off;
    s32* length;
    s32 cx, cy;

    i = 0;
    dst = (u_long*)buf;
    sector_off = &D_80048FE8->sector_off;
    length = &D_80048FE8->length;
    for (; i < 9; i++) {
        func_80033F40(sector_off[i * 2], length[i * 2], dst, 0);
        cx = 0x340 + (i / 5) * 0x18;
        cy = 0x100 + (i % 5) * 0x30;
        func_80025D14(dst, cx, cy, 0x180, i);
        DrawSync(0);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80025ED4);

void func_80026034(void) {}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8002603C);
#else
// --aspsx-version=2.21
u8 func_8002603C(u8 arg0) {
    return D_80049520[D_80049528[D_800730DC[arg0][1] & 0xF]];
}
#endif

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026090);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800260DC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026258);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800262D8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026408);

void func_80026448(Unk80026448* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4,
                   s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9, s32 arg10,
                   s32 arg11, s32 arg12, u16 arg13) {
    arg0->unkA = arg1;
    arg0->unkB = arg2;
    arg0->unkC = arg3;
    arg0->unkD = arg4;
    arg0->unk0 = arg5;
    arg0->unk2 = arg6;
    arg0->unk4 = arg7;
    arg0->unk6 = arg8;
    arg0->unkE = arg9;
    arg0->unkF = arg10;
    arg0->unk10 = arg11;
    arg0->unk11 = arg12;
    arg0->unk8 = arg13;
}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800264A8);

void func_800269C0(void* arg0) { D_80062F24.poly = arg0; }

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800269D0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800269E8);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026A00);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026A0C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026A20);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026A34);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026A94);

void func_80026B5C(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026B64);

// strlen but for FF7 strings
// FF7 string is 0x00: ' ', 0x10: '0', 0x21: 'A', 0xFF: terminator
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026B70);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026C5C);

// print FF7 string
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80026F44);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_8002708C);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80027354);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80027408);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80027990);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80027B84);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80028030);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80028484);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_800285AC);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80028930);

void func_80028CA0(s16, s16, s16, s16, s16, s16, s16, s16);
INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80028CA0);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80028E00);

INCLUDE_ASM("asm/us/main/nonmatchings/18B8", func_80029114);
