module program_counter #(
    parameter WIDTH = 32
) (
    input  logic               clk,
    input  logic               rst,
    input  logic [        1:0] pc_ctrl,
    input  logic [WIDTH-1 : 0] immediate,
    input  logic [WIDTH-1 : 0] alu_result,
    output logic [WIDTH-1 : 0] pc,
    pc_plus_4
);

    logic [WIDTH-1 : 0] pc_increment, pc_branch, pc_jump;
    assign pc_increment = pc + WIDTH'(4);
    assign pc_branch = pc + immediate;
    assign pc_jump = {alu_result[WIDTH-1:1], 1'b0};  // RISC-V enforces bit 0 to be 0

    // Save PC result because this must be written to register rd
    assign pc_plus_4 = pc_increment;

    always_ff @(posedge clk)
        // Instruction memory stretches 0xBFC00000-0xBFC00FFF in the brief's memory map
        if (rst)
            pc <= 32'hBFC00000;
        else begin
            case (pc_ctrl)
                2'b00:   pc <= pc_increment;
                2'b01:   pc <= pc_branch;
                2'b10:   pc <= pc_jump;
                default: pc <= pc_increment;
            endcase
        end

endmodule
