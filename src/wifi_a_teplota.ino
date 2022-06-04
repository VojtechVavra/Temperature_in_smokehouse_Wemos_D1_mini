#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "max6675.h"

#include "index.h"
ESP8266WebServer server(80);  // 443

// this example is public domain. enjoy!
// https://learn.adafruit.com/thermocouple/

int thermoDO = 12;  //2 MISO
int thermoCS = 15;  //1
int thermoCLK = 14; // 6

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "Axa"
#define WLAN_PASS       "MyPassword"

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Vooja"
#define AIO_KEY         "aio_secretkey"

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
// Setup a feeds for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish temperature_mqtt = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/arduino-1.teplota");

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
//MAX6675* thermocouple;

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 20);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional
////////////////////////////////////////////////////////



bool MQTT_send(float v_temperature);
void HTTP_temperature_send();
void MQTT_connect();
void handleRoot();
// https://circuitdigest.com/microcontroller-projects/ajax-with-esp8266-dynamic-web-page-update-without-reloading

float temperature = 0;
float now;
float lastTrigger;
const float timeSeconds = 5;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB
  }
  
  // Connect to WiFi access point.
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/temperature", HTTP_temperature_send);
  server.on("/index", handleRoot);
  server.begin();
}

void loop() {
   now = millis();
   if(now - lastTrigger > (timeSeconds*1000)) {
     Serial.println("Getting temperature");
     temperature = thermocouple.readCelsius();
     Serial.print("C = "); 
     Serial.println(temperature);
     MQTT_connect();
     MQTT_send(temperature);
     lastTrigger = millis();
   }
   
   server.handleClient();
   
   //MQTT_connect();
   //MQTT_send(temperature);

   //delay(6000);
}


bool MQTT_send(float v_temperature)
{
  bool success = false;
  if (temperature_mqtt.publish(v_temperature)) {
      Serial.println(F("OK!"));
      success = true;
   } else {
      Serial.println(F("Temperature send failed")); 
   }
   return success;
}

void HTTP_temperature_send() {
  //temperature = thermocouple.readCelsius();
  //Serial.print("C = "); 
  //Serial.println(temperature);
  String sensor_value = String(temperature);
  server.send(200, "text/plain", sensor_value);  
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

///////
// web server
void handleRoot() 
{
 String s = webpage;
 Serial.println("handle root");
 Serial.println(webpage);
 server.send(200, "text/html; charset=utf-8", s);
}
