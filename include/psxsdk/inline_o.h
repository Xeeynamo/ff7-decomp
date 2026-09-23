#ifndef INLINE_O_H
#define INLINE_O_H

#include <libgte.h>

#ifndef VERSION_PC
/*
 * Type 1 functions
 */

#define gte_ldv0(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $0,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $1,4($12)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_ldv3(r1,r2,r3) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("move  $13,%0": :"r"(r2):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("move  $14,%0": :"r"(r3):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $0,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $1,4($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $2,0($13)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $3,4($13)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $4,0($14)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $5,4($14)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_SetGeomScreen(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $12,$26": : :"$12","$13","$14","$15","memory"); \
}

#define gte_SetRotMatrix(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lw    $13,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lw    $14,4($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $13,$0": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $14,$1": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lw    $13,8($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lw    $14,12($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lw    $15,16($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $13,$2": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $14,$3": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $15,$4": : :"$12","$13","$14","$15","memory"); \
}

#define gte_SetTransMatrix(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lw    $13,20($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lw    $14,24($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $13,$5": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lw    $15,28($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $14,$6": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("ctc2  $15,$7": : :"$12","$13","$14","$15","memory"); \
}

/*
 * Type 2 functions
 */

#define gte_rtps() { \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile (".word 0x4A180001": : :"$12","$13","$14","$15","memory"); \
}

#define gte_rtpt() { \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile (".word 0x4A280030": : :"$12","$13","$14","$15","memory"); \
}

#define gte_rt() { \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile (".word 0x4A480012": : :"$12","$13","$14","$15","memory"); \
}

#define gte_rtv0() { \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile (".word 0x4A486012": : :"$12","$13","$14","$15","memory"); \
}

#define gte_nclip() { \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile (".word 0x4B400006": : :"$12","$13","$14","$15","memory"); \
}

/*
 * Type 3 functions
 */

#define gte_stsxy(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $14,0($12)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stsxy3(r1,r2,r3) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("move  $13,%0": :"r"(r2):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("move  $14,%0": :"r"(r3):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $12,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $13,0($13)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $14,0($14)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stsxy2(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $14,0($12)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stsz(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $19,0($12)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stsz3(r1,r2,r3) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("move  $13,%0": :"r"(r2):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("move  $14,%0": :"r"(r3):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $17,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $18,0($13)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $19,0($14)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stszotz(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("mfc2  $13,$19": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("sra   $13,$13,2": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("sw    $13,0($12)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stopz(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $24,0($12)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stlvnl(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $25,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $26,4($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $27,8($12)": : :"$12","$13","$14","$15","memory"); \
}
#define gte_ldlv0(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lhu   $14,4($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lhu   $13,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("sll   $14,$14,16": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("or    $13,$13,$14": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("mtc2  $13,$0": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lwc2  $1,8($12)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_ldclmv(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lhu   $13,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lhu   $14,6($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("lhu   $15,12($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("mtc2  $13,$9": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("mtc2  $14,$10": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("mtc2  $15,$11": : :"$12","$13","$14","$15","memory"); \
}

#define gte_rtir() { \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("nop   ": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile (".word 0x4A49E012": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stclmv(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("mfc2  $13,$9": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("mfc2  $14,$10": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("mfc2  $15,$11": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("sh    $13,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("sh    $14,6($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("sh    $15,12($12)": : :"$12","$13","$14","$15","memory"); \
}

#define gte_stlvl(r1) { \
  __asm__ volatile ("move  $12,%0": :"r"(r1):"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $9,0($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $10,4($12)": : :"$12","$13","$14","$15","memory"); \
  __asm__ volatile ("swc2  $11,8($12)": : :"$12","$13","$14","$15","memory"); \
}

#else
#define gte_ldv0(r1)
#define gte_ldv3(r1,r2,r3)
#define gte_SetGeomScreen(r1)
#define gte_SetRotMatrix(r1)
#define gte_SetTransMatrix(r1)
#define gte_rtps()
#define gte_rtpt()
#define gte_rt()
#define gte_rtv0()
#define gte_nclip()
#define gte_stsxy(r1)
#define gte_stsxy3(r1,r2,r3)
#define gte_stsxy2(r1)
#define gte_stsz(r1)
#define gte_stsz3(r1,r2,r3)
#define gte_stszotz(r1)
#define gte_stopz(r1)
#define gte_stlvnl(r1)
#define gte_ldlv0( r0 )
#define gte_ldclmv( r0 )
#define gte_rtir()
#define gte_stclmv( r0 )
#define gte_stlvl( r0 )
#endif

#endif
