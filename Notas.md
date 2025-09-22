# Sobre los archivos de TCP y UDP
## `/proc/net/tcp`
Muestra las conexiones TCP activas del sistema donde contiene la informacion sobre cada socket TCP.
1. Dirección local y puerto
2. Dirección remota y puerto
3. Estado de la conexión (ej. ESTABLISHED, LISTEN, TIME_WAIT, etc.)
4. UID del proceso dueño del socket
5. Cola de transmisión y recepción

## `/proc/net/udp`
Es similar a /proc/net/tcp pero para sockets UDP:
- Como UDP es un protocolo sin conexión, no hay estados como en TCP.
- Muestra dirección local, remota, UID y estadísticas de las colas.
