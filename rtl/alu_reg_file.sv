module reg_file #(
    parameter DATA_WIDTH = 32,
    parameter ADDR_WIDTH = 5
) (
    input logic                    clk,
    input logic                    WE3,
    input logic [ADDR_WIDTH-1 : 0] AD1,
    AD2,
    AD3,
    input logic [DATA_WIDTH-1 : 0] WD3,

    output logic [DATA_WIDTH-1 : 0] a0,
    output logic [DATA_WIDTH-1 : 0] RD1,
    RD2
);


    logic [DATA_WIDTH - 1 : 0] reg_array[2 ** ADDR_WIDTH - 1 : 0];

    // sequential write (x0 fix)
    always_ff @(posedge clk) if (WE3) reg_array[AD3] <= WD3;

    // combinational read
    assign RD1 = reg_array[AD1];
    assign RD2 = reg_array[AD2];
    assign a0  = reg_array[10];

endmodule
