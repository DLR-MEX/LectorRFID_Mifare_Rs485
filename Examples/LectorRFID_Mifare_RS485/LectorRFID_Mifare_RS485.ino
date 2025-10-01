#include <Arduino.h>

// --- Caracteres de control del protocolo ---
const byte STX = 0x02;   // Inicio de trama
const byte ETX = 0x03;   // Fin de trama
const byte CR  = 0x0D;   // Carriage Return
const byte LF  = 0x0A;   // Line Feed

// --- Configuración del buffer de recepción ---
const int MAX_BUFFER_SIZE = 20;
byte receiveBuffer[MAX_BUFFER_SIZE];
int bufferIndex = 0;
bool receivingPacket = false;

// --- Pines de UART ---
const int UART_RX_PIN = 16;
const int UART_TX_PIN = 17;

// --- Funciones auxiliares ---
byte charHexToValue(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0; // Valor por defecto si no es válido
}

void processReceivedPacket() {
  const int EXPECTED_ASCII_CHARS = 8;              // Ejemplo: "A99CE4C3"
  const int EXPECTED_PACKET_SIZE = EXPECTED_ASCII_CHARS + 2; // +CR+LF

  if (bufferIndex == EXPECTED_PACKET_SIZE &&
      receiveBuffer[EXPECTED_ASCII_CHARS] == CR &&
      receiveBuffer[EXPECTED_ASCII_CHARS + 1] == LF) {

    Serial.print("Trama recibida (ASCII): ");
    for (int i = 0; i < EXPECTED_ASCII_CHARS; i++) {
      Serial.print((char)receiveBuffer[i]);
    }
    Serial.println();

    // --- Paso 1: Conversión de ASCII a binario ---
    const int BINARY_SIZE = EXPECTED_ASCII_CHARS / 2; // 8 ASCII = 4 bytes
    byte binaryData[BINARY_SIZE];

    for (int i = 0; i < BINARY_SIZE; i++) {
      char highNibble = (char)receiveBuffer[i * 2];
      char lowNibble  = (char)receiveBuffer[i * 2 + 1];
      binaryData[i] = (charHexToValue(highNibble) << 4) | charHexToValue(lowNibble);
    }

    // --- Paso 2: Invertir los bytes ---
    for (int i = 0; i < BINARY_SIZE / 2; i++) {
      byte temp = binaryData[i];
      binaryData[i] = binaryData[BINARY_SIZE - 1 - i];
      binaryData[BINARY_SIZE - 1 - i] = temp;
    }

    // --- Paso 3: Mostrar resultado final ---
    Serial.print("Resultado en HEX: ");
    for (int i = 0; i < BINARY_SIZE; i++) {
      if (binaryData[i] < 0x10) Serial.print("0");
      Serial.print(binaryData[i], HEX);
    }
    Serial.println("\n--------------------");

  } else {
    Serial.print("Error: Trama inválida. Longitud recibida: ");
    Serial.println(bufferIndex);
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  Serial.println("=== Monitor UART para ESP32 ===");
  Serial.println("Esperando tramas en Serial2...");
}

void loop() {
  while (Serial2.available()) {
    byte incomingByte = Serial2.read();

    if (incomingByte == STX) {   // Comienza nueva trama
      receivingPacket = true;
      bufferIndex = 0;
      continue;
    }

    if (!receivingPacket) continue;

    if (incomingByte == ETX) {   // Fin de trama
      processReceivedPacket();
      receivingPacket = false;
    } 
    else if (bufferIndex < MAX_BUFFER_SIZE) {
      receiveBuffer[bufferIndex++] = incomingByte;
    } 
    else {
      receivingPacket = false;
      Serial.println("Error: Desbordamiento de buffer.");
    }
  }
}
