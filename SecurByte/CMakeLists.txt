# Define the minimum cmake version
cmake_minimum_required(VERSION 3.15)

# Define the project name and the language
project(SecurByte VERSION 1.0)

# Define the source files
set(SRC main.c aes.c sha256.c)

# Define the executable file name
set(EXE SecurByte)

# Add an executable target with the source files
add_executable(${EXE} ${SRC})
