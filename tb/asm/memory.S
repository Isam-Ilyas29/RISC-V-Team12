.text
.globl main
main:
    addi a0, zero, 0
    addi t0, zero, 16
    addi t1, zero, 123
    sw t1, -4(t0)
    addi t1, zero, 0
    lw a0, -4(t0)
    addi t1, zero, -1
    sw t1, 4(t0)
    lw zero, 4(t0)
    lw a0, 4(t0)
