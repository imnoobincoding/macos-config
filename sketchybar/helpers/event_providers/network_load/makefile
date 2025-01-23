# Compiler and Flags
CC := clang
CFLAGS := -std=c99 -O3 -Wall -Wextra -Werror -pedantic -march=native -flto -fstack-protector-strong -D_FORTIFY_SOURCE=2
LDFLAGS := -flto -fstack-protector-strong -Wl,-z,relro,-z,now

# Directories
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIRS := -I$(SRC_DIR) -I../

# Source and Target
TARGET := $(BIN_DIR)/network_load
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

# Phony Targets
.PHONY: all clean mkdir_bin

# Default Target
all: mkdir_bin $(TARGET)

# Create bin directory if it doesn't exist
mkdir_bin:
	@mkdir -p $(BIN_DIR)

# Link the target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Compile source files into object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Clean build artifacts
clean:
	@rm -rf $(BIN_DIR)