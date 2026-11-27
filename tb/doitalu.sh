#!/bin/bash
set -eu
bash "$(dirname "$0")/doit.sh" tests/alu_tb.cpp
