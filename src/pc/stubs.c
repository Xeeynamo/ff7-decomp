#include <game.h>
#include <libcd.h>
#include <libspu.h>

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

// Unported pieces of src/main/akao.c: its still-assembly helpers plus the
// data they own. Array sizes come from the gaps in build/us/main.map.
typedef struct {
    u8 pad[0x108];
} AkaoVoiceSlot;
typedef struct {
    u8 pad[0x210];
} AkaoChannelPair;
typedef struct {
    u8 pad[0x24];
} AkaoSoundMessage;

void (*D_80049548[0x43])(AkaoSoundMessage*);
u8 D_800499A8[0x280];
u8 D_80049C40[4];
s32 D_80063010;
u8 g_AkaoVoiceAttr[4];
s32 g_AkaoVoiceAttrMask;
s32 D_8007EBEC;
s32 D_8007EBF0;
s32 D_8007EBF4;
s32 D_8007EBF8;
s32 D_8007EBFC;
u16 D_8007EC00;
u16 D_8007EC02;
u16 D_8007EC04;
u16 D_8007EC06;
u16 D_8007EC08;
u16 D_8007EC0A;
s16 D_8007EC0C;
s16 D_8007EC0E;
s32 D_8007EC10;
u16 D_80062FC8;
s32 D_80062FE0;
s32 D_80062FF8;
s32 D_80063000;
u32 D_80063004;
u8 g_MovieLock;
AkaoSoundMessage D_80081DC8[32];
s32 D_80083334;
u16 D_8008337E;
s32 D_80083394;
u16 D_800833DE;
s32 D_80083580[0x4961];
s32 D_800804D0;
AkaoVoiceSlot D_80096608[48];
s32 D_80097768;
s32 D_80097870;
AkaoChannelPair D_80099788[4];
u16 D_80099E0C;
s32 D_80099FCC[4];
s32 D_80099FD8;
s32 D_8009A104;
s32 D_8009A10C;
s32 D_8009A110;
s32 D_8009A114;
s32 D_8009A13C;
u16 D_8009A14E;
SpuCommonAttr D_8009C578;
SpuReverbAttr g_ReverbAttr;
u32 g_ReverbMode;
u8 g_FieldMusicLock;
s32 g_AkaoCdVol;
s32 g_AkaoMusicActiveMask;
s32 g_AkaoMusicActiveMaskStored;
s32 g_AkaoMusicAltMask;
s32 g_AkaoMusicOverMask;
s32 g_AkaoNoiseMask;
s32 g_AkaoPitchLfoMask;
s32 g_AkaoPitchMulMusic;
s32 g_AkaoReverbMask;
s32 g_AkaoSoundActiveMaskStored;
s32 g_AkaoTempoMulMusic;

void SpuGetReverbModeParam(SpuReverbAttr* attr) { NOT_IMPLEMENTED; }
long SpuSetIRQ(long on_off) { return 0; }
u_long SpuSetIRQAddr(u_long addr) { return 0; }
SpuTransferCallbackProc SpuSetTransferCallback(SpuTransferCallbackProc func) { return 0; }
void SpuSetVoiceLoopStartAddr(int voiceNum, u_long addr) { NOT_IMPLEMENTED; }

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
u8 g_AkaoCdVolSlideStep[0x4] __attribute__((aligned(4)));
u8 g_AkaoCdVolSlideSteps[0x2] __attribute__((aligned(2)));
AkaoCmd g_AkaoCmd;
u8 g_AkaoCommandQueue[0x480] __attribute__((aligned(4)));
u8 g_AkaoCommandQueueId[0x4] __attribute__((aligned(4)));
u8 g_AkaoControlFlags[0x4] __attribute__((aligned(4)));
u8 g_AkaoEffectsAll[0x4] __attribute__((aligned(4)));
u8 g_AkaoEffectsAllSeq[0x4] __attribute__((aligned(4)));
u8 g_AkaoGlobalUpdateFlags[0x4] __attribute__((aligned(4)));
u8 g_AkaoMusicBuffer[0x12804] __attribute__((aligned(8)));
u8 g_AkaoMusicFadeSteps[0x2] __attribute__((aligned(2)));
u8 g_AkaoMusicId[0x2] __attribute__((aligned(2)));
u8 g_AkaoMusicKeyedMask[0x4] __attribute__((aligned(4)));
u8 g_AkaoMusicOffMask[0x4] __attribute__((aligned(4)));
u8 g_AkaoMusicOnMask[0x4] __attribute__((aligned(4)));
u8 g_AkaoMutex[0x4] __attribute__((aligned(4)));
u8 g_AkaoPitchMulMusicSlideStep[0x4] __attribute__((aligned(4)));
u8 g_AkaoPitchMulMusicSlideSteps[0x2] __attribute__((aligned(2)));
u8 g_AkaoReverbMul[0x2] __attribute__((aligned(2)));
u8 g_AkaoReverbPan[0x2] __attribute__((aligned(2)));
u8 g_AkaoSavedChannelConfig0[0x4] __attribute__((aligned(4)));
u8 g_AkaoSavedChannelConfig1[0x4] __attribute__((aligned(4)));
u8 g_AkaoSavedChannels0[0x1c80] __attribute__((aligned(8)));
u8 g_AkaoSavedChannels1[0x1c80] __attribute__((aligned(8)));
u8 g_AkaoSavedMusicId0[0x2] __attribute__((aligned(2)));
u8 g_AkaoSavedMusicId1[0x2] __attribute__((aligned(2)));
u8 g_AkaoSoundChannelsMode[0x2] __attribute__((aligned(2)));
u8 g_AkaoSoundSlots[0xd08] __attribute__((aligned(8)));
u8 g_AkaoStreamLoopSize[0x4] __attribute__((aligned(4)));
u8 g_AkaoStreamLoopSrc[0x8] __attribute__((aligned(8)));
u8 g_AkaoStreamMask[0x4] __attribute__((aligned(4)));
u8 g_AkaoStreamPan[0x4] __attribute__((aligned(4)));
u8 g_AkaoStreamPitch[0x2] __attribute__((aligned(2)));
u8 g_AkaoStreamRemainingBytes[0x4] __attribute__((aligned(4)));
u8 g_AkaoStreamSrc[0x8] __attribute__((aligned(8)));
u8 g_AkaoStreamVoice16UpdateMask[0x4] __attribute__((aligned(4)));
u8 g_AkaoStreamVoice17UpdateMask[0x4] __attribute__((aligned(4)));
u8 g_AkaoStreamVol[0x4] __attribute__((aligned(4)));
u8 g_AkaoTempoMulMusicSlideStep[0x4] __attribute__((aligned(4)));
u8 g_AkaoTempoMulMusicSlideSteps[0x2] __attribute__((aligned(2)));
u8 g_AkaoVoiceAttrAMode[0x4] __attribute__((aligned(4)));
u8 g_AkaoVoiceAttrAddr[0x4] __attribute__((aligned(4)));
u8 g_AkaoVoiceAttrAr[0x2] __attribute__((aligned(2)));
u8 g_AkaoVoiceAttrDr[0x2] __attribute__((aligned(2)));
u8 g_AkaoVoiceAttrLoopAddr[0x4] __attribute__((aligned(4)));
u8 g_AkaoVoiceAttrPitch[0x2] __attribute__((aligned(2)));
u8 g_AkaoVoiceAttrRMode[0x4] __attribute__((aligned(4)));
u8 g_AkaoVoiceAttrRr[0x2] __attribute__((aligned(2)));
u8 g_AkaoVoiceAttrSMode[0x4] __attribute__((aligned(4)));
u8 g_AkaoVoiceAttrSl[0x2] __attribute__((aligned(2)));
u8 g_AkaoVoiceAttrSr[0x2] __attribute__((aligned(2)));
u8 g_AkaoVoiceAttrVolL[0x2] __attribute__((aligned(2)));
u8 g_AkaoVoiceAttrVolR[0x2] __attribute__((aligned(2)));
u8 g_AkaoVolMulMusic[0x4] __attribute__((aligned(4)));
u8 g_AkaoVolMulMusicSlideStep[0x4] __attribute__((aligned(4)));
u8 g_AkaoVolMulMusicSlideSteps[0x2] __attribute__((aligned(2)));
u8 g_Channel1[0x22c0] __attribute__((aligned(8)));
u8 g_Channel1Config[0x4] __attribute__((aligned(4)));
u8 g_Channel3ActiveMask[0x10] __attribute__((aligned(8)));
u8 g_Channel3OffMask[0x4] __attribute__((aligned(4)));
s16 g_CurrentFieldIndex;
FieldEntity g_FieldEntity[0x100];
s16 g_PlayerModelId;
u8 g_SpuCommonAttr[0x30] __attribute__((aligned(8)));

s32 AkaoCmd_15_PlayMusicSwapSaved() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoCmd_82_ResetVolume() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoCmd_90_SetMuteMusicMask() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoCmd_92_SetCondition() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoCmd_C8_SetCdVol() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoCmd_F4_SaveState() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoCmd_F5_RestoreState() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoDeinit() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoInstrInit() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoLoadEffect() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoLoadInstr2() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoMusicChannelsInit() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoMusicCopyChannels1Into2() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoMusicCopyChannelsAndConfig() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoMusicRestoreChannelsAndConfig() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoMusicStopChannels12() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoMusicSyncKeyStatus() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoMusicVolReset() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_A0_FinishChannel() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_A1_LoadInstrument() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_A4_PitchBendSlide() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_A9_SetVolSlide() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_AC_NoiseClockFreq() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_AD_SetAr() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_AE_SetDr() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_AF_SetSl() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_B1_SetSr() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_B2_SetRr() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_B3_ResetAdsr() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_B4_Vibrato() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_B5_VibratoDepth() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_B7_AttackMode() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_B8_Tremolo() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_BB_SustainMode() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_BF_ReleaseMode() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_C9_LoopReturnTimes() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_DD_VibratoDepthSlide() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_E8_Tempo() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_E9_TempoSlide() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_EB_ReverbDepthSlide() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_F0_LoopJumpTimes() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_F1_LoopBreakTimes() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_F2_LoadInstrument() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_F4_OverlayVoiceOn() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_F5_OverlayVoiceOff() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_F7_OverlayVolBalanceSlide() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_F8_AltVoiceOn() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoOp_F9_AltVoiceOff() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoSoundChannelSlidePitch() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoSoundChannelsClear() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoSoundChannelsInit() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoSoundChannelsStop() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoSoundMenuChannelsInit() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoSoundVolReset() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoStart() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoStreamInit() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoUpdateChannelParamsToSpu() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoUpdateNoiseVoices() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoUpdatePitchLfoVoices() {
    NOT_IMPLEMENTED;
    return 0;
}
s32 AkaoUpdateReverbVoices() {
    NOT_IMPLEMENTED;
    return 0;
}
u_long* BreakDraw(void) {
    NOT_IMPLEMENTED;
    return 0;
}
int IsIdleGPU(int max_count) {
    NOT_IMPLEMENTED;
    return 0;
}
