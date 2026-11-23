#include "base_testbench.h"

class Memory : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->clk = 0;
        top->we = 0;
        top->a = 0;
        top->wd = 0;
    }

    uint32_t read(unsigned address)
    {
        top->a = address;
        sample();
        return top->rd;
    }

    void write(unsigned address, uint32_t value)
    {
        top->clk = 0;
        top->a = address;
        top->wd = value;
        top->we = 1;
        sample();
        top->clk = 1;
        sample();
        top->we = 0;
        top->clk = 0;
        sample();
    }
};

TEST_F(Memory, PreloadAndLittleEndianRead)
{
    WAVE_EXPECT_EQ(read(0), 0x12345678u);
    WAVE_EXPECT_EQ(read(4), 0x90abcdefu);
    WAVE_EXPECT_EQ(read(1), 0xef123456u);
}

TEST_F(Memory, WriteEnableAndClockEdges)
{
    WAVE_EXPECT_EQ(read(0), 0x12345678u);
    top->we = 1;
    top->wd = 0xa5c39e17u;
    sample();
    WAVE_EXPECT_EQ(top->rd, 0x12345678u);
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->rd, 0xa5c39e17u);
    top->wd = 0;
    sample();
    WAVE_EXPECT_EQ(top->rd, 0xa5c39e17u);
    top->clk = 0;
    sample();
    WAVE_EXPECT_EQ(top->rd, 0xa5c39e17u);
    top->we = 0;
    top->clk = 1;
    sample();
    WAVE_EXPECT_EQ(top->rd, 0xa5c39e17u);
}

TEST_F(Memory, IndependentWordsAndAddressLimits)
{
    write(0, 0x44332211u);
    write(4, 0x88776655u);
    write(508, 0xdeadbeefu);
    WAVE_EXPECT_EQ(read(0), 0x44332211u);
    WAVE_EXPECT_EQ(read(4), 0x88776655u);
    WAVE_EXPECT_EQ(read(508), 0xdeadbeefu);
    WAVE_EXPECT_EQ(read(1), 0x55443322u);
    WAVE_EXPECT_EQ(read(512), 0x44332211u);
}
