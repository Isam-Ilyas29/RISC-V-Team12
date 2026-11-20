.text
.globl main
main:
    addi a0, zero, -2048
    addi a0, a0, 2047
    addi zero, a0, 3
    addi a0, zero, 0
    addi t6, zero, -372
    addi a0, t6, 6
    bne  t6, a0, forward
    addi a0, zero, 99
forward:
    addi t5, t6, 0
    bne  t5, t6, done
    addi a0, a0, 366
done:
    addi a0, a0, -1
