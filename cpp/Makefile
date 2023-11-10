# CPP compiler
CC = g++

SRC = src
TARGET = kod

CFLAGS = -Wall -ggdb -std=c++17

DEPFLAGS = -I./include

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
CXXSRC = $(call rwildcard,$(SRC),*.cpp)

# compile C source
COMPILE.cpp = $(CC) $(DEPFLAGS) $(CFLAGS) -o ${TARGET}

TARGET:
	$(COMPILE.cpp) ${CXXSRC}