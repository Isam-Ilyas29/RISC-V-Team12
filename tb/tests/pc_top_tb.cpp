#include "base_testbench.h"

using ProgramCounter = BaseTestbench;

TEST_F(ProgramCounter, FallThroughAndRelativeBranches)
{
    top->clk = 0;
    top->rst = 1;
    top->PCsrc = 0;
    top->ImmOp = 16;
    sample();
    top->clk = 1;
    sample();
    EXPECT_EQ(top->PC, 0u);
    top->rst = 0;
    unsigned pc = 0;
    for (int displacement : {0, 16, -8, -24, 0}) {
        top->clk = 0;
        top->PCsrc = displacement != 0;
        top->ImmOp = displacement;
        sample();
        EXPECT_EQ(top->PC, pc);
        pc += displacement ? displacement : 4;
        top->clk = 1;
        sample();
        EXPECT_EQ(top->PC, pc);
    }
}