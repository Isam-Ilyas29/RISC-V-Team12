module alu #(parameter DATA_WIDTH = 32) (
    input logic [DATA_WIDTH-1:0] alu1, alu2,
    input logic [2:0] alu_ctrl,
    output logic [DATA_WIDTH-1:0] alu_out,
    output logic eq
);
    localparam ADD = 3'd0, SLL = 3'd2, SRL = 3'd3, SRA = 3'd4;

    always_comb begin
        case (alu_ctrl)
            ADD: alu_out = alu1 + alu2;
            SLL: alu_out = alu1 << alu2[4:0];
            SRL: alu_out = alu1 >> alu2[4:0];
            SRA: alu_out = $signed(alu1) >>> alu2[4:0];
            default: alu_out = '0;
        endcase
        eq = alu1 == alu2;
    end
endmodule
