/**
 * @file main.c
 * @brief Main program entry point for Simple Socket Monitor
 * @author Simple Socket Monitor Team
 */

#include "socket_monitor.h"
#include "connection.h"
#include "proc_parser.h"
#include "formatter.h"

/**
 * Print help message
 */
void print_help(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("\n%s\n\n", PROGRAM_DESCRIPTION);
    printf("OPTIONS:\n");
    printf("  -t, --tcp              Show TCP connections (default: both TCP and UDP)\n");
    printf("  -u, --udp              Show UDP connections (default: both TCP and UDP)\n");
    printf("  -l, --listening        Show only listening sockets\n");
    printf("  -e, --established      Show only established connections\n");
    printf("  -n, --numeric          Show numeric addresses instead of resolving hosts\n");
    printf("  -h, --help             Display this help message\n");
    printf("  -V, --version          Display version information\n");
    printf("\nExamples:\n");
    printf("  %s                     Show all TCP and UDP connections\n", program_name);
    printf("  %s -t -l               Show only TCP listening sockets\n", program_name);
    printf("  %s -u -n               Show UDP connections with numeric addresses\n", program_name);
    printf("\nThis program reads /proc/net/{tcp,udp} to display active network connections.\n");
}

/**
 * Print version information
 */
void print_version(void) {
    printf("%s version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
    printf("A simple implementation of netstat/ss functionality\n");
    printf("Licensed under BSD 3-Clause License\n");
}

/**
 * Parse command line arguments
 */
int parse_arguments(int argc, char *argv[], struct program_options *opts) {
    static struct option long_options[] = {
        {"tcp",         no_argument, 0, 't'},
        {"udp",         no_argument, 0, 'u'},
        {"listening",   no_argument, 0, 'l'},
        {"established", no_argument, 0, 'e'},
        {"numeric",     no_argument, 0, 'n'},
        {"help",        no_argument, 0, 'h'},
        {"version",     no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    /* Initialize options with defaults */
    memset(opts, 0, sizeof(struct program_options));
    
    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "tulenhV", long_options, &option_index)) != -1) {
        switch (c) {
            case 't':
                opts->show_tcp = 1;
                break;
            case 'u':
                opts->show_udp = 1;
                break;
            case 'l':
                opts->show_listening = 1;
                break;
            case 'e':
                opts->show_established = 1;
                break;
            case 'n':
                opts->show_numeric = 1;
                break;
            case 'h':
                opts->show_help = 1;
                return 0;
            case 'V':
                opts->show_version = 1;
                return 0;
            case '?':
                return -1;
            default:
                return -1;
        }
    }

    /* If neither TCP nor UDP specified, show both */
    if (!opts->show_tcp && !opts->show_udp) {
        opts->show_tcp = 1;
        opts->show_udp = 1;
    }

    return 0;
}

/**
 * Run the socket monitor
 */
int run_monitor(const struct program_options *opts) {
    struct connection_list *list = connection_list_create();
    if (!list) {
        fprintf(stderr, "Error: Failed to create connection list\n");
        return EXIT_FAILURE;
    }

    /* Parse TCP connections if requested */
    if (opts->show_tcp) {
        if (parse_proc_net_tcp(list) != 0) {
            fprintf(stderr, "Warning: Failed to parse TCP connections\n");
        }
    }

    /* Parse UDP connections if requested */
    if (opts->show_udp) {
        if (parse_proc_net_udp(list) != 0) {
            fprintf(stderr, "Warning: Failed to parse UDP connections\n");
        }
    }

    /* Sort connections for ordered display */
    connection_list_sort(list);

    /* Display results */
    print_connections(list, opts);

    /* Cleanup */
    connection_list_destroy(list);
    return EXIT_SUCCESS;
}

/**
 * Main program entry point
 */
int main(int argc, char *argv[]) {
    struct program_options opts;

    /* Parse command line arguments */
    if (parse_arguments(argc, argv, &opts) != 0) {
        fprintf(stderr, "Error: Invalid arguments. Use --help for usage information.\n");
        return EXIT_FAILURE;
    }

    /* Handle special options */
    if (opts.show_help) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    if (opts.show_version) {
        print_version();
        return EXIT_SUCCESS;
    }

    /* Run the main monitor functionality */
    return run_monitor(&opts);
}