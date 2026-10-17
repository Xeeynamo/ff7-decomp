#ifndef ECHOES_ASSETS_H
#define ECHOES_ASSETS_H
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
} EchoesAnimation0;
extern EchoesAnimation0 g_EchoesAnimation0;
extern SpriteRenderDesc D_801B6870;
extern u8 D_801B687C[];
extern u_long echoes_texture_tim[];
#endif
