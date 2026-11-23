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
        top->alu_ctrl = ADD;
    }
};

// ADD/SUB: zero, one, the sign bit and all ones, including wraparound.
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

// SUB: small positive operands giving positive and negative results.
TEST_F(Alu, SubtractSmallValues)
{
    load(10, 3, SUB);
    WAVE_EXPECT_EQ(top->alu_out, 7u);
    WAVE_EXPECT_EQ(top->eq, 0u);
    load(3, 10, SUB);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff9u);
    WAVE_EXPECT_EQ(top->eq, 0u);
}

// Shifts: signed limits and -8; amounts 0-31 plus 32, 33 and all ones to check masking.
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

// Unused codes 10-15 must return zero after a valid operation.
TEST_F(Alu, UnsupportedControlsDoNotRetainPreviousResult)
{
    top->alu1 = 0x12345678;
    top->alu2 = 0x12345678;
    for (unsigned mode : {10u, 11u, 12u, 13u, 14u, 15u}) {
        top->alu_ctrl = ADD;
        sample();
        WAVE_EXPECT_EQ(top->alu_out, 0x2468acf0u);
        top->alu_ctrl = mode;
        sample();
        WAVE_EXPECT_EQ(top->alu_out, 0u);
        WAVE_EXPECT_EQ(top->eq, 1u);
    }
}

// AND: zero, all ones, alternating bits and high/low-bit patterns.
TEST_F(Alu, BitwiseAnd)
{
    load(0, 0xffffffffu, AND);
    WAVE_EXPECT_EQ(top->alu_out, 0);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0xffffffffu, 0xffffffffu, AND);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0xaaaaaaaau, 0x55555555u, AND);
    WAVE_EXPECT_EQ(top->alu_out, 0);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000001u, 0x80000000u, AND);
    WAVE_EXPECT_EQ(top->alu_out, 0x80000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);
}

// OR: zero, all ones, alternating bits and high/low-bit patterns.
TEST_F(Alu, BitwiseOr)
{
    load(0, 0, OR);
    WAVE_EXPECT_EQ(top->alu_out, 0);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0, 0xffffffffu, OR);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0xaaaaaaaau, 0x55555555u, OR);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000000u, 1, OR);
    WAVE_EXPECT_EQ(top->alu_out, 0x80000001u);
    WAVE_EXPECT_EQ(top->eq, 0u);
}

// XOR: zero, all ones, alternating bits and high/low-bit patterns.
TEST_F(Alu, BitwiseXor)
{
    load(0, 0xffffffffu, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0xffffffffu, 0xffffffffu, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0xaaaaaaaau, 0xaaaaaaaau, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0xaaaaaaaau, 0x55555555u, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000001u, 1, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0x80000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);
}

// SLT: -2^31, -1, 0, 1 and 2^31-1, including equal operands.
TEST_F(Alu, SignedLessThan)
{
    const int64_t values[] = {-2147483648LL, -1, 0, 1, 2147483647LL};
    for (int64_t a : values)
        for (int64_t b : values) {
            load(static_cast<uint32_t>(a), static_cast<uint32_t>(b), SLT);
            WAVE_EXPECT_EQ(top->alu_out, a < b) << "a " << a << ", b " << b;
            WAVE_EXPECT_EQ(top->eq, a == b);
        }
}

// SLTU: 0 to 2^32-1, sampling both sides of the sign-bit boundary.
TEST_F(Alu, UnsignedLessThan)
{
    const uint32_t values[] = {0, 1, 0x7fffffffu, 0x80000000u, 0xffffffffu};
    for (uint32_t a : values)
        for (uint32_t b : values) {
            load(a, b, SLTU);
            WAVE_EXPECT_EQ(top->alu_out, a < b) << "a " << a << ", b " << b;
            WAVE_EXPECT_EQ(top->eq, a == b);
        }
}
