@echo off
setlocal

rem Create build directories if they don't exist
if not exist "build_vs2022" mkdir build_vs2022
if not exist "build_make" mkdir build_make

rem Generate Visual Studio 2022 build files using CMake
cmake -S . -B build_vs2022 -G "Visual Studio 17 2022"
if errorlevel 1 (
    echo Failed to generate Visual Studio 2022 build files.
    exit /b 1
)

rem Generate Makefile build files using MinGW Makefiles
cmake -S . -B build_make -G "MinGW Makefiles"
if errorlevel 1 (
    echo Failed to generate Makefile.
    exit /b 1
)

echo Build files generated successfully.
endlocal
pause