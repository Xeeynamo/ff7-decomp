//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <libgte.h>
#include <psxsdk/inline_c.h>
#include "chocobo_private.h"
#include <libetc.h>

typedef struct {
    u32 unk0;
    u32 unk4;
} UnkRectData;

extern UnkRectData D_800A0020;
extern UnkRectData D_800A0028;

extern VECTOR D_800A00A8;
extern SVECTOR D_800A00B8;

inline s32 func_800A89A0(s16 x, s16 y);

void func_800A157C(void) {
    Unk800B1254* table;
    ChocoboModel* model;
    Chocobo* chocobo;
    s32 i;

    for (i = 0, table = &D_800B1254; i < NUM_CHOCOBO; i++) {
        chocobo = &D_800B75CC[i];
        model = &table->unk0->models[chocobo->unk92];
        table->unk0->models[chocobo->unk90].r = model->r = 0xC0;
        table->unk0->models[chocobo->unk90].g = model->g = 0;
        table->unk0->models[chocobo->unk90].b = model->b = 0x80;
    }
}

void func_800A1630(void) {
    ChocoboTrack* track;
    u32 pad;

    track = (ChocoboTrack*)0x80110000;
    D_800F5078.track = track;
    D_800B7514 = 0xFF;
    *(u32*)&D_800F5040.fadeSpeed = -0x10;
    D_800B759C = -1;
    D_800B74FC = track->count;
    D_800B7478 = 0;
    D_800B7500 = track->segments;
    D_800B747C = -((Savemap.memory_bank_1[0] + (Savemap.memory_bank_1[1] << 8)) >= 1000);
    pad = InputReadPadsRaw() >> 16;
    if ((pad & (PADRup | PADRdown | PADRleft | PADRright)) == (PADRup | PADRdown | PADRleft | PADRright)) {
        if (pad & PADl) {
            Savemap.memory_bank_3[6] = 1;
        }
        if (pad & PADm) {
            Savemap.memory_bank_3[6] = 2;
        }
    }
    if (Savemap.memory_bank_3[8]) {
        Savemap.memory_bank_3[9] = 0xFF;
    }
    D_800B7530.unk8 = 0;
    D_800B74F8 = 0;
    D_800B7530.unkC = 0;
    D_800F5040.unkC = -1;
    D_800F5078.unk14 = -1;
    D_800F5040.unk10 = (D_800F5078.unk20 + 3) * 2;
    D_800F5078.unk20 = Savemap.memory_bank_3[23];
    if (Savemap.memory_bank_3[9]) {
        D_800B7A48.unk0 = -1;
    } else {
        D_800B7A48.unk0 = 0;
    }
    D_800F5124 = 1;
    D_800F5078.unk10 = -1;
    D_800B7594 = -1;
    D_800F5040.unk0 = 0;
    D_800F5040.unk8 = 0;
    D_800B7A48.unk1C = 0;
    D_800F5040.unk4 = 0;
    D_800B75CC[0].unk94 = 30;
    D_800B7530.unkC = 0;
}

void func_800A17F0(void) {
    g_AkaoCmd.opcode = AKAO_PLAY_FOUR_SOUNDS;
    g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
    g_AkaoCmd.params[4] = 0;
    g_AkaoCmd.params[3] = 0;
    g_AkaoCmd.params[2] = 0;
    g_AkaoCmd.params[1] = 0;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_SOUND_VOL_BALANCE_SLOT2;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_SOUND_VOL_BALANCE_SLOT1;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_SOUND_VOL_BALANCE_SLOT0;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_SOUND_VOL_BALANCE_SLOT3;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A18BC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A1F40);

static void func_800A272C(s32 arg0, s32 arg1) {
    RECT sp10;
    RECT sp18;
    s32 var_a0;
    u32 var_a1;

    sp10 = *(RECT*)&D_800A0020;
    sp18 = *(RECT*)&D_800A0028;

    if (arg0 != 0) {
        SysCdromStartLoadLzs(0x3C1, 0x20000, (u_long*)0x80110000, 0);
    } else {
        SysCdromStartLoadLzs(0x32C, 0x20000, (u_long*)0x80110000, 0);
    }

    while (SystemCdromReadChain()) {
    }

    LoadImage(&sp10, (u_long*)0x80110000);
    DrawSync(0);

    if (arg0 != 0) {
        SysCdromStartLoadLzs(0x3F1, 0x1E000, (u_long*)0x80190000, 0);
    } else {
        SysCdromStartLoadLzs(0x3CE, 0x1E800, (u_long*)0x80190000, 0);
    }

    while (SystemCdromReadChain()) {
    }

    switch (arg1) {
    case 0:
        SysCdromStartLoadLzs(0x459, 0x30000, (u_long*)0x80110000, 0);
        break;
    case 1:
        SysCdromStartLoadLzs(0x433, 0x30000, (u_long*)0x80110000, 0);
        break;
    case 2:
        SysCdromStartLoadLzs(0x417, 0x30000, (u_long*)0x80110000, 0);
        break;
    case 3:
        SysCdromStartLoadLzs(0x49C, 0x30000, (u_long*)0x80110000, 0);
        break;
    }

    while (SystemCdromReadChain()) {
    }

    LoadImage(&sp18, (u_long*)0x80110000);
    DrawSync(0);

    if (arg0) {
        SysCdromStartLoadLzs(0x33E, 0x6A000, (u_long*)0x80110000, 0);
    } else {
        SysCdromStartLoadLzs(0x293, 0x7D000, (u_long*)0x80110000, 0);
    }

    while (SystemCdromReadChain()) {
    }
}

static void func_800A28D8(void) {
    SysCdromStartLoadLzs(0x4C9, 0x1000, (u_long*)&D_80077F64[0][0x2000], NULL);
    do {

    } while (SystemCdromReadChain());
    SysCdromStartLoadLzs(0x4CA, 0x1000, (u_long*)&D_80077F64[0][0x3000], NULL);
    do {

    } while (SystemCdromReadChain());
    SysCdromStartLoadLzs(0x4C8, 0x800U, (u_long*)&D_80077F64[1][0xC00], NULL);
    do {

    } while (SystemCdromReadChain());
    SysCdromStartLoadLzs(0x4C7, 0x800U, (u_long*)&D_80077F64[1][0x1400], NULL);
    do {

    } while (SystemCdromReadChain());
}

void func_800A2984(void) {
    ChocoboTri* tri;
    POLY_G3* p;
    OT_TYPE* ot;
    s32 flag;
    VECTOR unused;
    s32 opz;
    GpuBuffer** gfx;
    s32 i;
    s32 n;

    n = 0;
    for (i = 0; i < (*&D_800F5078.track)->nTris; i++) {
        gfx = &D_800F5074;
        tri = &(*&D_800F5078.track)->tris[i];
        gte_ldv3c(tri->v);
        gte_rtpt();
        p = &(*gfx)->polys[n];
        *(u32*)&p->r0 = tri->rgb[0];
        setPolyG3(p);
        gte_readflg(flag);
        if (flag < 0) {
            continue;
        }
        gte_nclip();
        *(u32*)&p->r1 = tri->rgb[1];
        *(u32*)&p->r2 = tri->rgb[2];
        gte_stopz(&opz);
        if (opz < 0) {
            continue;
        }
        if (n > 0xF8) {
            return;
        }
        gte_stsxy3_g3(p);
        ot = &(*gfx)->ot2[1];
        addPrim(ot, p);
        n++;
    }
}

void func_800A2AFC(void) {
    s32 count;
    s32 start;
    s32 end;

    count = D_800F5078.track->count;
    start = (D_800B7598 + count) % count;
    end = (D_800F5078.unk4 + count) % count;
    D_800F5034 = 0;
    if (end < start) {
        func_800A2BD4(0, end);
        func_800A2BD4(start, count);
    } else {
        func_800A2BD4(start, end);
    }
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A2BD4);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A3308);

ChocoboPrizeTable(
    D_800B22D0, 22, //
    {18, 1, 5, 0},  // Turbo Ether
    {10, 0, 5, 0},  // Hero Drink
    {9, 0, 5, 1},   // Elixir
    {1, 0, 5, 1},   // Counter
    {5, 1, 5, 1},   // Enemy Away
    {6, 1, 5, 1},   // Sneak Attack
    {14, 1, 5, 0},  // Swift Bolt
    {15, 1, 5, 0},  // Fire Veil
    {11, 0, 2, 0},  // Bolt Plume
    {15, 1, 5, 0},  // Fire Veil
    {20, 0, 20, 0}, // Phoenix Down
    {16, 0, 5, 0},  // Ice Crystal
    {17, 1, 5, 1},  // Megalixir
    {18, 0, 5, 0},  // Turbo Ether
    {0, 1, 5, 1},   // Sprint Shoes
    {14, 0, 5, 0},  // Swift Bolt
    {4, 1, 5, 1},   // Cat's Bell
    {9, 0, 5, 1},   // Elixir
    {7, 1, 5, 1},   // Chocobracelet
    {16, 1, 5, 0},  // Ice Crystal
    {3, 1, 5, 1},   // Precious Watch
    {2, 1, 5, 1}    // Magic Counter
);

ChocoboPrizeTable(
    D_800B232C, 20, //
    {8, 0, 20, 0},  // Ether
    {10, 0, 10, 0}, // Hero Drink
    {14, 1, 10, 0}, // Swift Bolt
    {15, 1, 10, 0}, // Fire Veil
    {12, 0, 10, 0}, // Fire Fang
    {16, 1, 10, 0}, // Ice Crystal
    {1, 0, 10, 1},  // Counter
    {5, 1, 5, 1},   // Enemy Away
    {11, 0, 10, 0}, // Bolt Plume
    {12, 0, 10, 0}, // Fire Fang
    {13, 0, 10, 0}, // Antarctic Wind
    {9, 0, 10, 1},  // Elixir
    {23, 0, 5, 0},  // Hi-Potion
    {8, 0, 20, 0},  // Ether
    {0, 1, 7, 1},   // Sprint Shoes
    {9, 0, 5, 1},   // Elixir
    {20, 0, 20, 0}, // Phoenix Down
    {12, 0, 10, 0}, // Fire Fang
    {4, 1, 7, 1},   // Cat's Bell
    {6, 1, 7, 1}    // Sneak Attack
);

ChocoboPrizeTable(
    D_800B2380, 15, //
    {8, 0, 30, 0},  // Ether
    {10, 0, 10, 0}, // Hero Drink
    {20, 0, 20, 0}, // Phoenix Down
    {18, 0, 5, 1},  // Turbo Ether
    {8, 0, 30, 0},  // Ether
    {23, 0, 5, 0},  // Hi-Potion
    {21, 0, 10, 0}, // Hyper
    {22, 0, 10, 0}, // Tranquilizer
    {23, 0, 5, 0},  // Hi-Potion
    {11, 0, 10, 1}, // Bolt Plume
    {12, 0, 10, 0}, // Fire Fang
    {13, 0, 10, 0}, // Antarctic Wind
    {9, 0, 10, 1},  // Elixir
    {5, 1, 5, 1},   // Enemy Away
    {23, 0, 5, 0}   // Hi-Potion
);

ChocoboPrizeTable(
    D_800B23C0, 10, //
    {23, 0, 5, 1},  // Hi-Potion
    {8, 0, 20, 1},  // Ether
    {21, 0, 10, 0}, // Hyper
    {22, 0, 10, 0}, // Tranquilizer
    {20, 0, 20, 0}, // Phoenix Down
    {11, 0, 5, 1},  // Bolt Plume
    {12, 0, 5, 0},  // Fire Fang
    {13, 0, 5, 0},  // Antarctic Wind
    {19, 0, 5, 1},  // Potion
    {20, 0, 20, 1}  // Phoenix Down
);

// clang-format off
u8 D_800B23EC[][16] = {
    _SF(16, "Sprint Shoes"),
    _SF(16, "Counter"),
    _SF(16, "Magic Counter"),
    _SF(16, "Precious Watch"),
    _SF(16, "Cat's Bell"),
    _SF(16, "Enemy Away"),
    _SF(16, "Sneak Attack"),
    _SF(16, "Chocobracelet"),
    _SF(16, "Ether"),
    _SF(16, "Elixir"),
    _SF(16, "Hero Drink"),
    _SF(16, "Bolt Plume"),
    _SF(16, "Fire Fang"),
    _SF(16, "Antarctic Wind"),
    _SF(16, "Swift Bolt"),
    _SF(16, "Fire Veil"),
    _SF(16, "Ice Crystal"),
    _SF(16, "Megalixir"),
    _SF(16, "Turbo Ether"),
    _SF(16, "Potion"),
    _SF(16, "Phoenix Down"),
    _SF(16, "Hyper"),
    _SF(16, "Tranquilizer"),
    _SF(16, "Hi-Potion"),
};

u8 D_800B256C[][8] = {
    _SF(8, "SAM"),
    _SF(8, "ELEN"),
    _SF(8, "BLUES"),
    _SF(8, "TOM"),
    _SF(8, "JOHN"),
    _SF(8, "GARY"),
    _SF(8, "MIKE"),
    _SF(8, "SANDY"),
    _SF(8, "JU"),
    _SF(8, "LY"),
    _SF(8, "JOEL"),
    _SF(8, "GREY"),
    _SF(8, "EDWARD"),
    _SF(8, "JAMES"),
    _SF(8, "HARVEY"),
    _SF(8, "DAN"),
    _SF(8, "RUDY"),
    _SF(8, "GRAHAM"),
    _SF(8, "FOX"),
    _SF(8, "CLIVE"),
    _SF(8, "SEAN"),
    _SF(8, "YOUNG"),
    _SF(8, "ROBIN"),
    _SF(8, "DARIO"),
    _SF(8, "ARL"),
    _SF(8, "SARA"),
    _SF(8, "MARIE"),
    _SF(8, "SAMMY"),
    _SF(8, "LIA"),
    _SF(8, "KNIGHT"),
    _SF(8, "PAULA"),
    _SF(8, "PAU"),
    _SF(8, "LE"),
    _SF(8, "PETER"),
    _SF(8, "AIMEE"),
    _SF(8, "TERRY"),
    _SF(8, "ANDY"),
    _SF(8, "NANCY"),
    _SF(8, "TIM"),
    _SF(8, "ROBER"),
    _SF(8, "GEORGE"),
    _SF(8, "JENNY"),
    _SF(8, "RICA"),
    _SF(8, "JULIA"),
};
// clang-format on

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A34A8);

void func_800A44E4(void) {
    s32 keys[NUM_CHOCOBO];
    s32 ids[16]; // only the first NUM_CHOCOBO are used
    s32 i;
    s32 j;
    s32 tmp;
    Chocobo* c;

    for (i = 0; i < NUM_CHOCOBO; i++) {
        c = &D_800B75CC[i];
        keys[i] = c->unk0;
        ids[i] = i;
    }
    for (i = 0; i < NUM_CHOCOBO; i++) {
        for (j = i; j < NUM_CHOCOBO; j++) {
            if (keys[i] < keys[j]) {
                tmp = keys[i];
                keys[i] = keys[j];
                keys[j] = tmp;
                tmp = ids[i];
                ids[i] = ids[j];
                ids[j] = tmp;
            }
        }
    }
    for (i = 0; i < NUM_CHOCOBO; i++) {
        D_800B75CC[ids[i]].rank = i;
    }
    D_800B733C = ids[NUM_CHOCOBO - 1];
    for (i = 0; i < NUM_CHOCOBO; i++) {
        func_800A4888(i);
    }
    if (D_800B7A48.unk0) {
        func_800A6E50(1);
    } else {
        func_800A6E50(0);
    }
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A4888);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A500C);

void func_800A68D4(s32 id) {
    VECTOR dir;
    Chocobo* c;
    s32 w;

    if (D_800F5078.unk10 != id) {
        D_800F5078.unk10 = id;
        c = &D_800B75CC[6];
        *c = D_800B75CC[id];
        c->unk7C = 0x7F;
        w = 0x81;
        c->unk10 = (D_800B7500[c->unk0].p0.vx * c->unk7C + D_800B7500[c->unk0].p1.vx * w) / 256;
        c->unk12 = (D_800B7500[c->unk0].p0.vy * c->unk7C + D_800B7500[c->unk0].p1.vy * w) / 256;
        c->unk14 = (D_800B7500[c->unk0].p0.vz * c->unk7C + D_800B7500[c->unk0].p1.vz * w) / 256;
        c->unk28 = (D_800B7500[c->unk2].p0.vx * c->unk7C + D_800B7500[c->unk2].p1.vx * w) / 256;
        c->unk2A = (D_800B7500[c->unk2].p0.vy * c->unk7C + D_800B7500[c->unk2].p1.vy * w) / 256;
        c->unk2C = (D_800B7500[c->unk2].p0.vz * c->unk7C + D_800B7500[c->unk2].p1.vz * w) / 256;
        dir.vx = c->unk28 - c->unk10;
        dir.vy = 0;
        dir.vz = c->unk2C - c->unk14;
        VectorNormal(&dir, &dir);
        c->unk3A = func_800A89A0(dir.vx, dir.vz);
    }
}

void func_800A6B9C(s32 chocoboId, s32 speed, s32 seg) {
    VECTOR dir;
    Chocobo* c;
    s32 next;
    s32 w;

    D_800F5078.unk10 = chocoboId;
    c = &D_800B75CC[NUM_CHOCOBO];
    *c = D_800B75CC[chocoboId];
    next = (seg + 1 + D_800B74FC) % D_800B74FC;
    c->unk7C = 0x7F;
    w = 0x81;
    c->unk0 = seg;
    c->speed = speed;
    c->unk2 = next;
    c->unk10 = (D_800B7500[seg].p0.vx * c->unk7C + D_800B7500[seg].p1.vx * w) / 256;
    c->unk12 = (D_800B7500[seg].p0.vy * c->unk7C + D_800B7500[seg].p1.vy * w) / 256;
    c->unk14 = (D_800B7500[seg].p0.vz * c->unk7C + D_800B7500[seg].p1.vz * w) / 256;
    c->unk28 = (D_800B7500[next].p0.vx * c->unk7C + D_800B7500[next].p1.vx * w) / 256;
    c->unk2A = (D_800B7500[next].p0.vy * c->unk7C + D_800B7500[next].p1.vy * w) / 256;
    c->unk2C = (D_800B7500[next].p0.vz * c->unk7C + D_800B7500[next].p1.vz * w) / 256;
    dir.vx = c->unk28 - c->unk10;
    dir.vy = 0;
    dir.vz = c->unk2C - c->unk14;
    VectorNormal(&dir, &dir);
    c->unk3A = func_800A89A0(dir.vx, dir.vz);
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A6E50);

void func_800A7840(s16 id, s16 arg1) {
    if (D_800B7A48.unk0 == -1) {
        D_800F5040.event.type = -1;
        D_800F5028 = arg1;
        D_800F5078.unk24 = id;
        return;
    }
    D_800B1358 = -1;
    if (id != -1) {
        D_800B7530.unkC = arg1;
        D_800F5078.unk14 = arg1;
        D_800F5040.event = D_800F5078.track->events[id - 1];
    }
}

void func_800A7924(MATRIX* m, SVECTOR* eye, SVECTOR* at) {
    VECTOR dir;
    VECTOR side;
    VECTOR fwd;
    VECTOR up = D_800A0068;

    dir.vx = at->vx - eye->vx;
    dir.vy = at->vy - eye->vy;
    dir.vz = at->vz - eye->vz;
    VectorNormal(&dir, &fwd);
    OuterProduct12(&fwd, &up, &dir);
    VectorNormal(&dir, &side);
    OuterProduct12(&fwd, &side, &dir);
    VectorNormal(&dir, &up);
    m->m[0][0] = side.vx;
    m->m[0][1] = side.vy;
    m->m[0][2] = side.vz;
    m->m[1][0] = up.vx;
    m->m[1][1] = up.vy;
    m->m[1][2] = up.vz;
    m->m[2][0] = fwd.vx;
    m->m[2][1] = fwd.vy;
    m->m[2][2] = fwd.vz;
    ApplyMatrix(m, eye, &dir);
    m->t[0] = -dir.vx;
    m->t[1] = -dir.vy;
    m->t[2] = -dir.vz;
}

void func_800A7AB8(void) {
    s32 speed;
    s32* pending;

    if (D_800F5040.event.type == 2) {
        switch (D_800F5040.event.unk1) {
        case 0:
            speed = 0;
            break;
        case 1:
            speed = D_800B75CC[D_800B7530.unkC].speed / 2;
            break;
        case 2:
            speed = D_800B75CC[D_800B7530.unkC].speed;
            break;
        case 3:
            speed = D_800B75CC[D_800B7530.unkC].speed * 2;
            break;
        }
        pending = &D_800B1358;
        if (*pending) {
            func_800A6B9C(D_800B7530.unkC, speed, D_800F5040.event.unkA);
            *pending = 0;
        }
    } else {
        func_800A68D4(D_800B7530.unkC);
    }
    if ((D_800F5040.unk8 && !D_800B7A48.unk0) || (D_800B7A48.unk0 && D_800B75CC[0].unk7E)) {
        D_800F5040.event.type = 10;
        D_800B759C = -1;
        D_800B7530.unkC = D_800B7594;
    }
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A7CA4);

void func_800A869C(void) {
    VECTOR pos = D_800A00A8;
    MATRIX m;
    SVECTOR rot = D_800A00B8;
    int flag;
    s32* snap;

    snap = &D_800B759C;
    if (*snap) {
        D_800B7340 = D_800B1348;
        D_800B7348 = D_800B1350;
        *snap = 0;
    }
    D_800B7348.vx = (D_800B7348.vx * 3 + D_800B1350.vx) / 4;
    D_800B7348.vy = (D_800B7348.vy * 7 + D_800B1350.vy) / 8;
    D_800B7348.vz = (D_800B7348.vz * 3 + D_800B1350.vz) / 4;
    D_800B7340.vx = (D_800B7340.vx * 3 + D_800B1348.vx) / 4;
    D_800B7340.vy = (D_800B7340.vy * 7 + D_800B1348.vy) / 8;
    D_800B7340.vz = (D_800B7340.vz * 3 + D_800B1348.vz) / 4;
    func_800A7924(&m, &D_800B7340, &D_800B7348);
    RotMatrixYXZ(&rot, &D_800B7544);
    TransMatrix(&D_800B7544, &pos);
    MulMatrix2(&m, &D_800B7544);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    RotTrans((SVECTOR*)&pos, (VECTOR*)D_800B7544.t, &flag);
    SetRotMatrix(&D_800B7544);
    SetTransMatrix(&D_800B7544);
}

void func_800A8940(s16 node) {
    if (node > 0x80) {
        D_800F5078.track->nodes[node - 0x80].step = -1;
    } else {
        D_800F5078.track->nodes[node].step = 1;
    }
}

#include "acos.h"

inline s32 func_800A89A0(s16 x, s16 y) {
    s32 i;

    if (x >= 0x1000) {
        return 0x400;
    }
    if (x <= -0x1000) {
        return 0xC00;
    }
    i = (u16)(x + 0x1000) & 0x1FFF;
    if (y > 0) {
        return (D_800B26CC[i] + 0x400) & 0xFFF;
    }
    return (D_800B26CC[0x2000 - i] + 0xC00) & 0xFFF;
}

s32 func_800A8A18(SVECTOR* from, SVECTOR* to) {
    VECTOR dir;

    dir.vx = to->vx - from->vx;
    dir.vy = to->vy - from->vy;
    dir.vz = to->vz - from->vz;
    VectorNormal(&dir, &dir);
    return func_800A89A0(dir.vx, dir.vz);
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A8AE8);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A9828);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A9A94);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800A9D94);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800AAC00);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800AAF1C);

extern ChocoboPrizePtr D_800B7458[1];
extern Unk800B7480 D_800B7480[5][3];
extern u8* D_800F502C;

void func_800AB410(void) {
    s32 ids[16];
    s32 groups[16];
    ChocoboPrizePtr* table;
    s32* out;
    s32 i;
    s32 j;
    s32 nPrize;
    s32 prizeIndex;
    s32 x;
    s32 k;
    s32 locked;
    s32 strIndex;
    s32 id;
    s32 swap;
    ChocoboPrize* e;

    switch (D_800F5078.unk20) {
    case 0:
        D_800B7458->entries = (ChocoboPrize*)&D_800B23C0;
        break;
    case 1:
        D_800B7458->entries = (ChocoboPrize*)&D_800B2380;
        break;
    case 2:
        D_800B7458->entries = (ChocoboPrize*)&D_800B232C;
        break;
    case 3:
        D_800B7458->entries = (ChocoboPrize*)&D_800B22D0;
        break;
    }
    i = 0;
    locked = 0;
    nPrize = *(s32*)D_800B7458->entries;
    D_800B7458->entries++; // skip the header with the amount of entries
    D_800B745C[0] = D_800B745C[1] = D_800B745C[2] = -1;
    strIndex = -1;
    table = D_800B7458;
    out = D_800B745C;
    while (i != 3) {
        prizeIndex = rand() % nPrize;
        id = table->entries[prizeIndex].strIndex;
        if (id == D_800B745C[0] || id == D_800B745C[1] || id == D_800B745C[2]) {
            continue;
        }
        if (table->entries[prizeIndex].unk1) {
            if (locked && table->entries[prizeIndex].unk3) {
                continue;
            }
            if (D_800B747C) {
                *out++ = id;
                i++;
            }
            if (table->entries[prizeIndex].unk3) {
                strIndex = table->entries[prizeIndex].strIndex;
                locked = -1;
            }
        } else {
            e = &table->entries[prizeIndex];
            if (locked && e->unk3) {
                continue;
            }
            *out++ = id;
            i++;
            if (e->unk3) {
                strIndex = e->strIndex;
                locked = -1;
            }
        }
        if (i >= 3) {
            break;
        }
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            if (D_800B745C[j] > D_800B745C[j + 1]) {
                swap = D_800B745C[j];
                D_800B745C[j] = D_800B745C[j + 1];
                D_800B745C[j + 1] = swap;
            }
        }
    }

    if (strIndex != -1) {
        if (D_800B745C[0] == strIndex) {
            D_800B745C[0] = D_800B745C[2];
            D_800B745C[2] = strIndex;
        } else if (D_800B745C[1] == strIndex) {
            D_800B745C[1] = D_800B745C[2];
            D_800B745C[2] = strIndex;
        }
    }

    for (i = 0; i < 7; i++) {
        ids[i] = D_800B745C[0];
        groups[i] = 0;
    }
    for (i = 7; i < 12; i++) {
        ids[i] = D_800B745C[1];
        groups[i] = 1;
    }
    for (i = 12; i < 15; i++) {
        ids[i] = D_800B745C[2];
        groups[i] = 2;
    }

    for (i = 0; i < 100; i++) {
        x = rand() % 15;
        j = rand() % 15;
        swap = ids[x];
        ids[x] = ids[j];
        ids[j] = swap;
        swap = groups[x];
        groups[x] = groups[j];
        groups[j] = swap;
    }

    for (k = 0; k < 3; k++) {
        for (i = 0; i < 5; i++) {
            (&D_800B7480[i][k])->unk0 = 0;
            (&D_800B7480[i][k])->unk2 = 0;
            (&D_800B7480[i][k])->unk3 = 0;
            (&D_800B7480[i][k])->unk6 = ids[k * 5 + i];
            (&D_800B7480[i][k])->unk7 = groups[k * 5 + i];
            (&D_800B7480[i][k])->unk5 = 2;
            setlen(&D_800B7A68[0].unk1E368[i][k], 9);
            setlen(&D_800B7A68[1].unk1E368[i][k], 9);
        }
    }
    D_800F502C = (u8*)&Savemap.gil;
    D_800B7530.unkC = 0;
}

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800ABABC);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800AC554);

INCLUDE_ASM("asm/us/mini/chocobo/nonmatchings/chocobo2", func_800AD52C);

void func_800AD7B8(const char* str, s32 len, s32 x, s32 y) { SysMenuDrawString(x, y, str, 7); }

void func_800AD7E8(void) {
    DRAWENV env;
    DR_ENV dr;
    DRAWENV* src;
    POLY_F4* prim;
    s32* fade;
    s32* speed;

    if (D_800B7514) {
        src = &D_800F5074->draw;
        env = *src;
        env.tpage = GetTPage(0, 2, 0, 0);
        env.isbg = 0;
        SetDrawEnv(&dr, &env);
        DrawPrim(&dr);
        prim = &D_800B14B4;
        prim->r0 = prim->g0 = prim->b0 = D_800B7514;
        DrawPrim(prim);
    }
    fade = &D_800B7514;
    speed = &D_800F5040.fadeSpeed;
    *fade += *speed;
    if (*fade < 0) {
        *fade = 0;
        *speed = 0;
    } else if (*fade > 0x100) {
        *fade = 0x100;
    }
}
