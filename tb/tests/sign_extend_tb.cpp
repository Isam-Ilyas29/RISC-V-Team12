#include "base_testbench.h"

class Immediate : public BaseTestbench
{
protected:
    void load(uint32_t instruction, unsigned type)
    {
        top->imm = instruction >> 7;
        top->imm_src = type;
        sample();
    }

    void initializeInputs() override
    {
        top->imm = 0;
        top->imm_src = 0;
    }
};

// I-type immediates cover zero, one, minus one and signed 12-bit limits.
TEST_F(Immediate, ImmediateOffsets)
{
    // Immediate 0.
    load(0x000ad580u, 0);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000000u);

    // Immediate 1.
    load(0x001ad580u, 0);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000001u);

    // Immediate 2047.
    load(0x7ffad580u, 0);
    WAVE_EXPECT_EQ(top->imm_op, 0x000007ffu);

    // Immediate -2048.
    load(0x800ad580u, 0);
    WAVE_EXPECT_EQ(top->imm_op, 0xfffff800u);

    // Immediate -1.
    load(0xfffad580u, 0);
    WAVE_EXPECT_EQ(top->imm_op, 0xffffffffu);
}

// B-type immediates cover field boundaries and signed limits with two-byte alignment.
TEST_F(Immediate, BranchOffsets)
{
    // Immediate 0.
    load(0x01578000u, 1);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000000u);

    // Immediate 2.
    load(0x01578100u, 1);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000002u);

    // Immediate 32.
    load(0x03578000u, 1);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000020u);

    // Immediate 64.
    load(0x05578000u, 1);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000040u);

    // Immediate 2048.
    load(0x01578080u, 1);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000800u);

    // Immediate 4094.
    load(0x7f578f80u, 1);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000ffeu);

    // Immediate -4096.
    load(0x81578000u, 1);
    WAVE_EXPECT_EQ(top->imm_op, 0xfffff000u);

    // Immediate -2.
    load(0xff578f80u, 1);
    WAVE_EXPECT_EQ(top->imm_op, 0xfffffffeu);
}

// Check positive and negative JAL offsets, including the largest encodable values.
TEST_F(Immediate, JumpOffsets)
{
    // Immediate -1048576.
    load(0x80000000u, 3);
    WAVE_EXPECT_EQ(top->imm_op, 0xfff00000u);

    // Immediate -2048.
    load(0x801ff000u, 3);
    WAVE_EXPECT_EQ(top->imm_op, 0xfffff800u);

    // Immediate -4.
    load(0xffdff000u, 3);
    WAVE_EXPECT_EQ(top->imm_op, 0xfffffffcu);

    // Immediate 0.
    load(0x00000000u, 3);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000000u);

    // Immediate 2.
    load(0x00200000u, 3);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000002u);

    // Immediate 2048.
    load(0x00100000u, 3);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000800u);

    // Immediate 4096.
    load(0x00001000u, 3);
    WAVE_EXPECT_EQ(top->imm_op, 0x00001000u);

    // Immediate 1048574.
    load(0x7ffff000u, 3);
    WAVE_EXPECT_EQ(top->imm_op, 0x000ffffeu);
}

// Check that the two S-type immediate fields form the correct signed store offset.
TEST_F(Immediate, StoreOffsets)
{
    // Immediate -2048.
    load(0x81578023u, 2);
    WAVE_EXPECT_EQ(top->imm_op, 0xfffff800u);

    // Immediate -32.
    load(0xff578023u, 2);
    WAVE_EXPECT_EQ(top->imm_op, 0xffffffe0u);

    // Immediate -1.
    load(0xff578fa3u, 2);
    WAVE_EXPECT_EQ(top->imm_op, 0xffffffffu);

    // Immediate 0.
    load(0x01578023u, 2);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000000u);

    // Immediate 1.
    load(0x015780a3u, 2);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000001u);

    // Immediate 31.
    load(0x01578fa3u, 2);
    WAVE_EXPECT_EQ(top->imm_op, 0x0000001fu);

    // Immediate 32.
    load(0x03578023u, 2);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000020u);

    // Immediate 2047.
    load(0x7f578fa3u, 2);
    WAVE_EXPECT_EQ(top->imm_op, 0x000007ffu);
}


// U-type immediates retain bits 31:12 and clear the low twelve bits.
TEST_F(Immediate, UpperImmediate)
{
    // Immediate 0.
    load(0x00000000u, 4);
    WAVE_EXPECT_EQ(top->imm_op, 0x00000000u);

    // Immediate 305418240.
    load(0x12345fffu, 4);
    WAVE_EXPECT_EQ(top->imm_op, 0x12345000u);

    // Immediate 2147483648.
    load(0x80000037u, 4);
    WAVE_EXPECT_EQ(top->imm_op, 0x80000000u);

    // Immediate 4294963200.
    load(0xfffff037u, 4);
    WAVE_EXPECT_EQ(top->imm_op, 0xfffff000u);
}
