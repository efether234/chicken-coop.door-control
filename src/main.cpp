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

std::string doorState = "other"; // options: "open", "opening", "closed", "closing"

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

IPAddress local_IP(192, 168, 1, 108);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

WiFiClient wifiClient;

/*
 * MQTT Config
 * 
 */

const char *availTopic = "chateau-sadler/chicken-coop/door/availability";
const char *stateTopic = "chateau-sadler/chicken-coop/door/state";
const char *ctrlTopic  = "chateau-sadler/chicken-coop/door/control";

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

    if (client.connect("chicken-door_061123", SECRET_UN, SECRET_PW, availTopic, 0, true, "unavailable"))
        {
            Serial.println("Connected to broker");
            client.publish(availTopic, "available", true);
            client.subscribe(ctrlTopic);
        }
    // client.subscribe(ctrlTopic);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected())
    {
        if (client.connect("chicken-door_061123", SECRET_UN, SECRET_PW, availTopic, 0, true, "unavailable"))
        {
            Serial.println("Connected to broker");
            client.publish(availTopic, "available", true);
            client.subscribe(ctrlTopic);
        }
        delay(5000);
    }
    client.loop();
    delay(500);
}

/*
 * Function definitions
 * 
 */

void callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = '\0';
    std::string msg((char *)payload);
    std::string opn("open");
    std::string cls("close");

    if (msg.compare(opn) == 0)
    {
        open();
    }
    else if (msg.compare(cls) == 0)
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
    // sensorOpenDebouncer.update();
    if (doorState == "open")
    {
        return;
    }
    client.publish(stateTopic, "opening");
    digitalWrite(motorOpenPin, HIGH);
    digitalWrite(motorClosePin, LOW);

    while (true)
    {
        delay(50);
        sensorOpenDebouncer.update();
        if (sensorOpenDebouncer.read() == 0)
        {
            stop();
            doorState = "open";
            client.publish(stateTopic, "open", true);
            sensorCloseDebouncer.update();
            break;
        }
        continue;
    }
    
    // delay(9000);
    // stop();
    
    // sensorOpenDebouncer.update();
    // sensorCloseDebouncer.update();
}

void close()
{
    // sensorCloseDebouncer.update();
    if (doorState == "closed")
    {
        return;
    }
    client.publish(stateTopic, "closing");
    digitalWrite(motorOpenPin, LOW);
    digitalWrite(motorClosePin, HIGH);

    while (true)
    {
        delay(50);
        sensorCloseDebouncer.update();
        if (sensorCloseDebouncer.read() == 0)
        {
            stop();
            doorState = "closed";
            client.publish(stateTopic, "closed", true);
            sensorOpenDebouncer.update();
            break;
        }
        continue;
    }
    
    // delay(9000);
    // stop();
    
    // sensorCloseDebouncer.update();
    // sensorOpenDebouncer.update();
}

void stop()
{
    digitalWrite(motorOpenPin, LOW);
    digitalWrite(motorClosePin, LOW);
}