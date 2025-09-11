# Development Guide

## Team Development Setup

This project is designed for a team of 5 developers working over 1 week. Here's how to organize the work:

### Team Roles and Responsibilities

#### Developer 1: Main Program & CLI
- **Files**: `src/main.c`, `include/socket_monitor.h`
- **Tasks**:
  - Implement argument parsing with getopt_long
  - Program initialization and cleanup
  - Help and version display functions
  - Main program flow control

#### Developer 2: TCP Parser
- **Files**: `src/proc_parser.c` (TCP parts), `include/proc_parser.h`
- **Tasks**:
  - Parse `/proc/net/tcp` file format
  - Handle TCP connection states
  - Convert hexadecimal addresses and ports
  - Error handling for file I/O

#### Developer 3: UDP Parser  
- **Files**: `src/proc_parser.c` (UDP parts), `include/proc_parser.h`
- **Tasks**:
  - Parse `/proc/net/udp` file format
  - Handle UDP socket information
  - Coordinate with TCP parser for shared functions
  - Testing with actual UDP connections

#### Developer 4: Connection Management
- **Files**: `src/connection.c`, `include/connection.h`
- **Tasks**:
  - Design connection data structures
  - Implement dynamic list management
  - Connection sorting algorithms
  - Memory management and cleanup

#### Developer 5: Output Formatting
- **Files**: `src/formatter.c`, `include/formatter.h`  
- **Tasks**:
  - Table formatting and alignment
  - Address resolution (numeric vs hostnames)
  - Connection filtering logic
  - User interface polish

### Development Timeline (1 Week)

#### Day 1-2: Setup and Design
- Set up development environment
- Review and refine header interfaces
- Implement basic structure for each module
- Create stub functions for integration

#### Day 3-4: Core Implementation
- Each developer implements their core functionality
- Regular integration testing
- Resolve interface issues between modules

#### Day 5: Integration and Testing
- Merge all modules
- End-to-end testing
- Bug fixes and refinement

#### Day 6-7: Polish and Documentation
- Code cleanup and optimization
- Documentation completion
- Final testing and validation

### Coding Standards

#### C Code Style
```c
// Function names: snake_case
int parse_connection_line(const char *line);

// Variable names: snake_case  
struct connection_list *list;
int connection_count;

// Constants: UPPER_CASE
#define MAX_CONNECTIONS 10000
#define PROC_NET_TCP "/proc/net/tcp"

// Struct names: snake_case
struct network_address {
    uint32_t ip;
    uint16_t port;
};
```

#### Error Handling
```c
// Always check return values
FILE *file = fopen(filename, "r");
if (!file) {
    fprintf(stderr, "Error: Cannot open %s: %s\n", filename, strerror(errno));
    return -1;
}

// Use consistent error codes
// Return 0 for success, -1 for error
// Use errno when appropriate
```

#### Memory Management
```c
// Always free allocated memory
struct connection_list *list = connection_list_create();
if (!list) {
    return -1;
}

// ... use list ...

connection_list_destroy(list);  // Always cleanup
```

### Integration Guidelines

#### Header Interface Design
- Keep interfaces simple and well-documented
- Use forward declarations to minimize dependencies
- Include guards in all header files
- Document all function parameters and return values

#### Testing Strategy
- Test each module independently first
- Use actual `/proc/net` files for realistic testing
- Test error conditions (missing files, invalid format)
- Verify memory management with valgrind

#### Git Workflow
```bash
# Each developer works on feature branches
git checkout -b feature/tcp-parser
# ... make changes ...
git commit -m "Implement TCP connection parsing"
git push origin feature/tcp-parser

# Regular integration
git checkout main
git pull origin main
git merge feature/tcp-parser
```

### Build and Test Commands

```bash
# Development build
make debug

# Run with test data
make run

# Check for memory leaks
make memcheck

# Static analysis
make analyze

# Format code consistently
make format
```

### Common Pitfalls and Solutions

#### /proc/net File Format Issues
- **Problem**: Addresses are in little-endian hexadecimal
- **Solution**: Use proper conversion functions (hex_to_ip)
- **Testing**: Compare with `netstat` output

#### Memory Management
- **Problem**: Memory leaks in dynamic arrays
- **Solution**: Always pair malloc/free, use valgrind
- **Testing**: Run `make memcheck` regularly

#### Thread Safety (if needed later)
- **Current**: Single-threaded design
- **Future**: Use mutex for shared data structures
- **Design**: Keep modules independent for easier threading

### Module Integration Points

#### Connection Data Flow
1. `main.c` → calls parser functions
2. `proc_parser.c` → creates connection structures  
3. `connection.c` → manages connection lists
4. `formatter.c` → displays formatted output

#### Shared Data Structures
- `struct connection`: Core data structure
- `struct connection_list`: Dynamic array management
- `struct program_options`: User preferences

#### Error Propagation
- Use consistent return codes (0=success, -1=error)
- Propagate errors up through call stack
- Display user-friendly error messages in main.c