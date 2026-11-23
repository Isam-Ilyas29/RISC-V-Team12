#include "base_testbench.h"

class Memory : public BaseTestbench
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
        top->we = 0;
        top->data_ctrl = 0b010;
        top->a = 0;
        top->wd = 0;
    }

    uint32_t read(unsigned address, unsigned mode = 0b010)
    {
        top->a = address;
        top->data_ctrl = mode;
        sample();
        return top->rd;
    }

    void write(unsigned address, uint32_t value, unsigned mode = 0b010)
    {
        top->clk = 0;
        top->a = address;
        top->wd = value;
        top->data_ctrl = mode;
        top->we = 1;
        sample();
        clock(1);
        top->we = 0;
        clock(0);
    }
};

// Check the initial memory data and the order of bytes within a word.
TEST_F(Memory, PreloadAndLittleEndianRead)
{
    WAVE_EXPECT_EQ(read(0), 0x12345678u);
    WAVE_EXPECT_EQ(read(4), 0x90abcdefu);
    WAVE_EXPECT_EQ(read(1), 0xef123456u);
}

// Check that memory writes happen only on a rising edge with write enable high.
TEST_F(Memory, WriteEnableAndClockEdges)
{
    WAVE_EXPECT_EQ(read(0), 0x12345678u);
    top->we = 1;
    top->wd = 0xa5c39e17u;
    sample();
    WAVE_EXPECT_EQ(top->rd, 0x12345678u);
    clock(1);
    WAVE_EXPECT_EQ(top->rd, 0xa5c39e17u);
    top->wd = 0;
    sample();
    WAVE_EXPECT_EQ(top->rd, 0xa5c39e17u);
    clock(0);
    WAVE_EXPECT_EQ(top->rd, 0xa5c39e17u);
    top->we = 0;
    clock(1);
    WAVE_EXPECT_EQ(top->rd, 0xa5c39e17u);
}

// Check separate memory locations, the last word and low-address-bit aliasing.
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

// LB/LBU: 0x7f, 0x80 and 0xff check positive values and the byte sign bit.
TEST_F(Memory, ByteLoads)
{
    write(0, 0x00ff807fu);

    WAVE_EXPECT_EQ(read(0, 0b000), 0x7fu);
    WAVE_EXPECT_EQ(read(1, 0b000), 0xffffff80u);
    WAVE_EXPECT_EQ(read(2, 0b000), 0xffffffffu);

    WAVE_EXPECT_EQ(read(0, 0b100), 0x7fu);
    WAVE_EXPECT_EQ(read(1, 0b100), 0x80u);
    WAVE_EXPECT_EQ(read(2, 0b100), 0xffu);
}

// LH/LHU: 0x7fff, 0x8000 and 0xffff check the halfword sign boundary.
TEST_F(Memory, HalfwordLoads)
{
    write(0, 0x80007fffu);
    write(4, 0x0000ffffu);

    WAVE_EXPECT_EQ(read(0, 0b001), 0x7fffu);
    WAVE_EXPECT_EQ(read(2, 0b001), 0xffff8000u);
    WAVE_EXPECT_EQ(read(4, 0b001), 0xffffffffu);

    WAVE_EXPECT_EQ(read(0, 0b101), 0x7fffu);
    WAVE_EXPECT_EQ(read(2, 0b101), 0x8000u);
    WAVE_EXPECT_EQ(read(4, 0b101), 0xffffu);
}

// SB/SH: only the selected byte or halfword should change.
TEST_F(Memory, PartialStores)
{
    write(0, 0x44332211u);

    // SB at byte 1 stores the low eight bits and preserves its neighbours.
    write(1, 0xffffffaau, 0b000);
    WAVE_EXPECT_EQ(read(0), 0x4433aa11u);

    // SH at byte 2 stores the low sixteen bits.
    write(2, 0x1234beefu, 0b001);
    WAVE_EXPECT_EQ(read(0), 0xbeefaa11u);
}
