#include <game.h>
#include <libcd.h>
#include <libspu.h>
#include "../battle/battle.h"

u16 g_BattleMode;
s16 g_isFieldLoading;
FieldState g_FieldState;
SaveWork Savemap;
volatile s16 D_80095DD4;
volatile s16 D_800965EC;
s16 D_8007E768;
s16 D_80071A5C;
s16 D_8009A000[1];
u_long D_8009A004[1];
s32 D_8009A008[1];
u8 D_80062D99;
Unk80075D00* D_80075D00;
DRAWENV* D_8007EBD0;
DISPENV* D_8007EBD8;

s32 D_800707BC;
u16 D_800716D0;
CdlATV D_800698E4;     // CD audio volume
int D_800698E8;        // LZS source sector
s32 D_800698EC;        // seek retries
u8 D_800698F0[0x4800]; // disc buffer
int D_8006E0F0;
int D_8006E0F4;
u32 D_8006E0F8;           // sectors in the current lzs chunk read
s32 D_80071A60;           // current chain operation
int D_80071A64;           // disk number
CdlLOC D_80071A68;        // read position
size_t D_80071A6C;        // sectors left to read
u_long* D_80071A80;       // read destination
void (*D_80071A84)(void); // completion callback

void CdOpMovieBuffer(void) { NOT_IMPLEMENTED; }
void CdOpMoviePlay(void) { NOT_IMPLEMENTED; }
void func_8003DE6C(s32 arg0) { NOT_IMPLEMENTED; }
void func_8003DE84(s32 arg0) { NOT_IMPLEMENTED; }
void func_80041D28(int a, void* b, int c) { NOT_IMPLEMENTED; }
s32 func_80041E30(s32 arg0, s32 arg1) { return 0; }
s32 func_800484A8(void) { return 0; }
s32 func_80048540(s32 arg0) { return 0; }
MATRIX* MulMatrix2(MATRIX* m0, MATRIX* m1) { return m0; }
void SysMovieAbortPlay(void) { NOT_IMPLEMENTED; }
void SysMoviePlay(void* ptr, s16 a) { NOT_IMPLEMENTED; }
void SystemAkaoExecute(void) { NOT_IMPLEMENTED; }
s32 EndingOpcode15(void) { return 0; }
void FIELD_Main(void) { NOT_IMPLEMENTED; }
s32 EndingOpcode1C(void) { return 0; }
s32 EndingOpcode1D(void) { return 0; }
s32 func_800A1EEC(void) { return 0; }
s32 func_800A1F48(void) { return 0; }
void FIELD_Init(void) { NOT_IMPLEMENTED; }
int func_8001117C(void) { return 0; }
int func_80029818(void) { return 0; }
int func_8002988C(void) { return 0; }
int func_80029998(void) { return 0; }
int func_80034444(void) { return 0; }
int func_80034F3C(void) { return 0; }
void func_800354CC(void) { NOT_IMPLEMENTED; }
int func_80036298(void) { return 0; }
int func_8003DDA4(void) { return 0; }
int func_8003DE2C(void) { return 0; }
int func_800D8D78(void) { return 0; }
int SysBattleSwirlRender(void) { return 0; }
int SysBgFadeRender(void) { return 0; }
int SysMenuDrawBattleResult(void) { return 0; }
int SysMovieLoadMovieSettings(void) { return 0; }

void func_800A3178(void* node, s16 a, u8 b, void (*cb)(void)) { NOT_IMPLEMENTED; }
void* func_800A358C(void* a, s32 b, void* c, void* d) { return a; }

void SysMenuInitInput(void) { NOT_IMPLEMENTED; }
void SysMenuAddItem(s32 item) { NOT_IMPLEMENTED; }
s32 SysGetLimitCmdId(s32 charId, s32 limitIndex) {
    NOT_IMPLEMENTED;
    return 0;
}
s32 func_800A0514(s32 arg0) {
    NOT_IMPLEMENTED;
    return 0;
}
void SysMenuDrawMainMenu(void) { NOT_IMPLEMENTED; }
void func_801D080C(void) { NOT_IMPLEMENTED; }

Gpu g_PolyPtr;
u16 g_SaveSlotMask;
u16 D_80062F50;
u8 g_KernRndTable[256];
DRAWENV D_800706A4[2];
DISPENV D_8007075C[2];
u8* D_800707C0;
AttackData D_800722CC[256];
s32 g_PartyPortraitClut[256];
s8 D_80077F64[2][0x3400];
s32 g_MemcardEvents[8];
u8 D_8009C778[256];
u8 D_8009C798[256];
s32 D_8009CE60[256];
u8 D_8009D78A[256];
s32 D_801D07F0;
u8 D_801D07F4[2][8];
u8 D_801D0804[256];
u8 D_801D082C[21];
u8 D_801D0844[16];
u8 D_801D0854[7];
u8 D_801D085C[2];
MenuTable D_801D0860[256];
s32 D_801D4EC4;
RECT D_801D4EC8;
RECT D_801D4ED0;
u8 D_801DEEDC;
s32 D_801DEEF4;
RECT D_801DEEFC;
s32 D_801E3698;
s32 D_801E36A0;
s32 D_801E36A4;
s32 D_801E36A8;
s32 D_801E36AC;
s32 D_801E36B0;
s32 D_801E36B4;
s32 D_801E36B8;
DRAWENV D_801E36BC[2];
DISPENV D_801E3774[2];
s32 D_801E3850;
OT_TYPE* D_801E3854;
OT_TYPE* D_801E3858[2][1];
s32 D_801E3860;
SaveHeader D_801E3864[256];
s32 D_801E3D54;
s32 D_801E3D58;
OT_TYPE* D_801E3D5C;
OT_TYPE* D_801E3D60[2][4];
MenuTable D_801E3D80[2];
MenuTable D_801E3DEC[2];
DRAWENV D_801E3E34[2];
DISPENV D_801E3EEC[2];
s32 D_801E3F14;
s32 D_801E3F18;
s32 D_801E3F1C;
s32 D_801E3F20;
s32 D_801E3F2C[256];
s32 D_801E4538[256];
u8 D_801E8F38[2][3];
s32 D_801E8F44[256];
AccessoryRecord g_AccessoryTable[256];
ActiveCharacterData g_ActiveCharacters[9];
ArmorRecord g_ArmorTable[256];
MateriaData g_MateriaData[100];
s32 g_MenuRenderBufferIndex;
u8 g_SaveFile[0x2000];
u8 g_SaveFileData[8192];
u8 g_SaveFileHeader[0x200];
u8 g_SaveIcons[8192];
s32 g_SaveSlot;
s32 g_SaveWriteRemaining;
u8 g_ShiftJisTable[65536];
s32 g_TutorialActive;
WeaponRecord g_WeaponTable[256];
u8 menus[0x90];

int delete() { return 0; }
int func_801D131C() {
    NOT_IMPLEMENTED;
    return 0;
}
int func_801D1A6C() {
    NOT_IMPLEMENTED;
    return 0;
}
int func_801D2D74() {
    NOT_IMPLEMENTED;
    return 0;
}
int func_801D2DA8() {
    NOT_IMPLEMENTED;
    return 0;
}
int func_801D2E84() {
    NOT_IMPLEMENTED;
    return 0;
}
int func_801D2F00() {
    NOT_IMPLEMENTED;
    return 0;
}
int func_801D3018() {
    NOT_IMPLEMENTED;
    return 0;
}
int func_801D3138() {
    NOT_IMPLEMENTED;
    return 0;
}
int func_801D4118() {
    NOT_IMPLEMENTED;
    return 0;
}
void SysCalculateTotalLureGilPreemptiveValue(void) { NOT_IMPLEMENTED; }
int SysGetMinutesFromSeconds() {
    NOT_IMPLEMENTED;
    return 0;
}
const char* SysKernGetString(s32 arg0, s32 arg1, s32 arg2) {
    NOT_IMPLEMENTED;
    return 0;
}
int SysMenuDrawDigitsWithLeadingZeroes() {
    NOT_IMPLEMENTED;
    return 0;
}
void SysMenuDrawDigitsWithoutLeadingZeroes(s32 x, s32 y, s32 value, s32 digits, s32 color) { NOT_IMPLEMENTED; }
int SysMenuDrawMenuList() {
    NOT_IMPLEMENTED;
    return 0;
}
int SysMenuDrawDialogTimer(void) {
    NOT_IMPLEMENTED;
    return 0;
}
s32 SysMenuGetMenuListState(void) {
    NOT_IMPLEMENTED;
    return 0;
}
void SysMenuSetMenuListAnimation(s32 state, s32 menuId) { NOT_IMPLEMENTED; }
void SysBattleSwirlInit(void) { NOT_IMPLEMENTED; }

s32 D_80010100[64];
u8 D_80063690[0x5E00];
u8 D_800696F0[NUM_MENU_COLOR];
BattleCommandData D_800707C4[32];
AttackData D_800708C4[256];
s32 D_80071744;
u_long* D_800722C8;
s32 D_80095DD8;
volatile s16 D_8009C560;
s32 D_80062F88;
s32 D_80062F90;
u8 D_80062F18;
u8 D_80062F19;
u8 D_80062F1A;
u8 D_80062F1B;
u32 D_8006966C[16];
Unk8009D7BC D_8009D7BC;
u8 D_80063048[0x648];
const char* SysDecompKernStringWithF9(s32 a, s32 b, s32 c) { return 0; }
void BROM_Handle(void) { NOT_IMPLEMENTED; }
void func_801D11A8(void) { NOT_IMPLEMENTED; }
void SysCopyBoostedStatToUnitStructure(void) { NOT_IMPLEMENTED; }
void SysSortMagicInUnitStructure(s32 partyId) { NOT_IMPLEMENTED; }
void BATTLE_Main(void) { NOT_IMPLEMENTED; }
// battle overlay globals read by main (14C70.c)
BattleData g_BattleData;
BattleSceneContext g_BattleSceneContext;
s32 g_FFTextLetterOffset;
s32 g_FFTextNumberOffset;

volatile s16 g_GameState;
volatile s16 g_PrevGameState;
s16 g_IsFieldLoading;
u8* g_MenuTutorial;
u8 g_PartyUpdatedByFieldScript;
u8 s_PadBuffers[2][34];
u8 g_KernelTextBuffer[0x5DEC];
u16 g_KernelTextBlockOffsets[6];
u8 D_8007EBC8;
s8 D_8009C6D8;
s16 D_8007173C;
s32 D_80095DDC;
s32 D_80071E28;
s32 D_800730CC;
volatile s16 D_80075DEC;
u8 g_BattleLock;
volatile s32 D_8009D268[4];

void SetMem(int size) { NOT_IMPLEMENTED; }
s32 SysMenuShow(u8* tutorial) {
    NOT_IMPLEMENTED;
    return 0;
}

void SysInitDispenvDrawenv(void) { NOT_IMPLEMENTED; }
void SysInitFieldFromSavemap(void) { NOT_IMPLEMENTED; }
s32 WORLD_Main(s32* exitAction, s32* fieldId, s32* battleFlags, s32 resume) {
    NOT_IMPLEMENTED;
    return 0;
}
void func_800119E4(void) { NOT_IMPLEMENTED; }
void func_800260DC(void) { NOT_IMPLEMENTED; }
void func_800299C8(void) { NOT_IMPLEMENTED; }
s32 func_800A0000(s32 arg0) {
    NOT_IMPLEMENTED;
    return 0;
}
s32 func_800A00BC(s32 arg0) {
    NOT_IMPLEMENTED;
    return 0;
}
void func_800A00D0(void) { NOT_IMPLEMENTED; }
void MINI_Chocobo(void) { NOT_IMPLEMENTED; }
void func_800A0390(void) { NOT_IMPLEMENTED; }
void func_800A0448(void) { NOT_IMPLEMENTED; }
u16 MINI_Jet(void) {
    NOT_IMPLEMENTED;
    return 0;
}
void func_800A0C58(void) { NOT_IMPLEMENTED; }
void func_800B6B58(void) { NOT_IMPLEMENTED; }

// Unmatched pieces of src/main/17238.c, referenced by the matched ones.
void SysAddMateriaLongRange(u8 arg0) { NOT_IMPLEMENTED; }
void SysAddMagicSummonSkillToUnitStructure(u8 arg0, u8 arg1, u8 arg2) { NOT_IMPLEMENTED; }
s32 SysAddCommandToTemp(s32 arg0) {
    NOT_IMPLEMENTED;
    return 0;
}
void SysAddMateria00(u8 arg0, s32 arg1) { NOT_IMPLEMENTED; }
void SysAddMateria20(u8 arg0, s32 arg1) { NOT_IMPLEMENTED; }
void SysAddMateria40(u8 arg0, s32 arg1) { NOT_IMPLEMENTED; }
void SysRemoveStealIfMug(void) { NOT_IMPLEMENTED; }
void SysAddMateriaEquipStatBonus(u8 materiaId) { NOT_IMPLEMENTED; }
void SysAddMateriaX1(u8 materiaSubType, u8 materiaId, s32 materiaAp) { NOT_IMPLEMENTED; }
void SysAddMateriaX2(u8 materiaSubType, u8 materiaId, s32 materiaAp) { NOT_IMPLEMENTED; }
void SysAddMateriaX3(u8 materiaSubType, u8 materiaId, s32 materiaAp) { NOT_IMPLEMENTED; }
void SysAddMateriaX5(u8 materiaSubType, u8 materiaId, s32 materiaAp) { NOT_IMPLEMENTED; }
void SysAddMateriaX6(u8 materiaId, s32 materiaAp) { NOT_IMPLEMENTED; }
void SysAddMateriaX8(void) { NOT_IMPLEMENTED; }
void SysAddMateriaX9(u8 materiaId, s32 materiaAp) { NOT_IMPLEMENTED; }
void SysAddMateriaXa(void) { NOT_IMPLEMENTED; }
void SysAddMateriaXb(u8 materiaId, s32 materiaAp) { NOT_IMPLEMENTED; }
void SysAddMateriaXc(void) { NOT_IMPLEMENTED; }
u8 SysGetCommandOrder(u8 commandId) {
    NOT_IMPLEMENTED;
    return 0;
}
void SysCopyCommandToUnitStructure(u8 commandId, u8 order) { NOT_IMPLEMENTED; }
void SysAddPairMateriaUnordered(u32 materia1, u32 materia2, u8 arg2, u8 arg3, u8 arg4) { NOT_IMPLEMENTED; }

// Per-character scratch tables filled by src/main/17238.c while it parses equipped materia.
u8 D_800694B4[16];
u8 D_800694C4[16];
u8 D_800694D4[16];
s16 D_800694E4[12];
s16 D_800694FC[6];
CurrentCharBattleMenuCommand D_80069508[NUM_BATTLE_COMMANDS];
CurrentCharStats D_80069538;
CurrentCharMagicCommand D_80069554[NUM_MAGICS];

u16 D_80062F34[3];
u8 D_80069800[48];

// Entry points of menu overlays that are not part of the PC build yet.
void NAMEMENU_Main(s32 arg0) { NOT_IMPLEMENTED; }
void FORMMENU_Main(s32 arg0) { NOT_IMPLEMENTED; }
void SHOPMENU_Main(s32 arg0) { NOT_IMPLEMENTED; }
void ITEMMENU_StealAllMateria(void) { NOT_IMPLEMENTED; }
void ITEMMENU_ReturnStolenMateria(void) { NOT_IMPLEMENTED; }
void ITEMMENU_UnequipCharacterMateria(s32 charIdx) { NOT_IMPLEMENTED; }
void ITEMMENU_RestoreCharacterMateria(s32 charIdx) { NOT_IMPLEMENTED; }
void ITEMMENU_BackupCharacterMateria(s32 charIdx) { NOT_IMPLEMENTED; }
void ITEMMENU_LoadCoinTexture(void) { NOT_IMPLEMENTED; }

u8 g_MovieLock;
u8 g_FieldMusicLock;

void func_800293F4() { NOT_IMPLEMENTED; }
void func_80029C48() { NOT_IMPLEMENTED; }
void func_80029F44() { NOT_IMPLEMENTED; }
void func_8002A094() { NOT_IMPLEMENTED; }
void func_8002A28C() { NOT_IMPLEMENTED; }
void func_8002A43C() { NOT_IMPLEMENTED; }
void func_8002A510() { NOT_IMPLEMENTED; }
void func_8002A748() { NOT_IMPLEMENTED; }
void func_8002A798() { NOT_IMPLEMENTED; }
void func_8002A7E8() { NOT_IMPLEMENTED; }
void func_8002AABC() { NOT_IMPLEMENTED; }
void func_8002AFB8() { NOT_IMPLEMENTED; }
void func_8002B1A8() { NOT_IMPLEMENTED; }
void func_8002BD04() { NOT_IMPLEMENTED; }
void func_8002C004() { NOT_IMPLEMENTED; }
void func_8002C300() { NOT_IMPLEMENTED; }
void func_8002CFC0() { NOT_IMPLEMENTED; }
void func_8002E23C() { NOT_IMPLEMENTED; }
void func_8002FF4C() { NOT_IMPLEMENTED; }
void func_80030038() { NOT_IMPLEMENTED; }
void func_80030148() { NOT_IMPLEMENTED; }
void func_80031820() { NOT_IMPLEMENTED; }
void func_80032E6C() { NOT_IMPLEMENTED; }
void func_80032ED0() { NOT_IMPLEMENTED; }
void func_80033894() { NOT_IMPLEMENTED; }
void func_80038F04() { NOT_IMPLEMENTED; }
void func_801D0BA0() { NOT_IMPLEMENTED; }
void func_801D3228() { NOT_IMPLEMENTED; }
s32 DSCHANGE_WaitDiskLoop(s32 diskNo) { return 0; }
s32 FetchMemCardStatus(s32 cardId) {
    NOT_IMPLEMENTED;
    return 0;
}

// Symbols used by decompiled code whose definitions are still in asm.
// Data declared only inside a .c file is sized from that file, so its real type is not needed here.
DRAWENV D_8007EAAC[2];
DISPENV D_8007EB68[2];
u8 D_8009AD2C;
u8 D_8009C540;
AkaoCmd g_AkaoCmd;
s16 g_CurrentFieldIndex;
FieldEntity g_FieldEntity[0x100];
s16 g_PlayerModelId;

u_long* BreakDraw(void) {
    NOT_IMPLEMENTED;
    return 0;
}
int IsIdleGPU(int max_count) {
    NOT_IMPLEMENTED;
    return 0;
}
