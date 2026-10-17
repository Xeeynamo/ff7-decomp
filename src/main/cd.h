#ifndef MAIN_CD_H
#define MAIN_CD_H

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

typedef struct {
    /* 0x00 */ s32 op;
    /* 0x04 */ s32 disc;
    /* 0x08 */ CdlLOC loc;
    /* 0x0C */ size_t sectors;
    /* 0x10 */ u8 unk10[0x10];
    /* 0x20 */ u_long* dst;
    /* 0x24 */ void (*callback)(void);
} Unk80071A60; // size:0x28

extern Unk80071A60 D_80071A60;

#endif
