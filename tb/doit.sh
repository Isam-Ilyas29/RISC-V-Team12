#!/bin/bash

# This script runs the testbench
# Usage: ./doit.sh <file1.cpp> <file2.cpp>

# Constants
SCRIPT_DIR=$(dirname "$(realpath "$0")")
TEST_FOLDER=$(realpath "$SCRIPT_DIR/tests")
RTL_FOLDER=$(realpath "$SCRIPT_DIR/../rtl")
GREEN=$(tput setaf 2)
RED=$(tput setaf 1)
RESET=$(tput sgr0)

# Variables
passes=0
fails=0

# Handle terminal arguments
if [[ $# -eq 0 ]]; then
    # If no arguments provided, run all tests
    files=("${TEST_FOLDER}"/*.cpp)
else
    # If arguments provided, use them as input files
    files=("$@")
fi

cd "$SCRIPT_DIR" || exit 1

if [[ -f "$HOME/.local/include/gtest/gtest.h" ]]; then
    export CPLUS_INCLUDE_PATH="$HOME/.local/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"
    export LIBRARY_PATH="$HOME/.local/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"
fi

# Wipe previous test output
rm -rf test_out/*
mkdir -p test_out || exit 1
# Make needs a build path without spaces, even when shell arguments are quoted.
build_root=$(mktemp -d) || exit 1
mkdir "$build_root/obj_dir" || exit 1

# Iterate through files
for file in "${files[@]}"; do
    name=$(basename "$file" _tb.cpp | cut -f1 -d\-)

    # If verify.cpp -> we are testing the top module
    if [ $name == "verify.cpp" ] || [ $name == "cpu_regression" ]; then
        name="top"
    fi

    module="$name"
    extra_rtl=()
    parameters=()
    if [ $name == "data_mem" ]; then
        parameters=('-GINIT_FILE="tests/data.hex"')
    fi
    if [ $name == "pc" ]; then
        module="program_counter"
    fi
    if [ $name == "top" ]; then
        extra_rtl=(../rtl/pc.sv)
    fi

    export WAVE_DIR="test_out/$name"
    mkdir -p "$WAVE_DIR" || exit 1
    ln -sfn "$(realpath "$(dirname "$file")")" "$build_root/obj_dir/tests" || exit 1

    # Translate Verilog -> C++ including testbench
    verilator   -Wall -Wno-fatal --trace --top-module "$module" \
                --Mdir "$build_root/obj_dir" \
                -cc "../rtl/${name}.sv" "${extra_rtl[@]}" "${parameters[@]}" \
                --exe "tests/$(basename "$file")" \
                -y ../rtl \
                --prefix "Vdut" \
                -o Vdut \
                -LDFLAGS "-lgtest -lgtest_main -lpthread" || exit 1

    # Build C++ project with automatically generated Makefile
    make -j -C "$build_root/obj_dir" -f Vdut.mk || exit 1

    # Run executable simulation file
    "$build_root/obj_dir/Vdut"

    # Check if the test succeeded or not
    if [ $? -eq 0 ]; then
        ((passes++))
    else
        ((fails++))
    fi

done

python3 merge_status.py test_out || exit 1

# Save obj_dir in test_out
mv "$build_root/obj_dir" test_out/ || exit 1
rmdir "$build_root"
echo "$passes testbenches passed, $fails failed"
(( fails == 0 ))
