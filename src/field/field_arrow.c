//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

extern FieldTriggers* g_FieldTriggers;
extern u16 D_8011446C;

void FieldArrowsInit(SPRT_16* arrows, DR_MODE* drawMode) {
    s16 i;

    for (i = 0; i < 24; i++, arrows++) {
        SetSprt16(arrows);
        SetShadeTex(arrows, 1);
        SetSemiTrans(arrows, 0);
        setRGB0(arrows, 128, 128, 128);
        arrows->clut = GetClut(256, 489);
    }
    SetDrawMode(drawMode, 0, 1, GetTPage(0, 0, 960, 256), NULL);
}

void FieldArrowsAddToRender(struct FieldRenderData* renderData, MATRIX* matrix, FieldGateway* gateways) {
    SVECTOR pos;
    long p;
    long flag;
    s16 i;

    if ((Savemap.memory_bank_4[30] == 1 && !g_FieldState.characterLock) || Savemap.memory_bank_4[30] == 2) {
        PushMatrix();
        SetRotMatrix(matrix);
        SetTransMatrix(matrix);

        for (i = 0; i < 12; i++) {
            if (g_FieldTriggers->showArrow[i] == 1) {
                pos.vx = (gateways[i].pos.x1 + gateways[i].pos.x2) / 2;
                pos.vy = (gateways[i].pos.y1 + gateways[i].pos.y2) / 2;
                pos.vz = (gateways[i].pos.z1 + gateways[i].pos.z2) / 2;
                if (pos.vx || pos.vy) {
                    RotTransPers(&pos, (long*)&pos, &p, &flag);
                    setUV0(&renderData->arrows[i], ((D_8011446C * 4) & 0x30) + 48, 208);
                    renderData->arrows[i].x0 = pos.vx - 7;
                    renderData->arrows[i].y0 = pos.vy - 8;
                    addPrim(renderData->ot, &renderData->arrows[i]);
                }
            }
        }

        for (i = 0; i < 12; i++) {
            if (g_FieldTriggers->arrows[i].type) {
                pos.vx = g_FieldTriggers->arrows[i].x;
                pos.vy = g_FieldTriggers->arrows[i].z;
                pos.vz = g_FieldTriggers->arrows[i].y;
                RotTransPers(&pos, (long*)&pos, &p, &flag);
                setUV0(&renderData->arrows[i + 12], ((D_8011446C * 4) & 0x30) + 48, 208);
                renderData->arrows[i + 12].x0 = pos.vx - 7;
                renderData->arrows[i + 12].y0 = pos.vy - 8;
                if (g_FieldTriggers->arrows[i].type == 2) {
                    renderData->arrows[i + 12].clut = GetClut(256, 488);
                }
                addPrim(renderData->ot, &renderData->arrows[i + 12]);
            }
        }

        PopMatrix();
        addPrim(renderData->ot, &renderData->arrowsDm);
        D_8011446C++;
    }
}
