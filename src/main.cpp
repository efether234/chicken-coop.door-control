#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Bounce2.h>
#include <string>
using namespace std;

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

string doorState = "other"; // options: "open", "opening", "closed", "closing"

/*
 * Function Declarations
 * 
 */

void open();
void close();
void stop();

// WiFi Config
IPAddress local_IP(192, 168, 1, 109);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

WiFiClient wifiClient;

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
}

void loop() {
    open();
    delay(1000);
    stop();
    delay(1000);
    close();
    delay(1000);
    stop();
    delay(1000);
}

/*
 * Function definitions
 * 
 */

void open()
{
    doorState = "opening";
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
    doorState = "open";
}

void close()
{
    doorState = "closing";
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
    doorState = "closed";
}

void stop()
{
    digitalWrite(motorOpenPin, LOW);
    digitalWrite(motorClosePin, LOW);
}