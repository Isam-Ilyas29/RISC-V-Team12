module control_unit (
    input logic         eq,
    input logic [  6:0] opcode,
    input logic [14:12] funct3,
    input logic         funct7,  // Only 1 bit is relevant in RV32i

    output logic       reg_write,   // 1 bit cause you either write or dont for we3
    output logic       alu_ctrl,    // 1 bit cause you only have 2 instructions
    output logic       alu_src,     // 1 bit cause 2 input mux
    output logic [1:0] imm_src,     // 1 bit cause you only have I-type and B-type instructions
    output logic       pc_src,      // 1 bit cause 2 input mux
    output logic       result_src,  // 1 bit cause either lw or addi currently
    output logic       mem_write,   // 1 bit cause you either write or dont in data memory
    output logic [2:0] data_ctrl    // 3 bits to reflect the 3 bits load and store utilise from funct3
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
            // Load instructions
            reg_write  = 1'b1;
            alu_ctrl   = 1'b0;  // 0 is used for addition (add immediate constant to address)
            alu_src    = 1'b1;
            imm_src    = 2'b00;
            pc_src     = 1'b0;
            result_src = 1'b1;
            mem_write  = 1'b0;

            case (funct3)
                // lb
                3'b000:  data_ctrl = 3'b000;
                // lh
                3'b001:  data_ctrl = 3'b001;
                // lw
                3'b010:  data_ctrl = 3'b010;
                // lbu
                3'b100:  data_ctrl = 3'b100;
                // lhu
                3'b101:  data_ctrl = 3'b101;
                //
                default: data_ctrl = 3'b010;
            endcase
        end  // S-type instructions
        else if (opcode == 7'b0100011) begin
            reg_write  = 1'b0;
            alu_ctrl   = 1'b0;  // 0 is used for addition (add immediate constant to address)
            alu_src    = 1'b1;
            imm_src    = 2'b10;
            pc_src     = 1'b0;
            result_src = 1'b0;
            mem_write  = 1'b1;
            case (funct3)
                // sb
                3'b000:  data_ctrl = 3'b000;
                // sh
                3'b001:  data_ctrl = 3'b001;
                // sw
                3'b010:  data_ctrl = 3'b010;
                //
                default: data_ctrl = 3'b010;
            endcase
        end  // B-type instructions
        else if (opcode == 7'b1100011) begin
            // bne
            reg_write = 1'b0;
            alu_ctrl  = 1'b1;
            alu_src   = 1'b0;
            imm_src   = 2'b01;
            if (eq == 0) begin
                pc_src = 1'b1;
            end else if (eq == 1'b1) begin
                pc_src = 1'b0;
            end
            mem_write = 1'b0;
        end else begin
            reg_write = 1'b0;
            mem_write = 1'b0;
            pc_src    = 1'b0;
        end
    end

endmodule
