//! PSYQ=3.3 CC1=2.6.3 g=false gcoff=false
#include "world.h"
#include <libetc.h>

void WmSetActiveEntityDirectionAndRot(s16 arg0);
void WmGetPosFromPcEntity(VECTOR* arg0);
void WmRestoreEntityPosAndDirFromSavemap(WorldActor*);
s32 func_800ADFC0(void);
static s32 func_800B0800(void);
void func_800B58F8(u8*, RECT*);
static s32 func_800B716C(void);
static s32 func_800B7B2C(void);
s32 func_800B7B3C(void);
void func_800B87D8(s16 window);
void func_800B95E8(void);
s32 func_800B962C(s16 window, s16 stringId);
void func_800B98F0(s16 window);
void func_800B9B2C(s16 window);
void func_800BA938(s16 window);
void func_800BAA00(s16 window);
void func_800BAB60(s16 window);
s32 func_800BAC70(s16 window);
u16 func_800BAE60(s16 window);
void func_800BB350(u16 value, u8* dst);
void func_800BB450(u16 value, u8* dst);
void func_800BB568(u16 value, u8* dst);
void func_800BC9E8(s16 arg0);
s16 func_800BCA38(void);
void func_800BCA48(void);

const char D_800A0000[] = "NEW  ";
static const char D_800A0008[] = "OLD  ";
static const char D_800A0010[] = "JUMP ";
static const char D_800A0018[] = "FROM ";
static const char D_800A0020[] = "SCR-H";

static void func_800A0B40(s32 arg) {}

void WmSetRenderBuffers(void) {
    s32 flip;
    s32 off;
    u32* buf;

    flip = D_800D05E8 == 0;
    off = flip * 0x4074;
    D_800D05E8 = flip;
    D_800BD130 = (u32*)(D_800C8564 + off);
    buf = *(u32**)(D_800CC564 + off);
    D_800D05E4 = (D_800C8564 - 0x70) + off;
    D_800D05DC = buf;
    D_800D05E0 = buf;
    D_800C752C = D_800D05E8;
}

static s32 WmGetCurrRenderBufferId(void) { return D_800D05E8; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A0BE4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmLoadTxzFile);

static void func_800A0D1C(void) { D_800E55EC = 0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmLoadTxzDataAndInit);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A12AC);

void WmPrepareForRender(void) {
    s32 temp_s0;
    s32 var_a0;
    s32 var_a1;

    WmSetRenderBuffers();
    SetGeomScreen(D_800C65EC);
    if (D_800E5630 != 0) {
        var_a0 = (func_800ADFC0() >> 6) + 0x9E;
        var_a1 = (func_800ADFC0() >> 6) - 2;
        SetGeomOffset(var_a0, var_a1 + D_800E55F0);
    } else {
        SetGeomOffset(0xA0, D_800E55F0);
    }
    D_800C752D = 0xC;
    D_800C7530 = D_800BD130;
    ClearOTagR((OT_TYPE*)D_800BD130, 0x1000);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmRenderAll);

static void WmSetShaking(s32 arg0) { D_800E5630 = arg0; }

s32 func_800A16E0(void) {
    if (D_800E55F4 == 0) {
        return 0;
    }

    return (s32)D_800BD130 + 0x2710;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmIsTerrainPassableByModel);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A19FC);

static s32 func_800A1D04(void) { return D_800E5648; }

s32 WmGetRealCamRot(void) { return D_800E560C & 0xFFF; }

static void WmSetDesiredCamRot(s32 arg0) { D_800E5608 = arg0 & 0xFFF; }

void WmSetCamRot(s32 arg0) { D_800E560C = D_800E5608 = arg0 & 0xFFF; }

void func_800A1D54(s32 arg0) {
    D_800E5618 = arg0;
    if (arg0 == 1) {
        D_800E5638 = *(&D_800C6638 + D_800E5648);
        D_800E563C = *(&D_800C6628 + D_800E5648);
    }
}

static s32 func_800A1DB0(void) { return D_800E5618; }

static s32 func_800A1DC0(void) { return D_800E5654; }

static void func_800A1DD0(s32 arg0) { D_800E5654 = arg0; }

static s32 WmGetWmId(void) { return D_800E5634; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A1DF0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmSetTranslationVectorInScreenSpace);

void func_800A2040(void) {
    SetRotMatrix(&D_800E5698);
    SetTransMatrix(&D_800E56B8);
}

static s16 func_800A2078(void) { return D_800E56D8; }

void WmSetCamView(s32 arg0) {
    s32 temp_v0;

    temp_v0 = D_800E5648;
    D_800E5648 = arg0;
    D_800E5650 = temp_v0;
    if (arg0 < 2U && D_800E5634 != 3) {
        D_800E5608 = 0;
    }
    D_800E564C = D_800E5648 == 0 ? 0x78 : 0xA0;
    D_800E5604 = -(D_800E5648 != 3) & 0x78;
}

void func_800A2108(s32 arg0, s32 arg1) {
    VECTOR sp10;
    s32 var_a1;

    if (arg0 != 0) {
        if (arg1 == D_800E5600 || D_800E5600 == 6) {
            D_800E55FC = 1;
            WmGetPosFromPcEntity(&sp10);
            func_800A6994(&sp10, arg1 != 3 || D_800E5600 == 6);
        }
    } else {
        D_800E55FC = 0;
        D_800E5600 = arg1;
    }
}

s32 func_800A21A4(void) { return D_800E55FC; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmHandleButtons);

void func_800A31C0(s16 arg0) { D_800E5668 = arg0 - 0x800 + D_800E5608; }

s32 func_800A31E8(void) { return !D_800E5628; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A31F8);

s32 func_800A32F4(void) { return D_800E5678; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3304);

void func_800A368C(s32 arg0) { D_800E5658 = arg0; }

static s32 func_800A369C(void) { return D_800E5658; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A36AC);

void func_800A38C8(void) {
    if (g_PartyUpdatedByFieldScript == 1) {
        func_800260DC();
        func_80026090();
        g_PartyUpdatedByFieldScript = 0;
    }
}

void func_800A3908(void) {
    s32 ret;

    do {
        ret = func_800484A8();
        if (ret == -1) {
            VSync(0);
        }
    } while (ret == -1);
    if (ret) {
        do {
        } while (func_80048540(1));
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3964);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3C74);

void WmSubmarineFloatToPlanet(void) {
    D_800E566C = 5;
    D_800E5644 = 0x14;
    func_800B7714(0);
    func_800A2108(0, 0);
    func_800B0098(0x10, 1);
}

void WmSubmarineSubmergeUnderwater(void) {
    D_800E566C = 4;
    D_800E5644 = -0x14;
    func_800B7714(0);
    func_800A2108(0, 0);
    func_800B0098(0x10, 1);
}

void func_800A3E9C(s32 arg0) {
    func_800B7714(arg0);
    D_800E566C = 8;
}

void func_800A3EC8(s32 arg0) {
    if (D_800E566C <= 0) {
        D_800E563C = 0;
        D_800E5638 = 0;
        D_800E55F4 = 1;
        D_800E566C = 1;
    }
    if (D_800E566C == 1) {
        D_800E566C = 3;
        func_800A2108(0, 0);
        func_800B7714(arg0);
    }
}

void func_800A3F4C(s32 arg0) {
    if (D_800E566C <= 0) {
        D_800E563C = 0;
        D_800E5638 = 0;
        D_800E55F4 = 1;
        D_800E566C = 1;
    }
    if (D_800E566C > 0 && D_800E566C < 3) {
        if (D_80071E30 == 0) {
            D_800E566C = 3;
            func_800A2108(0, 0);
            func_800B77A8(arg0);
            return;
        }
        D_800E5670 = 0x1E;
        func_800B7C44();
    }
}

void WmResetGame(void) {
    if (D_800E566C <= 0) {
        D_800E563C = 0;
        D_800E5638 = 0;
        D_800E55F4 = 1;
        D_800E566C = 1;
    }
    if (D_800E566C == 1) {
        D_800E566C = 3;
        func_800A2108(0, 0);
        func_800B7838();
    }
}

s32 func_800A4080(void) { return (D_800E5648 & 3) | ((func_800BCA38() * 4) & 0xC); }

void func_800A40B8(s32 arg0) {
    WmSetCamView(arg0 & 3);
    func_800BC9E8((arg0 >> 2) & 3);
}

s32 func_800A40F0(s16 arg0) {
    if (arg0 < 0) {
        return 0;
    }
    if (arg0 >= D_800BE5F0[0]) {
        return 0;
    }

    return (s32)D_800BE5F0 + D_800BE5F0[arg0 + 1];
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4138);

void func_800A41E8(s32 arg0) {
    switch (arg0) {
    case 1:
        func_800A2108(0, 4);
        func_800B0098(4, 1);
        break;
    case 4:
        func_800A2108(1, 4);
        func_800AFFBC(0x10, 1);
        arg0 = 0;
        break;
    }
    D_800E56F4 = arg0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4268);

void func_800A4494(s32 arg0) { D_800E561C = arg0; }

void func_800A44A4(s32 arg0) { D_800E5620 = arg0; }

void func_800A44B4(s32 arg0) { D_800E5624 = arg0; }

void func_800A44C4(void) {
    s32 temp_a1;
    s32 temp_v1;
    s32 var_v0;
    s32 var_v0_2;

    switch (D_800E5618) {
    case 0:
        D_800E563C = 0;
        D_800E5638 = 0;
        D_800E5608 += D_800E561C;
        return;
    case 1:
        D_800E5608 += D_800E561C;
        D_800E5638 = D_800E5638 + D_800E5624;
        if (D_800E5638 >= 0x3A99)
            D_800E5638 = 0x3A98;
        else if (D_800E5638 < 0x3E8)
            D_800E5638 = 0x3E8;

        D_800E563C = D_800E563C + D_800E5620;
        if (D_800E563C >= 0x781)
            D_800E563C = 0x780;
        else if (D_800E563C < 0x480)
            D_800E563C = 0x480;
    }
}

void func_800A45C4(s32 arg0) { D_800E5638 = arg0; }

void func_800A45D4(s32 arg0) { D_800E563C = arg0; }

void func_800A45E4(s32 arg0) { D_800E5674 = arg0; }

s32 func_800A45F4(void) { return D_800E5674; }

static void func_800A4604(void) {}

// World Entry
INCLUDE_ASM("asm/us/world/nonmatchings/world", WmMain);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmInitLoadMapFileStruct);

// Converts x and y coords of megachunk into index
s16 WmGetBlockIdByXZForPlanet(s16 x, s16 y) {
    s16 var_a2;
    s16 var_v1;

    var_a2 = x;
    var_v1 = y;
    if (x < 0)
        var_a2 = x + 9;
    if (var_a2 >= 9)
        var_a2 -= 9;
    if (y < 0)
        var_v1 = y + 7;
    if (var_v1 >= 7)
        var_v1 -= 7;
    return var_a2 + var_v1 * 9;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A4F78);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5208);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A52A4);

void func_800A5348(void* arg0, void** arg1) {
    void* tmp;

    if (arg1 != NULL) {
        *arg1 = *(void**)arg0;
    } else {
        D_800E5810 = *(s32*)arg0;
    }
    WmStartLoadingMapFileBlock(arg0);

    tmp = D_800E580C;
    D_800E580C = arg0;
    *(void**)arg0 = tmp;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmLoadClosestMapFileBlock);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmStartLoadingMapFileBlock);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A57C8);

void WmMapLoadFinishedCallback(void) {
    if (D_800E5820 >= 0) {
        D_800E5814 = (D_8011650C * 5) - 1;
        func_800ADD4C(0);
    }
}

s16 WmGetNumberOfMapToLoad(void) {
    WorldListNode* p;
    s32 count;

    p = D_800E5768;
    count = 0;
    if (p != NULL) {
        do {
            p = p->next;
            count++;
        } while (p != NULL);
    }

    return count;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A59A0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmGetElementWithBlockIdAndSetItFirst);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5A94);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5AD8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5B88);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5C08);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5D00);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5E28);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5FB4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A60D8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6168);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A63FC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A64AC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A67A8);

void WmExtractLoopCoordsTopBottomParts(VECTOR* arg0, SVECTOR* arg1, s16* arg2, s16* arg3) {
    if (arg2 != NULL) {
        *arg2 = arg0->vx >> 0xD;
        if (*arg2 < 0) {
            *arg2 = *arg2 + 0x24;
        } else if (*arg2 >= 0x24) {
            *arg2 = *arg2 - 0x24;
        }
    }
    if (arg3 != NULL) {
        *arg3 = arg0->vz >> 0xD;
        if (*arg3 < 0) {
            *arg3 = *arg3 + 0x1C;
        } else if (*arg3 >= 0x1C) {
            *arg3 = *arg3 - 0x1C;
        }
    }
    if (arg1 != NULL) {
        arg1->vx = arg0->vx & 0x1FFF;
        arg1->vy = 0;
        arg1->vz = arg0->vz & 0x1FFF;
    }
}

void WmLoopCoordsAroundWorld(VECTOR* arg0) {

    if (arg0 != NULL) {
        if (arg0->vx > 0x47FFF) {
            arg0->vx -= 0x48000;
        } else if (arg0->vx < 0) {
            arg0->vx += 0x48000;
        }
        if (arg0->vz > 0x37FFF) {
            arg0->vz -= 0x38000;
        } else if (arg0->vz < 0) {
            arg0->vz += 0x38000;
        }
    }
}

void func_800A6994(VECTOR* arg0, s32 arg1) {
    if (func_800A1DB0() != 2) {
        if (arg1 < 0) {
            D_800E56F8 = 0;
        } else if (D_800E5A34 != 0) {
            D_800E56F8 |= arg1;
        }
        if (D_800E56F8 != 0) {
            if (arg0->vx - D_80109D44.vx > 0x24000) {
                D_80109D44.vx += 0x48000;
            } else if (arg0->vx - D_80109D44.vx < -0x24000) {
                D_80109D44.vx -= 0x48000;
            }
            if (arg0->vz - D_80109D44.vz > 0x1C000) {
                D_80109D44.vz += 0x38000;
            } else if (arg0->vz - D_80109D44.vz < -0x1C000) {
                D_80109D44.vz -= 0x38000;
            }
            D_80109D44.vx = ((D_80109D44.vx * 3) + arg0->vx) >> 2;
            D_80109D44.vz = ((D_80109D44.vz * 3) + arg0->vz) >> 2;
            WmLoopCoordsAroundWorld(&D_80109D44);
            D_800E56F8 = ABS(arg0->vx - D_80109D44.vx) + ABS(arg0->vz - D_80109D44.vz) >= 0xB;
            return;
        }
        D_80109D44 = *arg0;
    }
}

void func_800A6B8C(VECTOR* arg0) {
    if (arg0 != NULL) {
        *arg0 = D_80109D44;
    }
}

void func_800A6BCC(SVECTOR* arg0) {
    if (arg0 != NULL) {
        WmExtractLoopCoordsTopBottomParts(&D_80109D44, arg0, NULL, NULL);
    }
}

void func_800A6C00(s32 arg0) {
    if (func_800A1DB0() != 2)
        WmSetTranslationVectorInScreenSpace(arg0);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6C3C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6FC0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A71E8);

static s32 func_800A7E7C(void) { return D_80109D6C; }

static void func_800A7E8C(s32 arg0) { D_80109D6C = arg0; }

static void func_800A7E9C(void) {}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A7EA4);

void WmAbortMapLoadingWrapper(void) { WmAbortMapLoading(); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmAbortMapLoading);

static void func_800A8048(void) { D_800E5828 = 1; }

static s32 func_800A805C(void) { return D_800E5828; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A806C);

static void func_800A82DC(void) { D_80109D54 = 1; }

s32 func_800A82F0(void) { return D_80109D58; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A8300);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A835C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A86C4);

void func_800A886C(s32 arg0) {
    D_80109D64 = 2;
    D_80109D68 = arg0;
}

void WmSetWorldProgress(s32 arg0) { D_800E5824 = arg0; }

void func_800A8898(VECTOR* out) {
    u8* idx;
    SVECTOR* base;
    SVECTOR* a;
    SVECTOR* b;
    SVECTOR* c;

    idx = D_80109D60;
    base = *(SVECTOR**)(D_80109D5C + 0xC);
    /* the index must lead the addition; &base[i] emits the operands the
       other way round and costs the match */
    a = (SVECTOR*)((idx[0] * sizeof(SVECTOR)) + (s32)base);
    b = (SVECTOR*)((idx[1] * sizeof(SVECTOR)) + (s32)base);
    c = (SVECTOR*)((idx[2] * sizeof(SVECTOR)) + (s32)base);

    out->vx = a->vx + b->vx + c->vx;
    out->vy = a->vy + b->vy + c->vy;
    out->vz = a->vz + b->vz + c->vz;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmInitAllEntityStructs);

WorldActor* WmInsertInEntityStructList(void) {
    WorldActor* temp_s0;
    WorldActor* temp_v1;
    WorldActor* next;

    temp_s0 = D_8010AD34;
    if (temp_s0 == NULL)
        func_800A0B40(0x32);
    next = temp_s0->next;
    temp_v1 = D_8010AD38;
    D_8010AD3C = temp_s0;
    D_8010AD38 = temp_s0;
    D_8010AD34 = next;
    temp_s0->next = temp_v1;
    WmCleanEntityStruct(temp_s0);
    return temp_s0;
}

void func_800A8A88(void) {
    WorldActor* var_v0;

    for (var_v0 = D_8010AD38; var_v0 != NULL; var_v0 = var_v0->next)
        var_v0->unk5E = -1;
}

void WmCleanEntityStruct(WorldActor* arg0) {
    WorldActor** var_v1;

    arg0->unk5E = -1;

    for (var_v1 = &arg0->collide; var_v1 < (WorldActor**)(arg0 + 1); var_v1++)
        *var_v1 = NULL;
}

void WmUnlinkEntityAndInsertAsNew(WorldActor* arg0) {
    WorldActor* temp_v0;

    WmUnlinkEntityFromAll(arg0);
    temp_v0 = D_8010AD34;
    D_8010AD34 = arg0;
    arg0->next = temp_v0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmUnlinkEntityFromAll);

void WmInsertStructInEntityStructList(WorldActor* arg0) {
    WorldActor* temp_v0;

    if ((arg0 != NULL) && (arg0->next == NULL)) {
        temp_v0 = D_8010AD38;
        D_8010AD38 = arg0;
        arg0->next = temp_v0;
    }
}

s32 func_800A8CA4(void) {
    WorldActor* var_v1;
    for (var_v1 = D_8010AD38; var_v1 != NULL && var_v1->actorType < 0x20; var_v1 = var_v1->next)
        ;
    return var_v1 != NULL;
}

void WmLinkPcToActiveEntity(void) {
    WorldActor* temp_s0;

    temp_s0 = D_8010AD40;
    if ((temp_s0 != NULL) && (D_8010AD3C != NULL) && (D_8010AD3C->riding == NULL)) {
        WmUnlinkEntityFromAll(temp_s0);
        D_8010AD3C->riding = temp_s0;
        D_8010AD40 = D_8010AD3C;
    }
}

void func_800A8D58(void) {
    WorldActor* temp_s0;
    WorldActor* temp_v0;
    u16 temp_v1;

    if (D_8010AD40 == NULL)
        return;
    temp_s0 = D_8010AD40->riding;
    if (temp_s0 != NULL) {
        WmInsertStructInEntityStructList(temp_s0);
        temp_s0->collide = D_8010AD40;
        temp_s0->pos = D_8010AD40->pos;
        temp_s0->altPos = D_8010AD40->altPos;
        temp_s0->direction = D_8010AD40->direction;
        temp_s0->facing = D_8010AD40->facing;
        temp_s0->unk3C = D_8010AD40->unk3C;
        temp_v1 = D_8010AD40->unk3E;
        temp_s0->flags1 |= 2;
        temp_v0 = D_8010AD40;
        D_8010AD40 = temp_s0;
        temp_s0->unk3E = temp_v1;
        temp_v0->riding = NULL;
    }
}

void func_800A8E50(void) {
    WorldActor* temp_s0;
    WorldActor* temp_v0;
    u16 temp_v1;

    if (D_8010AD3C == NULL)
        return;
    temp_s0 = D_8010AD3C->riding;
    if (temp_s0 != NULL) {
        WmInsertStructInEntityStructList(temp_s0);
        temp_s0->collide = D_8010AD3C;
        temp_s0->pos = D_8010AD3C->pos;
        temp_s0->altPos = D_8010AD3C->altPos;
        temp_s0->direction = D_8010AD3C->direction;
        temp_s0->facing = D_8010AD3C->facing;
        temp_s0->unk3C = D_8010AD3C->unk3C;
        temp_v1 = D_8010AD3C->unk3E;
        temp_s0->flags1 |= 2;
        temp_v0 = D_8010AD3C;
        D_8010AD3C = temp_s0;
        temp_s0->unk3E = temp_v1;
        temp_v0->riding = NULL;
    }
}

s32 func_800A8F48(void) { return D_8010AD3C == NULL ? 0 : D_8010AD3C->riding != NULL; }

void WmUnlinkPcEntityFromAll(void) {
    if (D_8010AD40)
        WmUnlinkEntityAndInsertAsNew(D_8010AD40);
}

void WmUnlinkActiveEntityFromAll(void) {
    if (D_8010AD3C)
        WmUnlinkEntityAndInsertAsNew(D_8010AD3C);
}

void WmUnlinkPcLinkedEntityFromAll(void) {
    WorldActor* temp_a0;

    if (D_8010AD40 == NULL)
        return;
    temp_a0 = D_8010AD40->riding;
    if (temp_a0 != NULL) {
        WmUnlinkEntityAndInsertAsNew(temp_a0);
        D_8010AD40->riding = NULL;
    }
}

void func_800A9018(void) {
    WorldActor* temp_a0;

    if (D_8010AD3C == NULL)
        return;
    temp_a0 = D_8010AD3C->riding;
    if (temp_a0 != NULL) {
        WmUnlinkEntityAndInsertAsNew(temp_a0);
        D_8010AD3C->riding = NULL;
    }
}

void func_800A9064(s16 x, s16 z) {
    WorldActor* a;
    WorldStoredTriangle* tri;

    for (a = D_8010AD38; a != NULL; a = a->next)
        for (tri = &a->storedTris[0]; tri < &a->storedTris[6]; tri++)
            if ((tri->x == x) && (tri->z == z))
                tri->tri = NULL;
}

void WmSetPcEntityAsActiveEntity(void) {
    if (D_8010AD40 != NULL)
        D_8010AD3C = D_8010AD40;
}

void WmSetActiveEntityAsPcEntity(void) {
    if (D_8010AD3C != NULL)
        D_8010AD40 = D_8010AD3C;
}

WorldStoredTriangle* func_800A9134(void) { return D_8010AD40 != NULL ? D_8010AD40->storedTris : NULL; }

s32 WmGetModelIdFromActiveEntity(void) { return D_8010AD3C != NULL ? D_8010AD3C->actorType : NULL; }

s32 WmGetModelIdFromPcEntity(void) { return D_8010AD40 != NULL ? D_8010AD40->actorType : NULL; }

WorldActor* func_800A9194(void) { return D_8010AD3C; }

s32 WmIsPcEntityModelInMask(s32 arg0) {
    return D_8010AD40 != NULL && D_8010AD40->actorType < 0x20U ? (arg0 >> D_8010AD40->actorType) & 1 : 0;
}

s32 func_800A91E0(s32 arg0) {
    return D_8010AD3C != NULL && D_8010AD3C->actorType < 0x20U ? (arg0 >> D_8010AD3C->actorType) & 1 : 0;
}

s32 func_800A921C(s32 arg0, u8 arg1) { return arg1 >= 0x20 ? 0 : (arg0 >> arg1) & 1; }

s32 func_800A9240(void) {
    s32 out;
    u8 actorType;

    if (D_8010AD40 != NULL) {
        actorType = D_8010AD40->actorType;
        out = 0;
        if (actorType == 4 || actorType == 0x13 || D_8010AD40->actorType - 0x29 < 2U)
            out = 1;
    } else
        out = 0;
    return out;
}

s32 func_800A929C(void) {
    s32 out;
    u8 actorType;

    if (D_8010AD3C != NULL) {
        actorType = D_8010AD3C->actorType;
        out = 0;
        if (actorType == 4 || actorType == 0x13 || D_8010AD3C->actorType - 0x29 < 2U)
            out = 1;
    } else
        out = 0;
    return out;
}

s32 func_800A92F8(s32 arg0) { return (arg0 & 0xFF) == 4 || (arg0 & 0xFF) == 0x13 || ((arg0 - 0x29) & 0xFF) < 2U; }

static const s32 D_800A01D8[] = {0, 0xF000};
static const s32 D_800A01E0[] = {0, 0};
static const s32 D_800A01E8[] = {0, 0, 0, 0};

// TODO: this -> 800b624c, 800b58f8, 800ada64
void WmInitActiveEntityStruct(s32 arg0) {
    RECT rect;

    if (D_8010AD3C != NULL) {
        D_8010AD3C->actorType = (u8)arg0;
        switch (arg0) {
        case 5:
        case 13:
        case 28:
            break;
        case 3:
            if (func_800B716C() == 0)
                D_8010AD3C->riding = &D_80109E54;

            rect.x = 0x18;
            rect.y = 0x48;
            rect.w = 0xE;
            rect.h = 0x1F;
            break;
        case 10:
            rect.x = 0x90;
            rect.w = 0xF;
            rect.h = 0xF;
            rect.y = 0;
            D_8010AD3C->unk58 = 0x80;
            break;
        case 11:
            rect.y = 0x38;
            rect.w = 0x17;
            rect.h = 0x2F;
            rect.x = 0;
            D_8010AD3C->unk58 = 0x20;
            break;
        case 4:
            func_800B624C(4, 0);
            /* fallthrough */
        default:
            rect.x = 0x18;
            rect.y = 0x38;
            rect.w = 0xF;
            rect.h = 0xF;
            D_8010AD3C->unk58 = 0x20;
        }
        func_800B58F8(D_8010AD3C->unk90, &rect);
        WmRestoreEntityPosAndDirFromSavemap(D_8010AD3C);
    }
}

void WmSetActiveEntityDirectionAndRot(s16 arg0) {
    if (D_8010AD3C != NULL) {
        D_8010AD3C->direction = arg0;
        D_8010AD3C->facing = (s16)arg0;
        D_8010AD3C->unk3E = 0;
    }
}

void func_800A94A8(u16 arg0) {
    if (D_8010AD40 != NULL) {
        D_8010AD40->direction = arg0;
        D_8010AD40->facing = (s16)arg0;
        D_8010AD40->unk3E = 0;
    }
}

void func_800A94D0(u16 arg0) {
    if (D_8010AD3C != NULL) {
        D_8010AD3C->direction = arg0;
        D_8010AD3C->facing = (s16)arg0;
    }
}

void func_800A94F4(u16 arg0) {
    if (D_8010AD3C != NULL) {
        D_8010AD3C->direction = arg0;
        D_8010AD3C->unk3C = arg0;
        D_8010AD3C->facing = (s16)arg0;
        D_8010AD3C->unk3E = 0;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9520);

void func_800A9678(s16 arg0) { func_800A9520(D_8010AD3C, arg0); }

void func_800A96A4(s16 arg0) { func_800A9520(D_8010AD40, arg0); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A96D0);

s16 func_800A97A8(void) { return D_8010AD3C == NULL ? 0 : D_8010AD3C->unk3C + D_8010AD3C->unk3E; }

s16 WmGetPcEntityTotalRot(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->unk3C + D_8010AD40->unk3E; }

void func_800A9820(s32 arg0) {
    if (D_8010AD3C != NULL)
        D_8010AD3C->pos.vy += arg0;
}

s32 func_800A984C(void) { return D_8010AD3C == NULL ? 0 : D_8010AD3C->flags1 & 1; }

s32 func_800A9878(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->flags1 & 1; }

void func_800A98A4(s32 arg0) {
    u8 var_v0;
    WorldActor* var_v1;

    var_v1 = D_8010AD40;

    if (var_v1 != NULL) {
        if (arg0 != 0)
            var_v0 = var_v1->flags1 | 0x80;
        else
            var_v0 = var_v1->flags1 & 0x7F;
        var_v1->flags1 = var_v0;
    }
}

s32 WmIsPcEntityPosNeedRecalculation(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->flags1 >> 7; }

s16 WmGetPcEntityOriginalY(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->unk42; }

s32 WmSetActiveEntityWithModelId(s32 arg0) {
    WorldActor* var_v1;

    for (var_v1 = D_8010AD38; var_v1 != NULL && var_v1->actorType != arg0; var_v1 = var_v1->next)
        ;
    if (var_v1 != NULL)
        D_8010AD3C = var_v1;
    return var_v1 != NULL;
}

void func_800A9988(void) {
    if (D_8010AD40 != NULL && D_8010AD40->collide != NULL)
        D_8010AD3C = D_8010AD40->collide;
}

s32 func_800A99BC(void) {
    return D_8010AD40 != NULL && D_8010AD3C != NULL && D_8010AD40 != D_8010AD3C && !(D_8010AD3C->flags1 & 0x10);
}

void func_800A9A04(s8 arg0) {
    if (D_8010AD40)
        D_8010AD40->actorType = arg0;
}

void WmSetPcEntityTerrainData(s16 arg0) {
    if (D_8010AD40)
        D_8010AD40->walkmesh = arg0;
}

s32 WmGetPcEntityTerrainId(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->walkmesh & 0x1F; }

s32 func_800A9A70(void) { return D_8010AD40 == NULL ? 0 : (D_8010AD40->walkmesh >> 9) & 0x1F; }

u32 func_800A9AA4(void) { return D_8010AD40 == NULL ? 0 : (u16)D_8010AD40->walkmesh >> 0xF; }

s32 func_800A9AD0(void) { return D_8010AD40 == NULL ? 0 : (D_8010AD40->walkmesh >> 5) & 7; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9B04);

void func_800A9C64(WorldActor* arg0, VECTOR* arg1) {
    u8 var_v0;

    if (arg1 != NULL && arg0 != NULL) {
        WmLoopCoordsAroundWorld(arg1);
        if (arg0->flags1 & 0x80) {
            arg0->pos.vx = arg1->vx;
            arg0->pos.vz = arg1->vz;
        } else {
            arg0->pos = *arg1;
            arg0->pos.vy += func_800A9B04(arg0->walkmesh, arg0->actorType);
        }
        arg0->unk42 = arg1->vy;
        arg0->flags1 =
            arg0->pos.vx != arg0->altPos.vx || arg0->pos.vz != arg0->altPos.vz ? arg0->flags1 | 1 : arg0->flags1 & 0xFE;
    }
}

void func_800A9D5C(VECTOR* arg0) { func_800A9C64(D_8010AD3C, arg0); }

void func_800A9D88(VECTOR* arg0) { func_800A9C64(D_8010AD40, arg0); }

void func_800A9DB4(VECTOR* arg0) {
    func_800A9C64(D_8010AD40, arg0);
    if (D_8010AD40 != NULL)
        D_8010AD40->altPos = D_8010AD40->pos;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9E14);

static void func_800AA02C(s32 arg0) {
    if (D_8010AD40)
        D_8010AD40->pos.vy = arg0;
}

static void func_800AA04C(s32 arg0) {
    if (D_8010AD40 != NULL) {
        D_8010AD40->pos.vy = arg0;
        D_8010AD40->flags1 = D_8010AD40->flags1 | 4;
    }
}

static void func_800AA078(s16 arg0) {
    if (D_8010AD40)
        D_8010AD40->unk4E = arg0;
}

void WmGetPosFromActiveEntity(VECTOR* arg0) {
    if (arg0 != NULL && D_8010AD3C != NULL)
        *arg0 = D_8010AD3C->pos;
}

void WmGetPosFromPcEntity(VECTOR* arg0) {
    if (arg0 != NULL && D_8010AD40 != NULL)
        *arg0 = D_8010AD40->pos;
}

void WmGetPos2FromActiveEntity(VECTOR* arg0) {
    if (arg0 != NULL && D_8010AD3C != NULL)
        *arg0 = D_8010AD3C->altPos;
}

void WmGetPos2FromPcEntity(VECTOR* arg0) {
    if (arg0 != NULL && D_8010AD40 != NULL)
        *arg0 = D_8010AD40->altPos;
}

void func_800AA1B8(void) {
    if (D_8010AD3C != NULL) {
        if (D_8010AD3C->flags1 & 4) {
            D_8010AD3C->pos.vx = D_8010AD3C->altPos.vx;
            D_8010AD3C->pos.vz = D_8010AD3C->altPos.vz;
        } else {
            D_8010AD3C->pos.vx = D_8010AD3C->altPos.vx;
            D_8010AD3C->pos.vy = D_8010AD3C->altPos.vy;
            D_8010AD3C->pos.vz = D_8010AD3C->altPos.vz;
            D_8010AD3C->pos.pad = D_8010AD3C->altPos.pad;
        }
        D_8010AD3C->flags1 = D_8010AD3C->flags1 & 0xFE;
    }
}

void func_800AA238(void) {
    if (D_8010AD40 != NULL) {
        if (D_8010AD40->flags1 & 4) {
            D_8010AD40->pos.vx = D_8010AD40->altPos.vx;
            D_8010AD40->pos.vz = D_8010AD40->altPos.vz;
        } else {
            D_8010AD40->pos.vx = D_8010AD40->altPos.vx;
            D_8010AD40->pos.vy = D_8010AD40->altPos.vy;
            D_8010AD40->pos.vz = D_8010AD40->altPos.vz;
            D_8010AD40->pos.pad = D_8010AD40->altPos.pad;
        }
        D_8010AD40->flags1 &= 0xFE;
    }
}

void func_800AA2B8(void) {
    if (D_8010AD40 != NULL)
        D_8010AD40->flags1 &= 0xFD;
}

static void func_800AA2E4(s8 arg0) {
    if (D_8010AD3C)
        D_8010AD3C->animId = arg0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA304);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA580);

s32 func_800AA640(void) {
    s32 temp_v0;

    temp_v0 = func_800AA580(D_8010AD3C);
    if (temp_v0 != 0)
        func_800AA1B8();
    return temp_v0;
}

WorldActor* func_800AA684(void) { return D_8010AD3C != NULL ? D_8010AD3C->collide : NULL; }

void func_800AA6A4(void) {
    if (D_8010AD40 != NULL)
        D_8010AD40->flags1 |= 2;
}

void func_800AA6D0(WorldChunkHeader* arg0) {
    SVECTOR sp20;
    s16 sp28;
    s16 sp2A;
    WorldActor* var_s0;

    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next) {
        if (var_s0 != D_8010AD40) {
            WmExtractLoopCoordsTopBottomParts(&var_s0->pos, &sp20, &sp28, &sp2A);
            if ((sp28 == arg0->x) && (sp2A == arg0->z)) {
                func_800A19FC(arg0, &sp20, var_s0->storedTris, &var_s0->unk42, 0, &var_s0->walkmesh, var_s0->actorType);
                if (!(var_s0->flags1 & 0x80)) {
                    var_s0->pos.vy = var_s0->unk42 + func_800A9B04(var_s0->walkmesh, var_s0->actorType);
                }
                var_s0->flags1 |= 2;
            }
        }
    }

    func_800B0D98(arg0);
    func_800B1C80(arg0);
}

void WmMoveActiveEntity(s32 arg0, s32 arg1) {
    if ((D_8010AD3C != NULL) && ((arg0 != 0) || (arg1 != 0))) {
        D_8010AD3C->pos.vx += arg0;
        D_8010AD3C->pos.vz += arg1;
        if (D_8010AD3C->pos.vx < 0)
            D_8010AD3C->pos.vx = D_8010AD3C->pos.vx + 0x48000;
        if (D_8010AD3C->pos.vx > 0x47FFF)
            D_8010AD3C->pos.vx = D_8010AD3C->pos.vx - 0x48000;
        if (D_8010AD3C->pos.vz < 0)
            D_8010AD3C->pos.vz = D_8010AD3C->pos.vz + 0x38000;
        if (D_8010AD3C->pos.vz > 0x37FFF)
            D_8010AD3C->pos.vz = D_8010AD3C->pos.vz - 0x38000;
        D_8010AD3C->flags1 |= 1;
    }
}

void func_800AA8D8(s16 arg0, s16 arg1, s16 arg2) {
    D_8010AD44 = arg0;
    D_8010AD48 = arg1;
    D_8010AD4C = arg2;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA8F8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmGetBuggyMoveAnimationId);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AAB18);

void func_800AB36C(SVECTOR* arg0) {
    if (arg0 != NULL) {
        arg0->vy = 0;
        arg0->vx = (u16)D_8010AD54;
        arg0->vz = (u16)D_8010AD58;
    }
}

void func_800AB398(WorldActor* arg0) {
    if (!arg0) {
        return;
    }
    if (!D_8010AD40) {
        return;
    }
    if (!(arg0->flags1 & 2)) {
        return;
    }
    if ((func_800A921C(7, arg0->actorType) != 0 && (arg0->animId < 2 || arg0->animId > 5) ||
         func_800A92F8(arg0->actorType) != 0) &&
        (arg0 != D_8010AD40 || D_8010AD5C == 0) && ((0x311B6F05 >> (arg0->walkmesh & 0x1F)) & 1)) {
        func_800B5C7C(arg0);
    } else if (arg0->actorType == 3 || arg0->actorType == 0xB) {
        func_800B5C7C(arg0);
    }
}

void func_800AB48C(WorldActor* arg0) {
    if ((arg0->collide != NULL) && ((arg0->flags1 & 1) || (arg0->collide->flags1 & 1)))
        arg0->collide = NULL;
    arg0->altPos = arg0->pos;
}

void WmPrepareEntities(void) {
    WorldActor* var_s0;

    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        func_800AB48C(var_s0);
    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        var_s0->flags1 &= 0xF8;
}

void func_800AB570(void) {
    WorldActor* var_s0;

    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        func_800AAB18(var_s0);
    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        func_800AB398(var_s0);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmScriptInitVariables);

// pushes execution of given script to player's execution stack
void WmScriptRunFunction(s32 arg0, s32 arg1) {
    WorldScriptBSTEntry* var_a0;
    WorldScriptFrame* temp_a1;
    s32 var_s2;
    s32 var_v1;

    var_s2 = 0;
    if (D_8010AD3C != NULL && (D_8010AD3C->scriptPriority < arg1 || (D_8010AD3C->scriptPriority == arg1 && arg1 == 3) ||
                               D_8010AD3C->scriptIdx == 0)) {
        if (D_8010AD3C->scriptCallDepth >= 4)
            func_800A0B40(0x41);
        if (D_8010AD3C->scriptIdx != 0) {
            temp_a1 = &D_8010AD3C->scriptStack[D_8010AD3C->scriptCallDepth++];
            temp_a1->scriptIdx = D_8010AD3C->scriptIdx;
            temp_a1->waitFrames = D_8010AD3C->waitFrames;
            var_s2 = 1;
            temp_a1->scriptPriority = D_8010AD3C->scriptPriority;
        }

        for (var_v1 = 0x40, var_a0 = &D_8010AD68->bst[0x80]; var_v1 != 0; var_v1 >>= 1) {
            if (var_a0->unk0 == arg0)
                break;
            else if (var_a0->unk0 < arg0)
                var_a0 += var_v1;
            else
                var_a0 -= var_v1;
        }

        if (var_a0->unk0 == arg0) {
            D_8010AD3C->scriptIdx = var_a0->unk2;
            D_8010AD3C->scriptPriority = arg1;
        } else {
            D_8010AD3C->scriptIdx = 0;
            D_8010AD3C->scriptPriority = 0;
        }
        if (var_s2 != 0 && D_8010AD3C->scriptIdx == 0) {
            temp_a1 = &D_8010AD3C->scriptStack[--D_8010AD3C->scriptCallDepth];
            D_8010AD3C->scriptIdx = temp_a1->scriptIdx;
            D_8010AD3C->waitFrames = temp_a1->waitFrames;
            D_8010AD3C->scriptPriority = temp_a1->scriptPriority;
        }
    }
}

void WmScriptDisableForPcEntity(s32 arg0) {
    if (D_8010AD40 != NULL) {
        if (arg0 != 0)
            D_8010AD40->flags1 = D_8010AD40->flags1 | 0x10;
        else
            D_8010AD40->flags1 = D_8010AD40->flags1 & 0xEF;
    }
}

void func_800AB92C(s32 arg0) {
    s32 var_a1;
    s32 var_a0;

    if (D_8010AD3C != NULL) {
        var_a1 = arg0 - 2;
        if (!(D_8010AD3C->flags1 & 0x10)) {
            var_a0 = arg0 & 0xFF;
            if (var_a1 < 0)
                var_a1 = 0;
            if (var_a1 >= 4)
                var_a1 = 3;
            WmScriptRunFunction(var_a0, var_a1);
        }
    }
}

void func_800AB988(s32 arg0, s32 arg1) {
    s32 var_a1;
    s32 tmp0;
    s32 tmp1;
    s32 tmp2;

    WmSetActiveEntityWithModelId(arg0);
    if (D_8010AD3C != NULL) {
        var_a1 = arg1 - 2;
        if (!(D_8010AD3C->flags1 & 0x10)) {
            tmp0 = (arg0 << 8) & 0x3F00;
            tmp1 = (arg1 & 0xFF) | 0x4000;
            tmp2 = tmp0 | tmp1;
            if (var_a1 < 0)
                var_a1 = 0;
            if (var_a1 >= 4)
                var_a1 = 3;
            WmScriptRunFunction(tmp2, var_a1);
        }
    }
}

void func_800ABA18(s32 arg0) {
    s32 var_a1;
    s32 var_a0;

    D_8010AD3C = D_80109D74;
    if (arg0 == 1)
        D_8010ADEC = arg0;
    var_a1 = arg0 - 2;
    var_a0 = arg0 & 0xFF;
    if (var_a1 < 0)
        var_a1 = 0;
    if (var_a1 >= 4)
        var_a1 = 3;
    WmScriptRunFunction(var_a0, var_a1);
}

void func_800ABA78(s16 arg0, s16 arg1) {
    s16 sp10;
    s16 sp12;
    s32 tmp0;
    s32 tmp1;

    D_8010AD3C = D_80109D74;
    WmExtractLoopCoordsTopBottomParts(&D_8010AD40->pos, NULL, &sp10, &sp12);
    tmp0 = ((((sp12 * 0x24) + sp10) * 0x10) & 0x3FF0);
    tmp1 = (((arg0 + (arg1 * 5)) & 0xF) | 0x8000);
    WmScriptRunFunction(tmp0 | tmp1, 3);
}

static const s32 D_800A0260[] = {0, 0};

s32 WmScriptPopStack(void) {
    s32 var_s0;
    u8* temp_s0;

    var_s0 = 0;
    if (&D_8010AD70[0] >= D_8010AD90--) {
        func_800A0B40(0x3E);
    }

    if ((D_8010AD90->unk4 & 3) < 3) {
        switch (D_8010AD90->unk4 & ~3) {
        case 0x110:
            var_s0 = D_8010AD90->unk0;
            break;
        case 0x114:
            var_s0 = D_8010AD94[D_8010AD90->unk4 & 3][D_8010AD90->unk0 >> 3] >> (D_8010AD90->unk0 & 7) & 1;
            break;
        case 0x118:
            var_s0 = D_8010AD94[D_8010AD90->unk4 & 3][D_8010AD90->unk0];
            break;
        case 0x11C:
            temp_s0 = &D_8010AD94[D_8010AD90->unk4 & 3][D_8010AD90->unk0];
            if ((s32)temp_s0 & 1) {
                func_800A0B40(0x3F);
            }
            var_s0 = *(s16*)temp_s0;
        }
    } else {
        switch (D_8010AD90->unk0) {
        case 0: // Active actor X-chunk
            var_s0 = D_8010AD3C->pos.vx >> 0xD;
            break;
        case 1: // Active actor Y-chunk
            var_s0 = D_8010AD3C->pos.vz >> 0xD;
            break;
        case 2: // Active actor X coord within chunk
            var_s0 = D_8010AD3C->pos.vx & 0x1FFF;
            break;
        case 3: // Active actor Y coord within chunk
            var_s0 = D_8010AD3C->pos.vz & 0x1FFF;
            break;
        case 4: // Active actor direction (0-255)
            var_s0 = (D_8010AD3C->direction >> 4) & 0xFF;
            break;
        case 5: // Scenario parameter when entering WM from a field (see opcode
                // 0x318)
            var_s0 = func_800B785C();
            break;
        case 6: // Field ID of "wm" field that jumped to the WM
            var_s0 = func_800B786C();
            break;
        case 7: // Map options (eg. minimap & camera settings)
            var_s0 = func_800A4080();
            break;
        case 8: // Model ID of the player actor
            var_s0 = WmGetModelIdFromPcEntity();
            break;
        case 15: // Active Actor Model ID
            var_s0 = D_8010AD3C->actorType;
            break;
        case 9: // Current wild chocobo rating
            var_s0 = func_800B7B2C();
            break;
        case 10: // Player is currently riding a chocobo?
            var_s0 = func_800B7B3C();
            break;
        case 11: // Battle result - whether player escaped last battle (1: place
                 // party on the stables side, 0: place party on the cave side)
            var_s0 = func_800B0800();
            break;
        case 12: // Prompt window result
            var_s0 = func_800B86C4();
            break;
        case 13: // Script index of current mesh triangle of active actor
            var_s0 = ((D_8010AD3C->walkmesh) >> 5) & 7;
            break;
        case 14: // Player party member model ID (0=Cloud, 1=Tifa, 2=Cid)
            var_s0 = func_800B79B8();
            break;
        case 16: // Random byte
            var_s0 = func_800ADFC0();
            break;
        case 17:
            var_s0 = D_8010AE24;
            break;
        case 18:
            var_s0 = D_8010AE28;
            break;
        case 19:
            var_s0 = D_8010AE2C;
            break;
        case 20:
            var_s0 = D_8010AE30;
        }
    }
    return var_s0;
}

// executed during script opcode 0xE0 (write bank)
void WmScriptWriteBank(s32 arg0) {
    Unk8010AD70* temp_v1;
    s32 temp_a0;
    s32 var_v0;
    u8* temp_s0;

    if (&D_8010AD70[0] >= D_8010AD90--) {
        func_800A0B40(0x3E);
    }

    temp_s0 = D_8010AD94[D_8010AD90->unk4 & 3];
    switch (D_8010AD90->unk4 & ~3) {
    case 0x114:
        temp_s0 = &temp_s0[D_8010AD90->unk0 >> 3];
        temp_a0 = 1 << (D_8010AD90->unk0 & 7);
        *temp_s0 &= ~temp_a0;
        var_v0 = *temp_s0;
        if (arg0 != 0) {
            var_v0 |= temp_a0;
        }
        *temp_s0 = var_v0;
        break;
    case 0x118:
        temp_s0[D_8010AD90->unk0] = (u8)arg0;
        break;
    case 0x11C:
        temp_s0 = &temp_s0[D_8010AD90->unk0];
        if ((s32)temp_s0 & 1) {
            func_800A0B40(0x3F);
        }
        *(u16*)temp_s0 = arg0;
        break;
    default:
        func_800A0B40(0x40);
        break;
    }
    D_8010AD90->unk0 = arg0;
    D_8010AD90->unk4 = 0x110;
    D_8010AD90++;
}

/*
 * script opcodes are documented at
 * http://wiki.ffrtt.ru/index.php/FF7/WorldMap_Module/Script/Opcodes
 */

// script opcode 0x0XX, arithmetic and writing to memory
void WmScriptOpcode000Handle(u16 arg0) {
    VECTOR sp10;
    Unk8010AD70* temp_s0_17;
    WorldActor* temp_s0_16;
    s32 temp_s0;
    s32 temp_v0;
    s32 var_v0;

    switch (arg0) {
    case 0x15: // pop A push -A
        D_8010AD90->unk0 = -WmScriptPopStack();
        break;
    case 0x17: // pop A, push ~A
        D_8010AD90->unk0 = WmScriptPopStack() == 0;
        break;
    case 0x30: // pop A, B, push B * A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() * temp_s0;
        break;
    case 0x40: // pop A, B, push B + A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() + temp_s0;
        break;
    case 0x41: // pop A, B, push B - A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() - temp_s0;
        break;
    case 0x60: // pop A, B, push B < A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() < temp_s0;
        break;
    case 0x61: // pop A, B, push B > A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = temp_s0 < WmScriptPopStack();
        break;
    case 0x62: // pop A, B, push B <= A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = temp_s0 >= WmScriptPopStack();
        break;
    case 0x63: // pop A, B, push B >= A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() >= temp_s0;
        break;
    case 0x70: // pop A, B, push B == A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() == temp_s0;
        break;
    case 0x71: // pop A, B, push B != A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() != temp_s0;
        break;
    case 0x50: // pop A, B, push B << A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() << temp_s0;
        break;
    case 0x51: // pop A, B, push B >> A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() >> temp_s0;
        break;
    case 0x80: // pop A, B, push B & A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() & temp_s0;
        break;
    case 0xA0: // pop A, B, push B | A
        temp_s0 = WmScriptPopStack();
        D_8010AD90->unk0 = WmScriptPopStack() | temp_s0;
        break;
    case 0xB0: // pop A, B, push B && A
        temp_s0 = WmScriptPopStack();
        var_v0 = WmScriptPopStack();
        D_8010AD90->unk0 = var_v0 && temp_s0;
        break;
    case 0xC0: // pop A, B, push B || A
        temp_s0 = WmScriptPopStack();
        var_v0 = WmScriptPopStack();
        D_8010AD90->unk0 = var_v0 || temp_s0;
        break;
    case 0xE0: // pop A, write bank
        WmScriptWriteBank(WmScriptPopStack());
        break;
    case 0x18: // "push distance from active entity to point"
        func_800AF1A8(WmScriptPopStack(), -1);
        WmGetPosFromActiveEntity(&sp10);
        D_8010AD90->unk0 = func_800AF96C(&sp10) >> 5;
        break;
    case 0x19: // "push distance from active entity to entity by model id"
        temp_s0_16 = D_8010AD3C;
        D_8010AD90->unk0 = 0;
        if (WmSetActiveEntityWithModelId(WmScriptPopStack()) != 0) {
            D_8010AD90->unk0 = func_800AE024(&D_8010AD3C->pos, &temp_s0_16->pos) >> 4;
        }
        D_8010AD3C = temp_s0_16;
        break;
    case 0x1A: // unused and undocumented
        temp_v0 = WmScriptPopStack();
        temp_s0_17 = D_8010AD90;
        sp10 = D_8010AD3C->pos;
        sp10.vy = 0;
        if (temp_v0 >= 0 && temp_v0 < 3) {
            // not going to bother with the struct at 8010ADF4 yet, as it may be
            // unused
            temp_s0_17->unk0 = func_800AE024(&sp10, (VECTOR*)((temp_v0 * 0x4) + &D_8010ADF4)) >> 4;
        } else {
            temp_s0_17->unk0 = 0;
        }
        break;
    case 0x1B: // "push direction from active entity to point"
        func_800AF1A8(WmScriptPopStack(), -1);
        WmGetPosFromActiveEntity(&sp10);
        D_8010AD90->unk0 = (func_800AF9A0(&sp10) << 0x10) >> 0x14;
        break;
    }
    (D_8010AD90)->unk4 = 0x110;
    D_8010AD90++;
}

// script opcode 0x1XX, push value to stack
void WmScriptOpcode100Handle(u16 arg0) {
    if (arg0 == 0x100) { // reset stack. unused?
        D_8010AD90 = D_8010AD70;
        return;
    }
    if (D_8010AD90 == NULL || D_8010ADE4 == NULL) {
        func_800A0B40(0x3D);
    }
    D_8010AD90->unk4 = arg0;
    (D_8010AD90++)->unk0 = (D_8010AD6C[D_8010ADE4->scriptIdx++]);
}

// script opcode 0x2XX, flow control
s32 WmScriptOpcode200Handle(u16 arg0) {
    WorldActor* var_s0;
    WorldScriptFrame* temp_v0_4;
    s32 var_v0;
    s16 temp_s0;

    var_v0 = 0;
    switch (arg0) {
    case 0x200: // jump
        temp_s0 = D_8010AD6C[D_8010ADE4->scriptIdx++];
        D_8010ADE4->scriptIdx = temp_s0;
        break;
    case 0x201: // pop A, jump if ~A
        temp_s0 = D_8010AD6C[D_8010ADE4->scriptIdx++];
        if (WmScriptPopStack() == 0) {
            D_8010ADE4->scriptIdx = temp_s0;
        }
        break;
    case 0x203: // return
        if (D_8010ADE4->scriptCallDepth != 0) {
            temp_v0_4 = &D_8010ADE4->scriptStack[--D_8010ADE4->scriptCallDepth];
            D_8010ADE4->scriptIdx = temp_v0_4->scriptIdx;
            D_8010ADE4->waitFrames = temp_v0_4->waitFrames;
            D_8010ADE4->scriptPriority = temp_v0_4->scriptPriority;
            break;
        }
        D_8010ADE4->scriptIdx = D_8010ADE4->scriptPriority = 0;
        if (D_8010ADEC != 0) {
            if (D_8010ADE4 == D_80109D74) {
                for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next) {
                    func_800AB988(var_s0->actorType, 1);
                }
            }
        } else {
            if (D_8010ADE4 == D_80109D74) {
                func_800ABA18(2);
            } else {
                func_800AB988(D_8010ADE4->actorType, 2);
            }
        }
        var_v0 = 1;
        break;
    default: // run function number (opcode - 0x204)
        D_8010ADE4->scriptCallModel = WmScriptPopStack();
        if (D_8010ADE4->scriptCallModel < 0x40) {
            func_800AB988(D_8010ADE4->scriptCallModel, arg0 - 0x204);
        } else {
            D_8010ADE4->scriptCallModel = D_8010AD3C->actorType;
            func_800AB92C(arg0 - 0x204);
        }
        var_v0 = 1;
    }
    return var_v0;
}

// script opcode 0x3XX, system ops / misc
INCLUDE_ASM("asm/us/world/nonmatchings/world", WmScriptOpcode300Handle);

void WmScriptRunOne(WorldActor* arg0) {
    s32 var_a1;
    s32 var_s0;
    u16 temp_a0;
    u16 temp_v0;

    D_8010ADE8 = 0;
    if (D_8010ADE4->scriptIdx != 0) {
        var_s0 = 0;
        do {
            temp_a0 = D_8010AD6C[D_8010ADE4->scriptIdx++];
            if (temp_a0 < 0x100) {
                WmScriptOpcode000Handle(temp_a0);
            } else if (temp_a0 < 0x200) {
                WmScriptOpcode100Handle(temp_a0);
            } else if (temp_a0 < 0x300) {
                var_s0 = WmScriptOpcode200Handle(temp_a0);
            } else {
                var_s0 = WmScriptOpcode300Handle(temp_a0);
            }
        } while (var_s0 == 0);
    }
    func_800A9678(arg0->direction);
    if (arg0->collide != NULL && D_8010ADEC == 0 && func_800A21A4() != 0)
        func_800AB988(arg0->collide->actorType, (InputReadPads() & PADRright) ? 4 : 3);
}

void WmScriptRunAll(void) {
    WorldActor* var_s0;

    WmScriptRunOne(D_8010AD3C = D_8010ADE4 = D_80109D74);
    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        WmScriptRunOne(D_8010ADE4 = D_8010AD3C = var_s0);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AD804);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmScriptIsAnyScriptRuns);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AD970);

void func_800ADA08(void) {
    WorldActor* var_s0;

    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        if (!(var_s0->flags1 & 8))
            func_800AD970(var_s0);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmRestoreEntityPosAndDirFromSavemap);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADB30);

void func_800ADC3C(VECTOR* arg0) { D_8010AE34 = *arg0; }

void func_800ADC70(void) { D_8010AE54 = 0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADC80);

void func_800ADD4C(s16 arg0) {
    u32 var_v1;

    if (((-1 << (arg0 + 1)) & D_8010AE54) || !((D_8010AE54 >> arg0) & 1))
        func_800A0B40(0x50);

    D_8010AE54 = ~(1 << arg0) & D_8010AE54;
    if (arg0 != 0) {
        for (var_v1 = 1 << (arg0 - 1); var_v1 && !(var_v1 & D_8010AE54); var_v1 >>= 1)
            ;
        if (var_v1 == 1)
            func_800A8048();
    }
}

// Shuffle World Map RNG Buffer
void func_800ADE30(void) {
    s32 i;
    for (i = 0; i < 0x20; i++)
        D_8010AE5C[i] ^= D_8010AE5C[0x1E9 + i];
    for (i = 0x20; i < 0x209; i++)
        D_8010AE5C[i] ^= D_8010AE5C[i - 0x20];
}

// seed RNG
void func_800ADEA8(s32 arg0) {
    s32 var_a1;
    s32 var_a2;
    u32 var_a3;
    u32 arr[521];

    var_a3 = 0;
    for (var_a2 = 0; var_a2 < 0x11; var_a2++) {
        for (var_a1 = 0x1F; var_a1 >= 0; var_a1--) {
            arg0 = (arg0 * 0x5D588B65) + 1;
            var_a3 = (var_a3 >> 1) | (arg0 & 0x80000000);
        }
        arr[var_a2] = var_a3;
    }
    arr[0x10] = (arr[0x10] << 0x17) ^ (arr[0] >> 9) ^ arr[0xF];
    for (var_a2 = 0x11; var_a2 <= 0x208; var_a2++)
        arr[var_a2] = (arr[var_a2 - 0x11] << 0x17) ^ (arr[var_a2 - 0x10] >> 0x9) ^ (arr[var_a2 - 0x1]);
    for (var_a2 = 0; var_a2 <= 0x208; var_a2++)
        D_8010AE5C[var_a2] = arr[var_a2];
    func_800ADE30();
    func_800ADE30();
    func_800ADE30();
    D_8010AE58 = 0x208;
}

// World Map RNG rand(), returns u8
s32 func_800ADFC0(void) {
    D_8010AE58++;
    if (D_8010AE58 > 0x208) {
        func_800ADE30();
        D_8010AE58 = 0;
    }
    return D_8010AE5C[D_8010AE58];
}

s32 func_800AE024(VECTOR* arg0, VECTOR* arg1) {
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_a2;
    s32 var_a3;
    s32 var_v1;

    temp_v1 = arg0->vx;
    temp_v0 = arg1->vx;
    var_a2 = temp_v1 - temp_v0;
    if (var_a2 <= 0)
        var_a2 = temp_v0 - temp_v1;

    if (var_a2 >= 0x24000)
        var_a2 = 0x48000 - var_a2;

    temp_v1_2 = arg0->vy;
    temp_v0_2 = arg1->vy;
    var_a3 = temp_v1_2 - temp_v0_2;
    if (var_a3 <= 0)
        var_a3 = temp_v0_2 - temp_v1_2;

    temp_a0 = arg0->vz;
    temp_a1 = arg1->vz;
    var_v1 = temp_a0 - temp_a1;
    if (var_v1 <= 0)
        var_v1 = temp_a1 - temp_a0;

    if (var_v1 >= 0x1C000)
        var_v1 = 0x38000 - var_v1;

    return var_a2 + var_a3 + var_v1;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE0BC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE180);

s16 func_800AE47C(VECTOR* arg0, VECTOR* arg1) {
    return func_800AE180(arg1->vx - arg0->vx, arg1->vz - arg0->vz, arg1->vx);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE4B8);

void* func_800AE5B8(void) { return (WmGetCurrRenderBufferId() * 0x24) + &D_800C6770; }

void* func_800AE5F0(void) { return (WmGetCurrRenderBufferId() * 0xC) + D_8010B068; }

s32 func_800AE628(void) { return D_8010B080; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE638);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AE8AC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AEA48);

void func_800AF0A0(s32 arg0) { D_8010B174 = arg0; }

void func_800AF0B0(void) {
    void func_800AF110(void);
    s32 i;
    s32 offset;

    i = 0;
    offset = 0;
    for (; i < 0x10; i++) {
        D_8010B18B[offset] = 0;
        *(s32*)&D_8010B17C[offset] = 0;
        offset += 0x24;
    }

    D_8010B3B8 = NULL;
    func_800AF110();
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF110);

void func_800AF1A8(u32 arg0, s32 arg1) {
    Unk8010B178* temp_v1;

    if (arg0 < 0x10) {
        temp_v1 = &D_8010B178[arg0];
        D_8010B3B8 = (Unk8010B3B8*)temp_v1;
        if (arg1 != -1) {
            temp_v1->unk13 = arg1;
        }
    }
}

void func_800AF1E8(u32 arg0, u32 arg1) {
    if ((D_8010B3B8 != NULL) && (arg0 < 0x24) && (arg1 < 0x1C)) {
        D_8010B3B8->unk0.vx &= 0x1FFF;
        D_8010B3B8->unk0.vz &= 0x1FFF;
        D_8010B3B8->unk0.vx |= arg0 << 13;
        D_8010B3B8->unk0.vz |= arg1 << 13;
    }
}

void func_800AF24C(s32 arg0, s32 arg1) {
    if (D_8010B3B8 != NULL) {
        D_8010B3B8->unk0.vx &= -0x2000;
        D_8010B3B8->unk0.vz &= -0x2000;
        D_8010B3B8->unk0.vx |= arg0 & 0x1FFF;
        D_8010B3B8->unk0.vz |= arg1 & 0x1FFF;
    }
}

void func_800AF2A4(u8 arg0, u8 arg1, u8 arg2) {
    if (D_8010B3B8 != NULL) {
        D_8010B3B8->unk10 = arg0;
        D_8010B3B8->unk11 = arg1;
        D_8010B3B8->unk12 = arg2;
    }
}

static void func_800AF2E4(s32 arg0) {
    if (D_8010B3B8)
        D_8010B3B8->unk1C = arg0;
}

static void func_800AF304(s32 arg0) {
    if (D_8010B3B8)
        D_8010B3B8->unk20 = arg0;
}

void func_800AF324(u8 arg0, u8 arg1, u8 arg2) {
    if (D_8010B3B8 != NULL) {
        D_8010B3B8->unk14 = arg0;
        D_8010B3B8->unk15 = arg1;
        D_8010B3B8->unk16 = arg2;
    }
}

void func_800AF364(u8 arg0, u8 arg1, u8 arg2) {
    if (D_8010B3B8 != NULL) {
        D_8010B3B8->unk18 = arg0;
        D_8010B3B8->unk19 = arg1;
        D_8010B3B8->unk1A = arg2;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF3A4);

s32 func_800AF96C(VECTOR* v) { return D_8010B3B8 ? func_800AE024(v, &D_8010B3B8->unk0) : 0; }

s16 func_800AF9A0(VECTOR* arg0) { return D_8010B3B8 == NULL ? 0 : func_800AE47C(arg0, (VECTOR*)D_8010B3B8); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AF9DC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AFCC8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AFFBC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0098);

void func_800B017C(s32 arg0) { (&D_8010B47C)[arg0] = (D_8010B488[arg0] < D_8010B494[arg0]) << 4; }

void func_800B01C4(s32 arg0) { (&D_8010B47C)[arg0] = (D_8010B488[arg0] > 0) ? -0x10 : 0; }

void func_800B0200(s32 arg0) {
    func_800B017C(arg0);
    D_8010B4A0[arg0] = 0x64;
}

s32 func_800B0240(void) { return !D_8010B47C; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0250);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0334);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B04AC);

// zolom init position
void func_800B0670(void) {
    VECTOR sp10;
    WorldZolomSegment* var_a0;

    WmGetPosFromPcEntity(&sp10);
    if (sp10.vz <= 0x23A97)
        for (var_a0 = D_8010C2AC; var_a0 < &D_8010C2AC[0x30]; var_a0++) {
            var_a0->x = 0x2008;
            var_a0->z = 0x6338;
            var_a0->z2 = 0;
            var_a0->x2 = 0;
        }
    else
        for (var_a0 = D_8010C2AC; var_a0 < &D_8010C2AC[0x30]; var_a0++) {
            var_a0->x = 0x1D4C;
            var_a0->z = 0x11F8;
            var_a0->z2 = 0;
            var_a0->x2 = 0;
        }
    D_8010C7F0 = 0x400;
    D_8010C42C = D_8010C2AC;
}

void func_800B075C(void) {
    if (D_8010C804 == 0) {
        func_800B0670();
        D_8010C804 = 1;
    }
}

s32 func_800B0794(void) {
    VECTOR sp10;
    WmGetPosFromPcEntity(&sp10);
    return D_8010C804 != 0 && (sp10.vx - 0x30000) < 0x10000U && (sp10.vz - 0x1C000) < 0x10000U;
}

static s32 func_800B0800(void) { return D_8010C808; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0810);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0BF4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0D98);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0E84);

void func_800B104C(void) {
    D_8010CA20 = 0;
    D_8010CA1C = 0;
    D_8010CA78 = 0;
    D_8010CA74 = 0;
    D_800BD144 = 0;
    D_8010CAF0 = 0;
    D_8010CA8C = 0;
    D_8010CAC8 = 0;
    D_8010CAC4 = 0;
    D_8010CAC0 = 0;
    D_8010CAF4 = 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B10AC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B11C4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B1650);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B190C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B1C80);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B21E4);

static void func_800B22E4(void) { func_800B190C(); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B2304);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B2638);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B271C);

void func_800B28CC(s32 arg0) {
    s32 param;

    if (D_8010CAF0 != 3) {
        if (arg0 != 1) {
            param = 0x26;
            if (arg0 == 2)
                param = 0x2A;
        } else
            param = 0x29;
    } else
        param = 4;
    WmSetActiveEntityWithModelId(param);
    switch (D_8010CAF0) {
    case 3:
        WmUnlinkActiveEntityFromAll();
        D_8010CAF0 = 0;
        break;
    case 1:
        if (func_800A8F48() != 0) {
            WmSetActiveEntityAsPcEntity();
            func_800BBD0C();
        } else
            func_800AB988(WmGetModelIdFromActiveEntity(), 5);
        break;
    case 2:
        func_800A9018();
        WmUnlinkActiveEntityFromAll();
        break;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B29CC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B2E90);

static void func_800B2F94(s32 arg0) { D_8010CAF0 = arg0; }

s32 func_800B2FA4(void) { return D_8010CA8C != 2 ? -(D_8010CA8C == 3) : 1; }

s32 func_800B2FD0(void) { return D_8010CACC != 0 || D_8010CAD0 != 0 || D_8010CAD4 != 0; }

void func_800B3018(void) {
    D_8010CAF4 = 1;
    if (D_8010CA8C != 1)
        D_8010CA8C = 0;
}

void func_800B3044(s16 arg0) {
    D_8010CAFC = arg0;
    D_8010CB10 = 0;
    D_8010CB0C = 0;
    D_8010CB08 = 0;
    D_8010CB04 = 0;
    D_8010CB00 = 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B307C);

static s16 func_800B32F0(void) { return D_8010CAFC; }

void func_800B3300(u32 arg0) {
    D_800C68EE = arg0 & 0xFF;
    D_800C6902 = (arg0 >> 8) & 0xFF;
    D_800C6916 = (arg0 >> 0x10) & 0xFF;
    D_8010CB14 = arg0 >> 0x18;
    D_8010CB1C = 0;
    D_8010CB18 = 0;
}

s32 func_800B3350(void) { return D_800C68EE | (D_800C6902 << 8) | (D_800C6916 << 0x10) | (D_8010CB14 << 0x18); }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B338C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3418);

void func_800B37E0(s32* arg0, s32* arg1) {
    if (arg0 != NULL) {
        *arg0 += D_8010CB18;
    }
    if (arg1 != NULL) {
        *arg1 += D_8010CB1C;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3828);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B392C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B39B4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3C40);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B40B4);

static const s32 D_800A0768[] = {0, 0x28};

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B4244);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B45DC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5274);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5314);

void func_800B579C(s32 arg0, u8 arg1, u8 arg2, u8 arg3) {
    u8* p = &D_8010D9B8[arg0 * 4];

    p[0] = arg1;
    p[1] = arg2;
    p[2] = arg3;
    p[3] = 0;
}

void func_800B57C0(s32 arg0) { D_8010D9BA[arg0 * 4] = 0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B57DC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B58F8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B59F4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5C7C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5DD8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5E28);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B624C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6348);

static void func_800B63E0(s32 arg0) { D_801159DC = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B63F0);

void func_800B64A0(void) { func_800B63F0(D_801159E0); }

static s32 func_800B64C8(void) { return D_801159E0; }

void func_800B64D8(u32 arg0) {
    D_8009A000[0] = 0x30;
    D_8009A004[0] = arg0;
    SystemAkaoExecute();
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B650C);

void func_800B6570(u32 arg0) {
    D_8009A000[0] = 0xC0;
    D_8009A004[0] = arg0;
    SystemAkaoExecute();
}

void func_800B65A4(u32 arg0, s32 arg1) {
    D_8009A000[0] = 0xBD;
    D_8009A004[0] = arg0;
    D_8009A008[0] = arg1;
    SystemAkaoExecute();
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B65E0);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B667C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6724);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B69A4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6A4C);

void func_800B6AEC(void) {
    if (D_80115A60 != 0) {
        D_80115A60 = 0;
        func_800ADD4C(2);
        func_800B6A4C();
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6B28);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6C84);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6D10);

void func_800B6DCC(void) {
    if (D_80115A60 != 0) {
        D_80115A60 = 0;
        func_800ADD4C(2);
        func_800B6D10();
    }
}

void func_800B6E08(void) {
    s32 i;

    if (D_80115A50 != 0) {
        D_80115A50 = 0;
        D_80115A64 = 3;

        for (i = 0; i < 0x2B; i++) {
            D_80115A14[i] = 0;
        }

        for (i = 0x20; i < 0x2B; i++) {
            D_801159E8[i] = -1;
        }
    }
}

void func_800B6E78(void) {
    s32 temp_v0;

    if (D_80115A60 != 0)
        SystemCdromReadChain();
    if (D_80115A50 != 0 && func_800A8CA4() == 0)
        func_800B6E08();
    if (D_80115A64 != 0 && --D_80115A64 == 0)
        func_800A8048();
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B6EFC);

void func_800B7104(s16 arg0) {
    D_80115A58 = arg0;
    func_800B6B28(arg0);
}

void func_800B7134(void) {
    SystemCdromAbortLoading();
    D_80115A60 = 0;
}

static void func_800B715C(s32 arg0) { D_80115A68 = arg0; }

static s32 func_800B716C(void) { return D_80115A68; }

s32 func_800B717C(void) {
    s32 temp_a0;
    s32 temp_v1;
    s32 var_v0;

    var_v0 = 0;
    if (*D_8009D288 >= 1000) {
        if (*D_8009D288 < 1580) {
            temp_a0 = D_8009D302 & 1;
            if (*D_8009D288 >= 1620)
                return (temp_a0 | 2) + 1;
            return temp_a0 + 1;
        }
        temp_v1 = (*D_8009D288 < 1620) ^ 1;
        if (D_8009D60E & 0x10)
            return (temp_v1 | 2) + 5;
        return temp_v1 + 5;
    }
    return var_v0;
}

s32 func_800B7200(void) { return D_8009D288[0] >= 1000 && D_8009D288[0] < 1200; }

static u8 func_800B7218(void) { return D_8009D686; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7228);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7480);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7620);

void func_800B76A8(void) {
    u32 var_a0;

    var_a0 = D_8009D685;
    if (var_a0 >= 3) {
        if (var_a0 >= 0x2B) {
            WmSetActiveEntityWithModelId(0x13);
            func_800BBA5C();
            var_a0 = 3;
        }
        WmSetActiveEntityWithModelId(var_a0);
        func_800BBA5C();
        func_800A31C0(func_800A97A8());
    }
}

void func_800B7714(s32 arg0) {
    u8* p;
    s32 index;

    index = ((((arg0 >> 8) - 1) << 1) & 0x1FE) | (arg0 & 1);
    p = &D_800BF5F0[index * 12];

    D_8009ABF6 = *(u16*)(p + 6);
    D_8009ABF8 = *(u16*)(p + 0);
    D_8009ABFA = *(u16*)(p + 2);
    D_8009AC16 = *(u16*)(p + 4);
    D_8011626C = 0;
    D_80116270 = arg0;
    D_8009AC18 = p[8];
}

void func_800B77A8(s32 arg0) {
    void func_800B7714(s32);

    if (arg0 & 0x40000000) {
        func_800B7714(0x2100);
    }
    D_8011626C = 1;
    D_80116274 = arg0;
}

void func_800B77F4(s32 arg0) {
    D_8009D268[0] = arg0;
    D_80116278 = 1;
    D_8009D2E7 = 1;
}

void func_800B7820(void) {
    D_80116278 = 0;
    D_8009D2E7 = 0;
}

void func_800B7838(void) {
    D_8011626C = 2;
    D_80116270 = 0;
    D_80116274 = 0;
}

s32 func_800B785C(void) { return D_8011626C; }

s32 func_800B786C(void) { return D_80116270; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B787C);

extern u8 D_8009D392;
extern u8 D_8009D393;

// Player party member model ID (0=Cloud, 1=Tifa, 2=Cid)
s32 func_800B79B8(void) {
    if ((*D_8009D391 != 0) && (D_8009D392 != 0) && (D_8009D393 != 0)) {
        if ((*D_8009D391 != 2) && (D_8009D392 != 2) && (D_8009D393 != 2)) {
            if ((*D_8009D391 == 8) || (D_8009D392 == 8) || (D_8009D393 == 8)) {
                return 2;
            }
            return 0;
        }
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7A40);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7AC0);

void func_800B7B1C(u8 arg0) { D_8009D684 = arg0; }

static s32 func_800B7B2C(void) { return D_8009D684; }

s32 func_800B7B3C(void) { return (g_BattleMode >> 3) & 1; }

// Enemy Lure/Away Modifier
s32 func_800B7B54(void) {
    u32 var_v1;

    var_v1 = D_80062F19;
    if (var_v1 > 0x10)
        var_v1 *= 2;
    return var_v1;
}

s32 func_800B7B78(void) {
    s32 var_v1;

    var_v1 = D_80062F1B & 0x7F;
    if (var_v1 >= 0x11)
        var_v1 *= 2;
    return var_v1;
}

u8 func_800B7BA0(void) { return D_80062F1B >> 7; }

static u8 func_800B7BB0(void) { return D_80062F1A; }

u8 func_800B7BC0(void) { return D_8009D40D & 1; }

static s32 func_800B7BD0(void) { return 1; }

s32 func_800B7BD8(void) {
    s32 var_a0;
    s32 var_v1;

    for (var_a0 = 0, var_v1 = 0; var_v1 < 3; var_v1++)
        var_a0 += D_8009CBDC[var_v1] != 0xFF;
    return (var_a0 < 2) ^ 1;
}

static s32 func_800B7C14(void) { return 1; }

void func_800B7C1C(void) {
    D_80116284 = 0;
    func_800B7C44();
}

// Reset World Step Fraction to -140 (spawn reset)
static void func_800B7C44(void) { D_8011627C = -0x8C; }

// Reset World Step Fraction to -30 (vehicle reset)
static void func_800B7C58(void) { D_8011627C = -0x1E; }

static void func_800B7C6C(s32 arg0) { D_80116280 = arg0; }

// World encounter check
INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B7C7C);

void func_800B832C(void) {
    VECTOR sp10;
    VECTOR sp20;
    s32 temp_a0;
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;

    temp_a0 = WmGetWmId();
    if ((D_8009AC2F == 0) && (temp_a0 != 2) && (func_800B2FD0() == 0) && (func_800A21A4() != 0)) {
        temp_s0 = func_800A9AD0();
        WmGetPosFromPcEntity(&sp10);
        WmGetPos2FromPcEntity(&sp20);
        if ((WmIsPcEntityModelInMask(0x47) != 0) && (D_80116280 != 0)) {
            if ((temp_s0 == 0) && ((sp10.vx != sp20.vx) || (sp10.vz != sp20.vz))) {
                if ((D_8011627C == 8) || (D_8011627C == 0x10))
                    func_800262D8();
                temp_v0 = D_8011627C < 0x10;
                D_8011627C += 1;
                if (temp_v0 == 0) {
                    D_8011627C = 0;
                    temp_v0_2 = func_800B7C7C();
                    if (temp_v0_2 != -1) {
                        func_800A9D88(&sp20);
                        func_800A3F4C(temp_v0_2);
                    }
                }
            }
        } else
            D_8011627C = 0;
    }
}

// type?
void func_800B8488(FieldScriptHeader* fieldScripts) {
    D_8008326C[0] = 0xFF;
    g_CurrentEntity = 0xFF;
    g_FieldScripts = fieldScripts;
    fieldScripts->stringOffset = 8;
    func_800B8760();
    g_pFieldState = &g_FieldState;
}

void func_800B84D8(u8 arg0) {
    if (g_WindowData[0].state == WSTATE_INIT) {
        D_8011628C = 0;
        D_80116288 = 0;
        func_800B8D4C(0, arg0);
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B851C);

static s32 func_800B857C(void) { return g_WindowData[0].state != WSTATE_INIT; }

s32 func_800B858C(void) {
    if ((g_WindowData[0].state != WSTATE_INIT) && (g_WindowData[0].state != WSTATE_CLOSING)) {
        func_800B89C4(0);
    }

    return g_WindowData[0].state != WSTATE_INIT;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B85D4);

extern s16 D_80116290;

s16 func_800B86C4(void) { return g_WindowData[0].state == WSTATE_INIT ? D_80116290 : -1; }

void func_800B86E8(SVECTOR* arg0) {
    if (arg0 != NULL) {
        func_800B8B00(0, arg0->vx, arg0->vy, arg0->vz, arg0->pad);
    }
}

void func_800B8720(s16 arg0, s16 arg1) { func_800B8A5C(0, arg0, arg1); }

static void func_800B8750(const char* str, s32 val, s32 kind) {}

static void func_800B8758(const char* errmsg) {}

void func_800B8760(void) {
    s32 i;

    g_WindowCount = 0;
    for (i = 0; i < 1; i++) {
        func_800B87D8(i);
    }
    if (g_FieldScripts->stringOffset != 0) {
        D_80116298 = (u8*)g_FieldScripts + g_FieldScripts->stringOffset;
    } else {
        D_80116298 = NULL;
    }
}

void func_800B87D8(s16 window) {
    s32 i;

    if (window == 1) {
        g_WindowData[window].y = 8;
    } else {
        g_WindowData[window].y = 149;
    }

    g_WindowData[window].x = 8;
    g_WindowData[window].width = 304;
    g_WindowData[window].height = 73;
    g_WindowData[window].currentWidth = 1;
    g_WindowData[window].currentHeight = 1;
    g_WindowData[window].state = WSTATE_INIT;
    g_WindowData[window].style = WSTYLE_NORMAL;
    g_WindowData[window].numDisplayType = WNDT_OFF;
    g_WindowData[window].unk1C = 0;
    g_WindowData[window].numDisplayLength = 6;
    g_WindowData[window].numDisplayX = 0;
    g_WindowData[window].numDisplayY = 0;
    g_WindowData[window].preventClose = 0;
    g_WindowToEntity[window] = 0xFF;

    for (i = 0; i < 4; i++) {
        D_801163B4[window][i] = 0;
        D_801163B8[window][i] = 0;
    }

    D_801163D0[window] = 0;
    if (g_DebugLevel & 3) {
        func_800B8750("mes reset=", window, 1);
    }
}

s32 func_800B89C4(s16 window) {
    switch (g_WindowData[window].state) {
    case WSTATE_SHOW:
        return 0;
    case WSTATE_TXT:
    case WSTATE_WAIT_ROW:
    case WSTATE_TXT_DONE:
    case WSTATE_SCROLL_ROW:
    case WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK:
    case WSTATE_PAUSE_TXT_UNTIL_OK:
        g_WindowData[window].state = WSTATE_CLOSING;
    }
    return 1;
}

void func_800B8A5C(s16 window, s8 style, s16 preventClose) {
    g_WindowData[window].style = style;
    g_WindowData[window].preventClose = preventClose;
}

void func_800B8A98(void) {
    s32 i;

    for (i = 0; i < 1; i++) {
        g_WindowData[i].state = WSTATE_INIT;
        g_WindowData[i].stringLength = 0;
        g_WindowToEntity[i] = 0xFF;
        D_801163D0[i] = 0;
    }
    g_WindowCount = 0;
}

void func_800B8B00(s16 window, s16 x, s16 y, s16 width, s16 height) {
    if (x < 8) {
        if (g_DebugLevel & 3) {
            func_800B8750("win limit x=", x, 2);
        }
        x = 8;
    }
    if (x + width > 312) {
        if (g_DebugLevel & 3) {
            func_800B8750("win limit x=", x + width, 3);
        }
        x = 312 - width;
    }
    if (y < 8) {
        if (g_DebugLevel & 3) {
            func_800B8750("win limit y=", y, 2);
        }
        y = 8;
    }
    if (y + height > 224) {
        if (g_DebugLevel & 3) {
            func_800B8750("win limit y=", y + height, 3);
        }
        y = 224 - height;
    }

    g_WindowData[window].x = x;
    g_WindowData[window].y = y;
    g_WindowData[window].width = width;
    g_WindowData[window].height = height;
}

void func_800B8CBC(s16 window, s16 dx, s16 dy) {
    g_WindowData[window].x += dx;
    g_WindowData[window].y += dy;
}

void func_800B8D20(s16 window, s16 height) { g_WindowData[window].height = height; }

s32 func_800B8D4C(u8 window, u8 message) {
    switch (g_WindowData[window].state) {
    case WSTATE_INIT:
        if (func_800B962C(window, message)) {
            return 1;
        }
        break;
    case WSTATE_SHOW:
        func_800B98F0(window);
        break;
    case WSTATE_TXT:
        func_800B9B2C(window);
        break;
    case WSTATE_SCROLL_ROW:
        func_800BA938(window);
        break;
    case WSTATE_SCROLL_TXT_WHILE_OK:
        func_800BAA00(window);
        break;
    case WSTATE_PAUSE_TXT_UNTIL_OK:
        if (g_pFieldState->pressedKeys & PADRright) {
            g_WindowData[window].state = WSTATE_TXT;
        }
        break;
    case WSTATE_PAUSE_TXT:
        if (D_801163D0[window] == 0) {
            g_WindowData[window].state = WSTATE_TXT;
        } else {
            D_801163D0[window]--;
        }
        break;
    case WSTATE_WAIT_ROW:
        if (g_pFieldState->pressedKeys & PADRright) {
            if (g_WindowData[window].currentRow == (g_WindowData[window].height - 9) / 16 - 1 + D_801162A4[window]) {
                g_WindowData[window].state = WSTATE_SCROLL_ROW;
                g_WindowData[window].textScrolling -= 2;
                D_801162A4[window]++;
            }
        }
        break;
    case WSTATE_TXT_DONE:
        if (!(g_WindowData[window].preventClose & 1) && (g_pFieldState->pressedKeys & PADRright)) {
            g_WindowData[window].state = WSTATE_CLOSING;
            func_800BAC70(window);
        }
        break;
    case WSTATE_WAIT_NEXT_WINDOW:
        if (g_pFieldState->pressedKeys & PADRright) {
            func_800BAB60(window);
        }
        break;
    case WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK:
        if (g_pFieldState->pressedKeys & PADRright) {
            g_WindowData[window].state = WSTATE_SCROLL_TXT_WHILE_OK;
            D_801162A8[window] = g_WindowData[window].currentRow * 16 + 17;
            g_WindowData[window].textScrolling -= 2;
        }
        break;
    case WSTATE_INIT_NEXT:
        func_800BAB60(window);
        break;
    case WSTATE_UNK5:
    case WSTATE_CLOSING:
        if (func_800BAC70(window)) {
            return 1;
        }
        break;
    }

    return 0;
}

s32 func_800B90C0(u8 window, u8 message, u8 first, u8 last, s16* selectedLine) {
    switch (g_WindowData[window].state) {
    case WSTATE_INIT:
        if (func_800B962C(window, message)) {
            return 1;
        }
        break;
    case WSTATE_SHOW:
        func_800B98F0(window);
        break;
    case WSTATE_TXT:
        func_800B9B2C(window);
        break;
    case WSTATE_SCROLL_ROW:
        func_800BA938(window);
        break;
    case WSTATE_SCROLL_TXT_WHILE_OK:
        func_800BAA00(window);
        break;
    case WSTATE_PAUSE_TXT_UNTIL_OK:
        if (g_pFieldState->pressedKeys & PADRright) {
            g_WindowData[window].state = WSTATE_TXT;
        }
        break;
    case WSTATE_PAUSE_TXT:
        if (D_801163D0[window] == 0) {
            g_WindowData[window].state = WSTATE_TXT;
        } else {
            D_801163D0[window]--;
        }
        break;
    case WSTATE_WAIT_ROW:
        if (g_pFieldState->pressedKeys & PADRright) {
            if (g_WindowData[window].currentRow == (g_WindowData[window].height - 9) / 16 - 1 + D_801162A4[window]) {
                g_WindowData[window].state = WSTATE_SCROLL_ROW;
                g_WindowData[window].textScrolling -= 2;
                D_801162A4[window]++;
            }
        }
        break;
    case WSTATE_TXT_DONE:
        if (!(g_WindowData[window].preventClose & 1)) {
            g_WindowData[window].pointerEnabled = 1;

            if (g_pFieldState->pressedKeysRaw & PADLup) {
                if (first < *selectedLine) {
                    func_800B95E8();
                }
                (*selectedLine)--;
            }
            if (g_pFieldState->pressedKeysRaw & PADLdown) {
                if (*selectedLine < last) {
                    func_800B95E8();
                }
                (*selectedLine)++;
            }
            if (*selectedLine < first) {
                *selectedLine = first;
            }
            if (last < *selectedLine) {
                *selectedLine = last;
            }

            g_WindowData[window].pointerX = 5;
            g_WindowData[window].pointerY = *selectedLine * 16 + 6;

            if (g_pFieldState->pressedKeys & PADRright) {
                func_800B95E8();
                g_WindowData[window].state = WSTATE_CLOSING;
                func_800BAC70(window);
            }
        }
        break;
    case WSTATE_WAIT_NEXT_WINDOW:
        if (g_pFieldState->pressedKeys & PADRright) {
            func_800BAB60(window);
        }
        break;
    case WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK:
        if (g_pFieldState->pressedKeys & PADRright) {
            g_WindowData[window].state = WSTATE_SCROLL_TXT_WHILE_OK;
            D_801162A8[window] = g_WindowData[window].currentRow * 16 + 17;
            g_WindowData[window].textScrolling -= 2;
        }
        break;
    case WSTATE_INIT_NEXT:
        func_800BAB60(window);
        break;
    case WSTATE_UNK5:
    case WSTATE_CLOSING:
        if (func_800BAC70(window)) {
            g_WindowData[window].pointerEnabled = 0;
            return 1;
        }
        g_WindowData[window].pointerEnabled ^= 1;
        break;
    }

    return 0;
}

void func_800B95E8(void) {
    *D_8009A000 = 0x30;
    *D_8009A004 = 1;
    *D_8009A008 = 0x40;
    SystemAkaoExecute();
}

s32 func_800B962C(s16 window, s16 stringId) {
    if (D_80116298 == NULL) {
        func_800B8758("No mes data!");
        return 1;
    }

    if (g_WindowToEntity[window] != 0xFF) {
        if (g_DebugLevel & 3) {
            func_800B8750("mes busy=", window, 1);
        }
        return 0;
    }

    g_WindowToEntity[window] = g_CurrentEntity;
    g_WindowData[window].currentWidth = g_WindowData[window].width / 4;
    g_WindowData[window].currentHeight = g_WindowData[window].height / 4;
    if (g_WindowData[window].currentHeight < 8) {
        g_WindowData[window].currentHeight = 8;
    }
    if (g_WindowData[window].currentWidth < 8) {
        g_WindowData[window].currentWidth = 8;
    }

    g_WindowData[window].text = D_801162B4[window];
    g_WindowData[window].textScrolling = 0;
    g_WindowData[window].stringLength = 0;
    g_WindowData[window].stringByteLength = 0;
    g_WindowData[window].currentRow = 0;
    g_WindowData[window].pointerEnabled = 0;
    D_801162B4[window][0] = 0xFF;

    D_801162B0[window] = D_80116298;
    D_801162B0[window] += D_80116298[stringId * 2 + 2];
    D_801162B0[window] += D_80116298[stringId * 2 + 3] << 8;

    g_WindowCount++;
    D_8011629C[window] = 1;
    D_801162A0[window] = 0;
    D_801162A4[window] = 0;
    D_801162AC[window] = 0;
    D_801163C0[window] = 0;
    D_801163C4[window] = -1;
    g_WindowData[window].state = WSTATE_SHOW;
    return 0;
}

void func_800B98F0(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            func_800B8750("mes busy=", window, 1);
        }
        return;
    }

    g_WindowData[window].currentWidth += g_WindowData[window].width / 4;
    if (g_WindowData[window].currentWidth < 8) {
        g_WindowData[window].currentWidth = 8;
    }
    if (g_WindowData[window].width < g_WindowData[window].currentWidth) {
        g_WindowData[window].currentWidth = g_WindowData[window].width;
    }

    g_WindowData[window].currentHeight += g_WindowData[window].height / 4;
    if (g_WindowData[window].currentHeight < 8) {
        g_WindowData[window].currentHeight = 8;
    }
    if (g_WindowData[window].height < g_WindowData[window].currentHeight) {
        g_WindowData[window].currentHeight = g_WindowData[window].height;
    }

    if (g_WindowData[window].currentWidth == g_WindowData[window].width &&
        g_WindowData[window].currentHeight == g_WindowData[window].height) {
        g_WindowData[window].state = WSTATE_TXT;
    }
}

void func_800B9B2C(s16 window) {
    u8 opcode;
    u16 len;
    s16 i;
    s16 baseCredit;
    s16 characterCost;
    u8* name;
    u16 value;

    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            func_800B8750("mes busy=", window, 1);
        }
        return;
    }

    /*
     * To render text gradually, the game implements a text-writing credit
     * system. The > comparison in the while-loop means it can emit one
     * fewer character on the first update.
     * D_8011629C can add 0-8 characters per update and increase scrolling
     * speed. It's ramped up or down based on the state of OK.
     * field_msg_speed  baseCredit  characterCost   chars/update
     * 0                6           1               6
     * 1-32             5           1               5
     * 33-64            4           1               4
     * 65-96            3           1               3
     * 97-159           2           1               2
     * 160-191          2           2               1
     * 192-223          2           3               2/3
     * 224-255          2           4               1/2
     */

    if (g_WindowData[window].preventClose & 2) {
        baseCredit = 256;
        characterCost = 1;
    } else {
        SaveWork* save;

        if (g_pFieldState->activeKeys & PADRright) {
            D_8011629C[window]++;
            if (D_8011629C[window] > 128) {
                D_8011629C[window] = 128;
            }
        } else {
            D_8011629C[window]--;
            if (D_8011629C[window] < 2) {
                D_8011629C[window] = 1;
            }
        }

        save = &Savemap;
        if (save->field_msg_speed < 128) {
            baseCredit = ((128 - save->field_msg_speed) >> 5) + 2;
            characterCost = 1;
        } else {
            baseCredit = 2;
            characterCost = ((save->field_msg_speed - 128) >> 5) + 1;
        }
    }

    D_801162A0[window] += characterCost * (D_8011629C[window] >> 4) + baseCredit;

    while (D_801162A0[window] > characterCost) {
        switch (*D_801162B0[window]) {
        // End of string.
        case 0xFF:
            g_WindowData[window].state = WSTATE_TXT_DONE;
            D_801162A0[window] = 0;
            goto end;

        // Next row.
        case 0xE7:
            if (g_WindowData[window].currentRow == (g_WindowData[window].height - 9) / 16 - 1 + D_801162A4[window]) {
                g_WindowData[window].state = WSTATE_WAIT_ROW;
                D_8011629C[window] = 1;
                D_801162A0[window] = 0;
                goto end;
            }
            D_801162B4[window][g_WindowData[window].stringByteLength] = *D_801162B0[window];
            D_801162B0[window]++;
            g_WindowData[window].stringByteLength++;
            g_WindowData[window].currentRow++;
            continue;

        // Wait for next window.
        case 0xE8:
        case 0xE9:
            D_801162B0[window]++;
            g_WindowData[window].state = WSTATE_WAIT_NEXT_WINDOW;
            D_8011629C[window] = 1;
            D_801162A0[window] = 0;
            goto end;

        // Write player-chosen character name from savemap.
        case 0xEA: // Cloud
        case 0xEB: // Barret
        case 0xEC: // Tifa
        case 0xED: // Aerith
        case 0xEE: // Red XIII
        case 0xEF: // Yuffie
        case 0xF0: // Cait Sith
        case 0xF1: // Vincent
        case 0xF2: // Cid
            value = *D_801162B0[window] - 0xEA;
            name = GetCharacterName(value);
            if (name[D_801162AC[window]] == 0xFF || D_801162AC[window] >= 9) {
                D_801162B0[window]++;
                D_801162AC[window] = 0;
            } else {
                D_801162B4[window][g_WindowData[window].stringByteLength] = name[D_801162AC[window]];
                g_WindowData[window].stringByteLength++;
                D_801162AC[window]++;
                g_WindowData[window].stringLength++;
                D_801162A0[window] -= characterCost;
            }
            continue;

        // Write name of party member.
        case 0xF3:
        case 0xF4:
        case 0xF5:
            value = Savemap.memory_bank_1[22 + *D_801162B0[window]];
            if (value == 0xFF) {
                if (D_801162AC[window] >= 9) {
                    D_801162B0[window]++;
                    D_801162AC[window] = 0;
                } else {
                    D_801162B4[window][g_WindowData[window].stringByteLength] = 0xD2;
                    g_WindowData[window].stringByteLength++;
                    D_801162AC[window]++;
                    g_WindowData[window].stringLength++;
                    D_801162A0[window] -= characterCost;
                }
            } else {
                name = GetCharacterName(value);
                if (name[D_801162AC[window]] == 0xFF || D_801162AC[window] >= 9) {
                    D_801162B0[window]++;
                    D_801162AC[window] = 0;
                } else {
                    D_801162B4[window][g_WindowData[window].stringByteLength] = name[D_801162AC[window]];
                    g_WindowData[window].stringByteLength++;
                    D_801162AC[window]++;
                    g_WindowData[window].stringLength++;
                    D_801162A0[window] -= characterCost;
                }
            }
            continue;

        // Opcode prefix.
        case 0xFE:
            D_801162B4[window][g_WindowData[window].stringByteLength] = *D_801162B0[window];
            D_801162B0[window]++;
            g_WindowData[window].stringByteLength++;
            switch (*D_801162B0[window]) {
            // Pause writing text until the player presses OK.
            case 0xDC:
                g_WindowData[window].stringByteLength--;
                D_801162B0[window]++;
                g_WindowData[window].state = WSTATE_PAUSE_TXT_UNTIL_OK;
                D_8011629C[window] = 1;
                D_801162A0[window] = 0;
                goto end;

            // Pause writing and wait for OK before scrolling the text.
            case 0xE0:
                g_WindowData[window].stringByteLength--;
                D_801162B0[window]++;
                g_WindowData[window].state = WSTATE_PAUSE_TXT_SCROLL_UNTIL_OK;
                D_8011629C[window] = 1;
                D_801162A0[window] = 0;
                goto end;

            // Copy an integer from a memory bank.
            case 0xDE:
            case 0xDF:
            case 0xE1:
                g_WindowData[window].stringByteLength--;
                D_801162B0[window]--;
                if (D_801163C4[window] == -1) {
                    // First iteration. Fetch and convert the value.
                    value = func_800BAE60(window);
                    if (g_DebugLevel & 3) {
                        func_800B8750("mpara=", value, 4);
                    }
                    opcode = D_801162B0[window][1];
                    switch (opcode) {
                    // Integer to decimal string.
                    case 0xDE:
                        func_800BB350(value, D_801163C8[window]);
                        break;
                    // Integer to decimal string with space fill.
                    case 0xE1:
                        func_800BB450(value, D_801163C8[window]);
                        break;
                        // Integer to hexadecimal string.
                    case 0xDF:
                        func_800BB568(value, D_801163C8[window]);
                        break;
                    }
                    D_801163C4[window]++;
                } else if (D_801163C8[window][D_801163C4[window]] == 0xFF || D_801163C4[window] >= 8) {
                    // Last converted character has been copied.
                    D_801162B0[window] += 2;
                    D_801163C4[window] = -1;
                    D_801163C0[window]++;
                } else {
                    // Copy the next character of the converted integer.
                    D_801162B4[window][g_WindowData[window].stringByteLength] = D_801163C8[window][D_801163C4[window]];
                    g_WindowData[window].stringByteLength++;
                    D_801163C4[window]++;
                    g_WindowData[window].stringLength++;
                    D_801162A0[window] -= characterCost;
                }
                continue;

            // Copy a string from a memory bank.
            case 0xE2:
                g_WindowData[window].stringByteLength--;
                D_801162B0[window]--;
                if (D_801163C4[window] == -1) {
                    value = D_801162B0[window][2];
                    value |= D_801162B0[window][3] << 8;
                    len = D_801162B0[window][4];
                    len |= D_801162B0[window][5] << 8;
                    if (g_DebugLevel & 3) {
                        func_800B8750("gstr=", value, 4);
                        if (g_DebugLevel & 3) {
                            func_800B8750("glen=", len, 4);
                        }
                    }
                    for (i = 0; i < len; i++) {
                        D_801163C8[window][i] = Savemap.memory_bank_1[value + i];
                    }
                    D_801163C8[window][i] = 0xFF;
                    D_801163C4[window]++;
                } else if (D_801163C8[window][D_801163C4[window]] == 0xFF) {
                    D_801162B0[window] += 6;
                    D_801163C4[window] = -1;
                } else {
                    D_801162B4[window][g_WindowData[window].stringByteLength] = D_801163C8[window][D_801163C4[window]];
                    g_WindowData[window].stringByteLength++;
                    D_801163C4[window]++;
                    g_WindowData[window].stringLength++;
                    D_801162A0[window] -= characterCost;
                }
                continue;

            // Font colors.
            case 0xD2: // Gray
            case 0xD3: // Blue
            case 0xD4: // Red
            case 0xD5: // Purple
            case 0xD6: // Green
            case 0xD7: // Cyan
            case 0xD8: // Yellow
            case 0xD9: // White
            // Special global colors.
            case 0xDA: // Flash colors
            case 0xDB: // Rainbow colors, changes color for each character
            // Toggle left padding of characters.
            case 0xE9:
                D_801162B4[window][g_WindowData[window].stringByteLength] = *D_801162B0[window];
                D_801162B0[window]++;
                g_WindowData[window].stringByteLength++;
                continue;

            // Wait until the window wait time reaches 0 before resuming.
            case 0xDD:
                g_WindowData[window].state = WSTATE_PAUSE_TXT;
                D_801162B0[window]++;
                g_WindowData[window].stringByteLength++;
                D_801163D0[window] = *D_801162B0[window];
                D_801162B0[window]++;
                g_WindowData[window].stringByteLength++;
                D_801163D0[window] |= *D_801162B0[window] << 8;
                D_801162B0[window]++;
                g_WindowData[window].stringByteLength++;
                goto end;

            default:
                D_801162B4[window][g_WindowData[window].stringByteLength] = *D_801162B0[window];
                D_801162B0[window]++;
                g_WindowData[window].stringByteLength++;
                g_WindowData[window].stringLength++;
                D_801162A0[window] -= characterCost;
                continue;
            }

        // Two-byte characters used in the Japanese extended font(?)
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
            D_801162B4[window][g_WindowData[window].stringByteLength] = *D_801162B0[window];
            D_801162B0[window]++;
            g_WindowData[window].stringByteLength++;

        // Fall through to copy the second byte of the character.
        // Also used to copy all other characters directly.
        default:
            D_801162B4[window][g_WindowData[window].stringByteLength] = *D_801162B0[window];
            D_801162B0[window]++;
            g_WindowData[window].stringByteLength++;
            g_WindowData[window].stringLength++;
            D_801162A0[window] -= characterCost;
            continue;
        }
    }

end:
    D_801162B4[window][g_WindowData[window].stringByteLength] = 0xFF;
}

void func_800BA938(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            func_800B8750("mes busy=", window, 1);
        }
        return;
    }

    if (g_WindowData[window].textScrolling & 0xF) {
        g_WindowData[window].textScrolling -= 2;
    } else {
        g_WindowData[window].state = WSTATE_TXT;
    }
}

void func_800BAA00(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            func_800B8750("mes busy=", window, 1);
        }
        return;
    }

    if (g_WindowData[window].textScrolling + D_801162A8[window] > 0) {
        g_WindowData[window].textScrolling -= D_8011629C[window] >> 2;
        if (g_pFieldState->activeKeys & PADRright) {
            D_8011629C[window]++;
            if (D_8011629C[window] > 128) {
                D_8011629C[window] = 128;
            }
        } else {
            D_8011629C[window]--;
            if (D_8011629C[window] < 2) {
                D_8011629C[window] = 1;
            }
        }
    } else {
        g_WindowData[window].state = WSTATE_INIT_NEXT;
    }
}

void func_800BAB60(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            func_800B8750("mes busy=", window, 1);
        }
        return;
    }

    g_WindowData[window].state = WSTATE_TXT;
    g_WindowData[window].stringByteLength = 0;
    g_WindowData[window].stringLength = 0;
    g_WindowData[window].textScrolling = 0;
    g_WindowData[window].currentRow = 0;
    D_801162B4[window][0] = 0xFF;
    D_801162A4[window] = 0;
    D_8011629C[window] = 1;
}

s32 func_800BAC70(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            func_800B8750("mes busy=", window, 1);
        }
        return 1;
    }

    if (g_WindowData[window].currentWidth >= 8) {
        g_WindowData[window].currentWidth -= g_WindowData[window].width / 4;
    } else {
        g_WindowData[window].currentWidth = 8;
    }

    if (g_WindowData[window].currentHeight >= 8) {
        g_WindowData[window].currentHeight -= g_WindowData[window].height / 4;
    } else {
        g_WindowData[window].currentHeight = 8;
    }

    if (g_WindowData[window].currentWidth < 9 && g_WindowData[window].currentHeight < 9) {
        g_WindowData[window].stringLength = 0;
        g_WindowData[window].state = WSTATE_INIT;
        g_WindowToEntity[window] = 0xFF;
        g_WindowCount--;
        return 1;
    }
    return 0;
}

u16 func_800BAE60(s16 window) {
    u16 value;
    u16 offset;

    switch (D_801163B4[window][D_801163C0[window]]) {
    case 0:
        value = D_801163B8[window][D_801163C0[window]];
        break;
    case 1:
        offset = D_801163B8[window][D_801163C0[window]];
        value = Savemap.memory_bank_1[offset];
        break;
    case 2:
        offset = D_801163B8[window][D_801163C0[window]];
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 3:
        offset = D_801163B8[window][D_801163C0[window]] + 0x100;
        value = Savemap.memory_bank_1[offset];
        break;
    case 4:
        offset = D_801163B8[window][D_801163C0[window]] + 0x100;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 11:
        offset = D_801163B8[window][D_801163C0[window]] + 0x200;
        value = Savemap.memory_bank_1[offset];
        break;
    case 12:
        offset = D_801163B8[window][D_801163C0[window]] + 0x200;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 13:
        offset = D_801163B8[window][D_801163C0[window]] + 0x300;
        value = Savemap.memory_bank_1[offset];
        break;
    case 15:
        offset = D_801163B8[window][D_801163C0[window]] + 0x400;
        value = Savemap.memory_bank_1[offset];
        break;
    case 14:
        offset = D_801163B8[window][D_801163C0[window]] + 0x300;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 7:
        offset = D_801163B8[window][D_801163C0[window]] + 0x400;
        value = Savemap.memory_bank_1[offset];
        value |= Savemap.memory_bank_1[offset + 1] << 8;
        break;
    case 5:
        offset = D_801163B8[window][D_801163C0[window]];
        value = g_FieldMapVars[offset];
        break;
    case 6:
        offset = D_801163B8[window][D_801163C0[window]];
        value = g_FieldMapVars[offset];
        value |= g_FieldMapVars[offset + 1] << 8;
        break;
    default:
        value = 0;
        break;
    }

    return value;
}

void func_800BB350(u16 value, u8* dst) {
    u32 foundDigit;
    s16 i;
    s16 divisor;
    s16 digit;

    foundDigit = 0;
    divisor = 10000;
    i = 0;
    while (divisor > 1) {
        digit = value / divisor;
        if (foundDigit || digit) {
            foundDigit = 1;
            dst[i] = D_800C7304[digit];
            i++;
        }
        value -= digit * divisor;
        divisor /= 10;
    }
    dst[i] = D_800C7304[value];
    dst[i + 1] = 0xFF;
}

void func_800BB450(u16 value, u8* dst) {
    s32 foundDigit;
    s16 i;
    s16 divisor;
    s16 digit;

    foundDigit = 0;
    divisor = 10000;
    i = 0;
    while (divisor > 1) {
        digit = value / divisor;
        if (foundDigit || digit) {
            foundDigit = 1;
            dst[i] = D_800C7304[digit];
            i++;
        } else {
            dst[i] = 0x3F;
            i++;
        }
        value -= digit * divisor;
        divisor /= 10;
    }
    dst[i] = D_800C7304[value];
    dst[i + 1] = 0xFF;
}

void func_800BB568(u16 value, u8* dst) {
    u32 foundDigit;
    s16 i;
    s16 divisor;
    s16 digit;

    foundDigit = 0;
    divisor = 0x1000;
    i = 0;
    while (divisor > 1) {
        digit = value / divisor;
        if (foundDigit || digit) {
            foundDigit = 1;
            dst[i] = D_800C7304[digit];
            i++;
        }
        value -= digit * divisor;
        divisor /= 16;
    }
    dst[i] = D_800C7304[value];
    dst[i + 1] = 0xFF;
}

s32 func_800BB650(s16 stringId) {
    s16 i;
    s16 j;
    u8* str;
    u8* charName;
    u8 value;

    if (D_80116298 == NULL) {
        func_800B8758("No mes data!");
        return 0;
    }

    str = D_80116298;
    j = 0;
    i = 0;
    str += D_80116298[stringId * 2 + 2];
    str += D_80116298[stringId * 2 + 3] << 8;

    do {
        switch (*str) {
        case 0xFF:
            goto end;

        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
        case 0xF0:
        case 0xF1:
        case 0xF2:
            charName = GetCharacterName((s16)(*str - 0xEA)) + j;
            if (*charName == 0xFF || j >= 9) {
                str++;
                j = 0;
            } else {
                j++;
                Savemap.memory_bank_4[104 + i] = *charName;
                i++;
            }
            break;

        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
            value = *str;
            str++;
            Savemap.memory_bank_4[104 + i] = value;
            i++;

        default:
            value = *str;
            str++;
            Savemap.memory_bank_4[104 + i] = value;
            i++;
            break;
        }
    } while (i < 23);

end:
    Savemap.memory_bank_4[104 + i] = 0xFF;
    return 1;
}

void func_800BB7DC(s16 battleCharId, s16 stringId) {
    u8* newName;
    s16 len;
    u8* charName;

    if (D_80116298 == NULL) {
        func_800B8758("No mes data!");
        return;
    }

    newName = D_80116298;
    newName += D_80116298[stringId * 2 + 2];
    newName += D_80116298[stringId * 2 + 3] << 8;
    len = 0;
    charName = GetCharacterName(battleCharId);

    while (*newName != 0xFF) {
        *charName++ = *newName++;
        len++;
    }

    if (len < 9) {
        *charName = 0xFF;
    }
}

void func_800BB8B0(void) {
    D_801163D4 = 0;
    D_801163D8 = 0;
    D_801163E8 = &D_801163E0;
    D_801163EC = 0;
    D_801163DC = 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BB8E8);

void func_800BB9A0(u8 arg0) {
    s8* temp_v1;

    temp_v1 = D_801163E8;
    if (temp_v1 < &D_801163E8) {
        D_801163E8 = temp_v1 + 1;
        *temp_v1 = arg0;
    }
}

u8 func_800BB9D0(void) {
    if (&D_801163E0 >= D_801163E8)
        return 0;

    D_801163E8 = D_801163E8 - 1;
    return D_801163E8[0];
}

u8 func_800BBA0C(void) {
    u8 var_a0;

    var_a0 = 0;
    if (&D_801163E0 < D_801163E8)
        var_a0 = D_801163E8[-1];

    return var_a0;
}

static void func_800BBA34(s8 arg0) { D_801163E0 = arg0; }

s32 func_800BBA44(void) { return &D_801163E0 < D_801163E8; }

static void func_800BBA5C(void) {
    VECTOR sp10;
    s32 var_a1;
    s32 var_s0;

    if (WmGetModelIdFromPcEntity() == 5 && D_801163EC) {
        func_800A98A4(1);
        func_800A368C(1);
        return;
    }
    if (func_800A99BC()) {
        WmGetPosFromActiveEntity(&sp10);
        func_800A6994(&sp10, WmGetModelIdFromActiveEntity() == 3 ? -1 : 1);
        func_800BB9A0(WmGetModelIdFromPcEntity());
        if (func_800A929C()) {
            WmLinkPcToActiveEntity();
            func_800B63F0(2);
            return;
        }
        WmUnlinkPcLinkedEntityFromAll();
        WmUnlinkPcEntityFromAll();
        WmSetActiveEntityAsPcEntity();
        switch (WmGetModelIdFromPcEntity()) {
        case 3:
            func_800A98A4(1);
            func_800A368C(1);
            func_800B5274();
            if (func_800B64C8() < 6) {
                func_800B63F0(func_800B7200() ? 1 : 3);
            }
            break;
        case 6:
            func_800B65E0(0x1EC);
            break;
        }
    }
}

// credit to Ethanol for this match
s32 func_800BBBB0(void) {
    s32 temp_s0;

    temp_s0 = WmGetPcEntityTerrainId();
    if (func_800A9240())
        return (0x221B0F03 >> temp_s0) & 1;

    switch (WmGetModelIdFromPcEntity()) {
    case 3:
        return temp_s0 == 0;
    case 5:
        return (0x70 >> temp_s0) & 1;
    case 6:
        return (0x221B0F83 >> temp_s0) & 1;
    case 0xD:
        return 1;
    }
    return 0;
}

void func_800BBC4C(void) {
    s32 temp_s0;

    temp_s0 = WmGetModelIdFromPcEntity();
    if (D_801163D4 == 0) {
        if (func_800BBBB0() != 0) {
            if ((temp_s0 == 3) || ((temp_s0 == 5) && (WmIsPcEntityPosNeedRecalculation() != 0)))
                func_800A368C(-1);
            else
                func_800A2108(0, 2);
            D_801163D4 = 1;
            if (temp_s0 == 3 || temp_s0 == 4)
                WmScriptDisableForPcEntity(1);
        }
        if (WmGetPcEntityTerrainId() == 0x1B)
            func_800ABA18(9);
    }
}

static void func_800BBD0C(void) { D_801163D4 = 1; }

void func_800BBD20(s32 arg0) {
    VECTOR sp10;
    VECTOR sp20;
    s16 temp_v0;
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_s2;
    s32 temp_s4;
    s32 var_v0;

    temp_s2 = WmGetModelIdFromPcEntity() == 3;
    if (D_801163D8 != 0) {
        D_801163D8 -= 1;
        return;
    }

    if ((func_800A369C() == 0) && (WmGetWmId() != 3)) {
        temp_s4 = InputReadPads();
        if ((D_801163D4 == 0) && (arg0 == 1)) {
            func_800BBA5C();
        } else if (func_800A21A4() != 0) {
            var_v0 = func_800A9240() == 0 ? temp_s4 & PADRdown
                                          : temp_s4 & (PADLup | PADLdown | PADLleft | PADLright | PADRdown);
            if ((var_v0 == 0) && D_801163DC > 0 && D_801163DC < 15 &&
                ((WmIsPcEntityModelInMask(0x2000) == 0) || (WmGetPcEntityTerrainId() == 0x12)))
                func_800BBC4C();
            else
                goto block_15;
        } else {
        block_15:
            if (D_801163D4 == 1)
                D_801163D4 = 2;
            else if (D_801163D4 == 2) {
                D_801163D4 = 0;
                WmSetPcEntityAsActiveEntity();
                temp_s1 = WmGetModelIdFromActiveEntity();
                WmGetPosFromActiveEntity(&sp10);
                WmGetPos2FromActiveEntity(&sp20);
                func_800A2108(1, 2);
                if ((sp10.vx != sp20.vx) || (sp10.vz != sp20.vz)) {
                    func_800B7C58();
                    if (temp_s2 != 0) {
                        func_800A98A4(0);
                        WmScriptDisableForPcEntity(0);
                    }
                    func_800AA1B8();
                    func_800A1DD0(0);
                    if ((temp_s1 == 5) && (WmIsPcEntityPosNeedRecalculation() != 0))
                        func_800A98A4(0);
                    else {
                        if (temp_s1 == 4)
                            WmScriptDisableForPcEntity(0);

                        func_800AB988(temp_s1, 5);
                        if (temp_s1 == 4)
                            WmScriptDisableForPcEntity(1);

                        if (func_800A929C() != 0) {
                            temp_v0 = func_800AE47C(&sp20, &sp10) - 0x400;
                            WmSetActiveEntityDirectionAndRot(temp_v0);
                            func_800BB9D0();
                            func_800A8E50();
                            if (temp_s1 < 0x29)
                                WmSetActiveEntityAsPcEntity();

                            func_800AA2E4(2);
                            WmSetActiveEntityDirectionAndRot(temp_v0);
                            func_800B63F0(1);
                            func_800A2108(0, 6);
                            if (temp_s1 == 4)
                                func_800A82DC();
                            else if (temp_s1 >= 0x29) {
                                WmSetActiveEntityWithModelId(func_800B79B8());
                                WmSetActiveEntityAsPcEntity();
                            }
                            func_800ADC3C(&sp10);
                        } else {
                            WmInsertInEntityStructList();
                            temp_s0 = func_800BB9D0() & 0xFF;
                            if (func_800A92F8(temp_s0) != 0) {
                                WmInitActiveEntityStruct(func_800BBA0C() & 0xFF);
                                WmSetActiveEntityAsPcEntity();
                                WmInsertInEntityStructList();
                                WmInitActiveEntityStruct(temp_s0);
                                WmLinkPcToActiveEntity();
                            } else {
                                WmInitActiveEntityStruct(temp_s0);
                                WmSetActiveEntityAsPcEntity();
                                if (temp_s2 != 0)
                                    func_800BCA48();
                            }
                            func_800A9DB4(&sp10);
                            if (func_800A9240() != 0)
                                func_800B63F0(2);
                            else
                                func_800B63F0(1);

                            if (temp_s1 == 6)
                                func_800B65E0(-0x1EC);
                            else if (temp_s1 == 5)
                                func_800B65E0(-0x1ED);

                            func_800A6994(&sp10, 1);
                        }
                    }
                } else {
                    if ((WmGetModelIdFromPcEntity() == 3) || (WmGetModelIdFromPcEntity() == 4))
                        WmScriptDisableForPcEntity(0);

                    if ((temp_s2 != 0) || ((WmGetModelIdFromPcEntity() == 5) && (WmIsPcEntityPosNeedRecalculation() != 0)))
                        func_800A368C(1);
                }
            }
        }
        if ((temp_s4 & PADRdown) != 0) {
            D_801163DC += 1;
            return;
        }
        D_801163DC = 0;
    }
}

static s32 func_800BC1AC(void) { return D_801163D4; }

static void func_800BC1BC(s32 arg0) { D_801163EC = arg0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BC1CC);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BC420);

void func_800BC9E8(s16 arg0) {
    D_801164F8 = arg0;
    if (WmGetModelIdFromPcEntity() != 3)
        D_801164FC = D_801164F8;
}

s16 func_800BCA38(void) { return D_801164F8; }

void func_800BCA48(void) {
    if (D_801164F8 == 1)
        D_801164F8 = D_801164FC;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCA78);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCB2C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCBE8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCECC);
