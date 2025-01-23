# Makefile for Building Event Providers and Menus

# Declare phony targets to prevent conflicts with file names
.PHONY: all build_event_providers build_menus clean

# Directories
EVENT_PROVIDERS_DIR := event_providers
MENUS_DIR := menus

# Default goal
all: build_event_providers build_menus

# Build Event Providers
build_event_providers:
	@echo "Building Event Providers..."
	@$(MAKE) -C $(EVENT_PROVIDERS_DIR)

# Build Menus
build_menus:
	@echo "Building Menus..."
	@$(MAKE) -C $(MENUS_DIR)

# Clean build artifacts
clean:
	@echo "Cleaning Event Providers..."
	@$(MAKE) -C $(EVENT_PROVIDERS_DIR) clean
	@echo "Cleaning Menus..."
	@$(MAKE) -C $(MENUS_DIR) clean

# Optional: Enable parallel execution
# Usage: make -j