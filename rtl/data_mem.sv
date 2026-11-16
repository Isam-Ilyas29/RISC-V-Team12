module data_mem #(
    parameter ADDRESS_WIDTH = 9
) (
    input logic [31:0] a,   // ALU result
    input logic [31:0] wd,  // Write data (needed for sw but now lw extension of lab4)
    input logic        we,  // Write enable
    input logic        clk,

    output logic [31:0] rd  // Read data 
);

    logic [7:0] ram[2**ADDRESS_WIDTH-1:0];

    initial begin
        $readmemh("../rtl/data.hex", ram);
    end

    // Sequential write
    always @(posedge clk) begin
        if (we) begin
            ram[a[ADDRESS_WIDTH-1:0]+3] = wd[31:24];
            ram[a[ADDRESS_WIDTH-1:0]+2] = wd[23:16];
            ram[a[ADDRESS_WIDTH-1:0]+1] = wd[15:8];
            ram[a[ADDRESS_WIDTH-1:0]]   = wd[7:0];
        end
    end

    // Combinational read
    // 4 bytes for one word (32 bits) since RV32i is byte addressable
    assign rd = {
        {ram[a[ADDRESS_WIDTH-1:0]+3]},
        {ram[a[ADDRESS_WIDTH-1:0]+2]},
        {ram[a[ADDRESS_WIDTH-1:0]+1]},
        {ram[a[ADDRESS_WIDTH-1:0]]}
    };

endmodule
