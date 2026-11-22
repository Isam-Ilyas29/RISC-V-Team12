module sign_extend (
    input logic [31:7] imm,
    input logic [2:0] imm_src,
    output logic [31:0] imm_op
);
    always_comb begin
        case (imm_src)
            // In order: I B S J U
            3'b000: imm_op = {{20{imm[31]}}, imm[31:20]};
            3'b001: imm_op = {{19{imm[31]}}, imm[31], imm[7], imm[30:25], imm[11:8], 1'b0};
            3'b010: imm_op = {{20{imm[31]}}, imm[31:25], imm[11:7]};
            3'b011: imm_op = {{11{imm[31]}}, imm[31], imm[19:12], imm[20], imm[30:21], 1'b0};
            3'b100: imm_op = {imm[31:12], 12'b0};
            default: imm_op = '0;
        endcase
    end
endmodule
