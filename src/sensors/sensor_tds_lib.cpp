#include "sensor_tds_lib.h"
#include "calibration_tds_model.h"
#include "firebase_utils.h"
#include "notification.h"
#define CALIBRATION_FLAG_ADDR 0

// Membuat objek sensor TDS dan variabel suhu air
GravityTDS tdsSensor; // Tidak perlu TdsSensor::tdsSensor
float calibrationValue = 0.0;
bool calibrationStatus = false;
Notification notification;
float temperature = 25,
      tdsValue = 0;

// Fungsi untuk inisialisasi sensor TDS
void setupTdsSensor(uint8_t pin)
{
    Serial.println("[DR.ROBOT] Setup TDS Sensor");
    EEPROM.begin(512); 
    tdsSensor.setPin(pin);
    tdsSensor.setAref(3.3);
    tdsSensor.setAdcRange(4096);
    tdsSensor.begin();

    // Membaca data kalibrasi dari Firestore
    CalibrationTdsModel calibrationTds = readDataCalibrationTdsFromFirestore();
    if (calibrationTds.status)
    {
        Serial.println("MULAI KALIBRASI");
         EEPROM.write(CALIBRATION_FLAG_ADDR, 1);
        calibrationValue = calibrationTds.fluid_ppm;
        calibrateTdsSensor(calibrationValue);
        calibrationStatus = true;
        patchDataCalibrationTdsToFirestore("status", "false");
        delay(1000);
    }
    else
    {
        notification.sendNotification("Kalibrasi TDS", "Lakukan Kalibrasi TDS...");
    }
}

// Fungsi untuk mengkalibrasi sensor TDS
void calibrateTdsSensor(float calibrationEC)
{
    if (EEPROM.read(CALIBRATION_FLAG_ADDR) == 1)
    {
        Serial.println("Masuk ke mode kalibrasi");
        tdsSensor.ecCalibration(1); // Masuk ke mode kalibrasi

        // Simulasikan pengiriman data kalibrasi
        char buffer[20];
        sprintf(buffer, "CAL:%f", calibrationEC);
        strcpy(tdsSensor.cmdReceivedBuffer, buffer);
        tdsSensor.ecCalibration(2);

        tdsSensor.ecCalibration(3);

        EEPROM.write(CALIBRATION_FLAG_ADDR, 0);
        EEPROM.commit();
        Serial.println("Kalibrasi selesai dan disimpan ke EEPROM");
    }
}

// Fungsi untuk membaca nilai TDS
float readTdsValue()
{
    tdsSensor.setTemperature(temperature); // set the temperature and execute temperature compensation
    tdsSensor.update();                    // sample and calculate
    tdsValue = tdsSensor.getTdsValue();    // then get the value
    return tdsValue;
}
