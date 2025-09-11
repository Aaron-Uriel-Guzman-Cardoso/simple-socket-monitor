/**
 * @file formatter.h
 * @brief Output formatting and display functions
 * @author Simple Socket Monitor Team
 */

#ifndef FORMATTER_H
#define FORMATTER_H

#include "connection.h"
#include "socket_monitor.h"

/* Column widths for formatted output */
#define COL_PROTOCOL_WIDTH 8
#define COL_LOCAL_WIDTH 22
#define COL_REMOTE_WIDTH 22
#define COL_STATE_WIDTH 12
#define COL_UID_WIDTH 8

/* Function prototypes */
void print_header(void);
void print_connection(const struct connection *conn, const struct program_options *opts);
void print_connections(const struct connection_list *list, const struct program_options *opts);
int should_display_connection(const struct connection *conn, const struct program_options *opts);
void print_separator(void);

#endif /* FORMATTER_H */