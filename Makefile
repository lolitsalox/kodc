# C compiler
CC = gcc

SRC = src
TARGET = cli.exe

CFLAGS = -Wall

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
CSRC = $(call rwildcard,$(SRC),*.c)

# compile C source
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) -o ${TARGET}

TARGET:
	$(COMPILE.c) ${CSRC}