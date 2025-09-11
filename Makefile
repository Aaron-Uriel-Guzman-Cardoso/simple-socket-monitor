# Simple Socket Monitor - Makefile
# Professional build system for team development

# Project configuration
PROGRAM_NAME = socket-monitor
VERSION = 1.0.0

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
DOCDIR = docs

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
CFLAGS += -I$(INCDIR)
CFLAGS += -D_GNU_SOURCE  # For getopt_long

# Debug and release builds
DEBUG_CFLAGS = $(CFLAGS) -g -O0 -DDEBUG
RELEASE_CFLAGS = $(CFLAGS) -O2 -DNDEBUG

# Default to debug build
CURRENT_CFLAGS = $(DEBUG_CFLAGS)

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/$(PROGRAM_NAME)

# Default target
.PHONY: all
all: debug

# Debug build
.PHONY: debug
debug: CURRENT_CFLAGS = $(DEBUG_CFLAGS)
debug: $(TARGET)

# Release build
.PHONY: release
release: CURRENT_CFLAGS = $(RELEASE_CFLAGS)
release: clean-build $(TARGET)

# Create target executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@
	@echo "Built $(PROGRAM_NAME) successfully"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CURRENT_CFLAGS) -c $< -o $@

# Create directories
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Cleaned build artifacts"

# Clean without message (for release build)
.PHONY: clean-build  
clean-build:
	rm -rf $(OBJDIR) $(BINDIR)

# Install (for system-wide installation)
.PHONY: install
install: release
	install -D $(TARGET) /usr/local/bin/$(PROGRAM_NAME)
	install -D -m 644 docs/$(PROGRAM_NAME).1 /usr/local/share/man/man1/$(PROGRAM_NAME).1
	@echo "Installed $(PROGRAM_NAME) to /usr/local/bin/"

# Uninstall
.PHONY: uninstall
uninstall:
	rm -f /usr/local/bin/$(PROGRAM_NAME)
	rm -f /usr/local/share/man/man1/$(PROGRAM_NAME).1
	@echo "Uninstalled $(PROGRAM_NAME)"

# Run the program (for testing)
.PHONY: run
run: $(TARGET)
	./$(TARGET)

# Run with arguments (make run-args ARGS="--help")
.PHONY: run-args
run-args: $(TARGET)
	./$(TARGET) $(ARGS)

# Code formatting (if clang-format is available)
.PHONY: format
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRCDIR) $(INCDIR) -name "*.c" -o -name "*.h" | xargs clang-format -i; \
		echo "Code formatted with clang-format"; \
	else \
		echo "clang-format not found, skipping formatting"; \
	fi

# Static analysis (if cppcheck is available)
.PHONY: analyze
analyze:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c99 --platform=unix64 -I$(INCDIR) $(SRCDIR); \
	else \
		echo "cppcheck not found, skipping static analysis"; \
	fi

# Memory leak check (if valgrind is available)
.PHONY: memcheck
memcheck: $(TARGET)
	@if command -v valgrind >/dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET); \
	else \
		echo "valgrind not found, skipping memory check"; \
	fi

# Create distribution package
.PHONY: dist
dist: clean
	tar -czf $(PROGRAM_NAME)-$(VERSION).tar.gz \
		--exclude='.git*' \
		--exclude='*.tar.gz' \
		--exclude='$(OBJDIR)' \
		--exclude='$(BINDIR)' \
		--transform 's,^,$(PROGRAM_NAME)-$(VERSION)/,' \
		.

# Help target
.PHONY: help
help:
	@echo "Simple Socket Monitor Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build debug version (default)"
	@echo "  debug     - Build debug version with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install program system-wide"
	@echo "  uninstall - Remove program from system"
	@echo "  run       - Build and run the program"
	@echo "  run-args  - Build and run with arguments (use ARGS=\"...\")"
	@echo "  format    - Format source code (requires clang-format)"
	@echo "  analyze   - Run static analysis (requires cppcheck)"
	@echo "  memcheck  - Run memory leak check (requires valgrind)"
	@echo "  dist      - Create distribution package"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  make debug"
	@echo "  make run-args ARGS=\"--help\""
	@echo "  make release && sudo make install"

# Dependency tracking (automatic dependencies)
-include $(OBJECTS:.o=.d)

$(OBJDIR)/%.d: $(SRCDIR)/%.c | $(OBJDIR)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$