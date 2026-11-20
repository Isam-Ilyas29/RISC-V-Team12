# ALU tests

Supported environment: Windows through WSL. No Apple-specific tracing guards.

Run `bash tb/doitalu.sh` from the repository root. Requires the existing Verilator/Google Test setup and Python 3.

Each test writes `tb/waves/alu/Alu.<test-name>.vcd`. Open it in GTKWave and add the TEST_STATUS signals: check_valid, check_pass, check_id, actual, expected, test_failed, test_done and test_pass. check_pass is valid while check_valid is high; test_failed latches failures; test_pass is high only on successful completion. Assertions retain the failing exit status.

The tests use the current four-bit ALU encoding: ADD=0, SUB=1, AND=2, OR=3, XOR=4, SLT=5, SLTU=6, SLL=7, SRL=8, SRA=9. They cover arithmetic boundaries, equality, bitwise operations, signed/unsigned comparisons, shift amounts and unsupported controls. Only the ALU uses alu_testbench.h; other fixtures are unchanged.
