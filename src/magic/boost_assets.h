#ifndef BOOST_ASSETS_H
#define BOOST_ASSETS_H
#include "magic_assets.h"

typedef struct {
    s32 unk0, frameCount;
    MagicSpriteFrame1 frame0;
    MagicSpriteFrame1 frame1;
    MagicSpriteFrame1 frame2;
    MagicSpriteFrame1 frame3;
    MagicSpriteFrame1 frame4;
    MagicSpriteFrame1 frame5;
    MagicSpriteFrame1 frame6;
    MagicSpriteFrame1 frame7;
} BoostAnimation0;
extern BoostAnimation0 g_BoostAnimation0;
typedef struct {
    s32 unk0, frameCount;
    MagicSpriteFrame1 frame0;
    MagicSpriteFrame1 frame1;
    MagicSpriteFrame1 frame2;
    MagicSpriteFrame1 frame3;
    MagicSpriteFrame1 frame4;
    MagicSpriteFrame1 frame5;
} BoostAnimation1;
extern BoostAnimation1 g_BoostAnimation1;
extern SpriteRenderDesc D_801BA264;
extern SpriteRenderDesc D_801BA270;
extern u_long boost_texture_tim[];
#endif
