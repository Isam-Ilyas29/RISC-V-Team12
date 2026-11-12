module pc_top #(
    parameter WIDTH = 32
)(
    input logic                  clk,
    input logic                  rst,
    input logic                  PCsrc,
    input logic  [WIDTH-1:0]     ImmOp,
    output logic [WIDTH-1:0]     PC
);

    logic [WIDTH-1:0] inc_PC, branch_PC, next_PC;

    assign inc_PC    = PC + 32'd4;
    assign branch_PC = PC + ImmOp;

    mux #(
        .DATA_WIDTH(WIDTH)
    ) pc_mux (
            .in0(inc_PC), 
            .in1(branch_PC), 
            .sel(PCsrc), 
            .out(next_PC)
        );
    
    pc_reg #(
        .WIDTH(WIDTH)
    ) pc_r (
        .clk(clk),
        .rst(rst),
        .next_PC(next_PC),
        .PC(PC)
    );

    endmodule
