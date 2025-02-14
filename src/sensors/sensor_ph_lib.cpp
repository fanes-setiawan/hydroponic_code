#include "sensor_ph_lib.h"
#include "calibration_ph_model.h"
#include "firebase_utils.h"

#define CALIBRATION_FLAG_ADDR 0
#define CALIBRATION_VALUE_ADDR sizeof(int)  // Assuming int size is appropriate for EEPROM address calculation

#define PH_PIN 34

DFRobot_PH phSensor;
Notification notificationPH;
float temperaturePH = 25;  // Default calibration temperaturePH
float phValue = 0;

void setupPhSensor() {
    phSensor.begin();  // Initialize the pH sensor, no need to set pin explicitly
    EEPROM.begin(512);  // Initialize EEPROM with size 512 bytes

    CalibrationPhModel calibratePh = readDataCalibrationPhFromFirestore();
    if (calibratePh.status) {
        EEPROM.write(CALIBRATION_FLAG_ADDR, 1);  // Set the calibration flag
        EEPROM.put(CALIBRATION_VALUE_ADDR, calibratePh.fluid_ph);  // Store the pH value for calibration
        EEPROM.commit();

        if (calibratePh.fluid_ph == 4.01) {
            calibratePhSensor(calibratePh.fluid_ph, "CALIBRATE LOW");
        } else if (calibratePh.fluid_ph == 6.86) {
            calibratePhSensor(calibratePh.fluid_ph, "CALIBRATE HIGH");
        }

        patchDataCalibrationPhToFirestore("status", "false");
    } else {
        notificationPH.sendNotification("Kalibrasi pH", "Lakukan Kalibrasi pH...");
    }
}

void calibratePhSensor(float calibrationValue, const char* type) {
    if (EEPROM.read(CALIBRATION_FLAG_ADDR) == 1) {
        Serial.print("Masuk ke mode kalibrasi untuk ");
        Serial.println(type);

        phSensor.calibration(calibrationValue, 25.0, type);  // Perform calibration based on type

        EEPROM.write(CALIBRATION_FLAG_ADDR, 0); // Reset the calibration flag
        EEPROM.commit();
    }
}

float getMedianValues(float data[], int len) {
    for(int i = 0; i < len-1; i++) {
        for(int j = i+1; j < len; j++) {
            if(data[i] > data[j]) {
                float temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
    return (len % 2 == 0) ? (data[len/2] + data[len/2 - 1]) / 2.0 : data[len/2];
}

float readFilteredPhValue() {
    float phValues[5];
    for (int i = 0; i < 5; i++) {
        phValues[i] = readPhValue();
        delay(100);
    }
    return getMedianValues(phValues, 5);
}

float readPhValue() {
    float voltage = analogRead(PH_PIN) / 4095.0 * 3300; // Read the analog voltage and convert it
    return phSensor.readPH(voltage, temperaturePH);
}
