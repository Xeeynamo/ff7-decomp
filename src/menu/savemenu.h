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
    /* 0x00 */ MenuTable D_801E379C[2];
    /* 0x24 */ u8 unk24[0x40];
    /* 0x64 */ s32 D_801E3800;
    /* 0x68 */ s32 D_801E3804;
    /* 0x6C */ MenuTable D_801E3808[2];
} Menus; // size: 0x90

// PlayStation memory-card file header: a 0x80 title frame followed by three
// 0x80 icon frames. Built in g_SaveFileHeader, then copied to the head of the
// block that gets written to the card.
typedef struct {
    /* 0x000 */ u8 magic[2]; // "SC"
    /* 0x002 */ u8 iconFlag; // 0x11..0x13, low nibble = icon frame count
    /* 0x003 */ u8 blockCount;
    /* 0x004 */ u8 title[0x40]; // Shift-JIS, shown by the card manager
    /* 0x044 */ u8 reserved[0x1C];
    /* 0x060 */ u16 iconPalette[16];
    /* 0x080 */ u8 iconFrame[3][0x80];
} MemcardFileHeader; // size: 0x200

// The 8 KiB block written to the memory card.
typedef struct {
    /* 0x0000 */ MemcardFileHeader header;
    /* 0x0200 */ SaveWork save;
    /* 0x12F4 */ u8 unk12F4[0xD0C];
} MemcardSaveFile; // size: 0x2000

#define SAVE_ICON_SIZE 0x3F6

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
extern s32 g_SaveSlot;
extern s32 D_801E3D54;
extern s32 D_801E3D58;     // backbuffer id?
extern u_long* D_801E3D5C; // otag pointer
extern u_long* D_801E3D60[2][4];
extern MenuTable D_801E3DFE[2];
extern DRAWENV D_801E3E34[2];
extern DISPENV D_801E3EEC[2];
extern s32 D_801E3F2C[];
extern s32 D_801E4538[];
// FF7 char code -> 2-byte Shift-JIS, byte-indexed; digits start at 0x20
extern u8 g_ShiftJisTable[];
// Card icons, SAVE_ICON_SIZE each: CLUT at 0x00, bitmap at 0x2C
extern u8 g_SaveIcons[];

// staged, then copied into g_SaveFile
extern MemcardFileHeader g_SaveFileHeader;
// the 8 KiB block written to the card
extern MemcardSaveFile g_SaveFile;
extern u8 g_SaveFileData[];
// bytes still to write
extern s32 g_SaveWriteRemaining;
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
extern MenuTable D_801E3D80[2];
extern s32 D_801E3F14;
extern s32 D_801E3F18;
extern s32 D_801E3F1C;
extern s32 D_801E3F20;
extern s32 D_801E3660;

extern unsigned char D_801E2CFC[][0x24];
extern unsigned char D_801E3260[][0x30];
extern unsigned char D_801E33B0[][0x30];

s32 SysGetHoursFromSeconds(s32 seconds);
s32 SysGetMinutesFromSeconds(s32 seconds);
void func_801D19C4(void);
void func_801D1BA4(void);
u16 GetSaveSlotMask(s32 cardSlot);
SaveHeader* func_801D1D1C(s32 arg0);
static s16 func_801D2A34(s32 save_id);
void func_801D3668(s32 arg0);
void func_801D370C(s32 x, s32 y, s32 slot_no);
