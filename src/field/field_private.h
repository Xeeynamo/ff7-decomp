#ifndef FIELD_PRIVATE_H
#define FIELD_PRIVATE_H

#define GET_ENTITY_NAME(entityId) ((char*)g_FieldScripts + sizeof(FieldScriptHeader) + (entityId) * 8)

#define GET_FIELD_SCRIPT_OFFSET(dst, scriptOffset, entityDataOffset, extrasHeaderSize)                                 \
    (dst) = *((u8*)((scriptOffset) + (entityDataOffset) + (extrasHeaderSize) + (s32)g_FieldScripts) +                  \
              sizeof(FieldScriptHeader));                                                                              \
    (dst) |= *((u8*)((scriptOffset) + ((entityDataOffset) + (s32)g_FieldScripts) + (extrasHeaderSize)) +               \
               sizeof(FieldScriptHeader) + 1)                                                                          \
             << 8

struct FieldRenderData {
    OT_TYPE ot[0x1000];   // 0x00000: Main scene ordering table
    SPRT_16 arrows[0x18]; // 0x04000: Field arrow sprite packets
    DR_MODE arrowsDm;     // 0x04180: Arrow sprite draw mode

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

    u16 bgAnim[3012];   // 0x10d54: Background animation data
    DR_MODE bgDm[1700]; // 0x124dc: Background draw mode packets

    OT_TYPE otUi;     // 0x1748c: UI ordering table
    DR_MODE rainDm;   // 0x17490: Rain draw mode
    LINE_F2 Rain[64]; // 0x1749c: Rain line primitives
};

// Staged in scratchpad at 0x1F800004; describes the LZS blob to stream off the CD.
typedef struct FieldModelLzsRequest {
    /* 0x0 */ s32 sector;
    /* 0x4 */ s32 size;
} FieldModelLzsRequest;

typedef struct {
    u32 size;
    u16 textureCount;
    u16 paletteCount;
    u32 texturesOffset;
    u32 palettesOffset;
} FieldFaceTextureHeader;

extern u8* D_80075E10;
extern MATRIX* D_80083270;
extern MATRIX** D_80083578;
extern u32 g_FieldFileInfo[];
extern Yamada g_FieldGlobalModelFiles[9];
extern Yamada g_FieldFaceTextureFile;
extern FieldFaceTextureHeader* g_FieldFaceTextureData;
extern u8* D_800E0204;
extern volatile s16 g_FieldMovieJustStarted;
extern u8 SavedScriptIds[48][8];

void StopFieldMapPreload(void);
void PreloadNextFieldMap(FieldEntity* player, FieldGateway* gateways);
s32 FieldMainLoop(void);
void FieldLoadMimToVram(s32 arg0, void* mimData);
void FieldBackgroundInitPackets(SPRT_16* bg1, SPRT* bg2, u16* animation, DR_MODE* drawMode);
void AddBackgroundToRender(struct FieldRenderData* renderData);
s32 FieldCalcLinearStep(s32 start, s32 target, s32 duration, s32 step);
s32 FieldCalcWorldToScreenPos(SVECTOR* worldPos, SVECTOR* screenPos);
s32 FieldEntityGetDirVectorY(u8 angle);
void FieldEntityLineClear(FieldLine* lines);
void DebugRunEveryLoop(void);
void FieldRainInit(struct FieldRenderData* renderData);
void FieldRainAddToRender(u_long* ot, LINE_F2* rain, MATRIX* matrix, DR_MODE* rainDm);
u8* FieldModelStructInit(FieldModelLoaderHeader* header, FieldModelData* modelData);
u8* FieldModelLoadGlobalModels(FieldModelLoaderHeader* header, FieldModelData* modelData, u8* buffer, s32 arg3);
u8* LoadLocalFieldModelAndInitAll(
    FieldModelLoaderHeader* header, FieldModelData* modelData, const u32* arg2, void* arg3);
s32 KawaiLoadEyesMouthTexToVram(FieldModelEntry* entry, u8* scratch);
void KawaiClearData(void);

#endif
