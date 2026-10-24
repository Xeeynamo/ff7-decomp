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

typedef enum {
    TITLE_FADE_DONE = -1,
    TITLE_FADE_IN = 0,
    TITLE_FADE_ACTIVE = 1,
    TITLE_FADE_OUT = 2,
} TitleFadeState;

typedef enum {
    TITLE_OPTION_NEW_GAME = 0,
    TITLE_OPTION_CONTINUE = 1,
} TitleMenuOption;

typedef enum {
    FORMAT_OPTION_YES = 0,
    FORMAT_OPTION_NO = 1,
} FormatMenuOption;

typedef enum {
    TITLE_RESULT_CONTINUE = 0,
    TITLE_RESULT_NEW_GAME = 1,
} TitleResult;

typedef enum {
    CARD_STATUS_INSERTED = 0,
    CARD_STATUS_ERROR = 1,
    CARD_STATUS_UNFORMATTED = 2,
} CardStatusFlag;

typedef enum {
    CARD_EVENT_SUCCESS = 0,
    CARD_EVENT_ERROR = 1,
    CARD_EVENT_TIMEOUT = 2,
    CARD_EVENT_NEW_CARD = 3,
} CardEventCode;

typedef enum {
    TITLE_TABLE_SLOTS = 0,
    TITLE_TABLE_FILES = 1,
    TITLE_TABLE_FORMAT = 6,
    TITLE_TABLE_TITLE = 7,
} TitleMenuTableIndex;

#define NUM_CARD_SLOTS 2
#define CARD_SLOT_1 0
#define CARD_SLOT_2 1
#define CARD_PORT_STRIDE 0x10
#define NUM_SAVE_FILES_PER_CARD 15
#define SAVE_HEADER_MAX_RETRIES 20
#define TITLE_TRANSITION_DELAY 10
#define TITLE_FADE_STEP 15

#define SAVE_SCREEN_WIDTH 384
#define SAVE_SCREEN_HEIGHT 232

#define SAVE_CHECK_WAIT_FRAMES 240
#define SAVE_SCAN_DELAY_FRAMES 60
#define SAVE_TRANSITION_DELAY 10
#define SAVE_RETRY_COUNT 30
#define SAVE_DELETE_RETRY_COUNT 10

#define SAVE_HEADER_PAGE_SIZE 0x280
#define SAVE_CHECKSUM_SIZE 0x10F0
#define SAVE_ICON_BITMAP_OFFSET 0x2C
#define MEMCARD_TITLE_SIZE 0x40

#define SAVE_LEADER_NAME_LIMIT 0x10
#define SAVE_LOCATION_NAME_LIMIT 0x18
#define SJIS_DIGITS_OFFSET 0x20
#define NUM_MEMCARD_EVENTS 8

typedef enum {
    SAVE_MENU_STATE_SELECT_SLOT = 0,
    SAVE_MENU_STATE_SELECT_FILE = 1,
    SAVE_MENU_STATE_CHECKING_FILES = 2,
    SAVE_MENU_STATE_CHECKING_WAIT = 3,
    SAVE_MENU_STATE_SAVING = 4,
    SAVE_MENU_STATE_FORMAT_PROMPT = 6,
    SAVE_MENU_STATE_CONFIRM = 7,
} SaveMenuState;

typedef enum {
    SAVE_FADE_DONE = -1,
    SAVE_FADE_IN = 0,
    SAVE_FADE_ACTIVE = 1,
    SAVE_FADE_OUT = 2,
} SaveFadeState;

typedef enum {
    LABEL_LOAD = 0,
    SAVE_STR_SELECT_SLOT = 1,
    SAVE_STR_SELECT_FILE = 2,
    SAVE_STR_SLOT_1 = 3,
    SAVE_STR_SLOT_2 = 4,
    SAVE_STR_ARE_YOU_SURE = 5,
    LABEL_LOADING = 6,
    SAVE_STR_SAVING = 7,
    SAVE_STR_EMPTY = 8,
    SAVE_STR_FILE = 9,
    SAVE_STR_CONTINUE = 10,
    SAVE_STR_COULD_NOT_LOAD = 11,
    SAVE_STR_CHECKING_CARD = 12,
    SAVE_STR_SAVED = 28,
    SAVE_STR_FILE_RUINED = 31,
    SAVE_STR_NEW_GAME = 32,
    SAVE_STR_CONFIRM_SAVE = 33,
    SAVE_STR_YES = 34,
    SAVE_STR_NO = 35,
    SAVE_STR_COMPLETED = 41,
} SaveMenuStringId;

typedef enum {
    SAVE_FORMAT_STR_FAILED = 3,
    SAVE_FORMAT_STR_UNFORMATTED = 4,
    SAVE_FORMAT_STR_PROMPT = 5,
    SAVE_FORMAT_STR_NO_CARD = 6,
} SaveFormatStringId;

typedef enum {
    SAVE_ERR_NO_CARD = 0,
    SAVE_ERR_CARD_DAMAGED = 1,
    SAVE_ERR_INSERT_ANOTHER = 2,
    SAVE_ERR_OUT_OF_MEMORY = 3,
    SAVE_ERR_USE_ANOTHER = 4,
    SAVE_ERR_ERASE_BLOCK = 5,
    SAVE_ERR_READ_FAILED = 8,
    SAVE_ERR_STILL_BEGIN = 9,
} SaveErrorStringId;

typedef struct {
    // this whole thing might be a D_801E379C[6]
    /* 0x00 */ MenuTable tables[2];
    /* 0x24 */ u8 unk24[0x40];
    /* 0x64 */ s32 D_801E3800;
    /* 0x68 */ s32 D_801E3804;
    /* 0x6C */ MenuTable promptTables[2];
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

extern u8 buster_tim[];
extern StartMenuMode g_MenuStartMode;
extern s32 g_TitleFadeBrightness;
extern unsigned char g_SaveLabels[][0x24];
extern unsigned char g_SaveFormatStrings[][0x30];
extern unsigned char g_SaveErrorStrings[][0x30];

extern s32 g_TitleResult;
extern u8 D_801E369C[4];
extern s32 g_SaveScanStatus;
extern s32 g_SaveMenuDelayTimer;
extern s32 g_SaveScanInitialCheck;
extern s32 g_SaveScanSlotIndex;
extern s32 g_SaveFadeState;
extern s32 g_SaveBufferIndex;
extern s32 g_SaveMenuMode;
extern DRAWENV g_SaveDrawEnv[2];
extern DISPENV g_SaveDispEnv[2];
extern Menus menus;
extern u8 D_801E382C[0x24];
extern s32 g_SaveMenuState;
extern OT_TYPE* g_SaveOrderingTable;
extern OT_TYPE* g_SaveOrderingTableBuffers[2][1];
extern s32 g_SaveCardCheckWaitTimer;
extern SaveHeader g_SaveSlotHeaders[15];
extern s32 g_SaveSlot;
extern u_long g_SaveCharClutBackup[0x183];
extern u_long g_SaveFontVramBackup[0xA00];
extern MemcardFileHeader g_SaveFileHeader;
extern MemcardSaveFile g_SaveFile;
extern u8 g_MemCardSlotStatus[2][3];
extern u8 g_SaveSlotUnkFlags[2];
extern s32 g_SaveWriteRemaining;
extern u_long g_SaveAvatarVramBackup[0xB40];

s32 SysGetHoursFromSeconds(s32 seconds);
s32 SysGetMinutesFromSeconds(s32 seconds);
s32 SAVEMENU_Title(void);
s32 SAVEMENU_Main(void);
void SAVEMENU_Init(u8 arg0);
int SAVEMENU_HandleSave(s32 counter);
void SaveInitCardEvents(void);
void SaveCleanupCardEvents(void);
u16 GetSaveSlotMask(s32 cardSlot);
SaveHeader* SaveGetHeader(s32 arg0);
u16 SaveCalcChecksum(u16 len, u8* data);
static s16 SaveCheckFile(s32 saveId);
void SaveFetchAllCardStatus(s32 retryCounter);
s32 SaveFetchHeader(s32 cardId, s32 slotId);
void SaveDrawSlot(s32 x, s32 y, s32 slotIndex);
void SaveHandleScrollCursor(MenuTable* menu);
