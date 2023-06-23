#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <secrets.h>

IPAddress local_IP(192, 168, 1, 108);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    Serial.println();
    Serial.print("Connecting to WiFi network: ");
    Serial.print(SECRET_SSID);
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected to the WiFi network.");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    
}

void loop()
{

}