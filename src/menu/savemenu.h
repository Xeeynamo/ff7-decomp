#include <game.h>

typedef enum {
    START_MENU_MODE_SELECT_SLOT = 0,
    START_MENU_MODE_SELECT_FILE = 1,
    START_MENU_MODE_CHECKING_FILES = 2,
    START_MENU_MODE_CHECKING_WAIT = 3,
    START_MENU_MODE_LOADING = 4,
    START_MENU_MODE_FORMAT_PROMPT = 6,
    START_MENU_MODE_TITLE = 7,
} StartMenuMode;

typedef struct {
    // this whole thing might be a D_801E379C[6]
    /* 0x00 */ Unk80026448 D_801E379C[2];
    /* 0x24 */ u8 unk24[0x40];
    /* 0x64 */ s32 D_801E3800;
    /* 0x68 */ s32 D_801E3804;
    /* 0x6C */ Unk80026448 D_801E3808[2];
} Menus; // size: 0x90

extern const char D_801D018C[];
extern const char D_801D0194[];
extern s32 D_801D4EC4;
extern RECT D_801D4EC8;
extern RECT D_801D4ED0;
extern u8 D_801D4EDC[];
extern u8 D_801E2DF8;
extern u8 D_801E2E1C;
extern u8 D_801DEEDC;
extern s32 D_801DEEF4;
extern RECT D_801DEEFC;
extern u8 D_801E2EAC[];
extern const char* D_801E2CB8[];
extern StartMenuMode g_MenuStartMode;
extern s32 D_801E3440;
extern s32 D_801E3530;
extern RECT D_801E3650[3];
extern s32 D_801E36A0;
extern s32 D_801E36A4;
extern s32 D_801E36A8;
extern s32 D_801E36AC;
extern s32 D_801E368C[];
extern u8 D_801E3684[]; // "Level" label
extern s32 D_801E3698;
extern s32 D_801E36B0;
extern s32 D_801E36B8;
extern s32 D_801E36B4;
extern DRAWENV D_801E36BC[2];
extern DISPENV D_801E3774[2];
extern Menus menus;
extern u_long* D_801E3854; // otag pointer
extern u_long* D_801E3858[2][1];
extern SaveHeader D_801E3864[];
extern s32 D_801E3D50;
extern s32 D_801E3D54;
extern s32 D_801E3D58;     // backbuffer id?
extern u_long* D_801E3D5C; // otag pointer
extern u_long* D_801E3D60[2][4];
extern Unk80026448 D_801E3DFE[2];
extern DRAWENV D_801E3E34[2];
extern DISPENV D_801E3EEC[2];
extern s32 D_801E3F2C[];
extern s32 D_801E4538[];
extern u8 D_801E8F38[2][3];
extern s32 D_801E3850;
extern s32 D_801E3860;
extern u8 D_801E8F3B;
extern s32 D_801E8F44[];
extern u8 D_801E2E88;
extern u8 D_801E3158;
extern RECT D_801E3668;
extern s16 D_801E366A;
extern s16 D_801E366E;
extern Unk80026448 D_801E3D80[2];
extern s32 D_801E3F14;
extern s32 D_801E3F18;
extern s32 D_801E3F1C;
extern s32 D_801E3F20;
extern s32 D_801E3660;

extern unsigned char D_801E2CFC[][0x24];
extern unsigned char D_801E3260[][0x30];
extern unsigned char D_801E33B0[][0x30];

void func_801D19C4(void);
void func_801D1BA4(void);
SaveHeader* func_801D1D1C(s32 arg0);
static s16 func_801D2A34(s32 save_id);
void func_801D3668(s32 arg0);
void func_801D370C(s32 x, s32 y, s32 slot_no);
