#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Bounce2.h>
#include <PubSubClient.h>
#include <string>

#include <secrets.h>

/*
 * Pin Definitions
 *
 */

int motorOpenPin = 5;
int motorClosePin = 0;
int sensorOpenPin = 14;
int sensorClosePin = 13;

/*
 * Debouncer
 *
 */

Bounce sensorOpenDebouncer;
Bounce sensorCloseDebouncer;

// Door State

String doorState = "other"; // options: "open", "opening", "closed", "closing"

/*
 * Function Declarations
 * 
 */

void callback(char *topic, byte *payload, unsigned int length);
void open();
void close();
void stop();

/*
 * Wifi Config
 *
 */ 

IPAddress local_IP(192, 168, 1, 109);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

WiFiClient wifiClient;

/*
 * MQTT Config
 * 
 */

const char *availTopic = "test/chicken-coop/door/availability";
const char *stateTopic = "test/chicken-coop/door/state";
const char *ctrlTopic  = "test/chicken-coop/door/control";

const char *broker     = "192.168.1.104";
const int   port       = 1883;
PubSubClient client(wifiClient);

AsyncWebServer server(80);

void setup() {
    // Setup pins
    pinMode(motorOpenPin, OUTPUT);
    digitalWrite(motorOpenPin, LOW);
    pinMode(motorClosePin, OUTPUT);
    digitalWrite(motorClosePin, LOW);

    sensorOpenDebouncer.attach(sensorOpenPin, INPUT_PULLUP);
    sensorOpenDebouncer.interval(25);
    sensorCloseDebouncer.attach(sensorClosePin, INPUT_PULLUP);
    sensorCloseDebouncer.interval(25);

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

    AsyncElegantOTA.begin(&server);
    server.begin();

    client.setServer("192.168.1.104", 1883);
    if (client.connect("chicken-door_06231333", SECRET_UN, SECRET_PW, availTopic, 0, true, "unavailable"))
    {
        Serial.println("Connected to broker");
        client.publish(availTopic, "available");
    }
    client.subscribe(ctrlTopic);
    client.setCallback(callback);
}

void loop() {
    client.loop();
}

/*
 * Function definitions
 * 
 */

void callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = '\0';
    std::string msg((char *)payload);
    if (msg == "open")
    {
        open();
    }
    else if (msg == "close")
    {
        close();
    }
    else
    {
        // payload invalid
        return;
    }
}

void open()
{
    client.publish(stateTopic, "opening");
    digitalWrite(motorOpenPin, HIGH);
    digitalWrite(motorClosePin, LOW);

    while (true)
    {
        delay(50);
        sensorOpenDebouncer.update();
        if (sensorOpenDebouncer.changed())
        {
            stop();
            break;
        }
        continue;
    }
    client.publish(stateTopic, "open");
}

void close()
{
    client.publish(stateTopic, "closing");
    digitalWrite(motorOpenPin, LOW);
    digitalWrite(motorClosePin, HIGH);

    while (true)
    {
        delay(50);
        sensorCloseDebouncer.update();
        if (sensorCloseDebouncer.changed())
        {
            stop();
            break;
        }
        continue;
    }
    client.publish(stateTopic, "closed");
}

void stop()
{
    digitalWrite(motorOpenPin, LOW);
    digitalWrite(motorClosePin, LOW);
}