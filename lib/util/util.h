#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void connectToWifi(const char *ssid, const char *pass);
void connectToBroker(PubSubClient client, const char *user, const char *pass);

#endif