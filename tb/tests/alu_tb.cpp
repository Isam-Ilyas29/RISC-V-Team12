#include "alu_testbench.h"

class Alu : public AluWaveTestbench
{
protected:
    void initializeInputs() override
    {
        top->alu1 = 0;
        top->alu2 = 0;
        top->alu_ctrl = 0;
    }
};

TEST_F(Alu, AddAndCompare)
{
    for (unsigned a : {0u, 1u, 0x80000000u, 0xffffffffu})
        for (unsigned b : {0u, 1u, 0x80000000u, 0xffffffffu})
            for (unsigned mode : {0u, 1u}) {
                top->alu1 = a;
                top->alu2 = b;
                top->alu_ctrl = mode;
                sample();
                WAVE_EXPECT_EQ(top->eq, a == b);
                if (mode == 0)
                    WAVE_EXPECT_EQ(top->alu_out, a + b);
            }
}

TEST_F(Alu, ShiftBoundaries)
{
    for (unsigned value : {0u, 1u, 0x7fffffffu, 0x80000000u, 0xfffffff8u, 0xffffffffu})
        for (unsigned amount : {0u, 1u, 2u, 4u, 8u, 16u, 31u, 32u, 33u, 0xffffffffu})
            for (unsigned mode : {7u, 8u, 9u}) {
                unsigned expected = value;
                for (unsigned bit = 0; bit < (amount & 31); ++bit) {
                    if (mode == 7)
                        expected *= 2u;
                    else {
                        const unsigned sign = mode == 9 ? expected & 0x80000000u : 0;
                        expected = expected / 2u + sign;
                    }
                }

                //assertion check
                top->alu1 = value;
                top->alu2 = amount;
                top->alu_ctrl = mode;
                sample();
                WAVE_EXPECT_EQ(top->alu_out, expected)
                    << "value " << value << ", amount " << amount << ", mode " << mode;
            }
}


// Test that unsupported control codes do not retain the previous result
TEST_F(Alu, UnsupportedControlsDoNotRetainPreviousResult)
{
    top->alu1 = 0x12345678;
    top->alu2 = 0x12345678;
    for (unsigned mode : {10u, 11u, 15u}) {
        top->alu_ctrl = 0;
        sample();
        WAVE_EXPECT_EQ(top->alu_out, 0x2468acf0u);
        top->alu_ctrl = mode;
        sample();
        WAVE_EXPECT_EQ(top->alu_out, 0u);
        WAVE_EXPECT_EQ(top->eq, 1u);
    }
}

TEST_F(Alu, ArithmeticLogicAndComparisons)
{
    struct Case { unsigned mode, a, b, expected; };
    const Case cases[] = {
        {1, 10, 3, 7}, {1, 3, 10, 0xfffffff9u},
        {2, 0xaaaaaaaau, 0x55555555u, 0},
        {3, 0xaaaaaaaau, 0x55555555u, 0xffffffffu},
        {4, 0xaaaaaaaau, 0xaaaaaaaau, 0},
        {4, 0xaaaaaaaau, 0x55555555u, 0xffffffffu},
        {5, 0xffffffffu, 1, 1}, {5, 1, 0xffffffffu, 0}, {5, 1, 1, 0},
        {6, 0xffffffffu, 1, 0}, {6, 1, 0xffffffffu, 1}, {6, 1, 1, 0}
    };
    for (const auto &c : cases) {
        top->alu1 = c.a;
        top->alu2 = c.b;
        top->alu_ctrl = c.mode;
        sample();
        WAVE_EXPECT_EQ(top->alu_out, c.expected) << "mode " << c.mode;
        WAVE_EXPECT_EQ(top->eq, c.a == c.b);
    }
}
