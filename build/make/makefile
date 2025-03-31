# Compiler
CXX = g++

# Directories
SRC_DIR := ../../src
INC_DIR := ../../inc
OBJ_DIR := obj
BIN_DIR := bin
TARGET := $(BIN_DIR)/MPM-Geomechanics

# Include paths (header files)
INC = -I$(SRC_DIR) -I$(INC_DIR)

# Source and object files
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Build configurations
# These are overridden depending on the build mode (debug or release)
CXXFLAGS_DEBUG   = -std=c++17 -Wall -g -O0 -fopenmp
CXXFLAGS_RELEASE = -std=c++17 -Wall -Wextra -O3 -march=native -flto -ffast-math -funroll-loops -DNDEBUG -fopenmp

LDFLAGS_DEBUG   = -fopenmp
LDFLAGS_RELEASE = -flto -fopenmp

# Default flags (can be overridden by targets)
CXXFLAGS ?= $(CXXFLAGS_RELEASE)
LDFLAGS  ?= $(LDFLAGS_RELEASE)

# Default target: release mode
all: release

# Release mode build
release: CXXFLAGS := $(CXXFLAGS_RELEASE)
release: LDFLAGS  := $(LDFLAGS_RELEASE)
release: $(TARGET)

# Debug mode build
debug: CXXFLAGS := $(CXXFLAGS_DEBUG)
debug: LDFLAGS  := $(LDFLAGS_DEBUG)
debug: $(TARGET)

# Linking|
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^

# Compilation of individual .cpp files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Clean generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
