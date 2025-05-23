
#include "sensor_main.h"
#include <EEPROM.h>

bool isErrorSensorPH = false;
bool isFirstReading = true;

float tdsLevel = 0.0;

void saveTimeToEEPROM(unsigned long time)
{
    int startAddress = 0;
    EEPROM.put(startAddress, time);
    EEPROM.commit();
}

unsigned long readTimeFromEEPROM()
{
    int startAddress = 0;
    unsigned long savedTime;
    EEPROM.get(startAddress, savedTime);
    return savedTime;
}

void checkingSensor(int intervalMinutes)
{
    unsigned long currentInterval = intervalMinutes * 60000UL;
    unsigned long lastCheckTime = readTimeFromEEPROM();
    unsigned long currentTime = millis();
    Notification notifSensor;

    if (currentTime - lastCheckTime >= currentInterval || lastCheckTime == 0)
    {
        lastCheckTime = currentTime;
        saveTimeToEEPROM(lastCheckTime);

        // String timeNow = getTimeNow();
        float tdsValue = readFilteredTdsValue();
        float phValue = readFilteredPhValue();
        if (isFirstReading)
        {
            tdsLevel = tdsValue;
            isFirstReading = false;
        }
        if ((double)phValue < 6.0 || (double)phValue > 7.0)
        {
            phValue = readFilteredPhValue();
        }
        if ((double)phValue <= 14.0)
        {
            Serial.print("[SENSOR][PH] -> ::");
            Serial.print(phValue);
            sendDataPhToFirestore(phValue);
            isErrorSensorPH = false;
            // optimal 6.0-7.0
            if ((double)phValue <= 6.0)
            {
                notifSensor.sendNotification("Sensor pH :", "Nilai pH Dibawah Nilai Optimal 6.0-7.0");
            }
            else if ((double)phValue >= 7.0)
            {
                notifSensor.sendNotification("Sensor pH : ", "Nilai pH Diatas Nilai Optimal 6.0-7.0");
            }
            if ((double)phValue < 6.0 || (double)phValue > 7.0)
            {
                // saya hide dulu

                Serial.println("saya hiden dulu");
                automationPH(phValue, tdsLevel);
            }
        }\
        
        else if ((double)phValue > 14.0 && isErrorSensorPH == false)
        {
            notifSensor.sendNotification("❌ Sensor pH", "Kalibrasi Ulang Diperlukan");
            isErrorSensorPH = true;
        }

        if ((double)tdsValue > 0.0)
        {
            sendDataTdsToFirestore(tdsLevel);
            if ((double)tdsLevel < 560.0 || (double)tdsLevel > 840.0)
            {
                // saya hide dulu
                Serial.println("saya hiden dulu");
                  tdsLevel =  normalizeTDS(phValue , tdsLevel);
            }
        }
        else if ((double)tdsValue >= 0.0)
        {
            isFirstReading = true;
            tdsLevel = tdsValue;
            sendDataTdsToFirestore(tdsLevel);
        }
        if (double(phValue) > 6.0 && double(phValue) < 7.0 && double(tdsLevel) > 560.0 && double(tdsLevel) < 840.0)
        {
            dataEx.ppm = tdsLevel;
            dataEx.pH = phValue;
            dataEx.ppm_check = tdsLevel;
            dataEx.pH_check = phValue;
            dataEx.phDown = abs(0.0);
            dataEx.phUp = abs(0.0);
            dataEx.water = abs(0.0);
            dataEx.nutrisi = abs(0.0);
            dataEx.status = "NORMAL";
            checkStatus();
            Serial.println("-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-");
            Serial.println("Status : " + dataEx.status);
            oldStatusExcel.status = "NORMAL";
            uploadData(dataEx);
        }
        Serial.println("TDS Value: " + String(tdsValue));
        Serial.println("Simulated TDS Level: " + String(tdsLevel));
        Serial.println("pH Value: " + String(phValue));
    }
}
