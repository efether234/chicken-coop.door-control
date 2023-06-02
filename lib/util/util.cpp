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

// PubSubClient connectToBroker()
// {
//     WiFiClient wifiClient;
//     PubSubClient client(wifiClient);

//     client.setServer("192.168.1.104", 1883);
//     if (client.connect("door-controller", "mqtt-client", "password"))
//     {
//         Serial.println("Connected to MQTT Broker");
//     }
//     else
//     {
//         Serial.println("Not connected to Broker.");
//     }

//     return client;
// }