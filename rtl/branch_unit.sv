module branch_unit #(
    parameter DATA_WIDTH = 32
) (
    input  logic [DATA_WIDTH-1:0] rs1_data,
    rs2_data,
    input  logic [           2:0] branch_ctrl,
    output logic                  branch_taken
);
    // branch_ctrl
    localparam BEQ  = 3'b000;
    localparam BNE  = 3'b001;
    localparam BLT  = 3'b100;
    localparam BGE  = 3'b101;
    localparam BLTU = 3'b110;
    localparam BGEU = 3'b111;

    always_comb begin
        case (branch_ctrl)
            BEQ: branch_taken = rs1_data == rs2_data;
            BNE: branch_taken = rs1_data != rs2_data;
            BLT: branch_taken = $signed(rs1_data) < $signed(rs2_data);
            BGE: branch_taken = $signed(rs1_data) >= $signed(rs2_data);
            BLTU: branch_taken = rs1_data < rs2_data;
            BGEU: branch_taken = rs1_data >= rs2_data;
            default: branch_taken = 1'b0;
        endcase
    end
endmodule
