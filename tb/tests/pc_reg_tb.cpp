#include "base_testbench.h"

class PcRegister : public BaseTestbench
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
        top->next_PC = 0;
    }
};

// Check that the PC loads or resets on a rising edge, not on a falling edge.
TEST_F(PcRegister, LoadsOnlyOnRisingEdge)
{
    top->clk = 0;
    top->rst = 1;
    top->next_PC = 0x104;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->PC, 0u);
    top->rst = 0;
    clock(0);
    WAVE_EXPECT_EQ(top->PC, 0u);
    clock(1);
    WAVE_EXPECT_EQ(top->PC, 0x104u);
    top->clk = 0;
    top->rst = 1;
    top->next_PC = 8;
    sample();
    WAVE_EXPECT_EQ(top->PC, 0x104u);
    clock(1);
    WAVE_EXPECT_EQ(top->PC, 0u);
}