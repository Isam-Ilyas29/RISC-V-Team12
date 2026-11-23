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

// Check that the mux selects the correct input for both select values.
TEST_F(Mux, SelectsEitherWord)
{
    top->in0 = 0x12345678;
    top->in1 = 0xdeadbeef;
    for (unsigned sel : {0u, 1u, 0u}) {
        select(sel);
        WAVE_EXPECT_EQ(top->out, sel ? top->in1 : top->in0);
    }
}

// Change the inputs without changing select and check that the output follows.
TEST_F(Mux, TracksSelectedInputChanges)
{
    for (unsigned sel : {0u, 1u}) {
        top->sel = sel;
        top->in0 = 0;
        top->in1 = 0xffffffffu;
        sample();
        WAVE_EXPECT_EQ(top->out, sel ? 0xffffffffu : 0u);
        top->in0 = 0xaaaaaaaau;
        top->in1 = 0x55555555u;
        sample();
        WAVE_EXPECT_EQ(top->out, sel ? 0x55555555u : 0xaaaaaaaau);
    }
}
