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
        Serial.println(cmd);
        // start motor running in open direction.
        Serial.println("Door Opening...");
        // stop motor running when doorState changes to "closed"
    }
    else if (!strcmp(cmd, "close"))
    {
        Serial.println(cmd);
        // start motor running in close direction.
        Serial.println("Door Closing...");
        // stop motor running when doorState changes to "open"
    }

    // while (this->_doorState != "closed")
    // {
    //     if (checkStateChange())
    //     {
    //         this->_stopMotor();
    //         break;
    //     }
    //     delay(500);
    // }
}

void Door::_stopMotor()
{
    Serial.println("_stopMotor() called");
    digitalWrite(_motorOpenPin, LOW);
    digitalWrite(_motorClosePin, LOW);
}

void Door::_open()
{
    Serial.println("_open() called");
    digitalWrite(_motorOpenPin, HIGH);
    digitalWrite(_motorClosePin, LOW);
}

void Door::_close()
{
    Serial.println("_close() called");
    digitalWrite(_motorOpenPin, LOW);
    digitalWrite(_motorClosePin, HIGH);
}