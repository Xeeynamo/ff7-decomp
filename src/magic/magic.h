#ifndef MAGIC_H
#define MAGIC_H

#include <game.h>

// Public interface of the MAGIC/*.BIN effect overlays, consumed by battle.
// Every overlay is loaded to 0x801B0000, so at most one is ever resident.

// An overlay entrypoint, as stored in battle's dispatch tables. The result
// form is for the limit break table, whose caller keeps the returned v0.
typedef void (*MagicEntry)(s32 targetMask, s32 callbackArg);
typedef s32 (*MagicEntryResult)(s32 targetMask, s32 callbackArg);

// What a summon entrypoint returns; battle keeps it in D_800F57D0.
typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk800F57D0;

void MAGIC_Fire(s32 targetMask, s32 callbackArg);
void MAGIC_Faira(s32 targetMask, s32 callbackArg);
void MAGIC_Brizad(s32 targetMask, s32 callbackArg);
void MAGIC_Brizara(s32 targetMask, s32 callbackArg);
void MAGIC_Thunder(s32 targetMask, s32 callbackArg);
void MAGIC_Thundera(s32 targetMask, s32 callbackArg);
void MAGIC_Barrier(s32 targetMask, s32 callbackArg);
void MAGIC_MBarrier(s32 targetMask, s32 callbackArg);
void MAGIC_Refrec(s32 targetMask, s32 callbackArg);
void MAGIC_Lv5Death(s32 targetMask, s32 callbackArg);
Unk800F57D0* MAGIC_Choco0(s32 targetMask, s32 callbackArg);

#endif
