module control_unit (
    input logic       branch_taken,  // Zero flag
    input logic [6:0] opcode,
    input logic [2:0] funct3,
    input logic       funct7,        // Only 1 bit is relevant in RV32i

    output logic       reg_write,
    alu_src,
    mem_write,
    jalr,
    output logic [2:0] imm_src,
    result_src,
    output logic [3:0] alu_ctrl,
    output logic [2:0] data_ctrl,    // 3 bits to reflect the 3 bits load and store utilise from funct3
    output logic [2:0] branch_ctrl,  // 3 bits because 6 branch types
    output logic [1:0] pc_ctrl
);

    // opcode values
    localparam ALU_I = 7'b0010011;
    localparam ALU_R = 7'b0110011;
    localparam LOAD    = 7'b0000011;
    localparam STORE    = 7'b0100011;
    localparam BRANCH   = 7'b1100011;
    localparam JAL   = 7'b1101111;
    localparam JALR  = 7'b1100111;
    localparam LUI = 7'b0110111;
    localparam AUIPC = 7'b0010111;

    // alu_ctrl parameters
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

    // data_ctrl parameters
    localparam LB = 3'b000;
    localparam LH = 3'b001;
    localparam LW = 3'b010;
    localparam LBU = 3'b100;
    localparam LHU = 3'b101;
    localparam SB = 3'b000;
    localparam SH = 3'b001;
    localparam SW = 3'b010;

    // branch_ctrl parameters
    localparam BEQ = 3'b000;
    localparam BNE = 3'b001;
    localparam BLT = 3'b100;
    localparam BGE = 3'b101;
    localparam BLTU = 3'b110;
    localparam BGEU = 3'b111;

    // pc_ctrl parameters
    localparam PC_INCREMENT = 2'b00;
    localparam PC_BRANCH= 2'b01;
    localparam PC_JUMP = 2'b10;

    always_comb begin
        reg_write = 1'b0;
        alu_src = 1'b0;
        pc_ctrl = PC_INCREMENT;
        mem_write = 1'b0;
        jalr = 1'b0;
        alu_ctrl = ADD;
        imm_src = 3'b000;
        result_src = 2'b00;
        data_ctrl = 3'b010;

        case (opcode)
            // ALU instructions (I type and R type respectively)
            ALU_I, ALU_R: begin
                alu_src = opcode == ALU_I;  // if true, instruction becomes I type counterpart
                case (funct3)
                    // add / sub
                    3'b000: begin
                        // Bit 30 selects SUB only for register instructions
                        alu_ctrl  = (opcode == ALU_R && funct7) ? SUB : ADD;
                        reg_write = 1'b1;
                    end
                    // sll
                    3'b001: begin
                        alu_ctrl  = SLL;
                        reg_write = funct7 == 1'b0;
                    end
                    // slt
                    3'b010: begin
                        alu_ctrl  = SLT;
                        reg_write = (funct7 == 1'b0) || (opcode == ALU_I);
                    end
                    // sltu
                    3'b011: begin
                        alu_ctrl  = SLTU;
                        reg_write = (funct7 == 1'b0) || (opcode == ALU_I);
                    end
                    // xor
                    3'b100: begin
                        alu_ctrl  = XOR;
                        reg_write = (funct7 == 1'b0) || (opcode == ALU_I);
                    end
                    // srl / sra
                    3'b101: begin
                        alu_ctrl  = funct7 == 1'b1 ? SRA : SRL;
                        reg_write = funct7 == 1'b0 || funct7 == 1'b1;  // Always 1
                    end
                    // or
                    3'b110: begin
                        alu_ctrl  = OR;
                        reg_write = (funct7 == 1'b0) || (opcode == ALU_I);
                    end
                    // and
                    3'b111: begin
                        alu_ctrl  = AND;
                        reg_write = (funct7 == 1'b0) || (opcode == ALU_I);
                    end
                    default: begin
                        alu_ctrl  = ADD;
                        reg_write = 1'b0;
                    end
                endcase
            end
            // Load instructions (I-type)
            LOAD: begin
                reg_write  = 1'b1;
                alu_src    = 1'b1;
                result_src = 2'b01;

                case (funct3)
                    // lb
                    3'b000:  data_ctrl = LB;
                    // lh
                    3'b001:  data_ctrl = LH;
                    // lw
                    3'b010:  data_ctrl = LW;
                    // lbu
                    3'b100:  data_ctrl = LBU;
                    // lhu
                    3'b101:  data_ctrl = LHU;
                    // default to lw
                    default: data_ctrl = LW;
                endcase
            end
            // Store instructions (S-type)
            STORE: begin
                alu_src   = 1'b1;
                imm_src   = 3'b010;
                mem_write = 1'b1;

                case (funct3)
                    // sb
                    3'b000:  data_ctrl = SB;
                    // sh
                    3'b001:  data_ctrl = SH;
                    // sw
                    3'b010:  data_ctrl = SW;
                    // default to sw
                    default: data_ctrl = SW;
                endcase
            end
            // Branch instructions (B-type)
            BRANCH: begin
                imm_src = 3'b001;
                // 010 and 011 are undefined branches which is why we check against them
                if (branch_taken && funct3 != 3'b010 && funct3 != 3'b011) pc_ctrl = PC_BRANCH;

                case (funct3)
                    // beq
                    3'b000:  branch_ctrl = BEQ;
                    // bne
                    3'b001:  branch_ctrl = BNE;
                    // blt
                    3'b100:  branch_ctrl = BLT;
                    // bge
                    3'b101:  branch_ctrl = BGE;
                    // bltu
                    3'b110:  branch_ctrl = BLTU;
                    // bgeu
                    3'b111:  branch_ctrl = BGEU;
                    //
                    default: branch_ctrl = BEQ;
                endcase
            end
            // Jump instructions (J=type)
            JAL: begin
                imm_src = 3'b011;
                result_src = 2'b10;
                reg_write = 1'b1;
                pc_ctrl = PC_BRANCH;
            end
            JALR: begin
                alu_src = 1'b1;
                result_src = 2'b10;
                reg_write = funct3 == 3'b000;
                if (funct3 == 3'b000) pc_ctrl = PC_JUMP;
                jalr = funct3 == 3'b000;
            end
            // Upper immediate instructions (U-type)
            LUI: begin
                imm_src = 3'b100;
                result_src = 3'b011;
                reg_write = 1'b1;
            end
            AUIPC: begin
                imm_src = 3'b100;
                result_src = 3'b100;
                reg_write = 1'b1;
            end
            default: begin
            end
        endcase
    end
endmodule
