# List all source files to be compiled; separate with space
SOURCE_FILES := thread_sync.c
# SOURCES := thread_unsync.c

# Program executable name
TARGET = TaskB

BUILD_DIR := ./build
CC := gcc

# compiler flags
CFLAGS = -g -std=gnu11 -Wall -fsanitize=address

# linker flags
LDFLAGS = -g -std=gnu11 -fsanitize=address

# Define the object files in the /build dir
OBJECT_FILES = $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)

.DEFAULT_GOAL := $(TARGET)

# Creates the directory
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Linking files together
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compiles the program
$(TARGET) : $(OBJECT_FILES) | $(BUILD_DIR)
	$(CC)  -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)
