#include "base_testbench.h"

class Immediate : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->imm = 0;
        top->imm_src = 0;
    }
};

TEST_F(Immediate, IAndBBoundaries)
{
    for (unsigned bits : {0u, 1u, 2u, 4u, 8u, 16u, 32u, 64u, 128u,
                          256u, 512u, 1024u, 2047u, 2048u, 2049u, 4094u, 4095u}) {
        int value = bits < 2048 ? int(bits) : int(bits) - 4096;
        top->imm_src = 0;
        top->imm = (bits << 13) | 0x15ab;
        sample();
        WAVE_ASSERT_EQ(top->imm_op, unsigned(value));

        unsigned offset = bits << 1;
        unsigned instruction = ((offset >> 12) << 31) | (((offset >> 5) & 63) << 25)
                             | (0xabc << 13) | (((offset >> 1) & 15) << 8)
                             | (((offset >> 11) & 1) << 7);
        top->imm_src = 1;
        top->imm = instruction >> 7;
        sample();
        WAVE_ASSERT_EQ(top->imm_op, unsigned(value * 2));
    }
}
TEST_F(Immediate, JumpOffsets)
{
    for (int offset : {-1048576, -4096, -2048, -4, 0, 2, 4, 2048, 4096, 1048574}) {
        const unsigned bits = static_cast<unsigned>(offset) & 0x1fffff;
        const unsigned instruction = ((bits >> 20) << 31) | (((bits >> 1) & 0x3ff) << 21)
                                   | (((bits >> 11) & 1) << 20) | (((bits >> 12) & 0xff) << 12);
        top->imm = instruction >> 7;
        top->imm_src = 3;
        sample();
        WAVE_EXPECT_EQ(top->imm_op, static_cast<unsigned>(offset));
    }
}

TEST_F(Immediate, StoreOffsets)
{
    for (int offset : {-2048, -33, -32, -1, 0, 1, 16, 31, 32, 1024, 2047}) {
        const unsigned bits = static_cast<unsigned>(offset) & 0xfff;
        const unsigned instruction = ((bits >> 5) << 25) | (0xabc << 13)
                                   | ((bits & 31) << 7) | 0x23;
        top->imm_src = 2;
        top->imm = instruction >> 7;
        sample();
        WAVE_EXPECT_EQ(top->imm_op, static_cast<unsigned>(offset)) << "offset " << offset;
    }
}
