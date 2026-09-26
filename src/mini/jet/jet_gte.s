# Handwritten GTE routines. They follow the overlay's data rather than its
# text, so they are assembled into .data to keep the linker from hoisting
# them ahead of it.
.include "macro.inc"

.data
.set push
.set noreorder
.set noat
.align 2

.globl JetProject6Points
.type JetProject6Points, @function
JetProject6Points:
    lwc2 $0, 0x0($a0)
    lwc2 $1, 0x4($a0)
    lwc2 $2, 0x8($a0)
    lwc2 $3, 0xC($a0)
    lwc2 $4, 0x10($a0)
    lwc2 $5, 0x14($a0)
    nRTPT
    swc2 $12, 0x0($a1)
    swc2 $13, 0x4($a1)
    swc2 $14, 0x8($a1)
    ori $t0, $zero, 0x18
    ori $t1, $zero, 0xC
    add $a0, $a0, $t0
    add $a1, $a1, $t1
    lwc2 $0, 0x0($a0)
    lwc2 $1, 0x4($a0)
    lwc2 $2, 0x8($a0)
    lwc2 $3, 0xC($a0)
    lwc2 $4, 0x10($a0)
    lwc2 $5, 0x14($a0)
    nRTPT
    swc2 $12, 0x0($a1)
    swc2 $13, 0x4($a1)
    swc2 $14, 0x8($a1)
    ori $t0, $zero, 0x18
    ori $t1, $zero, 0xC
    add $a0, $a0, $t0
    add $a1, $a1, $t1
    lwc2 $0, 0x0($a0)
    lwc2 $1, 0x4($a0)
    lwc2 $2, 0x8($a0)
    lwc2 $3, 0xC($a0)
    lwc2 $4, 0x10($a0)
    lwc2 $5, 0x14($a0)
    nRTPT
    swc2 $12, 0x0($a1)
    swc2 $13, 0x4($a1)
    jr $ra
    nop
.size JetProject6Points, . - JetProject6Points

.globl JetProject3Points
.type JetProject3Points, @function
JetProject3Points:
    lwc2 $0, 0x0($a0)
    lwc2 $1, 0x4($a0)
    lwc2 $2, 0x8($a0)
    lwc2 $3, 0xC($a0)
    lwc2 $4, 0x10($a0)
    lwc2 $5, 0x14($a0)
    nRTPT
    swc2 $12, 0x0($a1)
    swc2 $13, 0x4($a1)
    swc2 $14, 0x8($a1)
    jr $ra
    nop
.size JetProject3Points, . - JetProject3Points

.globl JetDrawModelTris
.type JetDrawModelTris, @function
JetDrawModelTris:
    lw $a3, 0xC($a0)
    lw $a2, 0x8($a0)
    lw $a1, 0x4($a0)
    lw $a0, 0x0($a0)
    lh $v1, 0x4($a3)
    nop
.L800A84F4:
    lwc2 $0, 0x0($a0)
    lwc2 $1, 0x4($a0)
    rtps
    mfc2 $t2, $8
    lwc2 $0, 0x8($a0)
    lwc2 $1, 0xC($a0)
    rtps
    mfc2 $t3, $8
    lwc2 $0, 0x10($a0)
    lwc2 $1, 0x14($a0)
    rtps
    nclip
    mfc2 $t0, $24
    nop
    bltz $t0, .L800A85EC
    avsz3
    mfc2 $t7, $7
    nop
    blez $t7, .L800A85EC
    addu $t0, $t7, $zero
    addi $t0, $t0, -0x1000
    bgtz $t0, .L800A85EC
    swc2 $12, 0x8($a1)
    swc2 $13, 0x10($a1)
    swc2 $14, 0x18($a1)
    lwc2 $6, 0x20($a0)
    sll $t7, $t7, 2
    add $t7, $t7, $a2
    nop
    dpcs
    mtc2 $t3, $8
    lwc2 $6, 0x1C($a0)
    swc2 $22, 0x14($a1)
    nop
    dpcs
    mtc2 $t2, $8
    lwc2 $6, 0x18($a0)
    swc2 $22, 0xC($a1)
    nop
    dpcs
    mfc2 $t6, $22
    nop
    sb $t6, 0x4($a1)
    sw $t6, 0x4($a1)
    lw $t1, 0x0($t7)
    lw $t0, 0x0($a1)
    addu $t3, $t1, $zero
    lui $at, (0xFF000000 >> 16)
    and $t0, $t0, $at
    lui $at, (0xFFFFFF >> 16)
    ori $at, $at, (0xFFFFFF & 0xFFFF)
    and $t1, $t1, $at
    or $t2, $t0, $t1
    sw $t2, 0x0($a1)
    lui $at, (0xFFFFFF >> 16)
    ori $at, $at, (0xFFFFFF & 0xFFFF)
    and $a1, $a1, $at
    lui $at, (0xFF000000 >> 16)
    and $t3, $t3, $at
    or $t2, $a1, $t3
    sw $t2, 0x0($t7)
    addi $a1, $a1, 0x1C
.L800A85EC:
    addi $a0, $a0, 0x24
    addi $v1, $v1, -0x1
    bne $zero, $v1, .L800A84F4
    addu $v0, $a1, $zero
    jr $ra
    nop
.size JetDrawModelTris, . - JetDrawModelTris

.globl JetDrawModelTrisUI
.type JetDrawModelTrisUI, @function
JetDrawModelTrisUI:
    lw $a3, 0xC($a0)
    lw $a2, 0x8($a0)
    lw $a1, 0x4($a0)
    lw $a0, 0x0($a0)
    lh $v1, 0x4($a3)
    nop
.L800A861C:
    lwc2 $0, 0x0($a0)
    lwc2 $1, 0x4($a0)
    rtps
    mfc2 $t2, $8
    lwc2 $0, 0x8($a0)
    lwc2 $1, 0xC($a0)
    rtps
    mfc2 $t3, $8
    lwc2 $0, 0x10($a0)
    lwc2 $1, 0x14($a0)
    rtps
    nclip
    mfc2 $t0, $24
    nop
    bltz $t0, .L800A871C
    avsz3
    mfc2 $t7, $7
    nop
    srl $t7, $t7, 4
    nop
    blez $t7, .L800A871C
    addu $t0, $t7, $zero
    addi $t0, $t0, -0x1000
    bgtz $t0, .L800A871C
    swc2 $12, 0x8($a1)
    swc2 $13, 0x10($a1)
    swc2 $14, 0x18($a1)
    lwc2 $6, 0x20($a0)
    sll $t7, $t7, 2
    add $t7, $t7, $a2
    nop
    dpcs
    mtc2 $t3, $8
    lwc2 $6, 0x1C($a0)
    swc2 $22, 0x14($a1)
    nop
    dpcs
    mtc2 $t2, $8
    lwc2 $6, 0x18($a0)
    swc2 $22, 0xC($a1)
    nop
    dpcs
    mfc2 $t6, $22
    nop
    sb $t6, 0x4($a1)
    sw $t6, 0x4($a1)
    lw $t1, 0x0($t7)
    lw $t0, 0x0($a1)
    addu $t3, $t1, $zero
    lui $at, (0xFF000000 >> 16)
    and $t0, $t0, $at
    lui $at, (0xFFFFFF >> 16)
    ori $at, $at, (0xFFFFFF & 0xFFFF)
    and $t1, $t1, $at
    or $t2, $t0, $t1
    sw $t2, 0x0($a1)
    lui $at, (0xFFFFFF >> 16)
    ori $at, $at, (0xFFFFFF & 0xFFFF)
    and $a1, $a1, $at
    lui $at, (0xFF000000 >> 16)
    and $t3, $t3, $at
    or $t2, $a1, $t3
    sw $t2, 0x0($t7)
    addi $a1, $a1, 0x1C
.L800A871C:
    addi $a0, $a0, 0x24
    addi $v1, $v1, -0x1
    bne $zero, $v1, .L800A861C
    addu $v0, $a1, $zero
    jr $ra
    nop
.size JetDrawModelTrisUI, . - JetDrawModelTrisUI

.globl JetDrawTriangle
.type JetDrawTriangle, @function
JetDrawTriangle:
    lwc2 $0, 0x0($a0)
    lwc2 $1, 0x4($a0)
    rtps
    mfc2 $t2, $8
    lwc2 $0, 0x8($a0)
    lwc2 $1, 0xC($a0)
    rtps
    mfc2 $t3, $8
    lwc2 $0, 0x10($a0)
    lwc2 $1, 0x14($a0)
    rtps
    nclip
    mfc2 $t0, $24
    nop
    bltz $t0, .L800A8820
    avsz3
    mfc2 $t7, $7
    nop
    blez $t7, .L800A8820
    addu $t6, $t7, $zero
    addi $t6, $t6, -0xFA0
    bgtz $t6, .L800A8820
    swc2 $12, 0x8($a1)
    swc2 $13, 0x10($a1)
    swc2 $14, 0x18($a1)
    lwc2 $6, 0x20($a0)
    sll $t7, $t7, 2
    add $t7, $t7, $a2
    dpcs
    lwc2 $6, 0x1C($a0)
    swc2 $22, 0x14($a1)
    mtc2 $t3, $8
    dpcs
    lwc2 $6, 0x18($a0)
    swc2 $22, 0xC($a1)
    mtc2 $t2, $8
    dpcs
    mfc2 $t6, $22
    nop
    sb $t6, 0x4($a1)
    sw $t6, 0x4($a1)
    lw $t1, 0x0($t7)
    lw $t0, 0x0($a1)
    addu $t3, $t1, $zero
    lui $at, (0xFF000000 >> 16)
    and $t0, $t0, $at
    lui $at, (0xFFFFFF >> 16)
    ori $at, $at, (0xFFFFFF & 0xFFFF)
    and $t1, $t1, $at
    or $t2, $t0, $t1
    sw $t2, 0x0($a1)
    lui $at, (0xFFFFFF >> 16)
    ori $at, $at, (0xFFFFFF & 0xFFFF)
    and $a1, $a1, $at
    lui $at, (0xFF000000 >> 16)
    and $t3, $t3, $at
    or $t2, $a1, $t3
    sw $t2, 0x0($t7)
    addi $a1, $a1, 0x1C
.L800A8820:
    addu $v0, $a1, $zero
    jr $ra
    nop
.size JetDrawTriangle, . - JetDrawTriangle

.globl JetDrawTrackQuad
.type JetDrawTrackQuad, @function
JetDrawTrackQuad:
    lwc2 $0, 0x0($a0)
    lwc2 $1, 0x4($a0)
    lwc2 $2, 0x8($a3)
    lwc2 $3, 0xC($a3)
    lwc2 $4, 0x40($a0)
    lwc2 $5, 0x44($a0)
    rtpt
    avsz3
    mfc2 $t7, $7
    nop
    blez $t7, .L800A8910
    addu $t6, $t7, $zero
    addi $t6, $t6, -0xFA0
    bgtz $t6, .L800A8910
    swc2 $12, 0x8($a1)
    swc2 $13, 0x10($a1)
    swc2 $14, 0x18($a1)
    lwc2 $0, 0x48($a3)
    lwc2 $1, 0x4C($a3)
    rtps
    swc2 $14, 0x20($a1)
    ori $t6, $zero, 0x2C
    lui $t5, (0x808080 >> 16)
    ori $t5, $t5, (0x808080 & 0xFFFF)
    mtc2 $t5, $6
    sll $t7, $t7, 2
    add $t7, $t7, $a2
    dpcs
    swc2 $22, 0x4($a1)
    sb $t6, 0x7($a1)
    lui $t5, (0x78010000 >> 16)
    lui $t6, (0x2C001F >> 16)
    ori $t6, $t6, (0x2C001F & 0xFFFF)
    sw $t5, 0xC($a1)
    sw $t6, 0x14($a1)
    ori $t5, $zero, 0x1F00
    ori $t6, $zero, 0x1F1F
    sw $t5, 0x1C($a1)
    sw $t6, 0x24($a1)
    lw $t1, 0x0($t7)
    lw $t0, 0x0($a1)
    addu $t3, $t1, $zero
    lui $at, (0xFF000000 >> 16)
    and $t0, $t0, $at
    lui $at, (0xFFFFFF >> 16)
    ori $at, $at, (0xFFFFFF & 0xFFFF)
    and $t1, $t1, $at
    or $t2, $t0, $t1
    sw $t2, 0x0($a1)
    lui $at, (0xFFFFFF >> 16)
    ori $at, $at, (0xFFFFFF & 0xFFFF)
    and $a1, $a1, $at
    lui $at, (0xFF000000 >> 16)
    and $t3, $t3, $at
    or $t2, $a1, $t3
    sw $t2, 0x0($t7)
    addi $a1, $a1, 0x28
.L800A8910:
    addu $v0, $a1, $zero
    jr $ra
.size JetDrawTrackQuad, . - JetDrawTrackQuad

.set pop
