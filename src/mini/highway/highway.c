//! PSYQ=3.3

#include "highway_private.h"

enum HighwayLba {
    LBA_MINI_H_TEX = 1985,   // MINI/H_TEX.BIN
    LBA_MINI_H_KAWAI = 2203, // MINI/H_KAWAI.BIN
    LBA_MINI_H_XBIN = 2250,  // MINI/H_XBIN.BIN
};

VECTOR g_HighwayCameraPos = {0, 0, 0, 0};
VECTOR g_HighwayCameraEye = {0, 0, 0, 0};
VECTOR g_HighwayCameraTarget = {0, 0, 0, 0};
VECTOR g_HighwayCameraTargetOffset = {0, 0, 0, 0};
VECTOR g_HighwayCameraEyeOffset = {0, 0, 0, 0};
s32 g_HighwayLeftWallOtBias = 0;
s32 g_HighwayRightWallOtBias = 0;
SVECTOR D_800B4220 = {0, 0, 0, 0};
VECTOR D_800B4228 = {0, 0, 0, 0};

// MINI/ files read by HighwayLoadAssets.
Yamada g_HighwayAssetFiles[3] = {
    {LBA_MINI_H_TEX, 0x6D000},
    {LBA_MINI_H_KAWAI, 0x17314},
    {LBA_MINI_H_XBIN, 0x7C474},
};

u_long g_HighwayMusicAddr = 0x80120000;
HighwayModelInfo* g_HighwayModelInfoAddr = (HighwayModelInfo*)0x8012200C;
JetTriangle* g_HighwayTrianglesAddr = (JetTriangle*)0x80123104;
JetQuad* g_HighwayQuadsAddr = (JetQuad*)0x8018D1CC;
HighwayCourse g_HighwayCourses[2] = {
    {
        (u8*)0x8019023C,
        {
            (HighwayPropScript*)0x80191160,
            (HighwayPropScript*)0x801917A8,
            (HighwayPropScript*)0x80191C94,
            (HighwayPropScript*)0x801920D8,
            (HighwayPropScript*)0x80192468,
            (HighwayPropScript*)0x801927BC,
            (HighwayPropScript*)0x801929A8,
            (HighwayPropScript*)0x80192B28,
            (HighwayPropScript*)0x80192CF0,
            (HighwayPropScript*)0x80192E4C,
        },
        (HighwayTrackCommand*)0x80192F9C,
        (u8*)0x801932C0,
        (HighwayEvent*)0x801933D0,
    },
    {
        (u8*)0x80193460,
        {
            (HighwayPropScript*)0x80194384,
            (HighwayPropScript*)0x80194978,
            (HighwayPropScript*)0x80194D74,
            (HighwayPropScript*)0x801951A0,
            (HighwayPropScript*)0x80195548,
            (HighwayPropScript*)0x801957F4,
            (HighwayPropScript*)0x8019595C,
            (HighwayPropScript*)0x80195AF4,
            (HighwayPropScript*)0x80195CD4,
            (HighwayPropScript*)0x80195E48,
        },
        (HighwayTrackCommand*)0x80195FB0,
        (u8*)0x801962BC,
        (HighwayEvent*)0x801963CC,
    },
};
s32* g_HighwayPathLengths = (s32*)0x80196454;
s32* g_HighwayPathOffsets = (s32*)0x8019647C;
u8* g_HighwayPathData = (u8*)0x801964A4;
u8* D_800B42DC[4] = {
    (u8*)0x8019B37C,
    (u8*)0x80120000,
    (u8*)0x80195FA0,
    (u8*)0x801A1FA0,
};
u_long* g_HighwayTimAddr[52] = {
    (u_long*)0x80120000, (u_long*)0x80122040, (u_long*)0x80124080, (u_long*)0x801260C0, (u_long*)0x80128100,
    (u_long*)0x8012C320, (u_long*)0x8012E360, (u_long*)0x801303A0, (u_long*)0x801323E0, (u_long*)0x80134420,
    (u_long*)0x80136460, (u_long*)0x801384A0, (u_long*)0x801388E0, (u_long*)0x80138D20, (u_long*)0x80139160,
    (u_long*)0x801395A0, (u_long*)0x801399E0, (u_long*)0x80139E20, (u_long*)0x8013A260, (u_long*)0x8013A6A0,
    (u_long*)0x8014A8C0, (u_long*)0x8015AAE0, (u_long*)0x8016AD00, (u_long*)0x8017AF20, (u_long*)0x8017B160,
    (u_long*)0x8017BAA0, (u_long*)0x8017BCE0, (u_long*)0x8017C520, (u_long*)0x8017C960, (u_long*)0x8017CDA0,
    (u_long*)0x8017D5E0, (u_long*)0x8017DE20, (u_long*)0x8017E660, (u_long*)0x80186E00, (u_long*)0x80187040,
    (u_long*)0x80187280, (u_long*)0x801874C0, (u_long*)0x80187800, (u_long*)0x80187B80, (u_long*)0x80188840,
    (u_long*)0x80189500, (u_long*)0x80189D40, (u_long*)0x8018A580, (u_long*)0x8018A9C0, (u_long*)0x8018AE00,
    (u_long*)0x8018B240, (u_long*)0x8018B680, (u_long*)0x8018BAC0, (u_long*)0x8018BF00, (u_long*)0x8018C340,
    (u_long*)0x8018C780, (u_long*)0x8018CBC0,
};

void HighwayDrawNode(HighwayBuffer* db, JetNode* node, s16 otIndex, s32 unused);

// Main loop: runs one frame per iteration and swaps the two buffers until the race ends.
void func_800A00D0(void) {
    // FAKE: preserves stack space; the original local declarations are unknown.
    s32 unused[6];
    HighwayBuffer* next;
    s32 i;

    D_800BE2F0 = 1;
    g_HighwayScore = 0;
    g_HighwayHighScore = 0;
    i = 0x174;
    g_HighwayArcadeMode = Savemap.memory_bank_2[0x73];
    g_HighwayHighScore = Savemap.memory_bank_3[0x6E] << 8;
    g_HighwayHighScore += Savemap.memory_bank_3[0x6D];
    HighwayLoadAssets();
    HighwayBuffersInit();
    HighwayInit();
    HighwayAudioInit();
    HighwayPlaySfx(0xB3, 4, 0);
    HighwayPlaySfx(0x12B, 3, 0);
    while (!g_HighwayExit) {
        g_HighwayKawaiOt = &g_HighwayBufferPtr->ot[g_HighwayOtOffset + 75];
        HighwayInputUpdate();
        HighwayEventsUpdate();
        HighwayTrackAdvance();
        HighwayRidersUpdate(g_HighwayTrackPos, g_HighwayBufferPtr);
        if (g_HighwayFadeMode) {
            g_HighwayFadeMode = HighwayDrawFade(g_HighwayBufferPtr, g_HighwayFadeMode);
        }
        HighwayDrawRoad(g_HighwayBufferPtr);
        HighwayOverlaysDraw(g_HighwayBufferPtr);
        HighwayObjectsUpdate(g_HighwayBufferPtr);
        if (g_HighwayArcadeMode == 1) {
            HighwayDrawScore(g_HighwayBufferPtr);
        } else {
            HighwayDrawGauges(g_HighwayBufferPtr);
        }
        HighwayCameraUpdate(g_HighwayTrackPos);
        DrawSync(0);
        VSync(2);
        ResetGraph(1);
        PutDrawEnv(&g_HighwayBufferPtr->draw);
        PutDispEnv(&g_HighwayBufferPtr->disp);
        ClearImage(&g_HighwayBufferPtr->draw.clip, 0, 0, 0);
        DrawOTag(&g_HighwayBufferPtr->bgOt[LEN(g_HighwayBufferPtr->bgOt) - 1]);
        DrawOTag(&g_HighwayBufferPtr->ot[LEN(g_HighwayBufferPtr->ot) - 1]);
        DrawOTag(&g_HighwayBufferPtr->hudOt[LEN(g_HighwayBufferPtr->hudOt) - 1]);
        next = g_HighwayBuffers;
        g_HighwayKawaiBufferIndex[0] ^= 1;
        if (g_HighwayBufferPtr == next) {
            next++;
        }
        g_HighwayBufferPtr = next;
        ClearOTagR(g_HighwayBufferPtr->ot, LEN(g_HighwayBufferPtr->ot));
        ClearOTagR(g_HighwayBufferPtr->bgOt, LEN(g_HighwayBufferPtr->bgOt));
        ClearOTagR(g_HighwayBufferPtr->hudOt, LEN(g_HighwayBufferPtr->hudOt));
        HighwayPrimCursorsReset(&g_HighwayBufferPtr->prims);
    }
    if (g_HighwayArcadeMode == 1) {
        if (g_HighwayScore > g_HighwayHighScore) {
            g_HighwayHighScore = g_HighwayScore;
        }
        Savemap.memory_bank_3[0x6C] = g_HighwayScore >> 8;
        Savemap.memory_bank_3[0x6E] = (g_HighwayHighScore & 0xFF00) >> 8;
        Savemap.memory_bank_3[0x6D] = g_HighwayHighScore;
        Savemap.memory_bank_3[0x6B] = g_HighwayScore;
    } else {
        Savemap.memory_bank_1[i++] = g_HighwayGauges[0].hp;
        Savemap.memory_bank_1[i++] = g_HighwayGauges[0].hp >> 8;
        Savemap.memory_bank_1[i++] = g_HighwayGauges[1].hp;
        Savemap.memory_bank_1[i++] = g_HighwayGauges[1].hp >> 8;
        Savemap.memory_bank_1[i++] = g_HighwayGauges[3].hp;
        Savemap.memory_bank_1[i++] = g_HighwayGauges[3].hp >> 8;
        Savemap.memory_bank_1[i++] = g_HighwayGauges[2].hp;
        Savemap.memory_bank_1[i++] = g_HighwayGauges[2].hp >> 8;
        Savemap.memory_bank_1[i++] = g_HighwayGauges[4].hp;
        Savemap.memory_bank_1[i] = g_HighwayGauges[4].hp >> 8;
    }
}

void func_800A0554(void) { D_800B3BF8 = VSync(1); }

void HighwayDrawRoad(HighwayBuffer* db) {
    HighwayRoadSegment* seg;
    HighwayRoadSegment* rec;
    HighwayScratchpadProj* prev;
    HighwayScratchpadProj* cur;
    POLY_FT4* poly;
    SVECTOR* vtx;
    SVECTOR* prevVtx;
    SVECTOR* curVtx;
    SVECTOR* tmpl;
    s32* sxy;
    HighwayQuadUv* uv;
    HighwayQuadUv* wallUv;
    HighwayQuadUv* nearUvL;
    HighwayQuadUv* nearUvR;
    OT_TYPE* farOt;
    OT_TYPE* ot;
    s32 i;
    s32 visLeft;
    s32 visRight;
    s32 last;
    s32 code;
    s32 idx;
    s32 prevIdx;
    s32 j;
    s32 k;
    s32 z;
    s32 avg;
    s32 adjacent;
    s32 n;
    // FAKE: preserves stack space; the original local declarations are unknown.
    s32 unused[32];

    seg = &g_HighwayRoad[g_HighwayTrackSegment % LEN(g_HighwayRoad)];
    poly = db->prims.ft4Cursor;
    seg->m.t[0] = seg->pos.vx - g_HighwayCameraPos.vx;
    seg->m.t[1] = seg->pos.vy - g_HighwayCameraPos.vy;
    seg->m.t[2] = seg->pos.vz - g_HighwayCameraPos.vz;
    last = 0;
    gte_SetRotMatrix(&g_HighwayCameraMatrices->m[0]);
    gte_ldclmv(&seg->m.m[0][0]);
    gte_rtir();
    gte_stclmv(&g_HighwayWorldMatrix->m[0][0]);
    gte_ldclmv(&seg->m.m[0][1]);
    gte_rtir();
    gte_stclmv(&g_HighwayWorldMatrix->m[0][1]);
    gte_ldclmv(&seg->m.m[0][2]);
    gte_rtir();
    gte_stclmv(&g_HighwayWorldMatrix->m[0][2]);
    gte_SetTransMatrix(&g_HighwayCameraMatrices->m[0]);
    gte_ldlv0(&seg->m.t[0]);
    gte_rt();
    gte_stlvl(&g_HighwayWorldMatrix->t[0]);
    gte_SetRotMatrix(g_HighwayWorldMatrix);
    gte_SetTransMatrix(g_HighwayWorldMatrix);
    vtx = seg->unk5C;
    tmpl = &g_HighwayCameraMatrices->rot[1];
    tmpl[0].vx = -(seg->width >> 1);
    tmpl[1].vx = seg->width >> 1;
    tmpl[2].vx = -(seg->width >> 1) - 30;
    tmpl[3].vx = (seg->width >> 1) + 30;
    for (j = 0; j < 4; j++, tmpl++, vtx++) {
        prev = &g_HighwayScratchpad->unk18C;
        gte_ldv0(tmpl);
        gte_rtps();
        gte_stsxy(&prev->sxy[j]);
        gte_stszotz(&prev->sz[j]);
        gte_stsv(vtx);
    }

    for (i = 1; i < 80; i++) {
        idx = (g_HighwayTrackSegment + i) % LEN(g_HighwayRoad);
        seg = &g_HighwayRoad[idx];
        if (g_HighwayRoad[idx].unk46[9]) {
            continue;
        }
        seg->m.t[0] = seg->pos.vx - g_HighwayCameraPos.vx;
        seg->m.t[1] = seg->pos.vy - g_HighwayCameraPos.vy;
        seg->m.t[2] = seg->pos.vz - g_HighwayCameraPos.vz;
        gte_SetRotMatrix(&g_HighwayCameraMatrices->m[0]);
        gte_ldclmv(&seg->m.m[0][0]);
        gte_rtir();
        gte_stclmv(&g_HighwayWorldMatrix->m[0][0]);
        gte_ldclmv(&seg->m.m[0][1]);
        gte_rtir();
        gte_stclmv(&g_HighwayWorldMatrix->m[0][1]);
        gte_ldclmv(&seg->m.m[0][2]);
        gte_rtir();
        gte_stclmv(&g_HighwayWorldMatrix->m[0][2]);
        gte_SetTransMatrix(&g_HighwayCameraMatrices->m[0]);
        gte_ldlv0(&seg->m.t[0]);
        gte_rt();
        gte_stlvl(&g_HighwayWorldMatrix->t[0]);
        gte_SetRotMatrix(g_HighwayWorldMatrix);
        gte_SetTransMatrix(g_HighwayWorldMatrix);
        vtx = seg->unk5C;
        tmpl = &g_HighwayCameraMatrices->rot[1];
        tmpl[0].vx = -(seg->width >> 1);
        tmpl[1].vx = seg->width >> 1;
        tmpl[2].vx = -(seg->width >> 1) - 30;
        tmpl[3].vx = (seg->width >> 1) + 30;
        for (j = 0; j < 4; j++, tmpl++, vtx++) {
            cur = &g_HighwayScratchpad->unk1CC;
            gte_ldv0(tmpl);
            gte_rtps();
            gte_stsxy(&cur->sxy[j]);
            gte_stszotz(&cur->sz[j]);
            gte_stsv(vtx);
        }
        rec = &g_HighwayRoad[idx];
        code = 0x80 - (cur->sz[0] >> 5);
        code = code + (code << 8) + (code << 16) + 0x2C000000;
        for (k = 0; k < rec->nodeCount; k++) {
            HighwayDrawNode(db, rec->nodes[k], 200, 0);
        }
        visLeft = HighwaySVectorInsidePlanes(&rec->unk5C[2]);
        visRight = HighwaySVectorInsidePlanes(&rec->unk5C[3]);
        if (SquareRoot0(rec->unk5C[0].vx * rec->unk5C[0].vx + rec->unk5C[0].vy * rec->unk5C[0].vy +
                        rec->unk5C[0].vz * rec->unk5C[0].vz) < 1000) {
            visRight = 1;
        }
        adjacent = i - 1 == last;
        if (visLeft | visRight | adjacent) {
            uv = &g_HighwayRoadUv[g_HighwayRoad[idx].unk46[0]];
            wallUv = &g_HighwayWallUv[g_HighwayRoad[idx].unk46[3]];
            nearUvL = &g_HighwayNearUvLeft[g_HighwayRoad[idx].unk46[3]];
            nearUvR = &g_HighwayNearUvRight[g_HighwayRoad[idx].unk46[3]];
            avg = (cur->sz[0] + cur->sz[1]) >> 1;
            if (avg > 1000) {
                z = prev->sz[0];
                if (z < prev->sz[1]) {
                    z = prev->sz[1];
                }
                if (z < cur->sz[0]) {
                    z = cur->sz[0];
                }
                if (z < cur->sz[1]) {
                    z = cur->sz[1];
                }
                farOt = &db->ot[z];
                if (z < 0x1000 - 0x55) {
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[0];
                    *(u32*)&poly->x1 = g_HighwayScratchpad->unk18C.sxy[1];
                    *(u32*)&poly->x2 = g_HighwayScratchpad->unk1CC.sxy[0];
                    *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[1];
                    addPrim(&farOt[0x55], poly);
                    poly++;
                }
                z = prev->sz[2];
                if (z < prev->sz[0]) {
                    z = prev->sz[0];
                }
                if (z < cur->sz[2]) {
                    z = cur->sz[2];
                }
                if (z < cur->sz[0]) {
                    z = cur->sz[0];
                }
                if (z < 0x1000 - g_HighwayLeftWallOtBias) {
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&wallUv->u0;
                    *(u32*)&poly->u1 = *(u32*)&wallUv->u1;
                    *(u32*)&poly->u2 = *(u32*)&wallUv->u2;
                    *(u32*)&poly->u3 = *(u32*)&wallUv->u3;
                    *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[2];
                    *(u32*)&poly->x1 = g_HighwayScratchpad->unk1CC.sxy[2];
                    *(u32*)&poly->x2 = g_HighwayScratchpad->unk18C.sxy[0];
                    *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[0];
                    addPrim(db->ot + z + g_HighwayLeftWallOtBias, poly);
                    poly++;
                }
                z = prev->sz[3];
                if (z < prev->sz[1]) {
                    z = prev->sz[1];
                }
                if (z < cur->sz[3]) {
                    z = cur->sz[3];
                }
                if (z < cur->sz[1]) {
                    z = cur->sz[1];
                }
                if (z < 0x1000 - g_HighwayRightWallOtBias) {
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&wallUv->u0;
                    *(u32*)&poly->u1 = *(u32*)&wallUv->u1;
                    *(u32*)&poly->u2 = *(u32*)&wallUv->u2;
                    *(u32*)&poly->u3 = *(u32*)&wallUv->u3;
                    *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[3];
                    *(u32*)&poly->x1 = g_HighwayScratchpad->unk1CC.sxy[3];
                    *(u32*)&poly->x2 = g_HighwayScratchpad->unk18C.sxy[1];
                    *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[1];
                    addPrim(db->ot + z + g_HighwayRightWallOtBias, poly);
                    poly++;
                }
            } else if (avg < 300) {
                gte_SetRotMatrix(&g_HighwayOverlayMatrix);
                gte_SetTransMatrix(&g_HighwayOverlayMatrix);
                prevIdx = idx - 1;
                if (prevIdx < 0) {
                    prevIdx += 80;
                }
                z = prev->sz[0];
                if (z < prev->sz[1]) {
                    z = prev->sz[1];
                }
                if (z < cur->sz[0]) {
                    z = cur->sz[0];
                }
                if (z < cur->sz[1]) {
                    z = cur->sz[1];
                }
                z += 0x55;
                ot = db->ot + z + 0x55;
                if (z - 0x55 > 0x28) {
                    prevVtx = g_HighwayRoad[prevIdx].unk5C;
                    curVtx = g_HighwayRoad[idx].unk5C;
                    g_HighwaySubdivVerts[0]->vx = (prevVtx[1].vx + prevVtx[0].vx) >> 1;
                    g_HighwaySubdivVerts[0]->vy = (prevVtx[1].vy + prevVtx[0].vy) >> 1;
                    g_HighwaySubdivVerts[0]->vz = (prevVtx[1].vz + prevVtx[0].vz) >> 1;
                    g_HighwaySubdivVerts[1]->vx = (prevVtx[0].vx + curVtx[0].vx) >> 1;
                    g_HighwaySubdivVerts[1]->vy = (prevVtx[0].vy + curVtx[0].vy) >> 1;
                    g_HighwaySubdivVerts[1]->vz = (prevVtx[0].vz + curVtx[0].vz) >> 1;
                    g_HighwaySubdivVerts[2]->vx = (prevVtx[1].vx + curVtx[1].vx) >> 1;
                    g_HighwaySubdivVerts[2]->vy = (prevVtx[1].vy + curVtx[1].vy) >> 1;
                    g_HighwaySubdivVerts[2]->vz = (prevVtx[1].vz + curVtx[1].vz) >> 1;
                    g_HighwaySubdivVerts[3]->vx = (g_HighwaySubdivVerts[1]->vx + g_HighwaySubdivVerts[2]->vx) >> 1;
                    g_HighwaySubdivVerts[3]->vy = (g_HighwaySubdivVerts[1]->vy + g_HighwaySubdivVerts[2]->vy) >> 1;
                    g_HighwaySubdivVerts[3]->vz = (g_HighwaySubdivVerts[1]->vz + g_HighwaySubdivVerts[2]->vz) >> 1;
                    g_HighwaySubdivVerts[4]->vx = (g_HighwaySubdivVerts[0]->vx + prevVtx[0].vx) >> 1;
                    g_HighwaySubdivVerts[4]->vy = (g_HighwaySubdivVerts[0]->vy + prevVtx[0].vy) >> 1;
                    g_HighwaySubdivVerts[4]->vz = (g_HighwaySubdivVerts[0]->vz + prevVtx[0].vz) >> 1;
                    g_HighwaySubdivVerts[5]->vx = (g_HighwaySubdivVerts[0]->vx + prevVtx[1].vx) >> 1;
                    g_HighwaySubdivVerts[5]->vy = (g_HighwaySubdivVerts[0]->vy + prevVtx[1].vy) >> 1;
                    g_HighwaySubdivVerts[5]->vz = (g_HighwaySubdivVerts[0]->vz + prevVtx[1].vz) >> 1;
                    g_HighwaySubdivVerts[6]->vx = (g_HighwaySubdivVerts[1]->vx + g_HighwaySubdivVerts[3]->vx) >> 1;
                    g_HighwaySubdivVerts[6]->vy = (g_HighwaySubdivVerts[1]->vy + g_HighwaySubdivVerts[3]->vy) >> 1;
                    g_HighwaySubdivVerts[6]->vz = (g_HighwaySubdivVerts[1]->vz + g_HighwaySubdivVerts[3]->vz) >> 1;
                    g_HighwaySubdivVerts[7]->vx = (g_HighwaySubdivVerts[2]->vx + g_HighwaySubdivVerts[3]->vx) >> 1;
                    g_HighwaySubdivVerts[7]->vy = (g_HighwaySubdivVerts[2]->vy + g_HighwaySubdivVerts[3]->vy) >> 1;
                    g_HighwaySubdivVerts[7]->vz = (g_HighwaySubdivVerts[2]->vz + g_HighwaySubdivVerts[3]->vz) >> 1;
                    g_HighwaySubdivVerts[8]->vx = (curVtx[1].vx + curVtx[0].vx) >> 1;
                    g_HighwaySubdivVerts[8]->vy = (curVtx[1].vy + curVtx[0].vy) >> 1;
                    g_HighwaySubdivVerts[8]->vz = (curVtx[1].vz + curVtx[0].vz) >> 1;
                    g_HighwaySubdivVerts[9]->vx = (curVtx[1].vx + g_HighwaySubdivVerts[8]->vx) >> 1;
                    g_HighwaySubdivVerts[9]->vy = (curVtx[1].vy + g_HighwaySubdivVerts[8]->vy) >> 1;
                    g_HighwaySubdivVerts[9]->vz = (curVtx[1].vz + g_HighwaySubdivVerts[8]->vz) >> 1;
                    g_HighwaySubdivVerts[10]->vx = (curVtx[0].vx + g_HighwaySubdivVerts[8]->vx) >> 1;
                    g_HighwaySubdivVerts[10]->vy = (curVtx[0].vy + g_HighwaySubdivVerts[8]->vy) >> 1;
                    g_HighwaySubdivVerts[10]->vz = (curVtx[0].vz + g_HighwaySubdivVerts[8]->vz) >> 1;
                    vtx = g_HighwayScratchpad->unk108;
                    sxy = g_HighwayScratchpad->unk160;
                    for (j = 0; j < 11; j++, sxy++, vtx++) {
                        gte_ldv0(vtx);
                        gte_rtps();
                        gte_stsxy(sxy);
                    }
                    n = rec->unk46[0] * 8;
                    uv = &g_HighwayRoadUvEighths[n++];
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[0];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[4];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[1];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[6];
                    *(u32*)&poly->r0 = code;
                    addPrim(ot, poly);
                    poly++;
                    uv = &g_HighwayRoadUvEighths[n++];
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[4];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[0];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[6];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[3];
                    *(u32*)&poly->r0 = code;
                    addPrim(ot, poly);
                    poly++;
                    uv = &g_HighwayRoadUvEighths[n++];
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[0];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[5];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[3];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[7];
                    *(u32*)&poly->r0 = code;
                    addPrim(ot, poly);
                    poly++;
                    uv = &g_HighwayRoadUvEighths[n++];
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[5];
                    *(u32*)&poly->x1 = g_HighwayScratchpad->unk18C.sxy[1];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[7];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[2];
                    addPrim(ot, poly);
                    poly++;
                    uv = &g_HighwayRoadUvEighths[n++];
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[1];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[6];
                    *(u32*)&poly->x2 = g_HighwayScratchpad->unk1CC.sxy[0];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[10];
                    *(u32*)&poly->r0 = code;
                    addPrim(ot, poly);
                    poly++;
                    uv = &g_HighwayRoadUvEighths[n++];
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[6];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[3];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[10];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[8];
                    *(u32*)&poly->r0 = code;
                    addPrim(ot, poly);
                    poly++;
                    uv = &g_HighwayRoadUvEighths[n++];
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[3];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[7];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[8];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[9];
                    *(u32*)&poly->r0 = code;
                    addPrim(ot, poly);
                    poly++;
                    uv = &g_HighwayRoadUvEighths[n++];
                    *(u32*)&poly->u0 = *(u32*)&uv->u0;
                    *(u32*)&poly->u1 = *(u32*)&uv->u1;
                    *(u32*)&poly->u2 = *(u32*)&uv->u2;
                    *(u32*)&poly->u3 = *(u32*)&uv->u3;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[7];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[2];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[9];
                    *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[1];
                    *(u32*)&poly->r0 = code;
                    addPrim(ot, poly);
                    poly++;
                }
                z = prev->sz[2];
                if (z < prev->sz[0]) {
                    z = prev->sz[0];
                }
                if (z < cur->sz[2]) {
                    z = cur->sz[2];
                }
                if (z < cur->sz[0]) {
                    z = cur->sz[0];
                }
                if (z > 0x46) {
                    prevVtx = g_HighwayRoad[prevIdx].unk5C;
                    curVtx = g_HighwayRoad[idx].unk5C;
                    g_HighwaySubdivVerts[0]->vx = (prevVtx[0].vx + prevVtx[2].vx) >> 1;
                    g_HighwaySubdivVerts[0]->vy = (prevVtx[0].vy + prevVtx[2].vy) >> 1;
                    g_HighwaySubdivVerts[0]->vz = (prevVtx[0].vz + prevVtx[2].vz) >> 1;
                    g_HighwaySubdivVerts[1]->vx = (curVtx[0].vx + curVtx[2].vx) >> 1;
                    g_HighwaySubdivVerts[1]->vy = (curVtx[0].vy + curVtx[2].vy) >> 1;
                    g_HighwaySubdivVerts[1]->vz = (curVtx[0].vz + curVtx[2].vz) >> 1;
                    vtx = g_HighwayScratchpad->unk108;
                    sxy = g_HighwayScratchpad->unk160;
                    for (j = 0; j < 2; j++, sxy++, vtx++) {
                        gte_ldv0(vtx);
                        gte_rtps();
                        gte_stsxy(sxy);
                    }
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&nearUvL->u0;
                    *(u32*)&poly->u1 = *(u32*)&nearUvL->u1;
                    *(u32*)&poly->u2 = *(u32*)&nearUvL->u2;
                    *(u32*)&poly->u3 = *(u32*)&nearUvL->u3;
                    *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[2];
                    *(u32*)&poly->x1 = g_HighwayScratchpad->unk1CC.sxy[2];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[0];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[1];
                    addPrim(db->ot + z + g_HighwayLeftWallOtBias, poly);
                    poly++;
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&nearUvR->u0;
                    *(u32*)&poly->u1 = *(u32*)&nearUvR->u1;
                    *(u32*)&poly->u2 = *(u32*)&nearUvR->u2;
                    *(u32*)&poly->u3 = *(u32*)&nearUvR->u3;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[0];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[1];
                    *(u32*)&poly->x2 = g_HighwayScratchpad->unk18C.sxy[0];
                    *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[0];
                    addPrim(db->ot + z + g_HighwayLeftWallOtBias, poly);
                    poly++;
                }
                z = prev->sz[3];
                if (z < prev->sz[0]) {
                    z = prev->sz[1];
                }
                if (z < cur->sz[2]) {
                    z = cur->sz[3];
                }
                if (z < cur->sz[0]) {
                    z = cur->sz[1];
                }
                if (z > 0x46) {
                    SVECTOR* pv;
                    SVECTOR* cv;

                    pv = g_HighwayRoad[prevIdx].unk5C;
                    cv = g_HighwayRoad[idx].unk5C;
                    g_HighwaySubdivVerts[0]->vx = (pv[1].vx + pv[3].vx) >> 1;
                    g_HighwaySubdivVerts[0]->vy = (pv[1].vy + pv[3].vy) >> 1;
                    g_HighwaySubdivVerts[0]->vz = (pv[1].vz + pv[3].vz) >> 1;
                    g_HighwaySubdivVerts[1]->vx = (cv[1].vx + cv[3].vx) >> 1;
                    g_HighwaySubdivVerts[1]->vy = (cv[1].vy + cv[3].vy) >> 1;
                    g_HighwaySubdivVerts[1]->vz = (cv[1].vz + cv[3].vz) >> 1;
                    vtx = g_HighwayScratchpad->unk108;
                    sxy = g_HighwayScratchpad->unk160;
                    for (j = 0; j < 2; j++, sxy++, vtx++) {
                        gte_ldv0(vtx);
                        gte_rtps();
                        gte_stsxy(sxy);
                    }
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&nearUvL->u0;
                    *(u32*)&poly->u1 = *(u32*)&nearUvL->u1;
                    *(u32*)&poly->u2 = *(u32*)&nearUvL->u2;
                    *(u32*)&poly->u3 = *(u32*)&nearUvL->u3;
                    *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[3];
                    *(u32*)&poly->x1 = g_HighwayScratchpad->unk1CC.sxy[3];
                    *(u32*)&poly->x2 = *g_HighwaySubdivSxy[0];
                    *(u32*)&poly->x3 = *g_HighwaySubdivSxy[1];
                    addPrim(db->ot + z + g_HighwayRightWallOtBias, poly);
                    poly++;
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&nearUvR->u0;
                    *(u32*)&poly->u1 = *(u32*)&nearUvR->u1;
                    *(u32*)&poly->u2 = *(u32*)&nearUvR->u2;
                    *(u32*)&poly->u3 = *(u32*)&nearUvR->u3;
                    *(u32*)&poly->x0 = *g_HighwaySubdivSxy[0];
                    *(u32*)&poly->x1 = *g_HighwaySubdivSxy[1];
                    *(u32*)&poly->x2 = g_HighwayScratchpad->unk18C.sxy[1];
                    *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[1];
                    addPrim(db->ot + z + g_HighwayRightWallOtBias, poly);
                    poly++;
                }
            } else {
                gte_SetRotMatrix(&g_HighwayOverlayMatrix);
                gte_SetTransMatrix(&g_HighwayOverlayMatrix);
                prevIdx = idx - 1;
                if (prevIdx < 0) {
                    prevIdx += 80;
                }
                z = prev->sz[0];
                if (z < prev->sz[1]) {
                    z = prev->sz[1];
                }
                if (z < cur->sz[0]) {
                    z = cur->sz[0];
                }
                if (z < cur->sz[1]) {
                    z = cur->sz[1];
                }
                ot = db->ot + z + 0xAA;
                prevVtx = g_HighwayRoad[prevIdx].unk5C;
                curVtx = g_HighwayRoad[idx].unk5C;
                g_HighwaySubdivVerts[0]->vx = (prevVtx[1].vx + prevVtx[0].vx) >> 1;
                g_HighwaySubdivVerts[0]->vy = (prevVtx[1].vy + prevVtx[0].vy) >> 1;
                g_HighwaySubdivVerts[0]->vz = (prevVtx[1].vz + prevVtx[0].vz) >> 1;
                g_HighwaySubdivVerts[1]->vx = (curVtx[0].vx + curVtx[1].vx) >> 1;
                g_HighwaySubdivVerts[1]->vy = (curVtx[0].vy + curVtx[1].vy) >> 1;
                g_HighwaySubdivVerts[1]->vz = (curVtx[0].vz + curVtx[1].vz) >> 1;
                vtx = g_HighwayScratchpad->unk108;
                sxy = g_HighwayScratchpad->unk160;
                for (j = 0; j < 2; j++, sxy++, vtx++) {
                    gte_ldv0(vtx);
                    gte_rtps();
                    gte_stsxy(sxy);
                }
                n = rec->unk46[0] * 2;
                uv = &g_HighwayRoadUvHalves[n++];
                *(u32*)&poly->u0 = *(u32*)&uv->u0;
                *(u32*)&poly->u1 = *(u32*)&uv->u1;
                *(u32*)&poly->u2 = *(u32*)&uv->u2;
                *(u32*)&poly->u3 = *(u32*)&uv->u3;
                *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[0];
                *(u32*)&poly->x1 = *g_HighwaySubdivSxy[0];
                *(u32*)&poly->x2 = g_HighwayScratchpad->unk1CC.sxy[0];
                *(u32*)&poly->x3 = *g_HighwaySubdivSxy[1];
                *(u32*)&poly->r0 = code;
                addPrim(ot, poly);
                poly++;
                uv = &g_HighwayRoadUvHalves[n++];
                *(u32*)&poly->u0 = *(u32*)&uv->u0;
                *(u32*)&poly->u1 = *(u32*)&uv->u1;
                *(u32*)&poly->u2 = *(u32*)&uv->u2;
                *(u32*)&poly->u3 = *(u32*)&uv->u3;
                *(u32*)&poly->x0 = *g_HighwaySubdivSxy[0];
                *(u32*)&poly->x1 = g_HighwayScratchpad->unk18C.sxy[1];
                *(u32*)&poly->x2 = *g_HighwaySubdivSxy[1];
                *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[1];
                *(u32*)&poly->r0 = code;
                addPrim(ot, poly);
                poly++;
                z = prev->sz[2];
                if (z < prev->sz[0]) {
                    z = prev->sz[0];
                }
                if (z < cur->sz[2]) {
                    z = cur->sz[2];
                }
                if (z < cur->sz[0]) {
                    z = cur->sz[0];
                }
                if (z < 0x1000 - g_HighwayLeftWallOtBias) {
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&wallUv->u0;
                    *(u32*)&poly->u1 = *(u32*)&wallUv->u1;
                    *(u32*)&poly->u2 = *(u32*)&wallUv->u2;
                    *(u32*)&poly->u3 = *(u32*)&wallUv->u3;
                    *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[2];
                    *(u32*)&poly->x1 = g_HighwayScratchpad->unk1CC.sxy[2];
                    *(u32*)&poly->x2 = g_HighwayScratchpad->unk18C.sxy[0];
                    *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[0];
                    addPrim(db->ot + z + g_HighwayLeftWallOtBias, poly);
                    poly++;
                }
                z = prev->sz[3];
                if (z < prev->sz[1]) {
                    z = prev->sz[1];
                }
                if (z < cur->sz[3]) {
                    z = cur->sz[3];
                }
                if (z < cur->sz[1]) {
                    z = cur->sz[1];
                }
                if (z < 0x1000 - g_HighwayRightWallOtBias) {
                    *(u32*)&poly->r0 = code;
                    *(u32*)&poly->u0 = *(u32*)&wallUv->u0;
                    *(u32*)&poly->u1 = *(u32*)&wallUv->u1;
                    *(u32*)&poly->u2 = *(u32*)&wallUv->u2;
                    *(u32*)&poly->u3 = *(u32*)&wallUv->u3;
                    *(u32*)&poly->x0 = g_HighwayScratchpad->unk18C.sxy[3];
                    *(u32*)&poly->x1 = g_HighwayScratchpad->unk1CC.sxy[3];
                    *(u32*)&poly->x2 = g_HighwayScratchpad->unk18C.sxy[1];
                    *(u32*)&poly->x3 = g_HighwayScratchpad->unk1CC.sxy[1];
                    addPrim(db->ot + z + g_HighwayRightWallOtBias, poly);
                    poly++;
                }
            }
        }
        if (visLeft | visRight) {
            last = i;
        }
        prev->sxy[0] = cur->sxy[0];
        prev->sxy[1] = cur->sxy[1];
        prev->sxy[2] = cur->sxy[2];
        prev->sxy[3] = cur->sxy[3];
        prev->sz[0] = cur->sz[0];
        prev->sz[1] = cur->sz[1];
        prev->sz[2] = cur->sz[2];
        prev->sz[3] = cur->sz[3];
    }
    db->prims.ft4Cursor = poly;
}

void HighwayDrawNode(HighwayBuffer* db, JetNode* node, s16 otIndex, s32 unused) {
    JetModelDrawArgs args;
    POLY_FT3* prim;
    JetQuad* quad;
    MATRIX* m;
    s32 visible;
    s32 i;

    m = g_HighwayWorldMatrix;
    m->m[0][0] = node->m.m[0][0];
    m->m[0][1] = node->m.m[0][1];
    m->m[0][2] = node->m.m[0][2];
    m->m[1][0] = node->m.m[1][0];
    m->m[1][1] = node->m.m[1][1];
    m->m[1][2] = node->m.m[1][2];
    m->m[2][0] = node->m.m[2][0];
    m->m[2][1] = node->m.m[2][1];
    m->m[2][2] = node->m.m[2][2];
    m->t[0] = node->m.t[0];
    m->t[1] = node->m.t[1];
    m->t[2] = node->m.t[2];
    if (node->parent != &g_HighwayRootNode) {
        CompMatrix(&node->parent->m, m, m);
    }
    g_HighwayWorldMatrix->t[0] -= g_HighwayCameraPos.vx;
    g_HighwayWorldMatrix->t[1] -= g_HighwayCameraPos.vy;
    g_HighwayWorldMatrix->t[2] -= g_HighwayCameraPos.vz;
    gte_SetRotMatrix(&g_HighwayCameraMatrices->m[0]);
    gte_ldclmv(&g_HighwayWorldMatrix->m[0][0]);
    gte_rtir();
    gte_stclmv(&g_HighwayWorldMatrix->m[0][0]);
    gte_ldclmv(&g_HighwayWorldMatrix->m[0][1]);
    gte_rtir();
    gte_stclmv(&g_HighwayWorldMatrix->m[0][1]);
    gte_ldclmv(&g_HighwayWorldMatrix->m[0][2]);
    gte_rtir();
    gte_stclmv(&g_HighwayWorldMatrix->m[0][2]);
    gte_SetTransMatrix(&g_HighwayCameraMatrices->m[0]);
    gte_ldlv0(&g_HighwayWorldMatrix->t[0]);
    gte_rt();
    gte_stlvl(&g_HighwayWorldMatrix->t[0]);
    gte_SetRotMatrix(g_HighwayWorldMatrix);
    gte_SetTransMatrix(g_HighwayWorldMatrix);
    visible = HighwaySphereInsidePlanes((VECTOR*)g_HighwayWorldMatrix->t, node->model->unk1C);
    visible = g_HighwayWorldMatrix->t[2] < g_HighwayFogFar + 1000 ? visible & 1 : 0;
    if (!visible) {
        return;
    }
    if (node->model->quadCount) {
        quad = node->model->quads;
        prim = db->prims.ft3Cursor;
        for (i = 0; i < node->model->quadCount; i++) {
            prim = func_800B404C(quad, prim, &db->ot[otIndex]);
            quad++;
        }
        db->prims.ft3Cursor = prim;
    } else {
        args.tris = node->model->tris;
        args.prim = db->prims.g3Cursor;
        args.ot = &db->ot[otIndex];
        args.model = node->model;
        db->prims.g3Cursor = func_800B3BFC(&args);
    }
}

void HighwayDrawOverlayQuads(HighwayBuffer* db, JetNode* node, s16 depth) {
    POLY_FT3* prim;
    JetQuad* quad;
    s32 i;

    g_HighwayWorldMatrix->t[0] = 0;
    g_HighwayWorldMatrix->t[1] = depth;
    g_HighwayWorldMatrix->t[2] = 0;
    if (!g_HighwayCameraRolled) {
        gte_SetRotMatrix(&g_HighwayCameraMatrices->m[4]);
    } else {
        gte_SetRotMatrix(&g_HighwayCameraMatrices->m[0]);
    }
    gte_SetTransMatrix(g_HighwayWorldMatrix);
    prim = db->prims.ft3Cursor;
    quad = node->model->quads;
    for (i = 0; i < node->model->quadCount; i++) {
        prim = func_800B3EC0(quad, prim, db->bgOt);
        quad++;
    }
    db->prims.ft3Cursor = prim;
}

void HighwayDrawOverlayTris(HighwayBuffer* db, JetNode* node, s16 depth) {
    JetModelDrawArgs args;

    g_HighwayWorldMatrix->t[0] = 0;
    g_HighwayWorldMatrix->t[1] = depth;
    g_HighwayWorldMatrix->t[2] = 0;
    if (!g_HighwayCameraRolled) {
        gte_SetRotMatrix(&g_HighwayCameraMatrices->m[4]);
    } else {
        gte_SetRotMatrix(&g_HighwayCameraMatrices->m[0]);
    }
    gte_SetTransMatrix(g_HighwayWorldMatrix);
    args.tris = node->model->tris;
    args.prim = db->prims.g3Cursor;
    args.ot = db->bgOt;
    args.model = node->model;
    db->prims.g3Cursor = func_800B3D28(&args);
}

MATRIX* HighwayNodeViewMatrix(HighwayBuffer* db, JetNode* node, MATRIX* m) {
    m->m[0][0] = node->m.m[0][0];
    m->m[0][1] = node->m.m[0][1];
    m->m[0][2] = node->m.m[0][2];
    m->m[1][0] = node->m.m[1][0];
    m->m[1][1] = node->m.m[1][1];
    m->m[1][2] = node->m.m[1][2];
    m->m[2][0] = node->m.m[2][0];
    m->m[2][1] = node->m.m[2][1];
    m->m[2][2] = node->m.m[2][2];
    m->t[0] = node->m.t[0];
    m->t[1] = node->m.t[1];
    m->t[2] = node->m.t[2];
    if (node->parent != &g_HighwayRootNode) {
        CompMatrix(&node->parent->m, m, m);
    }
    m->t[0] -= g_HighwayCameraPos.vx;
    m->t[1] -= g_HighwayCameraPos.vy;
    m->t[2] -= g_HighwayCameraPos.vz;
    gte_SetRotMatrix(&g_HighwayCameraMatrices->m[0]);
    gte_ldclmv(&m->m[0][0]);
    gte_rtir();
    gte_stclmv(&m->m[0][0]);
    gte_ldclmv(&m->m[0][1]);
    gte_rtir();
    gte_stclmv(&m->m[0][1]);
    gte_ldclmv(&m->m[0][2]);
    gte_rtir();
    gte_stclmv(&m->m[0][2]);
    gte_SetTransMatrix(&g_HighwayCameraMatrices->m[0]);
    gte_ldlv0(&m->t[0]);
    gte_rt();
    gte_stlvl(&m->t[0]);
    gte_SetRotMatrix(m);
    gte_SetTransMatrix(m);
    if (HighwayVectorInsidePlanes((VECTOR*)m->t)) {
        return m;
    }
    return NULL;
}

void HighwayDrawScore(HighwayBuffer* db) {
    HighwayDrawNumberSmall(db, g_HighwayHighScore, 0x55, 0x18, 1);
    HighwayDrawSprite(db, 2, 0x12, 0x14, 0x6C, 0x14, 0, 0x20, 0x6C, 0x14, 0);
    HighwayDrawNumber(db, g_HighwayScore, 0xF8, 0x1A, 1);
    HighwayDrawSprite(db, 1, 0xC8, 0x14, 0x6D, 0x1B, 0, 0, 0x6D, 0x1B, 0);
    if (g_HighwayBanner == 1) {
        HighwayDrawSprite(db, 1, 0x64, 0x50, 0x76, 0x3A, 0, 0x3C, 0x76, 0x3A, 0);
    }
    if (g_HighwayBanner == 2) {
        HighwayDrawSprite(db, 1, 0x64, 0x50, 0x75, 0x38, 0x82, 0x3C, 0x75, 0x38, 0);
    }
    if (g_HighwayBanner == 3) {
        HighwayDrawSprite(db, 1, 0x64, 0x50, 0x73, 0x3E, 0, 0x81, 0x73, 0x3E, 0);
    }
}

void HighwayInit(void) {
    s32 i;

    SetDrawMode(&D_80116358, 0, 1, GetTPage(1, 1, 0x300, 0), NULL);
    HighwayNodesInit();
    HighwayScratchpadInit();
    HighwayModelsReset();
    for (i = 0; i < LEN(g_HighwayModelTable); i++) {
        g_HighwayModelTable[i] = HighwayModelBuild(i);
    }
    HighwayInputReset();
    HighwayRidersInit();
    HighwayTrackReset();
    HighwayFrustumInit();
    HighwayCameraInit();
    HighwayRaceInit();
    HighwayOverlaysInit();
    HighwayKawaiModelsInit();
    HighwayObjectsInit();
    g_HighwayCameraMatrices->rot[1].vy = 0;
    g_HighwayCameraMatrices->rot[1].vz = 0;
    g_HighwayCameraMatrices->rot[2].vy = 0;
    g_HighwayCameraMatrices->rot[2].vz = 0;
    g_HighwayCameraMatrices->rot[3].vy = -90;
    g_HighwayCameraMatrices->rot[3].vz = 0;
    g_HighwayCameraMatrices->rot[4].vy = -90;
    g_HighwayCameraMatrices->rot[4].vz = 0;
    for (i = 0; i < LEN(g_HighwayRoad); i++) {
        g_HighwayRoad[i].unk46[3] = i % 8;
        HighwayTrackGenerateSegment();
        g_HighwayTrackSegment++;
    }
}

void HighwayBuffersInit(void) {
    SetDefDrawEnv(&g_HighwayBuffers[0].draw, 0, 0, 320, 240);
    SetDefDispEnv(&g_HighwayBuffers[0].disp, 0, 240, 320, 240);
    SetDefDrawEnv(&g_HighwayBuffers[1].draw, 0, 240, 320, 240);
    SetDefDispEnv(&g_HighwayBuffers[1].disp, 0, 0, 320, 240);
    g_HighwayBuffers[0].draw.isbg = 0;
    g_HighwayBuffers[1].draw.isbg = 0;
    setRGB0(&g_HighwayBuffers[0].draw, 0, 0, 0);
    setRGB0(&g_HighwayBuffers[1].draw, 0, 0, 0);
    SetGeomOffset(160, 120);
    SetGeomScreen(300);
    SetDispMask(1);
    SetBackColor(0x80, 0x80, 0x80);
    SetFarColor(0, 0, 0);
    SetFogNearFar(6500, 11500, 300);
    HighwayPrimsInit(&g_HighwayBuffers[0].prims);
    HighwayPrimsInit(&g_HighwayBuffers[1].prims);
    HighwayPrimCursorsReset(&g_HighwayBuffers[0].prims);
    HighwayPrimCursorsReset(&g_HighwayBuffers[1].prims);
    ClearOTagR(g_HighwayBuffers[0].ot, LEN(g_HighwayBuffers[0].ot));
    ClearOTagR(g_HighwayBuffers[1].ot, LEN(g_HighwayBuffers[1].ot));
    ClearOTagR(g_HighwayBuffers[0].bgOt, LEN(g_HighwayBuffers[0].bgOt));
    ClearOTagR(g_HighwayBuffers[1].bgOt, LEN(g_HighwayBuffers[1].bgOt));
    ClearOTagR(g_HighwayBuffers[0].hudOt, LEN(g_HighwayBuffers[0].hudOt));
    ClearOTagR(g_HighwayBuffers[1].hudOt, LEN(g_HighwayBuffers[1].hudOt));
    g_HighwayBufferPtr = &g_HighwayBuffers[0];
}

void HighwayBufferReset(void) {
    ClearOTagR(g_HighwayBufferPtr->ot, LEN(g_HighwayBufferPtr->ot));
    ClearOTagR(g_HighwayBufferPtr->bgOt, LEN(g_HighwayBufferPtr->bgOt));
    ClearOTagR(g_HighwayBufferPtr->hudOt, LEN(g_HighwayBufferPtr->hudOt));
    HighwayPrimCursorsReset(&g_HighwayBufferPtr->prims);
}

void HighwayPrimsInit(HighwayPrimBuffer* prims) {
    s32 i;

    for (i = 0; i < LEN(prims->f3); i++) {
        SetPolyF3(&prims->f3[i]);
    }
    for (i = 0; i < LEN(prims->f4); i++) {
        SetPolyF4(&prims->f4[i]);
    }
    for (i = 0; i < LEN(prims->g3); i++) {
        SetPolyG3(&prims->g3[i]);
    }
    for (i = 0; i < LEN(prims->g4); i++) {
        SetPolyG4(&prims->g4[i]);
    }
    for (i = 0; i < LEN(prims->ft3); i++) {
        SetPolyFT3(&prims->ft3[i]);
    }
    for (i = 0; i < LEN(prims->ft4); i++) {
        SetPolyFT4(&prims->ft4[i]);
    }
}

void HighwayPrimCursorsReset(HighwayPrimBuffer* prims) {
    prims->f3Cursor = (POLY_F3*)prims->g3;
    prims->f4Cursor = (POLY_F4*)prims->g3;
    prims->g3Cursor = prims->g3;
    prims->g4Cursor = prims->g4;
    prims->ft3Cursor = prims->ft3;
    prims->ft4Cursor = prims->ft4;
    prims->gt3Cursor = (POLY_GT3*)&prims->ft4[LEN(prims->ft4)];
    prims->gt4Cursor = (POLY_GT4*)&prims->ft4[LEN(prims->ft4)];
}

// Same as JetNodesInit.
void HighwayNodesInit(void) {
    s32 i;

    HighwayNodeInit(&g_HighwayRootNode, 0);
    g_HighwayRootNode.depth = 0;
    g_HighwayNextFreeNode = 0;
    for (i = 0; i < LEN(g_HighwayNodeFreeList); i++) {
        g_HighwayNodeFreeList[i] = i + 1;
    }
    for (i = 0; i < LEN(g_HighwayNodeListHeads); i++) {
        g_HighwayNodeListHeads[i].next = &g_HighwayNodeListTails[i];
        g_HighwayNodeListTails[i].prev = &g_HighwayNodeListHeads[i];
        g_HighwayNodeListHeads[i].prev = NULL;
        g_HighwayNodeListTails[i].next = NULL;
    }
}
