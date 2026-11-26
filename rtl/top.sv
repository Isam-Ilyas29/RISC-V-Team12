/*
                              +----------------+
                      +------>|  Control Unit  |<----------------------------+
                      |       +----------------+                             |
                      |                                                      | branch_taken
                      |                                                      |
+------+    +-------+ |  +-------+                                    +------------+
|      |    |       | |  |       |-rd1---+--------------------------->|   Branch   |
|  PC  |--->| Instr |-+->|  Reg  |       |             +------------->|    Unit    |
|      |    |  Mem  |    | File  |-rd2-----------+-----+              +------------+
+------+    +-------+    +-------+       |       |
   ^            |            ^           |       v
   |            |            |           |    +-----+     +-----+    +----------+  +--------+
   |            +-> Extend ------ imm ------->| Mux |---->|     |--+>| Data Mem |->|        |
   |                         |           |    +-----+     | ALU |  | +----------+  | Result |
   |                         |           +--------------->|     |  +-------------->|  Mux   |
   |                         |                            +-----+  |               +--------+
   |                         |                                     |                    |
   |                         |                                     |                    |
   |                         |                                     |                    |
   |                         +--------------- write back -------------------------------+
   |                                                               |
   +-------------------------- alu_result (jalr) ------------------+

*/

module top #(
    parameter DATA_WIDTH = 32
) (
    input  logic                  clk,
    input  logic                  rst,
    output logic [DATA_WIDTH-1:0] a0
);
    // Signals

    // PC
    logic [DATA_WIDTH-1:0] pc, pc_plus_4, pc_target;
    // Instruction
    logic [31:0] instr;
    // Control
    logic reg_write, alu_src, mem_write, branch_taken;
    logic [1:0] pc_ctrl;
    logic [2:0] imm_src, result_src, data_ctrl, branch_ctrl;
    logic [3:0] alu_ctrl;
    // Datapath
    logic [DATA_WIDTH-1:0] rd1, rd2, imm_ext, src_b, alu_result, read_data, result;

    // Next PC: PC+4 / PC+imm (branch) / ALU result (jump)
    program_counter pc_unit (
        .clk       (clk),
        .rst       (rst),
        .pc_ctrl   (pc_ctrl),
        .immediate (imm_ext),
        .alu_result(alu_result),
        .pc        (pc),
        .pc_plus_4 (pc_plus_4)
    );

    // PC + imm, used as the result for AUIPC
    assign pc_target = pc + imm_ext;

    // FETCH
    instr_mem imem (
        .a (pc),
        .rd(instr)
    );

    // DECODE
    control_unit cu (
        .branch_taken(branch_taken),
        .opcode      (instr[6:0]),
        .funct3      (instr[14:12]),
        .funct7      (instr[30]),
        .reg_write   (reg_write),
        .alu_src     (alu_src),
        .mem_write   (mem_write),
        .imm_src     (imm_src),
        .result_src  (result_src),
        .alu_ctrl    (alu_ctrl),
        .data_ctrl   (data_ctrl),
        .branch_ctrl (branch_ctrl),
        .pc_ctrl     (pc_ctrl)
    );

    reg_file rf (
        .clk(clk),
        .we3(reg_write),
        .ad1(instr[19:15]),
        .ad2(instr[24:20]),
        .ad3(instr[11:7]),
        .wd3(result),
        .a0 (a0),
        .rd1(rd1),
        .rd2(rd2)
    );

    sign_extend ext (
        .imm    (instr[31:7]),
        .imm_src(imm_src),
        .imm_op (imm_ext)
    );

    // EXECUTE
    branch_unit bu (
        .rs1_data    (rd1),
        .rs2_data    (rd2),
        .branch_ctrl (branch_ctrl),
        .branch_taken(branch_taken)
    );

    mux src_b_mux (
        .in0(rd2),
        .in1(imm_ext),
        .sel(alu_src),
        .out(src_b)
    );

    alu alu_unit (
        .alu1    (rd1),
        .alu2    (src_b),
        .alu_ctrl(alu_ctrl),
        .alu_out (alu_result),
        .eq      ()             // unused: branch_unit does the comparisons
    );

    // MEMORY
    data_mem dmem (
        .a        (alu_result),
        .wd       (rd2),
        .we       (mem_write),
        .data_ctrl(data_ctrl),
        .clk      (clk),
        .rd       (read_data)
    );

    // WRITEBACK

    always_comb begin
        case (result_src)
            3'b000:  result = alu_result;   // ALU ops
            3'b001:  result = read_data;    // loads
            3'b010:  result = pc_plus_4;    // jal / jalr
            3'b011:  result = imm_ext;      // lui
            3'b100:  result = pc_target;    // auipc
            default: result = alu_result;
        endcase
    end

endmodule
