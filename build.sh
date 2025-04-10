#!/bin/bash
set -e

echo "Cleaning up build directory..."
rm -rf build
mkdir build && cd build

echo "Generating build files with compile_commands.json..."
cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make

[ -f compile_commands.json ] && cp compile_commands.json ..