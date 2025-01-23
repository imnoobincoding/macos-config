# Compiler and Flags
CC := clang
CFLAGS := -std=c99 -O3 -Wall -Wextra -Werror -MMD -MP
INCLUDES := -I../

# Directories
BIN_DIR := bin
SRC_DIR := src
INC_DIR := include

# Targets
TARGET := $(BIN_DIR)/cpu_load
SOURCES := cpu_load.c
HEADERS := cpu.h ../sketchybar.h
OBJECTS := $(SOURCES:.c=.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

# Default Target
all: $(TARGET)

# Compile Object Files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link Object Files
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up compiled files
clean:
	rm -rf $(BIN_DIR) *.o *.d

# Install the binary to /usr/local/bin
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	chmod +x /usr/local/bin/cpu_load
	@echo "cpu_load installed to /usr/local/bin/"

# Include dependencies
-include $(DEPENDENCIES)

# Phony Targets
.PHONY: all clean install
