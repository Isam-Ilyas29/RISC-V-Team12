module sign_extend (
    input logic [31:7] imm,
    input logic [1:0] imm_src,
    output logic [31:0] imm_op
);
    always_comb begin
        case (imm_src)
            2'b00: imm_op = {{20{imm[31]}}, imm[31:20]};
            2'b01: imm_op = {{19{imm[31]}}, imm[31], imm[7], imm[30:25], imm[11:8], 1'b0};
            2'b10: imm_op = {{20{imm[31]}}, imm[31:25], imm[11:7]};
            2'b11: imm_op = {{11{imm[31]}}, imm[31], imm[19:12], imm[20], imm[30:21], 1'b0};
            default: imm_op = '0;
        endcase
    end
endmodule
