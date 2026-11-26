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

// Positive and negative operands, zero, signed overflow and wraparound.
TEST_F(Alu, Add)
{
    load(0x0000000au, 0x00000005u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x0000000fu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x00000000u, 0x00000000u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0xffffffffu, 0x00000001u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x7fffffffu, 0x00000001u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x80000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000000u, 0xffffffffu, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x7fffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

}

// Positive, negative and equal operands, including wraparound.
TEST_F(Alu, Subtract)
{
    load(0x0000000au, 0x00000003u, SUB);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000007u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x00000003u, 0x0000000au, SUB);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff9u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x00000005u, 0x00000005u, SUB);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0x00000000u, 0x00000001u, SUB);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000000u, 0x00000001u, SUB);
    WAVE_EXPECT_EQ(top->alu_out, 0x7fffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

}

// Zero, all ones, alternating bits and isolated end bits.
TEST_F(Alu, BitwiseAnd)
{
    load(0x00000000u, 0xffffffffu, AND);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0xffffffffu, 0xffffffffu, AND);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0xaaaaaaaau, 0x55555555u, AND);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000001u, 0x80000000u, AND);
    WAVE_EXPECT_EQ(top->alu_out, 0x80000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

}

// Zero, all ones and complementary bit patterns.
TEST_F(Alu, BitwiseOr)
{
    load(0x00000000u, 0x00000000u, OR);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0x00000000u, 0xffffffffu, OR);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0xaaaaaaaau, 0x55555555u, OR);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000000u, 0x00000001u, OR);
    WAVE_EXPECT_EQ(top->alu_out, 0x80000001u);
    WAVE_EXPECT_EQ(top->eq, 0u);

}

// Equal and complementary operands, plus isolated end bits.
TEST_F(Alu, BitwiseXor)
{
    load(0x00000000u, 0xffffffffu, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0xaaaaaaaau, 0xaaaaaaaau, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0xaaaaaaaau, 0x55555555u, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000001u, 0x00000001u, XOR);
    WAVE_EXPECT_EQ(top->alu_out, 0x80000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

}

// Equality, operand order and the signed 32-bit limits.
TEST_F(Alu, SignedLessThan)
{
    load(0x00000001u, 0x00000001u, SLT);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0xffffffffu, 0x00000001u, SLT);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000001u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x00000001u, 0xffffffffu, SLT);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x80000000u, 0x7fffffffu, SLT);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000001u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x7fffffffu, 0x80000000u, SLT);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

}

// Equality, zero and unsigned comparisons across the sign bit.
TEST_F(Alu, UnsignedLessThan)
{
    load(0x00000001u, 0x00000001u, SLTU);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    load(0x00000000u, 0x00000001u, SLTU);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000001u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x00000001u, 0x00000000u, SLTU);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x7fffffffu, 0x80000000u, SLTU);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000001u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0xffffffffu, 0x00000001u, SLTU);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);
    WAVE_EXPECT_EQ(top->eq, 0u);

    load(0x00000001u, 0xffffffffu, SLTU);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000001u);
    WAVE_EXPECT_EQ(top->eq, 0u);

}

// Zero, signed data and shifts of 0, 1, 31, 32, 33 and all ones.
TEST_F(Alu, ShiftLeftLogical)
{
    load(0x00000000u, 0x00000001u, SLL);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);

    load(0xfffffff8u, 0x00000000u, SLL);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff8u);

    load(0xfffffff8u, 0x00000001u, SLL);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff0u);

    load(0x80000000u, 0x0000001fu, SLL);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);

    load(0x00000001u, 0x0000001fu, SLL);
    WAVE_EXPECT_EQ(top->alu_out, 0x80000000u);

    load(0xfffffff8u, 0x00000020u, SLL);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff8u);

    load(0xfffffff8u, 0x00000021u, SLL);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff0u);

    load(0xfffffff8u, 0xffffffffu, SLL);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);

}

// Zero, signed data and shifts of 0, 1, 31, 32, 33 and all ones.
TEST_F(Alu, ShiftRightLogical)
{
    load(0x00000000u, 0x00000001u, SRL);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);

    load(0xfffffff8u, 0x00000000u, SRL);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff8u);

    load(0xfffffff8u, 0x00000001u, SRL);
    WAVE_EXPECT_EQ(top->alu_out, 0x7ffffffcu);

    load(0x80000000u, 0x0000001fu, SRL);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000001u);

    load(0x00000001u, 0x0000001fu, SRL);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);

    load(0xfffffff8u, 0x00000020u, SRL);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff8u);

    load(0xfffffff8u, 0x00000021u, SRL);
    WAVE_EXPECT_EQ(top->alu_out, 0x7ffffffcu);

    load(0xfffffff8u, 0xffffffffu, SRL);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000001u);

}

// Zero, signed data and shifts of 0, 1, 31, 32, 33 and all ones.
TEST_F(Alu, ShiftRightArithmetic)
{
    load(0x00000000u, 0x00000001u, SRA);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);

    load(0xfffffff8u, 0x00000000u, SRA);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff8u);

    load(0xfffffff8u, 0x00000001u, SRA);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffffcu);

    load(0x80000000u, 0x0000001fu, SRA);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);

    load(0x00000001u, 0x0000001fu, SRA);
    WAVE_EXPECT_EQ(top->alu_out, 0x00000000u);

    load(0xfffffff8u, 0x00000020u, SRA);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffff8u);

    load(0xfffffff8u, 0x00000021u, SRA);
    WAVE_EXPECT_EQ(top->alu_out, 0xfffffffcu);

    load(0xfffffff8u, 0xffffffffu, SRA);
    WAVE_EXPECT_EQ(top->alu_out, 0xffffffffu);

}

// Unused codes 10-15 must return zero after a valid operation.
TEST_F(Alu, UnsupportedControlsDoNotRetainPreviousResult)
{
    // Control 10 must clear the preceding ADD result.
    load(0x12345678u, 0x12345678u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x2468acf0u);
    load(0x12345678u, 0x12345678u, 10);
    WAVE_EXPECT_EQ(top->alu_out, 0u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    // Control 11 must clear the preceding ADD result.
    load(0x12345678u, 0x12345678u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x2468acf0u);
    load(0x12345678u, 0x12345678u, 11);
    WAVE_EXPECT_EQ(top->alu_out, 0u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    // Control 12 must clear the preceding ADD result.
    load(0x12345678u, 0x12345678u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x2468acf0u);
    load(0x12345678u, 0x12345678u, 12);
    WAVE_EXPECT_EQ(top->alu_out, 0u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    // Control 13 must clear the preceding ADD result.
    load(0x12345678u, 0x12345678u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x2468acf0u);
    load(0x12345678u, 0x12345678u, 13);
    WAVE_EXPECT_EQ(top->alu_out, 0u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    // Control 14 must clear the preceding ADD result.
    load(0x12345678u, 0x12345678u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x2468acf0u);
    load(0x12345678u, 0x12345678u, 14);
    WAVE_EXPECT_EQ(top->alu_out, 0u);
    WAVE_EXPECT_EQ(top->eq, 1u);

    // Control 15 must clear the preceding ADD result.
    load(0x12345678u, 0x12345678u, ADD);
    WAVE_EXPECT_EQ(top->alu_out, 0x2468acf0u);
    load(0x12345678u, 0x12345678u, 15);
    WAVE_EXPECT_EQ(top->alu_out, 0u);
    WAVE_EXPECT_EQ(top->eq, 1u);
}
