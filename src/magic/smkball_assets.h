#ifndef SMKBALL_ASSETS_H
#define SMKBALL_ASSETS_H
#include "magic_assets.h"

typedef struct {
    s32 unk0, frameCount;
    MagicSpriteFrame4 frame0;
    MagicSpriteFrame4 frame1;
    MagicSpriteFrame4 frame2;
    MagicSpriteFrame4 frame3;
    MagicSpriteFrame4 frame4;
    MagicSpriteFrame4 frame5;
    MagicSpriteFrame4 frame6;
    MagicSpriteFrame4 frame7;
    MagicSpriteFrame4 frame8;
    MagicSpriteFrame4 frame9;
    MagicSpriteFrame3 frame10;
    MagicSpriteFrame2 frame11;
} SmkballAnimation0;
extern SmkballAnimation0 g_SmkballAnimation0;
extern MATRIX D_801B670C;
extern SpriteRenderDesc D_801B672C;
extern u8 D_801B6738[];
extern u_long smkball_texture_tim[];
#endif
