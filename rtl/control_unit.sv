module control_unit (
    input logic branch_taken,
    input logic [6:0] opcode, funct7,
    input logic [2:0] funct3,
    output logic reg_write, alu_src, pc_src, mem_write, jalr,
    output logic [3:0] alu_ctrl,
    output logic [1:0] imm_src, result_src
);
    localparam ADD = 4'd0, SLL = 4'd7, SRL = 4'd8, SRA = 4'd9;

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
            7'h13, 7'h33: begin
                alu_src = opcode == 7'h13;
                case (funct3)
                    3'b000: reg_write = opcode == 7'h13;
                    3'b001: begin
                        alu_ctrl = SLL;
                        reg_write = funct7 == 7'h00;
                    end
                    3'b101: begin
                        alu_ctrl = funct7 == 7'h20 ? SRA : SRL;
                        reg_write = funct7 == 7'h00 || funct7 == 7'h20;
                    end
                    default: reg_write = 1'b0;
                endcase
            end
            7'h03: begin
                alu_src = 1'b1;
                result_src = 2'b01;
                reg_write = funct3 == 3'b010;
            end
            7'h23: begin
                alu_src = 1'b1;
                imm_src = 2'b10;
                mem_write = funct3 == 3'b010;
            end
            7'h63: begin
                imm_src = 2'b01;
                pc_src = branch_taken && funct3 != 3'b010 && funct3 != 3'b011;
            end
            7'h6f: begin
                imm_src = 2'b11;
                result_src = 2'b10;
                reg_write = 1'b1;
                pc_src = 1'b1;
            end
            7'h67: begin
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
