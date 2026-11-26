#include "base_testbench.h"

class Mux : public BaseTestbench
{
protected:
    void select(unsigned input)
    {
        top->sel = input;
        sample();
    }

    void initializeInputs() override
    {
        top->in0 = 0;
        top->in1 = 0;
        top->sel = 0;
    }
};

// Select input zero, input one, then return to input zero.
TEST_F(Mux, SelectsEitherWord)
{
    top->in0 = 0x12345678;
    top->in1 = 0xdeadbeef;

    select(0);
    WAVE_EXPECT_EQ(top->out, 0x12345678u);
    select(1);
    WAVE_EXPECT_EQ(top->out, 0xdeadbeefu);
    select(0);
    WAVE_EXPECT_EQ(top->out, 0x12345678u);
}

// Changing input zero updates the output while select stays low.
TEST_F(Mux, TracksInputZero)
{
    top->in0 = 0;
    top->in1 = 0xffffffffu;
    select(0);
    WAVE_EXPECT_EQ(top->out, 0u);

    top->in0 = 0xaaaaaaaau;
    top->in1 = 0x55555555u;
    sample();
    WAVE_EXPECT_EQ(top->out, 0xaaaaaaaau);
}

// Changing input one updates the output while select stays high.
TEST_F(Mux, TracksInputOne)
{
    top->in0 = 0;
    top->in1 = 0xffffffffu;
    select(1);
    WAVE_EXPECT_EQ(top->out, 0xffffffffu);

    top->in0 = 0xaaaaaaaau;
    top->in1 = 0x55555555u;
    sample();
    WAVE_EXPECT_EQ(top->out, 0x55555555u);
}
