CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -pedantic -g
INCLUDES := -Iinclude
LDFLAGS := -lm

# Directories
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
LIB_DIR := lib
EXAMPLES_DIR := examples
TESTS_DIR := tests

# Output
LIB := $(LIB_DIR)/libchronobase.a
TARGET_LIB := libchronobase.a

# Source files (add new modules here as they're completed)
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Example programs (add new examples here)
EXAMPLE_SRCS := $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLES := $(EXAMPLE_SRCS:$(EXAMPLES_DIR)/%.c=$(BUILD_DIR)/%)

# Test files (add new tests here)
TEST_SRCS := $(wildcard $(TESTS_DIR)/*.c)
TESTS := $(TEST_SRCS:$(TESTS_DIR)/%.c=$(BUILD_DIR)/%)

# Colors for output
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[1;33m
BLUE := \033[0;34m
NC := \033[0m # No Color

# Default target
.PHONY: all
all: banner lib examples
	@echo "$(GREEN)✓ Build complete!$(NC)"
	@echo "$(BLUE)Run 'make help' for available commands$(NC)"

# Banner
.PHONY: banner
banner:
	@echo "$(BLUE)"
	@echo "╔═══════════════════════════════════════╗"
	@echo "║        ChronoBase Build System        ║"
	@echo "╚═══════════════════════════════════════╝"
	@echo "$(NC)"

# Create directories
$(BUILD_DIR) $(LIB_DIR):
	@mkdir -p $@

# Build static library
.PHONY: lib
lib: $(LIB_DIR) $(BUILD_DIR) $(LIB)

$(LIB): $(OBJS)
	@echo "$(YELLOW)→ Creating static library...$(NC)"
	@ar rcs $@ $^
	@echo "$(GREEN)✓ Library created: $@$(NC)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "$(YELLOW)→ Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build examples
.PHONY: examples
examples: lib $(EXAMPLES)
	@echo "$(GREEN)✓ Examples built$(NC)"

$(BUILD_DIR)/%: $(EXAMPLES_DIR)/%.c $(LIB) | $(BUILD_DIR)
	@echo "$(YELLOW)→ Building example: $*...$(NC)"
	@$(CC) $(CFLAGS) $(INCLUDES) $< -L$(LIB_DIR) -lchronobase $(LDFLAGS) -o $@

# Build tests
.PHONY: tests
tests: lib $(TESTS)
	@echo "$(GREEN)✓ Tests built$(NC)"

$(BUILD_DIR)/%: $(TESTS_DIR)/%.c $(LIB) | $(BUILD_DIR)
	@echo "$(YELLOW)→ Building test: $*...$(NC)"
	@$(CC) $(CFLAGS) $(INCLUDES) $< -L$(LIB_DIR) -lchronobase $(LDFLAGS) -o $@

# Run examples
.PHONY: run-hotel
run-hotel: $(BUILD_DIR)/hotel_demo
	@echo "$(BLUE)→ Running hotel demo...$(NC)"
	@./$(BUILD_DIR)/hotel_demo

.PHONY: run-enrollment
run-enrollment: $(BUILD_DIR)/enrollment_demo
	@echo "$(BLUE)→ Running enrollment demo...$(NC)"
	@./$(BUILD_DIR)/enrollment_demo

.PHONY: run-bus
run-bus: $(BUILD_DIR)/bus_demo
	@echo "$(BLUE)→ Running bus demo...$(NC)"
	@./$(BUILD_DIR)/bus_demo

.PHONY: run-all-examples
run-all-examples: run-hotel run-enrollment run-bus
	@echo "$(GREEN)✓ All examples completed$(NC)"

# Run tests
.PHONY: test
test: tests
	@echo "$(BLUE)→ Running tests...$(NC)"
	@for test in $(TESTS); do \
		echo "$(YELLOW)  Running $$test...$(NC)"; \
		./$$test || exit 1; \
	done
	@echo "$(GREEN)✓ All tests passed$(NC)"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "$(YELLOW)→ Cleaning build artifacts...$(NC)"
	@rm -rf $(BUILD_DIR) $(LIB_DIR)
	@echo "$(GREEN)✓ Clean complete$(NC)"

# Full clean (including editor files)
.PHONY: distclean
distclean: clean
	@echo "$(YELLOW)→ Removing all generated files...$(NC)"
	@find . -name "*.o" -delete
	@find . -name "*.a" -delete
	@find . -name "*~" -delete
	@find . -name "*.swp" -delete
	@find . -name "*.swo" -delete
	@echo "$(GREEN)✓ Full clean complete$(NC)"

# Install (for system-wide installation)
.PHONY: install
install: lib
	@echo "$(YELLOW)→ Installing ChronoBase...$(NC)"
	@install -d /usr/local/lib
	@install -m 644 $(LIB) /usr/local/lib/
	@install -d /usr/local/include/chronobase
	@install -m 644 $(INC_DIR)/*.h /usr/local/include/chronobase/
	@echo "$(GREEN)✓ ChronoBase installed to /usr/local$(NC)"

# Uninstall
.PHONY: uninstall
uninstall:
	@echo "$(YELLOW)→ Uninstalling ChronoBase...$(NC)"
	@rm -f /usr/local/lib/$(TARGET_LIB)
	@rm -rf /usr/local/include/chronobase
	@echo "$(GREEN)✓ ChronoBase uninstalled$(NC)"

# Check for memory leaks (requires valgrind)
.PHONY: memcheck
memcheck: $(BUILD_DIR)/hotel_demo
	@echo "$(BLUE)→ Running memory leak check...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all \
		--track-origins=yes ./$(BUILD_DIR)/hotel_demo

# Format code (requires clang-format)
.PHONY: format
format:
	@echo "$(YELLOW)→ Formatting code...$(NC)"
	@find $(SRC_DIR) $(INC_DIR) $(EXAMPLES_DIR) -name "*.c" -o -name "*.h" \
		| xargs clang-format -i
	@echo "$(GREEN)✓ Code formatted$(NC)"

# Show project statistics
.PHONY: stats
stats:
	@echo "$(BLUE)ChronoBase Project Statistics:$(NC)"
	@echo "$(YELLOW)─────────────────────────────$(NC)"
	@echo "Source files:    $(words $(SRCS))"
	@echo "Header files:    $(shell ls $(INC_DIR)/*.h 2>/dev/null | wc -l)"
	@echo "Examples:        $(words $(EXAMPLE_SRCS))"
	@echo "Tests:           $(words $(TEST_SRCS))"
	@echo "$(YELLOW)─────────────────────────────$(NC)"
	@echo "Lines of code:"
	@wc -l $(SRCS) $(INC_DIR)/*.h 2>/dev/null | tail -1 || echo "  No files yet"

# Help
.PHONY: help
help:
	@echo "$(BLUE)ChronoBase Build System - Available Commands:$(NC)"
	@echo ""
	@echo "$(GREEN)Building:$(NC)"
	@echo "  make              - Build library and examples"
	@echo "  make lib          - Build static library only"
	@echo "  make examples     - Build example programs"
	@echo "  make tests        - Build test suite"
	@echo ""
	@echo "$(GREEN)Running:$(NC)"
	@echo "  make run-hotel          - Run hotel booking demo"
	@echo "  make run-enrollment     - Run enrollment demo"
	@echo "  make run-bus            - Run bus scheduling demo"
	@echo "  make run-all-examples   - Run all example programs"
	@echo "  make test               - Run test suite"
	@echo ""
	@echo "$(GREEN)Cleaning:$(NC)"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make distclean    - Remove all generated files"
	@echo ""
	@echo "$(GREEN)Installation:$(NC)"
	@echo "  make install      - Install to /usr/local (requires sudo)"
	@echo "  make uninstall    - Remove from system"
	@echo ""
	@echo "$(GREEN)Quality:$(NC)"
	@echo "  make memcheck     - Check for memory leaks (requires valgrind)"
	@echo "  make format       - Format code (requires clang-format)"
	@echo "  make stats        - Show project statistics"
	@echo ""
	@echo "$(GREEN)Help:$(NC)"
	@echo "  make help         - Show this help message"

# Debug: show variables
.PHONY: debug
debug:
	@echo "$(BLUE)Makefile Debug Info:$(NC)"
	@echo "CC:          $(CC)"
	@echo "CFLAGS:      $(CFLAGS)"
	@echo "INCLUDES:    $(INCLUDES)"
	@echo "SRCS:        $(SRCS)"
	@echo "OBJS:        $(OBJS)"
	@echo "EXAMPLES:    $(EXAMPLES)"
	@echo "TESTS:       $(TESTS)"
	@echo "LIB:         $(LIB)"
