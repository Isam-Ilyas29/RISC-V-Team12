#include "base_testbench.h"

using Registers = BaseTestbench;

TEST_F(Registers, WriteEnableReadPortsAndZero)
{
    for (unsigned reg = 0; reg < 32; ++reg) {
        top->clk = 0;
        top->WE3 = 1;
        top->AD3 = reg;
        top->WD3 = 0xabcd0000 + reg;
        sample();
        top->clk = 1;
        sample();
    }
    for (unsigned reg = 0; reg < 32; ++reg) {
        top->clk = 0;
        top->WE3 = 0;
        top->AD3 = reg;
        top->WD3 = 0xdeadbeef;
        top->AD1 = reg;
        top->AD2 = 31 - reg;
        sample();
        top->clk = 1;
        sample();
        EXPECT_EQ(top->RD1, reg ? 0xabcd0000 + reg : 0u) << "rs1 " << reg;
        EXPECT_EQ(top->RD2, reg == 31 ? 0u : 0xabcd0000 + 31 - reg) << "rs2 " << 31-reg;
        EXPECT_EQ(top->a0, 0xabcd000au);
    }
    top->AD1 = 10;
    top->AD3 = 10;
    top->WE3 = 1;
    top->clk = 0;
    sample();
    EXPECT_EQ(top->RD1, 0xabcd000au);
    top->clk = 1;
    sample();
    EXPECT_EQ(top->a0, 0xdeadbeefu);
}