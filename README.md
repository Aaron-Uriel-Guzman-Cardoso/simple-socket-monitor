# Simple Socket Monitor

A simple but professional implementation of basic `netstat` and `ss` functionality in C. This program reads `/proc/net/{tcp,udp}` files to display active network connections in an ordered format.

## Project Overview

This project was designed for a one-week development cycle with a team of five developers. It provides a clean, modular codebase that allows team members to work on different components simultaneously.

## Features

- Display TCP and UDP network connections
- Read data directly from `/proc/net/{tcp,udp}` files  
- Show connections in an ordered, formatted table
- Filter by protocol (TCP/UDP)
- Filter by connection state (listening, established)
- Support for both numeric and resolved addresses
- Professional command-line interface with help and version options

## Quick Start

### Building

```bash
# Debug build (default)
make

# Release build (optimized)
make release

# Clean build artifacts
make clean
```

### Running

```bash
# Show all connections
./bin/socket-monitor

# Show only TCP connections
./bin/socket-monitor --tcp

# Show only listening sockets
./bin/socket-monitor --listening

# Show UDP connections with numeric addresses
./bin/socket-monitor --udp --numeric

# Get help
./bin/socket-monitor --help
```

## Usage

```
Usage: socket-monitor [OPTIONS]

Simple Socket Monitor - A basic netstat/ss replica

OPTIONS:
  -t, --tcp              Show TCP connections (default: both TCP and UDP)
  -u, --udp              Show UDP connections (default: both TCP and UDP)
  -l, --listening        Show only listening sockets
  -e, --established      Show only established connections
  -n, --numeric          Show numeric addresses instead of resolving hosts
  -h, --help             Display this help message
  -V, --version          Display version information

Examples:
  socket-monitor                     Show all TCP and UDP connections
  socket-monitor -t -l               Show only TCP listening sockets
  socket-monitor -u -n               Show UDP connections with numeric addresses
```

## Project Structure

```
simple-socket-monitor/
├── src/                    # Source files
│   ├── main.c             # Program entry point and argument parsing
│   ├── connection.c       # Connection data structures and management
│   ├── proc_parser.c      # /proc/net/{tcp,udp} file parsing
│   └── formatter.c        # Output formatting and display
├── include/               # Header files
│   ├── socket_monitor.h   # Main program header
│   ├── connection.h       # Connection data structures
│   ├── proc_parser.h      # Parser function declarations
│   └── formatter.h        # Formatter function declarations
├── docs/                  # Documentation
├── examples/              # Usage examples
├── Makefile              # Professional build system
├── README.md             # This file
├── LICENSE               # BSD 3-Clause License
└── .gitignore           # Git ignore file for C projects
```

## Architecture

The project uses a modular design that enables parallel development:

### Core Modules

1. **Main Program** (`main.c`)
   - Command-line argument parsing
   - Program flow control
   - Help and version display

2. **Connection Management** (`connection.c`)
   - Connection data structures
   - Dynamic list management
   - Connection sorting and comparison

3. **Proc Parser** (`proc_parser.c`) 
   - Reading `/proc/net/tcp` and `/proc/net/udp`
   - Parsing connection data from kernel format
   - Converting hexadecimal addresses and ports

4. **Output Formatter** (`formatter.c`)
   - Formatted table output
   - Address resolution (numeric vs. hostnames)
   - Connection filtering based on user options

### Data Structures

- `struct connection`: Represents a single network connection
- `struct connection_list`: Dynamic array of connections with sorting
- `struct program_options`: Command-line options and settings

## Development Guidelines

### Team Development

The modular architecture supports concurrent development:

- **Developer 1**: Main program logic and CLI parsing
- **Developer 2**: TCP connection parsing (`/proc/net/tcp`)
- **Developer 3**: UDP connection parsing (`/proc/net/udp`)  
- **Developer 4**: Connection data management and sorting
- **Developer 5**: Output formatting and filtering

### Coding Standards

- Follow C99 standard
- Use consistent naming conventions (snake_case)
- Include comprehensive documentation in headers
- Handle errors gracefully with appropriate messages
- Memory management: always free allocated memory

### Build System

The Makefile provides comprehensive build targets:

```bash
make help           # Show all available targets
make debug          # Debug build with symbols
make release        # Optimized release build  
make format         # Format code (requires clang-format)
make analyze        # Static analysis (requires cppcheck)
make memcheck       # Memory leak detection (requires valgrind)
make install        # System-wide installation
```

## Technical Details

### /proc/net File Format

The program parses `/proc/net/tcp` and `/proc/net/udp` files which contain:
- Local and remote addresses in hexadecimal format
- Port numbers in hexadecimal
- Connection states (for TCP)
- User IDs and inode numbers

### Address Conversion

- Addresses are stored in little-endian format in `/proc/net`
- The parser converts to network byte order for proper display
- Supports both IPv4 numeric and hostname resolution

### Memory Management

- Uses dynamic arrays that grow as needed
- Proper cleanup of all allocated memory
- No memory leaks (verified with valgrind)

## Examples

### Basic Usage

```bash
# Show all connections
./bin/socket-monitor

# Output:
# Proto    Local Address         Remote Address        State        UID
# -----------------------------------------------------------------------
# tcp      *:22                  *:*                   LISTEN       0
# tcp      127.0.0.1:631         *:*                   LISTEN       0
# udp      *:68                  *:*                   UNKNOWN      0
#
# Total: 3 connections
```

### Filtered Output

```bash
# Show only TCP listening sockets
./bin/socket-monitor --tcp --listening

# Show established connections with numeric addresses  
./bin/socket-monitor --established --numeric
```

## License

This project is licensed under the BSD 3-Clause License. See the [LICENSE](LICENSE) file for details.

## Contributing

1. Follow the coding standards outlined above
2. Test your changes with the provided build targets
3. Update documentation for any API changes
4. Use the modular architecture to avoid conflicts

## System Requirements

- Linux system with `/proc/net/{tcp,udp}` support
- GCC compiler with C99 support
- Make build system
- Optional: clang-format, cppcheck, valgrind for development tools