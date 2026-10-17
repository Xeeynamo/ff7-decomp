#ifndef MAGEND_ASSETS_H
#define MAGEND_ASSETS_H
#include "magic_assets.h"

typedef struct {
    s32 unk0, frameCount;
    MagicSpriteFrame4 frame0;
} MagendAnimation0;
extern MagendAnimation0 g_MagendAnimation0;
extern s16 D_801B1730;
extern SpriteRenderDesc D_801B1734;
extern SVECTOR D_801B1740;
extern u8 D_801B1748[];
extern void* D_801C1748;
extern u_long magend_texture_tim[];
#endif
