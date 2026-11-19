module top #(
    DATA_WIDTH = 32
) (
    input  logic                  clk,
    input  logic                  rst,
    output logic [DATA_WIDTH-1:0] a0
);
    assign a0 = 32'd5;
endmodule

// TODO: add a mux driven by ResultSrc for wd3 register file input (see page 13: https://elec50010-iac.github.io/course-website/lectures/Lecture%207%20-%20Microarchitecture%20(notes).pdf)
