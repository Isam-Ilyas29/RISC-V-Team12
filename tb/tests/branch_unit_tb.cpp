#include "base_testbench.h"

class Branch : public BaseTestbench
{
protected:
    static int64_t signedValue(uint32_t value)
    {
        if (value < 0x80000000u)
            return value;
        return int64_t(value) - 0x100000000LL;
    }

    void initializeInputs() override
    {
        top->rs1_data = 0;
        top->rs2_data = 0;
        top->funct3 = 0;
    }
};

// Check every branch condition with equal, positive and negative operands.
TEST_F(Branch, AllConditions)
{
    const unsigned values[] = {0, 1, 2, 0x7fffffff, 0x80000000, 0xfffffffe, 0xffffffff};
    for (unsigned a : values)
        for (unsigned b : values)
            for (unsigned condition = 0; condition < 8; ++condition) {
                const int64_t signed_a = signedValue(a);
                const int64_t signed_b = signedValue(b);
                const bool expected[] = {a == b, a != b, false, false,
                                         signed_a < signed_b, signed_a >= signed_b, a < b, a >= b};
                top->rs1_data = a;
                top->rs2_data = b;
                top->funct3 = condition;
                sample();
                WAVE_EXPECT_EQ(top->branch_taken, expected[condition])
                    << "rs1 " << a << ", rs2 " << b << ", funct3 " << condition;
            }
}
