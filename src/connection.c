/**
 * @file connection.c
 * @brief Connection data structures and management functions implementation
 * @author Simple Socket Monitor Team
 */

#include "connection.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>

/**
 * Create a new connection list
 */
struct connection_list* connection_list_create(void) {
    struct connection_list *list = malloc(sizeof(struct connection_list));
    if (!list) {
        return NULL;
    }

    list->connections = malloc(sizeof(struct connection) * 100);
    if (!list->connections) {
        free(list);
        return NULL;
    }

    list->count = 0;
    list->capacity = 100;
    return list;
}

/**
 * Destroy a connection list and free memory
 */
void connection_list_destroy(struct connection_list *list) {
    if (!list) {
        return;
    }

    if (list->connections) {
        free(list->connections);
    }
    free(list);
}

/**
 * Add a connection to the list
 */
int connection_list_add(struct connection_list *list, const struct connection *conn) {
    if (!list || !conn) {
        return -1;
    }

    /* Expand capacity if needed */
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        struct connection *new_connections = realloc(list->connections, 
                                                   sizeof(struct connection) * new_capacity);
        if (!new_connections) {
            return -1;
        }
        list->connections = new_connections;
        list->capacity = new_capacity;
    }

    /* Add the connection */
    list->connections[list->count] = *conn;
    list->count++;
    return 0;
}

/**
 * Comparison function for sorting connections
 */
static int compare_connections(const void *a, const void *b) {
    const struct connection *conn_a = (const struct connection *)a;
    const struct connection *conn_b = (const struct connection *)b;

    /* First sort by protocol */
    if (conn_a->protocol != conn_b->protocol) {
        return conn_a->protocol - conn_b->protocol;
    }

    /* Then by local address */
    if (conn_a->local.ip != conn_b->local.ip) {
        return (conn_a->local.ip < conn_b->local.ip) ? -1 : 1;
    }

    /* Then by local port */
    if (conn_a->local.port != conn_b->local.port) {
        return conn_a->local.port - conn_b->local.port;
    }

    /* Finally by remote address */
    if (conn_a->remote.ip != conn_b->remote.ip) {
        return (conn_a->remote.ip < conn_b->remote.ip) ? -1 : 1;
    }

    return conn_a->remote.port - conn_b->remote.port;
}

/**
 * Sort connections in the list
 */
void connection_list_sort(struct connection_list *list) {
    if (!list || list->count == 0) {
        return;
    }

    qsort(list->connections, list->count, sizeof(struct connection), compare_connections);
}

/**
 * Convert connection state to string
 */
const char* connection_state_to_string(connection_state_t state) {
    switch (state) {
        case CONN_STATE_ESTABLISHED: return "ESTABLISHED";
        case CONN_STATE_SYN_SENT:    return "SYN_SENT";
        case CONN_STATE_SYN_RECV:    return "SYN_RECV";
        case CONN_STATE_FIN_WAIT1:   return "FIN_WAIT1";
        case CONN_STATE_FIN_WAIT2:   return "FIN_WAIT2";
        case CONN_STATE_TIME_WAIT:   return "TIME_WAIT";
        case CONN_STATE_CLOSE:       return "CLOSE";
        case CONN_STATE_CLOSE_WAIT:  return "CLOSE_WAIT";
        case CONN_STATE_LAST_ACK:    return "LAST_ACK";
        case CONN_STATE_LISTEN:      return "LISTEN";
        case CONN_STATE_CLOSING:     return "CLOSING";
        default:                     return "UNKNOWN";
    }
}

/**
 * Convert protocol to string
 */
const char* protocol_to_string(protocol_t protocol) {
    switch (protocol) {
        case PROTOCOL_TCP: return "tcp";
        case PROTOCOL_UDP: return "udp";
        default:           return "unknown";
    }
}

/**
 * Format network address as string
 */
void format_address(const struct network_address *addr, char *buffer, size_t buffer_size, int numeric) {
    struct in_addr in_addr;
    in_addr.s_addr = addr->ip;

    if (numeric || addr->ip == INADDR_ANY) {
        /* Show numeric IP */
        char ip_str[INET_ADDRSTRLEN];
        if (addr->ip == INADDR_ANY) {
            snprintf(buffer, buffer_size, "*:%u", addr->port);
        } else {
            inet_ntop(AF_INET, &in_addr, ip_str, sizeof(ip_str));
            snprintf(buffer, buffer_size, "%s:%u", ip_str, addr->port);
        }
    } else {
        /* Try to resolve hostname */
        struct hostent *host = gethostbyaddr(&in_addr, sizeof(in_addr), AF_INET);
        if (host && host->h_name) {
            snprintf(buffer, buffer_size, "%s:%u", host->h_name, addr->port);
        } else {
            /* Fall back to numeric if resolution fails */
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &in_addr, ip_str, sizeof(ip_str));
            snprintf(buffer, buffer_size, "%s:%u", ip_str, addr->port);
        }
    }
}