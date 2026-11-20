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
TEST_F(Alu, ShiftBoundaries)
{
    for (unsigned value : {0u, 1u, 0x7fffffffu, 0x80000000u, 0xfffffff8u, 0xffffffffu})
        for (unsigned amount : {0u, 1u, 2u, 4u, 8u, 16u, 31u, 32u, 33u, 0xffffffffu})
            for (unsigned mode : {2u, 3u, 4u}) {
                unsigned expected = value;
                for (unsigned bit = 0; bit < (amount & 31); ++bit) {
                    if (mode == 2)
                        expected *= 2u;
                    else {
                        const unsigned sign = mode == 4 ? expected & 0x80000000u : 0;
                        expected = expected / 2u + sign;
                    }
                }
                top->alu1 = value;
                top->alu2 = amount;
                top->alu_ctrl = mode;
                sample();
                EXPECT_EQ(top->alu_out, expected)
                    << "value " << value << ", amount " << amount << ", mode " << mode;
            }
}
