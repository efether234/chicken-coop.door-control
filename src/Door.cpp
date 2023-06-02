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

    // pinMode(sensorOpenPin, INPUT_PULLUP);
    _sensorOpenDebouncer.attach(sensorOpenPin, INPUT_PULLUP);
    _sensorOpenDebouncer.interval(25);
    _sensorOpenPin = sensorOpenPin;

    // pinMode(sensorClosePin, INPUT_PULLUP);
    _sensorCloseDebouncer.attach(sensorClosePin, INPUT_PULLUP);
    _sensorCloseDebouncer.interval(25);
    _sensorClosePin = sensorClosePin;
}

const char *Door::getState()
{
    return _doorState;
}

bool Door::checkStateChange()
{
    // Debounce bother open and closed sensors. If either sensor value changes,
    // set the door state and return true. Otherwise, return false.
    _sensorOpenDebouncer.update();
    _sensorCloseDebouncer.update();

    if (_sensorOpenDebouncer.changed() || _sensorCloseDebouncer.changed())
    {
        if (_sensorOpenDebouncer.read() == 0) // open sensor is active
        {
            _doorState = "open";
        }
        else if (_sensorCloseDebouncer.read() == 0) // closed sensor is active
        {
            _doorState = "closed";
        }
        else // neither sensor is active
        {
            _doorState = "other";
        }
        return true;
    }
    return false;
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