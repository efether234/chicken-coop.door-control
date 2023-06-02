#include <util.h>

void connectToWifi(const char *ssid, const char *pass)
{
    Serial.println();
    Serial.println();
    Serial.print("Connecting to WiFi network ");
    Serial.print(ssid);

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

void connectToBroker(PubSubClient client, const char *user, const char *pass)
{
    client.setServer("192.168.1.104", 1883);
    client.connect("door-controller", user, pass);
    Serial.println("Connected to MQTT Broker");
}