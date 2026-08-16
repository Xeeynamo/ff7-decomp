//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <psxsdk/libcd.h>

extern s16* D_800A6528;
extern s32 D_800AF40C;
extern s32 D_800AF410;
extern void* D_800A6524;
typedef struct EndingNode {
    struct EndingNode* prev;
    struct EndingNode* next;
    void (*fn)(struct EndingNode*);
    u16 id;
    u8 state;
    u8 prio;
} EndingNode;

typedef struct EndingObj {
    struct EndingObj* parent;
    s32 unk4;
    MATRIX mtx;
    VECTOR scale;
    SVECTOR rot;
    SVECTOR pos;
} EndingObj;

extern EndingNode D_800AF3C8;
extern EndingNode D_800AF3D8;

extern s32 D_800A6390;
extern s32 D_800A6394;
extern u8 D_800A652E[];
extern u8 D_800A6532[];
extern u8 D_800A6540[];
extern u8 D_800A6541[];
extern u8 D_800A6542[];
extern u8 D_800A6544[];
extern u8 D_800A6545[];
extern u8 D_800A6546[];
extern u8 D_800A6534[];
extern u8 D_800A6538[];
extern u8 D_800A653C[];
extern u8 D_800A653D[];
extern u8 D_800A653E[];
extern u8 D_800A6588[];
extern u8 D_800A6594[];
extern u8 D_800A6598[];
extern u8 D_800A659C[];
extern u8 D_800A658A[];
extern u8 D_800A658C[];
extern EndingNode D_800A762C;
extern s32 D_800AF408;
extern u_long D_800A64E4[];
extern DR_MODE D_800A64EC[];
extern TILE D_800A6504[];
extern char D_800A0000[];
extern char D_800A0018[];
extern DISPENV D_800AF398[];
extern DRAWENV D_800AF2E0[];
extern DISPENV* D_8007EBD8;
extern DRAWENV* D_8007EBD0;
extern u32 D_800AF3C0;
extern u32 D_800AF3C4;
extern u32 D_800AF3EC;
extern u32 D_800AF3F0;
extern u32 D_800AF3F4;
extern u32 D_800AF3F8;
extern void* D_800AF3E8;
extern void* D_800AF3FC;
extern TILE D_800A763C[];
extern TILE D_800A765C[];
extern s32 (*D_800A63DC[])(void);
extern u8* D_8003623C;

typedef struct {
    u16 flags;
    s16 timer;
    s16 unk4;
    u16 delay;
    u16 frame;
    u16 unkA;
    void* anim;
    u8 r;
    u8 g;
    u8 b;
    u8 unk13;
    u8 dr;
    u8 dg;
    u8 db;
    u8 unk17;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 unk1B;
    u8 unk1C[0x40];
    s16 unk5C;
    s16 unk5E;
    s16 unk60;
    u8 unk62[6];
    s32 unk68;
    s32 unk6C;
    s32 unk70;
    u8 unk74[4];
    VECTOR unk78;
} EndingSprite;

extern EndingSprite D_800A652C[];
extern s32 D_800A63B0;

s32 func_80034410(void);
void func_80034FC8(void*, s16);
void* func_80034D18(void*, s32);
void func_800A2888(void*, s16*, s16*);
void func_80034D2C(void*, void*);
s32 func_80034D5C(void);
s32 func_800484A8(void);
MATRIX* ScaleMatrix(MATRIX*, VECTOR*);
void VectorNormal(VECTOR*, VECTOR*);
u32 func_8001C808(void);
void func_80041D28(u32, void*, s32);
s32 func_80041E30(s32, s32);
void SystemLzsDecompress(void*, void*);
s32 func_800A379C(EndingObj*, VECTOR*, VECTOR*, s32);
void func_80036244(void*, u16);
void func_800A3178(EndingNode*, s16, u8, void (*)());
void func_800A09DC(void);
void func_800A2504(s32, s32, s32, u8, u8, u8);
s32 SetGraphDebug(s32);
s32 func_800A273C(s32);
void func_800A3368(EndingSprite*);
void func_800A343C(EndingSprite*);
void* func_800A358C(void*, s32, void*, void*);
void func_800A0E68(void);

s32 func_80048540(s32);
EndingNode* func_800A3314(s16);
void func_800A32D8(EndingNode*);

void func_800A0030(void) {
    RECT rect;
    CdlFILE file;
    s32 buf;
    s32 r;
    s32 g;
    s32 b;
    s32 i;
    s32 res;
    s32 ode;
    s32 tp;
    u8* src;

    func_800A2504(0x280, 0x1E0, 0x200, 0, 0, 0);

    do {
        res = (s32)CdSearchFile(&file, D_800A0018);

        if (res <= 0) {
            if (res >= -1) {
                printf(D_800A0000);
                return;
            }
        }

        CdControlB(2, (u_char*)&file, NULL);
        func_80041D28((file.size + 0x7FF) >> 11, (void*)0x80180000, 0x80);

        do {
            res = func_80041E30(1, 0);
        } while (res > 0);
    } while (res != 0);

    SystemLzsDecompress((void*)0x80180000, (void*)0x80100000);
    SetDispMask(1);

    b = 0xFE;
    g = 0xFE;
    r = 0xFE;

    do {
        buf = buf == 0;
        func_800A273C(0);
        ode = GetODE() ^ 1;
        src = (u8*)((ode * 0x500) + 0x80100000);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x280;
        rect.h = 1;
        rect.y = ode;

        while (rect.y < 0x1E0) {
            LoadImage(&rect, (u_long*)src);
            DrawSync(0);
            rect.y += 2;
            src += 0xA00;
        }

        ClearOTagR(((u_long*)((buf * 4) + (s32)D_800A64E4)), 1);
        SetTile((TILE*)((buf * 0x10) + (s32)D_800A6504));
        SetSemiTrans(((TILE*)((buf * 0x10) + (s32)D_800A6504)), 1);
        ((TILE*)((buf * 0x10) + (s32)D_800A6504))->x0 = 0x1E;
        ((TILE*)((buf * 0x10) + (s32)D_800A6504))->y0 = 0xC8;
        ((TILE*)((buf * 0x10) + (s32)D_800A6504))->w = 0x244;
        ((TILE*)((buf * 0x10) + (s32)D_800A6504))->h = 0x4A;
        ((TILE*)((buf * 0x10) + (s32)D_800A6504))->r0 = r;
        ((TILE*)((buf * 0x10) + (s32)D_800A6504))->g0 = g;
        ((TILE*)((buf * 0x10) + (s32)D_800A6504))->b0 = b;
        AddPrim(((u_long*)((buf * 4) + (s32)D_800A64E4)),
                ((TILE*)((buf * 0x10) + (s32)D_800A6504)));
        tp = GetTPage(2, 2, 0, 0);
        SetDrawMode(((DR_MODE*)((buf * 0xC) + (s32)D_800A64EC)), 1, 1,
                    tp & 0xFFFF, NULL);
        AddPrim(((u_long*)((buf * 4) + (s32)D_800A64E4)),
                ((DR_MODE*)((buf * 0xC) + (s32)D_800A64EC)));
        DrawOTag((u_long*)((buf * 4) + (s32)D_800A64E4));
        r -= 2;
        g -= 2;
        b -= 2;
    } while (r > 0);

    i = 0;

    do {
        buf = buf == 0;
        func_800A273C(0);
        ode = GetODE() ^ 1;
        src = (u8*)((ode * 0x500) + 0x80100000);
        rect.x = 0;
        rect.y = 0;
        rect.w = 0x280;
        rect.h = 1;
        rect.y = ode;

        while (rect.y < 0x1E0) {
            LoadImage(&rect, (u_long*)src);
            DrawSync(0);
            rect.y += 2;
            src += 0xA00;
        }

        i++;
    } while (i < 0x12C);

    if (r < 0xFE) {
        do {
            buf = buf == 0;
            func_800A273C(0);
            ode = GetODE() ^ 1;
            src = (u8*)((ode * 0x500) + 0x80100000);
            rect.x = 0;
            rect.y = 0;
            rect.w = 0x280;
            rect.h = 1;
            rect.y = ode;

            while (rect.y < 0x1E0) {
                LoadImage(&rect, (u_long*)src);
                DrawSync(0);
                rect.y += 2;
                src += 0xA00;
            }

            ClearOTagR(((u_long*)((buf * 4) + (s32)D_800A64E4)), 1);
            SetTile((TILE*)((buf * 0x10) + (s32)D_800A6504));
            SetSemiTrans(((TILE*)((buf * 0x10) + (s32)D_800A6504)), 1);
            ((TILE*)((buf * 0x10) + (s32)D_800A6504))->x0 = 0x1E;
            ((TILE*)((buf * 0x10) + (s32)D_800A6504))->y0 = 0xC8;
            ((TILE*)((buf * 0x10) + (s32)D_800A6504))->w = 0x244;
            ((TILE*)((buf * 0x10) + (s32)D_800A6504))->h = 0x4A;
            ((TILE*)((buf * 0x10) + (s32)D_800A6504))->r0 = r;
            ((TILE*)((buf * 0x10) + (s32)D_800A6504))->g0 = g;
            ((TILE*)((buf * 0x10) + (s32)D_800A6504))->b0 = b;
            AddPrim(((u_long*)((buf * 4) + (s32)D_800A64E4)),
                    ((TILE*)((buf * 0x10) + (s32)D_800A6504)));
            tp = GetTPage(2, 2, 0, 0);
            SetDrawMode(((DR_MODE*)((buf * 0xC) + (s32)D_800A64EC)), 1, 1,
                        tp & 0xFFFF, NULL);
            AddPrim(((u_long*)((buf * 4) + (s32)D_800A64E4)),
                    ((DR_MODE*)((buf * 0xC) + (s32)D_800A64EC)));
            DrawOTag((u_long*)((buf * 4) + (s32)D_800A64E4));
            r += 2;
            g += 2;
            b += 2;
        } while (r < 0xFE);
    }

    SetDispMask(0);
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A04C4);

void func_800A09DC(void) {
    u8 unused[0x100];
    s32 i;

    for (i = 0; i < 0x20; i++) {
        if (D_800A652C[i].flags & 1) {
            EndingSprite* e = &D_800A652C[i];

            D_800A652C[i].unk5C = 0x28;
            D_800A652C[i].unk5E = 0x20;
            D_800A652C[i].unk60 = 0;

            func_800A34C4(e);
            func_800A343C(e);
            D_800AF3FC = func_800A358C(D_800AF3E8, 0, D_800AF3FC, e);
        }
    }
}

s32 func_800A0AB8(void) {
    s32 i = 0;
    s32 off = 0;

    do {
        *(u16*)((u8*)D_800A652C + off) = 0;
        *(u16*)&D_800A6532[off] = 0;
        *(u16*)&D_800A6534[off] = 0;
        *(s32*)&D_800A6538[off] = 0;
        *(u16*)&D_800A6588[off] = 0;
        *(u16*)&D_800A658A[off] = 0;
        *(u16*)&D_800A658C[off] = 0;
        D_800A653C[off] = 0;
        D_800A653D[off] = 0;
        D_800A653E[off] = 0;
        off += 0x88;
        i++;
    } while (i < 0x20);

    func_800A3178(&D_800A762C, 4, 0x80, func_800A09DC);

    return 1;
}

s32 func_800A0BA8(void) {
    s32 id = *D_800A6528++;
    s32 arg = *D_800A6528++;
    s32 off = id * 136;

    *(u16*)((u8*)D_800A652C + off) = 7;
    *(u16*)&D_800A6532[off] = 0;
    *(u16*)&D_800A6534[off] = 0;
    *(void**)&D_800A6538[off] = func_80034D18((void*)0x800D0000, arg);
    *(u16*)&D_800A6588[off] = 0;
    *(u16*)&D_800A658A[off] = 0;
    *(u16*)&D_800A658C[off] = 0;
    D_800A653C[off] = 0;
    D_800A653D[off] = 0;
    D_800A653E[off] = 0;

    return 1;
}

s32 func_800A0CAC(void) {
    s32 id = *D_800A6528++;
    s32 steps = *D_800A6528++;
    s32 target = *D_800A6528++;
    s32 off = id * 136;
    s32 dr = (target - D_800A653C[off]) / steps;
    s32 dg = (target - D_800A653D[off]) / steps;
    s32 db = (target - D_800A653E[off]) / steps;

    *(u16*)&D_800A652E[off] = steps;
    D_800A6544[off] = target;
    D_800A6545[off] = target;
    D_800A6546[off] = target;
    *(u16*)((u8*)D_800A652C + off) |= 8;
    D_800A6540[off] = dr;
    D_800A6541[off] = dg;
    D_800A6542[off] = db;
    D_800A6394 = 1;

    return 1;
}

void func_800A0E68(void) {
    s32 i;

    AddPrim(D_800AF3E8, &D_800A763C[D_800AF408]);
    AddPrim(D_800AF3E8, &D_800A765C[D_800AF408]);

    for (i = 0; i < 0x20; i++) {
        if (D_800A652C[i].flags & 1) {
            EndingSprite* e = &D_800A652C[i];

            D_800A652C[i].unk5E--;

            if (D_800A652C[i].unk5E == -0x10) {
                D_800A652C[i].flags = 0;
            }

            func_800A34C4(e);
            func_800A343C(e);
            D_800AF3FC = func_800A358C(D_800AF3E8, 0, D_800AF3FC, e);
        }
    }
}

s32 func_800A0F90(void) {
    s32 i = 0;
    s32 off = 0;
    s32 w;
    s32 h;
    s32 k;

    do {
        *(u16*)((u8*)D_800A652C + off) = 0;
        *(u16*)&D_800A6532[off] = 0;
        *(u16*)&D_800A6534[off] = 0;
        *(s32*)&D_800A6538[off] = 0;
        *(u16*)&D_800A6588[off] = 0;
        *(u16*)&D_800A658A[off] = 0;
        *(u16*)&D_800A658C[off] = 0;
        D_800A653C[off] = 0;
        D_800A653D[off] = 0;
        D_800A653E[off] = 0;
        off += 0x88;
        i++;
    } while (i < 0x20);

    func_800A3178(&D_800A762C, 4, 0x80, func_800A0E68);

    w = 0x140;
    h = 0x28;

    for (k = 0; k < 2; k++) {
        SetTile(&D_800A763C[k]);
        SetTile(&D_800A765C[k]);
        D_800A763C[k].x0 = 0;
        D_800A763C[k].y0 = 0;
        D_800A763C[k].w = w;
        D_800A763C[k].h = h;
        D_800A765C[k].x0 = 0;
        D_800A765C[k].y0 = 0xC8;
        D_800A765C[k].w = w;
        D_800A765C[k].h = h;
        D_800A763C[k].r0 = 0;
        D_800A763C[k].g0 = 0;
        D_800A763C[k].b0 = 0;
        D_800A765C[k].r0 = 0;
        D_800A765C[k].g0 = 0;
        D_800A765C[k].b0 = 0;
    }

    return 1;
}

s32 func_800A11B4(void) {
    s32 i = 0;
    s32 c = 0x80;
    s32 arg = *D_800A6528++;
    s32 v = *D_800A6528++;
    u8* p = (u8*)D_800A652C;
    s32 off = 0;

    for (; i < 0x20; i++, p += 0x88, off += 0x88) {
        if (*(u16*)p & 1) {
            continue;
        }

        *(u16*)p = 1;
        *(u16*)&D_800A6532[off] = 0;
        *(u16*)&D_800A6534[off] = v;
        *(void**)&D_800A6538[off] = func_80034D18((void*)0x800D0000, arg);
        *(u16*)&D_800A6588[off] = 0x18;
        *(u16*)&D_800A658A[off] = 0xC8;
        *(u16*)&D_800A658C[off] = 0;
        D_800A653C[off] = c;
        D_800A653D[off] = c;
        D_800A653E[off] = c;

        return 1;
    }

    return 0;
}

void func_800A12F0(void) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        if (D_800A652C[i].flags & 1) {
            EndingSprite* e = &D_800A652C[i];

            func_800A3368(e);
            func_800A34C4(e);
            func_800A343C(e);
            D_800AF3FC = func_800A358C(D_800AF3E8, 0, D_800AF3FC, e);
        }
    }
}

s32 func_800A139C(void) {
    s32 i = 0;
    s32 off = 0;

    do {
        *(u16*)((u8*)D_800A652C + off) = 0;
        *(u16*)&D_800A6532[off] = 0;
        *(u16*)&D_800A6534[off] = 0;
        *(s32*)&D_800A6538[off] = 0;
        *(u16*)&D_800A6588[off] = 0;
        *(u16*)&D_800A658A[off] = 0;
        *(u16*)&D_800A658C[off] = 0;
        *(s32*)&D_800A6594[off] = 0;
        *(s32*)&D_800A6598[off] = 0;
        *(s32*)&D_800A659C[off] = 0;
        D_800A653C[off] = 0;
        D_800A653D[off] = 0;
        D_800A653E[off] = 0;
        off += 0x88;
        i++;
    } while (i < 0x20);

    func_800A3178(&D_800A762C, 4, 0x80, func_800A12F0);

    return 1;
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A14BC);

s32 func_800A16E4(void) {
    s32 id = *D_800A6528++;

    D_800A652C[id].flags |= 0x10;
    D_800A652C[id].unk78.vx = *D_800A6528++;
    D_800A652C[id].unk78.vy = *D_800A6528++;
    D_800A652C[id].unk78.vz = *D_800A6528++;
    D_800A652C[id].unk4 = *D_800A6528++;

    return 1;
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A17C0);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A19A4);

void func_800A1E20(void) {
    s16* pc;

    do {
        pc = D_800A6528;
        D_800A6394 = 0;
        D_800A6528 = pc + 1;

        if (D_800A63DC[*pc]() == 0) {
            D_800A6528 = pc;
            D_800A6390 = 0;
        } else {
            D_800A6390 = 1;
        }
    } while (D_800A6394 != 0);
}

void func_800A1ED4(s16* arg0) { D_800A6528 = arg0; }

s32 func_800A1EE4(void) { return 0; }

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A1EEC);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A1F48);

s32 func_800A1FA4(void) { return func_80034410() == 0; }

s32 func_800A1FC8(void) {
    s16 sp10;
    s16 sp12;

    func_800A2888(
        func_80034D18((void*)0x800D0000, *D_800A6528++), &sp10, &sp12);

    return 1;
}

s32 func_800A2014(void) {
    s16 sp10;
    s16 sp12;
    s32 id = *D_800A6528++;

    if (D_800A6390 != 0) {
        func_80034D2C(func_80034D18((void*)0x800D0000, id), (void*)0x80120000);
    }

    if (func_80034D5C() != 0) {
        return 0;
    }

    func_800A2888((void*)0x80120000, &sp10, &sp12);

    return 1;
}

s32 func_800A208C(void) {
    D_800A6524 = (void*)0x801A0000;
    func_80034FC8((void*)0x801A0000, *D_800A6528++);
    return 1;
}

s32 func_800A20D4(void) { return func_80034410() == 0; }

s32 func_800A20F8(void) {
    u8 unused[8]; /* retail reserves it, nothing reads it */

    func_800A2504(*D_800A6528++, *D_800A6528++, *D_800A6528++,
                  *(u8*)D_800A6528++, *(u8*)D_800A6528++, *(u8*)D_800A6528++);
    func_800A273C(0);

    return 1;
}

s32 func_800A2190(void) {
    SetDispMask(*D_800A6528++);
    return 1;
}

s32 func_800A21CC(void) {
    if (D_800A6390 != 0) {
        D_800A63B0 = *D_800A6528++;
    } else {
        D_800A6528++;
    }

    return --D_800A63B0 == 0;
}

s32 func_800A2248(void) {
    func_800A32D8(func_800A3314(4));

    return 1;
}

s32 func_800A2274(void) {
    s32 count = *D_800A6528 + 1;

    D_800A6528 -= count;

    return 1;
}

s32 func_800A22A4(void) {
    D_800AF40C = *D_800A6528++;
    return 1;
}

s32 func_800A22D4(void) {
    D_800AF410 = 0;
    return 1;
}

s32 func_800A22E4(void) {
    *(s16*)((u8*)D_800A652C + (*D_800A6528++ * 136)) = 0;
    return 1;
}

s32 func_800A2328(void) {
    D_8009A000[0] = 0x10;
    D_8009A004[0] = (u32)func_80034D18((void*)0x800D0000, *D_800A6528++);
    SystemAkaoExecute();

    return 1;
}

s32 func_800A2380(void) {
    D_8009A000[0] = *D_800A6528++;
    D_8009A004[0] = *D_800A6528++;
    D_8009A008[0] = *D_800A6528++;
    SystemAkaoExecute();

    return 1;
}

s32 func_800A23F8(void) { return func_80034410() == 8; }

s32 func_800A2420(void) {
    if (D_80075D00->unk8 >= *D_800A6528++) {
        return 1;
    }
    return 0;
}

void func_800A2458(void) {
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    PadInit(0);
    InitGeom();
    func_80036298();
    func_80033B70();
}

void func_800A24A8(void) {
    s32 res;

    while ((res = func_800484A8()) == -1) {
        VSync(0);
    }

    if (res != 0) {
        do {
        } while (func_80048540(1) != 0);
    }
}

void func_800A2504(s32 w, s32 h, s32 dist, u8 r, u8 g, u8 b) {
    RECT rect;
    s32 y;
    s32 res;

    y = (h != 0x1E0) ? 0xF0 : 0;

    while ((res = func_800484A8()) == -1) {
        VSync(0);
    }

    if (res != 0) {
        do {
        } while (func_80048540(1) != 0);
    }

    ResetGraph(1);
    SetDispMask(0);
    SetGraphDebug(0);
    SetGeomOffset(w / 2, h / 2);
    SetGeomScreen(dist);
    SetDefDrawEnv(&D_800AF2E0[0], 0, 0, w, h);
    SetDefDispEnv(&D_800AF398[0], 0, y, w, h);
    SetDefDrawEnv(&D_800AF2E0[1], 0, y, w, h);
    SetDefDispEnv(&D_800AF398[1], 0, 0, w, h);
    D_800AF408 = 1;

    rect.w = w * 3 / 2;

    D_800AF398[1].isrgb24 = 0;
    D_800AF398[0].isrgb24 = 0;
    D_800AF2E0[1].isbg = 0;
    D_800AF2E0[0].isbg = 0;
    D_800AF2E0[1].dfe = 0;
    D_800AF2E0[0].dfe = 0;
    D_800AF2E0[1].dtd = 0;
    D_800AF2E0[0].dtd = 0;
    D_800AF2E0[1].tpage = 0;
    D_800AF2E0[0].tpage = 0;
    D_800AF2E0[0].r0 = r;
    D_800AF2E0[0].g0 = g;
    D_800AF2E0[0].b0 = b;
    D_800AF2E0[1].r0 = r;
    D_800AF2E0[1].g0 = g;
    D_800AF2E0[1].b0 = b;

    rect.x = 0;
    rect.y = 0;
    rect.h = 0x1E0;
    ClearImage(&rect, 0, 0, 0);
    func_800A273C(0);
}

s32 func_800A273C(s32 arg0) {
    u32 pad0;
    u32 pad1;
    u32 old0;
    u32 old1;

    D_800AF408 ^= 1;
    DrawSync(0);
    VSync(arg0);
    PutDispEnv(&D_800AF398[D_800AF408]);
    PutDrawEnv(&D_800AF2E0[D_800AF408]);
    D_8007EBD8 = &D_800AF398[D_800AF408];
    D_8007EBD0 = &D_800AF2E0[D_800AF408];

    pad0 = func_8001C808();
    old0 = D_800AF3C0;
    old1 = D_800AF3C4;
    pad1 = pad0 >> 16;

    D_800AF3F4 = pad0;
    D_800AF3F8 = pad1;
    D_800AF3C0 = ~pad0;
    D_800AF3C4 = ~pad1;
    D_800AF3EC = old0 & pad0;
    D_800AF3F0 = old1 & pad1;

    return D_800AF408;
}

void func_800A2888(void* addr, s16* tpage, s16* clut) {
    TIM_IMAGE tim;

    OpenTIM(addr);
    ReadTIM(&tim);

    if (tim.paddr != NULL) {
        LoadImage(tim.prect, tim.paddr);
        *tpage = GetTPage(tim.mode, 0, tim.prect->x, tim.prect->y);
    }

    if (tim.caddr != NULL) {
        LoadImage(tim.crect, tim.caddr);
        *clut = GetClut(tim.crect->x, tim.crect->y);
    }
}

void func_800A2934(EndingObj* parent, EndingObj* child) {
    parent->unk4 = (s32)child;
    child->parent = parent;
    child->scale.pad = 0x1000;
    child->scale.vz = 0x1000;
    child->scale.vy = 0x1000;
    child->scale.vx = 0x1000;
    child->pos.pad = 0;
    child->pos.vz = 0;
    child->pos.vy = 0;
    child->pos.vx = 0;
    child->rot.pad = 0;
    child->rot.vz = 0;
    child->rot.vy = 0;
    child->rot.vx = 0;
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2974);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2A2C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2C68);

s32 func_800A2E80(EndingObj* obj) {
    s32 flag;

    RotMatrix(&obj->rot, &obj->mtx);
    obj->mtx.t[0] = obj->pos.vx;
    obj->mtx.t[1] = obj->pos.vy;
    obj->mtx.t[2] = obj->pos.vz;
    ScaleMatrix(&obj->mtx, &obj->scale);
    MulMatrix2(&obj->parent->mtx, &obj->mtx);
    SetRotMatrix(&obj->parent->mtx);
    SetTransMatrix(&obj->parent->mtx);
    RotTrans(&obj->pos, (VECTOR*)obj->mtx.t, &flag);

    return flag;
}

s32 func_800A2F1C(EndingObj* obj) {
    s32 flag;

    RotMatrixYXZ(&obj->rot, &obj->mtx);
    obj->mtx.t[0] = obj->pos.vx;
    obj->mtx.t[1] = obj->pos.vy;
    obj->mtx.t[2] = obj->pos.vz;
    ScaleMatrix(&obj->mtx, &obj->scale);
    MulMatrix2(&obj->parent->mtx, &obj->mtx);
    SetRotMatrix(&obj->parent->mtx);
    SetTransMatrix(&obj->parent->mtx);
    RotTrans(&obj->pos, (VECTOR*)obj->mtx.t, &flag);

    return flag;
}

s32 func_800A2FB8(EndingObj* obj, VECTOR* target, VECTOR* out, s32 scale) {
    VECTOR d;

    d.vx = target->vx - obj->pos.vx;
    d.vy = target->vy - obj->pos.vy;
    d.vz = target->vz - obj->pos.vz;

    VectorNormal(&d, out);

    if ((u32)(d.vx + 2) < 4 && (u32)(d.vy + 2) < 4 && (u32)(d.vz + 2) < 4) {
        out->vx = d.vx << 12;
        out->vy = d.vy << 12;
        out->vz = d.vz << 12;

        return 1;
    }

    if (scale == 0x1000) {
        return 0;
    }

    out->vx = (out->vx * scale) / 4096;
    out->vy = (out->vy * scale) / 4096;
    out->vz = (out->vz * scale) / 4096;

    return 0;
}

void func_800A310C(void) {
    D_800AF3C8.id = 0;
    D_800AF3C8.state = 1;
    D_800AF3C8.prio = 0xFF;
    D_800AF3C8.prev = NULL;
    D_800AF3C8.next = &D_800AF3D8;

    D_800AF3D8.id = 1;
    D_800AF3D8.state = 1;
    D_800AF3D8.prio = 0;
    D_800AF3D8.prev = &D_800AF3C8;
    D_800AF3D8.next = NULL;
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A3178);

void func_800A3210(void) {
    EndingNode* node = D_800AF3C8.next;

    while (node->next != NULL) {
        if (node->state == 4) {
            node->fn(node);
        }
        node = node->next;
    }

    node = D_800AF3C8.next;

    while (node->next != NULL) {
        if (node->state == 2) {
            node->state = 4;
        }
        node = node->next;
    }
}

void func_800A32D8(EndingNode* node) {
    EndingNode* prev = node->prev;
    EndingNode* next = node->next;

    prev->next = next;
    next->prev = prev;
}

void func_800A32F0(u8* arg0) { arg0[0xE] = 8; }

void func_800A32FC(u8* arg0) { arg0[0xE] = 4; }

void func_800A3308(u8* arg0) { arg0[0xE] = 0x10; }

EndingNode* func_800A3314(s16 id) {
    EndingNode* node = D_800AF3C8.next;

    while (node->next != NULL) {
        if (node->id == id) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

void func_800A3368(EndingSprite* spr) {
    VECTOR d;

    if (!(spr->flags & 0x10)) {
        return;
    }

    if (func_800A379C((EndingObj*)spr->unk1C, &spr->unk78, &d, spr->unk4) !=
        0) {
        spr->flags ^= 0x10;
    }

    spr->unk68 += d.vx;
    spr->unk6C += d.vy;
    spr->unk70 += d.vz;

    spr->unk5C = spr->unk68 / 4096;
    spr->unk5E = spr->unk6C / 4096;
    spr->unk60 = spr->unk70 / 4096;
}

void func_800A343C(EndingSprite* spr) {
    if (spr->flags & 8) {
        spr->r += spr->dr;
        spr->g += spr->dg;
        spr->b += spr->db;

        if (--spr->timer == 0) {
            spr->flags ^= 8;
            spr->r = spr->r0;
            spr->g = spr->g0;
            spr->b = spr->b0;
        }
    }
}

void func_800A34C4(EndingSprite* spr) {
    u16 count;

    if (!(spr->flags & 2)) {
        return;
    }

    count = *(u16*)spr->anim;

    if (spr->delay == 0) {
        spr->frame++;

        if (spr->frame >= count) {
            if (spr->flags & 4) {
                spr->frame = 0;
            } else {
                spr->frame--;
            }
        }

        func_80036244(spr->anim, spr->frame);
        spr->delay = D_8003623C[1];
    }

    spr->delay--;
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A358C);

s32 func_800A379C(EndingObj* obj, VECTOR* target, VECTOR* out, s32 scale) {
    VECTOR d;
    s32 v;

    d.vx = target->vx - obj->pos.vx;
    d.vy = target->vy - obj->pos.vy;
    d.vz = target->vz - obj->pos.vz;

    if (d.vx == 0) {
        v = (d.vy < 0) ? -0x1000 : 0x1000;
        out->vx = 0;
        out->vy = v;
        out->vz = 0;
    } else if (d.vy == 0) {
        v = (d.vx < 0) ? -0x1000 : 0x1000;
        out->vx = v;
        out->vy = 0;
        out->vz = 0;
    } else {
        VectorNormal(&d, out);
    }

    if ((u32)(d.vx + 2) < 4 && (u32)(d.vy + 2) < 4 && (u32)(d.vz + 2) < 4) {
        out->vx = d.vx << 12;
        out->vy = d.vy << 12;
        out->vz = d.vz << 12;

        return 1;
    }

    if (scale == 0x1000) {
        return 0;
    }

    out->vx = (out->vx * scale) / 4096;
    out->vy = (out->vy * scale) / 4096;
    out->vz = (out->vz * scale) / 4096;

    return 0;
}
