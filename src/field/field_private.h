#ifndef FIELD_PRIVATE_H
#define FIELD_PRIVATE_H

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

typedef struct {
    /* 0x00 */ u8 entity; // low 6 bits index g_FieldEntityBgTrigger
    /* 0x01 */ u8 mask;   // bit tested against that entity's byte
} FieldBgAnimPair;

typedef struct FieldRenderData {
    OT_TYPE ot[0x1000];   // 0x00000: Main scene ordering table
    SPRT_16 Arrows[0x18]; // 0x04000: Field arrow sprite packets
    DR_MODE ArrowsDm;     // 0x04180: Arrow sprite draw mode

    OT_TYPE OtFadeDrenv;  // 0x0418c: Fade draw environment OT entry
    OT_TYPE OtSceneDrenv; // 0x04190: Scene draw environment OT entry

    DR_ENV FadeDrenv;  // 0x04194: Screen fade draw environment
    DR_ENV SceneDrenv; // 0x041d4: Main scene draw environment

    DR_ENV BgDrenv3S; // 0x04214: Background layer 3 start env
    DR_ENV BgDrenv4S; // 0x04254: Background layer 4 start env
    DR_ENV BgDrenv3E; // 0x04294: Background layer 3 end env
    DR_ENV BgDrenv4E; // 0x042d4: Background layer 4 end env

    u8 unk4314[0x600]; // 0x04314: Unknown render data

    SPRT_16 Bg1[0x9c4]; // 0x04914: Background layer 1/2 sprites
    SPRT Bg2[0x200];    // 0x0e554: Background layer 3/4 sprites

    FieldBgAnimPair BgAnim[0xbc4]; // 0x10d54: Per-sprite animation gate
    DR_MODE BgDm[0x6a4];           // 0x124dc: Background draw mode packets

    OT_TYPE OtUi;       // 0x1748c: UI ordering table
    DR_MODE RainDm;     // 0x17490: Rain draw mode
    LINE_F2 Rain[0x40]; // 0x1749c: Rain line primitives
};
extern struct FieldRenderData g_FieldRenderData[2]; // double buffered

extern char g_FieldDebugDigits[16]; // '0' to 'F' for hex digits
extern s32 (*g_FieldOpcodes[256])(void);
extern char D_800E0628[];
extern s8 g_FieldDebugWordLabel;
extern u16 g_FieldBGCameraHeightBias; // camera height bias applied to the
                                      // tracked entity
extern volatile u8
    g_FieldBGTrackedEntity; // entity the background scroll tracks
extern u32 g_FieldKeyState; // last raw pad state read this frame
extern u32
    g_FieldPad1State; // pad 1: state, previous, newly pressed, newly released
extern u8 g_FieldEntityBgTrigger[]; // per-entity background-trigger bits, one
                                    // byte each
extern u32 g_FieldPad1PrevState;
extern u32 g_FieldPad1Pressed;
extern u32 g_FieldPad1Released;
extern u32 g_FieldPad2State; // pad 2: same four
extern u32 g_FieldPad2PrevState;
extern u32 g_FieldPad2Pressed;
extern u32 g_FieldPad2Released;
extern s32 func_8001C808(void);
extern s16 D_800E0748[];
extern s16 g_FieldDebugPageY[];
extern s16 D_800E074C[];
extern s16 g_FieldDebugPageH[];
extern u8 g_FieldDebugPageR[];
extern u8 g_FieldDebugPageG[];
extern u8 g_FieldDebugPageB[];
extern s16 g_FieldDebugPageHeadRow[];
extern s16 g_FieldDebugPageRow[];
extern char g_FieldDebugRowText[];
extern u8 D_800E08A8[];
extern u8 g_FieldDebugPageHidden[];
extern u8 D_800E1028[];
extern u8 D_800E3B28[];
extern u8 D_800E3FA8[];
extern u8 D_800E4128[];
extern u16 D_800E4200[];
extern u8 g_FieldMimPalData[];
extern u8 g_FieldMimPalSize[];
extern u8 g_FieldMimPalX[];
extern u8 g_FieldMimPalY[];
extern u8 g_FieldMimPalW[];
extern u8 g_FieldMimPalH[];
extern u8 g_FieldMimTex0Data[];
extern u8 g_FieldMimTex0Size[];
extern u8 g_FieldMimTex0X[];
extern u8 g_FieldMimTex0Y[];
extern u8 g_FieldMimTex0Rect[];
extern u8 g_FieldMimTex0Tpage[];
extern u8 g_FieldMimTex1Data[];
extern u8 g_FieldMimTex1Size[];
extern u8 g_FieldMimTex1X[];
extern u8 g_FieldMimTex1Y[];
extern u8 g_FieldMimTex1Rect[];
extern u8 g_FieldMimTex1Tpage[];
extern u8 D_800DFDFC[];
extern u8 g_FieldRandomIndex;
extern u8 g_EntityForSplitJoin;
extern s16 g_FieldDirVectors[][2];
extern s16 g_FieldDebugRb;
/* Double-buffered 7-entry ordering table for the debug overlay. Entry 6 is the
 * tail the overlay's primitives hang off, entry 0 the head linked into the
 * caller's OT. */
extern u_long D_800E41C8[2][7];
extern s16 g_FieldDebugRChars;
extern s16 g_FieldDebugRLines;
extern s16 g_FieldDebugRRect;
extern s16 g_FieldDebugRDm;
extern u16 g_FieldDebugTransp;
extern char g_DebugText[];          // debug text
extern char g_DebugMessageBuffer[]; // debug value transformed into text

extern u8 g_FieldScriptDebugEntities[];
extern u8 g_actorIdCur;
extern u8 g_RandomTableStep;
extern u8 g_RandomTableIndex;
extern u8 g_RandomTable[256];
extern u8 g_DialogDigitCharacters[16];
extern s16 g_FieldRainDrops[0x40][12];
extern u8 g_WindowReplaceBank[4][8];
extern u16 g_WindowReplaceBankAddr[4][8];
extern s16 g_WindowWaitTime[4];
extern u8* g_WindowStringPtr[4];
extern u8 g_WindowString[4][256];
extern s16 g_WindowNameCopyCount[4];
extern s16 g_WindowReplaceParam[4];
extern s16 g_WindowExtraRows[4];
extern s16 g_WindowTextBudget[4];
extern s16 g_WindowFastForwardLevel[4];
extern s16 g_WindowBufferPos[4];
extern u8 g_WindowBuffer[4][16];
extern s16 g_WindowTotalRowsHeight[4];

/* volatile: the movie stream sets this from an interrupt callback, and it is
 * what keeps the s16 conversion in FieldUpdateMovieStream a separate
 * sign-extension instead of folding into a signed load. */
extern volatile u16 g_FieldMovieStreamActive;
extern u8 g_FieldExitArrowState[];
extern u8 g_FieldMateriaAddResult;
extern u8 g_FieldMovieLock;
extern u8
    g_FieldMovieOpcodeActive; // set while a movie opcode is driving playback
extern u32
    g_FieldModelBufferTop; // top of the buffer the movie stream decodes into
extern u16 g_FieldModelSavedMoveSpeed[]; // per-model default walk speed,
                                         // indexed by model id
extern s16 g_FieldMovieStreamDone;
void func_80034FC8(u32 buffer, s16 movieId); // STR ring setup
void func_800354CC(void);                    // STR playback start
void func_80035658(void);                    // STR playback stop
extern s32 g_FieldAkaoArg4;
extern s32 g_FieldAkaoArg5;
extern s16 g_FieldMovieLockFrame;
extern u8 g_FieldPalettes[];
/* "evt cmd=" and "evt result=", written into field4.c's .rodata by
 * OpcodeFuncMjump and OpcodeFuncTutor and shared with OpcodeFuncMenu over in
 * field5.c. See the comment there. */
extern char D_800A0848[];
extern char D_800A08D0[];
extern s32 g_BattleCharIdToCharId[11];
extern u8 g_CharIdToEntity[];
extern FieldState g_FieldStateData;
extern u8 g_FieldRandListIndex;
extern u8 g_FieldRandListOffset;
extern s32 g_AkaoMusicActiveMask;
extern s32 g_AkaoChannelMask[];
extern u8 g_FieldCameraMatrixSel;
extern u8 g_FieldAnimLock;
extern u8 g_FieldAnimFreeze;
extern u8 D_80081DC4;
extern s16 g_FieldExitArrowX;
extern s16 g_FieldExitArrowY;
extern u8 g_FieldExitArrowPktIdx;
// Two POLY_FT4 at 0x800E48F4, filling the gap between g_EntityForSplitJoin
// (0x800E48F0) and g_WindowString (0x800E4944) exactly. Confirmed by the
// 0x28 stride, len 9 / code 0x2C at +3 / +7, clut at +0x0E and tpage at +0x16.
extern POLY_FT4 g_FieldExitArrowPkts[2];
s32 GetGraphType(void);
extern MATRIX** D_80083578;
extern MATRIX* g_DebugMatrixP;
extern s16 D_8009A162;
extern u8 D_8009A15C;

void SystemRefreshParty(void);
/* Handwritten assembly. The s16 colour params are load-bearing: they put the
 * truncation at the call site, which is what splits `sll`/`sra` across the
 * loop preheader and body in KawaiSetColorToModelPkts. */
void KawaiSetColorToPartPkts(u8* part, s16 r, s16 g, s16 b);
void func_80025648(u32 materia, u8 slot);
void FieldDialogSetWindowStyleCbc(s16 window, u8 style, s16 preventClose);
void FieldDialogSetWindowHeight(s16 window, s16 height);
void FieldDebugPageSetPosSize(s16 page, s16 x, s16 y, s16 w, s16 h);
void FieldDebugPageResetStrings(s16 page);
void FieldDialogMove(s16 window, s16 dx, s16 dy);
u16 func_80025310(u16 itemId);
s32 OpcodeFuncWsize(void);
s32 FieldWindowSetStateToClose(s16 window);
s32 FieldDialogMessageUpdateStates(u8 window, u8 message);
void func_80025288(u16 itemId);
void func_80025380(u16 itemId);
s32 func_8002542C(u32 materia);
u8 func_80025650(u32 materia, u8 slot);
void SystemMenuAddHpByPartyId(s32 partyId, s32 amount);
void SystemMenuAddMpByPartyId(s32 partyId, s32 amount);
void func_80025800(s32 partyId, s32 amount);
void func_80025988(s32 partyId, s32 amount);
s32 FieldEventSetDirByActorId(s16 actorId);
s32 FieldMoveToEntityUpdate(s32 targetEntityId);
s32 FieldEntityTurnToEntity(s16 actorId);
void func_80020058(s16 partyId);
void func_8001786C(s16 partyId);
void func_80017678(void);

typedef struct {
    /* 0x00 */ LinePos pos;
    /* 0x0C */ s16 destPosX;
    /* 0x0E */ s16 destPosY;
    /* 0x10 */ u16 destWalkMeshId;
    /* 0x12 */ u16 destFieldId; // 0x7FFF marks an unused gateway slot
    /* 0x14 */ u8 destDirection;
    /* 0x15 */ u8 pad[3];
} FieldGateway; // size:0x18

typedef struct {
    /* 0x00 */ u16 control; /* bit 0 enables encounters, high byte is the step
                               divisor */
    /* 0x02 */ u16 encounters[5];
    /* 0x0C */ u16 fallback;
    /* 0x0E */ u16 special[4];
    /* 0x16 */ u16 pad;
} FieldEncounterTable; /* size:0x18 */

typedef struct {
    /* 0x00 */ u8 unk0;
    /* 0x01 */ u8 unk1;
    /* 0x02 */ u16 count;
    /* 0x04 */ FieldModelLoaderData models[0]; // variable length
} FieldModelFileDesc;

typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ u8 u;
    /* 0x05 */ u8 v;
    /* 0x06 */ u16 clut;
} FieldBgTile1; // size:0x8

typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ u8 u;
    /* 0x05 */ u8 v;
    /* 0x06 */ u16 clut;
    /* 0x08 */ u16 tpage;
    /* 0x0A */ u16 rg; // red in the low byte, green in the high one
    /* 0x0C */ u8 flags;
    /* 0x0D */ u8 param;
} FieldBgTile2; // size:0xE

typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ u8 u;
    /* 0x05 */ u8 v;
    /* 0x06 */ u16 clut;
    /* 0x08 */ u8 flags;
    /* 0x09 */ u8 param;
} FieldBgTile3; // size:0xA

typedef struct {
    /* 0x00 */ u32 layer1Offset;
    /* 0x04 */ u32 tpageOffset;
    /* 0x08 */ u32 layer2Offset;
    /* 0x0C */ u32 layer34Offset;
    /* 0x10 */ s16 runs[0]; // variable length
} FieldBgData;

typedef struct {
    /* 0x0 */ u32 magic;
    /* 0x4 */ u16 numPages;   // 0x200-byte texture pages
    /* 0x6 */ u16 numCluts;   // 0x20-byte CLUTs
    /* 0x8 */ u32 pageOffset; // offset of the pages within the block
    /* 0xC */ u32 clutOffset; // offset of the CLUTs within the block
} FieldTexBlockHeader;

typedef struct {
    u32 datSector; // +0x00
    u32 datSize;   // +0x04
    u32 mimSector; // +0x08
    u32 mimSize;   // +0x0C
    u32 bsxSector; // +0x10
    u32 bsxSize;   // +0x14
} FieldFileInfo;

typedef struct {
    /* 0x00 */ u16 triCount;
    /* 0x02 */ u16 unk2;
    /* 0x04 */ s16 tris[1];
} FieldWalkmesh;

typedef struct {
    /* 0x00 */ u8 unk00[0xA];
    /* 0x0A */ u16 camHeightBias;
} FieldTriggerHeader;

typedef struct {
    u16 v;
} FieldU16Slot;

typedef struct {
    /* 0x00 */ u8 unk00[0x18];
    /* 0x18 */ u16 wrapX3;
    /* 0x1A */ u16 wrapY3;
    /* 0x1C */ u16 wrapX4;
    /* 0x1E */ u16 wrapY4;
} FieldBgWrap;

typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
} FieldBgCamera;

typedef struct {
    /* 0x00 */ u16 layer4;
    /* 0x02 */ u16 layer3;
} FieldBgOtSlot;

typedef struct {
    /* 0x00 */ u8 unk00[0xC];
    /* 0x0C */ s16 minX;
    /* 0x0E */ s16 minY;
    /* 0x10 */ s16 maxX;
    /* 0x12 */ s16 maxY;
    /* 0x14 */ u8 scrollType;
} FieldScrollLimits;

typedef struct {
    /* 0x00 */ u8 unk00[0x18];
    /* 0x18 */ s16 wrapTilesX1;
    /* 0x1A */ s16 wrapTilesY1;
    /* 0x1C */ s16 wrapTilesX2;
    /* 0x1E */ s16 wrapTilesY2;
    /* 0x20 */ s16 scrollX1;
    /* 0x22 */ s16 scrollY1;
    /* 0x24 */ s16 scrollX2;
    /* 0x26 */ s16 scrollY2;
    /* 0x28 */ s16 parallaxX1;
    /* 0x2A */ s16 parallaxY1;
    /* 0x2C */ s16 parallaxX2;
    /* 0x2E */ s16 parallaxY2;
} FieldBgScroll;

typedef struct {
    /* 0x00 */ MATRIX view;
    /* 0x20 */ s16 centerOfsX;
    /* 0x22 */ s16 centerOfsY;
    /* 0x24 */ s16 screenDist;
} FieldCamera;

typedef struct {
    /* 0x0 */ u16 vx;
    /* 0x2 */ u16 vy;
} FieldScreenPos;

typedef struct {
    /* 0x00 */ LinePos pos;
    /* 0x0C */ u8 entityId;
    /* 0x0D */ u8 bgState;
    /* 0x0E */ u8 type;
    /* 0x0F */ u8 soundId;
} FieldBgTrigger;

typedef struct {
    /* 0x0 */ u8 mouth1;
    /* 0x1 */ u8 mouth2;
    /* 0x2 */ u8 eye;
    /* 0x3 */ u8 slot;
} KawaiFaceSel;

typedef struct {
    /* 0x0 */ u16 w;
    /* 0x2 */ u16 h;
    /* 0x4 */ u16 x;
    /* 0x6 */ u16 y;
    /* 0x8 */ u32 dataOffset;
} BsxTexEntry; // size:0xC

typedef struct {
    /* 0x0 */ u32 unk0;
    /* 0x4 */ u8 texCount;
    /* 0x5 */ u8 tdbOffsetHi;  // 24-bit offset of the TDB chunk, big-endian:
    /* 0x6 */ u16 tdbOffsetLo; // (hi << 16) | lo, zero when there is none
    /* 0x8 */ BsxTexEntry entries[1];
} BsxTexHeader;

typedef struct {
    /* 0x00 */ u16 unk0;
    /* 0x02 */ u16 scale;
    /* 0x04 */ u32 dataOffset; // bone/part/anim data, relative to this record
    /* 0x08 */ u8 light0[3];
    /* 0x0B */ u8 unkB;
    /* 0x0C */ u16 light0Dir[3];
    /* 0x12 */ s8 boneIndex;
    /* 0x13 */ u8 unk13;
    /* 0x14 */ u8 light1[3];
    /* 0x17 */ u8 boneCount;
    /* 0x18 */ u16 light1Dir[3];
    /* 0x1E */ s8 partIndex;
    /* 0x1F */ u8 unk1F;
    /* 0x20 */ u8 light2[3];
    /* 0x23 */ u8 partCount;
    /* 0x24 */ u16 light2Dir[3];
    /* 0x2A */ s8 animIndex;
    /* 0x2B */ u8 unk2B;
    /* 0x2C */ u8 ambient[3];
    /* 0x2F */ u8 animCount;
} BsxModelRecord; // size:0x30

typedef struct {
    /* 0x00 */ u32 unk0;
    /* 0x04 */ u32 modelCount;
    /* 0x08 */ u32 texOffset;
    /* 0x0C */ u32 recordsOffset;
    /* 0x10 */ BsxModelRecord models[1];
} BsxModelBlock;

typedef struct {
    /* 0x00 */ u8 unk0[4];
    /* 0x04 */ u8 gt4Count;
    /* 0x05 */ u8 gt3Count;
    /* 0x06 */ u8 ft4Count;
    /* 0x07 */ u8 ft3Count;
    /* 0x08 */ u8 f3Count;
    /* 0x09 */ u8 f4Count;
    /* 0x0A */ u8 g3Count;
    /* 0x0B */ u8 g4Count;
    /* 0x0C */ u8 unkC[2];
    /* 0x0E */ u16 polyOffset;     // the interleaved polygon table
    /* 0x10 */ u16 texCoordOffset; // u16 pairs, one per vertex
    /* 0x12 */ u16 texInfoOffset;  // u32 per texture slot: clut, tpage, mode
    /* 0x14 */ u16 texIndexOffset; // one byte per textured primitive
    /* 0x16 */ u16 pktSize;        // bytes of packets one pass emits
    /* 0x18 */ u8* data;
    /* 0x1C */ u8* pkts;
} FieldModelPart; // size:0x20

typedef struct {
    /* 0x00 */ u32 opcode; // 0=memcpy, 1=page patch, 2=CLUT patch, 3=LoadImage
    /* 0x04 */ u32 srcOff; // source pixels, relative to the tdb chunk
    /* 0x08 */ u32 size;   // memcpy byte count (op 0)
    /* 0x0C */ u32 dst;    // absolute dest (0) / page idx (1) / CLUT idx (2) /
                           // with dst2, the RECT LoadImage takes (op 3)
    /* 0x10 */ u32 dst2;
} TdbRecord; // size 0x14

typedef struct {
    /* 0x00 */ s16 curR;
    /* 0x02 */ s16 curG;
    /* 0x04 */ s16 curB;
    /* 0x06 */ s16 targetR;
    /* 0x08 */ s16 targetG;
    /* 0x0A */ s16 targetB;
    /* 0x0C */ s16 deltaR;
    /* 0x0E */ s16 deltaG;
    /* 0x10 */ s16 deltaB;
    /* 0x12 */ u8 unk12;
    /* 0x13 */ u8 done;
    /* 0x14 */ u8 unused[0x28];
} KawaiColorFadeSlot;

typedef struct {
    /* 0x00 */ u16 cur0;
    /* 0x02 */ u16 cur1;
    /* 0x04 */ u16 cur2;
    /* 0x06 */ u16 cur3;
    /* 0x08 */ s16 target0;
    /* 0x0A */ s16 target1;
    /* 0x0C */ s16 target2;
    /* 0x0E */ s16 target3;
    /* 0x10 */ s16 delta0;
    /* 0x12 */ s16 delta1;
    /* 0x14 */ s16 delta2;
    /* 0x16 */ s16 delta3;
    /* 0x18 */ u8 unk18;
    /* 0x19 */ u8 done;
} KawaiFadeBelowLvlSlot;

typedef struct {
    s16 unk00[6];
    s16 headRow;
    s16 unk0E;
} FieldDebugPageHdr;

/* Shared by the field translation units -- field.c, field_event.c,
 * field_opcodes.c and field2.c. They were one translation unit before the
 * split, so this header is largely field.c's old preamble. See the `field`
 * segment in config/us.yaml for the unit boundaries. */

#include <game.h>
#include <libetc.h>

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

/* The two bytes FieldBackgroundInitPackets copies out of each layer 2/3/4
 * tile. A sprite is linked into the OT only when the named entity's
 * background-trigger byte still has this bit set. */

void AddBackgroundToRender(struct FieldRenderData* buf);
/* Both were `static` before field.c was cut on jump-table alignment: the
 * cut runs between each one and half of its callers. */
s32 FieldCalcWorldToScreenPos(SVECTOR* worldPos, long* screenPos);
void FieldEntityLineClear(FieldLine* lines);
s32 FieldEntitySqrDistToLine(FieldLine* line, s32* point, s32* nearest);
void FieldEntityLineInteract(FieldEntity* entity, FieldLine* line);
void HandleKawaiDataInModel(struct FieldRenderData* buf);
void FieldEventOpcodeCycle(void);
void FieldUpdateAnimationState(void);
u8 FieldEventRequestRun(s16 entityId, s16 priority, s16 scriptId);
void DebugPrintOpcode(const char* name, s32 arg1);

s32 FieldDialogAskUpdateStates(
    u8 window, u8 message, u8 first, u8 last, s16* selectedLine);

u32 IfCheck(void);
u32 If2CheckSigned(void);
u32 If2CheckUnsigned(void);
s32 FieldEventRequest(s16 type, u8 target, u8 priority, u8 scriptId);
void FieldWindowReset(s16 window);
void FieldWindowResetTextAll(void);
s32 AddStrNextDebugRow(s16 page, const char* str);
s32 SetStrToDebugRow(s16 page, s16 row, const char* str);
void DebugUpdateActor(s16 arg0, s16 entityId);
s32 FieldDialogWindowInit(s16 window, s16 stringId);
void FieldDialogWindowGrowth(s16 window);
void FieldDialogCopyTextFromField(s16 window);
void DialogScrollText(s16 window);
void DialogScrollTextDuringOk(s16 window);
void FieldDialogWindowInitNext(s16 window);
s32 FieldDialogWindowDecrease(s16 window);
u16 FieldDialogGetVariableFromBank(s16 window);
void ConvertDigitToString(u16 value, u8* dst);
void ConvertNumToStrWithSpace(u16 value, u8* dst);
void ConvertHexToString(u16 value, u8* dst);

/* Were `static` before the split: each is now called from a unit other
 * than the one defining it, several of them from INCLUDE_ASM bodies. */

void DebugPrintToFieldWindow(const char* str);
void FieldDebugStringConcat(char* dest, char* src);
/* Defined in field4.c; the debug tracer in field5.c calls it across the split.
 */
void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind);
void FieldDebugStringCopy(char* dst, const char* src);
void FieldDebugStringU16hex(s32 val, char* msg_out);
void FieldDebugStringU32hex(s32 val, char* msg_out);
void FieldDebugStringU8hex(s32 val, char* msg_out);
void FieldEventDebugError(const char* errmsg);
void InitFieldDebugPages(void);

/* Declared in a unit body before the split. */
extern s16 D_80071A5C;
extern s16 D_8009AC1C;
extern s16 g_FieldRainDrops[0x40][12];
extern s16 g_CurrentFieldIndex;
extern s16 g_PlayerModelId;
extern s32 D_8007E770;
/* One field map's encounter table: a control word, five ordinary formations
 * and four "special" ones. Each formation word packs a cumulative six-bit
 * rate in its top bits and the formation id in the low ten. Two of these sit
 * back to back and BTLTB switches between them, which is why the second is
 * reached as +0x18. */

extern s32 g_FieldEncounters;
extern s32 g_FieldTriggers;
extern s32 g_WmPreSector;
extern s32* g_FieldEncountersP;
extern s32* g_FieldModelsP;
extern s32* g_FieldTriggersP;
extern volatile u16 g_FieldMoviePlayed;
extern u16 g_FieldPreloadMapId;
extern u32 g_FieldLzsInfo[];
extern u32 g_WmPreSize;
extern u8 g_FieldMusicLock;
extern u8 D_8009ABF5;
extern u8 g_FieldAnimLock;
extern u8 g_RainControl;
extern u8 g_RainForce;
extern u8 g_RandomTable[];

/* Header of the field's model-loader file (FieldModelStructInit parses it,
 * FieldEnablePartyModels reads its count). Shared: defined in field2.c,
 * used from field4.c, and the units must all see it before first use --
 * pycparser cannot prune a permuter scratch past a cast to an unknown type,
 * which is what silently leaves the whole unit in base.c. */

/* The field background's tile tables. One header sits at the head of the
 * loaded background block; each layer's tiles are a flat array behind one of
 * its four byte offsets, and the run list at +0x10 says how many consecutive
 * tiles share a texture page. A run is three s16 -- tag, sprite index, count
 * -- except the 0x7FFF terminator, which is a bare tag. */

/* Header of the shared field-model texture block at *D_800DFCA0. */

/* The block itself is loaded to 0x80128000; the pointer is set by
 * FieldModelLoadAndInit and read from both field2.c and field4.c, so the type
 * has to live here -- an extern whose type is declared in one unit becomes a
 * tentative definition in the other and only fails at link time. */
extern FieldTexBlockHeader* D_800DFCA0;

#define GET_FIELD_SCRIPT_PC(                                                   \
    dst, scriptOffset, entityDataOffset, extrasHeaderSize)                     \
    (dst) = *((u8*)((scriptOffset) + (entityDataOffset) + (extrasHeaderSize) + \
                    (s32)g_FieldScripts) +                                     \
              sizeof(FieldScriptHeader));                                      \
    (dst) |=                                                                   \
        *((u8*)((scriptOffset) + ((entityDataOffset) + (s32)g_FieldScripts) +  \
                (extrasHeaderSize)) +                                          \
          sizeof(FieldScriptHeader) + 1)                                       \
        << 8

/* File-scope declarations carried over with the decompiled bodies. They
 * live here rather than in one unit because several are used from more
 * than one of the four field translation units. */

extern volatile s16 D_8009AC1A[1];
extern volatile s16 D_8009AC1E;
extern u16 D_8009AC40[1];
extern volatile u32 D_8009AC3C[1];
extern void func_800128B8(void);
extern void func_800129D0(void);
void DebugRunEveryLoop(void);
void FieldEventInit(
    FieldState* state, FieldEntity* models, FieldScriptHeader* scripts);
void FieldEntityBgTriggerInit(FieldBgTrigger* triggers);
void FieldEnablePartyModels(void);

void FieldArrowsInit(SPRT_16* sprt, DR_MODE* dm);
void FieldLoadMimToVram(s32 arg0, u8* mim);
/* The walk mesh: a u16 triangle count, then that many 24-byte triangles. */

struct FieldRain {
    /* 0x00 */ SVECTOR p1;
    /* 0x08 */ SVECTOR p2;
    /* 0x10 */ s16 wait;
    /* 0x12 */ s16 rndSeed;
    /* 0x14 */ s16 z;
    /* 0x16 */ s16 render;
};

/* One record of a TDB ("texture delta") chunk inside a BSX model file. */

/////////////////////////////////////////////////
// Begin of field_main.c
/////////////////////////////////////////////////

extern FieldFileInfo g_FieldFileInfo[];
extern s32* g_FieldModelsP;
extern s32 g_FieldTriggers;
extern s32 g_FieldEncounters;
extern s32 D_8007E770;
extern s16 g_CurrentFieldIndex;
extern s32* g_FieldTriggersP;
extern s32* g_FieldEncountersP;
extern u32 g_FieldLzsInfo[];

extern u32 g_FieldFileTable[];
extern volatile u16 g_FieldMoviePlayed;
extern u16 g_FieldPreloadMapId;
extern s32 g_WmPreSector;
extern u32 g_WmPreSize;

// External Declarations
extern u8 D_8009ABF5;
extern u8 g_FieldAnimLock;
extern s16 D_80071A5C;

// D_8009ABF5 = g_FieldState -> command

extern DISPENV g_FieldDispEnv[2];
extern DRAWENV g_FieldDrawEnv[2];
extern DRAWENV g_FieldDrawEnvBg[2];
extern DRAWENV D_80113FE4[2];
extern DRAWENV D_8011409C[2];
extern DRAWENV D_80114154[2];
extern DRAWENV D_8011420C[2];
extern s8 D_800716D0;
extern u16 D_8007173C;
extern u8 D_80071A58;
extern s16 D_8007E768;
extern s32* D_8007EB64;
extern u8 D_8007EBC8;
extern u8 D_8009C6D8;
extern s8 D_8009A057;
extern s16 D_8009A100;
extern volatile u16 D_8009AC18;

s32 FieldMainLoop(void);

extern FieldLine D_8007E7AC;
extern volatile s16 g_FieldNextModule;
extern FieldWalkmesh** D_8009A044;
extern FieldBgData** D_8009D848;
extern s16 D_80071E38;
extern s16 D_80071E3C;

extern SVECTOR (*D_800E4274)[3];
extern s16* D_80114458;
extern s32 g_FieldMovieDrawBg;
extern volatile s32 g_FieldMovieVSyncMode;
extern u8 g_FieldLineCheckResult;
extern MATRIX* D_80071E40;
extern u32 g_FieldOTHead[2];
extern s32 g_FieldVSyncBeforeDraw;
extern s32 g_FieldVSyncAfterDraw;
extern s32 g_FieldScreenCenterX;
extern s32 g_FieldScreenCenterY;
extern DISPENV* g_FieldCurDispEnv;
extern DRAWENV* g_FieldCurDrawEnv;

extern FieldBgTile3* D_8007EBD4;
extern s16 D_8011448C;
extern s16 D_801144C8;
extern s16 D_801144D0;

extern FieldBgCamera D_80071A48[3];

extern FieldBgOtSlot D_8009ACA2;

#ifndef NON_MATCHINGS
#else
#endif

extern s16 g_CameraScrollSteps;  // total steps of the active scroll
extern s16 g_CameraScrollStep;   // current step, 0..g_CameraScrollSteps
extern s16 g_CameraScrollStartX; // where the scroll began
extern s16 g_CameraScrollStartY; // where the scroll began
extern s16 g_CameraScrollEndX;   // copied from cameraScrollTargetX
extern s16 g_CameraScrollEndY;   // copied from cameraScrollTargetY

#define FIELD_SCROLL_LIMITS ((FieldScrollLimits*)g_FieldTriggers)

/////////////////////////////////////////////////
// Begin of field_entity.c
/////////////////////////////////////////////////

void FieldBattleCheck(void);
extern s32 FieldEntityMove(s16 entityId);
extern /*?*/ s32 D_80074EBC;
extern /*?*/ s32 D_80074EC0;
extern /*?*/ s32 D_80074EC4;
extern /*?*/ s32 D_80074ED0;
extern /*?*/ s32 D_80074ED4;
extern /*?*/ s32 D_80074ED6;
extern /*?*/ s32 D_80074ED9;
extern /*?*/ s32 D_80074EDA;
extern /*?*/ s32 D_80074EDB;
extern /*?*/ s32 D_80074EDC;
extern /*?*/ s32 D_80074EDD;
extern /*?*/ s32 D_80074EDE;
extern /*?*/ s32 D_80074EDF;
extern /*?*/ s32 D_80074EE0;
extern /*?*/ s32 D_80074EE2;
extern /*?*/ s32 D_80074EE6;
extern /*?*/ s32 D_80074EE8;
extern /*?*/ s32 D_80074EEC;
extern /*?*/ s32 D_80074EEE;
extern /*?*/ s32 D_80074EF2;
extern /*?*/ s32 D_80074EF4;
extern /*?*/ s32 D_80074EF6;
extern /*?*/ s32 D_80074EF8;
extern /*?*/ s32 D_80074EFA;
extern /*?*/ s32 D_80074F00;
extern /*?*/ s32 D_80074F01;
extern /*?*/ s32 D_80074F02;
extern /*?*/ s32 D_80074F04;
extern /*?*/ s32 D_80074F06;
extern /*?*/ s32 D_80074F08;
extern /*?*/ s32 D_80074F0C;
extern /*?*/ s32 D_80074F0E;
extern /*?*/ s32 D_80074F14;
extern /*?*/ s32 D_80074F16;
extern /*?*/ s32 D_80074F18;
extern /*?*/ s32 D_80074F1C;
extern /*?*/ s32 D_80074F20;
extern /*?*/ s32 D_80074F24;

u8 FieldEntityDirByVec(VECTOR* from, VECTOR* to, s32* sqrDist);

extern u8 D_8009ACA6[]; // per-triangle edge access bits, one bit per link
extern u16 D_80113F28;
extern s16 D_801144CC;

u8 FieldEntityLineCheck(FieldEntity*, FieldLine*, VECTOR*); // extern
void FieldEntityTriggerCheck(
    FieldEntity* entity, FieldBgTrigger* trigger, VECTOR* dest);
extern s32 FieldEntityWalkmechCross(
    u16* triId, VECTOR* pos, VECTOR* delta, VECTOR* outEdge);
extern void OuterProduct0(VECTOR* v0, VECTOR* v1, VECTOR* out);
extern s32 VectorNormal(VECTOR* v0, VECTOR* out);
extern /*?*/ s32 D_80074F10;
extern s16 D_8009AC22;
extern s16 D_8009AC24;
extern u8 D_8009AC2A;

extern s16 D_8009AC1C;

s16 FieldEntityBgTriggerActivate(FieldBgTrigger* trigger, u8 type);

/////////////////////////////////////////////////
// Begin of field_camera.c
/////////////////////////////////////////////////

extern u8 g_FieldGlobalModelFiles[];
extern u8 g_FieldGlobalTexFile[];
extern u8* D_800E0204;

extern u32 g_FieldFileSectors[];

void* FieldModelStructInit(FieldModelFileDesc* desc, FieldModelData* data);
u8* FieldModelLoadGlobalModels(
    FieldModelFileDesc* desc, FieldModelData* data, u8* pkts, s32 readFile);
u8* LoadLocalFieldModelAndInitAll(
    FieldModelFileDesc* desc, FieldModelData* data, u8* readFromCd, u32* buf);
s32 KawaiLoadEyesMouthTexToVram(FieldModelEntry* model, u8* faceSel);
void KawaiClearData(void);

void FieldModelAnimCalcMtrxs(
    FieldModelEntry* model, MATRIX* mtx, u8 animId, s32 frame);
void FieldModelPrepareRender(FieldModelEntry* model);
s32 KawaiExecute(FieldModelEntry* model, u8* kawaiData, u8 index, MATRIX* mtx);
s32 KawaiLoadEyesMouthTexToVram(FieldModelEntry* model, u8* faceSel);

extern u8 D_800DF114;
extern struct FieldRenderData* D_800DF118;
extern u8 D_801144D8; // blink RNG cursor

// Possable Debug routine. Ran at beginning of every main field loop. (FPS?)

/////////////////////////////////////////////////
// Begin of field_rain.c
/////////////////////////////////////////////////

extern struct FieldRain g_FieldRain[64];
extern u8 g_RainForce;
extern s16 g_FieldRainDrops[0x40][12];

extern u8 g_RainControl;
extern s16 g_PlayerModelId;

extern u8 g_RandomTable[];

/////////////////////////////////////////////////
// Begin of field_battle.c
/////////////////////////////////////////////////

extern s16 g_FieldLastBattleFormationId;

extern s16 D_8009ABF6; /* g_FieldStateData.eventCmdParam */
extern u8 D_8009AC30;  /* g_FieldStateData.encounterTableId */

/////////////////////////////////////////////////
// Begin of field_arrow.c
/////////////////////////////////////////////////

extern u16 g_FieldArrowAnimTick;

/////////////////////////////////////////////////
// Begin of field_model.c
/////////////////////////////////////////////////

extern u8* D_800E0200;

void FieldModelBsxTdbModify(u8* tdb);
void FieldModelLoadBsxTexToVram(BsxTexHeader* bsx);
u8* FieldModelCreatePktsAndScale(FieldModelEntry* model, u8* pkts, s32 arg2);
void KawaiLightingApplyToModel(FieldModelEntry* model, u8* light);
s32 KawaiSetColorToModelPkts(FieldModelEntry* model, u8* data);

extern u8* FieldModelCreatePktsForPart(
    FieldModelPart* part, u8* pkts, s32 reset, s32 texY);
extern void FieldModelScaleModel(FieldModelEntry* model, s16 scale, s32 arg2);

u8* FieldModelLoadBcx(
    FieldModelFileDesc* desc, FieldModelData* data, u8* pkts, s32 index);

/////////////////////////////////////////////////
// Begin of field_kawai_char_model.c
/////////////////////////////////////////////////

extern SVECTOR D_800DF520[]; // light normals, indexed by a colour's code byte

extern u8 g_KawaiMouthTexPages[][7];
extern u8
    g_KawaiEyeTexPages[][3]; /* eye texture page index, per face, per frame */

extern u8 g_KawaiFadeScratch[]; /* scratch RGB quad, 0x20 before the table */
extern KawaiColorFadeSlot g_KawaiColorFadeSlots[16];

#define KawaiFadeSlots ((KawaiColorFadeSlot*)(g_KawaiFadeScratch + 0x20))

/////////////////////////////////////////////////
// Begin of field_event.c
/////////////////////////////////////////////////

extern u8 g_FieldMusicLock;
void FieldWindowResetAll(void);
void FieldInitDefaultValues(void);
void FieldEventRunInit(void);

extern s16 D_800716DC[];    // per-entity
extern s16 D_80071748[][8]; // per-entity, one halfword per script bank
extern u8 D_80071A88[][8];  // per-entity, one byte per script bank
extern s8 D_80075F23;       // top of a 0x100-byte block cleared downward
extern u8 D_8007EB98[];     // per-entity, reset to 0xFF
extern u8 D_80081D90[];     // per-entity
extern u8 D_800833F8[][8];  // per-entity, one byte per script bank
extern s16 D_80095D84;
extern u8 D_8009A1C4[]; // per-entity, reset to 7
extern u8 D_8009AD38;   // top of a 9-byte block set to 0xFF downward

// Inline as empty string when more is decompiled. Checksum fails now.

void DrawFieldExitArrow(s32* ot);

/////////////////////////////////////////////////
// Begin of field_event_debug.c
/////////////////////////////////////////////////

void FieldDebugPageSetColor(s16 page, u8 r, u8 g, u8 b);
s32 SetDebugStrRowColor(s16 page, s16 row, u8 color);
extern u8 D_800716C8;
extern u16 D_80075E12;
extern u8 D_8009CBDD;
extern u8 D_8009CBDE;
extern u8 D_8009D289;
extern u8 D_8009D29B;
extern u8 D_8009D392;
extern u8 D_8009D393;
extern u16 D_8009D78A;

/////////////////////////////////////////////////
// Begin of field_event_memory_bank.c
/////////////////////////////////////////////////

//////////////////////////////////////////////////
// Start of field_opcode_system.c
/////////////////////////////////////////////////

// This is called when there the script tries to execute an invalid opcode
// called for opcodes:
// 0C 0D 1A 1B 1C 1D 1E 1F 44 46 4C 4E BE

//////////////////////////////////////////////////
// Start of field_opcode_vars.c
/////////////////////////////////////////////////

//////////////////////////////////////////////////
// Start of field_opcode_line.c
/////////////////////////////////////////////////

//////////////////////////////////////////////////
// Start of field_opcode_if.c
/////////////////////////////////////////////////

//////////////////////////////////////////////////
// Start of field_opcode_controller.c
/////////////////////////////////////////////////

//////////////////////////////////////////////////
// Start of field_opcode_request.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Start of field_opcode_akao_sound.c
/////////////////////////////////////////////////

// In Akao because it uses the AKAO block area

extern u8* g_FieldTutorialAkaoBlock;

/////////////////////////////////////////////////
// Start of field_opcode_movie_overlay.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Start of field_opcode_char_control.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Start of field_opcode_model_animate.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Start of field_opcode_model_move.c
/////////////////////////////////////////////////

extern u8 D_800722C4;
extern /*?*/ s32 D_800831FC;
extern u8 D_8009D820;

/* SLDR2: SLIDR with a 16-bit radius. */

/* TALKR: set this entity's talk radius, scaled the same way as SLIDR. */

/* TLKR2: TALKR with a 16-bit radius. */

/////////////////////////////////////////////////
// Start of field_opcode_model_state.c
/////////////////////////////////////////////////

/* MSPED: set this entity's movement speed, scaled like the radius opcodes. */

/* GTDIR: write another entity's facing direction back into a memory bank. */

/////////////////////////////////////////////////
// Start of field_opcode_message.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Start of field_opcode_window.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_math.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_background.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_movie.c
////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_scroll.c
////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_palette.c
////////////////////////////////////////////////

// Partial replace of bank 2 party with newParty.
// Any free slots in newParty are taken up by members of bank 2 party.
// The result is then transferred to main party in save.

// Compares two sets of parties and returns which members don't exist in both.

// Transfers party from bank 2 to save while preserving order in save of
// characters existing in both parties.

// Transfers party from save to bank 2 while preserving order in bank 2 of
// characters existing in both parties.

// Adds characters from toAdd to the first free slots in party.
// Does not use force freed slots.

/////////////////////////////////////////////////
// Begin of field_opcode_party_manage.c
/////////////////////////////////////////////////

s32 FieldEventSplitSet(s16, s16, s16, s16, s16); // extern

/////////////////////////////////////////////////
// Begin of field_opcode_fade.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_intersect.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_window_color.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_field_effect.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_items.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_special.c
/////////////////////////////////////////////////

extern u8 g_FieldMessageSpeed[1];
void func_80033A90(void);
void SystemMessageSetCharName(s16 battleCharId, s16 stringId);

/////////////////////////////////////////////////
// Begin of field_opcode_layer.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_materia.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_menu.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_angle.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_party_stats.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_window_timer.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_battle.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_opcode_kawai_char.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_dialog.c
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Begin of field_debug.c
/////////////////////////////////////////////////

/* Same shape as FieldDebugPageAddPos above; see the note there. */

void FieldDebugRenderPage(s16 page);

void FieldDebugRenderString(s16 page, s16 row, u8* str, s32 x, s32 y);

extern u8 D_800E1036[];

extern u8 g_FieldAtanTable[];

extern s16 D_800DF120[][2];

#endif /* FIELD_PRIVATE_H */
