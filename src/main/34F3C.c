//! PSYQ=3.3
#include <game.h>
#include <libcd.h>
#include <libetc.h>
#include <libpress.h>
#include "cd.h"

typedef struct {
    /* 0x00 */ s32 sector;
    /* 0x04 */ u32 size;
    /* 0x08 */ u16 x;
    /* 0x0A */ u16 y;
    /* 0x0C */ union {
        struct {
            u16 left;
            u16 right;
        } volume;
        struct {
            u16 width;
            u16 height;
        } size;
    } params;
    /* 0x10 */ u16 frames;
    /* 0x12 */ u16 mode;
} Unk8009A1F4; // size:0x14

typedef struct {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 width;
    /* 0x0C */ s32 height;
    /* 0x10 */ u32 frames;
    /* 0x14 */ u_long* buffers[2];
    /* 0x1C */ u_long* image;
    /* 0x20 */ s32 bufferIndex;
    /* 0x24 */ s32 finished;
    /* 0x28 */ s32 rgb24;
    /* 0x2C */ RECT slice;
    /* 0x34 */ RECT rect;
    /* 0x3C */ u16 mode;
} Unk80095D88; // size:0x40

typedef union {
    u32 words[10];
    FieldCamera camera;
} Unk80077F3C; // size:0x28

extern u_long* D_8006E0FC;
extern s32 D_8006E100;
extern s32 D_8006E104;
extern s32 D_8006E108;
extern s32 D_8006E10C;
extern u32 D_8006E110;
extern u32 D_8006E114;
extern s32 D_8006E118;
extern Unk80075D00 D_80075DF0;
extern Unk80077F3C D_80077F3C;
extern Unk80095D88 D_80095D88;
extern s32 D_800965E4;
extern s32 D_8009A060;
extern Unk8009A1F4 D_8009A1F4[];
extern s32 StCdIntrFlag;

void func_80033C20(s32 left, s32 leftToRight, s32 right, s32 rightToLeft);
void StCdInterrupt(void);
void func_80035CF0(CdlLOC* loc);
void func_80035D64(void);
u_long* func_80035DC8(void);
s32 func_80035F14(void);
void func_80036038(void);
void func_80036100(void);
void func_80036190(void);

void func_80034F3C(void) { DecDCTReset(0); }

void SysMovieLoadMovieSettings(void) {
    CdlFILE file;
    s32 status;

    do {
        while (!CdSearchFile(&file, "\\MINT\\MOVIE_ID.BIN;1")) {
        }
        CdControlB(CdlSetloc, (u8*)&file.pos, NULL);
        CdRead(1, (u_long*)D_8009A1F4, CdlModeSpeed);
        do {
            status = CdReadSync(1, NULL);
        } while (status > 0);
    } while (status);
}

void* SysMoviePlay(void* buffer, s32 movie) {
    u16 frames;

    if (D_80071A60.op) {
        return NULL;
    }
    g_AkaoCmd.params[0] = 30;
    g_AkaoCmd.params[1] = 0x7FFF;
    if (D_80071A60.disc == 1) {
        if (movie == 0x26) {
            g_AkaoCmd.params[1] = 0;
        } else if (movie == 0x31) {
            g_AkaoCmd.params[1] = 0x1800;
        }
    }
    g_AkaoCmd.opcode = AKAO_CD_VOL_SLIDE_FROM_CURR;
    AkaoExec();
    DecDCTReset(1);
    D_80095D88.bufferIndex = 0;
    D_80095D88.rgb24 = 0;
    D_80095D88.finished = 0;
    D_80095D88.mode = D_8009A1F4[movie].mode;
    D_80075DF0.unk0 = 0;
    D_80075DF0.unk10 = 320;
    D_80075DF0.unk12 = 224;
    D_80075D00 = &D_80075DF0;
    D_8006E118 = 0;
    D_80075DF0.unk2 = 0;
    D_80075DF0.unk4 = 0;
    D_80075DF0.unk6 = 0;
    D_80075DF0.unkC = 0;
    D_80075DF0.unk14 = 0;
    D_80075DF0.unk18 = 0;
    D_80075DF0.unk8 = 0;
    D_8006E110 = 1;
    D_8006E114 = 0;
    switch (D_80095D88.mode) {
    case 1:
        return NULL;
    case 0:
        D_80095D88.x = D_8009A1F4[movie].x;
        D_80095D88.y = D_8009A1F4[movie].y;
        frames = D_8009A1F4[movie].frames;
        D_80095D88.buffers[0] = (u_long*)buffer;
        buffer += 0x19B00;
        D_80095D88.buffers[1] = (u_long*)buffer;
        buffer += 0x19B00;
        D_80095D88.image = (u_long*)buffer;
        buffer += 0x2D00;
        D_80095D88.width = 320;
        D_80095D88.height = 224;
        D_80095D88.frames = frames;
        StSetRing((u_long*)buffer, 36);
        buffer += 0x12000;
        g_MovieCamera = &D_80077F3C.camera;
        DecDCToutCallback(func_80036038);
        StSetStream(1, 0, -1, NULL, NULL);
        func_80033DAC(D_8009A1F4[movie].sector, func_80035D64);
        func_80033C20(D_8009A1F4[movie].params.volume.left, 0, D_8009A1F4[movie].params.volume.right, 0);
        D_800965E4 = 0;
        D_8009A060 = 0;
        break;
    case 2:
    case 3:
        D_80095D88.x = D_8009A1F4[movie].x;
        D_80095D88.y = D_8009A1F4[movie].y;
        D_80095D88.frames = D_8009A1F4[movie].frames;
        D_80095D88.width = D_8009A1F4[movie].params.size.width;
        D_80095D88.height = D_8009A1F4[movie].params.size.height;
        D_8006E0FC = (u_long*)buffer;
        buffer += ((D_8009A1F4[movie].size + 3) >> 2) * 4;
        D_80095D88.buffers[0] = (u_long*)buffer;
        buffer += 0xC800;
        D_80095D88.buffers[1] = (u_long*)buffer;
        buffer += 0xC800;
        D_80095D88.image = (u_long*)buffer;
        buffer += 0x1C00;
        DecDCToutCallback(func_80036190);
        func_80033C20(255, 0, 255, 0);
        StSetStream(0, 0, -1, NULL, NULL);
        SystemLoadFileBySector(D_8009A1F4[movie].sector, D_8009A1F4[movie].size, D_8006E0FC, func_80035D64);
        D_800965E4 = 1;
        D_8009A060 = 1;
        break;
    }
    return buffer;
}

void CdOpMovieBuffer(void) {
    s16 free;
    s16 filled;

    if (D_80071A60.op != CDOP_MOVIE_BUFFER) {
        return;
    }
    if (!D_80095D88.mode) {
        StRingStatus(&free, &filled);
        if (filled < 8) {
            return;
        }
    }
    D_80071A60.op = CDOP_MOVIE_READY;
    D_8007EBD8->isrgb24 = 0;
    D_8006E108 = 0;
    D_8006E104 = 0;
    D_8006E100 = 0;
    D_80095D88.rgb24 = 0;
}

s32 func_800354CC(void) {
    u32 i;
    u16 rgb24;
    u_long* data;
    u32* camera;

    if (D_80071A60.op == CDOP_MOVIE_READY) {
        if (!D_80095D88.mode) {
            i = 0x800000;
            while (!(data = func_80035DC8())) {
                if (!--i) {
                    return 0;
                }
            }
            for (i = 0, camera = D_80077F3C.words; i < LEN(D_80077F3C.words); i++) {
                *camera++ = *data++;
            }
            if (D_8006E108) {
                D_8006E108 = 0;
                D_80095D88.rgb24 = D_8006E104;
            } else if (D_8006E100 != (rgb24 = D_80077F3C.camera.unk)) {
                D_8006E100 = rgb24;
                D_8006E104 = rgb24;
                D_8006E108 = 1;
            }
            DecDCTvlc(data, D_80095D88.buffers[D_80095D88.bufferIndex]);
            StFreeRing(data);
        } else {
            DecDCTvlc(SysCdromGetPackPointer(D_8006E0FC, 0), D_80095D88.buffers[D_80095D88.bufferIndex]);
        }
        D_80071A60.op = CDOP_MOVIE_PLAY;
        D_8006E10C = 0;
    }
    return D_80071A60.op;
}

inline void SysMovieAbortPlay(void) {
    RECT rect;

    if ((u32)D_80071A60.op > CDOP_MOVIE_READY) {
        return;
    }
    if ((u32)D_80071A60.op < CDOP_MOVIE_PLAY) {
        return;
    }
    CdControl(CdlPause, NULL, NULL);
    if (!D_80095D88.mode) {
        StUnSetRing();
    }
    DecDCToutCallback(NULL);
    D_80071A60.op = CDOP_IDLE;
    D_800965E4 = 0;
    D_8009A060 = 0;
    if (D_8007EBD8->isrgb24) {
        rect.x = 0;
        rect.y = 0;
        rect.w = D_8007EBD0->clip.w * 3 / 2;
        rect.h = 480;
        ClearImage(&rect, 0, 0, 0);
        DrawSync(0);
    }
}

void CdOpMoviePlay(void) {
    CdlLOC loc;
    u16 x;
    u16 y;
    s16 right;

    D_8007EBD8->isrgb24 = D_8006E104;
    x = D_8007EBD0->clip.x;
    D_80095D88.rect.x = x;
    y = D_8007EBD0->clip.y;
    D_80095D88.rect.y = y;
    right = (u16)D_8007EBD0->clip.x + D_80095D88.width;
    D_80095D88.rect.w = right;
    D_80095D88.rect.h = (u16)D_8007EBD0->clip.y + D_80095D88.height;
    D_80095D88.slice.x = x + D_80095D88.x;
    D_80095D88.slice.y = y + D_80095D88.y;
    D_80095D88.slice.h = D_80095D88.height;
    if (!D_80095D88.rgb24) {
        D_80095D88.slice.w = 16;
    } else {
        D_80095D88.slice.w = 24;
        D_80095D88.rect.w = right * 3 / 2;
    }
    switch (D_80095D88.mode) {
    case 0:
        DecDCTin(D_80095D88.buffers[D_80095D88.bufferIndex], D_80095D88.rgb24);
        DecDCTout(D_80095D88.image, D_80095D88.slice.w * D_80095D88.slice.h / 2);
        while (!func_80035F14()) {
            StSetMask(1, StGetBackloc(&loc), -1);
            func_80035CF0(&loc);
        }
        func_80036100();
        if (D_80095D88.finished == 1) {
            SysMovieAbortPlay();
        }
        if (!D_8006E10C && D_80075D00->unk8 >= D_80095D88.frames - 15) {
            g_AkaoCmd.opcode = AKAO_CD_VOL_SLIDE_FROM_CURR;
            g_AkaoCmd.params[0] = 30;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
            D_8006E10C = 1;
        }
        break;
    case 2:
        DecDCTin(D_80095D88.buffers[D_80095D88.bufferIndex], 2);
        DecDCTout(D_80095D88.image, D_80095D88.slice.w * D_80095D88.slice.h / 2);
        D_80095D88.bufferIndex = !D_80095D88.bufferIndex;
        DecDCTvlc(SysCdromGetPackPointer(D_8006E0FC, D_8006E110), D_80095D88.buffers[D_80095D88.bufferIndex]);
        if (D_8006E110++ >= D_80095D88.frames) {
            D_8006E110 = 0;
        }
        func_80036100();
        break;
    case 3:
        DecDCTin(D_80095D88.buffers[D_80095D88.bufferIndex], 2);
        DecDCTout(D_80095D88.image, D_80095D88.slice.w * D_80095D88.slice.h / 2);
        D_80095D88.bufferIndex = !D_80095D88.bufferIndex;
        DecDCTvlc(SysCdromGetPackPointer(D_8006E0FC, D_8006E110), D_80095D88.buffers[D_80095D88.bufferIndex]);
        if (D_8006E110++ >= D_80095D88.frames) {
            SysMovieAbortPlay();
        }
        func_80036100();
        break;
    }
}

void func_80035CF0(CdlLOC* loc) {
    u8 mode = CdlModeSpeed;

    do {
        while (!CdControl(CdlSetloc, (u8*)loc, NULL)) {
        }
        while (!CdControl(CdlSetmode, &mode, NULL)) {
        }
        VSync(3);
    } while (!CdRead2(CdlModeStream | CdlModeSpeed | CdlModeRT | CdlModeSize1));
}

void func_80035D64(void) {
    D_80071A60.op = CDOP_MOVIE_BUFFER;
    if (!D_80095D88.mode) {
        while (!CdRead2(CdlModeStream | CdlModeSpeed | CdlModeRT | CdlModeSize1)) {
            while (!CdControl(CdlSetloc, (u8*)&D_80071A60.loc, NULL)) {
            }
        }
    }
}

u_long* func_80035DC8(void) {
    u_long* data;
    s32 retries = 0x800000;
    s32 width;

    while (StGetNext(&data, (u_long**)&D_80075D00)) {
        if (!--retries) {
            return NULL;
        }
    }
    if (D_80075D00->unk8 >= D_80095D88.frames) {
        D_80095D88.finished = 1;
    }
    if (D_8006E114 >= D_80075D00->unk8) {
        D_80095D88.finished = 1;
    }
    width = D_80075D00->unk10;
    if (D_80095D88.width != width || D_80095D88.height != D_80075D00->unk12) {
        D_80095D88.width = width;
        D_80095D88.height = D_80075D00->unk12;
        if (!D_80095D88.rgb24) {
            D_80095D88.rect.w = D_80095D88.width;
            D_80095D88.rect.h = D_80095D88.height;
            D_80095D88.slice.h = D_80095D88.height;
        } else {
            D_80095D88.rect.w = width * 3 / 2;
            D_80095D88.rect.h = D_80095D88.height;
            D_80095D88.slice.h = D_80095D88.height;
        }
    }
    D_8006E114 = D_80075D00->unk8;
    return data;
}

s32 func_80035F14(void) {
    u32 i = 0x800000;
    u16 rgb24;
    u_long* data;
    u32* camera;

    while (!(data = func_80035DC8())) {
        if (!--i) {
            return 0;
        }
    }
    for (i = 0, camera = D_80077F3C.words; i < LEN(D_80077F3C.words); i++) {
        *camera++ = *data++;
    }
    if (D_8006E108) {
        D_8006E108 = 0;
        D_80095D88.rgb24 = D_8006E104;
    } else if (D_8006E100 != (rgb24 = D_80077F3C.camera.unk)) {
        D_8006E100 = rgb24;
        D_8006E104 = rgb24;
        D_8006E108 = 1;
    }
    D_80095D88.bufferIndex = !D_80095D88.bufferIndex;
    DecDCTvlc(data, D_80095D88.buffers[D_80095D88.bufferIndex]);
    StFreeRing(data);
    return 1;
}

void func_80036038(void) {
    if (StCdIntrFlag) {
        StCdInterrupt();
        StCdIntrFlag = 0;
    }
    LoadImage(&D_80095D88.slice, D_80095D88.image);
    D_80095D88.slice.x += D_80095D88.slice.w;
    if (D_80095D88.slice.x < D_80095D88.rect.w) {
        DecDCTout(D_80095D88.image, D_80095D88.slice.w * D_80095D88.slice.h / 2);
    } else {
        D_8006E118 = 1;
    }
}

void func_80036100(void) {
    s32 retries;

    if (D_80071A60.op != CDOP_MOVIE_PLAY) {
        return;
    }
    retries = 0x800000;
    while (!D_8006E118) {
        retries--;
        if (!retries) {
            D_8006E118 = 1;
            D_80095D88.slice.x = D_80095D88.rect.x + D_80095D88.x;
            D_80095D88.slice.y = D_80095D88.rect.y + D_80095D88.y;
        }
    }
    D_8006E118 = 0;
}

void func_80036190(void) {
    LoadImage(&D_80095D88.slice, D_80095D88.image);
    D_80095D88.slice.x += D_80095D88.slice.w;
    if (D_80095D88.slice.x < D_80095D88.rect.w) {
        DecDCTout(D_80095D88.image, D_80095D88.slice.w * D_80095D88.slice.h / 2);
    } else {
        D_8006E118 = 1;
    }
}

u8* D_8003623C __attribute__((section(".text"))) = NULL;
u8* D_80036240 __attribute__((section(".text"))) = NULL;

INCLUDE_ASM("asm/us/main/nonmatchings/34F3C", func_80036244);
