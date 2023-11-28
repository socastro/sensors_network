//LABORATORIO 2

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL2RWj2AzCx"
#define BLYNK_TEMPLATE_NAME "MCU Esp8266"
#define BLYNK_AUTH_TOKEN "G3ZWQYS6niOrLLKM1vVF_-YeMCr1Lp6j"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Libraries
#include <dht.h>
dht DHT;
//Constants
#define DHT22_PIN 2     // DHT 22  (AM2302) - what pin we're connected to


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "BzyA";
char pass[] = "nohaycontra";

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
  Blynk.virtualWrite(V5, millis() / 1000);

}
//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value

void dataDHT22()
{
    int chk = DHT.read22(DHT22_PIN);
    //Read data and store it to variables hum and temp
    hum = DHT.humidity;
    temp= DHT.temperature;
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    Blynk.virtualWrite(V2, temp);
    Blynk.virtualWrite(V5, hum);
}
void setup()
{
  Serial.begin(9600); // Debug console
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server: //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80); 
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
  timer.setInterval(1000L, dataDHT22);   // Setup a function to be called every second

}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}


