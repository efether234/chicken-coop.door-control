#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "secrets.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);



// put function declarations here:
void connectToWifi();
void connectToBroker();

void setup() {
  Serial.begin(115200);
  connectToWifi();
  connectToBroker();
}

void loop() {
  // put your main code here, to run repeatedly:
}

/*
 * Function to Connect to to WiFi
 */
void connectToWifi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to WiFi network ");
  Serial.print(SECRET_SSID);

  WiFi.begin(SECRET_SSID, SECRET_PASS);

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to the WiFi network.");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
}

/*
 * Connect to MQTT
 */
void connectToBroker() {
  client.setServer("192.168.1.113", 1883);
  Serial.print("Connecting to MQTT Broker");
  while (!client.connect("door-controller")) {
    delay(1000);
    Serial.print(".");
  } 
  Serial.println();
  Serial.println("Connected to MQTT Broker");
  client.publish("chateau-sadler/chicken-coop/door-status", "connected!");
}
