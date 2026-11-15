module alu
    #(
        parameter DATA_WIDTH = 32

    )
    (
        input logic [DATA_WIDTH-1 : 0]    alu1,
        input logic [DATA_WIDTH-1 : 0]    alu2,
        input logic                       alu_ctrl,
        
        output logic [DATA_WIDTH-1 : 0]   alu_out,   
        output logic                      eq
    );

    always_comb begin
        if (alu_ctrl == 1'b0)
            //addi
            alu_out = alu1 + alu2;
        //bne
        eq = (alu1 == alu2);
    end

endmodule