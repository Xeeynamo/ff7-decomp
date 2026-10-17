#ifndef THREAT_ASSETS_H
#define THREAT_ASSETS_H
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
} ThreatAnimation0;
extern ThreatAnimation0 g_ThreatAnimation0;
extern SpriteRenderDesc D_801B8548;
extern u8 D_801B8554[];
extern u_long threat_texture_tim[];
#endif
