//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

#define GET_PARAM_U8(offset) (*(u8*)((s32)g_FieldScripts + g_FieldScriptPC[g_CurrentEntity] + (offset)))
#define GET_PARAM_S16(value, offset)                                                                                   \
    value = GET_PARAM_U8(offset);                                                                                      \
    value |= (GET_PARAM_U8((offset) + 1) << 8)

#define PC_INC(x) (g_FieldScriptPC[g_CurrentEntity] += (x))
#define PC_DEC(x) (g_FieldScriptPC[g_CurrentEntity] -= (x))

#define GET_PRIORITY(x) (((x) >> 5) & 0x7)
#define GET_SCRIPTID(x) ((x) & 0x1F)

#define ADD_PARTY_MEMBER(slot, charId)                                                                                 \
    Savemap.memory_bank_2[9 + slot] = charId;                                                                          \
    if (charId != 0xFF) {                                                                                              \
        u16 mask;                                                                                                      \
        u16 bit;                                                                                                       \
        bit = charId;                                                                                                  \
        mask = Savemap.phs_visibility_mask;                                                                            \
        bit = 1 << bit;                                                                                                \
        mask |= bit;                                                                                                   \
        Savemap.phs_visibility_mask = mask;                                                                            \
    }

typedef enum { IF_EQ, IF_NOT_EQ, IF_GT, IF_LT, IF_GTE, IF_LTE, IF_AND, IF_XOR, IF_OR, IF_BIT, IF_NOT_BIT } IfOps;

extern s32 (*g_FieldOpcodes[256])(void);
extern u8 g_EntityForSplitJoin;
extern u8 D_80114498[];
extern u8 g_RandomTableStep;
extern u8 g_RandomTableIndex;
extern u8 g_RandomTable[256];
extern char D_800E0628[];
extern char D_800E0630[];
extern char g_DebugText[];
extern char g_DebugMessageBuffer[];
extern s8 D_800716C8;
extern s16 D_80071E38;
extern s16 D_80071E3C;
extern u32 D_80075E10;
extern SVECTOR (*D_800E4274)[3];
extern u8* D_800E48E0;

void SysCalculateTotalLureGilPreemptiveValue(void);
void SysInitPlayerStatFromMateria(s32);
void SysInitPlayerStatFromEquip(s32);
void SystemMenuAddHpByPartyId(s32 partyId, u16 hp);
void SystemMenuAddMpByPartyId(s32 partyId, u16 mp);

u8 FieldEventRequestRun(s16 entityId, s16 priority, s16 scriptId);
void DebugUpdateActor(s16 arg0, s16 entityId);
void DebugPrintOpcode(const char* name, s32 arg1);
u32 IfCheck(void);
u32 If2CheckSigned(void);
u32 If2CheckUnsigned(void);
s32 FieldEventRequest(s16 type, u8 target, u8 priority, u8 scriptId);
void DebugPrintToFieldWindow(const char* str);
void FieldEventDebugError(const char* errmsg);
void FieldDebugStringCopy(char* dst, const char* src);
void FieldDebugStringConcat(char* dest, char* src);
void FieldDebugStringU8hex(s32 val, char* msg_out);
void FieldDebugStringU16hex(s32 val, char* msg_out);
void FieldDebugStringU32hex(s32 val, char* msg_out);
void AddStrNextDebugRow(s32 val, const char* msg_out);
void SetStrToDebugRow(s32 page, s16 row, const char* str);
void SetDebugStrRowColor(s16 page, s16 row, s16 color);
void FieldDebugPageSetColor(s32 page, s32 r, s32 g, s32 b);

static s32 KeyCheck(u16 keys);
static u32 GetAkaoBlockOffset(s16 akaoId);
static void PartyReplace(u8* newParty);
static void PartyFromBank2ToSave(s32 unused);
static void PartyRemove(u8* party, u8* toRemove);
static void PartyAdd(u8* party, u8* toAdd);

void DebugUpdateActor(s16 arg0, s16 entityId) {
    if (arg0 == 4) {
        if (!(D_80071E24 & 4) || (D_80114498[entityId])) {
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

    FieldDebugStringConcat(g_DebugText, GET_ENTITY_NAME(entityId));
    if (D_8009FE8C | (D_80071E24 & 1)) {
        SetStrToDebugRow(arg0, 0, g_DebugText);
    }
    if (D_80071E24 & 2) {
        DebugPrintToFieldWindow(g_DebugText);
    }
    FieldDebugStringCopy(g_DebugText, "RqLv=");
    FieldDebugStringU8hex(g_FieldScriptPriority[entityId], g_DebugMessageBuffer);
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
        FieldDebugStringU16hex(SavedScriptIds[entityId][g_FieldScriptPriority[entityId]], g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        break;
    }

    if (D_8009FE8C | (D_80071E24 & 1)) {
        SetStrToDebugRow(arg0, 1, g_DebugText);
    }
    if (D_80071E24 & 2) {
        DebugPrintToFieldWindow(g_DebugText);
    }

    if (g_EntityToModel[entityId] == 0xFF) {
        if (g_EntityToLine[entityId] == g_EntityToModel[entityId]) {
            FieldDebugStringCopy(g_DebugText, "Abst");
            if (D_8009FE8C | (D_80071E24 & 1)) {
                SetDebugStrRowColor(arg0, 2, 6);
            }
        } else {
            FieldDebugStringCopy(g_DebugText, "line=");
            FieldDebugStringU16hex(g_EntityToLine[entityId], g_DebugMessageBuffer);
            FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
            if (g_FieldLines[g_EntityToLine[entityId]].isActive) {
                FieldDebugStringConcat(g_DebugText, " on");
            } else {
                FieldDebugStringConcat(g_DebugText, " off");
            }
            if (D_8009FE8C | (D_80071E24 & 1)) {
                SetDebugStrRowColor(arg0, 2, 3);
            }
        }
    } else {
        FieldDebugStringCopy(g_DebugText, "man=");
        FieldDebugStringU16hex(g_EntityToModel[entityId], g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " dir=");
        FieldDebugStringU16hex(g_FieldModels[g_EntityToModel[entityId]].Dir, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetDebugStrRowColor(arg0, 2, 2);
        }
    }
    if (D_8009FE8C | (D_80071E24 & 1)) {
        SetStrToDebugRow(arg0, 2, g_DebugText);
    }
    if (D_80071E24 & 2) {
        DebugPrintToFieldWindow(g_DebugText);
    }

    if (g_EntityToModel[entityId] != 0xFF) {
        FieldDebugStringCopy(g_DebugText, "X=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].PosX >> 12, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Y=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].PosY >> 12, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 3, g_DebugText);
            SetDebugStrRowColor(arg0, 3, 1);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Z=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].PosZ >> 12, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " I=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].PosI, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 4, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringU8hex(
            g_FieldModelAnimStatus[g_EntityToModel[entityId]], g_DebugText);
        FieldDebugStringConcat(g_DebugText, "am");
        FieldDebugStringU16hex(g_FieldModels[g_EntityToModel[entityId]].activeAnimId, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, ".");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].animCurrentFrame, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, ".");
        FieldDebugStringU16hex(g_FieldModels[g_EntityToModel[entityId]].animLastFrame, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 5, g_DebugText);
            SetDebugStrRowColor(arg0, 5, 7);
        }
        if (D_80071E24 & 2) {
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
        FieldDebugStringU16hex(g_FieldModels[g_EntityToModel[entityId]].TalkRange, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, ".SR");
        FieldDebugStringU16hex(g_FieldModels[g_EntityToModel[entityId]].SolidRange, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 6, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "MS");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].MoveSpeed, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " AS");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].animSpeed, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 7, g_DebugText);
            SetDebugStrRowColor(arg0, 7, 7);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
    } else if (g_EntityToLine[entityId] != g_EntityToModel[entityId]) {
        FieldDebugStringCopy(g_DebugText, "AX");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.x1, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " AY");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.y1, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 3, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "AZ");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.z1, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 4, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "BX");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.x2, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " BY");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.y2, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 5, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "BZ");
        FieldDebugStringU32hex(g_FieldLines[g_EntityToLine[entityId]].pos.z2, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 6, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        SetStrToDebugRow(arg0, 7, "");
    } else if (D_8009FE8C | (D_80071E24 & 1)) {
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
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 8, g_DebugText);
            SetDebugStrRowColor(arg0, 8, 3);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "B-R    X=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][0].vx, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 9, g_DebugText);
            SetDebugStrRowColor(arg0, 9, 2);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Y=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][0].vy, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Z=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][0].vz, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 10, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "R-G    X=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][1].vx, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 11, g_DebugText);
            SetDebugStrRowColor(arg0, 11, 4);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Y=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][1].vy, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Z=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][1].vz, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 12, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "G-B    X=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][2].vx, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 13, g_DebugText);
            SetDebugStrRowColor(arg0, 13, 3);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Y=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][2].vy, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Z=");
        FieldDebugStringU32hex(D_800E4274[g_FieldEntity[g_FieldState.pcModelId].PosI][2].vz, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 14, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Offset X=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].OffsetX, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 15, g_DebugText);
            SetDebugStrRowColor(arg0, 15, 2);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "Y=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].OffsetY, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, " Z=");
        FieldDebugStringU32hex(g_FieldModels[g_EntityToModel[entityId]].OffsetZ, g_DebugMessageBuffer);
        FieldDebugStringConcat(g_DebugText, g_DebugMessageBuffer);
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 16, g_DebugText);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
        FieldDebugStringCopy(g_DebugText, "SF");
        FieldDebugStringU32hex(Savemap.memory_bank_1[0] | (Savemap.memory_bank_1[1] << 8), g_DebugMessageBuffer);
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
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 17, g_DebugText);
            SetDebugStrRowColor(arg0, 17, 6);
        }
        if (D_80071E24 & 2) {
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
        if (D_8009FE8C | (D_80071E24 & 1)) {
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
        if (D_80071E24 & 2) {
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
        if (D_8009FE8C | (D_80071E24 & 1)) {
            SetStrToDebugRow(arg0, 19, g_DebugText);
            SetDebugStrRowColor(arg0, 19, 0);
        }
        if (D_80071E24 & 2) {
            DebugPrintToFieldWindow(g_DebugText);
        }
    }
}

void DebugPrintOpcode(const char* name, s32 arg1) {
    s32 temp_s0;
    s32 temp_s4;
    s32 var_s1;

    var_s1 = arg1;
    if (!(D_80071E24 & 4) || (D_80114498[g_CurrentEntity] != 0)) {
        FieldDebugStringCopy(g_DebugText, D_800E0630);
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
                FieldDebugStringU16hex(GET_PARAM_U8(temp_s0), g_DebugMessageBuffer);
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
    if (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity]) {
        FieldDebugStringCopy(g_DebugText, str);
        switch (kind) {
        case 1:
            FieldDebugStringU8hex(val, g_DebugMessageBuffer); // to single hex digit
            break;
        case 2:
            FieldDebugStringU16hex(val, g_DebugMessageBuffer); // to double hex digit
            break;
        case 4:
            FieldDebugStringU32hex(val, g_DebugMessageBuffer); // to four hex digits
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
 * @brief Opcode 0x24 - **WAIT** - Wait
 *
 * Memory layout:
 *
 * | 0x24 | A |
 *
 * - const UShort A: Amount (number of frames) to wait.
 * @details
 * g_FieldWaitCounter[g_CurrentEntity] == 0 by default. The opcode then
 * sets it to how many frames to wait before returning 1, which halts
 * execution of the script until next frame.
 *
 * If parameter == 0, the opcode behaves the same way as NOP.
 *
 * The opcode is then called once per frame, decrementing the counter until it
 * reaches 1, at which point it's set to 0 and 0 is returned, which
 * tells the script parser to continue executing next opcode.
 */

s32 OpcodeFuncWait(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("wait", 2);
    }

    if (g_FieldWaitCounter[g_CurrentEntity] == 0) {
        GET_PARAM_S16(g_FieldWaitCounter[g_CurrentEntity], 1);
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("wait_st=", g_FieldWaitCounter[g_CurrentEntity], 4);
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
        FieldDebugAddParseValueToPage2("wait=", g_FieldWaitCounter[g_CurrentEntity], 4);
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
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) | (1 << FieldEventReadMemoryU8(2, 3)));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncBitof(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bitof", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) & ~(1 << FieldEventReadMemoryU8(2, 3)));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncBitxr(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("bitxr", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) ^ (1 << FieldEventReadMemoryU8(2, 3)));
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

/*
 * Field-script opcode SLIP: Enables or disables slipping along a line
 *
 * Slipping allows the player to slide along a wall when running
 * against it instead of stopping. The wall must previously have a
 * line defined alongside it with opcode LINE.
 */

s32 OpcodeFuncSlip(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("slip", 1);
    }
    g_FieldLines[g_EntityToLine[g_CurrentEntity]].slipDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

/*
 * Field-script opcode IF: If, 1 byte, unsigned
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

/*
 * Field-script opcode LIF: Long If, 1 byte, unsigned
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
        result = FieldEventReadMemoryU8(1, 2) & (1 << FieldEventReadMemoryU8(2, 3));
        break;
    case IF_NOT_BIT:
        result = FieldEventReadMemoryU8(1, 2) & (1 << FieldEventReadMemoryU8(2, 3));
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

/*
 * Field-script opcode IF2: If, 2 bytes, signed
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

/*
 * Field-script opcode LIF2: Long if, 2 bytes, signed
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
        result = FieldEventReadMemoryS16(1, 2) & (1 << FieldEventReadMemoryS16(2, 4));
        break;
    case IF_NOT_BIT:
        result = FieldEventReadMemoryS16(1, 2) & (1 << FieldEventReadMemoryS16(2, 4));
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

/*
 * Field-script opcode IF2U: If, 2 bytes, unsigned
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

/*
 * Field-script opcode LIF2U: Long if, 2 bytes, unsigned
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
        result = (u16)FieldEventReadMemoryS16(1, 2) == (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_NOT_EQ:
        result = (u16)FieldEventReadMemoryS16(1, 2) != (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_GT:
        result = (u16)FieldEventReadMemoryS16(1, 2) > (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_LT:
        result = (u16)FieldEventReadMemoryS16(1, 2) < (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_GTE:
        result = (u16)FieldEventReadMemoryS16(1, 2) >= (u16)FieldEventReadMemoryS16(2, 4);
        break;
    case IF_LTE:
        result = (u16)FieldEventReadMemoryS16(1, 2) <= (u16)FieldEventReadMemoryS16(2, 4);
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
        result = FieldEventReadMemoryS16(1, 2) & (1 << FieldEventReadMemoryS16(2, 4));
        break;
    case IF_NOT_BIT:
        result = FieldEventReadMemoryS16(1, 2) & (1 << FieldEventReadMemoryS16(2, 4));
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

/*
 * Field-script opcode KEY!: Key check
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

/*
 * Field-script opcode KEYON: Key On
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

/*
 * Field-script opcode KEYOF: Key Off
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
    return FieldEventRequest(1, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)), GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncReqsw(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("reqsw", 2);
    }
    return FieldEventRequest(2, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)), GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncReqew(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("reqew", 2);
    }
    return FieldEventRequest(3, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)), GET_SCRIPTID(GET_PARAM_U8(2)));
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
    return FieldEventRequest(1, entityId, GET_PRIORITY(GET_PARAM_U8(2)), GET_SCRIPTID(GET_PARAM_U8(2)));
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
    return FieldEventRequest(2, entityId, GET_PRIORITY(GET_PARAM_U8(2)), GET_SCRIPTID(GET_PARAM_U8(2)));
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
    return FieldEventRequest(3, entityId, GET_PRIORITY(GET_PARAM_U8(2)), GET_SCRIPTID(GET_PARAM_U8(2)));
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
        FieldDebugStringConcat(g_DebugMessageBuffer, GET_ENTITY_NAME(target));
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

        GET_FIELD_SCRIPT_OFFSET(
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
        g_SavedFieldScriptPC[target][g_FieldScriptPriority[target]] = g_FieldScriptPC[target];

        scriptOffset = scriptId * 2;
        entityDataSize = target * 64;
        extrasHeaderSize = (s16)(g_FieldScripts->numExtras * 4);

        GET_FIELD_SCRIPT_OFFSET(
            g_FieldScriptPC[target], scriptOffset,
            entityDataSize + (g_FieldScripts->numEntities << 3),
            extrasHeaderSize);

        g_FieldScriptPriority[target] = priority;

        if (g_EntityToModel[target] != 255) {
            g_FieldModels[g_EntityToModel[target]].scriptedMoveMode = SMODE_NONE;
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

    if (g_FieldScriptSyncState[g_CurrentEntity][g_FieldScriptPriority[g_CurrentEntity]] == SYNC_WAITING) {
        g_FieldScriptSyncState[g_CurrentEntity][g_FieldScriptPriority[g_CurrentEntity]] = SYNC_DONE;
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

        activePcSlot = (u16*)((entity * sizeof(*fieldScriptPC)) + (s32)fieldScriptPC);
        loopSavedRow = (u16*)((entity * sizeof(*savedPC)) + (s32)savedPC);
        nextPc = loopSavedRow[g_FieldScriptPriority[entity]];

        *activePcSlot = nextPc;
        scriptPc = nextPc;
    }

    g_SavedFieldScriptPC[g_CurrentEntity][g_FieldScriptPriority[g_CurrentEntity]] = 0;
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("ret=", g_FieldScriptPriority[g_CurrentEntity], 2);
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

    while (g_FieldScriptPriority[g_CurrentEntity] < (priority - 1) && g_FieldScriptPriority[g_CurrentEntity] < 7) {
        if (g_FieldScriptSyncState[g_CurrentEntity][g_FieldScriptPriority[g_CurrentEntity]] == SYNC_WAITING) {
            g_FieldScriptSyncState[g_CurrentEntity][g_FieldScriptPriority[g_CurrentEntity]] = SYNC_DONE;
        }
        g_FieldScriptPriority[g_CurrentEntity]++;
        g_SavedFieldScriptPC[g_CurrentEntity][g_FieldScriptPriority[g_CurrentEntity]] = 0;
    }
    SavedScriptIds[g_CurrentEntity][priority] = scriptId;
    scriptId *= 2;
    extrasHeaderSize = (s16)(g_FieldScripts->numExtras * 4);

    GET_FIELD_SCRIPT_OFFSET(
        g_FieldScriptPC[g_CurrentEntity], scriptId,
        (g_FieldScripts->numEntities * 8) + (g_CurrentEntity * 64),
        extrasHeaderSize);

    g_FieldScriptPriority[g_CurrentEntity] = priority;
    if (g_DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("ret=", g_FieldScriptPriority[g_CurrentEntity], 2);
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncPmjmp);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncPmjmp2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMgame);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncBatle);

void FieldEventClearAkaoStruct(void) {
    s32 i;
    s16* p;

    D_8009A000[0] = 0;
    for (i = 5, p = &D_8009A000[10]; i >= 0; i--) {
        *(s32*)(p + 2) = 0;
        p -= 2;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAkao);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAkao2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncSe);

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
    D_8009A00C = (s16)FieldEventReadMemoryS16(4, 6);
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

    akaoData = akaoId * 4 + g_FieldScripts->numEntities * 8 + (s32)g_FieldScripts;
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
        g_pFieldState->nextBattleMusic = (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
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
        g_pFieldState->nextFieldMusic = (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
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
        g_pFieldState->eventCmdParam = 1; // Tells party menu module to start tutorial.
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
            FieldDebugAddParseValueToPage2("evt result=", g_pFieldState->movieCommandState, 2);
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

/*
 * Field-script opcode MULCK (0xF5): set the music lock from the opcode operand.
 *
 * While g_FieldMusicLock is nonzero the MUSIC/FMUSC opcodes skip handing the
 * song to the sound engine, so field music stops responding until a later
 * MULCK 0 (or a reset) clears it again.
 *
 * The operand is read straight out of the running script:
 *   g_FieldScripts          - the current map's script bytecode
 *   g_FieldScriptPC[entity]  - that entity's program counter (byte offset into
 * it) g_CurrentEntity          - the entity whose script is currently executing
 * so g_FieldScripts[pc + 1] is the 1-byte operand. The program counter is then
 * stepped past the 2-byte instruction (opcode + operand).
 */
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

/*
 * Field-script opcode DSKCG: request a disc change.
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
        D_8009D588 = GET_PARAM_U8(1);
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

/*
 * Field-script opcode UC: lock or unlock player control.
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
        g_FieldModelAnimStatus[g_pFieldState->pcModelId] =
            ANIMSTATUS_DEFAULT_LOOP;
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

/*
 * Field-script opcode CC: hand player control to another entity.
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

/*
 * Field-script opcode CHAR: attach a field model to the current entity.
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

/*
 * Field-script opcode DFANM: set a model's default (looping) animation.
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
        g_FieldModelAnimId[g_EntityToModel[g_CurrentEntity]] = GET_PARAM_U8(1);
        g_FieldModelEffAnimSpeed[g_EntityToModel[g_CurrentEntity]] =
            g_FieldModelBaseAnimSpeed[g_EntityToModel[g_CurrentEntity]] /
            GET_PARAM_U8(2);
        modelIdx = g_EntityToModel[g_CurrentEntity];
        if (g_FieldModelAnimStatus[modelIdx] == ANIMSTATUS_HOLD_FRAME) {
            g_FieldModelAnimStatus[modelIdx] = ANIMSTATUS_DEFAULT_LOOP;
        }
    }
    PC_INC(3);
    return 1;
}

/*
 * Field-script opcode CCANM: set one of the player animation ids
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

/*
 * Starts the animation requested by the current ANIME-style opcode on the
 * model attached to the executing entity: animation id from the first
 * operand, playback speed from the per-model base speed divided by the
 * second operand, frame counter rewound and the last frame looked up in
 * the animation header of the model's file.
 */
void StartModelAnimation(void) {
    u8 modelIdx;
    u8* anims;
    FieldModelEntry* model;

    g_FieldModels[g_EntityToModel[g_CurrentEntity]].activeAnimId = GET_PARAM_U8(1);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed =
        g_FieldModelBaseAnimSpeed[g_EntityToModel[g_CurrentEntity]] /
        GET_PARAM_U8(2);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame = 0;
    modelIdx = g_EntityToModel[g_CurrentEntity];
    model = &g_FieldModelData->modelEntries[g_FieldModelLoaderData[modelIdx].modelEntryIndex];
    anims = model->modelData + model->animationOffset;
    g_FieldModels[modelIdx].animLastFrame = *(u16*)&anims[g_FieldEntity[modelIdx].activeAnimId * 16] - 1;
}

/*
 * Field-script opcode ANIME1/ANIME2: play an animation on the entity's
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

    switch (g_FieldModelAnimStatus[g_EntityToModel[g_CurrentEntity]]) {
    case ANIMSTATUS_DEFAULT_LOOP:
    case ANIMSTATUS_SCRIPTED_LOOP:
    case ANIMSTATUS_HOLD_FRAME:
        StartModelAnimation();
        if (g_FieldCurrentOpcode == 0xAE) {
            g_FieldModelAnimStatus[g_EntityToModel[g_CurrentEntity]] = 5;
            PC_INC(3);
            return 0;
        }
        g_FieldModelAnimStatus[g_EntityToModel[g_CurrentEntity]] = 2;
        break;
    case ANIMSTATUS_PLAY_ONCE_SYNC_DONE:
        g_FieldModelAnimStatus[g_EntityToModel[g_CurrentEntity]] = 0;
        PC_INC(3);
        return 0;
    }
    return 1;
}

/*
 * Field-script opcode ANIM!1/ANIM!2: like ANIME1/ANIME2 but the model
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

    switch (g_FieldModelAnimStatus[g_EntityToModel[g_CurrentEntity]]) {
    case ANIMSTATUS_DEFAULT_LOOP:
    case ANIMSTATUS_SCRIPTED_LOOP:
    case ANIMSTATUS_HOLD_FRAME:
        StartModelAnimation();
        if (g_FieldCurrentOpcode == 0xAF) {
            g_FieldModelAnimStatus[g_EntityToModel[g_CurrentEntity]] = 6;
            PC_INC(3);
            return 0;
        }
        g_FieldModelAnimStatus[g_EntityToModel[g_CurrentEntity]] = 2;
        break;
    case ANIMSTATUS_PLAY_ONCE_SYNC_DONE:
        g_FieldModelAnimStatus[g_EntityToModel[g_CurrentEntity]] = 3;
        PC_INC(3);
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncCanim);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncCanmEx);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAnimw);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAnimb);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMove);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncFmove);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncCmove);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncFcfix);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncJump);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncLader);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncPmova);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMova);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldMoveToEntityUpdate);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncDira);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncPdira);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldEventSetDirByActorId);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncTura);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncPtura);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldEntityTurnToEntity);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncOfstd);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncOfstw);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncTurnw);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncTurn);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncTurnr);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncDir);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncSlidr);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncSldr2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncTalkr);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncTlkr2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMsped);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAsped);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncGtdir);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncPgtdr);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncGetai);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncGetaxy);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAxyzi);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncPxyzi);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncVisi);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncTlkon);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncXyzi);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncXyz);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncXyi);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMes);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMpnam);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAsk);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncWclsEx);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncWsizw);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncWsize);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncWrow);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncWmove);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncWrest);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncWclse);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncWmode);

/**
 * @brief Opcode 0x8F - **AND** - Bitwise AND (8-bit)
 *
 * Memory layout:
 *
 * | 0x8F | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: Contains an operand of the bitwise AND and receives the
 * result.
 * - const UByte Oper: The second operand of the bitwise AND.
 * @details
 * Performs a bitwise AND operation between "Dest" and "Oper" and stores the
 * result back into "Dest". If the Source Bank is 0 then the "Oper" is the
 * operand to AND with. If the Source Bank is an 8 bit bank, then the "Oper" is
 * the address in that bank where the operand is.
 */
s32 OpcodeFuncAnd(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("and", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) & FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x90 - **AND2** - Bitwise AND (16-bit)
 *
 * Memory layout:
 *
 * | 0x90 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank, or zero if "Oper" is specified as a literal
 * value.
 * - const UByte Dest: Address containing an operand of the bitwise AND, and
 * that which receives the result.
 * - const UShort Oper: 16-bit operand of the bitwise AND, or address of the
 * second operand, if S is non-zero.
 * @details
 * Performs a bitwise AND operation between  "Dest"  and "Oper" and stores the
 * result back into  "Dest" . If the Source Bank is 0 then the "Oper" is the
 * operand to AND with. If the Source Bank is a 16-bit bank, then the "Oper" is
 * the address in that bank where the operand is.
 */
s32 OpcodeFuncAnd2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("and2", 3);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(1, 2) & FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x91 - **OR** - Bitwise OR (8-bit)
 *
 * Memory layout:
 *
 * | 0x91 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: S: Source bank
 * - const UByte Dest: Contains an operand of the bitwise OR and receives the
 * result.
 * - const UByte Oper: The second operand of the bitwise OR.
 * @details
 * Performs a bitwise OR operation between "Dest" and "Oper" and stores the
 * result back into "Dest". If the Source Bank is 0 then the "Oper" is the
 * operand to OR with. If the Source Bank
 * is an 8 bit bank, then the "Oper" is the address in that bank where
 * the operand is.
 */
s32 OpcodeFuncOr(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("or", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) | FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x92 - **OR2** - Bitwise OR (16-bit)
 *
 * Memory layout:
 *
 * | 0x92 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank, or zero if "Oper" is specified as a literal
 * value.
 * - const UByte Dest: Address containing an operand of the bitwise OR, and that
 * which receives the result.
 * - const UShort Oper: 16-bit operand of the bitwise OR, or address of the
 * second operand, if S is non-zero
 * @details
 * Performs a bitwise OR operation between  "Dest"  and "Oper" and stores the
 * result back into  "Dest" . If the Source Bank is 0 then "Oper" is the operand
 * to OR with. If the Source Bank is a 16-bit bank, then the "Oper" is the
 * address in that bank where the operand is.
 */
s32 OpcodeFuncOr2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("or2", 3);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(1, 2) | FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x93 - **XOR** - Bitwise XOR (8-bit)
 *
 * Memory layout:
 *
 * | 0x93 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: Contains an operand of the bitwise XOR and receives the
 * result.
 * - const UByte Oper: The second operand of the bitwise XOR.
 * @details
 * Performs a bitwise XOR operation between "Dest" and "Oper" and stores the
 * result back into "Dest". If the Source Bank is 0 then the Operis the operand
 * to XOR with. If the Source Bank is an 8 bit bank, then the "Oper" is the
 * address in that bank where the operand is.
 */
s32 OpcodeFuncXor(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("xor", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) ^ FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x94 - **XOR2** - Bitwise XOR (16-bit)
 *
 * Memory layout:
 *
 * | 0x94 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank, or zero if "Oper" is specified as a literal
 * value.
 * - const UByte Dest: Address containing an operand of the bitwise XOR, and
 * that which receives the result.
 * - const UShort Oper: 16-bit operand of the bitwise XOR, or address of the
 * second operand, if S is non-zero.
 * @details
 * Performs a bitwise XOR operation between  "Dest"  and "Oper" and stores the
 * result back into  "Dest" . If the Source Bank is 0 then the "Oper" is the
 * operand to XOR with. If the Source Bank is a 16-bit bank, then the "Oper" is
 * the address in that bank where the operand is.
 */
s32 OpcodeFuncXor2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("xor2", 3);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(1, 2) ^ FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x85 - **PLUS** - Addition (8-bit)
 *
 * Memory layout:
 *
 * | 0x85 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is added.
 * - const UByte Oper: The operand, added to the destination.
 * @details
 * Adds two numbers together and stores the result back into  "Dest" . The
 * result of the addition wraps around into the range of 0-255. If the Source
 * Bank is 0 then the "Oper" is added to the destination value. If the Source
 * Bank is an 8 bit bank, then the "Oper" is the address in that bank where the
 * operand is.
 */
s32 OpcodeFuncPlus(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("plus", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) + FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x76 - **PLUS!** - Saturated Addition (8-bit)
 *
 * Memory layout:
 *
 * | 0x76 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is added.
 * - const UByte Oper: The operand, added to the destination.
 * @details
 * Adds two numbers together and stores the result back into "Dest". The result
 * of the addition is capped at 255. If the Source Bank is 0 then the "Oper" is
 * added to the destination value. If the Source Bank is an 8 bit bank, then the
 * "Oper" is the address in that bank where the operand is.
 */
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
 * @brief Opcode 0x86 - **PLUS2** - Addition (16-bit)
 *
 * Memory layout:
 *
 * | 0x86 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is added.
 * - const SWord Oper: The operand, added to the destination.
 * @details
 * Adds two numbers together and stores the result back into  "Dest" . The
 * result of the addition wraps around into the 16-bit range. If the Source Bank
 * is 0 then the "Oper" is added to the destination value. If the
 * Source Bank is an 16 bit bank, then the "Oper" is the address in that bank
 * where the operand is.
 */
s32 OpcodeFuncPlus2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("plus2", 3);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(1, 2) + FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x77 - **PLS2!** - Saturated Addition (16-bit)
 *
 * Memory layout:
 *
 * | 0x77 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is added.
 * - const SWord Oper: The operand, added to the destination
 * @details
 * Adds two numbers together and stores the result back into "Dest" The result
 * of the addition is capped at 32767. The result is not capped at the negative
 * end, however (-32768), so adding two large negative numbers together will
 * still produce wrap-around. If the Source Bank is 0 then the "Oper" is added
 * to the destination value. If the Source Bank is an 16 bit bank, then the
 * "Oper" is the address in that bank where the operand is.
 */
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
 * @brief Opcode 0x87 - **MINUS** - Subtraction (8-bit)
 *
 * Memory layout:
 *
 * | 0x87 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is
 * subtracted.
 * - const UByte Oper: The operand to be subtracted from the destination.
 * @details
 * Subtracts two numbers and stores the result back into  "Dest" . The result of
 * the subtraction wraps around into the range of 0-255. If the Source Bank is 0
 * then the "Oper" is subtracted from the destination value. If the Source Bank
 * is an 8 bit bank, then the "Oper" is the address in that bank where the
 * operand is.
 */
s32 OpcodeFuncMinus(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("minus", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) - FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x78 - **MINS!** - Saturated Subtraction (8-bit)
 *
 * Memory layout:
 *
 * | 0x78 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is
 * subtracted.
 * - const UByte Oper: The operand to be subtracted from the destination.
 * @details
 * Subtracts "Oper" from "Dest" and stores the result back into "Dest". The
 * result of the subtraction is capped at 0. If the Source Bank is 0 then the
 * "Oper" is subtracted from the destination value. If the Source Bank is an 8
 * bit bank, then the "Oper" is the address in that bank where the operand is.
 */
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
 * @brief Opcode 0x88 - **MINS2** - Subtraction (16-bit)
 *
 * Memory layout:
 *
 * | 0x98 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is
 * subtracted.
 * - const SWord Oper: The operand to be subtracted from the destination.
 * @details
 * Subtracts two numbers and stores the result back into "Dest". The result of
 * the subtraction wraps around into the 16-bit range. If the Source Bank is 0
 * then the "Oper" is subtracted from the destination value. If the Source Bank
 * is an 16 bit bank, then the "Oper" is the address in that bank where the
 * operand is.
 */
s32 OpcodeFuncMins2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mins2", 3);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(1, 2) - FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x79 - **MNS2!** - Saturated Subtraction (16-bit)
 *
 * Memory layout:
 *
 * | 0x79 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is
 * subtracted.
 * - const SWord Oper: The operand to be subtracted from the destination.
 * @details
 * Subtracts "Oper" from "Dest" and stores the result back into "Dest". The
 * result of the subtraction is capped at -32768. The result is not capped at
 * the positive end (32767), so subtracting a large negative number from a large
 * positive number will still produce wrap-around. If the Source Bank is 0 then
 * the "Oper" is subtracted from the destination value. If the
 * Source
 * Bank is an 16 bit bank, then the "Oper" is the address in that bank
 * where the operand is.
 */
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
 * @brief Opcode 0x89 - **MUL** - Multiplication (8-bit)
 *
 * Memory layout:
 *
 * | 0x89 | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is
 * multiplied.
 * - const UByte Oper: The operand, which is multiplied with the destination.
 * @details
 * Multiplies two numbers together and stores the result back into "Dest". The
 * result of the Multiplication is capped at 255. If the Source Bank is 0 then
 * the the value "Oper" is multiplied with the destination value. If the Source
 * Bank is an 8 bit bank, then the "Oper" is the address in that bank where the
 * operand is.
 */
s32 OpcodeFuncMul(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mul", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) * FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x8A - **MUL2** - Multiplication (16-bit)
 *
 * Memory layout:
 *
 * | 0x8A | D/S | Dest | Oper |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: The destination variable, to which the operand is
 * multiplied.
 * - const SWord Oper: The operand, which is multiplied with the destination.
 * @details
 * Multiplies two numbers together and stores the result back into "Dest". The
 * result of the Multiplication is capped at 32767. If the Source Bank is 0 then
 * the the value "Oper" is multiplied with the destination value. If the Source
 * Bank is an 8 bit bank, then the "Oper" is the address in that bank where the
 * operand is.
 */
s32 OpcodeFuncMul2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("mul2", 3);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(1, 2) * FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x8B - **DIV** - Division (8-bit)
 *
 * Memory layout:
 *
 * | 0x8B | D/S | Dest | Den |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: Contains the nominator of the division and receives the
 * quotient.
 * - const UByte Den: The denominator of the division.
 * @details
 * Divides "Dest" by "Den" and stores the result back into "Dest". The result of
 * the division is rounded towards zero to the nearest integer. If the Source
 * Bank is 0 then the "Den" is the denominator. If the Source Bank is an 8 bit
 * bank, then the "Den" is the address in that bank where the denominator is.
 */
s32 OpcodeFuncDiv(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("div", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) / FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x8C - **DIV2** - Division (16-bit)
 *
 * Memory layout:
 *
 * | 0x8C | D/S | Dest | Den |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: Contains the nominator of the division and receives the
 * quotient.
 * - const UByte Den: The denominator of the division.
 * @details
 * Divides "Dest" by "Den" and stores the result back into "Dest". The
 * result of the division is rounded towards zero to the nearest integer. If the
 * Source Bank is 0 then the "Den" is the denominator. If the Source Bank is an
 * 8 bit bank, then the "Den" is the address in that bank where the denominator
 * is.
 */
s32 OpcodeFuncDiv2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("div2", 3);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(1, 2) / FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x8D - **REMAI** - Modulus (8-bit)
 *
 * Memory layout:
 *
 * | 0x8D | D/S | Dest | Den |
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: Contains the nominator of the division and receives the
 * remainder.
 * - const UByte Den: The denominator of the division.
 * @details
 * Divides "Dest" by "Den" and stores the remainder back into "Dest". If the
 * Source Bank is 0 then the "Den" is the denominator. If the Source Bank is an
 * 8 bit bank, then the "Den" is the address in that bank where the denominator
 * is.
 */
s32 OpcodeFuncRemai(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("remai", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) % FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

/**
 * @brief Opcode 0x8E - **REMA2** - Modulus (16-bit)
 *
 * Memory layout:
 *
 * | 0x8E | D/S | Dest | Den
 *
 * - const Bit[4] D: Destination bank
 * - const Bit[4] S: Source bank
 * - const UByte Dest: Contains the nominator of the division and receives the
 * remainder.
 * - const SWord Den: The denominator of the division.
 * @details
 * Divides "Dest" by "Den" and stores the remainder back into "Dest". If the
 * Source Bank is 0 then the "Den" is the denominator. If the Source Bank is an
 * 16 bit bank, then the "Den" is the address in that bank where the denominator
 * is.
 */
s32 OpcodeFuncRema2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("rema2", 3);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(1, 2) % FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

/**
 * @brief Opcode 0x95 - **INC** - Increment (8-bit)
 *
 * Memory layout:
 *
 * | 0x95 | B | A |
 *
 * - const UByte B: Destination bank.
 * - const UByte A: Address.
 * @details
 * Increments the 8-bit value found at bank B, address A. If the value is 0xFF,
 * it will roll over to 0x00. If you specify a 16-bit bank, only the lower byte
 * will be incremented, and if the lower byte is 0xFF, the higher byte will be
 * unaffected whilst the lower byte will return to 0x00.
 */
s32 OpcodeFuncInc(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("inc", 2);
    }
    FieldEventWriteMemoryU8(2, 2, FieldEventReadMemoryU8(2, 2) + 1);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x7A - **INC!** - Saturated Increment (8-bit)
 *
 * Memory layout:
 *
 * | 0x7A | 0/D | Dest |
 *
 * - const Bit[4] D: Destination bank
 * - const UByte Dest: The destination address in the bank where the variable is
 * incremented.
 * @details
 * Increments the value in "Dest" by 1. The result is capped at 255.
 */
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
 * @brief Opcode  0x96 - **INC2** - Increment (16-bit)
 *
 * Memory layout:
 *
 * | 0x96 | B | A |
 *
 * - const UByte B: Destination bank.
 * - const UByte A: Address.
 * @details
 * Increments the 16-bit value found at bank B, address A. If the value is
 * 0xFFFF, it will roll over to 0x0000.
 */
s32 OpcodeFuncInc2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("inc2", 3);
    }
    FieldEventWriteMemoryS16(2, 2, FieldEventReadMemoryS16(2, 2) + 1);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode  0x7B - **INC2!** - Saturated Increment (16-bit)
 *
 * Memory layout:
 *
 * | 0x7B | 0/D | Dest |
 *
 * - const UByte B: Destination bank.
 * - const UByte Dest: The destination address in the bank where the variable is
 * incremented.
 * @details
 * Increments the value in "Dest" by 1. The result is capped at
 * 32767.
 */
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
 * @brief Opcode 0x97 - **DEC** - Decrement (8-bit)
 *
 * Memory layout:
 *
 * | 0x97 | B | A |
 *
 * - const UByte B: Destination bank.
 * - const UByte A: Address.
 * @details
 * Decrements the 8-bit value found at bank B, address A. If the value is
 * 0x00, it will roll over to 0xFF. If you specify a 16-bit bank, only the
 * lower byte will be decremented, and if the lower byte is 0x00, the higher
 * byte will be unaffected whilst the lower byte will return to 0xFF.
 */
s32 OpcodeFuncDec(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dec", 2);
    }
    FieldEventWriteMemoryU8(2, 2, FieldEventReadMemoryU8(2, 2) - 1);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x7C - **DEC!** - Saturated Decrement (8-bit)
 *
 * Memory layout:
 *
 * | 0x7C | 0/D | Dest |
 *
 * - const UByte B: Destination bank.
 * - const UByte Dest: The destination address in the bank where the variable is
 * deccremented.
 * @details
 * Decreases the value in "Dest" by 1. The result is capped at 0.
 */
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
 * @brief Opcode 0x98 - **DEC2** - Decrement (16-bit)
 *
 * Memory layout:
 *
 * | 0x98 | B | A |
 *
 * - const UByte B: Destination bank.
 * - const UByte A: Address.
 * @details
 * Decrements the 16-bit value found at bank B, address A. If the value is
 * 0x0000, it will roll over to 0xFFFF.
 */
s32 OpcodeFuncDec2(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("dec2", 3);
    }
    FieldEventWriteMemoryS16(2, 2, FieldEventReadMemoryS16(2, 2) - 1);
    PC_INC(3);
    return 0;
}

/**
 * @brief Opcode 0x7D - **DEC2!** - Saturated Decrement (16-bit)
 *
 * Memory layout:
 *
 * | 0x7D | 0/D | Dest |
 *
 * - const Bit[4] D: Destination bank
 * - const UByte Dest: The destination address in the bank where the variable is
 * Decremented.
 * @details
 * Decreases the value in "Dest" by 1. The result is capped at -32768.
 */
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
 * @brief Opcode 0x99 - **RANDM** - Random
 *
 * Memory layout:
 *
 * | 0x99 | B | A |
 *
 * - const UByte B: Destination bank.
 * - const UByte A: Destination address.
 * @details
 * Places a random 8-bit value into the destination bank and address specified.
 * If you specify a 16-bit bank, only the lower byte is randomised.
 */
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
 * @brief Opcode 0x7F - **RDMSD** - Seed Random Generator
 *
 * Memory layout:
 *
 * | 0x7F | B | S |
 *
 * - const UByte B: Bank in which the seed value is stored, or zero if S is
 * specified as a literal value.
 * - const UByte A: Destination address.
 * @details
 * Seeds the random number generator used by RANDOM. The lower four bits of the
 * arguments are used as the seed value by altering the offset used to take a
 * value from the table of pseudo-random numbers.
 */
s32 OpcodeFuncRdmsd(void) {
    if (g_DebugLevel & 3) {
        DebugPrintOpcode("rdmsd", 2);
    }
    g_RandomTableStep = (FieldEventReadMemoryU8(2, 2) << 4) + 1;
    PC_INC(3);
    return 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncBgon);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncBgoff);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncBgclr);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncBgrol);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncBgrol2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncPmvie);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMovie);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMvief);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMpjpo);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncScr2d);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncScrlc);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncScrla);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncScrlp);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncScrcc);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncScr2dc);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncScr2dl);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncScrlw);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncStpal);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncStpls);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncLdpal);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncLdpls);

static void FieldEventRectClear(s16* arg0) {
    arg0[0] = 0;
    arg0[1] = 0;
    arg0[2] = 0;
    arg0[3] = 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncCppal);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncCppal2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncRtpal);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncRtpal2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAdpal);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncAdpal2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMppal2);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMppal);

static void SetPcModel(void) {
    if (Savemap.memory_bank_2[9] != 0xFF && g_CharIdToEntity[Savemap.memory_bank_2[9]] != 0xFF &&
        g_EntityToModel[g_CharIdToEntity[Savemap.memory_bank_2[9]]] != 0xFF) {
        g_pFieldState->pcModelId = g_EntityToModel[g_CharIdToEntity[Savemap.memory_bank_2[9]]];
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
                FieldDebugAddParseValueToPage2("p+ ef=", g_CharIdToEntity[charId], 2);
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
static void PartyCompare(u8* party1, u8* party2, u8* party2Only, u8* party1Only) {
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

    PartyCompare(Savemap.partyID, &Savemap.memory_bank_2[9], notInSave, notInBank2);
    PartyRemove(Savemap.partyID, notInBank2);
    PartyAdd(Savemap.partyID, notInSave);
    g_PartyUpdatedByFieldScript = 1;
}

// Transfers party from save to bank 2 while preserving order in bank 2 of
// characters existing in both parties.
static void PartyFromSaveToBank2(void) {
    u8 notInBank2[3];
    u8 notInSave[3];

    PartyCompare(&Savemap.memory_bank_2[9], Savemap.partyID, notInBank2, notInSave);
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
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].SolidOff = GET_PARAM_U8(1);
    }
    PC_INC(2);
    return 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncVwoft);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncJoin);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncSplit);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldEventJoinSet);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldEventSplitSet);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldEventSplitJoinSetMove);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldEventSplitJoinEndMove);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldEventSplitJoinSetTurn);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", FieldEventSplitJoinEndTurn);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncFade);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncNfade);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncFadew);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncIdlck);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncGwcol);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncSwcol);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncLstmp);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncShake);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncStitm);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncDlitm);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncCkitm);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncSpcal);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncBgscr);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncBgdph);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncSmtra);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncDmtra);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncCmtra);

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
        if (g_pFieldState->eventCmd == EVTCMD_PARTY_MENU && g_pFieldState->eventCmdParam == 0) {
            PC_INC(4);
        }
        return 1;
    }

    if (g_pFieldState->eventCmd == GET_PARAM_U8(2)) {
        if (g_DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("evt result=", g_pFieldState->movieCommandState, 1);
        }
        if (g_pFieldState->movieCommandState == MOVCMD_DONE) {
            PC_INC(4);
            g_pFieldState->eventCmd = EVTCMD_NONE;
            g_pFieldState->movieCommandState = MOVCMD_IDLE;
            PartyFromSaveToBank2();
            return 0;
        }
    } else if (GET_PARAM_U8(2) == EVTCMD_UNK14 && g_pFieldState->eventCmd == EVTCMD_PLAY_MOVIE) {
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncGetpc);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMpara);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMpra2);

/**
 * @brief Opcode 0xD4 - **SIN** - sine
 *
 * Memory layout:
 *
 * | 0xD4 | B1 / B2 | B3 / B4 | D | M | A | S |
 *
 * - const Bit[4] B1: Destination bank.
 * - const Bit[4] B2: Bank to retrieve M, or zero if M is specified as a literal
 * value.
 * - const Bit[4] B3: Bank to retrieve A, or zero if A is specified as a literal
 * value.
 * - const Bit[4] B4: Bank to retrieve S, or zero if S is specified as a literal
 * value.
 * - const UByte D: Destination address.
 * - const UByte M: Multiplicand, or address to retrieve value if B2 is
 * non-zero.
 * - const UByte A: Addition, or address to retrieve value if B3 is non-zero.
 * - const UByte S: Variable for sin angle, or source address to retrieve value
 * if B4 is non-zero.
 * @details
 * Creates a variable from the another variable, with SIN, a multiplicand and an
 * addition factor
 */
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
 * @brief Opcode 0xD4 - **COS** - cosine
 *
 * Memory layout:
 *
 * | 0xD5 | B1 / B2 | B3 / B4 | D | M | A | S |
 *
 * - const Bit[4] B1: Destination bank.
 * - const Bit[4] B2: Bank to retrieve M, or zero if M is specified as a literal
 * value.
 * - const Bit[4] B3: Bank to retrieve A, or zero if A is specified as a literal
 * value.
 * - const Bit[4] B4: Bank to retrieve S, or zero if S is specified as a literal
 * value.
 * - const UByte D: Destination address.
 * - const UByte M: Multiplicand, or address to retrieve value if B2 is
 * non-zero.
 * - const UByte A: Addition, or address to retrieve value if B3 is non-zero.
 * - const UByte S: Variable for sin angle, or source address to retrieve value
 * if B4 is non-zero.
 * @details
 * Creates a variable from the another variable, with COS, a multiplicand and an
 * addition factor
 */
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
            SysInitPlayerStatFromEquip(i);
            SysInitPlayerStatFromMateria(i);
        }
    }
    SysCalculateTotalLureGilPreemptiveValue();
}

void SystemRestoreParty(void) {
    s32 i;

    SystemRefreshParty();
    for (i = 0; i < 3; i++) {
        SystemMenuAddHpByPartyId(i, 10000);
        SystemMenuAddMpByPartyId(i, 10000);
        if (Savemap.partyID[i] != 0xFF && g_BattleCharIdToCharId[Savemap.partyID[i]] <= 8) {
            Savemap.party[g_BattleCharIdToCharId[Savemap.partyID[i]]].status_flags = 0;
        }
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMhmmx);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncHmpmx);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMpPlus);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncMpMinus);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncHpPlus);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncHpMinus);

/**
 * @brief Opcode 0x39 - **GOLDU** - Gold Up
 *
 * Memory layout:
 *
 * | 0x39 | 0 | A |
 *
 * Increase by a constant amount:
 * - const UByte 0: Zero.
 * - const ULong A: Amount to increase.
 *
 * Increase by an amount found in memory:
 * - const Bit[4] B: Source bank.
 * - const UByte A: Source address.
 * - const UByte[3] 0: Three zero bytes.
 * @details
 * Increases the amount of gil by a constant amount, or by an amount found in
 * the source bank B and address A. The total gil is capped above by 0xFFFFFFFF;
 * attempts to increment further will fail.
 */
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
 * @brief Opcode 0x3A - **GOLDD** - Gold Down
 *
 * Memory layout:
 *
 * | 0x3A | 0 | A |
 *
 * Decrease by a constant amount:
 * - const UByte 0: Zero.
 * - const ULong A: Amount to decrease
 *
 * Decrease by an amount found in memory:
 * - const Bit[4] B: Source bank.
 * - const UByte A: Source address.
 * - const UByte[3] 0: Three zero bytes.
 * @details
 * Decreases the amount of gil by a constant amount, or by an amount found in
 * the source bank B and address A. The total gil is capped below by 0; attempts
 * to decrement further will fail.
 */
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
 * @brief Opcode 0x3B - **CHGLD** - Change Gold
 *
 * Memory layout:
 *
 * | 0x3B | B1 / B2 | A1 | A2 |
 *
 * - const Bit[4] B1: Destination bank 1.
 * - const Bit[4] B2: Destination bank 2.
 * - const UByte A1: Destination address 1.
 * - const UByte A2: Destination address 2.
 * @details
 * Copies the amount of gil the party has into the destination addresses.
 * As the gil amount is a four-byte value, the arguments require two destination
 * addresses to place two two-byte values into. Address 1 takes the lower two
 * bytes of the gil amount, while address 2 takes the higher two bytes.
 */
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

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncChmph);

INCLUDE_ASM("asm/us/field/nonmatchings/field_opcodes", OpcodeFuncChmst);

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
        g_FieldModelData->modelEntries[g_FieldModelLoaderData[modelId].modelEntryIndex].kawaiType = kawaiType;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiOp1 = 1;
        type = kawaiType;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiOp0 = 0;
        g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiDataOffset = &GET_PARAM_U8(3);
    }

    if (type == 0) {
        params = g_FieldModels[g_EntityToModel[g_CurrentEntity]].KawaiDataOffset;
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
