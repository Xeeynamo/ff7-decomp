#include <game.h>

typedef enum { MONO = 0, STEREO = 1, VOL_RESET = 2 } MenuAudioMode;

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
extern MenuRect D_801D4EC8;
extern MenuRect D_801D4ED0;
extern u8 buster_tim[];
extern u8 D_801E2DF8;
extern u8 D_801E2E1C;
extern u8 D_801DEEDC;
extern s32 D_801DEEF4;
extern RECT D_801DEEFC;
extern u8 D_801E2EAC[];
extern StartMenuMode g_MenuStartMode;
extern s32 D_801E3440;
extern s32 D_801E3530;
extern RECT g_SaveSlotWindowRects[3];
extern s32 D_801E36A0;
extern s32 D_801E36A4;
extern s32 D_801E36A8;
extern s32 D_801E36AC;
extern u8 g_TitleDefaultWindowColors[NUM_MENU_COLOR]; // 4 corners x RGB
extern u8 g_SaveLevelLabel[];                         // "Level" label
extern s32 g_TitleResult;
extern s32 D_801E36B0;
extern s32 D_801E36B8;
extern s32 D_801E36B4;
extern DRAWENV D_801E36BC[2];
extern DISPENV D_801E3774[2];
extern Menus menus;
extern OT_TYPE* D_801E3854;
extern OT_TYPE* D_801E3858[2][1];
extern SaveHeader D_801E3864[];
extern s32 g_SaveSlot;
extern s32 g_TitleFadeBrightness;
extern s32 g_TitleFadeState;
extern s32 g_TitleBufferIndex;
extern OT_TYPE* g_TitleActiveOT;
extern OT_TYPE* g_TitleOrderingTable[2][4];
extern MenuTable g_TitleButtonTables[2];
extern DRAWENV g_TitleDrawEnv[2];
extern DISPENV g_TitleDispEnv[2];
extern s32 g_SaveCharClutBackup[];
extern s32 g_SaveFontVramBackup[];
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
extern u8 g_SaveCardSlotStatus[2][3];
extern s32 D_801E3850;
extern s32 D_801E3860;
extern s32 g_SaveAvatarVramBackup[];
extern u8 D_801E2E88;
extern u8 D_801E3158;
extern MenuRect g_TitleWindowRect;
extern MenuTable g_TitleMenuTables[];
extern s32 g_TitleScanUnk;
extern s32 g_TitleTimer;
extern s32 g_TitleScanInitial;
extern s32 g_TitleScanFileIndex;

extern unsigned char g_SaveMenuStrings[][0x24];
extern unsigned char g_SaveFormatStrings[][0x30];
extern unsigned char g_SaveErrorStrings[][0x30];

s32 SysGetHoursFromSeconds(s32 seconds);
s32 SysGetMinutesFromSeconds(s32 seconds);
s32 TitleMain(void);
s32 SaveMain(void);
int SaveUpdate(s32 counter);
void SaveInitCardEvents(void);
void SaveCleanupCardEvents(void);
u16 GetSaveSlotMask(s32 cardSlot);
SaveHeader* SaveGetHeader(s32 arg0);
u16 SaveCalcChecksum(u16 len, u8* data);
static s16 SaveCheckFile(s32 save_id);
void SaveFetchAllCardStatus(s32 arg0);
s32 SaveFetchHeader(s32 cardId, s32 slotId);
void SaveDrawSlot(s32 x, s32 y, s32 slotIndex);
void SaveHandleScrollCursor(MenuTable* table);
