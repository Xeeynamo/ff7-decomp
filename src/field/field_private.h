#ifndef FIELD_PRIVATE_H
#define FIELD_PRIVATE_H
#include <game.h>

#define GET_ENTITY_NAME(entityId) ((char*)g_FieldScripts + sizeof(FieldScriptHeader) + (entityId) * 8)

#define GET_FIELD_SCRIPT_OFFSET(dst, scriptOffset, entityDataOffset, extrasHeaderSize)                                 \
    (dst) = *((u8*)((scriptOffset) + (entityDataOffset) + (extrasHeaderSize) + (u_long)g_FieldScripts) +               \
              sizeof(FieldScriptHeader));                                                                              \
    (dst) |= *((u8*)((scriptOffset) + ((entityDataOffset) + (u_long)g_FieldScripts) + (extrasHeaderSize)) +            \
               sizeof(FieldScriptHeader) + 1)                                                                          \
             << 8

typedef struct {
    u8 parameter;
    u8 state;
} FieldBgAnimEntry;

typedef struct {
    u32 layer1Offset;
    u32 texturePageOffset;
    u32 layer2Offset;
    u32 layer3Offset;
    s16 data[1];
} FieldTilemap;

typedef struct {
    OT_TYPE ot[4096];   // 0x00000: Main scene ordering table
    SPRT_16 arrows[24]; // 0x04000: Field arrow sprite packets
    DR_MODE arrowsDm;   // 0x04180: Arrow sprite draw mode

    OT_TYPE otFadeDrenv;  // 0x0418c: Fade draw environment OT entry
    OT_TYPE otSceneDrenv; // 0x04190: Scene draw environment OT entry

    DR_ENV fadeDrenv;  // 0x04194: Screen fade draw environment
    DR_ENV sceneDrenv; // 0x041d4: Main scene draw environment

    DR_ENV bgDrenv3S; // 0x04214: Background layer 3 start env
    DR_ENV bgDrenv4S; // 0x04254: Background layer 4 start env
    DR_ENV bgDrenv3E; // 0x04294: Background layer 3 end env
    DR_ENV bgDrenv4E; // 0x042d4: Background layer 4 end env

    u8 unk4314[1536]; // 0x04314: Unknown render data

    SPRT_16 bg1[2500]; // 0x04914: Background layer 1/2 sprites
    SPRT bg2[512];     // 0x0e554: Background layer 3/4 sprites

    FieldBgAnimEntry bgAnim[3012]; // 0x10d54: Background animation data
    DR_MODE bgDm[1700];            // 0x124dc: Background draw mode packets

    OT_TYPE otUi;     // 0x1748c: UI ordering table
    DR_MODE rainDm;   // 0x17490: Rain draw mode
    LINE_F2 rain[64]; // 0x1749c: Rain line primitives
} FieldRenderData;

// Staged in scratchpad at 0x1F800004; describes the LZS blob to stream off the CD.
typedef struct {
    /* 0x0 */ s32 sector;
    /* 0x4 */ s32 size;
} FieldModelLzsRequest;

typedef struct {
    /* 0x0 */ Yamada* globalModels;
    /* 0x4 */ FieldModelLzsRequest* faceTexture;
} FieldModelScratch;

#define FIELD_MODEL_SCRATCH ((FieldModelScratch*)getScratchAddr(0))

typedef struct {
    u32 size;
    u16 textureCount;
    u16 paletteCount;
    u32 texturesOffset;
    u32 palettesOffset;
} FieldFaceTextureHeader;

typedef struct {
    s16 enabled;
    s16 standard[6];
    s16 special[4];
    s16 pad;
} FieldEncounters;

typedef struct {
    s16 access1;
    s16 access2;
    s16 access3;
} FieldWalkmeshAccess; // size: 0x06

typedef struct {
    u16 triangleCount;
    u16 pad;
    u8 data[1];
    /*
    SVECTOR triangles[triangleCount][3];
    FieldWalkmeshAccess[triangleCount];
    */
} FieldWalkmesh; // data offset: 0x04

typedef struct {
    /* 0x00 */ FieldScriptHeader* scripts;
    /* 0x04 */ FieldWalkmesh* walkmesh;
    /* 0x08 */ FieldTilemap* tilemap;
    /* 0x0C */ FieldCamera* camera;
    /* 0x10 */ FieldTriggers* triggers;
    /* 0x14 */ FieldEncounters* encounters;
    /* 0x18 */ FieldModelLoaderHeader* modelLoader;
} FieldDatHeader; // size: 0x1C

#define FIELD_DAT ((u8*)0x80115000)
#define FIELD_DAT_HEADER ((FieldDatHeader*)(FIELD_DAT - sizeof(FieldDatHeader)))
// MIM during map loading then face textures after the MIM is uploaded.
#define FIELD_TEXTURE_BUFFER ((void*)0x80128000)
// Shared by map preloads, BSX staging, and movie playback.
#define FIELD_LOAD_BUFFER ((void*)0x801B0000)

extern u8* D_80075E10;
extern FieldTilemap** g_FieldTilemap;
extern FieldCamera* g_FieldCamera;
extern FieldCamera** D_80083578;
extern u32 g_FieldFileInfo[787 * 6]; // Yamada pair for DAT, MIM, BSX

extern FieldRenderData g_FieldRenderData[2];
extern FieldTriggers* g_FieldTriggers;
extern FieldEncounters* g_FieldEncounters;
extern FieldEncounters** g_FieldEncountersP;
extern volatile s16 g_FieldMoviePlayed;
extern s32 g_CameraCenterX;
extern s32 g_CameraCenterY;
extern s16 g_CameraScrollX;
extern s16 g_CameraScrollY;
extern s16 g_CameraScrollEnabled;
extern u8 g_RandomTable[256];
extern u8 g_RainForce;
extern u8 g_FieldPrimitiveBufferIndex;
extern FieldWalkmesh** D_8009A044;
extern SVECTOR* g_WalkmeshTriangles;
extern FieldWalkmeshAccess* g_WalkmeshAccesses;
extern DRAWENV D_80113F2C[2];
extern s16 D_801142C8;
extern u8 D_80114498[48];
extern u8 g_PosCursorDisabled;
extern u8 g_WindowReplaceBank[4][8];
extern u16 g_WindowReplaceBankAddr[4][8];

extern char g_FieldDebugActorLabel[8];
extern char g_FieldDebugWordLabel[8];
extern char g_DebugText[];
extern char g_DebugMessageBuffer[];

extern FieldFaceTextureHeader* g_FieldFaceTextureData;
extern u8* g_FieldModelLoadCursor;
extern volatile s16 g_FieldMovieJustStarted;
extern u8 SavedScriptIds[48][8];
extern s32 (*g_FieldOpcodes[256])(void);

void StopFieldMapPreload(void);
void PreloadNextFieldMap(FieldEntity* player, FieldGateway* gateways);
s32 FieldMainLoop(void);
void FieldLoadMimToVram(s32 arg0, void* mimData);
void FieldBackgroundInitPackets(SPRT_16* sprite16, SPRT* sprite, FieldBgAnimEntry* animation, DR_MODE* drawMode);
void AddBackgroundToRender(FieldRenderData* renderData);
s32 FieldCalcWorldToScreenPos(SVECTOR* worldPos, SVECTOR* screenPos);
u8 FieldEntityDirByVec(VECTOR* start, VECTOR* target, s32* distance);
void FieldEntityLineClear(FieldLine* lines);
void DebugRunEveryLoop(void);
void FieldRainInit(FieldRenderData* renderData);
void FieldRainAddToRender(u_long* ot, LINE_F2* rain, MATRIX* matrix, DR_MODE* rainDm);
u8* FieldModelStructInit(FieldModelLoaderHeader* header, FieldModelData* modelData);
u8* FieldModelLoadGlobalModels(FieldModelLoaderHeader* header, FieldModelData* modelData, u8* buffer, s32 loadTextures);
u8* LoadLocalFieldModelAndInitAll(
    FieldModelLoaderHeader* header, FieldModelData* models, const u32* options, void* data);
void FieldModelAnimCalcMtrxs(FieldModel* model, MATRIX* matrix, u8 animationId, s32 frame);
s32 KawaiLoadEyesMouthTexToVram(FieldModel* model, u8* params);
s32 KawaiSetColorToModelPkts(FieldModel* model, u8* params);
void KawaiClearData(void);

void DebugPrintToFieldWindow(const char* str);
void DebugUpdateActor(s16 arg0, s16 entityId);
void FieldDebugAddParseValueToPage2(const char* str, s32 val, s32 kind);
void FieldDebugStringCopy(char* dst, const char* src);
void FieldDebugStringConcat(char* dest, const char* src);
s32 AddStrNextDebugRow(s16 pageId, const char* msg);
void FieldDebugPageInit(s16 pageId, s16 x, s16 y, s16 width, s16 height);
s32 SetStrToDebugRow(s16 pageId, s16 row, const char* str);

#endif
