# CPU testbenches

Supported environment: Windows running WSL. Apple-specific tracing guards are not used.

Each named test's VCD includes a `TEST_STATUS` scope. `check_valid` pulses for each instrumented equality check; `check_pass` is meaningful during that pulse. `check_id` counts checks from 1, with `actual` and `expected` shown as 64-bit buses. `source_line` identifies the assertion line. The adjacent `.vcd.checks.tsv` file maps each ID to its file, expression, time, values and result. `test_failed` latches on an equality failure; other Google Test failures are also reflected at teardown. `test_done` rises at completion and `test_pass` rises only if no failures occurred. Assertions still determine the process exit status. Status timestamps include observation ticks and are not a physical clock period.

`merge_status.py` combines the status events with the DUT trace automatically after each test executable. Open the resulting `.vcd`, expand `TEST_STATUS`, and add its signals. The overview GTKWave layout already includes them. The unchanged standalone control-unit tests have completion status only, not per-check instrumentation.

Run from the repository root in WSL with Verilator, Google Test and the RISC-V toolchain installed:

```sh
bash tb/doit.sh
bash tb/doit.sh tests/alu_tb.cpp
bash tb/trace.sh --open
```

The default suite excludes `control_unit_tb.cpp`. That existing file is unchanged and can still be run explicitly. CPU integration tests necessarily exercise the decoder as part of executing instructions.

| Test file | Checks |
|---|---|
| `alu_tb.cpp` | Add/subtract, bitwise operations, signed/unsigned comparisons, equality, shifts, amount masking and unsupported controls |
| `alu_top_tb.cpp` | Register/immediate operands, ALU/memory/link writeback, store data, x0 and branch operands |
| `branch_unit_tb.cpp` | All six comparisons, equality, signed/unsigned boundaries and reserved conditions |
| `reg_file_tb.cpp` | All register addresses, both read ports, x0, a0, write enable and edge timing |
| `mux_tb.cpp` | Both selections and changes without a selection transition |
| `pc_reg_tb.cpp` | Synchronous reset and rising-edge updates |
| `pc_top_tb.cpp` | Sequential/relative/register targets, negative offsets, bit-zero clearing and reset priority |
| `sign_extend_tb.cpp` | I/B/S/J immediates, sign extension and field boundaries |
| `instr_mem_tb.cpp` | Instruction fetch, byte order, address limits and current low-address-bit aliasing |
| `data_mem_tb.cpp` | Initial data, byte order, independent words, read/write timing, write enable and address limits |
| `verify.cpp` | Cycle-by-cycle CPU results for arithmetic, branches, shifts, jumps, memory and the combined overview |

`BaseTestbench` owns the DUT and trace. Each module fixture initializes its inputs; `sample()` evaluates combinational logic and records a sample. Clocked module tests explicitly drive clock edges. `cpu_testbench.h` provides CPU reset and instruction stepping on top of `testbench.h`'s clock driver. The CPU tests check a0 at each expected instruction boundary, including stability before the rising edge.

Every named test writes a separate VCD to `tb/waves/<module>/`. For example:

```sh
gtkwave tb/waves/data_mem/Memory.WriteEnableAndClockEdges.vcd
```

For a chosen case, set `GTEST_FILTER`, for example:

```sh
GTEST_FILTER='Memory.*' bash tb/doit.sh tests/data_mem_tb.cpp
```

The runner restores `rtl/program.hex` on exit. Assembly failures are checked before CPU execution; trace-open failures and unexpected simulation termination are test failures. The CPU driver enforces a cycle limit. The data-memory fixture is `tb/tests/data.hex`, selected only for its standalone test build.

These checks cover the current RTL, not the entire RV32I ISA or hardware timing. Memory uses only nine address bits: aliasing and byte-window reads document that implementation, not architectural exception support. Tests do not assume uninitialized registers or RAM contain zero. No misalignment traps, synthesis timing or formal proof are provided. The CPU-level checker observes a0; module tests check the other interfaces, and traces support diagnosis rather than replacing assertions.

ALU operation codes have shared names in `alu_controls.h`; module fixtures expose helpers such as `load`, `read`, `write`, `reset` and `step`.

The structure follows Imperial IAC Lecture 3 (stimulus, evaluation, checking and tracing) and Lecture 7 (separate datapath and control). The supplied example fixtures informed the setup/helpers/test separation; their different ALU encodings were not copied.
