#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h> //libreria para los sockets que contiene direcciones de internet.

/*
Sistemas Operactivos
Claudio Castro Murillo
1630190a
monitor de sockets, TCP y UDP
*/

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


/*
creamos los prototipos de la funcion, una funcion que lea /proc/net/tcp y /proc/net/udp
creamos otra funcion que convierte la direccion y puerto en formato hexadecimal a formato legible.
*/

//void ReadProcNet(const char *path,const char *proto);
/**
 * \brief Convierte una dirección hexadecimal en un formato legible por humanos
 * 
 * Esto convierte una dirección IPv4 de 32 bits leída en formato hexadecimal
 * (como se encuentra en /proc/net/tcp y /proc/net/udp) a un formato legible
 * por humanos (por ejemplo, pasamos de XXXXXXXX:XXXXX a "192.168.1.1:80").
 */
void ParseAddress(uint64_t hex_addr, char *out_ip, uint16_t *out_port){
	unsigned int ip_part[4],port;
	sprintf(out_ip, "%hhu.%hhu.%hhu.%hhu:%d",
		(hex_addr & (0xFF << 24)) >> 24, 
		(hex_addr & (0xFF << 16)) >> 16,
		(hex_addr & (0xFF << 8)) >> 8, 
		(hex_addr & 0xFF),
		port);
	*out_port = port;
}

//definimos nuestras funciones.
void ReadProcNet(const char *path,const char *proto){
	FILE *f = fopen(path,"r");
	if(!f){
		perror("error: no se pudo abrir el archivo");
		return;
	}
	char line[512];
	fgets(line,sizeof(line),f); //saltar el encabezado
	
	printf("\nProtocolo: %s\n", proto);
	printf("%-20s %-20s %-12s\n", "Local", "Remoto", "Estado");
	
	while(fgets(line,sizeof(line),f)){
		char localaddress[64],remoteaddress[64];
		uint64_t localhex,remotehex;
		uint16_t localport,remoteport; 
		unsigned int state;

		printf("Debug: Linea leida: %s", line);
		sscanf(line, "%*d: %llX:%hX %llX:%hX %X",
			&localhex, &localport, &remotehex, &remoteport, &state);
		printf("Debug: %llX %X %llX %X %X\n", localhex, localport, remotehex, remoteport, state);
		//sscanf(line,"%*d: %64X");
		//llamamos la otra funcion para convertir en hexadecimal a un formato legible desde mi funcion que lea el archivo
		ParseAddress(localhex,localaddress,&localport);		
		ParseAddress(remotehex,remoteaddress,&remoteport);
		printf("Debug: %s:%d %s:%d %X\n",localaddress,localport,remoteaddress,remoteport,state);
		getchar();
		
		//buffers para evitar warnings.
		char local[128],remote[128];
		snprintf(local,sizeof(local),"%s:%d",localaddress,localport);
		snprintf(remote,sizeof(remote),"%s:%d",remoteaddress,remoteport);
		
		//mostrar el estado solo en TCP.
		if(strcmp(proto,"TCP") == 0 && state < 12){
			printf("%-20s %-20s %-12s\n",local,remote,TCP_states[state]);
		}else{
		
			printf("%-20s %-20s %-12s\n",local,remote,"-");
		
		}
	
	}
	
	fclose(f);

}

int main(){

	printf("=== Monitor de Sockets abiertos ===\n");
	printf("Consulta directa en /proc/net/tcp y /proc/net/udp\n");
	
	//llamo mi funcion que lea el dir: /proc/net/tcp y /proc/net/udp desde mi main.
	ReadProcNet("/proc/net/tcp","TCP");
	ReadProcNet("/proc/net/udp","UDP");
return 0;
}

