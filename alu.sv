module alu
    #(
        parameter DATA_WIDTH = 32,
        parameter ALU_CTRL_WIDTH = 4
    )
    (
        input logic [DATA_WIDTH-1 : 0]    alu1,
        input logic [DATA_WIDTH-1 : 0]    alu2,
        input logic [ALU_CTRL_WIDTH-1:0]    alu_ctrl,
        
        output logic [DATA_WIDTH-1 : 0]   alu_out, 
        output logic                      eq
    );

    logic [4:0] shamt = alu2[4:0];

    always_comb begin
        case(alu_ctrl)
        4'b0000: alu_out = alu1 + alu2;                         //add
        4'b0001: alu_out = alu1 - alu2;                         //sub
        4'b0010: alu_out = alu1 & alu2;                         //AND
        4'b0011: alu_out = alu1 | alu2;                         //OR
        4'b0100: alu_out = alu1 ^ alu2;                         //XOR
        4'b0101: alu_out = $signed(alu1) < $signed(alu2);       //SLT
        4'b0110: alu_out = $unsigned(alu1) < $unsigned(alu2);   //SLTU
        4'b0111: alu_out = alu1 << shamt;                       //SLL
        4'b1000: alu_out = alu1 >> shamt;                       //SRL
        4'b1001: alu_out = $signed(alu1) >>> shamt;             //SRA
        default: alu_out = 32'b0;                               //else
        endcase

        eq = (alu1 == alu2);
    end

endmodule