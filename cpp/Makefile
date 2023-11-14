# # CPP compiler
# CC = g++

# SRC = src
# TARGET = kod

# CFLAGS = -Wall -ggdb -std=c++17

# DEPFLAGS = -I./include

# rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
# CXXSRC = $(call rwildcard,$(SRC),*.cpp)

# # compile C source
# COMPILE.cpp = $(CC) $(DEPFLAGS) $(CFLAGS) -o ${TARGET}

# TARGET:
# 	$(COMPILE.cpp) ${CXXSRC}

CXX := g++ # Compiler
SRCDIR := src # Source code directory
BUILDDIR := build # Build directory
TARGET := kod # Name of the executable

# Get all .cpp files in the source directory
SRCEXT := cpp
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))

# Create a list of corresponding object files in the build directory
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Compiler flags
CXXFLAGS := -std=c++17 -Wall -ggdb -I./include

# Linking flags
LDFLAGS :=

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $(TARGET) $(LDFLAGS)

# Compile each source file into an object file
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Phony target to clean the project
.PHONY: clean
clean:
	rm -r $(BUILDDIR) $(TARGET)

# Default target, build the executable
all: $(TARGET)
