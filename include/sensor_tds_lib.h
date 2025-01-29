#ifndef SENSOR_TDS_LIB_H
#define SENSOR_TDS_LIB_H

#include <GravityTDS.h>
#include <EEPROM.h>
#include "calibration_tds_model.h"

// Deklarasi fungsi dan variabel untuk sensor TDS
void setupTdsSensor(uint8_t pin);
CalibrationTdsModel readDataCalibrationTdsFromFirestore();
float readTdsValue();
void calibrateTdsSensor(float calibrationEC);

// Namespace opsional untuk menghindari konflik
namespace TdsSensor {
    extern GravityTDS tdsSensor;
    extern float waterTemperature;  // Variabel untuk suhu air
}

#endif 
