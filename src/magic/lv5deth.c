//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "../battle/battle.h"

extern s32 g_dbIndex;
// Battle far colour; func_800B9568 feeds r/g/b straight to SetFarColor.
extern CVECTOR D_800F5B70;
extern s16 D_800F5B74;
extern u8 D_80062D98; // set while the battle is paused

#define TARGET_LIFETIME 45
#define FADE_IN_FRAMES 8
#define FADE_OUT_START_FRAME 37
#define RESULT_POPUP_FRAME 35

#define FAR_DEPTH_PER_FRAME 320
#define FAR_DEPTH_MAX 2560
#define SCREEN_FADE_LIFETIME 53

// Only StartFrame/AnimationFrame are common to every magic overlay; the
// remaining 0x1C bytes are payload each effect lays out for itself.
typedef struct Lv5DeathEffect {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ u16 Scale; // func_800D4368 puts it on all three diagonal
                          // entries of its matrix
    /* 0x0E */ union {
        s16 TargetIndex;       // ring / sprite / attach effects
        s16 FadeOutStartFrame; // screen-fade effect only
    } u;
    /* 0x10 */ char pad10[0x10];
} Lv5DeathEffect; // size:0x20

extern Lv5DeathEffect g_BattleEffectSlots[];

// Primitive buffer, one 0xC000 page per double-buffered frame.
extern char g_Lv5DeathPrimBuffer0[];
extern char g_Lv5DeathPrimBuffer1[];
extern void* g_Lv5DeathBufferPtr;
extern Lv5DeathEffect* g_Lv5DeathFlipEffect; // slot running Lv5DeathBufferFlip
extern s32 g_Lv5DeathTargetsRemaining;
extern u_long g_Lv5DeathTexture[]; // 8bpp TIM + 256-colour CLUT, uploaded on setup

// Flat 16-point ring of radius 976 lying in the XY plane at z = -21.
extern s32 g_Lv5DeathRingModel[];

// .color.cd holds the GPU primitive code (0x2E).
extern SpriteRenderDesc g_Lv5DeathSpriteDesc;

static void Lv5DeathBufferFlip(void) {
    g_Lv5DeathBufferPtr = g_dbIndex == 0 ? g_Lv5DeathPrimBuffer0 : g_Lv5DeathPrimBuffer1;
    if (g_BattleEffectCount < 2) {
        *(s32*)g_Lv5DeathFlipEffect = -1;
    }
}

static void Lv5DeathMainSetup(s32 targetMask, s32 arg1);

// Overlay entry point, at 0x801B0054; func_800D0C80 dispatches magic id 32
// here.
void MAGIC_Lv5Death(s32 targetMask, s32 arg1) { Lv5DeathMainSetup(targetMask, arg1); }

static void Lv5DeathRenderRing(void) {
    // Unused; gives the function its 0x58 stack frame.
    char pad[0x34];
    Lv5DeathEffect* effect;
    s32 scale;
    s32 frame;
    ModelRenderDesc* desc;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    // The shift pair sign-extends Scale.
    scale = effect->Scale << 16;
    func_800D4368(&effect->Pos, scale >> 16, -(scale >> 19));

    // Render descriptor built in scratchpad RAM.
    desc = (ModelRenderDesc*)0x1F800000;
    desc->model = g_Lv5DeathRingModel;
    desc->flags = MODEL_DEPTH_CUE | MODEL_SEMI_TRANS;
    desc->uvOffset = 0;
    desc->color = 0x800;
    desc->tpage = 0;
    desc->clut = 0;

    frame = effect->AnimationFrame;
    if (frame < FADE_IN_FRAMES) {
        frame <<= 8;
        desc->color = 0x1000 - frame;
    } else if (frame >= FADE_OUT_START_FRAME) {
        desc->color = (frame << 8) - 0x1D00;
    }

    SetFarColor(0, 0, 0);
    g_Lv5DeathBufferPtr = func_800D29D4(desc, g_cDb->unk70, 12, g_Lv5DeathBufferPtr);

    if (effect->AnimationFrame >= TARGET_LIFETIME) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

static void Lv5DeathRenderTargetSprite(void) {
    Lv5DeathEffect* effect;
    s32 frame;
    u8 intensity;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_Lv5DeathSpriteDesc.frameIndex = effect->AnimationFrame & 7;

    frame = effect->AnimationFrame;
    if (frame < FADE_IN_FRAMES) {
        intensity = frame * 16;
    } else if (frame >= FADE_OUT_START_FRAME) {
        intensity = -128 - ((frame - FADE_OUT_START_FRAME) * 16);
    } else {
        intensity = 128;
    }
    g_Lv5DeathSpriteDesc.color.r = g_Lv5DeathSpriteDesc.color.g = g_Lv5DeathSpriteDesc.color.b = intensity;

    func_800D4368(&effect->Pos, (s16)effect->Scale, -((s16)effect->Scale >> 2));
    g_Lv5DeathBufferPtr = func_800D4D90(&g_Lv5DeathSpriteDesc, g_cDb->unk70, 12, g_Lv5DeathBufferPtr);

    if (effect->AnimationFrame >= TARGET_LIFETIME) {
        effect->StartFrame = -1;
        g_Lv5DeathTargetsRemaining--;
    }
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == RESULT_POPUP_FRAME) {
            func_800D5774(effect->u.TargetIndex);
        }
        effect->AnimationFrame++;
    }
}

static void Lv5DeathScreenFade(void) {
    Lv5DeathEffect* effect;
    s32 farDepth;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (effect->AnimationFrame < FADE_IN_FRAMES) {
        D_800F5B70.r = D_800F5B70.g = D_800F5B70.b = 0;
        farDepth = effect->AnimationFrame * FAR_DEPTH_PER_FRAME;
    } else if (g_Lv5DeathTargetsRemaining <= 0) {
        if (effect->u.FadeOutStartFrame == 0) {
            effect->u.FadeOutStartFrame = effect->AnimationFrame;
        }
        farDepth = FAR_DEPTH_MAX - (effect->AnimationFrame - effect->u.FadeOutStartFrame) * FAR_DEPTH_PER_FRAME;
    } else {
        farDepth = FAR_DEPTH_MAX;
    }

    if (effect->AnimationFrame >= SCREEN_FADE_LIFETIME) {
        farDepth = 0;
        effect->StartFrame = -1;
    }

    D_800F5B74 = farDepth;
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

static void Lv5DeathAttachToTarget(s32 target, s32 arg1) {
    Lv5DeathEffect* effect;
    Lv5DeathEffect* ring;

    effect = &g_BattleEffectSlots[BattleEffectRegister(Lv5DeathRenderTargetSprite)];
    BattleGetPartPosition(target, D_801518E4[target].D_8015190F, &effect->Pos);
    effect->Scale = 0x1CCC;
    effect->u.TargetIndex = target;

    ring = &g_BattleEffectSlots[BattleEffectRegister(Lv5DeathRenderRing)];
    ring->Pos = effect->Pos;
    ring->Scale = 0x13DC;

    BattleCommandSend(0x20, BattlePositionToStereoPan(&effect->Pos), 0xAA);
}

static void Lv5DeathMainSetup(s32 targetMask, s32 arg1) {
    Lv5DeathEffect* effect;
    s32 count;
    s32 i;

    g_Lv5DeathFlipEffect = &g_BattleEffectSlots[BattleEffectRegister(Lv5DeathBufferFlip)];
    func_800D2980(g_Lv5DeathTexture, 0, 0, 0);
    effect = &g_BattleEffectSlots[BattleEffectRegister(Lv5DeathScreenFade)];
    effect->u.FadeOutStartFrame = 0;
    // frameStep 2: with three targets the pairs spawn on frames 1, 3 and 5.
    MagicAnimationRegister(targetMask, arg1, 2, Lv5DeathAttachToTarget);

    i = 0;
    count = 0;
    while (i < 10) {
        if ((targetMask >> i++) & 1) {
            count++;
        }
    }
    g_Lv5DeathTargetsRemaining = count;
}
