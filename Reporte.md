# Monitor de Sockets Abiertos
## Sistemas Operativos - FIE

**Integrantes:**
- 🅰️ Aaron Uriel Guzman Cardoso
- 🅱️ Brandon Hernandez Vargas  
- 🅲 Claudio Castro Murillo
- 🅴 María Elena Gabriel Nicolás
- 🅵 Maria Fernanda Mendez Sanchez

---

## 📋 Índice
1. [¿Qué es un Monitor de Sockets?](#qué-es)
2. [Objetivos del Proyecto](#objetivos)
3. [Arquitectura del Sistema](#arquitectura)
4. [Proceso de Desarrollo](#desarrollo)
5. [Hallazgos y Desafíos](#hallazgos)
6. [Distribución de Tareas](#tareas)
7. [Demostración](#demo)
8. [Conclusiones](#conclusiones)

---

## 🔍 ¿Qué es un Monitor de Sockets? {#qué-es}
Un **monitor de sockets** es una herramienta que permite analizar las
conexiones actuales que tiene tu computadora, así podemos:
1. Saber que puertos están abiertos
2. Qué conexiones se han abierto o cerrado desde que estamos escuchando
4. Facilitar el diagnóstico en problemas de red

Su importancia yace en sus capacidades diagnóstico que nos ayudan a tomar
decisiones, por ejemplo, podemos detectar servicios no autorizados y detectar
sus procesos y usuarios asociados.

---

## 🎯 Objetivos del Proyecto {#objetivos}

### Objetivo Principal
Crear una **versión simplificada de `netstat`** que demuestre el uso básico de
llamadas de alto nivel al Kernel Linux.

### Específicamente
- ✅ Leer archivos del sistema de archivos virtual `/proc/net/`
- ✅ Analizar información de sockets TCP y UDP
- ✅ Decodificar la información a un formato legible
- ✅ Mostrar estados de conexión TCP
- ✅ Implementar llamadas al sistema básicas (`fopen`, `fgets`, `fclose`, etc)

### Consideraciones
- ⏰ 1 semana de desarrollo
- No es requerido hacer código óptimo.
- Deseamos solo demostrar uso de alto nivel del sistema operativo, no hay
  usamos Sockets Netlink (usados por `ss`).
- Tampoco ocupamos imitar toda la funcionalidad de `netstat`, solo la que
  lee los archivos.

---

## 🏗️ Arquitectura del Sistema {#arquitectura}

### Enfoque Seleccionado: Lectura de `/proc/net/`

```mermaid
graph TD
    A[main.c] --> B[ReadProcNet TCP]
    A --> C[ReadProcNet UDP]
    B --> D[/proc/net/tcp]
    C --> E[/proc/net/udp]
    B --> F[ParseAddress]
    C --> F
    F --> G[Salida Formateada]
```

---

## 🛠️ Proceso de Desarrollo {#desarrollo}

### Etapa 1: Investigación (todos)
- 📚 Análisis del código fuente de `netstat` y `ss`
- 🔍 Estudio del formato de `/proc/net/tcp` y `/proc/net/udp`
- 📋 Definición de lo que hará el programa

### Etapa 2: Implementación Base (Claudio)
- Lectura de los archivos e impresión básica incluyendo estados TCP.
    - Lectura con `fopen()` y `fgets()`.
    - Lectura con formato usando `sscanf()`
    - Impresión con formato usando `printf()`
- Definición inicial de las funciones
- Revisión de errores vistos.

### Etapa 3: Correcciones pequeñas (Aaron y Elena)
- ✅ Conversión de direcciones hexadecimales little-endian
- Uso de regexes en `sscanf()` y `strtoul()` para robustez al leer 
  hexadecimales.

### Etapa 4: Documentación refinada y presentación (Aaron, Claudio y Fernanda)
- Se documentan todas las funciones usadas con sintaxis de Doxygen
- Se realiza un reporte de todo el desarrollo
- Se integra una presentación en base al reporte.

---

## 🚧 Hallazgos y Desafíos {#hallazgos}

### 🔥 Problema Principal: Parsing de Puertos Hexadecimales

#### El Problema
```c
/* Una entrada típica: */
"0: 0100007F:0277 00000000:0000 0A ..."
//           ^^^^ El puerto es el 631 (0x0277)

/* Donde ocurre el problema*/
int result = sscanf(line, "%*d: %8X:%hX %8X:%hX %2X", ...);
/*                                  ^^^     ^^^
 * sscanf() por algún motivo devolvía 0 usando %X, y 512 con %hX, %4hX/ o %04hX
 * cuando el resultado real era 631.
 */
```

#### Solución Final
```c
char local_port_hex[5];
sscanf(line, "%*d: %8[0-9A-Fa-f]:%4[0-9A-Fa-f] ...", 
       local_addr_hex, local_port_hex, ...);
local_port = (uint16_t)strtoul(local_port_hex, NULL, 16);
```
- Curiosamente `netstat` también evita el uso de `%X` y recurre a
  expresiones regulares.

### 🧠 Aprendizajes Técnicos

#### 1. Endianness en `/proc/net/`
- Si decodificamos directamente la IP "0100007F" vamos a obtener "1.0.0.127",
  lo cual está claramente alrevés.
- La situación es que los bytes de la IP son recibidos/enviados en un orden de
  bytes Little-Endian (el byte más significativo es el de más a la derecha).
- Invirtiendo los bytes con ayuda de bitmasks a big-endian se resuelve el problema.

#### 2. Estados TCP en Hexadecimal
| Hex | Estado | Descripción |
|-----|--------|-------------|
| `01` | ESTABLISHED | Conexión activa |
| `0A` | LISTEN | Esperando conexiones |
| `06` | TIME_WAIT | Cerrando conexión |

#### 3. Llamadas al Sistema Utilizadas
```c
FILE *f = fopen(path, "r");     // open() syscall internamente
char *result = fgets(line, 512, f);  // read() syscall internamente  
fclose(f);                      // close() syscall internamente
```

### Herramientas Utilizadas
- **Git/GitHub**: Control de versiones
- **GCC**: Compilación y debugging  
- **VS Code**: Desarrollo colaborativo

---

## 🎯 Conclusiones {#conclusiones}

### ✅ Objetivos Cumplidos
- [x] **Llamadas al sistema**: Utilizamos `fopen()`, `fgets()`, `fclose()`
- [x] **Parsing de archivos del kernel**: Exitosa lectura de `/proc/net/`
- [x] **Conversión de formatos**: Hexadecimal little-endian a formato legible
- [x] **Funcionalidad completa**: Monitor funcional similar a `netstat`

### 📚 Aprendizajes Clave
1. **Interfaces del kernel**: `/proc/` como puente user-kernel space
2. **Parsing robusto**: `strtoul()` vs `sscanf()` para datos hexadecimales  
3. **Endianness**: Importancia del orden de bytes en datos del sistema
4. **Debugging sistemático**: Methodology para resolver bugs complejos

### 🚀 Posibles Mejoras Futuras
- 🔧 **Netlink sockets**: Para mayor eficiencia
- 🔄 **Modo tiempo real**: Monitoreo continuo
- 📊 **Estadísticas**: Métricas de tráfico de red
- 🎨 **GUI**: Interfaz gráfica con frameworks como GTK

### 💡 Impacto Académico
Este proyecto demuestra efectivamente:
- **Interacción con el kernel** a través del filesystem virtual
- **Manejo de datos del sistema** en formatos no estándar
- **Desarrollo colaborativo** en equipos de ingeniería
- **Problem-solving** en entornos de sistemas operativos

---

## 🙏 Gracias por su Atención

### Preguntas y Respuestas
¿Alguna pregunta sobre la implementación, los desafíos encontrados o las decisiones de diseño?

### Contacto
- 📧 **Repositorio**: [github.com/Aaron-Uriel-Guzman-Cardoso/simple-socket-monitor](https://github.com/Aaron-Uriel-Guzman-Cardoso/simple-socket-monitor)
- 📋 **Documentación completa**: Ver `README.md` y `Notas.md`
