#include <Arduino.h>
#include <Door.h>

Door::Door(int motorOpenPin, int motorClosePin, int sensorOpenPin, int sensorClosePin)
{
    pinMode(motorOpenPin, OUTPUT);
    digitalWrite(motorOpenPin, LOW);
    _motorOpenPin = motorOpenPin;

    pinMode(motorClosePin, OUTPUT);
    digitalWrite(motorClosePin, LOW);
    _motorClosePin = motorClosePin;

    pinMode(sensorOpenPin, INPUT_PULLUP);
    _sensorOpenPin = sensorOpenPin;

    pinMode(sensorClosePin, INPUT_PULLUP);
    _sensorClosePin = sensorClosePin;
}

void Door::open()
{
    digitalWrite(_motorOpenPin, HIGH);
    digitalWrite(_motorClosePin, LOW);
}

void Door::close()
{
    digitalWrite(_motorOpenPin, LOW);
    digitalWrite(_motorClosePin, HIGH);
}