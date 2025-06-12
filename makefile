# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Iclip -Iobj_loader -Ishader -Itexture -Ilightning

# Source files
SRC := $(wildcard clip/*.cpp obj_loader/*.cpp shader/*.cpp texturing/*.cpp lightning/*.cpp render/*.cpp)

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

