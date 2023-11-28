//LABORATORIO 3

#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HT_SSD1306Wire.h"
#include <WiFi.h> // Incluye la librería WiFi para ESP32
#include <HTTPClient.h>

#define RF_FREQUENCY           915000000 // Hz
#define TX_OUTPUT_POWER        14        // dBm
#define LORA_BANDWIDTH         0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR  7         // [SF7..SF12]
#define LORA_CODINGRATE        1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH   8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT    0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON   false
#define RX_TIMEOUT_VALUE       1000
#define BUFFER_SIZE            30        // Define the payload size here MAX 255

char rxpacket[BUFFER_SIZE];
static RadioEvents_t RadioEvents;
int16_t rssi, rxSize;
bool lora_idle = true;

// Declare the display instance
extern SSD1306Wire display;

// Define tus credenciales de Wi-Fi
const char* ssid = "BzyA";
const char* password = "nohaycontra";

void setup() {
    Serial.begin(115200);
    Mcu.begin();
    rssi = 0;
    rxSize = 0;

    // Inicializa la pantalla
    display.init();
    display.flipScreenVertically();

    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.clear();
    display.display();
    
    // Conéctate a la red Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        display.clear();
        display.drawString(0, 0, "Conectando a la red Wi-Fi...");
        Serial.println("Conectando a la red Wi-Fi...");
        display.display();
    }
    delay(1000);
    display.clear();
    Serial.println("Conexión a la red Wi-Fi exitosa!");
    display.drawString(0, 0, "Conexión a la red Wi-Fi exitosa!");
    display.display();

    RadioEvents.RxDone = OnRxDone;
    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                       0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
}

void loop() {
    if (lora_idle) {
        lora_idle = false;
        Serial.println("Into RX mode");
        Radio.Rx(0);
    }
    Radio.IrqProcess();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    rssi = rssi;
    rxSize = size;
    memcpy(rxpacket, payload, size);
    rxpacket[size] = '\0';
    Radio.Sleep();
    Serial.printf("\r\nReceived packet \"%s\" with RSSI %d, length %d\r\n", rxpacket, rssi, rxSize);
    lora_idle = true;

    // Display the received packet, RSSI, and RX size on the OLED screen
    display.clear();
    display.drawString(0, 0, "Mode: RX");
    display.drawString(0, 12, String(rxpacket));
    display.drawString(0, 24, "RSSI: " + String(rssi));
    display.drawString(0, 36, "RX Size: " + String(rxSize));
    display.display();

    // Leer los datos del sensor de temperatura
    float temperature = readTemperatureFromPacket();
    if (!isnan(temperature)) {
        // Enviar los datos de temperatura a ThingSpeak
        sendTemperatureToThingSpeak(temperature); // Enviar solicitud GET
    }
    lora_idle = true; // Esto permitirá recibir más paquetes después de procesar uno.

}

float readTemperatureFromPacket() {
    // Aquí debes implementar el código para extraer la temperatura de rxpacket
    // Asumiendo que los datos de temperatura están en formato de cadena, puedes usar funciones de conversión como atof o sscanf.
    // Por ejemplo:
    float temperature;
    if (sscanf(rxpacket, "Temperature:%f", &temperature) == 1) {
        return temperature;
    } else {
        return NAN; // Valor inválido si no se puede leer la temperatura.
    }
}
void sendTemperatureToThingSpeak(float temperature) {
  // Crear la URL de la solicitud GET
  String url = "https://api.thingspeak.com/update?api_key=T7GL7RLUZMXODO17&field1=";
  url += String(temperature);

  // Enviar la solicitud GET
  HTTPClient httpClient;
  httpClient.begin(url);
  int responseCode = httpClient.GET();

  // Comprobar el código de respuesta
  if (responseCode == 200) {
    // La solicitud se ha realizado correctamente
    Serial.println("Solicitud GET realizada correctamente");
  } else {
    // La solicitud ha fallado
    Serial.println("Error al realizar la solicitud GET");
  }
  // Cerrar la conexión HTTP
  httpClient.end();
}