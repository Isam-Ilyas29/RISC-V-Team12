.text
.globl main
main:
    addi a0, zero, 0
    addi t0, zero, 7
    addi t1, zero, 3
    add a0, t0, t1
    sub a0, t0, t1
    # Bit 30 is part of the immediate, not a SUB selector.
    addi a0, a0, 1024
    addi a0, a0, -1
    addi a0, a0, -2048
    addi a0, a0, 2047
