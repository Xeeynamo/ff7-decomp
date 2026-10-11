#ifndef BATTLE_GTE_MATRIX_H
#define BATTLE_GTE_MATRIX_H

#ifdef PLATFORM_PSYZ
#include <libgte.h>
#else
#include <inline_c.h>
#define gte_ldclmv(p)                                                                                                  \
    __asm__ volatile("lhu $12,0(%0);lhu $13,6(%0);lhu $14,12(%0);mtc2 $12,$9;mtc2 $13,$10;mtc2 $14,$11"                \
                     :                                                                                                 \
                     : "r"(p)                                                                                          \
                     : "$12", "$13", "$14")
#define gte_rtir() __asm__ volatile("nop;nop;.word 0x4A49E012")
#define gte_stclmv(p)                                                                                                  \
    __asm__ volatile("mfc2 $12,$9;mfc2 $13,$10;mfc2 $14,$11;sh $12,0(%0);sh $13,6(%0);sh $14,12(%0)"                   \
                     :                                                                                                 \
                     : "r"(p)                                                                                          \
                     : "$12", "$13", "$14", "memory")

#endif

#endif
