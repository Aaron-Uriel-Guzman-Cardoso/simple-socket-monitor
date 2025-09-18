#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*
Sistemas Operactivos
Claudio Castro Murillo
1630190a
monitor de sockets, TCP y UDP
*/

enum protocol {
	TCP,
	UDP
};

/*
los estados tcp (según include/net/tcp_states.h en Linux)
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

//definimos nuestras funciones.
void ReadProcNet(enum protocol protocol){
	if (protocol != TCP && protocol != UDP){
		fprintf(stderr,"Error: protocolo desconocido\n");
		return;
	}
	const char *path = (protocol == TCP) ? "/proc/net/tcp" : "/proc/net/udp";
	FILE *f = fopen(path,"r");
	if(!f){
		fprintf(stderr,"Error: no se pudo abrir el archivo %s\n",path);
		return;
	}

	char line[512];
	fgets(line,sizeof(line),f); //saltar el encabezado
	
	printf("\nProtocolo: %s\n", (protocol == TCP) ? "TCP" : "UDP");
	printf("%-24s %-24s %-12s\n", "Local", "Remoto", "Estado");
	
	while(fgets(line,sizeof(line),f)){
		char local_addr_str[16], remote_addr_str[16];
		uint32_t local_addr = 0, remote_addr = 0;
		uint16_t local_port = 0, remote_port = 0; 
		uint8_t state = 0;

		/**
		 * TODO: extraer los puertos correctamente, %04hX y similares no
		 *       interpretan bien los números hexadecimales (parece ser un
		 *       bug).
		 * \note Recomendable usar strtoul() para convertir la cadena
		 *       manualmente a un entero.
		 */
		int valid_inputs = sscanf(line, "%*d: %8X:%04hX %8X:%04hX %2X",
			&local_addr, &local_port, &remote_addr, &remote_port, &state);
		if (valid_inputs < 5){
			fprintf(stderr,"Error: formato inesperado en %s\n",path);
			continue;
		}

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

int main(){

	printf("=== Monitor de Sockets abiertos ===\n");
	printf("Consulta directa en /proc/net/tcp y /proc/net/udp\n");
	
	//llamo mi funcion que lea el dir: /proc/net/tcp y /proc/net/udp desde mi main.
	ReadProcNet(TCP);
	ReadProcNet(UDP);
return 0;
}

