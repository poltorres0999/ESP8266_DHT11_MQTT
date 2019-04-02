
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "ArduinoJson.h"
#include "ArduinoJson.h"

// I2C comunication pin
#define DHT11_PIN 0
#define DHTTYPE DHT11

// Device_details
const char* DEVICE_ID = "ESP8266_1";
const char* D_LOCATION = "Barcelona";

// Broker details
const char* BROKER_SERVER = "54.152.95.225";
const int BROKER_PORT = 1883;

// WiFi info/credentials
const char* W_SSID = "AndroidAP_1102";
const char* W_PASS = "Bartolo08";

// MQTT publish topics
const char* T_TOPIC = "RPI3_Temperature_Humidity/ESP8266_Temperature_Humidity/temperature";
const char* H_TOPIC = "RPI3_Temperature_Humidity/ESP8266_Temperature_Humidity/humidity";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT11_PIN, DHTTYPE);
long lastMsg = 0;
char msg[50];
int value = 0;
StaticJsonDocument<3> temperature;
StaticJsonDocument<3> humidity;

void setup() {

  Serial.begin(115200);
  setupWiFi();
  client.setServer(BROKER_SERVER, BROKER_PORT);
}

void loop() {

  if(!client.connected()) {
    reconnect();
  }
  client.loop();
  
  sendJsonTemperature();
  delay(1000);
  sendJsonHumidity();

  ESP.deepSleep(30e6); 
  
}

void setupWiFi() {
  delay(10);
  // Attempt to conencto to WiFi network
  Serial.print("Connecting to: ");
  Serial.println(W_SSID);

  WiFi.begin(W_SSID, W_PASS);

  while( WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connection stablished to: ");
  Serial.println(W_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}

void reconnect() {

  while(!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.print("MQTT connection stablished");
    } else {
      Serial.print("MQTT connection failed");
      Serial.print("New attempt in 5 seconds");
      delay(500);
    }
  }
}

void sendJsonTemperature() {

  float t = dht.readTemperature();
  
  temperature["device_id"] = DEVICE_ID;
  temperature["temperature"] = t;
  temperature["location"] = D_LOCATION;
  serializeJson(temperature, msg);
  
  client.publish(T_TOPIC, msg);
}

void sendJsonHumidity() {

  float h = dht.readHumidity();
  
  temperature["device_id"] = DEVICE_ID;
  temperature["humidity"] = h;
  temperature["location"] = D_LOCATION;
  serializeJson(humidity, msg);
  
  client.publish(H_TOPIC, msg);
  
}
