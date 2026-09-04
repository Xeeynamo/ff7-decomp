//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern s32 (*g_FieldOpcodes[256])(void);
extern u8 g_EntityForSplitJoin;
extern char g_DebugMessageBuffer[];
extern u8 D_80114498[48];
extern u8 g_PosCursorDisabled;
extern POLY_FT4 g_PosCursorPrims[2];
extern s16 g_PosCursorX;
extern s16 g_PosCursorY;
extern u8 g_PosCursorPrimId;
extern char D_800E0628[]; // "Actor:"
extern char g_DebugText[];
extern u8 D_800716D4;

void DebugPrintToFieldWindow(const char* str);
void FieldEventOpcodeCycle(void);
void FieldUpdateAnimationState(void);
u8 FieldEventRequestRun(s16 entityId, s16 priority, s16 scriptId);
void DebugUpdateActor(s32 arg0, u8 actorId);
void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind);
void FieldWindowResetTextAll(void);
void SetStrToDebugRow(s32 page, s16 row, const char* str);
void FieldDebugStringCopy(char* dst, const char* src);
void FieldDebugStringConcat(char* dest, char* src);
void InitFieldDebugPages(void);
void FieldEventClearAkaoStruct(void);
void FieldWindowResetAll(void);
void FieldInitDefaultValues(void);
void FieldEventRunInit(void);
void ResetPositionCursorPrimitives(void);
void UpdatePositionCursor(OT_TYPE* ot);
void DrawPositionCursor(OT_TYPE* ot);

void FieldEventInit(FieldState* fieldState, FieldEntity* fieldModels,
                    FieldScriptHeader* fieldScripts) {
    g_pFieldState = fieldState;
    g_FieldModels = fieldModels;
    g_FieldScripts = fieldScripts;
    D_80095DCC = 0;
    D_8007EBE0 = 1;
    D_8009FE8C = 0;
    if ((fieldState->activeKeysRaw >> 16) & PADselect) {
        D_80095DCC = 1;
        D_80099FFC = 4;
    }
    if (fieldScripts->eventDataVersion < 2) {
        SystemError('K', 10);
    }
    if (fieldScripts->eventDataVersion > 2 || fieldScripts->eventVersion > 5) {
        SystemError('K', 12);
    }
    if (fieldScripts->eventVersion < 5) {
        SystemError('K', 11);
    }
    FieldWindowResetAll();
    FieldInitDefaultValues();
    FieldEventRunInit();
    if (!D_800716D4) {
        FieldEventClearAkaoStruct();
        D_8009A000[0] = 242;
        SystemAkaoExecute();
    }
}

void FieldEventUpdate(OT_TYPE* ot) {
    if (D_8007EBE0) {
        FieldWindowResetTextAll();
        ResetPositionCursorPrimitives();
        FieldDebugInitBuffers();
        InitFieldDebugPages();
        D_80095DCC = 0;
        D_8009FE8C = 0;
        D_8007EBE0 = 0;
        if (g_FieldScripts->eventVersion < 5) {
            SystemError('K', 11);
        }
        if (g_FieldScripts->eventDataVersion < 2) {
            SystemError('K', 10);
        }
        if (g_FieldScripts->eventDataVersion > 2 || g_FieldScripts->eventVersion > 5) {
            SystemError('K', 12);
        }
    }
    if (D_80099FFC != 4) {
        if (D_80099FFC != 5 || D_80070788 != 0) {
            FieldEventOpcodeCycle();
        }
    }
    if (g_WindowCount) {
        SystemMenuDrawDialog(
            g_WindowData, 4, ot, g_pFieldState->renderBuffer ^ 1);
    }
    UpdatePositionCursor(ot);
}

void FieldInitDefaultValues(void) {
    s32 i, j;

    g_pFieldState->eventCmd = EVTCMD_NONE;
    g_pFieldState->eventCmdParam = 0;
    g_pFieldState->movieCommandState = MOVCMD_IDLE;
    g_pFieldState->modelCount = g_FieldScripts->numModels;
    g_pFieldState->pcModelId = 0;
    g_pFieldState->idleAnimId = 0;
    g_pFieldState->walkAnimId = 1;
    g_pFieldState->runAnimId = 2;
    g_pFieldState->characterLock = 0;
    g_CharacterLock = 0;
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
    g_pFieldState->viewOffsetMode = OMODE_INSTANT;
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
    g_pFieldState->cameraScrollMode = SCRL_OFF;
    g_pFieldState->currentFieldScale = g_FieldScripts->scale;

    for (i = 0; i < 256; i++) {
        g_FieldMapVars[i] = 0;
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < g_FieldScripts->numEntities; j++) {
            g_SavedFieldScriptPC[j][i] = 0;
            g_FieldScriptSyncState[j][i] = SYNC_NONE;
            g_FieldScriptSyncWaitEntity[j][i] = 0xFF;
            SavedScriptIds[j][i] = 0;
        }
    }

    for (i = 0; i < g_FieldScripts->numEntities; i++) {
        g_FieldScriptPriority[i] = 7;
        g_EntityToModel[i] = 0xFF;
        g_FieldWaitCounter[i] = 0;
        g_EntitySplitJoinState[i] = 0;
        g_EntityToLine[i] = 0xFF;
        D_80114498[i] = 0;
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
        g_FieldModels[i].OfsType = OMODE_INSTANT;
        g_FieldModels[i].activeAnimId = 0;
        g_FieldModels[i].animSpeed = 16;
        g_FieldModels[i].MoveEndX = 0;
        g_FieldModels[i].MoveEndY = 0;
        g_FieldModels[i].MoveEndZ = 0;
        g_FieldModels[i].animCurrentFrame = 0;
        g_FieldModels[i].animLastFrame = 0;
        g_FieldModels[i].visible = 0;
        g_FieldModels[i].MoveSpeed =
            1024 * g_pFieldState->currentFieldScale / 512;
        g_FieldModels[i].scriptedMoveMode = SMODE_NONE;
        g_FieldModels[i].ActionArg = 0;
        g_FieldModels[i].ActionState = 0;
        g_FieldModels[i].requestTalkScript = 0;
        g_FieldModels[i].requestPushScript = 0;
        g_FieldModels[i].SolidOff = 0;
        g_FieldModels[i].TalkOff = 0;
        g_FieldModels[i].DirLock = 0;
        g_FieldModels[i].SolidRange =
            30 * g_pFieldState->currentFieldScale / 512;
        g_FieldModels[i].TalkRange =
            80 * g_pFieldState->currentFieldScale / 512;
        g_FieldModelAnimId[i] = 0;
        g_FieldModelAnimStatus[i] = ANIMSTATUS_DEFAULT_LOOP;
        g_FieldModelBaseAnimSpeed[i] = 16;
        g_FieldModelEffAnimSpeed[i] = 16;
        g_FieldModels[i].KawaiOp1 = 0;
        g_FieldModels[i].KawaiOp0 = 0;
        g_FieldModels[i].KawaiDataOffset = 0;
        g_FieldModels[i].BlinkOn = 0;
        g_FieldModels[i].KawaiA = 0;
    }

    for (i = 0; i < 64; i++) {
        g_pFieldState->backgroundLayerVisibility[i] = 0;
    }
    for (i = 0; i < 64; i++) {
        g_pFieldState->blockedAccesses[i] = 0;
    }
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 16; j++) {
            g_FieldPaletteBuffer[i][j] = 0;
        }
    }

    for (i = 0; i < 32; i++) {
        g_FieldLines[i].requestTalkScript = 0;
        g_FieldLines[i].requestPushScript = 0;
        g_FieldLines[i].across = 0;
        g_FieldLines[i].touch = 0;
        g_FieldLines[i].requestTouchOnScript = 0;
        g_FieldLines[i].requestTouchOffScript = 0;
        g_FieldLines[i].isActive = 0;
        g_FieldLines[i].entityId = 0;
        g_FieldLines[i].slipDisabled = 0;
        g_FieldLines[i].pos.x1 = 0;
        g_FieldLines[i].pos.y1 = 0;
        g_FieldLines[i].pos.z1 = 0;
        g_FieldLines[i].pos.x2 = 0;
        g_FieldLines[i].pos.y2 = 0;
        g_FieldLines[i].pos.z2 = 0;
    }
    g_FieldLineCount = 0;
    for (i = 0; i < 9; i++) {
        g_CharIdToEntity[i] = 0xFF;
    }
    g_EntityForSplitJoin = 0xFF;
    g_PosCursorDisabled = 0;
    Savemap.memory_bank_1[31] |= 3; // Locks PHS and save menus
}

void FieldEventRunInit(void) {
    g_FieldModelCount = 0;
    for (g_CurrentEntity = 0; g_CurrentEntity < g_FieldScripts->numEntities;
         g_CurrentEntity++) {
        s16 extrasHeaderSize;

        if (D_80071E24 & 3) {
            FieldDebugStringCopy(g_DebugText, D_800E0628);
            FieldDebugStringConcat(
                g_DebugText, GET_ENTITY_NAME(g_CurrentEntity));
            if (D_80071E24 & 1) {
                SetStrToDebugRow(4, 0, g_DebugText);
            }
            if (D_80071E24 & 2) {
                DebugPrintToFieldWindow(g_DebugText);
            }
        }

        extrasHeaderSize = g_FieldScripts->numExtras * 4;
        GET_FIELD_SCRIPT_OFFSET(
            g_FieldScriptPC[g_CurrentEntity], 0,
            8 * g_FieldScripts->numEntities + 64 * g_CurrentEntity,
            extrasHeaderSize);

        // Keep executing the init script until we reach 0x00 = RET opcode.
        while ((g_FieldCurrentOpcode =
                    ((u8*)g_FieldScripts)[g_FieldScriptPC[g_CurrentEntity]]) !=
               0) {
            g_FieldOpcodes[g_FieldCurrentOpcode]();
        }
        g_FieldScriptPC[g_CurrentEntity]++; // Advance PC to the start of the
                                            // next script.
    }
    g_CurrentEntity = 0;
}

extern void* D_8007E770;
extern /*?*/s32 D_8007EB98;
extern /*?*/s32 D_8009AD30;
extern s32 D_8009C544;
extern void* D_8009C6DC;

void FieldEnablePartyModels(void)
{
    s16 temp_v0_3;
    s16 var_a1;
    s16 var_a2;
    s16 var_a2_2;
    s16 var_v0_2;
    s16 var_v0_4;
    s32 temp_a0_2;
    s32 temp_v0_4;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_3;
    s32 var_v0_5;
    u8 temp_a0;
    u8 temp_a0_3;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 temp_v1;

    var_a2 = 0;
    var_v0 = 0 << 0x10;
    do {
        temp_v0 = D_8009D391[var_v0 >> 0x10];
        if ((temp_v0 != 0xFF) && (temp_v0_2 = *(&D_8009AD30 + temp_v0), (temp_v0_2 != 0xFF))) {
            temp_a0 = *(&D_8007EB98 + temp_v0_2);
            var_v0_2 = var_a2 + 1;
            if (temp_a0 != 0xFF) {
                var_v0_2 = var_a2 + 1;
                if ((s32) temp_a0 < (s32) D_8007E770->unk2) {
                    g_FieldModelLoaderData[temp_a0].npcFlag = 1;
                    goto block_6;
                }
            }
        } else {
block_6:
            var_v0_2 = var_a2 + 1;
        }
        var_a2 = var_v0_2;
        var_v0 = var_a2 << 0x10;
    } while (var_v0_2 < 3);
    var_a2_2 = 0;
    if (D_8007E770->unk2 != 0) {
        var_v0_3 = 0 << 0x10;
        do {
            temp_a0_2 = var_v0_3 >> 0x10;
            temp_v1 = g_FieldModelLoaderData[temp_a0_2].npcFlag;
            var_v0_4 = var_a2_2 + 1;
            if (temp_v1 == 0) {
                var_a1 = 0;
                if ((s32) temp_v1 < (s32) D_8009C6DC->unk2) {
                    var_v0_5 = 0 << 0x10;
                    do {
                        temp_v0_4 = var_v0_5 >> 0x10;
                        temp_a0_3 = *(&D_8007EB98 + temp_v0_4);
                        if (temp_a0_3 == temp_a0_2) {
                            *(&D_8007EB98 + temp_v0_4) = 0xFF;
                            temp_v1_2 = temp_a0_3 * 0x84;
                            (temp_v1_2 + D_8009C544)->unk5C = 0;
                            (temp_v1_2 + D_8009C544)->unk59 = 1;
                            (temp_v1_2 + D_8009C544)->unk5B = 1;
                        }
                        temp_v0_3 = var_a1 + 1;
                        var_a1 = temp_v0_3;
                        var_v0_5 = var_a1 << 0x10;
                    } while (temp_v0_3 < (s32) D_8009C6DC->unk2);
                }
                var_v0_4 = var_a2_2 + 1;
            }
            var_a2_2 = var_v0_4;
            var_v0_3 = var_a2_2 << 0x10;
        } while (var_v0_4 < (s32) D_8007E770->unk2);
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
        if (D_80071E24 & 3) {
            DebugUpdateActor(4, g_CurrentEntity);
        }

        // Skip entities involved in a split or join animation
        // (g_EntitySplitJoinState[entity] != 0) except the entity they're
        // splitting from or joining to (g_EntityForSplitJoin).
        if (g_EntitySplitJoinState[g_CurrentEntity] == 0 || g_EntityForSplitJoin == g_CurrentEntity) {
            for (j = 8; j != 0; j--) {
                if (D_80099FFC == 5 && g_DebugLevel & 1 && (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity])) {
                    for (i = 1; i < 9; i++) {
                        SetStrToDebugRow(3, i, "");
                    }
                }
                g_FieldCurrentOpcode = ((u8*)g_FieldScripts)[g_FieldScriptPC[g_CurrentEntity]];

                // Script can yield early if opcode returns 1.
                if (g_FieldOpcodes[g_FieldCurrentOpcode]()) {
                    if (D_80099FFC == 5 && g_DebugLevel & 1 && (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity])) {
                        g_CurrentEntity++;
                        goto done;
                    }
                    break;
                }
                if (D_80099FFC == 5 && g_DebugLevel & 1 && (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity])) {
                    if (++D_8009A064 >= 8) {
                        D_8009A064 = 0;
                        g_CurrentEntity++;
                    }
                    goto done;
                }
            }
        }
        g_CurrentEntity++;
        count--;
        if (D_80099FFC == 5 && D_80071E24 & 1 && (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity])) {
            break;
        }
    } while (count != 0);

done:
    if (D_80099FFC == 5) {
        D_80070788 = 0;
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
            switch (g_FieldModelAnimStatus[g_EntityToModel[i]]) {
            case ANIMSTATUS_DEFAULT_LOOP:
                if (g_FieldModels[g_EntityToModel[i]].activeAnimId !=
                    g_FieldModelAnimId[g_EntityToModel[i]]) {
                    g_FieldModels[g_EntityToModel[i]].activeAnimId =
                        g_FieldModelAnimId[g_EntityToModel[i]];
                    g_FieldModels[g_EntityToModel[i]].animSpeed =
                        g_FieldModelEffAnimSpeed[g_EntityToModel[i]];
                    g_FieldModels[g_EntityToModel[i]].animCurrentFrame = 0;
                    modelEntryId = g_FieldModelLoaderData[g_EntityToModel[i]].modelEntryIndex;
                    model = &g_FieldModelData->modelEntries[modelEntryId];
                    anims = model->modelData + model->animationOffset;
                    g_FieldModels[g_EntityToModel[i]].animLastFrame =
                        *(u16*)&anims[g_FieldEntity[g_EntityToModel[i]].activeAnimId * 16] - 1;
                } else {
                    modelEntryId = g_FieldModelLoaderData[g_EntityToModel[i]].modelEntryIndex;
                    model = &g_FieldModelData->modelEntries[modelEntryId];
                    anims = model->modelData + model->animationOffset;
                    g_FieldModels[g_EntityToModel[i]].animLastFrame =
                        *(u16*)&anims[g_FieldEntity[g_EntityToModel[i]].activeAnimId * 16] - 1;
                    if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                        g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                        g_FieldModels[g_EntityToModel[i]].animCurrentFrame = 0;
                    }
                }
                break;
            case ANIMSTATUS_SCRIPTED_LOOP:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    g_FieldModels[g_EntityToModel[i]].animCurrentFrame = 0;
                }
                break;
            case ANIMSTATUS_PLAY_ONCE_SYNC:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    g_FieldModelAnimStatus[g_EntityToModel[i]] =
                        ANIMSTATUS_PLAY_ONCE_SYNC_DONE;
                    g_FieldModels[g_EntityToModel[i]].animCurrentFrame =
                        g_FieldModels[g_EntityToModel[i]].animLastFrame << 4;
                }
                break;
            case ANIMSTATUS_HOLD_FRAME:
            case ANIMSTATUS_PLAY_ONCE_SYNC_DONE:
                g_FieldModels[g_EntityToModel[i]].animCurrentFrame =
                    g_FieldModels[g_EntityToModel[i]].animLastFrame << 4;
                break;
            case ANIMSTATUS_PLAY_ONCE_THEN_RESET:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    g_FieldModelAnimStatus[g_EntityToModel[i]] =
                        ANIMSTATUS_DEFAULT_LOOP;
                }
                break;
            case ANIMSTATUS_PLAY_ONCE_THEN_HOLD:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    g_FieldModelAnimStatus[g_EntityToModel[i]] =
                        ANIMSTATUS_HOLD_FRAME;
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

        FieldDebugStringConcat(g_DebugMessageBuffer, GET_ENTITY_NAME(entityId));
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

        GET_FIELD_SCRIPT_OFFSET(
            offset, scriptOffset, entityDataSize, extrasHeaderSize);

        // Empty event scripts consist of just a RET (0x00) opcode.
        if (((u8*)g_FieldScripts)[offset] != 0) {

            // Save position of current active script of lower priority and
            // replace with new script.
            SavedScriptIds[entityId][priority] = scriptId;
            g_SavedFieldScriptPC[entityId][g_FieldScriptPriority[entityId]] = g_FieldScriptPC[entityId];
            g_FieldScriptPC[entityId] = offset;
            g_FieldScriptPriority[entityId] = priority;

            // Clear running animation if entity has a model.
            if (g_EntityToModel[entityId] != 0xFF) {
                if (g_FieldModels[g_EntityToModel[entityId]].scriptedMoveMode == SMODE_WALK) {
                    g_FieldModels[g_EntityToModel[entityId]].activeAnimId = 0;
                    g_FieldModels[g_EntityToModel[entityId]].animCurrentFrame = 0;
                    g_FieldModels[g_EntityToModel[entityId]].animLastFrame = 0;
                }
                g_FieldModels[g_EntityToModel[entityId]].scriptedMoveMode = SMODE_NONE;
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

void ResetPositionCursorPrimitives(void) {
    s16 tpage;

    g_PosCursorX = 32767;
    g_PosCursorY = 32767;
    g_PosCursorPrimId = 0;
    setPolyFT4(&g_PosCursorPrims[0]);
    setPolyFT4(&g_PosCursorPrims[1]);
    setSemiTrans(&g_PosCursorPrims[0], 0);
    setSemiTrans(&g_PosCursorPrims[1], 0);
    setShadeTex(&g_PosCursorPrims[0], 1);
    setShadeTex(&g_PosCursorPrims[1], 1);
    tpage = getTPage(0, 0, 960, 256);
    g_PosCursorPrims[1].tpage = tpage;
    g_PosCursorPrims[0].tpage = tpage;
    g_PosCursorPrims[1].clut = getClut(256, 481);
    g_PosCursorPrims[0].clut = getClut(256, 481);
    g_PosCursorPrims[0].r0 = 0;
    g_PosCursorPrims[1].r0 = 0;
    g_PosCursorPrims[0].g0 = 0;
    g_PosCursorPrims[1].g0 = 0;
    g_PosCursorPrims[0].b0 = 0;
    g_PosCursorPrims[1].b0 = 0;
}

void UpdatePositionCursor(OT_TYPE* ot) {
    if (g_pFieldState->pressedKeys & PADselect) {
        Savemap.memory_bank_4[30] ^= 1;
    }
    if (((Savemap.memory_bank_4[30] == 1) && (!g_pFieldState->characterLock)) ||
        Savemap.memory_bank_4[30] & 2) {
        DrawPositionCursor(ot);
    }
}

void DrawPositionCursor(OT_TYPE* ot) {
    s16 x, y;

    if (!g_PosCursorDisabled &&
        ((g_PosCursorX != 32767) || (g_PosCursorY != 32767))) {
        if (g_PosCursorX > 320) {
            x = 320;
        } else {
            x = g_PosCursorX;
        }
        if (g_PosCursorX < 0) {
            x = 0;
        }
        if (g_PosCursorY > 224) {
            y = 224;
        } else {
            y = g_PosCursorY;
        }
        if (g_PosCursorY < 0) {
            y = 0;
        }
        g_PosCursorPrimId ^= 1;
        // Rotate cursor about the Y axis when player is on right edge of
        // screen.
        if (x > 290) {
            g_PosCursorPrims[g_PosCursorPrimId].u0 = 143;
            g_PosCursorPrims[g_PosCursorPrimId].u1 = 127;
            g_PosCursorPrims[g_PosCursorPrimId].u2 = 143;
            g_PosCursorPrims[g_PosCursorPrimId].u3 = 127;
            g_PosCursorPrims[g_PosCursorPrimId].x0 = x - 16;
            g_PosCursorPrims[g_PosCursorPrimId].x1 = x;
            g_PosCursorPrims[g_PosCursorPrimId].x2 = x - 16;
            g_PosCursorPrims[g_PosCursorPrimId].x3 = x;
        } else {
            g_PosCursorPrims[g_PosCursorPrimId].u0 = 128;
            g_PosCursorPrims[g_PosCursorPrimId].u1 = 144;
            g_PosCursorPrims[g_PosCursorPrimId].u2 = 128;
            g_PosCursorPrims[g_PosCursorPrimId].u3 = 144;
            g_PosCursorPrims[g_PosCursorPrimId].x0 = x;
            g_PosCursorPrims[g_PosCursorPrimId].x1 = x + 16;
            g_PosCursorPrims[g_PosCursorPrimId].x2 = x;
            g_PosCursorPrims[g_PosCursorPrimId].x3 = x + 16;
        }
        // Rotate cursor about the X axis when player is at the top of the
        // screen.
        if (y <= 16) {
            g_PosCursorPrims[g_PosCursorPrimId].v0 = 111;
            g_PosCursorPrims[g_PosCursorPrimId].v1 = 111;
            g_PosCursorPrims[g_PosCursorPrimId].v2 = 95;
            g_PosCursorPrims[g_PosCursorPrimId].v3 = 95;
            g_PosCursorPrims[g_PosCursorPrimId].y0 = y;
            g_PosCursorPrims[g_PosCursorPrimId].y1 = y;
            g_PosCursorPrims[g_PosCursorPrimId].y2 = y + 16;
            g_PosCursorPrims[g_PosCursorPrimId].y3 = y + 16;
        } else {
            g_PosCursorPrims[g_PosCursorPrimId].v0 = 96;
            g_PosCursorPrims[g_PosCursorPrimId].v1 = 96;
            g_PosCursorPrims[g_PosCursorPrimId].v2 = 112;
            g_PosCursorPrims[g_PosCursorPrimId].v3 = 112;
            g_PosCursorPrims[g_PosCursorPrimId].y0 = y - 16;
            g_PosCursorPrims[g_PosCursorPrimId].y1 = y - 16;
            g_PosCursorPrims[g_PosCursorPrimId].y2 = y;
            g_PosCursorPrims[g_PosCursorPrimId].y3 = y;
        }
        addPrim(ot, &g_PosCursorPrims[g_PosCursorPrimId]);
    }
}
