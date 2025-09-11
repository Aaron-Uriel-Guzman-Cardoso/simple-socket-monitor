/**
 * @file connection.h
 * @brief Connection data structures and management functions
 * @author Simple Socket Monitor Team
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>
#include <arpa/inet.h>

/* Connection state enumeration */
typedef enum {
    CONN_STATE_ESTABLISHED = 1,
    CONN_STATE_SYN_SENT,
    CONN_STATE_SYN_RECV,
    CONN_STATE_FIN_WAIT1,
    CONN_STATE_FIN_WAIT2,
    CONN_STATE_TIME_WAIT,
    CONN_STATE_CLOSE,
    CONN_STATE_CLOSE_WAIT,
    CONN_STATE_LAST_ACK,
    CONN_STATE_LISTEN,
    CONN_STATE_CLOSING,
    CONN_STATE_UNKNOWN
} connection_state_t;

/* Protocol enumeration */
typedef enum {
    PROTOCOL_TCP,
    PROTOCOL_UDP
} protocol_t;

/* Network address structure */
struct network_address {
    uint32_t ip;            /* IPv4 address in network byte order */
    uint16_t port;          /* Port number in host byte order */
};

/* Connection information structure */
struct connection {
    protocol_t protocol;                    /* TCP or UDP */
    struct network_address local;           /* Local address and port */
    struct network_address remote;          /* Remote address and port */
    connection_state_t state;               /* Connection state */
    uint32_t uid;                          /* User ID */
    uint64_t inode;                        /* Inode number */
};

/* Connection list structure */
struct connection_list {
    struct connection *connections;         /* Array of connections */
    size_t count;                          /* Number of connections */
    size_t capacity;                       /* Allocated capacity */
};

/* Function prototypes */
struct connection_list* connection_list_create(void);
void connection_list_destroy(struct connection_list *list);
int connection_list_add(struct connection_list *list, const struct connection *conn);
void connection_list_sort(struct connection_list *list);
const char* connection_state_to_string(connection_state_t state);
const char* protocol_to_string(protocol_t protocol);
void format_address(const struct network_address *addr, char *buffer, size_t buffer_size, int numeric);

#endif /* CONNECTION_H */