#ifndef DOOR_H
#define DOOR_H

#include <Arduino.h>
class Door
{
public:
    Door(int motorOpenPin, int motorClosePin, int sensorOpenPin, int sensorClosePin);
    const char *getState();
    void open();
    void close();

private:
    // Motor control pins
    int _motorOpenPin;
    int _motorClosePin;

    // Sensor pins
    int _sensorOpenPin;
    int _sensorClosePin;

    const char *_doorState = "other";
};
#endif