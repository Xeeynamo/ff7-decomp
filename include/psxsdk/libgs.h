#ifndef LIBGS_H
#define LIBGS_H

#include <libgpu.h>

#define GsOFSGTE 0
#define GsOFSGPU 4
#define GsINTER 1
#define GsNONINTER 0
#define GsRESET0 0
#define GsRESET3 (3 << 4)

typedef unsigned char PACKET;

typedef struct {
#ifndef __psyz
    unsigned p : 24;
    unsigned char num : 8;
#else
    O_TAG;
#endif
} GsOT_TAG;

typedef struct {
    unsigned long length;
    GsOT_TAG* org;
    unsigned long offset;
    unsigned long point;
    GsOT_TAG* tag;
} GsOT;

void GsClearVcount(void);
long GsGetVcount();
void GsInitVcount();
void GsClearOt(unsigned short offset, unsigned short point, GsOT* otp);
void GsInitGraph(unsigned short x, unsigned short y, unsigned short intmode,
                 unsigned short dith, unsigned short varmmode);
void GsDefDispBuff(
    unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1);
int GsGetActiveBuff(void);
void GsSetWorkBase(PACKET* outpacketp);
void GsSwapDispBuff(void);
void GsSortClear(unsigned char, unsigned char, unsigned char, GsOT*);
void GsDrawOt(GsOT* ot);
void GsSetDrawBuffClip(void);
void GsSetDrawBuffOffset(void);

#endif
