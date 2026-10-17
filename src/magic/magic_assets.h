#ifndef MAGIC_ASSETS_H
#define MAGIC_ASSETS_H
#include "../battle/battle.h"

typedef struct {
    s16 unk0, quadCount;
    SpriteQuad quads[1];
} MagicSpriteFrame1;
typedef struct {
    s16 unk0, quadCount;
    SpriteQuad quads[2];
} MagicSpriteFrame2;
typedef struct {
    s16 unk0, quadCount;
    SpriteQuad quads[3];
} MagicSpriteFrame3;
typedef struct {
    s16 unk0, quadCount;
    SpriteQuad quads[4];
} MagicSpriteFrame4;
#endif
