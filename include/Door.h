#ifndef DOOR_H
#define DOOR_H

#include <Arduino.h>
#include <Bounce2.h>

class Door
{
public:
    Door(int motorOpenPin, int motorClosePin, int sensorOpenPin, int sensorClosePin);
    const char *getState();
    bool checkStateChange();
    void open();
    void close();

private:
    // Motor control pins
    int _motorOpenPin;
    int _motorClosePin;

    // Sensor pins
    int _sensorOpenPin;
    int _sensorClosePin;

    // Debouncer
    Bounce _sensorOpenDebouncer;
    Bounce _sensorCloseDebouncer;

    const char *_doorState = "other";
};
#endif