module top
    #(
        parameter DATA_WIDTH = 32
        parameter ADDR_WIDTH = 5
    )
    (
        input logic clk, WE3, alu_ctrl, alu_src,
        input logic [DATA_WIDTH-1 : 0] ImmOp
        input logic [ADDR_WIDTH-1 : 0] rs1, rs2, rd
        output logic [DATA_WIDTH-1 : 0] a0, eq
    );

    logic      [DATA_WIDTH-1 : 0] RD1, RD2, WD3, alu2,
    

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
                 .alu_out(WD3),
                 .eq(eq)
    );

