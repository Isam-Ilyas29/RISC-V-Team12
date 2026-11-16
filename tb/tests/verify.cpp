#include "testbench.h"
#include <cstdlib>
#include <cstdint>

unsigned int ticks = 0;

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
        EXPECT_EQ(top->a0, previous) << "a0 changed before rising edge " << ticks;
        runSimulation();
        return top->a0;
    }
};

TEST_F(CpuTestbench, AddiAndBne)
{
    ASSERT_EQ(std::system("./compile.sh asm/addi_bne.S"), 0);
    reset();

    const int32_t expected[] = {
        -2048, -1, -1, 0,
        0, -366, -366, -366, -366, 0, -1
    };
    for (int32_t value : expected)
        ASSERT_EQ(step(), static_cast<uint32_t>(value)) << "cycle " << ticks;

    reset();
    ASSERT_EQ(step(), 0xfffff800u) << "first instruction after reset";
}

TEST_F(CpuTestbench, BaseProgramTest)
{
    ASSERT_EQ(std::system("./compile.sh asm/program.S"), 0);
    reset();

    step();
    ASSERT_EQ(step(), 0u);
    ASSERT_EQ(step(), 0u);

    // Each inner loop executes addi a0, addi a1, then bne.
    for (unsigned count = 0; count < 255; ++count)
        for (int cycle = 0; cycle < 3; ++cycle)
            ASSERT_EQ(step(), count) << "cycle " << ticks;

    for (unsigned value : {254u, 254u, 0u, 0u, 0u, 1u})
        ASSERT_EQ(step(), value) << "cycle " << ticks;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
