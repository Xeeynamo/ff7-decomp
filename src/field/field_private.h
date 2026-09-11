#ifndef FIELD_PRIVATE_H
#define FIELD_PRIVATE_H

struct FieldRenderData {
    OT_TYPE ot[0x1000];   // 0x00000: Main scene ordering table
    SPRT_16 Arrows[0x18]; // 0x04000: Field arrow sprite packets
    DR_MODE ArrowsDm;     // 0x04180: Arrow sprite draw mode

    OT_TYPE OtFadeDrenv;  // 0x0418c: Fade draw environment OT entry
    OT_TYPE OtSceneDrenv; // 0x04190: Scene draw environment OT entry

    DR_ENV FadeDrenv;  // 0x04194: Screen fade draw environment
    DR_ENV SceneDrenv; // 0x041d4: Main scene draw environment

    DR_ENV BgDrenv3S; // 0x04214: Background layer 3 start env
    DR_ENV BgDrenv4S; // 0x04254: Background layer 4 start env
    DR_ENV BgDrenv3E; // 0x04294: Background layer 3 end env
    DR_ENV BgDrenv4E; // 0x042d4: Background layer 4 end env

    u8 unk4314[0x600]; // 0x04314: Unknown render data

    SPRT_16 Bg1[0x9c4]; // 0x04914: Background layer 1/2 sprites
    SPRT Bg2[0x200];    // 0x0e554: Background layer 3/4 sprites

    u16 BgAnim[0xbc4];   // 0x10d54: Background animation data
    DR_MODE BgDm[0x6a4]; // 0x124dc: Background draw mode packets

    OT_TYPE OtUi;       // 0x1748c: UI ordering table
    DR_MODE RainDm;     // 0x17490: Rain draw mode
    LINE_F2 Rain[0x40]; // 0x1749c: Rain line primitives
};

void StopFieldMapPreload(void);
s32 FieldCalcLinearStep(s32 start, s32 target, s32 duration, s32 step);
s32 FieldCalcWorldToScreenPos(SVECTOR* worldPos, long* screenPos);
s32 FieldEntityGetDirVectorY(u8 angle);
void FieldEntityLineClear(FieldLine* lines);
void DebugRunEveryLoop(void);
void FieldRainInit(struct FieldRenderData* renderData);
void FieldRainAddToRender(u32* ot, LINE_F2* rain, MATRIX* matrix, DR_MODE* rainDm);

#define GET_ENTITY_NAME(entityId) ((char*)g_FieldScripts + sizeof(FieldScriptHeader) + (entityId) * 8)

#define GET_FIELD_SCRIPT_OFFSET(dst, scriptOffset, entityDataOffset, extrasHeaderSize)                                 \
    (dst) = *((u8*)((scriptOffset) + (entityDataOffset) + (extrasHeaderSize) + (s32)g_FieldScripts) +                  \
              sizeof(FieldScriptHeader));                                                                              \
    (dst) |= *((u8*)((scriptOffset) + ((entityDataOffset) + (s32)g_FieldScripts) + (extrasHeaderSize)) +               \
               sizeof(FieldScriptHeader) + 1)                                                                          \
             << 8

#endif
