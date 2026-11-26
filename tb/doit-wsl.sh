#!/bin/bash
# Run from Windows through WSL: bash tb/doit-wsl.sh
set -eu
cd "$(dirname "$0")"

if [[ -f "$HOME/.local/include/gtest/gtest.h" ]]; then
    export CPLUS_INCLUDE_PATH="$HOME/.local/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"
    export LIBRARY_PATH="$HOME/.local/lib${LIBRARY_PATH:+:$LIBRARY_PATH}"
fi

mkdir -p test_out
build_root=$(mktemp -d /tmp/team12-build.XXXXXX)
ln -s "$(realpath test_out)" "$build_root/output"
saved_files=$(mktemp -d)
for name in program data; do
    if [[ -f $name.hex ]]; then cp "$name.hex" "$saved_files/$name.hex"; fi
done
cleanup() {
    for name in program data; do
        if [[ -f $saved_files/$name.hex ]]; then
            cp "$saved_files/$name.hex" "$name.hex"
        else
            rm -f "$name.hex"
        fi
    done
    rm -f "$saved_files/program.hex" "$saved_files/data.hex"
    rmdir "$saved_files"
}
trap cleanup EXIT

run_suite() {
    local file=$1 name=$2 module=$2
    local rtl="../rtl/$name.sv"
    local extra=()
    local parameters=()
    case "$name" in
        data_mem) parameters=('-GINIT_FILE="tests/data.hex"') ;;
        pc) module=program_counter ;;
        top) extra=(../rtl/pc.sv) ;;
    esac
    local build="$build_root/output/obj_dir/$name"
    export WAVE_DIR="test_out/$name"
    mkdir -p "$build" "$WAVE_DIR" || return 1
    ln -sfn "$(realpath "$(dirname "$file")")" "$build/tests" || return 1
    verilator -Wall -Wno-fatal --trace --Mdir "$build" --top-module "$module" \
        -cc "$rtl" "${extra[@]}" "${parameters[@]}" -y ../rtl --exe "tests/$(basename "$file")" \
        --prefix Vdut -o Vdut -LDFLAGS "-lgtest -lgtest_main -lpthread" || return 1
    make -j -C "$build" -f Vdut.mk || return 1
    local result=0
    "$build/Vdut" || result=$?
    python3 merge_status.py "$WAVE_DIR" || return 1
    if [[ $name == top ]]; then python3 merge_status.py test_out || return 1; fi
    return "$result"
}

files=("$@")
if (( $# == 0 )); then
    files=()
    for file in tests/*.cpp; do
        files+=("$file")
    done
fi
status=0
for file in "${files[@]}"; do
    name=$(basename "$file" _tb.cpp | cut -f1 -d-)
    case "$name" in
        verify.cpp) name=top ;;
        cpu_regression) name=top ;;
    esac
    run_suite "$file" "$name" || status=1
done
exit "$status"
