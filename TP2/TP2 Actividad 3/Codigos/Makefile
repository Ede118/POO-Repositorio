CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
INCLUDES := -Iinclude

SRC_DIR := src
OBJ_DIR := build
TARGET := paLogger

SRCS := $(SRC_DIR)/File.cpp \
        $(SRC_DIR)/RegistroEventos.cpp \
        $(SRC_DIR)/paLogger.cpp

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ_DIR) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

-include $(DEPS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

