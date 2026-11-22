module control_unit (
    input logic branch_taken,
    input logic [6:0] opcode, funct7,
    input logic [2:0] funct3,
    output logic reg_write, alu_src, pc_src, mem_write, jalr,
    output logic [3:0] alu_ctrl,
    output logic [2:0] imm_src, result_src
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
    localparam LUI   = 7'h37;
    localparam AUIPC = 7'h17;


    always_comb begin
        reg_write = 1'b0;
        alu_src = 1'b0;
        pc_src = 1'b0;
        mem_write = 1'b0;
        jalr = 1'b0;
        alu_ctrl = ADD;
        imm_src = 3'b000;
        result_src = 3'b000;

        case (opcode)
            ALU_I, ALU_R: begin                 // ALU instructions, I type and R type respectively
                alu_src = opcode == 7'h13;      //if true, instruction becomes I type
                case (funct3)
                    3'b000: begin                                       //funct7 differs between sub and add by 1 bit  
                        alu_ctrl  = (opcode == ALU_R && funct7 == 7'h20) ? SUB : ADD;        //check that instruction is valid   
                        reg_write = (funct7 == 7'h00) || (opcode == ALU_I) || (funct7 == 7'h20);
                    end
                    3'b001: begin
                        alu_ctrl  = SLL;
                        reg_write = funct7 == 7'h00;
                    end
                    3'b010: begin
                        alu_ctrl  = SLT;
                        reg_write = (funct7 == 7'h00) || (opcode == ALU_I);
                    end
                    3'b011: begin
                        alu_ctrl  = SLTU;
                        reg_write = (funct7 == 7'h00) || (opcode == ALU_I);
                    end
                    3'b100: begin
                        alu_ctrl  = XOR;
                        reg_write = (funct7 == 7'h00) || (opcode == ALU_I);
                    end
                    3'b101: begin
                        alu_ctrl  = funct7 == 7'h20 ? SRA : SRL;
                        reg_write = funct7 == 7'h00 || funct7 == 7'h20;
                    end
                    3'b110: begin
                        alu_ctrl  = OR;
                        reg_write = (funct7 == 7'h00) || (opcode == ALU_I);
                    end
                    3'b111: begin
                        alu_ctrl  = AND;
                        reg_write = (funct7 == 7'h00) || (opcode == ALU_I);
                    end
                    default: reg_write = 1'b0;
                endcase
            end
            LW: begin                    // lw instruction
                alu_src = 1'b1;
                result_src = 3'b001;
                reg_write = funct3 == 3'b010;
            end
            SW: begin                    // sw instruction
                alu_src = 1'b1;
                imm_src = 3'b010;
                mem_write = funct3 == 3'b010;
            end
            BEQ: begin                    // beq
                imm_src = 3'b001;
                pc_src = branch_taken && funct3 != 3'b010 && funct3 != 3'b011;
            end
            JAL: begin                    // jal instruction
                imm_src = 3'b011;
                result_src = 3'b010;
                reg_write = 1'b1;
                pc_src = 1'b1;
            end
            JALR: begin                    // jalr instruction
                alu_src = 1'b1;
                result_src = 3'b010;
                reg_write = funct3 == 3'b000;
                pc_src = funct3 == 3'b000;
                jalr = funct3 == 3'b000;
            end
            LUI: begin                    // lui instruction
                imm_src = 3'b100;
                result_src = 3'b011;
                reg_write = 1'b1;
            end
            AUIPC: begin                  // auipc instruction
                imm_src = 3'b100;
                result_src = 3'b100;
                reg_write = 1'b1;
            end
            default: begin
            end
        endcase
    end
endmodule
