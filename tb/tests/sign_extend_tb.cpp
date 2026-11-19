#include "base_testbench.h"

using Immediate = BaseTestbench;

TEST_F(Immediate, AllIAndBImmediates)
{
    for (unsigned bits = 0; bits < 4096; ++bits) {
        int value = bits < 2048 ? int(bits) : int(bits) - 4096;
        top->imm_src = 0;
        top->imm = (bits << 13) | 0x15ab;
        sample();
        ASSERT_EQ(top->imm_op, unsigned(value));

        unsigned offset = bits << 1;
        unsigned instruction = ((offset >> 12) << 31) | (((offset >> 5) & 63) << 25)
                             | (0xabc << 13) | (((offset >> 1) & 15) << 8)
                             | (((offset >> 11) & 1) << 7);
        top->imm_src = 1;
        top->imm = instruction >> 7;
        sample();
        ASSERT_EQ(top->imm_op, unsigned(value * 2));
    }
}