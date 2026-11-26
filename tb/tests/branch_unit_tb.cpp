#include "base_testbench.h"

class Branch : public BaseTestbench
{
protected:
    void load(uint32_t a, uint32_t b, unsigned condition)
    {
        top->rs1_data = a;
        top->rs2_data = b;
        top->branch_ctrl = condition;
        sample();
    }

    void initializeInputs() override
    {
        top->rs1_data = 0;
        top->rs2_data = 0;
        top->branch_ctrl = 0;
    }
};

// Check equality, both operand orders and signed/unsigned limits.
TEST_F(Branch, Equal)
{
    load(0x00000000u, 0x00000000u, 0b000);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000001u, 0x00000002u, 0b000);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000002u, 0x00000001u, 0b000);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0xffffffffu, 0x00000001u, 0b000);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000001u, 0xffffffffu, 0b000);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x80000000u, 0x7fffffffu, 0b000);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x7fffffffu, 0x80000000u, 0b000);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

}

// Check equality, both operand orders and signed/unsigned limits.
TEST_F(Branch, NotEqual)
{
    load(0x00000000u, 0x00000000u, 0b001);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000001u, 0x00000002u, 0b001);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000002u, 0x00000001u, 0b001);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0xffffffffu, 0x00000001u, 0b001);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000001u, 0xffffffffu, 0b001);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x80000000u, 0x7fffffffu, 0b001);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x7fffffffu, 0x80000000u, 0b001);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

}

// Check equality, both operand orders and signed/unsigned limits.
TEST_F(Branch, SignedLess)
{
    load(0x00000000u, 0x00000000u, 0b100);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000001u, 0x00000002u, 0b100);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000002u, 0x00000001u, 0b100);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0xffffffffu, 0x00000001u, 0b100);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000001u, 0xffffffffu, 0b100);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x80000000u, 0x7fffffffu, 0b100);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x7fffffffu, 0x80000000u, 0b100);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

}

// Check equality, both operand orders and signed/unsigned limits.
TEST_F(Branch, SignedGreaterOrEqual)
{
    load(0x00000000u, 0x00000000u, 0b101);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000001u, 0x00000002u, 0b101);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000002u, 0x00000001u, 0b101);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0xffffffffu, 0x00000001u, 0b101);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000001u, 0xffffffffu, 0b101);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x80000000u, 0x7fffffffu, 0b101);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x7fffffffu, 0x80000000u, 0b101);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

}

// Check equality, both operand orders and signed/unsigned limits.
TEST_F(Branch, UnsignedLess)
{
    load(0x00000000u, 0x00000000u, 0b110);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000001u, 0x00000002u, 0b110);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000002u, 0x00000001u, 0b110);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0xffffffffu, 0x00000001u, 0b110);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000001u, 0xffffffffu, 0b110);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x80000000u, 0x7fffffffu, 0b110);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x7fffffffu, 0x80000000u, 0b110);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

}

// Check equality, both operand orders and signed/unsigned limits.
TEST_F(Branch, UnsignedGreaterOrEqual)
{
    load(0x00000000u, 0x00000000u, 0b111);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000001u, 0x00000002u, 0b111);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x00000002u, 0x00000001u, 0b111);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0xffffffffu, 0x00000001u, 0b111);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x00000001u, 0xffffffffu, 0b111);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0x80000000u, 0x7fffffffu, 0b111);
    WAVE_EXPECT_EQ(top->branch_taken, 1u);

    load(0x7fffffffu, 0x80000000u, 0b111);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

}

// Both reserved controls must reject a branch, even with equal operands.
TEST_F(Branch, ReservedConditions)
{
    load(0, 0, 2);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0, 1, 2);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0, 0, 3);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

    load(0, 1, 3);
    WAVE_EXPECT_EQ(top->branch_taken, 0u);

}
