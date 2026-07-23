//! PSYQ=3.3 CC1=2.6.3
#include <game.h>

#define GET_PARAM_U8(offset)                                                   \
    (*(u8*)((s32)g_FieldScripts + g_FieldScriptPC[g_CurrentEntity] + (offset)))
#define GET_PARAM_S16(value, offset)                                           \
    value = GET_PARAM_U8(offset);                                              \
    value |= (GET_PARAM_U8((offset) + 1) << 8)
#define PC_INC(x) (g_FieldScriptPC[g_CurrentEntity] += (x))
#define PC_DEC(x) (g_FieldScriptPC[g_CurrentEntity] -= (x))

#define GET_PRIORITY(x) (((x) >> 5) & 0x7)
#define GET_SCRIPTID(x) ((x) & 0x1F)

typedef enum {
    IF_EQ,
    IF_NOT_EQ,
    IF_GT,
    IF_LT,
    IF_GTE,
    IF_LTE,
    IF_AND,
    IF_XOR,
    IF_OR,
    IF_BIT,
    IF_NOT_BIT
} IfOps;

struct GpuBuf {
    /* 0x00000 */ u_long ot[0x400];
    /* 0x01000 */ u8 unk1000[0x1689C];
    /* 0x3FFC used by DrawOTag */
    /* 0x418C used by DrawOTag */
    /* 0x4190 used by DrawOTag */
    /* 0x1748C */ u_long ot1748C[1];
    /* 0x17490 */ u8 unk17490[0xC];
    /* 0x1749C */ u8 unk1749C[0x400];
}; // size:0x1789C

const u32 D_800A0000[] = {0, 0x01D801E0};
extern char g_FieldDebugDigits[16]; // '0' to 'F' for hex digits
extern char D_800A0270[4];
extern s8 D_800E0628;
extern s8 D_800E0630;
extern u8 D_800E08C0[];
extern s16 D_800DF120[][2];
extern u16 g_FieldDebugRb;
extern s16 g_FieldDebugRChars;
extern s16 g_FieldDebugRLines;
extern s16 g_FieldDebugRRect;
extern s16 g_FieldDebugRDm;
extern u16 g_FieldDebugTransp;
extern char DebugText[];  // debug text
extern char DebugMessageBuffer[]; // debug value transformed into text
extern struct GpuBuf D_800E4DF0[2];
extern u8 D_80114498[];
extern u8 actorIdCur;


void func_800A364C(struct GpuBuf* buf);
void func_800AA180(Unk80074EA4* arg0, FieldLine* arg1);
void func_800AAB24(struct GpuBuf* buf);
s32 func_800A9CE8(FieldLine*, u_long*, u_long*);
void DebugPrintOpcode(char* arg0, s32 arg1);
u8 FieldEventReadMemoryU8(s16 arg0, s16 arg1);
void FieldEventWriteMemoryU8(s16 arg0, s16 arg1, u8 value);
s16 FieldEventReadMemoryS16(s16 arg0, s16 arg1);
void FieldEventWriteMemoryS16(s16 arg0, s16 arg1, s16 value);
u32 func_800C2000(void);
u32 func_800C24A8(void);
u32 func_800C2970(void);
static s32 KeyCheck(u16 keys);
static u32 GetAkaoBlockOffset(s16 akaoId);
s32 func_800C33B4(s16 type, u8 target, u8 priority, u8 scriptId);
static void func_800D4840(const char* str);
static void FieldEventDebugError(const char* errmsg);
void AddStrNextDebugRow(s32 val, const char* msg_out);
static void FieldDebugStringCopy(char* dst, const char* src);
static void FieldDebugStringConcat(char* arg0, char* arg1);
static void FieldDebugStringU8hex(s32 val, char* msg_out);
static void FieldDebugStringU16hex(s32 val, char* msg_out);
static void FieldDebugStringU32hex(s32 val, char* msg_out);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A1368);

void func_800A1498(void) {
    if (D_800965E8 == 1) {
        func_8003408C();
    }
    D_80071A5C = 0;
    D_800965E8 = 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A14D8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A16CC);

const u32 D_800A0024[] = {0x00000000, 0x000801E0};
const u32 D_800A002C[] = {0x00E80000, 0x000801E0};
const u32 D_800A0034[] = {0x01D00000, 0x000801E0};
const u32 D_800A003C[] = {0x00000000, 0x00080140};
const u32 D_800A0044[] = {0x00E80000, 0x00080140};
const u32 D_800A004C[] = {0x01D00000, 0x00080140};
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A2314);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A2D5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A2F78);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A3020);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A364C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A4094);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A4134);

static s32 func_800A41CC(SVECTOR* arg0, long* arg1) {
    long sp10;
    long sp14;
    s32 ret;

    PushMatrix();
    SetRotMatrix(D_80071E40);
    SetTransMatrix(D_80071E40);
    SetGeomOffset(0, 0);
    ret = RotTransPers(arg0, arg1, &sp10, &sp14);
    PopMatrix();
    return ret;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A424C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A4430);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A45D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A47F8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A48B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A496C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A4BEC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A5FB4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A635C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A6418);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A65A4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A82A0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8304);

s16 func_800A8600(u8 arg0) { return D_800DF120[arg0][0]; }

s16 func_800A8620(u8 arg0) { return D_800DF120[arg0][1]; }

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8640);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8858);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8968);

static void func_800A8DF4(s32* arg0, s16* arg1, s16* arg2) {
    arg0[0] = arg1[0] - arg2[0];
    arg0[1] = arg1[1] - arg2[1];
    arg0[2] = arg1[2] - arg2[2];
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8E34);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A8F88);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A9B64);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A9CE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800A9EEC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA180);

static void func_800AA32C(FieldLine* lines) {
    s32 i;

    for (i = 0; i < LEN(g_FieldLines); i++) {
        lines->isOnLine = 0;
        lines++;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA348);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA514);

const u32 D_800A00BC[] = {0x00360000, 0x012A007A};
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA5E4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA870);

const u32 D_800A00DC[] = {0x00000000};
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AA930);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AAB24);

void func_800AB2AC(void) {}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB2B4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB310);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB4AC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB5E8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB728);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AB9C8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ABA34);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ABA70);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ABF0C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ABFE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AC35C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ACBA0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ACC5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AD7B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AD858);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ADAA4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ADC90);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800ADD70);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AE23C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AE4DC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AEE24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AF6EC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AF96C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AFAC4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AFDE4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800AFE1C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B0618);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B0A48);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B0EDC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B0FB0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B1C7C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B1E40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B2598);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B2A00);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B2DD4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B2F40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B480C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B4B04);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B4EAC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B5260);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B5504);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B62C4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B69C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B6AE4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B6B4C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B79B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B86D8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B8CF0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800B9B0C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BA534);

static void InitFieldDebugPages(void); 
void func_800BA65C(s32 arg0) {
    if (D_8007EBE0) {
        func_800D4BFC();
        func_800BC338();
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
            func_800BB3A8();
        }
    }
    if (D_80071E2C) {
        func_8001F1BC(&D_80083274, 4, arg0, g_FieldState->unk0 ^ 1);
    }
    func_800BC438(arg0);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BA7C4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BAF54);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BB1B4);

const u32 D_800A013C[] = {0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BB3A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BBBCC);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEventRequestRun);
#else
u8 FieldEventRequestRun(s16 entityId, s16 priority, s16 scriptId) {
    u16 offset;
    s32 scriptOffset;
    s32 entityDataSize;
    s32 extrasHeaderSize;

    if (DebugLevel & 3) {
        switch (scriptId) {
        case 1: // Pressed OK.
            FieldDebugStringCopy(DebugMessageBuffer, "Talk=");
            break;
        case 2: // Pushed / within entity's collision radius.
            FieldDebugStringCopy(DebugMessageBuffer, "Push=");
            break;
        case 3: // Across line.
            FieldDebugStringCopy(DebugMessageBuffer, "Acrs=");
            break;
        case 4: // Touching line.
            FieldDebugStringCopy(DebugMessageBuffer, "Toch=");
            break;
        case 5: // Started touching line.
            FieldDebugStringCopy(DebugMessageBuffer, "TochON =");
            break;
        case 6: // Ended touching line.
            FieldDebugStringCopy(DebugMessageBuffer, "TochOFF=");
            break;
        }
        // Prints entity name.
        FieldDebugStringConcat(
            DebugMessageBuffer,
            (char*)g_FieldScripts + sizeof(FieldScriptHeader) + entityId * 8);
        FieldDebugAddParseValueToPage2(DebugMessageBuffer, 0, 0);
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

        offset = *((u8*)(scriptOffset + entityDataSize + extrasHeaderSize +
                         (s32)g_FieldScripts) +
                   sizeof(FieldScriptHeader));
        offset |=
            *((u8*)(scriptOffset + (entityDataSize + (s32)g_FieldScripts) +
                    extrasHeaderSize) +
              sizeof(FieldScriptHeader) + 1)
            << 8;

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

            // Reset wait counter.
            g_FieldWaitCounter[entityId] = 0;

            if (DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("=recieved", 0, 0);
            }
        } else {
            if (DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("=ret", 0, 0);
            }
        }
        return 1;
    }

    if (DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("=ignored", 0, 0);
    }
    return 0;
}
#endif

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC338);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC438);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC4D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC9FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", DebugPrintOpcode);

static void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind) {
    if (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity]) {
        FieldDebugStringCopy(DebugText, str);
        switch (kind) {
        case 1:
            FieldDebugStringU8hex(val, DebugMessageBuffer); // to single hex digit
            break;
        case 2:
            FieldDebugStringU16hex(val, DebugMessageBuffer); // to double hex digit
            break;
        case 4:
            FieldDebugStringU32hex(val, DebugMessageBuffer); // to four hex digits
            break;
        default:
            FieldDebugStringCopy(DebugMessageBuffer, D_800A0270);
            break;
        }
        FieldDebugStringConcat(DebugText, DebugMessageBuffer);
        if (DebugLevel & 1) {
            AddStrNextDebugRow(2, DebugText);
        }
        if (DebugLevel & 2) {
            func_800D4840(DebugText);
        }
    }
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEventReadMemoryU8);
#else
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
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G cons=", value, 2);
        }
        return value;
    case 1:
    case 2:
        indx = GET_PARAM_U8(offset);
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 3:
    case 4:
        indx = GET_PARAM_U8(offset) | 0x100;
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 11:
    case 12:
        indx = GET_PARAM_U8(offset) | 0x200;
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 13:
    case 14:
        indx = GET_PARAM_U8(offset) | 0x300;
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 15:
    case 7:
        indx = GET_PARAM_U8(offset) | 0x400;
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 2);
        }
        return value;
    case 5:
    case 6:
        indx = GET_PARAM_U8(offset);
        value = D_80075E24[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G mapv=", value, 2);
        }
        return value;
    }
    if (DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("G data err=", bankId, 2);
    }
    FieldEventDebugError("Bad Event arg!");
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEventWriteMemoryU8);
#else
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
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 3:
    case 4:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 11:
    case 12:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 13:
    case 14:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 15:
    case 7:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 5:
    case 6:
        indx = GET_PARAM_U8(arg1);
        D_80075E24[indx] = value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S mapv=", value, 2);
        }
        return;
    }
    if (DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("S data err=", bankId, 2);
    }
    FieldEventDebugError("Bad Event arg!");
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEventReadMemoryS16);
#else
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
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G cons=", value, 4);
        }
        return value;
    case 1:
        indx = GET_PARAM_U8(offset);
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 2:
        indx = GET_PARAM_U8(offset);
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 3:
        indx = GET_PARAM_U8(offset) | 0x100;
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 4:
        indx = GET_PARAM_U8(offset) | 0x100;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 11:
        indx = GET_PARAM_U8(offset) | 0x200;
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 12:
        indx = GET_PARAM_U8(offset) | 0x200;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 13:
        indx = GET_PARAM_U8(offset) | 0x300;
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 14:
        indx = GET_PARAM_U8(offset) | 0x300;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 15:
        indx = GET_PARAM_U8(offset) | 0x400;
        value = Savemap.memory_bank_1[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 7:
        indx = GET_PARAM_U8(offset) | 0x400;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G glov=", value, 4);
        }
        return value;
    case 5:
        indx = GET_PARAM_U8(offset);
        value = D_80075E24[indx];
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G mapv=", value, 4);
        }
        return value;
    case 6:
        indx = GET_PARAM_U8(offset);
        value = D_80075E24[indx];
        value |= D_80075E24[indx + 1] << 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("G indx=", indx, 4);
            FieldDebugAddParseValueToPage2("G mapv=", value, 4);
        }
        return value;
    }
    if (DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("G data err=", bankId, 2);
    }
    FieldEventDebugError("Bad Event arg!");
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldEventWriteMemoryS16);
#else
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
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 2:
        indx = GET_PARAM_U8(arg1);
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 3:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 4:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 11:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 12:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 13:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 14:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 15:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 2);
        }
        return;
    case 7:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S glov=", value, 4);
        }
        return;
    case 5:
        indx = GET_PARAM_U8(arg1);
        D_80075E24[indx] = (u8)value;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S mapv=", value, 2);
        }
        return;
    case 6:
        indx = GET_PARAM_U8(arg1);
        D_80075E24[indx] = (u8)value;
        D_80075E24[indx + 1] = value >> 8;
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("S indx=", indx, 4);
            FieldDebugAddParseValueToPage2("S mapv=", value, 4);
        }
        return;
    }
    if (DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("S data err=", bankId, 2);
    }
    FieldEventDebugError("Bad Event arg!");
}
#endif


// called for opcodes 0c 0d 1a 1b 1c 1d 1e 1f 44 46 4c 4e be
s32 OpcodeFuncBad(void) {
    if (DebugLevel & 3) {
        FieldDebugStringU16hex(D_8009A058, DebugMessageBuffer);
        FieldDebugStringConcat(DebugMessageBuffer, "???");
        DebugPrintOpcode(DebugMessageBuffer, 8);
        FieldDebugPageSetColor(3, 0x7F, 0, 0);
    } else {
        FieldEventDebugError("Bad Event code!");
    }
    return 1;
}



////////////////////////////////////////
// End of of event.c
////////////////////////////////////////



/*
 * Field-script opcode NOP: Halts execution until next frame.
 */
s32 OpcodeFuncNop(void) {
    PC_INC(1);
    return 1;
}

/*
 * Field-script opcode WAIT: Waits given number of frames before resuming.
 *
 * g_FieldWaitCounter[g_CurrentEntity] == 0 by default. The opcode then
 * sets it to how many frames to wait before returning 1, which halts
 * execution of the script until next frame.
 *
 * If parameter == 0, the opcode behaves the same way as HALT.
 *
 * The opcode is then called once per frame, decrementing the counter until it
 * reaches 1, at which point it's set to 0 and 0 is returned, which
 * tells the script parser to continue executing next opcode.
 */

s32 OpcodeFuncWait(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("wait", 2);
    }

    if (g_FieldWaitCounter[g_CurrentEntity] == 0) {
        GET_PARAM_S16(g_FieldWaitCounter[g_CurrentEntity], 1);
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("wait_st=", g_FieldWaitCounter[g_CurrentEntity], 4);
        }
        if (g_FieldWaitCounter[g_CurrentEntity] == 0) {
            PC_INC(3);
            return 1;
        }
        return 1;
    }

    if (g_FieldWaitCounter[g_CurrentEntity] == 1) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("wait_end=", 1, 4);
        }
        g_FieldWaitCounter[g_CurrentEntity] = 0;
        PC_INC(3);
        return 0;
    }

    if (DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("wait=", g_FieldWaitCounter[g_CurrentEntity], 4);
    }

    g_FieldWaitCounter[g_CurrentEntity]--;
    return 1;
}

s32 OpcodeFuncSet(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("set", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncSet2(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("set2", 4);
    }
    FieldEventWriteMemoryS16(1, 2, FieldEventReadMemoryS16(2, 3));
    PC_INC(5);
    return 0;
}

s32 OpcodeFuncLbyte(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("lbyte", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(2, 3));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncHbyte(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("hbyte", 4);
    }
    FieldEventWriteMemoryU8(1, 2, (u8)(FieldEventReadMemoryS16(2, 3) >> 8));
    PC_INC(5);
    return 0;
}

s32 OpcodeFunc2byte(void) {
    s16 lhs;

    if (DebugLevel & 3) {
        DebugPrintOpcode("2byte", 5);
    }
    lhs = FieldEventReadMemoryU8(2, 4);
    FieldEventWriteMemoryS16(1, 3, lhs | (FieldEventReadMemoryU8(4, 5) << 8));
    PC_INC(6);
    return 0;
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSetx);
#else
s32 OpcodeFuncSetx(void) {
    s16 offset;
    u8 bank;
    u8 value;

    if (DebugLevel & 3) {
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
        D_80075E24[offset] = value;
        break;
    }
    PC_INC(7);
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGetx);
#else
s32 OpcodeFuncGetx(void) {
    s16 offset;
    u8 bank;
    u8 value;

    if (DebugLevel & 3) {
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
        value = D_80075E24[offset];
        break;
    }

    FieldEventWriteMemoryU8(4, 5, value);
    PC_INC(7);
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSrchx);
#else
s32 OpcodeFuncSrchx(void) {
    s16 end;
    s16 start;
    s16 where;
    u8 bank;
    u8 value;
    s16 i;

    if (DebugLevel & 3) {
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
            if (D_80075E24[i] == value) {
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
#endif

s32 OpcodeFuncBiton(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("biton", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) | (1 << FieldEventReadMemoryU8(2, 3)));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncBitof(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("bitof", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) & ~(1 << FieldEventReadMemoryU8(2, 3)));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncBitxr(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("bitxr", 3);
    }
    FieldEventWriteMemoryU8(1, 2, FieldEventReadMemoryU8(1, 2) ^ (1 << FieldEventReadMemoryU8(2, 3)));
    PC_INC(4);
    return 0;
}

s32 OpcodeFuncLine(void) {
    s16 value;

    if (DebugLevel & 3) {
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

    if (DebugLevel & 3) {
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
    if (DebugLevel & 3) {
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
    if (DebugLevel & 3) {
        DebugPrintOpcode("slip", 1);
    }
    g_FieldLines[g_EntityToLine[g_CurrentEntity]].slipDisabled =
        GET_PARAM_U8(1);
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
    if (DebugLevel & 3) {
        DebugPrintOpcode("if", 5);
    }
    if (func_800C2000()) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if=true", 0, 0);
        }
        // If comparison is true, continue executing next opcode.
        PC_INC(6);
    } else {
        if (DebugLevel & 3) {
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

    if (DebugLevel & 3) {
        DebugPrintOpcode("lif", 6);
    }
    if (func_800C2000()) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif=true", 0, 0);
        }
        PC_INC(7);
    } else {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif=false", 0, 0);
        }
        GET_PARAM_S16(param, 5);
        PC_INC(param + 5);
    }
    return 0;
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2000);
#else
u32 func_800C2000(void) {
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
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("ope err=", ope, 2);
        }
        break;
    }
    return result;
}
#endif

/*
 * Field-script opcode IF2: If, 2 bytes, signed
 *
 * Compares two s16 using a given logical operator.
 */
#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncIf2);
#else
s32 OpcodeFuncIf2(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("if2", 7);
    }
    if (func_800C24A8() != 0) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if2=true", 0, 0);
        }
        PC_INC(8);
    } else {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if2=false", 0, 0);
        }
        PC_INC(GET_PARAM_U8(7) + 7);
    }
    return 0;
}
#endif

/*
 * Field-script opcode LIF2: Long if, 2 bytes, signed
 *
 * Compares two s16 using a given logical operator.
 */
#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncLif2);
#else
s32 OpcodeFuncLif2(void) {
    s16 param;

    if (DebugLevel & 3) {
        DebugPrintOpcode("lif2", 8);
    }
    if (func_800C24A8() != 0) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif2=true", 0, 0);
        }
        PC_INC(9);
    } else {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif2=false", 0, 0);
        }
        GET_PARAM_S16(param, 7);
        PC_INC(param + 7);
    }
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C24A8);
#else
u32 func_800C24A8(void) {
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
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("ope err=", ope, 2);
        }
        break;
    }
    return result;
}
#endif

/*
 * Field-script opcode IF2U: If, 2 bytes, unsigned
 *
 * Compares two u16 using a given logical operator.
 */
#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncIf2u);
#else
s32 OpcodeFuncIf2u(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("if2", 7);
    }
    if (func_800C2970()) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if2=false", 0, 0);
        }
        PC_INC(8);
    } else {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("if2=true", 0, 0);
        }
        PC_INC(GET_PARAM_U8(7) + 7);
    }
    return 0;
}
#endif

/*
 * Field-script opcode LIF2U: Long if, 2 bytes, unsigned
 *
 * Compares two u16 using a given logical operator.
 */
#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncLif2u);
#else
s32 OpcodeFuncLif2u(void) {
    s16 param;

    if (DebugLevel & 3) {
        DebugPrintOpcode("lif2", 8);
    }
    if (func_800C2970() != 0) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif2=false", 0, 0);
        }
        PC_INC(9);
    } else {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("lif2=true", 0, 0);
        }
        GET_PARAM_S16(param, 7);
        PC_INC(param + 7);
    }
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2970);
#else
u32 func_800C2970(void) {
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
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("ope err=", ope, 2);
        }
        break;
    }
    return result;
}
#endif

/*
 * Field-script opcode KEY!: Key check
 *
 * Jumps ahead given number of bytes if given key(s) are not active.
 * All key opcodes only check the lower half word which contains the keys
 * for controller 1.
 */

s32 OpcodeFuncKeyEx(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("key!", 3);
    }
    if (GET_PARAM_U8(2) & 2) {
        return KeyCheck((u16)g_FieldState->activeKeys2);
    } else {
        return KeyCheck((u16)g_FieldState->activeKeys);
    }
}

/*
 * Field-script opcode KEYON: Key On
 *
 * Checks keys that player pressed this frame.
 */

s32 OpcodeFuncKeyon(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("keyon", 3);
    }
    if (GET_PARAM_U8(2) & 2) {
        return KeyCheck((u16)g_FieldState->newActiveKeys2);
    } else {
        return KeyCheck((u16)g_FieldState->newActiveKeys);
    }
}

/*
 * Field-script opcode KEYOF: Key Off
 *
 * Checks keys that player released this frame.
 */

s32 OpcodeFuncKeyof(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("keyof", 3);
    }
    if (GET_PARAM_U8(2) & 2) {
        return KeyCheck((u16)g_FieldState->newInactiveKeys2);
    } else {
        return KeyCheck((u16)g_FieldState->newInactiveKeys);
    }
}

static s32 KeyCheck(u16 keys) {
    u16 param;

    GET_PARAM_S16(param, 1);
    if (DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("key now=", keys, 4);
        FieldDebugAddParseValueToPage2("key chk=", param, 4);
    }
    if (keys & param) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("key=true", 0, 0);
        }
        PC_INC(4);
    } else {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("key=false", 0, 0);
        }
        PC_INC(GET_PARAM_U8(3) + 3);
    }
    return 0;
}

s32 OpcodeFuncReq(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("req", 2);
    }
    return func_800C33B4(1, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)),
                         GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncReqsw(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("reqsw", 2);
    }
    return func_800C33B4(2, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)),
                         GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncReqew(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("reqew", 2);
    }
    return func_800C33B4(3, GET_PARAM_U8(1), GET_PRIORITY(GET_PARAM_U8(2)),
                         GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncPreq(void) {
    u8 charId;
    u8 entityId;

    if (DebugLevel & 3) {
        DebugPrintOpcode("preq", 2);
    }
    charId = Savemap.memory_bank_2[GET_PARAM_U8(1) + 9];
    if (charId == 0xFF) {
        entityId = 0xFF;
    } else {
        entityId = g_CharIdToEntity[charId];
    }
    return func_800C33B4(1, entityId, GET_PRIORITY(GET_PARAM_U8(2)),
                         GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncPrqsw(void) {
    u8 charId;
    u8 entityId;

    if (DebugLevel & 3) {
        DebugPrintOpcode("prqsw", 2);
    }
    charId = Savemap.memory_bank_2[GET_PARAM_U8(1) + 9];
    if (charId == 0xFF) {
        entityId = 0xFF;
    } else {
        entityId = g_CharIdToEntity[charId];
    }
    return func_800C33B4(2, entityId, GET_PRIORITY(GET_PARAM_U8(2)),
                         GET_SCRIPTID(GET_PARAM_U8(2)));
}

s32 OpcodeFuncPrqew(void) {
    u8 charId;
    u8 entityId;

    if (DebugLevel & 3) {
        DebugPrintOpcode("prqew", 2);
    }
    charId = Savemap.memory_bank_2[GET_PARAM_U8(1) + 9];
    if (charId == 0xFF) {
        entityId = 0xFF;
    } else {
        entityId = g_CharIdToEntity[charId];
    }
    return func_800C33B4(3, entityId, GET_PRIORITY(GET_PARAM_U8(2)),
                         GET_SCRIPTID(GET_PARAM_U8(2)));
}

// Depends on decomp of func_800bc9fc due to shared string.
#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C33B4);
#else
s32 func_800C33B4(s16 type, u8 target, u8 priority, u8 scriptId) {
    s32 scriptOffset;
    s32 entityDataSize;
    s32 extrasHeaderSize;

    if (target == 0xFF) {
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("rqew=no one", 0, 0);
        }
        PC_INC(3);
        return 0;
    }

    if (DebugLevel & 3) {
        FieldDebugStringCopy(DebugMessageBuffer, "rq=");
        FieldDebugStringConcat(
            DebugMessageBuffer, (char*)((s32)g_FieldScripts) +
                            sizeof(FieldScriptHeader) + (target * 8));
        FieldDebugStringConcat(DebugMessageBuffer, "/");
        FieldDebugAddParseValueToPage2(DebugMessageBuffer, scriptId, 2);
    }

    if (type > 0) {
        if (type >= 3) {
            if (type == 3 && g_FieldScriptSyncWaitEntity[target][priority] ==
                                 g_CurrentEntity) {
                switch (g_FieldScriptSyncState[target][priority]) {
                case SYNC_WAITING:
                    if (DebugLevel & 3) {
                        FieldDebugAddParseValueToPage2("rqew=wait", 0, 0);
                    }
                    return 1;
                case SYNC_DONE:
                    if (DebugLevel & 3) {
                        FieldDebugAddParseValueToPage2("rqew=end", 0, 0);
                    }
                    g_FieldScriptSyncState[target][priority] = SYNC_NONE;
                    g_FieldScriptSyncWaitEntity[target][priority] = 0xFF;
                    PC_INC(3);
                    return 0;
                }
            }
        }
    }

    if (g_FieldScriptPriority[target] == priority) {
        switch (type) {
        case 1:
            PC_INC(3);
            if (DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("rq=skip", 0, 0);
            }
            return 0;
        case 2:
        case 3:
            if (DebugLevel & 3) {
                FieldDebugAddParseValueToPage2("rqw=busy", 0, 0);
            }
        }
        return 1;
    } else if (g_FieldScriptPriority[target] < priority) {
        if (g_SavedFieldScriptPC[target][priority] != 0) {
            switch (type) {
            case 1:
                PC_INC(3);
                if (DebugLevel & 3) {
                    FieldDebugAddParseValueToPage2("rq=skip", 0, 0);
                }
                return 0;
            case 2:
            case 3:
                if (DebugLevel & 3) {
                    FieldDebugAddParseValueToPage2("rqw=busy", 0, 0);
                }
            }
            return 1;
        }
        scriptOffset = scriptId * 2;
        entityDataSize = target * 64;
        extrasHeaderSize = (s16)(g_FieldScripts->numExtras * 4);

        g_SavedFieldScriptPC[target][priority] =
            *((u8*)(scriptOffset +
                    (entityDataSize + (g_FieldScripts->numEntities << 3)) +
                    extrasHeaderSize + (s32)g_FieldScripts) +
              sizeof(FieldScriptHeader));
        g_SavedFieldScriptPC[target][priority] |=
            *((u8*)(scriptOffset +
                    ((entityDataSize + (g_FieldScripts->numEntities << 3)) +
                     (s32)g_FieldScripts) +
                    extrasHeaderSize) +
              sizeof(FieldScriptHeader) + 1)
            << 8;

        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("rq=send", 0, 0);
        }

        if (type <= 0) {
            return 1;
        }

        if (type >= 3) {
            if (type != 3) {
                return 1;
            }
        } else {
            PC_INC(3);
            return 0;
        }

        g_FieldScriptSyncWaitEntity[target][priority] = g_CurrentEntity;
        g_FieldScriptSyncState[target][priority] = SYNC_WAITING;
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

        g_FieldScriptPC[target] =
            *((u8*)(scriptOffset +
                    (entityDataSize + (g_FieldScripts->numEntities << 3)) +
                    extrasHeaderSize + (s32)g_FieldScripts) +
              sizeof(FieldScriptHeader));
        g_FieldScriptPC[target] |=
            *((u8*)(scriptOffset +
                    ((entityDataSize + (g_FieldScripts->numEntities << 3)) +
                     (s32)g_FieldScripts) +
                    extrasHeaderSize) +
              sizeof(FieldScriptHeader) + 1)
            << 8;

        g_FieldScriptPriority[target] = priority;

        if (g_EntityToModel[target] != 0xFF) {
            g_FieldModels[g_EntityToModel[target]].scriptedMoveMode =
                SMODE_NONE;
        }
        g_FieldWaitCounter[target] = 0;

        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("rq=send", 0, 0);
        }

        if (type <= 0) {
            return 1;
        }

        if (type >= 3) {
            if (type != 3) {
                return 1;
            }
        } else {
            PC_INC(3);
            return 0;
        }

        g_FieldScriptSyncWaitEntity[target][priority] = g_CurrentEntity;
        g_FieldScriptSyncState[target][priority] = SYNC_WAITING;
        return 1;
    }
    if (DebugLevel & 3) {
        FieldDebugAddParseValueToPage2("rqw=busy*", 0, 0);
    }
    return 1;
}
#endif

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncRet);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncRetto);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBack);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncLback);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSkip);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncLskip);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMjump);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPmjmp);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPmjmp2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMgame);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBatle);

void func_800C46A4(void) {
    s32 i;
    s16* p;

    D_8009A000[0] = 0;
    for (i = 5, p = &D_8009A000[10]; i >= 0; i--) {
        *(s32*)(p + 2) = 0;
        p -= 2;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAkao);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAkao2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSe);

s32 OpcodeFuncMusic(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("music", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x10;
    return func_800C4BCC();
}

s32 OpcodeFuncMusvt(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("musvt", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x14;
    return func_800C4BCC();
}

s32 OpcodeFuncMusvm(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("musvm", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x15;
    return func_800C4BCC();
}

s32 OpcodeFuncCmusc(void) {
    u32 result;

    if (DebugLevel & 3) {
        DebugPrintOpcode("cmusc", 5);
    }
    func_800C46A4();
    *D_8009A000 = GET_PARAM_U8(3);
    *D_8009A008 = (s16)FieldEventReadMemoryS16(3, 4);
    D_8009A00C = (s16)FieldEventReadMemoryS16(4, 6);
    result = func_800C4BCC();
    PC_INC(6);
    return result;
}

s32 func_800C4BCC(void) {
    // Indexes into AKAO block of field file which contains the list of music
    // tracks available for current field.
    u8 akaoId;

    if (g_FieldMusicLock == 0) {
        akaoId = GET_PARAM_U8(1);
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("music=", akaoId, 2);
        }
        *D_8009A004 = (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
        g_FieldState->nextFieldMusic = *D_8009A004;
        func_8002DA7C();
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

    if (DebugLevel & 3) {
        DebugPrintOpcode("bmusc", 1);
    }
    if (g_FieldMusicLock == 0) {
        akaoId = GET_PARAM_U8(1);
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("bmusic=", akaoId, 2);
        }
        g_FieldState->nextBattleMusic =
            (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
    } else {
        g_FieldState->nextBattleMusic = 0;
    }
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncFmusc(void) {
    u8 akaoId;

    if (DebugLevel & 3) {
        DebugPrintOpcode("fmusc", 1);
    }
    if (g_FieldMusicLock == 0) {
        akaoId = GET_PARAM_U8(1);
        if (DebugLevel & 3) {
            FieldDebugAddParseValueToPage2("bmusic=", akaoId, 2);
        }
        g_FieldState->nextFieldMusic =
            (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
    } else {
        g_FieldState->nextFieldMusic = 0;
    }
    PC_INC(2);
    return 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncTutor);

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
    if (DebugLevel & 3) {
        DebugPrintOpcode("mulck", 1);
    }
    g_FieldMusicLock = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncBgmovie(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("bgmovie", 1);
    }
    g_FieldState->backgroundMovieEnabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncScrlo(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("scrlo", 1);
    }
    g_FieldState->scrloSet = GET_PARAM_U8(1);
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
    if (DebugLevel & 3) {
        DebugPrintOpcode("dskcg", 1);
    }
    switch (g_FieldState->opcode) {
    case FIELDOP_NONE:
        g_FieldState->opcode = FIELDOP_CD_CHANGE;
        D_8009D588 = GET_PARAM_U8(1);
        return 1;
    case FIELDOP_CD_CHANGE:
        if (g_FieldState->movieCommandState == MOVCMD_DONE) {
            g_FieldState->opcode = FIELDOP_NONE;
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
    if (DebugLevel & 3) {
        DebugPrintOpcode("uc", 1);
    }
    g_CharacterLock = g_FieldState->characterLock = GET_PARAM_U8(1);
    if (g_CharacterLock == 0) {
        D_800756E8[g_FieldState->pcModelId] = 0;
    }
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncBtlon(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("btlon", 1);
    }
    g_FieldState->battlesDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncMpdsp(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("mpdsp", 1);
    }
    g_FieldState->mpdspSet = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncMvcam(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("mvcam", 1);
    }
    g_FieldState->movieCamDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 OpcodeFuncGmovr(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("gmovr", 0);
    }
    g_FieldState->opcode = FIELDOP_GAME_OVER;
    g_FieldState->movieCommandState = MOVCMD_IDLE;
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

    if (DebugLevel & 3) {
        DebugPrintOpcode("cc", 1);
    }
    charId = GET_PARAM_U8(1);
    if (g_EntityToModel[charId] != 0xFF) {
        g_FieldState->pcModelId = g_EntityToModel[charId];
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
    if (DebugLevel & 3) {
        DebugPrintOpcode("char", 1);
    }
    g_EntityToModel[g_CurrentEntity] = g_FieldModelCount++;
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].unk66 = GET_PARAM_U8(1);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].unk5C = 1;
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

    if (DebugLevel & 3) {
        DebugPrintOpcode("dfanm", 2);
    }
    if (g_EntityToModel[g_CurrentEntity] != 0xFF) {
        D_8008325C[g_EntityToModel[g_CurrentEntity]] = GET_PARAM_U8(1);
        D_80082248[g_EntityToModel[g_CurrentEntity]] =
            D_8009D828[g_EntityToModel[g_CurrentEntity]] / GET_PARAM_U8(2);
        modelIdx = g_EntityToModel[g_CurrentEntity];
        if (D_800756E8[modelIdx] == 3) {
            D_800756E8[modelIdx] = 0;
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
    if (DebugLevel & 3) {
        DebugPrintOpcode("ccanm", 3);
    }
    switch (GET_PARAM_U8(3)) {
    case 0:
        g_FieldState->idleAnimId = GET_PARAM_U8(1);
        break;
    case 1:
        g_FieldState->walkAnimId = GET_PARAM_U8(1);
        break;
    case 2:
        g_FieldState->runAnimId = GET_PARAM_U8(1);
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
    Unk8004A62CSub* file;

    g_FieldModels[g_EntityToModel[g_CurrentEntity]].activeAnimId =
        GET_PARAM_U8(1);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].animSpeed =
        D_8009D828[g_EntityToModel[g_CurrentEntity]] / GET_PARAM_U8(2);
    g_FieldModels[g_EntityToModel[g_CurrentEntity]].animCurrentFrame = 0;
    modelIdx = g_EntityToModel[g_CurrentEntity];
    file = &D_8004A62C->unk4[D_8008357C[modelIdx].unk4];
    anims = file->unk1C + file->unk1A;
    g_FieldModels[modelIdx].animLastFrame =
        *(u16*)&anims[D_80074EA4[modelIdx].activeAnimId * 16] - 1;
}

/*
 * Field-script opcode ANIME1/ANIME2: play an animation on the entity's
 * model. D_8009A058 distinguishes which opcode invoked the handler: the
 * asynchronous variant (0xAE, ANIME2) marks the model as playing (state 5)
 * and lets the script continue, while ANIME1 blocks (state 2) until the
 * animation system reports completion (state 4), then resets the model to
 * its default animation.
 */
s32 OpcodeFuncAnime(void) {
    if (DebugLevel & 3) {
        DebugPrintOpcode("anime", 2);
    }

    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(3);
        return 0;
    }

    switch (D_800756E8[g_EntityToModel[g_CurrentEntity]]) {
    case 0:
    case 1:
    case 3:
        StartModelAnimation();
        if (D_8009A058 == 0xAE) {
            D_800756E8[g_EntityToModel[g_CurrentEntity]] = 5;
            PC_INC(3);
            return 0;
        }
        D_800756E8[g_EntityToModel[g_CurrentEntity]] = 2;
        break;
    case 4:
        D_800756E8[g_EntityToModel[g_CurrentEntity]] = 0;
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
    if (DebugLevel & 3) {
        DebugPrintOpcode("anim!", 2);
    }

    if (g_EntityToModel[g_CurrentEntity] == 0xFF) {
        PC_INC(3);
        return 0;
    }

    switch (D_800756E8[g_EntityToModel[g_CurrentEntity]]) {
    case 0:
    case 1:
    case 3:
        StartModelAnimation();
        if (D_8009A058 == 0xAF) {
            D_800756E8[g_EntityToModel[g_CurrentEntity]] = 6;
            PC_INC(3);
            return 0;
        }
        D_800756E8[g_EntityToModel[g_CurrentEntity]] = 2;
        break;
    case 4:
        D_800756E8[g_EntityToModel[g_CurrentEntity]] = 3;
        PC_INC(3);
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncCanim);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncCanmEx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAnimw);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAnimb);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMove);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncFmove);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncCmove);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncFcfix);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncJump);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncLader);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPmova);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMova);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7D5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDira);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPdira);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C826C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncTura);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPtura);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8634);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncOfstd);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncOfstw);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncTurnw);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncTurn);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncTurnr);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDir);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSlidr);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSldr2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncTalkr);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncTlkr2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMsped);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAsped);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGtdir);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPgtdr);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGetai);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGetaxy);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAxyzi);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPxyzi);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncVisi);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncTlkon);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncXyzi);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncXyz);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncXyi);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMes);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMpnam);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAsk);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWclsEx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWsizw);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWsize);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWrow);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWmove);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWrest);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWclse);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWmode);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAnd);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAnd2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncOr);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncOr2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncXor);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncXor2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPlus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPlusEx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPlus2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPls2Ex);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMinus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMinsEx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMins2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMns2Ex);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMul);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMul2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDiv);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDiv2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncRemai);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncRema2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncInc);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncIncEx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncInc2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncInc2Ex);


/*
 * Field-script opcode DEC: Decrement (8-bit)
 *
 * Decrements the 8-bit value found at bank B, address A.
 * If the value is 0x00, it will roll over to 0xFF.
 * If you specify a 16-bit bank, only the lower byte will 
 * be decremented, and if the lower byte is 0x00, the higher
 * byte will be unaffected whilst the lower byte will return to 0xFF.
 */
s32 OpcodeFuncDec(void)
{
    if (DebugLevel & 3) {
        DebugPrintOpcode("dec", 2);
    }
    FieldEventWriteMemoryU8(2,2,
        (FieldEventReadMemoryU8(2, 2) - 1) & 0xFF
    );
	PC_INC(3);
    return 0;
}


INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDecEx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDec2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDec2Ex);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncRandm);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncRdmsd);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBgon);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBgoff);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBgclr);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBgrol);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBgrol2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPmvie);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMovie);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMvief);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMpjpo);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncScr2d);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncScrlc);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncScrla);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncScrlp);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncScrcc);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncScr2dc);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncScr2dl);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncScrlw);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncStpal);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncStpls);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncLdpal);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncLdpls);

static void func_800CDC14(s16* arg0) {
    arg0[0] = 0;
    arg0[1] = 0;
    arg0[2] = 0;
    arg0[3] = 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncCppal);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncCppal2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncRtpal);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncRtpal2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAdpal);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncAdpal2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMppal2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMppal);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CEB20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPc);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPrtyp);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPrtym);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPrtye);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSptye);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGptye);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF368);

void func_800CF4CC(void*, void*, void*, void*);
void func_800CF66C(void*, void*);
void func_800CF6C0(void*, void*);
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF4CC);

static void func_800CF5A0(void) {
    s32 sp10[2];
    s32 sp18[2];

    func_800CF4CC(D_8009CBDC, D_8009CBDC + 0x7B5, sp10, sp18);
    func_800CF66C(D_8009CBDC, sp18);
    func_800CF6C0(D_8009CBDC, sp10);
    D_80071E34 = 1;
}

static void func_800CF60C(void) {
    s32 sp10[2];
    s32 sp18[2];

    func_800CF4CC(D_8009D391, D_8009D391 - 0x7B5, sp10, sp18);
    func_800CF66C(D_8009D391, sp18);
    func_800CF6C0(D_8009D391, sp10);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF66C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF6C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncPrtyq);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMembq);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMmbPlusMinus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMmblk);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMmbuk);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSolid);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncVwoft);

////////////////////////////////////////
// Begin of event_code_actions.c
////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncJoin);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSplit);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0518);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0938);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0B4C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1200);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1350);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D152C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncFade);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncNfade);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncFadew);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncIdlck);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGwcol);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSwcol);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncLstmp);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncShake);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncStitm);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDlitm);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncCkitm);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSpcal);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBgscr);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBgdph);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSmtra);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncDmtra);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncCmtra);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMenu);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMenu2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGetpc);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMpara);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMpra2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSin);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncCos);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D33FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D348C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMhmmx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncHmpmx);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMpPlus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncMpMinus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncHpPlus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncHpMinus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGoldPlus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncGoldMinus);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncChgld);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncChmph);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncChmst);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncSttim);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWspcl);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncWnumb);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBtlmd);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBtmd2);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBtrlt);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBtltb);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncBlink);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncKawai);

INCLUDE_ASM("asm/us/field/nonmatchings/field", OpcodeFuncKawiw);

static void func_800D4838(void) {}

static void func_800D4840(const char* str) {
    // used to print debug messages -- dummied out on release
}

static void FieldEventDebugError(const char* errmsg) {
    FieldDebugPageInit(0, 100, 100, 150, 12);
    FieldDebugPageSetColor(0, 0x7F, 0, 0);
    AddStrNextDebugRow(0, errmsg);
    D_80095DCC = 1;
    D_80099FFC = 4;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D48C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D493C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4B28);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4BC0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4BFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4C68);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4E24);

void func_800D4E88(s16 arg0, s16 arg1) { D_8008327E[arg0 * 24] = arg1; }

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4EB4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D5228);

static void func_800D5750(void) {
    func_800C46A4();
    D_8009A000[0] = 0x30;
    D_8009A004[0] = 1;
    D_8009A008[0] = 0x40;
    func_8002DA7C();
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D579C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D5A60);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D5C9C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D6D44);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D6E0C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D6F6C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D707C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D726C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D775C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D785C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D7970);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D7A58);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D7C98);

////////////////////////////////////////
// Begin of debug.c
////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugInitBuffers);

static void InitFieldDebugPages(void) {
    FieldDebugPageInit(5, 0x6C, 0, 0x6C, 0x52);
    FieldDebugStringCopy(DebugText, "Authr:");
    FieldDebugStringConcat(DebugText, g_FieldScripts->author);
    AddStrNextDebugRow(5, DebugText);
    FieldDebugStringCopy(DebugText, "Event:");
    FieldDebugStringConcat(DebugText, g_FieldScripts->name);
    AddStrNextDebugRow(5, DebugText);
    AddStrNextDebugRow(5, "  Go");
    AddStrNextDebugRow(5, "  Stop");
    AddStrNextDebugRow(5, "  Step");
    SetStrToDebugRow(5, 5, "  Actor OFF");
    SetStrToDebugRow(5, 6, "  Info  OFF");
    FieldDebugPageHide(5);
    FieldDebugPageInit(4, 0x6C, 0x52, 0x6C, 0x52);
    AddStrNextDebugRow(4, &D_800E0628);
    FieldDebugPageHide(4);
    FieldDebugPageInit(3, 0x6C, 0xA4, 0x6C, 0x5C);
    AddStrNextDebugRow(3, &D_800E0630);
    FieldDebugPageHide(3);
    FieldDebugPageInit(1, 0, 0, 0x6C, 0xCA);
    AddStrNextDebugRow(1, &D_800E0628);
    FieldDebugPageHide(1);
    D_80099FFC = 3;
    D_8007EBCC = 4;
    D_8007EBDC = 8;
    D_80071E24 = 0;
    DebugLevel = 0;
    D_80070788 = 0;
    g_FieldDebugCurPage = 5;
    FieldDebugPageSetHeadRow(5, 4);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPagesResetPosSize);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageSetPosSize);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageAddPos);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageAddSize);

bool FieldDebugPageIsRender(s16 arg0) { return D_800E08C0[arg0 * 378] == 0; }

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageResetStrings);

static void FieldDebugRenderClear(void) {
    g_FieldDebugRChars = 0;
    g_FieldDebugRLines = 0;
    g_FieldDebugRRect = 0;
    g_FieldDebugRDm = 0;
    g_FieldDebugRb ^= 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugRender);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugRenderPage);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugRenderString);

INCLUDE_ASM("asm/us/field/nonmatchings/field", AddStrNextDebugRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field", AddColorStrNextDebugRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field", SetStrToDebugRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field", SetDebugStrRowColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageSetHeadRow);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageSetColor);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageNotInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field", FieldDebugPageHide);

static void FieldDebugTranspSwitch(void) {
    g_FieldDebugTransp = (g_FieldDebugTransp + 1) & 3;
}

static void FieldDebugStringCopy(char* dst, const char* src) {
    if (*src) {
        do {
            *dst++ = *src++;
        } while (*src != '\0');
    }
    *dst = '\0';
}

static void FieldDebugStringConcat(char* dest, char* src) {
    if (*dest != '\0') {
        while (*++dest != '\0') {
        }
    }
    if (*src != '\0') {
        do {
            *dest++ = *src++;
        } while (*src != '\0');
    }
    *dest = '\0';
}

static s32 FieldDebugStringSize(char* src) {
    s32 len = 0;

    while (*src != '\0') {
        src++;
        len++;
    }
    return len;
}

static void FieldDebugStringPartCopy(char* dst, char* src, s32 len) {
    s32 i;
    for (i = len - 1; i != -1; i--) {
        *dst = *src;
        src++;
        dst++;
    }
}

static void FieldDebugStringU8hex(s32 val, char* msg_out) {
    msg_out[1] = '\0';
    msg_out[0] = g_FieldDebugDigits[val & 0xF];
}

static void FieldDebugStringU16hex(s32 val, char* msg_out) {
    msg_out[2] = '\0';
    msg_out[0] = g_FieldDebugDigits[(val & 0xF0) >> 4];
    msg_out[1] = g_FieldDebugDigits[val & 0xF];
}

static void FieldDebugStringU32hex(s32 val, char* msg_out) {
    msg_out[4] = '\0';
    msg_out[0] = g_FieldDebugDigits[(val & 0xF000) >> 0xC];
    msg_out[1] = g_FieldDebugDigits[(val & 0xF00) >> 8];
    msg_out[2] = g_FieldDebugDigits[(val & 0xF0) >> 4];
    msg_out[3] = g_FieldDebugDigits[val & 0xF];
}

////////////////////////////////////////
// End of debug.c
////////////////////////////////////////

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA4FC);
