#include "base_testbench.h"

using Alu = BaseTestbench;

TEST_F(Alu, AddAndCompare)
{
    for (unsigned a : {0u, 1u, 0x80000000u, 0xffffffffu})
        for (unsigned b : {0u, 1u, 0x80000000u, 0xffffffffu})
            for (unsigned mode : {0u, 1u}) {
                top->alu1 = a;
                top->alu2 = b;
                top->alu_ctrl = mode;
                sample();
                EXPECT_EQ(top->eq, a == b);
                if (mode == 0)
                    EXPECT_EQ(top->alu_out, a + b);
            }
}