#include "sensor_tds.h"

TdsSensor::TdsSensor(int pin) : _pin(pin) {}

void TdsSensor::begin() {
    pinMode(_pin, INPUT);
}

float TdsSensor::read() {
    _sensorValue = analogRead(_pin);
    _voltage = _sensorValue * (3.3 / 4095.0);
    float tdsValue = _voltage * 1000;
    return tdsValue;
}
