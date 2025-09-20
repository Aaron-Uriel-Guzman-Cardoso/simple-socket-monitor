---
title: Haciendo cat /etc/net/tcp obte

---

# Monitor simple de Sockets
## Documentacion del codigo So_proyecto1.c
para comenzar con esta seccion primero lo que inclui son las librerias que son las siguientes:
1.-include<stdio.h> -donde: es la libreria estandar de entrada y salida(printf,perror,etc)
2.-include<stdlib.h> -es la libreria estandar de funciones (malloc,free,exit,atoi,etc)
3.-include<string.h> -es una libreria para el manejo de cadenas (strcmp,strtok,etc) 
4.-include<arpa/inet.h> -es una libreria para el manejo de sockets y direcciones de internet

declare una constante de arreglo con el tipo de dato caracter que se llama tcp_status  de una conexion de tcp cuando se lea /proc/net/tcp el valor hexadecimal del estado será usado como índice en este arreglo.la cual se muestra a continuacion:

/*
Los estados TCP (según include/net/tcp_states.h en Linux)
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

en este codigo tambien implemente dos funciones una que me lea los archivos de /proc/net/tcp y /proc/net/udp, y otra que me haga la conversion de hexadecimal a un formato legible en mi caso seria en decimal, como se muestra a continuacion:
/*
Creamos los prototipos:
- ReadProcNet: lee /proc/net/tcp o /proc/net/udp
- ParseAddress: convierte dirección y puerto en formato hexadecimal a legible
*/
void ReadProcNet(const char *path,const char *proto);
void ParseAddress(char *hex_addr, char *out_ip, int *out_port);

1.-Funcion ParseAddress(char *hex_addr,char *out_ip, int *out_port).
esta funcion permite la convercion de las direcciones hexadecimales del archivo del kernel en direcciones ipv4 legibles.
donde:
    -char *hex_addr es mi variable tipo caracter apuntador que es la direccion hexadecimal.
    -char *out_ip es mi variable tipo apuntador caracter que representa la ip de salida.
    -int *out_port representa mi puerto de salida.
void ParseAddress(char *hex_addr, char *out_ip, int *out_port){
    unsigned int ip_part[4], port;

    // Lee dirección y puerto en formato HEX de /proc/net/*
    sscanf(hex_addr,"%8X:%X",&ip_part[0],&port);

    struct in_addr addr;
    // Convierte a notación decimal con inet_ntop
    addr.s_addr = htonl(ip_part[0]);
    inet_ntop(AF_INET,&addr,out_ip,INET_ADDRSTRLEN);

    // Retorna el puerto en variable de salida
    *out_port = port;
}

2.-Funcion ReadProcNet(const char *path,const char *proto).
esta funcion nos permite leer los archivos de /proc/net/tcp y /proc/net/udp. donde ignora la primera linea de encabezado y prepara la tabla de salida.
    -const char *path
    -const char *proto  -es una constante que representa el protocolo
    while(fgets(line,sizeof(line),f)){
        char localaddress[64],remoteaddress[64];
        int localport,remoteport;
        unsigned int state;
        char localhex[64],remotehex[64];

        // Extrae direcciones y estado en HEX
        sscanf(line, "%*d: %64[0-9A-Fa-f]:%*X %64[0-9A-Fa-f]:%*X %X",
               localhex,remotehex,&state);

        // Convierte direcciones a formato legible
        ParseAddress(localhex,localaddress,&localport);		
        ParseAddress(remotehex,remoteaddress,&remoteport);

        // Construye cadenas "IP:puerto"
        char local[128],remote[128];
        snprintf(local,sizeof(local),"%s:%d",localaddress,localport);
        snprintf(remote,sizeof(remote),"%s:%d",remoteaddress,remoteport);

        // Si es TCP, muestra estado, si es UDP no aplica
        if(strcmp(proto,"TCP") == 0 && state < 12){
            printf("%-20s %-20s %-12s\n",local,remote,TCP_states[state]);
        }else{
            printf("%-20s %-20s %-12s\n",local,remote,"-");
        }
    }
    fclose(f);
}
por cada conexion encontrada:
    -convierte direccines en hexadecimal a ip:puerto
    -si el protocolo es TCP, muestra el estado de la conexion 
    -si es UDP, se imprime un guion (-) por que UDP no maneja estados de conexion.

3.-funcion main.
El programa arranca mostrando un encabezado y luego consulta directamente los archivos del sistema que contienen las conexiones abiertas.
int main(){
    printf("=== Monitor de Sockets abiertos ===\n");
    printf("Consulta directa en /proc/net/tcp y /proc/net/udp\n");

    // Llamadas a las funciones de lectura
    ReadProcNet("/proc/net/tcp","TCP");
    ReadProcNet("/proc/net/udp","UDP");

    return 0;
}
donde:
-proc/net/tcp: muestra las conexiones TCP activas del sistema donde contiene la informacion sobre cada socket TCP.
    1.-Dirección local y puerto

    2.-Dirección remota y puerto

    3.-Estado de la conexión (ej. ESTABLISHED, LISTEN, TIME_WAIT, etc.)

    4.-UID del proceso dueño del socket

    5.-Cola de transmisión y recepción



-proc/net/udp: es similar a /proc/net/tcp pero para sockets UDP.
    Como UDP es un protocolo sin conexión, no hay estados como en TCP.

    Muestra dirección local, remota, UID y estadísticas de las colas.

para concluir con la documentacion, el archivo So_proyecto1.c realiza una lectura de los archivos de /proc/net/tcp y /proc/net/udp, de esos archivos o direcctorios va a extraer la informacion de los canales de comunicacion es decir, los sockets. donde me va a mostrar la direcciones ipv4 con sus respectivos puertos y sus estados con el protocolo TCP, ya que UDP es un protocolo sin conexion.

## ¿Qué ya está hecho?
- Podemos leer `/proc/net/{tcp, udp}`
- Damos una salida con formato aceptable
- Hacemos bien la lectura con formato para las direcciones IPv4 expuestas en la carpeta

## ¿Qué falta?
- Almacenar exitosamente los puertos leídos, problemas:
    - `%X:%X` recopila la IP pero deja el puerto como 0.
    - `%X:%hX` recopila la IP pero no recopila correctamente el número (posible bug en la implementación de scanf).
    - `%X:%04X` hace lo mismo que el anterior.
- Documentar el código y el desarrollo realizado (usar bien los commits de git).
    - No olvidar listar cosas como: errores encontrados, como se creó el código, hallazgos, cosas que se podrían mejorar.
- Realizar presentación sintetizando lo que se documentó.
# Monitor simple de Sockets
## ¿Qué ya está hecho?
- Podemos leer `/proc/net/{tcp, udp}`
- Damos una salida con formato aceptable
- Hacemos bien la lectura con formato para las direcciones IPv4 expuestas en la carpeta

## ¿Qué falta?
- Almacenar exitosamente los puertos leídos, problemas:
    - `%X:%X` recopila la IP pero deja el puerto como 0.
    - `%X:%hX` recopila la IP pero no recopila correctamente el número (posible bug en la implementación de scanf).
    - `%X:%04X` hace lo mismo que el anterior.
- Documentar el código y el desarrollo realizado (usar bien los commits de git).
    - No olvidar listar cosas como: errores encontrados, como se creó el código, hallazgos, cosas que se podrían mejorar.
- Realizar presentación sintetizando lo que se documentó.
