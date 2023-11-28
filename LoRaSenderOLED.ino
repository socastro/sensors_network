//LABORATORIO 3

#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HT_SSD1306Wire.h"

#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             5        // dBm
#define LORA_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here MAX 255

char txpacket[BUFFER_SIZE];

double txNumber;

bool lora_idle = true;

static RadioEvents_t RadioEvents;

// OneWire pin for DS18B20 sensor
#define ONE_WIRE_BUS 13

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

extern SSD1306Wire display; // Declare the display instance

void OnTxDone(void);
void OnTxTimeout(void);

void setup() {
    Serial.begin(115200);
    Mcu.begin();

    txNumber = 0;

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;

    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
        LORA_SPREADING_FACTOR, LORA_CODINGRATE,
        LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
        true, 0, 0, LORA_IQ_INVERSION_ON, 3000);

    // Initialize DS18B20 sensor
    sensors.begin();

    // Initialize the OLED display
    display.init();
    //display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
}
unsigned long previousMillis = 0;
const unsigned long interval = 30000;  // Intervalo de 30 segundos (30000 milisegundos)

void loop() {
unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (lora_idle == true) {
            // Read temperature from DS18B20 sensor
            sensors.requestTemperatures();
            float temperatura = sensors.getTempCByIndex(0);

            // Create a message to send
            char message[50];
            snprintf(message, sizeof(message), "Temperature:%.2f°C", temperatura);

            // Display the message on the OLED screen
            display.clear();
            display.drawString(0, 0, "Mode: TX");
            display.drawString(0, 12, String(message));
            display.drawString(0, 24, "txNumber: " + String(txNumber));
            display.display();

            Serial.println("Paquete:" + String(message));

            // Send the message using LoRa
            Radio.Send((uint8_t *)message, strlen(message));
            lora_idle = false;
        }
    }

    Radio.IrqProcess();
}

void OnTxDone(void) {
    Serial.println("TX done......");
    lora_idle = true;
    txNumber += 1;
}

void OnTxTimeout(void) {
    Radio.Sleep();
    Serial.println("TX Timeout......");
    lora_idle = true;
}