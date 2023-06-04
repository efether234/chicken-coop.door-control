#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <secrets.h>

#include <util.h>
#include <Door.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

Door door(5, 0, 14, 13); // mOpen mClose sOpen sClose

const char *pubTopic = "chateau-sadler/chicken-coop/door/status";
const char *subTopic = "chateau-sadler/chicken-coop/door/control";

// callback function

void callback(char *topic, byte *payload, unsigned int length)
{
  StaticJsonDocument<256> doc;                       // Create JSON doc
  deserializeJson(doc, payload, length);             // deserialize JSON into doc
  char cmd[6];                                       // define empty string cmd
  strlcpy(cmd, doc["cmd"] | "default", sizeof(cmd)); // Copy value of doc["cmd"]
  door.control(cmd);                                 // Send command to the door
}

AsyncWebServer server(80);

/** ***********************************************************
 *
 *
 * Setup
 *
 *
 *
 * ************************************************************ */

void setup()
{
  Serial.begin(115200);

  // Connect to Wifi
  connectToWifi(SECRET_SSID, SECRET_PASS);

  AsyncElegantOTA.begin(&server);
  server.begin();

  client.setServer("192.168.1.104", 1883);
  if (client.connect("door-controller", "mqtt-client", "password", pubTopic, 0, true, door.getState()))
  {
    Serial.println("Connected to broker");
  }
  client.subscribe(subTopic);
  client.setCallback(callback);

  door.checkStateChange();
}

/** ***********************************************************
 *
 *
 * Loop
 *
 *
 *
 * ************************************************************ */

void loop()
{
  client.loop();

  if (door.checkStateChange())
  {
    client.publish(pubTopic, door.getState());
  };
}