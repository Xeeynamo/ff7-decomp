#include "battle_private.h"

const u8 D_800A0000 = 0;
const u8 D_800A0001 = 0;
const u16 D_800A0002 = 0;
const u8 D_800A0004[] = {
    0x14, 0x11, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x21, 0x37, 0x00, 0x00, 0x22, 0x00, 0x06,
    0x00, 0x26, 0x00, 0x12, 0x04, 0x21, 0x0A, 0x00, 0x00, 0x21, 0x09, 0x00, 0x00, 0x28, 0xB1, 0x00, 0x20, 0x2A, 0x11,
    0x00, 0x00, 0x2C, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x11, 0x04, 0x00, 0x00, 0x11, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x15, 0x11, 0x00,
    0x00, 0x16, 0x11, 0x00, 0x10, 0x1C, 0x11, 0x02, 0x00, 0x18, 0x11, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x2E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// opcode-byte program for func_800A1798's dispatch loop: a 0x1F-delimited
// stream of per-command opcode sequences, sliced by D_800F38AC[cmdIndex]
// (see func_800A283C) into per-command runs; each byte indexes D_800E7B28
// (function-pointer table) for func_800A1798 to jalr through in order
const u8 D_800A0098[] = {
    0x1F, 0x0E, 0x09, 0x1F, 0x00, 0x0C, 0x09, 0x1F, 0x01, 0x0C, 0x09, 0x1F, 0x02, 0x0D, 0x09, 0x1F, 0x1E, 0x09, 0x1F,
    0x0A, 0x16, 0x09, 0x1F, 0x1D, 0x09, 0x1F, 0x19, 0x09, 0x1F, 0x0E, 0x1C, 0x09, 0x1F, 0x0E, 0x1B, 0x09, 0x1F, 0x1A,
    0x09, 0x1F, 0x17, 0x1F, 0x03, 0x0C, 0x09, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x09, 0x1F, 0x04, 0x0B, 0x0F, 0x1F, 0x05,
    0x1F, 0x06, 0x0C, 0x09, 0x1F, 0x00, 0x0C, 0x09, 0x1F, 0x01, 0x0C, 0x09, 0x1F, 0x02, 0x0D, 0x09, 0x1F, 0x0E, 0x09,
    0x1F, 0x12, 0x0E, 0x09, 0x1F, 0x0E, 0x18, 0x09, 0x1F, 0x10, 0x0E, 0x09, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x0C,
    0x09, 0x1F, 0x08, 0x1F, 0x11, 0x1F, 0x13, 0x09, 0x1F, 0x14, 0x1F, 0x15, 0x0F, 0x1F, 0x00, 0x00, 0x00};
const s32 D_800A0108 = 21;
const s32 D_800A010C[] = {2, 22, 3, 23, 4};

// entrypoint
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1158);

// per-command opcode dispatcher: reads cmdIndex from the turn context
// (g_CurrentAction->unkC), looks up its opcode-sequence start via
// D_800F38AC[cmdIndex] into D_800A0098, then for each byte until the 0x1F
// delimiter, jalr's through D_800E7B28[opcode]. After each call, checks
// D_80062F14 -- if it goes >= 0 the whole sequence aborts immediately
// (handler requested a suspend, e.g. to wait on an animation), otherwise
// continues to the next opcode byte
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1798);

void BATTLE_SetFocusedActor(s32 arg0) {
    s32 i;

    if (D_800E7A38 != -1) {
        if (D_800E7A38 == arg0) {
            return;
        }
        for (i = 0; i < 64; i++) {
            if (D_800F5F44.messageQueue[i].unk0 == 6 &&
                D_800F5F44.messageQueue[i].unk2 == D_800E7A38) {
                break;
            }
        }
        if (i == 64) {
            g_CombatantTurnState[D_800E7A38].unk2A++;
            func_800A56B0(*(s16*)&D_800E7A38);
            func_800A7254(0, D_800E7A38, 0, 0);
        }
    }
    D_800E7A38 = arg0;
}

void func_800A23BC(s32 arg0) {
    if (D_800E7A38 == arg0) {
        D_800E7A38 = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A23E0);

void func_800A283C(void) {
    s32 next;
    s32* out;
    u32 i;
    u32 delim;

    i = 0;
    next = 0;
    delim = 0x1F;
    out = D_800F38AC;
    for (; i < 0x6D; i++) {
        if (i == next) {
            *out++ = i;
        }
        if (D_800A0098[i] == delim) {
            next = i + 1;
        }
    }
}

void BATTLE_CheckAllLucky7s(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (g_BattleState.combatant[i].curHP == 7777 &&
            !(g_CombatantTurnState[i].unk29 & 0x80)) {
            if ((*D_800F7DE2)++ < 64) {
                g_CombatantTurnState[i].unk29 |= 0x80;
                func_800A3E98(i, 1, 1, 0, 0);
            }
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2974);

void func_800A2B28(s32 arg0) {
    if (arg0 & 1) {
        g_CurrentAction->unk90 |= 0x80;
    }
    if (arg0 & 2) {
        g_CurrentAction->unk90 |= 0x40;
    }
    if (arg0 & 8) {
        g_CurrentAction->unk90 |= 0x04;
    }
    if (arg0 & 0x10) {
        g_CurrentAction->unk90 |= 0x800;
    }
    if (arg0 & 0xE0) {
        g_CurrentAction->unkE8 = (arg0 >> 5) * 10;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2BF4);

void BATTLE_QueueEffect(s32, s32, s32, s32, s32, s32, s32);
void func_800A2CC4(s32 arg0) {
    BATTLE_QueueEffect(
        g_CurrentAction->actorId, arg0, g_CurrentAction->unk28, g_CurrentAction->unk24, g_CurrentAction->unk98, 0, 0);
}

const u8 D_800A01A8[] = {0x05, 0x06, 0x07, 0x12, 0x0F, 0x00, 0x03, 0xA6};
s32 func_800A2D0C(void) {
    s32 temp_v1;

    if (g_CurrentAction->unk208 >= 3) {
        return g_BattleState.combatant[g_CurrentAction->unk208].unk11;
    }
    return D_800A01A8[g_CurrentAction->unkCC];
}

void func_800A2D68(u8 arg0) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (g_CurrentAction->unkD0[i] == 0xFF) {
            g_CurrentAction->unkD0[i] = arg0;
            return;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2DB0);

void func_800A2EFC(void) {
    D_800F3950 = D_800F3944;
    D_800F3954 = D_800F3948;
}

void func_800A2F24(void) {
    D_800F3944 = D_800F3950;
    D_800F3948 = D_800F3954;
}

Unk800A2F4C* func_800A2F4C(void) {
    Unk800A2F4C* unk = &D_80163798[D_800F3944];
    unk->unk3 = 0;
    unk->unk2 = 0;
    unk->unkA = D_800F3948;
    if (D_800F3944 < LEN(D_80163798)) {
        D_800F3944++;
    } else {
        func_800155A4(40);
    }
    return unk;
}

static Unk800FA9D0* func_800A2FD0(void) {
    Unk800FA9D0* ptr = &D_800FA9D0[D_800F3948];
    ptr->unk3 = -1;
    if (D_800F3948 < LEN(D_800FA9D0)) {
        D_800F3948++;
    } else {
        func_800155A4(40);
    }
    return ptr;
}

void BATTLE_DropSupersededQueuedActions(void) {
    s32 slot[10];
    s32 i;
    s32 actor;
    s32 prev;
    s32 none;

    none = -1;
    for (i = 0; i < 10; i++) {
        slot[i] = none;
    }
    for (i = 0; i < D_800F3948; i++) {
        actor = D_800FA9D0[i].unk0;
        if (actor != -1 && (D_800FA9D0[i].unk4 & 4)) {
            prev = slot[actor];
            if (prev != -1) {
                D_800FA9D0[prev].unk4 &= ~4;
            }
            slot[actor] = i;
        }
    }
}

static Unk800F9F3C* func_800A311C(Unk800FA9D0* arg0) {
    Unk800F9F3C* ptr = &D_800F9F3C[D_800F394C];

    arg0->unk3 = D_800F394C;
    ptr->unk0 = arg0->unk0;
    ptr->unk6 = -1;
    ptr->unk8 = -1;
    D_800F394C = (D_800F394C + 1) & 0x7F;
    return ptr;
}

void func_800A317C(void) {
    Unk800FA9D0* ret = func_800A2FD0();
    ret->unk0 = -1;
}

void func_800A31A0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Unk800A2F4C* unk = func_800A2F4C();
    unk->unk0 = arg0;
    unk->unk1 = arg1;
    unk->unk5 = arg2;
    unk->unk6 = arg3;
    unk->unk8 = -1;
    unk->unkA = -1;
}

static void func_800A3208(s8 arg0, s8 arg1) {
    if (D_800F3944 != 0) {
        Unk800A2F4C* ptr = &D_80163798[D_800F3944 - 1];
        ptr->unk3 = arg0;
        ptr->unk2 = arg1;
    }
}

static void func_800A3240(void) {
    if (D_800F3944 != 0) {
        D_80163798[D_800F3944 - 1].unk8 = -1;
    }
}

void func_800A3278(void) {
    D_800F3944 = 0;
    D_800F3948 = 0;
    D_80163798[0].unk0 = -1;
}

void func_800A329C(void) {
    if (D_800F3944) {
        D_800F3944--;
    }
}

void func_800A7254(s32, s32, s32, s32);
s32 func_800A37F8(s32);

static void func_800A32C0(s32 arg0) {
    s32 var_a3;

    if (D_800F5F44.D_800F7DAA != 0) {
        if (arg0 != 0) {
            if (D_800F5F44.D_800F7DBA == 6) {
                var_a3 = 1;
                if (D_800F5F44.D_800F6B9A != D_800F5F44.D_800F6BA1) {
                    var_a3 = 3;
                }
                func_800A7254(0, 0, 7, var_a3);
            }
        } else if (func_800A37F8(-1) != 0) {
            func_800A7254(0, 0, 7, 0);
        }
    }
}

void BATTLE_RunFrame();
void func_800155B0(void);
void func_800B6D6C();
void BATTLE_RunFrame(void) {
    s32 i;
    s32 a;

    func_800A32C0(D_800F3944);
    if (D_800F3944 != 0) {
        func_800A2F4C()->unk0 = -1;
    }
    func_800155B0();
    for (i = 0; i < 0x40; i++) {
        a = D_80163798[i].unk0;
        if (a == -1) {
            break;
        }
        if (a > 3 && a < 10) {
            D_801636B8[a].D_801636B9 = g_BattleState.combatant[a].unk10;
        }
    }
    func_800B6D6C();
    func_800A3278();
    for (i = 4; i < 0xA; i++) {
        D_801636B8[i].D_801636B9 = g_BattleState.combatant[i].unk10;
    }
}

void func_800A345C(void) {
    if (D_800F3944) {
        BATTLE_RunFrame();
    }
}

void func_800A3488(s32 arg0) {
    s32 i;

    for (i = 0; i < LEN(D_800FA9D0); i++) {
        Unk800FA9D0* p = &D_800FA9D0[i];
        if (p->unk0 == arg0) {
            p->unk4 &= ~4;
        }
    }
}

static void func_800A34CC(s32 arg0, s32 arg1, s8 arg2, s32 arg3) {
    s32 i = 0;

    for (; i < LEN(D_800FA9D0); i++) {
        Unk800FA9D0* p = &D_800FA9D0[i];
        if (p->unk0 != arg0 || p->unk2 != arg1) {
            continue;
        }
        if (arg3 != 1 || (p->unk4 & 4)) {
            p->unk2 = arg2;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3534);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A35F8);

s32 func_800A37F8(s32 arg0) {
    if (arg0 >= 0) {
        D_800F39E0 = arg0;
        D_800F39E4 = 0;
        return 0;
    }
    return D_800F39E0;
}

s32 func_800A3828(void) {
    s32 ret = 0;
    if (D_800F39E0 == 3) {
        D_800F39E4 += D_800F5F44.D_800F7DA6;
    }
    if (D_800F5F44.D_800F7DAA == 2) {
        switch (D_800F3896) {
        case 0:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 24:
        case 26:
        case 27:
            ret = 1;
            break;
        }
    }
    ret |= func_800A4A80();
    ret |= (g_BattleState.setupFlags & 3) ? 1 : 0;
    if (D_800F39E4 > 0x4000) {
        ret = 1;
    }
    return ret;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A38FC);

void func_800A3D4C(Unk800A3D4C* arg0) {
    s32 category;
    s32 i;

    category = arg0->unk0;
    for (i = 0; i < LEN(D_800F5F44.messageQueue); i++) {
        if (D_800F5F44.messageQueue[i].unk0 == 0xFF) {
            arg0->unk1 = D_800F5F44.unkC57[category];
            D_800F5F44.messageQueue[i] = *arg0;
            D_800F5F44.unkC57[category] += 1;
            D_800F5F44.D_800F7DDE = category;
            if (arg0->unk0 >= 2) {
                g_BattleState.combatant[arg0->unk2].unk4 &= ~0x20;
                if ((arg0->unk3 & 0x3F) == 0x13) {
                    g_BattleState.combatant[arg0->unk2].unk4 |= 0x20;
                }
            }
            return;
        }
    }
}

void func_800A3E98(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    Unk800A3D4C sp;

    sp.unk2 = arg0;
    sp.unk0 = arg1;
    sp.unk3 = arg2;
    sp.unk4 = arg3;
    sp.unk6 = arg4;
    func_800A3D4C(&sp);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3ED0);

void func_800A4350(s16 actorId, s16 cmdIndex, s16 attackIndex, u16 targetMask) {
    QueuedAction* entry;

    if (D_800F39D8 == ((D_800F39DC + 1) & 0xF)) {
        return;
    }

    entry = &D_800F3958[D_800F39DC];
    entry->priority = (cmdIndex == CMD_LIMIT) ? 5 : 6;
    entry->actorId = actorId;
    entry->cmdIndex = cmdIndex;
    entry->attackIndex = attackIndex;
    entry->targetMask = targetMask;

    // The three inventory-consuming commands all get this extra call --
    // compiles to retail's exact branch shape only as a switch (GCC's
    // binary-search lowering for these 3 sparse case values: pivot on
    // CMD_THROW, then a cmdIndex<9 range split between CMD_ITEM and
    // CMD_W_ITEM), not as a flat "||" chain.
    switch (cmdIndex) {
    case CMD_THROW:
    case CMD_ITEM:
    case CMD_W_ITEM:
        func_800A5660(actorId, attackIndex);
        break;
    }

    func_800A4D88(func_800A44D8(actorId));
    D_800F5F44.D_800F7DAC &= ~(1 << actorId);
    D_800F5F44.D_800F7DC2 |= 1 << actorId;
    D_800F39DC = (D_800F39DC + 1) & 0xF;
}

void func_800A4480(void) {
    s32 i;

    for (i = 0; i < LEN(g_CombatantTurnState); i++) {
        g_CombatantTurnState[i].unk3C = g_BattleState.combatant[i].curHP;
        g_CombatantTurnState[i].unk3E = g_BattleState.combatant[i].unk28;
    }
}

// Manipulate redirect: if arg0 (an enemy id) is currently manipulated
// (D_800F7DCA bit), return the party slot whose D_800F5E60[].unk6 is
// tracking it in place of arg0; otherwise arg0 passes through unchanged.
s32 func_800A44D8(s32 arg0) {
    s32 i;

    if (arg0 < 4) {
        goto end;
    }
    if (!((D_800F5F44.D_800F7DCA >> arg0) & 1)) {
        goto end;
    }
    for (i = 0; i < LEN(D_800F5E60); i++) {
        if (D_800F5E60[i].unk6 == arg0) {
            arg0 = i;
            goto end;
        }
    }
end:
    return arg0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4540);

void func_800A4844(s32 arg0) {
    s32 var_v0 = arg0 ? 3 : 1;
    D_800F39EC = var_v0;
}

s32 BATTLE_RunToResultScreen(void) {
    s32 ret;
    s32 i;

    ret = 0;
    D_800F39EC = 0;
    g_BattleState.setupFlags |= 2;
    for (i = 0; i < 3; i++) {
        func_800A7254(0, i, 4, 0);
    }
    for (i = 0; i < 4; i++) {
        BATTLE_RunFrame();
    }
    D_800F3896 = 0x1C;
    func_800D9E0C(-1, -1, 0x1C);
    while (D_800F39EC == 0) {
        BATTLE_RunFrame();
    }
    if (D_800F39EC & 2) {
        func_800E60F8();
        ret = 1;
    }
    return ret;
}

s32 BATTLE_RunEscapeSequence(void) {
    s32 ret;
    s32 i;

    ret = 0;
    D_800F39EC = 0;
    g_BattleState.setupFlags |= 2;
    for (i = 0; i < 3; i++) {
        func_800A7254(0, i, 4, 0);
    }
    for (i = 0; i < 4; i++) {
        BATTLE_RunFrame();
    }
    D_800F3896 = 9;
    func_800D9E0C(-1, -1, 9);
    while (D_800F39EC == 0) {
        BATTLE_RunFrame();
    }
    g_BattleState.setupFlags &= ~2;
    for (i = 0; i < 3; i++) {
        func_800A7254(0, i, 6, 0);
    }
    if (D_800F39EC & 2) {
        ret = 1;
    }
    return ret;
}

s32 func_800A4A80(void) {
    s32 ret;

    ret = 1;
    if (D_80163C7C > 3 && D_80163C7C < 6 && !(D_800F9DA4 & 1)) {
        if (D_800FAFDC) {
            ret = 1;
        } else {
            ret = 0;
        }
    }
    return ret;
}

void func_800A4ACC(s16 arg0, u16 arg1) { func_8001726C(arg0, arg1); }

// opcode 0x14 handler (D_800E7B28[0x14]): spins on func_800B6D6C() until
// status bit D_800F9DA4 & 2 clears. Not itself a damage dealer -- injecting
// cmdIndex 0x23 (single-opcode sequence: just this one) produced ~3.1%
// max-HP damage, but func_800B6D6C (still nonmatching, battle1 overlay) is
// just a generic drainer for the D_80163798 event queue (HP-counter ticks,
// status-icon show/hide, sound cues -- see its own comment in battle1.c),
// gated one-per-frame on D_800F7DE4 which func_800B7FDC sets. So this
// opcode is "wait for already-queued visual/counter effects to finish",
// not the source of the damage -- whatever queues an HP-tick entry into
// D_80163798 before this opcode runs is the real damage source, still
// untraced
void func_800B6D6C();
void func_800A4AF4(void) {
    while (D_800F9DA4 & 2) {
        func_800B6D6C();
    }
}

u8 func_800A4B3C(s32 index, s32 arg1) {
    if (arg1 != -1) {
        D_801518E4[index].D_8015190F = arg1;
    }
    return D_801518E4[index].D_8015190F;
}

void func_800A4B9C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4BA4);

s32 func_800A4CA8(s32 arg0) { return D_800F39F0[arg0][0]; }

s32 func_800A4CC8(s32 arg0) {
    s32 temp_v1;

    if (arg0 < 3) {
        temp_v1 = D_800F5E60[arg0].unk6;
        if ((temp_v1 >= 4) && ((D_800F5F44.D_800F7DCA >> temp_v1) & 1)) {
            arg0 = temp_v1;
        }
    }
    return arg0;
}

void func_800A4D2C(s32 arg0) {
    u32 i;

    if (g_BattleState.unk1A) {
        return;
    }
    for (i = 0; i < LEN(D_800E7A48); i++) {
        if (D_800E7A48[i] == arg0) {
            return;
        }
        if (D_800E7A48[i] == 0xFF) {
            D_800E7A48[i] = arg0;
            return;
        }
    }
}

void func_800A4D88(s32 arg0) {
    u32 i;

    for (i = 0; i < LEN(D_800E7A48); i++) {
        if (D_800E7A48[i] == arg0) {
            for (; i < LEN(D_800E7A48) - 1; i++) {
                D_800E7A48[i] = D_800E7A48[i + 1];
                if (D_800E7A48[i] == 0xFF) {
                    break;
                }
            }
            return;
        }
    }
}

s16 func_800A4E00(void) {
    s32 arg;
    s32 result;

    result = -1;
    arg = D_800E7A48[0] & 0xFF;
    if (arg != 0xFF) {
        arg = -1;
        result = func_800A4CC8(D_800E7A48[0]);
    }
    return result;
}

void func_800A4E40(void) {
    u8 temp_s0;

    temp_s0 = D_800E7A48[0];
    if (temp_s0 != 0xFF) {
        func_800A4D88(temp_s0);
        func_800A4D2C(temp_s0);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4E80);

void func_800A4F14(s32 arg0) {
    s32 temp_v0;

    temp_v0 = arg0 * 0x44;
    *(u16*)((u8*)&g_CombatantTurnState[0].unk8 + temp_v0) &= 0xFFFE;
    *(u8*)((u8*)&g_CombatantTurnState[0].unkE + temp_v0) |= 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4F60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A50E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5250);

void func_800A555C(s32 arg0, s32 arg1) {
    u8* row;
    u16 value;

    row = &D_8009D954[arg0 * 0x440];
    row += arg1 * 8;

    row[6] = 2;
    row[5] = D_800708D0[arg1][0];
    value = *(u16*)&D_800708C8[arg1 * 0x1C];
    arg1 -= 0x48;
    row[0] = arg1;
    row[1] = value;
}

typedef struct {
    s16 a;
    s16 b;
} Unk800F3A40;

extern Unk800F3A40 D_800F3A40[16];

void func_800A55BC(void) {
    s32 i;

    for (i = 0; i < 16; i++) {
        D_800F3A40[i].a = -1;
        D_800F3A40[i].b = -1;
    }
}

void func_800A55F4(s16 arg0, s16 arg1) {
    s32 i;

    for (i = 0; i < LEN(D_800F3A40); i++) {
        if (D_800F3A40[i].a == arg0 && D_800F3A40[i].b == arg1) {
            D_800F3A40[i].a = -1;
            D_800F3A40[i].b = -1;
            return;
        }
    }
}

void func_800A5660(s16 arg0, s16 arg1) {
    s32 i;

    for (i = 0; i < LEN(D_800F3A40); i++) {
        if (D_800F3A40[i].b == -1) {
            D_800F3A40[i].a = arg0;
            D_800F3A40[i].b = arg1;
            return;
        }
    }
}

// finds the free-list slot (D_800F3A40, see func_800A55BC/55F4/5660) whose
// `.b` matches arg0, and moves it into the D_800F3A20 ring buffer (write
// index D_800F3A1C, wraps at 16) before clearing the slot.
void func_800A56B0(s16 arg0) {
    s32 i;
    s16 entry;
    s32 nextIdx;
    s16* dst;

    for (i = 0; i < LEN(D_800F3A40); i++) {
        entry = D_800F3A40[i].b;
        if ((entry != -1) && (D_800F3A40[i].a == arg0)) {
            nextIdx = D_800F3A1C + 1;
            dst = &D_800F3A20[D_800F3A1C];
            D_800F3A1C = nextIdx;
            *dst = entry;
            D_800F3A1C = nextIdx & 0xF;
            D_800F3A40[i].a = -1;
            D_800F3A40[i].b = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5750);

// 0xFFFF when nothing is throwable, or the pick falls outside the range
s32 BATTLE_PickRandomThrowItem(void) {
    u16 list[320];
    s32 n;
    s32 i;
    s32 ret;
    s32 pick;
    s32 id;
    s32 flags;

    ret = 0xFFFF;
    n = 0;
    for (i = 0; i < 320; i++) {
        id = D_801671B8[i].id;
        flags = D_801671B8[i].unk4;
        if (id != 0xFFFF && !(flags & 9)) {
            list[n] = id;
            n++;
        }
    }
    if (n != 0) {
        pick = list[func_80014BA8(n)];
        if (pick >= 0x80 && pick < 0x100) {
            func_800A7254(0, 0, 0x10, pick);
            ret = pick;
        }
    }
    return ret;
}

const u8 D_800A0240[] = {
    0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0x54, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A,
    0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0x94, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54,
    0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0x14, 0x54, 0x0A, 0x80, 0x34, 0x54, 0x0A, 0x80, 0x74, 0x54, 0x0A, 0x80};
const u8 D_800A0278[] = {0x05, 0x06, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x00, 0x5C, 0x5A,
                         0x0A, 0x80, 0x88, 0x5A, 0x0A, 0x80, 0xA8, 0x5A, 0x0A, 0x80};
u8 func_800A5A5C(void) { return D_800A0278[SysGetRandomByteRange(7)]; }

s32 func_800A5A88(void) { return SysGetRandomByteRange(54); }

s32 func_800A5AA8(void) { return SysGetRandomByteRange(16) + 56; }

const u8 D_800A028C[] = {0x02, 0xFF, 0x01, 0x86};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5AC8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5BC8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5E0C);

s32 BATTLE_ExpandScriptToBuffer(u8* src, u16* patch) {
    u8 buf[0x100];
    s32 len;
    s32 slot;
    s32 i;

    len = func_800A5E0C(buf, src, patch);
    if (D_800F4300 + len > 0x800) {
        D_800F4300 = 0;
    }
    slot = D_800F4304++;
    D_800F4280[slot] = D_800F4300;
    D_800F4304 &= 0x3F;
    for (i = 0; i < len; i++) {
        D_800F3A80[D_800F4300 + i] = buf[i];
    }
    D_800F4300 += len;
    return slot;
}

s8* func_800A5F90(s32 arg0) { return &D_800F3A80[D_800F4280[arg0]]; }

s32 func_800A5FB0(u16* arg0, s32 arg1, s32 arg2) {
    s32 var_v1 = 0;
    u16* temp_a0;

    if (arg1 != -1) {
        arg1 = arg0[arg1];
        if (arg1 != 0xFFFF) {
            temp_a0 = arg0 + (arg1 >> 1);
            arg1 = temp_a0[arg2];
            if (arg1 != 0xFFFF) {
                var_v1 = (s32)temp_a0 + arg1;
            }
        }
    }
    return var_v1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6000);

void func_800B1D48(s32, s32, s32);
void func_800B2A2C(s32, s32);

void func_800A61D4(void) {
    s32 temp_v0;
    s32 var_s0;

    func_800B2A2C(-1, 0);
    for (var_s0 = 0; var_s0 < 8; var_s0 += 1) {
        if ((D_800F5F44.D_800F7DBC >> var_s0) & 1) {
            D_800F5F44.D_800F7DBC &= ~(1 << var_s0);
            temp_v0 = func_800A5FB0(D_800F5F44._5.unk0, g_BattleState.sceneID & 3, var_s0);
            if (temp_v0 != 0) {
                func_800B1D48(3, temp_v0, -1);
            }
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6278);

void func_800A64A0(s32 arg0, s8 arg1) { D_800E7A58[arg0] = arg1; }

u16 func_800A64B4(s32 arg0) {
    u16 var_a0;
    u8* countPtr;

    var_a0 = 0xFFFF;
    if (D_801671B8[arg0].count != 0) {
        D_801671B8[arg0].count -= 1;
        countPtr = &D_801671B8[arg0].count;
        var_a0 = D_801671B8[arg0].id;
        if (*countPtr == 0) {
            D_801671B8[arg0].id = 0xFFFF;
            D_801671B8[arg0].unk4 = 0xA;
        }
        D_80166F75 = 0xFF;
    }
    return var_a0;
}

void func_800A653C(s32 arg0) {
    s32 index = func_800A44D8(arg0);
    g_CombatantTurnState[index].unk4 = 0;
    D_800F5F44.D_800F7DC2 &= ~(1 << index);
}

void func_800A6590(s32 arg0) { func_800A4D88(arg0); }

void func_800A65B0(s32 arg0, s32 arg1) {
    u16* p;

    if (arg0 < 3) {
        func_800A4F14(arg0);
        D_8009D866[arg0].unk0 = 0;
        p = &D_80163762;
        *p &= ~(1 << arg0);
    }
}

void func_800A661C(s32 arg0) {
    func_800A4D88(arg0);
    if ((D_800F5F44.D_800F7DAC >> arg0) & 1) {
        if (g_CombatantTurnState[arg0].unk4 == 0xFFFF) {
            func_800A4D2C(arg0);
            return;
        }
        D_800F5F44.D_800F7DAC &= ~(1 << arg0);
    }
}

void func_800A66A4(s32 arg0, s32 arg1) {
    func_800A653C(arg0);
    func_800A65B0(arg0, arg1);
    func_800A4D88(arg0);
    D_800F5F44.D_800F7DAC &= ~(1 << arg0);
    D_800F5F44.D_800F7DC4 &= ~(1 << arg0);
}

void func_800A6720(s32 arg0, s16 arg1) { func_800A5660(10, arg1); }

void func_800A6748(s32 arg0) {
    func_800A653C(arg0);
    func_800A4D88(arg0);
    D_800F5F44.D_800F7DAC &= ~(1 << arg0);
}

void func_800A6798(s32 arg0, s32 arg1) { func_800A37F8(arg1); }

void func_800A67B8(s32 arg0) {
    func_800A4D88(arg0);
    D_800F5F44.D_800F7DC4 |= 1 << arg0;
    if ((D_800F5F44.D_800F7DAC >> arg0) & 1) {
        func_800A4350(arg0, func_800A4CA8(arg0), 0, 0);
    }
}

void func_800A6834(s32 arg0) { D_800F5F44.D_800F7DC4 &= ~(1 << arg0); }

void func_800A6858(s32 arg0, s32 arg1) {
    if (arg1 != 0) {
        func_800A66A4();
        D_800F5F44.D_800F7DCA |= 1 << arg0;
        return;
    }

    D_800F5F44.D_800F7DCA &= ~(1 << arg0);
    if ((D_800F5F44.D_800F7DAC >> arg0) & 1) {
        func_800A4D88(arg0);
        func_800A4350(arg0, -1, 0, 0);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A68FC);

void func_800A6A3C(s32 arg0, s32 arg1) { g_CombatantTurnState[arg0].unkE |= arg1; }

void func_800A555C();

void func_800A6A70(s32 arg0) {
    func_800A555C();
    g_CombatantTurnState[arg0].unkE |= 9;
}

void func_800A6AC4(void) {
    D_800F5F44.D_800F7DC4 &= 0xFC0F;
    D_800F5F44.D_800F7DCA &= 0xFC0F;
    D_800F5F44.D_800F7DAC &= 0xFC0F;
    D_800F5F44.D_800F7DC2 &= 0xFC0F;
}

void func_800A6B1C(void) {
    s32 i;
    u32 scratch;
    u16 pending;

    for (i = 0; i < 3; i++) {
        if (!((D_800F5F44.D_800F7DC4 >> i) & 1)) {
            scratch = D_800F5F44.D_800F7DC2;
            pending = scratch;

            if ((pending >> i) & 1) {
                D_800F5F44.D_800F7DC2 = pending & ~(1 << i);
                D_800F5F44.D_800F7DAC |= 1 << i;
            }
        }
    }
}

void func_800A6B88(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < 0x140; i++) {
        if (D_801671B8[i].id == arg1) {
            if (!(D_801671B8[i].unk4 & 9)) {
                func_800A64B4(i);
            }
            return;
        }
    }
}

void func_800A6BFC(void) {}

extern s32 SysGetPtrToKernBattleTxtWithId(s32);

void func_800A6C04(s32 arg0) {
    s16 sp10;

    sp10 = (s16)D_801636B8[arg0].D_801636B8;
    D_800F5F44.D_800F7DBE = BATTLE_ExpandScriptToBuffer(SysGetPtrToKernBattleTxtWithId(0x26), &sp10) + 0x100;
    D_800F5F44.D_800F7DC0 = 0xF;
}

void func_800A6C5C(s32 arg0, s32 arg1) {
    func_800A7254(2, arg0, 0x14, arg1);
    *(u16*)((u8*)&g_BattleState.combatant[arg0].unk44 + 0xE) = arg1;
}

extern const u8 g_StatusBitTable[];

void BATTLE_ClearStatusBit(s32 arg0, s32 arg1) {
    u32 mask = ~(1 << g_StatusBitTable[arg1]);
    g_BattleState.combatant[arg0].status &= mask;
}

void BATTLE_ExpireDeathSentence(s32 arg0) { func_800A3E98(arg0, 3, 2, 54, 0); }

void func_800A6D3C(s32 arg0) {
    s32 temp_v1;

    temp_v1 = g_BattleState.combatant[arg0].status;
    if (temp_v1 & 0x2000) {
        g_BattleState.combatant[arg0].status = (temp_v1 & ~0x2000) | 0x4000;
    }
}

void BATTLE_TickPoison(s32 arg0) {
    if (g_BattleState.combatant[arg0].status & 8) {
        g_CombatantTurnState[arg0].unk14[2] = 0xA;
        func_800A3E98(arg0, 3, 0x23, 0, 0);
    }
}

void func_800A6DFC(void) {}

void func_800A6E04(void) {}

void func_800A6E0C(s32 arg0) {
    if (arg0 < 3) {
        D_800F5E60[arg0].limitBarUI = 0;
        D_800F5E60[arg0].limitBar = 0;
        func_800A7254(0, arg0, 1, 0);
    }
}

void func_800A7254(s32, s32, s32, s32);
void func_800A6E6C(s32 arg0, s32 arg1) { func_800A7254(0, arg0, 13, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6E9C);

void func_800B0FFC(s32, s32, s32, s16*);
void func_800A7034(s32 arg0, s16 arg1) {
    s16 out = arg1;
    func_800B0FFC(arg0, 0x53, 1, &out);
}

void func_800A7060(s32 arg0, s32 arg1) { func_800A7254(0, arg0, 12, arg1); }

void func_800A7090(s32 arg0) { g_CombatantTurnState[arg0].unk29 |= 0x40; }

void func_800A70C4(s32 arg0, s32 arg1) {
    BATTLE_QueueEffect(arg0, 0x34, 2, D_800708D0[arg1][1], 0, 9, g_BattleState.combatant[arg0].status);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7130);

void func_800A71E0(void) {}

s32 func_800A71E8(s32 arg0) { return (arg0 + 1) & 0x7F; }

void func_800A71F4(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 3; i++) {
        for (j = 0x7F; j >= 0; j--) {
            D_800F4308[i][j].unk0 = 0xFF;
        }
        D_800F4908[i] = 0;
        D_800F4914[i] = 0;
    }
}

void func_800A7254(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32* base;
    s32* temp_s0;
    s32 temp_t0;
    Unk800F4308* temp_a0;

    base = D_800F4914;
    temp_s0 = base + arg0;
    temp_t0 = *temp_s0;
    temp_a0 = &D_800F4308[arg0][temp_t0];
    if (temp_a0->unk0 == 0xFF) {
        temp_a0->unk2 = arg3;
        temp_a0->unk1 = arg2;
        temp_a0->unk0 = arg1;
        *temp_s0 = func_800A71E8(temp_t0);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A72C8);

void BATTLE_ResolveMagicActionIndex(void) {
    g_CurrentAction->absoluteActionIndex = g_CurrentAction->relativeActionIndex;
}

void BATTLE_ResolveSummonActionIndex(void) {
    g_CurrentAction->absoluteActionIndex = g_CurrentAction->relativeActionIndex + 56;
}

void func_800A55F4(s16, s16);
void func_800A73F8(void) {
    g_CurrentAction->absoluteActionIndex = g_CurrentAction->relativeActionIndex;
    g_CurrentAction->unk24 = g_CurrentAction->relativeActionIndex;
    func_800A55F4(g_CurrentAction->actorId, (s16)g_CurrentAction->absoluteActionIndex);
    if (!(g_CurrentAction->allowedTargetsMask & 0xF)) {
        g_CurrentAction->unk20 = 0x21;
    } else {
        g_CurrentAction->unk20 = 0x20;
    }
}

void BATTLE_SetupThrowAction(void) {
    s32 id;
    s32 weapon;

    if (g_CurrentAction->relativeActionIndex == 0xFFFF) {
        g_CurrentAction->relativeActionIndex =
            BATTLE_PickRandomThrowItem() & 0xFFFF;
    }
    id = g_CurrentAction->relativeActionIndex;
    if (id != 0xFFFF) {
        g_CurrentAction->absoluteActionIndex = id;
        g_CurrentAction->unk98 = g_CurrentAction->relativeActionIndex;
        g_CurrentAction->unk24 = g_CurrentAction->relativeActionIndex - 0x80;
        func_800A55F4(
            g_CurrentAction->actorId, g_CurrentAction->absoluteActionIndex);
        g_CurrentAction->unk48 = 0x10;
        weapon = g_CurrentAction->unk24;
        g_CurrentAction->unkD8 = g_WeaponTable[weapon].attack +
                                 D_8009D84C[g_CurrentAction->actorId].strength;
        g_CurrentAction->unk68 = g_WeaponTable[weapon].impactEffect;
    } else {
        g_CurrentAction->unk20 = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7560);

void BATTLE_ResolveEnemySkillActionIndex(void) {
    g_CurrentAction->absoluteActionIndex = g_CurrentAction->relativeActionIndex + 72;
}

static u32 func_800B12DC(void);
void func_800A76CC(void) {
    s32 val;

    g_CurrentAction->unk20 = -1;
    if (func_800B12DC() != 0) {
        val = 4;
        if (g_BattleState.combatant[g_CurrentAction->actorId].unk4 & 0x40) {
            val = 3;
        }
        g_CurrentAction->unk20 = val;
        g_BattleState.combatant[g_CurrentAction->actorId].unk4 ^= 0x40;
    }
}

void func_800A7784(void) {}

// actorId here is the live party slot (0-2, indexes D_800F5E60's 3-element
// gauge table below) -- NOT the per-character Limit-name block index. Each
// of the 9 playable characters has a uniform 7-slot block in the shared
// name table (relativeActionIndex 0x00=Cloud, 0x07=Barret, 0x0E=Aerith,
// 0x15=Tifa, 0x1C=Cid, 0x23=Red XIII, 0x2A=Cait Sith+Vincent shared,
// 0x31=Yuffie); which block applies for the current actor is resolved
// elsewhere, not yet found in decompiled code.
void BATTLE_ResolveLimitActionIndex(void) {
    s32 actorId;
    s32 relativeActionIndex;

    actorId = g_CurrentAction->actorId;
    if (actorId >= 4) {
        func_800155A4(0x25, actorId);
        return;
    }
    relativeActionIndex = g_CurrentAction->relativeActionIndex;
    g_CurrentAction->absoluteActionIndex = relativeActionIndex;
    if (relativeActionIndex < 0x60) {
        s32 off = actorId * 0x34;
        g_CurrentAction->absoluteActionIndex = relativeActionIndex + 0x80;
        *(u16*)((u8*)D_800F5E60 + off + 8) = 0; // ideally D_800F5E60[actorId].limitBar = 0;
        D_800F5E60[actorId].limitCount++;
        if (!(g_BattleState.setupFlags & 8)) {
            func_800A7254(2, actorId, 0x11, 0);
        }
    }
}

const s16 D_800A0290[] = {0, 56, 72, 96, 256};
const s32 D_800A029C[] = {
    0x140D0302, 0x3D3CFFFF, 0x41403F3E, 0xFFFFFF42, 0xFFFFFFFF, 0x43424140, 0x47464544, 0xFF444843, 0xFFFFFFFF};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A784C);

void func_800A7940(void) {
    g_CurrentAction->unk80 = 0x400000;
    g_CurrentAction->unkE4 = 0x59;
}

void func_800B1060(s32);
void func_800A795C(void) { func_800B1060(g_CurrentAction->relativeActionIndex); }

void func_800AF9C8();
void func_800A7988(void) { func_800AF9C8(); }

void func_800A79A8(void) {
    g_CurrentAction->unk50 = 0;
    g_CurrentAction->allowedTargetsMask = 1 << g_CurrentAction->actorId;
}

void func_800A79CC();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A79CC);

void BATTLE_LoadActionAttackData(void) {
    AttackData* atk;
    u16 elements;

    g_CurrentAction->unk3C = 0xFF;
    atk = &D_800722CC[g_CurrentAction->absoluteActionIndex];
    g_CurrentAction->unk40 = atk->damageCalcID;
    g_CurrentAction->unk48 = atk->strength;
    elements = atk->elements;
    if (elements != 0xFFFF) {
        g_CurrentAction->unk44 = elements;
    }
    g_CurrentAction->unk60 = atk->cameraSingleID;
    g_CurrentAction->unk64 = atk->cameraSingleID;
    g_CurrentAction->unk24 = atk->attackEffectID;
    g_CurrentAction->unk6C = atk->flags;
    func_800A8D60(atk->targetFlags);
    BATTLE_SetActionStatusChange(atk->statusChange, atk->statuses);
    func_800A8D88(atk->additionalEffects, atk->effectsModifier);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A81B8);

void BATTLE_QueueCurrentActionEffect(void) {
    Unk800A2F4C* unk;
    Unk800FA9D0* act;

    if (g_CurrentAction->unk20 >= 0) {
        unk = func_800A2F4C();
        unk->unk0 = g_CurrentAction->actorId;
        unk->unk1 = g_CurrentAction->unk1C;
        unk->unk5 = g_CurrentAction->unk20;
        unk->unk3 = g_CurrentAction->unk28;
        unk->unk2 = g_CurrentAction->unk24;
        unk->unk8 = g_CurrentAction->unk60;
        unk->unk4 = 0;
        act = func_800A2FD0();
        act->unk0 = g_CurrentAction->actorId;
        act->unk1 = g_CurrentAction->actorId;
        act->unk2 = 0;
        act->unk4 = 0;
        func_800A317C();
    }
}

void func_800A8528(void) { g_CurrentAction->unkB4 = 4; }

void func_800A6000(s32, s32, s32);

void func_800A853C(void) {
    s32 var_s0;

    for (var_s0 = 0; var_s0 < 0xA; var_s0 += 1) {
        if ((g_CurrentAction->allowedTargetsMask >> var_s0) & 1) {
            func_800A6000(var_s0, g_CurrentAction->relativeActionIndex, 0);
        }
    }
}

void func_800A85A0(void) { g_CurrentAction->unkB4 = 2; }

void func_800A85B4(void) {
    g_CurrentAction->unk44 = 0x10;
    g_CurrentAction->unk48 = 1;
    g_CurrentAction->unk50 = 0;
    if (!((D_80163758[1] >> g_CurrentAction->actorId) & 1)) {
        g_CurrentAction->unk20 = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A85FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A866C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8A6C);

void func_800A8C94(void) {
    g_CurrentAction->unk8C = 0xFF;
    g_CurrentAction->unk40 = 0xB0;
    g_CurrentAction->unk80 |= 1;
    g_CurrentAction->unk3C = (s32)g_CurrentAction->unk3C >> 1;
}

void func_800A8CC8(void) {
    g_CurrentAction->unk6C &= ~0x2000;
    g_CurrentAction->unk3C /= 3;
}

void func_800A8D04(void) { g_CurrentAction->unk48 = 2; }

// seed this combatant's unk50 (a flag word later read by the damage formula
// in func_800AD804 -- bit 0x80 there appears to exempt a hit from the
// reduced per-target damage otherwise applied when an action strikes
// multiple targets) with a per-slot default, but only if nothing has set
// unk50 explicitly yet this turn (see func_800A8D60's sentinel check)
void func_800A8D60(s32 arg0);
void func_800A8D18(void) { func_800A8D60(D_800F5EFC[g_CurrentAction->actorId * 0x18]); }

void func_800A8D60(s32 arg0) {
    if (g_CurrentAction->unk50 == 0xFF) {
        g_CurrentAction->unk50 = arg0;
    }
}

static void func_800A8D88(s32 arg0, s32 arg1) {
    g_CurrentAction->unkBC = -1;
    if (arg0 != 0xFF) {
        g_CurrentAction->unkBC = arg0;
        g_CurrentAction->unkC0 = arg1;
        func_800A8E84(2);
    }
}

void BATTLE_SetActionStatusChange(u32 arg0, s32 arg1) {
    u8 unused[8]; // retail reserves it, nothing reads it
    Unk800A8D04* act = g_CurrentAction;
    s32 idx = arg0 >> 6;
    s32 v;
    s32 slot;
    s32 tmp;

    act->unk80 = 0;
    act->unk84 = 0;
    act->unk88 = 0;

    if (idx < 3) {
        v = (arg0 & 0x3F) * 4;
        slot = idx;
        tmp = 0x80000000;

        if (arg1 < 0) {
            act->unk80 = tmp;
            D_800F5F44.D_800F7DC6 = arg1 & 3;
        } else {
            act->unk8C = v;
            tmp = (s32)act;
            *(s32*)((slot * 4) + tmp + 0x80) = arg1;
        }
    }
}

void func_800A8E34(void) { func_800A79CC(); }

void func_800A8E54(s32 arg0) {
    g_CurrentAction->unkF8 = arg0;
    g_CurrentAction->unkAC = arg0 + 3;
    if (g_CurrentAction->unkAC > 8) {
        g_CurrentAction->unkAC = 8;
    }
}

const s16 D_800A02C0[] = {
    0x04, 0x3C, 0x04, 0x20, 0x01, 0x24, 0x10, 0x10, 0x04, 0x02, 0x02, 0x02, 0x02, 0x01, 0x20, 0x04, 0x24, 0x10,
    0x10, 0x04, 0x20, 0x10, 0x10, 0x10, 0x30, 0x10, 0x20, 0x10, 0x10, 0x14, 0x01, 0x01, 0x01, 0x01, 0x01, 0x18};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8E84);

// Cait Sith's "Slots" limit: check the 3 landed reel symbols against each of
// D_800E7BA4's 7 known combos in order. comboIndex==0 (Bar/Bar/Bar) means
// "cast a random Summon"; comboIndex 1-6 select one of the other 6 named
// results (Game Over, Death Joker, Toy Soldier, Lucky Girl, Moogle Dance,
// Transform -- kernel.bin section 18, absolute 105-110); falling off the end
// unmatched (comboIndex==7) lands on the generic "Toy Box" fallback (111).
static void BATTLE_ResolveCaitSithSlotsResult(void) {
    s32 savedUnk20;
    s32 rollSum;
    s32 comboIndex;

    comboIndex = 0;
    while (comboIndex < 7) {
        if (D_80163774[0] == D_800E7BA4[comboIndex][0] && D_80163774[1] == D_800E7BA4[comboIndex][1] &&
            D_80163774[2] == D_800E7BA4[comboIndex][2]) {
            break;
        }
        comboIndex++;
    }
    if (comboIndex) {
        g_CurrentAction->absoluteActionIndex = comboIndex + 0x68;
    } else {
        // Random Summon ID: sum of four Rnd(1..10) rolls, + Level/21, /2, -4,
        // clamped [0,15], then the Summon category base (D_800A0290[1] ==
        // 0x38 == 56).
        rollSum = 4;
        for (comboIndex = 0; comboIndex < 4; comboIndex++) {
            rollSum += SysGetRandomByteRange(10) & 0xFF;
            SysIncSeedForRandom();
        }
        rollSum += g_CurrentAction->characterLevel / 21;
        rollSum /= 2;
        rollSum -= 4;
        if (rollSum < 0) {
            rollSum = 0;
        }
        if (rollSum > 0xF) {
            rollSum = 0xF;
        }
        g_CurrentAction->absoluteActionIndex = rollSum + 0x38;
        g_CurrentAction->unk28 = 3;
    }
    g_CurrentAction->unk50 = 0xFF;
    g_CurrentAction->unk98 = g_CurrentAction->absoluteActionIndex;
    savedUnk20 = g_CurrentAction->unk20;
    func_800A8E34();
    g_CurrentAction->unk20 = savedUnk20;
    g_CurrentAction->unk38 = 0;
}

const u8 D_800A0398[] = {0x64, 0x14, 0x14, 0x14, 0xEC, 0xCE, 0xCE, 0x00};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A9DA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA1C8);

static void func_800AA468(void) {
    s32 temp_s0;
    s32 var_s1;

    var_s1 = g_CurrentAction->unkC8;
    if (func_800B10B4(g_CurrentAction->actorId)) {
        var_s1 |= 2;
    }
    temp_s0 = SysCountActiveBits(var_s1 & 0x0400029A);
    temp_s0 += SysCountActiveBits(var_s1 & 0x202000) * 2;
    g_CurrentAction->unk214 *= temp_s0 + 1;
}

static void func_800AA4FC(void) {
    s32 var_s0;

    var_s0 = 1;
    if (func_800B10B4(g_CurrentAction->actorId) != 0) {
        var_s0 = 2;
    }
    if (g_CurrentAction->unkC8 & 0x200000) {
        var_s0 *= 4;
    }
    g_CurrentAction->unk214 *= var_s0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA574);

void func_800AA688(void) {
    s32 var_a0;
    s32 var_a1;

    var_a1 = 1;
    for (var_a0 = 0; var_a0 < 3; var_a0++) {
        if (g_BattleState.combatant[var_a0].status & 1) {
            var_a1 += 1;
        }
    }
    g_CurrentAction->unk214 *= var_a1;
}

s32 func_800AA6E8(s32 arg0, s32 arg1) {
    arg0 = arg0 < 4 ? 1 : 0;
    if (arg1 < 4) {
        arg0++;
    }
    return arg0 & 1;
}

static s32 func_800AA700(s32 arg0) {
    s32 var_v0;

    var_v0 = 0xF;
    if (arg0 < 4) {
        var_v0 = 0x3F0;
    }
    return func_800B3030(*D_80163758 & var_v0) & 0xFFFF;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA738);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA950);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AABBC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB308);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB480);

void BATTLE_DropDyingEnemiesFromTargets(void) {
    s32 mask;
    s32 i;

    if (!(g_CurrentAction->unk90 & 0x10) && g_CurrentAction->actorId < 3 &&
        ((g_CurrentAction->unk44 & 0x1C00) || g_CurrentAction->unk28 == 5)) {
        mask = g_CurrentAction->allowedTargetsMask;
        for (i = 4; i < 10; i++) {
            if (g_BattleState.combatant[i].unk4E >= 16) {
                mask &= ~(1 << i);
            }
        }
        if (mask != g_CurrentAction->allowedTargetsMask) {
            if (D_8009D84C[g_CurrentAction->actorId].characterFlags & 4) {
                g_CurrentAction->unk90 |= 0x20000;
            } else {
                g_CurrentAction->allowedTargetsMask = mask;
                if (mask == 0) {
                    g_CurrentAction->unkDC = 0x77;
                }
            }
        }
    }
}

void BATTLE_LearnEnemySkill(void) {
    u16 id;
    s32 bit = 1 << (g_CurrentAction->absoluteActionIndex - 0x48);
    s32* flags;
    s32 mask;

    if (!(D_8016376A & 0x40)) {
        flags = (s32*)((u8*)g_CurrentAction->unk204 + 0x24);
        mask = *flags;

        if (!(mask & bit)) {
            *flags = mask | bit;
            id = (u16)g_CurrentAction->absoluteActionIndex;
            func_800B0FFC(g_CurrentAction->unk208, 0x73, 1, &id);
            func_800A7254(2, g_CurrentAction->unk208, 0x12, id);
            g_CurrentAction->unk224 = 0xA;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB830);

void func_800AB830(s32, s32);

void func_800AB9C4(s32 arg0, s32 arg1) {
    Unk800A2F4C* temp_v0;

    if (!(g_BattleState.combatant[arg0].status & 1)) {
        temp_v0 = func_800A2F4C();
        temp_v0->unk1 = 1;
        temp_v0->unk5 = 0x2E;
        temp_v0->unk0 = arg0;
        temp_v0->unk3 = 0;
        temp_v0->unk2 = 0;
        temp_v0->unk8 = -1;
        temp_v0->unk6 = 0;
        temp_v0->unk4 = 0;
        func_800AB830(arg0, arg1);
        func_800A317C();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ABA68);

// mutually exclusive status pairs -- row 0 Slow/Haste, row 1 Sadness/Fury.
// func_800ABB0C queues the partner for removal when one is applied; for
// row 1 an already-held partner cancels the incoming status instead
const s32 D_800A03A0[2][2] = {{0x200, 0x100}, {0x010, 0x020}};

void func_800ABB0C(s32 arg0, s32 arg1) {
    Unk800FA9D0* act;
    s32 cap;
    s32 capMP;
    s32 oldStatus;
    s32 newStatus;
    s32 mask;
    s32 flags;
    s32 bounceTarget;
    s32 i;
    s32 j;
    s32 isReflected;
    Unk800AF470* entry;

    // grab a free action-result slot, tag it attacker/target, clear the
    // "just processed" marker on the target
    act = func_800A2FD0();
    act->unk0 = arg1;
    act->unk1 = arg0;
    act->unk4 = 0;
    g_BattleState.combatant[arg1].unk17 = 0xFF;
    func_800AA950(act);
    func_800AC73C(act->unk0);
    if (act->unk0 != arg1) {
        // target got redirected (e.g. covered by another actor) -- flag it
        func_800A3240();
        g_CurrentAction->unk218 |= 0x20;
    }

    // reload the (possibly redirected) target id, then run the damage/effect
    // calculation pipeline for this hit
    arg1 = g_CurrentAction->unk208;
    func_800AE82C();
    func_800AB308();
    if (g_CurrentAction->unk44 & 0x200) {
        g_CurrentAction->unk220 |= 1;
    }
    if (!(g_CurrentAction->unk6C & 1)) {
        g_CurrentAction->unk220 |= 4;
    }
    if (g_BattleState.combatant[arg1].unk4 & 0x4000) {
        // target already marked -- treat as an automatic miss/no-effect
        g_CurrentAction->unk218 |= 1;
    }
    if (!(g_CurrentAction->unk218 & 1)) {
        func_800AD4EC();
    }
    func_800A8E84(3);
    if (g_CurrentAction->unk48 == 0) {
        g_CurrentAction->unk218 |= 2;
    }
    if (func_800ACD88(arg1) != 0) {
        g_CurrentAction->unk230 = 0x20;
    }

    // Reflect check: bounce the effect back instead of applying it here
    isReflected = 0;
    func_800AB480();
    if (!(g_CurrentAction->unk6C & 0x200) && !((D_800F4958 >> arg1) & 1)) {
        isReflected = (g_CurrentAction->unk228 >> 18) & 1;
    }
    if (!(g_CurrentAction->unk6C & 0x100) && !isReflected && !(g_CurrentAction->unk228 & 1) &&
        !(g_CurrentAction->unk230 & 0xC1)) {
        g_CurrentAction->unk218 |= 1;
    }

    if (!(g_CurrentAction->unk218 & 1)) {
        // hit actually lands on the target
        g_CurrentAction->unkE0++;
        act->unk4 |= 1;
        func_800A8E84(4);
        if (g_CurrentAction->actorId != arg1) {
            g_CurrentAction->unk78 |= 1 << arg1;
        }
        if ((g_CurrentAction->unk218 & 4) && (g_CurrentAction->unkB0 < 9)) {
            func_800A2974();
        }
        if (isReflected) {
            // pick who the effect bounces to (self, or a cycled ally) and
            // flag the reflect on the caster's status entry
            if (func_800AA6E8(arg0, arg1) != 0) {
                bounceTarget = arg0;
            } else {
                if (D_800F494C[arg1] == -1) {
                    D_800F494C[arg1] = SysGetLsbNumber(func_800AA700(arg1));
                }
                bounceTarget = D_800F494C[arg1];
            }
            D_800F4920 |= 2;
            D_800F4938[arg1] |= 1 << bounceTarget;
            func_800ACA24();
            entry = g_CurrentAction->unk200;
            if (entry->unk34 & 0x40000) {
                D_800F4958 |= 1 << arg1;
            } else if (entry->unk28 != 0) {
                entry->unk28--;
            } else {
                g_CurrentAction->unk23C |= 0x40000;
            }
            g_CurrentAction->unk218 |= 2;
            act->unk4 |= 2;
            if (arg1 < 3) {
                g_CurrentAction->unk224 = 0xA;
            }
        }
        if (g_CurrentAction->unk218 & 0x4000) {
            act->unk4 |= 0x10;
        }
        if (g_CurrentAction->unk218 & 0x8000) {
            act->unk4 |= 0x20;
        }
    } else {
        // hit missed/had no effect -- wipe any accumulated status/damage
        func_800ACA24();
    }

    // clamp the computed damage to this target's HP or MP cap
    if (arg1 < 3) {
        cap = D_800F5E60[arg1].capHP;
        capMP = D_800F5E60[arg1].capMP;
    } else {
        cap = 9999;
        capMP = 999;
    }
    if (g_CurrentAction->unk220 & 4) {
        cap = capMP;
    }
    if (cap < g_CurrentAction->unk214) {
        g_CurrentAction->unk214 = cap;
    }
    if (BATTLE_IsDamageNullified(arg1) != 0) {
        g_CurrentAction->unk214 = 0;
    }
    if (g_CurrentAction->unk214 != 0) {
        // All Lucky 7s: force the damage display to the "7777" value
        cap = g_CombatantTurnState[g_CurrentAction->actorId].unk3C;
        if (cap == 0x1E61) {
            g_CurrentAction->unk214 = cap;
        }
    }

    // pick which damage-number/message params to show for this hit
    flags = g_CurrentAction->unk218;
    if (!(flags & 3)) {
        if (!(g_CurrentAction->unk220 & 1) && (g_CurrentAction->actorId != arg1)) {
            g_CurrentAction->unkA8 |= 1 << arg1;
        }
        if (g_CurrentAction->unk250 == -1) {
            g_CurrentAction->unk250 = g_CurrentAction->unk214;
        }
        g_CurrentAction->unk24C = g_CurrentAction->unk68;
        if (g_CurrentAction->unk220 & 2) {
            g_CurrentAction->unk248 = g_CurrentAction->unk58;
        } else {
            g_CurrentAction->unk248 = g_CurrentAction->unk54;
        }
        if ((g_CurrentAction->unk220 & 1) || (g_CurrentAction->unk250 == 0)) {
            g_CurrentAction->unk224 = 0x33;
        } else {
            func_800AC6B4(0);
        }
    } else if (flags & 1) {
        g_CurrentAction->unk248 = g_CurrentAction->unk5C;
    } else {
        g_CurrentAction->unk248 = g_CurrentAction->unk54;
        if (g_CurrentAction->unk230 & 1) {
            func_800AC6B4(0);
        }
    }

    if (!(g_CurrentAction->unk218 & 1)) {
        // apply the pending status changes, honoring immunities (mask) and
        // the mutually-exclusive status pairs (Slow/Haste, Sadness/Fury)
        mask = ~g_CurrentAction->unk22C;
        oldStatus = g_CurrentAction->unk228;
        newStatus = oldStatus;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                if (g_CurrentAction->unk238 & D_800A03A0[i][j]) {
                    if ((i == 1) && (newStatus & D_800A03A0[1][j ^ 1])) {
                        // e.g. casting Fury on an already-Sad target just
                        // cancels the Sadness instead of stacking
                        g_CurrentAction->unk238 &= ~D_800A03A0[1][j];
                    }
                    // queue the paired status for removal
                    g_CurrentAction->unk23C |= D_800A03A0[i][j ^ 1];
                }
            }
        }
        if (g_CurrentAction->unk250 == -2) {
            mask |= 1;
        }
        // apply / remove / toggle against the immunity mask
        newStatus |= g_CurrentAction->unk238 & mask;
        newStatus &= ~(g_CurrentAction->unk23C & mask);
        newStatus ^= g_CurrentAction->unk240 & mask;
        g_CurrentAction->unk228 = newStatus;
        g_BattleState.combatant[arg1].status = newStatus;
        if (oldStatus != newStatus) {
            if (newStatus & g_CurrentAction->unk244) {
                if (g_CurrentAction->actorId != arg1) {
                    g_CurrentAction->unkA8 |= 1 << arg1;
                }
            }
            if ((oldStatus ^ newStatus) & 1) {
                // Death bit flipped -- pick the death/revive message
                func_800AC6B4(oldStatus & 1);
            } else {
                act->unk4 |= 8;
            }
        } else {
            g_CurrentAction->unk218 |= 0x800000;
        }
    } else {
        g_CurrentAction->unk218 |= 0x800000;
    }

    func_800AD0FC();
    if ((g_CurrentAction->unk218 & 0x40001) == 0x40001) {
        g_CurrentAction->unk218 &= ~2;
    }
    if (!(g_CurrentAction->unk218 & 2)) {
        // queue the hit's damage/message display
        func_800ABA68(
            act, g_CurrentAction->unk250, g_CurrentAction->unk220, g_CurrentAction->unk248, g_CurrentAction->unk24C);
    } else if (g_CurrentAction->unk218 & 0x800000) {
        BATTLE_QueueUnassignedResultDisplay(act);
    }
    if (!(g_CurrentAction->unk6C & 0x10)) {
        BATTLE_ApplyDefaultAbsorbEffect();
    }
    if (g_CurrentAction->unk90 & 0x80) {
        // HP-absorb / MP-absorb bonus effects
        func_800AD324(g_CurrentAction->unkF4, g_CurrentAction->unk208, g_CurrentAction->unk214 / 100, 1);
    }
    if (g_CurrentAction->unk90 & 0x40) {
        func_800AD324(g_CurrentAction->unkF4, g_CurrentAction->unk208, g_CurrentAction->unk214 / 10, 2);
    }
    if ((arg1 < 3) && (g_CurrentAction->actorId >= 4)) {
        // enemy attack triggered a scripted counter/follow-up
        if ((*(s32*)(g_CurrentAction->unk204 + 0x24) != 0) && (g_CurrentAction->unk28 == 0xD)) {
            BATTLE_LearnEnemySkill();
        }
    }

    // finalize the action-result descriptor for whatever consumes it next
    act->unk8 = g_BattleState.combatant[arg1].status;
    act->unk2 = g_CurrentAction->unk224;
    if (g_CurrentAction->unk218 & 0x20) {
        act->unk2 = 9;
    }
    if (g_BattleState.combatant[arg1].status & 1) {
        // target just died -- mark it and re-queue a death message if the
        // current message slot isn't already showing one
        act->unk4 = (act->unk4 | 4) & ~8;
        g_CurrentAction->unk7C |= 1 << arg1;
        if (g_CurrentAction->unk28 == 0x1A) {
            if (D_801636B8[arg1].D_801636BC < 0x11) {
                D_801636B8[arg1].D_801636BC = 8;
            }
            func_800ABA68(act, -2, 0, g_CurrentAction->unk248, g_CurrentAction->unk68);
        }
    }
}

void func_800AC6B4(s32 arg0) {
    s32 temp_a0;

    if (arg0 != 0) {
        if (g_CurrentAction->unk208 >= 4) {
            g_CurrentAction->unk224 = 0x39;
        }
    } else {
        temp_a0 = g_CurrentAction->unk228;
        if (temp_a0 & 0x400) {
            g_CurrentAction->unk224 = 0x30;
        } else if (temp_a0 & 0x800) {
            g_CurrentAction->unk224 = 5;
        } else {
            g_CurrentAction->unk224 = func_800A2D0C();
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AC73C);

void func_800ACA24(void) {
    g_CurrentAction->unk238 = 0;
    g_CurrentAction->unk23C = 0;
    g_CurrentAction->unk240 = 0;
    g_CurrentAction->unk244 = 0;
    g_CurrentAction->unk230 = 0;
    g_CurrentAction->unk214 = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACA4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACB98);

s32 func_800ACD88(s32 arg0) {
    s32 result;
    s32 flags;

    result = 0;
    if (g_CurrentAction->unk6C & 4) {
        flags = g_BattleState.combatant[arg0].unk4 & 0x200;
        result = flags != 0;
    } else if (g_BattleState.combatant[arg0].unk4 & 0x100) {
        result = 1;
    }

    return result;
}

s32 BATTLE_IsDamageNullified(s32 arg0) {
    return func_800ACD88(arg0) != 0 || (g_BattleState.combatant[arg0].status & (STATUS_PEERLESS | STATUS_PETRIFY)) != 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACE88);

// arg0 never got a ring slot from func_800A311C (still unassigned) --
// queue a placeholder display entry via func_800ABA68 anyway. unk22C here
// is the same status-immunity mask func_800ABB0C (this function's only
// caller) uses earlier.
void BATTLE_QueueUnassignedResultDisplay(Unk800FA9D0* arg0) {
    s8 temp_v1;

    if ((g_CurrentAction->unk80 | g_CurrentAction->unk84 | g_CurrentAction->unk88) & ~g_CurrentAction->unk22C) {
        temp_v1 = arg0->unk3;
        if (temp_v1 == -1) {
            func_800ABA68(arg0, -1, 0, -1, temp_v1);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD0FC);

void func_800AD324(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_t0;
    s32 var_a2;

    var_a2 = arg2;
    temp_t0 = g_CurrentAction->unk220 & 1;
    if (arg3 & 1) {
        if (arg1 == g_CurrentAction->unk208) {
            if (g_CurrentAction->unk25C < var_a2) {
                var_a2 = g_CurrentAction->unk25C;
            }
        }
        if (temp_t0) {
            var_a2 = -var_a2;
        }
        g_CombatantTurnState[arg0].unk30 -= var_a2;
    }
    if (arg3 & 2) {
        if (arg1 == g_CurrentAction->unk208) {
            if (g_CurrentAction->unk258 < var_a2) {
                var_a2 = g_CurrentAction->unk258;
            }
        }
        if (temp_t0) {
            var_a2 = -var_a2;
        }
        g_CombatantTurnState[arg0].unk2C -= var_a2;
    }
}

// same target (unk208) and value (unk214) forwarded to func_800AD324 as
// the absorb-effect calls below; result picks HP (bit0) / MP (bit1)
void BATTLE_ApplyDefaultAbsorbEffect(void) {
    s32 t0;
    s32 a3;
    s32 result;

    t0 = 2;
    if (g_CurrentAction->unk220 & 4) {
        t0 = 1;
    }

    a3 = g_CurrentAction->unk6C;
    a3 = a3 & 0x20;
    a3 = (a3 == 0) ? 3 : 0;
    result = t0 | a3;

    func_800AD324(g_CurrentAction->unkF4, g_CurrentAction->unk208, g_CurrentAction->unk214, result);
}

void func_800AD480(void) {
    s32 count;
    s32 next;
    u32 i;

    count = 0;
    i = 0;
    next = 0;
    for (; i < 0x1E; i++) {
        if (count < 0x10) {
            if (i == next) {
                D_800F495C[count] = i;
                count++;
            }
            if (D_800E7BCC[i] == 8) {
                next = i + 1;
            }
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD4EC);

const s8 D_800A04B0[] = {0x0A, 0x0B, 0x0C, 0x0D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x7F, 0x03, 0x34};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD5E8);

s32 func_800AD73C(s32 arg0) {
    if (g_CurrentAction->unk6C & 4) {
        if (g_CurrentAction->unk228 & 0x20000) {
            g_CurrentAction->unk218 |= 0x8000;
        }
    } else if (g_CurrentAction->unk228 & 0x10000) {
        g_CurrentAction->unk218 |= 0x4000;
    }

    if (g_CurrentAction->unk218 & 0xC000) {
        arg0 = arg0 / 2;
    }
    if (g_CurrentAction->unkE8 != 0) {
        arg0 += (arg0 * g_CurrentAction->unkE8) / 100;
    }

    return arg0;
}

// multi-target damage-reduction formula, s32 func_800AD804(s32 damage, s32
// fullDamage): if fullDamage is false, it still gets forced true when
// unkB8 < 2 (single target) or unk50 & 0x80 is set (the exemption bit
// documented on unk50's seed at func_800A8D18/func_800A8D60 above); then
// if unkAC != 0 (hit-sequence position, see func_800A8E54) returns
// damage>>1, else returns damage unchanged when fullDamage else damage/3
// (magic-number signed divide) -- this is the classic "multi-target hits
// deal reduced per-target damage" mechanic
s32 func_800AD804(s32 arg0, s32 arg1) {
    if (arg1 == 0) {
        if ((g_CurrentAction->unkB8 < 2) || (g_CurrentAction->unk50 & 0x80)) {
            arg1 = 1;
        }
    }

    if (g_CurrentAction->unkAC != 0) {
        arg0 >>= 1;
    } else if (arg1 == 0) {
        arg0 = (arg0 * 2) / 3;
    }

    return arg0;
}

// reduces arg0 by ~30% when Sadness (status bit 0x10, see D_800A03A0) is set
// on the current action's status mask; same reduction as
// BATTLE_ApplyConditionalReduction, gated on a different bit
static s32 BATTLE_ApplySadnessReduction(s32 arg0) {
    if (g_CurrentAction->unk228 & 0x10) {
        arg0 -= (arg0 * 3) / 10;
    }
    return arg0;
}

// scale arg0 by a fixed-point random variance factor (~93.77%..100%), then
// clamp the result to a minimum of 1
static s32 func_800AD8DC(s32 arg0) {
    s32 temp_s0;
    s32 var_v0;

    var_v0 = arg0;
    temp_s0 = ((s32)(var_v0 * (SysGetRandomByteFromTable() + 0xF01))) >> 0xC;
    var_v0 = temp_s0;
    if (temp_s0 == 0) {
        var_v0 = 1;
    }
    return var_v0;
}

void func_800AD924(void) { g_CurrentAction->unk218 |= 2; }

void func_800AD944();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD944);

void func_800ADBBC(void) {
    s32 temp_s0;
    s32 var_v1;
    s32 base;

    base = (g_CurrentAction->unk4C + g_CurrentAction->characterLevel) * 6;

    var_v1 = base * (0x200 - g_CurrentAction->unk210) * g_CurrentAction->unk48;
    temp_s0 = (g_CurrentAction->unk50 & 0xC) == 4;
    if (var_v1 < 0) {
        var_v1 += 0x1FFF;
    }
    g_CurrentAction->unk214 =
        func_800AD8DC(func_800AD73C(func_800AD804(BATTLE_ApplySadnessReduction(var_v1 >> 0xD), temp_s0)));
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADC70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADD2C);

s32 func_800AD804(s32, s32);
s32 func_800AD73C(s32);

void func_800ADDE8(void) {
    s32 base = g_CurrentAction->unk4C + g_CurrentAction->characterLevel;
    s32 term1 = base * 3;
    s32 term2 = g_CurrentAction->unk48 * 0xB;
    s32 damage = (term2 + term1) * 2;
    g_CurrentAction->unk214 = func_800AD8DC(func_800AD73C(func_800AD804(damage, 0)));
}

void func_800ADE5C(void) { g_CurrentAction->unk214 = g_CurrentAction->unk48 * 20; }

// Item attack damage formula.
void func_800ADE84(void) {
    s32 value = g_CurrentAction->unk48 * (0x200 - g_CurrentAction->unk210);
    g_CurrentAction->unk214 = func_800AD8DC(value / 32);
}

void func_800ADED8(void) {
    if (g_CurrentAction->unk230 & 0x40) {
        g_CurrentAction->unk230 = 1;
    } else {
        g_CurrentAction->unk230 = 0x80;
    }
}

void func_800ADF04(void) {
    g_CurrentAction->unk4C = g_CurrentAction->unkD8 * 2;
    func_800AD944();
}

void func_800ADF38(void) {
    s32 divisor = SysCountActiveBits(g_CurrentAction->allowedTargetsMask);
    s32 result = 0;
    if (divisor != 0) {
        result = (g_CurrentAction->unk48 + (divisor - 1)) / divisor;
    }
    g_CurrentAction->unk214 = result;
}

// White Wind "damage" formula. Restores HP equal to caster's HP to all allies.
void func_800ADFC0(void) { g_CurrentAction->unk214 = *(u16*)(&g_CombatantTurnState[g_CurrentAction->actorId].unk3C); }

void func_800ADFF4(void) {
    s32 index = g_CurrentAction->actorId;
    g_CurrentAction->unk214 = g_BattleState.combatant[index].maxHP - g_CombatantTurnState[index].unk3C;
}

void func_800AE050(void) {}

void func_800AE058(void) {}

void func_800AE060(void) {}

void func_800AE068(void) {}

void func_800AE070(void) {}

void func_800AE078(void) {}

// Cait Sith's Dice attack damage formula.
void func_800AE080(void) {
    s32 i;
    s32 j;
    s32 numDice;
    s32 dieValue;
    s32 dieValues[8];
    s32 diceSum;
    s32 repeat;
    s32 maxRepeat;

    numDice = g_CurrentAction->characterLevel / 10;
    if (numDice < 2) {
        numDice = 2;
    }
    if (numDice > 6) {
        numDice = 6;
    }

    for (i = 0; i < 4; i++) {
        D_80163774[i] = 0xFF;
    }

    diceSum = 0;
    for (i = 0; i < numDice; i++) {
        dieValue = SysGetRandomByteRange(6);
        dieValues[i] = dieValue;
        diceSum += dieValue + 1;
        if (i & 1) {
            D_80163774[i / 2] = dieValue << 4 | D_80163774[i / 2] & 0xF;
        } else {
            D_80163774[i / 2] = dieValue | 0xF0;
        }
        SysIncSeedForRandom();
    }

    maxRepeat = 0;
    for (i = 0; i < 6; i++) {
        repeat = 0;
        for (j = 0; j < numDice; j++) {
            if (dieValues[j] == i) {
                repeat++;
            }
        }
        if (maxRepeat < repeat) {
            maxRepeat = repeat;
        }
    }

    diceSum *= 100 * maxRepeat;
    g_CurrentAction->unk214 = diceSum;
}

// Chocobuckle attack damage formula.
void func_800AE234(void) {
    g_CurrentAction->unk214 =
        Savemap.memory_bank_1[26] + Savemap.memory_bank_1[27] * 256; // Number of escapes from battles.
}

// Sephiroth's Heartless Angel attack damage formula.
void func_800AE25C(void) { g_CurrentAction->unk214 = g_BattleState.combatant[g_CurrentAction->unk208].curHP - 1; }

// Tonberry's Time Damage attack damage formula.
void func_800AE2A0(void) {
    s32 minutes = Savemap.time / 60;
    g_CurrentAction->unk214 = (minutes / 60) * 100 + minutes % 60;
}

// target-side damage/effect scaling from the target's save-file kill count
// (party members only -- targetIdx >= 3 is an enemy, contributes 0)
void BATTLE_ApplyKillCountBonus(void) {
    s32 var_v1;

    var_v1 = 0;
    if (g_CurrentAction->unk208 < 3) {
        var_v1 = D_800F5E60[g_CurrentAction->unk208].partyMember->kill_count;
    }
    g_CurrentAction->unk214 = var_v1 * 0xA;
}

void BATTLE_CalcMateriaSlotScore(void) {
    SavePartyMember* pm;
    s32 slot;
    s32 count;
    s32 i;
    s32 none;

    slot = g_CurrentAction->unk208;
    count = 0;
    if (slot < 3) {
        i = 0;
        none = -1;
        pm = D_800F5E60[slot].partyMember;
        for (; i < 8; i++) {
            if (pm->materia_weapon[i] != none) {
                count++;
            }
            if (pm->materia_armor[i] != none) {
                count++;
            }
        }
    }
    g_CurrentAction->unk214 = count * 1111;
}

void func_800AE42C(s32, s32, s32, s32*, s32, s32);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE42C);

s32 func_800AE6C0(s32 arg0, s32 arg1, s32 arg2) {
    s32 masks[2][8];
    s32 i;

    func_800AE42C(arg1, arg2, arg0, masks, 0, 0);

    for (i = 0; i < 8; i++) {
        if ((masks[0][i] & arg1) || (masks[1][i] & arg2)) {
            break;
        }
    }

    if (i == 8) {
        i = 3;
    }

    return i;
}

void func_800AE764(s32 mask, s32 arg1, s32 arg2) {
    u8 unused[64]; // retail reserves it, nothing reads it
    s32 i;
    s32 result;
    s32 v;

    result = 0;
    for (i = 0; i < 10; i++) {
        g_BattleState.combatant[i].unkB = 3;
        if ((mask >> i) & 1) {
            v = func_800AE6C0(i, arg1, arg2);
            if (v != 3) {
                g_BattleState.combatant[i].unkB = v;
                result |= 1 << i;
            }
        }
    }
    g_BattleState.unk6 = result;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE82C);

void func_800AE954(int index);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE954);

const u8 D_800A04BC[] = {0x1E, 0x14, 0x3C, 0x1E, 0x7F, 0x7F, 0x0A, 0x64, 0x7F, 0x7F,
                         0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x0D, 0x00, 0x00};
static s32 func_800AF834(s32 arg0);

void func_800AEB20(s32 arg0, s32 arg1, s32 arg2) {
    s32 index;
    u8* p;

    index = func_800AF834(arg1);
    if (index >= 0) {
        p = (u8*)&g_CombatantTurnState[arg0].unk10;
        p[index] = D_800A04BC[index];
    }
}

// this data belong to functions located above:
const u8 g_StatusBitTable[] = {
    0x0A, 0x19, 0x15, 0x0D, 0x10, 0x11, 0x03, 0x02, 0x0F, 0x1B, 0x14, 0x18, 0xFF, 0xFF, 0xFF, 0xFF};
int func_800B0378();
int func_800B062C();
int BATTLE_RollPhysicalHit();
int func_800B0B8C();
int func_800B0910();
int func_800B089C();
int (* const D_800A04E0[])() = {
    func_800B0378, func_800B062C, BATTLE_RollPhysicalHit, func_800B0B8C,
    func_800B0B8C, func_800B0B8C, func_800B0910,          func_800B089C,
};
// ___end

void func_800B108C(s32 arg0);

void func_800AEB80(s32 arg0, s32 arg1, s32 arg2) {
    s32 index;
    u8* p;

    index = func_800AF834(arg1);
    if (index >= 0) {
        p = (u8*)&g_CombatantTurnState[arg0].unk10;
        p[index] = 0;
        if ((0xD8B >> index) & 1) {
            func_800B108C(arg0);
        }
    }
}

void func_800AEBF0(int index) { func_800AE954(index); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEC10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEF68);

void func_800AF0C4(s32 arg0, s32 arg1, s32 arg2) {
    if (!(g_BattleState.combatant[arg0].status & 0x2804444)) {
        func_800A7254(0, arg0, 6, 0);
    }
    if (!(g_BattleState.combatant[arg0].status & 0x2004404)) {
        if ((D_800F5F44.D_800F7DC2 >> arg0) & 1) {
            if (D_800F5F44.unkBF0[arg0].unk0 != 0xFF) {
                func_800A3D4C(&D_800F5F44.unkBF0[arg0]);
                D_800F5F44.unkBF0[arg0].unk0 = 0xFF;
            }
        }
    }
}

void func_800A7254(s32, s32, s32, s32);
void func_800AF1A8(s32 arg0) { func_800A7254(0, arg0, 8, 0); }

void func_800AF0C4(s32, s32, s32);

// skips (does nothing) while combatant[arg0].status has Berserk or Confusion
void BATTLE_TryApplyHitEffect(s32 arg0, s32 arg1, s32 arg2) {
    if (!(g_BattleState.combatant[arg0].status & (STATUS_BERSERK | STATUS_CONFU))) {
        func_800A7254(0, arg0, 9, 0);
        func_800AF0C4(arg0, arg1, arg2);
    }
}

void func_800AF264(s32 arg0, s32 arg1, s32 arg2) {
    s32 status;

    func_800AEBF0(arg0);
    func_800AEB20(arg0, arg1, arg2);
    func_800A7254(0, arg0, 4, 0);

    status = g_BattleState.combatant[arg0].status & 0xFFBFFFFF;
    g_BattleState.combatant[arg0].status = status;

    if (arg1 == 0xE) {
        g_BattleState.combatant[arg0].status = status & 0xF7FF7FB3;
    }
}

void func_800AF320(s32 arg0, s32 arg1, s32 arg2) {
    func_800AEBF0();
    func_800AEB80(arg0, arg1, arg2);
    func_800AF0C4(arg0, arg1, arg2);
}

void func_800AF380(s32 arg0) { func_800A7254(2, arg0, 0x15, 0xF); }

void BATTLE_ApplyRegenPoisonTick(s32 arg0, s32 arg1, s32 arg2) {
    s32 amount;
    s32 status;
    s32 step;

    amount = 0;
    step = g_BattleState.combatant[arg0].maxHP >> 5;
    status = g_BattleState.combatant[arg0].status;
    if (status < 0) {
        if (D_800F5F44.D_800F7DC6 == 1) {
            status |= 0x8000000;
        }
    }
    if (status & 0x8000) {
        amount += step;
    }
    if (status & 0x8000000) {
        amount -= step;
    }
    g_CombatantTurnState[arg0].unk6 = amount;
    if (arg2 != 0) {
        func_800AEB20(arg0, arg1, arg2);
    } else {
        func_800AEB80(arg0, arg1, 0);
    }
}

void func_800AF470(s32 arg0) { g_CombatantTurnState[arg0].unk28 = 3; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF494);

void BATTLE_ClearActorSlotReferences(s32 arg0, s32 arg1, s32 arg2) {
    s32 i;

    func_800A7254(0, arg0, 0xA, arg2);
    if (arg2 != 0) {
        func_800A23BC(arg0);
        return;
    }
    for (i = 0; i < LEN(D_800F5E60); i++) {
        if (D_800F5E60[i].unk6 == arg0) {
            D_800F5E60[i].unk6 = 0;
            func_800A7254(0, i, 6, 0);
        }
    }
    func_800B108C(arg0);
}

void func_800B108C(s32 arg0);
void func_800AF63C(s32 arg0) { func_800B108C(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF65C);

static s32 func_800AF834(s32 arg0) {
    s32 result;
    s32 i;

    result = -1;
    for (i = 0; i < LEN(g_StatusBitTable); i++) {
        if (g_StatusBitTable[i] == arg0) {
            result = i;
        }
    }
    return result;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF874);

void func_800AF9C8();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF9C8);

extern s32 D_800F499C;
extern s32 D_800F49F8[][10];

s32 func_800AFE98(s32 arg0) { return D_800F49F8[D_800F499C][arg0] >> 0xC; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AFECC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0170);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0234);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0378);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B062C);

static s32 func_800B2F70(void);
void BATTLE_RollPhysicalHit(void) {
    s32 acc;
    s32 attacker;
    s32 target;
    s32 v;

    attacker = g_CurrentAction->actorId;
    target = g_CurrentAction->unk208;
    if (!(g_CurrentAction->unk218 & 1)) {
        acc = 0xFF;
        if (!(g_CurrentAction->unkC8 & 0x40000000)) {
            v = (g_CurrentAction->characterLevel +
                 g_BattleState.combatant[attacker].unk15) -
                g_BattleState.combatant[target].unk9;
            acc = v / 4;
            if (attacker < 3) {
                acc += D_800F5F01[attacker * 0x18];
            }
        }
        if (acc >= func_800B2F70()) {
            g_CurrentAction->unk220 |= 2;
        }
    }
}

void func_800B089C(void) {
    s32 temp_v1;

    temp_v1 = g_CurrentAction->unk3C;
    if ((temp_v1 != 0) && ((g_CurrentAction->unk254 % temp_v1) != 0)) {
        g_CurrentAction->unk218 |= 1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0910);

int func_800B0B8C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0B94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0C14);

void func_800B0DF8(void) {
    if (g_CurrentAction->unk234 & 2) {
        g_BattleState.combatant[g_CurrentAction->unk208].unk4 ^= 0x80;
    }
}

// same ~30% reduction as BATTLE_ApplySadnessReduction, gated on a bit of unkC8
// (not unk218)
static s32 BATTLE_ApplyConditionalReduction(s32 arg0) {
    if ((arg0 < 0xFF) && (g_CurrentAction->unkC8 & 0x20)) {
        arg0 -= (arg0 * 3) / 10;
    }
    return arg0;
}

s32 func_800B0EB4(s32 arg0) {
    s32 status = g_BattleState.combatant[arg0].status;
    s32 count = arg0 < 4;

    if (status & 0x40) {
        count++;
    }
    if (status & 0x400000) {
        count++;
    }

    return count & 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0F04);

void func_800B0FFC(s32 arg0, s32 arg1, s32 arg2, s16* arg3) {
    func_800A31A0(arg0, 2, arg2, BATTLE_ExpandScriptToBuffer(SysGetPtrToUncompKernBattleTxtWithId(arg1), arg3) + 0x100);
}

void func_800B1060(s32 arg0) { func_800A31A0(10, 2, 1, arg0); }

void func_800B108C(s32 arg0) { func_800A31A0(arg0, 5, 0, 0); }

// true when the combatant's HP is at or below a quarter of max -- the "Near
// Death" threshold used by weapon-specific damage formulas (e.g. Powersoul's
// HP-based multiplier).
static s32 func_800B10B4(s32 arg0) {
    return g_BattleState.combatant[arg0].curHP <= g_BattleState.combatant[arg0].maxHP / 4;
}

void BATTLE_QueueEffect(
    s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    Unk800A2F4C* unk;
    Unk800FA9D0* act;

    unk = func_800A2F4C();
    act = func_800A2FD0();
    unk->unk1 = 1;
    unk->unk8 = -1;
    unk->unk0 = arg0;
    unk->unk5 = arg1;
    unk->unk3 = arg2;
    unk->unk2 = arg3;
    unk->unk4 = 0;
    unk->unk6 = arg4;
    act->unk0 = arg0;
    act->unk1 = arg0;
    act->unk2 = 0x33;
    act->unk4 = arg5;
    act->unk8 = arg6;
    func_800A317C();
}

// find arg0 in D_800F5F44.attackIDs[]; returns its index, or 0x20 (and signals
// func_800155A4) if it is not present
static s32 func_800B11B4(s32 arg0) {
    s32 i;
    u16* p;

    for (i = 0, p = D_800F5F44.attackIDs; i < LEN(D_800F5F44.attackIDs); i++) {
        if (*p == arg0) {
            break;
        }
        p++;
    }
    if (i == LEN(D_800F5F44.attackIDs)) {
        func_800155A4(0x20);
    }
    return i;
}

s32 func_800B1218(s32 arg0, s32 arg1, s32 arg2) {
    s8* p;

    p = (s8*)&g_CombatantTurnState[arg0].unk20;

    return arg1 + ((arg1 * p[arg2]) / 100);
}

void func_800B1268(s32 arg0, s32 arg1, s32 arg2) {
    s32 i;
    s8* p;

    i = 0;
    p = (s8*)&g_CombatantTurnState[arg0].unk20;
    for (; i < 8; i++, p++) {
        if ((arg2 >> i) & 1) {
            s32 value = *p + arg1;

            if (value > 100) {
                value = 100;
            }
            if (value < -100) {
                value = -100;
            }
            *p = value;
        }
    }
}

// nonzero if D_800F5F44.D_800F7DC8 is < 3
static u32 func_800B12DC(void) {
    u32 result = 0;
    s32 cmp = (s32)D_800F5F44.D_800F7DC8;

    if (cmp < 3) {
        result = (u32)~D_800F5F44.D_800F7DC8 >> 0x1F;
    }
    return result;
}

// invalidates (unk2 = -1) any occupied messageQueue entry (unk0 != 0xFF)
// of category arg0 whose unk0 is >= arg1; see func_800A3D4C, which pushes
// entries into this same queue
static void BATTLE_InvalidateQueuedMessages(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < LEN(D_800F5F44.messageQueue); i++) {
        if (D_800F5F44.messageQueue[i].unk2 == arg0) {
            u8 val = D_800F5F44.messageQueue[i].unk0;
            if (val != 0xFF && val >= arg1) {
                D_800F5F44.messageQueue[i].unk2 = -1;
            }
        }
    }
}

static s32 BATTLE_ScriptReadU16(void) {
    s32 value;

    value = D_800F4AC0[D_800F4AC4->pc++];
    value |= D_800F4AC0[D_800F4AC4->pc++] << 8;

    return value;
}

// Resolve a packed variable reference for the battle-script VM (func_800B1D48):
// map combatant arg0 + descriptor arg1 to a backing pointer (*arg2) and return
// a bit offset into it. arg1 < 0x2000 selects the per-combatant variable bank
// D_800F87F0[arg0] (0x80 bytes each); arg1 < 0x4000 selects the shared,
// battle-wide bank D_800F83A4; otherwise the per-combatant stat record
// g_BattleState.combatant[arg0] (0x68 bytes each). func_800B153C /
// func_800B141C then read or write at that bit offset.
static s32 func_800B13B0(s32 arg0, s32 arg1, void** arg2) {
    s32 var_a1;

    var_a1 = arg1;
    if (var_a1 < 0x2000) {
        *arg2 = &D_800F87F0[arg0 * 0x80];
    } else if (var_a1 < 0x4000) {
        *arg2 = D_800F83A4;
        var_a1 -= 0x2000;
    } else {
        *arg2 = &g_BattleState.combatant[arg0];
        var_a1 -= 0x4000;
    }
    return var_a1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B141C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B153C);

// Push `value` onto the operand stack as `size` bytes, most significant byte
// first. Sizes above 3 (or negative) push nothing; the cases deliberately fall
// through so that each one pushes one fewer byte than the last.
void func_800B1624(s32 size, u32 value) {
    switch (size) {
    case 3:
        D_800F4AC4->stack[--D_800F4AC4->sp] = value;
        value >>= 8;
    case 2:
        D_800F4AC4->stack[--D_800F4AC4->sp] = value;
        value >>= 8;
    case 1:
    case 0:
        D_800F4AC4->stack[--D_800F4AC4->sp] = value;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B16D0);

// Pop a `size`-byte big-endian value off the operand stack. The inverse of
// func_800B1624, and likewise falls through so each case consumes one byte.
s32 func_800B17F0(s32 size) {
    s32 value = 0;
    u8 byte;

    switch (size) {
    case 3:
        value = D_800F4AC4->stack[D_800F4AC4->sp++];
    case 2:
        byte = D_800F4AC4->stack[D_800F4AC4->sp++];
        value <<= 8;
        value |= byte;
    case 1:
    case 0:
        byte = D_800F4AC4->stack[D_800F4AC4->sp++];
        value <<= 8;
        value |= byte;
    }
    return value;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B18A8);

// Evaluate the operand at the script cursor without consuming it: run the
// normal operand fetch, then rewind the stack pointer to where it started so
// the operand bytes it popped stay available to the next read.
s32 func_800B1A5C(s32 arg0) {
    s32 sp = D_800F4AC4->sp;
    s32 result = func_800B18A8(arg0);

    D_800F4AC4->sp = sp;
    return result;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1AA0);

s32 BATTLE_ScriptCompare(s32 lhs, s32 rhs) {
    u32 a = D_800F4AC4->var[0][lhs];
    u32 b = D_800F4AC4->var[1][rhs];
    s32 result = 0;

    switch (D_800F4AC4->opcode) {
    case 0x40:
        if (a == b) {
            result = 1;
        }
        break;
    case 0x41:
        if (a != b) {
            result = 1;
        }
        break;
    case 0x42:
        if (a >= b) {
            result = 1;
        }
        break;
    case 0x43:
        if (a <= b) {
            result = 1;
        }
        break;
    case 0x44:
        if (a > b) {
            result = 1;
        }
        break;
    case 0x45:
        if (a < b) {
            result = 1;
        }
        break;
    }

    return result;
}

s32 func_800B1C1C(s32 arg0) {
    s32 result;
    s32 i;
    u16 mask;

    result = 0;
    i = 0;
    mask = D_800F4AC4->unk28[arg0];
    for (; i < LEN(D_800F4AC4->var[arg0]); i++) {
        if (((mask >> i) & 1) && (D_800F4AC4->var[arg0][i] != 0)) {
            result |= 1 << i;
        }
    }

    return (result & 0xFFFF) != 0;
}

s32 BATTLE_ScriptCollapseVarBank(s32 arg0) {
    s32 i;
    s32 v;
    s32 mask;

    i = 0;
    if (D_800F4AC4->unk18[arg0] == 2) {
        v = 0;
        mask = D_800F4AC4->unk28[arg0];
        for (i = 0; i < 10; i++) {
            if ((mask >> i) & 1) {
                v = D_800F4AC4->var[arg0][i];
                break;
            }
        }
        D_800F4AC4->unk28[arg0] = 0x3FF;
        for (i = 9; i >= 0; i--) {
            D_800F4AC4->var[arg0][i] = v;
        }
        i = 1;
    }
    return i;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1D48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2A2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2B5C);

AttackData* BATTLE_GetAttackData(s32);
s32 func_800B2C60(s32 arg0) {
    s32 var_s0;
    AttackData* ret;

    var_s0 = 0;
    if (arg0 <= 0xFFFE) {
        ret = BATTLE_GetAttackData(arg0);
        if (ret) {
            var_s0 = ret->mpCost;
        }
    }
    return var_s0;
}

static void func_800B2CAC(s32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        D_800F83A6 = Savemap.memory_bank_1[arg1];
        return;
    case 1:
        Savemap.memory_bank_1[arg1] = D_800F83A6;
        return;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2CFC);

// ids below 256 index the kernel table; higher ones are the scene's own
AttackData* BATTLE_GetAttackData(s32 id) {
    AttackData* ret;
    s32 i;

    ret = NULL;
    if (id < 256) {
        ret = &D_800708C4[id];
    } else {
        for (i = 0; i < 32; i++) {
            if (D_800F5F44.attackIDs[i] == id) {
                ret = &D_800F5F44.attacks[i];
                break;
            }
        }
    }
    return ret;
}

u8 func_800B2F30(void) { return SysGetRandomByteFromTable(); }

u16 func_800B2F50(void) { return SysRandomTwoBytes(); }

// scale a 16-bit value into the range 1..100
static s32 func_800B2F70(void) { return (((func_800B2F50() & 0xFFFF) * 0x63) / 0xFFFF) + 1; }

static s32 func_800B2FC4(s32 arg0) { return (arg0 * (func_800B2F30() + 0xF01)) >> 12; }

static s32 func_800B3000(u16 arg0) {
    s32 count = 0;

    while (arg0 != 0) {
        if (arg0 & 1) {
            count++;
        }
        arg0 >>= 1;
    }
    return count;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B3030);
