# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(pidof VERSION 1.1)

# Set the C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the target executable name
set(TARGET pidof.exe)

# Define the source file
set(SRC main.cpp)

# Add an executable target with the source file
add_executable(${TARGET} ${SRC})
