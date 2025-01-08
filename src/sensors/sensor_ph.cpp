#include "sensor_ph.h"

PhSensor::PhSensor(int pin) : _pin(pin), _neutralVoltage(2.5), _acidSlope(-3.5) {}

void PhSensor::begin() {
    pinMode(_pin, INPUT);
}

void PhSensor::calibrate(float bufferPh, float bufferVoltage) {
    if (bufferPh == 7.0) {
        _neutralVoltage = bufferVoltage;  // Kalibrasi untuk larutan netral pH 7.0
    } else {
        _acidSlope = (7.0 - bufferPh) / (2.5 - bufferVoltage);  // Menghitung slope berdasarkan larutan buffer pH asam/basa
    }
}

float PhSensor::read() {
    _sensorValue = analogRead(_pin);                   // Membaca nilai ADC
    _voltage = _sensorValue * (3.3 / 4095.0);          // Mengonversi nilai ADC ke tegangan
    float phValue = 7.0 + ((_neutralVoltage - _voltage) * _acidSlope);  // Menghitung nilai pH
    return phValue;
}
