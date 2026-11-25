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

    // data_ctrl
    localparam LB  = 3'b000;
    localparam LH  = 3'b001;
    localparam LW  = 3'b010;
    localparam LBU = 3'b100;
    localparam LHU = 3'b101;
    localparam SB  = 3'b000;
    localparam SH  = 3'b001;
    localparam SW  = 3'b010;

    logic [7:0] ram[2**ADDRESS_WIDTH-1:0];

    initial begin
        if (INIT_FILE != "") $readmemh(INIT_FILE, ram);
    end

    // Sequential write
    always_ff @(posedge clk) begin
        if (we) begin
            case (data_ctrl)
                SB: begin
                    // sb (store byte)
                    ram[a[ADDRESS_WIDTH-1:0]] <= wd[7:0];
                end
                SH: begin
                    // sh (store half word)
                    ram[a[ADDRESS_WIDTH-1:0]+1] <= wd[15:8];
                    ram[a[ADDRESS_WIDTH-1:0]]   <= wd[7:0];
                end
                SW: begin
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
            LB: begin
                // lb (load byte, sign extend)
                rd = {{24{ram[a[ADDRESS_WIDTH-1:0]][7]}}, {ram[a[ADDRESS_WIDTH-1:0]]}};
            end
            LH: begin
                // lh (load half word, sign extend)
                rd = {
                    {{16{ram[a[ADDRESS_WIDTH-1:0]+1][7]}}}, {ram[a[ADDRESS_WIDTH-1:0]+1]}, {ram[a[ADDRESS_WIDTH-1:0]]}
                };
            end
            LW: begin
                // lw (load word)
                rd = {
                    {ram[a[ADDRESS_WIDTH-1:0]+3]},
                    {ram[a[ADDRESS_WIDTH-1:0]+2]},
                    {ram[a[ADDRESS_WIDTH-1:0]+1]},
                    {ram[a[ADDRESS_WIDTH-1:0]]}
                };
            end
            LBU: begin
                // lbu (load byte, zero extend)
                rd = {24'b0, {ram[a[ADDRESS_WIDTH-1:0]]}};
            end
            LHU: begin
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
