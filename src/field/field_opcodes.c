//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

#define GET_PARAM_U8(offset)                                                   \
    (*(u8*)((s32)g_FieldScripts + g_FieldScriptPC[g_CurrentEntity] + (offset)))
#define GET_PARAM_S16(value, offset)                                           \
    value = GET_PARAM_U8(offset);                                              \
    value |= (GET_PARAM_U8((offset) + 1) << 8)

#define PC_INC(x) (g_FieldScriptPC[g_CurrentEntity] += (x))
#define PC_DEC(x) (g_FieldScriptPC[g_CurrentEntity] -= (x))

#define GET_PRIORITY(x) (((x) >> 5) & 0x7)
#define GET_SCRIPTID(x) ((x) & 0x1F)

#define ADD_PARTY_MEMBER(slot, charId)                                         \
    Savemap.memory_bank_2[9 + slot] = charId;                                  \
    if (charId != 0xFF) {                                                      \
        u16 mask;                                                              \
        u16 bit;                                                               \
        bit = charId;                                                          \
        mask = Savemap.phs_visibility_mask;                                    \
        bit = 1 << bit;                                                        \
        mask |= bit;                                                           \
        Savemap.phs_visibility_mask = mask;                                    \
    }

extern s32 (*g_FieldOpcodes[256])(void);
extern u8 g_EntityForSplitJoin;
extern u8 D_80114498[];
extern u8 g_RandomTableStep;
extern u8 g_RandomTableIndex;
extern u8 g_RandomTable[256];
extern char g_DebugText[];
extern char g_DebugMessageBuffer[];
extern s16 D_80071E38;
extern s16 D_80071E3C;
extern u32 D_80075E10;
extern s8 D_800E0630;
extern SVECTOR (*D_800E4274)[3];
extern u8* D_800E48E0;

void func_80017678(void);

void DebugPrintToFieldWindow(const char* str);
void FieldEventDebugError(const char* errmsg);
void FieldDebugStringCopy(char* dst, const char* src);
void FieldDebugStringU8hex(s32 val, char* msg_out);
void FieldDebugStringU16hex(s32 val, char* msg_out);
void FieldDebugStringU32hex(s32 val, char* msg_out);

static s32 KeyCheck(u16 keys);
static u32 GetAkaoBlockOffset(s16 akaoId);
static void PartyReplace(u8* newParty);
static void PartyFromBank2ToSave(s32 unused);
static void PartyRemove(u8* party, u8* toRemove);
static void PartyAdd(u8* party, u8* toAdd);

void DebugUpdateActor(s16 arg0, s16 entityId) {
    if (arg0 == 4) {
        if (!(g_FieldScriptDebugFlags & 4) || (D_80114498[entityId])) {
            D_800716C8 = entityId;
            if (D_80114498[entityId]) {
                FieldDebugPageSetColor(4, 127, 1, 127);
            } else {
                FieldDebugPageSetColor(4, 7, 15, 31);
            }
            FieldDebugStringCopy(g_DebugText, D_800E0628);
        } else {
            return;
        }
    } else {
        FieldDebugStringCopy(g_DebugText, "ctrl:");
    }

    FieldDebugStringConcat(
        g_DebugText, (char*)g_FieldScripts + 32 + (entityId * 8));
    if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
        SetStrToDebugRow(arg0, 0, g_DebugText);
    }
    if (g_FieldScriptDebugFlags & 2) {
        DebugPrintToFieldWindow(g_DebugText);
    }
    FieldDebugStringCopy(g_DebugText, "RqLv=");
    FieldDebugStringU8hex(
        g_FieldScriptPriority[entityId], g_DebugMessageBuffer);
    FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
    FieldDebugStringConcat(g_DebugText, " Tg=");

    switch (SavedScriptIds[entityId][g_FieldScriptPriority[entityId]]) {
    case 0:
        FieldDebugStringConcat(g_DebugText, "dft");
        break;
    case 1:
        FieldDebugStringConcat(g_DebugText, "tlk");
        break;
    case 2:
        FieldDebugStringConcat(g_DebugText, "psh");
        break;
    default:
        FieldDebugStringU16hex(
            SavedScriptIds[entityId][g_FieldScriptPriority[entityId]],
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        break;
    }

    if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
        SetStrToDebugRow(arg0, 1, g_DebugText);
    }
    if (g_FieldScriptDebugFlags & 2) {
        DebugPrintToFieldWindow(g_DebugText);
    }

    if (g_EntityToModel[entityId] == 0xFF) {
        if (g_EntityToLine[entityId] == g_EntityToModel[entityId]) {
            FieldDebugStringCopy(g_DebugText, "Abst");
            if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
                SetDebugStrRowColor(arg0, 2, 6);
            }
        } else {
            FieldDebugStringCopy(g_DebugText, "line=");
            FieldDebugStringU16hex(
                g_EntityToLine[entityId], g_DebugMessageBuffer);
            FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
            if (g_FieldLines[g_EntityToLine[entityId]].isActive) {
                FieldDebugStringConcat(g_DebugText, " on");
            } else {
                FieldDebugStringConcat(g_DebugText, " off");
            }
            if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
                SetDebugStrRowColor(arg0, 2, 3);
            }
        }
    } else {
        FieldDebugStringCopy(g_DebugText, "man=");
        FieldDebugStringU16hex(g_EntityToModel[entityId], g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " dir=");
        FieldDebugStringU16hex(
            g_FieldModels[g_EntityToModel[entityId]].Dir, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetDebugStrRowColor(arg0, 2, 2);
        }
    }
    if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
        SetStrToDebugRow(arg0, 2, g_DebugText);
    }
    if (g_FieldScriptDebugFlags & 2) {
        DebugPrintToFieldWindow(g_DebugText);
    }

    if (g_EntityToModel[entityId] != 0xFF) {
        FieldDebugStringCopy(g_DebugText, "X=");
        FieldDebugStringU32hex(
            g_FieldModels[g_EntityToModel[entityId]].PosX >> 12,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Y=");
        FieldDebugStringU32hex(
            g_FieldModels[g_EntityToModel[entityId]].PosY >> 12,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 3, g_DebugText);
            SetDebugStrRowColor(arg0, 3, 1);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Z=");
        FieldDebugStringU32hex(
            g_FieldModels[g_EntityToModel[entityId]].PosZ >> 12,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " I=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].PosI,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 4, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringU8hex(
            g_FieldModelAnimMode[g_EntityToModel[entityId]], g_DebugText);
        FieldDebugStringConcat(g_DebugText, "am");
        FieldDebugStringU16hex(
            g_FieldModels[g_EntityToModel[entityId]].activeAnimId,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, ".");
        FieldDebugStringU32hex(
            g_FieldModels[g_EntityToModel[entityId]].animCurrentFrame,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, ".");
        FieldDebugStringU16hex(
            g_FieldModels[g_EntityToModel[entityId]].animLastFrame,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 5, g_DebugText);
            SetDebugStrRowColor(arg0, 5, 7);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        if (g_FieldModels[g_EntityToModel[entityId]].visible) {
            FieldDebugStringCopy(g_DebugText, "V");
        } else {
            FieldDebugStringCopy(g_DebugText, ".");
        }
        if (g_FieldModels[g_EntityToModel[entityId]].TalkOff) {
            FieldDebugStringConcat(g_DebugText, ".");
        } else {
            FieldDebugStringConcat(g_DebugText, "T");
        }
        if (g_FieldModels[g_EntityToModel[entityId]].SolidOff) {
            FieldDebugStringConcat(g_DebugText, ".");
        } else {
            FieldDebugStringConcat(g_DebugText, "S");
        }
        FieldDebugStringConcat(g_DebugText, ":TR");
        FieldDebugStringU16hex(
            g_FieldModels[g_EntityToModel[entityId]].TalkRange,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, ".SR");
        FieldDebugStringU16hex(
            g_FieldModels[g_EntityToModel[entityId]].SolidRange,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 6, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "MS");
        FieldDebugStringU32hex(
            g_FieldModels[g_EntityToModel[entityId]].MoveSpeed,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " AS");
        FieldDebugStringU32hex(
            g_FieldModels[g_EntityToModel[entityId]].animSpeed,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 7, g_DebugText);
            SetDebugStrRowColor(arg0, 7, 7);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
    } else if (g_EntityToLine[entityId] != g_EntityToModel[entityId]) {
        FieldDebugStringCopy(g_DebugText, "AX");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.x1,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " AY");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.y1,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 3, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "AZ");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.z1,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 4, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "BX");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.x2,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " BY");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.y2,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 5, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "BZ");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.z2,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 6, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        SetStrToDebugRow(arg0, 7, "");
    } else if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
        SetStrToDebugRow(arg0, 3, "");
        SetStrToDebugRow(arg0, 4, "");
        SetStrToDebugRow(arg0, 5, "");
        SetStrToDebugRow(arg0, 6, "");
        SetStrToDebugRow(arg0, 7, "");
    }
    if (arg0 != 4) {
        FieldDebugStringCopy(g_DebugText, "SX");
        FieldDebugStringU32hex(D_80071E38, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " SY");
        FieldDebugStringU32hex(D_80071E3C, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 8, g_DebugText);
            SetDebugStrRowColor(arg0, 8, 3);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "B-R    X=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][0].vx,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 9, g_DebugText);
            SetDebugStrRowColor(arg0, 9, 2);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Y=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][0].vy,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Z=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][0].vz,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 10, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "R-G    X=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][1].vx,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 11, g_DebugText);
            SetDebugStrRowColor(arg0, 11, 4);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Y=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][1].vy,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Z=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][1].vz,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 12, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "G-B    X=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][2].vx,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 13, g_DebugText);
            SetDebugStrRowColor(arg0, 13, 3);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Y=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][2].vy,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Z=");
        FieldDebugStringU32hex(
            D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][2].vz,
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 14, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Offset X=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].OffsetX,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 15, g_DebugText);
            SetDebugStrRowColor(arg0, 15, 2);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Y=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].OffsetY,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Z=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].OffsetZ,
                               g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 16, g_DebugText);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "SF");
        FieldDebugStringU32hex(
            Savemap.memory_bank_1[0] | (Savemap.memory_bank_1[1] << 8),
            g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (g_pFieldState->characterLock) {
            if (g_CharacterLock) {
                FieldDebugStringConcat(g_DebugText, ".");
            } else {
                FieldDebugStringConcat(g_DebugText, "/");
            }
        } else if (g_CharacterLock) {
            FieldDebugStringConcat(g_DebugText, "+");
        } else {
            FieldDebugStringConcat(g_DebugText, "*");
        }
        FieldDebugStringConcat(g_DebugText, "B");
        FieldDebugStringU8hex(Savemap.partyID[0], g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringU8hex(Savemap.partyID[1], g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringU8hex(Savemap.partyID[2], g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (g_pFieldState->battlesDisabled) {
            FieldDebugStringConcat(g_DebugText, ">");
        } else {
            FieldDebugStringConcat(g_DebugText, "*");
        }
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 17, g_DebugText);
            SetDebugStrRowColor(arg0, 17, 6);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "DP ");
        FieldDebugStringU32hex(D_80075E10 >> 16, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " ");
        FieldDebugStringU32hex(D_80075E10 & 0xFFFF, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (g_FieldMusicLock) {
            FieldDebugStringConcat(g_DebugText, "M");
        }
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 18, g_DebugText);
            if (D_80075E10 > 0x801AFFFFU) {
                if (Savemap.memory_bank_1[19] & 0x10) {
                    SetDebugStrRowColor(arg0, 18, 5);
                } else {
                    SetDebugStrRowColor(arg0, 18, 3);
                }
            }
            if (D_80075E10 > 0x801ADFFFU) {
                SetDebugStrRowColor(arg0, 18, 5);
            } else if (D_80075E10 > 0x801AAFFFU) {
                SetDebugStrRowColor(arg0, 18, 4);
            } else if (D_80075E10 > 0x801A7FFFU) {
                SetDebugStrRowColor(arg0, 18, 1);
            } else if (D_80075E10 > 0x801A3FFFU) {
                SetDebugStrRowColor(arg0, 18, 3);
            } else if (D_80075E10 > 0x8019FFFFU) {
                SetDebugStrRowColor(arg0, 18, 2);
            } else if (D_80075E10 > 0x80197FFFU) {
                SetDebugStrRowColor(arg0, 18, 0);
            } else {
                SetDebugStrRowColor(arg0, 18, 7);
            }
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringU8hex(Savemap.memory_bank_2[9], g_DebugMessageBuffer);
        FieldDebugStringCopy(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringU8hex(Savemap.memory_bank_2[10], g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringU8hex(Savemap.memory_bank_2[11], g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (Savemap.phs_visibility_mask & 1) {
            FieldDebugStringConcat(g_DebugText, "C");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 2) {
            FieldDebugStringConcat(g_DebugText, "B");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 4) {
            FieldDebugStringConcat(g_DebugText, "T");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 8) {
            FieldDebugStringConcat(g_DebugText, "E");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 0x10) {
            FieldDebugStringConcat(g_DebugText, "R");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 0x20) {
            FieldDebugStringConcat(g_DebugText, "Y");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 0x40) {
            FieldDebugStringConcat(g_DebugText, "K");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 0x80) {
            FieldDebugStringConcat(g_DebugText, "V");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 0x100) {
            FieldDebugStringConcat(g_DebugText, "D");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 0x200) {
            FieldDebugStringConcat(g_DebugText, "U");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (Savemap.phs_visibility_mask & 0x400) {
            FieldDebugStringConcat(g_DebugText, "F");
        } else {
            FieldDebugStringConcat(g_DebugText, ".");
        }
        if (D_8009FE8C | (g_FieldScriptDebugFlags & 1)) {
            SetStrToDebugRow(arg0, 19, g_DebugText);
            SetDebugStrRowColor(arg0, 19, 0);
        }
        if (g_FieldScriptDebugFlags & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
    }
}

void DebugPrintOpcode(const char* name, s32 arg1) {
    s32 temp_s0;
    s32 temp_s4;
    s32 var_s1;

    var_s1 = arg1;
    if (!(g_FieldScriptDebugFlags & 4) || (D_80114498[g_CurrentEntity] != 0)) {
        FieldDebugStringCopy(g_DebugText, &D_800E0630);
        FieldDebugStringConcat(g_DebugText, name);
        if (g_DebugLevel & 1) {
            SetStrToDebugRow(3, 0, g_DebugText);
        }
        if (g_DebugLevel & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        temp_s4 = var_s1 + 1;
        if (var_s1 != 0) {
            do {
                FieldDebugStringCopy(g_DebugText, "arg");
                temp_s0 = temp_s4 - var_s1;
                FieldDebugStringU8hex(temp_s0, g_DebugMessageBuffer);
                FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
                FieldDebugStringConcat(g_DebugText, "=");
                FieldDebugStringU16hex(
                    GET_PARAM_U8(temp_s0), g_DebugMessageBuffer);
                FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
                if (g_DebugLevel & 1) {
                    SetStrToDebugRow(3, (s16)temp_s0, g_DebugText);
                }
                if (g_DebugLevel & 2) {
                    DebugPrintToFieldWindow(g_DebugText);
                }
                var_s1 -= 1;
            } while (var_s1 != 0);
        }
    }
}

void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind) {
    if (!(g_FieldScriptDebugFlags & 4) || D_80114498[g_CurrentEntity]) {
        FieldDebugStringCopy(g_DebugText, str);
        switch (kind) {
        case 1:
            FieldDebugStringU8hex(
                val, g_DebugMessageBuffer); // to single hex digit
            break;
        case 2:
            FieldDebugStringU16hex(
                val, g_DebugMessageBuffer); // to double hex digit
            break;
        case 4:
            FieldDebugStringU32hex(
                val, g_DebugMessageBuffer); // to four hex digits
            break;
        default:
            FieldDebugStringCopy(g_DebugMessageBuffer, "");
            break;
        }
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (g_DebugLevel & 1) {
            AddStrNextDebugRow(2, g_DebugText);
        }
        if (g_DebugLevel & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
    }
}

static u8 FieldEventReadMemoryU8(s16 mb_half, s16 offset) {
    s32 indx;
    u8 value;
    u8 bankId;

    switch (mb_half) {
    case 1:
        bankId = GET_PARAM_U8(1) >> 4;
        break;
    case 2:
        bankId = GET_PARAM_U8(1) & 0xF;
        break;
    case 3:
        bankId = GET_PARAM_U8(2) >> 4;
        break;
    case 4:
        bankId = GET_PARAM_U8(2) & 0xF;
        break;
    case 5:
        bankId = GET_PARAM_U8(3) >> 4;
        break;
    case 6:
        bankId = GET_PARAM_U8(3) & 0xF;
        break;
    }

    switch (bankId) {
    case 0:
        value = GET_PARAM_U8(offset);
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G cons=", value, 2);
        }
        return value;
    case 1:
    case 2:
        indx = GET_PARAM_U8(offset);
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 3:
    case 4:
        indx = GET_PARAM_U8(offset) | 0x100;
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 11:
    case 12:
        indx = GET_PARAM_U8(offset) | 0x200;
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 13:
    case 14:
        indx = GET_PARAM_U8(offset) | 0x300;
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 15:
    case 7:
        indx = GET_PARAM_U8(offset) | 0x400;
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 5:
    case 6:
        indx = GET_PARAM_U8(offset);
        value = g_FieldMapVars[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G mapv=", value, 2);
        }
        return value;
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("G data err=", bankId, 2);
    }
    FieldEventDebugError("Bad Event arg!");
    return 0;
}

static void FieldEventWriteMemoryU8(s16 arg0, s16 arg1, u8 value) {
    u8 bankId;
    s32 indx;

    switch (arg0) {
    case 1:
        bankId = GET_PARAM_U8(1) >> 4;
        break;
    case 2:
        bankId = GET_PARAM_U8(1) & 0xF;
        break;
    case 3:
        bankId = GET_PARAM_U8(2) >> 4;
        break;
    case 4:
        bankId = GET_PARAM_U8(2) & 0xF;
        break;
    case 5:
        bankId = GET_PARAM_U8(3) >> 4;
        break;
    case 6:
        bankId = GET_PARAM_U8(3) & 0xF;
        break;
    }

    switch (bankId) {
    case 1:
    case 2:
        indx = GET_PARAM_U8(arg1);
        Savemap.memory_bank_1[indx] = value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 3:
    case 4:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 11:
    case 12:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 13:
    case 14:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 15:
    case 7:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 5:
    case 6:
        indx = GET_PARAM_U8(arg1);
        g_FieldMapVars[indx] = value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S mapv=", value, 2);
        }
        return;
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("S data err=", bankId, 2);
    }
    FieldEventDebugError("Bad Event arg!");
}

static s16 FieldEventReadMemoryS16(s16 bank_id, s16 offset) {
    u8 bankId;
    s32 indx;
    s16 value;

    switch (bank_id) {
    case 1:
        bankId = GET_PARAM_U8(1) >> 4;
        break;
    case 2:
        bankId = GET_PARAM_U8(1) & 0xF;
        break;
    case 3:
        bankId = GET_PARAM_U8(2) >> 4;
        break;
    case 4:
        bankId = GET_PARAM_U8(2) & 0xF;
        break;
    case 5:
        bankId = GET_PARAM_U8(3) >> 4;
        break;
    case 6:
        bankId = GET_PARAM_U8(3) & 0xF;
        break;
    }

    switch (bankId) {
    case 0:
        GET_PARAM_S16(value, offset);
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G cons=", value, 4);
        }
        return value;
    case 1:
        indx = GET_PARAM_U8(offset);
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 2:
        indx = GET_PARAM_U8(offset);
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 3:
        indx = GET_PARAM_U8(offset) | 0x100;
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 4:
        indx = GET_PARAM_U8(offset) | 0x100;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 11:
        indx = GET_PARAM_U8(offset) | 0x200;
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 12:
        indx = GET_PARAM_U8(offset) | 0x200;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 13:
        indx = GET_PARAM_U8(offset) | 0x300;
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 14:
        indx = GET_PARAM_U8(offset) | 0x300;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 15:
        indx = GET_PARAM_U8(offset) | 0x400;
        value = Savemap.memory_bank_1[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 7:
        indx = GET_PARAM_U8(offset) | 0x400;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 5:
        indx = GET_PARAM_U8(offset);
        value = g_FieldMapVars[indx];
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G mapv=", value, 4);
        }
        return value;
    case 6:
        indx = GET_PARAM_U8(offset);
        value = g_FieldMapVars[indx];
        value |= g_FieldMapVars[indx + 1] << 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G mapv=", value, 4);
        }
        return value;
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("G data err=", bankId, 2);
    }
    FieldEventDebugError("Bad Event arg!");
    return 0;
}

static void FieldEventWriteMemoryS16(s16 arg0, s16 arg1, s16 value) {
    u8 bankId;
    s32 indx;

    switch (arg0) {
    case 1:
        bankId = GET_PARAM_U8(1) >> 4;
        break;
    case 2:
        bankId = GET_PARAM_U8(1) & 0xF;
        break;
    case 3:
        bankId = GET_PARAM_U8(2) >> 4;
        break;
    case 4:
        bankId = GET_PARAM_U8(2) & 0xF;
        break;
    case 5:
        bankId = GET_PARAM_U8(3) >> 4;
        break;
    case 6:
        bankId = GET_PARAM_U8(3) & 0xF;
        break;
    }

    switch (bankId) {
    case 1:
        indx = GET_PARAM_U8(arg1);
        Savemap.memory_bank_1[indx] = (u8)value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 2:
        indx = GET_PARAM_U8(arg1);
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 3:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 4:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 11:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 12:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 13:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 14:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 15:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 7:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 5:
        indx = GET_PARAM_U8(arg1);
        g_FieldMapVars[indx] = (u8)value;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S mapv=", value, 2);
        }
        return;
    case 6:
        indx = GET_PARAM_U8(arg1);
        g_FieldMapVars[indx] = (u8)value;
        g_FieldMapVars[indx + 1] = value >> 8;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S mapv=", value, 4);
        }
        return;
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("S data err=", bankId, 2);
    }
    FieldEventDebugError("Bad Event arg!");
}

// This is called when the script tries to execute an invalid opcode
// called for opcodes:
// 0C 0D 1A 1B 1C 1D 1E 1F 44 46 4C 4E BE
s32 OpcodeFuncBad(void) {
    if (g_DebugLevel & 3) {
        FieldDebugStringU16hex(g_FieldCurrentOpcode, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugMessageBuffer, "???");
        DebugPrintOpcode(g_DebugMessageBuffer, 8);
        FieldDebugPageSetColor(3, 0x7F, 0, 0);
    } else {
        FieldEventDebugError("Bad Event code!");
    }
    return 1;
}

/**
 @brief Opcode 0x5F - **WAIT1* - Wait 1 frame

 Memory layout:

 | 0x5F |
 @details
 Waits one frame and returns 1
 @note
 This does not emit a debug message.
 */
s32 OpcodeFuncWait1(void) {
    PC_INC(1);
    return 1;
}

/**
 * @brief Opcode 0x24 - **WAIT** - Wait */

s32 OpcodeFuncWait(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wait", 2);
    }

    if (g_FieldWaitCounter[g_CurrentEntity] == 0) {
        GET_PARAM_S16(g_FieldWaitCounter[g_CurrentEntity], 1);
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2(
                "wait_st=", g_FieldWaitCounter[g_CurrentEntity], 4);
        }
        if (g_FieldWaitCounter[g_CurrentEntity] == 0) {
            PC_INC(3);
            return 1;
        }
        return 1;
    }

    if (g_FieldWaitCounter[g_CurrentEntity] == 1) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("wait_end=", 1, 4);
        }
        g_FieldWaitCounter[g_CurrentEntity] = 0;
        PC_INC(3);
        return 0;
    }

    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2(
            "wait=", g_FieldWaitCounter[g_CurrentEntity], 4);
    }

    g_FieldWaitCounter[g_CurrentEntity]--;
    return 1;
}

s32 OpcodeFuncSet(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("set", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncSet2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("set2", 4);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncLbyte(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("lbyte", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncHbyte(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("hbyte", 4);
    }
    FieldEventWriteMemoryU8(1, 2, (u8)(FieldEventReadMemoryS16(2, 3) >> 8));
    PC_INC(5);
    return 0;
}

s32 OpcodeFunc2byte(void) {
    s16 lhs;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("2byte", 5);
    }
    lhs = FieldEventReadMemoryU8(2, 4);
    FieldEventWriteMemoryS16(1, 3, lhs | (FieldEventReadMemoryU8(4, 5) << 8));
    PC_INC(6);
    return 0;
}

s32 OpcodeFuncSetx(void) {
    s16 offset;
    u8 bank;
    u8 value;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("setx", 6);
    }
    bank = GET_PARAM_U8(1) >> 4;
    offset = GET_PARAM_U8(3) + FieldEventReadMemoryS16(2, 3);
    value = FieldEventReadMemoryU8(4, 5);
    switch (bank) {
    case 15:
        offset += 256;
    case 13:
        offset += 256;
    case 11:
        offset += 256;
    case 3:
        offset += 256;
    case 1:
        if (offset >= 1280) {
            offset = 1279;
        }
        Savemap.memory_bank_1[offset] = value;
        break;
    case 5:
        if (offset >= 256) {
            offset = 255;
        }
        g_FieldMapVars[offset] = value;
        break;
    }
    PC_INC(7);
    return 0;
}

s32 OpcodeFuncGetx(void) {
    s16 offset;
    u8 bank;
    u8 value;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("getx", 6);
    }
    bank = GET_PARAM_U8(1) >> 4;
    offset = GET_PARAM_U8(3) + FieldEventReadMemoryS16(2, 3);
    switch (bank) {
    case 15:
        offset += 256;
    case 13:
        offset += 256;
    case 11:
        offset += 256;
    case 3:
        offset += 256;
    case 1:
        if (offset >= 1280) {
            offset = 1279;
        }
        value = Savemap.memory_bank_1[offset];
        break;
    case 5:
        if (offset >= 256) {
            offset = 255;
        }
        value = g_FieldMapVars[offset];
        break;
    }

    FieldEventWriteMemoryU8(4, 5, value);
    PC_INC(7);
    return 0;
}

s32 OpcodeFuncSrchx(void) {
    s16 end;
    s16 start;
    s16 where;
    u8 bank;
    u8 value;
    s16 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("srchx", 8);
    }
    bank = GET_PARAM_U8(1) >> 4;
    start = GET_PARAM_U8(4) + FieldEventReadMemoryS16(2, 5);
    end = GET_PARAM_U8(4) + FieldEventReadMemoryS16(3, 7);
    value = FieldEventReadMemoryU8(4, 9);
    switch (bank) {
    case 15:
        start += 256;
        end += 256;
    case 13:
        start += 256;
        end += 256;
    case 11:
        start += 256;
        end += 256;
    case 3:
        start += 256;
        end += 256;
    case 1:
        if (start >= 1280) {
            start = 1279;
        }
        if (end >= 1280) {
            end = 1279;
        }
        for (i = start; i <= end; i++) {
            if (Savemap.memory_bank_1[i] == value) {
                FieldEventWriteMemoryS16(6, 10, i);
                PC_INC(11);
                return 0;
            }
        }
        break;
    case 5:
        if (start >= 256) {
            start = 255;
        }
        if (end >= 256) {
            end = 255;
        }
        for (i = start; i <= end; i++) {
            if (g_FieldMapVars[i] == value) {
                FieldEventWriteMemoryS16(6, 10, i);
                PC_INC(11);
                return 0;
            }
        }
        break;
    }
    FieldEventWriteMemoryS16(6, 10, -1);
    PC_INC(11);
    return 0;
}

s32 OpcodeFuncBiton(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("biton", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2,
        FieldEventReadMemoryU8(1, 2) | (1 << FieldEventReadMemoryU8(2, 3)));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncBitof(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bitof", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2,
        FieldEventReadMemoryU8(1, 2) & ~(1 << FieldEventReadMemoryU8(2, 3)));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncBitxr(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bitxr", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2,
        FieldEventReadMemoryU8(1, 2) ^ (1 << FieldEventReadMemoryU8(2, 3)));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncLine(void) {
    s16 value;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("line", 8);
    }

    if (g_FieldLineCount >= 32) {
        FieldEventDebugError("many lineobj!");
        PC_INC(13);
        return 0;
    }

    g_EntityToLine[g_CurrentEntity] = g_FieldLineCount;
    GET_PARAM_S16(value, 1);
    g_FieldLines[g_FieldLineCount].pos.x1 = value;
    GET_PARAM_S16(value, 3);
    g_FieldLines[g_FieldLineCount].pos.y1 = value;
    GET_PARAM_S16(value, 5);
    g_FieldLines[g_FieldLineCount].pos.z1 = value;
    GET_PARAM_S16(value, 7);
    g_FieldLines[g_FieldLineCount].pos.x2 = value;
    GET_PARAM_S16(value, 9);
    g_FieldLines[g_FieldLineCount].pos.y2 = value;
    GET_PARAM_S16(value, 11);
    g_FieldLines[g_FieldLineCount].pos.z2 = value;
    g_FieldLines[g_FieldLineCount].isActive = 1;
    g_FieldLines[g_FieldLineCount].entityId = g_CurrentEntity;
    g_FieldLineCount++;
    PC_INC(13);
    return 0;
}

s32 OpcodeFuncSline(void) {
    u8 lineId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("sline", 8);
    }
    lineId = g_EntityToLine[g_CurrentEntity];
    g_FieldLines[lineId].pos.x1 = FieldEventReadMemoryS16(1, 4);
    g_FieldLines[lineId].pos.y1 = FieldEventReadMemoryS16(2, 6);
    g_FieldLines[lineId].pos.z1 = FieldEventReadMemoryS16(3, 8);
    g_FieldLines[lineId].pos.x2 = FieldEventReadMemoryS16(4, 10);
    g_FieldLines[lineId].pos.y2 = FieldEventReadMemoryS16(5, 12);
    g_FieldLines[lineId].pos.z2 = FieldEventReadMemoryS16(6, 14);
    PC_INC(16);
    return 0;
}

s32 OpcodeFuncLinon(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("linon", 1);
    }
    g_FieldLines[g_EntityToLine[g_CurrentEntity]].isActive = GET_PARAM_U8(1);
    if (GET_PARAM_U8(1) == 0) {
        g_FieldLines[g_EntityToLine[g_CurrentEntity]].touch = 0;
    }
    PC_INC(2);
    return 0;
}

/* Field-script opcode SLIP: Enables or disables slipping along a line
 *
 * Slipping allows the player to slide along a wall when running
 * against it instead of stopping. The wall must previously have a
 * line defined alongside it with opcode LINE.
 */

s32 OpcodeFuncSlip(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("slip", 1);
    }
    g_FieldLines[g_EntityToLine[g_CurrentEntity]].slipDisabled =
        GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

/* Field-script opcode IF: If, 1 byte, unsigned
 *
 * Compares two u8 using a given logical operator.
 * Jumps given number of bytes ahead if the comparison is false.
 */

s32 OpcodeFuncIf(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("if", 5);
    }
    if (IfCheck()) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if=true", 0, 0);
        }
        // If comparison is true, continue executing next opcode.
        PC_INC(6);
    } else {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if=false", 0, 0);
        }
        // If comparison is false, jump number of bytes give in last parameter
        // from last parameter.
        PC_INC(GET_PARAM_U8(5) + 5);
    }
    return 0;
}

/* Field-script opcode LIF: Long If, 1 byte, unsigned
 *
 * Compares two u8 using a given logical operator.
 * Identical to IF except that the jump parameter is s16, allowing for longer
 * jumps.
 */

s32 OpcodeFuncLif(void) {
    s16 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("lif", 6);
    }
    if (IfCheck()) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif=true", 0, 0);
        }
        PC_INC(7);
    } else {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif=false", 0, 0);
        }
        GET_PARAM_S16(param, 5);
        PC_INC(param + 5);
    }
    return 0;
}

u32 IfCheck(void) {
    u8 ope;
    u8 result;

    ope = GET_PARAM_U8(4);
    switch (ope) {
    case IF_EQ:
        result = FieldEventReadMemoryU8(1, 2) == FieldEventReadMemoryU8(2, 3);
        break;
    case IF_NOT_EQ:
        result = FieldEventReadMemoryU8(1, 2) != FieldEventReadMemoryU8(2, 3);
        break;
    case IF_GT:
        result = FieldEventReadMemoryU8(1, 2) > FieldEventReadMemoryU8(2, 3);
        break;
    case IF_LT:
        result = FieldEventReadMemoryU8(1, 2) < FieldEventReadMemoryU8(2, 3);
        break;
    case IF_GTE:
        result = FieldEventReadMemoryU8(1, 2) >= FieldEventReadMemoryU8(2, 3);
        break;
    case IF_LTE:
        result = FieldEventReadMemoryU8(1, 2) <= FieldEventReadMemoryU8(2, 3);
        break;
    case IF_AND:
        result = FieldEventReadMemoryU8(1, 2) & FieldEventReadMemoryU8(2, 3);
        break;
    case IF_XOR:
        result = FieldEventReadMemoryU8(1, 2) ^ FieldEventReadMemoryU8(2, 3);
        break;
    case IF_OR:
        result = FieldEventReadMemoryU8(1, 2) | FieldEventReadMemoryU8(2, 3);
        break;
    case IF_BIT:
        result =
            FieldEventReadMemoryU8(1, 2) & (1 << FieldEventReadMemoryU8(2, 3));
        break;
    case IF_NOT_BIT:
        result =
            FieldEventReadMemoryU8(1, 2) & (1 << FieldEventReadMemoryU8(2, 3));
        result = result < 1;
        break;
    default:
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("ope err=", ope, 2);
        }
        break;
    }
    return result;
}

/* Field-script opcode IF2: If, 2 bytes, signed
 *
 * Compares two s16 using a given logical operator.
 */
s32 OpcodeFuncIf2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("if2", 7);
    }
    if (If2CheckSigned() != 0) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if2=true", 0, 0);
        }
        PC_INC(8);
    } else {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if2=false", 0, 0);
        }
        PC_INC(GET_PARAM_U8(7) + 7);
    }
    return 0;
}

/* Field-script opcode LIF2: Long if, 2 bytes, signed
 *
 * Compares two s16 using a given logical operator.
 */
s32 OpcodeFuncLif2(void) {
    s16 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("lif2", 8);
    }
    if (If2CheckSigned() != 0) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif2=true", 0, 0);
        }
        PC_INC(9);
    } else {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif2=false", 0, 0);
        }
        GET_PARAM_S16(param, 7);
        PC_INC(param + 7);
    }
    return 0;
}

u32 If2CheckSigned(void) {
    u8 ope;
    u8 result;

    ope = GET_PARAM_U8(6);
    switch (ope) {
    case IF_EQ:
        result = FieldEventReadMemoryS16(1, 2) == FieldEventReadMemoryS16(2, 4);
        break;
    case IF_NOT_EQ:
        result = FieldEventReadMemoryS16(1, 2) != FieldEventReadMemoryS16(2, 4);
        break;
    case IF_GT:
        result = FieldEventReadMemoryS16(1, 2) > FieldEventReadMemoryS16(2, 4);
        break;
    case IF_LT:
        result = FieldEventReadMemoryS16(1, 2) < FieldEventReadMemoryS16(2, 4);
        break;
    case IF_GTE:
        result = FieldEventReadMemoryS16(1, 2) >= FieldEventReadMemoryS16(2, 4);
        break;
    case IF_LTE:
        result = FieldEventReadMemoryS16(1, 2) <= FieldEventReadMemoryS16(2, 4);
        break;
    case IF_AND:
        result = FieldEventReadMemoryS16(1, 2) & FieldEventReadMemoryS16(2, 4);
        break;
    case IF_XOR:
        result = FieldEventReadMemoryS16(1, 2) ^ FieldEventReadMemoryS16(2, 4);
        break;
    case IF_OR:
        result = FieldEventReadMemoryS16(1, 2) | FieldEventReadMemoryS16(2, 4);
        break;
    case IF_BIT:
        result = FieldEventReadMemoryS16(1, 2) &
                 (1 << FieldEventReadMemoryS16(2, 4));
        break;
    case IF_NOT_BIT:
        result = FieldEventReadMemoryS16(1, 2) &
                 (1 << FieldEventReadMemoryS16(2, 4));
        result = result < 1;
        break;
    default:
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("ope err=", ope, 2);
        }
        break;
    }
    return result;
}

/* Field-script opcode IF2U: If, 2 bytes, unsigned
 *
 * Compares two u16 using a given logical operator.
 */
s32 OpcodeFuncIf2u(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("if2", 7);
    }
    if (If2CheckUnsigned()) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if2=true", 0, 0);
        }
        PC_INC(8);
    } else {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if2=false", 0, 0);
        }
        PC_INC(GET_PARAM_U8(7) + 7);
    }
    return 0;
}

/* Field-script opcode LIF2U: Long if, 2 bytes, unsigned
 *
 * Compares two u16 using a given logical operator.
 */
s32 OpcodeFuncLif2u(void) {
    s16 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("lif2", 8);
    }
    if (If2CheckUnsigned() != 0) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif2=true", 0, 0);
        }
        PC_INC(9);
    } else {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif2=false", 0, 0);
        }
        GET_PARAM_S16(param, 7);
        PC_INC(param + 7);
    }
    return 0;
}

u32 If2CheckUnsigned(void) {
    u8 ope;
    u8 result;

    ope = GET_PARAM_U8(6);
    switch (ope) {
    case IF_EQ:
        result = (u16)FieldEventReadMemoryS16(1, 2) ==
                 (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_NOT_EQ:
        result = (u16)FieldEventReadMemoryS16(1, 2) !=
                 (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_GT:
        result = (u16)FieldEventReadMemoryS16(1, 2) >
                 (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_LT:
        result = (u16)FieldEventReadMemoryS16(1, 2) <
                 (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_GTE:
        result = (u16)FieldEventReadMemoryS16(1, 2) >=
                 (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_LTE:
        result = (u16)FieldEventReadMemoryS16(1, 2) <=
                 (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_AND:
        result = FieldEventReadMemoryS16(1, 2) & FieldEventReadMemoryS16(2, 4);
        break;
    case IF_XOR:
        result = FieldEventReadMemoryS16(1, 2) ^ FieldEventReadMemoryS16(2, 4);
        break;
    case IF_OR:
        result = FieldEventReadMemoryS16(1, 2) | FieldEventReadMemoryS16(2, 4);
        break;
    case IF_BIT:
        result = FieldEventReadMemoryS16(1, 2) &
                 (1 << FieldEventReadMemoryS16(2, 4));
        break;
    case IF_NOT_BIT:
        result = FieldEventReadMemoryS16(1, 2) &
                 (1 << FieldEventReadMemoryS16(2, 4));
        result = result < 1;
        break;
    default:
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("ope err=", ope, 2);
        }
        break;
    }
    return result;
}

/* Field-script opcode KEY!: Key check
 *
 * Jumps ahead given number of bytes if given key(s) are not active.
 * All key opcodes only check the lower half word which contains the keys
 * for controller 1.
 */

s32 OpcodeFuncKeyEx(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("key!", 3);
    }
    if (GET_PARAM_U8(2) & 2) {
        return KeyCheck((u16)g_pFieldState->activeKeys);
    } else {
        return KeyCheck((u16)g_pFieldState->activeKeysRaw);
    }
}

/* Field-script opcode KEYON: Key On
 *
 * Checks keys that player pressed this frame.
 */

s32 OpcodeFuncKeyon(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("keyon", 3);
    }
    if (GET_PARAM_U8(2) & 2) {
        return KeyCheck((u16)g_pFieldState->pressedKeys);
    } else {
        return KeyCheck((u16)g_pFieldState->pressedKeysRaw);
    }
}

/* Field-script opcode KEYOF: Key Off
 *
 * Checks keys that player released this frame.
 */

s32 OpcodeFuncKeyof(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("keyof", 3);
    }
    if (GET_PARAM_U8(2) & 2) {
        return KeyCheck((u16)g_pFieldState->releasedKeys);
    } else {
        return KeyCheck((u16)g_pFieldState->releasedKeysRaw);
    }
}

static s32 KeyCheck(u16 keys) {
    u16 param;

    GET_PARAM_S16(param, 1);
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("key now=", keys, 4);
        FieldDebugAddParseValueToPage2("key chk=", param, 4);
    }
    if (keys & param) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("key=true", 0, 0);
        }
        PC_INC(4);
    } else {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("key=false", 0, 0);
        }
        PC_INC(GET_PARAM_U8(3) + 3);
    }
    return 0;
}

s32 OpcodeFuncReq(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("req", 2);
    }
    return FieldEventRequest(1, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)),
                             GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncReqsw(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("reqsw", 2);
    }
    return FieldEventRequest(2, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)),
                             GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncReqew(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("reqew", 2);
    }
    return FieldEventRequest(3, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)),
                             GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncPreq(void) {
    u8 charId;
    u8 entityId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("preq", 2);
    }
    charId = Savemap.memory_bank_2[GET_PARAM_U8(1) + 9];
    if (charId == 0xFF) {
        entityId = 0xFF;
    } else {
        entityId = g_CharIdToEntity[charId];
    }
    return FieldEventRequest(1, entityId, GET_PRIORITY(GET_PARAM_U8(2)),
                             GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncPrqsw(void) {
    u8 charId;
    u8 entityId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("prqsw", 2);
    }
    charId = Savemap.memory_bank_2[GET_PARAM_U8(1) + 9];
    if (charId == 0xFF) {
        entityId = 0xFF;
    } else {
        entityId = g_CharIdToEntity[charId];
    }
    return FieldEventRequest(2, entityId, GET_PRIORITY(GET_PARAM_U8(2)),
                             GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncPrqew(void) {
    u8 charId;
    u8 entityId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("prqew", 2);
    }
    charId = Savemap.memory_bank_2[GET_PARAM_U8(1) + 9];
    if (charId == 0xFF) {
        entityId = 0xFF;
    } else {
        entityId = g_CharIdToEntity[charId];
    }
    return FieldEventRequest(3, entityId, GET_PRIORITY(GET_PARAM_U8(2)),
                             GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 FieldEventRequest(s16 type, u8 target, u8 priority, u8 scriptId) {
    s32 scriptOffset;
    s32 entityDataSize;
    s32 extrasHeaderSize;

    if (target == 255) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("rqew=no one", 0, 0);
        }
        PC_INC(3);
        return 0;
    }

    if (g_DebugLevel & 3) {
        FieldDebugStringCopy(g_DebugMessageBuffer, "rq=");
        FieldDebugStringConcat(
            g_DebugMessageBuffer,
            (char*)g_FieldScripts + sizeof(FieldScriptHeader) + target * 8);
        FieldDebugStringConcat(g_DebugMessageBuffer, "/");
        FieldDebugAddParseValueToPage2(g_DebugMessageBuffer, scriptId, 2);
    }

    switch (type) {
    case 1:
    case 2:
        break;
    case 3:
        if (g_FieldScriptSyncWaitEntity[target][priority] == g_CurrentEntity) {
            switch (g_FieldScriptSyncState[target][priority]) {
            case SYNC_WAITING:
                if (g_DebugLevel & 3) {
                    FieldDebugAddParseValueToPage2("rqew=wait", 0, 0);
                }
                return 1;
            case SYNC_DONE:
                if (g_DebugLevel & 3) {
                    FieldDebugAddParseValueToPage2("rqew=end", 0, 0);
                }
                g_FieldScriptSyncState[target][priority] = SYNC_NONE;
                g_FieldScriptSyncWaitEntity[target][priority] = 255;
                PC_INC(3);
                return 0;
            }
        }
        break;
    }

    if (g_FieldScriptPriority[target] == priority) {
        switch (type) {
        case 1:
            PC_INC(3);
            if (g_DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("rq=skip", 0, 0);
            }
            return 0;
        case 2:
        case 3:
            if (g_DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("rqw=busy", 0, 0);
            }
        }
        return 1;
    } else if (g_FieldScriptPriority[target] < priority) {
        if (g_SavedFieldScriptPC[target][priority] != 0) {
            switch (type) {
            case 1:
                PC_INC(3);
                if (g_DebugLevel & 3) {
                    FieldDebugAddParseValueToPage2("rq=skip", 0, 0);
                }
                return 0;
            case 2:
            case 3:
                if (g_DebugLevel & 3) {
                    FieldDebugAddParseValueToPage2("rqw=busy", 0, 0);
                }
            }
            return 1;
        }
        scriptOffset = scriptId * 2;
        entityDataSize = target * 64;
        extrasHeaderSize = (s16)(g_FieldScripts->numExtras * 4);

        GET_FIELD_SCRIPT_PC(
            g_SavedFieldScriptPC[target][priority], scriptOffset,
            entityDataSize + (g_FieldScripts->numEntities << 3),
            extrasHeaderSize);

        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("rq=send", 0, 0);
        }

        switch (type) {
        case 1:
        case 2:
            PC_INC(3);
            return 0;
        case 3:
            g_FieldScriptSyncWaitEntity[target][priority] = g_CurrentEntity;
            g_FieldScriptSyncState[target][priority] = SYNC_WAITING;
            return 1;
        }
        return 1;
    } else if (g_FieldScriptSyncState[target][priority] == SYNC_NONE) {
        s32 scriptOffset;
        s32 entityDataSize;
        s32 extrasHeaderSize;

        SavedScriptIds[target][priority] = scriptId;
        g_SavedFieldScriptPC[target][g_FieldScriptPriority[target]] =
            g_FieldScriptPC[target];

        scriptOffset = scriptId * 2;
        entityDataSize = target * 64;
        extrasHeaderSize = (s16)(g_FieldScripts->numExtras * 4);

        GET_FIELD_SCRIPT_PC(g_FieldScriptPC[target], scriptOffset,
                            entityDataSize + (g_FieldScripts->numEntities << 3),
                            extrasHeaderSize);

        g_FieldScriptPriority[target] = priority;

        if (g_EntityToModel[target] != 255) {
            g_FieldModels[g_EntityToModel[target]].scriptedMoveMode =
                SMODE_NONE;
        }
        g_FieldWaitCounter[target] = 0;

        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("rq=send", 0, 0);
        }

        switch (type) {
        case 1:
        case 2:
            PC_INC(3);
            return 0;
        case 3:
            g_FieldScriptSyncWaitEntity[target][priority] = g_CurrentEntity;
            g_FieldScriptSyncState[target][priority] = SYNC_WAITING;
            return 1;
        }
        return 1;
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("rqw=busy*", 0, 0);
    }
    return 1;
}

s32 OpcodeFuncRet(void) {
    u16* fieldScriptPC;
    u16(*savedPC)[8];
    u16* savedRow;
    u16 scriptPc;
    u32 entity;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("ret", 0);
    }
    if (g_FieldScriptPriority[g_CurrentEntity] >= 7) {
        return 1;
    }

    if (g_FieldScriptSyncState[g_CurrentEntity]
                              [g_FieldScriptPriority[g_CurrentEntity]] ==
        SYNC_WAITING) {
        g_FieldScriptSyncState[g_CurrentEntity]
                              [g_FieldScriptPriority[g_CurrentEntity]] =
                                  SYNC_DONE;
    }

    g_FieldScriptPriority[g_CurrentEntity]++;

    entity = g_CurrentEntity;
    savedPC = g_SavedFieldScriptPC;
    fieldScriptPC = g_FieldScriptPC;

    savedRow = savedPC[entity];
    scriptPc = savedRow[g_FieldScriptPriority[entity]];
    fieldScriptPC[entity] = scriptPc;

    while (scriptPc == 0 && g_FieldScriptPriority[entity] < 7) {
        u16* activePcSlot;
        u16* loopSavedRow;
        u16 nextPc;

        g_FieldScriptPriority[g_CurrentEntity]++;
        entity = g_CurrentEntity;

        activePcSlot =
            (u16*)((entity * sizeof(*fieldScriptPC)) + (s32)fieldScriptPC);
        loopSavedRow = (u16*)((entity * sizeof(*savedPC)) + (s32)savedPC);
        nextPc = loopSavedRow[g_FieldScriptPriority[entity]];

        *activePcSlot = nextPc;
        scriptPc = nextPc;
    }

    g_SavedFieldScriptPC[g_CurrentEntity]
                        [g_FieldScriptPriority[g_CurrentEntity]] = 0;
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2(
            "ret=", g_FieldScriptPriority[g_CurrentEntity], 2);
    }
    return 0;
}

s32 OpcodeFuncRetto(void) {
    s16 scriptId;
    u8 priority;
    s32 extrasHeaderSize;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("retto", 1);
    }

    priority = GET_PRIORITY(GET_PARAM_U8(1));
    scriptId = GET_SCRIPTID(GET_PARAM_U8(1));

    while (g_FieldScriptPriority[g_CurrentEntity] < (priority - 1) &&
           g_FieldScriptPriority[g_CurrentEntity] < 7) {
        if (g_FieldScriptSyncState[g_CurrentEntity]
                                  [g_FieldScriptPriority[g_CurrentEntity]] ==
            SYNC_WAITING) {
            g_FieldScriptSyncState[g_CurrentEntity]
                                  [g_FieldScriptPriority[g_CurrentEntity]] =
                                      SYNC_DONE;
        }
        g_FieldScriptPriority[g_CurrentEntity]++;
        g_SavedFieldScriptPC[g_CurrentEntity]
                            [g_FieldScriptPriority[g_CurrentEntity]] = 0;
    }
    SavedScriptIds[g_CurrentEntity][priority] = scriptId;
    scriptId *= 2;
    extrasHeaderSize = (s16)(g_FieldScripts->numExtras * 4);

    GET_FIELD_SCRIPT_PC(
        g_FieldScriptPC[g_CurrentEntity], scriptId,
        (g_FieldScripts->numEntities * 8) + (g_CurrentEntity * 64),
        extrasHeaderSize);

    g_FieldScriptPriority[g_CurrentEntity] = priority;
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2(
            "ret=", g_FieldScriptPriority[g_CurrentEntity], 2);
    }
    return 0;
}

s32 OpcodeFuncBack(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("back", 1);
    }
    PC_DEC(GET_PARAM_U8(1));
    return 1;
}

s32 OpcodeFuncLback(void) {
    u16 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("lback", 2);
    }
    GET_PARAM_S16(param, 1);
    PC_DEC(param);
    return 1;
}

s32 OpcodeFuncSkip(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("skip", 1);
    }
    PC_INC(GET_PARAM_U8(1) + 1);
    return 0;
}

s32 OpcodeFuncLskip(void) {
    u16 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("lskip", 2);
    }
    GET_PARAM_S16(param, 1);
    PC_INC(param + 1);
    return 0;
}

s32 OpcodeFuncMjump(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mjump", 8);
    }

    switch (g_pFieldState->eventCmd) {
    case EVTCMD_NONE:
        g_pFieldState->eventCmd = EVTCMD_FIELD_MAP_CHANGE;
        g_pFieldState->movieCommandState = MOVCMD_IDLE;
        GET_PARAM_S16(g_pFieldState->eventCmdParam, 1);
        GET_PARAM_S16(g_pFieldState->pcPosX, 3);
        GET_PARAM_S16(g_pFieldState->pcPosY, 5);
        GET_PARAM_S16(g_pFieldState->pcWalkMeshId, 7);
        g_pFieldState->pcDirection = GET_PARAM_U8(9);
        return 1;
    case EVTCMD_FIELD_MAP_CHANGE:
        if (g_pFieldState->movieCommandState == MOVCMD_DONE) {
            PC_INC(10);
            g_pFieldState->eventCmd = EVTCMD_NONE;
            return 0;
        }
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("evt cmd=", g_pFieldState->eventCmd, 2);
    }
    return 1;
}

s32 OpcodeFuncPmjmp(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("pmjmp", 8);
    }
    GET_PARAM_S16(g_FieldPreloadMapId, 1);
    PC_INC(3);
    return 0;
}

s32 OpcodeFuncPmjmp2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("pmjmp", 8);
    }
    if (g_isFieldLoading != 2) {
        return 1;
    }
    PC_INC(1);
    return 0;
}

s32 OpcodeFuncMgame(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mgame", 8);
    }
    switch (g_pFieldState->eventCmd) {
    case EVTCMD_NONE:
        g_pFieldState->eventCmd = EVTCMD_LOAD_MINIGAME;
        g_pFieldState->movieCommandState = MOVCMD_IDLE;
        GET_PARAM_S16(g_pFieldState->eventCmdParam, 1);
        GET_PARAM_S16(g_pFieldState->pcPosX, 3);
        GET_PARAM_S16(g_pFieldState->pcPosY, 5);
        GET_PARAM_S16(g_pFieldState->pcWalkMeshId, 7);
        *(s16*)&g_pFieldState->pcDirection = GET_PARAM_U8(9);
        *(u8*)((u8*)g_pFieldState + 0xF2) = GET_PARAM_U8(10);
        return 1;
    case EVTCMD_LOAD_MINIGAME:
        if (g_pFieldState->movieCommandState == MOVCMD_DONE) {
            PC_INC(11);
            g_pFieldState->eventCmd = EVTCMD_NONE;
            return 0;
        }
        return 1;
    }
    return 1;
}

s32 OpcodeFuncBatle(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("batle", 3);
    }
    switch (g_pFieldState->eventCmd) {
    case EVTCMD_NONE:
        FieldWindowResetTextAll();
        g_pFieldState->eventCmd = EVTCMD_ENTERING_BATTLE;
        g_pFieldState->eventCmdParam = FieldEventReadMemoryS16(2, 2);
        D_8007EBE0 = 1;
        g_pFieldState->movieCommandState = MOVCMD_IDLE;
        return 1;
    case EVTCMD_ENTERING_BATTLE:
        if (g_pFieldState->movieCommandState == MOVCMD_DONE) {
            PC_INC(4);
            g_pFieldState->eventCmd = EVTCMD_NONE;
            g_pFieldState->movieCommandState = MOVCMD_IDLE;
            return 0;
        }
        break;
    }
    return 1;
}

void FieldEventClearAkaoStruct(void) {
    s32 i;
    s16* p;

    D_8009A000[0] = 0;
    for (i = 5, p = &D_8009A000[10]; i >= 0; i--) {
        *(s32*)(p + 2) = 0;
        p -= 2;
    }
}

s32 OpcodeFuncAkao(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("akao", 3);
    }
    FieldEventClearAkaoStruct();
    *D_8009A000 = GET_PARAM_U8(4);
    *D_8009A004 = FieldEventReadMemoryU8(1, 5);
    *D_8009A008 = (s16)FieldEventReadMemoryS16(2, 6);
    g_FieldAkaoArg3 = (s16)FieldEventReadMemoryS16(3, 8);
    g_FieldAkaoArg4 = (s16)FieldEventReadMemoryS16(4, 10);
    g_FieldAkaoArg5 = (s16)FieldEventReadMemoryS16(6, 12);
    SystemAkaoExecute();
    PC_INC(14);
    return 0;
}

s32 OpcodeFuncAkao2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("akao2", 3);
    }
    FieldEventClearAkaoStruct();
    *D_8009A000 = GET_PARAM_U8(4);
    *D_8009A004 = (s16)FieldEventReadMemoryS16(1, 5);
    *D_8009A008 = (s16)FieldEventReadMemoryS16(2, 7);
    g_FieldAkaoArg3 = (s16)FieldEventReadMemoryS16(3, 9);
    g_FieldAkaoArg4 = (s16)FieldEventReadMemoryS16(4, 11);
    g_FieldAkaoArg5 = (s16)FieldEventReadMemoryS16(6, 13);
    SystemAkaoExecute();
    PC_INC(15);
    return 0;
}

s32 OpcodeFuncSe(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("se", 3);
    }
    FieldEventClearAkaoStruct();
    D_8009A000[0] = 0x20;
    D_8009A004[0] = FieldEventReadMemoryU8(2, 4);
    D_8009A008[0] = (s16)FieldEventReadMemoryS16(1, 2);
    SystemAkaoExecute();
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncMusic(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("music", 1);
    }
    FieldEventClearAkaoStruct();
    D_8009A000[0] = 0x10;
    return SetAndApplyAkao();
}

s32 OpcodeFuncMusvt(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("musvt", 1);
    }
    FieldEventClearAkaoStruct();
    D_8009A000[0] = 0x14;
    return SetAndApplyAkao();
}

s32 OpcodeFuncMusvm(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("musvm", 1);
    }
    FieldEventClearAkaoStruct();
    D_8009A000[0] = 0x15;
    return SetAndApplyAkao();
}

s32 OpcodeFuncCmusc(void) {
    u32 result;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("cmusc", 5);
    }
    FieldEventClearAkaoStruct();
    *D_8009A000 = GET_PARAM_U8(3);
    *D_8009A008 = (s16)FieldEventReadMemoryS16(3, 4);
    g_FieldAkaoArg3 = (s16)FieldEventReadMemoryS16(4, 6);
    result = SetAndApplyAkao();
    PC_INC(6);
    return result;
}

s32 SetAndApplyAkao(void) {
    // Indexes into AKAO block of field file which contains the list of music
    // tracks available for current field.
    u8 akaoId;

    if (g_FieldMusicLock == 0) {
        akaoId = GET_PARAM_U8(1);
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("music=", akaoId, 2);
        }
        *D_8009A004 = (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
        g_pFieldState->nextFieldMusic = *D_8009A004;
        SystemAkaoExecute();
    }
    PC_INC(2);
    return 0;
}

static u32 GetAkaoBlockOffset(s16 akaoId) {
    s32 akaoData;
    u32 akaoOffset;

    akaoData =
        akaoId * 4 + g_FieldScripts->numEntities * 8 + (s32)g_FieldScripts;
    akaoOffset = ((u8*)akaoData)[sizeof(FieldScriptHeader)];
    akaoOffset |= ((u8*)akaoData)[sizeof(FieldScriptHeader) + 1] << 8;
    akaoOffset |= ((u8*)akaoData)[sizeof(FieldScriptHeader) + 2] << 16;
    akaoOffset |= ((u8*)akaoData)[sizeof(FieldScriptHeader) + 3] << 24;
    return akaoOffset;
}

s32 OpcodeFuncBmusc(void) {
    u8 akaoId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bmusc", 1);
    }
    if (g_FieldMusicLock == 0) {
        akaoId = GET_PARAM_U8(1);
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("bmusic=", akaoId, 2);
        }
        g_pFieldState->nextBattleMusic =
            (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
    } else {
        g_pFieldState->nextBattleMusic = 0;
    }
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncFmusc(void) {
    u8 akaoId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("fmusc", 1);
    }
    if (g_FieldMusicLock == 0) {
        akaoId = GET_PARAM_U8(1);
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("bmusic=", akaoId, 2);
        }
        g_pFieldState->nextFieldMusic =
            (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
    } else {
        g_pFieldState->nextFieldMusic = 0;
    }
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncTutor(void) {
    u8 tutorialId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("tutor", 1);
    }

    if (g_pFieldState->eventCmd == EVTCMD_NONE) {
        g_pFieldState->eventCmd = EVTCMD_PARTY_MENU;
        g_pFieldState->eventCmdParam =
            1; // Tells party menu module to start tutorial.
        g_pFieldState->movieCommandState = MOVCMD_IDLE;
        D_8007EBE0 = 1;
        tutorialId = GET_PARAM_U8(1);
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("data=", tutorialId, 2);
        }
        D_800E48E0 = (GetAkaoBlockOffset(tutorialId) + (u8*)g_FieldScripts);
        return 1;
    }

    if (g_pFieldState->eventCmd == EVTCMD_PARTY_MENU) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2(
                "evt result=", g_pFieldState->movieCommandState, 2);
        }
        if (g_pFieldState->movieCommandState == MOVCMD_DONE) {
            g_pFieldState->eventCmd = EVTCMD_NONE;
            g_pFieldState->movieCommandState = MOVCMD_IDLE;
            PC_INC(2);
            return 0;
        }
    }
    return 1;
}

/* Field-script opcode MULCK (0xF5): set the music lock from the opcode
 * operand. */
s32 OpcodeFuncMulck(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mulck", 1);
    }
    g_FieldMusicLock = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncBgmovie(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bgmovie", 1);
    }
    g_pFieldState->backgroundMovieEnabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncScrlo(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scrlo", 1);
    }
    g_pFieldState->scrloSet = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

/* Field-script opcode DSKCG: request a disc change.
 *
 * Runs as a small state machine on the field main-loop step (opcode):
 * on first execution it stores the requested disc number and switches the
 * field loop into the disc-change step (13), then keeps returning 1
 * (opcode not finished) until the loop reports the swap is done
 * (movieCommandState == 2). Only then does the script advance past the opcode.
 */
s32 OpcodeFuncDskcg(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dskcg", 1);
    }
    switch (g_pFieldState->eventCmd) {
    case EVTCMD_NONE:
        g_pFieldState->eventCmd = EVTCMD_CD_CHANGE;
        g_FieldDiscChangeRequest = GET_PARAM_U8(1);
        return 1;
    case EVTCMD_CD_CHANGE:
        if (g_pFieldState->movieCommandState == MOVCMD_DONE) {
            g_pFieldState->eventCmd = EVTCMD_NONE;
            PC_INC(2);
            return 0;
        }
        return 1;
    default:
        return 1;
    }
}

/* Field-script opcode UC: lock or unlock player control.
 *
 * A nonzero operand freezes the player character; on unlock the
 * per-model flag of the player's model is cleared as well.
 */
s32 OpcodeFuncUc(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("uc", 1);
    }
    g_CharacterLock = g_pFieldState->characterLock = GET_PARAM_U8(1);
    if (g_CharacterLock == 0) {
        g_FieldModelAnimMode[g_pFieldState->pcModelId] = 0;
    }
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncBtlon(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("btlon", 1);
    }
    g_pFieldState->battlesDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncMpdsp(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mpdsp", 1);
    }
    g_pFieldState->mpdspSet = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncMvcam(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mvcam", 1);
    }
    g_pFieldState->movieCamDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncGmovr(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("gmovr", 0);
    }
    g_pFieldState->eventCmd = EVTCMD_GAME_OVER;
    g_pFieldState->movieCommandState = MOVCMD_IDLE;
    return 1;
}

/* Field-script opcode CC: hand player control to another entity.
 *
 * The operand is a script entity id; if that entity has a field model
 * assigned (g_EntityToModel entry != 0xFF) it becomes the new player model.
 */
s32 OpcodeFuncCc(void) {
    u8 charId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("cc", 1);
    }
    charId = GET_PARAM_U8(1);
    if (g_EntityToModel[charId] != 0xFF) {
        g_pFieldState->pcModelId = g_EntityToModel[charId];
    }
    PC_INC(2);
    return 0;
}

/* Field-script opcode CHAR: attach a field model to the current entity.
 *
 * Allocates the next model slot (g_FieldModelCount) for the executing entity,
 * records the mapping in g_EntityToModel and initializes the model with the
 * model id from the opcode operand and the owning entity id.
 */
s32 OpcodeFuncChar(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("char", 1);
    }
    g_EntityToModel[g_CurrentEntity] = g_FieldModelCount++;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].charId = GET_PARAM_U8(1);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].visible = 1;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].entityId = g_CurrentEntity;
    PC_INC(2);
    return 0;
}

/* Field-script opcode DFANM: set a model's default (looping) animation.
 *
 * Stores the animation id and playback speed (per-model base speed divided
 * by the speed operand) for the model attached to the executing entity.
 * A model holding the last frame of a script animation (state 3) is
 * released so the new default animation starts playing.
 */
s32 OpcodeFuncDfanm(void) {
    u8 modelIdx;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dfanm", 2);
    }
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        g_FieldModelIdleAnimId[g_EntityToModel[g_CurrentEntity]] =
            GET_PARAM_U8(1);
        D_80082248[g_EntityToModel[g_CurrentEntity]] =
            D_8009D828[g_EntityToModel[g_CurrentEntity]] / GET_PARAM_U8(2);
        modelIdx = g_EntityToModel[g_CurrentEntity];
        if (g_FieldModelAnimMode[modelIdx] == 3) {
            g_FieldModelAnimMode[modelIdx] = 0;
        }
    }
    PC_INC(3);
    return 1;
}

/* Field-script opcode CCANM: set one of the player animation ids
 * (0: idle, 1: walk, 2: run) used while the player controls a model.
 */
s32 OpcodeFuncCcanm(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("ccanm", 3);
    }
    switch (GET_PARAM_U8(3)) {
    case 0:
        g_pFieldState->idleAnimId = GET_PARAM_U8(1);
        break;
    case 1:
        g_pFieldState->walkAnimId = GET_PARAM_U8(1);
        break;
    case 2:
        g_pFieldState->runAnimId = GET_PARAM_U8(1);
        break;
    }
    PC_INC(4);
    return 0;
}

/* Starts the animation requested by the current ANIME-style opcode on the
 * model attached to the executing entity: animation id from the first
 * operand, playback speed from the per-model base speed divided by the
 * second operand, frame counter rewound and the last frame looked up in
 * the animation header of the model's file.
 */
void StartModelAnimation(void) {
    u8 modelIdx;
    u8* anims;
    FieldModelEntry* model;

    g_FieldModels[g_EntityToModel[g_CurrentEntity]].activeAnimId =
        GET_PARAM_U8(1);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed =
        D_8009D828[g_EntityToModel[g_CurrentEntity]] / GET_PARAM_U8(2);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame = 0;
    modelIdx = g_EntityToModel[g_CurrentEntity];
    model =
        &g_FieldModelData
             ->modelEntries[g_FieldModelLoaderData[modelIdx].modelEntryIndex];
    anims = model->modelData + model->animationOffset;
    g_FieldModels[modelIdx].animLastFrame =
        *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] - 1;
}

/* Field-script opcode ANIME1/ANIME2: play an animation on the entity's
 * model. g_FieldCurrentOpcode distinguishes which opcode invoked the handler:
 * the asynchronous variant (0xAE, ANIME2) marks the model as playing (state 5)
 * and lets the script continue, while ANIME1 blocks (state 2) until the
 * animation system reports completion (state 4), then resets the model to
 * its default animation.
 */
s32 OpcodeFuncAnime(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("anime", 2);
    }

    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(3);
        return 0;
    }

    switch (g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]]) {
    case 0:
    case 1:
    case 3:
        StartModelAnimation();
        if (g_FieldCurrentOpcode == 0xAE) {
            g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 5;
            PC_INC(3);
            return 0;
        }
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 2;
        break;
    case 4:
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 0;
        PC_INC(3);
        return 0;
    }
    return 1;
}

/* Field-script opcode ANIM!1/ANIM!2: like ANIME1/ANIME2 but the model
 * keeps holding the last frame once the animation completes (state 3)
 * instead of returning to its default animation. 0xAE becomes 0xAF and
 * state 5 becomes 6 to tell the two opcode pairs apart.
 */
s32 OpcodeFuncAnimEx(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("anim!", 2);
    }

    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(3);
        return 0;
    }

    switch (g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]]) {
    case 0:
    case 1:
    case 3:
        StartModelAnimation();
        if (g_FieldCurrentOpcode == 0xAF) {
            g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 6;
            PC_INC(3);
            return 0;
        }
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 2;
        break;
    case 4:
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 3;
        PC_INC(3);
        return 0;
    }
    return 1;
}

/* CANIM1/CANIM2 (change animation): play an animation whose id, start frame
 * and end frame all come from the script scaled by a divisor in operand 4,
 * then clamp the end frame to the animation's own length. The
 * g_FieldModelAnimMode state machine and the 0xB0 test are ANIME's, so the
 * outer shape is OpcodeFuncAnime's and the body is StartModelAnimation's with
 * the divisor threaded through. Twin of OpcodeFuncCanmEx. */
s32 OpcodeFuncCanim(void) {
    u8 modelIdx;
    u8* anims;
    FieldModelEntry* model;
    s16 divisor;
    s16 lastFrame;
    s16 maxFrame;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("canim", 4);
    }

    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(5);
        return 0;
    }

    switch (g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]]) {
    case 0:
    case 1:
    case 3:
        divisor = GET_PARAM_U8(4);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].activeAnimId =
            GET_PARAM_U8(1);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed =
            D_8009D828[g_EntityToModel[g_CurrentEntity]] / divisor;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame =
            (GET_PARAM_U8(2) / divisor) * 16;
        modelIdx = g_EntityToModel[g_CurrentEntity];
        model = &g_FieldModelData->modelEntries[g_FieldModelLoaderData[modelIdx]
                                                    .modelEntryIndex];
        anims = model->modelData + model->animationOffset;
        maxFrame = *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] - 1;
        lastFrame = GET_PARAM_U8(3) / divisor;
        if (maxFrame < lastFrame) {
            g_FieldModels[modelIdx].animLastFrame = maxFrame;
        } else {
            g_FieldModels[modelIdx].animLastFrame = lastFrame;
        }
        if (g_FieldCurrentOpcode == 0xB0) {
            g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 5;
            PC_INC(5);
            return 0;
        }
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 2;
        break;
    case 4:
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 0;
        PC_INC(5);
        return 0;
    }
    return 1;
}

/* CANM!1/CANM!2 (change animation, hold the last frame): OpcodeFuncCanim with
 * three constants changed -- opcode 0xB1 instead of 0xB0, the asynchronous
 * state 6 instead of 5, and state 4 resetting to 3 rather than 0, the same
 * ANIME/ANIM! pairing one opcode down -- plus one real difference: the start
 * frame in operand 2 is *not* divided by the divisor here, only multiplied by
 * 16. Everything else, including the `s16` divisor and frame bound and the
 * `break` at the end of the arm, is CANIM's; read that note for why. */
s32 OpcodeFuncCanmEx(void) {
    u8 modelIdx;
    u8* anims;
    FieldModelEntry* model;
    s16 divisor;
    s16 lastFrame;
    s16 maxFrame;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("canm!", 4);
    }

    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(5);
        return 0;
    }

    switch (g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]]) {
    case 0:
    case 1:
    case 3:
        divisor = GET_PARAM_U8(4);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].activeAnimId =
            GET_PARAM_U8(1);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed =
            D_8009D828[g_EntityToModel[g_CurrentEntity]] / divisor;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame =
            GET_PARAM_U8(2) * 16;
        modelIdx = g_EntityToModel[g_CurrentEntity];
        model = &g_FieldModelData->modelEntries[g_FieldModelLoaderData[modelIdx]
                                                    .modelEntryIndex];
        anims = model->modelData + model->animationOffset;
        maxFrame = *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] - 1;
        lastFrame = GET_PARAM_U8(3) / divisor;
        if (maxFrame < lastFrame) {
            g_FieldModels[modelIdx].animLastFrame = maxFrame;
        } else {
            g_FieldModels[modelIdx].animLastFrame = lastFrame;
        }
        if (g_FieldCurrentOpcode == 0xB1) {
            g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 6;
            PC_INC(5);
            return 0;
        }
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 2;
        break;
    case 4:
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 3;
        PC_INC(5);
        return 0;
    }
    return 1;
}

s32 OpcodeFuncAnimw(void) {
    u8 modelIdx;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("animw", 0);
    }
    modelIdx = g_EntityToModel[g_CurrentEntity];
    if (modelIdx == 0xFF) {
        PC_INC(1);
        return 0;
    }
    switch (g_FieldModelAnimMode[modelIdx]) {
    case 2:
    case 5:
    case 6:
        return 1;
    case 4:
        g_FieldModelAnimMode[modelIdx] = 0;
        break;
    }
    PC_INC(1);
    return 0;
}

s32 OpcodeFuncAnimb(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("animb", 0);
    }
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animLastFrame =
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame >>
            4;
        g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 3;
    }
    PC_INC(1);
    return 0;
}

/////////////////////////////////////////////////
// Start of field_opcode_model_move.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Start of field_opcode_model_move.c
/////////////////////////////////////////////////

/* MOVE: walk the entity's model to an (x, y) target read from the event
 * memory banks, in 12.4 fixed point. The animation is picked from the map's
 * scale -- three times currentFieldScale against the model's MoveSpeed
 * chooses run (2) over walk (1) -- and only restarted when it actually
 * changes, which is what the two `goto started` arms are for. The tail is the
 * per-frame half: state 1 means still walking, state 2 means arrived, and the
 * opcode only advances the PC on arrival. */
s32 OpcodeFuncMove(void) {
    FieldEntity* entity;
    FieldEntity* moving;
    u8 modelIdx;
    u8* anims;
    FieldModelEntry* model;
    s32 entryIdx;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("move", 5);
    }

    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(6);
        return 0;
    }

    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionArg = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].DirLock = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndX =
        (FieldEventReadMemoryS16(1, 2) << 16) >> 4;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndY =
        (FieldEventReadMemoryS16(2, 4) << 16) >> 4;

    entity = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
    if (g_pFieldState->currentFieldScale * 3 < entity->MoveSpeed) {
        if (entity->activeAnimId == 2) {
            goto started;
        }
        entity->activeAnimId = 2;
    } else {
        if (entity->activeAnimId == 1) {
            goto started;
        }
        entity->activeAnimId = 1;
    }
    if (g_EntityToModel) {
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed = 0x10;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame = 0;
        modelIdx = g_EntityToModel[g_CurrentEntity];
        entryIdx = g_FieldModelLoaderData[modelIdx].modelEntryIndex;
        model = &g_FieldModelData->modelEntries[entryIdx];
        anims = model->modelData + model->animationOffset;
        g_FieldModels[modelIdx].animLastFrame =
            *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] - 1;
    } else {
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed = 0x10;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame = 0;
        modelIdx = g_EntityToModel[g_CurrentEntity];
        entryIdx = g_FieldModelLoaderData[modelIdx].modelEntryIndex;
        model = &g_FieldModelData->modelEntries[entryIdx];
        anims = model->modelData + model->animationOffset;
        g_FieldModels[modelIdx].animLastFrame =
            *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] - 1;
    }

started:
    g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 1;
    moving = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
    if (moving->scriptedMoveMode == 1) {
        switch (moving->ActionState) {
        case 1:
            return 1;
        case 2:
            moving->scriptedMoveMode = 0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
            g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 0;
            PC_INC(6);
            return 0;
        }
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode = 1;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
    return 1;
}

/* FMOVE (0xAD): walk the current entity to a target point, letting it turn to
 * face the way it is going -- it clears DirLock, where CMOVE below sets it.
 * If a move is in flight (scriptedMoveMode 1) it polls: ActionState 1 means
 * still walking (return 1), 2 means arrived (clear the mode and step the PC),
 * and anything else falls through to the same block the not-in-flight path
 * reaches, which arms a fresh move. */
s32 OpcodeFuncFmove(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("fmove", 5);
    }
    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(6);
        return 0;
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionArg = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].DirLock = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndX =
        (s32)FieldEventReadMemoryS16(1, 2) << 12;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndY =
        (s32)FieldEventReadMemoryS16(2, 4) << 12;
    if (g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode == 1) {
        switch (g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState) {
        case 1:
            return 1;
        case 2:
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode =
                0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
            PC_INC(6);
            return 0;
        }
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode = 1;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
    return 1;
}

/* CMOVE (0xA9): walk the current entity to a target point holding its facing
 * -- it sets DirLock, where FMOVE below clears it, and clears it again on
 * arrival. Same three-way poll as FMOVE: ActionState 1 keeps waiting, 2
 * finishes and steps the PC, anything else falls into the shared block that
 * arms a fresh move. Landed from FMOVE's reading of the target; see that
 * function's note for why the default has to reach the tail block rather than
 * return on its own. */
s32 OpcodeFuncCmove(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("cmove", 5);
    }
    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(6);
        return 0;
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionArg = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].DirLock = 1;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndX =
        (s32)FieldEventReadMemoryS16(1, 2) << 12;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndY =
        (s32)FieldEventReadMemoryS16(2, 4) << 12;
    if (g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode == 1) {
        switch (g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState) {
        case 1:
            return 1;
        case 2:
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].DirLock = 0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode =
                0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
            PC_INC(6);
            return 0;
        }
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode = 1;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
    return 1;
}

s32 OpcodeFuncFcfix(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("fcfix", 1);
    }
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].DirLock =
            GET_PARAM_U8(1);
    }
    PC_INC(2);
    return 0;
}

/* JUMP (0xC0): make the current entity jump to a target over a number of
 * frames. ActionState 1 keeps polling (return 1), 2 finishes -- and *returns*,
 * stepping the PC past the opcode -- and anything else falls through to arm a
 * fresh jump. The parked body had the ActionState 2 arm fall through into the
 * arming code instead of returning, which is a different program and was worth
 * 17 of the 23 rows. */
s32 OpcodeFuncJump(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("jump", 8);
    }
    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(11);
        return 0;
    }
    if (g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode ==
        SMODE_JUMP) {
        switch (g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState) {
        case 1:
            return 1;
        case 2:
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode =
                SMODE_NONE;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
            PC_INC(11);
            return 0;
        }
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode =
        SMODE_JUMP;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndX =
        (s32)FieldEventReadMemoryS16(1, 3) << 12;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndY =
        (s32)FieldEventReadMemoryS16(2, 5) << 12;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndI =
        FieldEventReadMemoryS16(3, 7);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveSteps =
        FieldEventReadMemoryS16(4, 9);
    return 1;
}

/* Field-script opcode LADER: send a model up or down a ladder or climb path.
 *
 * The direction operand picks the climb mode (4 = one pair of animations,
 * 5 = the other) and which end of it the model starts from. The three
 * coordinate operands are the destination in 1/16th units, and a fourth
 * names the walk mesh triangle it lands on. While a climb is already running
 * the opcode blocks on the model's action state, then clears it and steps
 * past its 15 bytes.
 */
s32 OpcodeFuncLader(void) {
    s32 mode;
    u8 modelIdx;
    FieldModelEntry* entry;
    u8* anims;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("lader", 8);
    }
    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(0xF);
        return 0;
    }
    mode = g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode;
    if (mode < 6) {
        if (mode >= 4) {
            switch (
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState) {
            case 1:
                return 1;
            case 2:
                g_FieldModels[g_EntityToModel[g_CurrentEntity]]
                    .scriptedMoveMode = 0;
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
                PC_INC(0xF);
                return 0;
            }
        }
    }
    switch (GET_PARAM_U8(0xB)) {
    case 0:
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode = 4;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionArg = 0;
        break;
    case 1:
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode = 4;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionArg = 1;
        break;
    case 2:
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode = 5;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionArg = 0;
        break;
    case 3:
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode = 5;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionArg = 1;
        break;
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndX =
        FieldEventReadMemoryS16(1, 3) << 12;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndY =
        FieldEventReadMemoryS16(2, 5) << 12;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndZ =
        FieldEventReadMemoryS16(3, 7) << 12;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndI =
        FieldEventReadMemoryS16(4, 9);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].activeAnimId =
        GET_PARAM_U8(0xC);
    modelIdx = g_EntityToModel[g_CurrentEntity];
    g_FieldModels[modelIdx].animSpeed =
        D_8009D828[modelIdx] / GET_PARAM_U8(0xE);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame = 0;
    modelIdx = g_EntityToModel[g_CurrentEntity];
    entry =
        &g_FieldModelData
             ->modelEntries[g_FieldModelLoaderData[modelIdx].modelEntryIndex];
    anims = entry->modelData + entry->animationOffset;
    g_FieldModels[modelIdx].animLastFrame =
        *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] - 1;
    g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].Dir = GET_PARAM_U8(0xD);
    return 1;
}

s32 OpcodeFuncPmova(void) {
    u8 partyId;
    u8 actorId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("pmova", 1);
    }
    partyId = D_8009D391[GET_PARAM_U8(1)];
    if (partyId == 0xFF) {
        actorId = 0xFF;
    } else {
        actorId = g_CharIdToEntity[partyId];
    }
    return FieldMoveToEntityUpdate(actorId);
}

s32 OpcodeFuncMova(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mova", 1);
    }
    return FieldMoveToEntityUpdate(GET_PARAM_U8(1));
}

/* MOVA/PMOVA: retarget the current entity's scripted move at another entity,
 * every frame, so the mover follows a moving target. The destination is the
 * target's live position and its solid radius becomes the stop distance; the
 * rest is OpcodeFuncMove's state machine, which is why the walk/run choice
 * and the animation restart are the same code. Returns the dispatcher's
 * "opcode consumed" flag, so MOVA and PMOVA pass it through rather than being
 * void. */
s32 FieldMoveToEntityUpdate(s32 targetEntityId) {
    FieldEntity* moving;
    FieldModelEntry* model;
    u8* anims;
    u8 modelIdx;
    s32 entryIdx;

    if (g_EntityToModel[g_CurrentEntity] == 0xFF ||
        g_EntityToModel[targetEntityId & 0xFF] == 0xFF) {
        PC_INC(2);
        return 0;
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionArg =
        g_FieldModels[g_EntityToModel[targetEntityId & 0xFF]].SolidRange;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].DirLock = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndX =
        g_FieldModels[g_EntityToModel[targetEntityId & 0xFF]].PosX;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveEndY =
        g_FieldModels[g_EntityToModel[targetEntityId & 0xFF]].PosY;

    moving = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
    if (moving->scriptedMoveMode == 1) {
        switch (moving->ActionState) {
        case 1:
            if (g_pFieldState->currentFieldScale * 3 < moving->MoveSpeed) {
                if (moving->activeAnimId == 2) {
                    goto started;
                }
                moving->activeAnimId = 2;
            } else {
                if (moving->activeAnimId == 1) {
                    goto started;
                }
                moving->activeAnimId = 1;
            }
            if (g_EntityToModel) {
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed =
                    0x10;
                g_FieldModels[g_EntityToModel[g_CurrentEntity]]
                    .animCurrentFrame = 0;
                modelIdx = g_EntityToModel[g_CurrentEntity];
                entryIdx = g_FieldModelLoaderData[modelIdx].modelEntryIndex;
                model = &g_FieldModelData->modelEntries[entryIdx];
                anims = model->modelData + model->animationOffset;
                g_FieldModels[modelIdx].animLastFrame =
                    *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] -
                    1;
            } else {
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed =
                    0x10;
                g_FieldModels[g_EntityToModel[g_CurrentEntity]]
                    .animCurrentFrame = 0;
                modelIdx = g_EntityToModel[g_CurrentEntity];
                entryIdx = g_FieldModelLoaderData[modelIdx].modelEntryIndex;
                model = &g_FieldModelData->modelEntries[entryIdx];
                anims = model->modelData + model->animationOffset;
                g_FieldModels[modelIdx].animLastFrame =
                    *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] -
                    1;
            }
        started:
            g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 1;
            return 1;
        case 2:
            moving->scriptedMoveMode = 0;
            g_FieldModelAnimMode[g_EntityToModel[g_CurrentEntity]] = 0;
            PC_INC(2);
            return 0;
        }
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].scriptedMoveMode = 1;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].ActionState = 0;
    return 1;
}

void OpcodeFuncDira(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dira", 1);
    }
    FieldEventSetDirByActorId(GET_PARAM_U8(1));
}

void OpcodeFuncPdira(void) {
    u8 partyId;
    u8 actorId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("pdira", 1);
    }
    partyId = D_8009D391[GET_PARAM_U8(1)];
    if (partyId == 0xFF) {
        actorId = 0xFF;
    } else {
        actorId = g_CharIdToEntity[partyId];
    }
    FieldEventSetDirByActorId(actorId);
}

/* Face the current entity towards another entity: read both models' fixed
 * point positions, compute the direction with FieldEntityDirByVec, and snap
 * the current entity's Dir to it, cancelling any turn in progress. This is the
 * whole of the DIRA/PDIRA opcode, not a helper -- it steps the script PC past
 * the two bytes itself and returns 1, or 0 when either entity has no model. */
s32 FieldEventSetDirByActorId(s16 actorId) {
    VECTOR from;
    VECTOR to;
    s32 sqrDist;

    if (g_EntityToModel[g_CurrentEntity] == 0xFF ||
        g_EntityToModel[actorId] == 0xFF) {
        PC_INC(2);
        return 0;
    }
    from.vx = g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosX >> 12;
    from.vy = g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosY >> 12;
    from.vz = g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosZ >> 12;
    to.vx = g_FieldModels[g_EntityToModel[actorId]].PosX >> 12;
    to.vy = g_FieldModels[g_EntityToModel[actorId]].PosY >> 12;
    to.vz = g_FieldModels[g_EntityToModel[actorId]].PosZ >> 12;
    if (from.vx == to.vx && from.vy == to.vy) {
        from.vx = from.vx + 1;
    }
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].Dir =
        FieldEntityDirByVec(&from, &to, &sqrDist);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnType = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnStep = 0;
    PC_INC(2);
    return 1;
}

s32 OpcodeFuncTura(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("tura", 3);
    }
    return FieldEntityTurnToEntity(GET_PARAM_U8(1));
}

s32 OpcodeFuncPtura(void) {
    u8 partyId;
    u8 actorId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("ptura", 3);
    }
    partyId = D_8009D391[GET_PARAM_U8(1)];
    if (partyId == 0xFF) {
        actorId = 0xFF;
    } else {
        actorId = g_CharIdToEntity[partyId];
    }
    return FieldEntityTurnToEntity(actorId);
}

/* Turn the current entity's model to face another entity: snapshot the
 * current direction, derive the target facing from the position delta with
 * FieldEntityDirByVec, and hand the turn to the state machine. Operand 3
 * picks the direction the same way TURNR's does -- 0 clockwise, 1
 * anticlockwise, 2 whichever way is shorter. TurnType 3 means the turn
 * finished, and only then does the PC advance; the 0/1 this returns is the
 * dispatcher's "opcode consumed" flag, which is why TURA and PTURA pass it
 * through rather than being void.
 */
s32 FieldEntityTurnToEntity(s16 actorId) {
    VECTOR from;
    VECTOR to;
    s32 sqrDist;
    FieldEntity* entity;
    FieldEntity* snapshot;
    FieldEntity* turning;
    FieldEntity* stepping;
    s16 delta;
    s16 dist;

    if (g_EntityToModel[g_CurrentEntity] == 0xFF ||
        g_EntityToModel[actorId] == 0xFF) {
        PC_INC(4);
        return 0;
    }
    entity = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
    if (entity->TurnType == 3) {
        entity->TurnType = 0;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnStep = 0;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnSteps = 0;
        PC_INC(4);
        return 0;
    }
    if (entity->TurnStep == 0 || entity->TurnType != 2 ||
        entity->TurnSteps != GET_PARAM_U8(2)) {
        snapshot = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
        snapshot->TurnStart = snapshot->Dir;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnType = 2;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnSteps =
            GET_PARAM_U8(2);
        from.vx = g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosX >> 12;
        from.vy = g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosY >> 12;
        from.vz = g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosZ >> 12;
        to.vx = g_FieldModels[g_EntityToModel[actorId]].PosX >> 12;
        to.vy = g_FieldModels[g_EntityToModel[actorId]].PosY >> 12;
        to.vz = g_FieldModels[g_EntityToModel[actorId]].PosZ >> 12;
        if (from.vx == to.vx) {
            if (from.vy == to.vy) {
                from.vx = from.vx + 1;
            }
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnEnd =
            FieldEntityDirByVec(&from, &to, &sqrDist) & 0xFF;
        switch (GET_PARAM_U8(3)) {
        case 2:
            turning = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
            delta = turning->TurnEnd - turning->TurnStart;
            dist = delta;
            if (delta < 0) {
                dist = ~delta + 1;
            }
            if (dist >= 0x81) {
                if (turning->TurnEnd > turning->TurnStart) {
                    turning->TurnEnd = turning->TurnEnd - 0x100;
                } else {
                    turning->TurnEnd = turning->TurnEnd + 0x100;
                }
            }
            break;
        case 1:
            stepping = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
            if (stepping->Dir < stepping->TurnEnd) {
                stepping->TurnEnd = stepping->TurnEnd - 0x100;
            }
            break;
        case 0:
            stepping = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
            if (stepping->TurnEnd < stepping->Dir) {
                stepping->TurnEnd = stepping->TurnEnd + 0x100;
            }
            break;
        }
    }
    return 1;
}

/* OFSTD/OFSTL/OFSTC: start a positional offset on the current entity's model.
 * The three names are one handler: operand 3 is the interpolation mode and
 * also selects which name the debug print uses. Mode 0 snaps the offset to
 * its target immediately; the others record the current offset as the start
 * of an interpolation. The PC always advances, so this returns 0
 * unconditionally. */
s32 OpcodeFuncOfstd(void) {
    u8 ofsType;

    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            switch (GET_PARAM_U8(3)) {
            case 0:
                DebugPrintOpcode("ofstd", 5);
                break;
            case 1:
                DebugPrintOpcode("ofstl", 5);
                break;
            case 2:
                DebugPrintOpcode("ofstc", 5);
                break;
            }
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetStep = 0;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetSteps =
            FieldEventReadMemoryS16(4, 0xA);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetEndX =
            FieldEventReadMemoryS16(1, 4);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetEndY =
            FieldEventReadMemoryS16(2, 6);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetEndZ =
            FieldEventReadMemoryS16(3, 8);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].OfsType = ofsType =
            GET_PARAM_U8(3);
        if (ofsType != 0) {
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetStartX =
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetX;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetStartY =
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetY;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetStartZ =
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetZ;
        } else {
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetX =
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetEndX;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetY =
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetEndY;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetZ =
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetEndZ;
        }
    }
    PC_INC(0xC);
    return 0;
}

/* Block until this entity's offset animation finishes. OfsType 3 means the last
 * step ran, so clear it and fall through; 0 means there was never one. */
s32 OpcodeFuncOfstw(void) {
    FieldEntity* model;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("ofstw", 0);
    }
    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(1);
        return 0;
    }
    model = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
    if (model->OfsType != 0 && model->OfsType != 3) {
        return 1;
    }
    model->OfsType = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetStep = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].OffsetSteps = 0;
    PC_INC(1);
    return 0;
}

/* Block until this entity's turn finishes. Returning 1 without advancing the
 * PC re-runs the opcode next frame; TurnType 3 means the turn just completed,
 * so clear it and fall through. */
s32 OpcodeFuncTurnw(void) {
    FieldEntity* model;

    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(1);
        return 0;
    }
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("turnw", 0);
    }
    model = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
    if (model->TurnType == 0) {
        PC_INC(1);
        return 0;
    }
    if (model->TurnType != 3) {
        return 1;
    }
    model->TurnType = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnStep = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnSteps = 0;
    PC_INC(1);
    return 0;
}

/* TURN/TURNC: turn the current entity's model to an absolute direction read
 * from the event memory banks. Sibling of TURNR (which takes a relative
 * direction) and of FieldEntityTurnToEntity (which derives one from another
 * entity's position); the state machine and the 0/1 return are the same in
 * all three. */
s32 OpcodeFuncTurn(void) {
    s16 dir;

    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            switch (GET_PARAM_U8(5)) {
            case 1:
                DebugPrintOpcode("turn", 5);
                break;
            case 2:
                DebugPrintOpcode("turnc", 5);
                break;
            }
        }
        if (g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnType == 3) {
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnType = 0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnStep = 0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnSteps = 0;
            PC_INC(6);
            return 0;
        }
        dir = FieldEventReadMemoryS16(2, 2);
        if (g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnStep != 0 &&
            (s16)dir ==
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnEnd &&
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnType ==
                GET_PARAM_U8(5) &&
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnSteps ==
                GET_PARAM_U8(4)) {
            goto done;
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnStart =
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].Dir;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnType =
            GET_PARAM_U8(5);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnSteps =
            GET_PARAM_U8(4);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnEnd = dir;
        goto done;
    }
    PC_INC(6);
    return 0;
done:
    return 1;
}

/* TURNR/TURNL/TRNRC/TRNLC: start (or restart) a turn on the current entity's
 * model. Operand 5 selects the turn kind and operand 3 the direction --
 * 0 clockwise, 1 anticlockwise, 2 whichever way is shorter -- which is why
 * one handler prints four different opcode names. The turn is only restarted
 * when the kind or the step count changed, so a script can spam the opcode
 * every frame; TurnType 3 means the turn system reported completion, and only
 * then does the PC advance. */
s32 OpcodeFuncTurnr(void) {
    FieldEntity* entity;
    FieldEntity* snapshot;
    FieldEntity* turning;
    FieldEntity* stepping;
    char* name;
    s16 delta;
    s16 dist;

    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            switch (GET_PARAM_U8(5)) {
            case 1:
                if (GET_PARAM_U8(3) != 0) {
                    name = "turnl";
                } else {
                    name = "turnr";
                }
                DebugPrintOpcode(name, 5);
                break;
            case 2:
                if (GET_PARAM_U8(3) != 0) {
                    name = "trnlc";
                } else {
                    name = "trnrc";
                }
                DebugPrintOpcode(name, 5);
                break;
            }
        }

        entity = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
        if (entity->TurnType == 3) {
            entity->TurnType = 0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnStep = 0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnSteps = 0;
            PC_INC(6);
            return 0;
        }
        if (entity->TurnStep == 0 || entity->TurnType != GET_PARAM_U8(5) ||
            entity->TurnSteps != GET_PARAM_U8(4)) {
            snapshot = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
            snapshot->TurnStart = snapshot->Dir;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnType =
                GET_PARAM_U8(5);
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnSteps =
                GET_PARAM_U8(4);
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnEnd =
                FieldEventReadMemoryU8(2, 2) & 0xFF;
            switch (GET_PARAM_U8(3)) {
            case 2:
                turning = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
                delta = turning->TurnEnd - turning->TurnStart;
                dist = delta;
                if (delta < 0) {
                    dist = ~delta + 1;
                }
                if (dist >= 0x81) {
                    if (turning->TurnEnd > turning->TurnStart) {
                        turning->TurnEnd = turning->TurnEnd - 0x100;
                    } else {
                        turning->TurnEnd = turning->TurnEnd + 0x100;
                    }
                }
                break;
            case 1:
                stepping = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
                if (stepping->Dir < stepping->TurnEnd) {
                    stepping->TurnEnd = stepping->TurnEnd - 0x100;
                }
                break;
            case 0:
                stepping = &g_FieldModels[g_EntityToModel[g_CurrentEntity]];
                if (stepping->TurnEnd < stepping->Dir) {
                    stepping->TurnEnd = stepping->TurnEnd + 0x100;
                }
                break;
            }
        }
        goto done;
    }
    PC_INC(6);
    return 0;
done:
    return 1;
}

/* Snap this entity to a facing, cancelling any turn in progress. Returns 1 when
 * the entity actually has a model, unlike most opcodes. */
s32 OpcodeFuncDir(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("dir", 2);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].Dir =
            FieldEventReadMemoryU8(2, 2);
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnType = 0;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TurnStep = 0;
        PC_INC(3);
        return 1;
    }
    PC_INC(3);
    return 0;
}

/* SLIDR: set this entity's collision radius. The script value is in map units,
 * so it is scaled by the field's own scale and divided back down by 512. */
s32 OpcodeFuncSlidr(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("slidR", 2);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].SolidRange =
            (FieldEventReadMemoryU8(2, 2) * g_pFieldState->currentFieldScale) /
            512;
    }
    PC_INC(3);
    return 0;
}

/* SLDR2: SLIDR with a 16-bit radius. */
s32 OpcodeFuncSldr2(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("sldR2", 3);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].SolidRange =
            (FieldEventReadMemoryS16(2, 2) * g_pFieldState->currentFieldScale) /
            512;
    }
    PC_INC(4);
    return 0;
}

/* TALKR: set this entity's talk radius, scaled the same way as SLIDR. */
s32 OpcodeFuncTalkr(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("talkR", 2);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TalkRange =
            (FieldEventReadMemoryU8(2, 2) * g_pFieldState->currentFieldScale) /
            512;
    }
    PC_INC(3);
    return 0;
}

/* TLKR2: TALKR with a 16-bit radius. */
s32 OpcodeFuncTlkr2(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("tlkR2", 3);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].TalkRange =
            (FieldEventReadMemoryS16(2, 2) * g_pFieldState->currentFieldScale) /
            512;
    }
    PC_INC(4);
    return 0;
}

/////////////////////////////////////////////////
// Start of field_opcode_model_state.c
/////////////////////////////////////////////////

/* MSPED: set this entity's movement speed, scaled like the radius opcodes. */
s32 OpcodeFuncMsped(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("msped", 3);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].MoveSpeed =
            (FieldEventReadMemoryS16(2, 2) * g_pFieldState->currentFieldScale) /
            512;
    }
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncAsped(void) {
    u8 modelIdx;
    s16 speed;

    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("asped", 3);
        }
        speed = FieldEventReadMemoryS16(2, 2);
        modelIdx = g_EntityToModel[g_CurrentEntity];
        g_FieldModels[modelIdx].animSpeed = speed;
        D_8009D828[modelIdx] = speed;
    }
    PC_INC(4);
    return 0;
}

/* GTDIR: write another entity's facing direction back into a memory bank. */
s32 OpcodeFuncGtdir(void) {
    u8 entityId;

    entityId = GET_PARAM_U8(2);
    if (g_EntityToModel[entityId] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("gtdir", 3);
        }
        FieldEventWriteMemoryU8(
            2, 3, g_FieldModels[g_EntityToModel[entityId]].Dir);
    }
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncPgtdr(void) {
    u8 slot;
    u8 partyId;
    u8 actorId;

    slot = GET_PARAM_U8(2);
    if (slot < 3) {
        partyId = D_8009D391[slot];
        if (partyId != 0xFF) {
            actorId = g_CharIdToEntity[partyId];
            if (actorId != 0xFF) {
                if (g_EntityToModel[actorId] != 0xFF) {
                    if (g_DebugLevel & 3) {
                        DebugPrintOpcode("pgtdr", 3);
                    }
                    FieldEventWriteMemoryU8(
                        2, 3, g_FieldModels[g_EntityToModel[actorId]].Dir);
                }
            }
        }
    }
    PC_INC(4);
    return 0;
}

/* GETAI: write another entity's walkmesh triangle id back into a memory bank.
 */
s32 OpcodeFuncGetai(void) {
    u8 entityId;

    entityId = GET_PARAM_U8(2);
    if (g_EntityToModel[entityId] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("getai", 3);
        }
        FieldEventWriteMemoryS16(
            2, 3, g_FieldModels[g_EntityToModel[entityId]].PosI);
    }
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncGetaxy(void) {
    u8 entityId;

    entityId = GET_PARAM_U8(2);
    if (g_EntityToModel[entityId] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("getaxy", 4);
        }
        FieldEventWriteMemoryS16(
            1, 3, g_FieldModels[g_EntityToModel[entityId]].PosX >> 12);
        FieldEventWriteMemoryS16(
            2, 4, g_FieldModels[g_EntityToModel[entityId]].PosY >> 12);
    }
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncAxyzi(void) {
    u8 entityId;

    entityId = GET_PARAM_U8(3);
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("axyzi", 7);
    }
    if (g_EntityToModel[entityId] != 0xFF) {
        FieldEventWriteMemoryS16(
            1, 4, g_FieldModels[g_EntityToModel[entityId]].PosX >> 12);
        FieldEventWriteMemoryS16(
            2, 5, g_FieldModels[g_EntityToModel[entityId]].PosY >> 12);
        FieldEventWriteMemoryS16(
            3, 6, g_FieldModels[g_EntityToModel[entityId]].PosZ >> 12);
        FieldEventWriteMemoryS16(
            4, 7, g_FieldModels[g_EntityToModel[entityId]].PosI);
    }
    PC_INC(8);
    return 0;
}

s32 OpcodeFuncPxyzi(void) {
    u8 slot;
    u8 partyId;
    u8 actorId;

    slot = GET_PARAM_U8(3);
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("pxyzi", 7);
    }
    if (slot < 3) {
        partyId = D_8009D391[slot];
        if (partyId < 9) {
            actorId = g_CharIdToEntity[partyId];
            if (g_EntityToModel[actorId] != 0xFF) {
                FieldEventWriteMemoryS16(
                    1, 4, g_FieldModels[g_EntityToModel[actorId]].PosX >> 12);
                FieldEventWriteMemoryS16(
                    2, 5, g_FieldModels[g_EntityToModel[actorId]].PosY >> 12);
                FieldEventWriteMemoryS16(
                    3, 6, g_FieldModels[g_EntityToModel[actorId]].PosZ >> 12);
                FieldEventWriteMemoryS16(
                    4, 7, g_FieldModels[g_EntityToModel[actorId]].PosI);
            }
        }
    }
    PC_INC(8);
    return 0;
}

s32 OpcodeFuncVisi(void) {
    u8 model;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("visi", 1);
    }
    model = g_EntityToModel[g_CurrentEntity];
    if (model != 0xFF) {
        g_FieldModels[model].visible = GET_PARAM_U8(1);
    }
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncTlkon(void) {
    u8 model;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("tlkon", 1);
    }
    model = g_EntityToModel[g_CurrentEntity];
    if (model != 0xFF) {
        g_FieldModels[model].TalkOff = GET_PARAM_U8(1);
    }
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncXyzi(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("xyzi", 8);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosX =
            FieldEventReadMemoryS16(1, 3) << 12;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosY =
            FieldEventReadMemoryS16(2, 5) << 12;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosZ =
            FieldEventReadMemoryS16(3, 7) << 12;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosI =
            FieldEventReadMemoryS16(4, 9);
    }
    PC_INC(11);
    return 1;
}

s32 OpcodeFuncXyz(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("xyz", 8);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosX =
            FieldEventReadMemoryS16(1, 3) << 12;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosY =
            FieldEventReadMemoryS16(2, 5) << 12;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosZ =
            FieldEventReadMemoryS16(3, 7) << 12;
    }
    PC_INC(9);
    return 1;
}

s32 OpcodeFuncXyi(void) {
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("xyi", 8);
        }
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosX =
            FieldEventReadMemoryS16(1, 3) << 12;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosY =
            FieldEventReadMemoryS16(2, 5) << 12;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].PosI =
            FieldEventReadMemoryS16(3, 7);
    }
    PC_INC(9);
    return 1;
}

/////////////////////////////////////////////////
// Start of field_opcode_message.c
/////////////////////////////////////////////////

s32 OpcodeFuncMes(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mes", 2);
    }
    if (FieldDialogMessageUpdateStates(GET_PARAM_U8(1), GET_PARAM_U8(2)) != 0) {
        PC_INC(3);
        return 0;
    }
    return 1;
}

s32 OpcodeFuncMpnam(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mpnam", 1);
    }
    CopyDialogToMapName(GET_PARAM_U8(1));
    PC_INC(2);
    return 0;
}

/* Field-script opcode ASK: run a menu prompt and store the chosen row.
 *
 * Blocks (returning 1 and holding the player) until FieldDialogAskUpdateStates
 * reports the prompt is finished; the answer is written back to the script
 * memory bank either way.
 */
s32 OpcodeFuncAsk(void) {
    s16 answer;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("ask", 6);
    }
    answer = FieldEventReadMemoryU8(2, 6);
    if (FieldDialogAskUpdateStates(
            GET_PARAM_U8(2), GET_PARAM_U8(3), GET_PARAM_U8(4), GET_PARAM_U8(5),
            &answer) != 0) {
        FieldEventWriteMemoryU8(2, 6, answer);
        g_pFieldState->characterLock = D_80081DC4;
        PC_INC(7);
        return 0;
    } else {
        FieldEventWriteMemoryU8(2, 6, answer);
        g_pFieldState->characterLock = 1;
        return 1;
    }
}

/////////////////////////////////////////////////
// Start of field_opcode_window.c
/////////////////////////////////////////////////

s32 OpcodeFuncWclsEx(void) {
    s16 window;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wcls!", 0);
    }
    window = GET_PARAM_U8(1);
    if (g_WindowToEntity[window] == 0xFF) {
        PC_INC(2);
        return 0;
    }
    FieldWindowSetStateToClose(window);
    FieldDialogMessageUpdateStates(window, 0);
    return 1;
}

s32 OpcodeFuncWsizw(void) {
    s16 window;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wsizw", 8);
    }
    window = GET_PARAM_U8(1);
    if (g_WindowToEntity[window] == 0xFF) {
        return OpcodeFuncWsize();
    }
    if (g_WindowToEntity[window] == g_CurrentEntity) {
        FieldWindowSetStateToClose(window);
        FieldDialogMessageUpdateStates(window, 0);
    }
    return 1;
}

s32 OpcodeFuncWsize(void) {
    s16 x;
    s16 y;
    s16 w;
    s16 h;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wsize", 8);
    }
    GET_PARAM_S16(x, 2);
    GET_PARAM_S16(y, 4);
    GET_PARAM_S16(w, 6);
    GET_PARAM_S16(h, 8);
    FieldDialogSetSize(GET_PARAM_U8(1), x, y, w, h);
    PC_INC(10);
    /* Not cosmetic: the statement boundary stops gcc sinking `move v0,zero`
     * into the load delay slot of the PC_INC read, which is what forces the
     * original's $v0 for the incremented value and its trailing `nop`.
     * Most likely a macro in the original. Found by decomp-permuter. */
    do {
        return 0;
    } while (0);
}

s32 OpcodeFuncWrow(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wrow", 2);
    }
    FieldDialogSetWindowHeight(GET_PARAM_U8(1), (GET_PARAM_U8(2) << 4) | 9);
    PC_INC(3);
    return 0;
}

s32 OpcodeFuncWmove(void) {
    s16 dx;
    s16 dy;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wmove", 8);
    }
    GET_PARAM_S16(dx, 2);
    GET_PARAM_S16(dy, 4);
    FieldDialogMove(GET_PARAM_U8(1), dx, dy);
    PC_INC(6);
    return 0;
}

s32 OpcodeFuncWrest(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wrest", 1);
    }
    FieldWindowReset(GET_PARAM_U8(1));
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncWclse(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wclse", 1);
    }
    if (FieldWindowSetStateToClose(GET_PARAM_U8(1)) != 0) {
        PC_INC(2);
        return 0;
    }
    return 1;
}

s32 OpcodeFuncWmode(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wmode", 3);
    }
    FieldDialogSetWindowStyleCbc(
        GET_PARAM_U8(1), GET_PARAM_U8(2), GET_PARAM_U8(3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x8F - **AND** - Bitwise AND (8-bit) */
s32 OpcodeFuncAnd(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("and", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2, FieldEventReadMemoryU8(1, 2) & FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x90 - **AND2** - Bitwise AND (16-bit) */
s32 OpcodeFuncAnd2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("and2", 3);
    }
    FieldEventWriteMemoryS16(
        1, 2, FieldEventReadMemoryS16(1, 2) & FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x91 - **OR** - Bitwise OR (8-bit) */
s32 OpcodeFuncOr(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("or", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2, FieldEventReadMemoryU8(1, 2) | FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x92 - **OR2** - Bitwise OR (16-bit) */
s32 OpcodeFuncOr2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("or2", 3);
    }
    FieldEventWriteMemoryS16(
        1, 2, FieldEventReadMemoryS16(1, 2) | FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x93 - **XOR** - Bitwise XOR (8-bit) */
s32 OpcodeFuncXor(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("xor", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2, FieldEventReadMemoryU8(1, 2) ^ FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x94 - **XOR2** - Bitwise XOR (16-bit) */
s32 OpcodeFuncXor2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("xor2", 3);
    }
    FieldEventWriteMemoryS16(
        1, 2, FieldEventReadMemoryS16(1, 2) ^ FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x85 - **PLUS** - Addition (8-bit) */
s32 OpcodeFuncPlus(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("plus", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2, FieldEventReadMemoryU8(1, 2) + FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x76 - **PLUS!** - Saturated Addition (8-bit) */
s32 OpcodeFuncPlusEx(void) {
    s16 sum;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("plus!", 3);
    }
    sum = FieldEventReadMemoryU8(1, 2) + FieldEventReadMemoryU8(2, 3);
    if (sum > 255) {
        sum = 255;
    }
    FieldEventWriteMemoryU8(1, 2, sum);
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x86 - **PLUS2** - Addition (16-bit) */
s32 OpcodeFuncPlus2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("plus2", 3);
    }
    FieldEventWriteMemoryS16(
        1, 2, FieldEventReadMemoryS16(1, 2) + FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x77 - **PLS2!** - Saturated Addition (16-bit) */
s32 OpcodeFuncPls2Ex(void) {
    s32 sum;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("pls2!", 3);
    }
    sum = FieldEventReadMemoryS16(1, 2) + FieldEventReadMemoryS16(2, 3);
    if (sum > 32767) {
        sum = 32767;
    }
    FieldEventWriteMemoryS16(1, 2, sum);
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x87 - **MINUS** - Subtraction (8-bit) */
s32 OpcodeFuncMinus(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("minus", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2, FieldEventReadMemoryU8(1, 2) - FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x78 - **MINS!** - Saturated Subtraction (8-bit) */
s32 OpcodeFuncMinsEx(void) {
    s16 differ;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mins!", 3);
    }
    differ = FieldEventReadMemoryU8(1, 2) - FieldEventReadMemoryU8(2, 3);
    if (differ < 0) {
        differ = 0;
    }
    FieldEventWriteMemoryU8(1, 2, differ);
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x88 - **MINS2** - Subtraction (16-bit) */
s32 OpcodeFuncMins2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mins2", 3);
    }
    FieldEventWriteMemoryS16(
        1, 2, FieldEventReadMemoryS16(1, 2) - FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x79 - **MNS2!** - Saturated Subtraction (16-bit) */
s32 OpcodeFuncMns2Ex(void) {
    s32 differ;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mns2!", 3);
    }
    differ = FieldEventReadMemoryS16(1, 2) - FieldEventReadMemoryS16(2, 3);
    if (differ <= 0x7FFF) {
        differ = 0x8000;
    }
    FieldEventWriteMemoryS16(1, 2, differ);
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x89 - **MUL** - Multiplication (8-bit) */
s32 OpcodeFuncMul(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mul", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2, FieldEventReadMemoryU8(1, 2) * FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x8A - **MUL2** - Multiplication (16-bit) */
s32 OpcodeFuncMul2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mul2", 3);
    }
    FieldEventWriteMemoryS16(
        1, 2, FieldEventReadMemoryS16(1, 2) * FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x8B - **DIV** - Division (8-bit) */
s32 OpcodeFuncDiv(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("div", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2, FieldEventReadMemoryU8(1, 2) / FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x8C - **DIV2** - Division (16-bit) */
s32 OpcodeFuncDiv2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("div2", 3);
    }
    FieldEventWriteMemoryS16(
        1, 2, FieldEventReadMemoryS16(1, 2) / FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x8D - **REMAI** - Modulus (8-bit) */
s32 OpcodeFuncRemai(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("remai", 3);
    }
    FieldEventWriteMemoryU8(
        1, 2, FieldEventReadMemoryU8(1, 2) % FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x8E - **REMA2** - Modulus (16-bit) */
s32 OpcodeFuncRema2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("rema2", 3);
    }
    FieldEventWriteMemoryS16(
        1, 2, FieldEventReadMemoryS16(1, 2) % FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x95 - **INC** - Increment (8-bit) */
s32 OpcodeFuncInc(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("inc", 2);
    }
    FieldEventWriteMemoryU8(2, 2, FieldEventReadMemoryU8(2, 2) + 1);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x7A - **INC!** - Saturated Increment (8-bit) */
s32 OpcodeFuncIncEx(void) {
    s16 result;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("inc!", 2);
    }
    result = FieldEventReadMemoryU8(2, 2) + 1;
    if (result >= 256) {
        result = 255;
    }
    FieldEventWriteMemoryU8(2, 2, result);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode  0x96 - **INC2** - Increment (16-bit) */
s32 OpcodeFuncInc2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("inc2", 3);
    }
    FieldEventWriteMemoryS16(2, 2, FieldEventReadMemoryS16(2, 2) + 1);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode  0x7B - **INC2!** - Saturated Increment (16-bit) */
s32 OpcodeFuncInc2Ex(void) {
    s32 sum;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("inc2!", 3);
    }
    sum = FieldEventReadMemoryS16(2, 2) + 1;
    if (sum > 32767) {
        sum = 32767;
    }
    FieldEventWriteMemoryS16(2, 2, sum);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x97 - **DEC** - Decrement (8-bit) */
s32 OpcodeFuncDec(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dec", 2);
    }
    FieldEventWriteMemoryU8(2, 2, FieldEventReadMemoryU8(2, 2) - 1);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x7C - **DEC!** - Saturated Decrement (8-bit) */
s32 OpcodeFuncDecEx(void) {
    s16 differ;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dec!", 2);
    }
    differ = FieldEventReadMemoryU8(2, 2) - 1;
    if (differ < 0) {
        differ = 0;
    }
    FieldEventWriteMemoryU8(2, 2, differ);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x98 - **DEC2** - Decrement (16-bit) */
s32 OpcodeFuncDec2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dec2", 3);
    }
    FieldEventWriteMemoryS16(2, 2, FieldEventReadMemoryS16(2, 2) - 1);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x7D - **DEC2!** - Saturated Decrement (16-bit) */
s32 OpcodeFuncDec2Ex(void) {
    s32 result;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dec2!", 3);
    }
    result = FieldEventReadMemoryS16(2, 2) - 1;
    if (result <= 0x7FFF) {
        result = 0x8000;
    }
    FieldEventWriteMemoryS16(2, 2, result);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x99 - **RANDM** - Random */
s32 OpcodeFuncRandm(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("randm", 2);
    }
    g_RandomTableIndex += g_RandomTableStep;
    FieldEventWriteMemoryU8(2, 2, g_RandomTable[g_RandomTableIndex]);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x7F - **RDMSD** - Seed Random Generator */
s32 OpcodeFuncRdmsd(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("rdmsd", 2);
    }
    g_RandomTableStep = (FieldEventReadMemoryU8(2, 2) << 4) + 1;
    PC_INC(3);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_background.c
/////////////////////////////////////////////////

s32 OpcodeFuncBgon(void) {
    u8 layer;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bgon", 3);
    }
    layer = FieldEventReadMemoryU8(1, 2);
    g_pFieldState->backgroundLayerVisibility[layer] |=
        1 << FieldEventReadMemoryU8(2, 3);
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncBgoff(void) {
    u8 layer;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bgoff", 3);
    }
    layer = FieldEventReadMemoryU8(1, 2);
    g_pFieldState->backgroundLayerVisibility[layer] &=
        ~(1 << FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncBgclr(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bgclr", 3);
    }
    g_pFieldState->backgroundLayerVisibility[FieldEventReadMemoryU8(2, 2)] = 0;
    PC_INC(3);
    return 0;
}

s32 OpcodeFuncBgrol(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bgrol", 3);
    }
    g_pFieldState->backgroundLayerVisibility[FieldEventReadMemoryU8(2, 2)] <<=
        1;
    PC_INC(3);
    return 0;
}

s32 OpcodeFuncBgrol2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bgrol", 3);
    }
    g_pFieldState->backgroundLayerVisibility[FieldEventReadMemoryU8(2, 2)] >>=
        1;
    PC_INC(3);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_movie.c
////////////////////////////////////////////////

/* Preload the movie named by the parameter, blocking until the load finishes.
 * Same post-then-poll shape as OpcodeFuncMovie, one event command earlier. */
s32 OpcodeFuncPmvie(void) {
    s16 movieId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("pmvie", 1);
    }
    if (g_FieldMovieLock != 0) {
        PC_INC(2);
        return 0;
    }
    switch (g_pFieldState->eventCmd) {
    case EVTCMD_LOAD_MOVIE:
        switch (g_pFieldState->movieCommandState) {
        case MOVCMD_ACTIVE:
            break;
        case MOVCMD_DONE:
            g_pFieldState->eventCmd = EVTCMD_NONE;
            g_pFieldState->movieCommandState = MOVCMD_IDLE;
            PC_INC(2);
            return 0;
        }
        return 1;
    case EVTCMD_NONE:
        g_pFieldState->eventCmd = EVTCMD_LOAD_MOVIE;
        movieId = GET_PARAM_U8(1);
        g_pFieldState->movieCommandState = MOVCMD_IDLE;
        g_pFieldState->eventCmdParam = movieId;
        break;
    }
    return 1;
}

/* Play the field map's movie, blocking until it finishes. Returning 1 without
 * advancing the PC re-runs the opcode next frame, so the request is posted once
 * as an event command and then polled. */
s32 OpcodeFuncMovie(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("movie", 0);
    }
    g_FieldMovieOpcodeActive = 1;
    if (g_FieldMovieLock != 0) {
        g_FieldMovieLockFrame = 0;
        PC_INC(1);
        return 0;
    }
    switch (g_pFieldState->eventCmd) {
    case EVTCMD_PLAY_MOVIE:
        switch (g_pFieldState->movieCommandState) {
        case MOVCMD_ACTIVE:
            break;
        case MOVCMD_DONE:
            g_pFieldState->eventCmd = EVTCMD_NONE;
            g_pFieldState->movieCommandState = MOVCMD_IDLE;
            PC_INC(1);
            return 0;
        }
        return 1;
    case EVTCMD_UNK14:
        PC_INC(1);
        return 0;
    case EVTCMD_NONE:
        g_pFieldState->eventCmd = EVTCMD_PLAY_MOVIE;
        g_pFieldState->movieCommandState = MOVCMD_IDLE;
        break;
    }
    return 1;
}

s32 OpcodeFuncMvief(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mvief", 2);
    }
    if (g_FieldMovieLock != 0) {
        FieldEventWriteMemoryS16(2, 2, g_FieldMovieLockFrame);
        g_FieldMovieLockFrame++;
        PC_INC(3);
        return 0;
    } else {
        FieldEventWriteMemoryS16(2, 2, g_pFieldState->currentMovieFrame);
        PC_INC(3);
        return 0;
    }
}

s32 OpcodeFuncMpjpo(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mpjpo", 0);
    }
    g_pFieldState->mapJumpDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_scroll.c
////////////////////////////////////////////////

s32 OpcodeFuncScr2d(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scr2d", 5);
    }
    g_pFieldState->cameraScrollMode = SCRL_TO_COORDS_INSTANT;
    g_pFieldState->cameraScrollTargetX = FieldEventReadMemoryS16(1, 2);
    g_pFieldState->cameraScrollTargetY = FieldEventReadMemoryS16(2, 4);
    g_pFieldState->cameraScrollState = SCRLST_INIT;
    PC_INC(6);
    return 0;
}

s32 OpcodeFuncScrlc(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scrlc", 0);
    }
    g_pFieldState->cameraScrollMode = GET_PARAM_U8(4);
    g_pFieldState->cameraScrollTargetId = g_pFieldState->pcModelId;
    g_pFieldState->cameraScrollNumSteps = FieldEventReadMemoryS16(2, 2);
    g_pFieldState->cameraScrollState = SCRLST_INIT;
    PC_INC(5);
    return 0;
}

/* Scroll the camera to an entity over a number of frames. Unlike SCR2D the
 * target is an entity id, so a missing model makes the opcode a no-op. */
s32 OpcodeFuncScrla(void) {
    u8 entityId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scrla", 0);
    }
    entityId = GET_PARAM_U8(4);
    if (g_EntityToModel[entityId] != 0xFF) {
        g_pFieldState->cameraScrollMode = GET_PARAM_U8(5);
        g_pFieldState->cameraScrollTargetId = g_EntityToModel[entityId];
        g_pFieldState->cameraScrollNumSteps = FieldEventReadMemoryS16(2, 2);
        g_pFieldState->cameraScrollState = 0;
    }
    PC_INC(6);
    return 0;
}

/* SCRLP is SCRLA addressed by party slot rather than by entity: the slot picks
 * a character, the character picks the field entity that represents them.
 *
 * The copy back into partyId is load-bearing, not redundant. Indexing
 * g_EntityToModel with actorId directly widens it in place as
 * `andi a1,v0,0xff`, where the original holds the resolved actor in v0 and
 * copies it out with a plain `move`. Going through the (by now dead) slot
 * variable is what produces that copy. Found by decomp-permuter. */
s32 OpcodeFuncScrlp(void) {
    u8 partyId;
    u8 actorId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scrlp", 0);
    }
    partyId = D_8009D391[GET_PARAM_U8(4)];
    if (partyId == 0xFF) {
        actorId = 0xFF;
    } else {
        actorId = g_CharIdToEntity[partyId];
    }
    partyId = actorId;
    if (g_EntityToModel[partyId] != 0xFF) {
        g_pFieldState->cameraScrollMode = GET_PARAM_U8(5);
        g_pFieldState->cameraScrollTargetId = g_EntityToModel[partyId];
        g_pFieldState->cameraScrollNumSteps = FieldEventReadMemoryS16(2, 2);
        g_pFieldState->cameraScrollState = 0;
    }
    PC_INC(6);
    return 0;
}

s32 OpcodeFuncScrcc(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scrcc", 0);
    }
    g_pFieldState->cameraScrollMode = SCRL_OFF;
    g_pFieldState->cameraScrollTargetId = g_pFieldState->pcModelId;
    g_pFieldState->cameraScrollState = SCRLST_INIT;
    PC_INC(1);
    return 0;
}

s32 OpcodeFuncScr2dc(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scr2dc", 8);
    }
    g_pFieldState->cameraScrollMode = SCRL_TO_COORDS_SMOOTH;
    g_pFieldState->cameraScrollTargetX = FieldEventReadMemoryS16(1, 3);
    g_pFieldState->cameraScrollTargetY = FieldEventReadMemoryS16(2, 5);
    g_pFieldState->cameraScrollNumSteps = FieldEventReadMemoryS16(4, 7);
    g_pFieldState->cameraScrollState = SCRLST_INIT;
    PC_INC(9);
    return 0;
}

s32 OpcodeFuncScr2dl(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scr2dl", 8);
    }
    g_pFieldState->cameraScrollMode = SCRL_TO_COORDS_LINEAR;
    g_pFieldState->cameraScrollTargetX = FieldEventReadMemoryS16(1, 3);
    g_pFieldState->cameraScrollTargetY = FieldEventReadMemoryS16(2, 5);
    g_pFieldState->cameraScrollNumSteps = FieldEventReadMemoryS16(4, 7);
    g_pFieldState->cameraScrollState = SCRLST_INIT;
    PC_INC(9);
    return 0;
}

s32 OpcodeFuncScrlw(void) {
    s32 mode;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("scrlw", 0);
    }
    if (g_pFieldState->cameraScrollState == SCRLST_DONE) {
        mode = g_pFieldState->cameraScrollMode;
        if (mode != SCRL_OFF) {
            if (mode < SCRL_TO_COORDS_INSTANT) {
                g_pFieldState->cameraScrollMode = SCRL_TO_ENTITY_INSTANT;
            } else if (mode < 7) {
                if (mode >= SCRL_TO_COORDS_LINEAR) {
                    g_pFieldState->cameraScrollMode = SCRL_TO_COORDS_INSTANT;
                }
            }
        }
        g_pFieldState->cameraScrollState = SCRLST_INIT;
        PC_INC(1);
        return 0;
    }
    return 1;
}

/////////////////////////////////////////////////
// Begin of field_opcode_palette.c
////////////////////////////////////////////////

s32 OpcodeFuncStpal(void) {
    RECT rect;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("stpal", 4);
    }
    FieldEventRectClear((s16*)&rect);
    rect.y = FieldEventReadMemoryU8(1, 2) + 0x1E0;
    rect.x = 0;
    rect.w = GET_PARAM_U8(4) + 1;
    rect.h = 1;
    StoreImage(
        &rect, (u_long*)&g_FieldPalettes[FieldEventReadMemoryU8(2, 3) * 32]);
    PC_INC(5);
    return 0;
}

/* STPAL with a start entry: the run of colours saved out of VRAM begins `x`
 * entries into the palette rather than at entry 0. */
s32 OpcodeFuncStpls(void) {
    RECT rect;
    s16 x;
    u8* p;
    s32 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("stpls", 4);
    }
    FieldEventRectClear((s16*)&rect);
    rect.y = GET_PARAM_U8(1) + 0x1E0;
    x = GET_PARAM_U8(3);
    rect.x = x;
    rect.w = GET_PARAM_U8(4) + 1;
    rect.h = 1;
    param = GET_PARAM_U8(2);
    p = &g_FieldPalettes[x * 2];
    StoreImage(&rect, (u_long*)(param * 32 + (s32)p));
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncLdpal(void) {
    RECT rect;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("ldpal", 4);
    }
    FieldEventRectClear((s16*)&rect);
    rect.y = FieldEventReadMemoryU8(2, 3) + 0x1E0;
    rect.x = 0;
    rect.w = GET_PARAM_U8(4) + 1;
    rect.h = 1;
    LoadImage(
        &rect, (u_long*)&g_FieldPalettes[FieldEventReadMemoryU8(1, 2) * 32]);
    PC_INC(5);
    return 0;
}

/* LDPAL with a start entry; same address-grouping residue as OpcodeFuncStpls
 * above, and the same phrasings have been tried against it. */
s32 OpcodeFuncLdpls(void) {
    RECT rect;
    s16 x;
    u8* p;
    s32 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("ldpls", 4);
    }
    FieldEventRectClear((s16*)&rect);
    rect.y = GET_PARAM_U8(2) + 0x1E0;
    x = GET_PARAM_U8(3);
    rect.x = x;
    rect.w = GET_PARAM_U8(4) + 1;
    rect.h = 1;
    param = GET_PARAM_U8(1);
    p = &g_FieldPalettes[x * 2];
    LoadImage(&rect, (u_long*)(param * 32 + (s32)p));
    PC_INC(5);
    return 0;
}

static void FieldEventRectClear(s16* arg0) {
    arg0[0] = 0;
    arg0[1] = 0;
    arg0[2] = 0;
    arg0[3] = 0;
}

/* Copy the first `count` entries of one 16-colour palette over another. The
 * palette store is a flat byte array of 32-byte pages, so both ends have to be
 * re-cast to u16 to walk entries rather than bytes. Declaring the two pointers
 * inside the loop is what makes gcc hoist each as one invariant; written above
 * the loop they land ahead of the zero-trip guard, and written inline gcc
 * reassociates the base out and the body needs a third `addu`.
 *
 * Same base-address recipe as ADPAL below: widen the palette id into an `s32`,
 * then take `u8* base = g_FieldPalettes;` as its own statement, then compute
 * the two pointers off `base`. The three invariant statements are hoisted in
 * source order, which is the order the target's preheader has them. */
s32 OpcodeFuncCppal(void) {
    s16 count;
    u8 src;
    u8 dst;
    s16 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("cppal", 4);
    }
    count = GET_PARAM_U8(4) + 1;
    src = FieldEventReadMemoryU8(1, 2);
    dst = FieldEventReadMemoryU8(2, 3);
    for (i = 0; i < count; i++) {
        s32 dp = dst;
        u8* base = g_FieldPalettes;
        u16* dstPal = (u16*)(base + dp * 32);
        u16* srcPal = (u16*)(base + src * 32);

        dstPal[i] = srcPal[i];
    }
    PC_INC(5);
    return 0;
}

/* As CPPAL, but source and destination each get their own start entry, so the
 * copy can shift a run of colours within or between palettes.
 *
 * Same recipe as CPPAL above; the store base is the one that has to be widened
 * and computed first, since that is the order this function's target builds
 * them in. The two are a .rodata unit -- CPPAL owns the "cppal" string CPPAL2
 * prints -- so they had to land in the same change. */
s32 OpcodeFuncCppal2(void) {
    s16 count;
    s16 srcPal;
    s16 dstPal;
    s16 src;
    s16 dst;
    s16 end;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("cppal", 7);
    }
    count = FieldEventReadMemoryU8(4, 7) + 1;
    srcPal = GET_PARAM_U8(3);
    dstPal = GET_PARAM_U8(4);
    src = FieldEventReadMemoryU8(1, 5);
    dst = FieldEventReadMemoryU8(2, 6);
    end = src + count;
    while (src < end) {
        s32 dp = dstPal;
        u8* base = g_FieldPalettes;
        u16* to = (u16*)(base + dp * 32);
        u16* from = (u16*)(base + srcPal * 32);

        to[dst] = from[src];
        src++;
        dst++;
    }
    PC_INC(8);
    return 0;
}

/* Rotate a palette: the run of colours ending at `count` is written back
 * starting `start` entries along, and the tail that falls off the end wraps
 * around to entry 0. Two passes, both walking the same pair of indices -- `i`
 * the source entry, `j` the destination one. */
s32 OpcodeFuncRtpal(void) {
    s16 count;
    u8 src;
    u8 dst;
    s16 start;
    s16 i;
    s16 j;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("rtpal", 6);
    }
    count = GET_PARAM_U8(6) + 1;
    src = FieldEventReadMemoryU8(1, 3);
    dst = FieldEventReadMemoryU8(2, 4);
    start = FieldEventReadMemoryU8(4, 5);
    i = 0;
    for (j = start; j <= count; j++) {
        s32 dp = dst;
        u8* base = g_FieldPalettes;
        u16* to = (u16*)(base + dp * 32);
        u16* from = (u16*)(base + src * 32);

        to[j] = from[i];
        i++;
    }
    j = 0;
    for (i = count - start; i <= count; i++, j++) {
        s32 dp = dst;
        u8* base = g_FieldPalettes;
        u16* to = (u16*)(base + dp * 32);
        u16* from = (u16*)(base + src * 32);

        to[j] = from[i];
    }
    PC_INC(7);
    return 0;
}

/* As RTPAL, but source and destination each get their own start entry, so the
 * rotation can move a run between two palettes as well as within one.
 *
 * Identical in shape to OpcodeFuncRtpal above and wants exactly the same two
 * fixes; read that note. */
s32 OpcodeFuncRtpal2(void) {
    s16 end;
    u8 src;
    u8 dst;
    s16 srcStart;
    s16 dstStart;
    s16 i;
    s16 j;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("rtpal", 7);
    }
    end = FieldEventReadMemoryU8(4, 7) + 1;
    src = GET_PARAM_U8(3);
    dst = GET_PARAM_U8(4);
    srcStart = FieldEventReadMemoryU8(1, 5);
    dstStart = FieldEventReadMemoryU8(2, 6);
    end += srcStart;
    i = srcStart;
    for (j = dstStart; j <= end; j++) {
        s32 dp = dst;
        u8* base = g_FieldPalettes;
        u16* to = (u16*)(base + dp * 32);
        u16* from = (u16*)(base + src * 32);

        to[j] = from[i];
        i++;
    }
    j = srcStart;
    for (i = end - dstStart; i <= end; i++, j++) {
        s32 dp = dst;
        u8* base = g_FieldPalettes;
        u16* to = (u16*)(base + dp * 32);
        u16* from = (u16*)(base + src * 32);

        to[j] = from[i];
    }
    PC_INC(8);
    return 0;
}

/* Add a signed per-channel delta to every colour of a palette. The three
 * deltas arrive as bytes, so a set sign bit is widened by hand -- `x ^= 0xFF00`
 * on a value already known to have bit 7 set is the original's sign extension.
 * Each channel is clamped to 0..0x1F on its own, and a colour that lands on
 * zero but did not start there is forced to 0x8000, since an all-zero entry is
 * the PS1's transparent pixel rather than black. */
s32 OpcodeFuncAdpal(void) {
    s16 count;
    u8 srcPal;
    u8 dstPal;
    s16 addB;
    s16 addG;
    s16 addR;
    s16 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("adpal", 8);
    }
    count = GET_PARAM_U8(9) + 1;
    srcPal = FieldEventReadMemoryU8(1, 4);
    dstPal = FieldEventReadMemoryU8(2, 5);
    addB = FieldEventReadMemoryU8(3, 6);
    addG = FieldEventReadMemoryU8(4, 7);
    addR = FieldEventReadMemoryU8(5, 8);
    if (addB & 0x80) {
        addB ^= 0xFF00;
    }
    if (addG & 0x80) {
        addG ^= 0xFF00;
    }
    if (addR & 0x80) {
        addR ^= 0xFF00;
    }
    for (i = 0; i < count; i++) {
        s32 sp = srcPal;
        u8* base = g_FieldPalettes;
        u16* from = (u16*)(base + sp * 32);
        u16* to = (u16*)(base + dstPal * 32);
        u16 color = from[i];
        s16 r;
        s16 g;
        s16 b;

        r = (color & 0x1F) + addR;
        if (r >= 0x20) {
            r = 0x1F;
        }
        if (r < 0) {
            r = 0;
        }
        g = ((color >> 5) & 0x1F) + addG;
        if (g >= 0x20) {
            g = 0x1F;
        }
        if (g < 0) {
            g = 0;
        }
        b = ((color >> 10) & 0x1F) + addB;
        if (b >= 0x20) {
            b = 0x1F;
        }
        if (b < 0) {
            b = 0;
        }
        to[i] = (b << 10) | (g << 5) | r | (color & 0x8000);
        if (to[i] == 0 && color != 0) {
            to[i] = 0x8000;
        }
    }
    PC_INC(0xA);
    return 0;
}

/* ADPAL over a sub-range: the run starts `start` entries in and the two
 * palettes come from the script rather than from event memory. */
s32 OpcodeFuncAdpal2(void) {
    s16 count;
    u8 srcPal;
    u8 dstPal;
    s16 start;
    s16 addB;
    s16 addG;
    s16 addR;
    s16 i;
    s16 end;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("adpal", 8);
    }
    count = FieldEventReadMemoryU8(6, 0xA) + 1;
    srcPal = GET_PARAM_U8(4);
    dstPal = GET_PARAM_U8(5);
    start = FieldEventReadMemoryU8(1, 6);
    addB = FieldEventReadMemoryU8(2, 7);
    addG = FieldEventReadMemoryU8(3, 8);
    addR = FieldEventReadMemoryU8(4, 9);
    if (addB & 0x80) {
        addB ^= 0xFF00;
    }
    if (addG & 0x80) {
        addG ^= 0xFF00;
    }
    if (addR & 0x80) {
        addR ^= 0xFF00;
    }
    end = start + count;
    for (i = start; i < end; i++) {
        u8* pal = g_FieldPalettes;
        u16* from = (u16*)(pal + srcPal * 32);
        u16* to = (u16*)(pal + dstPal * 32);
        u16 color = from[i];
        s16 r;
        s16 g;
        s16 b;

        r = (color & 0x1F) + addR;
        if (r >= 0x20) {
            r = 0x1F;
        }
        if (r < 0) {
            r = 0;
        }
        g = ((color >> 5) & 0x1F) + addG;
        if (g >= 0x20) {
            g = 0x1F;
        }
        if (g < 0) {
            g = 0;
        }
        b = ((color >> 10) & 0x1F) + addB;
        if (b >= 0x20) {
            b = 0x1F;
        }
        if (b < 0) {
            b = 0;
        }
        to[i] = (b << 10) | (g << 5) | r | (color & 0x8000);
        if (to[i] == 0 && color != 0) {
            to[i] = 0x8000;
        }
    }
    PC_INC(0xB);
    return 0;
}

/* Scale every colour of a palette per channel. The factor is a 1.7 fixed-point
 * byte, so the channel is doubled before the multiply and the product shifted
 * back down by 7. A transparent entry stays transparent -- the whole body is
 * skipped -- and one that scales down to zero is forced to 0x8000. */
s32 OpcodeFuncMppal2(void) {
    s16 count;
    u8 srcPal;
    u8 dstPal;
    u32 mulB;
    u16 mulG;
    u16 mulR;
    s16 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mppal", 8);
    }
    count = GET_PARAM_U8(9) + 1;
    srcPal = FieldEventReadMemoryU8(1, 4);
    dstPal = FieldEventReadMemoryU8(2, 5);
    mulB = FieldEventReadMemoryU8(3, 6);
    mulG = FieldEventReadMemoryU8(4, 7);
    mulR = FieldEventReadMemoryU8(5, 8);
    for (i = 0; i < count; i++) {
        s32 sp = srcPal;
        u8* base = g_FieldPalettes;
        u16* from = (u16*)(base + sp * 32);
        u16 color = from[i];

        if (color != 0) {
            u32 r = (mulR * ((u16)(color << 1) & 0x3EU)) >> 7;
            u32 g = (mulG * ((color >> 4) & 0x3FU)) >> 7;
            u32 b = (mulB * ((color >> 9) & 0x3FU)) >> 7;
            u16* to;

            if (b >= 0x20) {
                b = 0x1F;
            }
            if (g >= 0x20) {
                g = 0x1F;
            }
            if (r >= 0x20) {
                r = 0x1F;
            }
            to = (u16*)(base + dstPal * 32);
            to[i] = r | ((b << 10) | (g << 5)) | (color & 0x8000);
            if (to[i] == 0) {
                to[i] = 0x8000;
            }
        }
    }
    PC_INC(0xA);
    return 0;
}

/* MPPAL over a sub-range; the two palettes come from the script. */
s32 OpcodeFuncMppal(void) {
    s16 count;
    u8 srcPal;
    u8 dstPal;
    s16 start;
    u32 mulB;
    u16 mulG;
    u16 mulR;
    s16 i;
    s16 end;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mppal", 8);
    }
    count = FieldEventReadMemoryU8(6, 0xA) + 1;
    srcPal = GET_PARAM_U8(4);
    dstPal = GET_PARAM_U8(5);
    start = FieldEventReadMemoryU8(1, 6);
    mulB = FieldEventReadMemoryU8(2, 7);
    mulG = FieldEventReadMemoryU8(3, 8);
    mulR = FieldEventReadMemoryU8(4, 9);
    end = start + count;
    for (i = start; i < end; i++) {
        s32 sp = srcPal;
        u8* base = g_FieldPalettes;
        u16* from = (u16*)(base + sp * 32);
        u16 color = from[i];

        if (color != 0) {
            u32 r = (mulR * ((u16)(color << 1) & 0x3EU)) >> 7;
            u32 g = (mulG * ((color >> 4) & 0x3FU)) >> 7;
            u32 b = (mulB * ((color >> 9) & 0x3FU)) >> 7;
            u16* to;

            if (b >= 0x20) {
                b = 0x1F;
            }
            if (g >= 0x20) {
                g = 0x1F;
            }
            if (r >= 0x20) {
                r = 0x1F;
            }
            to = (u16*)(base + dstPal * 32);
            to[i] = r | ((b << 10) | (g << 5)) | (color & 0x8000);
            if (to[i] == 0) {
                to[i] = 0x8000;
            }
        }
    }
    PC_INC(0xB);
    return 0;
}

static void SetPcModel(void) {
    if (Savemap.memory_bank_2[9] != 0xFF &&
        g_CharIdToEntity[Savemap.memory_bank_2[9]] != 0xFF &&
        g_EntityToModel[g_CharIdToEntity[Savemap.memory_bank_2[9]]] != 0xFF) {
        g_pFieldState->pcModelId =
            g_EntityToModel[g_CharIdToEntity[Savemap.memory_bank_2[9]]];
    }
}

s32 OpcodeFuncPc(void) {
    u8 charId;
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("pc", 1);
    }

    charId = GET_PARAM_U8(1);
    g_CharIdToEntity[charId] = g_CurrentEntity;

    for (i = 0; i < 3; i++) {
        if (Savemap.memory_bank_2[9 + i] == charId) {
            if (i != 0) {
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].visible = 0;
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].SolidOff = 1;
                g_FieldModels[g_EntityToModel[g_CurrentEntity]].TalkOff = 1;
            } else {
                g_pFieldState->pcModelId = g_EntityToModel[g_CurrentEntity];
            }

            PC_INC(2);
            return 0;
        }
    }

    g_CharIdToEntity[charId] = g_CurrentEntity;

    g_FieldModels[g_EntityToModel[g_CurrentEntity]].visible = 0;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].SolidOff = 1;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].TalkOff = 1;

    PC_INC(2);
    return 0;
}

s32 OpcodeFuncPrtyp(void) {
    s32 i;
    u8 charId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("prtyp", 1);
    }

    charId = GET_PARAM_U8(1);
    for (i = 0; i < 3; i++) {
        if (Savemap.memory_bank_2[9 + i] == charId) {
            PC_INC(2);
            SetPcModel();
            PartyFromBank2ToSave(0);
            return 0;
        }
    }

    for (i = 0; i < 3; i++) {
        if (Savemap.memory_bank_2[9 + i] == 0xFF) {
            ADD_PARTY_MEMBER(i, charId);

            if (g_DebugLevel & 3) {
                FieldDebugAddParseValueToPage2(
                    "p+ ef=", g_CharIdToEntity[charId], 2);
            }
            PC_INC(2);
            SetPcModel();
            PartyFromBank2ToSave(1);
            return 0;
        }
    }

    ADD_PARTY_MEMBER(2, charId);
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("p+ lf=", g_CharIdToEntity[charId], 2);
    }
    PC_INC(2);
    SetPcModel();
    PartyFromBank2ToSave(1);
    return 0;
}

s32 OpcodeFuncPrtym(void) {
    s32 i;
    u8 charId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("prtym", 1);
    }

    charId = GET_PARAM_U8(1);

    for (i = 0; i < 3; i++) {
        if (Savemap.memory_bank_2[9 + i] == charId) {
            Savemap.memory_bank_2[9 + i] = 0xFF;
            PartyFromBank2ToSave(1);
            SetPcModel();
            PC_INC(2);
            return 0;
        }
    }

    PartyFromBank2ToSave(1);
    SetPcModel();
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncPrtye(void) {
    u8 newParty[3];
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("prtye", 3);
    }

    for (i = 0; i < 3; i++) {
        newParty[i] = (&GET_PARAM_U8(1))[i];
    }

    PartyReplace(newParty);
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncSptye(void) {
    u8 newParty[3];
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("sptye", 5);
    }

    for (i = 0; i < 3; i++) {
        newParty[i] = FieldEventReadMemoryU8(1 + i, 3 + i);
    }

    PartyReplace(newParty);
    PC_INC(6);
    return 0;
}

s32 OpcodeFuncGptye(void) {
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("gptye", 5);
    }

    for (i = 0; i < 3; i++) {
        FieldEventWriteMemoryU8(1 + i, 3 + i, Savemap.memory_bank_2[9 + i]);
    }
    PC_INC(6);
    return 0;
}

// Partial replace of bank 2 party with newParty.
// Any free slots in newParty are taken up by members of bank 2 party.
// The result is then transferred to main party in save.
static void PartyReplace(u8* newParty) {
    s32 i, j;

    // Remove requested members from old party.
    for (i = 0; i < 3; i++) {
        if (newParty[i] != 0xFF) {
            for (j = 0; j < 3; j++) {
                if (newParty[i] == Savemap.memory_bank_2[9 + j]) {
                    Savemap.memory_bank_2[9 + j] = 0xFF;
                }
            }
        }
    }

    // Add remaining members of old party to empty slots in new party.
    for (i = 0; i < 3; i++) {
        if (Savemap.memory_bank_2[9 + i] != 0xFF) {
            for (j = 0; j < 3; j++) {
                if (newParty[j] == 0xFF) {
                    newParty[j] = Savemap.memory_bank_2[9 + i];
                    j = 3;
                }
            }
        }
    }

    // Overwrite old party with new party.
    for (i = 0; i < 3; i++) {
        // Convert forced empty slots to regular empty slots.
        if (newParty[i] == 0xFE) {
            newParty[i] = 0xFF;
        }

        ADD_PARTY_MEMBER(i, newParty[i]);
    }

    PartyFromBank2ToSave(1);
    SetPcModel();
}

// Compares two sets of parties and returns which members don't exist in both.
static void PartyCompare(
    u8* party1, u8* party2, u8* party2Only, u8* party1Only) {
    s32 i, j, k;

    for (i = 0; i < 3; i++) {
        party2Only[i] = 0xFF;
        party1Only[i] = 0xFF;
    }

    k = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (party2[i] == party1[j]) {
                goto foundInParty1;
            }
        }
        party2Only[k++] = party2[i];
    foundInParty1:;
    }

    k = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (party1[i] == party2[j]) {
                goto foundInParty2;
            }
        }
        party1Only[k++] = party1[i];
    foundInParty2:;
    }
}

// Transfers party from bank 2 to save while preserving order in save of
// characters existing in both parties.
static void PartyFromBank2ToSave(s32 unused) {
    u8 notInSave[3];
    u8 notInBank2[3];

    PartyCompare(
        Savemap.partyID, &Savemap.memory_bank_2[9], notInSave, notInBank2);
    PartyRemove(Savemap.partyID, notInBank2);
    PartyAdd(Savemap.partyID, notInSave);
    g_PartyUpdatedByFieldScript = 1;
}

// Transfers party from save to bank 2 while preserving order in bank 2 of
// characters existing in both parties.
static void PartyFromSaveToBank2(void) {
    u8 notInBank2[3];
    u8 notInSave[3];

    PartyCompare(
        &Savemap.memory_bank_2[9], Savemap.partyID, notInBank2, notInSave);
    PartyRemove(&Savemap.memory_bank_2[9], notInSave);
    PartyAdd(&Savemap.memory_bank_2[9], notInBank2);
}

static void PartyRemove(u8* party, u8* toRemove) {
    s32 i, j;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (toRemove[i] == party[j]) {
                party[j] = 0xFF;
            }
        }
    }
}

// Adds characters from toAdd to the first free slots in party.
// Does not use force freed slots.
static void PartyAdd(u8* party, u8* toAdd) {
    s32 i, j;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (party[j] == 0xFF) {
                party[j] = toAdd[i];
                break;
            }
        }
    }
}

s32 OpcodeFuncPrtyq(void) {
    s32 i;
    u8 charId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("prtyq", 2);
    }

    charId = GET_PARAM_U8(1);

    for (i = 0; i < 3; i++) {
        if (Savemap.memory_bank_2[9 + i] == charId) {
            if (g_DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("prty=TRUE", 0, 0);
            }
            PC_INC(3);
            return 0;
        }
    }

    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("prty=FALSE", 0, 0);
    }
    PC_INC(GET_PARAM_U8(2) + 2);
    return 0;
}

s32 OpcodeFuncMembq(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("membq", 2);
    }

    if ((1 << GET_PARAM_U8(1)) & Savemap.phs_visibility_mask) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("memb=TRUE", 0, 0);
        }
        PC_INC(3);
        return 0;
    }

    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("memb=FALSE", 0, 0);
    }
    PC_INC(GET_PARAM_U8(2) + 2);
    return 0;
}

s32 OpcodeFuncMmbPlusMinus(void) {
    s16 i;
    s16 charId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mmb+-", 3);
    }

    charId = GET_PARAM_U8(2);

    if (GET_PARAM_U8(1)) {
        Savemap.phs_visibility_mask |= 1 << charId;
    } else {
        Savemap.phs_visibility_mask &= ~(1 << charId);
        for (i = 0; i < 3; i++) {
            if (Savemap.memory_bank_2[9 + i] == charId) {
                Savemap.memory_bank_2[9 + i] = 0xFF;
            }
        }
    }

    PC_INC(3);
    return 0;
}

s32 OpcodeFuncMmblk(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mmblk", 3);
    }

    Savemap.phs_locking_mask |= 1 << GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncMmbuk(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mmbuk", 3);
    }

    Savemap.phs_locking_mask &= ~(1 << GET_PARAM_U8(1));
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncSolid(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("solid", 1);
    }

    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].SolidOff =
            GET_PARAM_U8(1);
    }
    PC_INC(2);
    return 0;
}

/* VWOFT: set the view offset, either as a ramp (mode nonzero: start, target,
 * step count and mode, with the current step reset) or immediately (mode 0:
 * everything cleared and the offset written straight). */
s32 OpcodeFuncVwoft(void) {
    s32 pc;
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("vwoft", 6);
    }
    if (GET_PARAM_U8(6)) {
        g_pFieldState->viewOffsetStart = g_pFieldState->viewOffset;
        g_pFieldState->viewOffsetTarget = FieldEventReadMemoryS16(1, 2);
        g_pFieldState->viewOffsetNumSteps = FieldEventReadMemoryS16(2, 4);
        g_pFieldState->viewOffsetMode = GET_PARAM_U8(6);
        g_pFieldState->viewOffsetCurrentStep = 0;
        pc = g_CurrentEntity;
        pc = pc * 2;
        pc = pc + (s32)g_FieldScriptPC;
    } else {
        g_pFieldState->viewOffset = FieldEventReadMemoryS16(1, 2);
        g_pFieldState->viewOffsetNumSteps = 0;
        g_pFieldState->viewOffsetCurrentStep = 0;
        g_pFieldState->viewOffsetMode = 0;
        g_pFieldState->viewOffsetStart = 0;
        g_pFieldState->viewOffsetTarget = 0;
        pc = g_CurrentEntity;
        pc = pc * 2;
        pc = pc + (s32)g_FieldScriptPC;
    }
    *(u16*)pc += 7;
    return 0;
}

/* JOIN (0xC3): walk the two followers back onto the party leader, then relock
 * the party. Returns 1 while either of them is still moving.
 *
 * Two declarations carry the whole codegen. The per-follower flags are `s16'
 * -- the target truncates each one (`sll v0,s0,16') before testing it, which
 * an `s32' does not do -- and both guards are `if (x == 0xFF) ok = 1; else ok
 * = call;'. The obvious `ok = 1; if (x != 0xFF) ok = call;' for the first one
 * lets cse share the 0xFF between the two comparisons; the shared pseudo then
 * has to survive a call, so it lands in $s1 and the frame grows by a save and
 * a restore. Written as two if/elses gcc materialises `li v0,0xff' twice, as
 * the target does. That was 20 rows and the frame. */
s32 OpcodeFuncJoin(void) {
    s16 joinOk;
    s16 splitOk;
    s16 i;
    u8 modelId;
    u8 charId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("join", 1);
    }
    g_EntityForSplitJoin = g_CurrentEntity;
    if (Savemap.memory_bank_2[10] == 0xFF) {
        joinOk = 1;
    } else {
        joinOk = FieldEventJoinSet(
            g_CharIdToEntity[Savemap.memory_bank_2[10]], GET_PARAM_U8(1));
    }
    if (Savemap.memory_bank_2[11] != 0xFF) {
        splitOk = FieldEventJoinSet(
            g_CharIdToEntity[Savemap.memory_bank_2[11]], GET_PARAM_U8(1));
    } else {
        splitOk = 1;
    }
    if (joinOk && splitOk) {
        for (i = 0; i < 3; i++) {
            charId = Savemap.memory_bank_2[9 + i];
            if (charId != 0xFF) {
                g_EntitySplitJoinState[g_CharIdToEntity[charId]] = 0;
                if (i == 0) {
                    modelId = g_CharIdToEntity[charId];
                    if (modelId != 0xFF) {
                        g_FieldModels[g_EntityToModel[modelId]].SolidOff = 0;
                    }
                }
            }
        }
        g_pFieldState->characterLock = g_CharacterLock;
        g_EntityForSplitJoin = 0xFF;
        PC_INC(2);
        return 0;
    }
    g_pFieldState->characterLock = 1;
    if (Savemap.memory_bank_2[9] != 0xFF) {
        modelId = g_CharIdToEntity[Savemap.memory_bank_2[9]];
        if (modelId != 0xFF) {
            g_EntitySplitJoinState[modelId] = 1;
            g_FieldModels[g_EntityToModel[modelId]].scriptedMoveMode = 0;
            g_FieldModels[g_EntityToModel[modelId]].ActionState = 0;
            g_FieldModels[g_EntityToModel[modelId]].SolidOff = 1;
        }
    }
    return 1;
}

/* SPLIT (0xC2): walk the two followers away from the leader to their own
 * destinations, then unlock the party. Returns 1 while either is still moving.
 * Twin of OpcodeFuncJoin, and it wants the same two things -- s16 flags and
 * the `if (x == 0xFF) ok = 1; else ok = call;' shape that keeps gcc from
 * sharing the 0xFF across a call in a callee-saved register. */
s32 OpcodeFuncSplit(void) {
    s16 splitOkA;
    s16 splitOkB;
    s16 i;
    u8 modelId;
    u8 charId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("split", 8);
    }
    g_EntityForSplitJoin = g_CurrentEntity;
    splitOkA = Savemap.memory_bank_2[10];
    if (splitOkA == 0xFF) {
        splitOkA = 1;
    } else {
        splitOkA = FieldEventSplitSet(
            g_CharIdToEntity[splitOkA], FieldEventReadMemoryS16(1, 4),
            FieldEventReadMemoryS16(2, 6), FieldEventReadMemoryU8(3, 8) & 0xFF,
            GET_PARAM_U8(14));
    }
    splitOkB = Savemap.memory_bank_2[11];
    if (splitOkB == 0xFF) {
        splitOkB = 1;
    } else {
        splitOkB = FieldEventSplitSet(
            g_CharIdToEntity[splitOkB], FieldEventReadMemoryS16(4, 9),
            FieldEventReadMemoryS16(5, 11),
            FieldEventReadMemoryU8(6, 13) & 0xFF, GET_PARAM_U8(14));
    }
    if (splitOkA && splitOkB) {
        for (i = 0; i < 3; i++) {
            charId = Savemap.memory_bank_2[9 + i];
            if (charId != 0xFF) {
                g_EntitySplitJoinState[g_CharIdToEntity[charId]] = 0;
                if (i == 0) {
                    modelId = g_CharIdToEntity[charId];
                    if (modelId != 0xFF) {
                        g_FieldModels[g_EntityToModel[modelId]].SolidOff = 0;
                    }
                }
            }
        }
        g_pFieldState->characterLock = g_CharacterLock;
        g_EntityForSplitJoin = 0xFF;
        PC_INC(15);
        return 0;
    }
    g_pFieldState->characterLock = 1;
    if (Savemap.memory_bank_2[9] != 0xFF) {
        modelId = g_CharIdToEntity[Savemap.memory_bank_2[9]];
        if (modelId != 0xFF) {
            g_EntitySplitJoinState[modelId] = 1;
            g_FieldModels[g_EntityToModel[modelId]].scriptedMoveMode = 0;
            g_FieldModels[g_EntityToModel[modelId]].ActionState = 0;
            g_FieldModels[g_EntityToModel[modelId]].SolidOff = 1;
        }
    }
    return 1;
}

/* Drive one party member through a JOIN: state 0 turns them toward the party
 * leader, state 2 waits for that turn and then walks them onto the leader,
 * state 1 waits for the walk and then makes them intangible and invisible,
 * state 3 is done. Returns 1 once this member has finished. Twin of
 * FieldEventSplitSet -- written from the target rather than permuted, and the
 * two share a shape worth knowing: an `if' guard chain, then a four-case
 * switch whose non-terminal arms `break' to one `return 0'. */
s32 FieldEventJoinSet(s16 entityId, s16 steps) {
    VECTOR from;
    VECTOR to;
    s32 sqrDist;
    s16 leaderEntity;

    if (Savemap.memory_bank_2[9] == 0xFF) {
        return 1;
    }
    leaderEntity = g_CharIdToEntity[Savemap.memory_bank_2[9]];
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("join p0=", leaderEntity, 2);
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("join p1=", entityId, 2);
    }
    if (leaderEntity == 0xFF) {
        return 1;
    }
    if (entityId == 0xFF) {
        return 1;
    }
    switch (g_EntitySplitJoinState[entityId]) {
    case 0:
        from.vx = g_FieldModels[g_EntityToModel[entityId]].PosX >> 12;
        from.vy = g_FieldModels[g_EntityToModel[entityId]].PosY >> 12;
        from.vz = g_FieldModels[g_EntityToModel[entityId]].PosZ >> 12;
        to.vx = g_FieldModels[g_EntityToModel[leaderEntity]].PosX >> 12;
        to.vy = g_FieldModels[g_EntityToModel[leaderEntity]].PosY >> 12;
        to.vz = g_FieldModels[g_EntityToModel[leaderEntity]].PosZ >> 12;
        FieldEventSplitJoinSetTurn(
            entityId, g_FieldModels[g_EntityToModel[entityId]].Dir,
            FieldEntityDirByVec(&from, &to, &sqrDist) & 0xFF);
        g_EntitySplitJoinState[entityId] = 2;
        break;
    case 1:
        if (FieldEventSplitJoinEndMove(entityId) == 0) {
            break;
        }
        g_FieldModels[g_EntityToModel[entityId]].SolidOff = 1;
        g_FieldModels[g_EntityToModel[entityId]].TalkOff = 1;
        g_FieldModels[g_EntityToModel[entityId]].visible = 0;
        g_EntitySplitJoinState[entityId] = 3;
        return 1;
    case 2:
        if (FieldEventSplitJoinEndTurn(entityId) == 0) {
            break;
        }
        FieldEventSplitJoinSetMove(
            entityId,
            (g_FieldModels[g_EntityToModel[leaderEntity]].PosX * 16) >> 16,
            (g_FieldModels[g_EntityToModel[leaderEntity]].PosY * 16) >> 16,
            steps, 0);
        g_EntitySplitJoinState[entityId] = 1;
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("end setmove", 0, 0);
        }
        break;
    case 3:
        return 1;
    }
    return 0;
}

/* Drive one party member through a SPLIT: state 0 starts the move, state 1
 * waits for the move then starts the turn, state 2 waits for the turn, state 3
 * is done. Returns 1 once this member has finished, 0 while a step is still in
 * progress. */
s32 FieldEventSplitSet(s16 entityId, s16 x, s16 y, s16 turnDir, s16 steps) {
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("split p1=", entityId, 2);
    }
    if (entityId == 0xFF) {
        return 1;
    }
    switch (g_EntitySplitJoinState[entityId]) {
    case 0:
        FieldEventSplitJoinSetMove(entityId, x, y, steps, 1);
        g_EntitySplitJoinState[entityId] = 1;
        break;
    case 1:
        if (FieldEventSplitJoinEndMove(entityId) == 0) {
            break;
        }
        g_FieldModels[g_EntityToModel[entityId]].SolidOff = 0;
        g_FieldModels[g_EntityToModel[entityId]].TalkOff = 0;
        FieldEventSplitJoinSetTurn(
            entityId, g_FieldModels[g_EntityToModel[entityId]].Dir,
            turnDir & 0xFF);
        g_EntitySplitJoinState[entityId] = 2;
        break;
    case 2:
        if (FieldEventSplitJoinEndTurn(entityId) == 0) {
            break;
        }
        g_EntitySplitJoinState[entityId] = 3;
        return 1;
    case 3:
        return 1;
    }
    return 0;
}

/* Start one party member walking to (x, y) as part of a SPLIT or JOIN. */
void FieldEventSplitJoinSetMove(
    s16 entityId, s16 x, s16 y, s16 steps, u16 snapToLeader) {
    s32 entryIdx;
    VECTOR from;
    VECTOR to;
    s32 sqrDist;
    s16 leaderId;
    u8 modelIdx;
    FieldModelEntry* entry;
    u8* anims;

    if (D_8009D391[0] != 0xFF) {
        leaderId = g_CharIdToEntity[D_8009D391[0]];
        if (leaderId != 0xFF) {
            if (g_DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("set move x=", x, 4);
            }
            if (g_DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("set move y=", y, 4);
            }
            g_FieldModels[g_EntityToModel[entityId]].visible = 1;
            g_FieldModels[g_EntityToModel[entityId]].SolidOff = 1;
            g_FieldModels[g_EntityToModel[entityId]].TalkOff = 1;
            if (snapToLeader != 0) {
                g_FieldModels[g_EntityToModel[entityId]].PosX =
                    g_FieldModels[g_EntityToModel[leaderId]].PosX;
                g_FieldModels[g_EntityToModel[entityId]].PosY =
                    g_FieldModels[g_EntityToModel[leaderId]].PosY;
                g_FieldModels[g_EntityToModel[entityId]].PosZ =
                    g_FieldModels[g_EntityToModel[leaderId]].PosZ;
                g_FieldModels[g_EntityToModel[entityId]].PosI =
                    g_FieldModels[g_EntityToModel[leaderId]].PosI;
            }
            g_FieldModels[g_EntityToModel[entityId]].ActionArg = 0;
            g_FieldModels[g_EntityToModel[entityId]].DirLock = 0;
            g_FieldModels[g_EntityToModel[entityId]].MoveEndX = x << 12;
            g_FieldModels[g_EntityToModel[entityId]].MoveEndY = y << 12;
            modelIdx = g_EntityToModel[entityId];
            g_FieldModelSavedMoveSpeed[modelIdx] =
                g_FieldModels[modelIdx].MoveSpeed;
            from.vx = g_FieldModels[g_EntityToModel[entityId]].PosX >> 12;
            from.vy = g_FieldModels[g_EntityToModel[entityId]].PosY >> 12;
            from.vz = g_FieldModels[g_EntityToModel[entityId]].PosZ >> 12;
            to.vx = x;
            to.vy = y;
            to.vz = g_FieldModels[g_EntityToModel[entityId]].PosZ >> 12;
            FieldEntityDirByVec(&from, &to, &sqrDist);
            g_FieldModels[g_EntityToModel[entityId]].MoveSpeed =
                (sqrDist << 8) / steps;
            if (g_FieldModels[g_EntityToModel[entityId]].MoveSpeed >= 0x601) {
                if (g_FieldModels[g_EntityToModel[entityId]].activeAnimId !=
                    2) {
                    g_FieldModels[g_EntityToModel[entityId]].activeAnimId = 2;
                    g_FieldModels[g_EntityToModel[entityId]].animSpeed = 0x10;
                    g_FieldModels[g_EntityToModel[entityId]].animCurrentFrame =
                        0;
                    modelIdx = g_EntityToModel[entityId];
                    entryIdx = g_FieldModelLoaderData[modelIdx].modelEntryIndex;
                    entry = &g_FieldModelData->modelEntries[entryIdx];
                    anims = entry->modelData + entry->animationOffset;
                    g_FieldModels[modelIdx].animLastFrame =
                        *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId *
                                      16] -
                        1;
                }
            } else {
                if (g_FieldModels[g_EntityToModel[entityId]].activeAnimId !=
                    1) {
                    g_FieldModels[g_EntityToModel[entityId]].activeAnimId = 1;
                    g_FieldModels[g_EntityToModel[entityId]].animSpeed = 0x10;
                    g_FieldModels[g_EntityToModel[entityId]].animCurrentFrame =
                        0;
                    modelIdx = g_EntityToModel[entityId];
                    entryIdx = g_FieldModelLoaderData[modelIdx].modelEntryIndex;
                    entry = &g_FieldModelData->modelEntries[entryIdx];
                    anims = entry->modelData + entry->animationOffset;
                    g_FieldModels[modelIdx].animLastFrame =
                        *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId *
                                      16] -
                        1;
                }
            }
            g_FieldModelAnimMode[g_EntityToModel[entityId]] = 1;
            g_FieldModels[g_EntityToModel[entityId]].scriptedMoveMode = 1;
            g_FieldModels[g_EntityToModel[entityId]].ActionState = 0;
        }
    }
}

/* Poll one party member's walk during a SPLIT or JOIN. ActionState 2 means the
 * move just finished, so release the scripted-move lock and restore the
 * model's default speed. */
s32 FieldEventSplitJoinEndMove(s16 entityId) {
    if (g_FieldModels[g_EntityToModel[entityId]].ActionState != 2) {
        return 0;
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("end move", 0, 0);
    }
    g_FieldModels[g_EntityToModel[entityId]].scriptedMoveMode = 0;
    g_FieldModels[g_EntityToModel[entityId]].ActionState = 0;
    g_FieldModelAnimMode[g_EntityToModel[entityId]] = 0;
    g_FieldModels[g_EntityToModel[entityId]].MoveSpeed =
        g_FieldModelSavedMoveSpeed[g_EntityToModel[entityId]];
    return 1;
}

/* Begin a party member's turn to a facing during a SPLIT or JOIN. Sets the
 * turn target and step budget, then if the raw delta would exceed half a
 * circle wraps the target the short way round. */
void FieldEventSplitJoinSetTurn(s16 entityId, s16 startDir, s16 endDir) {
    FieldEntity* model;
    s16 delta;

    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("set turn=", endDir & 0xFF, 2);
    }
    if (g_EntityToModel[entityId] != 0xFF) {
        g_FieldModels[g_EntityToModel[entityId]].TurnStart = startDir & 0xFF;
        g_FieldModels[g_EntityToModel[entityId]].TurnType = 2;
        g_FieldModels[g_EntityToModel[entityId]].TurnStep = 0;
        g_FieldModels[g_EntityToModel[entityId]].TurnSteps = 0x10;
        g_FieldModels[g_EntityToModel[entityId]].TurnEnd = endDir & 0xFF;
        model = &g_FieldModels[g_EntityToModel[entityId]];
        delta = model->TurnEnd - model->TurnStart;
        if (delta < 0) {
            delta = ~delta + 1;
        }
        if (delta >= 0x81) {
            if ((s16)model->TurnEnd > (s16)model->TurnStart) {
                model->TurnEnd -= 0x100;
            } else {
                model->TurnEnd += 0x100;
            }
        }
    }
}

/* Poll one party member's turn during a SPLIT or JOIN. Returns 1 once the
 * entity has finished turning -- or has no model to turn -- and 0 while it is
 * still in progress. */
s32 FieldEventSplitJoinEndTurn(s16 entityId) {
    if (g_EntityToModel[entityId] == 0xFF) {
        return 1;
    }
    if (g_FieldModels[g_EntityToModel[entityId]].TurnType != 3) {
        return 0;
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("end turn", 0, 0);
    }
    g_FieldModels[g_EntityToModel[entityId]].TurnType = 0;
    g_FieldModels[g_EntityToModel[entityId]].TurnStep = 0;
    g_FieldModels[g_EntityToModel[entityId]].TurnSteps = 0;
    return 1;
}

/////////////////////////////////////////////////
// Begin of field_opcode_fade.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_fade.c
/////////////////////////////////////////////////

/* FADE (0x6B): start a screen fade. Reads the fade type and per-channel target
 * colours, then the speed. The jump table picks the fadeAdjust start value per
 * fade family: the odd types (1/5/7/9) start one above the parameter, the even
 * ones (2/6/8/10) at it, and 0/3/4 do not touch fadeAdjust at all. */
s32 OpcodeFuncFade(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("fade", 8);
    }
    g_pFieldState->fadeType = GET_PARAM_U8(7);
    switch ((s16) * (volatile u16*)&g_pFieldState->fadeType) {
    case FFT_INV4_TO_FIELD_SUB:
    case FFT_STANDARD_TO_FIELD_ADD:
    case FFT_INSTANT_INV1_SUB_HOLD_FIELD:
    case FFT_INSTANT_STANDARD_ADD_HOLD_FIELD:
        g_pFieldState->fadeAdjust = GET_PARAM_U8(8) + 1;
        break;
    case FFT_FIELD_TO_INV4_SUB:
    case FFT_FIELD_TO_STANDARD_ADD:
    case FFT_INSTANT_INV1_SUB_HOLD_COLOR:
    case FFT_INSTANT_STANDARD_ADD_HOLD_COLOR:
        g_pFieldState->fadeAdjust = GET_PARAM_U8(8);
        break;
    case FFT_INSTANT:
    case FFT_SYS_FADE_TO_BLACK_FIELD_CHANGE:
    case FFT_INSTANT_BLACK:
        break;
    }
    *(volatile s16*)&g_pFieldState->fadeSpeed = GET_PARAM_U8(6);
    g_pFieldState->fadeRed = FieldEventReadMemoryU8(1, 3);
    g_pFieldState->fadeGreen = FieldEventReadMemoryU8(2, 4);
    g_pFieldState->fadeBlue = FieldEventReadMemoryU8(4, 5);
    PC_INC(9);
    return 0;
}

/* The two volatile casts are delay-slot barriers, not a claim about the
 * hardware. gcc reorg happily sinks a plain store sitting just ahead of a call
 * into that call's delay slot; the original does not, leaving the first
 * FieldEventReadMemoryU8's slot empty outright and filling the
 * FieldEventReadMemoryS16's with the `ori a1,7` from the argument setup
 * instead. A volatile store is the one thing reorg refuses to move, so casting
 * exactly the two stores that precede a call pins them. The other four
 * assignments are plain: they are not adjacent to a call and schedule the same
 * either way. (A do/while barrier costs six extra instructions here by
 * breaking the g_pFieldState CSE.) */
s32 OpcodeFuncNfade(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("nfade", 8);
    }
    *(volatile u16*)&g_pFieldState->fadeType = GET_PARAM_U8(3);
    g_pFieldState->nFadeRedTarget = FieldEventReadMemoryU8(1, 4);
    g_pFieldState->nFadeGreenTarget = FieldEventReadMemoryU8(2, 5);
    g_pFieldState->nFadeBlueTarget = FieldEventReadMemoryU8(3, 6);
    *(volatile s16*)&g_pFieldState->fadeAdjust = 0;
    g_pFieldState->fadeSpeed = FieldEventReadMemoryS16(4, 7);
    PC_INC(9);
    return 0;
}

/* FADEW (0x6C): block the script until the active screen fade completes.
 * Returns 1 while waiting, 0 (advancing the PC) once done. What counts as
 * "complete" depends on the fade's direction, so the switch on fadeType
 * collapses to three tests: a fade to black is done when fadeAdjust has run
 * down to 0, a fade from black when it has reached 0xFF, and the NFADE forms
 * when it equals fadeSpeed. Types 0 and 4 are not fades and fall straight
 * through to the PC_INC. */
s32 OpcodeFuncFadew(void) {
    s16 type;
    s16 adjust;
    s16 speed;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("fadew", 0);
    }
    type = ((volatile FieldState*)g_pFieldState)->fadeType;
    switch (type) {
    case FFT_INSTANT:
    case FFT_INSTANT_BLACK:
        break;
    case FFT_INV4_TO_FIELD_SUB:
    case FFT_STANDARD_TO_FIELD_ADD:
    case FFT_INSTANT_INV1_SUB_HOLD_FIELD:
    case FFT_INSTANT_STANDARD_ADD_HOLD_FIELD:
        if (((volatile FieldState*)g_pFieldState)->fadeAdjust != 0) {
            return 1;
        }
        break;
    case FFT_FIELD_TO_INV4_SUB:
    case FFT_FIELD_TO_STANDARD_ADD:
    case FFT_INSTANT_INV1_SUB_HOLD_COLOR:
    case FFT_INSTANT_STANDARD_ADD_HOLD_COLOR:
        if (((volatile FieldState*)g_pFieldState)->fadeAdjust >= 0xFF) {
            PC_INC(1);
            return 0;
        }
        return 1;
    case FFT_SYS_FADE_TO_BLACK_FIELD_CHANGE:
    default:
        adjust = ((volatile FieldState*)g_pFieldState)->fadeAdjust;
        speed = ((volatile FieldState*)g_pFieldState)->fadeSpeed;
        if (adjust != speed) {
            return 1;
        }
        break;
    }
    PC_INC(1);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_intersect.c
/////////////////////////////////////////////////

/* IDLCK: set or clear the "player may not cross this walkmesh edge" bit for
 * one triangle. blockedAccesses is a bitfield, eight triangles per byte. */
s32 OpcodeFuncIdlck(void) {
    s16 triId;
    s32 byteIdx;
    s32 bitIdx;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("idlck", 3);
    }
    GET_PARAM_S16(triId, 1);
    byteIdx = triId / 8;
    bitIdx = triId - byteIdx * 8;
    if (GET_PARAM_U8(3)) {
        g_pFieldState->blockedAccesses[byteIdx] |= 1 << bitIdx;
    } else {
        g_pFieldState->blockedAccesses[byteIdx] &= ~(1 << bitIdx);
    }
    PC_INC(4);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_window_color.c
/////////////////////////////////////////////////

s32 OpcodeFuncGwcol(void) {
    s16 pane;
    s32 corner;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("gwcol", 6);
    }
    pane = FieldEventReadMemoryU8(1, 3);
    corner = pane * 3;
    FieldEventWriteMemoryU8(2, 4, g_FieldWindowColors[corner]);
    FieldEventWriteMemoryU8(3, 5, g_FieldWindowColors[corner + 1]);
    FieldEventWriteMemoryU8(4, 6, g_FieldWindowColors[corner + 2]);
    PC_INC(7);
    return 0;
}

s32 OpcodeFuncSwcol(void) {
    s16 pane;
    s32 corner;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("swcol", 6);
    }
    pane = FieldEventReadMemoryU8(1, 3);
    corner = pane * 3;
    g_FieldWindowColors[corner] = FieldEventReadMemoryU8(2, 4);
    g_FieldWindowColors[corner + 1] = FieldEventReadMemoryU8(3, 5);
    g_FieldWindowColors[corner + 2] = FieldEventReadMemoryU8(4, 6);
    PC_INC(7);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_field_effect.c
/////////////////////////////////////////////////

s32 OpcodeFuncLstmp(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("lstmp", 2);
    }
    FieldEventWriteMemoryS16(2, 2, g_pFieldState->prevFieldId);
    PC_INC(3);
    return 0;
}

/* SHAKE: arm the randomized camera shake on either axis. Bit 0 of parameter 3
 * enables the X shake, bit 1 the Y shake; a clear bit disables that axis. */
s32 OpcodeFuncShake(void) {
    s32 axes;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("shake", 7);
    }
    axes = GET_PARAM_U8(3);
    if (axes & 1) {
        g_pFieldState->shakeX.enabled = 1;
        g_pFieldState->shakeX.amplitude = FieldEventReadMemoryU8(1, 4);
        g_pFieldState->shakeX.numStepsPerSegment = FieldEventReadMemoryU8(2, 5);
    } else {
        g_pFieldState->shakeX.enabled = 0;
    }
    if (axes & 2) {
        g_pFieldState->shakeY.enabled = 1;
        g_pFieldState->shakeY.amplitude = FieldEventReadMemoryU8(3, 6);
        g_pFieldState->shakeY.numStepsPerSegment = FieldEventReadMemoryU8(4, 7);
    } else {
        g_pFieldState->shakeY.enabled = 0;
    }
    PC_INC(8);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_items.c
/////////////////////////////////////////////////

s32 OpcodeFuncStitm(void) {
    s32 itemHi;
    u16 itemId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("stitm", 4);
    }
    itemHi = FieldEventReadMemoryU8(2, 4);
    itemId = (itemHi & 0xFF) << 9;
    itemId |= FieldEventReadMemoryS16(1, 2);
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("S item=", itemId, 4);
    }
    func_80025380(itemId);
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncDlitm(void) {
    s32 itemHi;
    u16 itemId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dlitm", 4);
    }
    itemHi = FieldEventReadMemoryU8(2, 4);
    itemId = (itemHi & 0xFF) << 9;
    itemId |= FieldEventReadMemoryS16(1, 2);
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("G item=", itemId, 4);
    }
    func_80025288(itemId);
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncCkitm(void) {
    u16 itemId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("ckitm", 4);
    }
    itemId = func_80025310(FieldEventReadMemoryS16(1, 2));
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("C item=", itemId, 4);
    }
    if (itemId == 0xFFFF) {
        itemId = 0;
    }
    FieldEventWriteMemoryU8(2, 4, itemId >> 9);
    PC_INC(5);
    return 0;
}

/* The "special" opcode: one byte of sub-opcode selects among eleven unrelated
 * jobs, from clearing the item/materia inventories to writing a character's
 * name into the message name buffer. Sub-opcodes run 0xF5..0xFF, which is what
 * the jump table's `(u32)(sub - 0xF5) < 0xB` guard checks. */
s32 OpcodeFuncSpcal(void) {
    u8* name;
    s32 itemId;
    u16 offset;
    u16 len;
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("spcal", 8);
    }
    switch (GET_PARAM_U8(1)) {
    case 0xFF:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("clitm", 8);
        }
        for (i = 0; i < 0x200; i++) {
            itemId = i | 0xC600;
            func_80025288(itemId);
        }
        PC_INC(2);
        return 0;
    case 0xFE:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("rsglb", 8);
        }
        func_80033A90();
        PC_INC(2);
        return 0;
    case 0xFD:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("spcnm", 8);
        }
        SystemMessageSetCharName(GET_PARAM_U8(2), GET_PARAM_U8(3));
        PC_INC(4);
        return 0;
    case 0xFC:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("mvlck", 2);
        }
        g_FieldMovieLock = GET_PARAM_U8(2);
        PC_INC(3);
        return 0;
    case 0xFB:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("btlck", 2);
        }
        g_FieldBattleLock = GET_PARAM_U8(2);
        PC_INC(3);
        return 0;
    case 0xFA:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("flitm", 8);
        }
        for (i = 0; i < 0x200; i++) {
            itemId = i | 0xC600;
            func_80025380(itemId);
        }
        PC_INC(2);
        return 0;
    case 0xF9:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("flmat", 8);
        }
        for (i = 0; i < 0x50; i++) {
            func_8002542C(i);
        }
        PC_INC(2);
        return 0;
    case 0xF8:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("smspd", 3);
        }
        g_FieldMessageSpeed[0] = ~FieldEventReadMemoryU8(4, 3);
        PC_INC(4);
        return 0;
    case 0xF7:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("gmspd", 3);
        }
        FieldEventWriteMemoryU8(4, 3, ~g_FieldMessageSpeed[0]);
        PC_INC(4);
        return 0;
    case 0xF6:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("pname", 8);
        }
        name = GetCharacterName(FieldEventReadMemoryU8(3, 3));
        offset = 0;
        len = GET_PARAM_U8(5);
        switch (GET_PARAM_U8(2) & 0xF) {
        case 15:
            offset += 0x100;
        case 13:
            offset += 0x100;
        case 11:
            offset += 0x100;
        case 3:
            offset += 0x100;
        }
        for (i = 0; i < len; i++) {
            ((u8*)D_8009D288)[offset + i] = *name++;
        }
        ((u8*)D_8009D288)[offset + i] = 0xFF;
        PC_INC(6);
        return 0;
    case 0xF5:
        if (g_DebugLevel & 3) {
            DebugPrintOpcode("arrow", 8);
        }
        g_FieldMovieOpcodeActive = GET_PARAM_U8(2);
        PC_INC(3);
        return 0;
    }
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("?????", 8);
    }
    PC_INC(2);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_layer.c
/////////////////////////////////////////////////

s32 OpcodeFuncBgscr(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bgscr", 8);
    }
    switch (GET_PARAM_U8(2)) {
    case 2:
        g_pFieldState->layer2_bgScrollXSpeed = FieldEventReadMemoryS16(1, 3);
        g_pFieldState->layer2_bgScrollYSpeed = FieldEventReadMemoryS16(2, 5);
        break;
    case 3:
        g_pFieldState->layer3_bgScrollXSpeed = FieldEventReadMemoryS16(1, 3);
        g_pFieldState->layer3_bgScrollYSpeed = FieldEventReadMemoryS16(2, 5);
        break;
    }
    PC_INC(7);
    return 0;
}

s32 OpcodeFuncBgdph(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bgdph", 8);
    }
    switch (GET_PARAM_U8(2)) {
    case 2:
        g_pFieldState->layer2_depth = FieldEventReadMemoryS16(1, 3);
        break;
    case 3:
        g_pFieldState->layer3_depth = FieldEventReadMemoryS16(1, 3);
        break;
    }
    PC_INC(5);
    return 0;
}

/////////////////////////////////////////////////
// Begin of field_opcode_materia.c
/////////////////////////////////////////////////

s32 OpcodeFuncSmtra(void) {
    u32 materia;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("smtra", 6);
    }
    materia = FieldEventReadMemoryU8(1, 3);
    materia |= FieldEventReadMemoryU8(2, 4) << 8;
    materia |= FieldEventReadMemoryU8(3, 5) << 16;
    materia |= FieldEventReadMemoryU8(4, 6) << 24;
    if (func_8002542C(materia) == -1) {
        g_FieldMateriaAddResult = 0;
    } else {
        g_FieldMateriaAddResult = 1;
    }
    PC_INC(7);
    return 0;
}

s32 OpcodeFuncDmtra(void) {
    u32 materia;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dmtra", 7);
    }
    materia = FieldEventReadMemoryU8(1, 3);
    materia |= FieldEventReadMemoryU8(2, 4) << 8;
    materia |= FieldEventReadMemoryU8(3, 5) << 16;
    materia |= FieldEventReadMemoryU8(4, 6) << 24;
    func_80025648(materia, GET_PARAM_U8(7));
    PC_INC(8);
    return 0;
}

s32 OpcodeFuncCmtra(void) {
    u32 materia;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("cmtra", 8);
    }
    materia = FieldEventReadMemoryU8(1, 4);
    materia |= FieldEventReadMemoryU8(2, 5) << 8;
    materia |= FieldEventReadMemoryU8(3, 6) << 16;
    materia |= FieldEventReadMemoryU8(4, 7) << 24;
    FieldEventWriteMemoryU8(6, 9, func_80025650(materia, GET_PARAM_U8(8)));
    PC_INC(10);
    return 0;
}

s32 OpcodeFuncMenu(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("menu", 3);
    }
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("evt cmd=", g_pFieldState->eventCmd, 2);
    }

    if (g_pFieldState->eventCmd == EVTCMD_NONE) {
        g_pFieldState->eventCmd = GET_PARAM_U8(2);
        g_pFieldState->eventCmdParam = FieldEventReadMemoryU8(2, 3);
        g_pFieldState->movieCommandState = MOVCMD_IDLE;
        D_8007EBE0 = 1;
        if (g_pFieldState->eventCmd == EVTCMD_PARTY_MENU &&
            g_pFieldState->eventCmdParam == 0) {
            PC_INC(4);
        }
        return 1;
    }

    if (g_pFieldState->eventCmd == GET_PARAM_U8(2)) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2(
                "evt result=", g_pFieldState->movieCommandState, 1);
        }
        if (g_pFieldState->movieCommandState == MOVCMD_DONE) {
            PC_INC(4);
            g_pFieldState->eventCmd = EVTCMD_NONE;
            g_pFieldState->movieCommandState = MOVCMD_IDLE;
            PartyFromSaveToBank2();
            return 0;
        }
    } else if (GET_PARAM_U8(2) == EVTCMD_UNK14 &&
               g_pFieldState->eventCmd == EVTCMD_PLAY_MOVIE) {
        g_pFieldState->eventCmd = GET_PARAM_U8(2);
        g_pFieldState->movieCommandState = MOVCMD_IDLE;
    }
    return 1;
}

s32 OpcodeFuncMenu2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("menu", 1);
    }
    g_pFieldState->menuDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncGetpc(void) {
    s32 slot;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("getpc", 3);
    }
    slot = GET_PARAM_U8(2);
    if (slot < 3) {
        FieldEventWriteMemoryU8(2, 3, D_8009CBDC[slot]);
    }
    PC_INC(4);
    return 0;
}

/* MPARA: bind one of a window's replaceable text parameters to a memory bank
 * slot, so the window redraws with the current value of that variable. */
s32 OpcodeFuncMpara(void) {
    s32 window;
    s32 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mpara", 4);
    }
    window = GET_PARAM_U8(2);
    param = FieldEventReadMemoryU8(1, 3);
    g_WindowReplaceBank[window][param] = GET_PARAM_U8(1) & 0xF;
    g_WindowReplaceBankAddr[window][param] = GET_PARAM_U8(4);
    PC_INC(5);
    return 0;
}

/* MPRA2: as MPARA, but the bound address is 16-bit. Writing it through
 * GET_PARAM_S16 stores the low byte and then the combined halfword, which is
 * why the same slot is written twice. */
s32 OpcodeFuncMpra2(void) {
    s32 window;
    s32 param;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mpra2", 5);
    }
    window = GET_PARAM_U8(2);
    param = FieldEventReadMemoryU8(1, 3);
    g_WindowReplaceBank[window][param] = GET_PARAM_U8(1) & 0xF;
    GET_PARAM_S16(g_WindowReplaceBankAddr[window][param], 4);
    PC_INC(6);
    return 0;
}

/**
 * @brief Opcode 0xD4 - **SIN** - sine */
s32 OpcodeFuncSin(void) {
    s32 result;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("sin", 8);
    }

    result = rsin(FieldEventReadMemoryS16(1, 3));
    result *= FieldEventReadMemoryS16(2, 5);
    result += FieldEventReadMemoryS16(3, 7);

    FieldEventWriteMemoryS16(4, 9, result >> 12);

    PC_INC(10);
    return 0;
}

/**
 * @brief Opcode 0xD4 - **COS** - cosine */
s32 OpcodeFuncCos(void) {
    s32 result;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("cos", 8);
    }

    result = rcos(FieldEventReadMemoryS16(1, 3));
    result *= FieldEventReadMemoryS16(2, 5);
    result += FieldEventReadMemoryS16(3, 7);

    FieldEventWriteMemoryS16(4, 9, result >> 12);

    PC_INC(10);
    return 0;
}

void SystemRefreshParty(void) {
    s16 i;

    for (i = 0; i < 3; i++) {
        if (Savemap.partyID[i] != 0xFF) {
            func_80020058(i);
            func_8001786C(i);
        }
    }
    func_80017678();
}

void SystemRestoreParty(void) {
    s32 i;

    SystemRefreshParty();
    for (i = 0; i < 3; i++) {
        SystemMenuAddHpByPartyId(i, 10000);
        SystemMenuAddMpByPartyId(i, 10000);
        if (Savemap.partyID[i] != 0xFF &&
            g_BattleCharIdToCharId[Savemap.partyID[i]] <= 8) {
            Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[i]]]
                .status_flags = 0;
        }
    }
}

/* MHMMX: debug helper that walks the three fixed party line-ups (characters
 * 0-2, 3-5, 6-8) through SystemRestoreParty, then puts the real party back. */
s32 OpcodeFuncMhmmx(void) {
    u8 saved[3];
    u8* slot;
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mhmmx", 0);
    }
    for (i = 0; i < 3; i++) {
        saved[i] = D_8009CBDC[i];
    }
    for (i = 2, slot = &D_8009CBDC[2]; i >= 0; i--) {
        *slot-- = i;
    }
    SystemRestoreParty();
    for (i = 2; i >= 0; i--) {
        D_8009CBDC[i] = i + 3;
    }
    SystemRestoreParty();
    for (i = 2; i >= 0; i--) {
        D_8009CBDC[i] = i + 6;
    }
    SystemRestoreParty();
    for (i = 0; i < 3; i++) {
        D_8009CBDC[i] = saved[i];
    }
    SystemRestoreParty();
    PC_INC(1);
    return 0;
}

s32 OpcodeFuncHmpmx(void) {
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("hmpmx", 0);
    }
    SystemRefreshParty();
    for (i = 0; i < 3; i++) {
        SystemMenuAddHpByPartyId(i, 10000);
        SystemMenuAddMpByPartyId(i, 10000);
    }
    PC_INC(1);
    return 0;
}

s32 OpcodeFuncMpPlus(void) {
    s32 partyId;
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mp+", 4);
    }
    SystemRefreshParty();
    partyId = GET_PARAM_U8(2);
    if (D_8009D391[partyId] != 0xFF) {
        partyId = D_8009D391[partyId];
        for (i = 0; i < 3; i++) {
            if (D_8009CBDC[i] == partyId) {
                SystemMenuAddMpByPartyId(i, FieldEventReadMemoryS16(2, 3));
            }
        }
    }
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncMpMinus(void) {
    s32 partyId;
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mp-", 4);
    }
    SystemRefreshParty();
    partyId = GET_PARAM_U8(2);
    if (D_8009D391[partyId] != 0xFF) {
        partyId = D_8009D391[partyId];
        for (i = 0; i < 3; i++) {
            if (D_8009CBDC[i] == partyId) {
                func_80025988(i, FieldEventReadMemoryS16(2, 3));
            }
        }
    }
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncHpPlus(void) {
    s32 partyId;
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("hp+", 4);
    }
    SystemRefreshParty();
    partyId = GET_PARAM_U8(2);
    if (D_8009D391[partyId] != 0xFF) {
        partyId = D_8009D391[partyId];
        for (i = 0; i < 3; i++) {
            if (D_8009CBDC[i] == partyId) {
                SystemMenuAddHpByPartyId(i, FieldEventReadMemoryS16(2, 3));
            }
        }
    }
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncHpMinus(void) {
    s32 partyId;
    s32 i;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("hp-", 4);
    }
    SystemRefreshParty();
    partyId = GET_PARAM_U8(2);
    if (D_8009D391[partyId] != 0xFF) {
        partyId = D_8009D391[partyId];
        for (i = 0; i < 3; i++) {
            if (D_8009CBDC[i] == partyId) {
                func_80025800(i, FieldEventReadMemoryS16(2, 3));
            }
        }
    }
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x39 - **GOLDU** - Gold Up */
s32 OpcodeFuncGoldPlus(void) {
    u32 gold;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("gold+", 5);
    }
    gold = (u16)FieldEventReadMemoryS16(1, 2);
    gold |= (u16)FieldEventReadMemoryS16(2, 4) << 16;
    SystemMenuAddPartyGold(gold);
    PC_INC(6);
    return 0;
}

/**
 * @brief Opcode 0x3A - **GOLDD** - Gold Down */
s32 OpcodeFuncGoldMinus(void) {
    u32 gold;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("gold-", 5);
    }
    gold = (u16)FieldEventReadMemoryS16(1, 2);
    gold |= (u16)FieldEventReadMemoryS16(2, 4) << 16;
    SystemMenuRemovePartyGold(gold);
    PC_INC(6);
    return 0;
}

/**
 * @brief Opcode 0x3B - **CHGLD** - Change Gold */
s32 OpcodeFuncChgld(void) {
    u32 partyGold;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("chgld", 3);
    }
    partyGold = SystemMenuGetPartyGold();
    FieldEventWriteMemoryS16(1, 2, (u16)partyGold);
    FieldEventWriteMemoryS16(2, 3, (u16)(partyGold >> 16));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncChmph(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("chmph", 3);
    }
    FieldEventWriteMemoryS16(1, 2, D_8009A162);
    FieldEventWriteMemoryU8(2, 3, D_8009A15C);
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncChmst(void) {
    u8 state;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("chmst", 2);
    }
    state = g_AkaoMusicActiveMask != 0;
    if (g_AkaoChannelMask[0] != 0) {
        state |= 2;
    }
    FieldEventWriteMemoryU8(2, 2, state);
    PC_INC(3);
    return 0;
}

s32 OpcodeFuncSttim(void) {
    s32 time;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("sttim", 5);
    }

    time = FieldEventReadMemoryU8(1, 3) * 60 * 60;
    time += FieldEventReadMemoryU8(2, 4) * 60;
    time += FieldEventReadMemoryU8(4, 5);
    Savemap.countdown_timer_seconds = time;

    PC_INC(6);
    return 0;
}

s32 OpcodeFuncWspcl(void) {
    u8 window;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wspcl", 5);
    }

    window = GET_PARAM_U8(1);
    g_WindowData[window].numDisplayType = GET_PARAM_U8(2);
    g_WindowData[window].numDisplayX = GET_PARAM_U8(3);
    g_WindowData[window].numDisplayY = GET_PARAM_U8(4);

    PC_INC(5);
    return 0;
}

s32 OpcodeFuncWnumb(void) {
    u8 window;
    s32 value;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wnumb", 7);
    }

    window = GET_PARAM_U8(2);
    value = FieldEventReadMemoryS16(1, 3);
    value |= FieldEventReadMemoryS16(2, 5) << 16;
    g_WindowData[window].numDisplayValue = value;
    g_WindowData[window].numDisplayLength = GET_PARAM_U8(7);

    PC_INC(8);
    return 0;
}

s32 OpcodeFuncBtlmd(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("btlmd", 2);
    }

    g_pFieldState->battleMode2 = GET_PARAM_U8(1);
    g_pFieldState->battleMode1 = GET_PARAM_U8(2);

    PC_INC(3);
    return 0;
}

s32 OpcodeFuncBtmd2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("btmd2", 2);
    }

    g_pFieldState->battleMode2 = GET_PARAM_U8(1);
    g_pFieldState->battleMode2 |= GET_PARAM_U8(2) << 8;
    g_pFieldState->battleMode1 = GET_PARAM_U8(3);
    g_pFieldState->battleMode1 |= GET_PARAM_U8(4) << 8;

    PC_INC(5);
    return 0;
}

s32 OpcodeFuncBtrlt(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("btrlt", 2);
    }

    FieldEventWriteMemoryS16(2, 2, g_BattleMode);

    PC_INC(3);
    return 0;
}

s32 OpcodeFuncBtltb(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("btltb", 1);
    }

    g_pFieldState->encounterTableId = GET_PARAM_U8(1);

    PC_INC(2);
    return 0;
}

s32 OpcodeFuncBlink(void) {
    u8 modelId;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("blink", 8);
    }

    modelId = g_EntityToModel[g_CurrentEntity];
    if (modelId != 0xFF) {
        g_FieldModels[modelId].BlinkOn = GET_PARAM_U8(1);
    }

    PC_INC(2);
    return 0;
}

s32 OpcodeFuncKawai(void) {
    u16 size;
    u8 modelId;
    u8 kawaiType;
    u8 type;
    u8* params;

    if (g_DebugLevel & 3) {
        DebugPrintOpcode("kawai", 8);
    }

    size = GET_PARAM_U8(1);
    modelId = g_EntityToModel[g_CurrentEntity];
    if (modelId != 0xFF) {
        kawaiType = GET_PARAM_U8(2);
        g_FieldModelData
            ->modelEntries[g_FieldModelLoaderData[modelId].modelEntryIndex]
            .kawaiType = kawaiType;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiOp1 = 1;
        type = kawaiType;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiOp0 = 0;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiDataOffset =
            &GET_PARAM_U8(3);
    }

    if (type == 0) {
        params =
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiDataOffset;
        if (params[0] == 1 && params[1] == params[0] && params[2] == 0) {
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].BlinkOn = 0;
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiA = 0;
        } else {
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].BlinkOn = 1;
        }
    }

    PC_INC(size);
    return 0;
}

s32 OpcodeFuncKawiw(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("kawiw", 0);
    }

    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        if (g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiOp1 != 1) {
            g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiOp1 = 0;
            PC_INC(1);
            return 0;
        }
        return 1;
    }

    PC_INC(1);
    return 0;
}
