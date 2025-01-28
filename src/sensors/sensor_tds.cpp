#include "sensor_tds.h"

// Variabel global untuk buffer dan indeks
static int analogBuffer[SCOUNT];
static int analogBufferTemp[SCOUNT];
static int analogBufferIndex = 0;

// Pin sensor
static uint8_t TdsSensorPin;

// Fungsi untuk menghitung nilai median
int getMedianNum(int bArray[], int iFilterLen) {
    int bTab[iFilterLen];
    for (byte i = 0; i < iFilterLen; i++) {
        bTab[i] = bArray[i];
    }
    int i, j, bTemp;
    for (j = 0; j < iFilterLen - 1; j++) {
        for (i = 0; i < iFilterLen - j - 1; i++) {
            if (bTab[i] > bTab[i + 1]) {
                bTemp = bTab[i];
                bTab[i] = bTab[i + 1];
                bTab[i + 1] = bTemp;
            }
        }
    }
    if ((iFilterLen & 1) > 0) {
        bTemp = bTab[(iFilterLen - 1) / 2];
    } else {
        bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
    }
    return bTemp;
}

// Fungsi setup untuk sensor TDS
void setupTdsSensor(uint8_t pin) {
    TdsSensorPin = pin;
    pinMode(TdsSensorPin, INPUT);
}

// Fungsi utama untuk membaca sensor
void loopTdsSensor(float temperature) {
    static unsigned long analogSampleTimepoint = millis();
    static unsigned long printTimepoint = millis();

    // Baca data analog setiap 40 ms
    if (millis() - analogSampleTimepoint > 40U) {
        analogSampleTimepoint = millis();
        analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);
        analogBufferIndex++;
        if (analogBufferIndex == SCOUNT) {
            analogBufferIndex = 0;
        }
    }

    // Proses data setiap 800 ms
    if (millis() - printTimepoint > 800U) {
        printTimepoint = millis();
        for (int i = 0; i < SCOUNT; i++) {
            analogBufferTemp[i] = analogBuffer[i];
        }

        // Hitung tegangan rata-rata menggunakan median filtering
        float averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4096.0;
        Serial.println("Average Voltage:");
        Serial.println(averageVoltage);

        // Kompensasi suhu
        float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
        float compensationVoltage = averageVoltage / compensationCoefficient;

        // Konversi tegangan ke nilai TDS
        float tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
                        - 255.86 * compensationVoltage * compensationVoltage
                        + 857.39 * compensationVoltage) * 0.5;

        // Cetak nilai TDS
        Serial.print("TDS Value: ");
        Serial.print(tdsValue, 0);
        Serial.println(" ppm");
    }
}
