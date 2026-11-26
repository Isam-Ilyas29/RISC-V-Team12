#!/bin/bash
# The integrated datapath is now in top.sv.
set -eu
bash "$(dirname "$0")/doit-wsl.sh" tests/cpu_regression_tb.cpp
