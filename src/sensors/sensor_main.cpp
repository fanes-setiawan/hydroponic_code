
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
        if ((double)phValue <= 14.0 && isErrorSensorPH == false)
        {
            Serial.print("[SENSOR][PH] -> ::");
            Serial.print(phValue);
            sendDataPhToFirestore(phValue);
            isErrorSensorPH = false;
            //optimal 6.0-7.0
            if(phValue <= 6.0){
                notifSensor.sendNotification("Sensor pH :" , "Nilai pH Dibawah Nilai Optimal 6.0-7.0");
            }else if(phValue >= 7.0){
                notifSensor.sendNotification("Sensor pH : " , "Nilai pH Diatas Nilai Optimal 6.0-7.0");
            }
        }
        else if ((double)phValue > 14.0 && isErrorSensorPH == false)
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
