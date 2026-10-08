# Handwritten GTE model renderer and its helpers. func_800D32B4 and
# func_800D3354 return through $at, not $ra.
.include "macro.inc"

.text
.set push
.set noreorder
.set noat
.align 2

.globl BattleDrawModel
.type BattleDrawModel, @function
BattleDrawModel:
    sw $s0, -0x4($sp)
    sw $s1, -0x8($sp)
    sw $s2, -0xC($sp)
    sw $s3, -0x10($sp)
    sw $s4, -0x14($sp)
    sw $s5, -0x18($sp)
    sw $s6, -0x1C($sp)
    sw $ra, -0x20($sp)
    sw $s7, -0x24($sp)
    sw $fp, -0x28($sp)
    lui $v0, %hi(D_800D3544)
    ori $v0, $v0, %lo(D_800D3544)
    lhu $s7, 0x0($v0)
    lhu $fp, 0x2($v0)
    lw $s0, 0x4($a0)
    lhu $s1, 0x8($a0)
    lhu $s2, 0xA($a0)
    lhu $s3, 0xC($a0)
    lhu $s4, 0xE($a0)
    lw $a0, 0x0($a0)
    andi $v0, $s0, 0x80
    bnez $v0, .L800D2A44
    nop
    addu $v0, $s2, $zero
    sll $v0, $v0, 8
    or $s2, $s2, $v0
    sll $v0, $v0, 8
    or $s2, $s2, $v0
.L800D2A44:
    sll $s4, $s4, 16
    addiu $v0, $zero, 0xE
    sub $a2, $v0, $a2
    lw $v0, 0x0($a0)
    addiu $s5, $a0, 0x4
    addu $a0, $s5, $v0
    andi $v0, $s0, 0x8
    sll $v0, $v0, 22
    or $s2, $s2, $v0
    addu $s6, $zero, $zero
    addiu $t0, $zero, -0x1
    andi $v0, $s0, 0x1
    beqz $v0, .L800D2A84
    nop
    jal BattleModelFlipR11R21R31
    xor $s6, $s6, $t0
.L800D2A84:
    andi $v0, $s0, 0x2
    beqz $v0, .L800D2A98
    nop
    jal BattleModelFlipR12R22R32
    xor $s6, $s6, $t0
.L800D2A98:
    andi $v0, $s0, 0x4
    beqz $v0, .L800D2AAC
    nop
    jal BattleModelFlipR13R23R33
    xor $s6, $s6, $t0
.L800D2AAC:
    lui $at, %hi(.L800D2CD0)
    ori $at, $at, %lo(.L800D2CD0)
    andi $v0, $s0, 0x40
    bnez $v0, .L800D2AD0
    nop
    lh $v0, 0x2($a0)
    nop
    andi $v0, $v0, 0x19F
    add $s3, $v0, $s3
.L800D2AD0:
    lh $t8, 0x0($a0)
    addiu $a0, $a0, 0x4
    beqz $t8, .L800D2CD8
    addu $s7, $s7, $t8
    addiu $t8, $t8, -0x1
    lh $t4, 0x0($a0)
    lh $t5, 0x2($a0)
    lh $t6, 0x4($a0)
    addu $t4, $s5, $t4
    addu $t5, $s5, $t5
    addu $t6, $s5, $t6
.L800D2AFC:
    lwc2 $0, 0x0($t4)
    lwc2 $1, 0x4($t4)
    lwc2 $2, 0x0($t5)
    lwc2 $3, 0x4($t5)
    lwc2 $4, 0x0($t6)
    lwc2 $5, 0x4($t6)
    addiu $a0, $a0, 0x10
    rtpt
    lh $t4, 0x0($a0)
    lh $t5, 0x2($a0)
    lh $t6, 0x4($a0)
    addu $t4, $s5, $t4
    addu $t5, $s5, $t5
    addu $t6, $s5, $t6
    cfc2 $v0, $31
    nclip
    lui $v1, (0x60000 >> 16)
    and $v0, $v0, $v1
    bnez $v0, .L800D2CD0
    nop
    andi $v0, $s0, 0x20
    bnez $v0, .L800D2B70
    nop
    mfc2 $v0, $24
    nop
    beqz $v0, .L800D2CD0
    xor $v0, $v0, $s6
    bltz $v0, .L800D2CD0
    nop
.L800D2B70:
    mfc2 $t0, $12
    mfc2 $t1, $13
    mfc2 $t2, $14
    jal func_800D32B4
    nop
    sw $t0, 0x8($a3)
    sw $t1, 0x10($a3)
    sw $t2, 0x18($a3)
    avsz3
    mfc2 $t0, $7
    nop
    srav $t0, $t0, $a2
    sll $t0, $t0, 2
    addu $t0, $t0, $a1
    lw $t1, 0x0($t0)
    lui $v0, (0xFFFFFF >> 16)
    ori $v0, $v0, (0xFFFFFF & 0xFFFF)
    lui $v1, (0x7000000 >> 16)
    and $t1, $t1, $v0
    or $t1, $t1, $v1
    sw $t1, 0x0($a3)
    and $v0, $a3, $v0
    sw $v0, 0x0($t0)
    lw $t0, -0x8($a0)
    lh $t1, -0x4($a0)
    lh $t2, -0x2($a0)
    add $t0, $t0, $s4
    add $t0, $t0, $s1
    add $t1, $t1, $s1
    add $t2, $t2, $s1
    sw $t0, 0xC($a3)
    sh $t1, 0x14($a3)
    sh $t2, 0x1C($a3)
    lh $t0, -0xA($a0)
    andi $v0, $s0, 0x40
    beqz $v0, .L800D2C1C
    nop
    srl $v0, $t0, 8
    andi $v0, $v0, 0x19F
    add $v0, $v0, $s3
    sh $v0, 0x16($a3)
    j .L800D2C34
    addu $t1, $zero, $zero
.L800D2C1C:
    sh $s3, 0x16($a3)
    andi $v0, $s0, 0x100
    beqz $v0, .L800D2C34
    addu $t1, $zero, $zero
    andi $t1, $t0, 0xFF00
    sll $t1, $t1, 16
.L800D2C34:
    andi $v0, $s0, 0x80
    beqz $v0, .L800D2CB8
    nop
    lui $v0, (0xFF000000 >> 16)
    and $v0, $s2, $v0
    lui $v1, (0x24000000 >> 16)
    or $v0, $v0, $v1
    andi $v1, $t0, 0xFF
    or $v0, $v0, $v1
    sll $v1, $v1, 8
    or $v0, $v0, $v1
    sll $v1, $v1, 8
    or $v0, $v0, $v1
    andi $v1, $s0, 0x10
    bnez $v1, .L800D2CC0
    andi $v1, $s2, 0xFFFF
    beqz $v1, .L800D2CC0
    nop
    mtc2 $v1, $8
    mtc2 $v0, $6
    nop
    nop
    dpcs
    nop
    nop
    mfc2 $v0, $22
    srl $v1, $s2, 16
    sll $v1, $v1, 16
    or $v0, $v0, $v1
    or $v0, $v0, $t1
    sw $v0, 0x4($a3)
    j .L800D2CC8
    nop
.L800D2CB8:
    lui $v0, (0x24000000 >> 16)
    or $v0, $v0, $s2
.L800D2CC0:
    or $v0, $v0, $t1
    sw $v0, 0x4($a3)
.L800D2CC8:
    addiu $a3, $a3, 0x20
    addiu $fp, $fp, 0x1
.L800D2CD0:
    bnez $t8, .L800D2AFC
    addi $t8, $t8, -0x1
.L800D2CD8:
    lui $at, %hi(.L800D2F0C)
    ori $at, $at, %lo(.L800D2F0C)
    lw $t8, 0x0($a0)
    addiu $a0, $a0, 0x4
    beqz $t8, .L800D2F14
    addu $s7, $s7, $t8
    addu $s7, $s7, $t8
    addiu $t8, $t8, -0x1
    lh $t4, 0x0($a0)
    lh $t5, 0x2($a0)
    lh $t6, 0x4($a0)
    addu $t4, $s5, $t4
    addu $t5, $s5, $t5
    addu $t6, $s5, $t6
.L800D2D10:
    lwc2 $0, 0x0($t4)
    lwc2 $1, 0x4($t4)
    lwc2 $2, 0x0($t5)
    lwc2 $3, 0x4($t5)
    lwc2 $4, 0x0($t6)
    lwc2 $5, 0x4($t6)
    addiu $a0, $a0, 0x14
    rtpt
    lh $t4, 0x0($a0)
    lh $t5, 0x2($a0)
    lh $t6, 0x4($a0)
    addu $t4, $s5, $t4
    addu $t5, $s5, $t5
    addu $t6, $s5, $t6
    cfc2 $v0, $31
    nclip
    lui $v1, (0x60000 >> 16)
    and $v0, $v0, $v1
    bnez $v0, .L800D2F0C
    nop
    andi $v0, $s0, 0x20
    bnez $v0, .L800D2D84
    nop
    mfc2 $v0, $24
    nop
    beqz $v0, .L800D2D84
    xor $v0, $v0, $s6
    bltz $v0, .L800D2F0C
    nop
.L800D2D84:
    lh $t7, -0xE($a0)
    mfc2 $t0, $12
    addu $t7, $s5, $t7
    mfc2 $t1, $13
    lwc2 $0, 0x0($t7)
    lwc2 $1, 0x4($t7)
    mfc2 $t2, $14
    rtps
    mfc2 $t3, $14
    jal func_800D3354
    nop
    sw $t0, 0x8($a3)
    sw $t1, 0x10($a3)
    sw $t2, 0x18($a3)
    sw $t3, 0x20($a3)
    avsz4
    mfc2 $t0, $7
    nop
    srav $t0, $t0, $a2
    sll $t0, $t0, 2
    addu $t0, $t0, $a1
    lw $t1, 0x0($t0)
    lui $v0, (0xFFFFFF >> 16)
    ori $v0, $v0, (0xFFFFFF & 0xFFFF)
    lui $v1, (0x9000000 >> 16)
    and $t1, $t1, $v0
    or $t1, $t1, $v1
    sw $t1, 0x0($a3)
    and $v0, $a3, $v0
    sw $v0, 0x0($t0)
    lw $t0, -0xC($a0)
    lh $t1, -0x8($a0)
    lh $t2, -0x6($a0)
    lh $t3, -0x4($a0)
    add $t0, $t0, $s4
    add $t0, $t0, $s1
    add $t1, $t1, $s1
    add $t2, $t2, $s1
    add $t3, $t3, $s1
    sw $t0, 0xC($a3)
    sh $t1, 0x14($a3)
    sh $t2, 0x1C($a3)
    sh $t3, 0x24($a3)
    lh $t0, -0x2($a0)
    andi $v0, $s0, 0x40
    beqz $v0, .L800D2E58
    nop
    srl $v0, $t0, 8
    andi $v0, $v0, 0x19F
    add $v0, $v0, $s3
    sh $v0, 0x16($a3)
    j .L800D2E70
    addu $t1, $zero, $zero
.L800D2E58:
    sh $s3, 0x16($a3)
    andi $v0, $s0, 0x100
    beqz $v0, .L800D2E70
    addu $t1, $zero, $zero
    andi $t1, $t0, 0xFF00
    sll $t1, $t1, 16
.L800D2E70:
    andi $v0, $s0, 0x80
    beqz $v0, .L800D2EF4
    nop
    lui $v0, (0xFF000000 >> 16)
    and $v0, $s2, $v0
    lui $v1, (0x2C000000 >> 16)
    or $v0, $v0, $v1
    andi $v1, $t0, 0xFF
    or $v0, $v0, $v1
    sll $v1, $v1, 8
    or $v0, $v0, $v1
    sll $v1, $v1, 8
    or $v0, $v0, $v1
    andi $v1, $s0, 0x10
    bnez $v1, .L800D2EFC
    andi $v1, $s2, 0xFFFF
    beqz $v1, .L800D2EFC
    nop
    mtc2 $v1, $8
    mtc2 $v0, $6
    nop
    nop
    dpcs
    nop
    nop
    mfc2 $v0, $22
    srl $v1, $s1, 16
    sll $v1, $v1, 16
    or $v0, $v0, $v1
    or $v0, $v0, $t1
    sw $v0, 0x4($a3)
    j .L800D2F04
    nop
.L800D2EF4:
    lui $v0, (0x2C000000 >> 16)
    or $v0, $v0, $s2
.L800D2EFC:
    or $v0, $v0, $t1
    sw $v0, 0x4($a3)
.L800D2F04:
    addiu $a3, $a3, 0x28
    addiu $fp, $fp, 0x2
.L800D2F0C:
    bnez $t8, .L800D2D10
    addi $t8, $t8, -0x1
.L800D2F14:
    lui $at, %hi(.L800D3098)
    ori $at, $at, %lo(.L800D3098)
    lh $t8, 0x0($a0)
    addiu $a0, $a0, 0x4
    beqz $t8, .L800D30A0
    addu $s7, $s7, $t8
    addiu $t8, $t8, -0x1
    lh $t4, 0x0($a0)
    lh $t5, 0x2($a0)
    lh $t6, 0x4($a0)
    addu $t4, $s5, $t4
    addu $t5, $s5, $t5
    addu $t6, $s5, $t6
.L800D2F48:
    lwc2 $0, 0x0($t4)
    lwc2 $1, 0x4($t4)
    lwc2 $2, 0x0($t5)
    lwc2 $3, 0x4($t5)
    lwc2 $4, 0x0($t6)
    lwc2 $5, 0x4($t6)
    addiu $a0, $a0, 0x14
    rtpt
    lh $t4, 0x0($a0)
    lh $t5, 0x2($a0)
    lh $t6, 0x4($a0)
    addu $t4, $s5, $t4
    addu $t5, $s5, $t5
    addu $t6, $s5, $t6
    cfc2 $v0, $31
    nclip
    lui $v1, (0x60000 >> 16)
    and $v0, $v0, $v1
    bnez $v0, .L800D3098
    nop
    andi $v0, $s0, 0x20
    bnez $v0, .L800D2FBC
    nop
    mfc2 $v0, $24
    nop
    beqz $v0, .L800D3098
    xor $v0, $v0, $s6
    bltz $v0, .L800D3098
    nop
.L800D2FBC:
    mfc2 $t0, $12
    mfc2 $t1, $13
    mfc2 $t2, $14
    jal func_800D32B4
    nop
    sw $t0, 0x8($a3)
    sw $t1, 0x10($a3)
    sw $t2, 0x18($a3)
    avsz3
    mfc2 $t0, $7
    nop
    srav $t0, $t0, $a2
    sll $t0, $t0, 2
    addu $t0, $t0, $a1
    lw $t1, 0x0($t0)
    lui $v0, (0xFFFFFF >> 16)
    ori $v0, $v0, (0xFFFFFF & 0xFFFF)
    lui $v1, (0x6000000 >> 16)
    and $t1, $t1, $v0
    or $t1, $t1, $v1
    sw $t1, 0x0($a3)
    and $v0, $a3, $v0
    sw $v0, 0x0($t0)
    andi $v1, $s2, 0xFFFF
    beqz $v1, .L800D3074
    nop
    lw $v0, -0xC($a0)
    lwc2 $21, -0x8($a0)
    lwc2 $20, -0x4($a0)
    mtc2 $v0, $22
    mtc2 $v0, $6
    mtc2 $v1, $8
    nop
    nop
    dpct
    nop
    nop
    mfc2 $v0, $22
    swc2 $20, 0x14($a3)
    swc2 $21, 0xC($a3)
    srl $v1, $s2, 16
    sll $v1, $v1, 16
    or $v0, $v0, $v1
    sw $v0, 0x4($a3)
    j .L800D3090
    nop
.L800D3074:
    lw $t0, -0xC($a0)
    lw $t1, -0x8($a0)
    lw $t2, -0x4($a0)
    or $t0, $t0, $s2
    sw $t0, 0x4($a3)
    sw $t1, 0xC($a3)
    sw $t2, 0x14($a3)
.L800D3090:
    addiu $a3, $a3, 0x1C
    addiu $fp, $fp, 0x1
.L800D3098:
    bnez $t8, .L800D2F48
    addi $t8, $t8, -0x1
.L800D30A0:
    lui $at, %hi(.L800D326C)
    ori $at, $at, %lo(.L800D326C)
    lw $t8, 0x0($a0)
    addiu $a0, $a0, 0x4
    beqz $t8, .L800D3274
    addu $s7, $s7, $t8
    addu $s7, $s7, $t8
    addiu $t8, $t8, -0x1
    lh $t4, 0x0($a0)
    lh $t5, 0x2($a0)
    lh $t6, 0x4($a0)
    addu $t4, $s5, $t4
    addu $t5, $s5, $t5
    addu $t6, $s5, $t6
.L800D30D8:
    lwc2 $0, 0x0($t4)
    lwc2 $1, 0x4($t4)
    lwc2 $2, 0x0($t5)
    lwc2 $3, 0x4($t5)
    lwc2 $4, 0x0($t6)
    lwc2 $5, 0x4($t6)
    addiu $a0, $a0, 0x18
    rtpt
    lh $t4, 0x0($a0)
    lh $t5, 0x2($a0)
    lh $t6, 0x4($a0)
    addu $t4, $s5, $t4
    addu $t5, $s5, $t5
    addu $t6, $s5, $t6
    cfc2 $v0, $31
    nclip
    lui $v1, (0x60000 >> 16)
    and $v0, $v0, $v1
    bnez $v0, .L800D326C
    nop
    andi $v0, $s0, 0x20
    bnez $v0, .L800D314C
    nop
    mfc2 $v0, $24
    nop
    beqz $v0, .L800D314C
    xor $v0, $v0, $s6
    bltz $v0, .L800D326C
    nop
.L800D314C:
    lh $t7, -0x12($a0)
    mfc2 $t0, $12
    addu $t7, $s5, $t7
    mfc2 $t1, $13
    lwc2 $0, 0x0($t7)
    lwc2 $1, 0x4($t7)
    mfc2 $t2, $14
    rtps
    mfc2 $t3, $14
    jal func_800D3354
    nop
    sw $t0, 0x8($a3)
    sw $t1, 0x10($a3)
    sw $t2, 0x18($a3)
    sw $t3, 0x20($a3)
    avsz4
    mfc2 $t0, $7
    nop
    srav $t0, $t0, $a2
    sll $t0, $t0, 2
    addu $t0, $t0, $a1
    lw $t1, 0x0($t0)
    lui $v0, (0xFFFFFF >> 16)
    ori $v0, $v0, (0xFFFFFF & 0xFFFF)
    lui $v1, (0x8000000 >> 16)
    and $t1, $t1, $v0
    or $t1, $t1, $v1
    sw $t1, 0x0($a3)
    and $v0, $a3, $v0
    sw $v0, 0x0($t0)
    andi $v1, $s2, 0xFFFF
    beqz $v1, .L800D3240
    nop
    lw $v0, -0x10($a0)
    lwc2 $21, -0xC($a0)
    lwc2 $20, -0x8($a0)
    mtc2 $v0, $22
    mtc2 $v0, $6
    mtc2 $v1, $8
    nop
    nop
    dpct
    nop
    nop
    mfc2 $v0, $22
    swc2 $20, 0x14($a3)
    swc2 $21, 0xC($a3)
    srl $t0, $s2, 16
    sll $t0, $t0, 16
    or $v0, $v0, $t0
    sw $v0, 0x4($a3)
    lwc2 $6, -0x4($a0)
    mtc2 $v1, $8
    nop
    nop
    dpcs
    nop
    nop
    swc2 $22, 0x1C($a3)
    j .L800D3264
    nop
.L800D3240:
    lw $v0, -0x10($a0)
    lw $v1, -0xC($a0)
    lw $t0, -0x8($a0)
    lw $t1, -0x4($a0)
    or $v0, $v0, $s2
    sw $v0, 0x4($a3)
    sw $v1, 0xC($a3)
    sw $t0, 0x14($a3)
    sw $t1, 0x1C($a3)
.L800D3264:
    addiu $a3, $a3, 0x24
    addiu $fp, $fp, 0x2
.L800D326C:
    bnez $t8, .L800D30D8
    addi $t8, $t8, -0x1
.L800D3274:
    lui $v0, %hi(D_800D3544)
    ori $v0, $v0, %lo(D_800D3544)
    sh $s7, 0x0($v0)
    sh $fp, 0x2($v0)
    lw $fp, -0x28($sp)
    lw $s7, -0x24($sp)
    lw $ra, -0x20($sp)
    lw $s6, -0x1C($sp)
    lw $s5, -0x18($sp)
    lw $s4, -0x14($sp)
    lw $s3, -0x10($sp)
    lw $s2, -0xC($sp)
    lw $s1, -0x8($sp)
    lw $s0, -0x4($sp)
    jr $ra
    addu $v0, $a3, $zero
.size BattleDrawModel, . - BattleDrawModel

.globl func_800D32B4
.type func_800D32B4, @function
func_800D32B4:
    sll $v0, $t0, 16
    bltz $v0, .L800D32EC
    lui $v1, (0x1400000 >> 16)
    slt $v0, $v0, $v1
    bnez $v0, .L800D3308
    sll $v0, $t1, 16
    slt $v0, $v0, $v1
    bnez $v0, .L800D3308
    sll $v0, $t2, 16
    slt $v0, $v0, $v1
    bnez $v0, .L800D3308
    nop
    jr $at
    nop
.L800D32EC:
    sll $v0, $t1, 16
    bgez $v0, .L800D3308
    sll $v0, $t2, 16
    bgez $v0, .L800D3308
    nop
    jr $at
    nop
.L800D3308:
    bltz $t0, .L800D3334
    lui $v1, (0xA60000 >> 16)
    slt $v0, $t0, $v1
    bnez $v0, .L800D334C
    slt $v0, $t1, $v1
    bnez $v0, .L800D334C
    slt $v0, $t2, $v1
    bnez $v0, .L800D334C
    nop
    jr $at
    nop
.L800D3334:
    bgez $t1, .L800D334C
    nop
    bgez $t2, .L800D334C
    nop
    jr $at
    nop
.L800D334C:
    jr $ra
    nop
.size func_800D32B4, . - func_800D32B4

.globl func_800D3354
.type func_800D3354, @function
func_800D3354:
    sll $v0, $t0, 16
    bltz $v0, .L800D3398
    lui $v1, (0x1400000 >> 16)
    slt $v0, $v0, $v1
    bnez $v0, .L800D33BC
    sll $v0, $t1, 16
    slt $v0, $v0, $v1
    bnez $v0, .L800D33BC
    sll $v0, $t2, 16
    slt $v0, $v0, $v1
    bnez $v0, .L800D33BC
    sll $v0, $t3, 16
    slt $v0, $v0, $v1
    bnez $v0, .L800D33BC
    nop
    jr $at
    nop
.L800D3398:
    sll $v0, $t1, 16
    bgez $v0, .L800D33BC
    sll $v0, $t2, 16
    bgez $v0, .L800D33BC
    sll $v0, $t3, 16
    bgez $v0, .L800D33BC
    nop
    jr $at
    nop
.L800D33BC:
    bltz $t0, .L800D33F0
    lui $v1, (0xA60000 >> 16)
    slt $v0, $t0, $v1
    bnez $v0, .L800D3410
    slt $v0, $t1, $v1
    bnez $v0, .L800D3410
    slt $v0, $t2, $v1
    bnez $v0, .L800D3410
    slt $v0, $t3, $v1
    bnez $v0, .L800D3410
    nop
    jr $at
    nop
.L800D33F0:
    bgez $t1, .L800D3410
    nop
    bgez $t2, .L800D3410
    nop
    bgez $t3, .L800D3410
    nop
    jr $at
    nop
.L800D3410:
    jr $ra
    nop
.size func_800D3354, . - func_800D3354

.globl BattleModelFlipR11R21R31
.type BattleModelFlipR11R21R31, @function
BattleModelFlipR11R21R31:
    cfc2 $v0, $0
    nop
    andi $v1, $v0, 0xFFFF
    beqz $v1, .L800D3434
    xori $v0, $v0, 0xFFFF
    addiu $v0, $v0, 0x1
    ctc2 $v0, $0
.L800D3434:
    cfc2 $v0, $1
    nop
    lui $v1, (0xFFFF0000 >> 16)
    xor $v0, $v0, $v1
    lui $v1, (0x10000 >> 16)
    addu $v0, $v0, $v1
    ctc2 $v0, $1
    cfc2 $v0, $3
    nop
    andi $v1, $v0, 0xFFFF
    beqz $v1, .L800D346C
    xori $v0, $v0, 0xFFFF
    addiu $v0, $v0, 0x1
    ctc2 $v0, $3
.L800D346C:
    jr $ra
    nop
.size BattleModelFlipR11R21R31, . - BattleModelFlipR11R21R31

.globl BattleModelFlipR12R22R32
.type BattleModelFlipR12R22R32, @function
BattleModelFlipR12R22R32:
    cfc2 $v0, $0
    lui $v1, (0xFFFF0000 >> 16)
    xor $v0, $v0, $v1
    lui $v1, (0x10000 >> 16)
    add $v0, $v0, $v1
    ctc2 $v0, $0
    cfc2 $v0, $2
    nop
    andi $v1, $v0, 0xFFFF
    beqz $v1, .L800D34A8
    xori $v0, $v0, 0xFFFF
    addi $v0, $v0, 0x1
    ctc2 $v0, $2
.L800D34A8:
    cfc2 $v0, $3
    lui $v1, (0xFFFF0000 >> 16)
    xor $v0, $v0, $v1
    lui $v1, (0x10000 >> 16)
    add $v0, $v0, $v1
    ctc2 $v0, $3
    jr $ra
    nop
.size BattleModelFlipR12R22R32, . - BattleModelFlipR12R22R32

.globl BattleModelFlipR13R23R33
.type BattleModelFlipR13R23R33, @function
BattleModelFlipR13R23R33:
    cfc2 $v0, $1
    nop
    andi $v1, $v0, 0xFFFF
    beqz $v1, .L800D34E4
    xori $v0, $v0, 0xFFFF
    addi $v0, $v0, 0x1
    ctc2 $v0, $1
.L800D34E4:
    cfc2 $v0, $2
    lui $v1, (0xFFFF0000 >> 16)
    xor $v0, $v0, $v1
    lui $v1, (0x10000 >> 16)
    add $v0, $v0, $v1
    ctc2 $v0, $2
    cfc2 $v0, $4
    nop
    andi $v1, $v0, 0xFFFF
    beqz $v1, .L800D3518
    xori $v0, $v0, 0xFFFF
    addi $v0, $v0, 0x1
    ctc2 $v0, $4
.L800D3518:
    jr $ra
    nop
.size BattleModelFlipR13R23R33, . - BattleModelFlipR13R23R33

.globl BattleModelUpdateBoneHeight
.type BattleModelUpdateBoneHeight, @function
BattleModelUpdateBoneHeight:
    lw $v0, 0x0($a0)
    addiu $a0, $a0, 0x4
    addu $a0, $a0, $v0
    lh $v0, 0x2($a0)
    nop
    addu $v0, $a1, $v0
    sh $v0, 0x2($a0)
    jr $ra
    nop
.size BattleModelUpdateBoneHeight, . - BattleModelUpdateBoneHeight

.globl D_800D3544
.type D_800D3544, @object
D_800D3544:
    .half 0, 0
.size D_800D3544, . - D_800D3544

.set pop
