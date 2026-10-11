#include <game.h>
#include <libetc.h>
#include "unzip.h"

typedef struct {
    u16 magic[4];
    u16 unk8;
    u16 unkA;
    u16 colors[1];
} BromStruct;

extern volatile s16 g_GameState;
extern struct {
    u16 sceneId;
    u16 mode;
} D_800707BC;

void func_80014540(void);
void func_80014578(s32 file_no, void* dst, void (*cb)(void));
void SystemCdWaitCallback(void (*cb)(void));
void InputUpdateBattleKeyStates(void);
void func_80025174(void);
void func_80026090(void);
void SysMenuDraw8widthFont(s32 x, s32 y, u8* str, s32 color);

// "SCENE "
static u8 text_scene[] = {0xC6, 0xB6, 0xB8, 0xC1, 0xB8, 0x3F, 0xFF, 0xFF};

// "0000"
static u8 text_id[] = {0x33, 0x33, 0x33, 0x33, 0xFF, 0xFF, 0xFF, 0xFF};

static RECT D_800A06C4 = {0x73, 0x19A, 0x5A, 0x10};

extern DRAWENV D_800A06CC;
extern DISPENV D_800A0728;
extern u8 D_800A073C[2][0x2000]; // primitive buffers
extern OT_TYPE D_800A473C[2];

static void ScenePickerMain(void);
static void func_800A0534(BromStruct* arg0);
static u16 func_800A05D4(BromStruct* img, s32 arg1, s32 arg2);

static void ScenePickerInit(void) {
    ResetGraph(0);
    SetGraphDebug(0);
    SetDispMask(1);
    SetDefDispEnv(&D_800A0728, 0, 232, 320, 240);
    SetDefDrawEnv(&D_800A06CC, 0, 240, 320, 224);
    D_800A0728.isrgb24 = 0;
    D_800A06CC.dfe = 1;
    D_800A06CC.dtd = 0;
    D_800A06CC.isbg = 0;
    D_800A06CC.tpage = 0;
    VSync(0);
    PutDispEnv(&D_800A0728);
    PutDrawEnv(&D_800A06CC);
}

void BROM_Main(void) {
    while (D_80095DD4) {
    }
    D_80075DEC = 1;
    ScenePickerInit();
    ScenePickerMain();
    if (g_BattleMode) {
        g_GameState = GAMESTATE_BATTLE;
    } else {
        g_GameState = GAMESTATE_FIELD;
        func_80014540();
    }
}

static void func_800A015C(void) {
    if (Unzip((u8*)0x801B0000, (u8*)0x801C0000) > 0) {
        func_800A0534((BromStruct*)0x801C0000);
        func_800A05D4((BromStruct*)0x801C0000, -1, -1);
    }
}

static void ScenePickerMain(void) {
    u8 text[0x60];
    s32 i;
    u32 j;
    u32 prev;
    u32 pad;
    u32 pressed;
    s32 repeat;
    s32 done;
    s32 sceneId;
    s32 buf;
    s32 n;

    sceneId = D_800707BC.sceneId;
    done = 0;
    g_BattleMode = 0;
    InputUpdateBattleKeyStates();
    InputUpdateBattleKeyStates();
    for (i = 0; i < NUM_PARTY; i++) {
        SysInitPlayerStatFromEquip(i);
        SysInitPlayerStatFromMateria((u8)i);
    }
    SysCalcTotalLureGilPreempVal();
    while (!done) {
        repeat = 0;
        prev = 0;
        func_80014578(4, (void*)0x801B0000, func_800A015C);
        SystemCdWaitCallback(NULL);
        buf = 0;
        for (;;) {
            pad = InputReadPadsRaw();
            pressed = (prev ^ pad) & pad;
            ClearOTag(&D_800A473C[buf], 1);
            SysMenuSetOtag(&D_800A473C[buf]);
            SysMenuSetPoly(D_800A073C[buf]);
            if (prev == pad) {
                if (repeat++ >= 9) {
                    pressed = prev;
                }
            } else {
                repeat = 0;
            }
            prev = pad;
            if (pressed & PAD_SELECT) {
                done = 1;
                break;
            }
            if (pressed & (PAD_START | PAD_CIRCLE)) {
                g_BattleMode = 0x8000;
                done = 1;
                break;
            }
            if (!(pressed & PAD_CIRCLE)) {
                if (pressed & PAD_TRIANGLE) {
                    SysInitPlayerStatFromEquip(0);
                    SysInitPlayerStatFromMateria(0);
                    SysInitPlayerStatFromEquip(1);
                    SysInitPlayerStatFromMateria(1);
                    SysInitPlayerStatFromEquip(2);
                    SysInitPlayerStatFromMateria(2);
                    SysCalcTotalLureGilPreempVal();
                    func_80025174();
                    func_80026090();
                    SysInitPlayerStatFromEquip(0);
                    SysInitPlayerStatFromMateria(0);
                    SysInitPlayerStatFromEquip(1);
                    SysInitPlayerStatFromMateria(1);
                    SysInitPlayerStatFromEquip(2);
                    SysInitPlayerStatFromMateria(2);
                    SysCalcTotalLureGilPreempVal();
                    break;
                }
                if (pressed & PAD_UP) {
                    sceneId++;
                } else if (pressed & PAD_DOWN) {
                    sceneId--;
                } else if (pressed & PAD_LEFT) {
                    if (sceneId >= 0) {
                        sceneId += 10;
                    } else {
                        sceneId = 0;
                    }
                } else if (pressed & PAD_RIGHT) {
                    sceneId -= 10;
                }
            }
            if (sceneId < 0) {
                sceneId = 0;
            } else if (sceneId > 0x3FF) {
                sceneId = 0x3FF;
            }
            n = sceneId;
            for (j = 0; j < 8; j++) {
                text[j] = text_id[j];
            }
            i = 4;
            while (n != 0) {
                text[--i] += n % 10;
                n /= 10;
            }
            SysMenuDraw8widthFont(D_800A06C4.x, 178, text_scene, 0);
            SysMenuDraw8widthFont(D_800A06C4.x + 48, 178, text, 0);
            ClearImage(&D_800A06C4, 0xFF, 0xFF, 0xFF);
            DrawOTag(&D_800A473C[buf]);
            buf ^= 1;
            DrawSync(0);
            VSync(2);
        }
    }
    D_800707BC.sceneId = sceneId;
}

static s32 SwapRGB(u16 color) {
    u32 g = color & 0x3E0;
    u32 r = color & 0x1F;
    u32 b = (color & 0x7C00) >> 10;
    return (r << 10) | (b | g);
}

static void func_800A0534(BromStruct* arg0) {
    s32 total_pixels;
    s32 i;
    s32 limit;
    s32 padding[2];
    u16 w, h;

    w = (arg0->unk8 >> 8) | (arg0->unk8 << 8);
    h = (arg0->unkA >> 8) | (arg0->unkA << 8);
    total_pixels = w * h;
    if (total_pixels > 0) {
        i = 0;
        limit = total_pixels;
        do {
            arg0->colors[i] = SwapRGB((arg0->colors[i] >> 8) | (arg0->colors[i] << 8));
            i++;
        } while (i < limit);
    }
}

static u16 func_800A05D4(BromStruct* img, s32 x, s32 arg2) {
    RECT rect;
    s32 w, h;
    s32 y;
    u16 temp_v1;

    if ((temp_v1 = img->magic[0]) != 0x4152)
        return temp_v1;
    if ((temp_v1 = img->magic[1]) != 0x2057)
        return temp_v1;
    if ((temp_v1 = img->magic[2]) != 0x4752)
        return temp_v1;
    if ((temp_v1 = img->magic[3]) != 0x2042)
        return temp_v1;

    w = (img->unk8 >> 8) | (img->unk8 << 8);
    h = (img->unkA >> 8) | (img->unkA << 8);
    if (x == -1) {
        x = (320 - (w & 0xFFFF)) / 2;
    }
    y = arg2 + 232;
    if (arg2 == -1) {
        arg2 = (240 - (h & 0xFFFF)) / 2;
        y = arg2 + 232;
    }
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    LoadImage(&rect, (u_long*)img->colors);
    return 0;
}
