#include <game.h>
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
u8* D_8003623C;
Unk80075D00* D_80075D00;
DRAWENV* D_8007EBD0;
DISPENV* D_8007EBD8;

s32 D_800707BC;
u16 D_800716D0;
void (*D_8004A634[21])(void);
int D_8006E0F0;
int D_8006E0F4;

s32 func_80034410(void) { return 0; }
s32 func_80034D5C(void) { return 0; }
void func_80036244(void* anim, u16 frame) { NOT_IMPLEMENTED; }
void func_8003DE6C(s32 arg0) { NOT_IMPLEMENTED; }
void func_8003DE84(s32 arg0) { NOT_IMPLEMENTED; }
void func_80041D28(int a, void* b, int c) { NOT_IMPLEMENTED; }
s32 func_80041E30(s32 arg0, s32 arg1) { return 0; }
s32 func_800484A8(void) { return 0; }
s32 func_80048540(s32 arg0) { return 0; }
MATRIX* MulMatrix2(MATRIX* m0, MATRIX* m1) { return m0; }
void* SysCdromGetPackPointer(void* ptr, s32 a) { return ptr; }
void SysMovieAbortPlay(void) { NOT_IMPLEMENTED; }
void SysMoviePlay(void* ptr, s16 a) { NOT_IMPLEMENTED; }
void SystemAkaoExecute(void) { NOT_IMPLEMENTED; }
s32 func_800A14BC(void) { return 0; }
void func_800A16CC(void) { NOT_IMPLEMENTED; }
s32 func_800A17C0(void) { return 0; }
s32 func_800A19A4(void) { return 0; }
s32 func_800A1EEC(void) { return 0; }
s32 func_800A1F48(void) { return 0; }
void func_800CF60C(void) { NOT_IMPLEMENTED; }
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
void SysMenuDrawMainMenu(void) { NOT_IMPLEMENTED; }
void func_801D080C(void) { NOT_IMPLEMENTED; }
void func_801D0BA0(void) { NOT_IMPLEMENTED; }

Gpu D_80062F24;
u16 D_80062F3C;
DRAWENV D_800706A4[2];
DISPENV D_8007075C[2];
u8* D_800707C0;
AttackData D_800722CC[256];
s32 D_800756F8[256];
s8 D_80077F64[2][0x3400];
s32 D_8009A024[8];
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

int delete () { return 0; }
int func_800211C4() {
    NOT_IMPLEMENTED;
    return 0;
}
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
int func_801D3228() {
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
void SysInitPlayerStatFromMateria(s32 arg0) { NOT_IMPLEMENTED; }
const char* SysKernGetString(s32 arg0, s32 arg1, s32 arg2) {
    NOT_IMPLEMENTED;
    return 0;
}
int SysMenuDrawAddWindow() {
    NOT_IMPLEMENTED;
    return 0;
}
int SysMenuDrawDigitsWithLeadingZeroes() {
    NOT_IMPLEMENTED;
    return 0;
}
int SysMenuDrawDigitsWithoutLeadingZeroes() {
    NOT_IMPLEMENTED;
    return 0;
}
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
int SysMenuHandleButtons() {
    NOT_IMPLEMENTED;
    return 0;
}
void SysMenuSetMenuListAnimation(s32 state, s32 menuId) { NOT_IMPLEMENTED; }
void SysBattleSwirlInit(void) { NOT_IMPLEMENTED; }

s32 D_80010100[64];
u8 D_80063690[0x5E00];
u16 D_80069490[0x600];
u8 D_800696F0[NUM_MENU_COLOR];
BattleCommandData D_800707C4[32];
AttackData D_800708C4[256];
s32 D_80071744;
u_long* D_800722C8;
s32 D_80095DD8;
volatile s16 D_8009C560;
s32 D_80062F88;
s32 D_80062F90;
Unk8009D7BC D_8009D7BC;
u8 D_80063048[0x648];
const char* SysDecompKernStringWithF9(s32 a, s32 b, s32 c) { return 0; }
void D_800A00CC(void) { NOT_IMPLEMENTED; }
void D_800A1158(void) { NOT_IMPLEMENTED; }

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
u8 D_80071E30;
volatile s32 D_8009D268[4];

void SetMem(int size) { NOT_IMPLEMENTED; }
s32 SysMenuShow(u8* tutorial) {
    NOT_IMPLEMENTED;
    return 0;
}

void SysInitNewGame(void) { NOT_IMPLEMENTED; }
void SysInitDispenvDrawenv(void) { NOT_IMPLEMENTED; }
void SysInitFieldFromSavemap(void) { NOT_IMPLEMENTED; }
void SysCalcTotalLureGilPreempVal(void) { NOT_IMPLEMENTED; }
void func_800112E8(void) { NOT_IMPLEMENTED; }
void func_800119E4(void) { NOT_IMPLEMENTED; }
void func_80026090(void) { NOT_IMPLEMENTED; }
void func_800260DC(void) { NOT_IMPLEMENTED; }
void func_800299C8(void) { NOT_IMPLEMENTED; }
void func_80033BE0(void) { NOT_IMPLEMENTED; }
s32 func_800A0000(s32 arg0) {
    NOT_IMPLEMENTED;
    return 0;
}
s32 func_800A00BC(s32 arg0) {
    NOT_IMPLEMENTED;
    return 0;
}
void func_800A00D0(void) { NOT_IMPLEMENTED; }
void func_800A02D0(void) { NOT_IMPLEMENTED; }
void func_800A0390(void) { NOT_IMPLEMENTED; }
void func_800A0448(void) { NOT_IMPLEMENTED; }
s32 func_800A0450(void) {
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
s32 g_AkaoWaveTableKey[0x10];
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
u8 D_800716CC;
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
