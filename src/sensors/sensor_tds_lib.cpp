#include "sensor_tds_lib.h"
#include "calibration_tds_model.h"
#include "firebase_utils.h"
#include "notification.h"
#define CALIBRATION_FLAG_ADDR 0

GravityTDS tdsSensor;
float calibrationValue = 0.0;
bool calibrationStatus = false;
Notification notification;
float lastTdsOutput = -1;
float temperature = 25,
      tdsValue = 0;

void setupTdsSensor(uint8_t pin)
{
    Serial.println("[DR.ROBOT] Setup TDS Sensor");
    EEPROM.begin(512);
    tdsSensor.setPin(pin);
    tdsSensor.setAref(3.3);
    tdsSensor.setAdcRange(4096);
    tdsSensor.begin();

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
}

void calibrateTdsSensor(float calibrationEC)
{
    if (EEPROM.read(CALIBRATION_FLAG_ADDR) == 1)
    {
        Serial.println("Masuk ke mode kalibrasi");
        notification.sendNotification("Kalibrasi Sensor TDS", "Mulai Kalibrasi Sensor TDS");
        tdsSensor.ecCalibration(1);
        delay(500);
        char buffer[20];
        sprintf(buffer, "CAL:%f", calibrationEC);
        strcpy(tdsSensor.cmdReceivedBuffer, buffer);
        tdsSensor.ecCalibration(2);

        tdsSensor.ecCalibration(3);

        EEPROM.write(CALIBRATION_FLAG_ADDR, 0);
        EEPROM.commit();
        notification.sendNotification("Kalibrasi Sensor TDS", "Selesai Kalibrasi Sensor TDS");
        delay(1000);
    }
}

float getMedianValue(float data[], int len)
{
    float temp[len];
    for (int i = 0; i < len; i++)
    {
        temp[i] = data[i];
    }
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = 0; j < len - i - 1; j++)
        {
            if (temp[j] > temp[j + 1])
            {
                float t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }
    if (len % 2 == 1)
    {
        return temp[len / 2];
    }
    else
    {
        return (temp[len / 2] + temp[len / 2 - 1]) / 2.0;
    }
}

float readFilteredTdsValue()
{
    const int sampleCount = 10;
    float samples[sampleCount];

    if (lastTdsOutput < 0)
    {
        lastTdsOutput = readTdsValue();
        return lastTdsOutput;
    }

    for (int i = 0; i < sampleCount; i++)
    {
        samples[i] = readTdsValue();
        delay(100);
    }

    float medianTds = getMedianValue(samples, sampleCount);

    float threshold = 5.0;
    if (fabs(medianTds - lastTdsOutput) > threshold)
    {
        lastTdsOutput = medianTds;
    }
    Serial.print("[FILTER][TDS] -> ");
    Serial.println(lastTdsOutput);
    return lastTdsOutput;
}

float readTdsValue()
{
    tdsSensor.setTemperature(temperature);
    tdsSensor.update();
    tdsValue = tdsSensor.getTdsValue();
    return tdsValue;
}
