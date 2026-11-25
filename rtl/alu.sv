module alu #(
    parameter DATA_WIDTH = 32,
    parameter ALU_CTRL_WIDTH = 4
) (
    input logic [  DATA_WIDTH-1 : 0] alu1,
    input logic [  DATA_WIDTH-1 : 0] alu2,
    input logic [ALU_CTRL_WIDTH-1:0] alu_ctrl,

    output logic [DATA_WIDTH-1 : 0] alu_out,
    output logic                    eq
);

    localparam ADD  = 4'b0000;
    localparam SUB  = 4'b0001;
    localparam AND  = 4'b0010;
    localparam OR   = 4'b0011;
    localparam XOR  = 4'b0100;
    localparam SLT  = 4'b0101;
    localparam SLTU = 4'b0110;
    localparam SLL  = 4'b0111;
    localparam SRL  = 4'b1000;
    localparam SRA  = 4'b1001;

    logic [4:0] shamt = alu2[4:0];  // shift amount

    always_comb begin
        case (alu_ctrl)
            ADD:     alu_out = alu1 + alu2;
            SUB:     alu_out = alu1 - alu2;
            AND:     alu_out = alu1 & alu2;
            OR:      alu_out = alu1 | alu2;
            XOR:     alu_out = alu1 ^ alu2;
            SLT:     alu_out = $signed(alu1) < $signed(alu2);
            SLTU:    alu_out = $unsigned(alu1) < $unsigned(alu2);
            SLL:     alu_out = alu1 << shamt;
            SRL:     alu_out = alu1 >> shamt;
            SRA:     alu_out = $signed(alu1) >>> shamt;
            default: alu_out = 32'b0;
        endcase

        eq = (alu1 == alu2);
    end

endmodule
