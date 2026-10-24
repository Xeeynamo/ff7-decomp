//! PSYQ=3.3 CC1=2.6.3
#include "common.h"
#include "magic.h"
#include "../battle/battle.h"

// GATTAI.BIN: A thirty-frame gate accompanies the detached model fade and animation flags.

typedef struct {
    s16 StartFrame;
    s16 AnimationFrame;
    s16 Countdown;
    char pad6[0x1A];
} GattaiEffect;

// Detached slot fields are separated by sixteen halfwords.
extern GattaiEffect g_BattleEffectSlots[];
extern u8 D_80151922[];
extern u8 D_801524BE;
extern u8 D_8015305A;
extern s16 D_801621F2[];
extern s16 D_801621F4[];
extern s16 D_801621F6[];
extern s8 g_BattleModelFadeFrames;
void D_800C55B8(void);
s32 BattleDetachedRegister(void (*callback)(void));
void func_800BBA40(s32);

static void func_gattai_801B0000(void) {
    if (g_BattleEffectSlots[g_BattleEffectCursor].Countdown == 0) {
        g_BattleEffectSlots[g_BattleEffectCursor].StartFrame = -1;
    }
    g_BattleEffectSlots[g_BattleEffectCursor].Countdown--;
}

s32 MAGIC_Gattai(s32 targetMask, s32 callbackArg) {
    s32 slot;
    g_BattleEffectSlots[BattleEffectRegister(func_gattai_801B0000)].Countdown = 30;
    slot = BattleDetachedRegister(D_800C55B8);
    D_801621F4[slot * 16] = 16;
    D_801621F6[slot * 16] = callbackArg;
    D_801621F2[slot * 16] = 0x100;
    g_BattleModelFadeFrames = 14;
    func_800BBA40(0x29);
    if (callbackArg == 0) {
        D_80151922[sizeof(BattleModel)] |= 2;
        D_8015305A |= 2;
    }
    if (callbackArg == 1) {
        D_80151922[0] |= 2;
        D_8015305A |= 2;
    }
    if (callbackArg == 2) {
        D_80151922[0] |= 2;
        D_801524BE |= 2;
    }
    return 0;
}
