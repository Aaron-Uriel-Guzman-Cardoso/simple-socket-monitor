#!/bin/bash
# Simple Socket Monitor - Usage Examples
# This script demonstrates various ways to use the socket monitor

echo "Simple Socket Monitor - Usage Examples"
echo "======================================"

# Build the program first
echo "Building the program..."
make debug

if [ $? -ne 0 ]; then
    echo "Error: Failed to build the program"
    exit 1
fi

echo
echo "1. Show help message:"
echo "$ ./bin/socket-monitor --help"
./bin/socket-monitor --help

echo
echo "2. Show version information:"  
echo "$ ./bin/socket-monitor --version"
./bin/socket-monitor --version

echo
echo "3. Show all connections (default):"
echo "$ ./bin/socket-monitor"
./bin/socket-monitor

echo
echo "4. Show only TCP connections:"
echo "$ ./bin/socket-monitor --tcp"
./bin/socket-monitor --tcp

echo  
echo "5. Show only UDP connections:"
echo "$ ./bin/socket-monitor --udp"
./bin/socket-monitor --udp

echo
echo "6. Show only listening sockets:"
echo "$ ./bin/socket-monitor --listening"
./bin/socket-monitor --listening

echo
echo "7. Show only established connections:"
echo "$ ./bin/socket-monitor --established"  
./bin/socket-monitor --established

echo
echo "8. Show numeric addresses (no hostname resolution):"
echo "$ ./bin/socket-monitor --numeric"
./bin/socket-monitor --numeric

echo
echo "9. Combined options - TCP listening sockets:"
echo "$ ./bin/socket-monitor --tcp --listening"
./bin/socket-monitor --tcp --listening

echo
echo "10. Combined options - UDP with numeric addresses:"
echo "$ ./bin/socket-monitor --udp --numeric"
./bin/socket-monitor --udp --numeric

echo
echo "Examples complete!"
echo
echo "Try running these commands yourself:"
echo "  ./bin/socket-monitor --help"
echo "  ./bin/socket-monitor --tcp --listening --numeric"
echo "  ./bin/socket-monitor --udp"