//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "ayasii.h"
#include "../battle/battle.h"

// Effect shared by Funny Breath (ファニーのいき) and Shady Breath (怪しい息).

void func_801B04E4(s32 targetMask, s32 callbackArg);

// Primitive buffer, one 0xC000 page per double-buffered frame.
extern char g_AyasiiPrimBuffer0[];
extern char g_AyasiiPrimBuffer1[];
extern void* g_AyasiiBufferPtr;
extern s32* g_AyasiiFlipEffect; // slot running AyasiiBufferFlip

// Only StartFrame/AnimationFrame are common to every magic overlay; the
// rest is payload this effect lays out for itself.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ s16 unkC;
    /* 0x0E */ s16 unkE;
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ char pad16[2];
    /* 0x18 */ s16 unk18;
    /* 0x1A */ char pad1A[6];
} AyasiiData; // size:0x20

extern AyasiiData g_BattleEffectSlots[];
extern u_long D_801B0584[];
extern s32 D_801CE994;
extern s32 D_801CE998;
void func_801B0428(void);

typedef struct {
    /* 0x0 */ SVECTOR Pos;
    /* 0x8 */ s16 unk8;
    /* 0xA */ char padA[6];
} AyasiiBillboard; // size:0x10

extern AyasiiBillboard D_801CE99C[];
extern SpriteRenderDesc D_801B6980;

// Same as Lv5DeathBufferFlip.
static void AyasiiBufferFlip(void) {
    g_AyasiiBufferPtr = g_dbIndex == 0 ? g_AyasiiPrimBuffer0 : g_AyasiiPrimBuffer1;
    if (g_BattleEffectCount < 2) {
        *g_AyasiiFlipEffect = -1;
    }
}

// Same as MAGIC_Biora.
void func_801B0054(s32 targetMask, s32 callbackArg) { func_801B04E4(targetMask, callbackArg); }

void func_801B0074(void) {
    AyasiiData* effect;
    s32 i;
    s32 n;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    D_801B6980.frameIndex = effect->AnimationFrame;
    n = effect->unk18;
    for (i = 0; i < 4; i++) {
        BattleSetBillboardMatrix(&D_801CE99C[n].Pos, D_801CE99C[n].unk8, -(D_801CE99C[n].unk8 >> 2));
        g_AyasiiBufferPtr = func_800D4D90(&D_801B6980, (u_long**)&g_cDb->unk70, 12, g_AyasiiBufferPtr);
        n++;
        if (n >= 222) {
            n = 0;
        }
    }
    if (effect->AnimationFrame > 10) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

void func_801B0190(void) {
    AyasiiData* slots;
    AyasiiData* effect;
    s32 n;
    s32 i;
    s32 r;
    s32 child;

    if (D_80062D98 != 0) {
        return;
    }
    slots = g_BattleEffectSlots;
    effect = &slots[g_BattleEffectCursor];
    if ((u16)(effect->AnimationFrame - 2) < 0x1F) {
        n = D_801CE998;
        child = BattleEffectRegister(func_801B0074);
        slots[child].unk18 = n;
        for (i = 0; i < 4; i++) {
            r = rand() % 0x1000 + 0x800;
            D_801CE99C[n].Pos.vx = effect->Pos.vx + rand() % effect->unkE - effect->unkC;
            D_801CE99C[n].Pos.vy = effect->Pos.vy - rand() % effect->unk12 + effect->unk10;
            D_801CE99C[n].Pos.vz = effect->Pos.vz + rand() % effect->unkE - effect->unkC;
            D_801CE99C[n].unk8 = effect->unk14 * r >> 12;
            n++;
            if (n >= 222) {
                n = 0;
            }
        }
        D_801CE998 = n;
    }
    if (effect->AnimationFrame == 0x28) {
        for (i = 0; i < 10; i++) {
            if ((1 << i) & D_801CE994) {
                func_800D5774(i);
            }
        }
    }
    if (effect->AnimationFrame >= 0x30) {
        effect->StartFrame = -1;
    }
    effect->AnimationFrame++;
}

void func_801B0428(void) {
    AyasiiData* child;
    AyasiiData* slots;
    s32 idx;

    g_BattleEffectSlots[g_BattleEffectCursor].StartFrame = -1;
    idx = BattleEffectRegister(func_801B0190);
    slots = g_BattleEffectSlots;
    child = &slots[idx];
    BattleEntityGetCenter(D_801CE994, &child->Pos);
    child->Pos.vy = -0x540;
    child->unkC = 0x8C0;
    child->unkE = 0x1180;
    child->unk14 = 0x27D2;
    child->unk10 = -(child->Pos.vy >> 1);
    child->unk12 = child->unk10 * 3;
    child->unk10 <<= 1;
}

void func_801B04E4(s32 targetMask, s32 callbackArg) {
    g_AyasiiFlipEffect = (s32*)&g_BattleEffectSlots[BattleEffectRegister(AyasiiBufferFlip)];
    BattleSetLoadTimToVram(D_801B0584, 0, 0, 0);
    BattleEffectRegister(func_801B0428);
    D_801CE994 = targetMask;
    D_801CE998 = 0;
    BattleAkaoCommand(AKAO_PLAY_SOUND, BattleEntityGetStereoPan(targetMask), 0xA2);
}
