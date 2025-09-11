/**
 * @file proc_parser.c
 * @brief Functions for parsing /proc/net/{tcp,udp} files implementation
 * @author Simple Socket Monitor Team
 */

#include "proc_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
 * Parse /proc/net/tcp file
 */
int parse_proc_net_tcp(struct connection_list *list) {
    return parse_proc_net_file(PROC_NET_TCP, PROTOCOL_TCP, list);
}

/**
 * Parse /proc/net/udp file
 */
int parse_proc_net_udp(struct connection_list *list) {
    return parse_proc_net_file(PROC_NET_UDP, PROTOCOL_UDP, list);
}

/**
 * Parse a /proc/net file
 */
int parse_proc_net_file(const char *filename, protocol_t protocol, struct connection_list *list) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open %s: %s\n", filename, strerror(errno));
        return -1;
    }

    char line[1024];
    int line_number = 0;
    int parsed_count = 0;

    /* Skip header line */
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return -1;
    }
    line_number++;

    /* Parse each connection line */
    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        
        struct connection conn;
        if (parse_connection_line(line, protocol, &conn) == 0) {
            if (connection_list_add(list, &conn) == 0) {
                parsed_count++;
            }
        }
    }

    fclose(file);
    return parsed_count;
}

/**
 * Parse a single connection line from /proc/net file
 */
int parse_connection_line(const char *line, protocol_t protocol, struct connection *conn) {
    if (!line || !conn) {
        return -1;
    }

    /* Initialize connection structure */
    memset(conn, 0, sizeof(struct connection));
    conn->protocol = protocol;

    char local_addr[16], remote_addr[16];
    unsigned int local_port, remote_port;
    unsigned int state;
    unsigned long uid, inode;

    /* Parse the line - format varies between TCP and UDP */
    if (protocol == PROTOCOL_TCP) {
        /* TCP format: sl local_address rem_address st tx_queue rx_queue tr tm->when retrnsmt uid timeout inode */
        int parsed = sscanf(line, "%*d: %8[0-9A-F]:%X %8[0-9A-F]:%X %X %*X:%*X %*X:%*X %*X %lu %*d %lu",
                           local_addr, &local_port, remote_addr, &remote_port, &state, &uid, &inode);
        
        if (parsed != 7) {
            return -1;
        }
        
        conn->state = parse_tcp_state(state);
    } else {
        /* UDP format: sl local_address rem_address st tx_queue rx_queue tr tm->when retrnsmt uid timeout inode */
        int parsed = sscanf(line, "%*d: %8[0-9A-F]:%X %8[0-9A-F]:%X %*X %*X:%*X %*X:%*X %*X %lu %*d %lu",
                           local_addr, &local_port, remote_addr, &remote_port, &uid, &inode);
        
        if (parsed != 6) {
            return -1;
        }
        
        /* UDP doesn't have connection states like TCP */
        conn->state = CONN_STATE_UNKNOWN;
    }

    /* Convert addresses */
    conn->local.ip = hex_to_ip(local_addr);
    conn->local.port = (uint16_t)local_port;
    conn->remote.ip = hex_to_ip(remote_addr);
    conn->remote.port = (uint16_t)remote_port;
    conn->uid = (uint32_t)uid;
    conn->inode = (uint64_t)inode;

    return 0;
}

/**
 * Convert hexadecimal IP address string to uint32_t
 */
uint32_t hex_to_ip(const char *hex_str) {
    if (!hex_str) {
        return 0;
    }

    uint32_t ip = 0;
    sscanf(hex_str, "%X", &ip);
    
    /* /proc/net stores IP in little-endian format, convert to network byte order */
    return ((ip & 0xFF) << 24) | ((ip & 0xFF00) << 8) | ((ip & 0xFF0000) >> 8) | ((ip & 0xFF000000) >> 24);
}

/**
 * Convert hexadecimal port string to uint16_t
 */
uint16_t hex_to_port(const char *hex_str) {
    if (!hex_str) {
        return 0;
    }

    uint16_t port = 0;
    sscanf(hex_str, "%hX", &port);
    return port;
}

/**
 * Parse TCP connection state from numeric value
 */
connection_state_t parse_tcp_state(int state_num) {
    switch (state_num) {
        case 0x01: return CONN_STATE_ESTABLISHED;
        case 0x02: return CONN_STATE_SYN_SENT;
        case 0x03: return CONN_STATE_SYN_RECV;
        case 0x04: return CONN_STATE_FIN_WAIT1;
        case 0x05: return CONN_STATE_FIN_WAIT2;
        case 0x06: return CONN_STATE_TIME_WAIT;
        case 0x07: return CONN_STATE_CLOSE;
        case 0x08: return CONN_STATE_CLOSE_WAIT;
        case 0x09: return CONN_STATE_LAST_ACK;
        case 0x0A: return CONN_STATE_LISTEN;
        case 0x0B: return CONN_STATE_CLOSING;
        default:   return CONN_STATE_UNKNOWN;
    }
}