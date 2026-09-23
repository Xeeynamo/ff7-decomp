//! PSYQ=3.3
#include <game.h>
#include <libcd.h>
#include <libetc.h>

typedef enum {
    CDOP_IDLE,           // nothing in progress
    CDOP_SEEK,           // send Setloc for a seek-only request
    CDOP_SEEK_WAIT,      // wait for the seek, then COMPLETE
    CDOP_READ_SEEK,      // send Setloc for a raw file read
    CDOP_READ_SEEK_WAIT, // wait for the seek, then READ
    CDOP_READ,           // start CdRead of the whole file
    CDOP_READ_WAIT,      // wait for CdRead, then COMPLETE
    CDOP_STOPPED,        // drive stopped after too many errors, func_80034150 recovers it
    CDOP_MOVIE_PLAY,     // movie is streaming
    CDOP_MOVIE_BUFFER,   // movie stream started, waiting for buffer to fill
    CDOP_MOVIE_READY,    // movie buffered, waiting for playback to start
    CDOP_LZS_SEEK,       // seek to next lzs chunk
    CDOP_LZS_SEEK_WAIT,  // wait for the seek, then LZS_READ
    CDOP_LZS_READ,       // start CdRead of up to 9 sectors
    CDOP_LZS_READ_WAIT,  // wait for CdRead, extract the chunk, then LZS_SEEK or COMPLETE
    CDOP_UNUSED_15,      // never set, handler does nothing
    CDOP_CALLBACK,       // go IDLE and invoke the user callback
    CDOP_COMPLETE,       // request finished, CALLBACK on the next tick
    CDOP_PAUSE_SYNC,     // pauses the drive
    CDOP_PAUSE,          // send Pause to abort the current request
    CDOP_PAUSE_WAIT,     // wait for the pause, then go IDLE and invoke the callback
} CdOp;

extern CdlATV D_800698E4; // CD audio volume
extern s32 g_Channel1Config;
extern u8* D_80034CF0; // lzs extract source
extern int D_800698E8; // sector_no
extern s32 D_800698EC;
extern u8 D_800698F0[0x4800]; // disc buffer
extern int D_8006E0F0;
extern int D_8006E0F4;
extern u32 D_8006E0F8;       // sectors in the current lzs chunk read
extern CdOp D_80071A60;      // current state of the read chain
extern int D_80071A64;       //
extern CdlLOC D_80071A68;    // cd sector
extern size_t D_80071A6C;    // amount of sectors to read
extern u_long* D_80071A80;   // read content destination
extern void (*D_80071A84)(); // callback

void SystemCdromAbortLoading(void);
static void func_80034CAC(u32 arg0);
s32 func_80034D5C(void);
s32 func_80034150(void);
void func_80034104(void);
static void CdOpComplete(void);
static void CdOpCallback(void);
static void CdOpSeek(void);
static void CdOpSeekWait(void);
static void CdOpReadSeek(void);
static void CdOpReadSeekWait(void);
static void CdOpRead(void);
static void CdOpReadWait(void);
static void CdOpLzsSeek(void);
static void CdOpLzsSeekWait(void);
static void CdOpLzsRead(void);
static void CdOpLzsReadWait(void);
void CdOpMovieBuffer(void);
void CdOpMoviePlay(void);
static s32 ReadDiskNo(void);
void SysMovieLoadMovieSettings(void);

void SysSavemapReset(void) {
    s32 i;
    u8* bank;

    for (i = 0; i < 1280; i++) {
        Savemap.memory_bank_1[i] = 0;
    }

    for (i = 0; i < NUM_PARTY; i++) {
        Savemap.partyID[i] = 0xFF;
        Savemap.memory_bank_2[i + 9] = 0xFF;
    }

    Savemap.phs_visibility_mask = 1; // Only Cloud is visible.
    g_FieldMusicLock = 0;
    g_MovieLock = 0;
    g_BattleLock = 0;
    Savemap.partyID[0] = 0;
    Savemap.memory_bank_2[9] = 0;
    Savemap.memory_bank_4[0x68] = 0xFF; // Start of location name.
    Savemap.memory_bank_1[0x1C] = 0xFF; // Menu visibility, 2 bytes.
    Savemap.memory_bank_1[0x1D] = 0xFF;
    Savemap.time = 0;
    Savemap.countdown_timer_seconds = 0;
    g_FieldState.nFadeRedStart = 0;
    g_FieldState.nFadeGreenStart = 0;
    g_FieldState.nFadeBlueStart = 0;
    g_FieldState.movieCamDisabled = 0;
    g_PartyUpdatedByFieldScript = 0;
}

void SysCdromInit(void) {
    while (!CdInit()) {
    }
    D_80071A60 = CDOP_IDLE;
    CdSetDebug(0);
    func_80034F3C();
#ifndef PLATFORM_PSYZ
    // BUG: CdControlB will read at ptr CdlModeSpeed, not the intended value!
    CdControlB(CdlSetmode, (u8*)CdlModeSpeed, NULL);
#endif
    VSync(3);
    D_80071A64 = ReadDiskNo();
    SysMovieLoadMovieSettings();
}

void func_80033BE0(void) {
    SystemCdromAbortLoading();
    do {

    } while (SystemCdromReadChain() != 0);
    CdFlush();
    CdReset(0);
}

void func_80033C20(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (g_Channel1Config & 1) {
        D_800698E4.val0 = arg0;
        D_800698E4.val1 = arg1;
        D_800698E4.val2 = arg2;
        D_800698E4.val3 = arg3;
    } else {
        D_800698E4.val0 = arg0 / 2;
        D_800698E4.val1 = arg0 / 2;
        D_800698E4.val2 = arg2 / 2;
        D_800698E4.val3 = arg2 / 2;
    }
    CdMix(&D_800698E4);
}

static void SysCdromSetChainParam(int op, int sector, size_t len, u_long* dst, void (*cb)()) {
    s32 nextOp;

    do {
        nextOp = SystemCdromReadChain();
        switch (nextOp) {
        case CDOP_MOVIE_PLAY:
        case CDOP_MOVIE_BUFFER:
        case CDOP_MOVIE_READY:
            SysMovieAbortPlay();
            break;
        case CDOP_PAUSE_SYNC:
            CdControl(CdlPause, NULL, NULL);
            break;
        }
    } while (nextOp);
    CdIntToPos(sector, &D_80071A68);
    D_80071A6C = (len + 0x7FF) / 0x800;
    D_80071A80 = dst;
    D_80071A84 = cb;
    D_80071A60 = op;
}

int func_80033DAC(int sector_no, void (*cb)()) {
    SysCdromSetChainParam(CDOP_SEEK, sector_no, 0, NULL, cb);
    return 0;
}

int func_80033DE4(int sector_no) {
    SysCdromSetChainParam(CDOP_IDLE, sector_no, 0, NULL, NULL);
    do {

    } while (CdControl(CdlSetloc, (u_char*)&D_80071A68, NULL) == 0);
    return 0;
}

int SystemLoadFileBySector(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    SysCdromSetChainParam(CDOP_READ_SEEK, sector_no, size, dst, cb);
    return 0;
}

int SysCdromStartLoadLzs(int sector_no, size_t size, u_long* dst, void (*cb)()) {
    SysCdromSetChainParam(CDOP_LZS_SEEK, sector_no, size, dst, cb);
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
    D_80071A60 = CDOP_PAUSE;
    SystemCdromReadChain();
}

void SystemCdromAbortLoading(void) {
    switch (D_80071A60) {
    case CDOP_IDLE:
    case CDOP_STOPPED:
        return;
    case CDOP_READ:
    case CDOP_READ_WAIT:
    case CDOP_LZS_READ:
    case CDOP_LZS_READ_WAIT:
        CdSyncCallback(0);
        CdReadyCallback(0);
        break;
    case CDOP_MOVIE_PLAY:
    case CDOP_MOVIE_BUFFER:
    case CDOP_MOVIE_READY:
        SysMovieAbortPlay();
        return;
    case CDOP_SEEK:
    case CDOP_SEEK_WAIT:
    case CDOP_READ_SEEK:
    case CDOP_READ_SEEK_WAIT:
    case CDOP_LZS_SEEK:
    case CDOP_LZS_SEEK_WAIT:
    case CDOP_UNUSED_15:
    case CDOP_CALLBACK:
    case CDOP_COMPLETE:
    case CDOP_PAUSE_SYNC:
        break;
    }
    func_80034048();
}

void func_80034104(void) {
    CdControlB(CdlSetmode, NULL, NULL);
    VSync(3);
    CdControlB(CdlStop, NULL, NULL);
    D_80071A60 = CDOP_STOPPED;
}

s32 func_80034150(void) {
    u8 result[8];
    CdlLOC loc;
    s32 i;

    if (D_80071A60 == CDOP_STOPPED) {
        CdControlB(CdlNop, NULL, result);
        if (result[0] & CdlStatShellOpen) {
            return 3;
        }
        i = 600;
        CdControlB(CdlStandby, NULL, NULL);
        do {
            VSync(0);
            if (--i == 0) {
                return 5;
            }
            CdControlB(CdlNop, NULL, result);
        } while (!(result[0] & CdlStatStandby));
        switch (CdDiskReady(0)) {
        case CdlDiskError:
            return 2;
        case CdlComplete:
            break;
        case CdlStatShellOpen:
            return 3;
        default:
            return 1;
        }
        switch (CdGetDiskType()) {
        case CdlOtherFormat:
            return 4;
        case CdlStatNoDisk:
            return 5;
        case CdlCdromFormat:
            break;
        case CdlStatShellOpen:
            return 3;
        default:
            return 1;
        }
        CdIntToPos(LBA_SYSTEM_CNF, &loc);
        CdControlB(CdlSeekL, (u8*)&loc, result);
        if (result[0] & CdlStatError) {
            return 1;
        }
        if (result[1] & 0x40) {
            return 1;
        }
#ifndef PLATFORM_PSYZ
        // BUG: same as SysCdromInit, the mode is passed as a pointer
        CdControlB(CdlSetmode, (u8*)CdlModeSpeed, result);
#endif
        VSync(3);
        D_80071A60 = CDOP_IDLE;
        D_80071A64 = ReadDiskNo();
        switch (D_80071A64) {
        case 0:
            func_80034104();
            return 6;
        case -1:
            func_80034104();
            return 1;
        default:
            SysMovieLoadMovieSettings();
            break;
        }
    }
    return 0;
}

static s32 ReadDiskNo(void) {
    CdlFILE file;
    s32 fd;
    s32 res;

    do {
    } while (SystemCdromReadChain());
    do {
#ifdef PLATFORM_PSYZ
        // a 64-bit pointer does not fit in fd
        if (CdSearchFile(&file, "\\MINT\\DISKINFO.CNF;1") == NULL) {
            return -1;
        }
#else
        fd = (s32)CdSearchFile(&file, "\\MINT\\DISKINFO.CNF;1");
        if (fd <= 0) {
            if (fd >= -1) {
                return -1;
            }
        }
#endif
        CdControlB(CdlSetloc, &file.pos.minute, NULL);
        CdRead(1, D_800698F0, 0x80);
        do {
            res = CdReadSync(1, 0);
        } while (res > 0);
    } while (res != 0);

    // DISK0001, where [7] is '1'
    return D_800698F0[7] - '0';
}

s32 SYS_GetDiskNo(void) { return ReadDiskNo(); }

s32 func_80034410(void) { return D_80071A60; }

static void CdOpNop(void) {}

static void CdOpStopped(void) {}

static void CdOpComplete(void) { D_80071A60 = CDOP_CALLBACK; }

static void CdOpCallback(void) {
    D_80071A60 = CDOP_IDLE;
    if (D_80071A84 != NULL) {
        D_80071A84();
    }
}

static void CdOpSeek(void) {
    CdControlF(CdlSetloc, (u_char*)&D_80071A68);
    D_80071A60 = CDOP_SEEK_WAIT;
    D_8006E0F4 = 0;
    D_800698EC = 0;
}

static void CdOpSeekWait(void) {
    s32 temp_v0;
    s32* var_a1;
    s32* retries;

    switch (CdSync(1, 0)) {
    case 2:
        D_80071A60 = CDOP_COMPLETE;
        break;
    case 5:
        retries = &D_800698EC;
        (*retries)++;
        if (*retries >= 16) {
            *retries = 0;
            func_80034104();
            do {
                func_80034CAC(3);
            } while (func_80034150());
        }
        D_80071A60 = CDOP_SEEK;
        break;
    default:
        temp_v0 = VSync(-1);
        var_a1 = &D_8006E0F0;
        if (*var_a1 != temp_v0) {
            *var_a1 = temp_v0;
            D_8006E0F4++;
            if (D_8006E0F4 == 3600) {
                D_80071A60 = CDOP_SEEK;
                func_80034CAC(3);
            }
        }
        break;
    }
}

static void CdOpReadSeek(void) {
    CdControlF(CdlSetloc, (u_char*)&D_80071A68);
    D_80071A60 = CDOP_READ_SEEK_WAIT;
    D_8006E0F4 = 0;
    D_800698EC = 0;
}

static void CdOpReadSeekWait(void) {
    s32 temp_v0;
    s32* var_a1;
    s32* retries;

    switch (CdSync(1, 0)) {
    case 2:
        D_80071A60 = CDOP_READ;
        break;
    case 5:
        retries = &D_800698EC;
        (*retries)++;
        if (*retries >= 16) {
            *retries = 0;
            func_80034104();
            do {
                func_80034CAC(3);
            } while (func_80034150());
        }
        D_80071A60 = CDOP_READ_SEEK;
        break;
    default:
        temp_v0 = VSync(-1);
        var_a1 = &D_8006E0F0;
        if (*var_a1 != temp_v0) {
            *var_a1 = temp_v0;
            D_8006E0F4++;
            if (D_8006E0F4 == 3600) {
                D_80071A60 = CDOP_READ_SEEK;
                func_80034CAC(3);
            }
        }
        break;
    }
}

static void CdOpRead(void) {
    if (CdRead(D_80071A6C, D_80071A80, CdlModeSpeed) == 0) {
        D_80071A60 = CDOP_READ_SEEK;
        func_80034CAC(0x10);
        return;
    }
    D_80071A60 = CDOP_READ_WAIT;
}

static void CdOpReadWait(void) {
    switch (CdReadSync(1, NULL)) {
    case 0:
        D_80071A60 = CDOP_COMPLETE;
        break;
    case -1:
        D_80071A60 = CDOP_READ_SEEK;
        func_80034CAC(3);
        break;
    }
}

static void CdOpLzsSeek(void) {
    CdControlF(CdlSetloc, (u_char*)&D_80071A68);
    D_80071A60 = CDOP_LZS_SEEK_WAIT;
    D_8006E0F4 = 0;
    D_800698EC = 0;
}

static void CdOpLzsSeekWait(void) {
    s32 temp_v0;
    s32* var_a1;
    s32* retries;

    switch (CdSync(1, 0)) {
    case 2:
        D_80071A60 = CDOP_LZS_READ;
        break;
    case 5:
        retries = &D_800698EC;
        (*retries)++;
        if (*retries >= 16) {
            *retries = 0;
            func_80034104();
            do {
                func_80034CAC(3);
            } while (func_80034150());
        }
        D_80071A60 = CDOP_LZS_SEEK;
        break;
    default:
        temp_v0 = VSync(-1);
        var_a1 = &D_8006E0F0;
        if (*var_a1 != temp_v0) {
            *var_a1 = temp_v0;
            D_8006E0F4++;
            if (D_8006E0F4 == 3600) {
                D_80071A60 = CDOP_LZS_SEEK;
                func_80034CAC(3);
            }
        }
        break;
    }
}

static void CdOpLzsRead(void) {
    D_8006E0F8 = D_80071A6C;
    if (D_8006E0F8 > 8) {
        D_8006E0F8 = 9;
    }
    if (CdRead(D_8006E0F8, (u_long*)D_800698F0, CdlModeSpeed) == 0) {
        D_80071A60 = CDOP_LZS_SEEK;
        func_80034CAC(3);
        return;
    }
    D_80071A60 = CDOP_LZS_READ_WAIT;
}

static void CdOpLzsReadWait(void) {
    s32* sector;
    CdOp* op;

    switch (CdReadSync(1, NULL)) {
    case 0:
        sector = &D_800698E8;
        op = &D_80071A60;
        D_80034CF0 = D_800698F0;
        D_80071A6C -= 9;
        *sector += 9;
        if (func_80034D5C() == 0) {
            *op = CDOP_COMPLETE;
            return;
        }
#ifdef PLATFORM_PSYZ
        // globals are not laid out in address order on PC
        CdIntToPos(*sector, &D_80071A68);
#else
        CdIntToPos(*sector, (CdlLOC*)(op + 2));
#endif
        *op = CDOP_LZS_SEEK;
        break;
    case -1:
        CdIntToPos(D_800698E8, &D_80071A68);
        D_80071A60 = CDOP_LZS_SEEK;
        func_80034CAC(3);
        break;
    }
}

static void CdOpPause(void) {
    CdControlF(CdlPause, NULL);
    D_80071A60 = CDOP_PAUSE_WAIT;
    D_8006E0F4 = 0;
}

static void CdOpPauseWait(void) {
    s32 temp_v0;
    s32* var_a1;

    switch (CdSync(1, 0)) {
    case 2:
        CdOpCallback();
        return;
    case 5:
        D_80071A60 = CDOP_PAUSE;
        return;
    default:
        temp_v0 = VSync(-1);
        var_a1 = &D_8006E0F0;
        if (*var_a1 != temp_v0) {
            *var_a1 = temp_v0;
            D_8006E0F4++;
            if (D_8006E0F4 == 3600) {
                D_80071A60 = CDOP_PAUSE;
                func_80034CAC(3);
            }
        }
        return;
    }
}

static void (*cd_op_handlers[])(void) = {
    CdOpNop,          // CDOP_IDLE
    CdOpSeek,         // CDOP_SEEK
    CdOpSeekWait,     // CDOP_SEEK_WAIT
    CdOpReadSeek,     // CDOP_READ_SEEK
    CdOpReadSeekWait, // CDOP_READ_SEEK_WAIT
    CdOpRead,         // CDOP_READ
    CdOpReadWait,     // CDOP_READ_WAIT
    CdOpStopped,      // CDOP_STOPPED
    CdOpMoviePlay,    // CDOP_MOVIE_PLAY
    CdOpMovieBuffer,  // CDOP_MOVIE_BUFFER
    CdOpNop,          // CDOP_MOVIE_READY
    CdOpLzsSeek,      // CDOP_LZS_SEEK
    CdOpLzsSeekWait,  // CDOP_LZS_SEEK_WAIT
    CdOpLzsRead,      // CDOP_LZS_READ
    CdOpLzsReadWait,  // CDOP_LZS_READ_WAIT
    CdOpNop,          // CDOP_UNUSED_15
    CdOpCallback,     // CDOP_CALLBACK
    CdOpComplete,     // CDOP_COMPLETE
    CdOpNop,          // CDOP_PAUSE_SYNC
    CdOpPause,        // CDOP_PAUSE
    CdOpPauseWait,    // CDOP_PAUSE_WAIT
};

u32 SystemCdromReadChain(void) {
    u32* op;
    if (D_80071A60 >= LEN(cd_op_handlers)) {
        while (1) {
        }
    }
    op = &D_80071A60;
    cd_op_handlers[*op]();
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

static void func_80034CAC(u32 arg0) {
    g_AkaoCmd.opcode = AKAO_PLAY_MENU_SOUND;
    g_AkaoCmd.params[0] = arg0;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
    VSync(60);
}
