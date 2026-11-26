#!/bin/bash
set -eu
bash "$(dirname "$0")/doit-wsl.sh" tests/alu_tb.cpp
