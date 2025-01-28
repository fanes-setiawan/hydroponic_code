#ifndef SENSOR_TDS_H
#define SENSOR_TDS_H

#include <Arduino.h>

// Konstanta
#define SCOUNT 10      // Jumlah sampel
#define VREF 3.3        // Referensi tegangan ADC

// Deklarasi fungsi
void setupTdsSensor(uint8_t pin);
void loopTdsSensor(float temperature);

// Fungsi utility
int getMedianNum(int bArray[], int iFilterLen);

#endif
