module pc_top #(parameter WIDTH = 32) (
    input logic clk, rst, PCsrc, jalr,
    input logic [WIDTH-1:0] ImmOp, register_target,
    output logic [WIDTH-1:0] PC, PC_target
);
    logic [WIDTH-1:0] inc_PC, branch_PC, next_PC, target_PC;

    assign inc_PC = PC + 4;
    assign branch_PC = PC + ImmOp;
    assign target_PC = jalr ? {register_target[WIDTH-1:1], 1'b0} : branch_PC;
    assign next_PC = PCsrc ? target_PC : inc_PC;
    assign PC_target = branch_PC;

    pc_reg #(.WIDTH(WIDTH)) pc_r (
        .clk(clk), .rst(rst), .next_PC(next_PC), .PC(PC)
    );
endmodule
