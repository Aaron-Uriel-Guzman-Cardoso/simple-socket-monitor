# Monitor simple de Sockets (SSM)

Un mini-programa que se encarga solamente de imprimir toda la información de los Sockets TCP y UDP a través de los archivos expuestos por el Kernel `/proc/net/tcp` y `/proc/net/udp`.

## Propósito: para qué sirve este código
Se estás interesado en entender `netstat` puede servirte para ver en una forma muy simplificada como es que se puede hacer el parsing de los archivos ya mencionados, usando una forma similar a la que hace `netstat` pero quitando todo el aspecto de filtrado de sockets, es decir, imprimimos todos los que están ahí.

### ¿Qué cubre el código?
- Estados de Sockets.
- Decodificación de `int32_t` en formato Big-Endian a IPv4 legible en formato Little-Endian.
- Lectura e Impresión con formato básicas.

## Formato de la salida
### TCP
1. Dirección local y puerto
2. Dirección remota y puerto
3. Estado de la conexión (ej. ESTABLISHED, LISTEN, TIME_WAIT, etc.)

### UDP
1. Dirección local y puerto
2. Dirección remota y puerto
