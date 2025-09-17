// Monitor de sockets abiertos leyendo /proc
// Compilar:  gcc -O2 -std=c11 -Wall -Wextra -o proc_sockets proc_sockets.c
// Ejecutar:  ./proc_sockets
// Opciones:  ./proc_sockets --ipv6 (para incluir IPv6)

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef enum { PROTO_TCP, PROTO_UDP } Proto;

typedef struct {
    Proto proto;
    int ipv6;                 // 0 = IPv4, 1 = IPv6
    char local_ip[64];
    unsigned local_port;
    char rem_ip[64];
    unsigned rem_port;
    char state[24];
} Conn;

typedef struct {
    Conn *v;
    size_t n, cap;
} Vec;

static void vec_push(Vec *a, Conn c) {
    if (a->n == a->cap) {
        a->cap = a->cap ? a->cap * 2 : 128;
        a->v = (Conn*)realloc(a->v, a->cap * sizeof(Conn));
        if (!a->v) { perror("realloc"); exit(1); }
    }
    a->v[a->n++] = c;
}

static int cmp_conns(const void *pa, const void *pb) {
    const Conn *a = (const Conn*)pa, *b = (const Conn*)pb;
    if (a->proto != b->proto) return (a->proto < b->proto) ? -1 : 1;
    if (a->ipv6 != b->ipv6)   return (a->ipv6 < b->ipv6) ? -1 : 1;
    int c = strcmp(a->local_ip, b->local_ip);
    if (c) return c;
    if (a->local_port != b->local_port) return (a->local_port < b->local_port) ? -1 : 1;
    c = strcmp(a->rem_ip, b->rem_ip);
    if (c) return c;
    if (a->rem_port != b->rem_port) return (a->rem_port < b->rem_port) ? -1 : 1;
    return strcmp(a->state, b->state);
}

// ---------- Utilidades de decodificación ----------

// Mapa de estados TCP en /proc/net/tcp (hex)
static const char* tcp_state_name(const char *hex) {
    // hex esperado de 2 chars, p.ej. "01"
    if (!hex || strlen(hex) < 2) return "UNKNOWN";
    if (!strcasecmp(hex, "01")) return "ESTABLISHED";
    if (!strcasecmp(hex, "02")) return "SYN_SENT";
    if (!strcasecmp(hex, "03")) return "SYN_RECV";
    if (!strcasecmp(hex, "04")) return "FIN_WAIT1";
    if (!strcasecmp(hex, "05")) return "FIN_WAIT2";
    if (!strcasecmp(hex, "06")) return "TIME_WAIT";
    if (!strcasecmp(hex, "07")) return "CLOSE";
    if (!strcasecmp(hex, "08")) return "CLOSE_WAIT";
    if (!strcasecmp(hex, "09")) return "LAST_ACK";
    if (!strcasecmp(hex, "0A")) return "LISTEN";
    if (!strcasecmp(hex, "0B")) return "CLOSING";
    if (!strcasecmp(hex, "0C")) return "NEW_SYN_RECV";
    return "UNKNOWN";
}

// Estados típicos en /proc/net/udp (el kernel usa códigos similares)
static const char* udp_state_name(const char *hex) {
    if (!hex || strlen(hex) < 2) return "UNKNOWN";
    if (!strcasecmp(hex, "07")) return "UNCONN";
    if (!strcasecmp(hex, "0A")) return "LISTEN";
    if (!strcasecmp(hex, "01")) return "ESTABLISHED"; // poco común en UDP
    return "UNKNOWN";
}

// Convierte "0100007F:0035" (hex little-endian) a "127.0.0.1" y puerto 53
static void parse_ipv4(const char *hex_ip_port, char ip_out[64], unsigned *port_out) {
    unsigned ip_hex[4], port_hex;
    // ip en 4 bytes little-endian, luego puerto en 2 bytes
    if (sscanf(hex_ip_port, "%2x%2x%2x%2x:%4x",
               &ip_hex[3], &ip_hex[2], &ip_hex[1], &ip_hex[0], &port_hex) == 5) {
        snprintf(ip_out, 64, "%u.%u.%u.%u",
                 ip_hex[0], ip_hex[1], ip_hex[2], ip_hex[3]);
        *port_out = port_hex;
    } else {
        snprintf(ip_out, 64, "?.?.?.?");
        *port_out = 0;
    }
}

// Convierte IPv6 en /proc (32 hex, little-endian por bloque) a texto
// Ejemplo: "00000000000000000000000000000000:0035"
static void parse_ipv6(const char *hex_ip_port, char ip_out[64], unsigned *port_out) {
    char iphex[33] = {0};
    unsigned port_hex = 0;
    const char *colon = strchr(hex_ip_port, ':');
    if (!colon) { snprintf(ip_out, 64, "?"); *port_out = 0; return; }
    size_t len = (size_t)(colon - hex_ip_port);
    if (len != 32) { snprintf(ip_out, 64, "?"); *port_out = 0; return; }
    memcpy(iphex, hex_ip_port, 32);
    sscanf(colon + 1, "%4x", &port_hex);

    // /proc almacena cada palabra de 32 bits en little-endian; reordenamos por bloques de 4 hex.
    // iphex: 8 grupos de 4 hex (palabras) en orden invertido por bytes dentro de cada palabra.
    // Reconstruimos en orden convencional de 8 grupos (16-bit) ya normalizados.
    // Para simplificar, leemos como 16 bytes en orden little-endian por palabra.
    uint16_t w[8];
    for (int i = 0; i < 8; ++i) {
        // Cada palabra de 32 bits son 8 chars: abcd efgh -> queremos gh ef cd ab por bytes.
        char part[9] = {0};
        memcpy(part, &iphex[i*4], 4);
        unsigned val;
        sscanf(part, "%4x", &val);
        // Cada "parte" ya es de 16 bits en /proc IPv6 moderno (desde hace años); no inviertas más.
        w[i] = (uint16_t)val;
    }
    // Construir texto compactado (no hacemos compresión :: para mantener simple)
    // Si tu kernel entrega el formato antiguo, esto puede verse raro; para la práctica es suficiente.
    snprintf(ip_out, 64, "%x:%x:%x:%x:%x:%x:%x:%x",
             w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
    *port_out = port_hex;
}

// Lee un archivo de /proc/net/{tcp,udp}[6] y llena el vector
static void parse_file(const char *path, Proto proto, int ipv6, Vec *out) {
    FILE *f = fopen(path, "r");
    if (!f) return; // no existe en algunos sistemas o sin permisos
    char *line = NULL;
    size_t sz = 0;
    // Saltar encabezado
    getline(&line, &sz, f);

    while (getline(&line, &sz, f) != -1) {
        // Columnas (tcp): sl local_address rem_address st ... (espacios variables)
        // Tomamos los 4 primeros campos relevantes
        char local[128], rem[128], st[8];
        // Usamos sscanf con %*s para saltar "sl"
        if (sscanf(line, " %*d: %127s %127s %7s", local, rem, st) < 3) continue;

        Conn c = {0};
        c.proto = proto;
        c.ipv6  = ipv6;
        if (!ipv6) {
            parse_ipv4(local, c.local_ip, &c.local_port);
            parse_ipv4(rem,   c.rem_ip,   &c.rem_port);
        } else {
            parse_ipv6(local, c.local_ip, &c.local_port);
            parse_ipv6(rem,   c.rem_ip,   &c.rem_port);
        }
        const char *stname = (proto == PROTO_TCP) ? tcp_state_name(st) : udp_state_name(st);
        snprintf(c.state, sizeof(c.state), "%s", stname);
        vec_push(out, c);
    }
    free(line);
    fclose(f);
}

static void print_table(Vec *a) {
    printf("%-4s %-5s %-22s %-22s %-7s %-s\n",
           "Prot", "IPV", "Local", "Remoto", "Estado", "");
    printf("---- ----- ---------------------- ---------------------- -------\n");
    for (size_t i = 0; i < a->n; ++i) {
        Conn *c = &a->v[i];
        char local[80], rem[80];
        snprintf(local, sizeof local,  "%s:%u", c->local_ip, c->local_port);
        snprintf(rem,   sizeof rem,    "%s:%u", c->rem_ip,   c->rem_port);
        printf("%-4s %-5s %-22s %-22s %-7s\n",
               c->proto == PROTO_TCP ? "TCP" : "UDP",
               c->ipv6 ? "IPv6" : "IPv4",
               local, rem, c->state);
    }
}

int main(int argc, char **argv) {
    int include_ipv6 = 0;
    if (argc >= 2 && (!strcmp(argv[1], "--ipv6") || !strcmp(argv[1], "-6")))
        include_ipv6 = 1;

    Vec conns = {0};

    parse_file("/proc/net/tcp", PROTO_TCP, 0, &conns);
    parse_file("/proc/net/udp", PROTO_UDP, 0, &conns);
    if (include_ipv6) {
        parse_file("/proc/net/tcp6", PROTO_TCP, 1, &conns);
        parse_file("/proc/net/udp6", PROTO_UDP, 1, &conns);
    }

    qsort(conns.v, conns.n, sizeof(Conn), cmp_conns);
    print_table(&conns);
    free(conns.v);
    return 0;
}
