module top #(
    parameter DATA_WIDTH = 32,
    parameter ADDR_WIDTH = 5
) (
    input  logic                  clk,
    input  logic                  rst,
    output logic [DATA_WIDTH-1:0] a0

);

    logic [DATA_WIDTH-1:0] inc_PC, branch_PC, next_PC, PC, RD1, RD2, WD3, 
                            alu2, ImmOp, alu_out, RD, RD_in;    
    logic [ADDR_WIDTH-1 : 0] rs1, rs2, rd;
    logic [6:0] opcode;
    logic [31:7] imm;        
    logic                    WE3, eq, alu_ctrl, alu_src, we, result_src, imm_src, PCsrc;        
        
    
    
    assign inc_PC    = PC + 32'd4;
    assign branch_PC = PC + ImmOp;
    assign we = 1'b0;

    mux #(
        .DATA_WIDTH(DATA_WIDTH)
    ) pc_mux (
            .in0(inc_PC), 
            .in1(branch_PC), 
            .sel(PCsrc), 
            .out(next_PC)
        );

    pc_reg #(
        .WIDTH(DATA_WIDTH)
    ) pc_r (
        .clk(clk),
        .rst(rst),
        .next_PC(next_PC),
        .PC(PC)
    );

    mux #(
    ) alu_src_mux (
                 .in0(RD2),
                 .in1(ImmOp),
                 .sel(alu_src),
                 .out(alu2)
    );

    reg_file #(
    ) alu_reg_file (
                 .clk(clk),
                 .WE3(WE3),
                 .AD1(rs1),
                 .AD2(rs2),
                 .AD3(rd),
                 .WD3(WD3),
                 .a0(a0),
                 .RD1(RD1),
                 .RD2(RD2)
    );

    alu # (
    ) alu (
                 .alu1(RD1),
                 .alu2(alu2),
                 .alu_ctrl(alu_ctrl),
                 .alu_out(alu_out),
                 .eq(eq)
    );

    data_mem #(
    ) data_mem (
                 .a(alu_out),
                 .wd(RD2),
                 .we(we),
                 .clk(clk),
                 .rd(RD_in)
    );

    control_unit #(
    ) control_unit (
                 .eq(eq),
                 .opcode(opcode),
                 .reg_write(WE3),
                 .alu_ctrl(alu_ctrl),
                 .alu_src(alu_src),
                 .imm_src(imm_src),
                 .pc_src(PCsrc),
                 .result_src(result_src)
                 //.mem_write(we)
    );

    instr_mem #(
    ) instr_mem (
                 .a(PC),
                 .rd(RD)
    );

    assign opcode   = RD[6:0];
    assign rs1      = RD[19:15];
    assign rs2      = RD[24:20];
    assign rd       = RD[11:7];
    assign imm      = RD[31:7];

    mux #(
    ) wd3_mux (
                 .in0(alu_out),
                 .in1(RD_in),
                 .sel(result_src),
                 .out(WD3)
    );

    sign_extend #(
    ) sign_extend (
                 .imm(imm),
                 .imm_src(imm_src),
                 .imm_op(ImmOp)
    );

endmodule

// TODO: add a mux driven by ResultSrc for wd3 register file input (see page 13: https://elec50010-iac.github.io/course-website/lectures/Lecture%207%20-%20Microarchitecture%20(notes).pdf)
