# windows.mk

# Compiler settings
CXX = g++
CXXFLAGS = -lws2_32

# Target executable
TARGET = BackToIP.exe

# Source files
SOURCES = main.cpp

# Build rule
$(TARGET): $(SOURCES)
	$(CXX) $(SOURCES) -o $(TARGET) $(CXXFLAGS)

# Clean rule
clean:
	del $(TARGET)
