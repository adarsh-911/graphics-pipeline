# Compiler and flags
CXX := g++
CXXFLAGS := -g -O0 -std=c++17 -I. -I./modelLoader -I./objLoader -I./clipping -I./lightning -I./texturing -I./vertexTransform -I./raster

# Project directories
SRC_DIRS := . modelLoader objLoader clipping lightning texturing vertexTransform raster

# Find all .cpp files in those directories
SOURCES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Create object file names by replacing .cpp with .o
OBJECTS := $(SOURCES:.cpp=.o)

# Final binary name
TARGET := main

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean rule
clean:
	rm -f $(OBJECTS) $(TARGET)

# Convenience rule
.PHONY: all clean

