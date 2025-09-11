/**
 * @file socket_monitor.h
 * @brief Main header file for Simple Socket Monitor
 * @author Simple Socket Monitor Team
 * @version 1.0
 * @date 2025
 * 
 * Simple Socket Monitor - A basic implementation of netstat/ss functionality
 * Reads /proc/net/{tcp,udp} and displays active network connections in an ordered format.
 */

#ifndef SOCKET_MONITOR_H
#define SOCKET_MONITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* Program version and information */
#define PROGRAM_NAME "socket-monitor"
#define PROGRAM_VERSION "1.0.0"
#define PROGRAM_DESCRIPTION "Simple Socket Monitor - A basic netstat/ss replica"

/* Program constants */
#define MAX_LINE_LENGTH 1024
#define MAX_CONNECTIONS 10000

/* Command line options structure */
struct program_options {
    int show_tcp;           /* Show TCP connections */
    int show_udp;           /* Show UDP connections */
    int show_listening;     /* Show listening sockets only */
    int show_established;   /* Show established connections only */
    int show_numeric;       /* Show numeric addresses instead of resolving hosts */
    int show_help;          /* Show help message */
    int show_version;       /* Show version information */
};

/* Function prototypes */
int parse_arguments(int argc, char *argv[], struct program_options *opts);
void print_help(const char *program_name);
void print_version(void);
int run_monitor(const struct program_options *opts);

#endif /* SOCKET_MONITOR_H */