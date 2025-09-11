/**
 * @file formatter.c
 * @brief Output formatting and display functions implementation
 * @author Simple Socket Monitor Team
 */

#include "formatter.h"
#include <stdio.h>

/**
 * Print table header
 */
void print_header(void) {
    printf("%-*s %-*s %-*s %-*s %-*s\n",
           COL_PROTOCOL_WIDTH, "Proto",
           COL_LOCAL_WIDTH, "Local Address",
           COL_REMOTE_WIDTH, "Remote Address", 
           COL_STATE_WIDTH, "State",
           COL_UID_WIDTH, "UID");
    print_separator();
}

/**
 * Print separator line
 */
void print_separator(void) {
    int total_width = COL_PROTOCOL_WIDTH + COL_LOCAL_WIDTH + COL_REMOTE_WIDTH + COL_STATE_WIDTH + COL_UID_WIDTH + 4;
    for (int i = 0; i < total_width; i++) {
        printf("-");
    }
    printf("\n");
}

/**
 * Print a single connection
 */
void print_connection(const struct connection *conn, const struct program_options *opts) {
    if (!conn || !should_display_connection(conn, opts)) {
        return;
    }

    char local_addr[64];
    char remote_addr[64];

    /* Format addresses */
    format_address(&conn->local, local_addr, sizeof(local_addr), opts->show_numeric);
    format_address(&conn->remote, remote_addr, sizeof(remote_addr), opts->show_numeric);

    /* Print formatted connection info */
    printf("%-*s %-*s %-*s %-*s %-*u\n",
           COL_PROTOCOL_WIDTH, protocol_to_string(conn->protocol),
           COL_LOCAL_WIDTH, local_addr,
           COL_REMOTE_WIDTH, remote_addr,
           COL_STATE_WIDTH, connection_state_to_string(conn->state),
           COL_UID_WIDTH, conn->uid);
}

/**
 * Print all connections in the list
 */
void print_connections(const struct connection_list *list, const struct program_options *opts) {
    if (!list) {
        return;
    }

    /* Count connections to display */
    size_t display_count = 0;
    for (size_t i = 0; i < list->count; i++) {
        if (should_display_connection(&list->connections[i], opts)) {
            display_count++;
        }
    }

    if (display_count == 0) {
        printf("No connections found matching the specified criteria.\n");
        return;
    }

    /* Print header */
    print_header();

    /* Print each connection */
    for (size_t i = 0; i < list->count; i++) {
        print_connection(&list->connections[i], opts);
    }

    printf("\nTotal: %zu connection%s\n", display_count, display_count != 1 ? "s" : "");
}

/**
 * Check if a connection should be displayed based on options
 */
int should_display_connection(const struct connection *conn, const struct program_options *opts) {
    if (!conn || !opts) {
        return 0;
    }

    /* Check protocol filter */
    if (conn->protocol == PROTOCOL_TCP && !opts->show_tcp) {
        return 0;
    }
    if (conn->protocol == PROTOCOL_UDP && !opts->show_udp) {
        return 0;
    }

    /* Check state filters */
    if (opts->show_listening && conn->state != CONN_STATE_LISTEN) {
        return 0;
    }
    
    if (opts->show_established && conn->state != CONN_STATE_ESTABLISHED) {
        return 0;
    }

    return 1;
}