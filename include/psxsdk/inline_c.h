// clang-format off
#ifndef INLINE_C_H
#define INLINE_C_H
#include <libgte.h>

#ifdef PLATFORM_PSYZ

// the pc build has no gte, so every macro defined below needs a stand in here or a file
// that includes this header fails with an implicit declaration. inline_o.h already does this.
#define gte_ldv0(r0)
#define gte_ldv3(r0, r1, r2)
#define gte_ldv3c(r0)
#define gte_ldrgb(r0)
#define gte_ldcv(r0)
#define gte_lddp(r0)
#define gte_ldsxy3(r0, r1, r2)
#define gte_ldsz3(r0, r1, r2)
#define gte_strgb(r0)
#define gte_SetGeomScreen(r0)
#define gte_SetRotMatrix(r0)
#define gte_SetTransMatrix(r0)
#define gte_SetLightMatrix(r0)
#define gte_SetColorMatrix(r0)
#define gte_SetBackColor(r0, r1, r2)
#define gte_rtps()
#define gte_rtpt()
#define gte_rt()
#define gte_rtv0()
#define gte_rtv0tr()
#define gte_nclip()
#define gte_avsz3()
#define gte_nccs()
#define gte_gpl0()
#define gte_stsxy(r0)
#define gte_stsxy3(r0, r1, r2)
#define gte_stsxy3_g3(r0)
#define gte_stsxy2(r0)
#define gte_stsz(r0)
#define gte_stsz3(r0, r1, r2)
#define gte_stszotz(r0)
#define gte_stflg(r0)
#define gte_readflg(r0)
#define gte_stopz(r0)
#define gte_stlvnl(r0)
#define gte_ldlv0(r0)
#define gte_ldclmv(r0)
#define gte_rtir()
#define gte_stlvl(r0)
#define gte_stclmv(r0)
#define gte_ReadRotMatrix(r0)
#define gte_stsv(r0)
#define gte_ldsvllrow0(r0)
#else

/*
 * Type 1 functions
 */

#define gte_ReadRotMatrix( r0 ) __asm__ volatile (		\
	"cfc2	$12, $0;"					\
	"cfc2	$13, $1;"					\
	"sw	$12, 0( %0 );"					\
	"sw	$13, 4( %0 );"					\
	"cfc2	$12, $2;"					\
	"cfc2	$13, $3;"					\
	"cfc2	$14, $4;"					\
	"sw	$12, 8( %0 );"					\
	"sw	$13, 12( %0 );"					\
	"sw	$14, 16( %0 );"					\
	"cfc2	$12, $5;"					\
	"cfc2	$13, $6;"					\
	"cfc2	$14, $7;"					\
	"sw	$12, 20( %0 );"					\
	"sw	$13, 24( %0 );"					\
	"sw	$14, 28( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14", "memory" )

#define gte_stsv( r0 ) __asm__ volatile (			\
	"mfc2	$12, $9;"					\
	"mfc2	$13, $10;"					\
	"mfc2	$14, $11;"					\
	"sh	$12, 0( %0 );"					\
	"sh	$13, 2( %0 );"					\
	"sh	$14, 4( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14", "memory" )

#define gte_ldsvllrow0( r0 ) __asm__ volatile (			\
	"lw	$12, 0( %0 );"					\
	"lw	$13, 4( %0 );"					\
	"ctc2	$12, $8;"					\
	"ctc2	$13, $9"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13" )

#define gte_ldv0( r0 ) __asm__ volatile (			\
	"lwc2	$0, 0( %0 );"					\
	"lwc2	$1, 4( %0 )"					\
	:							\
	: "r"( r0 ) )

#define gte_ldrgb( r0 ) __asm__ volatile (			\
	"lwc2	$6, 0( %0 )"					\
	:							\
	: "r"( r0 ) )

#define gte_ldcv( r0 ) __asm__ volatile (			\
	"lbu	$12, 0( %0 );"					\
	"lbu	$13, 1( %0 );"					\
	"lbu	$14, 2( %0 );"					\
	"mtc2	$12, $9;"					\
	"mtc2	$13, $10;"					\
	"mtc2	$14, $11"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14" )

#define gte_lddp( r0 ) __asm__ volatile (			\
	"mtc2	%0, $8"						\
	:							\
	: "r"( r0 ) )

#define gte_ldsxy3( r0, r1, r2 ) __asm__ volatile (		\
	"mtc2	%0, $12;"					\
	"mtc2	%2, $14;"					\
	"mtc2	%1, $13"					\
	:							\
	: "r"( r0 ), "r"( r1 ), "r"( r2 ) )

#define gte_ldsz3( r0, r1, r2 ) __asm__ volatile (		\
	"mtc2	%0, $17;"					\
	"mtc2	%1, $18;"					\
	"mtc2	%2, $19"					\
	:							\
	: "r"( r0 ), "r"( r1 ), "r"( r2 ) )

#define gte_strgb( r0 ) __asm__ volatile (			\
	"swc2	$22, 0( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "memory" )

#define gte_ldv3( r0, r1, r2 ) __asm__ volatile (		\
	"lwc2	$0, 0( %0 );"					\
	"lwc2	$1, 4( %0 );"					\
	"lwc2	$2, 0( %1 );"					\
	"lwc2	$3, 4( %1 );"					\
	"lwc2	$4, 0( %2 );"					\
	"lwc2	$5, 4( %2 )"					\
	:							\
	: "r"( r0 ), "r"( r1 ), "r"( r2 ) )

#define gte_ldv3c( r0 ) __asm__ volatile (			\
	"lwc2	$0, 0( %0 );"					\
	"lwc2	$1, 4( %0 );"					\
	"lwc2	$2, 8( %0 );"					\
	"lwc2	$3, 12( %0 );"					\
	"lwc2	$4, 16( %0 );"					\
	"lwc2	$5, 20( %0 )"					\
	:							\
	: "r"( r0 ) )

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

#define gte_SetLightMatrix( r0 ) __asm__ volatile (		\
	"lw	$12, 0( %0 );"					\
	"lw	$13, 4( %0 );"					\
	"ctc2	$12, $8;"					\
	"ctc2	$13, $9;"					\
	"lw	$12, 8( %0 );"					\
	"lw	$13, 12( %0 );"					\
	"lw	$14, 16( %0 );"					\
	"ctc2	$12, $10;"					\
	"ctc2	$13, $11;\n\t"					\
	"ctc2	$14, $12"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14" )

#define gte_SetColorMatrix( r0 ) __asm__ volatile (		\
	"lw	$12, 0( %0 );"					\
	"lw	$13, 4( %0 );"					\
	"ctc2	$12, $16;"					\
	"ctc2	$13, $17;"					\
	"lw	$12, 8( %0 );"					\
	"lw	$13, 12( %0 );"					\
	"lw	$14, 16( %0 );"					\
	"ctc2	$12, $18;"					\
	"ctc2	$13, $19;\n\t"					\
	"ctc2	$14, $20"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14" )

#define gte_SetBackColor( r0, r1, r2 ) __asm__ volatile (	\
	"sll	$12, %0, 4;"					\
	"sll	$13, %1, 4;"					\
	"sll	$14, %2, 4;"					\
	"ctc2	$12, $13;"					\
	"ctc2	$13, $14;"					\
	"ctc2	$14, $15"					\
	:							\
	: "r"( r0 ), "r"( r1 ), "r"( r2 )			\
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

#define gte_rtv0tr() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4A480012" )

#define gte_nclip() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4B400006" )

#define gte_avsz3() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4B58002D" )

#define gte_nccs() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4B08041B" )

#define gte_gpl0() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4BA0003E" )

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

#define gte_stsxy3_g3( r0 ) __asm__ volatile (		\
	"swc2	$12, 8( %0 );"					\
	"swc2	$13, 16( %0 );"					\
	"swc2	$14, 24( %0 )"					\
	:							\
	: "r"( r0 )						\
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

#define gte_stflg( r0 ) __asm__ volatile (			\
	"cfc2	$12, $31;"					\
	"nop;"							\
	"sw	$12, 0( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "$12", "memory" )

// reads the GTE FLAG register into a variable; bit 31 is set on any error
#define gte_readflg( r0 ) __asm__ volatile (			\
	"cfc2	%0, $31;"					\
	"nop"							\
	: "=r"( r0 ) )

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

#define gte_ldlv0( r0 ) __asm__ volatile (			\
	"lhu	$13, 4( %0 );"					\
	"lhu	$12, 0( %0 );"					\
	"sll	$13, $13, 16;"					\
	"or	$12, $12, $13;"					\
	"mtc2	$12, $0;"					\
	"lwc2	$1, 8( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13" )

#define gte_ldclmv( r0 ) __asm__ volatile (			\
	"lhu	$12, 0( %0 );"					\
	"lhu	$13, 6( %0 );"					\
	"lhu	$14, 12( %0 );"					\
	"mtc2	$12, $9;"					\
	"mtc2	$13, $10;"					\
	"mtc2	$14, $11"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14" )

#define gte_rtir() __asm__ volatile (				\
	"nop;"							\
	"nop;"							\
	".word 0x4A49E012" )

#define gte_stlvl( r0 ) __asm__ volatile (			\
	"swc2	$9, 0( %0 );"					\
	"swc2	$10, 4( %0 );"					\
	"swc2	$11, 8( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "memory" )

#define gte_stclmv( r0 ) __asm__ volatile (			\
	"mfc2	$12, $9;"					\
	"mfc2	$13, $10;"					\
	"mfc2	$14, $11;"					\
	"sh	$12, 0( %0 );"					\
	"sh	$13, 6( %0 );"					\
	"sh	$14, 12( %0 )"					\
	:							\
	: "r"( r0 )						\
	: "$12", "$13", "$14", "memory" )
#endif

#endif
