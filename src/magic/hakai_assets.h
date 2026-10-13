#ifndef HAKAI_ASSETS_H
#define HAKAI_ASSETS_H
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
    MagicSpriteFrame1 frame8;
    MagicSpriteFrame1 frame9;
    MagicSpriteFrame1 frame10;
    MagicSpriteFrame1 frame11;
} HakaiAnimation0;
extern HakaiAnimation0 g_HakaiAnimation0;
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
    MagicSpriteFrame1 frame8;
    MagicSpriteFrame1 frame9;
    MagicSpriteFrame1 frame10;
    MagicSpriteFrame1 frame11;
} HakaiAnimation1;
extern HakaiAnimation1 g_HakaiAnimation1;
extern SpriteRenderDesc D_801B6754;
extern SpriteAnim* D_801B6760[];
extern u8 D_801B6768[];
extern void* D_801CE768;
extern u_long hakai_texture_tim[];
#endif
