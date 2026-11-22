#pragma once

#include "testbench.h"
#include <cstdint>

class CpuTestbench : public Testbench
{
protected:
    void initializeInputs() override
    {
        ticks = 0;
        top->clk = 0;
        top->rst = 1;
    }

    void reset()
    {
        top->rst = 1;
        runSimulation();
        top->rst = 0;
    }

    uint32_t step()
    {
        // runSimulation leaves the falling edge pending until the next eval.
        const uint32_t previous = top->a0;
        top->eval();
        WAVE_EXPECT_EQ(top->a0, previous) << "a0 changed before rising edge " << ticks;
        runSimulation();
        return top->a0;
    }
};

