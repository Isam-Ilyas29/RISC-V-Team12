#!/bin/bash
set -eu
cd "$(dirname "$0")"

if [[ -f "$HOME/.local/include/gtest/gtest.h" ]]; then
    export CPLUS_INCLUDE_PATH="$HOME/.local/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"
    export LIBRARY_PATH="$HOME/.local/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"
fi

build_root=obj_dir
if [[ $PWD == *" "* ]]; then
    build_root=$(mktemp -d)
    trap 'rm -rf -- "$build_root"' EXIT
fi

run_suite() {
    local file=$1 name=$2 module=$2
    local rtl="../rtl/$name.sv"
    local extra=()
    case "$name" in
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
        -cc "$rtl" "${extra[@]}" -y ../rtl --exe "tests/$(basename "$file")" \
        --prefix Vdut -o Vdut -LDFLAGS "-lgtest -lgtest_main -lpthread" || return 1
    make -j -C "$build" -f Vdut.mk || return 1
    "$build/Vdut"
}

files=("$@")
if (( $# == 0 )); then
    files=(tests/*.cpp)
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
