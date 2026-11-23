#include "alu_testbench.h"
#include "alu_controls.h"

class Alu : public AluWaveTestbench
{
protected:
    void load(unsigned a, unsigned b, unsigned operation)
    {
        top->alu1 = a;
        top->alu2 = b;
        top->alu_ctrl = operation;
        sample();
    }

    void initializeInputs() override
    {
        top->alu1 = 0;
        top->alu2 = 0;
        top->alu_ctrl = 0;
    }
};

TEST_F(Alu, AddSubtractAndEquality)
{
    for (unsigned a : {0u, 1u, 0x80000000u, 0xffffffffu})
        for (unsigned b : {0u, 1u, 0x80000000u, 0xffffffffu})
            for (unsigned mode : {ADD, SUB}) {
                load(a, b, mode);
                WAVE_EXPECT_EQ(top->eq, a == b);
                if (mode == ADD)
                    WAVE_EXPECT_EQ(top->alu_out, a + b);
                else
                    WAVE_EXPECT_EQ(top->alu_out, a - b);
            }
}

TEST_F(Alu, ShiftBoundaries)
{
    for (unsigned value : {0u, 1u, 0x7fffffffu, 0x80000000u, 0xfffffff8u, 0xffffffffu})
        for (unsigned amount : {0u, 1u, 2u, 4u, 8u, 16u, 31u, 32u, 33u, 0xffffffffu})
            for (unsigned mode : {SLL, SRL, SRA}) {
                unsigned expected = value;
                for (unsigned bit = 0; bit < (amount & 31); ++bit) {
                    if (mode == SLL)
                        expected *= 2u;
                    else {
                        const unsigned sign = mode == SRA ? expected & 0x80000000u : 0;
                        expected = expected / 2u + sign;
                    }
                }

                load(value, amount, mode);
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
        {SUB, 10, 3, 7}, {SUB, 3, 10, 0xfffffff9u},
        {AND, 0xaaaaaaaau, 0x55555555u, 0},
        {OR, 0xaaaaaaaau, 0x55555555u, 0xffffffffu},
        {XOR, 0xaaaaaaaau, 0xaaaaaaaau, 0},
        {XOR, 0xaaaaaaaau, 0x55555555u, 0xffffffffu},
        {SLT, 0xffffffffu, 1, 1}, {SLT, 1, 0xffffffffu, 0}, {SLT, 1, 1, 0},
        {SLTU, 0xffffffffu, 1, 0}, {SLTU, 1, 0xffffffffu, 1}, {SLTU, 1, 1, 0}
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
