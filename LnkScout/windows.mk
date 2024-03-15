# windows.mk

# Compiler settings
CXX = g++
CXXFLAGS = -lole32 -luuid

# Target executable
TARGET = LnkScout.exe

# Source files
SOURCES = main.cpp

# Build rule
$(TARGET): $(SOURCES)
	$(CXX) $(SOURCES) -o $(TARGET) $(CXXFLAGS)

# Clean rule
clean:
	del $(TARGET)
