# Hand-written model rendering routines (copies of field's FieldModelPrepareRender,
# FieldModelAddToRender and FieldModelAnimCalcMtrxs). They are not compiler output:
# func_800B12B0 has a -0x3C stack frame, func_800B1E98 uses the trapping `neg` and
# builds 0x1F800022 with ori/sll, and func_800B1550 interleaves loop increments with
# GTE hazard slots.
.include "macro.inc"

.text
.set push
.set noreorder
.set noat
.align 2


glabel func_800B12B0
    addiu      $sp, $sp, -0x3C
    sw         $s2, 0x20($sp)
    addu       $s2, $a0, $zero
    sw         $ra, 0x38($sp)
    sw         $s7, 0x34($sp)
    sw         $s6, 0x30($sp)
    sw         $s5, 0x2C($sp)
    sw         $s4, 0x28($sp)
    sw         $s3, 0x24($sp)
    sw         $s1, 0x1C($sp)
    sw         $s0, 0x18($sp)
    lw         $s5, 0x0($s2)
    nop
    andi       $v0, $s5, 0xFF
    beqz       $v0, .L800B1520
    nop
    lhu        $v0, 0x18($s2)
    lw         $v1, 0x1C($s2)
    nop
    addu       $a0, $v0, $v1
    srl        $v0, $s5, 24
    lw         $s1, 0x20($s2)
    beqz       $v0, .L800B1520
    addu      $s3, $zero, $zero
    lui        $s4, (0x1F80001C >> 16)
    addu       $s0, $a0, $zero
.L800B1318:
    lw         $s6, 0x0($s0)
    nop
    andi       $v0, $s6, 0xFF
    beqz       $v0, .L800B150C
    addiu     $v0, $zero, -0x1
    sll        $v1, $s5, 16
    sra        $v1, $v1, 24
    beq        $v1, $v0, .L800B14B4
    nop
    lw         $t5, 0x0($s1)
    lw         $t6, 0x4($s1)
    ctc2       $t5, $0
    ctc2       $t6, $1
    lw         $t5, 0x8($s1)
    lw         $t6, 0xC($s1)
    lw         $t7, 0x10($s1)
    ctc2       $t5, $2
    ctc2       $t6, $3
    ctc2       $t7, $4
    lw         $t5, 0x14($s1)
    lw         $t6, 0x18($s1)
    ctc2       $t5, $5
    lw         $t7, 0x1C($s1)
    ctc2       $t6, $6
    ctc2       $t7, $7
    sll        $v0, $s6, 16
    srl        $v0, $v0, 19
    addu       $t4, $s1, $v0
    lhu        $t5, 0x0($t4)
    lhu        $t6, 0x6($t4)
    lhu        $t7, 0xC($t4)
    mtc2       $t5, $9
    mtc2       $t6, $10
    mtc2       $t7, $11
    nop
    nop
    mvmva      1, 0, 3, 3, 0
    mfc2       $v0, $9
    mfc2       $a1, $10
    mfc2       $s7, $11
    lhu        $t5, 0x2($t4)
    lhu        $t6, 0x8($t4)
    lhu        $t7, 0xE($t4)
    mtc2       $t5, $9
    mtc2       $t6, $10
    mtc2       $t7, $11
    nop
    nop
    mvmva      1, 0, 3, 3, 0
    sh         $v0, (0x1F800000 & 0xFFFF)($s4)
    sh         $a1, (0x1F800006 & 0xFFFF)($s4)
    sh         $s7, (0x1F80000C & 0xFFFF)($s4)
    mfc2       $v0, $9
    mfc2       $a1, $10
    mfc2       $s7, $11
    lhu        $t5, 0x4($t4)
    lhu        $t6, 0xA($t4)
    lhu        $t7, 0x10($t4)
    mtc2       $t5, $9
    mtc2       $t6, $10
    mtc2       $t7, $11
    nop
    nop
    mvmva      1, 0, 3, 3, 0
    sh         $v0, (0x1F800002 & 0xFFFF)($s4)
    sh         $a1, (0x1F800008 & 0xFFFF)($s4)
    sh         $s7, (0x1F80000E & 0xFFFF)($s4)
    mfc2       $v0, $9
    mfc2       $a1, $10
    mfc2       $s7, $11
    lhu        $t6, 0x18($t4)
    lhu        $t5, 0x14($t4)
    sll        $t6, $t6, 16
    or         $t5, $t5, $t6
    mtc2       $t5, $0
    lwc2       $1, 0x1C($t4)
    nop
    nop
    mvmva      1, 0, 0, 0, 0
    sh         $v0, (0x1F800004 & 0xFFFF)($s4)
    sh         $a1, (0x1F80000A & 0xFFFF)($s4)
    sh         $s7, (0x1F800010 & 0xFFFF)($s4)
    swc2       $9, (0x1F800014 & 0xFFFF)($s4)
    swc2       $10, (0x1F800018 & 0xFFFF)($s4)
    swc2       $11, (0x1F80001C & 0xFFFF)($s4)
    lw         $t5, (0x1F800000 & 0xFFFF)($s4)
    lw         $t6, (0x1F800004 & 0xFFFF)($s4)
    ctc2       $t5, $0
    ctc2       $t6, $1
    lw         $t5, (0x1F800008 & 0xFFFF)($s4)
    lw         $t6, (0x1F80000C & 0xFFFF)($s4)
    lw         $t7, (0x1F800010 & 0xFFFF)($s4)
    ctc2       $t5, $2
    ctc2       $t6, $3
    ctc2       $t7, $4
    lw         $t5, (0x1F800014 & 0xFFFF)($s4)
    lw         $t6, (0x1F800018 & 0xFFFF)($s4)
    ctc2       $t5, $5
    lw         $t7, (0x1F80001C & 0xFFFF)($s4)
    ctc2       $t6, $6
    ctc2       $t7, $7
    j          .L800B1504
    nop
.L800B14B4:
    andi       $v0, $s6, 0xFF00
    sll        $v0, $v0, 16
    srl        $v0, $v0, 19
    addu       $v0, $s1, $v0
    lw         $t5, 0x0($v0)
    lw         $t6, 0x4($v0)
    ctc2       $t5, $0
    ctc2       $t6, $1
    lw         $t5, 0x8($v0)
    lw         $t6, 0xC($v0)
    lw         $t7, 0x10($v0)
    ctc2       $t5, $2
    ctc2       $t6, $3
    ctc2       $t7, $4
    lw         $t5, 0x14($v0)
    lw         $t6, 0x18($v0)
    ctc2       $t5, $5
    lw         $t7, 0x1C($v0)
    ctc2       $t6, $6
    ctc2       $t7, $7
.L800B1504:
    jal        func_800B1550
    addu      $a0, $s0, $zero
.L800B150C:
    srl        $v0, $s5, 24
    addiu      $s3, $s3, 0x1
    sltu       $v0, $s3, $v0
    bnez       $v0, .L800B1318
    addiu     $s0, $s0, 0x20
.L800B1520:
    lw         $ra, 0x38($sp)
    lw         $s7, 0x34($sp)
    lw         $s6, 0x30($sp)
    lw         $s5, 0x2C($sp)
    lw         $s4, 0x28($sp)
    lw         $s3, 0x24($sp)
    lw         $s2, 0x20($sp)
    lw         $s1, 0x1C($sp)
    lw         $s0, 0x18($sp)
    addiu      $sp, $sp, 0x3C
    jr         $ra
    nop
.size func_800B12B0, . - func_800B12B0


glabel func_800B1550
    addiu      $sp, $sp, -0x70
    sw         $s7, 0x64($sp)
    addu       $s7, $a0, $zero
    lui        $v1, (0xAAAAAAAB >> 16)
    sw         $fp, 0x68($sp)
    sw         $s6, 0x60($sp)
    sw         $s5, 0x5C($sp)
    sw         $s4, 0x58($sp)
    sw         $s3, 0x54($sp)
    sw         $s2, 0x50($sp)
    sw         $s1, 0x4C($sp)
    sw         $s0, 0x48($sp)
    lbu        $a2, 0x2($s7)
    ori        $v1, $v1, (0xAAAAAAAB & 0xFFFF)
    multu      $a2, $v1
    lui        $s0, (0x1F800008 >> 16)
    ori        $s0, $s0, (0x1F800008 & 0xFFFF)
    addu       $a3, $zero, $zero
    lw         $v0, 0x18($s7)
    lui        $s2, %hi(D_800B43C0)
    lw         $s2, %lo(D_800B43C0)($s2)
    addiu      $t0, $v0, 0x4
    mfhi       $v0
    lui        $a1, (0x1F800008 >> 16)
    ori        $a1, $a1, (0x1F800008 & 0xFFFF)
    srl        $v0, $v0, 1
    andi       $s1, $v0, 0xFF
    beqz       $s1, .L800B1614
    nop
.L800B15C4:
    lwc2       $0, 0x0($t0)
    lwc2       $1, 0x4($t0)
    lwc2       $2, 0x8($t0)
    lwc2       $3, 0xC($t0)
    lwc2       $4, 0x10($t0)
    lwc2       $5, 0x14($t0)
    nop
    nop
    rtpt
    addiu      $t0, $t0, 0x18
    addiu      $a3, $a3, 0x1
    swc2       $12, 0x0($a1)
    swc2       $13, 0x8($a1)
    swc2       $14, 0x10($a1)
    swc2       $17, 0x4($a1)
    swc2       $18, 0xC($a1)
    swc2       $19, 0x14($a1)
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B15C4
    addiu     $a1, $a1, 0x18
.L800B1614:
    sll        $v1, $s1, 1
    addu       $a3, $v1, $s1
    sltu       $v0, $a3, $a2
    beqz       $v0, .L800B1658
    sll       $v0, $a3, 3
.L800B1628:
    lwc2       $0, 0x0($t0)
    lwc2       $1, 0x4($t0)
    nop
    nop
    rtps
    addiu      $t0, $t0, 0x8
    addiu      $a3, $a3, 0x1
    swc2       $14, 0x0($a1)
    swc2       $19, 0x4($a1)
    sltu       $v0, $a3, $a2
    bnez       $v0, .L800B1628
    addiu     $a1, $a1, 0x8
.L800B1658:
    lui        $v0, %hi(D_800B43BC)
    lbu        $v0, %lo(D_800B43BC)($v0)
    lw         $a2, 0x1C($s7)
    beqz       $v0, .L800B1678
    addu      $a3, $zero, $zero
    lhu        $v0, 0x16($s7)
    nop
    addu       $a2, $a2, $v0
.L800B1678:
    lw         $fp, 0x4($s7)
    lui        $s6, (0xFF000000 >> 16)
    lui        $s3, (0xFFFFFF >> 16)
    ori        $s3, $s3, (0xFFFFFF & 0xFFFF)
    andi       $s1, $fp, 0xFF
    beqz       $s1, .L800B1790
    andi      $v0, $fp, 0xFF00
    addiu      $t8, $a2, 0x2C
.L800B1698:
    lw         $v1, 0x0($t0)
    nop
    andi       $v0, $v1, 0xFF
    sll        $v0, $v0, 3
    addu       $t9, $s0, $v0
    andi       $v0, $v1, 0xFF00
    srl        $v0, $v0, 5
    addu       $t3, $s0, $v0
    srl        $v0, $v1, 13
    andi       $v0, $v0, 0x7F8
    addu       $t2, $s0, $v0
    srl        $v1, $v1, 24
    sll        $v1, $v1, 3
    addu       $t1, $s0, $v1
    lw         $v0, 0x0($t9)
    lw         $v1, 0x0($t3)
    lw         $a0, 0x0($t2)
    mtc2       $v0, $12
    mtc2       $a0, $14
    mtc2       $v1, $13
    addiu      $a3, $a3, 0x1
    addiu      $t0, $t0, 0x18
    nclip
    lw         $s4, 0x0($a2)
    sw         $v0, -0x24($t8)
    sw         $v1, -0x18($t8)
    sw         $a0, -0xC($t8)
    and        $t6, $s4, $s6
    addiu      $t8, $t8, 0x34
    mfc2       $v0, $24
    nop
    bgtz       $v0, .L800B1724
    lw        $a1, 0x0($t1)
    j          .L800B1780
    sw        $t6, 0x0($a2)
.L800B1724:
    lw         $v1, 0x4($t9)
    sw         $a1, -0x34($t8)
    lw         $s5, 0x4($t3)
    lw         $v0, 0x4($t2)
    lw         $a0, 0x4($t1)
    addu       $v1, $v1, $s5
    addu       $v1, $v1, $v0
    addu       $v0, $v1, $a0
    bgez       $v0, .L800B1750
    nop
    addiu      $v0, $v0, 0xF
.L800B1750:
    sra        $v0, $v0, 4
    sll        $v0, $v0, 2
    addu       $t4, $v0, $s2
    lw         $t5, 0x0($t4)
    nop
    and        $v0, $t5, $s3
    or         $v1, $t6, $v0
    sw         $v1, 0x0($a2)
    and        $a0, $a2, $s3
    and        $v1, $t5, $s6
    or         $v1, $v1, $a0
    sw         $v1, 0x0($t4)
.L800B1780:
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B1698
    addiu     $a2, $a2, 0x34
    andi       $v0, $fp, 0xFF00
.L800B1790:
    srl        $s1, $v0, 8
    beqz       $s1, .L800B1884
    addu      $a3, $zero, $zero
    addiu      $t8, $a2, 0x20
    lw         $t6, 0x0($t0)
.L800B17A4:
    nop
    andi       $v0, $t6, 0xFF
    sll        $v0, $v0, 3
    addu       $t3, $s0, $v0
    andi       $v0, $t6, 0xFF00
    srl        $v0, $v0, 5
    addu       $t2, $s0, $v0
    srl        $v1, $t6, 13
    andi       $v1, $v1, 0x7F8
    addu       $a1, $s0, $v1
    lw         $v0, 0x0($t3)
    lw         $v1, 0x0($t2)
    lw         $a0, 0x0($a1)
    mtc2       $v0, $12
    mtc2       $a0, $14
    mtc2       $v1, $13
    addiu      $a3, $a3, 0x1
    addiu      $t0, $t0, 0x14
    nclip
    lw         $s4, 0x0($a2)
    sw         $v0, -0x18($t8)
    sw         $v1, -0xC($t8)
    sw         $a0, 0x0($t8)
    addiu      $t8, $t8, 0x28
    mfc2       $v0, $24
    nop
    bgtz       $v0, .L800B1824
    nop
    and        $v0, $s4, $s6
    sw         $v0, 0x0($a2)
    j          .L800B1878
    lw        $t6, 0x0($t0)
.L800B1824:
    lwc2       $17, 0x4($t3)
    lwc2       $18, 0x4($t2)
    lwc2       $19, 0x4($a1)
    nop
    nop
    avsz3
    lw         $t6, 0x0($t0)
    and        $v1, $s4, $s6
    and        $s5, $a2, $s3
    mfc2       $v0, $7
    nop
    sll        $v0, $v0, 2
    addu       $t4, $v0, $s2
    lw         $t5, 0x0($t4)
    nop
    and        $v0, $t5, $s3
    or         $v1, $v1, $v0
    sw         $v1, 0x0($a2)
    and        $v0, $t5, $s6
    or         $v0, $v0, $s5
    sw         $v0, 0x0($t4)
.L800B1878:
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B17A4
    addiu     $a2, $a2, 0x28
.L800B1884:
    srl        $v0, $fp, 16
    andi       $s1, $v0, 0xFF
    beqz       $s1, .L800B198C
    addu      $a3, $zero, $zero
    addiu      $t8, $a2, 0x20
.L800B1898:
    lw         $v1, 0x0($t0)
    nop
    andi       $v0, $v1, 0xFF
    sll        $v0, $v0, 3
    addu       $t9, $s0, $v0
    andi       $v0, $v1, 0xFF00
    srl        $v0, $v0, 5
    addu       $t3, $s0, $v0
    srl        $v0, $v1, 13
    andi       $v0, $v0, 0x7F8
    addu       $t2, $s0, $v0
    srl        $v1, $v1, 24
    sll        $v1, $v1, 3
    addu       $t1, $s0, $v1
    lw         $v0, 0x0($t9)
    lw         $v1, 0x0($t3)
    lw         $a0, 0x0($t2)
    mtc2       $v0, $12
    mtc2       $a0, $14
    mtc2       $v1, $13
    addiu      $a3, $a3, 0x1
    addiu      $t0, $t0, 0xC
    nclip
    lw         $s4, 0x0($a2)
    sw         $v0, -0x18($t8)
    sw         $v1, -0x10($t8)
    sw         $a0, -0x8($t8)
    and        $t6, $s4, $s6
    addiu      $t8, $t8, 0x28
    mfc2       $v0, $24
    nop
    bgtz       $v0, .L800B1924
    lw        $a1, 0x0($t1)
    j          .L800B1980
    sw        $t6, 0x0($a2)
.L800B1924:
    lw         $v1, 0x4($t9)
    sw         $a1, -0x28($t8)
    lw         $s5, 0x4($t3)
    lw         $v0, 0x4($t2)
    lw         $a0, 0x4($t1)
    addu       $v1, $v1, $s5
    addu       $v1, $v1, $v0
    addu       $v0, $v1, $a0
    bgez       $v0, .L800B1950
    nop
    addiu      $v0, $v0, 0xF
.L800B1950:
    sra        $v0, $v0, 4
    sll        $v0, $v0, 2
    addu       $t4, $v0, $s2
    lw         $t5, 0x0($t4)
    nop
    and        $v0, $t5, $s3
    or         $v1, $t6, $v0
    sw         $v1, 0x0($a2)
    and        $a0, $a2, $s3
    and        $v1, $t5, $s6
    or         $v1, $v1, $a0
    sw         $v1, 0x0($t4)
.L800B1980:
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B1898
    addiu     $a2, $a2, 0x28
.L800B198C:
    srl        $s1, $fp, 24
    beqz       $s1, .L800B1A78
    addu      $a3, $zero, $zero
    addiu      $t8, $a2, 0x18
    lw         $t6, 0x0($t0)
.L800B19A0:
    nop
    andi       $v0, $t6, 0xFF
    sll        $v0, $v0, 3
    addu       $t3, $s0, $v0
    andi       $v0, $t6, 0xFF00
    srl        $v0, $v0, 5
    addu       $t2, $s0, $v0
    srl        $v1, $t6, 13
    andi       $v1, $v1, 0x7F8
    addu       $a1, $s0, $v1
    lw         $v0, 0x0($t3)
    lw         $v1, 0x0($t2)
    lw         $a0, 0x0($a1)
    mtc2       $v0, $12
    mtc2       $a0, $14
    mtc2       $v1, $13
    addiu      $a3, $a3, 0x1
    addiu      $t0, $t0, 0xC
    nclip
    lw         $s4, 0x0($a2)
    sw         $v0, -0x10($t8)
    sw         $v1, -0x8($t8)
    sw         $a0, 0x0($t8)
    addiu      $t8, $t8, 0x20
    mfc2       $v0, $24
    nop
    bgtz       $v0, .L800B1A20
    nop
    and        $v0, $s4, $s6
    sw         $v0, 0x0($a2)
    j          .L800B1A6C
    lw        $t6, 0x0($t0)
.L800B1A20:
    lwc2       $17, 0x4($t3)
    lwc2       $18, 0x4($t2)
    lwc2       $19, 0x4($a1)
    and        $v1, $s4, $s6
    and        $s5, $a2, $s3
    avsz3
    lw         $t6, 0x0($t0)
    mfc2       $v0, $7
    nop
    sll        $v0, $v0, 2
    addu       $t4, $v0, $s2
    lw         $t5, 0x0($t4)
    nop
    and        $v0, $t5, $s3
    or         $v1, $v1, $v0
    sw         $v1, 0x0($a2)
    and        $v0, $t5, $s6
    or         $v0, $v0, $s5
    sw         $v0, 0x0($t4)
.L800B1A6C:
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B19A0
    addiu     $a2, $a2, 0x20
.L800B1A78:
    lw         $fp, 0x8($s7)
    nop
    andi       $s1, $fp, 0xFF
    beqz       $s1, .L800B1B6C
    addu      $a3, $zero, $zero
    addiu      $t8, $a2, 0x10
    lw         $t6, 0x0($t0)
.L800B1A94:
    nop
    andi       $v0, $t6, 0xFF
    sll        $v0, $v0, 3
    addu       $t3, $s0, $v0
    andi       $v0, $t6, 0xFF00
    srl        $v0, $v0, 5
    addu       $t2, $s0, $v0
    srl        $v1, $t6, 13
    andi       $v1, $v1, 0x7F8
    addu       $a1, $s0, $v1
    lw         $v0, 0x0($t3)
    lw         $v1, 0x0($t2)
    lw         $a0, 0x0($a1)
    mtc2       $v0, $12
    mtc2       $a0, $14
    mtc2       $v1, $13
    addiu      $a3, $a3, 0x1
    addiu      $t0, $t0, 0x8
    nclip
    lw         $s4, 0x0($a2)
    sw         $v0, -0x8($t8)
    sw         $v1, -0x4($t8)
    sw         $a0, 0x0($t8)
    addiu      $t8, $t8, 0x14
    mfc2       $v0, $24
    nop
    bgtz       $v0, .L800B1B14
    nop
    and        $v0, $s4, $s6
    sw         $v0, 0x0($a2)
    j          .L800B1B60
    lw        $t6, 0x0($t0)
.L800B1B14:
    lwc2       $17, 0x4($t3)
    lwc2       $18, 0x4($t2)
    lwc2       $19, 0x4($a1)
    and        $v1, $s4, $s6
    and        $s5, $a2, $s3
    avsz3
    lw         $t6, 0x0($t0)
    mfc2       $v0, $7
    nop
    sll        $v0, $v0, 2
    addu       $t4, $v0, $s2
    lw         $t5, 0x0($t4)
    nop
    and        $v0, $t5, $s3
    or         $v1, $v1, $v0
    sw         $v1, 0x0($a2)
    and        $v0, $t5, $s6
    or         $v0, $v0, $s5
    sw         $v0, 0x0($t4)
.L800B1B60:
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B1A94
    addiu     $a2, $a2, 0x14
.L800B1B6C:
    andi       $v0, $fp, 0xFF00
    srl        $s1, $v0, 8
    beqz       $s1, .L800B1C74
    addu      $a3, $zero, $zero
    addiu      $t8, $a2, 0x14
.L800B1B80:
    lw         $v1, 0x0($t0)
    nop
    andi       $v0, $v1, 0xFF
    sll        $v0, $v0, 3
    addu       $t9, $s0, $v0
    andi       $v0, $v1, 0xFF00
    srl        $v0, $v0, 5
    addu       $t3, $s0, $v0
    srl        $v0, $v1, 13
    andi       $v0, $v0, 0x7F8
    addu       $t2, $s0, $v0
    srl        $v1, $v1, 24
    sll        $v1, $v1, 3
    addu       $t1, $s0, $v1
    lw         $v0, 0x0($t9)
    lw         $v1, 0x0($t3)
    lw         $a0, 0x0($t2)
    mtc2       $v0, $12
    mtc2       $a0, $14
    mtc2       $v1, $13
    addiu      $t0, $t0, 0x8
    addiu      $a3, $a3, 0x1
    nclip
    lw         $s4, 0x0($a2)
    sw         $v0, -0xC($t8)
    sw         $v1, -0x8($t8)
    sw         $a0, -0x4($t8)
    and        $t6, $s4, $s6
    addiu      $t8, $t8, 0x18
    mfc2       $v0, $24
    nop
    bgtz       $v0, .L800B1C0C
    lw        $a1, 0x0($t1)
    j          .L800B1C68
    sw        $t6, 0x0($a2)
.L800B1C0C:
    lw         $v1, 0x4($t9)
    sw         $a1, -0x18($t8)
    lw         $s5, 0x4($t3)
    lw         $v0, 0x4($t2)
    lw         $a0, 0x4($t1)
    addu       $v1, $v1, $s5
    addu       $v1, $v1, $v0
    addu       $v0, $v1, $a0
    bgez       $v0, .L800B1C38
    nop
    addiu      $v0, $v0, 0xF
.L800B1C38:
    sra        $v0, $v0, 4
    sll        $v0, $v0, 2
    addu       $t4, $v0, $s2
    lw         $t5, 0x0($t4)
    nop
    and        $v0, $t5, $s3
    or         $v1, $t6, $v0
    sw         $v1, 0x0($a2)
    and        $a0, $a2, $s3
    and        $v1, $t5, $s6
    or         $v1, $v1, $a0
    sw         $v1, 0x0($t4)
.L800B1C68:
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B1B80
    addiu     $a2, $a2, 0x18
.L800B1C74:
    srl        $v0, $fp, 16
    andi       $s1, $v0, 0xFF
    beqz       $s1, .L800B1D64
    addu      $a3, $zero, $zero
    addiu      $t8, $a2, 0x18
    lw         $t6, 0x0($t0)
.L800B1C8C:
    nop
    andi       $v0, $t6, 0xFF
    sll        $v0, $v0, 3
    addu       $t3, $s0, $v0
    andi       $v0, $t6, 0xFF00
    srl        $v0, $v0, 5
    addu       $t2, $s0, $v0
    srl        $v1, $t6, 13
    andi       $v1, $v1, 0x7F8
    addu       $a1, $s0, $v1
    lw         $v0, 0x0($t3)
    lw         $v1, 0x0($t2)
    lw         $a0, 0x0($a1)
    mtc2       $v0, $12
    mtc2       $a0, $14
    mtc2       $v1, $13
    addiu      $a3, $a3, 0x1
    addiu      $t0, $t0, 0x10
    nclip
    lw         $s4, 0x0($a2)
    sw         $v0, -0x10($t8)
    sw         $v1, -0x8($t8)
    sw         $a0, 0x0($t8)
    addiu      $t8, $t8, 0x1C
    mfc2       $v0, $24
    nop
    bgtz       $v0, .L800B1D0C
    nop
    and        $v0, $s4, $s6
    sw         $v0, 0x0($a2)
    j          .L800B1D58
    lw        $t6, 0x0($t0)
.L800B1D0C:
    lwc2       $17, 0x4($t3)
    lwc2       $18, 0x4($t2)
    lwc2       $19, 0x4($a1)
    and        $v1, $s4, $s6
    and        $s5, $a2, $s3
    avsz3
    lw         $t6, 0x0($t0)
    mfc2       $v0, $7
    nop
    sll        $v0, $v0, 2
    addu       $t4, $v0, $s2
    lw         $a0, 0x0($t4)
    nop
    and        $v0, $a0, $s3
    or         $v1, $v1, $v0
    sw         $v1, 0x0($a2)
    and        $v0, $a0, $s6
    or         $v0, $v0, $s5
    sw         $v0, 0x0($t4)
.L800B1D58:
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B1C8C
    addiu     $a2, $a2, 0x1C
.L800B1D64:
    srl        $s1, $fp, 24
    beqz       $s1, .L800B1E68
    addu      $a3, $zero, $zero
    addiu      $t8, $a2, 0x20
.L800B1D74:
    lw         $v1, 0x0($t0)
    nop
    andi       $v0, $v1, 0xFF
    sll        $v0, $v0, 3
    addu       $t9, $s0, $v0
    andi       $v0, $v1, 0xFF00
    srl        $v0, $v0, 5
    addu       $t3, $s0, $v0
    srl        $v0, $v1, 13
    andi       $v0, $v0, 0x7F8
    addu       $t2, $s0, $v0
    srl        $v1, $v1, 24
    sll        $v1, $v1, 3
    addu       $t1, $s0, $v1
    lw         $v0, 0x0($t9)
    lw         $v1, 0x0($t3)
    lw         $a0, 0x0($t2)
    mtc2       $v0, $12
    mtc2       $a0, $14
    mtc2       $v1, $13
    addiu      $a3, $a3, 0x1
    addiu      $t0, $t0, 0x14
    nclip
    lw         $s4, 0x0($a2)
    sw         $v0, -0x18($t8)
    sw         $v1, -0x10($t8)
    sw         $a0, -0x8($t8)
    and        $t6, $s4, $s6
    addiu      $t8, $t8, 0x24
    mfc2       $v0, $24
    nop
    bgtz       $v0, .L800B1E00
    lw        $a1, 0x0($t1)
    j          .L800B1E5C
    sw        $t6, 0x0($a2)
.L800B1E00:
    lw         $v1, 0x4($t9)
    sw         $a1, -0x24($t8)
    lw         $s5, 0x4($t3)
    lw         $v0, 0x4($t2)
    lw         $a0, 0x4($t1)
    addu       $v1, $v1, $s5
    addu       $v1, $v1, $v0
    addu       $v0, $v1, $a0
    bgez       $v0, .L800B1E2C
    nop
    addiu      $v0, $v0, 0xF
.L800B1E2C:
    sra        $v0, $v0, 4
    sll        $v0, $v0, 2
    addu       $t4, $v0, $s2
    lw         $t5, 0x0($t4)
    nop
    and        $v0, $t5, $s3
    or         $v1, $t6, $v0
    sw         $v1, 0x0($a2)
    and        $a0, $a2, $s3
    and        $v1, $t5, $s6
    or         $v1, $v1, $a0
    sw         $v1, 0x0($t4)
.L800B1E5C:
    sltu       $v0, $a3, $s1
    bnez       $v0, .L800B1D74
    addiu     $a2, $a2, 0x24
.L800B1E68:
    lw         $fp, 0x68($sp)
    lw         $s7, 0x64($sp)
    lw         $s6, 0x60($sp)
    lw         $s5, 0x5C($sp)
    lw         $s4, 0x58($sp)
    lw         $s3, 0x54($sp)
    lw         $s2, 0x50($sp)
    lw         $s1, 0x4C($sp)
    lw         $s0, 0x48($sp)
    addiu      $sp, $sp, 0x70
    jr         $ra
    nop
.size func_800B1550, . - func_800B1550


glabel func_800B1E98
    addiu      $sp, $sp, -0x70
    sw         $s7, 0x64($sp)
    addu       $s7, $a0, $zero
    addu       $t3, $a2, $zero
    addu       $t0, $a3, $zero
    sw         $fp, 0x68($sp)
    sw         $s6, 0x60($sp)
    sw         $s5, 0x5C($sp)
    sw         $s4, 0x58($sp)
    sw         $s3, 0x54($sp)
    sw         $s2, 0x50($sp)
    sw         $s1, 0x4C($sp)
    sw         $s0, 0x48($sp)
    lbu        $v0, 0x0($s7)
    nop
    beqz       $v0, .L800B2730
    nop
    addu       $t4, $a1, $zero
    lw         $t5, 0x0($t4)
    lw         $t6, 0x4($t4)
    ctc2       $t5, $0
    ctc2       $t6, $1
    lw         $t5, 0x8($t4)
    lw         $t6, 0xC($t4)
    lw         $t7, 0x10($t4)
    ctc2       $t5, $2
    ctc2       $t6, $3
    ctc2       $t7, $4
    addu       $t4, $a1, $zero
    lw         $t5, 0x14($t4)
    lw         $t6, 0x18($t4)
    ctc2       $t5, $5
    lw         $t7, 0x1C($t4)
    ctc2       $t6, $6
    ctc2       $t7, $7
    lui        $fp, (0x1F800020 >> 16)
    lw         $v0, (0x1F800000 & 0xFFFF)($fp)
    lui        $t9, %hi(D_800B43C8)
    addiu      $t9, $t9, %lo(D_800B43C8)
    andi       $v0, $v0, 0x2
    beqz       $v0, .L800B2190
    ori       $fp, $fp, (0x1F800020 & 0xFFFF)
    lhu        $t6, 0xC($s7)
    lhu        $t5, 0x8($s7)
    sll        $t6, $t6, 16
    or         $t5, $t5, $t6
    mtc2       $t5, $0
    lwc2       $1, 0x10($s7)
    nop
    nop
    mvmva      1, 0, 0, 0, 0
    lw         $t4, 0x4($s7)
    nop
    sll        $v0, $t4, 16
    srl        $v0, $v0, 24
    swc2       $9, 0x14($fp)
    swc2       $10, 0x18($fp)
    swc2       $11, 0x1C($fp)
    sll        $v0, $v0, 2
    sll        $v1, $t4, 8
    srl        $v1, $v1, 24
    addu       $v0, $v0, $t9
    sll        $v1, $v1, 2
    addu       $v1, $v1, $t9
    lw         $t1, 0x0($v0)
    srl        $v0, $t4, 24
    lw         $a2, 0x0($v1)
    sll        $v0, $v0, 2
    addu       $v0, $v0, $t9
    lw         $a1, 0x0($v0)
    srl        $v0, $t1, 16
    addu       $s0, $v0, $zero
    addu       $t2, $t1, $zero
    srl        $a3, $a2, 16
    addu       $v1, $a2, $zero
    sra        $v0, $t1, 16
    negu       $v0, $v0
    srl        $s2, $a1, 16
    addu       $s1, $a1, $zero
    mtc2       $v0, $8
    andi       $v1, $v1, 0xFFFF
    mtc2       $v1, $9
    andi       $a3, $a3, 0xFFFF
    mtc2       $a3, $10
    ori        $s4, $zero, 0x1F80
    sll        $s4, $s4, 16
    gpf        1
    mfc2       $t5, $25
    nop
    andi       $t5, $t5, 0xFFFF
    mtc2       $t5, $9
    andi       $t6, $t2, 0xFFFF
    mtc2       $t6, $10
    mfc2       $t7, $26
    nop
    andi       $t7, $t7, 0xFFFF
    mtc2       $t7, $11
    ori        $s4, $s4, 0x22
    nop
    mvmva      1, 0, 3, 3, 0
    mfc2       $t5, $9
    mfc2       $t6, $10
    mfc2       $t7, $11
    mtc2       $t2, $8
    mtc2       $v1, $9
    mtc2       $a3, $10
    sh         $t5, 0x0($s4)
    sh         $t6, 0x6($s4)
    gpf        1
    sh         $t7, 0xC($s4)
    mfc2       $t1, $25
    nop
    andi       $t1, $t1, 0xFFFF
    mfc2       $t2, $26
    nop
    andi       $t2, $t2, 0xFFFF
    mtc2       $s1, $8
    mtc2       $t1, $9
    mtc2       $s0, $10
    mtc2       $t2, $11
    negu       $a0, $a2
    andi       $t7, $a0, 0xFFFF
    gpf        0
    mtc2       $s2, $8
    mtc2       $a3, $9
    mtc2       $zero, $10
    mtc2       $t7, $11
    nop
    nop
    gpl        0
    mfc2       $t5, $25
    mfc2       $t6, $26
    mfc2       $t7, $27
    sra        $t5, $t5, 12
    andi       $t5, $t5, 0xFFFF
    mtc2       $t5, $9
    sra        $t6, $t6, 12
    andi       $t6, $t6, 0xFFFF
    mtc2       $t6, $10
    sra        $t7, $t7, 12
    andi       $t7, $t7, 0xFFFF
    mtc2       $t7, $11
    nop
    nop
    mvmva      1, 0, 3, 3, 0
    mfc2       $t5, $9
    mfc2       $t6, $10
    mfc2       $t7, $11
    mtc2       $s2, $8
    mtc2       $t1, $9
    mtc2       $s0, $10
    mtc2       $t2, $11
    sh         $t5, 0x0($fp)
    sh         $t6, 0x6($fp)
    gpf        0
    sh         $t7, 0xC($fp)
    sll        $v0, $a1, 16
    sra        $v0, $v0, 16
    negu       $v0, $v0
    mtc2       $v0, $8
    mtc2       $a3, $9
    mtc2       $zero, $10
    andi       $t7, $a0, 0xFFFF
    mtc2       $t7, $11
    nop
    nop
    gpl        0
    mfc2       $t5, $25
    mfc2       $t6, $26
    mfc2       $t7, $27
    sra        $t5, $t5, 12
    andi       $t5, $t5, 0xFFFF
    mtc2       $t5, $9
    sra        $t6, $t6, 12
    andi       $t6, $t6, 0xFFFF
    mtc2       $t6, $10
    sra        $t7, $t7, 12
    andi       $t7, $t7, 0xFFFF
    mtc2       $t7, $11
    nop
    nop
    mvmva      1, 0, 3, 3, 0
    mfc2       $t5, $9
    mfc2       $t6, $10
    mfc2       $t7, $11
    sh         $t5, 0x4($fp)
    sh         $t6, 0xA($fp)
    sh         $t7, 0x10($fp)
    j          .L800B21D0
    nop
.L800B2190:
    lw         $v0, 0x0($a1)
    lw         $a0, 0x4($a1)
    sw         $v0, 0x0($fp)
    sw         $a0, 0x4($fp)
    lw         $v0, 0x8($a1)
    lw         $a0, 0xC($a1)
    sw         $v0, 0x8($fp)
    sw         $a0, 0xC($fp)
    lw         $v0, 0x10($a1)
    lw         $a0, 0x14($a1)
    sw         $v0, 0x10($fp)
    sw         $a0, 0x14($fp)
    lw         $v0, 0x18($a1)
    lw         $a0, 0x1C($a1)
    sw         $v0, 0x18($fp)
    sw         $a0, 0x1C($fp)
.L800B21D0:
    lhu        $a0, 0x1A($s7)
    lw         $v0, 0x1C($s7)
    addu       $s3, $zero, $zero
    addu       $a0, $a0, $v0
    sll        $v0, $t3, 4
    addu       $a0, $a0, $v0
    lw         $a1, 0xC($a0)
    lhu        $v0, 0x6($a0)
    sll        $v1, $t0, 1
    addu       $v0, $v0, $a1
    addu       $v0, $v0, $v1
    sw         $v0, -0x14($fp)
    lw         $v1, 0x8($a0)
    nop
    srl        $v0, $v1, 16
    andi       $v1, $v1, 0xFFFF
    addu       $v1, $v1, $a1
    addu       $v0, $v0, $a1
    addu       $t8, $v0, $t0
    sw         $v1, -0x10($fp)
    lbu        $s4, 0x2($s7)
    lhu        $s6, 0x0($a0)
    beqz       $s4, .L800B2730
    addiu     $a2, $a1, 0x4
    addu       $s5, $a2, $zero
.L800B2234:
    sll        $v0, $s3, 2
    lw         $v1, 0x1C($s7)
    sll        $t1, $s3, 5
    addu       $a2, $v0, $v1
    addiu      $v1, $t1, 0x20
    lw         $t4, 0x0($a2)
    addu       $t0, $fp, $v1
    sll        $v0, $t4, 8
    sra        $v0, $v0, 24
    sra        $v1, $t4, 24
    sll        $v0, $v0, 5
    addiu      $v0, $v0, 0x20
    beqz       $v1, .L800B2278
    addu      $t2, $fp, $v0
    lw         $v0, 0x20($s7)
    j          .L800B227C
    addu      $t3, $t1, $v0
.L800B2278:
    addu       $t3, $zero, $zero
.L800B227C:
    sll        $v0, $t4, 16
    sra        $v0, $v0, 16
    sw         $v0, 0x1C($t0)
    lw         $t5, 0x0($t2)
    lw         $t6, 0x4($t2)
    ctc2       $t5, $0
    ctc2       $t6, $1
    lw         $t5, 0x8($t2)
    lw         $t6, 0xC($t2)
    lw         $t7, 0x10($t2)
    ctc2       $t5, $2
    ctc2       $t6, $3
    ctc2       $t7, $4
    lw         $t5, 0x14($t2)
    lw         $t6, 0x18($t2)
    ctc2       $t5, $5
    lw         $t7, 0x1C($t2)
    ctc2       $t6, $6
    ctc2       $t7, $7
    lw         $a0, 0x0($s5)
    nop
    sll        $a2, $a0, 16
    srl        $a2, $a2, 24
    sll        $v1, $a0, 8
    srl        $v1, $v1, 24
    srl        $t2, $a0, 24
    andi       $v0, $v1, 0xFF
    mtc2       $s6, $8
    mtc2       $a2, $9
    mtc2       $v0, $10
    mtc2       $t2, $11
    nop
    nop
    gpf        0
    andi       $v0, $a0, 0x1
    beqz       $v0, .L800B2324
    andi      $v0, $a0, 0x2
    mfc2       $v0, $25
    nop
    addu       $v0, $t8, $v0
    lbu        $a2, 0x0($v0)
    andi       $v0, $a0, 0x2
.L800B2324:
    beqz       $v0, .L800B2340
    andi      $v0, $a0, 0x4
    mfc2       $v0, $26
    nop
    addu       $v0, $t8, $v0
    lbu        $v1, 0x0($v0)
    andi       $v0, $a0, 0x4
.L800B2340:
    beqz       $v0, .L800B235C
    sll       $v0, $a2, 2
    mfc2       $v0, $27
    nop
    addu       $v0, $t8, $v0
    lbu        $t2, 0x0($v0)
    sll        $v0, $a2, 2
.L800B235C:
    addu       $v0, $v0, $t9
    lw         $t1, 0x0($v0)
    sll        $v0, $v1, 2
    addu       $v0, $v0, $t9
    lw         $a2, 0x0($v0)
    sll        $v0, $t2, 2
    addu       $v0, $v0, $t9
    lw         $a1, 0x0($v0)
    srl        $s0, $t1, 16
    addu       $t2, $t1, $zero
    srl        $a3, $a2, 16
    addu       $v1, $a2, $zero
    srl        $s2, $a1, 16
    mtc2       $s0, $8
    andi       $t5, $v1, 0xFFFF
    mtc2       $t5, $9
    mtc2       $a3, $10
    addu       $s1, $a1, $zero
    negu       $v0, $t1
    gpf        1
    mfc2       $t5, $25
    nop
    andi       $t5, $t5, 0xFFFF
    mtc2       $t5, $9
    andi       $t6, $v0, 0xFFFF
    mtc2       $t6, $10
    mfc2       $t7, $26
    nop
    andi       $t7, $t7, 0xFFFF
    mtc2       $t7, $11
    nop
    nop
    mvmva      1, 0, 3, 3, 0
    sw         $zero, 0x14($t0)
    sw         $zero, 0x18($t0)
    mfc2       $t5, $9
    mfc2       $t6, $10
    mfc2       $t7, $11
    mtc2       $t2, $8
    andi       $t4, $v1, 0xFFFF
    mtc2       $t4, $9
    mtc2       $a3, $10
    sh         $t5, 0x4($t0)
    sh         $t6, 0xA($t0)
    gpf        1
    beqz       $t3, .L800B241C
    sh        $t7, 0x10($t0)
    sh         $t5, 0x4($t3)
.L800B241C:
    mfc2       $t1, $25
    nop
    andi       $t1, $t1, 0xFFFF
    mfc2       $t2, $26
    nop
    andi       $t2, $t2, 0xFFFF
    mtc2       $s1, $8
    mtc2       $t1, $9
    mtc2       $s0, $10
    mtc2       $t2, $11
    nop
    nop
    gpf        0
    beqz       $t3, .L800B2460
    nop
    nop
    sh         $t6, 0xA($t3)
.L800B2460:
    mtc2       $s2, $8
    mtc2       $a3, $9
    mtc2       $zero, $10
    negu       $v0, $a2
    andi       $v0, $v0, 0xFFFF
    mtc2       $v0, $11
    nop
    nop
    gpl        0
    beqz       $t3, .L800B2494
    nop
    nop
    sh         $t7, 0x10($t3)
.L800B2494:
    mfc2       $t5, $25
    nop
    sra        $t5, $t5, 12
    andi       $t5, $t5, 0xFFFF
    mtc2       $t5, $9
    mfc2       $t6, $26
    nop
    sra        $t6, $t6, 12
    andi       $t6, $t6, 0xFFFF
    mtc2       $t6, $10
    mfc2       $t7, $27
    nop
    sra        $t7, $t7, 12
    andi       $t7, $t7, 0xFFFF
    mtc2       $t7, $11
    nop
    nop
    mvmva      1, 0, 3, 3, 0
    mfc2       $t5, $9
    mfc2       $t6, $10
    mfc2       $t7, $11
    mtc2       $s2, $8
    mtc2       $t1, $9
    mtc2       $s0, $10
    mtc2       $t2, $11
    sh         $t5, 0x0($t0)
    sh         $t6, 0x6($t0)
    gpf        0
    sh         $t7, 0xC($t0)
    beqz       $t3, .L800B2518
    nop
    nop
    sh         $t5, 0x0($t3)
.L800B2518:
    mtc2       $s1, $8
    sll        $a3, $a3, 16
    sra        $a3, $a3, 16
    neg        $v0, $a3
    andi       $v0, $v0, 0xFFFF
    mtc2       $v0, $9
    mtc2       $zero, $10
    andi       $t4, $v1, 0xFFFF
    mtc2       $t4, $11
    nop
    nop
    gpl        0
    beqz       $t3, .L800B2554
    addiu     $v1, $t0, 0x2
    sh         $t6, 0x6($t3)
.L800B2554:
    mfc2       $t5, $25
    nop
    sra        $t5, $t5, 12
    andi       $t5, $t5, 0xFFFF
    mtc2       $t5, $9
    mfc2       $t6, $26
    nop
    sra        $t6, $t6, 12
    andi       $t6, $t6, 0xFFFF
    mtc2       $t6, $10
    mfc2       $t4, $27
    nop
    sra        $t4, $t4, 12
    andi       $t4, $t4, 0xFFFF
    mtc2       $t4, $11
    nop
    nop
    mvmva      1, 0, 3, 3, 0
    beqz       $t3, .L800B25AC
    nop
    nop
    sh         $t7, 0xC($t3)
.L800B25AC:
    mfc2       $t5, $9
    mfc2       $t6, $10
    mfc2       $t7, $11
    lw         $v0, -0x20($fp)
    nop
    andi       $v0, $v0, 0x1
    beqz       $v0, .L800B26C4
    andi      $v0, $a0, 0x40
    mtc2       $s6, $8
    lw         $a2, 0x4($s5)
    lui        $t4, (0xFF0000 >> 16)
    beqz       $v0, .L800B2604
    and       $v1, $a2, $t4
    srl        $v0, $v1, 16
    mtc2       $v0, $9
    lw         $v1, 0x1C($t0)
    nop
    gpf        0
    lw         $t4, -0x14($fp)
    mfc2       $v0, $25
    j          .L800B2614
    sll       $v0, $v0, 1
.L800B2604:
    beq        $v1, $t4, .L800B2628
    srl       $v0, $v1, 15
    lw         $t4, -0x10($fp)
    lw         $v1, 0x1C($t0)
.L800B2614:
    addu       $v0, $v0, $t4
    lh         $v0, 0x0($v0)
    nop
    addu       $v0, $v0, $v1
    sw         $v0, 0x1C($t0)
.L800B2628:
    andi       $v0, $a0, 0x10
    beqz       $v0, .L800B2650
    andi      $v1, $a2, 0xFF
    mtc2       $v1, $9
    lw         $t4, -0x14($fp)
    nop
    gpf        0
    mfc2       $v0, $25
    j          .L800B2660
    sll       $v0, $v0, 1
.L800B2650:
    ori        $v0, $zero, 0xFF
    beq        $v1, $v0, .L800B2674
    sll       $v0, $v1, 1
    lw         $t4, -0x10($fp)
.L800B2660:
    nop
    addu       $v0, $v0, $t4
    lh         $v0, 0x0($v0)
    nop
    sw         $v0, 0x14($t0)
.L800B2674:
    andi       $v0, $a0, 0x20
    beqz       $v0, .L800B26A0
    andi      $v1, $a2, 0xFF00
    srl        $v0, $v1, 8
    mtc2       $v0, $9
    lw         $t4, -0x14($fp)
    nop
    gpf        0
    mfc2       $v0, $25
    j          .L800B26B0
    sll       $v0, $v0, 1
.L800B26A0:
    ori        $v0, $zero, 0xFF00
    beq        $v1, $v0, .L800B26C4
    srl       $v0, $v1, 7
    lw         $t4, -0x10($fp)
.L800B26B0:
    nop
    addu       $v0, $v0, $t4
    lh         $v0, 0x0($v0)
    nop
    sw         $v0, 0x18($t0)
.L800B26C4:
    lhu        $t4, 0x18($t0)
    lhu        $v1, 0x14($t0)
    sll        $t4, $t4, 16
    or         $v1, $v1, $t4
    mtc2       $v1, $0
    lwc2       $1, 0x1C($t0)
    sh         $t5, 0x2($t0)
    sh         $t6, 0x8($t0)
    mvmva      1, 0, 0, 0, 0
    sh         $t7, 0xE($t0)
    beqz       $t3, .L800B2704
    nop
    nop
    sh         $t5, 0x2($t3)
    sh         $t6, 0x8($t3)
    sh         $t7, 0xE($t3)
.L800B2704:
    swc2       $9, 0x14($t0)
    swc2       $10, 0x18($t0)
    swc2       $11, 0x1C($t0)
    beqz       $t3, .L800B2724
    addiu     $s3, $s3, 0x1
    swc2       $9, 0x14($t3)
    swc2       $10, 0x18($t3)
    swc2       $11, 0x1C($t3)
.L800B2724:
    slt        $v0, $s3, $s4
    bnez       $v0, .L800B2234
    addiu     $s5, $s5, 0x8
.L800B2730:
    lw         $fp, 0x68($sp)
    lw         $s7, 0x64($sp)
    lw         $s6, 0x60($sp)
    lw         $s5, 0x5C($sp)
    lw         $s4, 0x58($sp)
    lw         $s3, 0x54($sp)
    lw         $s2, 0x50($sp)
    lw         $s1, 0x4C($sp)
    lw         $s0, 0x48($sp)
    addiu      $sp, $sp, 0x70
    jr         $ra
    nop
.size func_800B1E98, . - func_800B1E98

.set pop
