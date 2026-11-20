module control_unit (
    input logic       branch_taken,
    input logic [6:0] opcode,

    output logic       reg_write,   // 1 bit cause you either write or dont for we3
    output logic       alu_ctrl,    // 1 bit cause you only have 2 instructions
    output logic       alu_src,     // 1 bit cause 2 input mux
    output logic [1:0] imm_src,     // 1 bit cause you only have I-type and B-type instructions
    output logic       pc_src,      // 1 bit cause 2 input mux
    output logic       result_src,  // 1 bit cause either lw or addi currently
    output logic       mem_write    // 1 bit cause you either write or dont in data memory
);

    always_comb begin
        // I-type instructions
        if (opcode == 7'b0010011) begin
            // addi 
            reg_write  = 1'b1;
            alu_ctrl   = 1'b0;
            alu_src    = 1'b1;
            imm_src    = 2'b00;
            pc_src     = 1'b0;
            result_src = 1'b0;
            mem_write  = 1'b0;
        end else if (opcode == 7'b0000011) begin
            // lw
            reg_write  = 1'b1;
            alu_ctrl   = 1'b0;  // 0 is used for addition (add immediate constant to address)
            alu_src    = 1'b1;
            imm_src    = 2'b00;
            pc_src     = 1'b0;
            result_src = 1'b1;
            mem_write  = 1'b0;
        end  // S-type instructions
        else if (opcode == 7'b0100011) begin
            // sw
            reg_write  = 1'b0;
            alu_ctrl   = 1'b0;  // 0 is used for addition (add immediate constant to address)
            alu_src    = 1'b1;
            imm_src    = 2'b10;
            pc_src     = 1'b0;
            result_src = 1'b0;
            mem_write  = 1'b1;
        end  // B-type instructions
        else if (opcode == 7'b1100011) begin
            // Conditional branches
            reg_write = 1'b0;
            alu_ctrl  = 1'b1;
            alu_src   = 1'b0;
            imm_src   = 2'b01;
            pc_src    = branch_taken;
            mem_write = 1'b0;
        end else begin
            reg_write = 1'b0;
            mem_write = 1'b0;
            pc_src    = 1'b0;
        end
    end

endmodule
