@echo off
setlocal enabledelayedexpansion

echo Cleaning up all generated files...
if exist build rmdir /s /q build
if exist compile_commands.json del /f /q compile_commands.json

echo Generating project build with compile_commands.json...
mkdir build
cd build
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
if exist compile_commands.json copy /Y compile_commands.json ..
cd ..

echo Generating project files for latest IDEs...

echo Generating Visual Studio 2022 project files...
mkdir build_vs2022
cmake -G "Visual Studio 17 2022" -B build_vs2022