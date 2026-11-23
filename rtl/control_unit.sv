module control_unit (
    input logic branch_taken,
    input logic [6:0] opcode,
    input logic funct7, // Only 1 bit is relevant in RV32i
    input logic [2:0] funct3,
    output logic reg_write, alu_src, pc_src, mem_write, jalr,
    output logic [3:0] alu_ctrl,
    output logic [1:0] imm_src, result_src,
    output logic [2:0] data_ctrl    // 3 bits to reflect the 3 bits load and store utilise from funct3
);
    //alu_ctrl parameters
    localparam ADD  = 4'd0;
    localparam SUB  = 4'd1;
    localparam AND  = 4'd2;
    localparam OR   = 4'd3;
    localparam XOR  = 4'd4;
    localparam SLT  = 4'd5;
    localparam SLTU = 4'd6;
    localparam SLL  = 4'd7; 
    localparam SRL  = 4'd8; 
    localparam SRA  = 4'd9;

    //opcode values
    localparam ALU_I = 7'h13;
    localparam ALU_R = 7'h33;
    localparam LW    = 7'h03;
    localparam SW    = 7'h23;
    localparam BEQ   = 7'h63;
    localparam JAL   = 7'h6f;
    localparam JALR  = 7'h67;


    always_comb begin
        reg_write = 1'b0;
        alu_src = 1'b0;
        pc_src = 1'b0;
        mem_write = 1'b0;
        jalr = 1'b0;
        alu_ctrl = ADD;
        imm_src = 2'b00;
        result_src = 2'b00;

        case (opcode)
            ALU_I, ALU_R: begin                                                                     // ALU instructions, I type and R type respectively
                alu_src = opcode == 7'h13;                                                          //if true, instruction becomes I type
                case (funct3)
                    3'b000: begin 
                        alu_ctrl  = funct7 == 1'b1 ? SUB : ADD;                                    //funct7 differs between sub and add by 1 bit                      
                        reg_write = (funct7 == 1'b0) || (opcode=ALU_I) || (funct7 == 1'b1);       //check that instruction is valid
                    end
                    3'b001: begin
                        alu_ctrl  = SLL;
                        reg_write = funct7 == 1'b0;
                    end
                    3'b010: begin
                        alu_ctrl  = SLT;
                        reg_write = (funct7 == 1'b0) || (opcode == ALU_I);
                    end
                    3'b011: begin
                        alu_ctrl  = SLTU;
                        reg_write = (funct7 == 1'b0) || (opcode=ALU_I);
                    end
                    3'b100: begin
                        alu_ctrl  = XOR;
                        reg_write = (funct7 == 1'b0) || (opcode=ALU_I);
                    end
                    3'b101: begin
                        alu_ctrl  = funct7 == 1'b1 ? SRA : SRL;
                        reg_write = funct7 == 1'b0 || funct7 == 1'b1;
                    end
                    3'b110: begin
                        alu_ctrl  = OR;
                        reg_write = (funct7 == 1'b0) || (opcode=ALU_I);
                    end
                    3'b111: begin
                        alu_ctrl  = AND;
                        reg_write = (funct7 == 1'b0) || (opcode=ALU_I);
                    end
                    default: reg_write = 1'b0;
                endcase
            end
            LW: begin                    // lw instruction
              // Load instructions
              reg_write  = 1'b1;
              alu_src    = 1'b1;
              result_src = 1'b1;

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
            end
            SW: begin                    // sw instruction
              alu_src    = 1'b1;
              imm_src    = 2'b10;
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
            end
            BEQ: begin                    // beq
                imm_src = 2'b01;
                pc_src = branch_taken && funct3 != 3'b010 && funct3 != 3'b011;
            end
            JAL: begin                    // jal instruction
                imm_src = 2'b11;
                result_src = 2'b10;
                reg_write = 1'b1;
                pc_src = 1'b1;
            end
            JALR: begin                    // jalr instruction
                alu_src = 1'b1;
                result_src = 2'b10;
                reg_write = funct3 == 3'b000;
                pc_src = funct3 == 3'b000;
                jalr = funct3 == 3'b000;
            end
            default: begin
            end
        endcase
    end
endmodule
