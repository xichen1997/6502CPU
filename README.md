# 6502 CPU Emulator

A C++ implementation of a 6502 CPU emulator.

## Prerequisites

- CMake (version 3.10 or higher)
- C++ compiler with C++17 support
  - GCC 7 or higher
  - Clang 5 or higher
  - MSVC 2017 or higher

## Building the Project

### Using CMake (Recommended)

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Generate build files:
```bash
cmake ..
```

3. Build the project:
```bash
cmake --build .
```

### Build Options

- To build in Debug mode:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

- To treat warnings as errors:
```bash
cmake -DWARNINGS_AS_ERRORS=ON ..
```

### Running the Program

After building, the executable will be created in your build directory. You can run it with:
```bash
./6502cpu
```

## Project Structure

- `main.cpp` - Main program entry point
- `CPU65C02.h` - CPU class declaration
- `CPU65C02.cpp` - CPU class implementation
- `CMakeLists.txt` - CMake build configuration

## Features

- Implements basic 6502 CPU instructions
- Supports various addressing modes
- Memory management
- Register operations
- Status flag handling

## Introduction

This project is an emulator for the 6502 microprocessor. It is written in C++ and is designed to be easy to understand and modify.

The project is inspired by Ben Eater's 6502 tutorial. The final goal is to create a fully functional emulator that can run any 6502 program.

All parameters for 6502 cpu can be found here:
[text](https://www.westerndesigncenter.com/wdc/documentation/w65c02s.pdf)


## How to run

```
g++ main.cpp -o main
./main
```

Add `-DDEBUG` to the command line to enable debug mode.
Add `-DSINGLE_STEP` to the command line to enable single step mode.
```
g++ main.cpp -o main -DDEBUG -DSINGLE_STEP
```

