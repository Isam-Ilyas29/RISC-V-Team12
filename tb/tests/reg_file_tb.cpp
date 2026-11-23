#include "base_testbench.h"

class Registers : public BaseTestbench
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
        top->WE3 = 0;
        top->AD1 = 0;
        top->AD2 = 0;
        top->AD3 = 0;
        top->WD3 = 0;
    }
};

// Check both read ports, write enable, the a0 output and the fixed-zero register.
TEST_F(Registers, WriteEnableReadPortsAndZero)
{
    for (unsigned reg = 0; reg < 32; ++reg) {
        top->clk = 0;
        top->WE3 = 1;
        top->AD3 = reg;
        top->WD3 = 0xabcd0000 + reg;
        sample();
        clock(1);
    }
    for (unsigned reg = 0; reg < 32; ++reg) {
        top->clk = 0;
        top->WE3 = 0;
        top->AD3 = reg;
        top->WD3 = 0xdeadbeef;
        top->AD1 = reg;
        top->AD2 = 31 - reg;
        sample();
        clock(1);
        const unsigned rs2 = 31 - reg;
        const unsigned expected1 = reg == 0 ? 0u : 0xabcd0000u + reg;
        const unsigned expected2 = rs2 == 0 ? 0u : 0xabcd0000u + rs2;
        WAVE_EXPECT_EQ(top->RD1, expected1) << "rs1 " << reg;
        WAVE_EXPECT_EQ(top->RD2, expected2) << "rs2 " << rs2;
        WAVE_EXPECT_EQ(top->a0, 0xabcd000au);
    }
    top->AD1 = 10;
    top->AD3 = 10;
    top->WE3 = 1;
    clock(0);
    WAVE_EXPECT_EQ(top->RD1, 0xabcd000au);
    clock(1);
    WAVE_EXPECT_EQ(top->a0, 0xdeadbeefu);
}

// Check that register writes occur only on a rising edge and appear on both read ports.
TEST_F(Registers, WritesOnlyOnRisingEdge)
{
    top->AD1 = 10;
    top->AD2 = 10;
    top->AD3 = 10;
    top->WE3 = 1;
    top->WD3 = 7;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->RD1, 7u);
    top->WD3 = 99;
    sample();
    WAVE_EXPECT_EQ(top->RD1, 7u);
    clock(0);
    WAVE_EXPECT_EQ(top->RD2, 7u);
    clock(1);
    WAVE_EXPECT_EQ(top->RD1, 99u);
    WAVE_EXPECT_EQ(top->RD2, 99u);
    WAVE_EXPECT_EQ(top->a0, 99u);
}
