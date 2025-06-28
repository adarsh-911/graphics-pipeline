# Compiler and flags
CXX := g++
CXXFLAGS := -g -O0 -std=c++17 \
    -I. -I./modelLoader -I./objLoader \
    -I./clipping -I./lightning \
    -I./texturing -I./vertexTransform -I./raster

# Project directories
SRC_DIRS := . modelLoader objLoader clipping lightning texturing vertexTransform raster

# Find all .cpp files in those directories
SOURCES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

SOURCES := $(filter-out ./testloadModels.cpp modelLoader/loadModels.cpp, $(SOURCES))
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

# Special target to build and run only loadModels.cpp
LOADMODELS_SRC := testloadModels.cpp modelLoader/loadModels.cpp modelLoader/model.cpp objLoader/objLoader.cpp
LOADMODELS_OBJ := $(LOADMODELS_SRC:.cpp=.o)
LOADMODELS_EXE := runLoadModels

$(LOADMODELS_EXE): $(LOADMODELS_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

loadmodels: $(LOADMODELS_EXE)

SOURCES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
# Clean rule
clean:
	rm -f $(OBJECTS) $(TARGET) $(LOADMODELS_EXE)

.PHONY: all clean loadmodels
