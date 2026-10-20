#include <game.h>
#include "../field/field_private.h"

u8 D_80070788;
s8 D_800716C8;
s8 D_80071C0C;
u8 D_80071C20;
u8 D_80071E24;
u8* D_80075E10;
FieldModelLoaderHeader* D_8007E770;
s16 D_8007E774;
OT_TYPE D_8007E7A0[2] = {{0xFFFFFF, 0}, {0xFFFFFF, 0}}; // empty fade lists, SysFadeUpdate is a stub
FieldScriptHeader** D_8007EB64;
s8 D_8007EBCC;
s8 D_8007EBDC;
u8 D_8007EBE0;
FieldCamera** D_80083578;
s8 D_80095DCC;
s32 D_800965E4;
u8 D_80099FFC;
FieldWalkmesh** D_8009A044;
s8 D_8009A048[16];
s32 D_8009A060;
s32 D_8009A064;
u8 D_8009D824;
u8 D_8009FE8C;
SPRT_8 D_800E1028[2][0x158];
LINE_F3 D_800E3B28[2][24];
TILE D_800E3FA8[2][12];
DR_MODE D_800E4128[2][6];
u32 D_800E41C8[2][7];
u16 D_800E4200[8];
s32 D_800E48EC;
u16 D_80113F28;
DRAWENV D_80113F2C[2];
DRAWENV D_80113FE4[2];
DRAWENV D_8011409C[2];
DRAWENV D_80114154[2];
DRAWENV D_8011420C[2];
s16 D_801142C8;
u16 D_8011446C;
s32 D_80114478;
s32 D_8011447C;
u8 D_80114498[48];
s16 D_801144CC;
s16 D_801144D4;
u8 D_801144D8;
s8 g_BgFadeMode;
s32 g_CameraCenterX;
s32 g_CameraCenterY;
s16 g_CameraScrollCurrentStep;
s16 g_CameraScrollEnabled;
s16 g_CameraScrollNumSteps;
s16 g_CameraScrollStartX;
s16 g_CameraScrollStartY;
s16 g_CameraScrollTargetX;
s16 g_CameraScrollTargetY;
s16 g_CameraScrollX;
s16 g_CameraScrollY;
u8 g_CharacterLock;
u8 g_CharIdToEntity[9];
u8 g_CurrentEntity;
u8 g_DebugLevel;
char g_DebugMessageBuffer[0x20];
char g_DebugText[0x20];
u8 g_EntityForSplitJoin;
u8 g_EntitySplitJoinState[48];
u8 g_EntityToLine[48];
u8 g_EntityToModel[48];
s16 g_FieldBg16SpriteCount;
s16 g_FieldBgAnimatedDrawModeCount;
void* g_FieldBgLayer3;             // FieldBgTile*
DVECTOR g_FieldBgSourceOffsets[3]; // FieldBgSourceOffsets
s16 g_FieldBgSpriteToDrawModeOffset;
FieldCamera* g_FieldCamera;
u8 g_FieldCurrentOpcode;
s8 g_FieldDebugCurPage;
u8 g_FieldDebugPages[6][0x17A]; // FieldDebugPage
s16 g_FieldDebugRb;
s16 g_FieldDebugRChars;
s16 g_FieldDebugRDm;
s16 g_FieldDebugRLines;
s16 g_FieldDebugRRect;
s16 g_FieldDebugTransp;
FieldEncounters* g_FieldEncounters;
FieldEncounters** g_FieldEncountersP;
u32 g_FieldKeyState;
s16 g_FieldLineCount;
FieldLine g_FieldLines[32];
u8 g_FieldMapVars[256];
u8 g_FieldModelAnimId[16];
u8 g_FieldModelAnimStatus[16];
s16 g_FieldModelBaseAnimSpeed[16];
u8 g_FieldModelCount;
FieldModelData* g_FieldModelData = (FieldModelData*)0x80138250;
s16 g_FieldModelEffAnimSpeed[16];
FieldModelLoaderData* g_FieldModelLoaderData;
FieldEntity* g_FieldModels;
FieldModelLoaderHeader** g_FieldModelsP;
volatile s16 g_FieldMovieJustStarted;
volatile s16 g_FieldMoviePlayed;
u16 g_FieldPaletteBuffer[64][16];
DVECTOR g_FieldPlayerScreenPos;
s16 g_FieldPreloadMapId;
SVECTOR g_FieldRain[64][3]; // struct FieldRain
FieldRenderData g_FieldRenderData[2];
u16 g_FieldScriptPC[48];
u8 g_FieldScriptPriority[48];
FieldScriptHeader* g_FieldScripts;
u8 g_FieldScriptSyncState[48][8];
u8 g_FieldScriptSyncWaitEntity[48][8];
u8* g_FieldText;
FieldTilemap** g_FieldTilemap;
FieldTriggers* g_FieldTriggers;
FieldTriggers** g_FieldTriggersP;
u16 g_FieldWaitCounter[48];
u32 g_MimImage1BlockSize;
u_long* g_MimImage1Data;
u16 g_MimImage1Height;
u16 g_MimImage1TPage;
u16 g_MimImage1Width;
s16 g_MimImage1X;
s16 g_MimImage1Y;
u32 g_MimImage2BlockSize;
u_long* g_MimImage2Data;
u16 g_MimImage2Height;
u16 g_MimImage2TPage;
u16 g_MimImage2Width;
s16 g_MimImage2X;
s16 g_MimImage2Y;
u32 g_MimPaletteBlockSize;
u_long* g_MimPaletteData;
u16 g_MimPaletteHeight;
u16 g_MimPaletteWidth;
u16 g_MimPaletteX;
u16 g_MimPaletteY;
FieldCamera* g_MovieCamera;
FieldState* g_pFieldState;
u8 g_PosCursorDisabled;
u8 g_PosCursorPrimId;
POLY_FT4 g_PosCursorPrims[2];
s16 g_PosCursorX;
s16 g_PosCursorY;
u8 g_RainForce;
u8 g_RandomTableIndex;
u8 g_RandomTableStep = 1;
u16 g_SavedFieldScriptPC[48][8];
u16 g_SplitJoinOrigMoveSpeed[24];
FieldWalkmeshAccess* g_WalkmeshAccesses;
SVECTOR* g_WalkmeshTriangles;
u8 g_WindowBuffer[4][16];
s16 g_WindowBufferPos[4];
u8 g_WindowCount;
WindowData g_WindowData[4];
s16 g_WindowExtraRows[4];
s16 g_WindowFastForwardLevel[4];
s16 g_WindowNameCopyCount[4];
u8 g_WindowReplaceBank[4][8];
u16 g_WindowReplaceBankAddr[4][8];
s16 g_WindowReplaceParam[4];
u8 g_WindowString[4][256];
u8* g_WindowStringPtr[4];
s16 g_WindowTextBudget[4];
u8 g_WindowToEntity[4];
s16 g_WindowTotalRowsHeight[4];
s16 g_WindowWaitTime[4];
u8 SavedScriptIds[48][8];

void FieldModelAnimCalcMtrxs(FieldModel* model, MATRIX* matrix, u8 animationId, s32 frame) { NOT_IMPLEMENTED; }
void FieldModelPrepareRender(FieldModel* model) { NOT_IMPLEMENTED; }
void func_800262D8(void) { NOT_IMPLEMENTED; }
s32 KawaiAnimatedPointLight(FieldModel* model, u8* params) {
    NOT_IMPLEMENTED;
    return 0;
}
s32 KawaiDirectionalColorGradient(FieldModel* model, u8* params) {
    NOT_IMPLEMENTED;
    return 0;
}
s32 KawaiExecute(FieldModel* model, u8* params, s32 modelId, MATRIX* matrix) {
    NOT_IMPLEMENTED;
    return 0;
}
void KawaiSetColorToPartPktsBelowLvl(FieldModelPart* part, s32 r, s32 g, s32 b) { NOT_IMPLEMENTED; }
s32 KawaiSetSplashToPktsBelowLvl(FieldModel* model, u8* params) {
    NOT_IMPLEMENTED;
    return 0;
}
void SysFadeCopyScreen(void) { NOT_IMPLEMENTED; }
void SysFadeInitPoly(void) { NOT_IMPLEMENTED; }
void SysFadeUpdate(void) { NOT_IMPLEMENTED; }
void SysMenuRemoveHpByPartyId(s32 partyId, s32 hp) { NOT_IMPLEMENTED; }
void SysMenuRemoveItem(s32 item) { NOT_IMPLEMENTED; }
void SysMenuRemoveMpByPartyId(s32 partyId, s32 mp) { NOT_IMPLEMENTED; }
u16 SysMenuSearchItem(u16 item) {
    NOT_IMPLEMENTED;
    return 0;
}
void SystemMenuAddHpByPartyId(s32 partyId, s32 hp) { NOT_IMPLEMENTED; }
void SystemMenuAddMpByPartyId(s32 partyId, s32 mp) { NOT_IMPLEMENTED; }
void SystemMenuAddPartyGold(s32 gold) { NOT_IMPLEMENTED; }
void SystemMenuDrawDialog(WindowData* windows, s32 count, u_long* ot, s32 buffer) { NOT_IMPLEMENTED; }
void SystemMenuRemovePartyGold(s32 gold) { NOT_IMPLEMENTED; }
