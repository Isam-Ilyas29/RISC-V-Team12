#pragma once

#include "base_testbench.h"
#include <cstdlib>

class Testbench : public BaseTestbench
{
public:
    // Runs the simulation for a clock cycle, evaluates the DUT, dumps waveform.
    void runSimulation(int cycles = 1)
    {
        for (int i = 0; i < cycles; i++)
        {
            ASSERT_LT(ticks, MAX_SIM_CYCLES) << "Simulation cycle limit exceeded";
            for (int clk = 0; clk < 2; clk++)
            {
                top->eval();
                tfp->dump(sampleTime++);
                top->clk = !top->clk;
            }
            ticks++;

            ASSERT_FALSE(Verilated::gotFinish()) << "Simulation ended early at cycle " << ticks;
        }
    }

};
