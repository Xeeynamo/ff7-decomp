//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "magic.h"
#include "sennkoku.h"
#include "../battle/battle.h"

// Death Sentence (死の宣告 / Sennkoku) and Roulette (ルーレット / Roulette).

// Hit timeline, in frames of the effect slot.
#define START_EVENT_FRAME 1
#define SOUND_FRAME 0x11
#define END_EVENT_FRAME 0x20
#define APPLY_FRAME 0x24
#define TIMELINE_LENGTH 50

// Roulette pointer spin.
#define SPIN_STEPS 48
#define POINTER_OFFSET 0x18
#define PHASE_HIT_EVENT 10
#define PHASE_END 45

// Primitive buffer, one 0xC000 page per double-buffered frame.
extern char g_SennkokuPrimBuffer0[];
extern char g_SennkokuPrimBuffer1[];
extern void* g_SennkokuBufferPtr;
extern s32* g_SennkokuFlipEffect; // slot running SennkokuBufferFlip

// 0 = Death Sentence, 1 = Roulette.
extern s32 g_SennkokuMode;

extern EffectModel g_SennkokuEffectModel;
extern u_long g_SennkokuImage[]; // TIM uploaded on setup

extern s32 g_SennkokuTargetMask;
extern s32 g_SennkokuCandidateMask;
extern s32 g_SennkokuCandidateUnits[];
extern s32 g_SennkokuCandidateCount;
extern s32 g_SennkokuCursor;
extern s32 g_SennkokuStep;
extern s32 g_SennkokuStepDelay;
extern s32 g_SennkokuPhase;
extern s32 g_SennkokuSpinDelayTable[]; // frames per cursor step; slows towards the end

extern void* D_80163C74; // primitive buffer cursor
extern u16 D_800D3544;
extern u16 D_800D3546;
extern s32 g_SennkokuTargetUnit; // first set bit of the target mask

// Only StartFrame/AnimationFrame are common to every magic overlay; the
// rest is payload this effect lays out for itself.
typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ u16 RotY;
    /* 0x0E */ char pad0E[0x12];
} SennkokuData; // size:0x20

// What BattleEventQueuePush hands back for BATTLE_EVENT_EFFECT_MODEL_START.
typedef struct {
    /* 0x00 */ s16 unk0;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ s16 unkC[2];
    /* 0x10 */ u16 RotY;
} SennkokuStartEvent;

extern SennkokuData g_BattleEffectSlots[];

// Scratchpad layout this effect uses while drawing the pointer.
typedef struct {
    /* 0x00 */ char pad00[4];
    /* 0x04 */ int p;
    /* 0x08 */ char pad08[4];
    /* 0x0C */ int flag;
    /* 0x10 */ char pad10[0x20];
    /* 0x30 */ SVECTOR pos;
    /* 0x38 */ char pad38[0x78];
    /* 0xB0 */ DVECTOR corner[4];
} SennkokuScratch;

void SennkokuHitTimeline(void);
void SennkokuRouletteSpinTick(void);
void SennkokuStartTask(void);

// Same as Lv5DeathBufferFlip.
static void SennkokuBufferFlip(void) {
    g_SennkokuBufferPtr = g_dbIndex == 0 ? g_SennkokuPrimBuffer0 : g_SennkokuPrimBuffer1;
    if (g_BattleEffectCount < 2) {
        *g_SennkokuFlipEffect = -1;
    }
}

void SennkokuMainSetup(s32 targetMask);

EffectModel* MAGIC_DeathSentence(s32 targetMask, s32 callbackArg) {
    g_SennkokuMode = 0;
    SennkokuMainSetup(targetMask);
    return &g_SennkokuEffectModel;
}

EffectModel* MAGIC_Roulette(s32 targetMask, s32 callbackArg) {
    g_SennkokuMode = 1;
    SennkokuMainSetup(targetMask);
    return &g_SennkokuEffectModel;
}

void SennkokuHitTimeline(void) {
    SennkokuData* effect;
    SennkokuStartEvent* event;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (effect->AnimationFrame == START_EVENT_FRAME) {
        event = (SennkokuStartEvent*)BattleEventQueuePush(BATTLE_EVENT_EFFECT_MODEL_START);
        event->Pos = effect->Pos;
        event->RotY = effect->RotY;
    } else if (effect->AnimationFrame == SOUND_FRAME) {
        BattleAkaoCommand(AKAO_PLAY_SOUND, BattlePositionToStereoPan(&effect->Pos), SFX_1A4);
    } else if (effect->AnimationFrame == END_EVENT_FRAME) {
        BattleEventQueuePush(BATTLE_EVENT_EFFECT_MODEL_END);
    }

    if (effect->AnimationFrame >= TIMELINE_LENGTH) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        if (effect->AnimationFrame == APPLY_FRAME) {
            func_800D5774(g_SennkokuTargetUnit);
        }
        effect->AnimationFrame++;
    }
}

void SennkokuRouletteSpinTick(void) {
    SennkokuScratch* scratch;
    SennkokuData* effect;
    SennkokuData* hit;
    POLY_FT4* prim;
    s32 unit;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    scratch = (SennkokuScratch*)0x1F800000;
    if (g_SennkokuPhase == 0 && D_80062D98 == 0) {
        g_SennkokuStepDelay--;
        if (g_SennkokuStepDelay <= 0) {
            g_SennkokuCursor++;
            if (g_SennkokuCursor >= g_SennkokuCandidateCount) {
                g_SennkokuCursor = 0;
            }
            g_SennkokuStep++;
            if (g_SennkokuStep >= SPIN_STEPS) {
                if (g_SennkokuTargetUnit == g_SennkokuCandidateUnits[g_SennkokuCursor]) {
                    g_SennkokuPhase = 1;
                }
                g_SennkokuStepDelay = g_SennkokuSpinDelayTable[SPIN_STEPS - 1];
            } else {
                g_SennkokuStepDelay = g_SennkokuSpinDelayTable[g_SennkokuStep];
            }
            unit = g_SennkokuCandidateUnits[g_SennkokuCursor];
            BattleGetPartPosition(unit, g_BattleModels[unit].boneIndices[0], &scratch->pos);
            BattleAkaoCommand(AKAO_PLAY_SOUND, BattlePositionToStereoPan(&scratch->pos), SFX_1E3);
        }
    }

    unit = g_SennkokuCandidateUnits[g_SennkokuCursor];
    D_800D3544 = g_SennkokuTargetMask;
    D_800D3546 = g_SennkokuCandidateMask;
    BattleGetPartPosition(unit, g_BattleModels[unit].boneIndices[0], &scratch->pos);
    SetRotMatrix(&g_BattleWorldView.m);
    SetTransMatrix(&g_BattleWorldView.m);
    RotTransPers(&scratch->pos, (int*)&scratch->corner[0], &scratch->p, &scratch->flag);
    scratch->corner[1] = scratch->corner[2] = scratch->corner[3] = scratch->corner[0];

    // The pointer is a 24x16 sprite beside the unit, mirrored for the enemy side. Each word below packs
    // (u, v, clut) for vertex 0, (u, v, tpage) for vertex 1 and (u, v) for vertices 2 and 3.
    prim = D_80163C74;
    if (unit < START_ENEMY) {
        scratch->corner[0].vx -= POINTER_OFFSET;
        scratch->corner[2].vx -= POINTER_OFFSET;
        *(u32*)&prim->u0 = 0x78480018;
        *(u32*)&prim->u1 = 0x4D0000;
        *(u32*)&prim->u2 = 0x1018;
        *(u32*)&prim->u3 = 0x1000;
    } else {
        scratch->corner[1].vx += POINTER_OFFSET;
        scratch->corner[3].vx += POINTER_OFFSET;
        *(u32*)&prim->u0 = 0x78480000;
        *(u32*)&prim->u1 = 0x4D0018;
        *(u32*)&prim->u2 = 0x1000;
        *(u32*)&prim->u3 = 0x1018;
    }
    scratch->corner[2].vy += 0x10;
    scratch->corner[3].vy += 0x10;
    *(u32*)&prim->x0 = *(u32*)&scratch->corner[0];
    *(u32*)&prim->x1 = *(u32*)&scratch->corner[1];
    *(u32*)&prim->x2 = *(u32*)&scratch->corner[2];
    *(u32*)&prim->x3 = *(u32*)&scratch->corner[3];
    setlen(prim, 9);
    *(u32*)&prim->r0 = 0x808080;
    prim->code = 0x2E; // POLY_FT4, semi-transparent
    addPrim(&g_cDb->unk70[3], prim);
    prim++;
    D_80163C74 = prim;

    if (g_SennkokuPhase != 0) {
        if (g_SennkokuPhase == PHASE_HIT_EVENT) {
            hit = &g_BattleEffectSlots[BattleEffectRegister(SennkokuHitTimeline)];
            hit->Pos = effect->Pos;
            hit->RotY = effect->RotY;
        }
        if (g_SennkokuPhase >= PHASE_END) {
            effect->StartFrame = -1;
        }
        g_SennkokuPhase++;
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

void SennkokuStartTask(void) {
    SennkokuData* slots;
    SennkokuData* spin;
    u32 candidates;
    s32 count;
    s32 i;

    g_BattleEffectSlots[g_BattleEffectCursor].StartFrame = -1;
    slots = g_BattleEffectSlots;
    if (g_SennkokuMode != 0) {
        spin = &slots[BattleEffectRegister(SennkokuRouletteSpinTick)];
        BattleGetPartPosition(g_SennkokuTargetUnit, g_BattleModels[g_SennkokuTargetUnit].boneIndices[10], &spin->Pos);
        spin->RotY = g_BattleModels[g_SennkokuTargetUnit].rootRot.vy;

        // Every unit in the battle that is not downed. These must be read as fields of g_BattleData: declared
        // as two separate globals, the compiler moves both loads above the RotY store.
        candidates = g_BattleData.unk14C & ~g_BattleData.downedActors;
        g_SennkokuCandidateMask = candidates;
        count = 0;
        for (i = 0; i < NUM_BATTLE_ACTOR; i++) {
            if (candidates & (1 << i)) {
                g_SennkokuCandidateUnits[count++] = i;
            }
        }
        g_SennkokuCandidateCount = count;
        g_SennkokuCursor = rand() % count;
        g_SennkokuStep = 0;
        g_SennkokuPhase = 0;
        g_SennkokuStepDelay = g_SennkokuSpinDelayTable[0];
    } else {
        spin = &slots[BattleEffectRegister(SennkokuHitTimeline)];
        BattleGetPartPosition(g_SennkokuTargetUnit, g_BattleModels[g_SennkokuTargetUnit].boneIndices[10], &spin->Pos);
        spin->RotY = g_BattleModels[g_SennkokuTargetUnit].rootRot.vy;
    }
}

void SennkokuMainSetup(s32 targetMask) {
    s32 i;
    s32 count;

    BattleSetLoadTimToVram(g_SennkokuImage, 0, 0, 0);
    g_SennkokuTargetMask = targetMask;
    i = 0;
    count = 0;
    for (; i < NUM_BATTLE_ACTOR; i++) {
        if ((targetMask >> i) & 1) {
            count++;
            if (count == 1) {
                g_SennkokuTargetUnit = i;
            }
        }
    }
    BattleEffectRegister(SennkokuStartTask);
}
