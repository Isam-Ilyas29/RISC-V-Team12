module alu_top #(
    parameter DATA_WIDTH = 32,
    parameter ADDR_WIDTH = 5
) (
    input logic clk, WE3, alu_src,
    input logic [3:0] alu_ctrl,
    input logic [2:0] funct3,
    input logic [1:0] result_src,
    input logic [DATA_WIDTH-1:0] ImmOp, read_data, pc_plus4,
    input logic [ADDR_WIDTH-1:0] rs1, rs2, rd,
    output logic branch_taken, eq,
    output logic [DATA_WIDTH-1:0] a0, alu_result, store_data
);
    logic [DATA_WIDTH-1:0] RD1, RD2, WD3, alu2;

    mux #(.DATA_WIDTH(DATA_WIDTH)) alu_src_mux (
        .in0(RD2), .in1(ImmOp), .sel(alu_src), .out(alu2)
    );

    always_comb begin
        case (result_src)
            2'b00: WD3 = alu_result;
            2'b01: WD3 = read_data;
            2'b10: WD3 = pc_plus4;
            default: WD3 = '0;
        endcase
    end

    reg_file #(.DATA_WIDTH(DATA_WIDTH), .ADDR_WIDTH(ADDR_WIDTH)) alu_reg_file (
        .clk(clk), .WE3(WE3), .AD1(rs1), .AD2(rs2), .AD3(rd),
        .WD3(WD3), .a0(a0), .RD1(RD1), .RD2(RD2)
    );
    alu #(.DATA_WIDTH(DATA_WIDTH)) alu (
        .alu1(RD1), .alu2(alu2), .alu_ctrl(alu_ctrl), .alu_out(alu_result), .eq(eq)
    );
    branch_unit #(.DATA_WIDTH(DATA_WIDTH)) branch (
        .rs1_data(RD1), .rs2_data(RD2), .funct3(funct3), .branch_taken(branch_taken)
    );
    assign store_data = RD2;
endmodule
