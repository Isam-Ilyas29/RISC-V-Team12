#include "base_testbench.h"

using Datapath = BaseTestbench;

TEST_F(Datapath, WritebackAndBranchComparison)
{
    top->rs1 = 0;
    top->rs2 = 0;
    top->alu_ctrl = 0;
    top->alu_src = 1;
    top->WE3 = 1;
    top->ImmOp = 17;
    for (unsigned rd : {10u, 11u}) {
        top->rd = rd;
        top->clk = 0;
        sample();
        top->clk = 1;
        sample();
    }
    EXPECT_EQ(top->a0, 17u);
    top->WE3 = 0;
    top->rd = 10;
    top->alu_src = 0;
    top->alu_ctrl = 1;
    top->rs1 = 10;
    for (unsigned rs2 : {0u, 11u}) {
        top->rs2 = rs2;
        top->clk = 0;
        sample();
        EXPECT_EQ(top->eq, rs2 == 11);
        top->clk = 1;
        sample();
        EXPECT_EQ(top->a0, 17u);
    }
}