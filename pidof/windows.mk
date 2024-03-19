# windows.mk

# Define the compiler and the flags
CXX = g++
CXXFLAGS = -Wall -std=gnu++17

# Define the target executable name
TARGET = pidof.exe

# Define the source file
SRC = main.cpp

# Define the object file
OBJ = $(SRC:.cpp=.o)

# Define the rule for building the target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Define the rule for generating the object file
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $<

# Define the rule for cleaning the intermediate files
clean:
	del $(OBJ)
