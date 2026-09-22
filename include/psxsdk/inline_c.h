#ifndef INLINE_C_H
#define INLINE_C_H

#include <libgte.h>

#ifndef VERSION_PC
/*
 * Type 1 functions
 */

#define gte_ldv0( r0 ) __asm__ volatile (			\
	"lwc2	$0, 0( %0 );"					\
	"lwc2	$1, 4( %0 )"					\
	:							\
	: "r"( r0 ) )

#define gte_ldv3( r0, r1, r2 ) __asm__ volatile (		\
	"lwc2	$0, 0( %0 );"					\
	"lwc2	$1, 4( %0 );"					\
	"lwc2	$2, 0( %1 );"					\
	"lwc2	$3, 4( %1 );"					\
	"lwc2	$4, 0( %2 );"					\
	"lwc2	$5, 4( %2 )"					\
	:							\
	: "r"( r0 ), "r"( r1 ), "r"( r2 ) )

#define gte_SetGeomScreen( r0 ) __asm__ volatile (		\
	"ctc2	%0, $26"					\
	:							\
	: "r"( r0 ) )

#define gte_SetRotMatrix( r0 ) __asm__ volatile (		\
	"lw	$12, 0( %0 );"					\
	"lw	$13, 4( %0 );"					\
	"ctc2	$12, $0;"					\
	"ctc2	$13, $1;"					\
	"lw	$12, 8( %0 );"					\
	"lw	$13, 12( %0 );"					\
	"lw	$14, 16( %0 );"					\
	"ctc2	$12, $2;"					\
	"ctc2	$13, $3;\n\t"					\
	"ctc2	$14, $4"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14" )

#define gte_SetTransMatrix( r0 ) __asm__ volatile (		\
	"lw	$12, 20( %0 );"					\
	"lw	$13, 24( %0 );"					\
	"ctc2	$12, $5;"					\
	"lw	$14, 28( %0 );"					\
	"ctc2	$13, $6;"					\
	"ctc2	$14, $7"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14" )

/*
 * Type 2 functions
 */

#define gte_rtps() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4A180001" )

#define gte_rtpt() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4A280030" )

#define gte_rt() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4A480012" )

#define gte_rtv0() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4A486012" )

#define gte_nclip() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4B400006" )

/*
 * Type 3 functions
 */

#define gte_stsxy( r0 ) __asm__ volatile (			\
	"swc2	$14, 0( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "memory" )

#define gte_stsxy3( r0, r1, r2 ) __asm__ volatile (		\
	"swc2	$12, 0( %0 );"					\
	"swc2	$13, 0( %1 );"					\
	"swc2	$14, 0( %2 )"					\
	:							\
	: "r"( r0 ), "r"( r1 ), "r"( r2 )			\
	: "memory" )

#define gte_stsxy2( r0 ) __asm__ volatile (			\
	"swc2	$14, 0( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "memory" )

#define gte_stsz( r0 ) __asm__ volatile (			\
	"swc2	$19, 0( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "memory" )

#define gte_stsz3( r0, r1, r2 ) __asm__ volatile (		\
	"swc2	$17, 0( %0 );"					\
	"swc2	$18, 0( %1 );"					\
	"swc2	$19, 0( %2 )"					\
	:							\
	: "r"( r0 ), "r"( r1 ), "r"( r2 )			\
	: "memory" )

#define gte_stszotz( r0 ) __asm__ volatile (			\
	"mfc2	$12, $19;"					\
	"nop;"							\
	"sra	$12, $12, 2;"					\
	"sw	$12, 0( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "$12", "memory" )

#define gte_stopz( r0 ) __asm__ volatile (			\
	"swc2	$24, 0( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "memory" )

#define gte_stlvnl( r0 ) __asm__ volatile (			\
	"swc2	$25, 0( %0 );"					\
	"swc2	$26, 4( %0 );"					\
	"swc2	$27, 8( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "memory" )
#define gte_rt2()                                                             \
    __asm__ volatile("nop;"                                                    \
                     "nop;"                                                    \
                     ".word 0x4A480012"                                        \
                     :                                                         \
                     :                                                         \
                     : "memory")

#define gte_SetRotMatrix2(r0)                                                  \
    __asm__ volatile(                                                          \
        "addu	$12, %0, $0;"                                                    \
        "lw	$13, 0( $12 );"                                                    \
        "lw	$14, 4( $12 );"                                                    \
        "ctc2	$13, $0;"                                                        \
        "ctc2	$14, $1;"                                                        \
        "lw	$13, 8( $12 );"                                                    \
        "lw	$14, 12( $12 );"                                                   \
        "lw	$15, 16( $12 );"                                                   \
        "ctc2	$13, $2;"                                                        \
        "ctc2	$14, $3;"                                                        \
        "ctc2	$15, $4"                                                         \
        :                                                                      \
        : "r"(r0)                                                              \
        : "$12", "$13", "$14", "$15", "memory")

#define gte_SetTransMatrix2(r0)                                                \
    __asm__ volatile(                                                          \
        "addu	$12, %0, $0;"                                                    \
        "lw	$13, 20( $12 );"                                                   \
        "lw	$14, 24( $12 );"                                                   \
        "ctc2	$13, $5;"                                                        \
        "lw	$15, 28( $12 );"                                                   \
        "ctc2	$14, $6;"                                                        \
        "ctc2	$15, $7"                                                         \
        :                                                                      \
        : "r"(r0)                                                              \
        : "$12", "$13", "$14", "$15", "memory")

#define gte_ldclmv2(r0)                                                        \
    __asm__ volatile(                                                          \
        "addu	$12, %0, $0;"                                                    \
        "lhu	$13, 0( $12 );"                                                   \
        "lhu	$14, 6( $12 );"                                                   \
        "lhu	$15, 12( $12 );"                                                  \
        "mtc2	$13, $9;"                                                        \
        "mtc2	$14, $10;"                                                       \
        "mtc2	$15, $11"                                                        \
        :                                                                      \
        : "r"(r0)                                                              \
        : "$12", "$13", "$14", "$15")

#define gte_ldlv0_2(r0)                                                        \
    __asm__ volatile(                                                          \
        "addu	$12, %0, $0;"                                                    \
        "lhu	$14, 4( $12 );"                                                   \
        "lhu	$13, 0( $12 );"                                                   \
        "sll	$14, $14, 16;"                                                    \
        "or	$13, $13, $14;"                                                    \
        "mtc2	$13, $0;"                                                        \
        "lwc2	$1, 8( $12 )"                                                    \
        :                                                                      \
        : "r"(r0)                                                              \
        : "$12", "$13", "$14")

#define gte_rtir()                                                             \
    __asm__ volatile("nop;"                                                    \
                     "nop;"                                                    \
                     ".word 0x4A49E012"                                        \
                     :                                                         \
                     :                                                         \
                     : "memory")

#define gte_stclmv2(r0)                                                        \
    __asm__ volatile(                                                          \
        "addu	$12, %0, $0;"                                                    \
        "mfc2	$13, $9;"                                                        \
        "mfc2	$14, $10;"                                                       \
        "mfc2	$15, $11;"                                                       \
        "sh	$13, 0( $12 );"                                                    \
        "sh	$14, 6( $12 );"                                                    \
        "sh	$15, 12( $12 )"                                                    \
        :                                                                      \
        : "r"(r0)                                                              \
        : "$12", "$13", "$14", "$15", "memory")

#define gte_stlvnl2(r0)                                                        \
    __asm__ volatile(                                                          \
        "addu	$12, %0, $0;"                                                    \
        "swc2	$9, 0( $12 );"                                                   \
        "swc2	$10, 4( $12 );"                                                  \
        "swc2	$11, 8( $12 )"                                                   \
        :                                                                      \
        : "r"(r0)                                                              \
        : "$12", "memory")

#else
#define gte_ldv0( r0 )
#define gte_ldv3( r0, r1, r2 )
#define gte_SetGeomScreen( r0 )
#define gte_SetRotMatrix( r0 )
#define gte_SetTransMatrix( r0 )
#define gte_rtps()
#define gte_rtpt()
#define gte_rt()
#define gte_rtv0()
#define gte_nclip()
#define gte_stsxy( r0 )
#define gte_stsxy3( r0, r1, r2 )
#define gte_stsxy2( r0 )
#define gte_stsz( r0 )
#define gte_stsz3( r0, r1, r2 )
#define gte_stszotz( r0 )
#define gte_stopz( r0 )
#define gte_stlvnl( r0 )
#define gte_SetRotMatrix2(r0)
#define gte_SetTransMatrix2(r0)
#define gte_ldclmv2(r0)
#define gte_ldlv0_2(r0)
#define gte_rtir()
#define gte_stclmv2(r0)
#define gte_stlvnl2(r0)
#endif

#endif
