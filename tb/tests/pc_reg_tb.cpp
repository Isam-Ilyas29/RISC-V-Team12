#include "base_testbench.h"

using PcRegister = BaseTestbench;

TEST_F(PcRegister, LoadsOnlyOnRisingEdge)
{
    top->clk = 0;
    top->rst = 1;
    top->next_PC = 0x104;
    sample();
    top->clk = 1;
    sample();
    EXPECT_EQ(top->PC, 0u);
    top->rst = 0;
    top->clk = 0;
    sample();
    EXPECT_EQ(top->PC, 0u);
    top->clk = 1;
    sample();
    EXPECT_EQ(top->PC, 0x104u);
    top->clk = 0;
    top->rst = 1;
    top->next_PC = 8;
    sample();
    EXPECT_EQ(top->PC, 0x104u);
    top->clk = 1;
    sample();
    EXPECT_EQ(top->PC, 0u);
}