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
void openDoor();
void closeDoor();
void callback(char *topic, byte *payload, unsigned int length);

char *doorState;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

Door door(5, 0, 14, 13); // mOpen-D1 mClose-D3 sOpen-D5 Close-D7

const char *pubTopic = "test/chicken-coop/door/status";
const char *subTopic = "test/chicken-coop/door/control";

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
  if (client.connect("door-controller", "mqtt-client", "password"))
  {
    Serial.println("Connected to broker");
  }
  client.subscribe(subTopic);
  client.setCallback(callback);

  door.checkStateChange();
  strcpy(doorState, door.getState());
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
    strcpy(doorState, door.getState());
    client.publish(pubTopic, doorState), true);
  };
}

/** ***********************************************************
 *
 *
 * Function Definitions
 *
 *
 *
 * ************************************************************ */

void openDoor() {
  door.open();
}

void closeDoor() {
  door.close();
}

void callback(char *topic, byte *payload, unsigned int length)
{
  if (!strncmp((char *)payload, "open", length))
  {
    openDoor();
    strcpy(doorState, "opening");
    client.publish(pubTopic, doorState);
  }
  if (!strncmp((char *)payload, "close", length))
  {
    closeDoor();
    strcpy(doorState, "closing");
    client.publish(pubTopic, doorState);
  }
  if (!strncmp((char *)payload, "stop", length))
  {
    door.stop();
  }
}