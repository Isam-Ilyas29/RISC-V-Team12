#include "alu_testbench.h"
#include "alu_controls.h"

class Datapath : public AluWaveTestbench
{
protected:
    void initializeInputs() override
    {
        top->clk = 0;
        top->WE3 = 0;
        top->alu_src = 0;
        top->alu_ctrl = ADD;
        top->funct3 = 0;
        top->result_src = 0;
        top->ImmOp = 0;
        top->read_data = 0;
        top->pc_plus4 = 0;
        top->rs1 = 0;
        top->rs2 = 0;
        top->rd = 0;
    }

    void write(unsigned rd, unsigned source)
    {
        top->clk = 0;
        top->rd = rd;
        top->result_src = source;
        top->WE3 = 1;
        sample();
        top->clk = 1;
        sample();
        top->clk = 0;
        top->WE3 = 0;
        sample();
    }
};

TEST_F(Datapath, WritebackAndBranchComparison)
{
    top->rs1 = 0;
    top->rs2 = 0;
    top->alu_ctrl = ADD;
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
    WAVE_EXPECT_EQ(top->a0, 17u);
    top->WE3 = 0;
    top->rd = 10;
    top->alu_src = 0;
    top->alu_ctrl = SUB;
    top->rs1 = 10;
    for (unsigned rs2 : {0u, 11u}) {
        top->rs2 = rs2;
        top->clk = 0;
        sample();
        WAVE_EXPECT_EQ(top->eq, rs2 == 11);
        top->clk = 1;
        sample();
        WAVE_EXPECT_EQ(top->a0, 17u);
    }
}

TEST_F(Datapath, WritebackSourcesAndOperandSelection)
{
    top->alu_src = 1;
    top->ImmOp = 7;
    write(10, 0);
    WAVE_EXPECT_EQ(top->a0, 7u);
    top->read_data = 0xfffffff8u;
    write(5, 1);
    top->pc_plus4 = 0x104;
    write(6, 2);
    top->rs1 = 5;
    top->rs2 = 6;
    top->alu_src = 0;
    sample();
    WAVE_EXPECT_EQ(top->alu_result, 0xfcu);
    WAVE_EXPECT_EQ(top->store_data, 0x104u);
    top->alu_src = 1;
    top->ImmOp = 1;
    top->alu_ctrl = SRA;
    sample();
    WAVE_EXPECT_EQ(top->alu_result, 0xfffffffcu);
    WAVE_EXPECT_EQ(top->store_data, 0x104u);
    write(10, 0);
    WAVE_EXPECT_EQ(top->a0, 0xfffffffcu);
    write(10, 2);
    WAVE_EXPECT_EQ(top->a0, 0x104u);
    write(10, 1);
    WAVE_EXPECT_EQ(top->a0, 0xfffffff8u);
    top->read_data = 123;
    write(0, 1);
    top->rs2 = 0;
    sample();
    WAVE_EXPECT_EQ(top->store_data, 0u);
}

TEST_F(Datapath, SignedAndUnsignedBranchOperands)
{
    top->read_data = 0xffffffffu;
    write(5, 1);
    top->read_data = 1;
    write(6, 1);
    top->rs1 = 5;
    top->rs2 = 6;
    const bool expected[] = {false, true, false, false, true, false, false, true};
    for (unsigned condition = 0; condition < 8; ++condition) {
        top->funct3 = condition;
        sample();
        WAVE_EXPECT_EQ(top->branch_taken, expected[condition]) << "funct3 " << condition;
    }
}
