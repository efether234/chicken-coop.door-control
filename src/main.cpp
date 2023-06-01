#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "secrets.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Pin Definitions
const int DOOR_OPEN_SENSOR = 12;
const int DOOR_CLOSED_SENSOR = 13;
const int MOTOR_CW = 5;
const int MOTOR_CCW = 4;

/* *****************************************
 * Topic Definitions
 *  
 * *************************************** */

// Publish door open/closed status
const char* PUB_TOPIC = "chateau-sadler/chicken-coop/door-status";

// Publish MQTT connection status
const char* CON_TOPIC = "chateau-sadler/chicken-coop/connected";

// Subscribe door control
char* SUB_TOPIC = "chateau-sadler/chicken-coop/door-control";

// Variable Declarations
String doorState;
bool subscribed = false;

// Function Declarations
void connectToWifi();
void connectToBroker();
void callback(char*, byte*, unsigned int);
void openDoor();
void closeDoor();

// JSON setup
const int capacity = JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> status;
StaticJsonDocument<capacity> command;

void setup() {
  Serial.begin(115200);

  pinMode(DOOR_OPEN_SENSOR, INPUT_PULLUP);
  pinMode(DOOR_CLOSED_SENSOR, INPUT_PULLUP);
  pinMode(MOTOR_CW, OUTPUT);
  pinMode(MOTOR_CCW, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  connectToWifi();
  connectToBroker();
  client.subscribe(SUB_TOPIC);

  // Serial.println(subscribed);
}

void loop() {
  client.loop();
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

  status["doorState"] = doorState.c_str();

  char buffer[256];
  serializeJson(status, buffer);
  client.publish(PUB_TOPIC, buffer);
  delay(100);

  // openDoor();
  // delay(1000);
  // closeDoor();
  // delay(1000);
}



/* *****************************************
 *
 * Function Definitions
 * 
 * 
 * *************************************** */

// Door controls

void openDoor() {
  digitalWrite(MOTOR_CW, HIGH);
  digitalWrite(MOTOR_CCW, LOW);
}

void closeDoor() {
  digitalWrite(MOTOR_CCW, HIGH);
  digitalWrite(MOTOR_CW, LOW);
}

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
  client.setCallback(callback);
  Serial.print("Connecting to MQTT Broker");
  while (!client.connect("door-controller")) {
    delay(1000);
    Serial.print(".");
  } 
  Serial.println();
  Serial.println("Connected to MQTT Broker");
  client.publish(CON_TOPIC, "connected!");
}

// Callback

void callback(char* topic, byte* payload, unsigned int length) {
  deserializeJson(command, payload);

  if (command["cmd"] == "open") {
    openDoor();
  } else if (command["cmd"] == "close") {
    closeDoor();
  }
}
