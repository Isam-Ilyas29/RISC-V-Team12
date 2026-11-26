#include "cpu_testbench.h"
#include <cstdlib>
#include <cstdint>

unsigned int ticks = 0;

class CpuRegression : public CpuTestbench {};

// Check ADD/SUB selection and ADDI immediates from -2048 to 2047.
TEST_F(CpuRegression, ArithmeticDecode)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("arithmetic_decode", ".S"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    WAVE_ASSERT_EQ(step(), 0);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0);  // li t0,7
    WAVE_ASSERT_EQ(step(), 0);  // li t1,3
    WAVE_ASSERT_EQ(step(), 10);  // add a0,t0,t1
    WAVE_ASSERT_EQ(step(), 4);  // sub a0,t0,t1
    WAVE_ASSERT_EQ(step(), 1028);  // addi a0,a0,1024
    WAVE_ASSERT_EQ(step(), 1027);  // addi a0,a0,-1
    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-1021));  // addi a0,a0,-2048
    WAVE_ASSERT_EQ(step(), 1026);  // addi a0,a0,2047
}

// Run the ADDI/BNE program and check a0 after each instruction and after reset.
TEST_F(CpuRegression, AddiAndBne)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("addi_bne", ".S"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());

    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-2048));  // li a0,-2048
    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-1));  // addi a0,a0,2047
    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-1));  // addi zero,a0,3
    WAVE_ASSERT_EQ(step(), 0);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0);  // li t6,-372
    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-366));  // addi a0,t6,6
    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-366));  // bne t6,a0,bfc00020 <forward>
    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-366));  // mv t5,t6
    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-366));  // bne t5,t6,bfc0002c <done>
    WAVE_ASSERT_EQ(step(), 0);  // addi a0,a0,366
    WAVE_ASSERT_EQ(step(), static_cast<uint32_t>(-1));  // addi a0,a0,-1

    reset();
    WAVE_ASSERT_EQ(step(), 0xfffff800u) << "first instruction after reset";
}

// Run all six branch instructions and check the expected a0 sequence.
TEST_F(CpuRegression, AllBranchInstructions)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("branches", ".S"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    WAVE_ASSERT_EQ(step(), 0);  // li t1,255
    WAVE_ASSERT_EQ(step(), 0);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0);  // li a1,0
    WAVE_ASSERT_EQ(step(), 0);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 2);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 2);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 2);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 2);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 3);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 5);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 5);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 5);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 5);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 7);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 7);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 7);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 7);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 8);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 10);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 10);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 10);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 10);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 12);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 12);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 12);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 12);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 13);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 15);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 15);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 15);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 15);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 17);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 17);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 17);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 17);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 18);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 20);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 20);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 20);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 20);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 22);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 22);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 22);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 22);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 23);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 25);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 25);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 25);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 25);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 27);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 27);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 27);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 27);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 28);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 30);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 30);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 30);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 30);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 31);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 31);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 31);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 32);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 32);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 32);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 32);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 32);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 33);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 33);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 33);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 34);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 34);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 34);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 34);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 34);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 35);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 35);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 35);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 36);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 36);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 36);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 36);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 36);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 37);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 37);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 37);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 38);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 38);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 38);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 38);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 38);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 39);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 39);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 39);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 40);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 40);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 40);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 40);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 40);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 41);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 41);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 41);  // addi a1,a1,1
    WAVE_ASSERT_EQ(step(), 42);  // bne a1,t1,bfc0000c <iloop>
    WAVE_ASSERT_EQ(step(), 42);  // mv a0,a1
    WAVE_ASSERT_EQ(step(), 43);  // addi a1,a1,1
}

// Run register and immediate shifts, checking masking, sign extension and x0.
TEST_F(CpuRegression, ShiftInstructions)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("shifts", ".S"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    WAVE_ASSERT_EQ(step(), 0x00000000u);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0x00000000u);  // li t0,-8
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // slli a0,t0,0x0
    WAVE_ASSERT_EQ(step(), 0xfffffff0u);  // slli a0,t0,0x1
    WAVE_ASSERT_EQ(step(), 0x00000000u);  // slli a0,t0,0x1f
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // srli a0,t0,0x0
    WAVE_ASSERT_EQ(step(), 0x7ffffffcu);  // srli a0,t0,0x1
    WAVE_ASSERT_EQ(step(), 0x00000001u);  // srli a0,t0,0x1f
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // srai a0,t0,0x0
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // srai a0,t0,0x1
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // srai a0,t0,0x1f
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // li t1,0
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // sll a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // li t1,1
    WAVE_ASSERT_EQ(step(), 0xfffffff0u);  // sll a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffff0u);  // li t1,31
    WAVE_ASSERT_EQ(step(), 0x00000000u);  // sll a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0x00000000u);  // li t1,32
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // sll a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // li t1,33
    WAVE_ASSERT_EQ(step(), 0xfffffff0u);  // sll a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffff0u);  // li t1,-1
    WAVE_ASSERT_EQ(step(), 0x00000000u);  // sll a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0x00000000u);  // li t1,0
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // srl a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // li t1,1
    WAVE_ASSERT_EQ(step(), 0x7ffffffcu);  // srl a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0x7ffffffcu);  // li t1,31
    WAVE_ASSERT_EQ(step(), 0x00000001u);  // srl a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0x00000001u);  // li t1,32
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // srl a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // li t1,33
    WAVE_ASSERT_EQ(step(), 0x7ffffffcu);  // srl a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0x7ffffffcu);  // li t1,-1
    WAVE_ASSERT_EQ(step(), 0x00000001u);  // srl a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0x00000001u);  // li t1,0
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // sra a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // li t1,1
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // sra a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // li t1,31
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // sra a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // li t1,32
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // sra a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffff8u);  // li t1,33
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // sra a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // li t1,-1
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // sra a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0x00000001u);  // li a0,1
    WAVE_ASSERT_EQ(step(), 0x80000000u);  // slli a0,a0,0x1f
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // srai a0,a0,0x1f
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // srli zero,a0,0x1
    WAVE_ASSERT_EQ(step(), 0x00000000u);  // li a0,0
}

// Check forward and backward jumps, return addresses and JALR returns.
TEST_F(CpuRegression, JumpInstructions)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("jumps", ".S"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    WAVE_ASSERT_EQ(step(), 0);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0);  // jal ra,bfc00010 <forward>
    WAVE_ASSERT_EQ(step(), 0xbfc00008u);  // mv a0,ra
    WAVE_ASSERT_EQ(step(), 0xbfc00008u);  // li t0,33
    WAVE_ASSERT_EQ(step(), 0xbfc00008u);  // jalr t0,t0
    WAVE_ASSERT_EQ(step(), 0xbfc0001cu);  // mv a0,t0
    WAVE_ASSERT_EQ(step(), 0xbfc0001cu);  // li t1,53
    WAVE_ASSERT_EQ(step(), 0xbfc0001cu);  // jalr t2,-4(t1)
    WAVE_ASSERT_EQ(step(), 44);  // mv a0,t2
    WAVE_ASSERT_EQ(step(), 44);  // ret
    WAVE_ASSERT_EQ(step(), 45);  // addi a0,a0,1
    WAVE_ASSERT_EQ(step(), 45);  // j bfc00038 <done>
    WAVE_ASSERT_EQ(step(), 45);  // j bfc00048 <jump_back>
    WAVE_ASSERT_EQ(step(), 45);  // jal t1,bfc0003c <back>
    WAVE_ASSERT_EQ(step(), 0xbfc0004cu);  // mv a0,t1
    WAVE_ASSERT_EQ(step(), 0xbfc0004cu);  // j bfc00050 <finish>
    WAVE_ASSERT_EQ(step(), 0xbfc0004du);  // addi a0,a0,1
    WAVE_ASSERT_EQ(step(), 0xbfc0004du);  // j bfc00054 <hold>
    WAVE_ASSERT_EQ(step(), 0xbfc0004du);  // j bfc00054 <hold>
}

// Store and load words, checking that loaded data reaches the correct register.
TEST_F(CpuRegression, LoadStoreWriteback)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("memory", ".S"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    WAVE_ASSERT_EQ(step(), 0);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0);  // li t0,16
    WAVE_ASSERT_EQ(step(), 0);  // li t1,123
    WAVE_ASSERT_EQ(step(), 0);  // sw t1,-4(t0)
    WAVE_ASSERT_EQ(step(), 0);  // li t1,0
    WAVE_ASSERT_EQ(step(), 123);  // lw a0,-4(t0)
    WAVE_ASSERT_EQ(step(), 123);  // li t1,-1
    WAVE_ASSERT_EQ(step(), 123);  // sw t1,4(t0)
    WAVE_ASSERT_EQ(step(), 123);  // lw zero,4(t0)
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // lw a0,4(t0)
}

// Check signed/unsigned byte and halfword loads, and preserve bytes outside each store.
TEST_F(CpuRegression, ByteAndHalfwordTransfers)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("partial_memory", ".S"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    WAVE_ASSERT_EQ(step(), 0);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0);  // li t0,16
    WAVE_ASSERT_EQ(step(), 0);  // li t1,-1
    WAVE_ASSERT_EQ(step(), 0);  // sw t1,0(t0)
    WAVE_ASSERT_EQ(step(), 0);  // li t1,-128
    WAVE_ASSERT_EQ(step(), 0);  // sb t1,1(t0)
    WAVE_ASSERT_EQ(step(), 0xffffff80u);  // lb a0,1(t0)
    WAVE_ASSERT_EQ(step(), 0x80u);  // lbu a0,1(t0)
    WAVE_ASSERT_EQ(step(), 0xffff80ffu);  // lw a0,0(t0)
    WAVE_ASSERT_EQ(step(), 0xffff80ffu);  // li t1,1
    WAVE_ASSERT_EQ(step(), 0xffff80ffu);  // slli t1,t1,0xf
    WAVE_ASSERT_EQ(step(), 0xffff80ffu);  // sh t1,2(t0)
    WAVE_ASSERT_EQ(step(), 0x800080ffu);  // lw a0,0(t0)
    WAVE_ASSERT_EQ(step(), 0xffff8000u);  // lh a0,2(t0)
    WAVE_ASSERT_EQ(step(), 0x8000u);  // lhu a0,2(t0)
    WAVE_ASSERT_EQ(step(), 0xffffffffu);  // lb a0,0(t0)
    WAVE_ASSERT_EQ(step(), 0xffu);  // lbu a0,0(t0)
    WAVE_ASSERT_EQ(step(), 0xffu);  // li t1,127
    WAVE_ASSERT_EQ(step(), 0xffu);  // sb t1,1(t0)
    WAVE_ASSERT_EQ(step(), 0x7fu);  // lb a0,1(t0)
    WAVE_ASSERT_EQ(step(), 0x7fu);  // lbu a0,1(t0)
    WAVE_ASSERT_EQ(step(), 0x7fffu);  // lh a0,0(t0)
    WAVE_ASSERT_EQ(step(), 0x7fffu);  // lhu a0,0(t0)
    WAVE_ASSERT_EQ(step(), 0x80007fffu);  // lw a0,0(t0)
}

// Run shifts, memory operations, branches and jumps together for the overview waveform.
TEST_F(CpuRegression, Overview)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("overview", ".S"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    WAVE_ASSERT_EQ(step(), 0);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0);  // li t0,-8
    WAVE_ASSERT_EQ(step(), 0);  // li t1,1
    WAVE_ASSERT_EQ(step(), 0xfffffff0u);  // slli a0,t0,0x1
    WAVE_ASSERT_EQ(step(), 0x7ffffffcu);  // srli a0,t0,0x1
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // srai a0,t0,0x1
    WAVE_ASSERT_EQ(step(), 0xfffffff0u);  // sll a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0x7ffffffcu);  // srl a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // sra a0,t0,t1
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // sw a0,0(zero)
    WAVE_ASSERT_EQ(step(), 0);  // li a0,0
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // lw a0,0(zero)
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // beq t0,t0,bfc00038 <check_bne>
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // bne t0,t1,bfc00040 <check_blt>
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // blt t0,t1,bfc00048 <check_bge>
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // bge t1,t0,bfc00050 <check_bltu>
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // bltu t1,t0,bfc00058 <check_bgeu>
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // bgeu t0,t1,bfc00060 <call_function>
    WAVE_ASSERT_EQ(step(), 0xfffffffcu);  // jal ra,bfc0006c <increment>
    WAVE_ASSERT_EQ(step(), 0xfffffffdu);  // addi a0,a0,1
    WAVE_ASSERT_EQ(step(), 0xfffffffdu);  // ret
    WAVE_ASSERT_EQ(step(), 0xfffffffeu);  // addi a0,a0,1
    WAVE_ASSERT_EQ(step(), 0xfffffffeu);  // j bfc00074 <done>
    WAVE_ASSERT_EQ(step(), 0xfffffffeu);  // j bfc00074 <done>
    WAVE_ASSERT_EQ(step(), 0xfffffffeu);  // j bfc00074 <done>
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
