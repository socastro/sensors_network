//LABORATORIO 2
#include <dht.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
dht DHT;
// Constants
#define DHT22_PIN 2 // DHT 22 (AM2302) - what pin we're connected to
// Variables
float hum;  // Stores humidity value
float temp; // Stores temperature value

// Your WiFi credentials.
char ssid[] = "BzyA"; char pass[] = "nohaycontra";
// ThingSpeak API key
char apiKey[] = "IV5J49VTSJN0WILI";
// Time variables
unsigned long previousSensorReadTime = 0;  // Store the last time the sensor was read
unsigned long previousThingSpeakTime = 0;  // Store the last time ThingSpeak was updated
const unsigned long sensorInterval = 10000; // Read the sensor every 10 seconds
const unsigned long thingspeakInterval = 30000; // Update ThingSpeak every 60 seconds

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  unsigned long currentMillis = millis();

  // Read the sensor data at a different interval
  if (currentMillis - previousSensorReadTime >= sensorInterval) {
    previousSensorReadTime = currentMillis;
    int chk = DHT.read22(DHT22_PIN);
    hum = DHT.humidity;
    temp = DHT.temperature;
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    delay(2000); //Delay 2 sec.

  }

  // Update ThingSpeak at a different interval
  if (currentMillis - previousThingSpeakTime >= thingspeakInterval) {
    previousThingSpeakTime = currentMillis;
    // Check if the sensor data is valid
    if (temp != -999.00 && hum != -999.00) {
      sendTemperatureToThingSpeak(temp);
      Serial.println("GET request successful");

    }
  }
}

void sendTemperatureToThingSpeak(float temperature) {
  WiFiClient client;
  if (client.connect("api.thingspeak.com", 80)) {
    String postStr = "field7=" + String(temperature);
    postStr += "\r\n\r\n";
    
    client.print("GET /update?api_key=");
    client.print(apiKey);
    client.print("&");
    client.print(postStr);
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266/1.0");
    client.println("Connection: close");
    client.println();
    
    // Wait for the server's response
    while (!client.available()) {
      delay(1);
    }
    
    // Print the response to the Serial Monitor
    while (client.available()) {
      char c = client.read();

    }
    
    client.stop();
  }
}