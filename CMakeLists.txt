cmake_minimum_required(VERSION 3.10)
project(6502CPU VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debug option
option(CPU_DEBUG "Enable CPU debug output" OFF)

# Add source files
set(SOURCES
    main.cpp
    CPU65C02.cpp
)

# Add header files
set(HEADERS
    CPU65C02.h
)

# Create executable
add_executable(6502cpu ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(6502cpu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Add debug definition if enabled
if(CPU_DEBUG)
    target_compile_definitions(6502cpu PRIVATE CPU_DEBUG=1)
endif()

# Set compiler flags
if(MSVC)
    target_compile_options(6502cpu PRIVATE /W4)
else()
    target_compile_options(6502cpu PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional: Enable debug symbols
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(6502cpu PRIVATE /Zi)
    else()
        target_compile_options(6502cpu PRIVATE -g)
    endif()
endif()

# Optional: Add compiler warnings as errors
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
if(WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(6502cpu PRIVATE /WX)
    else()
        target_compile_options(6502cpu PRIVATE -Werror)
    endif()
endif() 