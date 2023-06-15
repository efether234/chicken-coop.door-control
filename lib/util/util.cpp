#include <util.h>

void connectToWifi(const char *ssid, const char *pass)
{
    IPAddress local_IP(192, 168, 1, 108);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to WiFi network ");
    Serial.print(ssid);

    if (!WiFi.config(local_IP, gateway, subnet))
    {
        Serial.println("STA failed to configure");
    }
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected to the WiFi network.");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
}
