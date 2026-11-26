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
        top->branch_taken = 0;
        top->opcode = 0;
        top->funct3 = 0;
        top->funct7 = 0;
    }

    void loadInstr(uint32_t instr)
    {
        top->opcode = instr & 0x7F;         // bits [6:0]
        top->funct3 = (instr >> 12) & 0x7;  // bits [14:12]
        top->funct7 = (instr >> 30) & 0x1;  // bit 30
        sample();  // eval + dump a timestep to the VCD
    }
};


// lui
TEST_F(ControlUnitTestbench, Lui) {
    // lui x1, 0x12345
    loadInstr(0x123450B7);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->imm_src, 0b100);
    WAVE_EXPECT_EQ(top->result_src, 0b011);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);
    WAVE_EXPECT_EQ(top->mem_write, 0);
}


// auipc
TEST_F(ControlUnitTestbench, Auipc) {
    // auipc x1, 0x12345
    loadInstr(0x12345097);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->imm_src, 0b100);
    WAVE_EXPECT_EQ(top->result_src, 0b100);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);
    WAVE_EXPECT_EQ(top->mem_write, 0);
}


// jal
TEST_F(ControlUnitTestbench, Jal) {
    // jal x1, 8
    loadInstr(0x008000EF);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->imm_src, 0b011);
    WAVE_EXPECT_EQ(top->result_src, 0b010);
    WAVE_EXPECT_EQ(top->pc_ctrl, 1);
    WAVE_EXPECT_EQ(top->mem_write, 0);
}


// jalr
TEST_F(ControlUnitTestbench, Jalr) {
    // jalr x1, 0(x2)
    loadInstr(0x000100E7);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0);
    WAVE_EXPECT_EQ(top->imm_src, 0b000);
    WAVE_EXPECT_EQ(top->result_src, 0b010);
    WAVE_EXPECT_EQ(top->pc_ctrl, 2);
    WAVE_EXPECT_EQ(top->mem_write, 0);
}


// branches (beq, bne, blt, bge, bltu, bgeu)
TEST_F(ControlUnitTestbench, Branches) {
    // beq x1, x2, 8 (taken)
    top->branch_taken = 1;
    loadInstr(0x00208463);
    WAVE_EXPECT_EQ(top->reg_write, 0);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->imm_src, 0b001);
    WAVE_EXPECT_EQ(top->mem_write, 0);
    WAVE_EXPECT_EQ(top->branch_ctrl, 0b000);
    WAVE_EXPECT_EQ(top->pc_ctrl, 1);

    // beq x1, x2, 8 (not taken)
    top->branch_taken = 0;
    loadInstr(0x00208463);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);

    // bne x1, x2, 8
    top->branch_taken = 1;
    loadInstr(0x00209463);
    WAVE_EXPECT_EQ(top->branch_ctrl, 0b001);
    WAVE_EXPECT_EQ(top->pc_ctrl, 1);
    top->branch_taken = 0;
    loadInstr(0x00209463);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);

    // blt x1, x2, 8
    top->branch_taken = 1;
    loadInstr(0x0020C463);
    WAVE_EXPECT_EQ(top->branch_ctrl, 0b100);
    WAVE_EXPECT_EQ(top->pc_ctrl, 1);
    top->branch_taken = 0;
    loadInstr(0x0020C463);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);

    // bge x1, x2, 8
    top->branch_taken = 1;
    loadInstr(0x0020D463);
    WAVE_EXPECT_EQ(top->branch_ctrl, 0b101);
    WAVE_EXPECT_EQ(top->pc_ctrl, 1);
    top->branch_taken = 0;
    loadInstr(0x0020D463);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);

    // bltu x1, x2, 8
    top->branch_taken = 1;
    loadInstr(0x0020E463);
    WAVE_EXPECT_EQ(top->branch_ctrl, 0b110);
    WAVE_EXPECT_EQ(top->pc_ctrl, 1);
    top->branch_taken = 0;
    loadInstr(0x0020E463);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);

    // bgeu x1, x2, 8
    top->branch_taken = 1;
    loadInstr(0x0020F463);
    WAVE_EXPECT_EQ(top->branch_ctrl, 0b111);
    WAVE_EXPECT_EQ(top->pc_ctrl, 1);
    top->branch_taken = 0;
    loadInstr(0x0020F463);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);
}


// loads (lb, lh, lw, lbu, lhu)
TEST_F(ControlUnitTestbench, Loads) {
    // lb x1, 0(x2)
    loadInstr(0x00010083);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->imm_src, 0b000);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);
    WAVE_EXPECT_EQ(top->result_src, 0b001);
    WAVE_EXPECT_EQ(top->mem_write, 0);
    WAVE_EXPECT_EQ(top->data_ctrl, 0b000);

    // lh x1, 0(x2)
    loadInstr(0x00011083);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->result_src, 0b001);
    WAVE_EXPECT_EQ(top->data_ctrl, 0b001);

    // lw x1, 0(x2)
    loadInstr(0x00012083);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->result_src, 0b001);
    WAVE_EXPECT_EQ(top->data_ctrl, 0b010);

    // lbu x1, 0(x2)
    loadInstr(0x00014083);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->result_src, 0b001);
    WAVE_EXPECT_EQ(top->data_ctrl, 0b100);

    // lhu x1, 0(x2)
    loadInstr(0x00015083);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->result_src, 0b001);
    WAVE_EXPECT_EQ(top->data_ctrl, 0b101);
}


// stores (sb, sh, sw)
TEST_F(ControlUnitTestbench, Stores) {
    // sb x1, 0(x2)
    loadInstr(0x00110023);
    WAVE_EXPECT_EQ(top->reg_write, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->imm_src, 0b010);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);
    WAVE_EXPECT_EQ(top->mem_write, 1);
    WAVE_EXPECT_EQ(top->data_ctrl, 0b000);

    // sh x1, 0(x2)
    loadInstr(0x00111023);
    WAVE_EXPECT_EQ(top->reg_write, 0);
    WAVE_EXPECT_EQ(top->mem_write, 1);
    WAVE_EXPECT_EQ(top->data_ctrl, 0b001);

    // sw x1, 0(x2)
    loadInstr(0x00112023);
    WAVE_EXPECT_EQ(top->reg_write, 0);
    WAVE_EXPECT_EQ(top->mem_write, 1);
    WAVE_EXPECT_EQ(top->data_ctrl, 0b010);
}


// I-type ALU (addi, slti, sltiu, xori, ori, andi, slli, srli, srai)
TEST_F(ControlUnitTestbench, AluImmediate) {
    // addi x1, x2, 5
    loadInstr(0x00510093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->imm_src, 0b000);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);
    WAVE_EXPECT_EQ(top->result_src, 0b000);
    WAVE_EXPECT_EQ(top->mem_write, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0000);

    // slti x1, x2, 5
    loadInstr(0x00512093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0101);

    // sltiu x1, x2, 5
    loadInstr(0x00513093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0110);

    // xori x1, x2, 5
    loadInstr(0x00514093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0100);

    // ori x1, x2, 5
    loadInstr(0x00516093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0011);

    // andi x1, x2, 5
    loadInstr(0x00517093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0010);

    // slli x1, x2, 3
    loadInstr(0x00311093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0111);

    // srli x1, x2, 3
    loadInstr(0x00315093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b1000);

    // srai x1, x2, 3
    loadInstr(0x40315093);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 1);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b1001);
}


// R-type ALU (add, sub, sll, slt, sltu, xor, srl, sra, or, and)
TEST_F(ControlUnitTestbench, AluRegister) {
    // add x1, x2, x3
    loadInstr(0x003100B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->pc_ctrl, 0);
    WAVE_EXPECT_EQ(top->result_src, 0b000);
    WAVE_EXPECT_EQ(top->mem_write, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0000);

    // sub x1, x2, x3
    loadInstr(0x403100B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0001);

    // sll x1, x2, x3
    loadInstr(0x003110B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0111);

    // slt x1, x2, x3
    loadInstr(0x003120B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0101);

    // sltu x1, x2, x3
    loadInstr(0x003130B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0110);

    // xor x1, x2, x3
    loadInstr(0x003140B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0100);

    // srl x1, x2, x3
    loadInstr(0x003150B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b1000);

    // sra x1, x2, x3
    loadInstr(0x403150B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b1001);

    // or x1, x2, x3
    loadInstr(0x003160B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0011);

    // and x1, x2, x3
    loadInstr(0x003170B3);
    WAVE_EXPECT_EQ(top->reg_write, 1);
    WAVE_EXPECT_EQ(top->alu_src, 0);
    WAVE_EXPECT_EQ(top->alu_ctrl, 0b0010);
}
