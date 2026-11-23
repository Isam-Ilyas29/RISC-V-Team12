.text
.globl main
main:
    addi a0, zero, 0
    addi t0, zero, -8
    addi t1, zero, 1
    slli a0, t0, 1
    srli a0, t0, 1
    srai a0, t0, 1
    sll a0, t0, t1
    srl a0, t0, t1
    sra a0, t0, t1
    sw a0, 0(zero)
    addi a0, zero, 0
    lw a0, 0(zero)
    beq t0, t0, check_bne
    addi a0, zero, -99
check_bne:
    bne t0, t1, check_blt
    addi a0, zero, -99
check_blt:
    blt t0, t1, check_bge
    addi a0, zero, -99
check_bge:
    bge t1, t0, check_bltu
    addi a0, zero, -99
check_bltu:
    bltu t1, t0, check_bgeu
    addi a0, zero, -99
check_bgeu:
    bgeu t0, t1, call_function
    addi a0, zero, -99
call_function:
    jal ra, increment
    addi a0, a0, 1
    jal zero, done
increment:
    addi a0, a0, 1
    jalr zero, 0(ra)
done:
    jal zero, done
