module reg_file #(
    parameter DATA_WIDTH = 32,
    parameter ADDRESS_WIDTH = 5
) (
    input logic                    clk,
    input logic                    we3,
    input logic [ADDR_WIDTH-1 : 0] ad1,
    ad2,
    ad3,
    input logic [DATA_WIDTH-1 : 0] wd3,

    output logic [DATA_WIDTH-1 : 0] a0,
    output logic [DATA_WIDTH-1 : 0] rd1,
    rd2
);


    logic [DATA_WIDTH - 1 : 0] reg_array[2 ** ADDRESS_WIDTH - 1 : 0];

    // sequential write (x0 fix)
    always_ff @(posedge clk) if (we3 && ad3 != 0) reg_array[ad3] <= wd3;

    // combinational read
    assign rd1 = (ad1 == 0) ? '0 : reg_array[ad1];
    assign rd2 = (ad2 == 0) ? '0 : reg_array[ad2];
    assign a0  = reg_array[10];

endmodule
