#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "secrets.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Pin Definitions
const int DOOR_OPEN_SENSOR = 5;
const int DOOR_CLOSED_SENSOR = 4;

// Const Definitions
const char* PUB_TOPIC = "chateau-sadler/chicken-coop/door-status";

// Variable Declarations
String doorState;

// Function Declarations
void connectToWifi();
void connectToBroker();

// JSON setup
const int size = JSON_OBJECT_SIZE(3);
StaticJsonDocument<size> doc;

void setup() {
  Serial.begin(115200);

  pinMode(DOOR_OPEN_SENSOR, INPUT_PULLUP);
  pinMode(DOOR_CLOSED_SENSOR, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  connectToWifi();
  connectToBroker();
}

void loop() {;
  // Read which door sensor is currently active
  // and set the doorState var to 'open', 'closed', or
  // 'other'

  // TODO: Implement 'opening' and 'closing' instead of 'other'

  if (digitalRead(DOOR_OPEN_SENSOR) == 0) {
    doorState = "open";
    digitalWrite(LED_BUILTIN, LOW);
  } else if (digitalRead(DOOR_CLOSED_SENSOR) == 0) {
    doorState = "closed";
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    doorState = "other";
    digitalWrite(LED_BUILTIN, HIGH);
  }

  doc["door-state"] = doorState.c_str();

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(PUB_TOPIC, buffer);
  delay(2500);
}

/* *****************************************
 *
 * Function Definitions
 * 
 * 
 * *************************************** */

// Connect to Wifi

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

// Connect to Broker

void connectToBroker() {
  client.setServer("192.168.1.113", 1883);
  Serial.print("Connecting to MQTT Broker");
  while (!client.connect("door-controller")) {
    delay(1000);
    Serial.print(".");
  } 
  Serial.println();
  Serial.println("Connected to MQTT Broker");
  client.publish(PUB_TOPIC, "connected!");
}
