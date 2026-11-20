// Test wiring for the team modules while rtl/top.sv is a placeholder.
module lab4_cpu(input logic clk, rst, output logic [31:0] a0);
    logic [31:0] pc, instruction, immediate;
    logic eq, branch_taken, reg_write, alu_ctrl, alu_src, pc_src;
    logic [1:0] imm_src;

    pc_top counter(.clk(clk), .rst(rst), .PCsrc(pc_src), .ImmOp(immediate), .PC(pc));
    instr_mem rom(.a(pc), .rd(instruction));
    control_unit control(.opcode(instruction[6:0]), .branch_taken(branch_taken), .reg_write(reg_write),
                         .alu_ctrl(alu_ctrl), .alu_src(alu_src), .imm_src(imm_src), .pc_src(pc_src));
    sign_extend extend(.imm(instruction[31:7]), .imm_src(imm_src), .imm_op(immediate));
    alu_top datapath(.clk(clk), .WE3(reg_write), .alu_ctrl(alu_ctrl), .alu_src(alu_src),
                     .ImmOp(immediate), .rs1(instruction[19:15]), .rs2(instruction[24:20]),
                     .funct3(instruction[14:12]), .branch_taken(branch_taken),
                     .rd(instruction[11:7]), .a0(a0), .eq(eq));
endmodule
