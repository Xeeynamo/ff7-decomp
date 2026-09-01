//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern s32 (*g_FieldOpcodes[256])(void);
extern u8 g_EntityForSplitJoin;
extern char g_DebugMessageBuffer[];
extern u8 D_80114498[];

void FieldEventOpcodeCycle(void);
void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind);
void FieldWindowResetTextAll(void);
void FieldDebugStringCopy(char* dst, const char* src);
void InitFieldDebugPages(void);

/* Installs the field's state, model and script pointers, checks the script
 * header's version bytes, then brings the event system up. */
void FieldEventInit(
    FieldState* state, FieldEntity* models, FieldScriptHeader* scripts) {
    s32 flags;

    /* The high half of FieldState's 0x68 word. The low half is the
     * controller-1 key bits (see OpcodeFuncKeyEx, which matches against
     * activeKeys as a u32), so this cannot be a named field without splitting
     * that member. Widening to s32 is what makes the load lh rather than lhu:
     * held in an s16 the value is only ever masked, and gcc narrows it. */
    flags = *(s16*)((u8*)state + 0x6A);
    g_pFieldState = state;
    g_FieldModels = models;
    g_FieldScripts = scripts;
    g_FieldScriptHalted = 0;
    D_8007EBE0 = 1;
    D_8009FE8C = 0;
    if (flags & 0x100) {
        g_FieldScriptHalted = 1;
        g_FieldScriptRunState = 4;
    }
    if (scripts->eventDataVersion < 2) {
        SystemError('K', 10);
    }
    if (scripts->eventDataVersion > 2 || scripts->eventVersion > 5) {
        SystemError('K', 12);
    }
    if (scripts->eventVersion < 5) {
        SystemError('K', 11);
    }
    FieldWindowResetAll();
    FieldInitDefaultValues();
    FieldEventRunInit();
    if (g_FieldMusicLock == 0) {
        FieldEventClearAkaoStruct();
        *D_8009A000 = 0xF2;
        SystemAkaoExecute();
    }
}

void FieldEventUpdate(s32 arg0) {
    if (D_8007EBE0) {
        FieldWindowResetTextAll();
        ResetFieldRenderState();
        FieldDebugInitBuffers();
        InitFieldDebugPages();
        g_FieldScriptHalted = 0;
        D_8009FE8C = 0;
        D_8007EBE0 = 0;
        if (g_FieldScripts->eventVersion < 5) {
            SystemError('K', 11);
        }
        if (g_FieldScripts->eventDataVersion < 2) {
            SystemError('K', 10);
        }
        if (g_FieldScripts->eventDataVersion > 2 ||
            g_FieldScripts->eventVersion > 5) {
            SystemError('K', 12);
        }
    }
    if (g_FieldScriptRunState != 4) {
        if (g_FieldScriptRunState != 5 || g_FieldDebugStepRequest != 0) {
            FieldEventOpcodeCycle();
        }
    }
    if (g_WindowCount) {
        SystemMenuDrawDialog(
            g_WindowData, 4, arg0, g_pFieldState->renderBuffer ^ 1);
    }
    UpdateFieldExitArrows(arg0);
}

extern u8 D_8009A1C4[]; // per-entity, reset to 7
extern u8 D_8009AD38;   // top of a 9-byte block set to 0xFF downward

/* Zero and default-initialise the whole field runtime state: the entity table,
 * the per-model flags, the script state, and the various counters. */
void FieldInitDefaultValues(void) {
    s32 numModels;
    s32 i;
    s32 j;
    s8* p;
    u8* q;
    s16* cell;
    s32 off;
    u8 fill;

    g_pFieldState->eventCmd = 0;
    g_pFieldState->eventCmdParam = 0;
    g_pFieldState->movieCommandState = 0;
    numModels = g_FieldScripts->numModels;
    g_pFieldState->walkAnimId = 1;
    g_pFieldState->pcModelId = 0;
    g_pFieldState->idleAnimId = 0;
    g_pFieldState->runAnimId = 2;
    g_pFieldState->modelCount = numModels;
    g_pFieldState->characterLock = 0;
    D_80081DC4 = 0;
    g_pFieldState->suspendWalkAndAnim = 0;
    g_pFieldState->menuDisabled = 0;
    g_pFieldState->unk35 = 0;
    g_pFieldState->battlesDisabled = 0;
    g_pFieldState->mapJumpDisabled = 0;
    g_pFieldState->scrloSet = 0;
    g_pFieldState->nextFieldMusic = 0;
    g_pFieldState->nextBattleMusic = 0;
    g_pFieldState->unk40 = 0;
    g_pFieldState->battleMode2 = 0;
    g_pFieldState->battleMode1 = 0;
    g_pFieldState->encounterTableId = 0;
    g_pFieldState->viewOffsetNumSteps = 0;
    g_pFieldState->viewOffsetCurrentStep = 0;
    g_pFieldState->viewOffsetMode = 0;
    g_pFieldState->viewOffsetStart = 0;
    g_pFieldState->viewOffsetTarget = 0;
    g_pFieldState->shakeX.enabled = 0;
    g_pFieldState->shakeY.enabled = 0;
    g_pFieldState->shakeX.segmentActive = 0;
    g_pFieldState->shakeY.segmentActive = 0;
    g_pFieldState->shakeX.amplitude = 0;
    g_pFieldState->shakeY.amplitude = 0;
    g_pFieldState->shakeX.numStepsPerSegment = 0;
    g_pFieldState->shakeY.numStepsPerSegment = 0;
    g_pFieldState->shakeX.currentStep = 0;
    g_pFieldState->shakeY.currentStep = 0;
    g_pFieldState->backgroundMovieEnabled = 0;
    g_pFieldState->cameraScrollMode = 0;
    g_pFieldState->currentFieldScale = g_FieldScripts->scale;

    p = &D_80075F23;
    i = 0xFF;
    do {
        *p = 0;
        i--;
        p--;
    } while (i >= 0);

    for (i = 0; i < 8; i++) {
        for (j = 0; j < g_FieldScripts->numEntities; j++) {
            D_80071748[j][i] = 0;
            D_800833F8[j][i] = 0;
            D_80071A88[j][i] = 0xFF;
            SavedScriptIds[j][i] = 0;
        }
    }
    for (i = 0; i < g_FieldScripts->numEntities; i++) {
        D_8009A1C4[i] = 7;
        D_8007EB98[i] = 0xFF;
        D_800716DC[i] = 0;
        D_80081D90[i] = 0;
        g_EntityToLine[i] = 0xFF;
        g_FieldScriptDebugEntities[i] = 0;
    }
    for (i = 0; i < g_FieldScripts->numModels; i++) {
        g_FieldModels[i].charId = 0;
        g_FieldModels[i].PosX = 0;
        g_FieldModels[i].PosY = 0;
        g_FieldModels[i].PosZ = 0;
        g_FieldModels[i].PosI = 0;
        g_FieldModels[i].MoveEndI = 0;
        g_FieldModels[i].MoveDir = 0;
        g_FieldModels[i].Dir = 0;
        g_FieldModels[i].TurnType = 0;
        g_FieldModels[i].TurnSteps = 0;
        g_FieldModels[i].TurnStep = 0;
        g_FieldModels[i].TurnStart = 0;
        g_FieldModels[i].TurnEnd = 0;
        g_FieldModels[i].OffsetX = 0;
        g_FieldModels[i].OffsetY = 0;
        g_FieldModels[i].OffsetZ = 0;
        g_FieldModels[i].OffsetStartX = 0;
        g_FieldModels[i].OffsetStartY = 0;
        g_FieldModels[i].OffsetStartZ = 0;
        g_FieldModels[i].OffsetEndX = 0;
        g_FieldModels[i].OffsetEndY = 0;
        g_FieldModels[i].OffsetEndZ = 0;
        g_FieldModels[i].OffsetSteps = 0;
        g_FieldModels[i].OffsetStep = 0;
        g_FieldModels[i].OfsType = 0;
        g_FieldModels[i].activeAnimId = 0;
        g_FieldModels[i].animSpeed = 0x10;
        g_FieldModels[i].MoveEndX = 0;
        g_FieldModels[i].MoveEndY = 0;
        g_FieldModels[i].MoveEndZ = 0;
        g_FieldModels[i].animCurrentFrame = 0;
        g_FieldModels[i].animLastFrame = 0;
        g_FieldModels[i].visible = 0;
        g_FieldModels[i].MoveSpeed = g_pFieldState->currentFieldScale * 2;
        g_FieldModels[i].scriptedMoveMode = 0;
        g_FieldModels[i].ActionArg = 0;
        g_FieldModels[i].ActionState = 0;
        g_FieldModels[i].requestTalkScript = 0;
        g_FieldModels[i].requestPushScript = 0;
        g_FieldModels[i].SolidOff = 0;
        g_FieldModels[i].TalkOff = 0;
        g_FieldModels[i].DirLock = 0;
        g_FieldModels[i].SolidRange =
            g_pFieldState->currentFieldScale * 0x1E / 512;
        g_FieldModels[i].TalkRange =
            g_pFieldState->currentFieldScale * 0x50 / 512;
        g_FieldModelIdleAnimId[i] = 0;
        g_FieldModelAnimMode[i] = 0;
        D_8009D828[i] = 0x10;
        D_80082248[i] = 0x10;
        g_FieldModels[i].KawaiOp1 = 0;
        g_FieldModels[i].KawaiOp0 = 0;
        g_FieldModels[i].KawaiDataOffset = 0;
        g_FieldModels[i].BlinkOn = 0;
        g_FieldModels[i].KawaiA = 0;
    }
    i = 0;
    do {
        g_pFieldState->backgroundLayerVisibility[i] = 0;
        i++;
    } while (i < 0x40);
    i = 0;
    do {
        g_pFieldState->blockedAccesses[i] = 0;
        i++;
    } while (i < 0x40);
    i = 0;
    do {
        j = 0xF;
        cell = (s16*)&g_FieldPalettes[i * 0x20];
        do {
            cell[j] = 0;
            j--;
        } while (j >= 0);
        i++;
    } while (i < 0x40);
    i = 0;
    off = 0;
    do {
        *((u8*)&D_8007E7AC + 0x11 + off) = 0;
        *((u8*)&D_8007E7AC + 0x10 + off) = 0;
        *((u8*)&D_8007E7AC + 0xF + off) = 0;
        *((u8*)&D_8007E7AC + 0xE + off) = 0;
        *((u8*)&D_8007E7AC + 0x12 + off) = 0;
        *((u8*)&D_8007E7AC + 0x13 + off) = 0;
        *((u8*)&D_8007E7AC + 0xC + off) = 0;
        *((u8*)&D_8007E7AC + 0xD + off) = 0;
        *((u8*)&D_8007E7AC + 0x16 + off) = 0;
        *(s16*)((u8*)&D_8007E7AC + off) = 0;
        *(s16*)((u8*)&D_8007E7AC + 2 + off) = 0;
        *(s16*)((u8*)&D_8007E7AC + 4 + off) = 0;
        *(s16*)((u8*)&D_8007E7AC + 6 + off) = 0;
        *(s16*)((u8*)&D_8007E7AC + 8 + off) = 0;
        *(s16*)((u8*)&D_8007E7AC + 0xA + off) = 0;
        off += 0x18;
        i++;
    } while (i < 0x20);
    D_80095D84 = 0;
    fill = 0xFF;
    i = 8;
    q = &D_8009AD38;
    do {
        *q = fill;
        i--;
        q--;
    } while (i >= 0);
    g_EntityForSplitJoin = 0xFF;
    g_FieldMovieOpcodeActive = 0;
    Savemap.memory_bank_1[31] |= 3;
}

/* Walks every entity's first script and runs its initialisation opcodes
 * (everything up to the terminating 0). The script-offset table sits past the
 * entity-name table and the extras table in the script header. */
void FieldEventRunInit(void) {
    s32 numExtras;
    s32 numEnt;
    s32 slotIdx;
    u8* scriptBase;
    u8* pcBase;
    u16 pc;
    u16* slot;
    u16* slot2;
    u8 lo;
    u8 op;
    u8 op2;
    u16* pcTable;
    FieldScriptHeader* scripts;

    g_FieldModelCount = 0;
    g_CurrentEntity = 0;
    if (g_FieldScripts->numEntities != 0) {
        do {
            if (g_FieldScriptDebugFlags & 3) {
                FieldDebugStringCopy(g_DebugText, D_800E0628);
                FieldDebugStringConcat(g_DebugText, (u8*)g_FieldScripts + 0x20 +
                                                        (g_CurrentEntity * 8));
                if (g_FieldScriptDebugFlags & 1) {
                    SetStrToDebugRow(4, 0, g_DebugText);
                }
                if (g_FieldScriptDebugFlags & 2) {
                    DebugPrintToFieldWindow(g_DebugText);
                }
            }
            /* Below the debug block, so g_DebugText's address is the first
             * movable `move_movables' records and the preheader's hoists come
             * out in the target's order. The empty loop is a basic-block
             * boundary and nothing else: without it `pcBase' and the `slot'
             * use below are in one extended block, cse folds the address into
             * the subscript, and the inner loop's own materialisation of
             * g_FieldScriptPC goes with it. See the note above. */
            pcBase = (u8*)g_FieldScriptPC;
            do {
            } while (0);
            scripts = g_FieldScripts;
            numExtras = (s16)(scripts->numExtras * 4);
            slotIdx = g_CurrentEntity * 2;
            scriptBase = (u8*)(g_CurrentEntity << 6);
            numEnt = scripts->numEntities * 8;
            lo = (scriptBase + numEnt + numExtras + (s32)scripts)[0x20];
            slot = (u16*)(slotIdx + (s32)pcBase);
            *slot = (u16)lo;
            numEnt = scripts->numEntities * 8;
            *slot = lo | ((scriptBase + numEnt + (s32)scripts + numExtras)[0x21]
                          << 8);
            op = *((u8*)scripts + *slot);
            g_FieldCurrentOpcode = op;
            if (op != 0) {
                pcTable = g_FieldScriptPC;
                do {
                    g_FieldOpcodes[g_FieldCurrentOpcode]();
                    op2 = *((u8*)g_FieldScripts + pcTable[g_CurrentEntity]);
                    g_FieldCurrentOpcode = op2;
                } while (op2 != 0);
            }
            slot2 = (u16*)((g_CurrentEntity * 2) + (u8*)g_FieldScriptPC);
            pc = *slot2;
            g_CurrentEntity += 1;
            *slot2 = pc + 1;
        } while ((u8)g_CurrentEntity < (u8)g_FieldScripts->numEntities);
        g_CurrentEntity = 0;
    }
}

/* Enable the loaded field models that correspond to party members, then
 * disable (make non-solid, non-talkable, invisible) every model whose loader
 * slot was not claimed. */
void FieldEnablePartyModels(void) {
    s16 i;
    s16 j;
    u8 charId;
    u8 modelId;
    s32 entityModel;

    /* Mark the loader slot of each present party member's model as an NPC. */
    for (i = 0; i < 3; i++) {
        charId = Savemap.memory_bank_2[9 + i];
        if (charId == 0xFF) {
            continue;
        }
        modelId = g_CharIdToEntity[charId];
        if (modelId == 0xFF) {
            continue;
        }
        entityModel = g_EntityToModel[modelId];
        if (entityModel == 0xFF) {
            continue;
        }
        if (entityModel < (s32)((FieldModelFileDesc*)D_8007E770)->count) {
            g_FieldModelLoaderData[entityModel].npcFlag = 1;
        }
    }

    /* Disable every model whose loader slot was not claimed above. */
    for (i = 0; i < ((FieldModelFileDesc*)D_8007E770)->count; i++) {
        if (g_FieldModelLoaderData[i].npcFlag == 0) {
            for (j = 0; j < g_FieldScripts->numEntities; j++) {
                if (g_EntityToModel[j] == i) {
                    g_EntityToModel[j] = 0xFF;
                    g_FieldModels[i].visible = 0;
                    g_FieldModels[i].SolidOff = 1;
                    g_FieldModels[i].TalkOff = 1;
                }
            }
        }
    }
}

void FieldEventOpcodeCycle(void) {
    s32 i, j, count;
    u16 hours, seconds;
    s32 talkDone = 0;

    // Update display values for play time and countdown.
    hours = Savemap.time / 3600;
    if (hours > 255) {
        hours = 255;
    }
    Savemap.memory_bank_1[16] = hours;
    hours = Savemap.time % 3600;
    Savemap.memory_bank_1[17] = hours / 60;
    seconds = hours % 60;
    if (Savemap.memory_bank_1[18] != seconds) {
        Savemap.memory_bank_1[18] = seconds;
        Savemap.memory_bank_1[19] = 0;
    } else {
        Savemap.memory_bank_1[19]++;
    }

    hours = Savemap.countdown_timer_seconds / 3600;
    if (hours > 255) {
        hours = 255;
    }
    Savemap.memory_bank_1[20] = hours;
    hours = Savemap.countdown_timer_seconds % 3600;
    Savemap.memory_bank_1[21] = hours / 60;
    seconds = hours % 60;
    if (Savemap.memory_bank_1[22] != seconds) {
        Savemap.memory_bank_1[22] = seconds;
        Savemap.memory_bank_1[23] = 30;
    } else if (Savemap.memory_bank_1[23]) {
        Savemap.memory_bank_1[23]--;
    }

    count = g_FieldScripts->numModels;
    for (i = 0; i < count; i++) {
        if (g_FieldModels[i].requestTalkScript) {
            if (!g_pFieldState->characterLock && !talkDone) {
                FieldEventRequestRun(g_FieldModels[i].entityId, 1, 1);
                talkDone = 1;
            }
            g_FieldModels[i].requestTalkScript = 0;
        }
        if (g_FieldModels[i].requestPushScript) {
            FieldEventRequestRun(g_FieldModels[i].entityId, 1, 2);
            g_FieldModels[i].requestPushScript = 0;
        }
    }
    for (i = 0; i < g_FieldLineCount; i++) {
        if (g_FieldLines[i].requestTalkScript) {
            if (!g_pFieldState->characterLock) {
                FieldEventRequestRun(g_FieldLines[i].entityId, 1, 1);
            }
            g_FieldLines[i].requestTalkScript = 0;
        }
        if (g_FieldLines[i].requestPushScript) {
            FieldEventRequestRun(g_FieldLines[i].entityId, 1, 2);
            g_FieldLines[i].requestPushScript = 0;
        }
        if (g_FieldLines[i].across) {
            FieldEventRequestRun(g_FieldLines[i].entityId, 1, 3);
            g_FieldLines[i].across = 0;
        }
        if (g_FieldLines[i].requestTouchOnScript) {
            FieldEventRequestRun(g_FieldLines[i].entityId, 1, 5);
            g_FieldLines[i].requestTouchOnScript = 0;
        }
        if (g_FieldLines[i].requestTouchOffScript) {
            FieldEventRequestRun(g_FieldLines[i].entityId, 1, 6);
            g_FieldLines[i].requestTouchOffScript = 0;
        }
        if (g_FieldLines[i].touch) {
            FieldEventRequestRun(g_FieldLines[i].entityId, 1, 4);
        }
    }

    // Loop through all entities in field map and execute up to 8 opcodes of
    // each entity's active script.
    count = g_FieldScripts->numEntities;
    do {
        if (g_CurrentEntity >= g_FieldScripts->numEntities) {
            g_CurrentEntity = 0;
        }
        if (g_FieldScriptDebugFlags & 3) {
            DebugUpdateActor(4, g_CurrentEntity);
        }

        // Skip entities involved in a split or join animation
        // (g_EntitySplitJoinState[entity] != 0) except the entity they're
        // splitting from or joining to (g_EntityForSplitJoin).
        if (g_EntitySplitJoinState[g_CurrentEntity] == 0 ||
            g_EntityForSplitJoin == g_CurrentEntity) {
            for (j = 8; j != 0; j--) {
                if (g_FieldScriptRunState == 5 && g_DebugLevel & 1 &&
                    (!(g_FieldScriptDebugFlags & 4) ||
                     D_80114498[g_CurrentEntity])) {
                    for (i = 1; i < 9; i++) {
                        SetStrToDebugRow(3, i, "");
                    }
                }
                g_FieldCurrentOpcode =
                    ((u8*)g_FieldScripts)[g_FieldScriptPC[g_CurrentEntity]];

                // Script can yield early if opcode returns 1.
                if (g_FieldOpcodes[g_FieldCurrentOpcode]()) {
                    if (g_FieldScriptRunState == 5 && g_DebugLevel & 1 &&
                        (!(g_FieldScriptDebugFlags & 4) ||
                         D_80114498[g_CurrentEntity])) {
                        g_CurrentEntity++;
                        goto done;
                    }
                    break;
                }
                if (g_FieldScriptRunState == 5 && g_DebugLevel & 1 &&
                    (!(g_FieldScriptDebugFlags & 4) ||
                     D_80114498[g_CurrentEntity])) {
                    if (++g_FieldDebugStepCounter >= 8) {
                        g_FieldDebugStepCounter = 0;
                        g_CurrentEntity++;
                    }
                    goto done;
                }
            }
        }
        g_CurrentEntity++;
        count--;
        if (g_FieldScriptRunState == 5 && g_FieldScriptDebugFlags & 1 &&
            (!(g_FieldScriptDebugFlags & 4) || D_80114498[g_CurrentEntity])) {
            break;
        }
    } while (count != 0);

done:
    if (g_FieldScriptRunState == 5) {
        g_FieldDebugStepRequest = 0;
    }
    FieldUpdateAnimationState();
}

void FieldUpdateAnimationState(void) {
    s32 i;
    s32 modelEntryId;
    u8* anims;
    FieldModelEntry* model;

    for (i = 0; i < g_FieldScripts->numEntities; i++) {
        if (g_EntityToModel[i] != 0xFF &&
            (g_pFieldState->pcModelId != g_EntityToModel[i] ||
             g_pFieldState->characterLock)) {
            switch (g_FieldModelAnimMode[g_EntityToModel[i]]) {
            case 0:
                if (g_FieldModels[g_EntityToModel[i]].activeAnimId !=
                    g_FieldModelIdleAnimId[g_EntityToModel[i]]) {
                    g_FieldModels[g_EntityToModel[i]].activeAnimId =
                        g_FieldModelIdleAnimId[g_EntityToModel[i]];
                    g_FieldModels[g_EntityToModel[i]].animSpeed =
                        D_80082248[g_EntityToModel[i]];
                    g_FieldModels[g_EntityToModel[i]].animCurrentFrame = 0;
                    modelEntryId = g_FieldModelLoaderData[g_EntityToModel[i]]
                                       .modelEntryIndex;
                    model = &g_FieldModelData->modelEntries[modelEntryId];
                    anims = model->modelData + model->animationOffset;
                    g_FieldModels[g_EntityToModel[i]].animLastFrame =
                        *(u16*)&anims[g_FieldEntity[g_EntityToModel[i]]
                                          .activeAnimId *
                                      16] -
                        1;
                } else {
                    modelEntryId = g_FieldModelLoaderData[g_EntityToModel[i]]
                                       .modelEntryIndex;
                    model = &g_FieldModelData->modelEntries[modelEntryId];
                    anims = model->modelData + model->animationOffset;
                    g_FieldModels[g_EntityToModel[i]].animLastFrame =
                        *(u16*)&anims[g_FieldEntity[g_EntityToModel[i]]
                                          .activeAnimId *
                                      16] -
                        1;
                    if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >>
                         4) >=
                        g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                        g_FieldModels[g_EntityToModel[i]].animCurrentFrame = 0;
                    }
                }
                break;
            case 1:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    g_FieldModels[g_EntityToModel[i]].animCurrentFrame = 0;
                }
                break;
            case 2:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    g_FieldModelAnimMode[g_EntityToModel[i]] = 4;
                    g_FieldModels[g_EntityToModel[i]].animCurrentFrame =
                        g_FieldModels[g_EntityToModel[i]].animLastFrame << 4;
                }
                break;
            case 3:
            case 4:
                g_FieldModels[g_EntityToModel[i]].animCurrentFrame =
                    g_FieldModels[g_EntityToModel[i]].animLastFrame << 4;
                break;
            case 5:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    g_FieldModelAnimMode[g_EntityToModel[i]] = 0;
                }
                break;
            case 6:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    g_FieldModelAnimMode[g_EntityToModel[i]] = 3;
                }
                break;
            }
        }
    }
}

u8 FieldEventRequestRun(s16 entityId, s16 priority, s16 scriptId) {
    u16 offset;
    s32 scriptOffset;
    s32 entityDataSize;
    s32 extrasHeaderSize;

    if (g_DebugLevel & 3) {
        switch (scriptId) {
        case 1: // Pressed OK.
            FieldDebugStringCopy(g_DebugMessageBuffer, "Talk=");
            break;
        case 2: // Pushed / within entity's collision radius.
            FieldDebugStringCopy(g_DebugMessageBuffer, "Push=");
            break;
        case 3: // Across line.
            FieldDebugStringCopy(g_DebugMessageBuffer, "Acrs=");
            break;
        case 4: // Touching line.
            FieldDebugStringCopy(g_DebugMessageBuffer, "Toch=");
            break;
        case 5: // Started touching line.
            FieldDebugStringCopy(g_DebugMessageBuffer, "TochON =");
            break;
        case 6: // Ended touching line.
            FieldDebugStringCopy(g_DebugMessageBuffer, "TochOFF=");
            break;
        }
        // Prints entity name.
        FieldDebugStringConcat(
            g_DebugMessageBuffer,
            (char*)g_FieldScripts + sizeof(FieldScriptHeader) + entityId * 8);
        FieldDebugAddParseValueToPage2(g_DebugMessageBuffer, 0, 0);
    }

    // Only request script if active script has lower priority.
    if (g_FieldScriptPriority[entityId] > priority) {

        // Entity is busy waiting for another script to return.
        if (g_FieldScriptSyncState[entityId][priority] != SYNC_NONE) {
            return g_FieldScriptSyncState[entityId][priority];
        }

        scriptOffset = scriptId * 2;
        extrasHeaderSize = (s16)(g_FieldScripts->numExtras * 4);
        entityDataSize = entityId * 64;
        entityDataSize += g_FieldScripts->numEntities * 8;

        GET_FIELD_SCRIPT_PC(
            offset, scriptOffset, entityDataSize, extrasHeaderSize);

        // Empty event scripts consist of just a RET (0x00) opcode.
        if (((u8*)g_FieldScripts)[offset] != 0) {

            // Save position of current active script of lower priority and
            // replace with new script.
            SavedScriptIds[entityId][priority] = scriptId;
            g_SavedFieldScriptPC[entityId][g_FieldScriptPriority[entityId]] =
                g_FieldScriptPC[entityId];
            g_FieldScriptPC[entityId] = offset;
            g_FieldScriptPriority[entityId] = priority;

            // Clear running animation if entity has a model.
            if (g_EntityToModel[entityId] != 0xFF) {
                if (g_FieldModels[g_EntityToModel[entityId]].scriptedMoveMode ==
                    SMODE_WALK) {
                    g_FieldModels[g_EntityToModel[entityId]].activeAnimId = 0;
                    g_FieldModels[g_EntityToModel[entityId]].animCurrentFrame =
                        0;
                    g_FieldModels[g_EntityToModel[entityId]].animLastFrame = 0;
                }
                g_FieldModels[g_EntityToModel[entityId]].scriptedMoveMode =
                    SMODE_NONE;
            }

            g_FieldWaitCounter[entityId] = 0;

            if (g_DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("=recieved", 0, 0);
            }
        } else {
            if (g_DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("=ret", 0, 0);
            }
        }
        return 1;
    }

    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("=ignored", 0, 0);
    }
    return 0;
}

void ResetFieldRenderState(void) {
    s16 tpage;

    g_FieldExitArrowPktIdx = 0;
    g_FieldExitArrowX = 0x7FFF;
    g_FieldExitArrowY = 0x7FFF;
    setPolyFT4(&g_FieldExitArrowPkts[0]);
    setPolyFT4(&g_FieldExitArrowPkts[1]);
    setSemiTrans(&g_FieldExitArrowPkts[0], 0);
    setSemiTrans(&g_FieldExitArrowPkts[1], 0);
    setShadeTex(&g_FieldExitArrowPkts[0], 1);
    setShadeTex(&g_FieldExitArrowPkts[1], 1);
    if (GetGraphType() == 1 || GetGraphType() == 2) {
        tpage = 0x2F;
    } else {
        tpage = 0x1F;
    }
    g_FieldExitArrowPkts[1].tpage = tpage;
    g_FieldExitArrowPkts[0].tpage = tpage;
    g_FieldExitArrowPkts[1].clut = 0x7850;
    g_FieldExitArrowPkts[0].clut = 0x7850;
    g_FieldExitArrowPkts[0].r0 = 0;
    g_FieldExitArrowPkts[1].r0 = 0;
    g_FieldExitArrowPkts[0].g0 = 0;
    g_FieldExitArrowPkts[1].g0 = 0;
    g_FieldExitArrowPkts[0].b0 = 0;
    g_FieldExitArrowPkts[1].b0 = 0;
}

/* Select toggles the exit arrows on and off (bit 0); bit 1 is a debug override
 * that shows them regardless of the toggle and of the movement lock. */
void UpdateFieldExitArrows(s32 arg0) {
    if (g_pFieldState->pressedKeys & (1 << 8)) {
        g_FieldExitArrowState[0] ^= 1;
    }
    if (((g_FieldExitArrowState[0] == 1) &&
         (g_pFieldState->characterLock == 0)) ||
        (g_FieldExitArrowState[0] & 2)) {
        DrawFieldExitArrow(arg0);
    }
}

/* Draw the field-exit arrow: a single textured quad at the exit's projected
 * screen position, double-buffered through g_FieldExitArrowPktIdx so the packet
 * being added is never the one the GPU is reading. The clamp keeps it on
 * screen, and the two `if`s pick which corner of the 16x16 texture cell maps to
 * which vertex -- the arrow flips horizontally past x = 0x123 and vertically
 * above y = 0x11, so it always points inward from the edge it sits on. */
void DrawFieldExitArrow(s32* ot) {
    s16 x;
    s16 y;

    if (g_FieldMovieOpcodeActive == 0 &&
        (g_FieldExitArrowX != 0x7FFF ||
         g_FieldExitArrowY != g_FieldExitArrowX)) {
        x = g_FieldExitArrowX;
        if (g_FieldExitArrowX >= 0x141) {
            x = 0x140;
        }
        if (g_FieldExitArrowX < 0) {
            x = 0;
        }
        if (g_FieldExitArrowY >= 0xE1) {
            y = 0xE0;
        } else {
            y = g_FieldExitArrowY;
        }
        if (g_FieldExitArrowY < 0) {
            y = 0;
        }
        g_FieldExitArrowPktIdx ^= 1;
        if (x >= 0x123) {
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].u0 = 0x8F;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].u1 = 0x7F;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].u2 = 0x8F;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].u3 = 0x7F;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].x0 = x - 0x10;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].x1 = x;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].x2 = x - 0x10;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].x3 = x;
        } else {
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].u0 = 0x80;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].u1 = 0x90;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].u2 = 0x80;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].u3 = 0x90;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].x0 = x;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].x1 = x + 0x10;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].x2 = x;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].x3 = x + 0x10;
        }
        if (y < 0x11) {
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].v0 = 0x6F;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].v1 = 0x6F;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].v2 = 0x5F;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].v3 = 0x5F;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].y0 = y;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].y1 = y;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].y2 = y + 0x10;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].y3 = y + 0x10;
        } else {
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].v0 = 0x60;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].v1 = 0x60;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].v2 = 0x70;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].v3 = 0x70;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].y0 = y - 0x10;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].y1 = y - 0x10;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].y2 = y;
            g_FieldExitArrowPkts[g_FieldExitArrowPktIdx].y3 = y;
        }
        addPrim(ot, &g_FieldExitArrowPkts[g_FieldExitArrowPktIdx]);
    }
}
