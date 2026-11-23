#include "base_testbench.h"

class Registers : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->clk = 0;
        top->WE3 = 0;
        top->AD1 = 0;
        top->AD2 = 0;
        top->AD3 = 0;
        top->WD3 = 0;
    }
};

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
        WAVE_EXPECT_EQ(top->RD1, reg ? 0xabcd0000 + reg : 0u) << "rs1 " << reg;
        WAVE_EXPECT_EQ(top->RD2, reg == 31 ? 0u : 0xabcd0000 + 31 - reg) << "rs2 " << 31-reg;
        WAVE_EXPECT_EQ(top->a0, 0xabcd000au);
    }
    top->AD1 = 10;
    top->AD3 = 10;
    top->WE3 = 1;
    top->clk = 0;
    sample();
    WAVE_EXPECT_EQ(top->RD1, 0xabcd000au);
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->a0, 0xdeadbeefu);
}

TEST_F(Registers, WritesOnlyOnRisingEdge)
{
    top->AD1 = 10;
    top->AD2 = 10;
    top->AD3 = 10;
    top->WE3 = 1;
    top->WD3 = 7;
    sample();
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->RD1, 7u);
    top->WD3 = 99;
    sample();
    WAVE_EXPECT_EQ(top->RD1, 7u);
    top->clk = 0;
    sample();
    WAVE_EXPECT_EQ(top->RD2, 7u);
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->RD1, 99u);
    WAVE_EXPECT_EQ(top->RD2, 99u);
    WAVE_EXPECT_EQ(top->a0, 99u);
}
