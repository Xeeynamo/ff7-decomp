//! PSYQ=3.3 CC1=2.7.2
#include "battle_private.h"
#include "game.h"
#include <libetc.h>

static void BattleMenuUpdateHpMpBars(void);
static void BattleMenuUpdateSelectorIcons(void);
static void BattleMenuResetSelectorState(void);
static s32 BattleMenuGetSelectorStatus(void);
static void BattleMenuSetTargetMask(void);
static s32 BattleMenuGetSelectorStatusAlt(void);
void func_800E58CC(void);

static void BattleUnusedHook0(void) {}

static s32 BattleAbs32(s32 arg0) { return arg0 < 0 ? -arg0 : arg0; }

static void BattleUnusedHook1(void) {}

static void BattleEventLogClear(void) { D_800F199C = 0; }

static void BattleEventLogPush(s32 arg0) { D_800F9780[D_800F199C++] = arg0; }

static void BattleUnusedHook2(void) {}

void BattleSetVsyncMode(s8 arg0) { D_800F19A4 = arg0; }

int BattleFlipDoubleBuffer(void) {
    int ret;

    DrawSync(0);
    ret = VSync(D_800F19A4);
    // flip to the other of the two DB buffers
    g_cDb = (g_cDb == &g_db) ? &g_db + 1 : &g_db;
    g_dbIndex ^= 1;
    return ret;
}

static void BattleTriggerSoundCommand(u16 arg0) {
    g_AkaoCmd.opcode = arg0;
    g_AkaoCmd.params[0] = arg0;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
}

void BattlePlaySavemapDoneSound(void) {
    if (g_SavemapBusy) {
        BattleTriggerSoundCommand(AKAO_FLUSH_ALL_PENDING_UPDATES);
    }
    g_SavemapBusy = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D8B60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D8D78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D91DC);

static void BattleUnusedHook3(void) {}

void func_800D93E4(OT_TYPE* ot);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D93E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D9BF4);

u8 BattleGetStatusFlashState(s16 arg0) { return D_800F514C[arg0]; }

void BattleMenuWidgetOpen(s16 x, s16 y, s16 id) {
    D_800F514C[id] = 1;
    if (x != -1 && y != -1) {
        D_800F1E54[id].x = x;
        D_800F1E54[id].y = y;
    }
    D_800F1E54[id].unkE = D_800F1E54[id].unkC = 1;
    D_800F1E54[id].halfW = D_800F1E54[id].w / 2;
    D_800F1E54[id].halfH = D_800F1E54[id].h / 2;
    D_800F2F8C[id]();
}

static void BattleSetStatusFlashState(s16 arg0) { D_800F514C[arg0] = 3; }

static void BattleResetOtClipRegion(void) {
    D_800F311C = 0;
    D_800FAFEC = -0x100;
    D_800FAFF0 = -0x100;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800D9FA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DA380);

void func_800DB818(OT_TYPE*, u32, s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DB818);

const s32 D_800A0E48[] = {0, 0, 0};
void func_800DBC18(OT_TYPE*, s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DBC18);

static void BattleAddOtPrim(OT_TYPE* arg0, s16 arg1) { func_800DBC18(arg0, arg1); }

// ot: this frame's ordering table, selected by func_800D8D78 via the
// D_800F1994 double-buffer index -- forwarded through to BattleAddOtPrim and
// func_800DB818's libgpu OT insert, not otherwise used here
static void BattleAddOtPrimClipped(OT_TYPE* ot) {
    if (D_800F3896 == 0) {
        BattleAddOtPrim(ot, D_800F38A9);
        return;
    }

    if ((D_800F3120 != 0) && (D_800FAFEC >= 0 && D_800FAFEC < 0x128) && (D_800FAFF0 > 0xF && D_800FAFF0 < 0xA6)) {
        func_800DB818(ot, D_800FAFEC, D_800FAFF0);
    }
}

static void BattleSetMenuPolyTable(void) { SysMenuSetPoly(D_80077F64); }

static void BattleClearMenuFlag(void) { D_800F3122 = 0; }

void BattleFormatAmountString(s32 value) {
    s32 i;
    s32 digit;
    u8 pos;
    u8 leading;

    pos = 0;
    leading = 1;
    for (i = 0; i < 5; i++) {
        digit = value / D_800F3124[i];
        if (digit == 0) {
            if (!leading) {
                D_800F55D8[pos++] = 0x10;
            }
        } else {
            leading = 0;
            D_800F55D8[pos++] = digit + 0x10;
        }
        value %= D_800F3124[i];
    }
    D_800F55D8[pos++] = 0x10;
    for (i = 0; i < 5; i++) {
        D_800F55D8[pos++] = g_Labels.labels[15][i];
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DC0CC);

static void BattleUnusedHook4(void) {}

void BattleSetPendingMarkerPos(s16 arg0, s16 arg1) {
    D_800F3138 = 1;
    D_800F313C = arg0;
    D_800F3140 = arg1;
}

void BattleBannerSetEncounterString(s16 stringId) {
    if (stringId == -1) {
        g_EncounterBannerActive = 0;
        return;
    }
    g_EncounterBannerActive = 1;
    g_EncounterBannerStringId = stringId;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DCFD4);

static void BattleMenuDrawItemDescription(void);
static void BattleDrawSetupTypeInfo(OT_TYPE* ot, s16 arg1) {
    s32 setupType;
    s32 temp_v1;

    SysMenuSetOtag(ot);
    switch (arg1) {
    case 0:
        func_800E2098();
        break;
    case 1:
        func_800E2C6C(&D_800F90B4[D_800F38A0]);
        break;
    case 5:
        func_800E3088();
        break;
    case 6:
        func_800E3B64();
        break;
    case 4:
        func_800E3FB4();
        break;
    case 7:
        func_800E3E10();
        break;
    case 2:
        setupType = SETUP_SIDE_ATTACK_3;
        temp_v1 = g_ActiveEncounter.setup.type;
        if (temp_v1 < NUM_SETUP) {
            setupType = -(temp_v1 <= SETUP_BACK_ATTACK) & 7;
        }
        SysMenuDrawString(8, 7, SysKernGetString(5, 0x12, 8), setupType);
        break;
    case 3:
        SysMenuDrawString(9, 7, SysKernGetString(5, 0x13, 8), 7);
        break;
    case 24:
        func_800E33A0();
        break;
    case 20:
        func_800E4180();
        break;
    case 21:
        func_800E4394();
        break;
    case 19:
        BattleMenuDrawSlotNameList();
        break;
    case 26:
        func_800E4C08();
        break;
    case 27:
        func_800E5530();
        break;
    case 28:
        func_800E5978();
        break;
    case 9:
        BattleMenuDrawItemDescription();
        break;
    }
}

void func_800DD85C(OT_TYPE* ot, s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DD85C);

void BattleMenuDrawFrameWindows(OT_TYPE* ot, s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", BattleMenuDrawFrameWindows);

static void BattleAppendSetupInfoRow(OT_TYPE* ot, s16 arg1) {
    func_800DD85C(&ot[D_800F5628 * 5], arg1);
    BattleMenuDrawFrameWindows(&ot[D_800F5628 * 5 + 2], arg1);
    BattleDrawSetupTypeInfo(&ot[D_800F5628 * 5 + 4], arg1);
    D_800F5628++;
}

void BattleDrawPauseOverlay(OT_TYPE* ot);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", BattleDrawPauseOverlay);

void BattleMenuDrawActiveWidgets(OT_TYPE* ot) {
    s16 i;

    if (g_SavemapBusy) {
        BattleDrawPauseOverlay(ot);
    }
    if ((D_800F514D != 0) && (Savemap.partyID[D_800F38A0] == 0xFF)) {
        for (i = 1; i < 32; i++) {
            if (D_800F514C[i] != 0) {
                BattleSetStatusFlashState(i);
            }
        }
    }
    D_800F5628 = 0;
    for (i = 0; i < 32; i++) {
        if (i == 0 || (D_800F514C[i] != 0 && !(g_Pad1Keys & PADRleft))) {
            if (D_800F514C[i] != 0) {
                BattleAppendSetupInfoRow(ot, i);
            }
        }
    }
    func_800D93E4(ot);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DDFEC);

void BattleMenuCommitWItemPair(void) {
    u8 newActor = D_800F5630;
    u16 newItem = D_800F314E;
    u16 newSlot = D_800F562C;
    u16 newTargets = D_800F5634;
    u8 newTargetIdx = D_800F5638;
    u8 newTargetType = D_800F563C;
    u16 item = D_800F389E;
    u16 slot = D_800FAFD4;
    u8 actor = D_800F38A6;
    u16 targets = D_801516F8;
    u8 targetIdx = D_800F38A7;
    u8 targetType = D_80151698;

    D_800F38A6 = newActor;
    D_801516F8 = newTargets;
    D_800F38A7 = newTargetIdx;
    D_800F389E = newItem;
    D_800FAFD4 = newSlot;
    D_80151698 = newTargetType;
    func_800DDFEC();
    D_800F38A6 = actor;
    D_801516F8 = targets;
    D_800F38A7 = targetIdx;
    D_800F389E = item;
    D_800FAFD4 = slot;
    D_80151698 = targetType;
    func_800DDFEC();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DE3CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", BattleMenuInputState13);

void BattleSetupScreenNoop1(void) {}

void BattleSetupScreenNoop2(void) {}

void BattleSetupScreenNoop3(void) {}

void BattleSetupScreenNoop4(void) {}

void BattleSetupScreenNoop5(void) {}

void BattleSetupScreenNoop6(void) {}

void BattleSetupScreenNoop7(void) {}

void BattleSetupScreenNoop8(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DE618);

void BattleSetupScreenNoop9(void) {}

void BattleClampBattleTimer(void) {
    D_80151840 = Savemap.gil;
    if (D_80151840 > 600000) {
        D_80151840 = 600000;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DE94C);

void BattleSetupScreenNoop10(void) {}

void BattleMenuWidgetCommandReset(void) {
    MenuTable* s;
    s16 rows;

    D_800F977C = 0;
    func_800A4F60(D_800F38A0, 0);
    rows = g_ActiveCharacters[D_800F38A0].unk21;
    D_800F1EF0 = D_800F3163[rows];
    D_800F1F02 = D_800F3163[rows];
    s = &D_800F90B4[D_800F38A0].table00;
    if (D_800F57CC == 0) {
        s->column = 0;
        s->row = 0;
        s->unk0 = 0;
        s->rowOffset = 0;
    }
    s->numRowsPerPage = 4;
    s->numTotalRows = 4;
    s->numColumns = rows;
    s->unk4 = rows;
    s->unk10 = 0;
    s->unk11 = 1;
    s->unkE = 0;
    s->unkF = 0;
    s->scrolling = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DEC10);

void BattleSetupScreenNoop11(void) {}

void BattleMenuWidgetInit(void) {
    BattleMenuWidget* widget = &D_800F90B4[D_800F38A0].widget;
    u8 v;

    if (D_800F57CC == 0) {
        widget->unkA = 0;
        widget->cursorRow = 0;
        widget->unk0 = 0;
        widget->scroll = 0;
    }
    widget->unkC = 1;
    widget->unkD = 3;
    *(u16*)widget->unk4 = 1;
    v = D_80166F74;
    widget->unk10 = 2;
    widget->unk11 = 0;
    widget->unkE = 0;
    widget->unkF = 0;
    widget->unk8 = 0;
    widget->unk6 = v << 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF2CC);

void BattleMenuWidgetNoop1(void) {}

void BattleMenuWidgetSubReset(void) {
    MenuTable* s = &D_800F90B4[D_800F38A0].table24;

    if (D_800F57CC == 0) {
        s->column = 0;
        s->row = 0;
        s->unk0 = 0;
        s->rowOffset = 0;
    }
    s->numColumns = 3;
    s->numRowsPerPage = 3;
    s->unk4 = 3;
    s->numTotalRows = 0x12;
    s->unk10 = 2;
    s->unk11 = 0;
    s->unkE = 0;
    s->unkF = 0;
    s->scrolling = 0;
    func_800A4F60(D_800F38A0, 1);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF5C8);

void BattleMenuWidgetNoop2(void) {}

void BattleMenuNoop1(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DF7CC);

void BattleMenuNoop2(void) {}

void BattleMenuNoop3(void) {}

void BattleMenuInputState3(void) {
    u16* tapped;

    if (D_800F3896 == 3 && D_800F99E4 == 0) {
        tapped = &g_Pad1KeysRepeat;
        if (*tapped & PADRright) {
            func_800BB9B8(1);
            D_800F99E4 = 1;
            D_800F3896 = -1;
            func_800A4350(D_800F38A0, D_800F389C, D_800F389E, D_801516F8);
            BattleSetStatusFlashState(3);
            BattleSetStatusFlashState(1);
        } else if (g_Pad1KeysRepeat & (PADLleft | PADRdown)) {
            func_800BB9B8(4);
            D_800F99E4 = 1;
            D_800F3896 = 1;
            BattleSetStatusFlashState(3);
        }
    }
}

void BattleMenuNoop4(void) {}

void BattleMenuWidgetSub2Reset(void) {
    MenuTable* s = &D_800F90B4[D_800F38A0].table36;

    if (D_800F57CC == 0) {
        s->column = 0;
        s->row = 0;
        s->unk0 = 0;
        s->rowOffset = 0;
    }
    s->numColumns = 1;
    s->numRowsPerPage = 3;
    s->unk4 = 1;
    s->numTotalRows = 0x10;
    s->unk10 = 2;
    s->unk11 = 0;
    s->unkE = 0;
    s->unkF = 0;
    s->scrolling = 0;
    func_800A4F60(D_800F38A0, 2);
}

void BattleMenuInputState7(void) {
    MagicRecord* summons = &g_ActiveCharacters[D_800F38A0].enabledMagic[56];
    MenuTable* menu = &D_800F90B4[D_800F38A0].table36;
    s32 index;
    MagicRecord* summon;
    u8 id;

    func_800A4F60(D_800F38A0, 2);
    if (D_800F3896 == 7 && D_800F99E4 == 0) {
        SysMenuHandleButtons(menu);
        if (menu->scrolling == 0) {
            if (g_Pad1KeysRepeat & PADRright) {
                D_800F99E4 = 1;
                index = menu->column + menu->row + menu->rowOffset;
                summon = (MagicRecord*)((index * sizeof(MagicRecord)) + (s32)summons);
                if (!(summon->menuflags & 2) && summon->id != 0xFF) {
                    func_800BB9B8(1);
                    id = summon->id;
                    D_800FAFD4 = index;
                    D_800F389E = id;
                    D_800F38A2 = summon->targetFlags;
                    func_800E6B94();
                    D_800F3896 = 0;
                    D_800F3894 = 7;
                } else {
                    func_800BB9B8(3);
                }
            } else if (g_Pad1KeysRepeat & PADRdown) {
                func_800BB9B8(4);
                D_800F99E4 = 1;
                D_800F3896 = 1;
                BattleSetStatusFlashState(7);
            }
        }
    }
}

void BattleMenuNoop5(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DFC40);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800DFE34);

void BattleMenuNoop6(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", BattleMenuWidgetSub4Reset);

void BattleMenuInputState18(void) {
    BattleLimitData* limits = &g_ActiveCharacters[D_800F38A1].limits;
    MenuTable* menu = &D_800F90B4[D_800F38A1].table5A;

    if (D_800F3896 == 0x18 && D_800F99E4 == 0) {
        SysMenuHandleButtons(menu);
        if (g_Pad1KeysRepeat & PADRright) {
            func_800BB9B8(1);
            D_800F38A2 = limits->unk3[menu->row];
            D_800F389E = limits->limitId[menu->row];
            D_800FAFD4 = menu->row;
            func_800E6B94();
            D_800F3894 = 0x18;
            D_800F3896 = 0;
            D_800F99E4 = 1;
        } else if (g_Pad1KeysRepeat & PADRdown) {
            func_800BB9B8(4);
            D_800F99E4 = 1;
            D_800F3896 = 1;
            BattleSetStatusFlashState(0x18);
        }
    }
}

void BattleMenuNoop7(void) {}

void BattleMenuResetSelectorStateAlt();
void BattleMenuEnterStateA(void) { BattleMenuResetSelectorStateAlt(); }

void BattleMenuInputState1B(void) {
    BattleLimitData* temp_s0;

    temp_s0 = &g_ActiveCharacters[D_800F38A1].limits;
    if (D_800F3896 == 0x1B && D_800F99E4 == 0) {
        if (g_Pad1KeysRepeat & PADRright) {
            if (BattleMenuGetSelectorStatusAlt() == 2) {
                D_800F99E4 = 1;
                func_800BB9B8(1);
                D_800F38A2 = temp_s0->unk3[0];
                D_800F389E = temp_s0->limitId[0];
                D_800FAFD4 = 0;
                func_800E6B94();
                func_800DDFEC();
                BattleMenuUpdateSelectorIconsAlt();
                BattleSetStatusFlashState(0x1B);
                BattleSetStatusFlashState(1);
            }
        } else if ((g_Pad1KeysRepeat & PADRdown) && (BattleMenuGetSelectorStatusAlt() == 0)) {
            func_800BB9B8(4);
            D_800F99E4 = 1;
            D_800F3896 = 1;
            BattleSetStatusFlashState(0x1B);
        }
    }
}

void BattleMenuNoop8(void) {}

void BattleMenuEnterStateB(void) { BattleMenuResetSelectorState(); }

void BattleMenuInputState1A(void) {
    BattleLimitData* temp_s0;

    temp_s0 = &g_ActiveCharacters[D_800F38A1].limits;
    if ((D_800F3896 == 0x1A) && (D_800F99E4 == 0)) {
        if (g_Pad1KeysRepeat & PADRright) {
            D_800F99E4 = 1;
            if (BattleMenuGetSelectorStatus() == 2) {
                func_800BB9B8(1);
                D_800F38A2 = temp_s0->unk3[0];
                D_800F389E = temp_s0->limitId[0];
                D_800FAFD4 = 0;
                func_800E6B94();
                func_800DDFEC();
                BattleMenuUpdateSelectorIcons();
                BattleSetStatusFlashState(0x1A);
                BattleSetStatusFlashState(1);
            }
        } else if ((g_Pad1KeysRepeat & PADRdown) && (BattleMenuGetSelectorStatus() == 0)) {
            func_800BB9B8(4);
            D_800F99E4 = 1;
            D_800F3896 = 1;
            BattleSetStatusFlashState(0x1A);
        }
    }
}

void BattleMenuNoop9(void) {}

void BattleMenuInitCommandCursor(void) {
    s32 i;

    SysMenuSetCursorMovement(&D_800F90B4[0].table7E, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 1, 0, 0);
    for (i = 1; i < 0x1C; i++) {
        if (D_800F514C[i] != 0) {
            BattleSetStatusFlashState(i);
        }
    }
    D_800F57D4 = 5;
    func_800E58CC();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E05E4);

void BattleMenuNoop10(void) {}

void BattleMenuFlagTableReset(void) {
    s32 i;

    SysMenuSetCursorMovement(&D_800F90B4[0].table90, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 1, 0, 0);
    i = 1;
    do {
        if ((i != 9) && (D_800F514C[i] != 0)) {
            BattleSetStatusFlashState(i);
        }
        i++;
    } while (i < 0x20);
}

void BattleMenuInputState9(void) {
    if (D_800F3896 == 9) {
        SysMenuHandleButtons(&D_800F90B4[0].table90);
        if (g_Pad1KeysPressed & PADRright) {
            if (D_800F90B4[0].table90.column == 0) {
                func_800A4844(1);
            } else {
                func_800A4844(0);
            }
            BattleSetStatusFlashState(9);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E08C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E0BE0);

static void BattleMenuClearWidgetFlags(void) {
    s16 i;

    for (i = 0; i < 0x20; i++) {
        D_800F514C[i] = 0;
    }
}

// Per-frame update for the in-battle command/item/magic menus: refreshes the 32
// menu widgets (open/update/close via the handler table), then routes pad
// input.
//
// For the item list (menu state 0xA) this is where item counts change. Pressing
// OK decrements the count of the entry under the cursor in the battle item list
// (D_801671B8, one 6-byte entry per battle-usable item); on the first W-Item
// selection it also saves the chosen entry (id/index/target) into
// D_800F314E/D_800F562C/... before advancing to the second selection.
//
// Pressing Cancel during the second W-Item selection refunds one unit, but to
// the entry at the SAVED index (D_800F562C), not the one that was decremented,
// and without checking that a refund is still owed. Repeating select/cancel
// therefore adds one to the first item's count each time (clamped at 99), and
// restores its slot id if it had reached zero: the W-Item duplication glitch.
static void BattleMenuUpdate(void) {
    BattleItemEntry* list;
    BattleMenuWidget* menu;
    void (*handler)(void);
    s16 i;
    if ((D_800F514D != 0) && (Savemap.partyID[D_800F38A0] == 0xFF)) {
        for (i = 1; i < 0x20; i++) {
            if (D_800F514C[i] != 0) {
                BattleSetStatusFlashState(i);
            }
        }
    }
    menu = &D_800F90B4[D_800F38A0].widget;
    list = D_801671B8;
    if (g_Pad1Keys & PADRleft) {
        D_800F99E4 = 1;
    } else {
        D_800F99E4 = 0;
    }
    if ((D_800F99E4 == 0) && ((D_800F514D == 2) || (D_800F515F == 2))) {
        if ((D_800F5166 != 2) && (D_800F5167 != 2)) {
            if (g_Pad1KeysPressed & PADRup) {
                func_800BB9B8(1);
                func_800A4E40();
                D_800F99E4 = 1;
                D_800F3896 = -1;
                for (i = 1; i < 0x20; i++) {
                    if (D_800F514C[i] != 0) {
                        BattleSetStatusFlashState(i);
                    }
                }
            }
        }
    }
    {
        for (i = 0; i < 0x20; i++) {
            switch (D_800F514C[i]) {
            case 1:
                func_800E08C4(i);
                break;

            case 2:
                if (i == 0x1C) {
                    D_800F300C[i]();
                } else if (D_800F5168 == 0) {
                    handler = D_800F300C[i];
                    handler();
                }
                break;

            case 3:
                func_800E0BE0(i);
                break;

            case 0:

            case 4:
                break;
            }
        }
    }
    if (D_800F3896 == 0) {
        BattleMenuSetTargetMask();
        func_800E7170();
        D_800F310E = 0;
        if (!((((s32)D_801516F8) >> D_800F38A9) & 1)) {
            D_800F310E = 1;
        }
        if ((D_80166F75 != 0) && (((D_800F389D == 0xA) || (D_800F389D == 3)) || (D_800F389D == 5))) {
            D_800F3120 = 1;
            D_80166F75 = 0;
            D_800F99E4 = 1;
            D_800F3896 = (s16)D_800F3894;
            if (D_800F5161 != 0) {
                BattleSetStatusFlashState(0x15);
            }
            return;
        }
        if (D_800F99E4 == 0) {
            if ((g_Pad1KeysRepeat & PADRright) != 0) { // pressed Confirm/OK
                if ((D_800FAFDC != 0) || (D_800F310E != 0)) {
                    func_800BB9B8(3);
                    D_800F99E4 = 1;
                    return;
                }
                if (D_800F38A4) {
                    if (D_800F38A4 == 2) {
                        if (D_800F389D == 0xA) {
                            list[menu->cursorRow + menu->scroll].count -= 1;
                            if (list[menu->cursorRow + menu->scroll].count == 0) {
                                list[menu->cursorRow + menu->scroll].id = 0xFFFF;
                            }
                        }
                        BattleMenuCommitWItemPair();
                        return;
                    }
                    if (D_800F389D == 0xA) {
                        list[menu->cursorRow + menu->scroll].count -= 1;
                        if (list[menu->cursorRow + menu->scroll].count == 0) {
                            list[menu->cursorRow + menu->scroll].id = 0xFFFF;
                        }
                    }
                    D_800F3120 = 1;
                    D_800F99E4 = 1;
                    D_800F38A4 += 1;
                    // remember the first item (id, slot, target); the second
                    // pick and the cancel refund read these back
                    D_800F314E = D_800F389E;
                    D_800F562C = D_800FAFD4;
                    D_800F5630 = D_800F38A6;
                    D_800F5634 = D_801516F8;
                    D_800F5638 = D_800F38A7;
                    D_800F563C = D_80151698;
                    D_800F3896 = (s16)D_800F3894;
                    if (D_800F5161 != 0) {
                        BattleSetStatusFlashState(0x15);
                    }
                    return;
                }
                if (((D_800F389D == 3) || (D_800F389D == 0xA)) || (D_800F389D == 5)) {
                    list[menu->cursorRow + menu->scroll].count -= 1;
                    if (list[menu->cursorRow + menu->scroll].count == 0) {
                        list[menu->cursorRow + menu->scroll].id = 0xFFFF;
                    }
                }
                func_800DDFEC();
            } else if (g_Pad1KeysRepeat & PADRdown) {           // pressed Cancel/Back
                if ((D_800F38A4 == 2) && (D_800F389D == 0xA)) { // in the item menu?
                    if (list[D_800F562C].count == 0) {
                        list[D_800F562C].id = D_800F314E;
                    }
                    list[D_800F562C].count += 1;
                    if (list[D_800F562C].count >= 0x64) { // max stack is 99
                        list[D_800F562C].count = 0x63;
                    }
                }
                func_800BB9B8(4);
                D_800F3120 = 1;
                D_800F99E4 = 1;
                D_800F3896 = (s16)D_800F3894;
                if (D_800F5161 != 0) {
                    BattleSetStatusFlashState(0x15);
                }
            }
        }
    }
}

typedef struct {
    /* 0x156 */ u16 limitReadyMask;
} BattleSceneData; // size:0x178

void BattleMenuInit(void) {
    BattleSceneData* battleSceneData;

    _D_80062DFD = 1;
    D_80163604 = 0;
    D_801635F8 = 0;
    D_80163600 = Savemap.time & 0x7F;
    BattleMenuUpdateHpMpBars();
    battleSceneData = (BattleSceneData*)&D_80163762;
    D_800F3150 = battleSceneData->limitReadyMask;
    D_800F3110 = 1;
    D_800F3896 = -1;
    BattleMenuClearWidgetFlags();
    if (D_800F3110 != 0) {
        D_800F3110 = 0;
    }
    D_80062D98 = 0;
    g_SavemapBusy = 0;
    D_800F198C = battleSceneData->limitReadyMask;
    D_800F57CC = (Savemap.config >> 4) & 3;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E16B8);

static void BattleMenuDrawIcon(s16 arg0, s16 arg1, s32 arg2) {
    s32 temp_a2;
    u32 var_a3;
    s32 x;
    s32 y;

    if (arg2 < 0x80) {
        var_a3 = 0;
    } else if (arg2 < 0x100) {
        temp_a2 = arg2 - 0x80;
        if (temp_a2 < 0x10) {
            var_a3 = 1;
        } else if (temp_a2 < 0x20) {
            var_a3 = 3;
        } else if (temp_a2 < 0x30) {
            var_a3 = 2;
        } else if (temp_a2 < 0x3E) {
            var_a3 = 5;
        } else if (temp_a2 < 0x49) {
            var_a3 = 4;
        } else if (temp_a2 < 0x57) {
            var_a3 = 9;
        } else if (temp_a2 < 0x65) {
            var_a3 = 6;
        } else if (temp_a2 < 0x72) {
            var_a3 = 7;
        } else {
            var_a3 = 8;
        }
    } else if (arg2 < 0x120) {
        var_a3 = 10;
    } else {
        var_a3 = 11;
    }
    x = ((var_a3 & 1) << 4) | 0x60;
    y = ((var_a3 >> 1) << 4) + 0x70;
    SysMenuDrawTexturedRect(arg0, arg1, x, y, 0x10, 0x10, 1, 0);
}

s32 BattleMenuCalcRatio16(void) {
    s32 i;

    for (i = 0; i < NUM_PARTY; i++) {
        if (Savemap.partyID[i] == 0) {
            s16 val1 = g_ActiveCharacters[i].hp;
            s16 val2 = g_ActiveCharacters[i].baseHp;
            return (val1 * 0xFFFF) / val2;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", BattleMenuDrawPartyHpMpBars);

void BattleMenuUpdateHpMpBars(void) {
    ActiveCharacterData* activeCharacters = g_ActiveCharacters;
    s16 i;
    s32 hp;
    s32 mp;

    // Used for representation of active battle characters' HP and MP in battle
    // menu.
    if (!g_ActiveCharsHPMPInited) {
        for (i = 0; i < 3; i++) {
            D_801516A4[i] = activeCharacters[i].hp << 8;
            D_801516CC[i] = activeCharacters[i].mp << 8;
        }
        g_ActiveCharsHPMPInited = 1;
    }

    // Makes the ATB bar pulse orange after it's filled.
    g_AtbBarPulseColor += g_AtbBarPulseValue;
    if (g_AtbBarPulseColor > 176) {
        g_AtbBarPulseValue = -g_AtbBarPulseValue;
    }
    if (g_AtbBarPulseColor < 0) {
        g_AtbBarPulseColor = 0;
        g_AtbBarPulseValue = -g_AtbBarPulseValue;
    }

    for (i = 0; i < 3; i++) {
        if (Savemap.party[i].char_id != 0xFF) {
            hp = activeCharacters[i].hp << 8;
            if (D_801516A4[i] > hp) {
                D_801516A4[i] -= (activeCharacters[i].baseHp << 8) / 240;
                if (D_801516A4[i] < hp) {
                    D_801516A4[i] = activeCharacters[i].hp << 8;
                }
                D_8015174C[i] = hp;
                D_801517C8[i] = D_801516A4[i];
                D_801031F4[i] = 2;
            } else if (D_801516A4[i] < hp) {
                D_801516A4[i] += (activeCharacters[i].baseHp << 8) / 240;
                if (D_801516A4[i] > hp) {
                    D_801516A4[i] = activeCharacters[i].hp << 8;
                }
                D_8015174C[i] = D_801516A4[i];
                D_801517C8[i] = hp;
                D_801031F4[i] = 1;
            } else {
                D_8015174C[i] = hp;
                D_801517C8[i] = hp;
                D_801031F4[i] = 0;
            }

            mp = activeCharacters[i].mp << 8;
            if (D_801516CC[i] > mp) {
                D_801516CC[i] -= (activeCharacters[i].baseMp << 8) / 240;
                if (D_801516CC[i] < mp) {
                    D_801516CC[i] = activeCharacters[i].mp << 8;
                }
                D_8015178C[i] = mp;
                D_8015187C[i] = D_801516CC[i];
                D_80151688[i] = 2;
            } else if (D_801516CC[i] < mp) {
                D_801516CC[i] += (activeCharacters[i].baseMp << 8) / 240;
                if (D_801516CC[i] > mp) {
                    D_801516CC[i] = activeCharacters[i].mp << 8;
                }
                D_8015178C[i] = D_801516CC[i];
                D_8015187C[i] = mp;
                D_80151688[i] = 1;
            } else {
                D_8015187C[i] = mp;
                D_8015178C[i] = mp;
                D_80151688[i] = 0;
            }
        }
    }
}

static void BattleMenuDrawDigitString(s32 arg0, s32 arg1) {
    SysMenuDraw8widthFont(0xB0, arg0, &D_800F3184[arg1 * 10], 3);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E2098);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E2C6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E3088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E33A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E368C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E3B64);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E3E10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E3FB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E4180);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E4394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", BattleMenuDrawSlotNameList);

static void BattleMenuResetSelectorState(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        D_800F33A0[i] = 0;
    }

    D_800F5760 = 10;
    D_800F33AA = 0;
    D_800F5764 = 2;
}

static s32 BattleMenuGetSelectorStatus(void) {
    if (D_800F33A0[0] == 0) {
        return 0;
    }
    if (D_800F33AA == 3) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E4C08);

static void BattleMenuUpdateSelectorIcons(void) {
    s32 i;
    s32 index;

    for (i = 0; i < 3; i++) {
        index = (1 - ((D_800F338C[i] >> (D_800F5764 + 1)) & 0xF)) & 0xF;
        D_80163774[i] = D_800F332C[i][index];
    }
}

void BattleMenuResetSelectorStateAlt();
void BattleMenuResetSelectorStateAlt(void) {
    s32 i;
    s32 count;
    u8* p;

    for (i = 6, p = &D_800F33A0[6]; i >= 0; i--) {
        *p-- = 0;
    }
    D_800F5760 = 10;
    D_800F5764 = 2;
    D_800F33AA = 0;
    switch (Savemap.party[2].limit_level) {
    case 1:
        count = 3;
        break;
    case 2:
        count = 6;
        break;
    case 3:
        count = 9;
        break;
    case 4:
        count = 10;
        break;
    }
    D_800F5774 = 0;
    for (i = 0; i < count; i++) {
        if ((Savemap.party[2].limit_learn >> i) & 1) {
            D_80163B70[D_800F5774] = i;
            D_800F5774++;
        }
    }
}

static s32 BattleMenuGetSelectorStatusAlt(void) {
    if (D_800F33A0[0] == 0) {
        return 0;
    }
    if (D_800F33AA == D_800F5774) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E5530);

void BattleMenuUpdateSelectorIconsAlt(void) {
    s32 i;

    for (i = 0; i < D_800F5774; i++) {
        D_80163778[i] = D_800F33B0[D_80163B70[i]][(2 - D_800F338C[i] / 4) & 0xF];
    }
}

static void BattleMenuResetSelectorCursor(void) {
    D_800F3468 = 0;
    D_800F5760 = 10;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E58CC);

static u8 BattleMenuGetActivePartyByte(void) { return D_800F381C[D_80163604]; }

static s32 BattleAbsS32(s32 arg0) { return arg0 < 0 ? -arg0 : arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E5978);

// BUG: ret is not initialized
static s32 BattleMenuGetLastActivePartyId() {
    s32 i;
    s32 ret;

    for (i = 0; i < NUM_PARTY; i++) {
        if (Savemap.partyID[i] != 0xFF) {
            ret = Savemap.partyID[i];
        }
    }
    return ret;
}

// BUG: ret is not initialized
static s32 BattleMenuGetLastActivePartyIndex(void) {
    s32 i;
    s32 ret;

    for (i = 0; i < NUM_PARTY; i++) {
        if (Savemap.partyID[i] != 0xFF) {
            ret = i;
        }
    }

    return ret;
}

#ifndef NON_MATCHING
// might require a file split to match due to mis-aligned rodata
INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", BattleMenuIsMateriaColorBanned);
#else
s32 BattleMenuIsMateriaColorBanned(u32 arg0, s32 arg1) {
    s32 ret;

    ret = 0;
    switch (arg0) {
    case 0:
        if (arg1 == 4) {
            ret = 1;
        }
        break;
    case 1:
        if (arg1 == 10) {
            ret = 1;
        }
        break;
    case 2:
        if (arg1 == 5) {
            ret = 1;
        }
        break;
    case 3:
        if (arg1 == 9) {
            ret = 1;
        }
        break;
    case 4:
        if (arg1 == 8) {
            ret = 1;
        }
        break;
    case 5:
        ret = 1;
        break;
    }
    return ret;
}
#endif

void BattleMenuStripRestrictedMateria(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (Savemap.party[arg0].materia_weapon[i] != -1 &&
            BattleMenuIsMateriaColorBanned(arg1, SysMenuGetMateriaColorByType(Savemap.party[arg0].materia_weapon[i])) !=
                0) {
            Savemap.party[arg0].materia_weapon[i] = -1;
        }
        if (Savemap.party[arg0].materia_armor[i] != -1 &&
            BattleMenuIsMateriaColorBanned(arg1, SysMenuGetMateriaColorByType(Savemap.party[arg0].materia_armor[i])) !=
                0) {
            Savemap.party[arg0].materia_armor[i] = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E60F8);

static s32 BattleMenuGetStringWidth(void) { return SysGetSingleStringWidth(D_800F384A); }

static void BattleMenuDrawItemDescription(void) {
    SysMenuDrawString(0x10, 8, &D_800F3828[0], 7);
    SysMenuDrawString(0x2C, 0x1C, &D_800F3828[0x22], 7);
    SysMenuDrawString(BattleMenuGetStringWidth() + 0x4C, 0x1C, &D_800F3828[0x44], 7);
}

static void BattleMenuSetTargetMask(void) {
    switch (D_800F38A5) {
    case 0:
        D_801516F8 = D_8016375C;
        break;
    case 1:
        D_801516F8 = D_8016375E;
        break;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E6904);

// BUG function can return undefined value
static s32 BattleMenuFindTargetCharacterIndex(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if ((1 << D_800F38A7) & D_801516F8 & g_BattleMultiInfo.characterMask[i]) {
            return i;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E6B94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E6DCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle3", func_800E7170);
