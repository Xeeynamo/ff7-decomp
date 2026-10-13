#ifndef MINIMAM_ASSETS_H
#define MINIMAM_ASSETS_H
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
} MinimamAnimation0;
extern MinimamAnimation0 g_MinimamAnimation0;
extern SpriteRenderDesc D_801B48A0;
extern SVECTOR g_MinimamParticles[444];
extern u8 D_801B48AC[];
extern u8 D_801C08AC[];
extern void* D_801CC8AC;
extern s32* D_801CC8B0;
extern s32 D_801CC8B4;
extern u_long minimam_texture_tim[];
#endif
