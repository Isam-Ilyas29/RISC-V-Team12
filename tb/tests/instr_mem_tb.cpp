#include "base_testbench.h"
#include <cstdlib>
#include <fstream>
#include <iomanip>

class Rom : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->a = 0;
    }
};

TEST_F(Rom, FetchesCounterInstructions)
{
    WAVE_ASSERT_EQ(std::system("./compile.sh asm/program.S"), 0);
    const unsigned words[] = {0x0ff00313, 0x00000513, 0x00000593, 0x00058513,
                              0x00158593, 0xfe659ce3, 0xfe0318e3};
    for (unsigned address : {24u, 0u, 20u, 4u, 16u, 8u, 12u}) {
        top->a = address;
        sample();
        WAVE_EXPECT_EQ(top->rd, words[address / 4]) << "address " << address;
    }
}

TEST_F(Rom, ByteOrderAndAddressLimits)
{
    std::ofstream program("../rtl/program.hex");
    ASSERT_TRUE(program.is_open());
    for (unsigned byte = 0; byte < 512; ++byte)
        program << std::hex << std::setw(2) << std::setfill('0') << (byte & 255) << '\n';
    program.close();
    ASSERT_FALSE(program.fail());
    for (unsigned address : {0u, 1u, 4u, 252u, 256u, 508u, 512u, 0xbfc00000u}) {
        top->a = address;
        sample();
        const unsigned first = address & 255;
        const unsigned expected = first | (((first + 1) & 255) << 8)
                                | (((first + 2) & 255) << 16) | (((first + 3) & 255) << 24);
        WAVE_EXPECT_EQ(top->rd, expected) << "address " << address;
    }
}
