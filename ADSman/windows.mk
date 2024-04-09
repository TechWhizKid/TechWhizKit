# windows.mk

# Define the compiler
CXX = g++

# Target executable
TARGET = ADSman.exe

# Source files
SOURCES = main.cpp

# Build rule
$(TARGET): $(SOURCES)
	$(CXX) $(SOURCES) -o $(TARGET)

# Clean rule
clean:
	del $(TARGET)
