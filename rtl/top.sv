module top #(parameter DATA_WIDTH = 32) (
    input logic clk, rst,
    output logic [DATA_WIDTH-1:0] a0
);
    logic [31:0] pc, instruction, immediate, alu_result, store_data, read_data;
    logic branch_taken, reg_write, alu_src, pc_src, mem_write, jalr;
    logic [3:0] alu_ctrl;
    logic [1:0] imm_src, result_src;
    logic [2:0] data_ctrl;

    instr_mem rom(.a(pc), .rd(instruction));
    control_unit control (
        .opcode(instruction[6:0]), .funct3(instruction[14:12]), .funct7(instruction[30]),
        .branch_taken(branch_taken), .reg_write(reg_write), .alu_ctrl(alu_ctrl),
        .alu_src(alu_src), .imm_src(imm_src), .pc_src(pc_src), .result_src(result_src),
        .mem_write(mem_write), .jalr(jalr), .data_ctrl(data_ctrl)
    );
    sign_extend extend(.imm(instruction[31:7]), .imm_src(imm_src), .imm_op(immediate));
    pc_top counter (
        .clk(clk), .rst(rst), .PCsrc(pc_src), .jalr(jalr),
        .ImmOp(immediate), .register_target(alu_result), .PC(pc)
    );
    alu_top datapath (
        .clk(clk), .WE3(reg_write && !rst), .alu_ctrl(alu_ctrl), .alu_src(alu_src),
        .result_src(result_src), .read_data(read_data), .pc_plus4(pc + 32'd4),
        .ImmOp(immediate), .rs1(instruction[19:15]), .rs2(instruction[24:20]),
        .rd(instruction[11:7]), .funct3(instruction[14:12]),
        .branch_taken(branch_taken), .eq(), .a0(a0),
        .alu_result(alu_result), .store_data(store_data)
    );
    data_mem #(.INIT_FILE("")) ram (
        .clk(clk), .we(mem_write && !rst), .a(alu_result), .wd(store_data),
        .data_ctrl(data_ctrl), .rd(read_data)
    );
endmodule
