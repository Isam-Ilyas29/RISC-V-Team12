module pc_reg #(
    parameter WIDTH = 32
)(
    //interface signals
    input   logic               clk,        //clock
    input   logic               rst,        //reset
    input   logic [WIDTH-1:0]   next_PC,    //Next PC
    output  logic [WIDTH-1:0]   PC          //PC OUT
);

always_ff @(posedge clk)
    if (rst) PC <= '0;
    else     PC <= next_PC;

endmodule
