# Streaming LZSS decoder, including its 40-byte state block in .text.
.text
.set push
.set noreorder
.set at
.align 2
.globl D_80034CF0
.type D_80034CF0, @object
D_80034CF0:
    .word 0, 0, 0, 0, 0, 0, 0, 0, 0
.size D_80034CF0, . - D_80034CF0
.globl D_80034D14
.type D_80034D14, @object
D_80034D14:
    .word 0
.size D_80034D14, . - D_80034D14

.globl SysCdromGetPackPointer
.type SysCdromGetPackPointer, @function
SysCdromGetPackPointer:
    sll $a1, $a1, 2
    addu $v1, $a0, $a1
    lw $v0, 0x0($v1)
    jr $ra
    addu $v0, $v0, $a0
.size SysCdromGetPackPointer, . - SysCdromGetPackPointer

.globl SysCdromSetLzsExtract
.type SysCdromSetLzsExtract, @function
SysCdromSetLzsExtract:
    lui $v0, %hi(SysCdromLzsExtract)
    addiu $v0, $v0, %lo(SysCdromLzsExtract)
    sw $a0, D_80034CF0
    sw $a1, D_80034CF0 + 0x4
    sw $a1, D_80034CF0 + 0xC
    sw $v0, D_80034D14
    jr $ra
    nop
.size SysCdromSetLzsExtract, . - SysCdromSetLzsExtract

.globl func_80034D5C
.type func_80034D5C, @function
func_80034D5C:
    lui $v0, %hi(D_80034D14)
    lw $v0, %lo(D_80034D14)($v0)
    lui $t0, %hi(D_80034CF0)
    lw $t0, %lo(D_80034CF0)($t0)
    lui $t1, %hi(D_80034CF0 + 0x4)
    lw $t1, %lo(D_80034CF0 + 0x4)($t1)
    lui $t2, %hi(D_80034CF0 + 0x8)
    lw $t2, %lo(D_80034CF0 + 0x8)($t2)
    lui $t4, %hi(D_80034CF0 + 0xC)
    lw $t4, %lo(D_80034CF0 + 0xC)($t4)
    lui $t5, %hi(D_80034CF0 + 0x10)
    lw $t5, %lo(D_80034CF0 + 0x10)($t5)
    lui $t6, %hi(D_80034CF0 + 0x14)
    lw $t6, %lo(D_80034CF0 + 0x14)($t6)
    ori $t7, $zero, 0x4800
    lui $a0, %hi(D_80034CF0 + 0x18)
    lw $a0, %lo(D_80034CF0 + 0x18)($a0)
    sw $ra, D_80034CF0 + 0x20
    jr $v0
    nop
.size func_80034D5C, . - func_80034D5C

.globl func_80034DB0
.type func_80034DB0, @function
func_80034DB0:
    sw $ra, D_80034D14
    lui $ra, %hi(D_80034CF0 + 0x20)
    lw $ra, %lo(D_80034CF0 + 0x20)($ra)
    sw $t0, D_80034CF0
    sw $t1, D_80034CF0 + 0x4
    sw $t2, D_80034CF0 + 0x8
    sw $t4, D_80034CF0 + 0xC
    sw $t5, D_80034CF0 + 0x10
    sw $t6, D_80034CF0 + 0x14
    sw $a0, D_80034CF0 + 0x18
    jr $ra
    ori $v0, $zero, 0x1
.size func_80034DB0, . - func_80034DB0

.globl SysCdromLzsExtract
.type SysCdromLzsExtract, @function
SysCdromLzsExtract:
    lw $t2, 0x0($t0)
    addiu $t0, $t0, 0x4
    addiu $t7, $t7, -0x4
    addu $t6, $zero, $zero
.L80034E10:
    bnez $t6, .L80034E40
    nop
    ori $t6, $zero, 0x8
    lbu $t5, 0x0($t0)
    addiu $t0, $t0, 0x1
    addiu $t2, $t2, -0x1
    beqz $t2, .L80034F34
    addiu $t7, $t7, -0x1
    bnez $t7, .L80034E40
    nop
    jal func_80034DB0
    nop
.L80034E40:
    andi $v0, $t5, 0x1
    beqz $v0, .L80034E7C
    nop
    lbu $v0, 0x0($t0)
    addiu $t0, $t0, 0x1
    sb $v0, 0x0($t1)
    addiu $t1, $t1, 0x1
    addiu $t2, $t2, -0x1
    beqz $t2, .L80034F34
    addiu $t7, $t7, -0x1
    bnez $t7, .L80034F24
    nop
    jal func_80034DB0
    nop
    j .L80034F24
.L80034E7C:
    lbu $a0, 0x0($t0)
    addiu $t0, $t0, 0x1
    addiu $t7, $t7, -0x1
    bnez $t7, .L80034E98
    nop
    jal func_80034DB0
    nop
.L80034E98:
    lbu $a1, 0x0($t0)
    addiu $t0, $t0, 0x1
    andi $v0, $a1, 0xF0
    sll $v0, $v0, 4
    or $a0, $a0, $v0
    andi $v0, $a1, 0xF
    addu $t3, $t1, $v0
    addiu $t3, $t3, 0x3
    addiu $v0, $a0, -0xFEE
    subu $v1, $t1, $t4
    subu $v0, $v1, $v0
    andi $v0, $v0, 0xFFF
    subu $a3, $t1, $v0
.L80034ECC:
    sltu $v0, $a3, $t4
    beqz $v0, .L80034EE8
    nop
    sb $zero, 0x0($t1)
    addiu $t1, $t1, 0x1
    j .L80034ECC
    addiu $a3, $a3, 0x1
.L80034EE8:
    sltu $v0, $t1, $t3
    beqz $v0, .L80034F08
    nop
    lbu $v0, 0x0($a3)
    addiu $a3, $a3, 0x1
    sb $v0, 0x0($t1)
    j .L80034EE8
    addiu $t1, $t1, 0x1
.L80034F08:
    addiu $t2, $t2, -0x2
    beqz $t2, .L80034F34
    addiu $t7, $t7, -0x1
    bgtz $t7, .L80034F24
    nop
    jal func_80034DB0
    nop
.L80034F24:
    sra $t5, $t5, 1
    addiu $t6, $t6, -0x1
    j .L80034E10
    nop
.L80034F34:
    jr $ra
    addu $v0, $zero, $zero
.size SysCdromLzsExtract, . - SysCdromLzsExtract

.set pop
