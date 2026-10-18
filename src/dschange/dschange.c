//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include <game.h>
#include <libcd.h>
#include <akao.h>

#define PAD_SOFT_RESET (PAD_L2 | PAD_R2 | PAD_L1 | PAD_R1 | PAD_SELECT | PAD_START)
#define PAD_ANY_BUTTON (PAD_TRIANGLE | PAD_CIRCLE | PAD_CROSS | PAD_SQUARE)

static Yamada diskchange_pics[] = {
    {LBA_MOVIE_DISK1, 28051},    // MOVIE/DISK1.LZS
    {LBA_MOVIE_DISK2, 28391},    // MOVIE/DISK2.LZS
    {LBA_MOVIE_DISK3, 28477},    // MOVIE/DISK3.LZS
    {LBA_MOVIE_CHANGE0, 145964}, // MOVIE/CHANGE0.LZS
    {LBA_MOVIE_CHANGE1, 160255}, // MOVIE/CHANGE1.LZS
    {LBA_MOVIE_CHANGE2, 112987}, // MOVIE/CHANGE2.LZS
    {LBA_MOVIE_CHANGE3, 134482}, // MOVIE/CHANGE3.LZS
    {LBA_MOVIE_CHANGE4, 141402}, // MOVIE/CHANGE4.LZS
    {LBA_MOVIE_CHANGE5, 192713}, // MOVIE/CHANGE5.LZS
    {LBA_MOVIE_CHANGE6, 154508}, // MOVIE/CHANGE6.LZS
    {LBA_MOVIE_CHANGE7, 133281}, // MOVIE/CHANGE7.LZS
    {LBA_MOVIE_CHANGE8, 150725}, // MOVIE/CHANGE8.LZS
};

static s32 db_index;
static DRAWENV db_drawenv[2];
static DISPENV db_dispenv[2];
static DISPENV* cdb_dispenv;
static DRAWENV* cdb_drawenv;
static u32 pad0_released;
static u32 pad1_released;
static u32 pad0_raw;
static u32 pad1_raw;
static u32 pad0_pressed;
static u32 pad1_pressed;
static OT_TYPE db_ot[2];
static TILE db_fadeTile[2];
static DR_MODE db_drMode[2];
static u8 D_800A1724[0x50]; // unreferenced

void func_80034104(void);
s32 func_80034150(void);

static inline s32 DsChangeStartFrame(void) {
    u32 pad0;
    u32 pad1;
    u32 old0;
    u32 old1;

    DrawSync(0);
    VSync(0);
    db_index = db_index == 0;
    PutDispEnv(&db_dispenv[db_index]);
    PutDrawEnv(&db_drawenv[db_index]);
    cdb_dispenv = &db_dispenv[db_index];
    cdb_drawenv = &db_drawenv[db_index];

    pad0 = InputReadPadsRaw();
    old0 = pad0_released;
    old1 = pad1_released;
    pad1 = pad0 >> 16;

    pad0_raw = pad0;
    pad1_raw = pad1;
    pad0_released = ~pad0;
    pad1_released = ~pad1;
    pad0_pressed = old0 & pad0;
    pad1_pressed = old1 & pad1;

    return db_index;
}

static inline void DsChangeInitGraph(s32 h) {
    RECT rect; // never used, but it is still part of the stack frame
    s32 y;
    s32 res;

    y = (h != 480) ? 240 : 0;
#ifndef PLATFORM_PSYZ
    while ((res = BreakDraw()) == -1) {
        VSync(0);
    }
    if (res != 0) {
        do {
        } while (IsIdleGPU(1) != 0);
    }
#endif
    ResetGraph(1);
    SetDefDrawEnv(&db_drawenv[0], 0, 0, 640, h);
    SetDefDispEnv(&db_dispenv[0], 0, y, 640, h);
    SetDefDrawEnv(&db_drawenv[1], 0, y, 640, h);
    SetDefDispEnv(&db_dispenv[1], 0, 0, 640, h);
    db_dispenv[1].isrgb24 = 0;
    db_dispenv[0].isrgb24 = 0;
    db_dispenv[0].isinter = db_dispenv[1].isinter = 1;
    db_drawenv[1].isbg = 0;
    db_drawenv[0].isbg = 0;
    db_drawenv[1].dfe = 0;
    db_drawenv[0].dfe = 0;
    db_drawenv[1].dtd = 0;
    db_drawenv[0].dtd = 0;
    db_drawenv[1].tpage = 0;
    db_drawenv[0].tpage = 0;
    db_drawenv[0].r0 = 0;
    db_drawenv[0].g0 = 0;
    db_drawenv[0].b0 = 0;
    db_drawenv[1].r0 = 0;
    db_drawenv[1].g0 = 0;
    db_drawenv[1].b0 = 0;
    db_index = 0;
}

s32 DSCHANGE_WaitDiskLoop(s32 diskNo) {
    RECT rect;
    u8 cdResult[8];
    s32 ode;
    s32 buf;
    s32 skip;
    s16 fade;
    s32 i;
    s16 n;
    u8* src;

    i = (diskNo - 1) * 2;
    SysCdromLoadLzs(((s32*)diskchange_pics)[i], ((s32*)diskchange_pics)[i + 1], (u_long*)0x800B0000, NULL);
    i = VSync(-1) % 9 * 2 + 6; // skip the 3 disk images
    SysCdromStartLoadLzs(((s32*)diskchange_pics)[i], ((s32*)diskchange_pics)[i + 1], (u_long*)0x800C8000, NULL);

    SetDispMask(0);
    DsChangeInitGraph(480);
    rect.x = 0;
    rect.y = 0;
    rect.w = 640;
    rect.h = 480;
    ClearImage(&rect, 0, 0, 0);
    DsChangeStartFrame();
    while (SystemCdromReadChain() != 0) {
    }
    DsChangeStartFrame();
    SetDispMask(1);

    while (1) {
        for (fade = 0xFF; fade >= 0; fade -= 4) {
            buf = DsChangeStartFrame();
            ode = GetODE() ^ 1;
            src = (u8*)((ode * 0x320) + 0x800B0000);
            setRECT(&rect, 0x78, 0x154, 0x190, 1);
            rect.y = ode + 0x154;
            while (rect.y < 0x1CC) {
                LoadImage(&rect, (u_long*)src);
                rect.y += 2;
                src += 0x640;
            }
            src = (u8*)((ode * 0x320) + 0x800C8000);
            setRECT(&rect, 0x78, 0x20, 0x190, 1);
            rect.y = ode + 0x20;
            while (rect.y < 0x14C) {
                LoadImage(&rect, (u_long*)src);
                rect.y += 2;
                src += 0x640;
            }
            ClearOTagR(&db_ot[buf], 1);
            SetTile(&db_fadeTile[buf]);
            SetSemiTrans(&db_fadeTile[buf], 1);
            (&db_fadeTile[buf])->r0 = fade;
            (&db_fadeTile[buf])->g0 = fade;
            (&db_fadeTile[buf])->b0 = fade;
            (&db_fadeTile[buf])->x0 = 0x78;
            (&db_fadeTile[buf])->y0 = 0x20;
            (&db_fadeTile[buf])->w = 0x190;
            (&db_fadeTile[buf])->h = 0x1C0;
            AddPrim(&db_ot[buf], &db_fadeTile[buf]);
            SetDrawMode(&db_drMode[buf], 0, 0, GetTPage(2, 2, 0, 0), NULL);
            AddPrim(&db_ot[buf], &db_drMode[buf]);
            DrawOTag(&db_ot[buf]);
        }

        skip = 0;
        func_80034104();
        do {
            DsChangeStartFrame();
            if (pad0_raw == PAD_SOFT_RESET) {
                skip = 1;
                goto fade_out;
            }
            if (pad0_pressed & PAD_ANY_BUTTON) {
                goto fade_out;
            }
            CdControlB(CdlNop, NULL, cdResult);
        } while (!(cdResult[0] & CdlStatShellOpen));
        do {
            DsChangeStartFrame();
            if (pad0_raw == PAD_SOFT_RESET) {
                skip = 1;
                break;
            }
            if (pad0_pressed & PAD_ANY_BUTTON) {
                break;
            }
            CdControlB(CdlNop, NULL, cdResult);
        } while (cdResult[0] & CdlStatShellOpen);

    fade_out:
        for (n = 0x40; n != 0; n--) {
            buf = DsChangeStartFrame();
            ClearOTagR(&db_ot[buf], 1);
            SetTile(&db_fadeTile[buf]);
            SetSemiTrans(&db_fadeTile[buf], 1);
            (&db_fadeTile[buf])->r0 = 8;
            (&db_fadeTile[buf])->g0 = 8;
            (&db_fadeTile[buf])->b0 = 8;
            (&db_fadeTile[buf])->x0 = 0;
            (&db_fadeTile[buf])->y0 = 0;
            (&db_fadeTile[buf])->w = 640;
            (&db_fadeTile[buf])->h = 480;
            AddPrim(&db_ot[buf], &db_fadeTile[buf]);
            SetDrawMode(&db_drMode[buf], 0, 0, GetTPage(2, 2, 0, 0), NULL);
            AddPrim(&db_ot[buf], &db_drMode[buf]);
            DrawOTag(&db_ot[buf]);
        }

        if (func_80034150() != 0) {
            continue;
        }
        if (skip == 1) {
            return 1;
        }
        if (SYS_GetDiskNo() == diskNo) {
            return 0;
        }
    }
}

void DSCHANGE_GameOver(void) {
    RECT rect;
    s32 ode;
    s32 buf;
    s16 fade;
    u8* src;

    SetDispMask(0);
    DsChangeInitGraph(480);
    SysCdromLoadFile(LBA_MOVIE_OVER2, 0x800, (u_long*)0x800B0000, NULL);
    g_AkaoCmd.opcode = AKAO_PLAY_MUSIC;
    g_AkaoCmd.params[0] = 0x800B0000;
    AkaoExec();
    SysCdromLoadLzs(LBA_MOVIE_OVER, 0x40000, (u_long*)0x800B0000, NULL);
    rect.w = 640;
    rect.x = 0;
    rect.y = 0;
    rect.h = 480;
    ClearImage(&rect, 0, 0, 0);
    DsChangeStartFrame();
    while (SystemCdromReadChain() != 0) {
    }
    SetDispMask(1);

    for (fade = 0xFF; fade >= 0; fade -= 4) {
        buf = DsChangeStartFrame();
        ode = GetODE() ^ 1;
        src = (u8*)((ode * 0x480) + 0x800B0000);
        setRECT(&rect, 0x20, 0x20, 0x240, 1);
        rect.y = ode + 0x20;
        while (rect.y < 0x1C0) {
            LoadImage(&rect, (u_long*)src);
            rect.y += 2;
            src += 0x900;
        }
        ClearOTagR(&db_ot[buf], 1);
        SetTile(&db_fadeTile[buf]);
        SetSemiTrans(&db_fadeTile[buf], 1);
        (&db_fadeTile[buf])->r0 = fade;
        (&db_fadeTile[buf])->g0 = fade;
        (&db_fadeTile[buf])->b0 = fade;
        (&db_fadeTile[buf])->x0 = 0x20;
        (&db_fadeTile[buf])->y0 = 0x20;
        (&db_fadeTile[buf])->w = 0x240;
        (&db_fadeTile[buf])->h = 0x1A0;
        AddPrim(&db_ot[buf], &db_fadeTile[buf]);
        SetDrawMode(&db_drMode[buf], 0, 0, GetTPage(2, 2, 0, 0), NULL);
        AddPrim(&db_ot[buf], &db_drMode[buf]);
        DrawOTag(&db_ot[buf]);
        if (pad0_pressed & (PAD_ANY_BUTTON | PAD_SELECT | PAD_START)) {
            break;
        }
    }
    while (!(pad0_pressed & (PAD_ANY_BUTTON | PAD_SELECT | PAD_START))) {
        DsChangeStartFrame();
    }

    g_AkaoCmd.opcode = AKAO_VOL_SLIDE_FROM_CURR;
    g_AkaoCmd.params[0] = 0x3C;
    g_AkaoCmd.params[1] = 0;
    AkaoExec();
    for (fade = 0x40; fade != 0; fade--) {
        buf = DsChangeStartFrame();
        ClearOTagR(&db_ot[buf], 1);
        SetTile(&db_fadeTile[buf]);
        SetSemiTrans(&db_fadeTile[buf], 1);
        (&db_fadeTile[buf])->r0 = 8;
        (&db_fadeTile[buf])->g0 = 8;
        (&db_fadeTile[buf])->b0 = 8;
        (&db_fadeTile[buf])->x0 = 0;
        (&db_fadeTile[buf])->y0 = 0;
        (&db_fadeTile[buf])->w = 640;
        (&db_fadeTile[buf])->h = 480;
        AddPrim(&db_ot[buf], &db_fadeTile[buf]);
        SetDrawMode(&db_drMode[buf], 0, 0, GetTPage(2, 2, 0, 0), NULL);
        AddPrim(&db_ot[buf], &db_drMode[buf]);
        DrawOTag(&db_ot[buf]);
    }
}
