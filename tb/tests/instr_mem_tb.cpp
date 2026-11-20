#include "base_testbench.h"
#include <cstdlib>

using Rom = BaseTestbench;

TEST_F(Rom, FetchesCounterInstructions)
{
    ASSERT_EQ(std::system("./compile.sh asm/program.S"), 0);
    const unsigned words[] = {0x0ff00313, 0x00000513, 0x00000593, 0x00058513,
                              0x00158593, 0xfe659ce3, 0xfe0318e3};
    for (unsigned address : {24u, 0u, 20u, 4u, 16u, 8u, 12u}) {
        top->a = address;
        sample();
        EXPECT_EQ(top->rd, words[address / 4]) << "address " << address;
    }
}