module lab4_cpu(input logic clk, rst, output logic [31:0] a0);
    top cpu(.clk(clk), .rst(rst), .a0(a0));
endmodule
