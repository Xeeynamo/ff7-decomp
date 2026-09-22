//! PSYQ=3.3 CC1=2.6.3 g=false gcoff=false
#include "world.h"
#include <libetc.h>
#include <psxsdk/inline_o.h>

static void WmSetActiveEntityDirectionAndRot(s16 arg0);
static void WmGetPosFromPcEntity(VECTOR* arg0);
static void WmGetPos2FromPcEntity(VECTOR*);
static void WmSetPcEntityAsActiveEntity(void);
static void func_800B7820(void);
static s32 func_800BC1AC(void);
WorldListNode* WmAssignRegionToNode(WorldListNode*);
static s32 func_800A9A70(void);
static void CopyAreaName(s16);
static void func_800AA02C(s32);
static void func_800AA04C(s32);
void func_800A12AC(void);
s32 WmFadeIsStopped(void);
void WmWriteSavemap(void);
void WmSyncPartyAndPcModel(void);
void SysMenuShow(s32);
void WmRotateVectorByYAngle(SVECTOR*, s16);
void WmQueueBlocksAroundPos(VECTOR*);
void func_800A5E28(void);
void WmRequestVisibleChunks(VECTOR*, s16);
s32 func_800A63FC(WorldChunkHeader*, s16, s16, s16*, s16*);
static WorldChunkHeader* AllocChunk(void);
void func_800A6168(VECTOR*, s32, s32 (*)[5]);
void func_800A71E8(VECTOR*, VECTOR*, VECTOR*, s16);
void WmMovePcEntityByDistance(s32);
static void WmRestoreEntityPosAndDirFromSavemap(WorldActor*);
s32 func_800ADFC0(void);
static s32 func_800B0800(void);
static void WmCreateShadowPacket(POLY_FT4*, RECT*);
static s32 func_800B716C(void);
static s32 func_800B7B2C(void);
static s32 func_800B7B3C(void);
static void RegisterChunksForNode(WorldListNode* arg0);
static void WmAbortMapLoading(void);
static void func_800A9064(s16 x, s16 z);
static s32 WmAddMutexPriority(s16 arg0);
void func_800AF1A8(u32 arg0, s32 arg1);
static s16 func_800AF9A0(VECTOR* arg0);
static void WmSetFadeIn(s32 arg0, s32 arg1);
static void WmSetFadeOut(s32 arg0, s32 arg1);
static void WmAbortModelLoading(void);
static s32 WmDialogSetWindowToCloseIfPossible(s16 window);
static void WmDialogSetPosAndSize(s16 window, s16 x, s16 y, s16 width, s16 height);
static s32 WmDialogSetMessageToShow(u8 window, u8 message);
static void WmDialogReset(s16 window);
static void WmDialogPlaySound(void);
static s32 WmDialogInitWindow(s16 window, s16 stringId);
static void WmDialogInscreaseWindow(s16 window);
static void WmDialogStringOutput(s16 window);
static void WmDialogTextScrollByRow(s16 window);
static void WmDialogTextScrollDuringOk(s16 window);
static void WmDialogStartText(s16 window);
static s32 WmDialogDiscreaseWindow(s16 window);
static u16 func_800BAE60(s16 window);
static void WmDialogAddDigitWithoutLeadingSpace(u16 value, u8* dst);
static void WmDialogAddDigitWithLeadingSpace(u16 value, u8* dst);
static void WmDialogAddHexDigitWithoutLeadingSpace(u16 value, u8* dst);
static void WmSetCamMode(s16 arg0);
static s16 WmGetCamMode(void);
static void func_800BCA48(void);
static s32 WmGetDistanceBetweenPoints(VECTOR* arg0, VECTOR* arg1);
static s32 WmGetDistanceToActivePoint(VECTOR* v);
static s32 func_800B785C(void);
static s32 func_800B786C(void);
static void WmDialogSetModeAndPermanency(s16 window, s16 style, s16 preventClose);
void func_800B4244(void);
void WmCalculateBoneMatrixes(void*, MATRIX*, s32, s32);
void WmCalculateModelLighting(void*, u8*);
void func_800B5314(WorldActor*, s32);
void WmDialogUpdate(void);
void WmFadeInit(void);
void WmFadeRender(void);
void WmHandleButtons(void);
void WmLoadTxzDataAndInit(s32);
void WmRenderAll(s16);
void WmScriptInitVariables(WorldScriptData*);
void WmUiMapCreate(void);
void WmUiMapUpdate(s16);
void WmUpdateLightingFromPoints(VECTOR*);
void WmUpdateSkyboxOverlayVertexes(s16);
void func_800A12AC(void);
void func_800A3964(void);
void func_800A3C74(void);
void func_800A806C(s16, s32);
void func_800A835C(void);
void func_800AEA48(s16);
void func_800B04AC(void);
void func_800B650C(void);
void func_800B7228(s32*, s32*, s32);
static void InitWorldState(void);
static void WmAbortMapLoadingWrapper(void);
static void func_800A886C(s32);
static void func_800A8A88(void);
static s32 WmGetPcEntityOriginalY(void);
static void func_800AA02C(s32);
static void func_800AA238(void);
static void WmPrepareEntities(void);
static void func_800AB570(void);
static void WmScriptRunAll(void);
static void UpdateSurfaceEffect(void);
static void func_800AF0B0(void);
static void func_800B104C(void);
static void UpdateWorldMode(void);
static void InitEffectPool(void);
static void WmUpdateEffects(void);
static void func_800B63E0(s32);
static void func_800B64A0(void);
static s32 func_800B64C8(void);
static void WmSetMusicVolume(u32);
static void func_800B6E78(void);
static void func_800B715C(s32);
static s32 func_800B717C(void);
static s32 func_800B7218(void);
static void GetSavedParams(s32*, s32*, s32*);
static void func_800B76A8(void);
static void func_800B7C1C(void);
static void func_800B7C6C(s32);
static void func_800B832C(void);
static void WmDialogsInit(FieldScriptHeader*);
static void func_800BB8B0(void);
static void func_800BBD20(s32);
static s32 WmScriptIsAnyScriptRuns(void);
void func_800ADC70(void);
void WmInitOverlayTexturePrims(void);
void WmUpdateZolom(void);
void WmUpdateAmbientSoundTimers(void);
static s32 func_800A9878(void);
static s32 func_800A82F0(void);
static s32 WmIsPcEntityModelInMask(s32);

extern s32 D_800BD13C;
extern u8 D_800BE5E8[];
extern s32 D_800E55F8;
extern s32 D_800E5640;
extern s32 D_800E567C;

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

static u32* AllocPrims(s32 arg0) {
    u32* cur;
    u32* next;
    u32* ret;

    cur = D_800D05E0;
    next = (u32*)((arg0 * 0x28) + (s32)cur);
    if (next < (u32*)((u8*)D_800D05DC + 0x20800)) {
        D_800D05E0 = next;
        ret = cur;
    } else {
        func_800A0B40(1);
    }
    return ret;
}

void WmLoadTxzFile(s32 sector, s32 size, u_long* dst, s32 compressed) {
    int (*load)(int, size_t, u_long*, void (*)());
    s32 i;

    while (1) {
        load = SystemLoadFileBySector;
        if (compressed) {
            load = SysCdromStartLoadLzs;
        }
        if (load(sector, size, dst, NULL) != 0) {
            i = 10000;
        } else {
            for (i = 0; i < 10000; i++) {
                if (SystemCdromReadChain() == 0) {
                    break;
                }
                VSync(0);
            }
        }
        if (i < 10000) {
            break;
        }
        SysCdromInit();
    }
}

static void func_800A0D1C(void) { D_800E55EC = 0; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmLoadTxzDataAndInit);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A12AC);

static void WmPrepareForRender(void) {
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

static s32 func_800A16E0(void) {
    if (D_800E55F4 == 0) {
        return 0;
    }

    return (s32)D_800BD130 + 0x2710;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmIsTerrainPassableByModel);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A19FC);

static s32 func_800A1D04(void) { return D_800E5648; }

static s32 WmGetRealCamRot(void) { return D_800E560C & 0xFFF; }

static void WmSetDesiredCamRot(s32 arg0) { D_800E5608 = arg0 & 0xFFF; }

static void WmSetCamRot(s32 arg0) { D_800E560C = D_800E5608 = arg0 & 0xFFF; }

static void func_800A1D54(s32 arg0) {
    D_800E5618 = arg0;
    if (arg0 == 1) {
        D_800E5638 = D_800C6638[D_800E5648];
        D_800E563C = D_800C6628[D_800E5648];
    }
}

static s32 func_800A1DB0(void) { return D_800E5618; }

static s32 func_800A1DC0(void) { return D_800E5654; }

static void func_800A1DD0(s32 arg0) { D_800E5654 = arg0; }

static s32 WmGetWmId(void) { return D_800E5634; }

void WmCalcViewMatrix(s16 rotY) {
    SVECTOR rot;
    VECTOR trans;
    VECTOR out;
    MATRIX m;
    MATRIX tilt;
    s32 value;

    rot.vx = WmUpdateCameraTransition();
    rot.vz = 0;
    rot.vy = 0;
    RotMatrix(&rot, &m);
    trans.vz = 0;
    trans.vy = 0;
    trans.vx = 0;
    TransMatrix(&m, &trans);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    rot.vy = 0;
    rot.vx = 0;
    rot.vz = D_800E5614;
    RotTrans(&rot, &out, &value);
    trans.vx = 0;
    trans.vy = 0;
    value = out.vy * 16 - D_80116508;
    trans.vz = D_800E5614;
    TransMatrix(&D_800E56B8, &trans);
    value = (value * 256) / (out.vz * 16);
    if (value < 0) {
        value = 0;
    } else if (value >= 512) {
        value = 511;
    }
    D_800E56D8 = rot.vx = D_800BE1E8[value] + 0x800;
    rot.vy = rotY;
    rot.vz = 0;
    RotMatrix(&rot, &m);
    rot.vy = 0;
    rot.vx = 0;
    rot.vz = D_800E5654;
    RotMatrix(&rot, &tilt);
    MulMatrix0(&tilt, &m, &D_800E5698);
}

static void WmSetTranslationVectorInScreenSpace(SVECTOR* arg0) {
    MATRIX m;
    SVECTOR v;
    VECTOR out;
    int flag;

    v = *arg0;
    v.vy = -D_80116508;
    SetRotMatrix(&D_800E5698);
    SetTransMatrix(&D_800E56B8);
    RotTrans(&v, &out, &flag);
    TransMatrix(&m, &out);
    SetTransMatrix(&m);
}

static void func_800A2040(void) {
    SetRotMatrix(&D_800E5698);
    SetTransMatrix(&D_800E56B8);
}

static s16 func_800A2078(void) { return D_800E56D8; }

static void WmSetCamView(s32 arg0) {
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

static void func_800A2108(s32 arg0, s32 arg1) {
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

static s32 func_800A21A4(void) { return D_800E55FC; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmHandleButtons);

void func_800A31C0(s16 arg0) { D_800E5668 = arg0 - 0x800 + D_800E5608; }

static s32 func_800A31E8(void) { return !D_800E5628; }

static void UpdateFogRanges(void) {
    s32 level;

    if (D_800E5650 == 0) {
        level = 120 - ((D_800E5660 * 30) >> 8);
    } else {
        level = 160 - ((D_800E5660 * 70) >> 8);
    }
    D_800E55F0 = level;
    D_800C65EC = 400 - ((D_800E5660 * 200) >> 8);
    D_800E5614 = 10000 - ((D_800E5660 * 6000) >> 8);
    D_800E5678 = 5000 - ((D_800E5660 * 2500) >> 8);
}

static s32 func_800A32F4(void) { return D_800E5678; }

s16 WmUpdateCameraTransition(void) {
    VECTOR pos;
    VECTOR pos2;
    s32 step;
    s32 rate;
    s32 y;
    s32 target;
    s32 camView;
    s32 camX;
    s32 camZ;
    s32 next;
    s32 next2;

    if (D_800E5658 != 0) {
        if (D_800E565C == 0) {
            if (D_800E5658 > 0) {
                WmSetCamView(3);
                D_800E5640 = WmGetModelIdFromPcEntity() == 3 ? 4000 : 2000;
            }
            D_800C84D4 = D_800E5608;
            if (D_800E5608 >= 2048) {
                D_800C84D4 = D_800E5608 - 4096;
            }
        }
        if (D_800E5660 < 128) {
            step = D_800E565C + D_8011650C;
        } else {
            rate = D_8011650C;
            step = D_800E565C - rate;
        }
        D_800E565C = step;
        if (D_800E565C != 0) {
            D_800E5660 += D_800E565C;
            if (D_800E5660 < 0) {
                D_800E5660 = 0;
            } else if (D_800E5660 > 256) {
                D_800E5660 = 256;
            }
            UpdateFogRanges();
            WmGetPosFromPcEntity(&pos);
            WmGetPos2FromPcEntity(&pos2);
            y = WmGetPcEntityOriginalY();
            if (D_800E5658 > 0) {
                target = D_800E5640;
            } else {
                target = pos2.vy;
            }
            target = ((target - y) * D_800E5660) >> 8;
            func_800AA04C(y + target);
            if ((u32)D_800E5650 < 2) {
                D_800E5608 = (D_800E5660 * D_800C84D4) >> 8;
            }
        } else {
            if (D_800E5658 < 0) {
                WmSetCamView(D_800E5650);
                func_800A2108(0, 2);
            }
            D_800E5658 = 0;
        }
    }
    if (D_800E5658 >= 0 || D_800E5650 != 0) {
        camView = D_800E5648;
    } else {
        camView = 0;
    }
    camX = D_800E563C;
    if (camX == 0) {
        camX = D_800C6628[camView] + D_800E5664;
    }
    if (D_8011650C == 1) {
        next = ((D_800E5610 * 7) + camX) >> 3;
    } else {
        next = ((D_800E5610 * 3) + camX) >> 2;
    }
    camZ = D_800E5638;
    D_800E5610 = next;
    if (camZ == 0) {
        camZ = D_800C6638[camView];
    }
    if (D_8011650C == 1) {
        next2 = ((D_800E5614 * 7) + camZ) >> 3;
    } else {
        next2 = ((D_800E5614 * 3) + camZ) >> 2;
    }
    D_800E5614 = next2;
    return ((((D_80116508 >> 5) + 1750) * D_800E5660) + ((256 - D_800E5660) * D_800E5610)) >> 8;
}

void func_800A368C(s32 arg0) { D_800E5658 = arg0; }

static s32 func_800A369C(void) { return D_800E5658; }

void WmSetModelTransformMatrix(FieldModelEntry* model, SVECTOR* rot, MATRIX* m, s32 arg3) {
    MATRIX tmp;
    VECTOR trans;
    SVECTOR v;

    tmp.m[0][0] = tmp.m[1][1] = tmp.m[2][2] = 0x1000;
    tmp.m[0][1] = tmp.m[0][2] = tmp.m[1][0] = tmp.m[1][2] = tmp.m[2][0] = tmp.m[2][1] = 0;
    RotMatrixX(rot->vx, &tmp);
    RotMatrixZ(rot->vz, &tmp);
    RotMatrixY(rot->vy, &tmp);
    MulMatrix0(&D_800E5698, &tmp, m);
    SetRotMatrix(&D_800E5698);
    if (D_800E5670 > 0 && arg3 != 0) {
        trans.vx = D_800E56B8.t[0] / 8;
        trans.vy = D_800E56B8.t[1] / 8;
        trans.vz = D_800E56B8.t[2] / 8;
        D_800E5670--;
        if (D_800E5670 == 0) {
            PlayMusicTrack(D_80116510);
        }
    } else {
        trans.vx = D_800E56B8.t[0] / 4;
        trans.vy = D_800E56B8.t[1] / 4;
        trans.vz = D_800E56B8.t[2] / 4;
    }
    TransMatrix(m, &trans);
    SetTransMatrix(m);
    v.vx = model->translationX;
    v.vy = model->translationY;
    v.vz = model->translationZ;
    gte_ldv0(&v);
    gte_rt();
    gte_stlvnl(&trans);
    model->translationX = model->translationY = model->translationZ = 0;
    TransMatrix(m, &trans);
}

static void func_800A38C8(void) {
    if (g_PartyUpdatedByFieldScript == 1) {
        func_800260DC();
        func_80026090();
        g_PartyUpdatedByFieldScript = 0;
    }
}

static void func_800A3908(void) {
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

void WmUpdateWorldState(void) {
    VECTOR pos;

    switch (D_800E566C) {
    case 0:
        if (D_800E5638 != 0) {
            D_800E5638 += 80;
            if ((D_800E5638 >= (D_800C6638[D_800E5648] >> 1)) && (D_800E5634 != 2)) {
                D_800E55F4 = 1;
            }
            if (D_800E5638 >= D_800C6638[D_800E5648]) {
                D_800E5638 = 0;
                if (D_800E563C == 0) {
                    D_800E566C = 1;
                }
            }
        }
        if (D_800E563C != 0) {
            D_800E563C += 3;
            if (D_800E563C >= D_800C6628[D_800E5648]) {
                D_800E563C = 0;
                if (D_800E5634 != 2) {
                    D_800E55F4 = 1;
                }
                if (D_800E5638 == 0) {
                    D_800E566C = 1;
                }
            }
        }
        break;
    case 2:
        if (WmFadeIsStopped() != 0) {
            CopyAreaName(func_800A9A70());
            WmWriteSavemap();
            WmAddMutexPriority(3);
            func_800A38C8();
            func_800A3908();
            SysMenuShow(0);
            WmRemoveMutexPriority(3);
            func_800A2108(1, 1);
            WmSyncPartyAndPcModel();
            func_800A12AC();
            WmPrepareForRender();
            WmSetFadeIn(16, 1);
            D_800E566C = 1;
        }
        break;
    case 4:
    case 5:
        WmGetPosFromPcEntity(&pos);
        func_800AA02C(pos.vy + D_800E5644);
        D_800E5644 += D_800E5644 >> 2;
        if (WmFadeIsStopped() != 0) {
            D_800E566C = (D_800E566C == 4) ? 6 : 7;
        }
        break;
    case 3:
        D_800E566C = 9;
        break;
    default:
        if (D_800E566C < 0) {
            D_800E566C = D_800E566C + 1;
            if (D_800E566C == 0) {
                SetDispMask(1);
                if (D_800E5634 != 2) {
                    D_800E55F4 = 1;
                }
                D_800E566C = 1;
                WmSetFadeIn(16, 1);
            }
        }
        break;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A3C74);

static void WmSubmarineFloatToPlanet(void) {
    D_800E566C = 5;
    D_800E5644 = 0x14;
    WmSetFieldToLoad(0);
    func_800A2108(0, 0);
    WmSetFadeOut(0x10, 1);
}

static void WmSubmarineSubmergeUnderwater(void) {
    D_800E566C = 4;
    D_800E5644 = -0x14;
    WmSetFieldToLoad(0);
    func_800A2108(0, 0);
    WmSetFadeOut(0x10, 1);
}

static void func_800A3E9C(s32 arg0) {
    WmSetFieldToLoad(arg0);
    D_800E566C = 8;
}

static void func_800A3EC8(s32 arg0) {
    if (D_800E566C <= 0) {
        D_800E563C = 0;
        D_800E5638 = 0;
        D_800E55F4 = 1;
        D_800E566C = 1;
    }
    if (D_800E566C == 1) {
        D_800E566C = 3;
        func_800A2108(0, 0);
        WmSetFieldToLoad(arg0);
    }
}

static void func_800A3F4C(s32 arg0) {
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

static void WmResetGame(void) {
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

static s32 func_800A4080(void) { return (D_800E5648 & 3) | ((WmGetCamMode() * 4) & 0xC); }

static void func_800A40B8(s32 arg0) {
    WmSetCamView(arg0 & 3);
    WmSetCamMode((arg0 >> 2) & 3);
}

static s32 func_800A40F0(s16 arg0) {
    if (arg0 < 0) {
        return 0;
    }
    if (arg0 >= D_800BE5F0[0]) {
        return 0;
    }

    return (s32)D_800BE5F0 + D_800BE5F0[arg0 + 1];
}

static void InitSpritePrims(void) {
    s32 i;
    s32 tpage;

    D_800E56F4 = 0;
    for (i = 0; i < 2; i++) {
        setSprt(&D_800C6648[i]);
        if (GetGraphType() == 1 || GetGraphType() == 2) {
            tpage = 0x29;
        } else {
            tpage = 0x19;
        }
        SetDrawMode((DR_MODE*)((i * 0xC) + (s32)D_800E56DC), 0, 0, tpage, NULL);
    }
}

static void func_800A41E8(s32 arg0) {
    switch (arg0) {
    case 1:
        func_800A2108(0, 4);
        WmSetFadeOut(4, 1);
        break;
    case 4:
        func_800A2108(1, 4);
        WmSetFadeIn(0x10, 1);
        arg0 = 0;
        break;
    }
    D_800E56F4 = arg0;
}

static void UpdateFadeOverlay(void) {
    s32 slot;
    s32 vsync;

    slot = WmGetCurrRenderBufferId();
    switch (D_800E56F4) {
    case 1:
        if (WmFadeIsStopped() != 0) {
            D_800E56F4 = 2;
        }
        break;
    case 2:
        vsync = VSync(-1);
        D_800C6648[slot].v0 = ((vsync >> 2) & 0x10) - 128;
        addPrim(D_800BD130, &D_800C6648[slot]);
        addPrim(D_800BD130, &D_800E56DC[slot]);
        if ((vsync - D_800C84F0) >= 4 && (vsync - D_800C84F0) <= 5) {
            D_800E56F4 = 3;
        }
        D_800C84F0 = vsync;
        break;
    case 3:
        D_800C6648[slot].v0 = 160;
        addPrim(D_800BD130, &D_800C6648[slot]);
        addPrim(D_800BD130, &D_800E56DC[slot]);
        break;
    }
}

static void func_800A4494(s32 arg0) { D_800E561C = arg0; }

static void func_800A44A4(s32 arg0) { D_800E5620 = arg0; }

static void func_800A44B4(s32 arg0) { D_800E5624 = arg0; }

static void func_800A44C4(void) {
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

static void func_800A45C4(s32 arg0) { D_800E5638 = arg0; }

static void func_800A45D4(s32 arg0) { D_800E563C = arg0; }

static void func_800A45E4(s32 arg0) { D_800E5674 = arg0; }

static s32 func_800A45F4(void) { return D_800E5674; }

static void func_800A4604(void) {}

void WORLD_Main(s32* arg0, s32* arg1, s32* arg2, s32 arg3) {
    VECTOR pcEntityPos;
    s32 temp_v0;
    s16 var_a0;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 var_s0;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v1;

    D_800E566C = 0;
    D_800E567C = arg3;
    D_800E566C = 0;
    while (D_800E566C < 9) {
        if (*arg0 != 0) {
            var_v1 = func_800B7218();
        } else {
            var_v1 = 2;
            if (D_800E566C != 6) {
                if (*arg1 != 0x1E && *arg1 != 0x1F && *arg1 != 0x2A && *arg1 != 0x38) {
                    var_v1 = 0;
                    if (D_800E566C != 7) {
                        var_v1 = -(*arg1 >= 0x3C) & 3;
                    }
                }
            }
        }
        D_800E5634 = var_v1;
        temp_s0 = func_800B717C();
        if ((*arg0 == 0) && ((u32)(*arg1 - 0x24) < 4U)) {
            WmLoadTxzDataAndInit(temp_s0 > 0 ? 10 : 9);
        } else {
            var_v0 = (D_800E5634 == 0 ? temp_s0 : (u16)D_800E5634 + 9);
            var_v0 <<= 16;
            WmLoadTxzDataAndInit(var_v0 >> 16);
        }
        func_800B650C();
        func_800B715C(temp_s0 == 3 || temp_s0 == 4 || temp_s0 == 6 || temp_s0 == 8);
        D_800E5648 = 0;
        WmUiMapCreate();
        InitWorldState();
        func_800B7228(arg0, arg1, D_800E5634 != 3);
        if (D_800E5634 == 2) {
            WmSetCamView(2);
        }
        D_800E5604 = 0;
        UpdateFogRanges();
        func_800A12AC();
        func_800ADC70();
        func_800AF0B0();
        WmFadeInit();
        func_800B104C();
        func_800B7C1C();
        func_800B7C6C(1);
        InitSpritePrims();
        WmDialogsInit((FieldScriptHeader*)D_800BE5E8);
        func_800B04AC();
        WmInitOverlayTexturePrims();
        InitEffectPool();
        func_800A8A88();
        func_800BB8B0();
        WmSetMusicVolume(127);
        WmScriptInitVariables(&D_800D05EC);
        func_800ABA18(0);
        WmScriptRunAll();
        D_800E5640 = WmGetModelIdFromPcEntity() == 3 ? 0xFA0 : 0x7D0;
        if (*arg0 == 1 || *arg0 == 2 || D_800E566C == 6 || D_800E566C == 7) {
            func_800B76A8();
            if (D_800E566C == 6 || D_800E566C == 7) {
                func_800A886C(0xBB8);
            }
        }
        if (D_800E5634 == 2) {
            func_800A98A4(1);
            func_800AA02C(-0xBB8);
        }
        func_800A835C();
        WmGetModelDataByModelId(WmGetPcCharModelIdFromParty());
        if (func_800B64C8() < 6) {
            func_800B63E0(1);
        }
        if (func_800B64C8() != 0) {
            func_800B64A0();
        } else {
            PlayMusicTrack(1);
        }
        WmHandleButtons();
        D_800E566C = -15;
        while (D_800E566C < 6) {
            WmPrepareForRender();
            D_800BD13C = 0;
            WmPrepareEntities();
            WmGetPosFromPcEntity(&pcEntityPos);
            var_s0 = pcEntityPos.vy - D_80116508;
            if (var_s0 <= 0) {
                var_s0 = D_80116508 - pcEntityPos.vy;
            }
            if (D_800E5618 != 2) {
                if (func_800A9878() || var_s0 > 0xC8) {
                    var_v0_2 = ((D_80116508 * 7) + pcEntityPos.vy) >> 3;
                } else if (var_s0 > 0x32) {
                    var_v0_2 = pcEntityPos.vy <= D_80116508 ? D_80116508 - 0x32 : D_80116508 + 0x32;
                } else {
                    var_v0_2 = pcEntityPos.vy;
                }
                var_v1 = var_v0_2;
                D_80116508 = var_v1;
            }
            WmHandleButtons();
            func_800A44C4();
            WmGetPosFromPcEntity(&pcEntityPos);
            WmUpdateLightingFromPoints(&pcEntityPos);
            WmScriptRunAll();
            WmCalcViewMatrix(D_800E560C);
            func_800A806C(D_800E560C, 1);
            UpdateSurfaceEffect();
            func_800A3C74();
            func_800A3964();
            if (D_800E566C < 9) {
                WmDialogUpdate();
            }
            WmUpdateAmbientSoundTimers();
            if ((D_800E5648 == 3 && D_800E5658 == 0) || (D_800E5634 == 2 && func_800A82F0())) {
                WmGetPosFromPcEntity(&pcEntityPos);
                temp_v0 = WmGetPcEntityOriginalY();
                temp_s0_2 = temp_v0 + 0xC8;
                D_800E55F8 = temp_s0_2 < pcEntityPos.vy;
                if (!D_800E55F8 && !D_800E5658) {
                    if (WmIsPcEntityModelInMask(0x2000)) {
                        func_800AA02C(temp_s0_2 > -0xBB8 ? -0xBB8 : temp_s0_2);
                    } else if (WmGetModelIdFromPcEntity() == 3 || temp_v0 < 0x7D0) {
                        func_800AA02C(temp_s0_2);
                    } else {
                        func_800AA238();
                    }
                }
            }
            WmUpdateSkyboxOverlayVertexes(D_800E560C);
            func_800AEA48(D_800E560C);
            UpdateFadeOverlay();
            WmFadeRender();
            if ((D_800E566C < 9) && (D_800E5634 != 3)) {
                WmUiMapUpdate(D_800E560C);
            }
            func_800BBD20(0);
            UpdateWorldMode();
            SetGeomScreen(D_800C65EC);
            if (D_800E566C >= 0) {
                func_800AB570();
            }
            WmUpdateEffects();
            WmUpdateZolom();
            func_800B6E78();
            if (D_800E566C == 1) {
                func_800B832C();
            }
            WmRenderAll(D_8011650C == 1 ? D_8011650C - 1 : D_8011650C);
            if ((InputReadPadsRaw() & 0x90F) == 0x90F) {
                WmResetGame();
            }
        }
        func_800ABA18(1);
        while (WmScriptIsAnyScriptRuns() != 0) {
            WmScriptRunAll();
        }
        WmAbortMapLoadingWrapper();
        GetSavedParams(arg0, arg1, arg2);
        if (*arg0 == 1) {
            WmAddMutexPriority(3);
            func_800A38C8();
        }
        func_800B650C();
        while (DrawSync(1) != 0) {
        }
        func_800A3908();
    }
    WmGetCurrRenderBufferId();
}

void WmInitLoadMapFileStruct(void) {
    s16 i;

    D_800E5714 = 0x80156000;
    for (i = 0; i < 8; i++) {
        D_800E5718[i].next = &D_800E5718[i + 1];
    }
    D_800E5718[i].next = NULL;
    D_800E5760 = D_800E5718;
    D_800E5768 = NULL;
    D_800E5764 = NULL;
    for (i = 0; i < 19; i++) {
        D_800E576C[i].next = &D_800E576C[i + 1];
    }
    D_800E576C[i].next = NULL;
    D_800E580C = D_800E576C;
    D_800E5810 = NULL;
    D_800E5814 = 0;
    D_800E5818 = 0;
    D_800E581C = 0;
    D_800E5820 = -1;
    D_800E5824 = 0;
}

// Converts x and y coords of megachunk into index
static s16 WmGetBlockIdByXZForPlanet(s16 x, s16 y) {
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

void WmQueueBlocksAroundPos(VECTOR* pos) {
    s32 x0;
    s32 z0;
    s32 x1;
    s32 z1;
    s32 x;
    s32 z;
    WorldListNode* list;
    WorldListNode* node;
    WorldListNode* prev;
    WorldListNode* cur;
    WorldListNode* last;

    x0 = (pos->vx - 0x4000) / 32768;
    z0 = (pos->vz - 0x4000) / 32768;
    x1 = (pos->vx + 0x4000) / 32768;
    z1 = (pos->vz + 0x4000) / 32768;
    list = NULL;
    for (z = z0; z <= z1; z++) {
        for (x = x0; x <= x1; x++) {
            node = D_800E580C;
            if (node == NULL) {
                func_800A0B40(10);
            }
            D_800E580C = node->next;
            node->next = list;
            list = node;
            node->unk4 = WmGetBlockIdByXZForPlanet(x, z);
        }
    }
    cur = D_800E5764;
    last = NULL;
    while (cur != NULL) {
        prev = NULL;
        node = list;
        while (node != NULL) {
            if (node->unk4 == cur->unk4) {
                break;
            }
            prev = node;
            node = node->next;
        }
        if (node != NULL) {
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                list = node->next;
            }
            node->next = D_800E580C;
            D_800E580C = node;
            if (last != NULL) {
                last->next = cur->next;
                cur->next = D_800E5764;
                D_800E5764 = cur;
                cur = last->next;
                continue;
            }
        }
        last = cur;
        cur = cur->next;
    }
    cur = D_800E5768;
    while (cur != NULL) {
        prev = NULL;
        node = list;
        while (node != NULL) {
            if (node->unk4 == cur->unk4) {
                break;
            }
            prev = node;
            node = node->next;
        }
        if (node != NULL) {
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                list = node->next;
            }
            node->next = D_800E580C;
            D_800E580C = node;
        }
        cur = cur->next;
    }
    node = list;
    while (node != NULL) {
        node = WmAssignRegionToNode(node);
    }
}

static WorldListNode* AllocRegionNode(void);

WorldListNode* WmAssignRegionToNode(WorldListNode* node) {
    WorldListNode* p;
    s16 idx;

    p = AllocRegionNode();
    if (p != NULL) {
        idx = p - D_800E5718;
        node->unk6 = idx;
        D_800E5718[idx].unk4 = node->unk4;
        p = node->next;
        node->next = D_800E5810;
        D_800E5810 = node;
    } else {
        p = node->next;
        node->next = D_800E580C;
        D_800E580C = node;
    }
    return p;
}

static WorldListNode* AllocRegionNode(void) {
    WorldListNode* node;
    WorldListNode* prev;
    WorldListNode* head;

    if (D_800E5760 != NULL) {
        node = D_800E5760;
        D_800E5760 = node->next;
    } else {
        node = D_800E5764;
        if (node != NULL) {
            prev = NULL;
            while (node->next != NULL) {
                prev = node;
                node = node->next;
            }
            if (prev != NULL) {
                prev->next = NULL;
            } else {
                node = NULL;
            }
        }
    }
    if (node != NULL) {
        head = D_800E5768;
        D_800E5768 = node;
        node->next = head;
    }
    return node;
}

static void FreeListNode(WorldListNode* arg0, WorldListNode* arg1) {
    WorldListNode* tmp;

    if (arg1 != NULL) {
        arg1->next = arg0->next;
    } else {
        D_800E5810 = arg0->next;
    }
    WmStartLoadingMapFileBlock(arg0);

    tmp = D_800E580C;
    D_800E580C = arg0;
    arg0->next = tmp;
}

static void WmLoadClosestMapFileBlock(s16 arg0, s16 arg1) {
    WorldListNode* node;
    WorldListNode* prev;
    WorldListNode* best;
    WorldListNode* bestPrev;
    s32 bestDist;
    s32 dist;
    s32 dx;
    s32 dz;
    s32 row;
    s32 col;
    s16 idx;

    if (D_800E5814 != 0 || D_800E5810 == NULL) {
        return;
    }
    if (WmAddMutexPriority(0) == 0) {
        return;
    }
    bestPrev = NULL;
    best = NULL;
    bestDist = 0x7FFF;
    node = D_800E5810;
    prev = NULL;
    while (node != NULL) {
        idx = node->unk4;
        row = idx / 4;
        col = idx - row * 4;
        dx = col - arg0;
        if (dx <= 0) {
            dx = arg0 - col;
        }
        if (dx >= 0x13) {
            dx = 0x24 - dx;
        }
        dz = row - arg1;
        if (dz <= 0) {
            dz = arg1 - row;
        }
        if (dz >= 0xF) {
            dz = 0x1C - dz;
        }
        dist = dx + dz;
        if (dist < bestDist) {
            bestDist = dist;
            best = node;
            bestPrev = prev;
        }
        prev = node;
        node = node->next;
    }
    if (best == NULL) {
        func_800A0B40(0x22);
    }
    FreeListNode(best, bestPrev);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmStartLoadingMapFileBlock);

static void UpdateRegionLoad(void) {
    WorldListNode* node;
    WorldListNode* prev;
    WorldListNode* head;
    s32 tick;

    if (D_800E5814 >= D_8011650C * 5) {
        SystemCdromReadChain();
    }
    if (D_800E5814 < D_8011650C * 5 && D_800E5820 >= 0) {
        node = D_800E5768;
        prev = NULL;
        while (node != NULL) {
            if (node == &D_800E5718[D_800E5820]) {
                break;
            }
            prev = node;
            node = node->next;
        }
        if (node == NULL) {
            func_800A0B40(0xB);
        }
        if (prev != NULL) {
            prev->next = node->next;
        } else {
            D_800E5768 = node->next;
        }
        head = D_800E5764;
        D_800E5764 = node;
        node->next = head;
        RegisterChunksForNode(node);
        D_800E5820 = -1;
    }
    tick = 0;
    if (D_800E5814 != 0) {
        tick = D_800E5814 < D_8011650C * 5;
    }
    D_800E5814 -= tick;
}

static void WmMapLoadFinishedCallback(void) {
    if (D_800E5820 >= 0) {
        D_800E5814 = (D_8011650C * 5) - 1;
        WmRemoveMutexPriority(0);
    }
}

static s16 WmGetNumberOfMapToLoad(void) {
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

static void InitChunkNodePool(void) {
    s16 i;

    for (i = 0; i < 0x3F; i++) {
        D_800E582C[i].next = &D_800E582C[i + 1];
    }
    D_800E582C[i].next = NULL;
    D_800E5A30 = D_800E582C;
    D_800E5A2C = 0;
    D_800E5A34 = 0;
}

static WorldListNode* WmGetElementWithBlockIdAndSetItFirst(s16 arg0) {
    WorldListNode* node;
    WorldListNode* prev;
    WorldListNode* head;
    WorldListNode* next;

    node = D_800E5764;
    prev = NULL;
    while (node != NULL) {
        if (node->unk4 == arg0) {
            break;
        }
        prev = node;
        node = node->next;
    }
    if (node != NULL && prev != NULL) {
        next = node->next;
        head = D_800E5764;
        D_800E5764 = node;
        prev->next = next;
        node->next = head;
    }
    return node;
}

static s32 IsRegionLoading(s16 arg0) {
    WorldListNode* node;

    node = D_800E5768;
    while (node != NULL) {
        if (node->unk4 == arg0) {
            break;
        }
        node = node->next;
    }
    return node != NULL;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5AD8);

static void RegisterChunksForNode(WorldListNode* arg0) {
    WorldChunkHeader* c;

    c = D_80109D40;
    while (c != NULL) {
        if ((((c->z >> 2) * 9) + (c->x >> 2)) == arg0->unk4) {
            func_800A5AD8(c);
        }
        c = c->next;
    }
}

void WmLzsDecompress(s32* file, u8* dst) {
    u8* in;
    s32 count;
    s32 flags;
    u8* start;
    u8* end;
    s32 back;
    s32 length;
    s32 pos;
    u8* copy;
    u8* copyEnd;

    in = (u8*)file;
    count = 0;
    start = dst;
    end = in + *file + 3;
    in += 4;
    while (1) {
        if (count == 0) {
            count = 8;
            if (in >= end) {
                return;
            }
            flags = *in++;
        }
        if (flags & 1) {
            if (in >= end) {
                return;
            }
            *dst++ = *in++;
        } else {
            if (in >= end) {
                return;
            }
            back = *in++;
            length = *in++;
            back |= (length & 0xF0) << 4;
            pos = dst - start + 0xFEE;
            copy = dst - ((pos - back) & 0xFFF);
            copyEnd = dst + (length & 0xF) + 3;
            while (copy < start) {
                *dst++ = 0;
                copy++;
            }
            while (dst < copyEnd) {
                *dst++ = *copy++;
            }
        }
        flags >>= 1;
        count--;
    }
}

static void WmLoadChunkGeometry(WorldChunkHeader* chunk, WorldListNode* node) {
    s32* file;
    u8* buf;
    s16 numTris;

    file = (s32*)(((node - D_800E5718) * 0xB800) + D_800E5714);
    buf = D_800E5A38[chunk - D_80109A38];
    WmLzsDecompress((s32*)((s32)file + (((u32)file[((chunk->z & 3) * 4) | (chunk->x & 3)] >> 2) * 4)), buf);
    chunk->tris = (WorldTriangle*)(buf + 4);
    numTris = *(u16*)buf;
    chunk->numTris = numTris;
    chunk->verts = (SVECTOR*)((s32)chunk->tris + (numTris * 12));
    chunk->norms = (SVECTOR*)((s32)chunk->verts + (*(s16*)(buf + 2) * 8));
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A5E28);

static s32 ExpireChunks(void) {
    WorldChunkHeader* chunk;
    WorldChunkHeader* prev;
    WorldChunkNode* node;
    WorldChunkNode* nodePrev;
    WorldChunkHeader* freeChunk;
    WorldChunkNode* freeNode;
    s32 live;
    u16 age;

    live = 0;
    chunk = D_80109D40;
    prev = NULL;
    while (chunk != NULL) {
        age = chunk->numVerts;
        chunk->numVerts = age + 1;
        if ((s16)age >= 0x96) {
            if (prev != NULL) {
                prev->next = chunk->next;
            } else {
                D_80109D40 = chunk->next;
            }
            node = D_800E5A2C;
            freeChunk = D_80109D38;
            nodePrev = NULL;
            D_80109D38 = chunk;
            chunk->next = freeChunk;
            while (node != NULL) {
                if (node->chunk == chunk) {
                    break;
                }
                nodePrev = node;
                node = node->next;
            }
            if (node != NULL) {
                if (nodePrev != NULL) {
                    nodePrev->next = node->next;
                } else {
                    D_800E5A2C = node->next;
                }
                freeNode = D_800E5A30;
                D_800E5A30 = node;
                node->next = freeNode;
            }
            if (prev != NULL) {
                chunk = prev->next;
            } else {
                chunk = D_80109D40;
            }
        } else {
            prev = chunk;
            chunk = chunk->next;
            live++;
        }
    }
    return live;
}

static void InitChunkPool(void) {
    s16 i;

    for (i = 0; i < 0x1F; i++) {
        D_80109A38[i].next = &D_80109A38[i + 1];
    }
    D_80109A38[i].next = NULL;
    D_80109D38 = D_80109A38;
    D_80109D40 = NULL;
    D_80109D3C = 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6168);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A63FC);

void WmRequestVisibleChunks(VECTOR* pos, s16 angle) {
    s32 visible[5][5];
    s16 chunkX;
    s16 chunkZ;
    s16 x;
    s16 z;
    WorldChunkHeader* chunk;
    WorldListNode* node;
    s16 cx;
    s16 cz;
    s16 blockId;

    if (ExpireChunks() < 16) {
        WmExtractLoopCoordsTopBottomParts(pos, NULL, &chunkX, &chunkZ);
        func_800A6168(pos, angle, visible);
        for (chunk = D_80109D3C; chunk != NULL; chunk = chunk->next) {
            if (func_800A63FC(chunk, chunkX, chunkZ, &x, &z) != 0) {
                visible[z + 2][x + 2] = 0;
            }
        }
        for (chunk = D_80109D40; chunk != NULL; chunk = chunk->next) {
            if (func_800A63FC(chunk, chunkX, chunkZ, &x, &z) != 0) {
                visible[z + 2][x + 2] = 0;
            }
        }
        for (z = 0; z < 5; z++) {
            for (x = 0; x < 5; x++) {
                if (visible[z][x] != 0) {
                    cx = (x + (u16)chunkX) - 2;
                    if (cx < 0) {
                        cx = (x + (u16)chunkX) + 34;
                    } else if (cx >= 36) {
                        cx = (x + (u16)chunkX) - 38;
                    }
                    cz = ((u16)z + (u16)chunkZ) - 2;
                    if (cz < 0) {
                        cz = ((u16)z + (u16)chunkZ) + 26;
                    } else if (cz >= 28) {
                        cz = ((u16)z + (u16)chunkZ) - 30;
                    }
                    chunk = AllocChunk();
                    if (chunk != NULL) {
                        blockId = ((cz >> 2) * 9) + (cx >> 2);
                        chunk->z = cz;
                        chunk->x = cx;
                        if (WmGetElementWithBlockIdAndSetItFirst(blockId) != 0) {
                            func_800A5AD8(chunk);
                        } else if (IsRegionLoading(blockId) == 0) {
                            if (D_800E580C == NULL) {
                                func_800A0B40(15);
                            }
                            node = D_800E580C;
                            node->unk4 = blockId;
                            D_800E580C = node->next;
                            WmAssignRegionToNode(node);
                        }
                    }
                }
            }
        }
    }
}

static WorldChunkHeader* AllocChunk(void) {
    WorldChunkHeader* chunk;
    WorldChunkHeader* prev;
    WorldChunkHeader* head;

    chunk = NULL;
    if (D_80109D38 != NULL) {
        chunk = D_80109D38;
        D_80109D38 = chunk->next;
    } else if (D_80109D3C != NULL) {
        chunk = D_80109D3C;
        prev = NULL;
        while (chunk->next != NULL) {
            prev = chunk;
            chunk = chunk->next;
        }
        if (prev != NULL) {
            prev->next = NULL;
        } else {
            D_80109D3C = NULL;
        }
        func_800A9064(chunk->x, chunk->z);
    }
    if (chunk != NULL) {
        head = D_80109D40;
        D_80109D40 = chunk;
        chunk->numVerts = 0;
        chunk->next = head;
    }
    return chunk;
}

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

static void func_800A6B8C(VECTOR* arg0) {
    if (arg0 != NULL) {
        *arg0 = D_80109D44;
    }
}

static void func_800A6BCC(SVECTOR* arg0) {
    if (arg0 != NULL) {
        WmExtractLoopCoordsTopBottomParts(&D_80109D44, arg0, NULL, NULL);
    }
}

static void func_800A6C00(SVECTOR* arg0) {
    if (func_800A1DB0() != 2)
        WmSetTranslationVectorInScreenSpace(arg0);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A6C3C);

void WmSetMapPosAndNeighbors(WorldMapPos* out, SVECTOR* dir, SVECTOR* rot, s32 dist) {
    MATRIX m;
    VECTOR pos;

    RotMatrix(rot, &m);
    SetRotMatrix(&m);
    gte_ldv0(dir);
    gte_rt();
    gte_stlvnl(&pos);

    out[0].pos = pos;
    WmExtractLoopCoordsTopBottomParts(&out[0].pos, &out[0].offset, &out[0].chunkX, &out[0].chunkZ);
    out[0].unk24 = 1;
    out[0].unk28 = 0;
    out[0].unk1C = 0;
    out[0].rotY = rot->vy;

    out[1].pos = pos;
    out[1].pos.vx -= dist;
    WmExtractLoopCoordsTopBottomParts(&out[1].pos, &out[1].offset, &out[1].chunkX, &out[1].chunkZ);
    out[1].unk24 = 1;
    out[1].unk28 = 0;
    out[1].unk1C = 0;
    out[1].rotY = rot->vy;

    out[2].pos = pos;
    out[2].pos.vx += dist;
    WmExtractLoopCoordsTopBottomParts(&out[2].pos, &out[2].offset, &out[2].chunkX, &out[2].chunkZ);
    out[2].unk24 = 1;
    out[2].unk28 = 0;
    out[2].unk1C = 0;
    out[2].rotY = rot->vy;

    out[3].pos = pos;
    out[3].pos.vz -= dist;
    WmExtractLoopCoordsTopBottomParts(&out[3].pos, &out[3].offset, &out[3].chunkX, &out[3].chunkZ);
    out[3].unk24 = 1;
    out[3].unk28 = 0;
    out[3].unk1C = 0;
    out[3].rotY = rot->vy;

    out[4].pos = pos;
    out[4].pos.vz += dist;
    WmExtractLoopCoordsTopBottomParts(&out[4].pos, &out[4].offset, &out[4].chunkX, &out[4].chunkZ);
    out[4].unk24 = 1;
    out[4].unk28 = 0;
    out[4].unk1C = 0;
    out[4].rotY = rot->vy;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A71E8);

static s32 func_800A7E7C(void) { return D_80109D6C; }

static void func_800A7E8C(s32 arg0) { D_80109D6C = arg0; }

static void func_800A7E9C(void) {}

static void InitWorldState(void) {
    WmInitLoadMapFileStruct();
    InitChunkNodePool();
    InitChunkPool();
    D_80109D54 = 0;
    D_80109D58 = 0;
    D_80109D5C = NULL;
    D_80109D60 = NULL;
    D_800E5828 = 1;
    D_80109D64 = 0;
    D_80109D68 = 0;
    D_80109D6C = 0;
}

static void WmAbortMapLoadingWrapper(void) { WmAbortMapLoading(); }

static void WmAbortMapLoading(void) {
    WorldListNode* node;
    WorldListNode* prev;
    WorldListNode* head;

    D_800E5828 = 0;
    if (D_800E5820 >= 0) {
        if (D_800E5814 >= D_8011650C * 5) {
            prev = NULL;
            SystemCdromAbortLoading();
            node = D_800E5768;
            while (node != NULL) {
                if (node == &D_800E5718[D_800E5820]) {
                    break;
                }
                prev = node;
                node = node->next;
            }
            if (node == NULL) {
                func_800A0B40(0xD);
            }
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                D_800E5768 = node->next;
            }
            head = D_800E5760;
            D_800E5760 = node;
            D_800E5814 = 0;
            D_800E5820 = -1;
            node->next = head;
        }
    }
}

static void func_800A8048(void) { D_800E5828 = 1; }

static s32 func_800A805C(void) { return D_800E5828; }

void WmUpdateStreamingAndCamera(s16 angle, s32 arg1) {
    VECTOR pos;
    VECTOR newPos;
    VECTOR pos2;
    SVECTOR offset;
    VECTOR target;
    s16 mapX;
    s16 mapZ;
    s32 prev;

    WmSetPcEntityAsActiveEntity();
    WmGetPosFromPcEntity(&pos);
    WmGetPos2FromPcEntity(&pos2);
    if (D_800E5828 != 0) {
        offset.vx = offset.vy = 0;
        offset.vz = -8192;
        WmRotateVectorByYAngle(&offset, -angle);
        target.vx = offset.vx + pos.vx;
        target.vy = pos.vy;
        target.vz = offset.vz + pos.vz;
        if (target.vx < 0) {
            target.vx += 0x48000;
        } else if (target.vx > 0x47FFF) {
            target.vx -= 0x48000;
        }
        if (target.vz < 0) {
            target.vz += 0x38000;
        } else if (target.vz > 0x37FFF) {
            target.vz -= 0x38000;
        }
        WmQueueBlocksAroundPos(&target);
        WmRequestVisibleChunks(&pos, angle);
        WmExtractLoopCoordsTopBottomParts(&target, NULL, &mapX, &mapZ);
        WmLoadClosestMapFileBlock(mapX, mapZ);
        UpdateRegionLoad();
        func_800A5E28();
        prev = D_800E5818;
        D_800E5818 = (WmGetNumberOfMapToLoad() >= 3) ? D_800E5818 + 1 : 0;
        if (prev < 300) {
            if (D_800E5818 >= 300) {
                func_800A41E8(1);
            }
        } else if (D_800E5818 == 0) {
            func_800A41E8(4);
        }
    }
    if (func_800BC1AC() == 2) {
        WmMovePcEntityByDistance(0);
    } else if (D_80109D64 == 1) {
        WmMovePcEntityByDistance(D_80109D68);
    }
    WmGetPosFromPcEntity(&newPos);
    D_800BD134 = 5500;
    D_800BD138 = (WmGetWmId() == 2) ? 16000 : 0;
    if (arg1 != 0) {
        func_800A71E8(&pos2, &pos, &newPos, angle);
    }
    if (func_800A31E8() != 0) {
        func_800A7E9C();
    }
}

static void func_800A82DC(void) { D_80109D54 = 1; }

static s32 func_800A82F0(void) { return D_80109D58; }

static s32 IsChunkLoaded(s16 arg0, s16 arg1) {
    WorldChunkHeader* chunk;

    chunk = D_80109D3C;
    while (chunk != NULL) {
        if (chunk->x == arg0 && chunk->z == arg1) {
            break;
        }
        chunk = chunk->next;
    }
    return chunk != NULL;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A835C);

s32 WmReserveMapBuffer(s16 slot) {
    WorldListNode* node;
    WorldListNode* last;
    WorldListNode* prev;

    if (D_800E5828 != 0) {
        func_800A0B40(14);
    }
    last = NULL;
    for (node = D_800E5810; node != NULL; node = node->next) {
        last = node;
    }
    prev = NULL;
    if (last != NULL) {
        last->next = D_800E580C;
        D_800E580C = D_800E5810;
        D_800E5810 = NULL;
    }
    for (last = D_800E5768; last != NULL; last = last->next) {
        prev = last;
    }
    if (prev != NULL) {
        prev->next = D_800E5760;
        D_800E5760 = D_800E5768;
        D_800E5768 = NULL;
    }
    prev = NULL;
    last = D_800E5764;
    while (last != NULL) {
        if (last >= &D_800E5718[slot]) {
            if (prev != NULL) {
                prev->next = last->next;
            } else {
                D_800E5764 = last->next;
            }
            last->next = D_800E5760;
            D_800E5760 = last;
            if (prev != NULL) {
                last = prev->next;
            } else {
                last = D_800E5764;
            }
        } else {
            prev = last;
            last = last->next;
        }
    }
    return (slot * 0xB800) + D_800E5714;
}

static void func_800A886C(s32 arg0) {
    D_80109D64 = 2;
    D_80109D68 = arg0;
}

static void WmSetWorldProgress(s32 arg0) { D_800E5824 = arg0; }

static void func_800A8898(VECTOR* out) {
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

static WorldActor* WmInsertInEntityStructList(void) {
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

static void func_800A8A88(void) {
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

static void WmUnlinkEntityAndInsertAsNew(WorldActor* arg0) {
    WorldActor* temp_v0;

    WmUnlinkEntityFromAll(arg0);
    temp_v0 = D_8010AD34;
    D_8010AD34 = arg0;
    arg0->next = temp_v0;
}

void WmUnlinkEntityFromAll(WorldActor* actor) {
    WorldActor* it;
    WorldActor* prev;
    FieldModelEntry* model;

    if (actor == D_8010AD3C) {
        D_8010AD3C = NULL;
    }
    if (actor == D_8010AD40) {
        D_8010AD40 = NULL;
    }
    for (it = D_8010AD38; it != NULL; it = it->next) {
        if (it->collide == actor) {
            it->collide = NULL;
        }
        if (it->riding == actor) {
            it->riding = NULL;
        }
    }
    prev = NULL;
    for (it = D_8010AD38; it != NULL; it = it->next) {
        if (it == actor) {
            break;
        }
        prev = it;
    }
    if (it != NULL) {
        if (prev != NULL) {
            prev->next = it->next;
        } else {
            D_8010AD38 = it->next;
        }
    }
    if (it != NULL) {
        it->next = NULL;
    }
    model = WmGetModelDataByModelId(actor->actorType);
    if (model != NULL && (s8)model->kawaiType == 12) {
        WmSetCalculateAllPartsLighting(model);
        actor->unk5E = -1;
    }
}

void WmInsertStructInEntityStructList(WorldActor* arg0) {
    WorldActor* temp_v0;

    if ((arg0 != NULL) && (arg0->next == NULL)) {
        temp_v0 = D_8010AD38;
        D_8010AD38 = arg0;
        arg0->next = temp_v0;
    }
}

static s32 func_800A8CA4(void) {
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

static void func_800A8D58(void) {
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

static void func_800A8E50(void) {
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

static s32 func_800A8F48(void) { return D_8010AD3C == NULL ? 0 : D_8010AD3C->riding != NULL; }

void WmUnlinkPcEntityFromAll(void) {
    if (D_8010AD40)
        WmUnlinkEntityAndInsertAsNew(D_8010AD40);
}

static void WmUnlinkActiveEntityFromAll(void) {
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

static void func_800A9018(void) {
    WorldActor* temp_a0;

    if (D_8010AD3C == NULL)
        return;
    temp_a0 = D_8010AD3C->riding;
    if (temp_a0 != NULL) {
        WmUnlinkEntityAndInsertAsNew(temp_a0);
        D_8010AD3C->riding = NULL;
    }
}

static void func_800A9064(s16 x, s16 z) {
    WorldActor* a;
    WorldStoredTriangle* tri;

    for (a = D_8010AD38; a != NULL; a = a->next)
        for (tri = &a->storedTris[0]; tri < &a->storedTris[6]; tri++)
            if ((tri->x == x) && (tri->z == z))
                tri->tri = NULL;
}

static void WmSetPcEntityAsActiveEntity(void) {
    if (D_8010AD40 != NULL)
        D_8010AD3C = D_8010AD40;
}

void WmSetActiveEntityAsPcEntity(void) {
    if (D_8010AD3C != NULL)
        D_8010AD40 = D_8010AD3C;
}

static WorldStoredTriangle* func_800A9134(void) { return D_8010AD40 != NULL ? D_8010AD40->storedTris : NULL; }

s32 WmGetModelIdFromActiveEntity(void) { return D_8010AD3C != NULL ? D_8010AD3C->actorType : NULL; }

s32 WmGetModelIdFromPcEntity(void) { return D_8010AD40 != NULL ? D_8010AD40->actorType : NULL; }

static WorldActor* WmGetActiveEntity(s32 modelId) { return D_8010AD3C; }

static s32 WmIsPcEntityModelInMask(s32 arg0) {
    return D_8010AD40 != NULL && D_8010AD40->actorType < 0x20 ? (arg0 >> D_8010AD40->actorType) & 1 : 0;
}

static s32 func_800A91E0(s32 arg0) {
    return D_8010AD3C != NULL && D_8010AD3C->actorType < 0x20 ? (arg0 >> D_8010AD3C->actorType) & 1 : 0;
}

static s32 func_800A921C(s32 arg0, u8 arg1) { return arg1 >= 0x20 ? 0 : (arg0 >> arg1) & 1; }

static s32 func_800A9240(void) {
    s32 out;

    if (D_8010AD40 != NULL) {
        out = 0;
        if (D_8010AD40->actorType == 4 || D_8010AD40->actorType == 0x13 || D_8010AD40->actorType == 0x29 ||
            D_8010AD40->actorType == 0x2A)
            out = 1;
    } else
        out = 0;
    return out;
}

s32 func_800A929C(void) {
    s32 out;

    if (D_8010AD3C != NULL) {
        out = 0;
        if (D_8010AD3C->actorType == 4 || D_8010AD3C->actorType == 0x13 || D_8010AD3C->actorType == 0x29 ||
            D_8010AD3C->actorType == 0x2A)
            out = 1;
    } else
        out = 0;
    return out;
}

static s32 func_800A92F8(u8 actorType) {
    return actorType == 4 || actorType == 0x13 || actorType == 0x29 || actorType == 0x2A;
}

static const s32 D_800A01D8[] = {0, 0xF000};
static const s32 D_800A01E0[] = {0, 0};
static const s32 D_800A01E8[] = {0, 0, 0, 0};

// TODO: this -> 800b624c, 800b58f8, 800ada64
static void WmInitActiveEntityStruct(s32 arg0) {
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
                D_8010AD3C->riding = &D_80109D74[1];

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
            WmApplyModelLightingById(4, 0);
            /* fallthrough */
        default:
            rect.x = 0x18;
            rect.y = 0x38;
            rect.w = 0xF;
            rect.h = 0xF;
            D_8010AD3C->unk58 = 0x20;
        }
        WmCreateShadowPacket(D_8010AD3C->prims, &rect);
        WmRestoreEntityPosAndDirFromSavemap(D_8010AD3C);
    }
}

static void WmSetActiveEntityDirectionAndRot(s16 arg0) {
    if (D_8010AD3C != NULL) {
        D_8010AD3C->direction = arg0;
        D_8010AD3C->facing = (s16)arg0;
        D_8010AD3C->unk3E = 0;
    }
}

static void func_800A94A8(u16 arg0) {
    if (D_8010AD40 != NULL) {
        D_8010AD40->direction = arg0;
        D_8010AD40->facing = (s16)arg0;
        D_8010AD40->unk3E = 0;
    }
}

static void func_800A94D0(s16 arg0) {
    if (D_8010AD3C != NULL) {
        D_8010AD3C->direction = arg0;
        D_8010AD3C->facing = (s16)arg0;
    }
}

static void func_800A94F4(s16 arg0) {
    if (D_8010AD3C != NULL) {
        D_8010AD3C->direction = arg0;
        D_8010AD3C->unk3C = arg0;
        D_8010AD3C->facing = (s16)arg0;
        D_8010AD3C->unk3E = 0;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800A9520);

static void func_800A9678(s16 arg0) { func_800A9520(D_8010AD3C, arg0); }

static void func_800A96A4(s16 arg0) { func_800A9520(D_8010AD40, arg0); }

static void BlendActorFacing(s16 arg0) {
    WorldActor* actor;
    u32 blend;

    if (D_8010AD3C == NULL) {
        return;
    }
    actor = D_8010AD3C;
    if (D_8011650C == 1) {
        if (actor->flags1 & 1) {
            blend = (u32)(((s16)actor->unk3E * 15) + arg0) >> 4;
        } else {
            blend = (u32)(((s16)actor->unk3E * 3) + arg0) >> 2;
        }
    } else if (actor->flags1 & 1) {
        blend = (u32)(((s16)actor->unk3E * 7) + arg0) >> 3;
    } else {
        blend = (u32)((s16)actor->unk3E + arg0) >> 1;
    }
    actor->unk3E = blend;
}

s16 func_800A97A8(void) { return D_8010AD3C == NULL ? 0 : D_8010AD3C->unk3C + D_8010AD3C->unk3E; }

static s16 WmGetPcEntityTotalRot(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->unk3C + D_8010AD40->unk3E; }

static void func_800A9820(s32 arg0) {
    if (D_8010AD3C != NULL)
        D_8010AD3C->pos.vy += arg0;
}

static s32 func_800A984C(void) { return D_8010AD3C == NULL ? 0 : D_8010AD3C->flags1 & 1; }

static s32 func_800A9878(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->flags1 & 1; }

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

static s32 WmGetPcEntityOriginalY(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->unk42; }

static s32 WmSetActiveEntityWithModelId(s32 arg0) {
    WorldActor* var_v1;

    for (var_v1 = D_8010AD38; var_v1 != NULL && var_v1->actorType != arg0; var_v1 = var_v1->next)
        ;
    if (var_v1 != NULL)
        D_8010AD3C = var_v1;
    return var_v1 != NULL;
}

static void func_800A9988(void) {
    if (D_8010AD40 != NULL && D_8010AD40->collide != NULL)
        D_8010AD3C = D_8010AD40->collide;
}

s32 func_800A99BC(void) {
    return D_8010AD40 != NULL && D_8010AD3C != NULL && D_8010AD40 != D_8010AD3C && !(D_8010AD3C->flags1 & 0x10);
}

static void func_800A9A04(s8 actorType) {
    if (D_8010AD40)
        D_8010AD40->actorType = actorType;
}

static void WmSetPcEntityTerrainData(s16 walkmesh) {
    if (D_8010AD40)
        D_8010AD40->walkmesh = walkmesh;
}

static s32 WmGetPcEntityTerrainId(void) { return D_8010AD40 == NULL ? 0 : D_8010AD40->walkmesh & 0x1F; }

static s32 func_800A9A70(void) { return D_8010AD40 == NULL ? 0 : (D_8010AD40->walkmesh >> 9) & 0x1F; }

static u32 func_800A9AA4(void) { return D_8010AD40 == NULL ? 0 : (u16)D_8010AD40->walkmesh >> 0xF; }

static s32 func_800A9AD0(void) { return D_8010AD40 == NULL ? 0 : (D_8010AD40->walkmesh >> 5) & 7; }

static s32 WmGetModelYOffset(s32 walkmesh, s32 actorType) {
    s32 modelId = actorType & 0xFF;
    s32 terrainId = walkmesh & 0x1F;
    s32 target;

    switch (modelId) {
    case 8:
        return -15;
    case 13:
        return -240;
    case 19:
        if ((terrainId >= 3 && terrainId <= 6) || terrainId == 26) {
            if (D_8010AD40->flags1 & 1) {
                if (D_8010AD64 >= 0) {
                    return D_8010AD64;
                }
                D_8010AD64 += 20;
                if (D_8010AD64 > 0) {
                    D_8010AD64 = 0;
                }
                return D_8010AD64;
            }
            target = (terrainId == 4 || terrainId == 6) ? -50 : -250;
            if (target < D_8010AD64) {
                D_8010AD64 -= 10;
            } else if (D_8010AD64 < target - 20) {
                D_8010AD64 = target;
            }
            return D_8010AD64;
        }
        D_8010AD64 = 0;
        break;
    case 25:
        return 0;
    }
    if (terrainId != 1 && terrainId != 25) {
        if (terrainId == 4 || terrainId == 7) {
            return -64;
        }
        return 0;
    }
    return -192;
}

static void func_800A9C64(WorldActor* arg0, VECTOR* arg1) {
    u8 var_v0;

    if (arg1 != NULL && arg0 != NULL) {
        WmLoopCoordsAroundWorld(arg1);
        if (arg0->flags1 & 0x80) {
            arg0->pos.vx = arg1->vx;
            arg0->pos.vz = arg1->vz;
        } else {
            arg0->pos = *arg1;
            arg0->pos.vy += WmGetModelYOffset(arg0->walkmesh, arg0->actorType);
        }
        arg0->unk42 = arg1->vy;
        arg0->flags1 =
            arg0->pos.vx != arg0->altPos.vx || arg0->pos.vz != arg0->altPos.vz ? arg0->flags1 | 1 : arg0->flags1 & 0xFE;
    }
}

static void func_800A9D5C(VECTOR* arg0) { func_800A9C64(D_8010AD3C, arg0); }

static void func_800A9D88(VECTOR* arg0) { func_800A9C64(D_8010AD40, arg0); }

static void func_800A9DB4(VECTOR* arg0) {
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

static void WmGetPosFromActiveEntity(VECTOR* arg0) {
    if (arg0 != NULL && D_8010AD3C != NULL)
        *arg0 = D_8010AD3C->pos;
}

static void WmGetPosFromPcEntity(VECTOR* arg0) {
    if (arg0 != NULL && D_8010AD40 != NULL)
        *arg0 = D_8010AD40->pos;
}

static void WmGetPos2FromActiveEntity(VECTOR* arg0) {
    if (arg0 != NULL && D_8010AD3C != NULL)
        *arg0 = D_8010AD3C->altPos;
}

static void WmGetPos2FromPcEntity(VECTOR* arg0) {
    if (arg0 != NULL && D_8010AD40 != NULL)
        *arg0 = D_8010AD40->altPos;
}

static void func_800AA1B8(void) {
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

static void func_800AA238(void) {
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

static void func_800AA2B8(void) {
    if (D_8010AD40 != NULL)
        D_8010AD40->flags1 &= 0xFD;
}

static void func_800AA2E4(s8 arg0) {
    if (D_8010AD3C)
        D_8010AD3C->animId = arg0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AA304);

static WorldActor* FindCollidingActor(WorldActor* arg0) {
    WorldActor* hit;
    s32 rc;

    rc = 0;
    hit = NULL;
    if (D_8010AD3C != NULL) {
        if (D_8010AD3C->actorType != 0xD || WmGetWmId() != 0) {
            hit = D_8010AD38;
            while (hit != NULL) {
                rc = func_800AA304(D_8010AD3C, hit);
                if (rc != 0) {
                    break;
                }
                hit = hit->next;
            }
            if (hit != NULL) {
                D_8010AD3C->collide = hit;
            }
        }
    }
    return rc < 2 ? hit : NULL;
}

static WorldActor* func_800AA640(void) {
    WorldActor* actor;

    actor = FindCollidingActor(D_8010AD3C);
    if (actor)
        func_800AA1B8();
    return actor;
}

static WorldActor* func_800AA684(void) { return D_8010AD3C != NULL ? D_8010AD3C->collide : NULL; }

static void func_800AA6A4(void) {
    if (D_8010AD40 != NULL)
        D_8010AD40->flags1 |= 2;
}

static void func_800AA6D0(WorldChunkHeader* arg0) {
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
                    var_s0->pos.vy = var_s0->unk42 + WmGetModelYOffset(var_s0->walkmesh, var_s0->actorType);
                }
                var_s0->flags1 |= 2;
            }
        }
    }

    UpdateZolomGroundHeight(arg0);
    func_800B1C80(arg0);
}

static void WmMoveActiveEntity(s32 arg0, s32 arg1) {
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

static void func_800AA8D8(s16 arg0, s16 arg1, s16 arg2) {
    D_8010AD44 = arg0;
    D_8010AD48 = arg1;
    D_8010AD4C = arg2;
}

static const SVECTOR D_800A0260;

s32 WmGetHorizonCurveDrop(s32 x, s32 z) {
    SVECTOR pos;
    s32 otz;
    s32 sxy;
    s16 sx;

    pos = D_800A0260;
    WmSetTranslationVectorInScreenSpace(&pos);
    pos.vx = x;
    pos.vy = 0;
    pos.vz = z;
    gte_ldv0(&pos);
    gte_rtps();
    gte_stszotz(&otz);
    gte_stsxy2(&sxy);
    sx = sxy;
    otz -= func_800A32F4();
    otz = otz > 0 ? ((otz * otz) >> 12) + ((sx * sx) >> 10) : 0;
    return otz >> 2;
}

static s32 WmGetBuggyMoveAnimationId(u8* arg0) {
    s32 prev;
    s32 event;

    prev = D_80109D70;
    event = WmGetPcEntityTerrainId();
    switch (D_80109D70) {
    case 1:
        if (event == 0xB) {
            D_80109D70 = 2;
        }
        break;
    case 2:
        if (*arg0 >= 6) {
            D_80109D70 = 3;
        }
        break;
    case 3:
        if (event != 4 && event != 0xB) {
            D_80109D70 = 4;
        }
        break;
    case 4:
        if (*arg0 >= 6) {
            D_80109D70 = 1;
        }
        break;
    default:
        D_80109D70 = 1;
        break;
    }
    if (D_80109D70 != prev) {
        *arg0 = 0;
    }
    return D_80109D70;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AAB18);

static void func_800AB36C(SVECTOR* arg0) {
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

static void WmPrepareEntities(void) {
    WorldActor* var_s0;

    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        func_800AB48C(var_s0);
    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        var_s0->flags1 &= 0xF8;
}

static void func_800AB570(void) {
    WorldActor* var_s0;

    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        func_800AAB18(var_s0);
    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        func_800AB398(var_s0);
}

void WmScriptInitVariables(WorldScriptData* data) {
    s32 i;

    D_8010AD68 = data;
    D_8010AD6C = data->scr;
    D_8010AD90 = D_8010AD70;
    D_8010AD94[0] = Savemap.memory_bank_1;
    D_8010AD94[1] = D_8010AD94[2] = D_8010AD94[3] = D_8010ADA4;
    D_8010ADE4 = NULL;
    D_8010ADF0 = 0;
    D_8010ADEC = 0;
    D_8010ADE8 = 0;
    for (i = 0; i < 64; i++) {
        D_8010ADA4[i] = 0;
    }
    for (i = 0; i < 3; i++) {
        D_8010ADF4[i].vx = D_8010ADF4[i].vy = D_8010ADF4[i].vz = 0;
    }
    D_8010AE24 = D_8010AE28 = 0;
    D_8010AE2C = D_8010AE30 = 0;
    D_8010AE34.vx = D_8010AE34.vy = D_8010AE34.vz = 0;
}

// pushes execution of given script to player's execution stack
static void WmScriptRunFunction(s32 arg0, s32 arg1) {
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

static void func_800AB92C(s32 arg0) {
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

static void func_800AB988(s32 arg0, s32 arg1) {
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

static void func_800ABA78(s16 arg0, s16 arg1) {
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

static const SVECTOR D_800A0260 = {0, 0, 0, 0};

static s32 WmScriptPopStack(void) {
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
            var_s0 = WmDialogGetAskResult();
            break;
        case 13: // Script index of current mesh triangle of active actor
            var_s0 = ((D_8010AD3C->walkmesh) >> 5) & 7;
            break;
        case 14: // Player party member model ID (0=Cloud, 1=Tifa, 2=Cid)
            var_s0 = WmGetPcCharModelIdFromParty();
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
static void WmScriptWriteBank(s32 arg0) {
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
        D_8010AD90->unk0 = WmGetDistanceToActivePoint(&sp10) >> 5;
        break;
    case 0x19: // "push distance from active entity to entity by model id"
        temp_s0_16 = D_8010AD3C;
        D_8010AD90->unk0 = 0;
        if (WmSetActiveEntityWithModelId(WmScriptPopStack()) != 0) {
            D_8010AD90->unk0 = WmGetDistanceBetweenPoints(&D_8010AD3C->pos, &temp_s0_16->pos) >> 4;
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
            temp_s0_17->unk0 = WmGetDistanceBetweenPoints(&sp10, &D_8010ADF4[temp_v0]) >> 4;
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

static void WmScriptRunAll(void) {
    WorldActor* var_s0;

    WmScriptRunOne(D_8010AD3C = D_8010ADE4 = D_80109D74);
    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        WmScriptRunOne(D_8010ADE4 = D_8010AD3C = var_s0);
}

static void UpdateSurfaceEffect(void) {
    s32 kind;

    if (D_8010ADEC != 0 || (D_8010AD40->flags1 & 8)) {
        return;
    }
    if (WmIsPcEntityModelInMask(0x2000) != 0) {
        if ((D_8010AD40->pos.vy - D_8010AD40->unk42) >= 0x1F4) {
            return;
        }
    } else if (D_8010AD40->flags1 & 0x80) {
        return;
    }
    kind = func_800A9AD0();
    if (kind >= 3) {
        if (D_8010ADF0 != kind) {
            D_8010ADF0 = kind;
            func_800ABA78(kind - 3, 0);
        }
        if (kind == 7 && WmIsPcEntityModelInMask(0x2000) == 0) {
            func_800AA238();
        }
    } else {
        D_8010ADF0 = 0;
    }
}

static s32 WmScriptIsAnyScriptRuns(void) {
    WorldActor* a;
    s32 flag;

    flag = D_80109D74[0].scriptIdx != 0;
    a = D_8010AD38;
    while (a != NULL && flag == 0) {
        flag |= a->scriptIdx != 0;
        a = a->next;
    }
    return flag;
}

void SaveActorState(WorldActor* actor) {
    u8 type;
    u8 slot;
    s32* out;

    type = actor->actorType;
    slot = D_800C6748[type];
    if (slot < 6) {
        out = (s32*)((slot * 8) + D_8010AD50);
        out[0] = (actor->altPos.vx & 0x7FFFF) | ((type << 19) & 0xF80000) | ((actor->direction << 20) & 0xFF000000);
        out[1] = (actor->altPos.vz & 0x3FFFF) | (actor->altPos.vy << 18);
    }
}

static void func_800ADA08(void) {
    WorldActor* var_s0;

    for (var_s0 = D_8010AD38; var_s0 != NULL; var_s0 = var_s0->next)
        if (!(var_s0->flags1 & 8))
            SaveActorState(var_s0);
}

static void WmRestoreEntityPosAndDirFromSavemap(WorldActor* arg0) {
    s32* p;
    s32* end;
    s32 t;
    s16 f;
    s32 type;

    p = (s32*)D_8010AD50;
    end = (s32*)(D_8010AD50 + 0x30);
    if (p >= end) {
        return;
    }
    type = arg0->actorType;
    while (p < end) {
        if (((*p >> 19) & 0x1F) == type) {
            break;
        }
        p += 2;
    }
    if (p < (s32*)(D_8010AD50 + 0x30) && arg0 != NULL) {
        t = p[0] & 0x7FFFF;
        arg0->altPos.vx = t;
        arg0->pos.vx = t;
        t = p[1] >> 18;
        arg0->altPos.vy = t;
        arg0->pos.vy = t;
        t = p[1] & 0x3FFFF;
        arg0->altPos.vz = t;
        arg0->pos.vz = t;
        f = (p[0] >> 20) & 0xFF0;
        arg0->unk3E = 0;
        arg0->direction = f;
        arg0->unk3C = f;
        arg0->facing = f;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800ADB30);

static void func_800ADC3C(VECTOR* arg0) { D_8010AE34 = *arg0; }

void func_800ADC70(void) { D_8010AE54 = 0; }

static s32 WmAddMutexPriority(s16 arg0) {
    if ((D_8010AE54 >> arg0) & 1) {
        if (!((-1 << (arg0 + 1)) & D_8010AE54)) {
            return 1;
        }
    }
    if (D_8010AE54 < (1 << arg0)) {
        if (D_8010AE54 == 1) {
            WmAbortMapLoading();
        } else if (D_8010AE54 & 4) {
            WmAbortModelLoading();
        }
        D_8010AE54 |= 1 << arg0;
        return 1;
    }
    return 0;
}

void WmRemoveMutexPriority(s16 arg0) {
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
static void WmRandomTwiddle(void) {
    s32 i;
    for (i = 0; i < 0x20; i++)
        D_8010AE5C[i] ^= D_8010AE5C[0x1E9 + i];
    for (i = 0x20; i < 0x209; i++)
        D_8010AE5C[i] ^= D_8010AE5C[i - 0x20];
}

// seed RNG
static void WmRandomInit(s32 arg0) {
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
    WmRandomTwiddle();
    WmRandomTwiddle();
    WmRandomTwiddle();
    D_8010AE58 = 0x208;
}

// World Map RNG rand(), returns u8
s32 func_800ADFC0(void) {
    D_8010AE58++;
    if (D_8010AE58 > 0x208) {
        WmRandomTwiddle();
        D_8010AE58 = 0;
    }
    return D_8010AE5C[D_8010AE58];
}

static s32 WmGetDistanceBetweenPoints(VECTOR* arg0, VECTOR* arg1) {
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

void WmRotateVectorByYAngle(SVECTOR* vec, s16 angle) {
    MATRIX m;
    SVECTOR rot;
    VECTOR trans;

    if (vec != NULL) {
        trans.vx = trans.vy = trans.vz = 0;
        rot.vx = rot.vz = 0;
        rot.vy = angle;
        RotMatrix(&rot, &m);
        SetRotMatrix(&m);
        TransMatrix(&m, &trans);
        SetTransMatrix(&m);
        gte_ldv0(vec);
        gte_rt();
        gte_stlvnl(&trans);
        vec->vx = trans.vx;
        vec->vy = trans.vy;
        vec->vz = trans.vz;
    }
}

s16 WmGetRotFromVector(s32 x, s32 z, s32 arg2) {
    s32 rot;
    s32 nx;
    s32 nz;

    rot = 0;
    if (x != 0 || z != 0) {
        if (z >= 0) {
            if (x >= 0) {
                if (x >= z) {
                    rot = 1024 - D_800BE1E8[(z << 8) / x];
                } else {
                    rot = D_800BE1E8[(x << 8) / z];
                }
            } else {
                nx = -x;
                if (z >= nx) {
                    rot = -D_800BE1E8[(nx << 8) / z];
                } else {
                    rot = D_800BE1E8[(z << 8) / nx] - 1024;
                }
            }
        } else if (x < 0) {
            nx = -x;
            nz = -z;
            if (nx >= nz) {
                rot = -D_800BE1E8[(nz << 8) / nx] - 1024;
            } else {
                rot = D_800BE1E8[(nx << 8) / nz] - 2048;
            }
        } else {
            nz = -z;
            if (nz >= x) {
                rot = 2048 - D_800BE1E8[(x << 8) / nz];
            } else {
                rot = D_800BE1E8[(nz << 8) / x] + 1024;
            }
        }
    }
    return rot;
}

static s16 WmGetRotFromEntityToEntity(VECTOR* arg0, VECTOR* arg1) {
    return WmGetRotFromVector(arg1->vx - arg0->vx, arg1->vz - arg0->vz, arg1->vx);
}

static void WmCreateSkyboxOverlayRenderBuffers(void) {
    CVECTOR* a;
    CVECTOR* b;
    POLY_G4* p;
    s32 i;

    i = 0;
    a = &D_800C6768;
    b = &D_800C676C;
    do {
        p = &D_800C6770[i];
        p->r0 = p->r1 = a->r;
        p->g0 = p->g1 = a->g;
        p->b0 = p->b1 = a->b;
        p->r2 = p->r3 = b->r;
        p->g2 = p->g3 = b->g;
        p->b2 = p->b3 = b->b;
        setlen(p, 8);
        setcode(p, 0x38);
        SetDrawMode((DR_MODE*)((i * 0xC) + (s32)D_8010B068), 0, 1, 0, NULL);
        i++;
    } while (i < 2);
    D_8010B080 = 0;
}

static void* WmGetSkyboxOverlayCurrRenderBuffer(void) { return &D_800C6770[WmGetCurrRenderBufferId()]; }

static void* WmGetSkyboxOverlayCurrTextureSettingBuffer(void) { return (WmGetCurrRenderBufferId() * 0xC) + D_8010B068; }

static s32 func_800AE628(void) { return D_8010B080; }

void WmUpdateSkyboxOverlayVertexes(s16 angle) {
    SVECTOR v;
    SVECTOR rot;
    VECTOR out;
    MATRIX m;
    s32 y;
    POLY_G4* prim;

    prim = &D_800C6770[WmGetCurrRenderBufferId()];
    v.vx = 0;
    v.vz = -0x4000;
    v.vy = -D_80116508;
    WmRotateVectorByYAngle(&v, -angle);
    WmSetTranslationVectorInScreenSpace(&v);
    v.vx = v.vy = v.vz = 0;
    gte_ldv0(&v);
    gte_rtps();
    gte_stsxy2(&y);
    rot.vx = rot.vy = 0;
    y = (u16)(y >> 16) + 0x1A;
    D_8010B080 = y;
    rot.vz = func_800A1DC0();
    RotMatrix(&rot, &m);
    SetRotMatrix(&m);
    out.vx = 160;
    out.vz = 0;
    out.vy = y;
    TransMatrix(&m, &out);
    SetTransMatrix(&m);
    v.vz = 0;
    v.vx = -180;
    v.vy = -y - 24;
    gte_ldv0(&v);
    gte_rt();
    gte_stlvnl(&out);
    prim->x0 = out.vx;
    prim->y0 = out.vy;
    v.vx = 180;
    v.vy = -y - 24;
    gte_ldv0(&v);
    gte_rt();
    gte_stlvnl(&out);
    prim->x1 = out.vx;
    prim->y1 = out.vy;
    v.vx = -180;
    v.vy = 0;
    gte_ldv0(&v);
    gte_rt();
    gte_stlvnl(&out);
    prim->x2 = out.vx;
    prim->y2 = out.vy;
    v.vx = 180;
    v.vy = 0;
    gte_ldv0(&v);
    gte_rt();
    gte_stlvnl(&out);
    prim->x3 = out.vx;
    prim->y3 = out.vy;
}

void WmInitOverlayTexturePrims(void) {
    POLY_FT4* p;
    s32 i;

    for (i = 0; i < 2; i++) {
        for (p = &D_8010B084[i][0]; p < &D_8010B084[i + 1][0]; p++) {
            setPolyFT4(p);
            p->v1 = 192;
            p->r0 = p->g0 = p->b0 = 64;
            p->v0 = 192;
            p->v3 = 255;
            p->v2 = 255;
            p->clut = 0x7B0F;
            setSemiTrans(p, 1);
            p->tpage = getTPage(0, 1, 0x240, 0x100);
        }
        setPolyFT4(&D_800C67B8[i]);
        setSemiTrans(&D_800C67B8[i], 1);
        D_800C67B8[i].tpage = getTPage(0, 1, 0x2C0, 0x100);
    }
    D_8010B174 = 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800AEA48);

static void func_800AF0A0(s32 arg0) { D_8010B174 = arg0; }

static void WmSetGteColourSettings(void);
static void func_800AF0B0(void) {
    s32 i;

    for (i = 0; i < 0x10; i++) {
        D_8010B178[i].unk13 = 0;
        D_8010B178[i].unk04 = 0;
    }

    D_8010B3B8 = NULL;
    WmSetGteColourSettings();
}

static void WmSetGteColourSettings(void) {
    MATRIX* m;
    s32 mode;

    mode = 2;
    if (WmGetWmId() == mode) {
        m = &D_800C6828;
    } else {
        m = &D_800C6808;
    }
    SetLightMatrix(m);
    if (WmGetWmId() == mode) {
        SetBackColor(0x20, 0x20, 0x30);
    } else {
        SetBackColor(0x40, 0x40, 0x40);
    }
    SetColorMatrix(&D_800C6848);
    SetFarColor(0, 0, 0);
}

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

static void WmSetActivePointMeshCoords(u32 arg0, u32 arg1) {
    if ((D_8010B3B8 != NULL) && (arg0 < 0x24) && (arg1 < 0x1C)) {
        D_8010B3B8->unk0.vx &= 0x1FFF;
        D_8010B3B8->unk0.vz &= 0x1FFF;
        D_8010B3B8->unk0.vx |= arg0 << 13;
        D_8010B3B8->unk0.vz |= arg1 << 13;
    }
}

static void WmSetActivePointCoordsInMesh(s32 arg0, s32 arg1) {
    if (D_8010B3B8 != NULL) {
        D_8010B3B8->unk0.vx &= -0x2000;
        D_8010B3B8->unk0.vz &= -0x2000;
        D_8010B3B8->unk0.vx |= arg0 & 0x1FFF;
        D_8010B3B8->unk0.vz |= arg1 & 0x1FFF;
    }
}

static void WmSetActivePointColour(u8 arg0, u8 arg1, u8 arg2) {
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

static void WmSetActivePointSkyColour(u8 arg0, u8 arg1, u8 arg2) {
    if (D_8010B3B8 != NULL) {
        D_8010B3B8->unk14 = arg0;
        D_8010B3B8->unk15 = arg1;
        D_8010B3B8->unk16 = arg2;
    }
}

static void func_800AF364(u8 arg0, u8 arg1, u8 arg2) {
    if (D_8010B3B8 != NULL) {
        D_8010B3B8->unk18 = arg0;
        D_8010B3B8->unk19 = arg1;
        D_8010B3B8->unk1A = arg2;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmUpdateLightingFromPoints);

static s32 WmGetDistanceToActivePoint(VECTOR* v) {
    return D_8010B3B8 ? WmGetDistanceBetweenPoints(v, &D_8010B3B8->unk0) : 0;
}

static s16 func_800AF9A0(VECTOR* arg0) {
    return D_8010B3B8 == NULL ? 0 : WmGetRotFromEntityToEntity(arg0, (VECTOR*)D_8010B3B8);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmFadeInit);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmFadeRender);

static void WmSetFadeIn(s32 arg0, s32 arg1) {
    s32 i;
    s32 tpage;

    if (D_8010B488[0] > 0) {
        D_8010B47C = -arg0;
        for (i = 0; i < 2; i++) {
            if (GetGraphType() == 1 || GetGraphType() == 2) {
                tpage = 0xA6;
                if (arg1 == 1) {
                    tpage = 0x126;
                }
            } else {
                tpage = 0x36;
                if (arg1 == 1) {
                    tpage = 0x56;
                }
            }
            SetDrawMode(&D_8010B434[i].mode, 0, 1, tpage, NULL);
        }
    } else {
        D_8010B47C = 0;
    }
}

static void WmSetFadeOut(s32 arg0, s32 arg1) {
    s32 i;
    s32 tpage;

    if (D_8010B488[0] < D_8010B494[0]) {
        D_8010B47C = arg0;
        for (i = 0; i < 2; i++) {
            if (GetGraphType() == 1 || GetGraphType() == 2) {
                tpage = 0xA6;
                if (arg1 == 1) {
                    tpage = 0x126;
                }
            } else {
                tpage = 0x36;
                if (arg1 == 1) {
                    tpage = 0x56;
                }
            }
            SetDrawMode(&D_8010B434[i].mode, 0, 1, tpage, NULL);
        }
    } else {
        D_8010B47C = 0;
    }
}

static void WmFadeInSnow(s32 arg0) { (&D_8010B47C)[arg0] = (D_8010B488[arg0] < D_8010B494[arg0]) << 4; }

static void WmFadeOutSnow(s32 arg0) { (&D_8010B47C)[arg0] = (D_8010B488[arg0] > 0) ? -0x10 : 0; }

static void WmFadeStartSnow(s32 arg0) {
    WmFadeInSnow(arg0);
    D_8010B4A0[arg0] = 0x64;
}

s32 WmFadeIsStopped(void) { return !D_8010B47C; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0250);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0334);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B04AC);

// zolom init position
static void func_800B0670(void) {
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

static void func_800B075C(void) {
    if (D_8010C804 == 0) {
        func_800B0670();
        D_8010C804 = 1;
    }
}

static s32 func_800B0794(void) {
    VECTOR sp10;
    WmGetPosFromPcEntity(&sp10);
    return D_8010C804 != 0 && (sp10.vx - 0x30000) < 0x10000U && (sp10.vz - 0x1C000) < 0x10000U;
}

static s32 func_800B0800(void) { return D_8010C808; }

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B0810);

void WmDrawZolomSegments(void) {
    s32 buffer;
    VECTOR pos;
    WorldZolomSegment* seg;
    POLY_FT4* prim;
    s32 base;
    s32 d;

    buffer = WmGetCurrRenderBufferId();
    func_800A6B8C(&pos);
    seg = D_8010C42C - 1;
    if (seg < D_8010C2AC) {
        seg = &D_8010C2AC[0x2F];
    }
    for (prim = D_8010C430[buffer]; prim < &D_8010C430[buffer][12]; prim++) {
        seg += 4;
        if (seg >= &D_8010C2AC[0x30]) {
            seg -= 0x30;
        }
        if (IsChunkLoaded((seg->x >> 13) + 26, (seg->z >> 13) | 16) != 0) {
            base = pos.vx - 0x34000;
            d = seg->x - base;
            if (d >= -30000 && d <= 30000) {
                prim->pad1 = d;
                base = pos.vz - 0x20000;
                d = seg->z - base;
                if (d >= -30000 && d <= 30000) {
                    prim->pad2 = d;
                    WmDrawGroundQuad(300, 300, seg->z2 - WmGetHorizonCurveDrop(prim->pad1, prim->pad2) * 4,
                                     seg->x2 + 0x800, prim, 0);
                }
            }
        }
    }
}

void UpdateZolomGroundHeight(WorldChunkHeader* arg0) {
    VECTOR pos;
    SVECTOR dir;
    s16 chunkX;
    s16 chunkZ;
    s32 hit;

    if (func_800B0794() == 0) {
        return;
    }
    pos.vx = D_8010C42C->x + 0x34000;
    pos.vz = D_8010C42C->z + 0x20000;
    WmExtractLoopCoordsTopBottomParts(&pos, &dir, &chunkX, &chunkZ);
    if (chunkX == arg0->x && chunkZ == arg0->z) {
        hit = func_800A19FC(arg0, &dir, D_8010C80C, &D_8010C42C->z2, 0, NULL, 0x64);
        D_8010C800 = hit;
        if (hit == 0) {
            D_8010C42C->z2 = 0;
        }
    }
}

void WmUpdateZolom(void) {
    VECTOR pos;
    s32 dist;
    s32 total;
    s32 d;
    s32 terrain;
    s32 model;

    WmGetPosFromPcEntity(&pos);
    if (func_800B0794() != 0) {
        if (D_8010C800 == 0) {
            D_8010C42C->x = (D_8010C42C->x * 63 + 10000) >> 6;
            D_8010C42C->z = (D_8010C42C->z * 63 + 15000) >> 6;
            D_8010C7F0 = D_8010C42C->x2 + 0x800;
        }
        D_8010C800 = 0;
        d = pos.vx - D_8010C42C->x;
        dist = d - 0x34000;
        if (dist <= 0) {
            dist = 0x34000 - d;
        }
        d = pos.vz - D_8010C42C->z;
        if (d - 0x20000 > 0) {
            total = dist - 0x20000;
            total += d;
        } else {
            total = dist + 0x20000;
            total -= d;
        }
        if (total >= 201 || (WmIsPcEntityModelInMask(7) == 0 && func_800A9240() == 0)) {
            func_800B0810();
        } else {
            func_800ABA18(7);
            D_8010C804 = 0;
        }
        WmDrawZolomSegments();
    }
    terrain = WmGetPcEntityTerrainId() & 0x1F;
    model = WmGetModelIdFromPcEntity();
    if (model != D_8010AE50) {
        terrain = D_8010AE4C;
    }
    D_8010AE50 = model;
    if (terrain == 7 && D_8010AE4C == 0) {
        D_8010C808 = pos.vx > 0x36000;
    }
    D_8010AE4C = terrain;
}

static void func_800B104C(void) {
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

static void RegisterChunk(WorldChunkHeader* arg0) {
    WorldChunkHeader** slot;
    WorldTriangle* t;

    if (arg0 == NULL) {
        return;
    }
    slot = D_8010CA24;
    while (slot < D_8010CA74) {
        if (*slot == arg0) {
            break;
        }
        slot++;
    }
    if (slot < D_8010CA74) {
        return;
    }
    if (D_8010CA74 >= &D_8010CA24[20]) {
        func_800A0B40(0x47);
    }
    slot = D_8010CA74;
    D_8010CA74 = slot + 1;
    *slot = arg0;
    for (t = arg0->tris; t < arg0->tris + arg0->numTris; t++) {
        *((u8*)&t->textureAndLocationAndFlags + 1) &= ~0x40;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B11C4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B1650);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B190C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B1C80);

static void SetCurrentTriangle(WorldChunkHeader* arg0, WorldTriangle* arg1) {
    VECTOR pos;
    s32 inside;
    s32 zoff;

    if (D_8010CA8C != 0 && D_8010CA8C != 2 && D_8010CA8C != 3) {
        return;
    }
    D_8010CA1C = &D_8010C83C;
    D_8010C83C.chunk = arg0;
    D_8010C83C.tri = arg1;
    D_8010C83C.unk8 = 0;
    D_8010CA20 = 0;
    D_8010CA78 = 0;
    D_8010CA74 = D_8010CA24;
    RegisterChunk(arg0);
    if (arg1 != NULL) {
        *((u8*)&arg1->textureAndLocationAndFlags + 1) |= 0x40;
    }
    D_800BD144 = 0;
    D_8010CA8C = 1;
    WmGetPosFromPcEntity(&pos);
    inside = 0;
    if (pos.vx >= 0x36000 && pos.vx <= 0x3FFFF) {
        zoff = pos.vz - 0x1C000;
        inside = (u32)zoff <= 0x9FFF;
    }
    D_8010CAF8 = inside;
}

static void func_800B22E4(void) { func_800B190C(); }

static s32 WmGetTriangleEdgeFacingCamera(WorldTriangleRef* ref) {
    VECTOR center;
    VECTOR edge;
    VECTOR entity;
    WorldTriangle* tri;
    SVECTOR* verts;
    s32 rot;
    s32 best;
    s32 result;
    s32 diff;

    verts = ref->chunk->verts;
    tri = ref->tri;
    result = 1;
    WmGetRealCamRot();
    center.vx = (verts[tri->vert[0]].vx + verts[tri->vert[1]].vx + verts[tri->vert[2]].vx) / 3;
    center.vz = (verts[tri->vert[0]].vz + verts[tri->vert[1]].vz + verts[tri->vert[2]].vz) / 3;
    if (D_8010CAF8 != 0) {
        entity.vx = (ref->chunk->x << 13) + center.vx;
        entity.vz = (ref->chunk->z << 13) + center.vz;
        edge.vx = 0x3A681;
        edge.vz = 0x2195F;
        rot = WmGetRotFromEntityToEntity(&entity, &edge);
    } else {
        rot = WmGetRealCamRot();
    }
    edge.vx = (verts[tri->vert[0]].vx + verts[tri->vert[1]].vx) >> 1;
    edge.vz = (verts[tri->vert[0]].vz + verts[tri->vert[1]].vz) >> 1;
    best = (WmGetRotFromEntityToEntity(&center, &edge) - rot) & 0xFFF;
    if (best >= 0x800) {
        best = 0x1000 - best;
    }
    edge.vx = (verts[tri->vert[1]].vx + verts[tri->vert[2]].vx) >> 1;
    edge.vz = (verts[tri->vert[1]].vz + verts[tri->vert[2]].vz) >> 1;
    diff = (WmGetRotFromEntityToEntity(&center, &edge) - rot) & 0xFFF;
    if (diff >= 0x800) {
        diff = 0x1000 - diff;
    }
    if (diff < best) {
        best = diff;
        result = 2;
    }
    edge.vx = (verts[tri->vert[2]].vx + verts[tri->vert[0]].vx) >> 1;
    edge.vz = (verts[tri->vert[2]].vz + verts[tri->vert[0]].vz) >> 1;
    diff = (WmGetRotFromEntityToEntity(&center, &edge) - rot) & 0xFFF;
    if (diff >= 0x800) {
        diff = 0x1000 - diff;
    }
    if (diff < best) {
        result = 4;
    }
    return result;
}

static void GetTriangleCenter(WorldTriangleRef* arg0, VECTOR* arg1) {
    WorldTriangle* tri;
    WorldChunkHeader* chunk;
    SVECTOR* verts;

    if (arg1 == NULL) {
        return;
    }
    tri = arg0->tri;
    chunk = arg0->chunk;
    verts = chunk->verts;
    arg1->vx = ((verts[tri->vert[0]].vx + verts[tri->vert[1]].vx + verts[tri->vert[2]].vx) / 3) + (chunk->x << 13);
    arg1->vy = 0;
    arg1->vz =
        ((verts[tri->vert[0]].vz + verts[tri->vert[1]].vz + verts[tri->vert[2]].vz) / 3) + (arg0->chunk->z << 13);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B271C);

static void func_800B28CC(s32 arg0) {
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

static void UpdateWorldMode(void) {
    func_800B22E4();
    if (D_8010CAF4 != 0) {
        if (D_8010CA8C == 3) {
            D_8010CAF4 = 0;
            D_8010CA8C = 0;
            func_800A2108(1, 5);
        } else if (D_8010CA8C == 2) {
            func_800A1D54(1);
            func_800A45C4(6000);
            func_800A45D4(1500);
            func_800ABA18(8);
            D_8010CAF4 = 0;
        }
    }
    if (D_8010CAF0 == 3 && (D_8010CA8C == 2 || D_8010CA8C == 3) && D_8010CACC == 0 && D_8010CAD0 == 0) {
        func_800B271C(3);
    }
    func_800B29CC();
}

static void func_800B2F94(s32 arg0) { D_8010CAF0 = arg0; }

static s32 func_800B2FA4(void) { return D_8010CA8C != 2 ? -(D_8010CA8C == 3) : 1; }

static s32 func_800B2FD0(void) { return D_8010CACC != 0 || D_8010CAD0 != 0 || D_8010CAD4 != 0; }

static void func_800B3018(void) {
    D_8010CAF4 = 1;
    if (D_8010CA8C != 1)
        D_8010CA8C = 0;
}

static void WmSnowReset(s16 arg0) {
    D_8010CAFC = arg0;
    D_8010CB10 = 0;
    D_8010CB0C = 0;
    D_8010CB08 = 0;
    D_8010CB04 = 0;
    D_8010CB00 = 0;
}

static s16 WmSnowUpdate(s32 arg0, s32 arg1) {
    VECTOR pos;
    s16 vec[3];
    s32 camRot;
    s16 angle;
    s32 step;

    camRot = WmGetRealCamRot();
    angle = camRot;
    if (D_8010CB04 == D_8010CB08) {
        D_8010CB10 = func_800A97A8() + camRot;
        WmFadeStartSnow(1);
        WmFadeStartSnow(2);
    } else if (D_8010CB04 < D_8010CB08) {
        func_800A94D0(D_8010CB10 - camRot);
    }
    step = 0;
    if (arg0 != 0 || D_8010CB04 < D_8010CB08) {
        step = 1;
    }
    D_8010CB04 -= step;
    if (D_8010CB04 <= 0) {
        D_8010CB04 = D_8010CB0C = func_800ADFC0() + 128;
        D_8010CB08 = (func_800ADFC0() >> 3) + 64;
        D_8010CB00 = ((func_800ADFC0() & 2) - 1) * 16;
    }
    if (D_8010CB04 >= D_8010CB08) {
        if (arg1 != 0) {
            if (WmSetActiveEntityWithModelId(D_8010CAFC + 21) == 0) {
                WmInsertInEntityStructList();
                WmInitActiveEntityStruct(D_8010CAFC + 21);
            }
            angle = -angle;
            vec[1] = 0;
            vec[0] = 0;
            vec[2] = 300;
            D_8010CAFC = (D_8010CAFC + 1) % 3;
            WmRotateVectorByYAngle(vec, angle);
            WmGetPosFromPcEntity(&pos);
            pos.vx = vec[0] + pos.vx;
            pos.vz = vec[2] + pos.vz;
            func_800A9D5C(&pos);
            func_800A94F4(angle);
            WmSetPcEntityAsActiveEntity();
        }
        if (D_8010CB04 >= D_8010CB08) {
            return 0;
        }
    }
    return D_8010CB00;
}

static s16 func_800B32F0(void) { return D_8010CAFC; }

static void func_800B3300(u32 arg0) {
    D_800C68E8[0].timer = arg0 & 0xFF;
    D_800C68E8[1].timer = (arg0 >> 8) & 0xFF;
    D_800C68E8[2].timer = (arg0 >> 0x10) & 0xFF;
    D_8010CB14 = arg0 >> 0x18;
    D_8010CB1C = 0;
    D_8010CB18 = 0;
}

static s32 func_800B3350(void) {
    return D_800C68E8[0].timer | (D_800C68E8[1].timer << 8) | (D_800C68E8[2].timer << 0x10) | (D_8010CB14 << 0x18);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B338C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3418);

static void func_800B37E0(s32* arg0, s32* arg1) {
    if (arg0 != NULL) {
        *arg0 += D_8010CB18;
    }
    if (arg1 != NULL) {
        *arg1 += D_8010CB1C;
    }
}

void WmUpdateAmbientSoundTimers(void) {
    VECTOR pos;
    s16 x;
    s16 z;
    WorldSoundArea* area;
    WorldSoundArea* it;

    if (WmGetPcEntityTerrainId() == 14 && (InputReadPads() & (PADLup | PADLdown | PADLleft | PADLright))) {
        WmGetPosFromPcEntity(&pos);
        WmExtractLoopCoordsTopBottomParts(&pos, NULL, &x, &z);
        area = func_800B338C(x, z);
        if (area != NULL && area->timer < 100) {
            area->timer += 10;
        }
        ToggleAmbientSound(203);
    }
    for (it = D_800C68E8; it < &D_800C68E8[3]; it++) {
        if ((it->timer -= it->timer > 0) == 1) {
            ToggleAmbientSound(-203);
        }
    }
}

static void InitEffectPool(void) {
    s32 off;
    s32 next;
    s32 i;

    next = (s32)&D_8010D930;
    off = 0xD98;
    do {
        *(s32*)&D_8010CB24[off] = next;
        off -= 0x74;
        next -= 0x74;
    } while (off >= 0);

    i = 4;
    D_8010D930 = 0;
    D_8010D9A4 = D_8010CB24;
    D_8010D9B0 = 0;
    D_8010D9AC = 0;
    D_8010D9A8 = 0;
    D_8010D9B4 = 0;
    do {
        ((u8*)D_8010D9B8)[i + 2] = 0; // WorldEffectSlot.interval
        i -= 4;
    } while (i >= 0);
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B39B4);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B3C40);

void WmDrawBillboardSprites(void) {
    s32 dz;
    WorldSprite* node;
    s32 dy;
    s32 dx;
    VECTOR pos;
    SVECTOR v;
    SVECTOR rot;
    MATRIX rotMat;
    MATRIX camMat;
    MATRIX worldMat;

    rot.vx = 0;
    rot.vy = -WmGetRealCamRot();
    rot.vz = 0;
    RotMatrixYXZ(&rot, &worldMat);
    rot.vx = -func_800A2078();
    RotMatrixYXZ(&rot, &camMat);
    func_800A6B8C(&pos);
    for (node = D_8010D9A8; node != NULL; node = node->next) {
        dx = node->x - pos.vx;
        dy = node->y - D_80116508;
        dz = node->z - pos.vz;
        if (dx > -30000 && dx < 30000 && dz > -30000 && dz < 30000) {
            v.vx = dx;
            v.vy = dy - WmGetHorizonCurveDrop(dx, dz);
            v.vz = dz;
            if (node->rotate != 0) {
                rot.vz = 0;
                rot.vx = 0;
                rot.vy = node->rotY;
                RotMatrix(&rot, &rotMat);
                SetRotMatrix(&rotMat);
            } else {
                SetRotMatrix(&camMat);
            }
            func_800B3C40(&node->prims[WmGetCurrRenderBufferId()], &D_800C6A10[node->type * 12], &v, node->rotate);
        }
    }
}

static const s32 D_800A0768[] = {0, 0x28};

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B4244);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B45DC);

void ResetEffectState(void) {
    u8* p;

    *(s16*)0x1F800010 = 0;
    *(s16*)0x1F800012 = 0x32;
    *(s16*)0x1F800014 = 0;
    *(s16*)0x1F800020 = 0;
    WmGetPosFromPcEntity((VECTOR*)0x1F800000);
    *(s16*)0x1F80001C = 0;
    *(s16*)0x1F80001A = 0;
    *(s16*)0x1F800018 = 0;
    func_800B39B4(0, 0, 7, 0);
    p = &D_800C6A10[D_800C6940 * 0xC];
    p[3] = 0x10;
    p[2] = 0x10;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5314);

static void func_800B579C(s32 arg0, u8 arg1, u8 arg2, u8 arg3) {
    WorldEffectSlot* slot = &D_8010D9B8[arg0];

    slot->modelId = arg1;
    slot->unk1 = arg2;
    slot->interval = arg3;
    slot->timer = 0;
}

static void func_800B57C0(s32 arg0) { D_8010D9B8[arg0].interval = 0; }

static void WmUpdateEffects(void) {
    WorldEffectSlot* slot;
    WorldActor* actor;
    s32* tail;
    u8* head;
    s32 timer;

    if (D_8010D9AC != 0) {
        tail = (s32*)D_8010D9B0;
        if (tail != NULL) {
            head = D_8010D9A4;
            D_8010D9A4 = (u8*)D_8010D9AC;
            D_8010D9B0 = 0;
            D_8010D9AC = 0;
            *tail = (s32)head;
        }
    }
    WmDrawBillboardSprites();
    func_800B4244();
    for (slot = D_8010D9B8; slot < &D_8010D9B8[2]; slot++) {
        if (slot->interval > 0) {
            timer = slot->timer;
            slot->timer = timer - 1;
            if ((s8)timer < 2) {
                if (WmSetActiveEntityWithModelId(slot->modelId) != 0) {
                    slot->timer = slot->interval;
                    actor = WmGetActiveEntity(slot->modelId);
                    func_800B5314(actor, slot->unk1);
                } else {
                    slot->interval = 0;
                }
            }
        }
    }
}

static void WmCreateShadowPacket(POLY_FT4* prims, RECT* rect) {
    s32 i;
    s32 tpage;

    if (prims != NULL && rect != NULL) {
        i = 0;
        do {
            setlen(prims, 9);
            setcode(prims, 0x2E);
            prims->r0 = prims->g0 = prims->b0 = 0x20;
            prims->clut = 0x7CC4;
            if (GetGraphType() == 1 || GetGraphType() == 2) {
                tpage = 0x129;
            } else {
                tpage = 0x59;
            }
            prims->tpage = tpage;
            prims->u0 = prims->u2 = rect->x;
            prims->v0 = prims->v1 = rect->y;
            prims->u1 = prims->u3 = rect->x + rect->w;
            prims->v2 = prims->v3 = rect->y + rect->h;
            prims++;
            i++;
        } while (i < 2);
    }
}

void WmDrawGroundQuad(s16 halfX, s16 halfZ, s16 y, s16 angle, POLY_FT4* prim, s32 offsetY) {
    SVECTOR v[4];
    SVECTOR rot;
    SVECTOR pos;
    MATRIX rotMatrix;
    MATRIX m;
    s32 sz0;
    s32 sz1;
    s32 sz2;
    s32 minz;
    s32 minz2;
    s32 otz;

    pos.vx = prim->pad1;
    pos.vy = -D_80116508;
    pos.vz = prim->pad2;
    WmSetTranslationVectorInScreenSpace(&pos);
    v[1].vx = v[3].vx = halfX;
    v[2].vz = v[3].vz = halfZ;
    v[0].vx = v[2].vx = -halfX;
    v[0].vz = v[1].vz = -halfZ;
    v[0].vy = v[1].vy = v[2].vy = v[3].vy = y;
    rot.vx = rot.vz = 0;
    rot.vy = angle;
    RotMatrix(&rot, &rotMatrix);
    MulRotMatrix0(&rotMatrix, &m);
    SetRotMatrix(&m);
    gte_ldv3(&v[0], &v[1], &v[2]);
    gte_rtpt();
    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
    gte_stsz3(&sz0, &sz1, &sz2);
    minz = sz0;
    if (sz1 < minz) {
        minz = sz1;
    }
    if (sz2 < minz) {
        minz = sz2;
    }
    sz0 = minz;
    gte_ldv0(&v[3]);
    gte_rtps();
    gte_stsxy2(&prim->x3);
    prim->y0 = offsetY + prim->y0;
    prim->y1 = offsetY + prim->y1;
    prim->y2 = offsetY + prim->y2;
    prim->y3 = offsetY + prim->y3;
    gte_stsz(&sz1);
    minz2 = sz0;
    if (sz1 < sz0) {
        minz2 = sz1;
    }
    sz0 = minz2 >> 4;
    if ((u32)sz0 < 0x1000) {
        addPrim(&D_800BD130[sz0], prim);
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B5C7C);

void WmSetCalculateAllPartsLighting(FieldModelEntry* model) {
    u8* part;
    s32 i;

    part = model->partsOffset + model->modelData;
    if (part == NULL) {
        return;
    }
    for (i = 0; i < model->partCount; i++) {
        *part = 1;
        part += 0x20;
    }
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmUpdateModelByAnimationFrame);

void WmApplyModelLightingById(s16 modelId, s16 lightId) {
    u8* model;
    s32 i;
    SVECTOR* light;
    u8* packet;
    u8* buffer;

    model = WmGetModelDataByModelId(modelId);
    if (model == NULL) {
        return;
    }
    packet = (u8*)0x1F800000;
    model[1] = 2;
    i = 0;
    light = &D_800C70DC[lightId];
    buffer = &D_800C752C;
    do {
        packet[0] = light->vx;
        packet[1] = (u16)light->vx >> 8;
        packet[2] = light->vy;
        packet[3] = (u16)light->vy >> 8;
        packet[4] = light->vz;
        packet[5] = (u16)light->vz >> 8;
        packet[6] = 1;
        WmApplyModelLightingToPacket(model, packet);
        i++;
        *buffer = *buffer == 0;
    } while (i < 2);
}

void WmInitMusicData(u32* file) {
    s32* src;
    s32 i;

    src = (s32*)((s32)file + ((file[5] >> 2) << 2));
    for (i = 0; i < 0x2000; i++) {
        D_8010D9C0[i] = src[i];
    }
    for (i = 0; i < 7; i++) {
        D_801159BC[i + 1] = (s32)&D_8010D9C0[(file[5 + i] - file[5]) >> 2];
    }
    D_801159DC = 0;
    D_801159E0 = 0;
}

static void func_800B63E0(s32 arg0) { D_801159DC = arg0; }

void PlayMusicTrack(s32 arg0) {
    s16* cmd;
    s32 prev;

    if (D_801159DC != 0) {
        cmd = D_8009A000;
        if (arg0 != 1) {
            s16 mode = 0x10;
            if (D_801159E0 == 1) {
                mode = 0x14;
            }
            *cmd = mode;
        } else {
            *cmd = 0x18;
        }
        D_8009A004[0] = D_801159BC[arg0];
        D_8009A008[0] = 4;
        AkaoExec();
    }
    prev = D_801159E0;
    D_801159E0 = arg0;
    D_80116510 = prev;
}

static void func_800B64A0(void) { PlayMusicTrack(D_801159E0); }

static s32 func_800B64C8(void) { return D_801159E0; }

static void func_800B64D8(u32 arg0) {
    D_8009A000[0] = 0x30;
    D_8009A004[0] = arg0;
    AkaoExec();
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800B650C);

static void WmSetMusicVolume(u32 arg0) {
    D_8009A000[0] = 0xC0;
    D_8009A004[0] = arg0;
    AkaoExec();
}

static void func_800B65A4(u32 arg0, s32 arg1) {
    D_8009A000[0] = 0xBD;
    D_8009A004[0] = arg0;
    D_8009A008[0] = arg1;
    AkaoExec();
}

void ToggleAmbientSound(s32 arg0) {
    if (D_8010CB20 < arg0) {
        D_8009A000[0] = 0x20;
        D_8010CB20 = arg0;
        D_8009A004[0] = 0x40;
        D_8009A008[0] = arg0;
        AkaoExec();
    } else if (arg0 == -D_8010CB20) {
        D_8010CB20 = 0;
        D_8009A000[0] = 0xF1;
        AkaoExec();
        D_8009A000[0] = 0xBC;
        D_8009A004[0] = 0;
        AkaoExec();
    }
}

static void WmInitModelVariablesAndArray(void) {
    s32 i;

    for (i = 0; i < 0x2B; i++) {
        D_801159E8[i] = -1;
        D_80115A14[i] = 0;
    }
    D_80115A40 = NULL;
    D_80115A44 = 0;
    D_80115A50 = 0;
    D_80115A4C = 0;
    D_80115A48 = 0;
    D_80115A54 = 0;
    D_80115A64 = 0;
    D_80115A5C = 0;
    D_80115A60 = 0;
    D_80115A58 = 0;
    D_80115A68 = 0;
    D_800C80BC = D_80115A6C;
}

void WmCalculateBonesAndLighting(FieldModelEntry* model) {
    MATRIX m;
    u8* packet;
    u8* buffer;

    packet = (u8*)0x1F800000;
    m.m[0][0] = m.m[1][1] = m.m[2][2] = 0x1000;
    m.t[0] = m.t[1] = m.t[2] = 0;
    m.m[0][1] = m.m[0][2] = m.m[1][0] = m.m[1][2] = m.m[2][0] = m.m[2][1] = 0;
    *(s32*)0x1F800000 = 1;
    WmCalculateBoneMatrixes(model, &m, 0, 0);
    packet[0] = 0x40;
    packet[1] = 0x40;
    packet[2] = 0x40;
    packet[3] = 0x6E;
    packet[4] = 0x6E;
    packet[5] = 0x6E;
    packet[6] = 0xAA;
    packet[7] = 0xAA;
    packet[8] = 0xAA;
    packet[9] = 0x3C;
    packet[10] = 0x3C;
    packet[11] = 0x3C;
    packet[12] = 0x6D;
    packet[13] = 0xFD;
    packet[14] = 0x9B;
    packet[15] = 0x01;
    packet[16] = 0xC2;
    packet[17] = 0x0F;
    packet[18] = 0x00;
    packet[19] = 0x06;
    packet[20] = 0xEB;
    packet[21] = 0xF2;
    packet[22] = 0xD1;
    packet[23] = 0xF8;
    packet[24] = 0x98;
    packet[25] = 0xF4;
    packet[26] = 0x3E;
    packet[27] = 0xFA;
    packet[28] = 0x4C;
    packet[29] = 0xF6;
    packet[30] = 0x00;
    WmCalculateModelLighting(model, (u8*)0x1F800000);
    packet[0] = 0;
    packet[1] = 0;
    packet[2] = 0;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 1;
    WmApplyModelLightingToPacket(model, (u8*)0x1F800000);
    buffer = &D_800C752C;
    packet[0] = 0;
    packet[1] = 0;
    packet[2] = 0;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 1;
    *buffer ^= 1;
    WmApplyModelLightingToPacket(model, (u8*)0x1F800000);
    *buffer ^= 1;
    WmSetGteColourSettings();
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmLoadPcCharModelFile);

void WmLoadPcCharModelIntoMemory(void) {
    FieldModelEntry* model;

    WmLoadTexturesToVram(((D_8014A608 >> 2) << 2) + 0x8014A604);
    model = D_8014A610;
    WmLoadModelPacketAndScale(model, 0x8014FC00, 0);
    WmCalculateBonesAndLighting(model);
    D_80115A48 = 1;
    D_801159E8[0] = D_801159E8[1] = D_801159E8[2] = -1;
    D_801159E8[D_80115A5C] = 0;
}

void WmPcCharModelLoadFileCallback(void) {
    if (D_80115A60 != 0) {
        D_80115A60 = 0;
        WmRemoveMutexPriority(2);
        WmLoadPcCharModelIntoMemory();
    }
}

void WmLoadModelPacketsForSet(s16 arg0) {
    s32 dst;
    s32 i;
    s32 j;
    s8 id;
    FieldModelEntry* model;
    WorldModelPart* parts;

    dst = 0x80117000;
    for (i = 0; i < D_8013A800; i++) {
        dst = WmLoadModelPacketAndScale(&D_8013A804[i], dst, i);
    }
    for (i = 3; i < 32; i++) {
        id = D_800C7114[D_80115A58][i];
        D_801159E8[i] = id;
        if (id >= 0 && i == 24) {
            model = &D_8013A804[D_801159E8[24] - 1];
            parts = (WorldModelPart*)(model->partsOffset + model->modelData);
            for (j = 0; j < model->partCount; j++) {
                WmUpdatePartTransparency(&parts[j], 1);
            }
        }
    }
    D_80115A4C = 1;
}

static void StartStreamRead(void) {
    if (D_80115A60 != 0) {
        return;
    }
    WmAbortMapLoading();
    D_80115A40 = (u_long*)WmReserveMapBuffer(2);
    if (WmAddMutexPriority(2) != 0) {
        D_80115A60 = 1;
        D_80115A50 = 0;
        SysCdromStartLoadLzs(D_800C74DC, D_800C74E0, D_80115A40, WmPackModelLoadFileCallback);
        SystemCdromReadChain();
    }
}

void WmLoadPackModelsIntoMemory(void) {
    u_long* pack = D_80115A40;
    s32 i;
    s32 offset;

    D_80115A50 = 1;
    offset = ((pack[2] >> 2) << 2) + 4;
    D_80115A44 = offset + (s32)pack;
    for (i = 32; i < 43; i++) {
        D_801159E8[i] = i - 19;
        if (D_801159E8[i] >= 0 && i >= 41 && i < 43) {
            WmApplyModelLightingById(i, 0);
        }
    }
}

void WmPackModelLoadFileCallback(void) {
    if (D_80115A60 != 0) {
        D_80115A60 = 0;
        WmRemoveMutexPriority(2);
        WmLoadPackModelsIntoMemory();
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

static void func_800B6E78(void) {
    s32 temp_v0;

    if (D_80115A60 != 0)
        SystemCdromReadChain();
    if (D_80115A50 != 0 && func_800A8CA4() == 0)
        func_800B6E08();
    if (D_80115A64 != 0 && --D_80115A64 == 0)
        func_800A8048();
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmGetModelDataByModelId);

static void func_800B7104(s16 arg0) {
    D_80115A58 = arg0;
    WmLoadModelPacketsForSet(arg0);
}

static void WmAbortModelLoading(void) {
    SystemCdromAbortLoading();
    D_80115A60 = 0;
}

static void func_800B715C(s32 arg0) { D_80115A68 = arg0; }

static s32 func_800B716C(void) { return D_80115A68; }

static s32 func_800B717C(void) {
    s32 temp_a0;
    s32 temp_v1;
    s32 var_v0;

    var_v0 = 0;
    if (*(u16*)Savemap.memory_bank_1 >= 1000) {
        if (*(u16*)Savemap.memory_bank_1 < 1580) {
            temp_a0 = Savemap.memory_bank_1[0x7A] & 1;
            if (*(u16*)Savemap.memory_bank_1 >= 1620)
                return (temp_a0 | 2) + 1;
            return temp_a0 + 1;
        }
        temp_v1 = (*(u16*)Savemap.memory_bank_1 < 1620) ^ 1;
        if (Savemap.memory_bank_4[0x86] & 0x10)
            return (temp_v1 | 2) + 5;
        return temp_v1 + 5;
    }
    return var_v0;
}

s32 func_800B7200(void) {
    u16 progress = *(u16*)Savemap.memory_bank_1;
    return progress >= 1000 && progress < 1200;
}

static s32 func_800B7218(void) { return Savemap.memory_bank_4[0xFE]; }

void WmInitAllEntityStructs(u8*);

void WmReadSavemap(s32* arg0, s32* arg1, s32 arg2) {
    VECTOR pos;
    u16* entityPos;
    s32* slot;
    u32 view;
    s32 i;
    s32 sum;

    sum = 0;
    for (i = 0x380; i < 0x400; i++) {
        sum += Savemap.memory_bank_1[i];
    }
    if (sum & 0xFF) {
        func_800A0B40(2);
    }
    if (arg0 != NULL) {
        D_8011626C = *arg0;
    } else {
        D_8011626C = 0;
    }
    if (arg1 != NULL) {
        D_80116270 = *arg1;
    } else {
        D_80116270 = 0;
    }
    view = (*(u16*)&Savemap.memory_bank_4[0xF8] >> 12) & 3;
    WmSetCamView(view < 3 ? view : 0);
    if ((D_8011626C == 0 && D_80116270 == 0) || (u32)(D_8011626C - 1) < 2) {
        WmSetCamRot(*(u16*)&Savemap.memory_bank_4[0xF8] & 0xFFF);
    }
    WmSetCamMode(*(u16*)&Savemap.memory_bank_4[0xF8] >> 14);
    WmSyncPartyMembers(&Savemap.memory_bank_2[9], Savemap.partyID, 0);
    WmRandomInit(Savemap.time);
    func_800B3300(*(s32*)&Savemap.memory_bank_4[0xF4]);
    func_800A7E8C(*(u16*)&Savemap.memory_bank_4[0xB6]);
    WmInitAllEntityStructs(&Savemap.memory_bank_4[0xB8]);
    if (arg2 != 0) {
        *(u16*)&Savemap.memory_bank_1[0x1E] &= ~0x300;
    } else {
        WmSnowReset(Savemap.memory_bank_4[0xFB]);
        if (D_8011626C == 1) {
            for (i = 0, entityPos = (u16*)&Savemap.memory_bank_4[0xE8]; i < 3; i++) {
                if (*(s32*)entityPos != 0) {
                    WmInsertInEntityStructList();
                    WmInitActiveEntityStruct(i + 21);
                    pos.vx = entityPos[0];
                    pos.vy = 0;
                    pos.vz = entityPos[1];
                    func_800A9D5C(&pos);
                }
                entityPos += 2;
            }
        } else {
            for (i = 2, slot = (s32*)&Savemap.memory_bank_4[0xF0]; i >= 0; i--) {
                *slot-- = 0;
            }
        }
    }
    func_800B7820();
}

static u8 WmScriptGetTopFromStoreStack(void);

void WmWriteSavemap(void) {
    VECTOR pos;
    s32* entityPos;
    s32 i;
    s32 sum;

    if (func_800A1D04() == 0 && WmGetWmId() != 3) {
        WmSetCamRot(0);
    }
    *(s16*)&Savemap.memory_bank_4[0xF8] =
        (WmGetRealCamRot() & 0xFFF) | ((func_800A1D04() << 12) & 0x3000) | (WmGetCamMode() << 14);
    Savemap.memory_bank_4[0xFD] = WmGetModelIdFromPcEntity();
    if (Savemap.memory_bank_4[0xFD] == 3 && func_800A92F8(WmScriptGetTopFromStoreStack() & 0xFF) != 0) {
        Savemap.memory_bank_4[0xFD] = 43;
    }
    func_800ADA08();
    Savemap.memory_bank_4[0xFE] = WmGetWmId();
    *(s32*)&Savemap.memory_bank_4[0xF4] = func_800B3350();
    *(s16*)&Savemap.memory_bank_4[0xB6] = func_800A7E7C();
    if (Savemap.memory_bank_4[0xFE] == 3) {
        Savemap.memory_bank_4[0xFB] = func_800B32F0();
        for (i = 0, entityPos = (s32*)&Savemap.memory_bank_4[0xE8]; i < 3; i++, entityPos++) {
            if (WmSetActiveEntityWithModelId(i + 21) != 0) {
                WmGetPosFromActiveEntity(&pos);
                *entityPos = (u16)pos.vx | (pos.vz << 16);
            } else {
                *entityPos = 0;
            }
        }
    }
    sum = 0;
    for (i = 0x380; i < 0x3FF; i++) {
        sum += Savemap.memory_bank_1[i];
    }
    Savemap.memory_bank_4[0xFF] = -sum;
}

static void GetSavedParams(s32* arg0, s32* arg1, s32* arg2) {
    s32 flags;

    if (arg0 != NULL) {
        *arg0 = D_8011626C;
    }
    if (arg1 != NULL) {
        *arg1 = D_80116270;
    }
    if (arg2 != NULL) {
        flags = D_80116274;
        if (D_80116278 != 0) {
            flags |= 0x20000000;
        }
        *arg2 = flags;
    }
    *(u16*)&Savemap.memory_bank_1[0x1E] |= 0x300;
    WmWriteSavemap();
}

static void func_800B76A8(void) {
    u32 var_a0;

    var_a0 = Savemap.memory_bank_4[0xFD];
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

void WmSetFieldToLoad(s32 arg0) {
    u8* p;
    s32 index;

    index = ((((arg0 >> 8) - 1) << 1) & 0x1FE) | (arg0 & 1);
    p = &D_800BF5F0[index * 12];

    g_FieldState.eventCmdParam = *(u16*)(p + 6);
    g_FieldState.pcPosX = *(u16*)(p + 0);
    g_FieldState.pcPosY = *(u16*)(p + 2);
    g_FieldState.pcWalkMeshId = *(u16*)(p + 4);
    D_8011626C = 0;
    D_80116270 = arg0;
    g_FieldState.pcDirection = p[8];
}

void func_800B77A8(s32 arg0) {
    void WmSetFieldToLoad(s32);

    if (arg0 & 0x40000000) {
        WmSetFieldToLoad(0x2100);
    }
    D_8011626C = 1;
    D_80116274 = arg0;
}

static void func_800B77F4(s32 arg0) {
    *(volatile s32*)&Savemap.countdown_timer_seconds = arg0;
    D_80116278 = 1;
    Savemap.memory_bank_1[0x5F] = 1;
}

static void func_800B7820(void) {
    D_80116278 = 0;
    Savemap.memory_bank_1[0x5F] = 0;
}

void func_800B7838(void) {
    D_8011626C = 2;
    D_80116270 = 0;
    D_80116274 = 0;
}

static s32 func_800B785C(void) { return D_8011626C; }

static s32 func_800B786C(void) { return D_80116270; }

void WmSyncPartyMembers(u8* slots, u8* next, s32 notify) {
    s32 oldMask;
    s32 newMask;
    s32 bits;
    s32 mask;
    u32 i;
    u8 id;

    oldMask = (1 << slots[0]) | (1 << slots[1]) | (1 << slots[2]);
    newMask = (1 << next[0]) | (1 << next[1]) | (1 << next[2]);
    if (notify != 0) {
        func_800ADB30(oldMask, newMask);
    }
    bits = oldMask & ~newMask;
    for (i = 0; i < 3; i++) {
        mask = 1 << slots[i];
        if (bits & mask) {
            slots[i] = 0xFF;
        }
    }
    bits = ~oldMask & newMask;
    i = 0;
    for (id = 0; bits != 0; bits = (u32)bits >> 1, id++) {
        if (bits & 1) {
            for (; i < 3; i++) {
                if (slots[i] == 0xFF) {
                    break;
                }
            }
            if (i >= 3) {
                return;
            }
            slots[i++] = id;
        }
    }
}

// Player party member model ID (0=Cloud, 1=Tifa, 2=Cid)
s32 WmGetPcCharModelIdFromParty(void) {
    if ((Savemap.memory_bank_2[0x9] != 0) && (Savemap.memory_bank_2[0xA] != 0) && (Savemap.memory_bank_2[0xB] != 0)) {
        if ((Savemap.memory_bank_2[0x9] != 2) && (Savemap.memory_bank_2[0xA] != 2) &&
            (Savemap.memory_bank_2[0xB] != 2)) {
            if ((Savemap.memory_bank_2[0x9] == 8) || (Savemap.memory_bank_2[0xA] == 8) ||
                (Savemap.memory_bank_2[0xB] == 8)) {
                return 2;
            }
            return 0;
        }
        return 1;
    }
    return 0;
}

static void WmScriptSetFirstToStoreStack(s8);
static s32 WmScriptIsDataInStoreStack(void);

void WmSyncPartyAndPcModel(void) {
    void (*setModel)();
    s32 modelId;

    WmSyncPartyMembers(&Savemap.memory_bank_2[9], Savemap.partyID, WmGetWmId() != 2);
    if (WmGetWmId() != 2) {
        modelId = WmGetPcCharModelIdFromParty() & 0xFF;
        setModel = WmScriptIsDataInStoreStack() ? WmScriptSetFirstToStoreStack : func_800A9A04;
        setModel(modelId);
    }
}

static void CopyAreaName(s16 arg0) {
    u8* src;
    u8* base;
    u8* dst;
    u8* end;
    s32 term;
    u8 c;

    src = (u8*)func_800A40F0(arg0);
    term = 0xFF;
    base = Savemap.memory_bank_1;
    dst = base + 0x368;
    end = base + 0x380;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != term && (s32)dst < (s32)end);
}

static void func_800B7B1C(u8 arg0) { Savemap.memory_bank_4[0xFC] = arg0; }

static s32 func_800B7B2C(void) { return Savemap.memory_bank_4[0xFC]; }

static s32 func_800B7B3C(void) { return (g_BattleMode >> 3) & 1; }

// Enemy Lure/Away Modifier
static s32 func_800B7B54(void) {
    u32 var_v1;

    var_v1 = D_80062F19;
    if (var_v1 > 0x10)
        var_v1 *= 2;
    return var_v1;
}

static s32 func_800B7B78(void) {
    s32 var_v1;

    var_v1 = D_80062F1B & 0x7F;
    if (var_v1 >= 0x11)
        var_v1 *= 2;
    return var_v1;
}

static u8 func_800B7BA0(void) { return D_80062F1B >> 7; }

static u8 func_800B7BB0(void) { return D_80062F1A; }

static u8 func_800B7BC0(void) { return Savemap.memory_bank_2[0x85] & 1; }

static s32 func_800B7BD0(void) { return 1; }

static s32 func_800B7BD8(void) {
    s32 var_a0;
    s32 var_v1;

    for (var_a0 = 0, var_v1 = 0; var_v1 < 3; var_v1++)
        var_a0 += Savemap.partyID[var_v1] != 0xFF;
    return (var_a0 < 2) ^ 1;
}

static s32 func_800B7C14(void) { return 1; }

static void func_800B7C1C(void) {
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

static void func_800B832C(void) {
    VECTOR sp10;
    VECTOR sp20;
    s32 temp_a0;
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;

    temp_a0 = WmGetWmId();
    if (g_FieldState.battlesDisabled == 0 && temp_a0 != 2 && !func_800B2FD0() && func_800A21A4()) {
        temp_s0 = func_800A9AD0();
        WmGetPosFromPcEntity(&sp10);
        WmGetPos2FromPcEntity(&sp20);
        if (WmIsPcEntityModelInMask(0x47) && D_80116280) {
            if ((temp_s0 == 0) && (sp10.vx != sp20.vx || sp10.vz != sp20.vz)) {
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
static void WmDialogsInit(FieldScriptHeader* fieldScripts) {
    g_WindowToEntity[0] = 0xFF;
    g_CurrentEntity = 0xFF;
    g_FieldScripts = fieldScripts;
    fieldScripts->stringOffset = 8;
    WmDialog0ResetAndPointerInit();
    g_pFieldState = &g_FieldState;
}

static void WmDialogSetMessageToShowForId0(u8 arg0) {
    if (g_WindowData[0].state == WSTATE_INIT) {
        D_8011628C = 0;
        D_80116288 = 0;
        WmDialogSetMessageToShow(0, arg0);
    }
}

static void WmDialogSetAskToShowForId0(s32 arg0, s16 arg1, s16 arg2) {
    if (g_WindowData[0].state == WSTATE_INIT) {
        D_80116288 = arg1;
        D_8011628C = arg2;
        WmDialogSetAskToShow(0, arg0, D_80116288, D_8011628C, &D_80116290);
    }
}

static s32 WmDialogIsWindowWithId0Showing(void) { return g_WindowData[0].state != WSTATE_INIT; }

static s32 WmDialogSetWindowWithId0ToClose(void) {
    if ((g_WindowData[0].state != WSTATE_INIT) && (g_WindowData[0].state != WSTATE_CLOSING)) {
        WmDialogSetWindowToCloseIfPossible(0);
    }

    return g_WindowData[0].state != WSTATE_INIT;
}

void SystemMenuDrawDialog(WindowData*, s32, u32*, s32);

void WmDialogUpdate(void) {
    FieldState* fs;
    s32 keys;
    s32 prev;
    s32 prev2;

    if (g_WindowData[0].state == WSTATE_INIT) {
        return;
    }
    keys = InputReadPads();
    fs = g_pFieldState;
    prev = fs->activeKeysRaw;
    fs->activeKeysRaw = keys;
    fs->pressedKeysRaw = keys & ~prev;
    keys = InputReadPads();
    fs = g_pFieldState;
    prev2 = fs->activeKeys;
    fs->activeKeys = keys;
    fs->pressedKeys = keys & ~prev2;
    if (D_80116288 || D_8011628C) {
        WmDialogSetAskToShow(0, 0, D_80116288, D_8011628C, &D_80116290);
    } else {
        WmDialogSetMessageToShow(0, 0);
    }
    SystemMenuDrawDialog(g_WindowData, 1, D_800BD130, WmGetCurrRenderBufferId() == 0);
}

extern s16 D_80116290;

s16 WmDialogGetAskResult(void) { return g_WindowData[0].state == WSTATE_INIT ? D_80116290 : -1; }

static void WmDialogSetPosAndSizeForId0(SVECTOR* arg0) {
    if (arg0 != NULL) {
        WmDialogSetPosAndSize(0, arg0->vx, arg0->vy, arg0->vz, arg0->pad);
    }
}

static void WmDialogSetModeAndPermanencyForId0(s16 arg0, s16 arg1) { WmDialogSetModeAndPermanency(0, arg0, arg1); }

static void WmDialogPrintParam(const char* str, s32 val, s32 kind) {}

static void WmDialogPrintNoParam(const char* errmsg) {}

void WmDialog0ResetAndPointerInit(void) {
    s32 i;

    g_WindowCount = 0;
    for (i = 0; i < 1; i++) {
        WmDialogReset(i);
    }
    if (g_FieldScripts->stringOffset != 0) {
        D_80116298 = (u8*)g_FieldScripts + g_FieldScripts->stringOffset;
    } else {
        D_80116298 = NULL;
    }
}

static void WmDialogReset(s16 window) {
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
        WmDialogPrintParam("mes reset=", window, 1);
    }
}

static s32 WmDialogSetWindowToCloseIfPossible(s16 window) {
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

static void WmDialogSetModeAndPermanency(s16 window, s16 style, s16 preventClose) {
    g_WindowData[window].style = style;
    g_WindowData[window].preventClose = preventClose;
}

static void WmDialogsReset(void) {
    s32 i;

    for (i = 0; i < 1; i++) {
        g_WindowData[i].state = WSTATE_INIT;
        g_WindowData[i].stringLength = 0;
        g_WindowToEntity[i] = 0xFF;
        D_801163D0[i] = 0;
    }
    g_WindowCount = 0;
}

static void WmDialogSetPosAndSize(s16 window, s16 x, s16 y, s16 width, s16 height) {
    if (x < 8) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("win limit x=", x, 2);
        }
        x = 8;
    }
    if (x + width > 312) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("win limit x=", x + width, 3);
        }
        x = 312 - width;
    }
    if (y < 8) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("win limit y=", y, 2);
        }
        y = 8;
    }
    if (y + height > 224) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("win limit y=", y + height, 3);
        }
        y = 224 - height;
    }

    g_WindowData[window].x = x;
    g_WindowData[window].y = y;
    g_WindowData[window].width = width;
    g_WindowData[window].height = height;
}

static void WmDialogAddPos(s16 window, s16 dx, s16 dy) {
    g_WindowData[window].x += dx;
    g_WindowData[window].y += dy;
}

static void WmDialogSetHeight(s16 window, s16 height) { g_WindowData[window].height = height; }

static s32 WmDialogSetMessageToShow(u8 window, u8 message) {
    switch (g_WindowData[window].state) {
    case WSTATE_INIT:
        if (WmDialogInitWindow(window, message)) {
            return 1;
        }
        break;
    case WSTATE_SHOW:
        WmDialogInscreaseWindow(window);
        break;
    case WSTATE_TXT:
        WmDialogStringOutput(window);
        break;
    case WSTATE_SCROLL_ROW:
        WmDialogTextScrollByRow(window);
        break;
    case WSTATE_SCROLL_TXT_WHILE_OK:
        WmDialogTextScrollDuringOk(window);
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
            WmDialogDiscreaseWindow(window);
        }
        break;
    case WSTATE_WAIT_NEXT_WINDOW:
        if (g_pFieldState->pressedKeys & PADRright) {
            WmDialogStartText(window);
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
        WmDialogStartText(window);
        break;
    case WSTATE_UNK5:
    case WSTATE_CLOSING:
        if (WmDialogDiscreaseWindow(window)) {
            return 1;
        }
        break;
    }

    return 0;
}

s32 WmDialogSetAskToShow(u8 window, u8 message, u8 first, u8 last, s16* selectedLine) {
    switch (g_WindowData[window].state) {
    case WSTATE_INIT:
        if (WmDialogInitWindow(window, message)) {
            return 1;
        }
        break;
    case WSTATE_SHOW:
        WmDialogInscreaseWindow(window);
        break;
    case WSTATE_TXT:
        WmDialogStringOutput(window);
        break;
    case WSTATE_SCROLL_ROW:
        WmDialogTextScrollByRow(window);
        break;
    case WSTATE_SCROLL_TXT_WHILE_OK:
        WmDialogTextScrollDuringOk(window);
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
                    WmDialogPlaySound();
                }
                (*selectedLine)--;
            }
            if (g_pFieldState->pressedKeysRaw & PADLdown) {
                if (*selectedLine < last) {
                    WmDialogPlaySound();
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
                WmDialogPlaySound();
                g_WindowData[window].state = WSTATE_CLOSING;
                WmDialogDiscreaseWindow(window);
            }
        }
        break;
    case WSTATE_WAIT_NEXT_WINDOW:
        if (g_pFieldState->pressedKeys & PADRright) {
            WmDialogStartText(window);
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
        WmDialogStartText(window);
        break;
    case WSTATE_UNK5:
    case WSTATE_CLOSING:
        if (WmDialogDiscreaseWindow(window)) {
            g_WindowData[window].pointerEnabled = 0;
            return 1;
        }
        g_WindowData[window].pointerEnabled ^= 1;
        break;
    }

    return 0;
}

static void WmDialogPlaySound(void) {
    *D_8009A000 = 0x30;
    *D_8009A004 = 1;
    *D_8009A008 = 0x40;
    AkaoExec();
}

static s32 WmDialogInitWindow(s16 window, s16 stringId) {
    if (D_80116298 == NULL) {
        WmDialogPrintNoParam("No mes data!");
        return 1;
    }

    if (g_WindowToEntity[window] != 0xFF) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("mes busy=", window, 1);
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

static void WmDialogInscreaseWindow(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("mes busy=", window, 1);
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

static void WmDialogStringOutput(s16 window) {
    u8 opcode;
    u16 len;
    s16 i;
    s16 baseCredit;
    s16 characterCost;
    u8* name;
    u16 value;

    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("mes busy=", window, 1);
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
                        WmDialogPrintParam("mpara=", value, 4);
                    }
                    opcode = D_801162B0[window][1];
                    switch (opcode) {
                    // Integer to decimal string.
                    case 0xDE:
                        WmDialogAddDigitWithoutLeadingSpace(value, D_801163C8[window]);
                        break;
                    // Integer to decimal string with space fill.
                    case 0xE1:
                        WmDialogAddDigitWithLeadingSpace(value, D_801163C8[window]);
                        break;
                        // Integer to hexadecimal string.
                    case 0xDF:
                        WmDialogAddHexDigitWithoutLeadingSpace(value, D_801163C8[window]);
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
                        WmDialogPrintParam("gstr=", value, 4);
                        if (g_DebugLevel & 3) {
                            WmDialogPrintParam("glen=", len, 4);
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

static void WmDialogTextScrollByRow(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("mes busy=", window, 1);
        }
        return;
    }

    if (g_WindowData[window].textScrolling & 0xF) {
        g_WindowData[window].textScrolling -= 2;
    } else {
        g_WindowData[window].state = WSTATE_TXT;
    }
}

static void WmDialogTextScrollDuringOk(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("mes busy=", window, 1);
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

static void WmDialogStartText(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("mes busy=", window, 1);
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

static s32 WmDialogDiscreaseWindow(s16 window) {
    if (g_WindowToEntity[window] != g_CurrentEntity) {
        if (g_DebugLevel & 3) {
            WmDialogPrintParam("mes busy=", window, 1);
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

static u16 func_800BAE60(s16 window) {
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

static void WmDialogAddDigitWithoutLeadingSpace(u16 value, u8* dst) {
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

static void WmDialogAddDigitWithLeadingSpace(u16 value, u8* dst) {
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

static void WmDialogAddHexDigitWithoutLeadingSpace(u16 value, u8* dst) {
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

static s32 func_800BB650(s16 stringId) {
    s16 i;
    s16 j;
    u8* str;
    u8* charName;
    u8 value;

    if (D_80116298 == NULL) {
        WmDialogPrintNoParam("No mes data!");
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

static void WmDialogCopyStringIntoCharName(s16 battleCharId, s16 stringId) {
    u8* newName;
    s16 len;
    u8* charName;

    if (D_80116298 == NULL) {
        WmDialogPrintNoParam("No mes data!");
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

static void func_800BB8B0(void) {
    D_801163D4 = 0;
    D_801163D8 = 0;
    D_801163E8 = &D_801163E0;
    D_801163EC = 0;
    D_801163DC = 0;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmMovePcEntityByDistance);

void WmScriptPushToStoreStack(u8 arg0) {
    s8* temp_v1;

    temp_v1 = D_801163E8;
    if (temp_v1 < (s8*)&D_801163E8) {
        D_801163E8 = temp_v1 + 1;
        *temp_v1 = arg0;
    }
}

static u8 WmScriptPopFromStoreStack(void) {
    if (&D_801163E0 >= D_801163E8)
        return 0;

    D_801163E8 = D_801163E8 - 1;
    return D_801163E8[0];
}

static u8 WmScriptGetTopFromStoreStack(void) {
    u8 var_a0;

    var_a0 = 0;
    if (&D_801163E0 < D_801163E8)
        var_a0 = D_801163E8[-1];

    return var_a0;
}

static void WmScriptSetFirstToStoreStack(s8 arg0) { D_801163E0 = arg0; }

static s32 WmScriptIsDataInStoreStack(void) { return &D_801163E0 < D_801163E8; }

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
        WmScriptPushToStoreStack(WmGetModelIdFromPcEntity());
        if (func_800A929C()) {
            WmLinkPcToActiveEntity();
            PlayMusicTrack(2);
            return;
        }
        WmUnlinkPcLinkedEntityFromAll();
        WmUnlinkPcEntityFromAll();
        WmSetActiveEntityAsPcEntity();
        switch (WmGetModelIdFromPcEntity()) {
        case 3:
            func_800A98A4(1);
            func_800A368C(1);
            ResetEffectState();
            if (func_800B64C8() < 6) {
                PlayMusicTrack(func_800B7200() ? 1 : 3);
            }
            break;
        case 6:
            ToggleAmbientSound(0x1EC);
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

static void func_800BBC4C(void) {
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

static void func_800BBD20(s32 arg0) {
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
                            temp_v0 = WmGetRotFromEntityToEntity(&sp20, &sp10) - 0x400;
                            WmSetActiveEntityDirectionAndRot(temp_v0);
                            WmScriptPopFromStoreStack();
                            func_800A8E50();
                            if (temp_s1 < 0x29)
                                WmSetActiveEntityAsPcEntity();

                            func_800AA2E4(2);
                            WmSetActiveEntityDirectionAndRot(temp_v0);
                            PlayMusicTrack(1);
                            func_800A2108(0, 6);
                            if (temp_s1 == 4)
                                func_800A82DC();
                            else if (temp_s1 >= 0x29) {
                                WmSetActiveEntityWithModelId(WmGetPcCharModelIdFromParty());
                                WmSetActiveEntityAsPcEntity();
                            }
                            func_800ADC3C(&sp10);
                        } else {
                            WmInsertInEntityStructList();
                            temp_s0 = WmScriptPopFromStoreStack() & 0xFF;
                            if (func_800A92F8(temp_s0) != 0) {
                                WmInitActiveEntityStruct(WmScriptGetTopFromStoreStack() & 0xFF);
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
                                PlayMusicTrack(2);
                            else
                                PlayMusicTrack(1);

                            if (temp_s1 == 6)
                                ToggleAmbientSound(-0x1EC);
                            else if (temp_s1 == 5)
                                ToggleAmbientSound(-0x1ED);

                            func_800A6994(&sp10, 1);
                        }
                    }
                } else {
                    if ((WmGetModelIdFromPcEntity() == 3) || (WmGetModelIdFromPcEntity() == 4))
                        WmScriptDisableForPcEntity(0);

                    if ((temp_s2 != 0) ||
                        ((WmGetModelIdFromPcEntity() == 5) && (WmIsPcEntityPosNeedRecalculation() != 0)))
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

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmUiMapCreate);

INCLUDE_ASM("asm/us/world/nonmatchings/world", WmUiMapUpdate);

static void WmSetCamMode(s16 arg0) {
    D_801164F8 = arg0;
    if (WmGetModelIdFromPcEntity() != 3)
        D_801164FC = D_801164F8;
}

static s16 WmGetCamMode(void) { return D_801164F8; }

static void func_800BCA48(void) {
    if (D_801164F8 == 1)
        D_801164F8 = D_801164FC;
}

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCA78);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCB2C);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCBE8);

INCLUDE_ASM("asm/us/world/nonmatchings/world", func_800BCECC);
