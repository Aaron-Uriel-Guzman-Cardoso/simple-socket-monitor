---
title: Haciendo cat /etc/net/tcp obte

---

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
