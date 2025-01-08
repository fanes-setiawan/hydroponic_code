#ifndef SENSOR_TDS_H
#define SENSOR_TDS_H

#include <Arduino.h>

class TdsSensor {
public:
    TdsSensor(int pin);    // Konstruktor untuk menentukan pin sensor
    void begin();          // Inisialisasi sensor
    float read();          // Membaca nilai TDS dari sensor

private:
    int _pin;              // Pin untuk sensor TDS
    float _sensorValue;    // Nilai yang dibaca dari sensor
    float _voltage;        // Nilai tegangan sensor
};

#endif
