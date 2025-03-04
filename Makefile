# Define required raylib variables
PROJECT_NAME       ?= game
RAYLIB_VERSION     ?= 5.5
RAYLIB_PATH        ?= $(RAYLIB_PATH)

# Define source and build directories
SRC_DIR   = src
OBJ_DIR   = obj

# Define default options
PLATFORM           ?= PLATFORM_DESKTOP
BUILD_MODE         ?= DEBUG

# Define default C++ compiler
CXX = g++

# Detect platform
ifeq ($(OS),Windows_NT)
	PLATFORM_OS = WINDOWS
	MKDIR = mkdir
	RM = del /Q /F
	RM_DIR = rmdir /Q /S
	SEP = \\
else
	UNAMEOS = $(shell uname)
	ifeq ($(UNAMEOS),Linux)
	    PLATFORM_OS = LINUX
	endif
	ifeq ($(UNAMEOS),Darwin)
	    PLATFORM_OS = OSX
	endif
	MKDIR = mkdir -p
	RM = rm -f
	RM_DIR = rm -rf
	SEP = /
endif

# Compiler flags
CXXFLAGS = -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces -fopenmp

# Optimization and debug flags
ifeq ($(BUILD_MODE),DEBUG)
	CXXFLAGS += -g -O0
else
	CXXFLAGS += -s -O2
endif

# Include paths
INCLUDE_PATHS = -I$(SRC_DIR) -I$(RAYLIB_PATH)/include

# Library paths and libraries
LDFLAGS = -L. -L$(RAYLIB_PATH)/lib -fopenmp

# Platform-specific settings
ifeq ($(PLATFORM_OS),WINDOWS)
	LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
	EXT = .exe
endif

ifeq ($(PLATFORM_OS),LINUX)
	LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(PLATFORM_OS),OSX)
	LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit
endif

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Target
$(PROJECT_NAME)$(EXT): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDE_PATHS)

# Phony targets
.PHONY: all clean

all: $(PROJECT_NAME)$(EXT)

clean:
ifeq ($(PLATFORM_OS),WINDOWS)
	@if exist $(OBJ_DIR) $(RM_DIR) $(OBJ_DIR)
	@$(RM) $(PROJECT_NAME)$(EXT)
else
	$(RM_DIR) $(OBJ_DIR)
	$(RM) $(PROJECT_NAME)$(EXT)
endif