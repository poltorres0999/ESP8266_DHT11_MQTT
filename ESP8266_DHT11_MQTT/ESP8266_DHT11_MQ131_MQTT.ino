#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "ArduinoJson.h"
#include "ArduinoJson.h"
#include "MQ131.h"

// I2C comunication pin

#define DHT11_PIN 2
#define DHTTYPE DHT11
#define M_SIZE 50
#define TEMP_HUM_DELAY 3000
#define MQ131_H_CONTROL 3
#define MQ131_ANALOG_READ
#define MQ131_MODEL LOW_CONCENTRATION
#define MQ131_LOAD_RESISTANCE 10000
#define MQ131_DELAY 80000
#define MQ131_UNIT PPM

// Device_details
const char* DEVICE_ID = "ESP8266_1";
const char* D_LOCATION = "Barcelona";

// Broker details
const char* BROKER_SERVER = "54.152.95.225";
const int BROKER_PORT = 1883;

// WiFi info/credentials
const char* W_SSID = "MIWIFI_2G_M3RC";
const char* W_PASS = "furJTDav";

// MQTT publish topics
const char* T_TOPIC = "RPI3_Temperature_Humidity/ESP8266_Temperature_Humidity/temperature";
const char* H_TOPIC = "RPI3_Temperature_Humidity/ESP8266_Temperature_Humidity/humidity";
const char* AQ_TOPIC = "RPI3_Temperature_Humidity/ESP8266_Temperature_Humidity/airQuality";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT11_PIN, DHTTYPE);
MQ131 mqSensor(MQ131_H_CONTROL, MQ131_ANALOG_READ, LOW_CONCENTRATION, MQ131_LOAD_RESISTANCE);
unsigned long mqTimer = millis();
char msg[M_SIZE];
int value = 0;
float temp = 0.0;
float hum = 0.0;
float ariQu = 0.0;
StaticJsonDocument<M_SIZE> temperature;
StaticJsonDocument<M_SIZE> humidity;
StaticJsonDocument<M_SIZE> airQuality;

void setup() {
  Serial.begin(9600);
  setupWiFi();
  dht.begin();
  mqSensor.calibrate();
  client.setServer(BROKER_SERVER, BROKER_PORT);

}

void loop() {
  if(!client.connected()) {
    reconnect();
  }

  client.loop();
  
  sendJsonTemperature();
  delay(TEMP_HUM_DELAY);
  sendJsonHumidity(); 
  delay(TEMP_HUM_DELAY);
  
  if (millis() - mqTimer >= MQ131_DELAY) {
    sendJsonAirQuality();
    mqTimer = millis();
  }
  
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
  if (!isnan(t)) {
    temp = t;
  } 

  temperature["d_id"] = DEVICE_ID;
  temperature["temp"] = temp;
  temperature["loc"] = D_LOCATION;
  serializeJson(temperature, msg);
  serializeJson(temperature, Serial);
  Serial.print("\n");
  client.publish(T_TOPIC, msg);

}

void sendJsonHumidity() {
  float h = dht.readHumidity();
   if (!isnan(h)) {
    hum = h;
  } 

  humidity["d_id"] = DEVICE_ID;
  humidity["hum"] = hum;
  humidity["loc"] = D_LOCATION;
  serializeJson(humidity, msg);
  serializeJson(humidity, Serial);
  Serial.print("\n");
  client.publish(H_TOPIC, msg);

}

void sendJsonAirQuality() {
  
  float aq = sensor.getO3(MQ131_UNIT));
   if (!isnan(aq)) {
    airQu = aq;
  } 
  
  mqSensor.begin();
  airQuality["d_id"] = DEVICE_ID;
  airQuality["airq"] = airQu;
  airQuality["loc"] = D_LOCATION;
  serializeJson(airQuality, msg);
  serializeJson(airQuality, Serial);
  Serial.print("\n");
  client.publish(AQ_TOPIC, msg);

}




