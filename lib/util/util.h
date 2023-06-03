#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void connectToWifi(const char *ssid, const char *pass);

#endif