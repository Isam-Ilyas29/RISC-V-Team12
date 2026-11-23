module data_mem #(
    parameter ADDRESS_WIDTH = 9,
    parameter INIT_FILE = "../rtl/data.hex"
) (
    input logic [31:0] a,          // ALU result
    input logic [31:0] wd,         // Write data
    input logic        we,         // Write enable
    input logic [ 2:0] data_ctrl,  // Data control (synonymous to funct3)
    input logic        clk,

    output logic [31:0] rd  // Read data 
);

    logic [7:0] ram[2**ADDRESS_WIDTH-1:0];

    initial begin
        if (INIT_FILE != "")
            $readmemh(INIT_FILE, ram);
    end

    // Sequential write
    always_ff @(posedge clk) begin
        if (we) begin
            case (data_ctrl)
                3'b000: begin
                    // sb (store byte)
                    ram[a[ADDRESS_WIDTH-1:0]] <= wd[7:0];
                end
                3'b001: begin
                    // sh (store half word)
                    ram[a[ADDRESS_WIDTH-1:0]+1] <= wd[15:8];
                    ram[a[ADDRESS_WIDTH-1:0]]   <= wd[7:0];
                end
                3'b010: begin
                    // sw (store word)
                    ram[a[ADDRESS_WIDTH-1:0]+3] <= wd[31:24];
                    ram[a[ADDRESS_WIDTH-1:0]+2] <= wd[23:16];
                    ram[a[ADDRESS_WIDTH-1:0]+1] <= wd[15:8];
                    ram[a[ADDRESS_WIDTH-1:0]]   <= wd[7:0];
                end
                default: begin
                    // sw (store word)
                    ram[a[ADDRESS_WIDTH-1:0]+3] <= wd[31:24];
                    ram[a[ADDRESS_WIDTH-1:0]+2] <= wd[23:16];
                    ram[a[ADDRESS_WIDTH-1:0]+1] <= wd[15:8];
                    ram[a[ADDRESS_WIDTH-1:0]]   <= wd[7:0];
                end
            endcase
        end
    end

    // Combinational read
    always_comb begin
        case (data_ctrl)
            3'b000: begin
                // lb (load byte, sign extend)
                rd = {{24{ram[a[ADDRESS_WIDTH-1:0]][7]}}, {ram[a[ADDRESS_WIDTH-1:0]]}};
            end
            3'b001: begin
                // lh (load half word, sign extend)
                rd = {
                    {{16{ram[a[ADDRESS_WIDTH-1:0]+1][7]}}}, {ram[a[ADDRESS_WIDTH-1:0]+1]}, {ram[a[ADDRESS_WIDTH-1:0]]}
                };
            end
            3'b010: begin
                // lw (load word)
                rd = {
                    {ram[a[ADDRESS_WIDTH-1:0]+3]},
                    {ram[a[ADDRESS_WIDTH-1:0]+2]},
                    {ram[a[ADDRESS_WIDTH-1:0]+1]},
                    {ram[a[ADDRESS_WIDTH-1:0]]}
                };
            end
            3'b100: begin
                // lbu (load byte, zero extend)
                rd = {24'b0, {ram[a[ADDRESS_WIDTH-1:0]]}};
            end
            3'b101: begin
                // lhu (load half word, zero extend)
                rd = {16'b0, {ram[a[ADDRESS_WIDTH-1:0]+1]}, {ram[a[ADDRESS_WIDTH-1:0]]}};
            end
            default: begin
                rd = {
                    {ram[a[ADDRESS_WIDTH-1:0]+3]},
                    {ram[a[ADDRESS_WIDTH-1:0]+2]},
                    {ram[a[ADDRESS_WIDTH-1:0]+1]},
                    {ram[a[ADDRESS_WIDTH-1:0]]}
                };
            end
        endcase
    end

endmodule
