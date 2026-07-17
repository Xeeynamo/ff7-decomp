//! PSYQ=3.3 CC1=2.6.3
#include <game.h>

#define GET_PARAM_U8(offset)                                                   \
    (*(u8*)((s32)g_FieldScripts + g_FieldScriptPC[g_CurrentEntity] + (offset)))
#define GET_PARAM_S16(value, offset)                                           \
    value = GET_PARAM_U8(offset);                                              \
    value |= (GET_PARAM_U8((offset) + 1) << 8)
#define PC_INC(x) (g_FieldScriptPC[g_CurrentEntity] += (x))
#define PC_DEC(x) (g_FieldScriptPC[g_CurrentEntity] -= (x))

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
extern char D_800E0208[16]; // '0' to 'F' for hex digits
extern char D_800A0270[4];
extern s8 D_800E0628;
extern s8 D_800E0630;
extern u8 D_800E08C0[];
extern s16 D_800DF120[][2];
extern u16 D_800E1024;
extern s16 D_800E41B8;
extern s16 D_800E41C0;
extern s16 D_800E41BC;
extern s16 D_800E41C4;
extern u16 D_800E4210;
extern char D_800E4254[]; // debug text
extern char D_800E4288[]; // debug value transformed into text
extern struct GpuBuf D_800E4DF0[2];
extern u8 D_80114498[];

void func_800A364C(struct GpuBuf* buf);
void func_800AA180(Unk80074EA4* arg0, FieldLine* arg1);
void func_800AAB24(struct GpuBuf* buf);
s32 func_800A9CE8(FieldLine*, u_long*, u_long*);
u8 func_800BEE10(s16 arg0, s16 arg1);
void func_800BF3AC(s16 arg0, s16 arg1, u8 value);
s16 func_800BF908(s16 arg0, s16 arg1);
void func_800C0248(s16 arg0, s16 arg1, s16 value);
static u32 GetAkaoBlockOffset(s16 akaoId);
static void func_800D4840(const char* str);
static void func_800D4848(const char* errmsg);
void func_800D9F00(s32 val, const char* msg_out);
static void func_800DA334(char* dst, const char* src);
static void func_800DA368(char* arg0, char* arg1);
static void func_800DA424(s32 val, char* msg_out);
static void func_800DA444(s32 val, char* msg_out);
static void func_800DA480(s32 val, char* msg_out);

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

static void func_800D7F9C(void);
void func_800BA65C(s32 arg0) {
    if (D_8007EBE0) {
        func_800D4BFC();
        func_800BC338();
        func_800D7D6C();
        func_800D7F9C();
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
        func_8001F1BC(&D_80083274, 4, arg0, D_8009C6E0->unk0[0] ^ 1);
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
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BBF74);
#else
u8 func_800BBF74(s16 entityId, s16 priority, s16 scriptId) {
    u16 offset;
    s32 scriptOffset;
    s32 entityDataSize;
    s32 extrasHeaderSize;

    if (D_8009D820 & 3) {
        switch (scriptId) {
        case 1: // Pressed OK.
            func_800DA334(D_800E4288, "Talk=");
            break;
        case 2: // Pushed / within entity's collision radius.
            func_800DA334(D_800E4288, "Push=");
            break;
        case 3: // Across line.
            func_800DA334(D_800E4288, "Acrs=");
            break;
        case 4: // Touching line.
            func_800DA334(D_800E4288, "Toch=");
            break;
        case 5: // Started touching line.
            func_800DA334(D_800E4288, "TochON =");
            break;
        case 6: // Ended touching line.
            func_800DA334(D_800E4288, "TochOFF=");
            break;
        }
        // Prints entity name.
        func_800DA368(D_800E4288, (char*)g_FieldScripts +
                                      sizeof(FieldScriptHeader) + entityId * 8);
        func_800BECA4(D_800E4288, 0, 0);
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
            if (D_8007EB98[entityId] != 0xFF) {
                if (g_FieldModels[D_8007EB98[entityId]].scriptedMoveMode ==
                    SMODE_WALK) {
                    g_FieldModels[D_8007EB98[entityId]].activeAnimId = 0;
                    g_FieldModels[D_8007EB98[entityId]].animCurrentFrame = 0;
                    g_FieldModels[D_8007EB98[entityId]].animLastFrame = 0;
                }
                g_FieldModels[D_8007EB98[entityId]].scriptedMoveMode =
                    SMODE_NONE;
            }

            // Reset wait counter.
            D_800716DC[entityId] = 0;

            if (D_8009D820 & 3) {
                func_800BECA4("=recieved", 0, 0);
            }
        } else {
            if (D_8009D820 & 3) {
                func_800BECA4("=ret", 0, 0);
            }
        }
        return 1;
    }

    if (D_8009D820 & 3) {
        func_800BECA4("=ignored", 0, 0);
    }
    return 0;
}
#endif

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC338);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC438);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC4D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BC9FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BEAD4);

static void func_800BECA4(const char* str, s32 val, s32 kind) {
    if (!(D_80071E24 & 4) || D_80114498[g_CurrentEntity]) {
        func_800DA334(D_800E4254, str);
        switch (kind) {
        case 1:
            func_800DA424(val, D_800E4288); // to single hex digit
            break;
        case 2:
            func_800DA444(val, D_800E4288); // to double hex digit
            break;
        case 4:
            func_800DA480(val, D_800E4288); // to four hex digits
            break;
        default:
            func_800DA334(D_800E4288, D_800A0270);
            break;
        }
        func_800DA368(D_800E4254, D_800E4288);
        if (D_8009D820 & 1) {
            func_800D9F00(2, D_800E4254);
        }
        if (D_8009D820 & 2) {
            func_800D4840(D_800E4254);
        }
    }
}

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BEE10);
#else
static u8 func_800BEE10(s16 arg0, s16 arg1) {
    s32 indx;
    u8 value;
    u8 bankId;

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
    case 0:
        value = GET_PARAM_U8(arg1);
        if (D_8009D820 & 3) {
            func_800BECA4("G cons=", value, 2);
        }
        return value;
    case 1:
    case 2:
        indx = GET_PARAM_U8(arg1);
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 2);
        }
        return value;
    case 3:
    case 4:
        indx = GET_PARAM_U8(arg1) | 0x100;
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 2);
        }
        return value;
    case 11:
    case 12:
        indx = GET_PARAM_U8(arg1) | 0x200;
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 2);
        }
        return value;
    case 13:
    case 14:
        indx = GET_PARAM_U8(arg1) | 0x300;
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 2);
        }
        return value;
    case 15:
    case 7:
        indx = GET_PARAM_U8(arg1) | 0x400;
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 2);
        }
        return value;
    case 5:
    case 6:
        indx = GET_PARAM_U8(arg1);
        value = D_80075E24[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G mapv=", value, 2);
        }
        return value;
    }
    if (D_8009D820 & 3) {
        func_800BECA4("G data err=", bankId, 2);
    }
    func_800D4848("Bad Event arg!");
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BF3AC);
#else
static void func_800BF3AC(s16 arg0, s16 arg1, u8 value) {
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
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 3:
    case 4:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 11:
    case 12:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 13:
    case 14:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 15:
    case 7:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 5:
    case 6:
        indx = GET_PARAM_U8(arg1);
        D_80075E24[indx] = value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S mapv=", value, 2);
        }
        return;
    }
    if (D_8009D820 & 3) {
        func_800BECA4("S data err=", bankId, 2);
    }
    func_800D4848("Bad Event arg!");
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800BF908);
#else
static s16 func_800BF908(s16 arg0, s16 arg1) {
    u8 bankId;
    s32 indx;
    s16 value;

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
    case 0:
        GET_PARAM_S16(value, arg1);
        if (D_8009D820 & 3) {
            func_800BECA4("G cons=", value, 4);
        }
        return value;
    case 1:
        indx = GET_PARAM_U8(arg1);
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 2:
        indx = GET_PARAM_U8(arg1);
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 3:
        indx = GET_PARAM_U8(arg1) | 0x100;
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 4:
        indx = GET_PARAM_U8(arg1) | 0x100;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 11:
        indx = GET_PARAM_U8(arg1) | 0x200;
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 12:
        indx = GET_PARAM_U8(arg1) | 0x200;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 13:
        indx = GET_PARAM_U8(arg1) | 0x300;
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 14:
        indx = GET_PARAM_U8(arg1) | 0x300;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 15:
        indx = GET_PARAM_U8(arg1) | 0x400;
        value = Savemap.memory_bank_1[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 7:
        indx = GET_PARAM_U8(arg1) | 0x400;
        value = Savemap.memory_bank_1[indx];
        value |= Savemap.memory_bank_1[indx + 1] << 8;
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G glov=", value, 4);
        }
        return value;
    case 5:
        indx = GET_PARAM_U8(arg1);
        value = D_80075E24[indx];
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G mapv=", value, 4);
        }
        return value;
    case 6:
        indx = GET_PARAM_U8(arg1);
        value = D_80075E24[indx];
        value |= D_80075E24[indx + 1] << 8;
        if (D_8009D820 & 3) {
            func_800BECA4("G indx=", indx, 4);
            func_800BECA4("G mapv=", value, 4);
        }
        return value;
    }
    if (D_8009D820 & 3) {
        func_800BECA4("G data err=", bankId, 2);
    }
    func_800D4848("Bad Event arg!");
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0248);
#else
static void func_800C0248(s16 arg0, s16 arg1, s16 value) {
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
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 2:
        indx = GET_PARAM_U8(arg1);
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 4);
        }
        return;
    case 3:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 4:
        indx = GET_PARAM_U8(arg1) | 0x100;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 4);
        }
        return;
    case 11:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 12:
        indx = GET_PARAM_U8(arg1) | 0x200;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 4);
        }
        return;
    case 13:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 14:
        indx = GET_PARAM_U8(arg1) | 0x300;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 4);
        }
        return;
    case 15:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = (u8)value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 2);
        }
        return;
    case 7:
        indx = GET_PARAM_U8(arg1) | 0x400;
        Savemap.memory_bank_1[indx] = (u8)value;
        Savemap.memory_bank_1[indx + 1] = value >> 8;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S glov=", value, 4);
        }
        return;
    case 5:
        indx = GET_PARAM_U8(arg1);
        D_80075E24[indx] = (u8)value;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S mapv=", value, 2);
        }
        return;
    case 6:
        indx = GET_PARAM_U8(arg1);
        D_80075E24[indx] = (u8)value;
        D_80075E24[indx + 1] = value >> 8;
        if (D_8009D820 & 3) {
            func_800BECA4("S indx=", indx, 4);
            func_800BECA4("S mapv=", value, 4);
        }
        return;
    }
    if (D_8009D820 & 3) {
        func_800BECA4("S data err=", bankId, 2);
    }
    func_800D4848("Bad Event arg!");
}
#endif

s32 func_800C0B54(void) {
    if (D_8009D820 & 3) {
        func_800DA444(D_8009A058, D_800E4288);
        func_800DA368(D_800E4288, "???");
        func_800BEAD4(D_800E4288, 8);
        func_800DA214(3, 0x7F, 0, 0);
    } else {
        func_800D4848("Bad Event code!");
    }
    return 1;
}

// Nop in field scripts
s32 func_800C0BE8(void) {
    PC_INC(1);
    return 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0C18);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0DE0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0E5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0EDC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0F58);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C0FD8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C107C);

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1214);
#else
s32 func_800C1214(void) {
    s16 offset;
    u8 bank;
    u8 value;

    if (D_8009D820 & 3) {
        func_800BEAD4("getx", 6);
    }
    bank = GET_PARAM_U8(1) >> 4;
    offset = GET_PARAM_U8(3) + func_800BF908(2, 3);
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

    func_800BF3AC(4, 5, value);
    PC_INC(7);
    return 0;
}
#endif

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C13B0);
#else
s32 func_800C13B0(void) {
    s16 end;
    s16 start;
    s16 where;
    u8 bank;
    u8 value;
    s16 i;

    if (D_8009D820 & 3) {
        func_800BEAD4("srchx", 8);
    }
    bank = GET_PARAM_U8(1) >> 4;
    start = GET_PARAM_U8(4) + func_800BF908(2, 5);
    end = GET_PARAM_U8(4) + func_800BF908(3, 7);
    value = func_800BEE10(4, 9);
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
                func_800C0248(6, 10, i);
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
                func_800C0248(6, 10, i);
                PC_INC(11);
                return 0;
            }
        }
        break;
    }
    func_800C0248(6, 10, -1);
    PC_INC(11);
    return 0;
}
#endif

s32 func_800C1674(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("biton", 3);
    }
    func_800BF3AC(1, 2, func_800BEE10(1, 2) | (1 << func_800BEE10(2, 3)));
    PC_INC(4);
    return 0;
}

s32 func_800C1714(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("bitof", 3);
    }
    func_800BF3AC(1, 2, func_800BEE10(1, 2) & ~(1 << func_800BEE10(2, 3)));
    PC_INC(4);
    return 0;
}

s32 func_800C17B8(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("bitxr", 3);
    }
    func_800BF3AC(1, 2, func_800BEE10(1, 2) ^ (1 << func_800BEE10(2, 3)));
    PC_INC(4);
    return 0;
}

s32 func_800C1858(void) {
    s16 value;

    if (D_8009D820 & 3) {
        func_800BEAD4("line", 8);
    }

    if (g_FieldLineCount >= 32) {
        func_800D4848("many lineobj!");
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

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1AB4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1BF4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1D24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1DE4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C1EEC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2000);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C228C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2394);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C24A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2754);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C285C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2970);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2BFC);

s32 func_800C2CA8(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("keyon", 3);
    }
    if (GET_PARAM_U8(2) & 2) {
        return func_800C2E00(D_8009C6E0->unk80);
    } else {
        return func_800C2E00(D_8009C6E0->unk70);
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2D54);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2E00);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2F7C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C2FFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C307C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C30FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C31E4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C32CC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C33B4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3A20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3C34);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3EA0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3F1C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C3FA0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C401C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C40A4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C42B0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4350);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C43C4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C45AC);

void func_800C46A4(void) {
    s32 i;
    s16* p;

    D_8009A000[0] = 0;
    for (i = 5, p = &D_8009A000[10]; i >= 0; i--) {
        *(s32*)(p + 2) = 0;
        p -= 2;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C46D0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4804);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C493C);

s32 func_800C49EC(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("music", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x10;
    return func_800C4BCC();
}

s32 func_800C4A40(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("musvt", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x14;
    return func_800C4BCC();
}

s32 func_800C4A94(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("musvm", 1);
    }
    func_800C46A4();
    D_8009A000[0] = 0x15;
    return func_800C4BCC();
}

s32 func_800C4AE8(void) {
    u32 result;

    if (D_8009D820 & 3) {
        func_800BEAD4("cmusc", 5);
    }
    func_800C46A4();
    *D_8009A000 = GET_PARAM_U8(3);
    *D_8009A008 = (s16)func_800BF908(3, 4);
    D_8009A00C = (s16)func_800BF908(4, 6);
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
        if (D_8009D820 & 3) {
            func_800BECA4("music=", akaoId, 2);
        }
        *D_8009A004 = (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
        D_8009C6E0->unk48 = *D_8009A004;
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

s32 func_800C4CE8(void) {
    u8 akaoId;

    if (D_8009D820 & 3) {
        func_800BEAD4("bmusc", 1);
    }
    if (g_FieldMusicLock == 0) {
        akaoId = GET_PARAM_U8(1);
        if (D_8009D820 & 3) {
            func_800BECA4("bmusic=", akaoId, 2);
        }
        D_8009C6E0->unk44 =
            (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
    } else {
        D_8009C6E0->unk44 = 0;
    }
    PC_INC(2);
    return 0;
}

s32 func_800C4DE8(void) {
    u8 akaoId;

    if (D_8009D820 & 3) {
        func_800BEAD4("fmusc", 1);
    }
    if (g_FieldMusicLock == 0) {
        akaoId = GET_PARAM_U8(1);
        if (D_8009D820 & 3) {
            func_800BECA4("bmusic=", akaoId, 2);
        }
        D_8009C6E0->unk48 =
            (u8*)((s32)g_FieldScripts + GetAkaoBlockOffset(akaoId));
    } else {
        D_8009C6E0->unk48 = 0;
    }
    PC_INC(2);
    return 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C4EE8);

/* func_800BEAD4 returns void; the callers above already match with the implicit
   int declaration, but SetMusicLock only matches with the void prototype in
   scope (it frees v0 for reuse instead of treating it as a return value). */
extern void func_800BEAD4(const char*, int);

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
s32 SetMusicLock(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("mulck", 1);
    }
    g_FieldMusicLock = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 func_800C50EC(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("bgmovie", 1);
    }
    D_8009C6E0->backgroundMovieEnabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 func_800C5194(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("scrlo", 1);
    }
    D_8009C6E0->scrloSet = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

/*
 * Field-script opcode DSKCG: request a disc change.
 *
 * Runs as a small state machine on the field main-loop step (unk0[1]):
 * on first execution it stores the requested disc number and switches the
 * field loop into the disc-change step (13), then keeps returning 1
 * (opcode not finished) until the loop reports the swap is done
 * (movieState == 2). Only then does the script advance past the opcode.
 */
s32 RequestDiskChange(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("dskcg", 1);
    }
    switch (D_8009C6E0->unk0[1]) {
    case 0:
        D_8009C6E0->unk0[1] = 13;
        D_8009D588 = GET_PARAM_U8(1);
        return 1;
    case 13:
        if (D_8009C6E0->movieState == 2) {
            D_8009C6E0->unk0[1] = 0;
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
s32 SetCharacterLock(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("uc", 1);
    }
    D_80081DC4 = D_8009C6E0->characterLock = GET_PARAM_U8(1);
    if (D_80081DC4 == 0) {
        D_800756E8[(s16)D_8009C6E0->unk2A] = 0;
    }
    PC_INC(2);
    return 0;
}

s32 func_800C5414(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("btlon", 1);
    }
    D_8009C6E0->battlesDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 func_800C54BC(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("mpdsp", 1);
    }
    D_8009C6E0->mpdspSet = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 func_800C5564(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("mvcam", 1);
    }
    D_8009C6E0->movieCamDisabled = GET_PARAM_U8(1);
    PC_INC(2);
    return 0;
}

s32 func_800C560C(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("gmovr", 0);
    }
    D_8009C6E0->unk0[1] = 26;
    D_8009C6E0->movieState = 0;
    return 1;
}

/*
 * Field-script opcode CC: hand player control to another entity.
 *
 * The operand is a script entity id; if that entity has a field model
 * assigned (D_8007EB98 entry != 0xFF) it becomes the new player model.
 */
s32 SetControlCharacter(void) {
    u8 charId;

    if (D_8009D820 & 3) {
        func_800BEAD4("cc", 1);
    }
    charId = GET_PARAM_U8(1);
    if (D_8007EB98[charId] != 0xFF) {
        D_8009C6E0->unk2A = D_8007EB98[charId];
    }
    PC_INC(2);
    return 0;
}

/*
 * Field-script opcode CHAR: attach a field model to the current entity.
 *
 * Allocates the next model slot (g_FieldModelCount) for the executing entity,
 * records the mapping in D_8007EB98 and initializes the model with the
 * model id from the opcode operand and the owning entity id.
 */
s32 AssignCharacterModel(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("char", 1);
    }
    D_8007EB98[g_CurrentEntity] = g_FieldModelCount++;
    g_FieldModels[D_8007EB98[g_CurrentEntity]].unk66 = GET_PARAM_U8(1);
    g_FieldModels[D_8007EB98[g_CurrentEntity]].unk5C = 1;
    g_FieldModels[D_8007EB98[g_CurrentEntity]].entityId = g_CurrentEntity;
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
s32 SetDefaultAnimation(void) {
    u8 modelIdx;

    if (D_8009D820 & 3) {
        func_800BEAD4("dfanm", 2);
    }
    if (D_8007EB98[g_CurrentEntity] != 0xFF) {
        D_8008325C[D_8007EB98[g_CurrentEntity]] = GET_PARAM_U8(1);
        D_80082248[D_8007EB98[g_CurrentEntity]] =
            D_8009D828[D_8007EB98[g_CurrentEntity]] / GET_PARAM_U8(2);
        modelIdx = D_8007EB98[g_CurrentEntity];
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
s32 SetControlCharacterAnimation(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("ccanm", 3);
    }
    switch (GET_PARAM_U8(3)) {
    case 0:
        D_8009C6E0->unk2C = GET_PARAM_U8(1);
        break;
    case 1:
        D_8009C6E0->unk2E = GET_PARAM_U8(1);
        break;
    case 2:
        D_8009C6E0->unk30 = GET_PARAM_U8(1);
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

    g_FieldModels[D_8007EB98[g_CurrentEntity]].activeAnimId = GET_PARAM_U8(1);
    g_FieldModels[D_8007EB98[g_CurrentEntity]].animSpeed =
        D_8009D828[D_8007EB98[g_CurrentEntity]] / GET_PARAM_U8(2);
    g_FieldModels[D_8007EB98[g_CurrentEntity]].animCurrentFrame = 0;
    modelIdx = D_8007EB98[g_CurrentEntity];
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
#ifndef NON_MATCHINGS
// Close but not matching: the register allocator picks $v1/$a0 instead of
// $v0/$v1 for the state-2 branch (7 bytes of register fields differ).
INCLUDE_ASM("asm/us/field/nonmatchings/field", PlayAnimation);
#else
s32 PlayAnimation(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("anime", 2);
    }
    if (D_8007EB98[g_CurrentEntity] != 0xFF) {
        switch (D_800756E8[D_8007EB98[g_CurrentEntity]]) {
        case 0:
        case 1:
        case 3:
            StartModelAnimation();
            if (D_8009A058 == 0xAE) {
                D_800756E8[D_8007EB98[g_CurrentEntity]] = 5;
                PC_INC(3);
                return 0;
            }
            D_800756E8[D_8007EB98[g_CurrentEntity]] = 2;
            return 1;
        case 4:
            D_800756E8[D_8007EB98[g_CurrentEntity]] = 0;
            PC_INC(3);
            return 0;
        default:
            return 1;
        }
    }
    PC_INC(3);
    return 0;
}
#endif

/*
 * Field-script opcode ANIM!1/ANIM!2: like ANIME1/ANIME2 but the model
 * keeps holding the last frame once the animation completes (state 3)
 * instead of returning to its default animation. 0xAE becomes 0xAF and
 * state 5 becomes 6 to tell the two opcode pairs apart.
 */
#ifndef NON_MATCHINGS
// Same register-allocation mismatch as PlayAnimation above.
INCLUDE_ASM("asm/us/field/nonmatchings/field", PlayAnimationHold);
#else
s32 PlayAnimationHold(void) {
    if (D_8009D820 & 3) {
        func_800BEAD4("anim!", 2);
    }
    if (D_8007EB98[g_CurrentEntity] != 0xFF) {
        switch (D_800756E8[D_8007EB98[g_CurrentEntity]]) {
        case 0:
        case 1:
        case 3:
            StartModelAnimation();
            if (D_8009A058 == 0xAF) {
                D_800756E8[D_8007EB98[g_CurrentEntity]] = 6;
                break;
            }
            D_800756E8[D_8007EB98[g_CurrentEntity]] = 2;
            return 1;
        case 4:
            D_800756E8[D_8007EB98[g_CurrentEntity]] = 3;
            break;
        default:
            return 1;
        }
    }
    PC_INC(3);
    return 0;
}
#endif

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C5FF4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C63CC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C6748);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C684C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C6924);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C6D64);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C6FD8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C728C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7354);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C75F0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7C3C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7CE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C7D5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C814C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C81C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C826C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8514);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8588);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8634);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8B98);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C8F64);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9080);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C91D8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C955C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9A20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9B88);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9C84);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9D80);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9E7C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800C9F78);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA074);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA158);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA254);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA394);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA490);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA5D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA77C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CA95C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CAA24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CAAEC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CAC98);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CADFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CAF60);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB01C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB0B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB1CC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB28C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB354);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB450);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB4F8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB5C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB660);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB718);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB7C0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB858);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB8F4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CB98C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBA28);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBAC0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBB5C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBBF4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBCA4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBD40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBDFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBE94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBF40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CBFDC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC098);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC134);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC1D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC284);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC358);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC404);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC4D8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC558);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC5EC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC670);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC70C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC78C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC824);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC8A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC944);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CC9EC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCA68);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCB10);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCBBC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCC3C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCCC8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCD54);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCE94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CCFE8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD0C4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD16C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD2E8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD3F0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD554);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD5F0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD6B0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD770);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD834);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CD91C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDA24);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDB0C);

static void func_800CDC14(s16* arg0) {
    arg0[0] = 0;
    arg0[1] = 0;
    arg0[2] = 0;
    arg0[3] = 0;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDC28);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDD40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CDE8C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE054);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE480);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE6F4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CE904);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CEB20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CEB94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CEE44);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF028);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF140);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF200);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF2BC);

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

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF718);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF874);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CF9B8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFAF0);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFB84);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFC1C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFCE4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800CFE78);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0180);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0518);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0938);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D0B4C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1200);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1350);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D152C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1654);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D184C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D195C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1A80);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1B94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1C68);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1D3C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1DB8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1F20);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D1FDC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2098);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2164);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2794);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D28A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D298C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2A70);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2B60);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2C60);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2E94);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D2F3C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3004);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3124);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3264);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3330);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D33FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D348C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3548);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D368C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3728);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3840);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3958);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3A70);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3B88);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3C18);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3CA8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3D40);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3DCC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3E64);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D3F30);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4038);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4160);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4300);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4378);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4420);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D44E8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D4780);

static void func_800D4838(void) {}

static void func_800D4840(const char* str) {
    // used to print debug messages -- dummied out on release
}

static void func_800D4848(const char* errmsg) {
    func_800D828C(0, 100, 100, 150, 12);
    func_800DA214(0, 0x7F, 0, 0);
    func_800D9F00(0, errmsg);
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

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D7D6C);

static void func_800D7F9C(void) {
    func_800D828C(5, 0x6C, 0, 0x6C, 0x52);
    func_800DA334(D_800E4254, "Authr:");
    func_800DA368(D_800E4254, g_FieldScripts->author);
    func_800D9F00(5, D_800E4254);
    func_800DA334(D_800E4254, "Event:");
    func_800DA368(D_800E4254, g_FieldScripts->name);
    func_800D9F00(5, D_800E4254);
    func_800D9F00(5, "  Go");
    func_800D9F00(5, "  Stop");
    func_800D9F00(5, "  Step");
    func_800DA124(5, 5, "  Actor OFF");
    func_800DA124(5, 6, "  Info  OFF");
    func_800DA2CC(5);
    func_800D828C(4, 0x6C, 0x52, 0x6C, 0x52);
    func_800D9F00(4, &D_800E0628);
    func_800DA2CC(4);
    func_800D828C(3, 0x6C, 0xA4, 0x6C, 0x5C);
    func_800D9F00(3, &D_800E0630);
    func_800DA2CC(3);
    func_800D828C(1, 0, 0, 0x6C, 0xCA);
    func_800D9F00(1, &D_800E0628);
    func_800DA2CC(1);
    D_80099FFC = 3;
    D_8007EBCC = 4;
    D_8007EBDC = 8;
    D_80071E24 = 0;
    D_8009D820 = 0;
    D_80070788 = 0;
    D_80071C08 = 5;
    func_800DA1D4(5, 4);
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8194);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D828C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8334);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D83A8);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8420);

s32 func_800D8498(s16 arg0) { return D_800E08C0[arg0 * 378] == 0; }

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D84CC);

static void func_800D85C0(void) {
    D_800E41B8 = 0;
    D_800E41C0 = 0;
    D_800E41BC = 0;
    D_800E41C4 = 0;
    D_800E1024 ^= 1;
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D85FC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D8710);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D9C04);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D9F00);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800D9FFC);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA124);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA194);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA1D4);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA214);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA28C);

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA2CC);

static void func_800DA310(void) { D_800E4210 = (D_800E4210 + 1) & 3; }

static void func_800DA334(char* dst, const char* src) {
    if (*src) {
        do {
            *dst++ = *src++;
        } while (*src != '\0');
    }
    *dst = '\0';
}

static void func_800DA368(char* arg0, char* arg1) {
    if (*arg0 != '\0') {
        while (*++arg0 != '\0') {
        }
    }
    if (*arg1 != '\0') {
        do {
            *arg0++ = *arg1++;
        } while (*arg1 != '\0');
    }
    *arg0 = '\0';
}

static s32 func_800DA3C4(char* arg0) {
    s32 len = 0;

    while (*arg0 != '\0') {
        arg0++;
        len++;
    }
    return len;
}

static void func_800DA3F0(char* dst, char* src, s32 len) {
    s32 i;
    for (i = len - 1; i != -1; i--) {
        *dst = *src;
        src++;
        dst++;
    }
}

static void func_800DA424(s32 val, char* msg_out) {
    msg_out[1] = '\0';
    msg_out[0] = D_800E0208[val & 0xF];
}

static void func_800DA444(s32 val, char* msg_out) {
    msg_out[2] = '\0';
    msg_out[0] = D_800E0208[(val & 0xF0) >> 4];
    msg_out[1] = D_800E0208[val & 0xF];
}

static void func_800DA480(s32 val, char* msg_out) {
    msg_out[4] = '\0';
    msg_out[0] = D_800E0208[(val & 0xF000) >> 0xC];
    msg_out[1] = D_800E0208[(val & 0xF00) >> 8];
    msg_out[2] = D_800E0208[(val & 0xF0) >> 4];
    msg_out[3] = D_800E0208[val & 0xF];
}

INCLUDE_ASM("asm/us/field/nonmatchings/field", func_800DA4FC);
