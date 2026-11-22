#include "base_testbench.h"

class ProgramCounter : public BaseTestbench
{
protected:
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

TEST_F(ProgramCounter, FallThroughAndRelativeBranches)
{
    top->clk = 0;
    top->rst = 1;
    top->PCsrc = 0;
    top->jalr = 0;
    top->register_target = 0;
    top->ImmOp = 16;
    sample();
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->PC, 0u);
    top->rst = 0;
    unsigned pc = 0;
    for (int displacement : {0, 16, -8, -24, 0}) {
        top->clk = 0;
        top->PCsrc = displacement != 0;
        top->ImmOp = displacement;
        sample();
        WAVE_EXPECT_EQ(top->PC, pc);
        pc += displacement ? displacement : 4;
        top->clk = 1;
        sample();
        WAVE_EXPECT_EQ(top->PC, pc);
    }
}
TEST_F(ProgramCounter, RegisterJumpClearsOnlyBitZero)
{
    top->rst = 1;
    top->clk = 0;
    sample();
    top->clk = 1;
    sample();
    top->rst = 0;
    top->PCsrc = 1;
    top->jalr = 1;
    for (unsigned target : {0x101u, 0x200u, 0xfffffffdu, 3u}) {
        top->clk = 0;
        top->register_target = target;
        sample();
        top->clk = 1;
        sample();
        WAVE_EXPECT_EQ(top->PC, target & ~1u);
    }
}

TEST_F(ProgramCounter, ResetPriorityAndSequentialSelection)
{
    top->PCsrc = 1;
    top->jalr = 1;
    top->register_target = 0x101;
    top->rst = 1;
    sample();
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->PC, 0u);
    top->clk = 0;
    top->rst = 0;
    top->PCsrc = 0;
    sample();
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->PC, 4u);
    top->clk = 0;
    top->PCsrc = 1;
    top->jalr = 0;
    top->ImmOp = 0;
    sample();
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->PC, 4u);
}
