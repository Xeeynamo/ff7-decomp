#ifndef JINRAI_ASSETS_H
#define JINRAI_ASSETS_H
#include "magic_assets.h"

typedef struct {
    s32 unk0, frameCount;
    MagicSpriteFrame3 frame0;
    MagicSpriteFrame3 frame1;
    MagicSpriteFrame3 frame2;
    MagicSpriteFrame2 frame3;
    MagicSpriteFrame2 frame4;
} JinraiAnimation0;
extern JinraiAnimation0 g_JinraiAnimation0;
extern SpriteRenderDesc D_801B5220;
extern u_long jinrai_texture_tim[];
#endif
