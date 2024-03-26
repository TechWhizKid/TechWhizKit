# CMake version check
cmake_minimum_required(VERSION 3.13)

# CMake project name
project(UnRecover VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)

# Add main.c file of the project root directory as a source file
set(SOURCE_FILES main.c)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(UnRecover ${SOURCE_FILES})
