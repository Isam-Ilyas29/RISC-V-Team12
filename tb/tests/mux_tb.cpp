#include "base_testbench.h"

using Mux = BaseTestbench;

TEST_F(Mux, SelectsEitherWord)
{
    top->in0 = 0x12345678;
    top->in1 = 0xdeadbeef;
    for (unsigned sel : {0u, 1u, 0u}) {
        top->sel = sel;
        sample();
        EXPECT_EQ(top->out, sel ? top->in1 : top->in0);
    }
}