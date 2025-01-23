# Compiler and Flags
CC := clang
CFLAGS := -std=c99 -O3 -Wall -Wextra -Werror -pedantic -march=native \
          -flto -fstack-protector-strong -D_FORTIFY_SOURCE=2
LDFLAGS := -flto -fstack-protector-strong -Wl,-z,relro,-z,now \
          -F/System/Library/PrivateFrameworks/ \
          -framework Carbon -framework SkyLight

# Directories
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

# Source and Target
TARGET := $(BIN_DIR)/menus
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Phony Targets
.PHONY: all clean mkdirs

# Default Target
all: mkdirs $(TARGET)

# Create necessary directories
mkdirs: $(BIN_DIR) $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Link the target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@rm -rf $(BIN_DIR) $(OBJ_DIR)

# Optional: Install the binary to /usr/local/bin
install: all
	@cp $(TARGET) /usr/local/bin/
	@chmod +x /usr/local/bin/$(notdir $(TARGET))
	@echo "Installed $(TARGET) to /usr/local/bin/"

# Optional: Run the application
run: all
	@$(TARGET)