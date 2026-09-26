//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <libetc.h>
#include <libgte.h>
#include <psxsdk/inline_c.h>
#include "chocobo_private.h"

#ifndef VERSION_PC
// macros from SEMINAR/ADVANCED/SOURCE/GRAPHICS/GTE/PROGRAM/MAINRDIV.C
#define SetSpadStack(addr)                                                                                             \
    {                                                                                                                  \
        __asm__ volatile("move $8,%0" ::"r"(addr) : "$8", "memory");                                                   \
        __asm__ volatile("sw $sp,0($8)" ::: "$8", "memory");                                                           \
        __asm__ volatile("addiu $8,$8,-4" ::: "$8", "memory");                                                         \
        __asm__ volatile("move $sp,$8" ::: "$8", "$sp", "memory");                                                     \
    }
#define ResetSpadStack()                                                                                               \
    {                                                                                                                  \
        __asm__ volatile("addiu $sp,$sp,4" ::: "$sp", "memory");                                                       \
        __asm__ volatile("lw $sp,0($sp)" ::: "$sp", "memory");                                                         \
    }
#else
// TODO: move these to PsyZ
#define SetSpadStack(addr)
#define ResetSpadStack()
#endif

static void ChocoboDrawTrackProps(void);
static void ChocoboDrawTrackPropRange(s32 start, s32 end);

void MINI_Chocobo(void) {
    SVECTOR rot1 = D_800A0000;
    SVECTOR rot2 = D_800A0000;
    VECTOR pos = D_800A0008;
    s32 ids[NUM_CHOCOBO];
    s32 keys[NUM_CHOCOBO];
    ChocoboTrackSegment* segment;
    s32 maxVSync1;
    s32 maxVSync2;
    s32 paused = 0;
    s32 frames;
    GpuBuffer* gfx;
    Chocobo* chocobos;
    Unk800B1254* modelTable;
    s32* flags;
    Chocobo* model;
    TILE* tile;
    ChocoboModels* models;
    s32 i;
    s32 j;
    s32 k;
    s32 n;
    s32 next;
    s32 count;
    s32 tmp;
    s32 rank;
    s32 pos2;
    s16 dir;
    s32 mode;
    s32 limit;

    chocobos = D_800B75CC;
    func_800A18BC();
    D_800F5074 = &D_800B7A68[0];
    DrawSync(0);
    ChocoboRaceInit();
    func_800A34A8();
    ChocoboSelectRacerAtSegment(0, 0, 0);
    ChocoboInitMusic();
    g_AkaoCmd.opcode = AKAO_VOLUME_SET;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    frames = 0;
    if (!Savemap.memory_bank_3[8]) {
        func_800A9D94();
    }
    ChocoboResetRacerColors();
    D_800B7514 = 0xFF;
    D_800B7530.unkC = 0;
    D_800F5040.fadeSpeed = -0x10;
    for (k = 0; k < NUM_CHOCOBO; k++) {
        if (!D_800B7A48.unk0 || chocobos[k].unk92) {
            chocobos[k].unk68 *= 2;
            chocobos[k].unk6C *= 2;
        }
    }
    g_AkaoCmd.opcode = AKAO_PLAY_MUSIC;
    g_AkaoCmd.params[0] = (u_long)&D_80077F64[0][0x2000];
    AkaoExec();
    maxVSync2 = 0;
    maxVSync1 = 0;
    D_800B7530.unk8 = 0;
    VSync(2);
    while (1) {
        if (D_800F5040.unk8 > 3000) {
            D_800F5040.fadeSpeed = 0x10;
            g_AkaoCmd.opcode = AKAO_VOL_SLIDE_FROM_CURR;
            g_AkaoCmd.params[0] = 0x3C;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
        }
        D_800F5078.count = 0;
        D_800B66CC.index ^= 1;
        gfx = &D_800B7A68[0];
        if (D_800F5074 == &D_800B7A68[0]) {
            gfx++;
        }
        D_800F5074 = gfx;
        ClearOTagR(gfx->ot, LEN(gfx->ot));
        D_800B66CC.cur = D_800F5074;
        SetSpadStack(getScratchAddr(0xFF));
        func_800A7CA4();
        ResetSpadStack();
        segment = &D_800B7500[chocobos[D_800B7530.unkC].unk0];
        if (segment->flags & 1) {
            ClearOTag(D_800F5074->ot2, LEN(D_800F5074->ot2));
            SetSpadStack(getScratchAddr(0xFF));
            ChocoboDrawTrackTris();
            ResetSpadStack();
        }
        if (segment->flags & 2) {
            D_800F5074->bg.r0 = segment->r;
            D_800F5074->bg.g0 = segment->g;
            D_800F5074->bg.b0 = segment->b;
            tile = &D_800F5074->bg;
            addPrim(&D_800F5074->ot[10], tile);
        }
        SetSpadStack(getScratchAddr(0xFF));
        ChocoboUpdateRanking();
        ChocoboDrawTrackSegments();
        ResetSpadStack();
        ChocoboDrawTrackProps();
        for (i = 0, mode = 3, modelTable = &D_800B1254, flags = D_800B7564; i < NUM_CHOCOBO; i++) {
            if (!flags[i]) {
                continue;
            }
            model = &chocobos[i];
            pos2 = model->unk80;
            dir = model->unk72;
            *(u_long*)getScratchAddr(0) = mode;
            func_800AF11C(&modelTable->unk0->models[model->unk90], &D_800B7544, pos2, dir >> 8);
            *(u_long*)getScratchAddr(0) = mode;
            func_800AF11C(&modelTable->unk0->models[model->unk92], &D_800B7544, pos2, dir >> 8);
        }
        for (i = 0; i < NUM_CHOCOBO; i++) {
            if (!D_800B7564[i]) {
                continue;
            }
            model = &chocobos[i];
            count = D_800F5078.count;
            next = count + 1;
            D_800F5078.count = next;
            D_800F50A0[count] = &(models = D_800B1254.unk0)->models[model->unk90];
            D_800F5078.count = count + 2;
            D_800F50A0[next] = &models->models[model->unk92];
        }
        for (i = 0; i < D_800F5078.count; i++) {
            func_800AE534(D_800F50A0[i]);
        }
        if (++D_800F5040.unk0 == 60) {
            D_800B7A48.unk1C = -1;
            for (k = 0; k < NUM_CHOCOBO; k++) {
                if (chocobos[k].unkA0) {
                    chocobos[k].unk48 = 60;
                } else {
                    chocobos[k].unk48 = 10;
                }
            }
        }
        D_800F5078.unk8 = D_800B7530.pressed;
        if ((D_800B7530.pressed & 0x800) && D_800B7478) {
            D_800F5040.fadeSpeed = 0x10;
            g_AkaoCmd.opcode = AKAO_VOL_SLIDE_FROM_CURR;
            g_AkaoCmd.params[0] = 0x3C;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
        }
        D_800B7530.pressed = InputReadPadsRaw();
        if (D_800F5078.unk8 != D_800B7530.pressed && D_800B7A48.unk0 && (D_800B7530.pressed & PADk)) {
            chocobos->unk60 = !chocobos->unk60;
        }
        D_800F5078.unk18 = VSync(1);
        DrawSync(0);
        D_800F5078.unk1C = VSync(1);
        if (maxVSync1 < D_800F5078.unk18) {
            maxVSync1 = D_800F5078.unk18;
        }
        if (maxVSync2 < D_800F5078.unk1C) {
            maxVSync2 = D_800F5078.unk1C;
        }
        if (paused) {
            if (D_800B7530.pressed & PADl) {
                paused = !paused;
                frames++;
            }
        } else if (D_800B7530.pressed & PADm) {
            paused = !paused;
            frames++;
        }
        if (D_800B7A48.unk0 && frames > 10) {
            limit = chocobos->unk6C;
            chocobos->unk68 += limit / 150;
            if (limit < chocobos->unk68) {
                chocobos->unk68 = limit;
            }
            frames = 0;
        }
        if (D_800B7514 >= 0x100 && D_800F5040.fadeSpeed > 0) {
            D_800F5040.fadeSpeed = 0;
            break;
        }
        chocobos->unk94--;
        VSync(2);
        D_800B7530.unk8++;
        D_800F5040.unk28 = VSync(1);
        PutDrawEnv(&D_800F5074->draw);
        PutDispEnv(&D_800F5074->disp);
        ClearImage(&D_800F5074->draw.clip, segment->r, segment->g, segment->b);
        if (segment->flags & 1) {
            DrawOTag(D_800F5074->ot2);
        }
        SetSpadStack(getScratchAddr(0xFF));
        func_800A8AE8();
        ResetSpadStack();
        DrawOTag(&D_800F5074->ot[LEN(D_800F5074->ot) - 1]);
        ChocoboDrawFade();
    }

    n = 0;
    rank = 0;
    for (k = 0; k < NUM_CHOCOBO; k++) {
        if (rank < D_800B75CC[k].unk70) {
            rank = D_800B75CC[k].unk70;
        }
    }
    rank++;
    for (k = 0; k < NUM_CHOCOBO; k++) {
        if (!chocobos[k].unk70) {
            ids[n] = k;
            keys[n] = chocobos[k].unk0;
            n++;
        }
    }
    for (k = 0; k < n; k++) {
        for (j = 0; j < n - 1; j++) {
            if (keys[j] < keys[k]) {
                tmp = keys[j];
                keys[j] = keys[k];
                keys[k] = tmp;
                tmp = ids[j];
                ids[j] = ids[k];
                ids[k] = tmp;
            }
        }
    }
    for (k = 0; k < n; k++) {
        (&D_800B75CC[ids[k]])->unk70 = rank++;
    }
    g_AkaoCmd.opcode = AKAO_SET_REVERB_MUL;
    g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
    AkaoExec();
    if (!Savemap.memory_bank_3[8]) {
        func_800AC554();
    }
    Savemap.memory_bank_3[25] = D_800B75CC[0].unk70 - 1;
}

static void ChocoboDrawTrackProps(void) {
    s32 count;
    s32 end;
    s32 start;

    count = D_800F5078.track->count;
    start = (D_800B7598 + count) % count;
    end = (D_800F5078.unk4 + count) % count;
    PushMatrix();
    if (end < start) {
        ChocoboDrawTrackPropRange(0, end);
        ChocoboDrawTrackPropRange(start, count);
    } else {
        ChocoboDrawTrackPropRange(start, end);
    }

    PopMatrix();
}

static void ChocoboDrawTrackPropRange(s32 start, s32 end) {
    ChocoboTrackNode* prop;
    ChocoboModel* model;
    u16* anim;
    s32 flag;
    s32 i;
    u8 n;
    Unk800B1254* table;
    s32 id;

    for (i = start; i < end; i++) {
        table = &D_800B1254;
        n = D_800B7500[i].prop;
        if (n == 0) {
            continue;
        }
        prop = &D_800F5078.track->nodes[n - 1];
        id = prop->model;
        gte_ldv0(prop);
        gte_rtps();
        model = &table->unk0->models[id];
        anim = (u16*)(model->animOffset + model->data);
        gte_stflg(&flag);
        if (flag < 0) {
            continue;
        }
        model->x = prop->x;
        model->y = prop->y;
        model->z = prop->z;
        model->r = prop->r;
        model->g = prop->g;
        model->b = prop->b;
        PushMatrix();
        *(u_long*)getScratchAddr(0) = 3;
        func_800AF11C(&table->unk0->models[id], &D_800B7544, 0, prop->frame);
        D_800F50A0[D_800F5078.count++] = &table->unk0->models[id];
        PopMatrix();
        prop->frame += prop->step;
        if (prop->frame >= *anim) {
            if (prop->flags & 2) {
                prop->frame = 0;
            } else {
                prop->frame = *anim - 1;
            }
        }
        if (prop->frame < 0) {
            prop->frame = 0;
        }
    }
}
