//! PSYQ=3.3 FORCE_MEM=true

#include "jet_private.h"

s32 D_800A8310 = 0x9D8;
u32 D_800A8314 = 0x28;
s32 D_800A8318 = 0x9D9;
u32 D_800A831C = 0x4DE8;
s32 D_800A8320 = 0x9E3;
u32 D_800A8324 = 0x44;
s32 D_800A8328 = 0x9E4;
u32 D_800A832C = 0xA7958;
s32 D_800A8330 = 0x7F;
s32 D_800A8334 = 0x7F;
s32 D_800A8338 = 0;
s32 D_800A833C = 0;
MATRIX D_800A8340 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
MATRIX D_800A8360 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
MATRIX D_800A8380 = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};
SVECTOR D_800A83A0 = {0, 0, 0, 0}; // world rotation
VECTOR g_JetCameraPos = {0, 0, 0, 0};
VECTOR D_800A83B8 = {0, 0, 0, 0};
VECTOR D_800A83C8 = {0, 0, 0, 0};
VECTOR D_800A83D8 = {0, 0, 0, 0};
s32 D_800A83E8[2] = {0, 0};

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", MINI_Jet);

// Draw one object's model, project its bounding box and flag a cursor hit.
void func_800A0874(JetBuffer* db, JetNode* node, s16 otIndex, s32 arg3, Unk800A4390* obj) {
    Unk800A8604 args;
    s16 xs[6];
    s16 ys[6];
    MATRIX** world;
    MATRIX* m;
    MATRIX* wm;
    MATRIX* cam;
    s16 minX;
    s16 maxX;
    s16 minY;
    s16 maxY;
    s16 i;

    world = &g_JetWorldMatrix;
    m = world[0];
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
    if (node->parent != &g_JetRootNode) {
        CompMatrix(&node->parent->m, m, m);
    }
    wm = world[0];
    wm->t[0] -= g_JetCameraPos.vx;
    wm->t[1] -= g_JetCameraPos.vy;
    wm->t[2] -= g_JetCameraPos.vz;
    cam = &g_JetCameraRot;
    gte_SetRotMatrix(cam);
    gte_ldclmv(&world[0]->m[0][0]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][0]);
    gte_ldclmv(&world[0]->m[0][1]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][1]);
    gte_ldclmv(&world[0]->m[0][2]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][2]);
    gte_SetTransMatrix(cam);
    gte_ldlv0(&world[0]->t[0]);
    gte_rt();
    gte_stlvl(&world[0]->t[0]);
    gte_SetRotMatrix(world[0]);
    gte_SetTransMatrix(world[0]);
    args.tris = node->model->tris;
    args.prim = db->prims.g3Cursor;
    args.ot = &db->ot[otIndex];
    args.model = node->model;
    db->prims.g3Cursor = JetDrawModelTris(&args);
    JetProject6Points(obj->unkDC, obj->unk11C);
    ys[0] = obj->unk11C[0] >> 16;
    minY = ys[0];
    maxY = minY;
    xs[0] = obj->unk11C[0];
    minX = xs[0];
    maxX = minX;
    for (i = 1; i < 6; i++) {
        ys[i] = (obj->unk11C[i] & 0xFFFF0000) >> 16;
        xs[i] = obj->unk11C[i];
        if (minX > xs[i]) {
            minX = xs[i];
        }
        if (maxX < xs[i]) {
            maxX = xs[i];
        }
        if (minY > ys[i]) {
            minY = ys[i];
        }
        if (maxY < ys[i]) {
            maxY = ys[i];
        }
    }
    if (JetVectorInsidePlanes((VECTOR*)g_JetWorldMatrix->t)) {
        obj->unk28.hit = 0;
        if (g_JetCursorX < maxX && minX < g_JetCursorX && g_JetCursorY < maxY && minY < g_JetCursorY &&
            g_JetFiring == 1) {
            obj->unk28.hit = g_JetFiring;
        }
    }
}

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A0D78);

void JetDrawNodeUI(JetBuffer* db, JetNode* node, s16 otIndex, s32 arg3, s32 arg4) {
    Unk800A8604 args;
    MATRIX** world;
    MATRIX* m;

    world = &g_JetWorldMatrix;
    m = world[0];
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
    gte_SetRotMatrix(world[0]);
    gte_SetTransMatrix(world[0]);
    args.tris = node->model->tris;
    args.prim = db->prims.g3Cursor;
    args.ot = &db->ot2[otIndex];
    args.model = node->model;
    db->prims.g3Cursor = JetDrawModelTrisUI(&args);
}

void JetDrawTriangleList(void) {
    JetListLink* list;
    JetTriangle* tris;
    u16 triId;
    POLY_G3* prim;

    prim = g_JetBufferPtr[0]->prims.g3Cursor;
    tris = g_JetTrianglesBase;
    if (g_JetTriangleListCount) {
        triId = g_JetTriangleListHead;
        list = &g_JetTriangleLinks;
        do {
            prim = JetDrawTriangle(&tris[triId], prim, g_JetBufferPtr[0]->ot, &tris[triId]);
            triId = list[triId].next;
        } while (triId != 0xFFFF);
    }
    g_JetBufferPtr[0]->prims.g3Cursor = prim;
}

void JetDrawTrack(void) {
    JetListLink* list;
    SVECTOR* left;
    SVECTOR* right;
    u16 trackId;
    POLY_FT4* prim;

    trackId = g_JetTrackListHead;
    prim = g_JetBufferPtr[0]->prims.ft4Cursor;
    list = g_JetTrackLinks;
loop:
    left = g_JetTrackLeft;
    right = g_JetTrackRight;
    prim = JetDrawTrackQuad(&left[trackId], prim, g_JetBufferPtr[0]->ot, &right[trackId]);
    trackId = list[trackId].next;
    if (trackId != 0xFFFF) {
        goto loop;
    }
    g_JetBufferPtr[0]->prims.ft4Cursor = prim;
}

void JetSetWorldMatrix(void) {
    MATRIX** view;
    MATRIX** world;
    MATRIX* cam;

    view = &g_JetViewMatrix;
    view[0]->t[0] = -g_JetCameraPos.vx;
    view[0]->t[1] = -g_JetCameraPos.vy;
    view[0]->t[2] = -g_JetCameraPos.vz;
    cam = &g_JetCameraRot;
    gte_SetRotMatrix(cam);
    gte_ldclmv(&view[0]->m[0][0]);
    gte_rtir();
    world = &g_JetWorldMatrix;
    gte_stclmv(&world[0]->m[0][0]);
    gte_ldclmv(&view[0]->m[0][1]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][1]);
    gte_ldclmv(&view[0]->m[0][2]);
    gte_rtir();
    gte_stclmv(&world[0]->m[0][2]);
    gte_SetTransMatrix(cam);
    gte_ldlv0(&view[0]->t[0]);
    gte_rt();
    gte_stlvl(&world[0]->t[0]);
    gte_SetRotMatrix(world[0]);
    gte_SetTransMatrix(world[0]);
}

void JetTrackSample(u32 at, s32 lift, VECTOR* pos, SVECTOR* rot) {
    VECTOR left;
    VECTOR right;
    VECTOR mid;
    VECTOR nextMid;
    VECTOR curMid;
    VECTOR across;
    VECTOR along;
    VECTOR normal;
    VECTOR unit;
    VECTOR dLeft;
    VECTOR dRight;
    SVECTOR* rotCur;
    SVECTOR* rotNext;
    SVECTOR* leftCur;
    SVECTOR* leftNext;
    SVECTOR* rightCur;
    SVECTOR* rightNext;
    s32 seg;
    s32 frac;
    s32 dx;
    s32 dy;
    s32 dz;

    seg = at >> 16;
    frac = at & 0xFFFF;
    rotCur = &g_JetTrackRot[seg];
    rotNext = &g_JetTrackRot[seg + 1];
    dx = rotNext->vx - rotCur->vx;
    dy = rotCur->vy - rotNext->vy;
    dz = rotNext->vz - rotCur->vz;
    if (dx > 0x800) {
        dx -= 0x1000;
    }
    if (dy > 0x800) {
        dy -= 0x1000;
    }
    if (dz > 0x800) {
        dz -= 0x1000;
    }
    if (dx < -0x800) {
        dx += 0x1000;
    }
    if (dy < -0x800) {
        dy += 0x1000;
    }
    if (dz < -0x800) {
        dz += 0x1000;
    }
    dx *= frac;
    dy *= frac;
    dz *= frac;
    dx >>= 16;
    dy >>= 16;
    dz >>= 16;

    leftCur = &g_JetTrackLeft[seg];
    leftNext = &g_JetTrackLeft[seg + 1];
    dLeft.vx = (leftNext->vx - leftCur->vx) * frac;
    dLeft.vy = (leftNext->vy - leftCur->vy) * frac;
    dLeft.vz = (leftNext->vz - leftCur->vz) * frac;

    rightCur = &g_JetTrackRight[seg];
    rightNext = &g_JetTrackRight[seg + 1];
    left.vx = leftCur->vx + (dLeft.vx >> 16);
    left.vy = leftCur->vy + (dLeft.vy >> 16);
    left.vz = leftCur->vz + (dLeft.vz >> 16);

    dRight.vx = (rightNext->vx - rightCur->vx) * frac;
    dRight.vy = (rightNext->vy - rightCur->vy) * frac;
    dRight.vz = (rightNext->vz - rightCur->vz) * frac;

    right.vx = rightCur->vx + (dRight.vx >> 16);
    right.vy = rightCur->vy + (dRight.vy >> 16);
    right.vz = rightCur->vz + (dRight.vz >> 16);
    mid.vx = (right.vx + left.vx) >> 1;
    mid.vy = (right.vy + left.vy) >> 1;
    mid.vz = (right.vz + left.vz) >> 1;

    curMid.vx = (rightCur->vx + leftCur->vx) >> 1;
    curMid.vy = (rightCur->vy + leftCur->vy) >> 1;
    curMid.vz = (rightCur->vz + leftCur->vz) >> 1;

    nextMid.vx = (rightNext->vx + leftNext->vx) >> 1;
    nextMid.vy = (rightNext->vy + leftNext->vy) >> 1;
    nextMid.vz = (rightNext->vz + leftNext->vz) >> 1;
    along.vx = nextMid.vx - curMid.vx;
    along.vy = nextMid.vy - curMid.vy;
    along.vz = nextMid.vz - curMid.vz;

    across.vx = right.vx - left.vx;
    across.vy = right.vy - left.vy;
    across.vz = right.vz - left.vz;

    OuterProduct0(&along, &across, &normal);
    VectorNormal(&normal, &unit);

    pos->vx = (s16)mid.vx + ((unit.vx * lift) >> 12);
    pos->vy = (s16)mid.vy + ((unit.vy * lift) >> 12);
    pos->vz = (s16)mid.vz + ((unit.vz * lift) >> 12);

    rot->vx = rotCur->vx + dx;
    rot->vy = dy - rotCur->vy;
    rot->vz = rotCur->vz + dz;
}

void JetDrawEnergyGauge(void) {
    JetBuffer** db;
    POLY_G4* poly;
    s16 power;
    s32 top;

    db = g_JetBufferPtr;
    poly = db[0]->prims.g4Cursor;
    power = g_JetShotPower;
    top = 220 - power;
    setXY4(poly, 20, top, 28, top, 20, 220, 28, 220);
    setRGB0(poly, -0x80 - power, power, 0);
    setRGB1(poly, -0x80 - power, power, 0);
    setRGB2(poly, 0x80, 0, 0);
    setRGB3(poly, 0x80, 0, 0);
    SetSemiTrans(poly, 0);
    addPrim(&db[0]->ot2[1], poly);
    poly++;
    db[0]->prims.g4Cursor = poly;
}

void JetDrawScorePopup(JetBuffer* arg0, s16 arg1, s32 arg2, s32 arg3, s32 arg4) {
    JetNode* node;
    u8* alternate;
    s16* counter;
    s32 index;
    s32 unused;

    if (arg1 == 0 || arg1 == 0x5B) {
        return;
    }
    alternate = &D_800E25E8;
    index = 0;
    node = D_800A8A74[index];
    node->model = g_JetModelTable[arg1];
    g_JetPopupRot.vx += arg2;
    g_JetPopupRot.vy += arg3;
    g_JetPopupRot.vz += arg4;
    if (alternate[0] == 1) {
        RotMatrix(&g_JetPopupRot, &D_800A8A74[index]->m);
        JetDrawNodeUI(arg0, D_800A8A74[index], 0, 0, unused);
        func_800A1CD8(g_JetPopupPoints, 220, 160, 0, 0x18);
    }
    counter = &g_JetPopupTimer;
    (*counter)--;
    if (*counter < 50) {
        if (alternate[0] == 0) {
            alternate[0] = 1;
        } else {
            alternate[0] = 0;
        }
    }
    if (g_JetPopupTimer == 0) {
        g_JetPopupModelId = 0;
    }
}

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A1CD8);

void JetDrawSprite(s16 spriteId, s16 x, s16 y, s16 w, s16 h, u8 u, u8 v, u8 uw, u8 vh, u8 semiTrans) {
    JetBuffer** db;
    POLY_FT4* poly;

    db = g_JetBufferPtr;
    poly = db[0]->prims.ft4Cursor;
    setXYWH(poly, x, y, w, h);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUVWH(poly, u, v, uw, vh);
    poly->tpage = g_JetSpriteTPage[spriteId];
    poly->clut = g_JetSpriteClut[spriteId];
    SetSemiTrans(poly, semiTrans);
    addPrim(&db[0]->ot2[1], poly);
    poly++;
    db[0]->prims.ft4Cursor = poly;
}

// Queue two blank textured quads, one at each end of the background OT.
void func_800A2058(void) {
    JetBuffer** db;
    POLY_FT4* poly;

    db = g_JetBufferPtr;
    poly = db[0]->prims.ft4Cursor;
    setXY4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    poly->tpage = g_JetSpriteTPage[5];
    poly->clut = g_JetSpriteClut[5];
    SetSemiTrans(poly, 0);
    addPrim(&db[0]->ot[0xFFF], poly);
    poly++;
    setXY4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    setRGB0(poly, 0x80, 0x80, 0x80);
    setUV4(poly, 0, 0, 0, 0, 0, 0, 0, 0);
    poly->tpage = g_JetSpriteTPage[5];
    poly->clut = g_JetSpriteClut[5];
    SetSemiTrans(poly, 0);
    addPrim(&db[0]->ot[2], poly);
    poly++;
    db[0]->prims.ft4Cursor = poly;
}

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2214);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2420);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2518);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A27F0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2860);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2938);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A29AC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2AA0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2B78);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2BE0);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2C50);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2DE4);

void func_800A2E30(void) {}

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A2E38);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A334C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3414);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A35DC);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A372C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A385C);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A38D4);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3980);

INCLUDE_ASM("asm/us/mini/jet/nonmatchings/jet", func_800A3A20);
