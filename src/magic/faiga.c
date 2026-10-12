//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include <libetc.h>
#include "magic.h"
#include "faiga.h"
#include "../battle/battle.h"

// Fire3 (ファイガ / Firaga), tier 3.

typedef struct {
    /* 0x00 */ s16 StartFrame;
    /* 0x02 */ s16 AnimationFrame;
    /* 0x04 */ SVECTOR Pos;
    /* 0x0C */ u16 unkC;
    /* 0x0E */ s16 unkE;
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 Scale;
    /* 0x14 */ s16 unk14;
    /* 0x16 */ u16 unk16;
    /* 0x18 */ char pad18[8];
} FaigaData; // size:0x20

extern FaigaData g_BattleEffectSlots[];
extern SpriteRenderDesc g_FaigaSpriteDesc;
// Battle far colour; func_800B9568 feeds r/g/b straight to SetFarColor.
extern CVECTOR D_800F5B70;
extern s32 D_801C9D88[];
extern s32 D_801C9364[];

// Primitive buffer, one page per double-buffered frame.
extern char g_FaigaPrimBuffer0[];
extern char g_FaigaPrimBuffer1[];
extern void* g_FaigaBufferPtr;
extern s32* g_FaigaFlipEffect; // slot running func_801B0030

void func_801B0F28(s32 targetMask, s32 callbackArg);
void func_801B0D48(void);
void func_801B0914(void);
void func_801B062C(void);
void func_801B0B04(void);
extern u_long D_801B0FC0[];
extern s32 D_801E2FCC;

// Same as Lv5DeathBufferFlip.
void func_801B0030(void) {
    g_FaigaBufferPtr = g_dbIndex == 0 ? g_FaigaPrimBuffer0 : g_FaigaPrimBuffer1;
    if (g_BattleEffectCount < 2) {
        *g_FaigaFlipEffect = -1;
    }
}

// Entry: calls the effect setup.
void func_801B0084(s32 targetMask, s32 callbackArg) { func_801B0F28(targetMask, callbackArg); }

void func_801B00A4(void) {
    s32 temp;
    FaigaData* effect;
    SVECTOR rot;
    MATRIX mat;
    VECTOR scale;
    ModelRenderDesc* desc;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vx = 0;
    rot.vy = effect->unkC + effect->unkE;
    rot.vz = 0;
    RotMatrix(&rot, &mat);
    mat.t[0] = effect->Pos.vx;
    mat.t[1] = effect->Pos.vy;
    mat.t[2] = effect->Pos.vz;
    scale.vx = scale.vy = scale.vz = effect->Scale;
    ScaleMatrix(&mat, &scale);
    CompMatrix(&g_BattleWorldView.m, &mat, &mat);
    SetRotMatrix(&mat);
    SetTransMatrix(&mat);

    desc = (ModelRenderDesc*)getScratchAddr(0);
    desc->model = D_801C9364;
    desc->flags = MODEL_SEMI_TRANS;
    desc->uvOffset = 0;
    temp = 0x80;
    desc->color = temp;
    desc->tpage = 0x20;
    desc->clut = 0;
    if (effect->AnimationFrame < 0xD) {
        desc->color = 0x80 - (0xD - effect->AnimationFrame) * 9;
    } else if (effect->AnimationFrame >= 0x14) {
        desc->color = 0x80 - (effect->AnimationFrame - 0x14) * 14;
    }
    g_FaigaBufferPtr = func_800D29D4(desc, (u_long**)&g_cDb->unk70, 12, g_FaigaBufferPtr);
    if (effect->AnimationFrame >= 0x1D) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->Scale += effect->unk14;
        if (effect->Scale < 0) {
            effect->Scale = 0x7000;
        }
        effect->unkC += effect->unk10;
        effect->AnimationFrame++;
    }
}

void func_801B0284(void) {
    s32 temp;
    FaigaData* effect;
    SVECTOR rot;
    MATRIX mat;
    VECTOR scale;
    ModelRenderDesc* desc;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vx = 0;
    rot.vy = effect->unkC + effect->unkE;
    rot.vz = 0;
    RotMatrix(&rot, &mat);
    mat.t[0] = effect->Pos.vx;
    mat.t[1] = effect->Pos.vy;
    mat.t[2] = effect->Pos.vz;
    scale.vx = scale.vy = scale.vz = effect->Scale;
    ScaleMatrix(&mat, &scale);
    CompMatrix(&g_BattleWorldView.m, &mat, &mat);
    SetRotMatrix(&mat);
    SetTransMatrix(&mat);

    desc = (ModelRenderDesc*)getScratchAddr(0);
    desc->model = D_801C9D88;
    desc->flags = MODEL_SEMI_TRANS;
    desc->uvOffset = 0;
    temp = 0x80;
    desc->color = temp;
    desc->tpage = 0x20;
    desc->clut = 0;
    if (effect->AnimationFrame < 0xD) {
        desc->color = 0x80 - (0xD - effect->AnimationFrame) * 9;
    } else if (effect->AnimationFrame >= 0x16) {
        desc->color = 0x80 - (effect->AnimationFrame - 0x16) * 9;
    }
    g_FaigaBufferPtr = func_800D29D4(desc, (u_long**)&g_cDb->unk70, 12, g_FaigaBufferPtr);
    if (effect->AnimationFrame >= 0x23) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->Scale += effect->unk14;
        if (effect->Scale < 0) {
            effect->Scale = 0x7000;
        }
        effect->unkC += effect->unk10;
        effect->AnimationFrame++;
    }
}

void func_801B0458(void) {
    s32 temp;
    FaigaData* effect;
    SVECTOR rot;
    MATRIX mat;
    VECTOR scale;
    ModelRenderDesc* desc;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    rot.vx = 0;
    rot.vy = effect->unkC + effect->unkE;
    rot.vz = 0;
    RotMatrix(&rot, &mat);
    mat.t[0] = effect->Pos.vx;
    mat.t[1] = effect->Pos.vy;
    mat.t[2] = effect->Pos.vz;
    scale.vx = scale.vy = scale.vz = effect->Scale;
    ScaleMatrix(&mat, &scale);
    CompMatrix(&g_BattleWorldView.m, &mat, &mat);
    SetRotMatrix(&mat);
    SetTransMatrix(&mat);

    desc = (ModelRenderDesc*)getScratchAddr(0);
    desc->model = D_801C9D88;
    desc->flags = MODEL_SEMI_TRANS;
    desc->uvOffset = 0;
    temp = 0x80;
    desc->color = temp;
    desc->tpage = 0x20;
    desc->clut = 0;
    if (effect->AnimationFrame < 0xD) {
        desc->color = 0x80 - (0xD - effect->AnimationFrame) * 9;
    } else if (effect->AnimationFrame >= 0x17) {
        desc->color = 0x80 - (effect->AnimationFrame - 0x17) * 9;
    }
    g_FaigaBufferPtr = func_800D29D4(desc, (u_long**)&g_cDb->unk70, 12, g_FaigaBufferPtr);
    if (effect->AnimationFrame >= 0x24) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->Scale += effect->unk14;
        if (effect->Scale < 0) {
            effect->Scale = 0x7000;
        }
        effect->unkC += effect->unk10;
        effect->AnimationFrame++;
    }
}

void func_801B062C(void) {
    FaigaData* effect;
    FaigaData* child;
    s32 i;
    s32 size;
    s16 fade;

    if (D_80062D98 == 0) {
        effect = &g_BattleEffectSlots[g_BattleEffectCursor];
        if (effect->AnimationFrame == 8) {
            size = effect->Scale << 12;
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B00A4)];
            child->unkC = 0;
            child->unkE = 0;
            child->unk10 = -0x14;
            child->Pos = effect->Pos;
            child->Scale = (size / 750) >> 1;
            child->unk14 = size / 24000;
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0284)];
            child->unkC = 0;
            child->unkE = -0x400;
            child->unk10 = -0x22;
            child->Pos = effect->Pos;
            child->Scale = (size / 620) >> 1;
            child->unk14 = size / 19840;
            child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0458)];
            child->unkC = 0;
            child->unkE = -0x400;
            child->unk10 = -0x1A;
            child->Pos = effect->Pos;
            child->Scale = (size / 600) >> 1;
            child->unk14 = size / 19200;
        }
        if (effect->AnimationFrame < 8) {
            D_800F5B70.r = 0x20;
            D_800F5B70.b = 0;
            D_800F5B70.g = 0;
            fade = effect->AnimationFrame * 448;
        } else if (effect->AnimationFrame >= 0x26) {
            fade = 0xE00 - (effect->AnimationFrame - 0x26) * 597;
            if (fade < 0) {
                fade = 0;
            }
        } else {
            fade = 0xE00;
        }
        if (effect->AnimationFrame == 0x1D) {
            for (i = 0; i < 10; i++) {
                if ((1 << i) & D_801E2FCC) {
                    func_800D5774(i);
                }
            }
        }
        if (effect->AnimationFrame >= 0x2E) {
            fade = 0;
            D_800F5B70.r = 0;
            effect->StartFrame = -1;
        }
        D_800F5B74 = fade;
        // match: removing the empty loop changes the code
        do {
        } while (0); // match: empty loop, see above
        effect->AnimationFrame++;
    }
}

extern SpriteRenderDesc D_801CAFAC;

void func_801B0914(void) {
    FaigaData* effect;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    BattleSetBillboardMatrix(&effect->Pos, effect->Scale, -effect->unk10);
    g_FaigaBufferPtr = func_800D4D90(&D_801CAFAC, (u_long**)&g_cDb->unk70, 12, g_FaigaBufferPtr);
    if (effect->AnimationFrame >= 0xC) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        switch (effect->AnimationFrame) {
        case 2:
        case 6:
            func_800D5138(1);
            break;
        case 3:
            BattleFixedPointRampReconfigure(2);
            break;
        case 4:
            effect->Scale = effect->unk16;
            goto skip; // match: switch with the subtract repeated per case, if/else and a flag local all failed
        case 7:
            BattleFixedPointRampReconfigure(4);
            break;
        case 0:
        case 1:
        case 5:
        case 8:
        case 9:
        case 10:
        case 11:
            break;
        default:
            goto skip; // match: as above
        }
        effect->Scale -= effect->unk14;
    skip:
        effect->AnimationFrame++;
    }
}

// Same as func_801B1988 in club2.c.
void func_801B0A38(void) {
    FaigaData* effect;
    s16 scale;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    g_FaigaSpriteDesc.frameIndex = effect->AnimationFrame;
    scale = effect->Scale;
    BattleSetBillboardMatrix(&effect->Pos, scale, -(scale >> 2));
    g_FaigaBufferPtr = func_800D4D90(&g_FaigaSpriteDesc, (u_long**)&g_cDb->unk70, 12, g_FaigaBufferPtr);
    if (effect->AnimationFrame >= 7) {
        effect->StartFrame = -1;
    }
    if (D_80062D98 == 0) {
        effect->AnimationFrame++;
    }
}

void func_801B0B04(void) {
    FaigaData* effect;
    FaigaData* child;
    s32 i;
    s32 range;
    s32 diameter;
    s32 step;

    effect = &g_BattleEffectSlots[g_BattleEffectCursor];
    if (D_80062D98 == 0) {
        if ((u32)((u16)effect->AnimationFrame - 3) < 0x22) {
            range = (s16)effect->Scale;
            step = (range << 12) / 650;
            range += range >> 1;
            diameter = range * 2;
            for (i = 0; i < 3; i++) {
                child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0A38)];
                child->Pos = effect->Pos;
                child->Pos.vx += rand() % diameter - range;
                child->Pos.vy += rand() % diameter - range;
                child->Pos.vz += rand() % diameter - range;
                child->Scale = (effect->unk14 + 0x800 + rand() % 0x1000) * step >> 12;
            }
            effect->unk14 += effect->unk16;
        }
        if (effect->AnimationFrame >= 0x2E) {
            effect->StartFrame = -1;
        }
        effect->AnimationFrame++;
    }
}

void func_801B0D48(void) {
    FaigaData* effect;
    FaigaData* child;
    s32 i;
    s32 count;
    s32 unit;
    s32 size;
    FaigaData* slots;

    i = 0;
    count = 0;
    for (; i < 10; i++) {
        if ((1 << i) & D_801E2FCC) {
            count++;
            if (count == 1) {
                unit = i;
            }
        }
    }
    g_BattleEffectSlots[g_BattleEffectCursor].StartFrame = -1;
    slots = g_BattleEffectSlots;
    effect = &slots[BattleEffectRegister(func_801B0914)];
    if (count == 1) {
        BattleGetPartPosition(unit, g_BattleModels[unit].boneIndices[0], &effect->Pos);
        size = g_BattleModels[unit].collisionRadius + 0x100;
        if (size > 0x76C) {
            size = 0x76C;
        }
    } else {
        BattleEntityGetCenter(D_801E2FCC, &effect->Pos);
        size = 0x600;
        effect->Pos.vy = -0x500;
    }
    effect->Scale = 0x5555;
    effect->unk14 = 0xAAA;
    effect->unk10 = size >> 1;
    effect->unk16 = effect->Scale;

    child = &g_BattleEffectSlots[BattleEffectRegister(func_801B062C)];
    child->Pos = effect->Pos;
    child->Scale = size;

    child = &g_BattleEffectSlots[BattleEffectRegister(func_801B0B04)];
    child->Pos = effect->Pos;
    child->Scale = size;
    child->unk14 = 0;
    child->unk16 = 0x80;
}

void func_801B0F28(s32 targetMask, s32 callbackArg) {
    g_FaigaFlipEffect = (s32*)&g_BattleEffectSlots[BattleEffectRegister(func_801B0030)];
    BattleSetLoadTimToVram(D_801B0FC0, 0, 0, 0);
    BattleEffectRegister(func_801B0D48);
    D_801E2FCC = targetMask;
    BattleAkaoCommand(AKAO_PLAY_SOUND, BattleEntityGetStereoPan(targetMask), 0x8E);
}
