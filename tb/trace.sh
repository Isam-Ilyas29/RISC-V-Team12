#!/bin/bash
set -eu
cd "$(dirname "$0")"
GTEST_FILTER=CpuRegression.Overview bash doit.sh tests/cpu_regression_tb.cpp
if [[ ${1:-} == --open ]]; then
    gtkwave test_out/overview/waveform.vcd overview.gtkw
fi
