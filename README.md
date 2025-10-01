# 📡 Monitor UART para ESP32 (RS485 ↔ TTL)

Este proyecto permite **recibir y procesar tramas UART en un ESP32** utilizando un lector **RS485 a TTL**.  
El código implementa un pequeño **protocolo con delimitadores** y convierte datos recibidos en formato **ASCII hexadecimal** a **bytes binarios**, listos para usarse.

---

## ⚙️ Características

- Compatible con **ESP32** y módulos RS485 ↔ TTL.
- Recepción de tramas delimitadas por:
  - **STX (0x02)** → Inicio de trama
  - **ETX (0x03)** → Fin de trama
  - **CR (0x0D)** + **LF (0x0A)** → Terminadores de línea
- Conversión automática de datos:
  - ASCII Hex → Binario (ejemplo: `"A9"` → `0xA9`)
  - Inversión de bytes para cumplir con el formato requerido
- Detección de errores:
  - Trama inválida
  - Desbordamiento de buffer

---

## 🔌 Conexiones de hardware

| ESP32 Pin | RS485 ↔ TTL Módulo |
|-----------|-------------------|
| GPIO16    | RX                |
| GPIO17    | TX                |
| GND       | GND               |
| 3V3 / 5V  | VCC (según módulo)|

---

## 📜 Formato de Trama

Cada mensaje válido debe respetar la siguiente estructura:

Ejemplo de trama recibida:

ESTX (0X 02) DATOS (8HEX) CR (0D) LF (0A) ETX (0X 03)

Nota: STX es el personaje inicial 02 (hexadecimal)

DATA es el número de tarjeta de personaje hexadecimal de 8 dígitos.

CR es 0D (indica un espacio)

LF es 0A (para alimentación de línea).

ETX es el personaje final 03 (hexadecimal)

Ejemplo: salida del lector de tarjetas al tipo de número de tarjeta hexadecimal de la computadora.

02 30 31 38 30 31 44 34 0D 0A 03

Dónde: 30 31 31 30 31 44 34 es el código ASCII de cuatro bytes para el número de tarjeta.


---

## 🖥️ Flujo de Procesamiento

1. **Recepción de trama** → El ESP32 detecta `STX` y comienza a almacenar datos hasta `ETX`.
2. **Validación** → La trama debe contener 8 caracteres ASCII + `CR` + `LF`.
3. **Conversión ASCII → Binario**  
   - `"A9"` → `0xA9`
   - `"9C"` → `0x9C`
   - `"E4"` → `0xE4`
   - `"C3"` → `0xC3`
4. **Inversión de bytes**  
   - Entrada: `A9 9C E4 C3`
   - Salida:  `C3 E4 9C A9`
5. **Impresión en Serial Monitor** → Muestra el resultado en formato HEX.

---

## 📋 Ejemplo de salida en Serial Monitor


---

## 🛠️ Configuración

- **Velocidad UART**: `9600 baud`
- **Protocolo UART**: `8N1`
- **Buffer máximo**: `20 bytes`

---

## 🚀 Uso

1. Conectar el módulo RS485 ↔ TTL al ESP32 según la tabla de pines.
2. Cargar el código en el ESP32 mediante Arduino IDE o PlatformIO.
3. Abrir el **Serial Monitor** a `115200 baud`.
4. Enviar una trama con el formato definido.
5. Ver el resultado procesado en HEX.

---

## 📂 Estructura del Código

- **`loop()`** → Recepción de datos UART y armado de la trama.
- **`processReceivedPacket()`** → Validación, conversión y presentación de resultados.
- **`charHexToValue()`** → Conversión de caracteres ASCII a valores numéricos.
- **Buffer de recepción** → Almacena temporalmente la trama hasta su procesamiento.

---

## 📌 Requisitos

- ESP32 (cualquier versión con UART2 disponible).
- Módulo RS485 ↔ TTL.
- Arduino IDE o PlatformIO.

---

## 📖 Licencia

Este proyecto es de uso libre para fines educativos e industriales.  
Puedes modificarlo y adaptarlo a tus necesidades.



