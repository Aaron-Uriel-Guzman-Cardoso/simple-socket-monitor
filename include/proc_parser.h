/**
 * @file proc_parser.h
 * @brief Functions for parsing /proc/net/{tcp,udp} files
 * @author Simple Socket Monitor Team
 */

#ifndef PROC_PARSER_H
#define PROC_PARSER_H

#include "connection.h"

/* /proc/net file paths */
#define PROC_NET_TCP "/proc/net/tcp"
#define PROC_NET_UDP "/proc/net/udp"

/* Function prototypes */
int parse_proc_net_tcp(struct connection_list *list);
int parse_proc_net_udp(struct connection_list *list);
int parse_proc_net_file(const char *filename, protocol_t protocol, struct connection_list *list);
int parse_connection_line(const char *line, protocol_t protocol, struct connection *conn);
uint32_t hex_to_ip(const char *hex_str);
uint16_t hex_to_port(const char *hex_str);
connection_state_t parse_tcp_state(int state_num);

#endif /* PROC_PARSER_H */