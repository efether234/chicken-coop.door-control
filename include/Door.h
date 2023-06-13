#ifndef DOOR_H
#define DOOR_H

#include <Arduino.h>
#include <Bounce2.h>

class Door
{
public:
    Door(int motorOpenPin, int motorClosePin, int sensorOpenPin, int sensorClosePin);
    const char *getState();

    // Debouncer
    Bounce _sensorOpenDebouncer;
    Bounce _sensorCloseDebouncer;

    bool checkStateChange();
    void stop();
    void open();
    void close();

private:
    // Motor control pins
    int _motorOpenPin;
    int _motorClosePin;

    // Sensor pins
    int _sensorOpenPin;
    int _sensorClosePin;

    // Sensor vals
    int _sensorOpenVal;
    int _sensorCloseVal;

    

    const char *_doorState = "other";

    
};
#endif