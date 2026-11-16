module data_mem #(
    parameter ADDRESS_WIDTH = 9
) (
    input logic [31:0] a,   // ALU result
    input logic [31:0] wd,  // Write data (needed for sw but now lw extension of lab4)
    input logic        we,  // Write enable
    input logic        clk,

    output logic [31:0] rd  // Read data 
);

    logic [7:0] data_mem_array[2**ADDRESS_WIDTH-1:0];

    initial begin
        $readmemh("../rtl/data.hex", data_mem_array);
    end

    // TODO: Sequential write (for sw)
    //always @(posedge clk) begin
    //end

    // Combinational read
    // 4 bytes for one word (32 bits) since RV32i is byte addressable
    assign rd = {
        {data_mem_array[a[ADDRESS_WIDTH-1:0]+3]},
        {data_mem_array[a[ADDRESS_WIDTH-1:0]+2]},
        {data_mem_array[a[ADDRESS_WIDTH-1:0]+1]},
        {data_mem_array[a[ADDRESS_WIDTH-1:0]]}
    };

endmodule
