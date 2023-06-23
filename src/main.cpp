#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <secrets.h>

// WiFi Config
IPAddress local_IP(192, 168, 1, 109);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

void setup() {
    Serial.begin(115200);
    Serial.println();

    if (!WiFi.config(local_IP, gateway, subnet))
    {
        Serial.println("WiFi failed to configure");
    }

    Serial.print("Connecting to WiFi network: ");
    Serial.print(SECRET_SSID);

    WiFi.begin(SECRET_SSID, SECRET_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Connected to WiFi network.");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {

}