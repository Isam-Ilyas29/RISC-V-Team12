#!/bin/bash
set -eu
cd "$(dirname "$0")"

saved_program=$(mktemp)
cp ../rtl/program.hex "$saved_program"
trap 'cp "$saved_program" ../rtl/program.hex; rm -f "$saved_program"' EXIT
GTEST_FILTER=CpuTestbench.Overview bash doit.sh tests/verify.cpp

if [[ ${1:-} == --open ]]; then
    gtkwave waves/top/CpuTestbench.Overview.vcd overview.gtkw
fi
