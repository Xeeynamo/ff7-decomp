//! PSYQ=3.6
#include "main_private.h"
#include "libgpu.h"
#include "libgte.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define DRAW_HEIGHT 224
#define SWIRL_CELL_SIZE 32
#define SWIRL_QUAD_COLS (SCREEN_WIDTH / SWIRL_CELL_SIZE)
#define SWIRL_QUAD_ROWS (DRAW_HEIGHT / SWIRL_CELL_SIZE)
#define SWIRL_VERTEX_COLS (SWIRL_QUAD_COLS + 1)
#define SWIRL_VERTEX_ROWS (SWIRL_QUAD_ROWS + 1)
#define SWIRL_CENTER_X (SCREEN_WIDTH / 2)
#define SWIRL_CENTER_Y (DRAW_HEIGHT / 2)
#define SWIRL_CELL_UV_EXTENT (SWIRL_CELL_SIZE - 1)
#define SWIRL_BUFFER_COUNT 2

#define SWIRL_DRAW_Y 240
#define SWIRL_TEXTURE_Y ((SCREEN_HEIGHT - DRAW_HEIGHT) / 2)
#define SWIRL_DISPLAY_Y (SWIRL_DRAW_Y - SWIRL_TEXTURE_Y)
#define SWIRL_SCREEN_Y_THRESHOLD 16
#define SWIRL_SCREEN_Y_OFFSET 24
#define SWIRL_GEOM_OFFSET_X (SCREEN_WIDTH / 2 - 1)
#define SWIRL_GEOM_OFFSET_Y (SCREEN_HEIGHT / 2 - 1)
#define SWIRL_PROJECTION_DISTANCE 480
#define SWIRL_TPAGE_X_MASK 0x3F

#define SWIRL_BLEND_INTERVAL 5
#define BLEND_AVERAGE 0
#define BLEND_ADD_QUARTER 3

#define SWIRL_STRIP_COUNT 3
#define SWIRL_STRIP_HEIGHT (DRAW_HEIGHT / SWIRL_STRIP_COUNT)
#define SWIRL_PIXELS_PER_WORD 2
#define SWIRL_STRIP_WORD_COUNT (SCREEN_WIDTH * SWIRL_STRIP_HEIGHT / SWIRL_PIXELS_PER_WORD)
#define SWIRL_PIXEL_PAIR_STP_MASK 0x80008000 // Sets transparency bit for two 16-bit pixels

#define SWIRL_FADE_FRAME_THRESHOLD 46
#define SWIRL_STOP_FRAME_THRESHOLD 78
#define SWIRL_ROTATION_STEP 3
#define SWIRL_INITIAL_SCALE_DELTA 4
#define SWIRL_SCALE_ACCELERATION 10
#define SWIRL_INITIAL_SCALE_XY 2080
#define SWIRL_SCALE_ONE 4096
#define SWIRL_INITIAL_INTENSITY 128

typedef struct {
    OT_TYPE ot;
    POLY_FT4 quads[SWIRL_QUAD_COLS][SWIRL_QUAD_ROWS];
} SwirlPacketBuffer;

typedef struct {
    SwirlPacketBuffer buffers[SWIRL_BUFFER_COUNT];
    OT_TYPE* activeOt;
    SVECTOR vertices[SWIRL_VERTEX_COLS][SWIRL_VERTEX_ROWS];
    DVECTOR points[SWIRL_VERTEX_COLS][SWIRL_VERTEX_ROWS];
    SVECTOR rotation;
    VECTOR scale;
    DRAWENV drawEnv;
    DISPENV dispEnv;
    s32 bufferCounter;
    s32 scaleDelta;
    s32 frame;
    s32 renderCounter;
    s32 colorIntensity;
} SwirlData;

#define SWIRL_DATA ((SwirlData*)0x8019C000)
#define SWIRL_PIXEL_BUFFER_A ((u_long*)0x801B8000)
#define SWIRL_PIXEL_BUFFER_B ((u_long*)0x801B0000)

void SysMemCopy32(void* dst, const void* src, const s32 len);

static RECT buf_rect = {0, SWIRL_DRAW_Y, SCREEN_WIDTH, DRAW_HEIGHT};
static MATRIX transform_matrix;

static void SysBattleSwirlUpdate(void) {
    long p, flag;
    CVECTOR color;
    SwirlPacketBuffer* buf;
    s32 i, j, delta;

    MoveImage(&buf_rect, 0, SWIRL_TEXTURE_Y);
    SWIRL_DATA->bufferCounter++;
    buf = &SWIRL_DATA->buffers[SWIRL_DATA->bufferCounter & 1];
    SWIRL_DATA->activeOt = &buf->ot;

    if (SWIRL_DATA->frame++ > SWIRL_FADE_FRAME_THRESHOLD) {
        if (SWIRL_DATA->colorIntensity != 0) {
            SWIRL_DATA->colorIntensity--;
        }

        color.cd = buf->quads[0][0].code;
        color.r = SWIRL_DATA->colorIntensity;
        color.g = SWIRL_DATA->colorIntensity;
        color.b = SWIRL_DATA->colorIntensity;
        for (j = 0; j < SWIRL_QUAD_ROWS; j++) {
            for (i = 0; i < SWIRL_QUAD_COLS; i++) {
                *(CVECTOR*)&buf->quads[i][j].r0 = color;
            }
        }

        if (SWIRL_DATA->frame > SWIRL_STOP_FRAME_THRESHOLD) {
            D_80095DD4 = 0;
        }
    } else {
        for (j = 0; j < SWIRL_QUAD_ROWS; j++) {
            for (i = 0; i < SWIRL_QUAD_COLS; i++) {
                buf->quads[i][j].tpage =
                    GetTPage(2, SWIRL_DATA->frame % SWIRL_BLEND_INTERVAL ? BLEND_AVERAGE : BLEND_ADD_QUARTER,
                             (i * SWIRL_CELL_SIZE) & ~SWIRL_TPAGE_X_MASK, 0);
            }
        }
    }

    RotMatrix(&SWIRL_DATA->rotation, &transform_matrix);
    ScaleMatrix(&transform_matrix, &SWIRL_DATA->scale);
    SetRotMatrix(&transform_matrix);
    SetTransMatrix(&transform_matrix);

    SWIRL_DATA->rotation.vz -= SWIRL_ROTATION_STEP;
    delta = SWIRL_DATA->scaleDelta >> 4;
    SWIRL_DATA->scaleDelta += SWIRL_SCALE_ACCELERATION;
    SWIRL_DATA->scale.vx += delta;
    SWIRL_DATA->scale.vy += delta;

    for (j = 0; j < SWIRL_VERTEX_ROWS; j++) {
        for (i = 0; i < SWIRL_VERTEX_COLS; i++) {
            RotTransPers(&SWIRL_DATA->vertices[i][j], (long*)&SWIRL_DATA->points[i][j], &p, &flag);
        }
    }

    ClearOTag(&buf->ot, 1);
    for (j = 0; j < SWIRL_QUAD_ROWS; j++) {
        for (i = 0; i < SWIRL_QUAD_COLS; i++) {
            setXY4(&buf->quads[i][j], SWIRL_DATA->points[i][j].vx, SWIRL_DATA->points[i][j].vy,
                   SWIRL_DATA->points[i + 1][j].vx, SWIRL_DATA->points[i + 1][j].vy, SWIRL_DATA->points[i][j + 1].vx,
                   SWIRL_DATA->points[i][j + 1].vy, SWIRL_DATA->points[i + 1][j + 1].vx,
                   SWIRL_DATA->points[i + 1][j + 1].vy);
            AddPrim(&buf->ot, &buf->quads[i][j]);
        }
    }
}

void SysBattleSwirlRender(void) {
    SWIRL_DATA->renderCounter++;
    if (!(SWIRL_DATA->renderCounter & 1)) {
        DrawOTag(SWIRL_DATA->activeOt);
        SysBattleSwirlUpdate();
    }
}

void SysBattleSwirlInit(void) {
    RECT rect, prevRect;
    u_long *pixels, *prevPixels;
    SwirlPacketBuffer* buf;
    s32 i, j, screenY;

    D_80095DD4 = 0;
    VSync(0);
    GetDispEnv(&SWIRL_DATA->dispEnv);
    screenY = SWIRL_DATA->dispEnv.screen.y > SWIRL_SCREEN_Y_THRESHOLD ? SWIRL_SCREEN_Y_OFFSET : 0;
    SetDefDispEnv(&SWIRL_DATA->dispEnv, 0, SWIRL_DISPLAY_Y, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&SWIRL_DATA->drawEnv, 0, SWIRL_DRAW_Y, SCREEN_WIDTH, DRAW_HEIGHT);
    SWIRL_DATA->dispEnv.screen.y = screenY;
    SWIRL_DATA->dispEnv.isrgb24 = 0;
    SWIRL_DATA->drawEnv.dtd = 0;
    SWIRL_DATA->drawEnv.dfe = 1;
    SWIRL_DATA->drawEnv.isbg = 0;
    SWIRL_DATA->drawEnv.tpage = 0;
    PutDispEnv(&SWIRL_DATA->dispEnv);
    PutDrawEnv(&SWIRL_DATA->drawEnv);
    SetGeomOffset(SWIRL_GEOM_OFFSET_X, SWIRL_GEOM_OFFSET_Y);
    SetGeomScreen(SWIRL_PROJECTION_DISTANCE);
    SWIRL_DATA->renderCounter = 0;

    if (!D_80075DEC) {
        setRECT(&rect, 0, SWIRL_TEXTURE_Y, SCREEN_WIDTH, DRAW_HEIGHT);
        MoveImage(&rect, 0, SWIRL_DRAW_Y);
    }

    for (i = 0; i < SWIRL_STRIP_COUNT + 1; i++) {
        prevRect = rect;
        prevPixels = pixels;
        pixels = SWIRL_PIXEL_BUFFER_A;
        setRECT(&rect, 0, i * SWIRL_STRIP_HEIGHT + SWIRL_DRAW_Y, SCREEN_WIDTH, SWIRL_STRIP_HEIGHT);
        if (i & 1) {
            pixels = SWIRL_PIXEL_BUFFER_B;
        }
        DrawSync(0);
        if (i > 0) {
            for (j = 0; j < SWIRL_STRIP_WORD_COUNT; j++) {
                prevPixels[j] |= SWIRL_PIXEL_PAIR_STP_MASK;
            }
            LoadImage(&prevRect, prevPixels);
        }
        if (i < SWIRL_STRIP_COUNT) {
            StoreImage(&rect, pixels);
        }
    }

    setVector(&SWIRL_DATA->scale, SWIRL_INITIAL_SCALE_XY, SWIRL_INITIAL_SCALE_XY, SWIRL_SCALE_ONE);
    SWIRL_DATA->scaleDelta = SWIRL_INITIAL_SCALE_DELTA;
    setVector(&SWIRL_DATA->rotation, 0, 0, 0);
    SWIRL_DATA->frame = 0;
    SWIRL_DATA->colorIntensity = SWIRL_INITIAL_INTENSITY;

    for (j = 0; j < SWIRL_VERTEX_ROWS; j++) {
        for (i = 0; i < SWIRL_VERTEX_COLS; i++) {
            setVector(&SWIRL_DATA->vertices[i][j], i * SWIRL_CELL_SIZE - SWIRL_CENTER_X,
                      j * SWIRL_CELL_SIZE - SWIRL_CENTER_Y, 0);
        }
    }

    buf = &SWIRL_DATA->buffers[0];
    for (j = 0; j < SWIRL_QUAD_ROWS; j++) {
        for (i = 0; i < SWIRL_QUAD_COLS; i++) {
            SetPolyFT4(&buf->quads[i][j]);
            SetSemiTrans(&buf->quads[i][j], 1);
            SetShadeTex(&buf->quads[i][j], 0);
            setRGB0(&buf->quads[i][j], SWIRL_INITIAL_INTENSITY, SWIRL_INITIAL_INTENSITY, SWIRL_INITIAL_INTENSITY);
            setUVWH(&buf->quads[i][j], (i * SWIRL_CELL_SIZE) & SWIRL_TPAGE_X_MASK,
                    j * SWIRL_CELL_SIZE + SWIRL_TEXTURE_Y, SWIRL_CELL_UV_EXTENT, SWIRL_CELL_UV_EXTENT);
        }
    }

    SysMemCopy32(&SWIRL_DATA->buffers[1], &SWIRL_DATA->buffers[0], sizeof(SwirlPacketBuffer));
    SysBattleSwirlUpdate();
    D_80095DD4 = 3;
}
