/*
Verifies the control unit, exits with a 0 on success.
*/

#include "base_testbench.h"

unsigned int ticks = 0;

class ControlUnitTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->eq = 0;
        top->opcode = 0;
        top->funct3 = 0;
        top->funct7 = 0;
    }

    void loadInstr(uint32_t instr)
    {
        top->opcode = instr & 0x7F;         // bits [6:0]
        top->funct3 = (instr >> 12) & 0x7;  // bits [14:12]
        top->funct7 = (instr >> 30) & 0x1;  // bit 30
        top->eval();
    }
};


// addi
TEST_F(ControlUnitTestbench, Addi) {
    // addi x1, x0, 5
    loadInstr(0b00000000010100000000000010010011);
    EXPECT_EQ(top->reg_write, 1);
    EXPECT_EQ(top->alu_ctrl, 0);
    EXPECT_EQ(top->alu_src, 1);
    EXPECT_EQ(top->imm_src, 0b00);
    EXPECT_EQ(top->pc_src, 0);
    EXPECT_EQ(top->result_src, 0);
    EXPECT_EQ(top->mem_write, 0);
}


// loads (lb, lh, lw, lbu, lhu)
TEST_F(ControlUnitTestbench, Loads) {
    // lb x1, 0(x2)
    loadInstr(0b00000000000000010000000010000011);
    EXPECT_EQ(top->reg_write, 1);
    EXPECT_EQ(top->alu_ctrl, 0);
    EXPECT_EQ(top->alu_src, 1);
    EXPECT_EQ(top->imm_src, 0b00);
    EXPECT_EQ(top->pc_src, 0);
    EXPECT_EQ(top->result_src, 1);
    EXPECT_EQ(top->mem_write, 0);
    EXPECT_EQ(top->data_ctrl, 0b000);

    // lh x1, 0(x2)
    loadInstr(0b00000000000000010001000010000011);
    EXPECT_EQ(top->result_src, 1);
    EXPECT_EQ(top->data_ctrl, 0b001);

    // lw x1, 0(x2)
    loadInstr(0b00000000000000010010000010000011);
    EXPECT_EQ(top->result_src, 1);
    EXPECT_EQ(top->data_ctrl, 0b010);

    // lbu x1, 0(x2)
    loadInstr(0b00000000000000010100000010000011);
    EXPECT_EQ(top->result_src, 1);
    EXPECT_EQ(top->data_ctrl, 0b100);

    // lhu x1, 0(x2)
    loadInstr(0b00000000000000010101000010000011);
    EXPECT_EQ(top->result_src, 1);
    EXPECT_EQ(top->data_ctrl, 0b101);
}


// stores (sb, sh, sw)
TEST_F(ControlUnitTestbench, Stores) {
    // sb x1, 0(x2)
    loadInstr(0b00000000000100010000000000100011);
    EXPECT_EQ(top->reg_write, 0);
    EXPECT_EQ(top->alu_ctrl, 0);
    EXPECT_EQ(top->alu_src, 1);
    EXPECT_EQ(top->imm_src, 0b10);
    EXPECT_EQ(top->pc_src, 0);
    EXPECT_EQ(top->mem_write, 1);
    EXPECT_EQ(top->data_ctrl, 0b000);

    // sh x1, 0(x2)
    loadInstr(0b00000000000100010001000000100011);
    EXPECT_EQ(top->mem_write, 1);
    EXPECT_EQ(top->data_ctrl, 0b001);

    // sw x1, 0(x2)
    loadInstr(0b00000000000100010010000000100011);
    EXPECT_EQ(top->mem_write, 1);
    EXPECT_EQ(top->data_ctrl, 0b010);
}


// brances (bne)
TEST_F(ControlUnitTestbench, Branches) {
    // bne x1, x2, 0 -- (taken, registers differ)
    top->eq = 0;
    loadInstr(0b00000000001000001001000001100011);
    EXPECT_EQ(top->reg_write, 0);
    EXPECT_EQ(top->alu_ctrl, 1);
    EXPECT_EQ(top->alu_src, 0);
    EXPECT_EQ(top->imm_src, 0b01);
    EXPECT_EQ(top->pc_src, 1);
    EXPECT_EQ(top->mem_write, 0);

    // bne x1, x2, 0 (not taken, registers equal)
    top->eq = 1;
    loadInstr(0b00000000001000001001000001100011);
    EXPECT_EQ(top->pc_src, 0);
}
