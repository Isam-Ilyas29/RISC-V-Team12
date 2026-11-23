.text
.globl main
main:
    addi a0, zero, 0
    addi t0, zero, 1
    addi t1, zero, 1
    beq t0, t1, beq_1
    addi a0, a0, 1
beq_1:
    addi a0, a0, 2
    addi t0, zero, 1
    addi t1, zero, 2
    beq t0, t1, beq_0
    addi a0, a0, 1
beq_0:
    addi a0, a0, 2
    addi t0, zero, 1
    addi t1, zero, 2
    bne t0, t1, bne_1
    addi a0, a0, 1
bne_1:
    addi a0, a0, 2
    addi t0, zero, 1
    addi t1, zero, 1
    bne t0, t1, bne_0
    addi a0, a0, 1
bne_0:
    addi a0, a0, 2
    addi t0, zero, -1
    addi t1, zero, 1
    blt t0, t1, blt_1
    addi a0, a0, 1
blt_1:
    addi a0, a0, 2
    addi t0, zero, 1
    addi t1, zero, -1
    blt t0, t1, blt_0
    addi a0, a0, 1
blt_0:
    addi a0, a0, 2
    addi t0, zero, 1
    addi t1, zero, -1
    bge t0, t1, bge_1
    addi a0, a0, 1
bge_1:
    addi a0, a0, 2
    addi t0, zero, -1
    addi t1, zero, 1
    bge t0, t1, bge_0
    addi a0, a0, 1
bge_0:
    addi a0, a0, 2
    addi t0, zero, 1
    addi t1, zero, -1
    bltu t0, t1, bltu_1
    addi a0, a0, 1
bltu_1:
    addi a0, a0, 2
    addi t0, zero, -1
    addi t1, zero, 1
    bltu t0, t1, bltu_0
    addi a0, a0, 1
bltu_0:
    addi a0, a0, 2
    addi t0, zero, -1
    addi t1, zero, 1
    bgeu t0, t1, bgeu_1
    addi a0, a0, 1
bgeu_1:
    addi a0, a0, 2
    addi t0, zero, 1
    addi t1, zero, -1
    bgeu t0, t1, bgeu_0
    addi a0, a0, 1
bgeu_0:
    addi a0, a0, 2
    addi t0, zero, 0
    addi t1, zero, 1
beq_loop:
    addi t0, t0, 1
    addi a0, a0, 1
    beq t0, t1, beq_loop
    addi t0, zero, 0
    addi t1, zero, 2
bne_loop:
    addi t0, t0, 1
    addi a0, a0, 1
    bne t0, t1, bne_loop
    addi t0, zero, 0
    addi t1, zero, 2
blt_loop:
    addi t0, t0, 1
    addi a0, a0, 1
    blt t0, t1, blt_loop
    addi t0, zero, 2
    addi t1, zero, 1
bge_loop:
    addi t0, t0, -1
    addi a0, a0, 1
    bge t0, t1, bge_loop
    addi t0, zero, 0
    addi t1, zero, 2
bltu_loop:
    addi t0, t0, 1
    addi a0, a0, 1
    bltu t0, t1, bltu_loop
    addi t0, zero, 2
    addi t1, zero, 1
bgeu_loop:
    addi t0, t0, -1
    addi a0, a0, 1
    bgeu t0, t1, bgeu_loop
    addi a0, a0, 1
