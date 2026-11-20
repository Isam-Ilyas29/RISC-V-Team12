#include "base_testbench.h"

using Control = BaseTestbench;

TEST_F(Control, Decode)
{
    for (unsigned opcode : {0x13u, 0x33u, 0x03u, 0x23u, 0x63u, 0x6fu, 0x67u, 0u, 0x7fu})
        for (unsigned funct3 = 0; funct3 < 8; ++funct3)
            for (unsigned funct7 : {0u, 0x20u, 1u, 0x7fu})
                for (unsigned taken : {0u, 1u}) {
                    if (taken && opcode != 0x63)
                        continue;
                    if (funct7 && !((opcode == 0x13 || opcode == 0x33) &&
                                    (funct3 == 1 || funct3 == 5)))
                        continue;
                    top->opcode = opcode;
                    top->funct3 = funct3;
                    top->funct7 = funct7;
                    top->branch_taken = taken;
                    sample();
                    const bool shift_left = funct3 == 1 && funct7 == 0;
                    const bool shift_right = funct3 == 5 && (funct7 == 0 || funct7 == 0x20);
                    const bool shift = (opcode == 0x13 || opcode == 0x33) && (shift_left || shift_right);
                    const bool addi = opcode == 0x13 && funct3 == 0;
                    const bool load = opcode == 3 && funct3 == 2;
                    const bool store = opcode == 0x23 && funct3 == 2;
                    const bool jal = opcode == 0x6f;
                    const bool jalr = opcode == 0x67 && funct3 == 0;
                    const bool branch = opcode == 0x63 && funct3 != 2 && funct3 != 3;
                    EXPECT_EQ(top->reg_write, shift || addi || load || jal || jalr);
                    EXPECT_EQ(top->mem_write, store);
                    EXPECT_EQ(top->pc_src, jal || jalr || (branch && taken));
                    EXPECT_EQ(top->jalr, jalr);
                    if (shift) {
                        EXPECT_EQ(top->alu_src, opcode == 0x13);
                        EXPECT_EQ(top->alu_ctrl, shift_left ? 2u : funct7 == 0x20 ? 4u : 3u);
                    }
                    if (jal || jalr)
                        EXPECT_EQ(top->result_src, 2u);
                    if (jal)
                        EXPECT_EQ(top->imm_src, 3u);
                }
}
