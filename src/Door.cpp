#include <Arduino.h>
#include <Door.h>
#include <string.h>

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
            _doorState = "close";
        }
        else // neither sensor is active
        {
            _doorState = "other";
        }
        return true;
    }
    return false;
}

/**
 *
 * Motor Controls
 *
 *
 */

void Door::control(char cmd[6])
{
    // HEADS UP: with strcmp(), a truthy value means false and a falsey
    // value means true
    if ((strcmp(cmd, "open")) && (strcmp(cmd, "close")))
    {
        // invalid command. do nothing
        Serial.println("Invalid Command");
        return;
    }
    else if (!strcmp(cmd, "open"))
    {
        _open();
    }
    else if (!strcmp(cmd, "close"))
    {
        _close();
    }
}

void Door::_stopMotor()
{
    digitalWrite(_motorOpenPin, LOW);
    digitalWrite(_motorClosePin, LOW);
}

void Door::_open()
{
    digitalWrite(_motorOpenPin, HIGH);
    digitalWrite(_motorClosePin, LOW);

    while (true) // Door is not yet open
    {
        delay(50); // MCU crashes without this delay in the loop

        _sensorOpenDebouncer.update();
        if (_sensorOpenDebouncer.changed()) // Now door is open
        {
            _stopMotor();
            break;
        }
        continue;
    }
}

void Door::_close()
{
    digitalWrite(_motorOpenPin, LOW);
    digitalWrite(_motorClosePin, HIGH);

    while (true) // Door is not yet open
    {
        delay(50); // MCU crashes without this delay in the loop

        _sensorCloseDebouncer.update();
        if (_sensorCloseDebouncer.changed()) // Now door is open
        {
            delay(50);
            _stopMotor();
            break;
        }
        continue;
    }
}