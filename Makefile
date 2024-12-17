#**************************************************************************************************
#
#   raylib makefile for Desktop platforms, Raspberry Pi, Android and HTML5
#
#   Copyright (c) 2013-2019 Ramon Santamaria (@raysan5)
#
#   This software is provided "as-is", without any express or implied warranty. In no event
#   will the authors be held liable for any damages arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose, including commercial
#   applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not claim that you
#     wrote the original software. If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be misrepresented
#     as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.
#
#**************************************************************************************************

# Define required raylib variables
PROJECT_NAME       ?= game
RAYLIB_VERSION     ?= 3.0.0
RAYLIB_API_VERSION ?= 300
RAYLIB_PATH        ?= ../raylib

# Define default options
# One of PLATFORM_DESKTOP, PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
PLATFORM           ?= PLATFORM_DESKTOP

# Default build mode
BUILD_MODE         ?= DEBUG

# Define default C++ compiler
CXX = g++

# Detect platform
ifeq ($(OS),Windows_NT)
    PLATFORM_OS = WINDOWS
else
    UNAMEOS = $(shell uname)
    ifeq ($(UNAMEOS),Linux)
        PLATFORM_OS = LINUX
    endif
    ifeq ($(UNAMEOS),Darwin)
        PLATFORM_OS = OSX
    endif
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
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external

# Library paths and libraries
LDFLAGS = -L. -L$(RAYLIB_PATH)/src -fopenmp

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
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Target
$(PROJECT_NAME)$(EXT): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS)

# Compile source files
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDE_PATHS)

# Phony targets
.PHONY: all clean

all: $(PROJECT_NAME)$(EXT)

clean:
ifeq ($(PLATFORM_OS),WINDOWS)
	del *.o *.exe
else
	rm -f *.o $(PROJECT_NAME)$(EXT)
endif
