#include "sensor_ph_lib.h"
#include "calibration_ph_model.h"
#include "firebase_utils.h"

DFRobot_PH phSensor;
Notification notificationPH;
float temperaturePH = 25;
float phValue = 0;

void setupPhSensor()
{
    Serial.println("🔹 Memulai Sensor pH...");
    phSensor.begin();
    EEPROM.begin(512);
    
    if (checkEEPROMCalibration()) {
        Serial.println("Sistem siap untuk proses berikutnya.");
    } else {
        Serial.println("Sistem membutuhkan kalibrasi.");
    }

    CalibrationPhModel calibratePh = readDataCalibrationPhFromFirestore();
    if (calibratePh.status)
    {
        Serial.println("🔹 Data Kalibrasi Terdeteksi dari Firestore.");

        // Simpan status kalibrasi di EEPROM
        EEPROM.write(CALIBRATION_FLAG_ADDR, 1);
        EEPROM.put(CALIBRATION_VALUE_ADDR, calibratePh.fluid_ph);
        EEPROM.commit();
        delay(100);

        // Baca tegangan sensor dari ADC
        int adcValue = analogRead(PH_PIN);
        float voltage = (adcValue / 4095.0) * 3.3;

        Serial.print("🔹 Tegangan Sensor Saat Kalibrasi: ");
        Serial.println(voltage, 3);
        Serial.println(calibratePh.fluid_ph);

        if (fabs(calibratePh.fluid_ph - 4.01) < 0.01)
        {
            Serial.println("[XXXXXX] Memulai kalibrasi pH 4.01...");
            delay(4000);
            calibratePhSensor(voltage, 4.01, "CALIBRATE pH4");
        }
        else if (fabs(calibratePh.fluid_ph == 6.86) < 0.01)
        {
            Serial.println("[XXXXXX] Memulai kalibrasi pH 6.86...");
            calibratePhSensor(voltage, 6.86, "CALIBRATE pH7");
        }
        else if (calibratePh.fluid_ph == 9.18)
        {
            calibratePhSensor(voltage, 9.18, "CALIBRATE pH10");
        }
        else
        {
            Serial.println("⚠️ Error: Nilai pH tidak valid!");
            return;
        }

        // Setelah kalibrasi selesai, baca ulang data dari EEPROM
        Serial.println("🔹 Memuat ulang kalibrasi dari EEPROM...");
        phSensor.begin();
    }
}

bool checkEEPROMCalibration()
{

    int calibrationFlag = EEPROM.read(CALIBRATION_FLAG_ADDR);

    Serial.print("Status Flag Kalibrasi: ");
    if (calibrationFlag == 1)
    {
        Serial.println("Ada");
    }
    else
    {
        Serial.println("Tidak Ada");
        return false;
    }

    float storedCalibrationValue;
    EEPROM.get(CALIBRATION_VALUE_ADDR, storedCalibrationValue);

    Serial.print("Nilai Kalibrasi yang Tersimpan: ");
    Serial.println(storedCalibrationValue, 2);

    if (storedCalibrationValue >= 4.0 && storedCalibrationValue <= 10.0)
    {
        Serial.println("Nilai kalibrasi valid.");
        return true;
    }
    else
    {
        Serial.println("Nilai kalibrasi tidak valid.");
        return false;
    }
}

void calibratePhSensor(float voltage, float calibrationValue, const char *type)
{
    Serial.print("[🔹][🔹][🔹][🔹][🔹][🔹][🔹][🔹][🔹][🔹][🔹][🔹]");
    delay(4000);

    int flagValue = EEPROM.read(CALIBRATION_FLAG_ADDR);
    Serial.print("📌 EEPROM Flag Status: ");
    Serial.println(flagValue); // Cek apakah flag bernilai 1 atau tidak

    if (flagValue == 1)
    {
        Serial.print("✅ EEPROM Flag = 1, masuk mode kalibrasi untuk ");
        Serial.println(type);
    }
    else
    {
        Serial.println("⚠️ EEPROM Flag ≠ 1, keluar dari mode kalibrasi.");
        return; // Tidak masuk ke mode kalibrasi jika flag tidak 1
    }

    if (strcmp(type, "CALIBRATE pH4") == 0)
    {
        EEPROM.put(PHVALUEADDR, voltage);
    }
    else if (strcmp(type, "CALIBRATE pH7") == 0)
    {
        EEPROM.put(PH7VALUEADDR, voltage);
    }
    else
    {
        Serial.println("⚠️ Error: Perintah kalibrasi tidak valid!");
        return;
    }

    EEPROM.commit();
    delay(100);

    Serial.println("🔹 Menyimpan kalibrasi ke EEPROM...");

    phSensor.calibration(voltage, calibrationValue, type);
    Serial.println("✅✅✅ Kalibrasi Selesai ✅✅✅");

    // Reset flag setelah selesai
    EEPROM.write(CALIBRATION_FLAG_ADDR, 0);
    EEPROM.commit();

    patchDataCalibrationPhToFirestore("status", "false");
}

float getMedianValues(float data[], int len)
{
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = i + 1; j < len; j++)
        {
            if (data[i] > data[j])
            {
                float temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
    return (len % 2 == 0) ? (data[len / 2] + data[len / 2 - 1]) / 2.0 : data[len / 2];
}

float readFilteredPhValue()
{
    const int numSamples = 20;
    float phValues[numSamples];
    float sum = 0;
    int countInRange = 0;
    float bestPh = 0;

    for (int i = 0; i < numSamples; i++)
    {
        phValues[i] = readPhValue();

        if (phValues[i] >= 6.0 && phValues[i] <= 7.0)
        {
            countInRange++;
            bestPh = phValues[i];
        }

        sum += phValues[i];
        delay(150);
    }

    if (countInRange > 0)
    {
        return bestPh;
    }

    return sum / numSamples;
}

float readPhValue()
{
    int adcValue = analogRead(PH_PIN);
    float voltage = (adcValue / 4095.0) * 3.3;

    Serial.print("[PH][LIB] | ADC: ");
    Serial.print(adcValue);
    Serial.print(" [PH][LIB] | Tegangan: ");
    Serial.println(voltage, 3);

    return phSensor.readPH(voltage);
}
