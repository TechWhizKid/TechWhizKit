cmake_minimum_required(VERSION 3.0)

# Define the project name
project(BackToIP VERSION 1.0)

# Add an executable to the project using the specified source files
add_executable(main main.cpp)

# Link the target to the libraries
target_link_libraries(main lws2_32)
