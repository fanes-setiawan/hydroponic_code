#ifndef SENSOR_PH_H
#define SENSOR_PH_H

#include <Arduino.h>

class PhSensor {
public:
    PhSensor(int pin);     // Konstruktor untuk menentukan pin sensor
    void begin();          // Inisialisasi sensor
    void calibrate(float bufferPh, float bufferVoltage); // Fungsi kalibrasi
    float read();          // Membaca nilai pH dari sensor

private:
    int _pin;              // Pin untuk sensor pH
    float _sensorValue;    // Nilai yang dibaca dari sensor
    float _voltage;        // Nilai tegangan sensor
    float _neutralVoltage; // Tegangan pada pH 7.0
    float _acidSlope;      // Slope kalibrasi pH
};

#endif
