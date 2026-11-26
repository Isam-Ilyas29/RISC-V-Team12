.text
.globl main
.equ trigger_address, 0x20000
.equ light_delay, 64

# The assembler expands each call inline. The delay runs in the subroutine.
.macro light_then_wait mask
    li a0, \mask
    mv t0, s1
    jal ra, delay
.endm

# XOR old bits 6 and 5 to get the next low bit, then keep only seven bits.
.macro advance_lfsr
    srli t1, s2, 6
    srli t2, s2, 5
    xor t1, t1, t2
    andi t1, t1, 1
    slli s2, s2, 1
    andi s2, s2, 127
    or s2, s2, t1
.endm

main:
    li a0, 0
    li s0, trigger_address
    li s1, light_delay
    li s2, 1                 # Nonzero seed for the seven-bit LFSR.
    j wait_release           # A button held during reset is not a new press.

wait_trigger:
    # The CPU exposes the button as a read-only byte at trigger_address.
    lbu t0, 0(s0)
    bne t0, zero, sequence_lights
    advance_lfsr             # How long the driver waits changes the final delay.
    j wait_trigger

sequence_lights:
    # Each set bit in a0 lights one LED.
    light_then_wait 0x01
    light_then_wait 0x03
    light_then_wait 0x07
    light_then_wait 0x0f
    light_then_wait 0x1f
    light_then_wait 0x3f
    light_then_wait 0x7f
    li a0, 0xff

final_hold:
    # All lights stay on for 64 + LFSR delay iterations, then turn off together.
    advance_lfsr
    add t0, s1, s2
    jal ra, delay
    li a0, 0

wait_release:
    # A held button must not start another sequence.
    lbu t0, 0(s0)
    bne t0, zero, wait_release
    j wait_trigger

delay:
    # t0 counts down to zero. ret jumps back to the address saved by jal.
    addi t0, t0, -1
    bne t0, zero, delay
    ret
