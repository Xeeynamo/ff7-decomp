//! PSYQ=3.3

#include <game.h>

#include <libetc.h>
#include <psxsdk/inline_c.h>

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_model2", func_800B2760);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_model2", func_800B29E0);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_model2", func_800B2B38);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_model2", func_800B2E58);

extern u8 g_HighwayKawaiBufferIndex[4];

#ifdef PLATFORM_PSYZ
#define gte_ldmac(r0, r1, r2)
#define gte_strgb0(r0)
#define gte_strgb1(r0)
#define gte_strgb2(r0)
#else
#define gte_ldmac(r0, r1, r2)                                                                                          \
    __asm__ volatile("mtc2 %0, $25;"                                                                                   \
                     "mtc2 %1, $26;"                                                                                   \
                     "mtc2 %2, $27"                                                                                    \
                     :                                                                                                 \
                     : "r"(r0), "r"(r1), "r"(r2))
#define gte_strgb0(r0) __asm__ volatile("swc2 $20, 0(%0)" : : "r"(r0) : "memory")
#define gte_strgb1(r0) __asm__ volatile("swc2 $21, 0(%0)" : : "r"(r0) : "memory")
#define gte_strgb2(r0) __asm__ volatile("swc2 $22, 0(%0)" : : "r"(r0) : "memory")

#endif

// Lights one vertex: MAC = base color, IR = vertex color, then interpolates.
#define LIGHT_VERTEX(r, g, b, col)                                                                                     \
    gte_ldmac(r, g, b);                                                                                                \
    gte_ldcv(col);                                                                                                     \
    gte_gpl0()

void func_800B2F2C(FieldModelPart* part, s16 r, s16 g, s16 b) {
    u8* pkt;
    u8* poly;
    u32 counts;
    u32 n;
    u32 i;
    u8 code;
    s32 cr;
    s32 cg;
    s32 cb;

    pkt = part->packets;
    poly = part->data + part->polygonsOffset;
    if (g_HighwayKawaiBufferIndex[0]) {
        pkt += part->packetBufferSize;
    }
    gte_lddp(0x10);
    cr = r << 4;
    cg = g << 4;
    cb = b << 4;
    if (*(s32*)0x1F800200 & 1) {
        counts = *(u32*)&part->polyGT4Count;
        n = counts & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_GT4), poly += 0x18) {
            code = ((POLY_GT4*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            LIGHT_VERTEX(cr, cg, cb, poly + 8);
            LIGHT_VERTEX(cr, cg, cb, poly + 12);
            gte_strgb0(&((POLY_GT4*)pkt)->r0);
            gte_strgb1(&((POLY_GT4*)pkt)->r1);
            gte_strgb2(&((POLY_GT4*)pkt)->r2);
            LIGHT_VERTEX(cr, cg, cb, poly + 16);
            gte_strgb2(&((POLY_GT4*)pkt)->r3);
            ((POLY_GT4*)pkt)->code = code;
        }
        n = (counts & 0xFF00) >> 8;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_GT3), poly += 0x14) {
            code = ((POLY_GT3*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            LIGHT_VERTEX(cr, cg, cb, poly + 8);
            LIGHT_VERTEX(cr, cg, cb, poly + 12);
            gte_strgb0(&((POLY_GT3*)pkt)->r0);
            gte_strgb1(&((POLY_GT3*)pkt)->r1);
            gte_strgb2(&((POLY_GT3*)pkt)->r2);
            ((POLY_GT3*)pkt)->code = code;
        }
        n = (counts >> 16) & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_FT4), poly += 0xC) {
            code = ((POLY_FT4*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            gte_strgb2(&((POLY_FT4*)pkt)->r0);
            ((POLY_FT4*)pkt)->code = code;
        }
        n = counts >> 24;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_FT3), poly += 0xC) {
            code = ((POLY_FT3*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            gte_strgb2(&((POLY_FT3*)pkt)->r0);
            ((POLY_FT3*)pkt)->code = code;
        }
        counts = *(u32*)&part->polyF3Count;
        n = counts & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_F3), poly += 0x8) {
            code = ((POLY_F3*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            gte_strgb2(&((POLY_F3*)pkt)->r0);
            ((POLY_F3*)pkt)->code = code;
        }
        n = (counts & 0xFF00) >> 8;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_F4), poly += 0x8) {
            code = ((POLY_F4*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            gte_strgb2(&((POLY_F4*)pkt)->r0);
            ((POLY_F4*)pkt)->code = code;
        }
        n = (counts >> 16) & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_G3), poly += 0x10) {
            code = ((POLY_G3*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            LIGHT_VERTEX(cr, cg, cb, poly + 8);
            LIGHT_VERTEX(cr, cg, cb, poly + 12);
            gte_strgb0(&((POLY_G3*)pkt)->r0);
            gte_strgb1(&((POLY_G3*)pkt)->r1);
            gte_strgb2(&((POLY_G3*)pkt)->r2);
            ((POLY_G3*)pkt)->code = code;
        }
        n = counts >> 24;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_G4), poly += 0x14) {
            code = ((POLY_G4*)pkt)->code;
            LIGHT_VERTEX(cr, cg, cb, poly + 4);
            LIGHT_VERTEX(cr, cg, cb, poly + 8);
            LIGHT_VERTEX(cr, cg, cb, poly + 12);
            gte_strgb0(&((POLY_G4*)pkt)->r0);
            gte_strgb1(&((POLY_G4*)pkt)->r1);
            gte_strgb2(&((POLY_G4*)pkt)->r2);
            LIGHT_VERTEX(cr, cg, cb, poly + 16);
            gte_strgb2(&((POLY_G4*)pkt)->r3);
            ((POLY_G4*)pkt)->code = code;
        }
    } else {
        // Only relight packets that are linked into an ordering table.
        counts = *(u32*)&part->polyGT4Count;
        n = counts & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_GT4), poly += 0x18) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_GT4*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                LIGHT_VERTEX(cr, cg, cb, poly + 8);
                LIGHT_VERTEX(cr, cg, cb, poly + 12);
                gte_strgb0(&((POLY_GT4*)pkt)->r0);
                gte_strgb1(&((POLY_GT4*)pkt)->r1);
                gte_strgb2(&((POLY_GT4*)pkt)->r2);
                LIGHT_VERTEX(cr, cg, cb, poly + 16);
                gte_strgb2(&((POLY_GT4*)pkt)->r3);
                ((POLY_GT4*)pkt)->code = code;
            }
        }
        n = (counts & 0xFF00) >> 8;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_GT3), poly += 0x14) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_GT3*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                LIGHT_VERTEX(cr, cg, cb, poly + 8);
                LIGHT_VERTEX(cr, cg, cb, poly + 12);
                gte_strgb0(&((POLY_GT3*)pkt)->r0);
                gte_strgb1(&((POLY_GT3*)pkt)->r1);
                gte_strgb2(&((POLY_GT3*)pkt)->r2);
                ((POLY_GT3*)pkt)->code = code;
            }
        }
        n = (counts >> 16) & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_FT4), poly += 0xC) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_FT4*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                gte_strgb2(&((POLY_FT4*)pkt)->r0);
                ((POLY_FT4*)pkt)->code = code;
            }
        }
        n = counts >> 24;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_FT3), poly += 0xC) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_FT3*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                gte_strgb2(&((POLY_FT3*)pkt)->r0);
                ((POLY_FT3*)pkt)->code = code;
            }
        }
        counts = *(u32*)&part->polyF3Count;
        n = counts & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_F3), poly += 0x8) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_F3*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                gte_strgb2(&((POLY_F3*)pkt)->r0);
                ((POLY_F3*)pkt)->code = code;
            }
        }
        n = (counts & 0xFF00) >> 8;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_F4), poly += 0x8) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_F4*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                gte_strgb2(&((POLY_F4*)pkt)->r0);
                ((POLY_F4*)pkt)->code = code;
            }
        }
        n = (counts >> 16) & 0xFF;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_G3), poly += 0x10) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_G3*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                LIGHT_VERTEX(cr, cg, cb, poly + 8);
                LIGHT_VERTEX(cr, cg, cb, poly + 12);
                gte_strgb0(&((POLY_G3*)pkt)->r0);
                gte_strgb1(&((POLY_G3*)pkt)->r1);
                gte_strgb2(&((POLY_G3*)pkt)->r2);
                ((POLY_G3*)pkt)->code = code;
            }
        }
        n = counts >> 24;
        for (i = 0; i < n; i++, pkt += sizeof(POLY_G4), poly += 0x14) {
            if (*(u32*)pkt << 8) {
                code = ((POLY_G4*)pkt)->code;
                LIGHT_VERTEX(cr, cg, cb, poly + 4);
                LIGHT_VERTEX(cr, cg, cb, poly + 8);
                LIGHT_VERTEX(cr, cg, cb, poly + 12);
                gte_strgb0(&((POLY_G4*)pkt)->r0);
                gte_strgb1(&((POLY_G4*)pkt)->r1);
                gte_strgb2(&((POLY_G4*)pkt)->r2);
                LIGHT_VERTEX(cr, cg, cb, poly + 16);
                gte_strgb2(&((POLY_G4*)pkt)->r3);
                ((POLY_G4*)pkt)->code = code;
            }
        }
    }
}
