# Handwritten GTE routines, placed between the compiled code and the data.
.include "macro.inc"

.text
.set push
.set noreorder
.set noat
.align 2

glabel D_800B3BF8
    .word 0x00000000
.size D_800B3BF8, . - D_800B3BF8


glabel func_800B3BFC
    lw         $a3, 0xC($a0)
    lw         $a2, 0x8($a0)
    lw         $a1, 0x4($a0)
    lw         $a0, 0x0($a0)
    lh         $v1, 0x4($a3)
    nop
.L800B3C14:
    lwc2       $0, 0x0($a0)
    lwc2       $1, 0x4($a0)
    rtps
    mfc2       $t2, $8
    lwc2       $0, 0x8($a0)
    lwc2       $1, 0xC($a0)
    rtps
    mfc2       $t3, $8
    lwc2       $0, 0x10($a0)
    lwc2       $1, 0x14($a0)
    rtps
    nclip
    mfc2       $t0, $24
    nop
    bltz       $t0, .L800B3D10
    avsz3
    mfc2       $t7, $7
    nop
    blez       $t7, .L800B3D10
    addu      $t0, $t7, $zero
    addi       $t0, $t0, -0x1000
    bgtz       $t0, .L800B3D10
    swc2      $12, 0x8($a1)
    swc2       $13, 0x10($a1)
    swc2       $14, 0x18($a1)
    lwc2       $6, 0x20($a0)
    sll        $t7, $t7, 2
    add        $t7, $t7, $a2
    dpcs
    swc2       $22, 0x14($a1)
    mtc2       $t3, $8
    lwc2       $6, 0x1C($a0)
    nop
    nop
    dpcs
    swc2       $22, 0xC($a1)
    mtc2       $t2, $8
    lwc2       $6, 0x18($a0)
    nop
    nop
    dpcs
    mfc2       $t6, $22
    nop
    sb         $t6, 0x4($a1)
    sw         $t6, 0x4($a1)
    lw         $t1, 0x0($t7)
    lw         $t0, 0x0($a1)
    addu       $t3, $t1, $zero
    lui        $at, (0xFF000000 >> 16)
    and        $t0, $t0, $at
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $t1, $t1, $at
    or         $t2, $t0, $t1
    sw         $t2, 0x0($a1)
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $a1, $a1, $at
    lui        $at, (0xFF000000 >> 16)
    and        $t3, $t3, $at
    or         $t2, $a1, $t3
    sw         $t2, 0x0($t7)
    addi       $a1, $a1, 0x1C
.L800B3D10:
    addi       $a0, $a0, 0x24
    addi       $v1, $v1, -0x1
    bne        $zero, $v1, .L800B3C14
    addu      $v0, $a1, $zero
    jr         $ra
    nop
.size func_800B3BFC, . - func_800B3BFC


glabel func_800B3D28
    lw         $a3, 0xC($a0)
    lw         $a2, 0x8($a0)
    lw         $a1, 0x4($a0)
    lw         $a0, 0x0($a0)
    lh         $v1, 0x4($a3)
    nop
.L800B3D40:
    lwc2       $0, 0x0($a0)
    lwc2       $1, 0x4($a0)
    rtps
    mfc2       $t2, $8
    lwc2       $0, 0x8($a0)
    lwc2       $1, 0xC($a0)
    rtps
    mfc2       $t3, $8
    lwc2       $0, 0x10($a0)
    lwc2       $1, 0x14($a0)
    rtps
    nclip
    mfc2       $t0, $24
    nop
    bltz       $t0, .L800B3DEC
    addu      $t7, $a2, $zero
    swc2       $12, 0x8($a1)
    swc2       $13, 0x10($a1)
    swc2       $14, 0x18($a1)
    lw         $t6, 0x20($a0)
    lw         $t5, 0x1C($a0)
    lw         $t4, 0x18($a0)
    sw         $t6, 0x14($a1)
    sw         $t5, 0xC($a1)
    sw         $t4, 0x4($a1)
    lw         $t1, 0x0($t7)
    lw         $t0, 0x0($a1)
    addu       $t3, $t1, $zero
    lui        $at, (0xFF000000 >> 16)
    and        $t0, $t0, $at
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $t1, $t1, $at
    or         $t2, $t0, $t1
    sw         $t2, 0x0($a1)
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $a1, $a1, $at
    lui        $at, (0xFF000000 >> 16)
    and        $t3, $t3, $at
    or         $t2, $a1, $t3
    sw         $t2, 0x0($t7)
    addi       $a1, $a1, 0x1C
.L800B3DEC:
    addi       $a0, $a0, 0x24
    addi       $v1, $v1, -0x1
    bne        $zero, $v1, .L800B3D40
    addu      $v0, $a1, $zero
    jr         $ra
    nop
.size func_800B3D28, . - func_800B3D28


glabel func_800B3E04
    lwc2       $0, 0x0($a0)
    lwc2       $1, 0x4($a0)
    lwc2       $2, 0x8($a0)
    lwc2       $3, 0xC($a0)
    lwc2       $4, 0x10($a0)
    lwc2       $5, 0x14($a0)
    rtpt
    nclip
    mfc2       $t0, $24
    avsz3
    bltz       $t0, .L800B3EB4
    addu      $t7, $a2, $zero
    swc2       $12, 0x8($a1)
    swc2       $13, 0x14($a1)
    swc2       $14, 0x20($a1)
    lui        $t0, (0x36808080 >> 16)
    ori        $t0, $t0, (0x36808080 & 0xFFFF)
    sw         $t0, 0x4($a1)
    sw         $t0, 0x10($a1)
    sw         $t0, 0x1C($a1)
    lw         $t5, 0x1C($a0)
    lw         $t6, 0x20($a0)
    lw         $t4, 0x24($a0)
    sw         $t5, 0xC($a1)
    sw         $t6, 0x18($a1)
    sw         $t4, 0x24($a1)
    lw         $t1, 0x0($t7)
    lw         $t0, 0x0($a1)
    addu       $t3, $t1, $zero
    lui        $at, (0xFF000000 >> 16)
    and        $t0, $t0, $at
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $t1, $t1, $at
    or         $t2, $t0, $t1
    sw         $t2, 0x0($a1)
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $a1, $a1, $at
    lui        $at, (0xFF000000 >> 16)
    and        $t3, $t3, $at
    or         $t2, $a1, $t3
    sw         $t2, 0x0($t7)
    addi       $a1, $a1, 0x28
.L800B3EB4:
    addu       $v0, $a1, $zero
    jr         $ra
    nop
.size func_800B3E04, . - func_800B3E04


glabel func_800B3EC0
    lwc2       $0, 0x0($a0)
    lwc2       $1, 0x4($a0)
    lwc2       $2, 0x8($a0)
    lwc2       $3, 0xC($a0)
    lwc2       $4, 0x10($a0)
    lwc2       $5, 0x14($a0)
    rtpt
    nclip
    mfc2       $t0, $24
    avsz3
    bltz       $t0, .L800B3F68
    swc2      $12, 0x8($a1)
    swc2       $13, 0x10($a1)
    swc2       $14, 0x18($a1)
    lui        $t0, (0x26808080 >> 16)
    ori        $t0, $t0, (0x26808080 & 0xFFFF)
    addu       $t7, $a2, $zero
    sw         $t0, 0x4($a1)
    lw         $t5, 0x1C($a0)
    lw         $t6, 0x20($a0)
    lw         $t4, 0x24($a0)
    sw         $t5, 0xC($a1)
    sw         $t6, 0x14($a1)
    sw         $t4, 0x1C($a1)
    lw         $t1, 0x0($t7)
    lw         $t0, 0x0($a1)
    addu       $t3, $t1, $zero
    lui        $at, (0xFF000000 >> 16)
    and        $t0, $t0, $at
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $t1, $t1, $at
    or         $t2, $t0, $t1
    sw         $t2, 0x0($a1)
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $a1, $a1, $at
    lui        $at, (0xFF000000 >> 16)
    and        $t3, $t3, $at
    or         $t2, $a1, $t3
    sw         $t2, 0x0($t7)
    addi       $a1, $a1, 0x20
.L800B3F68:
    addu       $v0, $a1, $zero
    jr         $ra
    nop
.size func_800B3EC0, . - func_800B3EC0


glabel func_800B3F74
    lwc2       $0, 0x0($a0)
    lwc2       $1, 0x4($a0)
    lwc2       $2, 0x8($a0)
    lwc2       $3, 0xC($a0)
    lwc2       $4, 0x10($a0)
    lwc2       $5, 0x14($a0)
    rtpt
    nclip
    mfc2       $t0, $24
    nop
    bltz       $t0, .L800B4040
    avsz3
    mfc2       $t7, $7
    nop
    blez       $t7, .L800B4040
    swc2      $12, 0x8($a1)
    swc2       $13, 0x14($a1)
    swc2       $14, 0x20($a1)
    sll        $t7, $t7, 2
    add        $t7, $t7, $a2
    lui        $t6, (0x36808080 >> 16)
    ori        $t6, $t6, (0x36808080 & 0xFFFF)
    nop
    sw         $t6, 0x4($a1)
    sw         $t6, 0x10($a1)
    sw         $t6, 0x1C($a1)
    lw         $t5, 0x1C($a0)
    lw         $t6, 0x20($a0)
    sw         $t5, 0xC($a1)
    sw         $t6, 0x18($a1)
    lw         $t5, 0x24($a0)
    nop
    sw         $t5, 0x24($a1)
    lw         $t1, 0x0($t7)
    lw         $t0, 0x0($a1)
    addu       $t3, $t1, $zero
    lui        $at, (0xFF000000 >> 16)
    and        $t0, $t0, $at
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $t1, $t1, $at
    or         $t2, $t0, $t1
    sw         $t2, 0x0($a1)
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $a1, $a1, $at
    lui        $at, (0xFF000000 >> 16)
    and        $t3, $t3, $at
    or         $t2, $a1, $t3
    sw         $t2, 0x0($t7)
    addi       $a1, $a1, 0x28
.L800B4040:
    addu       $v0, $a1, $zero
    jr         $ra
    nop
.size func_800B3F74, . - func_800B3F74


glabel func_800B404C
    lwc2       $0, 0x0($a0)
    lwc2       $1, 0x4($a0)
    lwc2       $2, 0x8($a0)
    lwc2       $3, 0xC($a0)
    lwc2       $4, 0x10($a0)
    lwc2       $5, 0x14($a0)
    rtpt
    nclip
    mfc2       $t0, $24
    nop
    bltz       $t0, .L800B4108
    avsz3
    mfc2       $t7, $7
    lui        $t6, (0x26808080 >> 16)
    ori        $t6, $t6, (0x26808080 & 0xFFFF)
    blez       $t7, .L800B4108
    swc2      $12, 0x8($a1)
    swc2       $13, 0x10($a1)
    swc2       $14, 0x18($a1)
    sll        $t7, $t7, 2
    add        $t7, $t7, $a2
    sw         $t6, 0x4($a1)
    lw         $t5, 0x1C($a0)
    lw         $t6, 0x20($a0)
    sw         $t5, 0xC($a1)
    sw         $t6, 0x14($a1)
    lw         $t5, 0x24($a0)
    nop
    sw         $t5, 0x1C($a1)
    lw         $t1, 0x0($t7)
    lw         $t0, 0x0($a1)
    addu       $t3, $t1, $zero
    lui        $at, (0xFF000000 >> 16)
    and        $t0, $t0, $at
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $t1, $t1, $at
    or         $t2, $t0, $t1
    sw         $t2, 0x0($a1)
    lui        $at, (0xFFFFFF >> 16)
    ori        $at, $at, (0xFFFFFF & 0xFFFF)
    and        $a1, $a1, $at
    lui        $at, (0xFF000000 >> 16)
    and        $t3, $t3, $at
    or         $t2, $a1, $t3
    sw         $t2, 0x0($t7)
    addi       $a1, $a1, 0x20
.L800B4108:
    addu       $v0, $a1, $zero
    jr         $ra
    nop
.size func_800B404C, . - func_800B404C


glabel func_800B4114
    lwc2       $0, 0x0($a0)
    lwc2       $1, 0x4($a0)
    lwc2       $2, 0x8($a0)
    lwc2       $3, 0xC($a0)
    lwc2       $4, 0x10($a0)
    lwc2       $5, 0x14($a0)
    nop
    nop
    rtpt
    swc2       $12, 0x0($a1)
    swc2       $13, 0x4($a1)
    swc2       $14, 0x8($a1)
    jr         $ra
    nop
.size func_800B4114, . - func_800B4114


glabel func_800B414C
    lw         $t0, 0x10($a0)
    lw         $t1, 0x14($a0)
    addu       $v0, $t0, $zero
    sub        $t2, $t1, $t0
    bltz       $t2, .L800B4168
    nop
    addu       $v0, $t1, $zero
.L800B4168:
    lw         $t1, 0x50($a0)
    nop
    sub        $t2, $t1, $v0
    bltz       $t2, .L800B4180
    nop
    addu       $v0, $t1, $zero
.L800B4180:
    lw         $t1, 0x54($a0)
    ori        $t3, $zero, 0x1000
    sub        $t2, $t1, $v0
    bltz       $t2, .L800B4198
    nop
    addu       $v0, $t1, $zero
.L800B4198:
    sub        $t3, $t3, $v0
    bltz       $t3, .L800B41C0
    lw        $t0, 0x0($a1)
    lw         $t1, 0x4($a1)
    lw         $t2, 0x8($a1)
    lw         $t3, 0xC($a1)
    sw         $t0, 0xC($a3)
    sw         $t1, 0x14($a3)
    sw         $t2, 0x1C($a3)
    sw         $t3, 0x24($a3)
.L800B41C0:
    jr         $ra
    nop
.size func_800B414C, . - func_800B414C

.set pop
