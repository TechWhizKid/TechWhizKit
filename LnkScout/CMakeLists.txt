cmake_minimum_required(VERSION 3.0)

# Define the project name
project(LnkScout)

# Add an executable to the project using the specified source files
add_executable(main main.cpp)

# Link the target to the libraries
target_link_libraries(main ole32 uuid)
