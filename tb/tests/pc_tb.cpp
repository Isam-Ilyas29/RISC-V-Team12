#include "base_testbench.h"

class ProgramCounter : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->clk = 0;
        top->rst = 0;
        top->pc_ctrl = 0;
        top->immediate = 0;
        top->alu_result = 0;
    }

    void clock(bool high)
    {
        top->clk = high;
        sample();
    }

    void reset()
    {
        top->rst = 1;
        clock(0);
        clock(1);
        top->rst = 0;
        clock(0);
    }
};

// Reset loads the instruction base; normal cycles advance by four bytes.
TEST_F(ProgramCounter, ResetAndIncrement)
{
    reset();
    WAVE_EXPECT_EQ(top->pc, 0xbfc00000u);
    WAVE_EXPECT_EQ(top->pc_plus_4, 0xbfc00004u);
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0xbfc00004u);
    clock(0);
    WAVE_EXPECT_EQ(top->pc, 0xbfc00004u);
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0xbfc00008u);
}

// Relative branches add positive, negative and zero offsets to the current PC.
TEST_F(ProgramCounter, RelativeBranches)
{
    reset();
    top->pc_ctrl = 1;
    top->immediate = 16;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0xbfc00010u);
    clock(0);
    top->immediate = uint32_t(-8);
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0xbfc00008u);
    clock(0);
    top->immediate = 0;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0xbfc00008u);
}

// JALR clears bit zero of the ALU target, including odd and negative targets.
TEST_F(ProgramCounter, RegisterJumps)
{
    reset();
    top->pc_ctrl = 2;

    // Target 0x00000000u with bit zero cleared.
    clock(0);
    top->alu_result = 0x00000000u;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0x00000000u);

    // Target 0x00000001u with bit zero cleared.
    clock(0);
    top->alu_result = 0x00000001u;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0x00000000u);

    // Target 0x00000002u with bit zero cleared.
    clock(0);
    top->alu_result = 0x00000002u;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0x00000002u);

    // Target 0x00000003u with bit zero cleared.
    clock(0);
    top->alu_result = 0x00000003u;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0x00000002u);

    // Target 0x00000101u with bit zero cleared.
    clock(0);
    top->alu_result = 0x00000101u;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0x00000100u);

    // Target 0xffffffffu with bit zero cleared.
    clock(0);
    top->alu_result = 0xffffffffu;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0xfffffffeu);
}

// Reset takes priority over a pending jump; reserved control increments the PC.
TEST_F(ProgramCounter, ResetPriority)
{
    top->pc_ctrl = 2;
    top->alu_result = 0x101;
    reset();
    WAVE_EXPECT_EQ(top->pc, 0xbfc00000u);
    top->pc_ctrl = 3;
    sample();
    clock(1);
    WAVE_EXPECT_EQ(top->pc, 0xbfc00004u);
}
