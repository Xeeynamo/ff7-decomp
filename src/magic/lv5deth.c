//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"

extern s32 g_dbIndex;
// Battle far colour; func_800B9568 feeds r/g/b straight to SetFarColor.
extern CVECTOR D_800F5B70;
extern s16 D_800F5B74;
extern u8 D_80062D98; // set while the battle is paused

// One slot of the shared battle effect array, as Lv5 Death lays it out. Only
// StartFrame/AnimationFrame are common to every magic overlay; the remaining
// 0x1C bytes are payload each effect defines for itself -- compare BarrierData
// in barrier.c, which puts its SVECTORs at completely different offsets.
typedef struct Lv5DeathEffect {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ u16 Scale; // matrix diagonal handed to func_800D4368
    /* 0x0E */ union {
        s16 TargetIndex;       // ring / sprite / attach effects
        s16 FadeOutStartFrame; // screen-fade effect only
    } u;
    /* 0x10 */ char pad10[0x10];
} Lv5DeathEffect; // size:0x20

extern Lv5DeathEffect D_80162978[];

// Primitive buffer, one 0xC000 page per double-buffered frame.
extern char Lv5DeathPrimBuffer0[];
extern char Lv5DeathPrimBuffer1[];
extern void* Lv5DeathBufferPtr;
extern Lv5DeathEffect* Lv5DeathFlipEffect; // slot running Lv5DeathBufferFlip
extern s32 Lv5DeathTargetsRemaining;
extern u_long
    Lv5DeathTexture[]; // 8bpp TIM + 256-colour CLUT, uploaded on setup

// Flat 16-point ring of radius 976 lying in the XY plane at z = -21.
extern s32 Lv5DeathRingModel[];

// Render descriptor for the func_800D4D90 pass. Same layout func_800D29D4
// reads, but this renderer only touches offsets 0x0..0xB, so the instance in
// ROM is truncated to 0xC bytes.
typedef struct {
    /* 0x0 */ s32* model;
    /* 0x4 */ CVECTOR color; // .cd holds the GPU primitive code (0x2E)
    /* 0x8 */ s16 TextureFrame;
    /* 0xA */ s16 unkA;
} Lv5DeathDesc; // size:0xC

extern Lv5DeathDesc Lv5DeathSpriteDesc;

static void Lv5DeathBufferFlip(void) {
    Lv5DeathBufferPtr =
        g_dbIndex == 0 ? Lv5DeathPrimBuffer0 : Lv5DeathPrimBuffer1;
    if (D_80162080 < 2) {
        *(s32*)Lv5DeathFlipEffect = -1;
    }
}

static void Lv5DeathMainSetup(s32 targetMask, s32 arg1);

// Overlay entry point, at 0x801B0054; func_800D0C80 dispatches magic id 32
// here. Trampolines to Lv5DeathMainSetup, which sits at the end of the file.
void MAGIC_Lv5Death(s32 targetMask, s32 arg1) {
    Lv5DeathMainSetup(targetMask, arg1);
}

// The ring spins 0 -> 180 degrees while it fades in, holds, then completes the
// turn on the way out. Scales from 1.0 down to 0.5 over the same window.
static void Lv5DeathRenderRing(void) {
    // Unused, but required for the match; it gives the function its 0x58 frame.
    char pad[0x34];
    Lv5DeathEffect* effect;
    s32 rot;
    s32 val;
    Unk801B0C98* desc;

    effect = &D_80162978[D_8015169C];
    rot = effect->Scale << 16;
    func_800D4368(&effect->Pos, rot >> 16, -(rot >> 19));

    // Render descriptor built in scratchpad RAM.
    desc = (Unk801B0C98*)0x1F800000;
    desc->unk0 = Lv5DeathRingModel;
    desc->unk4 = 0x88;
    desc->unk8 = 0;
    desc->unkA = 0x800;
    desc->unkC = 0;
    desc->unkE = 0;

    val = effect->AnimationFrame;
    if (val < 8) {
        val <<= 8;
        desc->unkA = 0x1000 - val;
    } else if (val >= 37) {
        desc->unkA = (val << 8) - 0x1D00;
    }

    SetFarColor(0, 0, 0);
    Lv5DeathBufferPtr =
        func_800D29D4(desc, g_cDb->unk70, 12, Lv5DeathBufferPtr);

    if (effect->AnimationFrame >= 45) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

// Textured sprite over the target: an 8-frame texture cycle, fading up to
// half intensity over 8 frames, holding, then fading back out from frame 37.
// The result popup fires at frame 35, the slot retires at 45.
static void Lv5DeathRenderTargetSprite(void) {
    Lv5DeathEffect* effect;
    s32 frame;
    u8 color;

    effect = &D_80162978[D_8015169C];
    Lv5DeathSpriteDesc.TextureFrame = effect->AnimationFrame & 7;

    frame = effect->AnimationFrame;
    if (frame < 8) {
        color = frame * 16;
    } else if (frame >= 37) {
        color = -128 - ((frame - 37) * 16);
    } else {
        color = 128;
    }
    Lv5DeathSpriteDesc.color.r = Lv5DeathSpriteDesc.color.g =
        Lv5DeathSpriteDesc.color.b = color;

    func_800D4368(&effect->Pos, (s16)effect->Scale, -((s16)effect->Scale >> 2));
    Lv5DeathBufferPtr =
        func_800D4D90(&Lv5DeathSpriteDesc, g_cDb->unk70, 12, Lv5DeathBufferPtr);

    if (effect->AnimationFrame >= 45) {
        effect->StartFrame = -1;
        Lv5DeathTargetsRemaining--;
    }
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == 35) {
            func_800D5774(effect->u.TargetIndex);
        }
        effect->AnimationFrame++;
    }
}

// Darkens the whole battlefield by driving the far-colour depth from 0 up to
// 2560 over 8 frames, holding while any target is still animating, then
// easing back down once the last one retires.
static void Lv5DeathScreenFade(void) {
    Lv5DeathEffect* effect;
    s32 val;

    effect = &D_80162978[D_8015169C];
    if (effect->AnimationFrame < 8) {
        D_800F5B70.r = D_800F5B70.g = D_800F5B70.b = 0;
        val = effect->AnimationFrame * 320;
    } else if (Lv5DeathTargetsRemaining <= 0) {
        if (effect->u.FadeOutStartFrame == 0) {
            effect->u.FadeOutStartFrame = effect->AnimationFrame;
        }
        val =
            2560 - (effect->AnimationFrame - effect->u.FadeOutStartFrame) * 320;
    } else {
        val = 2560;
    }

    if (effect->AnimationFrame >= 53) {
        val = 0;
        effect->StartFrame = -1;
    }

    D_800F5B74 = val;
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

// Per-target callback: spawns the sprite and the ring on the target's part.
static void Lv5DeathAttachToTarget(s32 target) {
    Lv5DeathEffect* effect;
    Lv5DeathEffect* ring;

    effect = &D_80162978[BattleEffectRegister(Lv5DeathRenderTargetSprite)];
    BattleGetPartPosition(target, D_801518E4[target].D_8015190F, &effect->Pos);
    effect->Scale = 0x1CCC;
    effect->u.TargetIndex = target;

    ring = &D_80162978[BattleEffectRegister(Lv5DeathRenderRing)];
    ring->Pos = effect->Pos;
    ring->Scale = 0x13DC;

    BattleCommandSend(0x20, BattlePositionToStereoPan(&effect->Pos), 0xAA);
}

static void Lv5DeathMainSetup(s32 targetMask, s32 arg1) {
    Lv5DeathEffect* effect;
    s32 count;
    s32 i;

    Lv5DeathFlipEffect = &D_80162978[BattleEffectRegister(Lv5DeathBufferFlip)];
    func_800D2980(Lv5DeathTexture, 0, 0, 0);
    effect = &D_80162978[BattleEffectRegister(Lv5DeathScreenFade)];
    effect->u.FadeOutStartFrame = 0;
    MagicAnimationRegister(targetMask, arg1, 2, Lv5DeathAttachToTarget);

    i = 0;
    count = 0;
    while (i < 10) {
        if ((targetMask >> i++) & 1) {
            count++;
        }
    }
    Lv5DeathTargetsRemaining = count;
}
