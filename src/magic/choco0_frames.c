#include "common.h"
#include "../battle/battle.h"
#include "choco0.h"

static SpriteAnim choco0_puff_anims[16] = {
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 0, 0, 0x2C, 0x7808, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 64, 0, 0x2C, 0x7848, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 128, 0, 0x2C, 0x7888, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 192, 0, 0x2C, 0x78C8, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 0, 64, 0x2C, 0x7908, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 64, 64, 0x2C, 0x7948, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 128, 64, 0x2C, 0x7988, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 192, 64, 0x2C, 0x79C8, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 0, 128, 0x2C, 0x7A08, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 64, 128, 0x2C, 0x7A48, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 128, 128, 0x2C, 0x7A88, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 192, 128, 0x2C, 0x7AC8, 64, 63, 64, 63}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 0, 192, 0x2C, 0x7B08, 64, 63, 63, 62}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 64, 192, 0x2C, 0x7B48, 64, 63, 63, 62}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 128, 192, 0x2C, 0x7B88, 64, 63, 63, 62}}}}},
    {0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 192, 192, 0x2C, 0x7BC8, 64, 63, 63, 62}}}}}};

SpriteAnim g_Choco0StarFrames = {
    0x2023, 1, {{0, 1, {{SPRITE_QUAD_TEX_SIZE, -16, -16, 0, 192, 0x2D, 0x7C08, 32, 31, 32, 31}}}}};

SpriteAnim* g_Choco0PuffFrames[] = {
    &choco0_puff_anims[0],  &choco0_puff_anims[1],  &choco0_puff_anims[2],  &choco0_puff_anims[3],
    &choco0_puff_anims[4],  &choco0_puff_anims[5],  &choco0_puff_anims[6],  &choco0_puff_anims[7],
    &choco0_puff_anims[8],  &choco0_puff_anims[9],  &choco0_puff_anims[10], &choco0_puff_anims[11],
    &choco0_puff_anims[12], &choco0_puff_anims[13], &choco0_puff_anims[14], &choco0_puff_anims[15]};

Choco0SwirlEyeAnim g_Choco0SwirlEyeFrames = {
    {0x2023, 8, {{0, 1, {{0, -32, -32, 0, 128, 0x2F, 0x7D48, 64, 64, 64, 64}}}}},
    {{0, 1, {{0, -32, -32, 64, 128, 0x2F, 0x7D48, 64, 64, 64, 64}}},
     {0, 1, {{0, -32, -32, 128, 128, 0x2F, 0x7D48, 64, 64, 64, 64}}},
     {0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 192, 128, 0x2F, 0x7D48, 64, 63, 64, 64}}},
     {0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 0, 192, 0x2F, 0x7D48, 64, 64, 64, 63}}},
     {0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 64, 192, 0x2F, 0x7D48, 64, 64, 64, 63}}},
     {0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 128, 192, 0x2F, 0x7D48, 64, 64, 64, 63}}},
     {0, 1, {{SPRITE_QUAD_TEX_SIZE, -32, -32, 192, 192, 0x2F, 0x7D48, 64, 63, 64, 63}}}}};
