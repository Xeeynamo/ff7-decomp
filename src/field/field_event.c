//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern s32 (*g_FieldOpcodes[256])(void);
extern u8 g_EntityForSplitJoin;
extern char g_DebugMessageBuffer[];
extern u8 D_80114498[];

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

INCLUDE_ASM("asm/us/field/nonmatchings/field_event", FieldEventInit);

void FieldEventUpdate(s32 arg0) {
    if (D_8007EBE0) {
        FieldWindowResetTextAll();
        ResetFieldRenderState();
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
        if (g_FieldScripts->eventDataVersion > 2 ||
            g_FieldScripts->eventVersion > 5) {
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
            g_WindowData, 4, arg0, g_pFieldState->renderBuffer ^ 1);
    }
    UpdateFieldExitArrows(arg0);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_event", FieldInitDefaultValues);

INCLUDE_ASM("asm/us/field/nonmatchings/field_event", FieldEventRunInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field_event", FieldEnablePartyModels);

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
        if (g_EntitySplitJoinState[g_CurrentEntity] == 0 ||
            g_EntityForSplitJoin == g_CurrentEntity) {
            for (j = 8; j != 0; j--) {
                if (D_80099FFC == 5 && g_DebugLevel & 1 &&
                    (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity])) {
                    for (i = 1; i < 9; i++) {
                        SetStrToDebugRow(3, i, "");
                    }
                }
                g_FieldCurrentOpcode =
                    ((u8*)g_FieldScripts)[g_FieldScriptPC[g_CurrentEntity]];

                // Script can yield early if opcode returns 1.
                if (g_FieldOpcodes[g_FieldCurrentOpcode]()) {
                    if (D_80099FFC == 5 && g_DebugLevel & 1 &&
                        (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity])) {
                        g_CurrentEntity++;
                        goto done;
                    }
                    break;
                }
                if (D_80099FFC == 5 && g_DebugLevel & 1 &&
                    (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity])) {
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
        if (D_80099FFC == 5 && D_80071E24 & 1 &&
            (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity])) {
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
            switch (D_800756E8[g_EntityToModel[i]]) {
            case 0:
                if (g_FieldModels[g_EntityToModel[i]].activeAnimId !=
                    D_8008325C[g_EntityToModel[i]]) {
                    g_FieldModels[g_EntityToModel[i]].activeAnimId =
                        D_8008325C[g_EntityToModel[i]];
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
                    D_800756E8[g_EntityToModel[i]] = 4;
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
                    D_800756E8[g_EntityToModel[i]] = 0;
                }
                break;
            case 6:
                if ((g_FieldModels[g_EntityToModel[i]].animCurrentFrame >> 4) >=
                    g_FieldModels[g_EntityToModel[i]].animLastFrame) {
                    D_800756E8[g_EntityToModel[i]] = 3;
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_event", ResetFieldRenderState);

INCLUDE_ASM("asm/us/field/nonmatchings/field_event", UpdateFieldExitArrows);

INCLUDE_ASM("asm/us/field/nonmatchings/field_event", DrawFieldExitArrow);
