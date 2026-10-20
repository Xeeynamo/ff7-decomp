#ifndef GTE_H
#define GTE_H

#ifdef PLATFORM_PSYZ
#define gte_ldVXY0(src)
#define gte_ldVZ0(src)
#define gte_ldIR1(src)
#define gte_ldIR2(src)
#define gte_ldIR3(src)
#define gte_ldMAC1(src)
#define gte_ldMAC2(src)
#define gte_ldMAC3(src)
#define gte_rdOTZ(dst)
#define gte_rdIR1_b(dst)
#define gte_rdIR2_b(dst)
#define gte_rdIR3_b(dst)
#define gte_rdMAC0(dst)
#define gte_rdMAC1(dst)
#define gte_rdMAC2(dst)
#define gte_rdMAC3(dst)
#define gte_stRGB0(dst)
#define gte_stRGB1(dst)
#define gte_stMAC2(dst, offs)
#define gte_stMAC3(dst, offs)
#define gte_ldR11R12(src)
#define gte_ldR13R21(src)
#define gte_ldR22R23(src)
#define gte_ldR31R32(src)
#define gte_ldR33(src)
#define gte_LoadPackedColor(color)
#else
// clang-format off

// Data registers
// From CPU to GTE
#define gte_ldVXY0(src)         __asm__ volatile("mtc2 %0, $0" : : "r"(src))
#define gte_ldVZ0(src)          __asm__ volatile("mtc2 %0, $1" : : "r"(src))
#define gte_ldIR1(src)          __asm__ volatile("mtc2 %0, $9" : : "r"(src))
#define gte_ldIR2(src)          __asm__ volatile("mtc2 %0, $10" : : "r"(src))
#define gte_ldIR3(src)          __asm__ volatile("mtc2 %0, $11" : : "r"(src))
#define gte_ldMAC1(src)         __asm__ volatile("mtc2 %0, $25" : : "r"(src))
#define gte_ldMAC2(src)         __asm__ volatile("mtc2 %0, $26" : : "r"(src))
#define gte_ldMAC3(src)         __asm__ volatile("mtc2 %0, $27" : : "r"(src))
// From GTE to CPU
#define gte_rdOTZ(dst)          __asm__ volatile("mfc2 %0, $7;nop;" : "=r"(dst))
#define gte_rdIR1_b(dst)        __asm__ volatile("mfc2 %0, $9" : "=r"(dst))
#define gte_rdIR2_b(dst)        __asm__ volatile("mfc2 %0, $10" : "=r"(dst))
#define gte_rdIR3_b(dst)        __asm__ volatile("mfc2 %0, $11" : "=r"(dst))
#define gte_rdMAC0(dst)         __asm__ volatile("mfc2 %0, $24;nop;" : "=r"(dst))
#define gte_rdMAC1(dst)         __asm__ volatile("mfc2 %0, $25;nop;" : "=r"(dst))
#define gte_rdMAC2(dst)         __asm__ volatile("mfc2 %0, $26;nop;" : "=r"(dst))
#define gte_rdMAC3(dst)         __asm__ volatile("mfc2 %0, $27;nop;" : "=r"(dst))
// From GTE to memory
#define gte_stRGB0(dst)         __asm__ volatile("swc2 $20, 0(%0)" : : "r"(dst) : "memory")
#define gte_stRGB1(dst)         __asm__ volatile("swc2 $21, 0(%0)" : : "r"(dst) : "memory")
#define gte_stMAC2(dst, offs)   __asm__ volatile("swc2 $26, %1(%0)" : : "r"(dst), "i"(offs) : "memory")
#define gte_stMAC3(dst, offs)   __asm__ volatile("swc2 $27, %1(%0)" : : "r"(dst), "i"(offs) : "memory")

// Control registers
// From CPU to GTE
#define gte_ldR11R12(src)       __asm__ volatile("ctc2 %0, $0" : : "r"(src))
#define gte_ldR13R21(src)       __asm__ volatile("ctc2 %0, $1" : : "r"(src))
#define gte_ldR22R23(src)       __asm__ volatile("ctc2 %0, $2" : : "r"(src))
#define gte_ldR31R32(src)       __asm__ volatile("ctc2 %0, $3" : : "r"(src))
#define gte_ldR33(src)          __asm__ volatile("ctc2 %0, $4" : : "r"(src))

// Writes 24-bit RGB color to GTE registers IR1-3. Uses register T4 as scratch.
#define gte_LoadPackedColor(color) {                                \
    __asm__ volatile("andi $12, %0, 0xFF" : : "r"(color) : "$12");  \
    __asm__ volatile("mtc2 $12, $9" : : : "$12");                   \
    __asm__ volatile("srl $12, %0, 8" : : "r"(color) : "$12");      \
    __asm__ volatile("andi $12, $12, 0xFF" : : : "$12");            \
    __asm__ volatile("mtc2 $12, $10" : : : "$12");                  \
    __asm__ volatile("srl $12, %0, 16" : : "r"(color) : "$12");     \
    __asm__ volatile("andi $12, $12, 0xFF" : : : "$12");            \
    __asm__ volatile("mtc2 $12, $11" : : : "$12");                  \
}
#endif

#define gte_LoadIR(r0, r1, r2) {    \
    gte_ldIR1(r0);                  \
    gte_ldIR2(r1);                  \
    gte_ldIR3(r2);                  \
}
#define gte_LoadMAC(r0, r1, r2) {   \
    gte_ldMAC1(r0);                 \
    gte_ldMAC2(r1);                 \
    gte_ldMAC3(r2);                 \
}
#define gte_ReadIR(r0, r1, r2) {    \
    gte_rdIR1_b(r0);                \
    gte_rdIR2_b(r1);                \
    gte_rdIR3_b(r2);                \
}
// clang-format on
#endif
