# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17  -Iinclude
# Source and output
SRC_DIR := .
BUILD_DIR := build
TARGET := db

# Find all .cpp files recursively
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: $(TARGET)

# Link the final binary
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile each cpp file to an object file
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
