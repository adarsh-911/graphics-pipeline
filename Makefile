# Compiler and flags
CXX := g++
CXXFLAGS := -g -O0 -std=c++17 -Iclip -Iobj_loader -Ishader -Itexture -Ilightning -Ilocal_test

# Source files
SRC := $(wildcard ./*.cpp clip/*.cpp objLoader/*.cpp shader/*.cpp texturing/*.cpp lightning/*.cpp raster/*.cpp)

OBJ := $(SRC:.cpp=.o)

TARGET := out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)

