
#include "sensor_main.h"
#include <EEPROM.h>

bool isErrorSensorPH = false;

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

        String timeNow = getTimeNow();
        float tdsValue = readFilteredTdsValue();
        float phValue = readFilteredPhValue();
        if (phValue <= 14.0 && isErrorSensorPH == false)
        {
            sendDataPhToFirestore(phValue);
            isErrorSensorPH = false;
        }
        else if (phValue > 14.0 && isErrorSensorPH == false)
        {
            notifSensor.sendNotification("❌ Sensor pH", "Kalibrasi Ulang Diperlukan");
            isErrorSensorPH = true;
        }

        if (tdsValue >= 0)
        {

            sendDataTdsToFirestore(tdsValue);
        }

        Serial.println("TDS Value: " + String(tdsValue));
        Serial.println("pH Value: " + String(phValue));
    }
}
