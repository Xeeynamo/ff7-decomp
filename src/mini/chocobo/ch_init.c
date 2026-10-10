//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <game.h>
#include <libgte.h>
#include <psxsdk/inline_c.h>
#include "chocobo_private.h"
#include <libetc.h>

typedef struct {
    u32 unk0;
    u32 unk4;
} UnkRectData;

extern POLY_FT4 D_800B1298;
extern POLY_FT4 D_800B12C0;
extern s32 D_800F5030;
extern s16 D_800F5038;
extern s16 D_800F503A;
extern s16 D_800F503C;

const RECT D_800A0018 = {0, 0, 320, 480};
const UnkRectData D_800A0020 = {0x00000180, 0x01000100};
const UnkRectData D_800A0028 = {0x01000140, 0x01000180};

static void ChocoboLoadTrackData(s32 arg0, s32 arg1);
static void ChocoboLoadTextures(void);

void ChocoboRaceInit(void) {
    ChocoboTrack* track;
    u32 pad;

    track = (ChocoboTrack*)0x80110000;
    D_800F5078.track = track;
    D_800B7514 = 0xFF;
    *(u32*)&D_800F5040.fadeSpeed = -0x10;
    D_800B759C = -1;
    D_800B74FC = track->count;
    D_800B7478 = 0;
    D_800B7500 = track->segments;
    D_800B747C = -((Savemap.memory_bank_1[0] + (Savemap.memory_bank_1[1] << 8)) >= 1000);
    pad = InputReadPadsRaw() >> 16;
    if ((pad & (PAD_TRIANGLE | PAD_CROSS | PAD_SQUARE | PAD_CIRCLE)) ==
        (PAD_TRIANGLE | PAD_CROSS | PAD_SQUARE | PAD_CIRCLE)) {
        if (pad & PAD_R1) {
            Savemap.memory_bank_3[6] = 1;
        }
        if (pad & PAD_R2) {
            Savemap.memory_bank_3[6] = 2;
        }
    }
    if (Savemap.memory_bank_3[8]) {
        Savemap.memory_bank_3[9] = 0xFF;
    }
    D_800B7530.unk8 = 0;
    D_800B74F8 = 0;
    D_800B7530.unkC = 0;
    D_800F5040.unkC = -1;
    D_800F5078.unk14 = -1;
    D_800F5040.unk10 = (D_800F5078.unk20 + 3) * 2;
    D_800F5078.unk20 = Savemap.memory_bank_3[23];
    if (Savemap.memory_bank_3[9]) {
        D_800B7A48.unk0 = -1;
    } else {
        D_800B7A48.unk0 = 0;
    }
    D_800F5124 = 1;
    D_800F5078.unk10 = -1;
    D_800B7594 = -1;
    D_800F5040.unk0 = 0;
    D_800F5040.unk8 = 0;
    D_800B7A48.unk1C = 0;
    D_800F5040.unk4 = 0;
    D_800B75CC[0].unk94 = 30;
    D_800B7530.unkC = 0;
}

void ChocoboInitMusic(void) {
    g_AkaoCmd.opcode = AKAO_PLAY_FOUR_SOUNDS;
    g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
    g_AkaoCmd.params[4] = 0;
    g_AkaoCmd.params[3] = 0;
    g_AkaoCmd.params[2] = 0;
    g_AkaoCmd.params[1] = 0;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT2;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT1;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT0;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT3;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
}

void func_800A18BC(void) {
    RECT rect;
    s32* ptr;
    POLY_F4* p0;
    POLY_F4* p1;
    s32 i;

    rect = D_800A0018;
    ptr = (s32*)&D_800B7530;
    for (;;) {
        if (ptr == &D_800F5030) {
            break;
        }
        *ptr = 0;
        ptr++;
    }
    SetGeomOffset(160, 120);
    SetGeomScreen(270);
    SetDispMask(1);
    SetDefDrawEnv(&D_800B7A68[0].draw, 0, 0, 320, 232);
    SetDefDrawEnv(&D_800B7A68[1].draw, 0, 240, 320, 232);
    D_800B7A68[1].draw.tpage = D_800B7A68[0].draw.tpage = GetTPage(0, 1, 0, 0);
    D_800B7A68[1].draw.isbg = D_800B7A68[0].draw.isbg = 0;
    SetDefDispEnv(&D_800B7A68[0].disp, 0, 240, 320, 232);
    SetDefDispEnv(&D_800B7A68[1].disp, 0, 0, 320, 232);
    D_800B7A68[0].draw.dfe = D_800B7A68[1].draw.dfe = 1;
    ClearOTagR(D_800B7A68[0].ot, LEN(D_800B7A68[0].ot));
    ClearOTagR(D_800B7A68[1].ot, LEN(D_800B7A68[1].ot));
    ClearOTag(D_800B7A68[0].ot2, LEN(D_800B7A68[0].ot2));
    ClearOTag(D_800B7A68[1].ot2, LEN(D_800B7A68[1].ot2));
    ClearImage(&rect, 0, 0, 0);
    SetBackColor(64, 64, 64);
    SetDispMask(1);
    PutDispEnv(&D_800B7A68[0].disp);
    PutDrawEnv(&D_800B7A68[1].draw);
    srand(VSync(-1));
    ChocoboRaceInit();
    D_800F5038 = 0;
    D_800F503A = 0;
    D_800F503C = 0;
    ChocoboLoadTextures();
    ChocoboLoadTrackData(Savemap.memory_bank_3[7], Savemap.memory_bank_3[0x17]);
    D_800F5040.event = *D_800F5078.track->events;
    if (D_800B7A48.unk0) {
        D_800F5040.event.type = -1;
    }
    D_800F5040.event.type = -1;
    p0 = D_800B7A68[0].unk1C6B0;
    p1 = D_800B7A68[1].unk1C6B0;
    for (i = 0; i < NUM_CHOCOBO; i++) {
        SetPolyF4(&p0[i]);
        SetPolyF4(&p1[i]);
        p0[i].r0 = (i & 2) ? 255 : 0;
        p0[i].g0 = (i & 4) ? 255 : 0;
        p0[i].b0 = (i & 1) ? 255 : 0;
        p1[i].r0 = (i & 2) ? 255 : 0;
        p1[i].g0 = (i & 4) ? 255 : 0;
        p1[i].b0 = (i & 1) ? 255 : 0;
    }
    D_800B7A68[0].bg.x0 = D_800B7A68[0].bg.x2 = D_800B7A68[0].bg.y0 = D_800B7A68[0].bg.y1 = 0;
    D_800B7A68[0].bg.x1 = D_800B7A68[0].bg.x3 = 320;
    D_800B7A68[0].bg.y2 = D_800B7A68[0].bg.y3 = 232;
    SetPolyF4(&D_800B7A68[0].bg);
    SetSemiTrans(&D_800B7A68[0].bg, 1);
    D_800B7A68[1].bg = D_800B7A68[0].bg;

    D_800B7A68[0].unk1C740 = D_800B1298;
    SetPolyFT4(&D_800B7A68[0].unk1C740);
    SetSemiTrans(&D_800B7A68[0].unk1C740, 1);
    D_800B7A68[0].unk1C740.clut = GetClut(576, 128);
    D_800B7A68[0].unk1C740.tpage = GetTPage(0, 0, 384, 0);
    D_800B7A68[1].unk1C740 = D_800B7A68[0].unk1C740;

    D_800B7A68[0].unk1C7C0 = D_800B12C0;
    SetPolyFT4(&D_800B7A68[0].unk1C7C0);
    SetSemiTrans(&D_800B7A68[0].unk1C7C0, 1);
    D_800B7A68[0].unk1C7C0.clut = GetClut(576, 129);
    D_800B7A68[0].unk1C7C0.tpage = GetTPage(0, 0, 384, 0);
    D_800B7A68[1].unk1C7C0 = D_800B7A68[0].unk1C7C0;

    SetPolyF4(&D_800B7A68[0].unk1C780);
    D_800B7A68[0].unk1C780.x0 = D_800B7A68[0].unk1C780.x2 = 25;
    D_800B7A68[0].unk1C780.x1 = D_800B7A68[0].unk1C780.x3 = 30;
    D_800B7A68[0].unk1C780.y2 = D_800B7A68[0].unk1C780.y3 = 209;
    D_800B7A68[1].unk1C780 = D_800B7A68[0].unk1C780;
    func_800A1F40(D_800B1254.unk0, Savemap.memory_bank_3[7]);
    func_800A9828();
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/ch_init", func_800A1F40);

static void ChocoboLoadTrackData(s32 arg0, s32 arg1) {
    RECT sp10;
    RECT sp18;
    s32 var_a0;
    u32 var_a1;

    sp10 = *(RECT*)&D_800A0020;
    sp18 = *(RECT*)&D_800A0028;

    if (arg0 != 0) {
        SysCdromStartLoadLzs(0x3C1, 0x20000, (u_long*)0x80110000, 0);
    } else {
        SysCdromStartLoadLzs(0x32C, 0x20000, (u_long*)0x80110000, 0);
    }

    while (SystemCdromReadChain()) {
    }

    LoadImage(&sp10, (u_long*)0x80110000);
    DrawSync(0);

    if (arg0 != 0) {
        SysCdromStartLoadLzs(0x3F1, 0x1E000, (u_long*)0x80190000, 0);
    } else {
        SysCdromStartLoadLzs(0x3CE, 0x1E800, (u_long*)0x80190000, 0);
    }

    while (SystemCdromReadChain()) {
    }

    switch (arg1) {
    case 0:
        SysCdromStartLoadLzs(0x459, 0x30000, (u_long*)0x80110000, 0);
        break;
    case 1:
        SysCdromStartLoadLzs(0x433, 0x30000, (u_long*)0x80110000, 0);
        break;
    case 2:
        SysCdromStartLoadLzs(0x417, 0x30000, (u_long*)0x80110000, 0);
        break;
    case 3:
        SysCdromStartLoadLzs(0x49C, 0x30000, (u_long*)0x80110000, 0);
        break;
    }

    while (SystemCdromReadChain()) {
    }

    LoadImage(&sp18, (u_long*)0x80110000);
    DrawSync(0);

    if (arg0) {
        SysCdromStartLoadLzs(0x33E, 0x6A000, (u_long*)0x80110000, 0);
    } else {
        SysCdromStartLoadLzs(0x293, 0x7D000, (u_long*)0x80110000, 0);
    }

    while (SystemCdromReadChain()) {
    }
}

static void ChocoboLoadTextures(void) {
    SysCdromStartLoadLzs(0x4C9, 0x1000, (u_long*)&D_80077F64[0][0x2000], NULL);
    do {

    } while (SystemCdromReadChain());
    SysCdromStartLoadLzs(0x4CA, 0x1000, (u_long*)&D_80077F64[0][0x3000], NULL);
    do {

    } while (SystemCdromReadChain());
    SysCdromStartLoadLzs(0x4C8, 0x800U, (u_long*)&D_80077F64[1][0xC00], NULL);
    do {

    } while (SystemCdromReadChain());
    SysCdromStartLoadLzs(0x4C7, 0x800U, (u_long*)&D_80077F64[1][0x1400], NULL);
    do {

    } while (SystemCdromReadChain());
}
