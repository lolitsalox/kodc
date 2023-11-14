CXX := g++
SRC_DIR := src
BUILD_DIR := build
TARGET := kod
CFLAGS = -Wall -ggdb -std=c++17 -I./include

RWILDCARD = $(foreach d,$(wildcard $1*),$(call RWILDCARD,$d/,$2) $(filter $(subst *,%,$2),$d))
SRC := $(call RWILDCARD,$(SRC),*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(shell if not exist "$(dir $@)" mkdir "$(dir $@)")
	$(CXX) $(CFLAGS) -c $< -o $@

.PHONY: clean run
clean:
	$(shell if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)")

run: $(TARGET)
	./$(TARGET)
