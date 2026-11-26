module cache #(
    parameter BYTE_OFFSET = 2,
    parameter SET_LENGTH = 2,
    parameter TAG_LENGTH = 28,
    parameter ADDR_WIDTH = 32,
    parameter DATA_WIDTH = 32,
    parameter WAYS       = 4
) (
    //some inputs copied directly from data_mem module as cache takes same inputs, only using main memory when necessary
    input logic  [ADDR_WIDTH-1:0] a,           // Address input
    input logic  [DATA_WIDTH-1:0] wd,          // Write data
    input logic                   we,          // Write enable
    input logic  [2:0]            data_ctrl,   // Data control (synonymous to funct3)
    input logic                   clk,
    input logic  [DATA_WIDTH-1:0] rd,          // Read data from data_mem

    output logic [DATA_WIDTH-1:0] data_out,    // Read data full output
    output logic [ADDR_WIDTH-1:0] D_a,         // Address to data_mem
    output logic [DATA_WIDTH-1:0] D_wd,        // Write data sent to data_mem
    output logic                  D_we,        // Write enable sent to data_mem
    output logic [2:0]            D_data_ctrl  // Data control sent to data_mem
);

    //internal arrays
    logic [WAYS-1:0]                    valid [2**SET_LENGTH-1:0];
    logic [TAG_LENGTH + DATA_WIDTH-1:0] cache [SET_LENGTH-1:0][WAYS-1:0];
    logic [$clog2(WAYS)]                time  [2**SET_LENGTH-1:0][WAYS-1:0];

    //represents different segments of addresses in a more readable manner
    logic hit;
    logic [TAG_LENGTH-1:0]  tag;
    logic [SET_LENGTH-1:0]  index;
    logic [BYTE_OFFSET-1:0] b_offset;
    logic [DATA_WIDTH-1:0]  word;

    logic [$clog2(WAYS):0] hit_match;   // which way matched
    logic [$clog2(WAYS):0] replace;     // which way to be replaced
    logic [$clog2(WAYS):0] current_way; // the way used this cycle

    assign tag      = a[ADDR_WIDTH-1 : BYTE_OFFSET + SET_LENGTH];
    assign index    = a[BYTE_OFFSET + SET_LENGTH-1 : BYTE_OFFSET];
    assign byte_off = a[BYTE_OFFSET-1 : 0]; 

    //assign hit = valid[index] && (tag == cache[index][TAG_LENGTH + DATA_WIDTH-1 : DATA_WIDTH]);

    always_comb begin
        hit     = 1'b0;
        hit_way = '0;
        replace  = '0;
        for (int i = 0; i < WAYS; i++) begin
            if (valid[index][i] && (tag == cache[index][i][TAG_LENGTH + DATA_WIDTH-1 : DATA_WIDTH])) begin
                hit     = 1'b1;
                hit_way = i[$clog2(WAYS)-1:0];
            end
            if (age[index][i] == WAYS-1)
                replace = i[$clog2(WAYS)-1:0];
        end
    end

    assign current_way = hit ? hit_way : replace; 

    initial begin
        //initialise every value in the cache to be 0 initially
        for (int i = 0; i < (2**SET_LENGTH); i++) begin
            for (int j = 0; j < WAYS; j++)
                cache[i][j] = '0;
                age[i][j]   = j;
        end
    end

    always_comb begin
        D_we <= we;
        D_wd <= wd;
        if (we) begin
            D_a         = a;                // stores: exact address and funct3 (write-through)
            D_data_ctrl = data_ctrl;
        end else begin
            D_a         = {a[ADDR_WIDTH-1:BYTE_OFFSET], {BYTE_OFFSET{1'b0}}};  // loads: whole aligned word
            D_data_ctrl = 3'b010;           // so a miss can fill the cache
        end

        if (hit) word = cache[index][hit_way][DATA_WIDTH-1:0];
        else     word = rd;         

        case (data_ctrl)
            3'b000: begin
                // lb (load byte, sign extend)
                data_out = {{24{word[8*byte_off+7]}}, word[8*byte_off +: 8]};
            end
            3'b001: begin
                // lh (load half word, sign extend)
                data_out = {{16{word[8*byte_off+15]}}, word[8*byte_off+8 +: 8], word[8*byte_off +: 8]};
            end
            3'b100: begin
                // lbu (load byte, zero extend)
                data_out = {24'b0, word[8*byte_off +: 8]};
            end
            3'b101: begin
                // lhu (load half word, zero extend)
                data_out = {16'b0, word[8*byte_off+8 +: 8], word[8*byte_off +: 8]};
            end
            default: begin
                // lw (load word)
                data_out = word;
                //note: got rid of the 3'b010 instruction as it is redundant with the default case
            end
        endcase
    end

    always_ff @(posedge clk) begin //sequential write
        if (we) begin
            if(hit) begin
                case (data_ctrl)
                    3'b000: begin
                        // sb (store byte)
                        cache[index][8*byte_off +: 8] <= wd[7:0];
                    end
                    3'b001: begin
                        // sh (store half word)
                        cache[index][8*byte_off+8 +: 8] <= wd[15:8];
                        cache[index][8*byte_off +: 8]   <= wd[7:0];
                    end
                    default: begin
                        // sw (store word)
                        cache[index][DATA_WIDTH-1:0] <= wd;
                        //note: got rid of the 3'b010 instruction as it is redundant with the default case
                    end
                endcase
            end
            else
                valid[index][replace] <= 1'b1;
                cache[index][replace] <= {tag, rd};
        end

        //LRU
        if (hit || !we) begin
            for (int i = 0; i < WAYS; i++)
                if (age[index][i] < age[index][current_way])
                    age[index][i] <= age[index][i] + 1'b1;
            age[index][current_way] <= '0;
        end


    end




endmodule
