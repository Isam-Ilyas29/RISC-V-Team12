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
    WAVE_ASSERT_EQ(std::system("./compile.sh asm/program.S"), 0);
    const unsigned words[] = {0x0ff00313, 0x00000513, 0x00000593, 0x00058513,
                              0x00158593, 0xfe659ce3, 0xfe0318e3};
    for (unsigned address : {24u, 0u, 20u, 4u, 16u, 8u, 12u}) {
        WAVE_EXPECT_EQ(read(address), words[address / 4]) << "address " << address;
    }
}

// Check instruction byte order and addresses near the ends of the memory.
TEST_F(Rom, ByteOrderAndAddressLimits)
{
    std::ofstream program("../rtl/program.hex");
    ASSERT_TRUE(program.is_open());
    for (unsigned byte = 0; byte < 512; ++byte)
        program << std::hex << std::setw(2) << std::setfill('0') << (byte & 255) << '\n';
    program.close();
    ASSERT_FALSE(program.fail());
    for (unsigned address : {0u, 1u, 4u, 252u, 256u, 508u, 512u, 0xbfc00000u}) {
        const unsigned first = address & 255;
        unsigned expected = 0;
        for (unsigned byte = 0; byte < 4; ++byte)
            expected |= ((first + byte) & 255) << (8 * byte);
        WAVE_EXPECT_EQ(read(address), expected) << "address " << address;
    }
}
