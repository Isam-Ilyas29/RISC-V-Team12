#include "base_testbench.h"
#include <cstdlib>
#include <fstream>
#include <iomanip>

class Rom : public BaseTestbench
{
protected:
    unsigned read(unsigned address)
    {
        top->a = address;
        sample();
        return top->rd;
    }

    void initializeInputs() override
    {
        top->a = 0;
    }
};

// Check that instruction memory returns the expected words from the counter program.
TEST_F(Rom, FetchesCounterInstructions)
{
    WAVE_ASSERT_EQ(std::system("bash compile.sh asm/program.S"), 0);
    WAVE_EXPECT_EQ(read(24), 0xfe0318e3u);
    WAVE_EXPECT_EQ(read(0), 0x0ff00313u);
    WAVE_EXPECT_EQ(read(20), 0xfe659ce3u);
    WAVE_EXPECT_EQ(read(4), 0x00000513u);
    WAVE_EXPECT_EQ(read(16), 0x00158593u);
    WAVE_EXPECT_EQ(read(8), 0x00000593u);
    WAVE_EXPECT_EQ(read(12), 0x00058513u);
}

// Check instruction byte order and addresses near the ends of the memory.
TEST_F(Rom, ByteOrderAndAddressLimits)
{
    std::ofstream program("program.hex");
    ASSERT_TRUE(program.is_open());
    for (unsigned byte = 0; byte < 512; ++byte)
        program << std::hex << std::setw(2) << std::setfill('0') << (byte & 255) << '\n';
    program.close();
    ASSERT_FALSE(program.fail());
    WAVE_EXPECT_EQ(read(0x00000000u), 0x03020100u);
    WAVE_EXPECT_EQ(read(0x00000001u), 0x04030201u);
    WAVE_EXPECT_EQ(read(0x00000004u), 0x07060504u);
    WAVE_EXPECT_EQ(read(0x000000fcu), 0xfffefdfcu);
    WAVE_EXPECT_EQ(read(0x00000100u), 0x03020100u);
    WAVE_EXPECT_EQ(read(0x000001fcu), 0xfffefdfcu);
    WAVE_EXPECT_EQ(read(0x00000200u), 0x03020100u);
    WAVE_EXPECT_EQ(read(0xbfc00000u), 0x03020100u);
}
