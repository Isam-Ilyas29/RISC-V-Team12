#include "base_testbench.h"

class PcRegister : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->clk = 0;
        top->rst = 0;
        top->next_PC = 0;
    }
};

TEST_F(PcRegister, LoadsOnlyOnRisingEdge)
{
    top->clk = 0;
    top->rst = 1;
    top->next_PC = 0x104;
    sample();
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->PC, 0u);
    top->rst = 0;
    top->clk = 0;
    sample();
    WAVE_EXPECT_EQ(top->PC, 0u);
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->PC, 0x104u);
    top->clk = 0;
    top->rst = 1;
    top->next_PC = 8;
    sample();
    WAVE_EXPECT_EQ(top->PC, 0x104u);
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->PC, 0u);
}