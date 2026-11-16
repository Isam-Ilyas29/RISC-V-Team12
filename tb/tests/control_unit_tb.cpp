#include "base_testbench.h"

using Control = BaseTestbench;

TEST_F(Control, Decode)
{
    for (unsigned opcode = 0; opcode < 128; ++opcode)
        for (unsigned equal : {0u, 1u}) {
            SCOPED_TRACE(::testing::Message() << "opcode " << opcode << ", eq " << equal);
            top->opcode = opcode;
            top->eq = equal;
            sample();
            const bool writes = opcode == 0x13 || opcode == 0x03;
            EXPECT_EQ(top->reg_write, writes);
            EXPECT_EQ(top->pc_src, opcode == 0x63 && !equal);
            if (writes)
                EXPECT_EQ(top->result_src, opcode == 0x03);
            if (writes || opcode == 0x63) {
                EXPECT_EQ(top->alu_src, writes);
                EXPECT_EQ(top->alu_ctrl, opcode == 0x63);
                EXPECT_EQ(top->imm_src, opcode == 0x63);
            }
        }
}
