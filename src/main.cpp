#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "secrets.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

/* *****************************************
 * Pin Definitions
 *
 * *************************************** */
const int DOOR_OPEN_SENSOR = 12;
const int DOOR_CLOSED_SENSOR = 13;
const int MOTOR_OPEN = 5;
const int MOTOR_CLOSE = 4;

/* *****************************************
 * Topic Definitions
 *
 * *************************************** */

// Publish door open/closed status
const char *PUB_TOPIC = "chateau-sadler/chicken-coop/door-status";

// Publish MQTT connection status
const char *CON_TOPIC = "chateau-sadler/chicken-coop/connected";

// Subscribe door control
char *SUB_TOPIC = "chateau-sadler/chicken-coop/door-control";

// mqtt info
const char MQTT_USER[] = SECRET_MQTT_USER;
const char MQTT_PASS[] = SECRET_MQTT_PASS;

/* *****************************************
 * Variable Declarations
 *
 * *************************************** */
String doorState;
bool subscribed = false;

// Door Open Sensor
int doorOpenSensorVal;
int doorOpenSensorVal2;
int doorOpenSensorState;

// Door Close Sensor
int doorCloseSensorVal;
int doorCloseSensorVal2;
int doorCloseSensorState;

bool motorRunning = false;

// debounce delay
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 100;

// command received state
bool cmd = false;
// char* open = "open";
// char* close = "close";

/* *****************************************
 * Function Definitions
 *
 * *************************************** */
void connectToWifi();
void connectToBroker();
void callback(char *, byte *, unsigned int);
void stopDoorMotor();
void openDoor();
void closeDoor();

// debounce door sensors
void debounceDoorOpenSensor();
void debounceDoorCloseSensor();

/* *****************************************
 * JSON Setup
 *
 * *************************************** */
const int capacity = JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> status;
StaticJsonDocument<capacity> command;

void setup()
{
  Serial.begin(115200);

  pinMode(DOOR_OPEN_SENSOR, INPUT_PULLUP);
  pinMode(DOOR_CLOSED_SENSOR, INPUT_PULLUP);
  pinMode(MOTOR_OPEN, OUTPUT);
  digitalWrite(MOTOR_OPEN, LOW);
  pinMode(MOTOR_CLOSE, OUTPUT);
  digitalWrite(MOTOR_OPEN, LOW);
  pinMode(LED_BUILTIN, OUTPUT);

  connectToWifi();
  connectToBroker();
  client.subscribe(SUB_TOPIC);

  // Serial.println(subscribed);
}

void loop()
{
  client.loop();

  debounceDoorOpenSensor();
  debounceDoorCloseSensor();

  // Read which door sensor is currently active
  // and set the doorState var to 'open', 'closed', or
  // 'other'

  if (doorOpenSensorState == 0)
  {
    doorState = "open";
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (doorCloseSensorState == 0)
  {
    doorState = "closed";
    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    doorState = "other";
    digitalWrite(LED_BUILTIN, HIGH);
  }

  status["doorState"] = doorState.c_str();

  char buffer[256];
  serializeJson(status, buffer);
  client.publish(PUB_TOPIC, buffer);

  if (cmd)
  {
    /*
     * When a command is received via MQTT, check to see if
     * the door sensor is in the "open" position, then check
     * to see if the motor is running. If it is, the doorOpen
     * sequence has already started and the door is now open,
     * so stop the motor.
     *
     * If the motor isn't running, then the door is already
     * open the the closeDoor sequence should begin.
     */
    if (doorState == "open")
    {
      if (motorRunning)
      {
        stopDoorMotor();
        cmd = false;
      }
      else
      {
        closeDoor();
        delay(2000);
        doorState = "other";
      }
    }

    /*
     * When a command is received via MQTT, check to see if
     * the door sensor is in the "closed" position, then check
     * to see if the motor is running. If it is, the doorClose
     * sequence has already started and the door is now closed,
     * so stop the motor.
     *
     * If the motor isn't running, then the door is already
     * closed the the openDoor sequence should begin.
     */
    if (doorState == "closed")
    {
      if (motorRunning)
      {
        stopDoorMotor();
        cmd = false;
      }
      else
      {
        openDoor();
        delay(2000);
        doorState = "other";
      }
    }
  }

  delay(100);
}

/* *****************************************
 * Function Definitions
 *
 * *************************************** */

// Connect to Wifi

void connectToWifi()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to WiFi network ");
  Serial.print(SECRET_SSID);

  WiFi.begin(SECRET_SSID, SECRET_PASS);

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

// Connect to Broker

void connectToBroker()
{
  client.setServer("192.168.1.104", 1883);
  client.setCallback(callback);
  Serial.print("Connecting to MQTT Broker");
  while (!client.connect("door-controller", MQTT_USER, MQTT_PASS))
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to MQTT Broker");
  client.publish(CON_TOPIC, "connected!");
}

// Callback

void callback(char *topic, byte *payload, unsigned int length)
{
  cmd = true;
}

// Debounce door sensors
void debounceDoorOpenSensor()
{
  doorOpenSensorVal = digitalRead(DOOR_OPEN_SENSOR); // read value and store it in val

  if ((unsigned long)(millis() - lastDebounceTime) > debounceDelay)
  {                                                     // delay 10ms for consistent readings
    doorOpenSensorVal2 = digitalRead(DOOR_OPEN_SENSOR); // read input value again to check or bounce

    if (doorOpenSensorVal == doorOpenSensorVal2)
    { // make sure we have 2 consistent readings
      if (doorOpenSensorVal != doorOpenSensorState)
      { // sensor state has changed
        doorOpenSensorState = doorOpenSensorVal;
      }
    }
  }
}

void debounceDoorCloseSensor()
{
  doorCloseSensorVal = digitalRead(DOOR_CLOSED_SENSOR);

  if ((unsigned long)(millis() - lastDebounceTime) > debounceDelay)
  {
    doorCloseSensorVal2 = digitalRead(DOOR_CLOSED_SENSOR);

    if (doorCloseSensorVal == doorCloseSensorVal2)
    {
      if (doorCloseSensorVal != doorCloseSensorState)
      {
        doorCloseSensorState = doorCloseSensorVal;
      }
    }
  }
}

// Stop Door Motor
void stopDoorMotor()
{
  digitalWrite(MOTOR_OPEN, LOW);
  digitalWrite(MOTOR_CLOSE, LOW);
  motorRunning = false;
}

// Open Door
void openDoor()
{
  digitalWrite(MOTOR_OPEN, HIGH);
  digitalWrite(MOTOR_CLOSE, LOW);
  motorRunning = true;
}

// Close Door
void closeDoor()
{
  digitalWrite(MOTOR_OPEN, LOW);
  digitalWrite(MOTOR_CLOSE, HIGH);
  motorRunning = true;
}
