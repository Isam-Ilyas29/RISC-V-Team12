module sign_extend (
    input logic [31:7] imm,
    input logic [ 1:0] imm_src,

    output logic [31:0] imm_op
);

    always_comb begin
        // I-type instruction
        if (imm_src == 2'b00) begin
            imm_op = {{20{imm[31]}}, imm[31:20]};
        end  // B-type instruction 
        else if (imm_src == 2'b01) begin
            imm_op = {{19{imm[31]}}, imm[31], imm[7], imm[30:25], imm[11:8], 1'b0};
        end  // S-type instruction
        else if (imm_src == 2'b10) begin
            imm_op = {{20{imm[31]}}, imm[31:25], imm[11:7]};
        end
    end

endmodule
