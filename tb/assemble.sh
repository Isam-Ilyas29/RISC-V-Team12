#!/bin/bash
# Run through WSL. Each program keeps its own disassembly in test_out.
set -eu
cd "$(dirname "$0")"
input=$1
name=$(basename "${input%.*}")
mkdir -p "test_out/$name"
work=$(mktemp -d)
trap 'rm -f "$work/program.o" "$work/program.elf" "$work/program.bin"; rmdir "$work"' EXIT
riscv64-unknown-elf-as -R -march=rv32im -mabi=ilp32 -o "$work/program.o" "$input"
riscv64-unknown-elf-ld -melf32lriscv -e 0xBFC00000 -Ttext 0xBFC00000 -o "$work/program.elf" "$work/program.o"
riscv64-unknown-elf-objcopy -O binary -j .text "$work/program.elf" "$work/program.bin"
riscv64-unknown-elf-objdump -f -d --source -m riscv "$work/program.elf" > "test_out/$name/program.dis"
od -v -An -t x1 "$work/program.bin" | awk '{$1=$1};1' > program.hex
