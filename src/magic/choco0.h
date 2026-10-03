#ifndef CHOCO0_H
#define CHOCO0_H

// A SpriteAnim whose frames run on past its one-element array.
typedef struct {
    SpriteAnim anim;
    SpriteFrame moreFrames[7];
} Choco0SwirlEyeAnim;

extern SpriteAnim* g_Choco0PuffFrames[];
extern SpriteAnim g_Choco0StarFrames;
extern Choco0SwirlEyeAnim g_Choco0SwirlEyeFrames;

#endif
