//! PSYQ=3.3 G=0
#include "main_private.h"
#include "unzip.h"
static Yamada yama_sound_instr_all = {LBA_SOUND_INSTR_ALL, 483232};
static Yamada yama_sound_effect = {LBA_SOUND_EFFECT, 51200};
static Yamada yama_sound_instr_dat = {LBA_SOUND_INSTR_DAT, 8192};
static Yamada yama_sound_instr2_all = {LBA_SOUND_INSTR2_ALL, 251120};
static Yamada yama_sound_instr2_dat = {LBA_SOUND_INSTR2_DAT, 8192};
static Yamada yama_field_field = {LBA_FIELD_FIELD, 85435};
static Yamada yama_world_world = {LBA_WORLD_WORLD, 66715};
static Yamada yama_mini_condor = {LBA_MINI_CONDOR, 39600};
static Yamada yama_mini_snobo = {LBA_MINI_SNOBO, 70075};
static Yamada yama_mini_snobo2 = {LBA_MINI_SNOBO2, 81441};
static Yamada yama_field_dschange = {LBA_FIELD_DSCHANGE, 6004};
static Yamada yama_field_ending = {LBA_FIELD_ENDING, 62484};
static Yamada yama_mini_chocobo = {LBA_MINI_CHOCOBO, 36521};
static Yamada yama_mini_jet = {LBA_MINI_JET, 14067};
static Yamada yama_mini_submar = {LBA_MINI_SUBMAR, 31341};
static Yamada yama_mini_highway = {LBA_MINI_HIGHWAY, 34138};

// likely a left-over from a debug build that used to load sparse files instead from the Yamada LBA.
static char unk_signature[8] = {'Y', 'A', 'M', 'A', '@', 'F', 'F', '7'};

extern u8 D_8007EBC8;
extern s8 D_8009C6D8;
extern s16 D_8007173C;
extern s32 D_80095DDC; // Battle mode flags from world map
extern s32 D_80071E28; // Which module to transition to from world map
extern u8* g_MenuTutorial;
extern s32 SYS_GetDiskNo(void);
extern s32 SysMenuShow(u8*);
extern volatile s16 g_GameState;

void __main(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", __SN_ENTRY_POINT);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_8001117C);

static void func_800111E4(void) {
    D_8009A000[0] = 0xF4;
    AkaoExec();
    if (!(Savemap.memory_bank_4[97] & 0x30)) {
        func_8001117C(0x2B);
    }
    D_800707BC.battleId = g_FieldState.eventCmdParam;
    D_800707BC.mode = g_FieldState.battleMode2;
    D_800707BC.mode = D_800716D0 | D_800707BC.mode;
    func_800146A4();
    D_800716D0 = 0;
}

static void func_80011274(void) {
    SystemLoadFileBySector(yama_sound_instr2_dat.loc, yama_sound_instr2_dat.len, (u_long*)0x800E0000, NULL);

    while (1) {
        if (SystemCdromReadChain() == 0) {
            break;
        }
    }

    SystemLoadFileBySector(yama_sound_instr2_all.loc, yama_sound_instr2_all.len, (u_long*)0x800A0000, NULL);

    while (1) {
        if (SystemCdromReadChain() == 0) {
            break;
        }
    }

    AkaoLoadInstr2((u32*)0x800A0000, (u32*)0x800E0000);
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", func_800112E8);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysBgFadeRender);

static void VSyncCallbackFunc(void) {
    switch (D_80095DD4) {
    case 0:
        break;
    case 1:
        SysBgFadeRender();
        break;
    case 2:
        func_800D8D78();
        break;
    case 3:
        SysBattleSwirlRender();
        break;
    case 4:
        SysMenuDrawBattleResult();
        break;
    }
    if (!D_80062D98 && !g_SavemapBusy) {
        Savemap.game_timer_fraction += 1092; // 65536 / 1092 = ~60
        if (Savemap.game_timer_fraction >> 16) {
            Savemap.time++;
            Savemap.game_timer_fraction &= 0xFFFF;
        }
        Savemap.countdown_timer_fraction += 1092;
        if (Savemap.countdown_timer_fraction >> 16) {
            if (!(Savemap.memory_bank_1[95] & 2)) {
                if (Savemap.countdown_timer_seconds != 0) {
                    Savemap.countdown_timer_seconds--;
                }
            } else {
                Savemap.countdown_timer_seconds++;
            }
            Savemap.countdown_timer_fraction &= 0xFFFF;
        }
    }
    D_8007E768 = 1;
}

static void SysInitBase(void) {
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    func_80036298();
    D_80095DD4 = 0;
    VSyncCallback(VSyncCallbackFunc);
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
}

static void SysInitDispenvDrawenv(void) {
    SetDefDispEnv(&D_8007EB68[0], 0, 232, 320, 240);
    SetDefDispEnv(&D_8007EB68[1], 0, 0, 320, 240);
    SetDefDrawEnv(&D_8007EAAC[0], 0, 8, 320, 224);
    SetDefDrawEnv(&D_8007EAAC[1], 0, 240, 320, 224);
    D_8007EAAC[0].dtd = 1;
    D_8007EAAC[1].dtd = 1;
    D_8007EAAC[0].isbg = 0;
    D_8007EAAC[1].isbg = 0;
    PutDispEnv(&D_8007EB68[0]);
    PutDrawEnv(&D_8007EAAC[0]);
}

void FIELD_Main(void);
void FIELD_Init(void);
static void HandleField(void) {
#ifndef VERSION_PC
    if (g_PrevGameState != GAMESTATE_MENU && g_PrevGameState != GAMESTATE_MENU_COMMANND) {
        if (g_PrevGameState != GAMESTATE_BATTLE) {
            SystemLoadFileBySector(yama_field_field.loc, yama_field_field.len, (u_long*)0x80180000, NULL);
            while (1) {
                if (SystemCdromReadChain() == 0) {
                    break;
                }
            }
            SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
        } else {
            while (1) {
                if (SystemCdromReadChain() == 0) {
                    break;
                }
            }
            SysGzipBinDecompress((GzHeader*)0x801C0000, (u8*)0x800A0000);
        }
    }
#endif
    FIELD_Init();
    FIELD_Main();
}

static void func_80011920(void) {
    g_IsFieldLoading = 0;
    D_80071A5C = 0;
}

static void AkaoInit(void) {
    SystemLoadFileBySector(yama_sound_instr_all.loc, yama_sound_instr_all.len, (u_long*)0x800F0000, NULL);
    do {
    } while (SystemCdromReadChain());
    SystemLoadFileBySector(yama_sound_effect.loc, yama_sound_effect.len, (u_long*)0x801B0000, NULL);
    do {
    } while (SystemCdromReadChain());
    SystemLoadFileBySector(yama_sound_instr_dat.loc, yama_sound_instr_dat.len, (u_long*)0x801BC800, NULL);
    do {
    } while (SystemCdromReadChain());
    func_8002988C(0x800F0000, 0x801BC800);
    func_80029998(0x801B0000);
}

static void InitWorldFromSavemap(void) {
    s32 modelId;
    s32 state;

    state = g_GameState;
    if (state == 3) {
        Savemap.worldmap_exit_action = 2;
        Savemap.current_module = 3;
    } else {
        Savemap.worldmap_exit_action = 0;
        Savemap.current_module = 1;
    }
    Savemap.current_location_id = g_CurrentFieldIndex;
    modelId = g_PlayerModelId;
    Savemap.field_x = g_FieldEntity[modelId].PosX >> 12;
    Savemap.field_y = g_FieldEntity[modelId].PosY >> 12;
    Savemap.field_triangle = g_FieldEntity[modelId].PosI;
    Savemap.field_direction = g_FieldEntity[modelId].MoveDir;
    Savemap.step_id = D_8009C540;
    Savemap.step_offset = D_8009AD2C;
}

static void InitFieldFromSavemap(void) {
    s32 exitAction;

    exitAction = Savemap.worldmap_exit_action;
    g_GameState = Savemap.current_module;
    D_80071E28 = exitAction;
    if (g_GameState == 0) {
        g_GameState = 1;
    }
    g_CurrentFieldIndex = Savemap.current_location_id;
    g_FieldState.prevFieldId = Savemap.current_location_id;
    g_FieldState.pcPosX = Savemap.field_x;
    g_FieldState.pcPosY = Savemap.field_y;
    g_FieldState.pcWalkMeshId = Savemap.field_triangle;
    g_FieldState.pcDirection = Savemap.field_direction;
    D_8009C540 = Savemap.step_id;
    D_8009AD2C = Savemap.step_offset;
}

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysInitNewGame);

void main(void) {
    char name[9] = "battle.x";
    RECT rect = {0, 0, 480, 472};
    u16 minigameResult;

    SetMem(8);
    SysInitBase();
    SysCdromInit();
    SysCdromLoadFile(yama_field_ending.loc, yama_field_ending.len, (u_long*)0x800A0000, NULL);
    ENDING_SceaLoop();
    func_800148B4();
    while (1) {
        g_FieldState.battleMode2 = 0;
        g_FieldState.battlesDisabled = 0;
        func_800148A0();
        AkaoInit();
        InputInit();
        SysCdromLoadFile(yama_field_ending.loc, yama_field_ending.len, (u_long*)0x800A0000, NULL);
        ENDING_Loop(0);
        *D_8009A000 = 192;
        *D_8009A004 = 127;
        AkaoExec();
        ClearImage(&rect, 0, 0, 0);
        func_80026258();
        func_80011920();
        SysSavemapReset();
        if (HandleTitle() == 1) {
            func_80014934();
            func_80026258();
            SysSavemapReset();
            SysInitNewGame();
        }
        func_80026090();
        while (1) {
            if (Savemap.memory_bank_1[768] != SYS_GetDiskNo()) {
                SysCdromLoadFile(yama_field_dschange.loc, yama_field_dschange.len, (u_long*)0x800A0000, NULL);
                if (DSCHANGE_WaitDiskLoop(Savemap.memory_bank_1[768]) == 1) {
                    g_FieldState.eventCmd = EVTCMD_NONE;
                    func_80033BE0();
                    func_800299C8();
                    break;
                }
            }
            SysInitDispenvDrawenv();
            D_8007EBC8 = 0;
            D_8009C6D8 = 0;
            D_8007173C = 0;
            InitFieldFromSavemap();
            g_PrevGameState = 0;
            do {
                switch (g_GameState) {
                case GAMESTATE_FIELD:
                    HandleField();
                    break;
                case GAMESTATE_BATTLE:
                case GAMESTATE_BROM:
                    Savemap.memory_bank_1[24] += 1;
                    if (!Savemap.memory_bank_1[24]) {
                        Savemap.memory_bank_1[25] += 1;
                    }
                    if (g_PartyUpdatedByFieldScript == 1) {
                        func_800260DC();
                        func_80026090();
                        g_PartyUpdatedByFieldScript = 0;
                    }
                    if (g_PrevGameState == GAMESTATE_FIELD) {
                        if (!D_80071E30) {
                            if (g_FieldState.nextBattleMusic) {
                                *D_8009A000 = 20;
                                *D_8009A004 = (u32)g_FieldState.nextBattleMusic;
                                AkaoExec();
                            }
                            D_800722C8 = (u_long*)0x801C0000;
                            D_80071744 = yama_field_field.loc;
                            D_80095DD8 = yama_field_field.len;
                            func_800111E4();
                            if (D_800707BC.mode & 8) {
                                Savemap.memory_bank_1[26] += 1;
                                if (!Savemap.memory_bank_1[26]) {
                                    Savemap.memory_bank_1[27] += 1;
                                }
                            }
                        }
                        if (D_800707BC.mode & 1) {
                            if (!g_FieldState.battleMode1) {
                                g_FieldState.eventCmd = EVTCMD_GAME_OVER;
                                D_800707BC.mode = 0;
                            }
                        }
                        g_PrevGameState = GAMESTATE_BATTLE;
                        g_GameState = GAMESTATE_FIELD;
                    } else {
                        if (D_80095DDC & 0x80000000) { // Pre-emptive battle
                            D_800716D0 = 4;
                        } else {
                            D_800716D0 = 0;
                        }
                        if (D_80095DDC & 0x20000000) { // Countdown timer active
                            D_800716D0 |= 2;
                        }
                        D_800722C8 = (u_long*)0x801C0000;
                        g_FieldState.eventCmdParam = D_80095DDC;
                        D_80071744 = yama_world_world.loc;
                        D_80095DD8 = yama_world_world.len;
                        func_800111E4();
                        if (D_800707BC.mode & 8) {
                            Savemap.memory_bank_1[26] += 1;
                            if (!Savemap.memory_bank_1[26]) {
                                Savemap.memory_bank_1[27] += 1;
                            }
                        } else if (
                            D_800707BC.mode & 1 || (!Savemap.countdown_timer_seconds && D_80095DDC & 0x20000000)) {
                            D_800707BC.mode = 0;
                            g_FieldState.eventCmd = EVTCMD_GAME_OVER;
                        } else if (D_80095DDC & 0x40000000) { // Yuffie battle
                            while (D_80095DD4) {
                            }
                            g_PrevGameState = GAMESTATE_FIELD;
                            g_GameState = GAMESTATE_FIELD;
                            break;
                        }
                        g_PrevGameState = GAMESTATE_BATTLE;
                        g_GameState = GAMESTATE_WORLD;
                    }
                    break;
                case GAMESTATE_WORLD:
                    InitWorldFromSavemap();
                    func_800112E8();
                    switch (D_80071E28) {
                    case 0:
                        g_PrevGameState = GAMESTATE_WORLD;
                        g_GameState = GAMESTATE_FIELD;
                        break;
                    case 1:
                        g_PrevGameState = GAMESTATE_WORLD;
                        g_GameState = GAMESTATE_BATTLE;
                        break;
                    case 2:
                        g_FieldState.eventCmd = EVTCMD_TITLE_SCREEN;
                        break;
                    }
                    break;
                case GAMESTATE_LOAD_INSTR2:
                    func_80011274();
                    g_FieldState.movieCommandState = MOVCMD_DONE;
                    g_PrevGameState = GAMESTATE_LOAD_INSTR2;
                    g_GameState = GAMESTATE_FIELD;
                    break;
                case GAMESTATE_MENU:
                    while (D_80095DD4) {
                    }
                    while (DrawSync(1)) {
                    }
                    InitWorldFromSavemap();
                    if (g_PartyUpdatedByFieldScript == 1) {
                        func_800260DC();
                        func_80026090();
                        g_PartyUpdatedByFieldScript = 0;
                    }
                    switch (g_FieldState.eventCmd) {
                    case EVTCMD_CHAR_NAME_ENTRY:
                        func_80024D88(g_FieldState.eventCmdParam);
                        break;
                    case EVTCMD_PARTY_SELECT:
                        func_80024DD4(g_FieldState.eventCmdParam);
                        func_800260DC();
                        func_80026090();
                        break;
                    case EVTCMD_SHOP:
                        func_80024E18(g_FieldState.eventCmdParam);
                        break;
                    case EVTCMD_PARTY_MENU:
                        if (g_FieldState.eventCmdParam == 1) {
                            SysMenuShow(g_MenuTutorial);
                        } else {
                            SysMenuShow(NULL);
                            g_FieldState.eventCmd = EVTCMD_NONE;
                        }
                        break;
                    case EVTCMD_SAVE_SCREEN:
                        HandleSaveMenu();
                        break;
                    case EVTCMD_UNK12:
                        func_80024FC4(g_FieldState.eventCmdParam);
                        break;
                    case EVTCMD_UNK13:
                        func_80024F80(g_FieldState.eventCmdParam);
                        break;
                    }
                    FIELD_Init();
                    g_FieldState.movieCommandState = MOVCMD_DONE;
                    g_PrevGameState = GAMESTATE_MENU;
                    g_GameState = GAMESTATE_FIELD;
                    break;
                case GAMESTATE_MENU_COMMANND:
                    InitWorldFromSavemap();
                    switch (g_FieldState.eventCmd) {
                    case EVTCMD_YUFFIE_STEALS_MATERIA:
                        func_80024ECC();
                        break;
                    case EVTCMD_YUFFIE_RETURNS_MATERIA:
                        func_80024F04();
                        break;
                    case EVTCMD_REMOVE_CHARS_MATERIA_ACCESSORY:
                        func_80024F3C(g_FieldState.eventCmdParam);
                        break;
                    case EVTCMD_UNK15:
                        func_800250B4();
                        break;
                    case EVTCMD_MASTER_MATERIA_CHECK:
                        func_800250EC(g_FieldState.eventCmdParam);
                        break;
                    case EVTCMD_ADD_MASTER_MATERIA:
                        func_80025130(g_FieldState.eventCmdParam);
                        break;
                    case EVTCMD_JENOVA_SYNTH_COPY_LEVELS:
                        SnapshotPartyLevels();
                        break;
                    }
                    if (g_FieldState.eventCmd != EVTCMD_UNK19) {
                        FIELD_Init();
                    }
                    g_FieldState.movieCommandState = MOVCMD_DONE;
                    g_PrevGameState = GAMESTATE_MENU_COMMANND;
                    g_GameState = GAMESTATE_FIELD;
                    break;
                case GAMESTATE_CHANGE_DISK:
                    if (Savemap.memory_bank_1[768] != SYS_GetDiskNo()) {
                        SysCdromLoadFile(yama_field_dschange.loc, yama_field_dschange.len, (u_long*)0x800A0000, NULL);
                        if (DSCHANGE_WaitDiskLoop(Savemap.memory_bank_1[768]) == 1) {
                            g_FieldState.eventCmd = EVTCMD_TITLE_SCREEN;
                            break;
                        }
                    }
                    SysInitDispenvDrawenv();
                    g_FieldState.movieCommandState = MOVCMD_DONE;
                    g_PrevGameState = GAMESTATE_CHANGE_DISK;
                    g_GameState = GAMESTATE_FIELD;
                    break;
                case GAMESTATE_HIGHWAY:
                    SystemLoadFileBySector(yama_mini_highway.loc, yama_mini_highway.len, (u_long*)0x80180000, NULL);
                    while (SystemCdromReadChain()) {
                    }
                    SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
                    func_800A00D0();
                    g_PrevGameState = GAMESTATE_HIGHWAY;
                    g_GameState = GAMESTATE_FIELD;
                    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
                    break;
                case GAMESTATE_CHOCOBO:
                    SystemLoadFileBySector(yama_mini_chocobo.loc, yama_mini_chocobo.len, (u_long*)0x80180000, NULL);
                    while (SystemCdromReadChain()) {
                    }
                    SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
                    MINI_Chocobo();
                    g_PrevGameState = GAMESTATE_CHOCOBO;
                    g_GameState = GAMESTATE_FIELD;
                    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
                    break;
                case GAMESTATE_SNOWBOARD1:
                    SystemLoadFileBySector(yama_mini_snobo.loc, yama_mini_snobo.len, (u_long*)0x80180000, NULL);
                    while (SystemCdromReadChain()) {
                    }
                    SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
                    func_800A0390();
                    g_PrevGameState = GAMESTATE_SNOWBOARD1;
                    g_GameState = GAMESTATE_FIELD;
                    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
                    break;
                case GAMESTATE_SNOWBOARD2:
                    SystemLoadFileBySector(yama_mini_snobo2.loc, yama_mini_snobo2.len, (u_long*)0x80180000, NULL);
                    while (SystemCdromReadChain()) {
                    }
                    SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
                    func_800A0448();
                    g_PrevGameState = GAMESTATE_SNOWBOARD2;
                    g_GameState = GAMESTATE_FIELD;
                    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
                    break;
                case GAMESTATE_FORTCONDOR:
                    SystemLoadFileBySector(yama_mini_condor.loc, yama_mini_condor.len, (u_long*)0x80180000, NULL);
                    while (SystemCdromReadChain()) {
                    }
                    SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
                    func_800B6B58();
                    g_PrevGameState = GAMESTATE_FORTCONDOR;
                    g_GameState = GAMESTATE_FIELD;
                    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
                    break;
                case GAMESTATE_JET:
                    SystemLoadFileBySector(yama_mini_jet.loc, yama_mini_jet.len, (u_long*)0x80180000, NULL);
                    while (SystemCdromReadChain()) {
                    }
                    SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
                    minigameResult = MINI_Jet();
                    Savemap.memory_bank_1[354] = minigameResult;
                    Savemap.memory_bank_1[355] = minigameResult >> 8;
                    g_PrevGameState = GAMESTATE_JET;
                    g_GameState = GAMESTATE_FIELD;
                    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
                    break;
                case GAMESTATE_SUBMARIME:
                    SystemLoadFileBySector(yama_mini_submar.loc, yama_mini_submar.len, (u_long*)0x80180000, NULL);
                    while (SystemCdromReadChain()) {
                    }
                    SysGzipBinDecompress((GzHeader*)0x80180000, (u8*)0x800A0000);
                    minigameResult = func_800A00BC(Savemap.memory_bank_1[861]);
                    Savemap.memory_bank_1[862] = minigameResult;
                    Savemap.memory_bank_1[863] = minigameResult >> 8;
                    g_PrevGameState = GAMESTATE_SUBMARIME;
                    g_GameState = GAMESTATE_FIELD;
                    g_FieldState.eventCmd = EVTCMD_FIELD_MAP_CHANGE;
                    break;
                }
            } while (g_FieldState.eventCmd != EVTCMD_PLAY_ENDING_FMV && g_FieldState.eventCmd != EVTCMD_GAME_OVER &&
                     g_FieldState.eventCmd != EVTCMD_TITLE_SCREEN);

            if (g_FieldState.eventCmd == EVTCMD_PLAY_ENDING_FMV) {
                SysCdromLoadFile(yama_field_ending.loc, yama_field_ending.len, (u_long*)0x800A0000, NULL);
                ENDING_Loop(1);
                func_80033BE0();
                func_800299C8();
                break;
            } else if (g_FieldState.eventCmd == EVTCMD_GAME_OVER) {
                g_FieldState.eventCmd = EVTCMD_NONE;
                AkaoExec();
                SysCdromLoadFile(yama_field_dschange.loc, yama_field_dschange.len, (u_long*)0x800A0000, NULL);
                func_800A0C58();
                *D_8009A000 = 192;
                *D_8009A004 = 127;
                AkaoExec();
                func_80033BE0();
                func_800299C8();
                break;
            } else if (g_FieldState.eventCmd == EVTCMD_TITLE_SCREEN) {
                g_FieldState.eventCmd = EVTCMD_NONE;
                func_80033BE0();
                func_800299C8();
                break;
            }
        }
    }
}

// Start of field_fade.c
INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetDrawMode);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeInitPoly);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeCopyScreen);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetPolyMonochrome);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetPolyRgbGradual);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetPolyRgbDirect);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeStepsDec);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeStepsInc);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeColorInterpolate);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeSetPolyRgbInterpolate);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeBgUpdate);

INCLUDE_ASM("asm/us/main/nonmatchings/110B8", SysFadeUpdate);
