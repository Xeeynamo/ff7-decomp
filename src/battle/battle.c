#include "battle_private.h"

const u8 D_800A0000 = 0;
const u8 D_800A0001 = 0;
const u16 D_800A0002 = 0;
const u8 D_800A0004[] = {
    0x14, 0x11, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
    0x21, 0x37, 0x00, 0x00, 0x22, 0x00, 0x06, 0x00, 0x26, 0x00, 0x12, 0x04,
    0x21, 0x0A, 0x00, 0x00, 0x21, 0x09, 0x00, 0x00, 0x28, 0xB1, 0x00, 0x20,
    0x2A, 0x11, 0x00, 0x00, 0x2C, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x24, 0x11, 0x04, 0x00, 0x00, 0x11, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00,
    0x1F, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x15, 0x11, 0x00, 0x00,
    0x16, 0x11, 0x00, 0x10, 0x1C, 0x11, 0x02, 0x00, 0x18, 0x11, 0x00, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x2E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00};
// opcode-byte program for func_800A1798's dispatch loop: a 0x1F-delimited
// stream of per-command opcode sequences, sliced by D_800F38AC[cmdIndex]
// (see func_800A283C) into per-command runs; each byte indexes D_800E7B28
// (function-pointer table) for func_800A1798 to jalr through in order
const u8 D_800A0098[] = {
    0x1F, 0x0E, 0x09, 0x1F, 0x00, 0x0C, 0x09, 0x1F, 0x01, 0x0C, 0x09, 0x1F,
    0x02, 0x0D, 0x09, 0x1F, 0x1E, 0x09, 0x1F, 0x0A, 0x16, 0x09, 0x1F, 0x1D,
    0x09, 0x1F, 0x19, 0x09, 0x1F, 0x0E, 0x1C, 0x09, 0x1F, 0x0E, 0x1B, 0x09,
    0x1F, 0x1A, 0x09, 0x1F, 0x17, 0x1F, 0x03, 0x0C, 0x09, 0x1F, 0x1F, 0x1F,
    0x1F, 0x0E, 0x09, 0x1F, 0x04, 0x0B, 0x0F, 0x1F, 0x05, 0x1F, 0x06, 0x0C,
    0x09, 0x1F, 0x00, 0x0C, 0x09, 0x1F, 0x01, 0x0C, 0x09, 0x1F, 0x02, 0x0D,
    0x09, 0x1F, 0x0E, 0x09, 0x1F, 0x12, 0x0E, 0x09, 0x1F, 0x0E, 0x18, 0x09,
    0x1F, 0x10, 0x0E, 0x09, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x0C, 0x09,
    0x1F, 0x08, 0x1F, 0x11, 0x1F, 0x13, 0x09, 0x1F, 0x14, 0x1F, 0x15, 0x0F,
    0x1F, 0x00, 0x00, 0x00};
const s32 D_800A0108 = 21;
const s32 D_800A010C[] = {2, 22, 3, 23, 4};

// entrypoint
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1158);

// per-command opcode dispatcher: reads cmdIndex from the turn context
// (D_80063014->unkC), looks up its opcode-sequence start via
// D_800F38AC[cmdIndex] into D_800A0098, then for each byte until the 0x1F
// delimiter, jalr's through D_800E7B28[opcode]. After each call, checks
// D_80062F14 -- if it goes >= 0 the whole sequence aborts immediately
// (handler requested a suspend, e.g. to wait on an animation), otherwise
// continues to the next opcode byte
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1798);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A22C0);

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
    /* i is initialised separately and delim kept in a temp: both are
     * load-bearing for codegen */
    for (; i < 0x6D; i++) {
        if (i == next) {
            *out++ = i;
        }
        if (D_800A0098[i] == delim) {
            next = i + 1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2894);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2974);

void func_800A2B28(s32 arg0) {
    if (arg0 & 1) {
        D_80063014->unk90 |= 0x80;
    }
    if (arg0 & 2) {
        D_80063014->unk90 |= 0x40;
    }
    if (arg0 & 8) {
        D_80063014->unk90 |= 0x04;
    }
    if (arg0 & 0x10) {
        D_80063014->unk90 |= 0x800;
    }
    if (arg0 & 0xE0) {
        D_80063014->unkE8 = (arg0 >> 5) * 10;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2BF4);

void func_800B10F0(s32, s32, s32, s32, s32, s32, s32);
void func_800A2CC4(s32 arg0) {
    func_800B10F0(D_80063014->unk0, arg0, D_80063014->unk28, D_80063014->unk24,
                  D_80063014->unk98, 0, 0);
}

const u8 D_800A01A8[] = {0x05, 0x06, 0x07, 0x12, 0x0F, 0x00, 0x03, 0xA6};
u8 func_800A2D0C(void) {
    s32 temp_v1;

    if (D_80063014->unk208 >= 3) {
        return D_800F83E0[D_80063014->unk208].unk11;
    }
    return D_800A01A8[D_80063014->unkCC];
}

void func_800A2D68(u8 arg0) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (D_80063014->unkD0[i] == 0xFF) {
            D_80063014->unkD0[i] = arg0;
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

s8* func_800A2FD0();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2FD0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A304C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A311C);

void func_800A317C(void) {
    s8* ret = func_800A2FD0();
    *ret = -1;
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

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A32C0);

void func_800A3354();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3354);

void func_800A345C(void) {
    if (D_800F3944) {
        func_800A3354();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3488);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A34CC);

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
    ret |= (D_800F83D0 & 3) ? 1 : 0;
    if (D_800F39E4 > 0x4000) {
        ret = 1;
    }
    return ret;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A38FC);

typedef struct {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
    s16 unk4;
    s16 unk6;
} Unk800A3D4C; // size:8
void func_800A3D4C(Unk800A3D4C*);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3D4C);

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

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4480);

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

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4860);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4954);

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

u8 func_800A4CA8(s32 arg0) { return D_800F39F0[arg0][0]; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4CC8);

void func_800A4D2C(s32 arg0) {
    u32 i;

    if (D_800F83C6) {
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
        result = func_800A4CC8(D_800E7A48[0], arg);
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

void func_800A4F14(s32 arg0)
{
    s32 temp_v0;

    temp_v0 = arg0 * 0x44;
    *(u16*)((u8*)&D_800F5BB8[0].unk8 + temp_v0) &= 0xFFFE;
    *(u8*)((u8*)&D_800F5BB8[0].unkE + temp_v0) |= 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4F60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A50E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5250);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A555C);

typedef struct {
    s16 a;
    s16 b;
} Unk800F3A40;

extern Unk800F3A40 D_800F3A40[];

void func_800A55BC(void)
{
    s32 i;

    for (i = 0; i < 16; i++) {
        D_800F3A40[i].a = -1;
        D_800F3A40[i].b = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A55F4);

void func_800A5660(s32, s16);
;
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5660);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A56B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5750);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5990);

const u8 D_800A0240[] = {
    0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0x54, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0xA8, 0x54, 0x0A, 0x80, 0x94, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0x14, 0x54, 0x0A, 0x80,
    0x34, 0x54, 0x0A, 0x80, 0x74, 0x54, 0x0A, 0x80};
const u8 D_800A0278[] = {
    0x05, 0x06, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x00, 0x5C, 0x5A,
    0x0A, 0x80, 0x88, 0x5A, 0x0A, 0x80, 0xA8, 0x5A, 0x0A, 0x80};
u8 func_800A5A5C(void) { return D_800A0278[func_80014BA8(7)]; }

s32 func_800A5A88(void) { return func_80014BA8(54); }

s32 func_800A5AA8(void) { return func_80014BA8(16) + 56; }

const u8 D_800A028C[] = {0x02, 0xFF, 0x01, 0x86};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5AC8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5BC8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5E0C);

s32 func_800A5EB0(s32, s16*);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5EB0);

s8* func_800A5F90(s32 arg0) { return &D_800F3A80[D_800F4280[arg0]]; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5FB0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6000);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A61D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6278);

void func_800A64A0(s32 arg0, s8 arg1) { D_800E7A58[arg0] = arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A64B4);

void func_800A653C(s32 arg0) {
    s32 index = func_800A44D8(arg0);
    D_800F5BB8[index].unk4 = 0;
    D_800F5F44.D_800F7DC2 &= ~(1 << index);
}

void func_800A6590(s32 arg0) { func_800A4D88(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A65B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A661C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A66A4);

void func_800A6720(s32 arg0, s16 arg1) { func_800A5660(10, arg1); }

void func_800A6748(s32 arg0) {
    func_800A653C(arg0);
    func_800A4D88(arg0);
    D_800F5F44.D_800F7DAC &= ~(1 << arg0);
}

void func_800A6798(s32 arg0, s32 arg1) { func_800A37F8(arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A67B8);

void func_800A6834(s32 arg0) { D_800F5F44.D_800F7DC4 &= ~(1 << arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6858);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A68FC);

void func_800A6A3C(s32 arg0, s32 arg1) { D_800F5BB8[arg0].unkE |= arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6A70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6AC4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6B1C);

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

extern s32 func_800151F4(s32);

void func_800A6C04(s32 arg0)
{
    s16 sp10;

    sp10 = (s16) D_801636B8[arg0].D_801636B8;
    D_800F5F44.D_800F7DBE = func_800A5EB0(func_800151F4(0x26), &sp10) + 0x100;
    D_800F5F44.D_800F7DC0 = 0xF;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6C5C);

extern const u8 D_800A04D0[];

void func_800A6CC0(s32 arg0, s32 arg1)
{
    u32 mask = ~(1 << D_800A04D0[arg1]);
    D_800F83E0[arg0].status &= mask;
}

void func_800A6D10(s32 arg0) { func_800A3E98(arg0, 3, 2, 54, 0); }

void func_800A6D3C(s32 arg0)
{
    s32 temp_v1;

    temp_v1 = D_800F83E0[arg0].status;
    if (temp_v1 & 0x2000) {
        D_800F83E0[arg0].status = (temp_v1 & ~0x2000) | 0x4000;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6D88);

void func_800A6DFC(void) {}

void func_800A6E04(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6E0C);

void func_800A7254(s32, s32, s32, s32);
void func_800A6E6C(s32 arg0, s32 arg1) { func_800A7254(0, arg0, 13, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6E9C);

void func_800B0FFC(s32, s32, s32, s16*);
void func_800A7034(s32 arg0, s16 arg1) {
    s16 out = arg1;
    func_800B0FFC(arg0, 0x53, 1, &out);
}

void func_800A7060(s32 arg0, s32 arg1) { func_800A7254(0, arg0, 12, arg1); }

void func_800A7090(s32 arg0)
{
    D_800F5BB8[arg0].unk29 |= 0x40;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A70C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7130);

void func_800A71E0(void) {}

s32 func_800A71E8(s32 arg0) { return (arg0 + 1) & 0x7F; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A71F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7254);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A72C8);

void func_800A73C0(void) { D_80063014->unk2C = D_80063014->unk10; }

void func_800A73D8(void) { D_80063014->unk2C = D_80063014->unk10 + 56; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A73F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7458);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7560);

void func_800A76AC(void) { D_80063014->unk2C = D_80063014->unk10 + 72; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A76CC);

void func_800A7784(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A778C);

const s16 D_800A0290[] = {0, 56, 72, 96, 256};
const s32 D_800A029C[] = {
    0x140D0302, 0x3D3CFFFF, 0x41403F3E, 0xFFFFFF42, 0xFFFFFFFF,
    0x43424140, 0x47464544, 0xFF444843, 0xFFFFFFFF};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A784C);

void func_800A7940(void) {
    D_80063014->unk80 = 0x400000;
    D_80063014->unkE4 = 0x59;
}

void func_800B1060(s32);
void func_800A795C(void) { func_800B1060(D_80063014->unk10); }

void func_800AF9C8();
void func_800A7988(void) { func_800AF9C8(); }

void func_800A79A8(void) {
    D_80063014->unk50 = 0;
    D_80063014->unk18 = 1 << D_80063014->unk0;
}

void func_800A79CC();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A79CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A80DC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A81B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8424);

void func_800A8528(void) { D_80063014->unkB4 = 4; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A853C);

void func_800A85A0(void) { D_80063014->unkB4 = 2; }

void func_800A85B4(void) {
    D_80063014->unk44 = 0x10;
    D_80063014->unk48 = 1;
    D_80063014->unk50 = 0;
    if (!((D_80163758[1] >> D_80063014->unk0) & 1)) {
        D_80063014->unk20 = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A85FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A866C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8A6C);

void func_800A8C94(void)
{
    D_80063014->unk8C = 0xFF;
    D_80063014->unk40 = 0xB0;
    D_80063014->unk80 |= 1;
    D_80063014->unk3C = (s32) D_80063014->unk3C >> 1;
}

void func_800A8CC8(void)
{
    D_80063014->unk6C &= ~0x2000;
    D_80063014->unk3C /= 3;
}

void func_800A8D04(void) { D_80063014->unk48 = 2; }

// seed this combatant's unk50 (a flag word later read by the damage formula
// in func_800AD804 -- bit 0x80 there appears to exempt a hit from the
// reduced per-target damage otherwise applied when an action strikes
// multiple targets) with a per-slot default, but only if nothing has set
// unk50 explicitly yet this turn (see func_800A8D60's sentinel check)
void func_800A8D60(s32 arg0);
void func_800A8D18(void) { func_800A8D60(D_800F5EFC[D_80063014->unk0 * 0x18]); }

void func_800A8D60(s32 arg0) {
    if (D_80063014->unk50 == 0xFF) {
        D_80063014->unk50 = arg0;
    }
}

static void func_800A8D88(s32 arg0, s32 arg1) {
    D_80063014->unkBC = -1;
    if (arg0 != 0xFF) {
        D_80063014->unkBC = arg0;
        D_80063014->unkC0 = arg1;
        func_800A8E84(2);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8DCC);

void func_800A8E34(void) { func_800A79CC(); }

void func_800A8E54(s32 arg0) {
    D_80063014->unkF8 = arg0;
    D_80063014->unkAC = arg0 + 3;
    if (D_80063014->unkAC > 8) {
        D_80063014->unkAC = 8;
    }
}

const s16 D_800A02C0[] = {
    0x04, 0x3C, 0x04, 0x20, 0x01, 0x24, 0x10, 0x10, 0x04, 0x02, 0x02, 0x02,
    0x02, 0x01, 0x20, 0x04, 0x24, 0x10, 0x10, 0x04, 0x20, 0x10, 0x10, 0x10,
    0x30, 0x10, 0x20, 0x10, 0x10, 0x14, 0x01, 0x01, 0x01, 0x01, 0x01, 0x18};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8E84);

static void func_800A9C24(void) {
    s32 temp_s0;
    s32 var_a0;
    s32 var_s0;
    s32 i;

    i = 0;
    var_a0 = 0;
    while (i < 7) {
        if (D_80163774[0] == D_800E7BA4[var_a0 + 0] &&
            D_80163774[1] == D_800E7BA4[var_a0 + 1] &&
            D_80163774[2] == D_800E7BA4[(var_s0 = var_a0) + 2]) {
            break;
        }
        i++;
        var_a0 += 3;
    }
    if (i) {
        D_80063014->unk2C = i + 0x68;
    } else {
        var_s0 = 4;
        for (i = 0; i < 4; i++) {
            var_s0 += func_80014BA8(10) & 0xFF;
            func_80014B54();
        }
        var_s0 += D_80063014->unk4 / 21;
        var_s0 /= 2;
        var_s0 -= 4;
        if (var_s0 < 0) {
            var_s0 = 0;
        }
        if (var_s0 > 0xF) {
            var_s0 = 0xF;
        }
        D_80063014->unk2C = var_s0 + 0x38;
        D_80063014->unk28 = 3;
    }
    D_80063014->unk50 = 0xFF;
    D_80063014->unk98 = D_80063014->unk2C;
    temp_s0 = D_80063014->unk20;
    func_800A8E34();
    D_80063014->unk20 = temp_s0;
    D_80063014->unk38 = 0;
}

const u8 D_800A0398[] = {0x64, 0x14, 0x14, 0x14, 0xEC, 0xCE, 0xCE, 0x00};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A9DA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA1C8);

static void func_800AA468(void) {
    s32 temp_s0;
    s32 var_s1;

    var_s1 = D_80063014->unkC8;
    if (func_800B10B4(D_80063014->unk0)) {
        var_s1 |= 2;
    }
    temp_s0 = func_80014A58(var_s1 & 0x0400029A);
    temp_s0 += func_80014A58(var_s1 & 0x202000) * 2;
    D_80063014->unk214 *= temp_s0 + 1;
}

static void func_800AA4FC(void) {
    s32 var_s0;

    var_s0 = 1;
    if (func_800B10B4(D_80063014->unk0) != 0) {
        var_s0 = 2;
    }
    if (D_80063014->unkC8 & 0x200000) {
        var_s0 *= 4;
    }
    D_80063014->unk214 *= var_s0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA574);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA688);

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

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB674);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB788);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB830);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB9C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ABA68);

const s32 D_800A03A0[] = {0x200, 0x100, 0x010, 0x020};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ABB0C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AC6B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AC73C);

void func_800ACA24(void) {
    D_80063014->unk238 = 0;
    D_80063014->unk23C = 0;
    D_80063014->unk240 = 0;
    D_80063014->unk244 = 0;
    D_80063014->unk230 = 0;
    D_80063014->unk214 = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACA4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACB98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACD88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACE14);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACE88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD0FC);

void func_800AD324(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_t0;
    s32 var_a2;

    var_a2 = arg2;
    temp_t0 = D_80063014->unk220 & 1;
    if (arg3 & 1) {
        if (arg1 == D_80063014->unk208) {
            if (D_80063014->unk25C < var_a2) {
                var_a2 = D_80063014->unk25C;
            }
        }
        if (temp_t0) {
            var_a2 = -var_a2;
        }
        D_800F5BB8[arg0].unk30 -= var_a2;
    }
    if (arg3 & 2) {
        if (arg1 == D_80063014->unk208) {
            if (D_80063014->unk258 < var_a2) {
                var_a2 = D_80063014->unk258;
            }
        }
        if (temp_t0) {
            var_a2 = -var_a2;
        }
        D_800F5BB8[arg0].unk2C -= var_a2;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD420);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD4EC);

const s8 D_800A04B0[] = {
    0x0A, 0x0B, 0x0C, 0x0D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x7F, 0x03, 0x34};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD5E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD73C);

// multi-target damage-reduction formula, s32 func_800AD804(s32 damage, s32
// fullDamage): if fullDamage is false, it still gets forced true when
// unkB8 < 2 (single target) or unk50 & 0x80 is set (the exemption bit
// documented on unk50's seed at func_800A8D18/func_800A8D60 above); then
// if unkAC != 0 (hit-sequence position, see func_800A8E54) returns
// damage>>1, else returns damage unchanged when fullDamage else damage/3
// (magic-number signed divide) -- this is the classic "multi-target hits
// deal reduced per-target damage" mechanic
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD804);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD890);

// scale arg0 by a fixed-point random variance factor (~93.77%..100%), then
// clamp the result to a minimum of 1
static s32 func_800AD8DC(s32 arg0) {
    s32 temp_s0;
    s32 var_v0;

    var_v0 = arg0;
    temp_s0 = ((s32)(var_v0 * (func_80014B70() + 0xF01))) >> 0xC;
    var_v0 = temp_s0;
    if (temp_s0 == 0) {
        var_v0 = 1;
    }
    return var_v0;
}

void func_800AD924(void) { D_80063014->unk218 |= 2; }

void func_800AD944();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD944);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADBBC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADC70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADD2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADDE8);

void func_800ADE5C(void) { D_80063014->unk214 = D_80063014->unk48 * 20; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADE84);

void func_800ADED8(void) {
    if (D_80063014->unk230 & 0x40) {
        D_80063014->unk230 = 1;
    } else {
        D_80063014->unk230 = 0x80;
    }
}

void func_800ADF04(void) {
    D_80063014->unk4C = D_80063014->unkD8 * 2;
    func_800AD944();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADF38);

void func_800ADFC0(void)
{
    D_80063014->unk214 = *(u16*)(&D_800F5BB8[D_80063014->unk0].unk3C);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADFF4);

void func_800AE050(void) {}

void func_800AE058(void) {}

void func_800AE060(void) {}

void func_800AE068(void) {}

void func_800AE070(void) {}

void func_800AE078(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE080);

void func_800AE234(void) {
    D_80063014->unk214 =
        Savemap.memory_bank_1[26] + Savemap.memory_bank_1[27] * 256;
}

void func_800AE25C(void)
{
    D_80063014->unk214 = D_800F83E0[D_80063014->unk208].curHP - 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE2A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE318);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE378);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE42C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE6C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE764);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE82C);

void func_800AE954();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE954);

const s8 D_800A04BC[] = {
    0x1E, 0x14, 0x3C, 0x1E, 0x7F, 0x7F, 0x0A, 0x64, 0x7F, 0x7F,
    0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x0D, 0x00, 0x00};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEB20);

// this data belong to functions located above:
const u8 D_800A04D0[] = {0x0A, 0x19, 0x15, 0x0D, 0x10, 0x11, 0x03, 0x02,
                         0x0F, 0x1B, 0x14, 0x18, 0xFF, 0xFF, 0xFF, 0xFF};
int func_800B0378();
int func_800B062C();
int func_800B079C();
int func_800B0B8C();
int func_800B0910();
int func_800B089C();
int (* const D_800A04E0[])() = {
    func_800B0378, func_800B062C, func_800B079C, func_800B0B8C,
    func_800B0B8C, func_800B0B8C, func_800B0910, func_800B089C,
};
// ___end

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEB80);

void func_800AEBF0(void) { func_800AE954(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEC10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEF68);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF0C4);

void func_800A7254(s32, s32, s32, s32);
void func_800AF1A8(s32 arg0) { func_800A7254(0, arg0, 8, 0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF1D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF264);

void func_800AF320(s32 arg0, s32 arg1, s32 arg2) {
    func_800AEBF0();
    func_800AEB80(arg0, arg1, arg2);
    func_800AF0C4(arg0, arg1, arg2);
}

void func_800AF380(s32 arg0) { func_800A7254(2, arg0, 0x15, 0xF); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF3AC);

void func_800AF470(s32 arg0) { D_800F5BB8[arg0].unk28 = 3; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF494);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF594);

void func_800B108C(s32 arg0);
void func_800AF63C(s32 arg0) { func_800B108C(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF65C);

static s32 func_800AF834(s32 arg0) {
    s32 result;
    s32 i;

    result = -1;
    for (i = 0; i < LEN(D_800A04D0); i++) {
        if (D_800A04D0[i] == arg0) {
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

s32 func_800AFE98(s32 arg0)
{
    return D_800F49F8[D_800F499C][arg0] >> 0xC;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AFECC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0170);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0234);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0378);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B062C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B079C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B089C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0910);

int func_800B0B8C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0B94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0C14);

void func_800B0DF8(void) {
    if (D_80063014->unk234 & 2) {
        D_800F83E0[D_80063014->unk208].unk4 ^= 0x80;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0E5C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0EB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0F04);

void func_800B0FFC(s32 arg0, s32 arg1, s32 arg2, s16* arg3) {
    func_800A31A0(
        arg0, 2, arg2, func_800A5EB0(func_8001521C(arg1), arg3) + 0x100);
}

void func_800B1060(s32 arg0) { func_800A31A0(10, 2, 1, arg0); }

void func_800B108C(s32 arg0) { func_800A31A0(arg0, 5, 0, 0); }

// true when the combatant's HP is at or below a quarter of max -- the "Near
// Death" threshold used by weapon-specific damage formulas (e.g. Powersoul's
// HP-based multiplier).
static s32 func_800B10B4(s32 arg0) {
    return D_800F83E0[arg0].curHP <= D_800F83E0[arg0].maxHP / 4;
}

void func_800B10F0(s32, s32, s32, s32, s32, s32, s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B10F0);

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

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1218);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1268);

// nonzero if D_800F5F44.D_800F7DC8 is < 3
static u32 func_800B12DC(void) {
    u32 result = 0;
    s32 cmp = (s32)D_800F5F44.D_800F7DC8;

    if (cmp < 3) {
        result = (u32)~D_800F5F44.D_800F7DC8 >> 0x1F;
    }
    return result;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1368);

// Resolve a packed variable reference for the battle-script VM (func_800B1D48):
// map combatant arg0 + descriptor arg1 to a backing pointer (*arg2) and return
// a bit offset into it. arg1 < 0x2000 selects the per-combatant variable bank
// D_800F87F0[arg0] (0x80 bytes each); arg1 < 0x4000 selects the shared,
// battle-wide bank D_800F83A4; otherwise the per-combatant stat record
// D_800F83E0[arg0] (0x68 bytes each). func_800B153C / func_800B141C then read
// or write at that bit offset.
static s32 func_800B13B0(s32 arg0, s32 arg1, void** arg2) {
    s32 var_a1;

    var_a1 = arg1;
    if (var_a1 < 0x2000) {
        *arg2 = &D_800F87F0[arg0 * 0x80];
    } else if (var_a1 < 0x4000) {
        *arg2 = D_800F83A4;
        var_a1 -= 0x2000;
    } else {
        *arg2 = &D_800F83E0[arg0];
        var_a1 -= 0x4000;
    }
    return var_a1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B141C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B153C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1624);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B16D0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B17F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B18A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1A5C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1AA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1B64);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1C94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1D48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2A2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2B5C);

u16* func_800B2EBC();
s32 func_800B2C60(s32 arg0) {
    s32 var_s0;
    u16* ret;

    var_s0 = 0;
    if (arg0 <= 0xFFFE) {
        ret = func_800B2EBC();
        if (ret) {
            var_s0 = ret[2];
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

u16* func_800B2EBC();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2EBC);

u8 func_800B2F30(void) { return func_80014B70(); }

u16 func_800B2F50(void) { return func_80014BE4(); }

// scale a 16-bit value into the range 1..100
static s32 func_800B2F70(void) {
    return (((func_800B2F50() & 0xFFFF) * 0x63) / 0xFFFF) + 1;
}

static s32 func_800B2FC4(s32 arg0) {
    return (arg0 * (func_800B2F30() + 0xF01)) >> 12;
}

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
