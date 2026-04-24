#ifndef LIBETC_H
#define LIBETC_H

#include <types.h>

extern int PadIdentifier;
/*
 * PAD I/O (SIO Pad)
 */
#define PADLup (1 << 12)
#define PADLdown (1 << 14)
#define PADLleft (1 << 15)
#define PADLright (1 << 13)
#define PADRup (1 << 4)
#define PADRdown (1 << 6)
#define PADRleft (1 << 7)
#define PADRright (1 << 5)
#define PADi (1 << 9)
#define PADj (1 << 10)
#define PADk (1 << 8)
#define PADl (1 << 3)
#define PADm (1 << 1)
#define PADn (1 << 2)
#define PADo (1 << 0)
#define PADh (1 << 11)
#define PADL1 PADn
#define PADL2 PADo
#define PADR1 PADl
#define PADR2 PADm
#define PADstart PADh
#define PADselect PADk

#define _PAD(x, y) ((y) << ((x) << 4))

#define MOUSEleft (1 << 3)
#define MOUSEright (1 << 2)

#define MODE_NTSC 0
#define MODE_PAL 1

struct Callbacks {
    void (*unk0)(void);
    void* (*DMACallback)(int dma, void (*func)());
    void (*unk8)(void);
    int (*ResetCallback)(void);
    int (*StopCallback)(void);
    int (*VSyncCallbacks)(int ch, void (*f)());
    int (*RestartCallback)(void);
};

extern u16 D_8002C2BA;
extern struct Callbacks* D_8002D340;

int VSync(int mode);
int VSyncCallback(void (*f)());
int VSyncCallbacks(int ch, void (*f)());

void* DMACallback(int dma, void (*func)());
int ResetCallback(void);
int StopCallback(void);
int RestartCallback(void);
u16 CheckCallback(void);
long GetVideoMode(void);
long SetVideoMode(long mode);

void PadInit(int mode);
u_long PadRead(int id);
void PadStop(void);

#endif
