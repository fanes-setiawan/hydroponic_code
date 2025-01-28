#include "sensor_tds_lib.h"
#include "calibration_tds_model.h"
#include "firebase_utils.h"

// Membuat objek sensor TDS dan variabel suhu air
GravityTDS TdsSensor::tdsSensor;
float calibrationValue = 0.0;
bool calibrationStatus = false;

// Fungsi untuk inisialisasi sensor TDS
void setupTdsSensor(uint8_t pin, float temperature)
{
    Serial.println("[DR.ROBOT] Setup TDS Sensor");
    TdsSensor::tdsSensor.setPin(pin);
    TdsSensor::tdsSensor.setAref(3.3);
    TdsSensor::tdsSensor.setAdcRange(4096);
    TdsSensor::tdsSensor.setTemperature(temperature);
    TdsSensor::tdsSensor.begin();

    // Membaca data kalibrasi dari Firestore
    CalibrationTdsModel calibrationTds = readDataCalibrationTdsFromFirestore();

    Serial.print("Calibration status from Firestore: ");
    Serial.println(calibrationTds.status);

    if (calibrationTds.status)
    {
        calibrationValue = calibrationTds.fluid_ppm;
        Serial.println("Memulai kalibrasi menggunakan data Firestore...");
        calibrateTdsSensor(calibrationValue);
        calibrationStatus = true;
        delay(500);
        patchDataCalibrationTdsToFirestore("status", "false");
    }
    else
    {
        Serial.println("Data kalibrasi tidak ditemukan. Sensor berjalan tanpa kalibrasi.");
    }
}

// Fungsi untuk mengkalibrasi sensor TDS
void calibrateTdsSensor(float calibrationEC)
{
    Serial.println("[INFO] Memulai kalibrasi..." + String(calibrationEC));
    TdsSensor::tdsSensor.setKValue(calibrationEC); // Set kalibrasi EC (dalam ppm)

    if (calibrationEC > 0)
    {
        Serial.print("[INFO] Menggunakan larutan dengan nilai EC: ");
        Serial.println(calibrationEC);
        TdsSensor::tdsSensor.ecCalibration(1);

        delay(2000);
        TdsSensor::tdsSensor.ecCalibration(2);
        Serial.print("[INFO] Kalibrasi:::.");
    
        TdsSensor::tdsSensor.ecCalibration(3); // Simpan kalibrasi dan keluar
        Serial.println("[INFO] Kalibrasi berhasil dan disimpan.");
    }
    else
    {
        Serial.println("[ERROR] Nilai EC tidak valid untuk kalibrasi.");
    }
}

// Fungsi untuk membaca nilai TDS
float readTdsValue()
{
    TdsSensor::tdsSensor.update();
    float tdsValue = TdsSensor::tdsSensor.getTdsValue();
    Serial.print("Nilai TDS: ");
    Serial.println(tdsValue);
    return tdsValue;
}
