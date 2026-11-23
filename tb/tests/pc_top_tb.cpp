#include "base_testbench.h"

class ProgramCounter : public BaseTestbench
{
protected:
    void clock(bool high)
    {
        top->clk = high;
        sample();
    }

    void initializeInputs() override
    {
        top->clk = 0;
        top->rst = 0;
        top->PCsrc = 0;
        top->jalr = 0;
        top->ImmOp = 0;
        top->register_target = 0;
    }
};

// Check PC + 4 and forward and backward relative branches.
TEST_F(ProgramCounter, FallThroughAndRelativeBranches)
{
    top->clk = 0;
    top->rst = 1;
    top->PCsrc = 0;
    top->jalr = 0;
    top->register_target = 0;
    top->ImmOp = 16;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->PC, 0u);
    top->rst = 0;
    unsigned pc = 0;
    for (int displacement : {0, 16, -8, -24, 0}) {
        top->clk = 0;
        top->PCsrc = displacement != 0;
        top->ImmOp = displacement;
        sample();
        WAVE_EXPECT_EQ(top->PC, pc);
        if (top->PCsrc)
            pc += displacement;
        else
            pc += 4;
        clock(1);
        WAVE_EXPECT_EQ(top->PC, pc);
    }
}

// Check that a register jump clears target bit 0 and preserves the other bits.
TEST_F(ProgramCounter, RegisterJumpClearsOnlyBitZero)
{
    top->rst = 1;
    clock(0);
    clock(1);
    top->rst = 0;
    top->PCsrc = 1;
    top->jalr = 1;
    for (unsigned target : {0x101u, 0x200u, 0xfffffffdu, 3u}) {
        top->clk = 0;
        top->register_target = target;
        sample();
        clock(1);
        WAVE_EXPECT_EQ(top->PC, target & ~1u);
    }
}

// Check reset priority, normal PC increments and a branch with zero offset.
TEST_F(ProgramCounter, ResetPriorityAndSequentialSelection)
{
    top->PCsrc = 1;
    top->jalr = 1;
    top->register_target = 0x101;
    top->rst = 1;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->PC, 0u);
    top->clk = 0;
    top->rst = 0;
    top->PCsrc = 0;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->PC, 4u);
    top->clk = 0;
    top->PCsrc = 1;
    top->jalr = 0;
    top->ImmOp = 0;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->PC, 4u);
}
