.text
.globl main
main:
    addi a0, zero, 0
    jal ra, forward
    addi a0, a0, 1
    jal zero, done
forward:
    addi a0, ra, 0
    addi t0, zero, 33
    jalr t0, 0(t0)
    addi a0, zero, -99
    addi a0, t0, 0
    addi t1, zero, 53
    jalr t2, -4(t1)
    addi a0, zero, -99
    addi a0, t2, 0
    jalr zero, 0(ra)
done:
    jal zero, jump_back
back:
    addi a0, t1, 0
    jal zero, finish
    addi zero, zero, 0
jump_back:
    jal t1, back
    addi a0, zero, -99
finish:
    addi a0, a0, 1
hold:
    jal zero, hold
