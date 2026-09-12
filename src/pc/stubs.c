#include <game.h>

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
void func_80036244(void* anim, u16 frame) {}
void func_8003DE6C(s32 arg0) {}
void func_8003DE84(s32 arg0) {}
void func_80041D28(int a, void* b, int c) {}
s32 func_80041E30(s32 arg0, s32 arg1) { return 0; }
s32 func_800484A8(void) { return 0; }
s32 func_80048540(s32 arg0) { return 0; }
MATRIX* MulMatrix2(MATRIX* m0, MATRIX* m1) { return m0; }
void* SysCdromGetPackPointer(void* ptr, s32 a) { return ptr; }
void SysMovieAbortPlay(void) {}
void SysMoviePlay(void* ptr, s16 a) {}
void SystemAkaoExecute(void) {}
s32 func_800A14BC(void) { return 0; }
void func_800A16CC(void) {}
s32 func_800A17C0(void) { return 0; }
s32 func_800A19A4(void) { return 0; }
s32 func_800A1EEC(void) { return 0; }
s32 func_800A1F48(void) { return 0; }
void func_800CF60C(void) {}
int func_8001117C(void) { return 0; }
int func_80029818(void) { return 0; }
int func_8002988C(void) { return 0; }
int func_80029998(void) { return 0; }
int func_80034444(void) { return 0; }
int func_80034F3C(void) { return 0; }
int func_80036298(void) { return 0; }
int func_8003DDA4(void) { return 0; }
int func_8003DE2C(void) { return 0; }
int func_800D8D78(void) { return 0; }
int SysBattleSwirlRender(void) { return 0; }
int SysBgFadeRender(void) { return 0; }
int SysMenuDrawBattleResult(void) { return 0; }
int SysMovieLoadMovieSettings(void) { return 0; }

void func_800A3178(void* node, s16 a, u8 b, void (*cb)(void)) {}
void* func_800A358C(void* a, s32 b, void* c, void* d) { return a; }

void SysMenuInitInput(void) {}
void SysMenuDrawMainMenu(void) {}
void func_801D080C(void) {}
void func_801D0BA0(void) {}

Yamada D_80048FE8[15];
Gpu D_80062F24;
u16 D_80062F3C;
OT_TYPE* D_80062FC4;
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
u8 g_ArmorTable[256][0x24];
s32 g_BattleCharIdToCharId[14];
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
int func_800211C4() { return 0; }
int func_80025ED4() { return 0; }
int func_801D131C() { return 0; }
int func_801D1A6C() { return 0; }
int func_801D2D74() { return 0; }
int func_801D2DA8() { return 0; }
int func_801D2E84() { return 0; }
int func_801D2F00() { return 0; }
int func_801D3018() { return 0; }
int func_801D3138() { return 0; }
int func_801D3228() { return 0; }
int func_801D3478() { return 0; }
int func_801D4118() { return 0; }
void SysCalculateTotalLureGilPreemptiveValue(void) {}
int SysGetMinutesFromSeconds() { return 0; }
void SysInitPlayerStatFromEquip(s32 arg0) {}
void SysInitPlayerStatFromMateria(s32 arg0) {}
const char* SysKernGetString(s32 arg0, s32 arg1, s32 arg2) { return 0; }
int SysMenuDrawAddWindow() { return 0; }
int SysMenuDrawDigitsWithLeadingZeroes() { return 0; }
int SysMenuDrawDigitsWithoutLeadingZeroes() { return 0; }
int SysMenuDrawMenuList() { return 0; }
int SysMenuDrawProgressBar() { return 0; }
int SysMenuDrawScrollbarSlider() { return 0; }
int SysMenuDrawScrollbarTrack() { return 0; }
int SysMenuDrawSingleFontLetter() { return 0; }
int SysMenuDrawTexturedRect() { return 0; }
int SysMenuDrawWindow() { return 0; }
s32 SysMenuGetMenuListState(void) { return 0; }
int SysMenuHandleButtons() { return 0; }
void SysMenuSetMenuListAnimation(s32 state, s32 menuId) {}

s32 D_80010100[64];
u8 D_80063690[0x5E00];
u16 D_80069490[0x600];
BattleCommandData D_800707C4[32];
AttackData D_800708C4[256];
s32 D_80071744;
u_long* D_800722C8;
s32 D_80095DD8;
volatile s16 D_8009C560;
void SysBattleSwirlInit(void) {}
s32 D_80062F88;
u8 D_80063048[0x648];
const char* SysDecompKernStringWithF9(s32 a, s32 b, s32 c) { return 0; }
void D_800A00CC(void) {}
void D_800A1158(void) {}
