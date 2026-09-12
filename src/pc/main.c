#include <game.h>
#include <psyz/dbgserver.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

static void ReservePsxRam(void) {
    void* p =
        mmap((void*)0x80010000, 0x001F0000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (p == MAP_FAILED) {
        ERRORF("failed to reserve PSX RAM");
        exit(1);
    }
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

static u8 D_800696F0[NUM_MENU_COLOR];
void SysMenuStoreWindowColor(void) {
    s32 i;
    for (i = 0; i < NUM_MENU_COLOR; i++) {
        D_800696F0[i] = g_MenuColors[i];
    }
}

void SysMenuRestoreWindowColor(void) {
    s32 i;
    for (i = 0; i < NUM_MENU_COLOR; i++) {
        g_MenuColors[i] = D_800696F0[i];
    }
}

extern u8* D_800707C0;
extern s8 D_80062DFC;

typedef struct {
    s32 tableBase;
    s32 rowBase;
    s32 vAdd;
    s32 clutRow;
} GlyphMode;

s32 SysMenuDrawSingleLetter(s32 x, s32 y, s32 color, s32 packed) {
    GlyphMode m = {0, 0, 0, 0};
    u8 ctrl = (u8)packed;
    u8 glyph;
    u8 tableByte;
    u32 quot, rem;
    s32 u, v;
    SPRT* sprt;

    switch (ctrl) {
    case 0xF8:
        m.rowBase = 0xE7;
        m.vAdd = 0x84;
        glyph = (u8)(packed >> 8);
        break;
    case 0xFA:
        m.rowBase = 0x1B9;
        m.vAdd = 0;
        m.tableBase = 0x10;
        glyph = (u8)(packed >> 8);
        break;
    case 0xFB:
        m.rowBase = 0x2A0;
        m.vAdd = 0x84;
        m.tableBase = 0x10;
        glyph = (u8)(packed >> 8);
        break;
    case 0xFC:
        m.rowBase = 0x372;
        m.vAdd = 0x84;
        m.clutRow = -0x40;
        glyph = (u8)(packed >> 8);
        break;
    case 0xFE:
        m.rowBase = 0x444;
        m.vAdd = 0x84;
        m.tableBase = 0x10;
        m.clutRow = -0x40;
        glyph = (u8)(packed >> 8);
        break;
    default:
        glyph = ctrl;
        break;
    }

    tableByte = D_800707C0[glyph + m.rowBase];
    x += tableByte >> 5;

    quot = (u32)glyph / 21;
    rem = (u32)glyph % 21;
    u = rem * 12;
    v = quot * 12 + m.vAdd;

    sprt = D_80062F24.sprt;
    setSprt(sprt);
    SetShadeTex(sprt, 1);
    sprt->x0 = x;
    sprt->y0 = y + m.tableBase;
    sprt->u0 = (u8)u;
    sprt->v0 = (u8)v;
    sprt->w = 0xC;
    sprt->h = 0xC;
    sprt->clut = GetClut(256, color + 0x1F0);
    AddPrim(D_80062FC4, sprt);
    D_80062F24.sprt++;

    tableByte = D_800707C0[glyph + m.rowBase];
    x += tableByte & 0x1F;

    if (!_D_80062DFD) {
        RECT tw;
        DR_MODE* mode;
        s32 tpage = (((m.clutRow + 0x380) & 0x3FF) >> 6) | 0x30;
        tw.x = 0;
        tw.y = 0;
        tw.w = 0xFF;
        tw.h = 0xFF;
        mode = D_80062F24.poly;
        SetDrawMode(mode, 0, 1, (u16)tpage, &tw);
        D_80062F24.poly = (u8*)mode + sizeof(DR_MODE);
        AddPrim(D_80062FC4, mode);
    }
    return x;
}

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

static void* D_80063008;
static OT_TYPE* D_8006300C;

void func_800269D0(void) { D_80063008 = D_80062F24.poly; }

void func_800269E8(void) { D_80062F24.poly = D_80063008; }

void SysMenuSetOtag(OT_TYPE* ot) { D_80062FC4 = ot; }

void func_80026A0C(void) { D_8006300C = D_80062FC4; }

void func_80026A20(void) { D_80062FC4 = D_8006300C; }

void SysMenuSetDrawMode(s32 dfe, s32 dtd, u16 tpage, RECT* tw) {
    DR_MODE* p = D_80062F24.poly;
    SetDrawMode(p, dfe, dtd, tpage, tw);
    D_80062F24.poly = (u8*)p + sizeof(DR_MODE);
    AddPrim(D_80062FC4, p);
}

void SysMenuSetDrawenv(DRAWENV* src, DISPENV* disp) {
    DR_ENV* p = D_80062F24.poly;
    DRAWENV env;

    SetDefDrawEnv(&env, src->clip.x, src->clip.y, src->clip.w, src->clip.h);
    env.isbg = 1;
    env.dtd = 0;
    env.clip.x += disp->disp.x;
    env.clip.y += disp->disp.y;
    env.clip.w = disp->disp.w;
    env.clip.h = disp->disp.h;
    env.tw.w = 0x3F;
    SetDrawEnv(p, &env);
    AddPrim(D_80062FC4, p);
    D_80062F24.poly = (u8*)p + sizeof(DR_ENV);
}

extern s32 D_80062F88;
extern u8 D_80063048[];

void func_800148A0(void) {
    D_80062F88 = 0;
    g_BattleMode = 0;
}

void func_80014610(void);
void func_80014750(void);
void func_80014804(void);
void func_80014C70(void);
void func_80014578(s32 file_no, void* dst, void (*cb)(void));
void func_800145BC(void (*cb)(void));
void func_800148B4(void) {
    func_800148A0();
    D_800707C0 = D_80063048;
    func_80014610();
    func_80014C70();
    func_80014578(1, (void*)0x801B0000, func_80014804);
    func_800145BC(0);
    func_80014578(2, (void*)0x801B0000, func_80014750);
    func_800145BC(0);
}

void SysLoadUncompressImg(u8* tim) {
    if (*(u32*)(tim + 4) & 8) {
        LoadImage((RECT*)(tim + 0xC), (u_long*)(tim + 0x14));
        tim += (*(u32*)(tim + 8) >> 2) << 2;
    }
    LoadImage((RECT*)(tim + 0xC), (u_long*)(tim + 0x14));
}

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    Psyz_DebugServer(8081);
    ReservePsxRam();

    SysInitBase();
    SysCdromInit();
    func_800148B4(); // YAMADA.BIN + WINDOW.BIN
    InputInit();

    // func_800A0030(); // SCEA message
    func_801D4CC0(); // TITLE screen

    return 0;
}
