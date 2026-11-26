#include <cstdlib>
#include <utility>

#include "cpu_testbench.h"

unsigned int ticks = 0;

#define CYCLES 10000

// Run the supplied counter loop and expect 254.
TEST_F(CpuTestbench, TestAddiBne)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("1_addi_bne"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    ASSERT_NO_FATAL_FAILURE(runSimulation(CYCLES));
    WAVE_EXPECT_EQ(top->a0, 254);
}

// Run the supplied LI/ADD program and expect 1000.
TEST_F(CpuTestbench, TestLiAdd)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("2_li_add"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    ASSERT_NO_FATAL_FAILURE(runSimulation(CYCLES));
    WAVE_EXPECT_EQ(top->a0, 1000);
}

// Store and load two bytes, then expect their sum of 300.
TEST_F(CpuTestbench, TestLbuSb)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("3_lbu_sb"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    ASSERT_NO_FATAL_FAILURE(runSimulation(CYCLES));
    WAVE_EXPECT_EQ(top->a0, 300);
}

// Run the supplied call/return program and expect 53.
TEST_F(CpuTestbench, TestJalRet)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("4_jal_ret"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    ASSERT_NO_FATAL_FAILURE(runSimulation(CYCLES));
    WAVE_EXPECT_EQ(top->a0, 53);
}

// Build the supplied Gaussian histogram and expect a bin sum of 15363.
TEST_F(CpuTestbench, TestPdf)
{
    ASSERT_NO_FATAL_FAILURE(setupTest("5_pdf"));
    ASSERT_NO_FATAL_FAILURE(setData("reference/gaussian.mem"));
    ASSERT_NO_FATAL_FAILURE(initSimulation());
    ASSERT_NO_FATAL_FAILURE(runSimulation(CYCLES * 100));
    WAVE_EXPECT_EQ(top->a0, 15363);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    return res;
}
