.text
.globl main
main:
    addi a0, zero, 0
    addi t0, zero, 16
    addi t1, zero, -1
    sw t1, 0(t0)

    # Store one byte; the other three bytes must remain unchanged.
    addi t1, zero, -128
    sb t1, 1(t0)
    lb a0, 1(t0)
    lbu a0, 1(t0)
    lw a0, 0(t0)

    # Store 0x8000, then compare signed and unsigned halfword loads.
    addi t1, zero, 1
    slli t1, t1, 15
    sh t1, 2(t0)
    lw a0, 0(t0)
    lh a0, 2(t0)
    lhu a0, 2(t0)
    lb a0, 0(t0)
    lbu a0, 0(t0)

    # Positive byte and halfword limits should not be sign extended.
    addi t1, zero, 127
    sb t1, 1(t0)
    lb a0, 1(t0)
    lbu a0, 1(t0)
    lh a0, 0(t0)
    lhu a0, 0(t0)
    lw a0, 0(t0)
