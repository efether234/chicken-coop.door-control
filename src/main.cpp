#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "secrets.h"

WiFiClient wifiClient;

// put function declarations here:
void connectToWifi();

void setup() {
  Serial.begin(115200);
  connectToWifi();
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
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
