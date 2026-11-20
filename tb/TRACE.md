# CPU overview trace

From the repository root, in WSL with the RISC-V toolchain, Verilator and Google Test available:

```sh
bash tb/trace.sh --open
```

Omit `--open` to generate the trace without launching GTKWave. The script restores the original `rtl/program.hex` after running.

The waveform is `tb/waves/top/CpuTestbench.Overview.vcd`; `tb/overview.gtkw` selects the PC, instruction, decoder, register operands, ALU, writeback and memory signals. Use Zoom Fit in GTKWave to see the entire execution. The full signal hierarchy remains available to add other signals.

`asm/overview.S` executes all six shifts, SW/LW, all six conditional branches taken, a JAL call, a JALR return and an unconditional jump into a terminal loop. The test checks a0 after every instruction; its final value is `fffffffe` (-2). The focused tests retain additional boundary and untaken-branch checks.

Register writes and PC updates occur on rising clock edges. The PC and combinational signals immediately after an edge describe the next instruction, while a0 reflects the instruction that just completed. Trace timestamps use two simulator ticks per cycle, not a measured hardware clock period.

Generated VCD files remain ignored by Git; the program, test and saved view reproduce the trace.
