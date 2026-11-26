#include "base_testbench.h"

class Registers : public BaseTestbench
{
protected:
    void clock(bool high)
    {
        top->clk = high;
        sample();
    }

    void write(unsigned reg, uint32_t value, bool enable = true)
    {
        top->clk = 0;
        top->we3 = enable;
        top->ad3 = reg;
        top->wd3 = value;
        sample();
        clock(1);
    }

    void read(unsigned first, unsigned second)
    {
        top->ad1 = first;
        top->ad2 = second;
        sample();
    }

    void initializeInputs() override
    {
        top->clk = 0;
        top->we3 = 0;
        top->ad1 = 0;
        top->ad2 = 0;
        top->ad3 = 0;
        top->wd3 = 0;
    }
};

// Check both read ports, write enable, the a0 output and the fixed-zero register.
TEST_F(Registers, WriteEnableReadPortsAndZero)
{
    // Exercise each address bit, a0, x0 and the highest address.
    write(0, 0xabcd0000u);
    write(1, 0xabcd0001u);
    write(2, 0xabcd0002u);
    write(4, 0xabcd0004u);
    write(8, 0xabcd0008u);
    write(16, 0xabcd0010u);
    write(10, 0xabcd000au);
    write(31, 0xabcd001fu);

    // Read each selected register through both ports.
    read(0, 31);
    WAVE_EXPECT_EQ(top->rd1, 0x00000000u);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd001fu);
    read(0, 0);
    WAVE_EXPECT_EQ(top->rd2, 0x00000000u);

    read(1, 31);
    WAVE_EXPECT_EQ(top->rd1, 0xabcd0001u);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd001fu);
    read(0, 1);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd0001u);

    read(2, 31);
    WAVE_EXPECT_EQ(top->rd1, 0xabcd0002u);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd001fu);
    read(0, 2);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd0002u);

    read(4, 31);
    WAVE_EXPECT_EQ(top->rd1, 0xabcd0004u);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd001fu);
    read(0, 4);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd0004u);

    read(8, 31);
    WAVE_EXPECT_EQ(top->rd1, 0xabcd0008u);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd001fu);
    read(0, 8);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd0008u);

    read(16, 31);
    WAVE_EXPECT_EQ(top->rd1, 0xabcd0010u);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd001fu);
    read(0, 16);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd0010u);

    read(10, 31);
    WAVE_EXPECT_EQ(top->rd1, 0xabcd000au);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd001fu);
    read(0, 10);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd000au);

    read(31, 0);
    WAVE_EXPECT_EQ(top->rd1, 0xabcd001fu);
    WAVE_EXPECT_EQ(top->rd2, 0x00000000u);
    read(0, 31);
    WAVE_EXPECT_EQ(top->rd2, 0xabcd001fu);

    // A disabled write preserves a0; an enabled write updates it.
    write(10, 99, false);
    WAVE_EXPECT_EQ(top->a0, 0xabcd000au);
    write(10, 99);
    WAVE_EXPECT_EQ(top->a0, 99u);
    write(0, 99);
    read(0, 0);
    WAVE_EXPECT_EQ(top->rd1, 0u);
    WAVE_EXPECT_EQ(top->rd2, 0u);
}

// Check that register writes occur only on a rising edge and appear on both read ports.
TEST_F(Registers, WritesOnlyOnRisingEdge)
{
    top->ad1 = 10;
    top->ad2 = 10;
    top->ad3 = 10;
    top->we3 = 1;
    top->wd3 = 7;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->rd1, 7u);
    top->wd3 = 99;
    sample();
    WAVE_EXPECT_EQ(top->rd1, 7u);
    clock(0);
    WAVE_EXPECT_EQ(top->rd2, 7u);
    clock(1);
    WAVE_EXPECT_EQ(top->rd1, 99u);
    WAVE_EXPECT_EQ(top->rd2, 99u);
    WAVE_EXPECT_EQ(top->a0, 99u);
}
