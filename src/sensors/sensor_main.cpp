
#include "sensor_main.h"
#include <EEPROM.h>
#include <map>
#include <vector>

bool isErrorSensorPH = false;
bool isFirstReading = true;

float tdsLevel = 0.0;

float lastPhValue = 0.0;

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
float mostFrequentValue(const std::vector<float> &values, float lastPhValue)
{
    std::map<int, std::vector<float>> groups;

    // 1. Kelompokkan berdasarkan nilai integer sebelum koma
    for (float value : values)
    {
        int key = static_cast<int>(value); // Gunakan bagian integer sebagai kunci kelompok
        groups[key].push_back(value);
    }

    int maxCount = 0;
    int selectedGroup = -1;

    // 2. Cari kelompok dengan jumlah elemen terbanyak
    for (auto &group : groups)
    {
        if (group.second.size() > maxCount)
        {
            maxCount = group.second.size();
            selectedGroup = group.first;
        }
        else if (group.second.size() == maxCount)
        {
            // Jika jumlah kelompok sama, pilih kelompok yang lebih dekat dengan lastPhValue
            if (abs(group.first - lastPhValue) < abs(selectedGroup - lastPhValue))
            {
                selectedGroup = group.first;
            }
        }
    }

    // 3. Pilih nilai terakhir dari kelompok yang dipilih
    if (selectedGroup != -1 && !groups[selectedGroup].empty())
    {
        float closestValue = groups[selectedGroup].back();

        // Cari nilai terakhir yang paling dekat dengan lastPhValue di kelompok terpilih
        for (float value : groups[selectedGroup])
        {
            if (abs(value - lastPhValue) < abs(closestValue - lastPhValue))
            {
                closestValue = value;
            }
        }

        return closestValue;
    }

    // 4. Jika tidak ditemukan kelompok, ambil nilai terakhir langsung
    return values.back();
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

        // Jika perubahan melebihi 1.0, cek ulang
        if (abs(phValue - lastPhValue) > 1.0)
        {
            std::vector<float> phCk;
            for (int i = 0; i < 5; i++)
            {
                phCk.push_back(readFilteredPhValue());
                delay(500);
            }

            Serial.print("phCk: ");
            for (float value : phCk) {
                Serial.print(value);
                Serial.print(" ");
            }
            phValue = mostFrequentValue(phCk, lastPhValue);
            Serial.print("Most Frequent Value: ");
            Serial.println(phValue);
            lastPhValue = phValue;
        }
        else
        {
            lastPhValue = phValue;
        }

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
                automationPH(phValue, tdsLevel);
            }
        }
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
                tdsLevel = normalizeTDS(phValue, tdsLevel);
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