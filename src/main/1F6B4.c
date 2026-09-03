//! PSYQ=3.3 CC1=2.7.2 G=8
#include "main_private.h"

s32 D_80062DCC = 0x00000000;
s32 D_80062DD0 = 0x00000000;
s32 D_80062DD4 = 0x00000000;
s16 g_RewardMenuState = 0x0000;
u8 D_80062DDA = 0x00;
u8 D_80062DDB = 0x00;
u8 D_80062DDC = 0x02;
static s8 _D_80062DDD = 0x00;
static s8 _D_80062DDE = 0x00;
static s8 _D_80062DDF = 0x00;
s32 D_80062DE0 = 0x00000000;
u8 D_80062DE4 = 0x00;
u8 D_80062DE5 = 0x00;
s16 D_80062DE6 = 0x00B4;
s16 D_80062DE8 = 0x0068;
s16 D_80062DEA = 0x0000;
s32 D_80062DEC = 0x801D0000;
s32 D_80062DF0 = 0x00000084;
s32 D_80062DF4 = 0xFFFFFFFF;
s32 g_PartyMenuListState = 0x00000001;

s32 g_RewardMenuHasEarnedItems;
u_long* g_CurrentMenuOrderingTable;
s32 D_80062EA8;
s32 D_80062EAC;
s32 D_80062EB0;
u32 D_80062EB4;
u8* D_80062EB8;
u8 D_80062EBC;
u32 D_80062EC0;
u32 D_80062EC4;
u32 D_80062EC8;
s32 D_80062ECC;

void func_8001155C(void);
void func_80014A00(s32* dst, s32* src, s32 len);
u16* func_80014D9C(s32, s32, s32);
s32 func_800150E4(u16*, u16*);
u16* func_800151F4(s32);
void func_80015CA0(GzHeader* src, s32* dst);
s32 func_8001AC9C(u8, s32);
s32 func_8001B834(s32);
void func_8001BD50(u8, u8, u8);
u8 func_8001F6B4();
void func_8001F6E4(
    s16 enabled, s16 x, s16 y); // PC: menu_setNotificationWindowPosition
void func_80026A34(s32 dfe, s32 dtd, u16 tpage, RECT* tw);

u8 func_8001F6B4(void) { return D_80062DDB; }

void func_8001F6C0(u8* arg0, s8 arg1) {
    D_80062DDB = 1;
    D_80062DDC = arg1;
    D_80062DE0 = 0x28;
    D_80062EB8 = arg0;
    D_80062DE5 = 1;
}

void func_8001F6E4(s16 arg0, s16 arg1, s16 arg2) {
    D_80062DE4 = arg0;
    if (arg0) {
        D_80062DE6 = arg1;
        D_80062DE8 = arg2;
    } else {
        D_80062DDB = 0;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8001F710);

void func_8001FA28(u16 arg0) {
    D_8009A000[0] = 0x30;
    D_8009A004[0] = arg0;
    D_8009A008[0] = arg0;
    SystemAkaoExecute();
}

void func_8001FA68(u16 arg0) {
    D_8009A000[0] = 0x28;
    D_8009A004[0] = 0x40;
    D_8009A008[0] = arg0;
    SystemAkaoExecute();
}

void func_8001FAAC(u16 arg0) {
    D_8009A000[0] = 0x29;
    D_8009A004[0] = 0x40;
    D_8009A008[0] = arg0;
    SystemAkaoExecute();
}

void func_8001FAF0(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8001FAF8);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8001FBAC);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8001FCDC);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8001FE6C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8001FF50);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8001FF8C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8001FFD4);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8002001C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80020058);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800206E4);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80020B68);

void func_80021044(DRAWENV* draw_env, DISPENV* disp_env) {
    VSync(0);
    SetDefDrawEnv(draw_env, 0, 0, 0x180, 0x1D8);
    draw_env[0].dfe = 1;
    draw_env[0].isbg = 1;
    PutDrawEnv(draw_env);
    VSync(0);
    SetDefDrawEnv(draw_env, 0, 8, 0x180, 0xE0);
    SetDefDrawEnv(&draw_env[1], 0, 0xF0, 0x180, 0xE0);
    SetDefDispEnv(&disp_env[0], 0, 0xE8, 0x16C, 0xF0);
    SetDefDispEnv(&disp_env[1], 0, 0, 0x16C, 0xF0);
    draw_env[1].isbg = 1;
    draw_env[0].isbg = 1;
    draw_env[1].dfe = 1;
    draw_env[0].dfe = 1;
    draw_env[1].dtd = 1;
    draw_env[0].dtd = 1;
    draw_env[0].r0 = 0;
    draw_env[0].g0 = 0;
    draw_env[0].b0 = 0;
    draw_env[1].r0 = 0;
    draw_env[1].g0 = 0;
    draw_env[1].b0 = 0;
    draw_env[0].tpage = draw_env[1].tpage =
        GetGraphType() != 1 && GetGraphType() != 2 ? 0x3F : 0xAF;
    VSync(0);
    PutDispEnv(disp_env);
    PutDrawEnv(draw_env);
    SetDispMask(1);
}

void func_800211B8(s32 arg0) { D_80062DEC = arg0; }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800211C4);

void func_8002120C(s32 arg0) {
    s32 prev;

    prev = D_80062DD4;
    D_80062DD4 = arg0;
    D_80062DD0 = prev;
    if (arg0 != 0 && (prev < 3 || prev > 4 || arg0 < 3 || arg0 > 4)) {
        func_800211C4(arg0);
    }
}

const char* func_80021258(s32 arg0) { return func_80015248(13, arg0, 8); }

void func_80021280(s32 arg0) { func_80015248(4, arg0, 8); }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800212A8);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021BAC);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021C4C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021D5C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021E70);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80021F58);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80022B5C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80022DE4);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80022FE0);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80023050);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8002305C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800230C4);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8002368C);

// Extract the hours field (0-99) of the HH:MM play-time clock from a seconds
// counter, capped at 99:59:59 (0x57E3F seconds). Returned as a plain decimal
// (tens*10 + units) so the 2-digit number drawer renders it. func_8002382C
// formats the matching minutes field.
s32 func_80023788(s32 arg0) {
    s32 var_a0;

    var_a0 = arg0;
    if (var_a0 > 0x57E3F) { // clamp to 99:59:59, in seconds
        var_a0 = 0x57E3F;
    }
    // tens-of-hours (sec / 36000) * 10 + units-of-hours ((sec % 36000) / 3600)
    return ((var_a0 / D_80049474[0]) * 0xA) +
           ((var_a0 % D_80049474[0]) / D_80049474[1]);
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_8002382C);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80023940);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80023AC4);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80023AD4);

// Push the current display and draw environments to the GPU: the per-frame
// double-buffer flip (activate the finished buffer for scanout, point drawing
// at the other one).
static void func_80024A04(void) {
    PutDispEnv(D_8007075C);
    PutDrawEnv(&D_80070700);
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80024A3C);

void func_80024D88(s32 arg0) {
    func_800211C4(0xD);
    do {
    } while (SystemCdromReadChain());
    VSync(30);
    func_801D131C(arg0);
}

void func_80024DD4(s32 arg0) {
    func_800211C4(0xE);
    do {
    } while (SystemCdromReadChain());
    func_801D1A6C(arg0);
}

void func_80024E18(s32 arg0) {
    func_800211C4(0xF);
    do {
    } while (SystemCdromReadChain());
    func_801D4118(arg0);
}

// This should be the title screen handler
void func_80024E5C(void) {
    func_800211C4(0x10); // load title screen?
    do {                 // wait until it's loaded?
    } while (SystemCdromReadChain());
    func_801D4CC0(); // jump into title screen loop?
}

void func_80024E94(void) {
    func_800211C4(0xA);
    do {
    } while (SystemCdromReadChain());
    func_801D1774();
}

void func_80024ECC(void) {
    func_800211C4(1);
    do {
    } while (SystemCdromReadChain());
    func_801D2D74();
}

void func_80024F04(void) {
    func_800211C4(1);
    do {
    } while (SystemCdromReadChain());
    func_801D2E84();
}

void func_80024F3C(s32 arg0) {
    func_800211C4(1);
    do {
    } while (SystemCdromReadChain());
    func_801D2F00(arg0);
}

void func_80024F80(s32 arg0) {
    func_800211C4(1);
    do {
    } while (SystemCdromReadChain());
    func_801D3138(arg0);
}

void func_80024FC4(s32 arg0) {
    func_800211C4(1);
    do {
    } while (SystemCdromReadChain());
    func_801D3018(arg0);
}

void func_80025008(void) {
    func_800211C4(1);
    do {
    } while (SystemCdromReadChain());
    func_801D3228();
}

// MENU event 0x18: snapshot each present party member's level into
// D_8009D44C[]. The endgame battle AI (Jenova-SYNTHESIS) counts how many of
// these are 99 to scale Safer-Sephiroth's HP.

#ifndef NON_MATCHINGS
INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SnapshotPartyLevels);
#else
void SnapshotPartyLevels(void) {
    s32 i;
    u16* present;
    for (i = 0, present = &D_8009D78A; i < 8; i++) {
        if ((*present >> D_80049500[i]) & 1) {
            D_8009D44C[i] = D_8009C738[D_80049500[i]].level;
        }
    }
}
#endif

void func_800250B4(void) {
    func_800211C4(0xC);
    do {
    } while (SystemCdromReadChain());
    ScalePartyHp();
}

void func_800250EC(s32 arg0) {
    func_800211C4(0xC);
    do {
    } while (SystemCdromReadChain());
    func_801D05C4(arg0);
}

void func_80025130(s32 arg0) {
    func_800211C4(0xC);
    do {
    } while (SystemCdromReadChain());
    func_801D0704(arg0);
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025174);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025288);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025310);

void func_80025360() { func_8001FA28(0x19F); }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025380);

s32 func_8002542C(s32 arg0) {
    s32 i;
    for (i = 0; i < MAX_MATERIA_COUNT; i++) {
        if (Savemap.materia[i] == -1) {
            Savemap.materia[i] = arg0;
            if (func_8002603C(arg0 & 0xFF) == 10) {
                Savemap.memory_bank_1[75] |= 1;
            }
            if ((arg0 & 0xFF) == 44) {
                Savemap.memory_bank_1[75] |= 2;
            }
            return -1;
        }
    }
    return arg0;
}

void func_800254D8(void) { D_80062EBC = 0; }

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800254E4);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025514);

void func_80025648(void) {}

void func_80025650(void) {}

// get party leader (Cloud) level
s32 func_80025658() { return D_8009C738[0].level; }

// Party slot -> equipped character -> that character's equipped armor's
// materia-slot configuration (slot count / linked-pair layout / growth rate;
// see ArmorRecord in main_private.h). Returns sentinel (void*)0xFF for an
// empty party slot.
u8* GetPartySlotArmorMateriaSlots(s32 arg0) {
    u8 temp_v1;
    u8* var_v0;

    temp_v1 = D_8009CBDC[arg0];
    var_v0 = (u8*)0xFF;
    if (temp_v1 != 0xFF) {
        u32 idx = g_BattleCharIdToCharId[temp_v1];
        var_v0 = g_ArmorTable[D_8009C755[idx * 0x84]].materiaSlot;
    }
    return var_v0;
}

// Weapon counterpart of GetPartySlotArmorMateriaSlots: party slot -> equipped
// character -> that character's equipped weapon's materia-slot configuration
// (see WeaponRecord in main_private.h). Returns sentinel (void*)0xFF for an
// empty party slot.
void* GetPartySlotWeaponMateriaSlots(s32 arg0) {
    u8 temp_v1;
    void* var_v0;

    temp_v1 = D_8009CBDC[arg0];
    var_v0 = (void*)0xFF;
    if (temp_v1 != 0xFF) {
        u32 idx = g_BattleCharIdToCharId[temp_v1];
        // SMELL: raw 0x84 char-record stride math; wants a CharacterRecord
        // struct (equippedWeapon at +0xC) ->
        // g_CharacterRecords[idx].equippedWeapon
        var_v0 = g_WeaponTable[D_8009C754[idx * 0x84]].materiaSlot;
    }
    return var_v0;
}

ArmorRecord* func_80025758(s32 armorId) { return &g_ArmorTable[armorId]; }

s32* func_80025774(s32 arg0) { return (s32*)&g_AccessoryTable[arg0]; }

ActiveCharacterData* func_80025788(s32 partyId) {
    if (Savemap.partyID[partyId] != 0xFF) {
        return &g_ActiveCharacters[partyId];
    }
    return (ActiveCharacterData*)0xFF;
}

void func_800257C4(void) {}

u8* GetCharacterName(s32 battleCharId) {
    return Savemap.party[g_BattleCharIdToCharId[battleCharId]].name;
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025800);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SystemMenuAddHpByPartyId);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025988);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SystemMenuAddMpByPartyId);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SystemMenuRemovePartyGold);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", SystemMenuAddPartyGold);

s32 SystemMenuGetPartyGold(void) { return Savemap.gil; }

void func_80025B8C(u_long* image) {
    RECT rect;
    rect.x = 0x340;
    rect.y = 0x184;
    rect.w = 0x30;
    rect.h = 0x78;
    StoreImage(&rect, image);
}

void func_80025BD0(u_long* image) {
    RECT rect;
    rect.x = 0x340;
    rect.y = 0x184;
    rect.w = 0x30;
    rect.h = 0x78;
    LoadImage(&rect, image);
}

void func_80025C14(u_long* image) {
    RECT rect;
    rect.x = 0x180;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 9;
    StoreImage(&rect, image);
}

void func_80025C54(u_long* image) {
    RECT rect;
    rect.x = 0x180;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 9;
    LoadImage(&rect, image);
}

void func_80025C94(u_long* image) {
    RECT rect;
    rect.x = 0x100;
    rect.y = 0x1ED;
    rect.w = 0x100;
    rect.h = 3;
    LoadImage(&rect, image);
}

void func_80025CD4(u_long* image) {
    RECT rect;
    rect.x = 0x100;
    rect.y = 0x1ED;
    rect.w = 0x100;
    rect.h = 3;
    StoreImage(&rect, image);
}

void func_80025D14(u_long* addr, s32 px, s32 py, s32 cx, s32 cy) {
    TIM_IMAGE tim;
    OpenTIM(addr);
    while (ReadTIM(&tim)) {
        if (tim.caddr) {
            tim.crect->x = cx;
            tim.crect->y = cy;
            LoadImage(tim.crect, tim.caddr);
            DrawSync(0);
        }
        if (tim.paddr) {
            tim.prect->x = px;
            tim.prect->y = py;
            LoadImage(tim.prect, tim.paddr);
            DrawSync(0);
        }
    }
}

// this function seems to be responsible of loading the characters' portrait
void func_80025DF8(void) {
    u8 dummy[8];
    u8 buf[0x1000];
    u_long* dst;
    s32 i;
    s32* sector_off;
    s32* length;
    s32 cx, cy;

    i = 0;
    dst = (u_long*)buf;
    sector_off = &D_80048FE8->sector_off;
    length = &D_80048FE8->length;
    for (; i < 9; i++) {
        func_80033F40(sector_off[i * 2], length[i * 2], dst, 0);
        cx = 0x340 + (i / 5) * 0x18;
        cy = 0x100 + (i % 5) * 0x30;
        func_80025D14(dst, cx, cy, 0x180, i);
        DrawSync(0);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80025ED4);

void func_80026034(void) {}

s32 func_8002603C(u8 arg0) {
    return D_80049520[D_80049528[g_MateriaData[arg0].materiaType & 0xF]];
}

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80026090);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800260DC);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_80026258);

INCLUDE_ASM("asm/us/main/nonmatchings/1F6B4", func_800262D8);
