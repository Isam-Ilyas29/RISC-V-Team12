#!/bin/bash
set -eu
cd "$(dirname "$0")"
result=0
bash doit.sh tests/alu_top_tb.cpp || result=$?
python3 merge_status.py waves/alu_top
exit "$result"
