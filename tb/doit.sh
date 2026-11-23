#!/bin/bash
set -eu
cd "$(dirname "$0")"

if [[ -f "$HOME/.local/include/gtest/gtest.h" ]]; then
    export CPLUS_INCLUDE_PATH="$HOME/.local/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"
    export LIBRARY_PATH="$HOME/.local/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"
fi

build_root=obj_dir
saved_program=$(mktemp)
cp ../rtl/program.hex "$saved_program"
cleanup() {
    cp "$saved_program" ../rtl/program.hex
    rm -f -- "$saved_program"
    if [[ $build_root != obj_dir ]]; then rm -rf -- "$build_root"; fi
}
trap cleanup EXIT
if [[ $PWD == *" "* ]]; then
    build_root=$(mktemp -d)
fi

run_suite() {
    local file=$1 name=$2 module=$2
    local rtl="../rtl/$name.sv"
    local extra=()
    local parameters=()
    case "$name" in
        data_mem) parameters=('-GINIT_FILE="tests/data.hex"') ;;
        reg_file) rtl=../rtl/alu_reg_file.sv ;;
        top) extra=(../rtl/alu_reg_file.sv) ;;
        alu_top) extra=(../rtl/alu_reg_file.sv) ;;
        lab4_cpu) rtl=lab4_cpu.sv; extra=(../rtl/alu_top.sv ../rtl/alu_reg_file.sv) ;;
    esac
    local build="$build_root/$name"
    export WAVE_DIR="waves/$name"
    mkdir -p "$build" "$WAVE_DIR" || return 1
    ln -sfn "$(realpath "$(dirname "$file")")" "$build/tests" || return 1
    verilator -Wall -Wno-fatal --trace --Mdir "$build" --top-module "$module" \
        -cc "$rtl" "${extra[@]}" "${parameters[@]}" -y ../rtl --exe "tests/$(basename "$file")" \
        --prefix Vdut -o Vdut -LDFLAGS "-lgtest -lgtest_main -lpthread" || return 1
    make -j -C "$build" -f Vdut.mk || return 1
    local result=0
    "$build/Vdut" || result=$?
    python3 merge_status.py "$WAVE_DIR" || return 1
    return "$result"
}

files=("$@")
if (( $# == 0 )); then
    files=()
    for file in tests/*.cpp; do
        [[ $file == tests/control_unit_tb.cpp ]] || files+=("$file")
    done
fi
status=0
for file in "${files[@]}"; do
    name=$(basename "$file" _tb.cpp | cut -f1 -d-)
    case "$name" in
        verify.cpp) name=top ;;
        lab4_cpu) file=tests/verify.cpp ;;
    esac
    run_suite "$file" "$name" || status=1
done
exit "$status"
