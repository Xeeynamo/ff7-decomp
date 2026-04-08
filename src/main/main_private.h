#include <game.h>

typedef enum {
    KERNEL_COMMAND,
    KERNEL_ATTACK,
    KERNEL_GROWTH,
    KERNEL_INIT,
    KERNEL_ITEM,
    KERNEL_WEAPON,
    KERNEL_ARMOR,
    KERNEL_ACCESSORY,
    KERNEL_MATERIA,
    KERNEL_DESC_COMMAND,
    KERNEL_DESC_MAGIC,
    KERNEL_DESC_ITEM,
    KERNEL_DESC_WEAPON,
    KERNEL_DESC_ARMOR,
    KERNEL_DESC_ACCESSORY,
    KERNEL_DESC_MATERIA,
    KERNEL_DESC_KEY_ITEM,
    KERNEL_NAME_COMMAND,
    KERNEL_NAME_MAGIC,
    KERNEL_NAME_ITEM,
    KERNEL_NAME_WEAPON,
    KERNEL_NAME_ARMOR,
    KERNEL_NAME_ACCESSORY,
    KERNEL_NAME_MATERIA,
    KERNEL_NAME_KEY_ITEM,
    KERNEL_TEXT_BATTLE,
    KERNEL_NAME_SUMMON,
} KernelID;

typedef struct {
    s32 len; // decompressed length
    s32 unk4;
} GzHeader;

typedef struct {
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
} Unk8001DE0C;

typedef struct {
    u8 padABuffer;
    u8 unk1;
    u8 unk2;
    u8 unk3[31];
    u8 padBBuffer;
    u8 unk23;
    u8 unk24;
    u8 unk25[31];
} Unk800696AC;

typedef struct {
    s32 sector_off;
    s32 length;
} PortraitEntry;

extern s32 D_80010100[];
extern s32 D_80048CFC;
extern s32 D_80048D00;
extern s32 D_80048D04;
extern s32 D_80048D08;
extern s32 D_80048D0C;
extern s32 D_80048D10;
extern s32 D_80048D14;
extern s32 D_80048D18;
extern s32 D_80048D1C;
extern s32 D_80048D20;
extern Yamada D_80048D84[];
extern s32 D_80048DD4[];
extern PortraitEntry D_80048FE8[15];
extern u8 D_80049208[12]; // menu color RGB-quadruplet
extern s32 D_80049224[8];
extern u8 D_80049520[];
extern u8 D_80049528[];
extern u8 D_80063690[];
extern Unk800696AC D_800696AC;
extern u32 D_80062FA0; // Some sort of pad state
extern u16 D_80069490[];
extern u16 D_800707BC;
extern u16 D_800707BE;
extern u8 D_800708D4[];
extern u8 D_800716D0;
extern s32 D_80071744; // LBA loc for func_80014540
extern s16 D_80071A5C;
extern s32 D_80071C24[];   // accessory data?
extern s32 D_80071E44[];   // armor data?
extern u_long* D_800722C8; // LBA dst for func_80014540
extern u8 D_800722DC[];
extern u8 D_800730DC[][0x14];
extern s32 D_80095DD8; // LBA len for func_80014540
extern s16 D_800965E8;
extern u16 D_8009C560;
extern SavePartyMember D_8009C738[8];
extern s16 D_8009A000;
extern u16 D_8009ABF6;
extern u16 D_8009AC32;
extern u8 D_8009D5E9;
extern void (*D_800A00CC)(void); // battle/brom entrypoint
extern void (*D_800A1158)(void); // battle/battle entrypoint
