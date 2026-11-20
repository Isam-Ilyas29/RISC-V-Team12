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

TEST_F(CpuTestbench, AllBranchInstructions)
{
    ASSERT_EQ(std::system("./compile.sh asm/branches.S"), 0);
    reset();
    const uint32_t expected[] = {
        0, 0, 0, 0, 2, 2, 2, 2, 3, 5, 5, 5,
        5, 7, 7, 7, 7, 8, 10, 10, 10, 10, 12, 12,
        12, 12, 13, 15, 15, 15, 15, 17, 17, 17, 17, 18,
        20, 20, 20, 20, 22, 22, 22, 22, 23, 25, 25, 25,
        25, 27, 27, 27, 27, 28, 30, 30, 30, 30, 31, 31,
        31, 32, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34,
        34, 34, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37,
        37, 38, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40,
        40, 40, 41, 41, 41, 42, 42, 43,
    };
    for (uint32_t value : expected)
        ASSERT_EQ(step(), value) << "cycle " << ticks;
}

TEST_F(CpuTestbench, ShiftInstructions)
{
    ASSERT_EQ(std::system("./compile.sh asm/shifts.S"), 0);
    reset();
    const uint32_t expected[] = {
        0x00000000u, 0x00000000u, 0xfffffff8u, 0xfffffff0u, 0x00000000u, 0xfffffff8u, 0x7ffffffcu, 0x00000001u,
        0xfffffff8u, 0xfffffffcu, 0xffffffffu, 0xffffffffu, 0xfffffff8u, 0xfffffff8u, 0xfffffff0u, 0xfffffff0u,
        0x00000000u, 0x00000000u, 0xfffffff8u, 0xfffffff8u, 0xfffffff0u, 0xfffffff0u, 0x00000000u, 0x00000000u,
        0xfffffff8u, 0xfffffff8u, 0x7ffffffcu, 0x7ffffffcu, 0x00000001u, 0x00000001u, 0xfffffff8u, 0xfffffff8u,
        0x7ffffffcu, 0x7ffffffcu, 0x00000001u, 0x00000001u, 0xfffffff8u, 0xfffffff8u, 0xfffffffcu, 0xfffffffcu,
        0xffffffffu, 0xffffffffu, 0xfffffff8u, 0xfffffff8u, 0xfffffffcu, 0xfffffffcu, 0xffffffffu, 0x00000001u,
        0x80000000u, 0xffffffffu, 0xffffffffu, 0x00000000u
    };
    for (uint32_t value : expected)
        ASSERT_EQ(step(), value) << "cycle " << ticks;
}

TEST_F(CpuTestbench, JumpInstructions)
{
    ASSERT_EQ(std::system("./compile.sh asm/jumps.S"), 0);
    reset();
    const uint32_t expected[] = {0, 0, 8, 8, 8, 28, 28, 28, 44, 44, 45, 45,
                                 45, 45, 76, 76, 77, 77, 77};
    for (uint32_t value : expected)
        ASSERT_EQ(step(), value) << "cycle " << ticks;
}

TEST_F(CpuTestbench, LoadStoreWriteback)
{
    ASSERT_EQ(std::system("./compile.sh asm/memory.S"), 0);
    reset();
    const uint32_t expected[] = {0, 0, 0, 0, 0, 123, 123, 123, 123, 0xffffffffu};
    for (uint32_t value : expected)
        ASSERT_EQ(step(), value) << "cycle " << ticks;
}

TEST_F(CpuTestbench, Overview)
{
    ASSERT_EQ(std::system("./compile.sh asm/overview.S"), 0);
    reset();
    const uint32_t expected[] = {
        0, 0, 0, 0xfffffff0u, 0x7ffffffcu, 0xfffffffcu,
        0xfffffff0u, 0x7ffffffcu, 0xfffffffcu, 0xfffffffcu, 0, 0xfffffffcu,
        0xfffffffcu, 0xfffffffcu, 0xfffffffcu, 0xfffffffcu, 0xfffffffcu, 0xfffffffcu,
        0xfffffffcu, 0xfffffffdu, 0xfffffffdu, 0xfffffffeu, 0xfffffffeu,
        0xfffffffeu, 0xfffffffeu
    };
    for (uint32_t value : expected)
        ASSERT_EQ(step(), value) << "cycle " << ticks;
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
