#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

enum protocol {
	TCP,
	UDP
};

/**
 * \brief Estados de Sockets TCP según include/net/tcp_states.h en Linux
*/
const char *TCP_states[] = {
    "UNKNOWN",
    "ESTABLISHED",
    "SYN_SENT",
    "SYN_RECV",
    "FIN_WAIT1",
    "FIN_WAIT2",
    "TIME_WAIT",
    "CLOSE",
    "CLOSE_WAIT",
    "LAST_ACK",
    "LISTEN",
    "CLOSING"

};

/**
 * \brief Convierte una dirección IPv4 guardado en un entero a un 
 *        formato legible por humanos.
 * 
 * Convierte la dirección IPv4 \param net_addr a un formato legible a un
 * formato legible por humanos, así por ejemplo, pasamos de 0101A8C0 a 
 * 192.168.1.1:80.
 * 
 * \param net_addr Dirección IPv4 en formato big-endian (tal como se encuentra
 *                 en /proc/net/tcp y /proc/net/udp)
 * \param addr_str La dirección en donde se escribirá la dirección en formato
 *                 legible.
 */
void
ParseAddress(uint32_t net_addr, char *addr_str, size_t n)
{
	snprintf(addr_str, n, "%hhu.%hhu.%hhu.%hhu",
		net_addr & 0xFF,
	    (net_addr >> 8) & 0xFF,
	    (net_addr >> 16) & 0xFF,
	    (net_addr >> 24) & 0xFF);
}

/**
 * \brief Leemos e imprimimos sockets TCP y UDP activos
 * 
 * Leemos el contenido de los archivos /proc/net/tcp y /proc/net/udp e
 * imprimimos su contenido en un formato más legible, ya que el Kernel
 * ofrece la información en big-endian y en hexadecimal.
 * 
 * \param protocol El protocolo a leer: TCP o UDP.
 */
void ReadProcNet(enum protocol protocol) {
	if (protocol != TCP && protocol != UDP) {
		fprintf(stderr, "Error: protocolo desconocido\n");
		return;
	}
	const char *path = (protocol == TCP) ? "/proc/net/tcp" : "/proc/net/udp";
	FILE *f = fopen(path,"r");
	if(!f){
		fprintf(stderr,"Error: no se pudo abrir el archivo %s\n",path);
		return;
	}

	char line[512]; // Búffer fijo, no queremos complicarnos la vida
	fgets(line,sizeof(line),f); //saltar el encabezado
	
	printf("\nProtocolo: %s\n", (protocol == TCP) ? "TCP" : "UDP");
	printf("%-24s %-24s %-12s\n", "Local", "Remoto", "Estado");
	
	while(fgets(line,sizeof(line),f)){
		char local_addr_str[16], remote_addr_str[16];
		uint32_t local_addr = 0, remote_addr = 0;
		uint16_t local_port = 0, remote_port = 0; 
		uint8_t state = 0;


		//Declarar buffers para guardar las cadenas hexadecimales.
		char local_addr_hex[9], local_port_hex[5]; // 8 y 5 pero con uno mas por el \0
		char remote_addr_hex[9], remote_port_hex[5];

		/*
		 * Extraemos de los archivos sin usar "%X" porque causa demasiados
		 * problemas, posiblemente hay un bug al usar %hX. En su lugar 
		 * aplicamos expresiones regulares, las cuales parecen ser más 
		 * confiables.
		 */
		int valid_inputs = sscanf(line, 
			"%*d: %8[0-9A-Fa-f]:%4[0-9A-Fa-f] %8[0-9A-Fa-f]:%4[0-9A-Fa-f] %hhX",
			local_addr_hex, local_port_hex, remote_addr_hex, remote_port_hex, 
			&state);
		
		if (valid_inputs < 5){
			fprintf(stderr,"Error: formato inesperado en %s\n",path);
			continue;
		}

		//Convertir manualmente las cadenas hexadecimales a enteros con strtoul.
		local_addr = (uint32_t)strtoul(local_addr_hex, NULL, 16);
		local_port = (uint16_t)strtoul(local_port_hex, NULL, 16);
		remote_addr = (uint32_t)strtoul(remote_addr_hex, NULL, 16);
		remote_port = (uint16_t)strtoul(remote_port_hex, NULL, 16);

		ParseAddress(local_addr,local_addr_str, sizeof(local_addr_str));
		ParseAddress(remote_addr,remote_addr_str, sizeof(remote_addr_str));

		char sock_local[23], sock_remote[23];
		snprintf(sock_local, sizeof(sock_local), "%s:%d", local_addr_str, local_port);
		snprintf(sock_remote, sizeof(sock_remote), "%s:%d", remote_addr_str, remote_port);

		if(protocol == TCP && state < 12){
			printf("Debug: %.60s\n", line);
			printf("%-24s %-24s %-12s\n",sock_local, sock_remote,TCP_states[state]);
		} else {
			printf("%-24s %-24s\n",sock_local, sock_remote);
		}
	}
	
	fclose(f);
}


int main() {
	printf("=== Monitor de Sockets abiertos ===\n");
	printf("Consulta directa en /proc/net/tcp y /proc/net/udp\n");
	
	ReadProcNet(TCP);
	ReadProcNet(UDP);
    return 0;
}